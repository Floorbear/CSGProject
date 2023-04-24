#pragma once

#include "Mesh.h"
#include "Shader.h"

#include <glm/glm.hpp>

#include <list>

using namespace glm;
class GUI;

class CSGNode{
    std::list<CSGNode*> children;
    Mesh result;

    Transform transform;
public:
    enum class Type{
        Operand, // children 제한 : 0개
        Union,
        Intersection,
        Difference // children 제한 : 2개
    };

    Type type; // private?

    CSGNode(const Mesh& mesh);
    CSGNode(Type type_, const Mesh& mesh1, const Mesh& mesh2);
    ~CSGNode();

    CSGNode* main_child();
    Transform* get_transform(); // TODO : child에서 자기 트랜스폼 조작하게하면 포인터 반환 필요없음?
    void render();
};

class Model{
    std::list<Component> components;
    CSGNode* csgmesh; // root node
    Shader* shader;

    class ModelTransform : Transform{ // 트리 구조에서 자식들을 모두 함께 움직여야하기 때문에 필요
        Model* parent; // 값 타입으로 전달할땐 슬라이싱 되어도 무관, Transform*으로 전달할땐 접근 못해도 무관
        ModelTransform();
    };

public:
    std::string name;

    Model(std::string name_);
    ~Model();
    void set_new(const Mesh& mesh);


    // TODO : getcomponent

    Transform* get_transform();
    //Transform get_world_position();
    
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

public:
    vec2 viewport_size;
    // Camera main_camera;

    Renderer();
    void set_parent(GUI* parent_);

    void init();
    void render(const std::list<Model*>& models);
    void dispose();

    //===== Camera ======
    float xPos = 0.f;
    float yPos = 0.f;
    float zPos = 0.f;

    float xDegree = 0.f;
    float yDegree = 0.f;

    Transform cameraTransfrom;

    Camera* camera;
};

