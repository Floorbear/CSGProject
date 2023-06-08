#include "Material.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "PointLight.h"

Material::Material() : Component("Material"){
    shader = new Shader();
    selectionShader = new Shader("DefaultVertexShader.glsl", "SelectionFragmentShader.glsl");
}

Material::~Material(){
    if (shader != nullptr){
        delete shader;
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

const std::list<PointLight*>* Material::get_uniform_lights(){
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

void Material::set_uniform_lights(const std::list<PointLight*>* lights){
    uniform_lights = lights;
}

void Material::set_uniform_selection_id(SelectionPixelIdInfo selection_id){
    uniform_selection_id = selection_id;
}

void Material::apply(){
    shader->use();

    shader->set_mat4("world", uniform_model_transform->get_world_matrix());
    shader->set_mat4("view", uniform_camera->get_view());
    shader->set_mat4("projection", uniform_camera->get_projection());

    if (uniform_lights == nullptr || uniform_lights->empty()){ // TODO : 여러개 입력으로 바꾸기?
        shader->set_vec3("lightPos", vec3(0, 0, 0));// TODO : 지우기
    } else{
        shader->set_vec3("lightPos", vec3(uniform_lights->front()->get_position()));
    }
}

void Material::apply_selection_id(){
    selectionShader->use();

    selectionShader->set_mat4("world", uniform_model_transform->get_world_matrix());
    selectionShader->set_mat4("view", uniform_camera->get_view());
    selectionShader->set_mat4("projection", uniform_camera->get_projection());

    selectionShader->set_uint("objectType", uniform_selection_id.object_type);
    selectionShader->set_uint("modelID", uniform_selection_id.model_id);
    selectionShader->set_uint("meshID", uniform_selection_id.mesh_id);
}


// ===== MonotoneMaterial ===== //

MonotoneMaterial::MonotoneMaterial(vec3 color_) : color(color_){
    shader = new Shader("monotone_vs.glsl", "monotone_fs.glsl");
}

void MonotoneMaterial::apply(){
    Material::apply();

    shader->set_vec3("color", vec3(color));
}

vec3 MonotoneMaterial::get_color(){
    return color;
}


// ===== ColorMaterial ===== //

ColorMaterial::ColorMaterial(){
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

void ColorMaterial::apply(){
    Material::apply();

    shader->set_int("fragmentShaderType", static_cast<int>(FragmentShaderType::Color));

    shader->set_vec3("objectColor", vec3(color));
    shader->set_float("ambient", ambient);
}


// ===== TextureMaterial ===== //

TextureMaterial::TextureMaterial() : TextureMaterial(nullptr){
    // TODO : texture parameter 구현해서 추가
}

TextureMaterial::TextureMaterial(Texture* texture_) : texture(texture_){
}

TextureMaterial::~TextureMaterial(){
    delete texture;
}

void TextureMaterial::apply(){
    Material::apply();

    shader->set_int("fragmentShaderType", static_cast<int>(FragmentShaderType::Texture));
    //텍스처가 존재하는경우 유니폼으로 보내버립니다
    if (texture != nullptr){
        shader->set_int("texture1", 0); //텍스처는 location은  0->1->2->3 , texture 
        glActiveTexture(GL_TEXTURE0);
        texture->enable();
        //if 2번째 texture
        //screenShader->set_int("texture2", 1); //텍스처는 location은  0->1->2->3 , texture 
        //glActiveTexture(GL_TEXTURE1);
        //texture2->enable();
    }
}

Texture* TextureMaterial::get_texture() const{
    return texture;
}

void TextureMaterial::set_texture(Texture* _texture){
    texture = _texture;
}
