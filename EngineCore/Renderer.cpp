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

int Renderer::id_counter = 1; // 1부터 시작
float Renderer::default_camera_pos_z = 20.0f;

Renderer::Renderer(int viewport_width, int viewport_height){
    id = id_counter++;
    workspace = nullptr;
    camera = nullptr;

    texture_size = vec2(512, 512); // default
    viewport_size = vec2(viewport_width, viewport_height);
    camera = new Camera((float)viewport_width, (float)viewport_height);
    camera->get_transform()->set_position(vec3(0.0f, 0.0f, default_camera_pos_z));
    camera->get_transform()->set_rotation({0,-90,0});
}

Renderer::~Renderer(){
    delete camera;
}

void Renderer::set_parent(WorkSpace* workspace_){
    workspace = workspace_;
}

int Renderer::get_id(){
    return id;
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

    // ===== 일반 스크린 렌더링 ===== //

    glEnable(GL_DEPTH_TEST);
    for (Model* model : models){
        model->get_material()->set_uniform_camera(camera);
        model->get_material()->set_uniform_lights(lights);
        model->render(this);
    }
}

void Renderer::render_outline(const std::list<Model*>& models){ // render()의 연장
    if (models.empty()){
        return;
    }

    framebuffer_screen->enable();

    // ===== 윤곽선 렌더링 ===== //

    //조금 더 큰 모델을 윤곽선 쉐이더로 그립니다.
    //이때 스텐실이 1인 픽셀에는 그리지 않습니다.
    glDisable(GL_DEPTH_TEST);
    for (Model* model : models){
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        model->get_material()->set_uniform_camera(camera);
        float ff = 1.05f;
        model->render_outline({ff, ff, ff});

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }

    // ===== 기즈모 렌더링 ===== //

    for (Model* model : models){
        model->render_gizmo();
    }
}

void Renderer::find_selection_gizmo(const std::list<Model*>& models, vec2 mouse_position){
    if (models.empty()){
        return;
    }

    if (framebuffer_selection == nullptr){
        framebuffer_selection = SelectionFrameBuffer::create_selectionFrameBuffer(texture_size);
    }
    framebuffer_selection->enable();

    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);
    glStencilMask(0x00);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //기즈모 정보 렌더링
    for (Model* model : models){
        model->get_gizmo()->render_selectionBuffer(camera);
    }

    // 셀렉션 렌더링 정보 읽기
    SelectionPixelIdInfo Pixel = framebuffer_selection->
        read_pixel((int)(texture_size.x * mouse_position.x / viewport_size.x),
            (int)(texture_size.y * mouse_position.y / viewport_size.y));

    framebuffer_selection->disable();

    models.front()->get_gizmo()->set_selectedAxis(-1);
    if (Pixel.object_type != SelectionPixelInfo::object_type_none){
        int axis = Pixel.object_type - 2; // 0 : x , 1 : y , 2 : z, 3 : mainDot
        models.front()->get_gizmo()->set_selectedAxis(axis);
    }
}

SelectionPixelObjectInfo Renderer::find_selection(const std::list<Model*>& models, vec2 mouse_position){
    if (models.empty()){
        return SelectionPixelObjectInfo(); // null
    }

    if (framebuffer_selection == nullptr){
        framebuffer_selection = SelectionFrameBuffer::create_selectionFrameBuffer(texture_size);
    }
    framebuffer_selection->enable();

    // 셀렉션 버퍼에 렌더링
    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);
    glStencilMask(0x00);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //모델 정보 렌더링
    uint32_t selection_id_model_acc = 1;
    for (Model* model : models){
        model->get_material()->set_uniform_camera(camera);
        model->render_selection_id(&selection_id_model_acc);
    }

    //기즈모 정보 렌더링
    for (Model* model : workspace->selected_models){
        model->get_gizmo()->render_selectionBuffer(camera);
    }

    // 셀렉션 렌더링 정보 읽기
    SelectionPixelIdInfo pixel = framebuffer_selection->
        read_pixel((int)(texture_size.x * mouse_position.x / viewport_size.x),
                   (int)(texture_size.y * mouse_position.y / viewport_size.y));

    framebuffer_selection->disable();

    if (pixel.object_type == SelectionPixelInfo::object_type_object){
        selection_id_model_acc = 1;
        for (Model* model : models){
            SelectionPixelObjectInfo info = model->from_selection_id(pixel, &selection_id_model_acc);
            if (!info.empty()){
                return info;
            }
        }
    }
    return SelectionPixelObjectInfo(pixel.object_type);
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
