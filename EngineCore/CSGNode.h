#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "SelectionPixelInfo.h"

#include <glm/glm.hpp>

#include <list>

class Material;
class CSGNode {
public:
    enum class Type {
        Operand, // children 제한 : 0개
        Union,
        Intersection,
        Difference // children 제한 : 2개
    };

private:
    CSGNode* parent = nullptr;
    std::list<CSGNode*> children;

    Type type;
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
    CSGNodeTransform transform; // CSGNode의 유일한 component

public:
    bool selection_group = false;

    CSGNode(const Mesh& mesh);
    CSGNode(Type type_, const Mesh& mesh1, const Mesh& mesh2);
    ~CSGNode();

    CSGNode* get_parent();
    std::list<CSGNode*> get_children();
    bool is_leaf_node();

    CSGNode* main_child();
    Transform* get_transform();

    void render();
    void render_selection_id(Material* material, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc);
    SelectionPixelObjectInfo from_selection_id(SelectionPixelIdInfo selection_id, Model* model, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc);
};
