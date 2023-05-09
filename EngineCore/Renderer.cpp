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

void Renderer::set_bind_fbo(int texture_width, int texture_height){ // TODO : 리팩토링
    if (screenFrameBuffer == nullptr){ // 지연 초기화
        screenFrameBuffer = ScreenFrameBuffer::create_screenFrameBuffer(ivec2(texture_width, texture_height));
    }
    screenFrameBuffer->enable();
}

void Renderer::render(const std::list<Model*>& models, const std::list<PointLight*>* lights){
    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);

    set_bind_fbo((int)texture_size.x, (int)texture_size.y);

    ImVec4 clear_color = ImVec4(0.03f, 0.30f, 0.70f, 1.00f);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera->calculate_view();

    //test
    if(parent->active_workspace != nullptr){
        static Model* newModel = nullptr;
        static PointLight* newLight = nullptr;
        if (newModel == nullptr) {
            newModel = new Model("MyModel");

            //newModel->set_new(Mesh::Cube2);
            //newModel->set_new(Mesh::compute_difference(Mesh::t_Cube2, Mesh::Cube2));
            //newModel->set_new(Mesh::compute_union(Mesh::Cube2, Mesh::Sphere));
            //newModel->set_new(Mesh::compute_intersection(Mesh::Cube2, Mesh::Sphere));
            //newModel->set_new(Mesh::compute_difference(Mesh::Cube2, Mesh::Sphere));
            //newModel->set_new(Mesh::compute_difference2(Mesh::Cube2, Mesh::Sphere));
            //newModel->set_new(Mesh::compute_intersection(Mesh::Sphere, Mesh::t_Cube2));  ///union
            newModel->add_component(newLight = new PointLight(parent->active_workspace, vec3(30, -100, -50)));
            newLight->set_position(vec3(42, 0, 42));
            
            //newModel->set_new(Mesh::Sphere);

            parent->active_workspace->models.push_back(newModel);
            camera->get_transform()->set_position(vec3(0.0f, 0.0f, 20.0f));
            camera->get_transform()->set_rotation({ 0,-90,0 });
        }

        /*if (newLight != nullptr) {
            newLight->set_position(vec3(50 * sin(Utils::time_acc()), 0, 50 * sin(Utils::time_acc())));
        }*/


        Model* model = parent->active_workspace->find_model("MyModel");
        if (model != NULL) {
            //CSGMesh* newMesh = 
            Transform* newMesh = model->get_transform();
            newMesh->set_position(vec3(0, 0, 2));
            newMesh->set_scale(vec3(1.5f, 1.0f, 0.5f));
        }


        static bool TextureTest = true;
        if (TextureTest)
        {
            TextureTest = false;
            EnginePath newPath = FileSystem::GetProjectPath();
            newPath.Move("EngineResource");
            newPath.Move("Texture");
            newPath.Move("rockTexture.jpg");
            Texture* newTexture = Texture::create_texture(newPath);

            model->set_material<TextureMaterial>();
            static_cast<TextureMaterial*>(newModel->get_material())->set_texture(newTexture);
        }
        if (Utils::time_acc() > 5.0f)
        {
            model->set_material<ColorMaterial>();
        }
    }

    for (Model* model : models){
        model->get_material()->set_uniform_camera(camera);
        model->get_material()->set_uniform_lights(lights);
        model->render();
    }
}

SelectionPixelObjectInfo Renderer::find_selection(const std::list<Model*>& models, vec2 mouse_position){
    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);

    //===== init ======
    if (selectioinFrameBuffer == nullptr){
        selectioinFrameBuffer = SelectionFrameBuffer::create_selectionFrameBuffer(texture_size);
    }

    //===== 셀렉션 버퍼에 렌더링 ======
    {
        selectioinFrameBuffer->enable();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        uint32_t selection_id_model_acc = 1;
        for (Model* model : models){
            model->get_material()->set_uniform_camera(camera);
            model->render_selection_id(&selection_id_model_acc);
        }
        selectioinFrameBuffer->disable();
    }

    // ===== 셀렉션 렌더링 정보 읽기 ======
    selectioinFrameBuffer->enable();
    SelectionPixelIdInfo Pixel = selectioinFrameBuffer->read_pixel(texture_size.x * mouse_position.x / viewport_size.x, texture_size.y * mouse_position.y / viewport_size.y);
    selectioinFrameBuffer->disable();

    uint32_t selection_id_model_acc = 1;
    for(Model* model : models){
        SelectionPixelObjectInfo info = model->from_selection_id(Pixel, &selection_id_model_acc);
        if(!info.empty()){
            return info;
        }
    }
    return SelectionPixelObjectInfo(); // null
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
