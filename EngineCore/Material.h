#pragma once
#include "Component.h"
#include "SelectionPixelInfo.h"

#include <glm/glm.hpp>

using namespace glm;

class Texture;
class Shader;
class Camera;
class Transform;
class PointLight;
class Material : public Component{
    Shader* screenShader = nullptr;
    Shader* selectionShader = nullptr;

    vec4 color = {1.0f, 1.0f, 0.0f, 1.0f};
    float ambient = 0.1f;

    Transform* uniform_model_transform = nullptr;
    Camera* uniform_camera = nullptr;
    const std::list<PointLight*>* uniform_lights = nullptr;
    SelectionPixelIdInfo uniform_selection_id;

public:
    Material(); // default material
    ~Material();

    Transform* get_uniform_model_transform();
    Camera* get_uniform_camera();
    const std::list<PointLight*>* get_uniform_lights();
    SelectionPixelIdInfo get_uniform_selection_id();

    void set_uniform_model_transform(Transform* model_transform);
    void set_uniform_camera(Camera* camera);
    void set_uniform_lights(const std::list<PointLight*>* lights);
    void set_uniform_selection_id(SelectionPixelIdInfo selection_id);

    void apply();
    void apply_selection_id();

    // ===== Texture =====
private:
    Texture* texture = nullptr;

public:
    Texture* get_texture() const;
    void set_texture(Texture* _texture);
};

