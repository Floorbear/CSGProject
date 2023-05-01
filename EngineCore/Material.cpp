#include "Material.h"
#include "Shader.h"
#include "Camera.h"

Material::Material() : Component("Material"){
    shader = new Shader();

    parameters.push_back(new ColorParameter("color", [this](){
        return color;
    }, [this](vec4 value){
        color = value;
    }));
    parameters.push_back(new FloatParameter("ambient", [this](){
        return ambient;
    }, [this](float value){
        ambient = value;
    }));
}

Material::~Material(){
    if (shader != nullptr){
        delete shader;
    }
}

void Material::apply(Transform* model_transform, Camera* camera, vec3 light_position){
    shader->use();

    shader->set_mat4("world", *model_transform->get_matrix());
    shader->set_mat4("view", camera->get_view());
    shader->set_mat4("projection", camera->get_projection());

    shader->set_vec3("objectColor", vec3(color));
    shader->set_vec3("lightPos", light_position);
    shader->set_float("ambient", ambient);
}

Shader* Material::get_shader(){
    return shader;
}