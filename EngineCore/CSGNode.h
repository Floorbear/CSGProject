#pragma once

#include "Mesh.h"
#include "Shader.h"

#include <glm/glm.hpp>

#include <list>

class CSGNode {
    CSGNode* parent = nullptr;
    std::list<CSGNode*> children;
    Mesh result;

    class CSGNodeTransform : Transform { // 트리 구조에서 자식들을 모두 함께 움직여야하기 때문에 필요
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
    enum class Type {
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
