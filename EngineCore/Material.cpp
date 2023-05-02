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


void Material::set_uniform_transform(Transform* model_transform){
    uniform_model_transform = model_transform;
}

void Material::set_uniform_camera(Camera* camera){
    uniform_camera = camera;
}

void Material::set_uniform_lights(vec3 lights){
    uniform_lights = lights;
}

void Material::set_uniform_selection_id(int selection_id){
    uniform_selection_id = selection_id;
}

void Material::apply(){
    screenShader->use();

    screenShader->set_mat4("world", *uniform_model_transform->get_matrix());
    screenShader->set_mat4("view", uniform_camera->get_view());
    screenShader->set_mat4("projection", uniform_camera->get_projection());

    screenShader->set_vec3("objectColor", vec3(color));
    screenShader->set_float("ambient", ambient);
}

void Material::apply_object_selection(){
    selectionShader->use();

    selectionShader->set_mat4("world", *uniform_model_transform->get_matrix());
    selectionShader->set_mat4("view", uniform_camera->get_view());
    selectionShader->set_mat4("projection", uniform_camera->get_projection());

    selectionShader->set_uint("objectID", uniform_selection_id);
}