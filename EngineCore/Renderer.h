#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Utils.h"

#include <glm/glm.hpp>

#include <list>

using namespace glm;
class GUI;

class CSGNode{
    CSGNode* parent = nullptr;
    std::list<CSGNode*> children;
    Mesh result;

    class CSGNodeTransform : Transform{ // 트리 구조에서 자식들을 모두 함께 움직여야하기 때문에 필요
        CSGNode* parent; // 값 타입으로 전달할땐 슬라이싱 되어도 무관, Transform*으로 전달할땐 접근 못해도 무관

    public:
        CSGNodeTransform(CSGNode* parent_);

        void set_position(const vec3& value) override;
        void set_rotation(const vec3& value) override;
        void set_scale(const vec3& value) override;

        void translate(const vec3& value) override;
        void rotate(const vec3& value) override;
        void scale(const vec3& value) override;
        void add_position(const vec3& value) override;
    };

    CSGNodeTransform transform;

public:
    enum class Type{
        Operand, // children 제한 : 0개
        Union,
        Intersection,
        Difference // children 제한 : 2개
    };

    Type type; // private? get / set

    CSGNode(const Mesh& mesh);
    CSGNode(Type type_, const Mesh& mesh1, const Mesh& mesh2);
    ~CSGNode();

    CSGNode* get_parent();
    std::list<CSGNode*> get_children();

    CSGNode* main_child();
    Transform* get_transform();
    void render();

};

class Model{
    Model* parent = nullptr;
    std::list<Model*> children;

    std::list<Component*> components;
    CSGNode* csgmesh = nullptr; // root node
    Shader* shader = nullptr;

public:
    std::string name;

    Model(std::string name_);
    ~Model();
    void set_new(const Mesh& mesh);

    Model* get_parent();
    std::list<Model*> get_children();
    CSGNode* get_mesh();

    std::list<Component*> get_components();
    // TODO : getcomponent

    Transform* get_transform();
    //Transform get_world_position();
    
    bool is_renderable();
    void render(class Renderer* renderer);


    //===== Material =====
    static vec3 lightPos;
    vec3 objectColor = { 1.0f,1.0f,0.0f };
    float ambient = 0.1f;

    Shader* get_shader();

};

class Camera;
class Renderer{
    GUI* parent;

    vec2 texture_size;
    vec2 viewport_size;

    GLuint fbo = 0;
    void set_bind_fbo(int texture_width, int texture_height);

public:
    GLuint frame_texture = 0;
    Camera* camera;

    Renderer(int viewport_width, int viewport_height);
    ~Renderer();
    void set_parent(GUI* parent_);

    void init();
    void render(const std::list<Model*>& models);
    void dispose();

    void resize(int viewport_width, int viewport_height);

    //===== Camera ======
    float xPos = 0.f;
    float yPos = 0.f;
    float zPos = 0.f;

    float xDegree = 0.f;
    float yDegree = 0.f;

    Transform cameraTransfrom;
};

