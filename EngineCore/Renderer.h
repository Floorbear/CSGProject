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
    CSGNode* csgmesh; // root node
    Shader* shader;

public:
    std::string name;

    Model(std::string name_);
    ~Model();
    void set_new(const Mesh& mesh);

    Transform get_transform();
    //Transform get_world_position();
    Transform* test_get_main_transform(); // TEST
    void set_transform(Transform transform);
    void set_position(Transform transform);
    void translate(vec3 delta); // 자식들까지 이동
    // TODO : transform 조정 기능들을 그대로 복사? : 이것은 Model자신의 transform이 존재하지않고 대표 메쉬의 것을 대신 사용하기 때문

    Shader* get_shader();
    void render(class Renderer* renderer);
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

