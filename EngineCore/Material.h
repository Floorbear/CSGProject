#pragma once
#include "Component.h"

#include <glm/glm.hpp>

enum class RenderSpace
{
    Selection,
    Screen
};


class Shader;
class Camera;
class Transform;
class Material : public Component{
    Shader* screenShader = nullptr;
    Shader* selectionShader = nullptr;
    vec4 color = {1.0f, 1.0f, 0.0f, 1.0f};
    float ambient = 0.1f;

public:
    Material(); // default material
    ~Material();

    void apply(Transform* model_transform, Camera* camera, vec3 light_position, RenderSpace _space = RenderSpace::Screen);//, std::list<Light> lights);
};

