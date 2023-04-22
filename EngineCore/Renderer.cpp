#include "Renderer.h"
#include "Core.h"
#include "Utils.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EngineCore/Camera.h"
// ===== CSGMesh ===== // TODO : CSGNode?

CSGNode::CSGNode(const Mesh& mesh) : result(mesh){
    type = Type::Operand;
}

CSGNode::CSGNode(Type type_, const Mesh& mesh1, const Mesh& mesh2){
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

CSGNode* CSGNode::main_child(){
    if(children.empty()){
        return nullptr;
    }
    return children.front();
}

Transform* CSGNode::get_transform(){
    if (type == Type::Operand){
        return &transform;
    }
    if (!children.empty()){
        return main_child()->get_transform();
    }
    return NULL;
}

void CSGNode::render(){
    result.render();
}


// ===== Model ===== //

Model::Model(std::string name_) : name(name_){
    csgmesh = nullptr;
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
}

Transform Model::get_transform(){
    return *(csgmesh->get_transform());
}


Transform* Model::test_get_main_transform(){ // TEST
    return csgmesh->get_transform();
}

Shader* Model::get_shader(){
    return shader;
}

void Model::render(Renderer* renderer){
    get_shader()->set_vec3("objectColor", objectColor);
    csgmesh->render();
}


// ===== Renderer ===== //

Renderer::Renderer(){
    parent = NULL;
    viewport_size = vec2(100, 100); // default
    camera = nullptr;
}

void Renderer::set_parent(GUI* parent_){
    parent = parent_;
}

void Renderer::init(){
}

void Renderer::render(const std::list<Model*>& models){

    if (camera == nullptr) //디폴트 카메라 = mainCamera
    {
        camera = parent->active_workspace->get_mainCamera();
    }

    static Model* newModel = NULL;
    if (newModel == NULL){
        newModel = new Model("MyModel");
        newModel->set_new(Mesh::Cube);
        parent->active_workspace->models.push_back(newModel);
        camera->get_transform()->set_localPostition(vec3(0.0f, 0.0f, -5.0f));
    }


    Model* model = parent->active_workspace->find_model("MyModel");
    if(model!=NULL){
        //CSGMesh* newMesh = 
        Transform* newMesh = model->test_get_main_transform();//->FindMesh("Cube1");
        newMesh->set_localPostition(vec3(0,0, 2));
        newMesh->set_localScale(vec3(1.5f, 1.0f, 0.5f));
    }

    for (Model* model : models){
        model->get_shader()->Use();

        //좌표 정보 전달
        {
            model->get_shader()->set_mat4("world", *model->get_transform().get_transformMat());
            model->get_shader()->set_mat4("view", camera->get_view());
            model->get_shader()->set_mat4("projection", camera->get_projection());
        }
        model->render(this);
    }
}

void Renderer::dispose(){
}