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
//public:
    /*enum class RenderSpace{
        Selection,
        Screen
    };*/

//private:
    GUI* parent;

    vec2 texture_size;
    vec2 viewport_size;

    void set_bind_fbo(int texture_width, int texture_height);

public:
    ScreenFrameBuffer* screenFrameBuffer;
    unsigned int frame_texture = 0;
    Camera* camera;

    Renderer(int viewport_width, int viewport_height);
    ~Renderer();
    void set_parent(GUI* parent_);

    void init();
    void render(const std::list<Model*>& models, const std::list<PointLight*>* lights);
    void dispose();

    void resize(vec2 size);

    vec2 get_viewport_size();

    //===== Selection =====
    SelectionPixelObjectInfo find_selection(const std::list<Model*>& models, vec2 mouse_position);
    SelectionFrameBuffer* selectioinFrameBuffer;

};

