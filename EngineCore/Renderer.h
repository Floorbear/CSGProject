#pragma once

#include "FrameBuffer.h"
#include "Material.h"
#include "PostProcessor.h"
#include "SelectionPixelInfo.h"

#include <glm/glm.hpp>

#include <list>

using namespace glm;

class WorkSpace;
class Camera;
class Model;
class CSGNode;
class PointLight;

class Renderer{
    static int id_counter;
    int id;

    vec2 texture_size;
    vec2 viewport_size;

    ScreenFrameBuffer* framebuffer_outline = nullptr; // 윤곽선을 그릴 모델만 재 렌더링해서 후처리 // TODO : color만 있는 버퍼로 변경 
    ScreenFrameBuffer* framebuffer_outline_temp_output = nullptr; // 포스트프로세싱 시 출력버퍼로 사용 // TODO : color만 있는 버퍼로 변경 
    ScreenFrameBuffer* framebuffer_mesh_overlay = nullptr;
    SelectionFrameBuffer* framebuffer_selection = nullptr;

    MonotoneMaterial* material_outline = nullptr;
    PostProcessor* postprocessor_outline_expand = nullptr;
    PostProcessor* postprocessor_outline_overlay = nullptr;

    ColorMaterial* material_mesh_overlay = nullptr;
    PostProcessor* postprocessor_mesh_overlay = nullptr;

public:
    WorkSpace* workspace = nullptr;
    Camera* camera = nullptr;
    Gizmo* gizmo = nullptr;

    unsigned int frame_texture = 0;
    ScreenFrameBuffer* framebuffer_screen = nullptr; // 전체 모델을 렌더링

    Renderer(int viewport_width, int viewport_height);
    ~Renderer();

    void set_parent(WorkSpace* workspace_);
    int get_id();

    void render(const std::list<Model*>& models, const std::list<PointLight*>* lights);
    void render_outline(const std::list<Model*>& models);
    void render_mesh_overlay_and_outline(const std::list<Model*>& models, const std::list<CSGNode*>& selected_meshes, const std::list<PointLight*>* lights);

    void dispose();

    void resize(vec2 size);
    vec2 get_viewport_size();

    SelectionPixelObjectInfo find_selection_objects(const std::list<Model*>& models, vec2 mouse_position);
    SelectionPixelObjectInfo find_selection_gizmo(const std::list<TransformEntity*>& models, vec2 mouse_position);
};

