#include "Material.h"
#include "Shader.h"
#include "Camera.h"

Material::Material() : Component("Material"){
    screenShader = new Shader();
    selectionShader = new Shader();

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

    if (selectionShader != nullptr) {
        delete selectionShader;
    }
}

void Material::apply(Transform* model_transform, Camera* camera, vec3 light_position, RenderSpace _space){
    switch (_space)
    {
    case RenderSpace::Selection:
    {
        selectionShader->use();

        selectionShader->set_mat4("world", *model_transform->get_matrix());
        selectionShader->set_mat4("view", camera->get_view());
        selectionShader->set_mat4("projection", camera->get_projection());
        break;
    }

    case RenderSpace::Screen:
    {
        screenShader->use();

        screenShader->set_mat4("world", *model_transform->get_matrix());
        screenShader->set_mat4("view", camera->get_view());
        screenShader->set_mat4("projection", camera->get_projection());

        screenShader->set_vec3("objectColor", vec3(color));
        screenShader->set_vec3("lightPos", light_position);
        screenShader->set_float("ambient", ambient);
        break;
    }
    }

}
