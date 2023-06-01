#include "Renderer.h"
#include "Core.h"
#include "Utils.h"
#include "Camera.h"
#include "Material.h"
#include "Model.h"
#include "WorkSpace.h"
#include "Texture.h"
#include "PointLight.h"
#include "FileSystem.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(disable : 4717)

Renderer::Renderer(int viewport_width, int viewport_height){
    parent = nullptr;
    camera = nullptr;

    texture_size = vec2(512, 512); // default
    viewport_size = vec2(viewport_width, viewport_height);
    camera = new Camera((float)viewport_width, (float)viewport_height);
}

Renderer::~Renderer(){
    delete camera;
}

void Renderer::set_parent(GUI* parent_){
    parent = parent_;
}

void Renderer::init(){
}

void Renderer::render(const std::list<Model*>& models, const std::list<PointLight*>* lights){
    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);

    if (framebuffer_screen == nullptr){ // 지연 초기화
        framebuffer_screen = ScreenFrameBuffer::create_screenFrameBuffer(ivec2(texture_size));
    }
    framebuffer_screen->enable();

    ImVec4 clear_color = ImVec4(0.03f, 0.30f, 0.70f, 1.00f);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    camera->calculate_view();

    // TEST
    if (parent->active_workspace != nullptr){
        static Model* newModel = nullptr;
        if (newModel == nullptr){
            newModel = new Model("MyModel");

            //parent->active_workspace->root_model->add_child(newModel);
            camera->get_transform()->set_position(vec3(0.0f, 0.0f, 20.0f));
            camera->get_transform()->set_rotation({0,-90,0});
        }



        Model* model = parent->active_workspace->find_model("MyModel");
        if (model != NULL){
            //CSGMesh* newMesh = 
            Transform* newMesh = model->get_transform();
            newMesh->set_position(vec3(0, 0, 2));
            newMesh->set_scale(vec3(1.5f, 1.0f, 0.5f));

            static bool TextureTest = true;
            static bool TextureTest2 = true;
            if (TextureTest)        {
                TextureTest = false;
                EnginePath newPath = FileSystem::GetProjectPath();
                newPath.Move("EngineResource");
                newPath.Move("Texture");
                newPath.Move("rockTexture.jpg");
                Texture* newTexture = Texture::create_texture(newPath);

                model->set_material(new TextureMaterial(newTexture));
            }
            if (TextureTest2 && Utils::time_acc() > 5.0f)        {
                TextureTest2 = false;
                model->set_material(new ColorMaterial());
            }
        }

    }


    for (Model* model : models){
        glEnable(GL_DEPTH_TEST);
        bool isSelected = Utils::contains(get_selected_models(), model);
        //윤곽선을 그리기 위해 Stencil 셋팅
        //Selected Model의 모델의 픽셀들은 스텐실 값이 1로 초기화 됩니다. 그 외에는 0입니다.
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        if(isSelected){
            glStencilMask(0xFF);
        }
        else{
            glStencilMask(0x00);
        }
        model->get_material()->set_uniform_camera(camera);
        model->get_material()->set_uniform_lights(lights);
        model->render();
    }

    //조금 더 큰 모델을 윤곽선 쉐이더로 그립니다.
    //이때 스텐실이 1인 픽셀에는 그리지 않습니다.
    for (Model* model : get_selected_models()) {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        model->get_material()->set_uniform_camera(camera);
        float ff = 1.05f;
        model->render_outline({ ff, ff, ff });
        //parent->active_workspace->selected_models

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }

    //기즈모 렌더링
    for (Model* model : get_selected_models())
    {
        model->render_gizmo();
    }

}

SelectionPixelObjectInfo Renderer::find_selection(const std::list<Model*>& models, vec2 mouse_position){
    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);

    if (framebuffer_selection == nullptr){
        framebuffer_selection = SelectionFrameBuffer::create_selectionFrameBuffer(texture_size);
    }

    // 셀렉션 버퍼에 렌더링
    {
        framebuffer_selection->enable();
        glStencilMask(0x00);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //모델 정보 렌더링
        uint32_t selection_id_model_acc = 1;
        for (Model* model : models){
            model->get_material()->set_uniform_camera(camera);
            model->render_selection_id(&selection_id_model_acc);
        }

        //기즈모 정보 렌더링
        for (Model* model : get_selected_models()){
            model->get_gizmo()->render_selectionBuffer(camera);
        }

        framebuffer_selection->disable();
    }

    // 셀렉션 렌더링 정보 읽기
    framebuffer_selection->enable();
    SelectionPixelIdInfo Pixel = framebuffer_selection->
        read_pixel((int)(texture_size.x * mouse_position.x / viewport_size.x),
                   (int)(texture_size.y * mouse_position.y / viewport_size.y));
    framebuffer_selection->disable();

    //3번째 픽셀값이 1 == 기즈모 셀렉트
    if (Pixel.objectType == 1)
    {
        Model* firstSelectedModel = nullptr;
        for (Model* model : get_selected_models())
        {
            firstSelectedModel = model;
            break;
        }

        int axis = Pixel.model_id; // 0 : x , 1 : y , 2 : z, 3 : mainDot
        parent->active_workspace->dragDelegate =
            std::bind(&Gizmo::move, firstSelectedModel->get_gizmo(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,axis);

        return SelectionPixelObjectInfo();
    }
    else
    {
        uint32_t selection_id_model_acc = 1;
        for (Model* model : models) {
            SelectionPixelObjectInfo info = model->from_selection_id(Pixel, &selection_id_model_acc);
            if (!info.empty()) {
                return info;
            }
        }
    }

    return SelectionPixelObjectInfo(); // null
}

std::list<Model*> Renderer::get_selected_models()
{
    return parent->active_workspace->selected_models;
}

void Renderer::dispose(){
}

void Renderer::resize(vec2 size){
    viewport_size = size;
    camera->resize(size.x, size.y);
}

vec2 Renderer::get_viewport_size(){
    return viewport_size;
}
