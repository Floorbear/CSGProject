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

Renderer::Renderer(int viewport_width, int viewport_height){
    parent = nullptr;
    camera = nullptr;

    texture_size = vec2(512, 512); // default
    viewport_size = vec2(viewport_width, viewport_height);
    camera = new Camera(viewport_width, viewport_height);
}

Renderer::~Renderer(){
    delete camera;
}

void Renderer::set_parent(GUI* parent_){
    parent = parent_;
}

void Renderer::init(){
}

void Renderer::set_bind_fbo(int texture_width, int texture_height){ // TODO : 리팩토링
    if (fbo == 0){ // 지연 초기화
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        //GLuint  f_tex = CreateTexture(512, 512, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
        glGenTextures(1, &frame_texture);
        glBindTexture(GL_TEXTURE_2D, frame_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, frame_texture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_texture, 0);

        GLuint depthrenderbuffer;
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture_width, texture_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Renderer::render(const std::list<Model*>& models){
    glViewport(0, 0, texture_size.x, texture_size.y);
    set_bind_fbo(texture_size.x, texture_size.y);

    ImVec4 clear_color = ImVec4(0.03f, 0.30f, 0.70f, 1.00f);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera->calculate_view();

    static Model* newModel = NULL;
    if (newModel == NULL){
        newModel = new Model("MyModel");

        //newModel->set_new(Mesh::Cube);
        newModel->set_new(Mesh::compute_intersection(Mesh::Cube2,Mesh::Cube));
        //newModel->set_new(Mesh::Sphere);

        parent->active_workspace->models.push_back(newModel);
        camera->get_transform()->set_position(vec3(0.0f, 0.0f, 20.0f));
        camera->get_transform()->set_rotation({ 0,-90,0 });
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
            model->get_shader()->set_mat4("view", camera->get_view());
            model->get_shader()->set_mat4("projection", camera->get_projection());
        }
        model->render(this);
    }
}

void Renderer::dispose(){
}

void Renderer::resize(int width, int height){
    viewport_size = vec2(width, height);
    camera->resize(width, height);
}