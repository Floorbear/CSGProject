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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera->calculate_view();

    // TEST
    if(parent->active_workspace != nullptr){
        static Model* newModel = nullptr;
        static PointLight* newLight = nullptr;
        if (newModel == nullptr) {
            newModel = new Model("MyModel");

            newModel->set_new(Mesh::Cube);
            newModel->add_component(newLight = new PointLight(parent->active_workspace, vec3(42, 0, 42)));

            Model* newModel2 = new Model("MyModel2");

            newModel->add_child(newModel2);
            newModel2->set_new(Mesh::Cube);
            newModel2->get_transform()->set_position(vec3(0, 2, 2));
            //newModel->set_new(Mesh::compute_intersection(Mesh::Cube2,Mesh::Cube));
            //newModel->set_new(Mesh::Sphere);

            parent->active_workspace->root_model->add_child(newModel);
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
        static bool TextureTest2 = true;
        if (TextureTest)
        {
            TextureTest = false;
            EnginePath newPath = FileSystem::GetProjectPath();
            newPath.Move("EngineResource");
            newPath.Move("Texture");
            newPath.Move("rockTexture.jpg");
            Texture* newTexture = Texture::create_texture(newPath);

            model->set_material(new TextureMaterial(newTexture));
        }
        if (TextureTest2 && Utils::time_acc() > 5.0f)
        {
            TextureTest2 = false;
            model->set_material(new ColorMaterial());
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

    if (framebuffer_selection == nullptr){
        framebuffer_selection = SelectionFrameBuffer::create_selectionFrameBuffer(texture_size);
    }

    // 셀렉션 버퍼에 렌더링
    {
        framebuffer_selection->enable();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        uint32_t selection_id_model_acc = 1;
        for (Model* model : models){
            model->get_material()->set_uniform_camera(camera);
            model->render_selection_id(&selection_id_model_acc);
        }
        framebuffer_selection->disable();
    }

    // 셀렉션 렌더링 정보 읽기
    framebuffer_selection->enable();
    SelectionPixelIdInfo Pixel = framebuffer_selection->read_pixel(texture_size.x * mouse_position.x / viewport_size.x, texture_size.y * mouse_position.y / viewport_size.y);
    framebuffer_selection->disable();

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
