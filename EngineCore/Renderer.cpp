#include "Renderer.h"
#include "Core.h"
#include "Utils.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    // TODO : 정리 필요
    shader->Use();

    //----------------------------- view & projection
    mat4 view = mat4(1.0f);
    view = glm::translate(view, vec3(0.0, 0.0, -5.f));

    mat4 projection = mat4(1.0f);
    float fov = 45.f;
    projection = glm::perspective(glm::radians(fov), renderer->viewport_size.x / renderer->viewport_size.y, 0.1f, 100.f);

    // TODO 여기까진 고정인걸 어떻게 전달할지 다시 고민?

    int worldLocation = glGetUniformLocation(shader->GetShaderProgram(), "world");
    int viewLocation = glGetUniformLocation(shader->GetShaderProgram(), "view");
    int projectionLocation = glGetUniformLocation(shader->GetShaderProgram(), "projection");
    
    glUniformMatrix4fv(worldLocation, 1, GL_FALSE, glm::value_ptr(*(get_transform().GetTransformMat())));
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    csgmesh->render();
}


// ===== Renderer ===== //

Renderer::Renderer(){
    parent = NULL;
    viewport_size = vec2(100, 100); // default
}

void Renderer::set_parent(GUI* parent_){
    parent = parent_;
}

void Renderer::init(){
}

void Renderer::render(const std::list<Model*>& models){
    static Model* newModel = NULL;
    if (newModel == NULL){
        newModel = new Model("MyModel");
        newModel->set_new(Mesh::Cube);

        newModel->test_get_main_transform()->AddLocalPosition({ 3.f,0,0 });
        parent->active_workspace->models.push_back(newModel);// 원래는 이런걸 참조할일이 많지 않다
                                                             //newModel->main_child()->get_transform()->AddLocalPosition({3.f,0,0}); 내일 이거 열어보기
    }

    Model* model = parent->active_workspace->find_model("MyModel");
    if(model!=NULL){
        //CSGMesh* newMesh = 
        Transform* newMesh = model->test_get_main_transform();//->FindMesh("Cube1");
        newMesh->SetLocalPosition(vec3(cos(Utils::time_acc()), sin(Utils::time_acc()), 0));
        newMesh->SetLocalScale(vec3(0.5f, 0.5f, 0.5f));
        newMesh->SetLocalRotation(vec3(Utils::time_acc() * 100, 0, 0));
    }

    for (Model* model : models){
        model->render(this);
    }
}

void Renderer::dispose(){
}