#pragma once
#include "Component.h"

#include <glm/glm.hpp>

using namespace glm;

class Shader;
class Camera;
class Transform;
class Material : public Component{
    Shader* screenShader = nullptr;
    Shader* selectionShader = nullptr;

    vec4 color = {1.0f, 1.0f, 0.0f, 1.0f};
    float ambient = 0.1f;

    Transform* uniform_model_transform = nullptr;
    Camera* uniform_camera = nullptr;
    vec3 uniform_lights; // TODO : std::list<Light>*
    int uniform_selection_id = 0;

public:
    Material(); // default material
    ~Material();

    void set_uniform_transform(Transform* model_transform);
    void set_uniform_camera(Camera* camera);
    void set_uniform_lights(vec3 lights);
    void set_uniform_selection_id(int selection_id);

    void apply();
    void apply_object_selection();
};

