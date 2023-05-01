#pragma once
#include "Component.h"

#include <glm/glm.hpp>

class Shader;

class Camera;
class Transform;
class Material : public Component{
    Shader* shader = nullptr;
    vec4 color = {1.0f, 1.0f, 0.0f, 1.0f};
    float ambient = 0.1f;

public:
    Material(); // default material
    ~Material();

    void apply(Transform* model_transform, Camera* camera, vec3 light_position);//, std::list<Light> lights);
    Shader* get_shader();
};

