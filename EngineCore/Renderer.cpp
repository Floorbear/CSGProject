#include "Renderer.h"
#include "Core.h"
#include "Utils.h"
#include "Camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(disable : 4717)

// ===== CSGMesh ===== // TODO : CSGNode?

CSGNode::CSGNode(const Mesh& mesh) : result(mesh), transform(CSGNodeTransform(this)){
    type = Type::Operand;
}

CSGNode::CSGNode(Type type_, const Mesh& mesh1, const Mesh& mesh2) : transform(CSGNodeTransform(this)){
    assert(type != Type::Operand);
    type = type_;
    children.push_back(new CSGNode(mesh1));
    children.push_back(new CSGNode(mesh1));
}

CSGNode::~CSGNode(){
    for (CSGNode* child : children){
        delete child;
    }
}

CSGNode* CSGNode::get_parent(){
    return parent;
}

std::list<CSGNode*> CSGNode::get_children(){
    return children;
}

CSGNode* CSGNode::main_child(){
    if(children.empty()){
        return nullptr;
    }
    return children.front();
}

Transform* CSGNode::get_transform(){
    if (type == Type::Operand){
        return (Transform*)&transform;
    }
    if (!children.empty()){
        return main_child()->get_transform();
    }
    return NULL;
}

void CSGNode::render(){
    result.render();
}

CSGNode::CSGNodeTransform::CSGNodeTransform(CSGNode* parent_) : Transform(), parent(parent_){
}
// TODO : gui 조작을 위한 특정시점 값 저장 후 차이값 적용 기능 추가

void CSGNode::CSGNodeTransform::set_position(const vec3& value){
    vec3 delta = value - get_position();
    Transform::set_position(value);
    for(CSGNode* node : parent->children){
        node->transform.add_position(delta);
    }
}

void CSGNode::CSGNodeTransform::set_rotation(const vec3& value){
    vec3 delta = value - get_rotation();
    Transform::set_rotation(value);
    for(CSGNode* node : parent->children){
        node->transform.rotate(delta);
    }
}

void CSGNode::CSGNodeTransform::set_scale(const vec3& value){
    vec3 ratio = value / get_scale();
    Transform::set_scale(value);
    for(CSGNode* node : parent->children){
        node->transform.scale(ratio);
    }
}

void CSGNode::CSGNodeTransform::translate(const vec3& value){
    
}

void CSGNode::CSGNodeTransform::rotate(const vec3& value){
}

void CSGNode::CSGNodeTransform::scale(const vec3& value){
}

void CSGNode::CSGNodeTransform::add_position(const vec3& value){
    parent->transform.add_position(value);
    for(CSGNode* node : parent->children){
        node->transform.add_position(value);
    }
}


// ===== Model ===== //

vec3 Model::lightPos = vec3(30, -100, -50);

Model::Model(std::string name_) : name(name_){
    shader = new Shader();
}

Model::~Model(){
    if (csgmesh != nullptr){
        delete csgmesh;
    }
    if (shader != nullptr){
        delete shader;
    }
}

void Model::set_new(const Mesh& mesh){
    csgmesh = new CSGNode(mesh);
    // TODO : 기존 transform 제거
    components.push_back(csgmesh->get_transform());
    // TODO : material 추가
}

Model* Model::get_parent(){
    return parent;
}

std::list<Model*> Model::get_children(){
    return children;
}

CSGNode* Model::get_mesh(){
    return csgmesh;
}

std::list<Component*> Model::get_components(){
    return components;
}

Transform* Model::get_transform(){
    if(csgmesh == nullptr){
        return nullptr;
    }
    return csgmesh->get_transform();
}

Shader* Model::get_shader(){
    if(csgmesh == nullptr){
        return nullptr;
    }
    return shader;
}

bool Model::is_renderable(){
    return csgmesh != nullptr;
}

void Model::render(Renderer* renderer){
    lightPos.x = 50 * sin(Utils::time_acc());
    lightPos.z = 50*sin(Utils::time_acc());
    get_shader()->set_vec3("objectColor", objectColor);
    get_shader()->set_vec3("lightPos", lightPos);
    get_shader()->set_float("ambient", ambient);
    csgmesh->render();
}


// ===== Renderer ===== //

Renderer::Renderer(){
    parent = NULL;
    viewport_size = vec2(100, 100); // default
    main_camera = nullptr;
}

void Renderer::set_parent(GUI* parent_){
    parent = parent_;
}

void Renderer::init(){
}

void Renderer::render(const std::list<Model*>& models){

    if (main_camera == nullptr){
        main_camera = parent->active_workspace->get_mainCamera();
    }

    static Model* newModel = NULL;
    if (newModel == NULL){
        newModel = new Model("MyModel");

        //newModel->set_new(Mesh::Cube);
        newModel->set_new(Mesh::compute_intersection(Mesh::Cube2,Mesh::Cube));
        //newModel->set_new(Mesh::Sphere);

        parent->active_workspace->models.push_back(newModel);
        main_camera->get_transform()->set_position(vec3(0.0f, 0.0f, -5.0f));
    }


    Model* model = parent->active_workspace->find_model("MyModel");
    if(model!=NULL){
        //CSGMesh* newMesh = 
        /*Transform* newMesh = model->get_transform();
        newMesh->set_position(vec3(0,0, 2));
        newMesh->set_scale(vec3(1.5f, 1.0f, 0.5f));*/
    }
    for (Model* model : models){
        
        model->get_shader()->Use();

        //좌표 정보 전달
        {
            model->get_shader()->set_mat4("world", *model->get_transform()->get_matrix());
            model->get_shader()->set_mat4("view", main_camera->get_view());
            model->get_shader()->set_mat4("projection", main_camera->get_projection());
        }
        model->render(this);
    }
}

void Renderer::dispose(){
}