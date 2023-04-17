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
    }

    Model* model = parent->active_workspace->find_model("MyModel");
    if(model!=NULL){
        //CSGMesh* newMesh = 
        Transform* newMesh = model->test_get_main_transform();//->FindMesh("Cube1");
        newMesh->set_localPostition(vec3(0,0, 5));
       // newMesh->SetLocalPosition(vec3(cos(Utils::time_acc()), sin(Utils::time_acc()), Utils::time_acc()));
        newMesh->set_localScale(vec3(0.5f, 0.5f, 0.5f));
        //newMesh->SetLocalRotation(vec3(Utils::time_acc() * 100, 0, 0));
    }



    //===== Camera ====
    {
        static int testValue = 0;
        //bind Key
        if (testValue == 0)
        {
            testValue = 1;
            parent->shortcuts.push_back(Shortcut("D", false, false, false, ImGuiKey_D, [=]() {
                camera->get_transform()->add_localPosition({ 0.5f,0,0 });
                }));
            parent->shortcuts.push_back(Shortcut("A", false, false, false, ImGuiKey_A, [=]() {
                camera->get_transform()->add_localPosition({ -0.5f,0,0 });
                }));
            parent->shortcuts.push_back(Shortcut("W", false, false, false, ImGuiKey_W, [=]() {
                camera->get_transform()->add_localPosition({ 0.f,0.5f,0 });
                }));
            parent->shortcuts.push_back(Shortcut("S", false, false, false, ImGuiKey_S, [=]() {
                camera->get_transform()->add_localPosition({ 0.f,-0.5f,0 });
                }));
            parent->shortcuts.push_back(Shortcut("Q", false, false, false, ImGuiKey_Q, [=]() {
                zPos += 0.5f;
                }));
            parent->shortcuts.push_back(Shortcut("E", false, false, false, ImGuiKey_E, [=]() {
                zPos -= 0.5f;
                }));
            parent->shortcuts.push_back(Shortcut("Z", false, false, false, ImGuiKey_Z, [=]() {
                xDegree -= 10.f;
                }));
            parent->shortcuts.push_back(Shortcut("X", false, false, false, ImGuiKey_X, [=]() {
                xDegree += 10.0f;
                }));
            parent->shortcuts.push_back(Shortcut("C", false, false, false, ImGuiKey_C, [=]() {
                yDegree -= 10.0f;
                }));
            parent->shortcuts.push_back(Shortcut("V", false, false, false, ImGuiKey_V, [=]() {
                yDegree += 10.0f;
                }));
        }
    }



    for (Model* model : models){
        model->get_shader()->Use();

        {
            unsigned int shaderProgram = model->get_shader()->get_shaderProgram();
            int worldLocation = glGetUniformLocation(shaderProgram, "world");
            int viewLocation = glGetUniformLocation(shaderProgram, "view");
            int projectionLocation = glGetUniformLocation(shaderProgram, "projection");

            glUniformMatrix4fv(worldLocation, 1, GL_FALSE, glm::value_ptr(*(model->get_transform().get_transformMat())));
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(camera->get_view()));
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(camera->get_projection()));
        }

        model->render(this);
    }
}

void Renderer::dispose(){
}