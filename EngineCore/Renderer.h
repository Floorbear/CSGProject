#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Utils.h"
#include "SelectionPixelInfo.h"
#include "FrameBuffer.h"

#include <glm/glm.hpp>

#include <list>

using namespace glm;
class GUI;
class Model;
class Camera;
class PointLight;
class Renderer{
    static int id_counter;
    static int default_camera_pos_z;

    GUI* parent;
    int id;

    vec2 texture_size;
    vec2 viewport_size;

public:
    ScreenFrameBuffer* framebuffer_screen = nullptr;
    SelectionFrameBuffer* framebuffer_selection = nullptr;
    unsigned int frame_texture = 0;

    Camera* camera;

    Renderer(int viewport_width, int viewport_height);
    ~Renderer();
    void set_parent(GUI* parent_);
    int get_id();

    void render(const std::list<Model*>& models, const std::list<PointLight*>* lights);
    void dispose();

    void resize(vec2 size);

    vec2 get_viewport_size();

    SelectionPixelObjectInfo find_selection(const std::list<Model*>& models, vec2 mouse_position);

    std::list<Model*> get_selected_models();
};

