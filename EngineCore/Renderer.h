#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Utils.h"
#include "SelectionPixelInfo.h"
#include "FrameBuffer.h"

#include <glm/glm.hpp>

#include <list>

using namespace glm;
class WorkSpace;
class Model;
class Camera;
class PointLight;
class Renderer{
    static int id_counter;
    static int default_camera_pos_z;

    int id;

    vec2 texture_size;
    vec2 viewport_size;

public:
    WorkSpace* workspace;
    Camera* camera;

    ScreenFrameBuffer* framebuffer_screen = nullptr;
    SelectionFrameBuffer* framebuffer_selection = nullptr;
    unsigned int frame_texture = 0;

    Renderer(int viewport_width, int viewport_height);
    ~Renderer();
    void set_parent(WorkSpace* workspace_);
    int get_id();

    void render(const std::list<Model*>& models, const std::list<PointLight*>* lights);
    void render_outline(const std::list<Model*>& models);

    void find_selection_gizmo(const std::list<Model*>& models, vec2 mouse_position);
public:
    void dispose();

    void resize(vec2 size);
    vec2 get_viewport_size();

    SelectionPixelObjectInfo find_selection(const std::list<Model*>& models, vec2 mouse_position);
};

