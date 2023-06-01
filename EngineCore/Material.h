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
public:
    Material(); // default material
    virtual ~Material();

    Transform* get_uniform_model_transform();
    Camera* get_uniform_camera();
    SelectionPixelIdInfo get_uniform_selection_id();

    void set_uniform_model_transform(Transform* model_transform);
    void set_uniform_camera(Camera* camera);
    void set_uniform_lights(const std::list<PointLight*>* lights);
    void set_uniform_selection_id(SelectionPixelIdInfo selection_id);

    virtual void apply();
    void apply_selection_id();


    // ===== Transform & Camera ====== //
protected:
    Transform* uniform_model_transform = nullptr;
    Camera* uniform_camera = nullptr;

    // ===== Selection ===== //
protected:
    Shader* selectionShader = nullptr;
    SelectionPixelIdInfo uniform_selection_id;

    // ===== FragmentShader ===== //
protected:
    Shader* screenShader = nullptr;
    FragmentShaderType fragmentShaderType = FragmentShaderType::Color;

    // ===== OutlineShader =====
protected:
    Shader* outlineShader = nullptr;
public:
    void apply_outline();


    // ====== Light ===== //
public:
    const std::list<PointLight*>* get_uniform_lights();

protected:
    const std::list<PointLight*>* uniform_lights = nullptr;

};




class ColorMaterial : public Material{
    vec4 color = {1.0f, 1.0f, 0.0f, 1.0f};
    float ambient = 0.5f;//0.1f;
public:
    ColorMaterial();
    ~ColorMaterial();

    void apply() override;
};



class TextureMaterial : public Material{
public:
    TextureMaterial();
    TextureMaterial(Texture* texture_);
    ~TextureMaterial();

    void apply() override;

    // ===== Texture =====
private:
    Texture* texture = nullptr;

public:
    Texture* get_texture() const;
    void set_texture(Texture* _texture);
};

