#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Utils.h"

#include <glm/glm.hpp>

#include <list>

using namespace glm;
class GUI;
class Model;
class Camera;
class Renderer{
public:
    enum class RenderSpace{
        Selection,
        Screen
    };

private:
    GUI* parent;

    vec2 texture_size;
    vec2 viewport_size;

    unsigned int fbo = 0;
    void set_bind_fbo(int texture_width, int texture_height);

public:
    unsigned int frame_texture = 0;
    Camera* camera;

    Renderer(int viewport_width, int viewport_height);
    ~Renderer();
    void set_parent(GUI* parent_);

    void init();
    void render(const std::list<Model*>& models, RenderSpace space_ = RenderSpace::Screen);
    void dispose();

    void resize(int viewport_width, int viewport_height);

    vec2 get_mouse_position();

    //===== Camera ======
    float xPos = 0.f;
    float yPos = 0.f;
    float zPos = 0.f;

    float xDegree = 0.f;
    float yDegree = 0.f;

    Transform cameraTransfrom;

    //===== Material ===== 
    static vec3 lightPos; // TODO : 라이트 컴포넌트 만들때 정리하기

    //===== Selection ====== // TODO : 클래스 분리
    struct PixelInfo {
        uint objectID = 0;
        uint secondID = 0;
        uint primID = 0;
    };

};

