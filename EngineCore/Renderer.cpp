#include "Renderer.h"
#include "WorkSpace.h"
#include "Camera.h"
#include "Model.h"
#include "CSGNode.h"
#include "PointLight.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(disable : 4717)

int Renderer::id_counter = 1; // 1부터 시작
float Renderer::default_camera_pos_z = 15.0f;

Renderer::Renderer(int viewport_width, int viewport_height){
    id = id_counter++;

    texture_size = vec2(512, 512); // default
    viewport_size = vec2(viewport_width, viewport_height);

    camera = new Camera((float)viewport_width, (float)viewport_height);
    camera->get_transform()->set_position(vec3(0.0f, 0.0f, default_camera_pos_z));
    camera->get_transform()->set_rotation({0,-90,0});
    gizmo = new Gizmo();

    material_outline = new MonotoneMaterial(vec3(1.0, 0.5, 0)); // 윤곽선 색상
    material_outline->set_uniform_camera(camera);
    material_mesh_overlay = new ColorMaterial();
    material_mesh_overlay->set_uniform_camera(camera);
    material_mesh_overlay->color = vec4(0.5, 0.5, 0.5, 1.0);
}

Renderer::~Renderer(){
    delete camera;
    delete gizmo;
    delete material_outline;
    delete postprocessor_outline_expand;
    delete postprocessor_outline_overlay;
    delete material_mesh_overlay;
    delete postprocessor_mesh_overlay;
}

void Renderer::set_parent(WorkSpace* workspace_){
    workspace = workspace_;
}

int Renderer::get_id(){
    return id;
}

void Renderer::render(const std::list<Model*>& models, const std::list<PointLight*>* lights){
    if (framebuffer_screen == nullptr){
        framebuffer_screen = new ScreenFrameBuffer(texture_size);
    }
    framebuffer_screen->bind();

    const static ImVec4 clear_color = ImVec4(0.03f, 0.30f, 0.70f, 1.00f);
    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    camera->calculate_view();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    for (Model* model : models){
        model->get_material()->set_uniform_camera(camera);
        model->get_material()->set_uniform_lights(lights);
        model->render(this);
    }
    glDisable(GL_STENCIL_TEST);
    glStencilMask(0xFF);

    framebuffer_screen->disable();
}

void Renderer::render_outline(const std::list<Model*>& models){ // render() 후에 실행
    if (models.empty()){
        return;
    }

    if (framebuffer_outline == nullptr){
        framebuffer_outline = new ScreenFrameBuffer(texture_size);
        framebuffer_outline_temp_output = new ScreenFrameBuffer(texture_size);
        postprocessor_outline_expand = new Expand3PostProcessor(material_outline->get_color());
        postprocessor_outline_overlay = new OverlayPostProceesor(1);
    }
    framebuffer_outline->bind();

    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);
    glClearColor(0, 1.0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    for (Model* model : models){
        model->render_monotone(material_outline);
    }
    glEnable(GL_DEPTH_TEST);

    framebuffer_outline->disable();

    { // expand_3 1-pass
        framebuffer_outline_temp_output->bind();
        glClear(GL_COLOR_BUFFER_BIT);

        postprocessor_outline_expand->apply(framebuffer_outline, framebuffer_outline_temp_output);
        std::swap(framebuffer_outline, framebuffer_outline_temp_output);
    }

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    postprocessor_outline_overlay->apply(framebuffer_outline, framebuffer_screen);
    glDisable(GL_STENCIL_TEST);
    glStencilMask(0xFF);

    // ===== 기즈모 렌더링 ===== //=

    std::list<TransformEntity*> transform_entities;
    for(Model* model : models){
        transform_entities.push_back(model);
    }

    framebuffer_screen->bind();

    glClear(GL_DEPTH_BUFFER_BIT);
    gizmo->render(camera, transform_entities);
}

void Renderer::render_mesh_overlay_and_outline(const std::list<Model*>& models, const std::list<CSGNode*>& selected_meshes, const std::list<PointLight*>* lights){ // render() 후에 실행
    if (models.empty()){
        return;
    }

    if (framebuffer_outline == nullptr){
        framebuffer_outline = new ScreenFrameBuffer(texture_size);
        framebuffer_outline_temp_output = new ScreenFrameBuffer(texture_size);
        postprocessor_outline_expand = new Expand3PostProcessor(material_outline->get_color());
        postprocessor_outline_overlay = new OverlayPostProceesor(1);
    }
    if (framebuffer_mesh_overlay == nullptr){
        framebuffer_mesh_overlay = new ScreenFrameBuffer(texture_size);
        postprocessor_mesh_overlay = new OverlayPostProceesor(0.5);
    }

    // ===== 오버레이 ===== //

    framebuffer_mesh_overlay->bind();

    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    material_mesh_overlay->set_uniform_lights(lights);
    for (Model* model : models){
        model->get_csg_mesh()->render_operands(material_mesh_overlay);
    }

    postprocessor_mesh_overlay->apply(framebuffer_mesh_overlay, framebuffer_screen);

    // ===== 스텐실 ===== //

    framebuffer_outline->bind();

    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_STENCIL_BUFFER_BIT); // TODO : ㅠㅠ 최적화 가능??

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    // glStencilMask(0xFF); always true
    for (CSGNode* mesh : selected_meshes){
        mesh->render_monotone(material_outline);
    }
    glDisable(GL_STENCIL_TEST);
    glStencilMask(0xFF);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_outline->get_fbo());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_screen->get_fbo());
    glBlitFramebuffer(0, 0, (GLint)texture_size.x, (GLint)texture_size.y, 0, 0, (GLint)texture_size.x, (GLint)texture_size.y, GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // ===== 윤곽선 ===== //

    framebuffer_outline->bind();

    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    for (CSGNode* selected_mesh : selected_meshes){
        selected_mesh->render_monotone(material_outline);
    }
    glEnable(GL_DEPTH_TEST);

    framebuffer_outline->disable();

    { // expand_3 1-pass
        framebuffer_outline_temp_output->bind();
        glClear(GL_COLOR_BUFFER_BIT);

        postprocessor_outline_expand->apply(framebuffer_outline, framebuffer_outline_temp_output);
        std::swap(framebuffer_outline, framebuffer_outline_temp_output);
    }

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    postprocessor_outline_overlay->apply(framebuffer_outline, framebuffer_screen);
    glDisable(GL_STENCIL_TEST);
    glStencilMask(0xFF);

    // ===== 기즈모 렌더링 ===== //

    std::list<TransformEntity*> transform_entities;
    for(CSGNode* mesh : selected_meshes){
        transform_entities.push_back(mesh);
    }

    framebuffer_screen->bind();

    glClear(GL_DEPTH_BUFFER_BIT);
    gizmo->render(camera, transform_entities);

    glStencilMask(0x00);
}

SelectionPixelObjectInfo Renderer::find_selection(const std::list<Model*>& models, vec2 mouse_position){
    if (models.empty()){
        return SelectionPixelObjectInfo(); // null
    }

    if (framebuffer_selection == nullptr){
        framebuffer_selection = new SelectionFrameBuffer(texture_size);
    }
    framebuffer_selection->bind();

    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //모델 정보 렌더링
    uint32_t selection_id_model_acc = 1;
    for (Model* model : models){
        model->get_material()->set_uniform_camera(camera);
        model->render_selection_id(&selection_id_model_acc);
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    //기즈모 정보 렌더링
    std::list<TransformEntity*> transform_entities;
    for(Model* model : models){
        transform_entities.push_back(model);
    }
    gizmo->render_selection(camera, transform_entities);

    // 셀렉션 렌더링 정보 읽기
    SelectionPixelIdInfo pixel = framebuffer_selection->
        read_pixel(texture_size.x * mouse_position.x / viewport_size.x,
                   texture_size.y * mouse_position.y / viewport_size.y);

    framebuffer_selection->disable();

    if (pixel.object_type == SelectionPixelInfo::object_type_object){
        selection_id_model_acc = 1;
        for (Model* model : models){
            SelectionPixelObjectInfo info = model->from_selection_id(pixel, &selection_id_model_acc);
            if (!info.empty()){
                return info;
            }
        }
    } else if (pixel.object_type == SelectionPixelInfo::object_type_gizmo){
        if (pixel.model_id == 0){
            return SelectionPixelObjectInfo(GizmoAxis::X);
        } else if (pixel.model_id == 1){
            return SelectionPixelObjectInfo(GizmoAxis::Y);
        } else if (pixel.model_id == 2){
            return SelectionPixelObjectInfo(GizmoAxis::Z);
        } else if (pixel.model_id == 3){
            return SelectionPixelObjectInfo(GizmoAxis::XYZ);
        } else{
            return SelectionPixelObjectInfo(GizmoAxis::None);
        }
    }

    return SelectionPixelObjectInfo(pixel.object_type);
}

SelectionPixelObjectInfo Renderer::find_selection_gizmo(const std::list<Model*>& models, vec2 mouse_position){
    if (models.empty()){
        return SelectionPixelObjectInfo(); // null
    }

    if (framebuffer_selection == nullptr){
        framebuffer_selection = new SelectionFrameBuffer(texture_size);
    }
    framebuffer_selection->bind();

    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::list<TransformEntity*> transform_entities;
    for(Model* model : models){
        transform_entities.push_back(model);
    }

    //기즈모 정보 렌더링
    gizmo->render_selection(camera, transform_entities);

    // 셀렉션 렌더링 정보 읽기
    SelectionPixelIdInfo pixel = framebuffer_selection->
        read_pixel(texture_size.x * mouse_position.x / viewport_size.x,
                   texture_size.y * mouse_position.y / viewport_size.y);

    framebuffer_selection->disable();

    if (pixel.object_type == SelectionPixelInfo::object_type_gizmo){
        if (pixel.model_id == 0){
            return SelectionPixelObjectInfo(GizmoAxis::X);
        } else if (pixel.model_id == 1){
            return SelectionPixelObjectInfo(GizmoAxis::Y);
        } else if (pixel.model_id == 2){
            return SelectionPixelObjectInfo(GizmoAxis::Z);
        } else if (pixel.model_id == 3){
            return SelectionPixelObjectInfo(GizmoAxis::XYZ);
        } else{
            return SelectionPixelObjectInfo(GizmoAxis::None);
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
