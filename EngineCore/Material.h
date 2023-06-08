#pragma once

#include "Component.h"
#include "SelectionPixelInfo.h"

#include <glm/glm.hpp>

using namespace glm;

enum class FragmentShaderType{
    Color,
    Texture,
    None
};

class Texture;
class Shader;
class Camera;
class Transform;
class PointLight;
class Material : public Component{
protected:
    Shader* shader = nullptr;

    Transform* uniform_model_transform = nullptr;
    Camera* uniform_camera = nullptr;
    const std::list<PointLight*>* uniform_lights = nullptr;

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

    virtual void apply();
    void apply_selection_id();

protected:
    Shader* selectionShader = nullptr;
    SelectionPixelIdInfo uniform_selection_id;
};

// ===== Internal Use Materials ===== //

class MonotoneMaterial : public Material{
    vec3 color;
public:
    MonotoneMaterial(vec3 color_);

    void apply() override;
    vec3 get_color();
};

// ===== Model Materials ===== //

class ColorMaterial : public Material{
public:
    vec4 color = {1.0f, 1.0f, 0.0f, 1.0f};
    float ambient = 0.5f;//0.1f;

    ColorMaterial();

    void apply() override;
};

class TextureMaterial : public Material{
    Texture* texture = nullptr;
public:
    TextureMaterial();
    TextureMaterial(Texture* texture_);
    ~TextureMaterial();

    void apply() override;

    Texture* get_texture() const;
    void set_texture(Texture* texture_);
};
