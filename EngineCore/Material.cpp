#include "Material.h"
#include "Shader.h"
#include "Camera.h"

Material::Material() : Component("Material"){
    screenShader = new Shader();
    selectionShader = new Shader("DefaultVertexShader.glsl", "SelectionFragmentShader.glsl");

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
    if (screenShader != nullptr){
        delete screenShader;
    }
    if (selectionShader != nullptr){
        delete selectionShader;
    }
}

Transform* Material::get_uniform_model_transform(){
    return uniform_model_transform;
}

Camera* Material::get_uniform_camera(){
    return uniform_camera;
}

vec3 Material::get_uniform_lights(){
    return uniform_lights;
}

SelectionPixelIdInfo Material::get_uniform_selection_id(){
    return uniform_selection_id;
}


void Material::set_uniform_model_transform(Transform* model_transform){
    uniform_model_transform = model_transform;
}

void Material::set_uniform_camera(Camera* camera){
    uniform_camera = camera;
}

void Material::set_uniform_lights(vec3 lights){
    uniform_lights = lights;
}

void Material::set_uniform_selection_id(SelectionPixelIdInfo selection_id){
    uniform_selection_id = selection_id;
}

void Material::apply(){
    screenShader->use();

    screenShader->set_mat4("world", *uniform_model_transform->get_matrix());
    screenShader->set_mat4("view", uniform_camera->get_view());
    screenShader->set_mat4("projection", uniform_camera->get_projection());

    screenShader->set_vec3("objectColor", vec3(color));
    screenShader->set_vec3("lightPos", vec3(uniform_lights));
    screenShader->set_float("ambient", ambient);
}

void Material::apply_selection_id(){
    selectionShader->use();

    selectionShader->set_mat4("world", *uniform_model_transform->get_matrix());
    selectionShader->set_mat4("view", uniform_camera->get_view());
    selectionShader->set_mat4("projection", uniform_camera->get_projection());

    selectionShader->set_uint("objectID", uniform_selection_id.model_id); // TODO : 구조체 다 보내도록 처리, 셰이더도 수정.
}