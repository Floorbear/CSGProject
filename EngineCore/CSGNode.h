#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "SelectionPixelInfo.h"
#include "Component.h"
#include "TreeNode.hpp"

#include <glm/glm.hpp>

#include <list>

class Material;
class CSGNode : public Entity, Component, public TreeNode<CSGNode>{
public:
    enum class Type{
        Operand, // children 제한 : 0개
        Union,
        Intersection,
        Difference // children 제한 : 2개
    };

private:
    static const char* type_string_values[];

    class CSGNodeTransform : public TransformComponent{ // 트리 구조에서 자식들을 모두 함께 움직여야하기 때문에 필요
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

    Mesh result;
    bool is_result_valid = false;

    CSGNodeTransform transform;
    Type type;

public:
    Model* model = nullptr; // root mesh 조작시 사용

    bool selection_group = false;

    CSGNode(const Mesh& mesh);
    CSGNode(Type type_, CSGNode* node1, CSGNode* node2);
    ~CSGNode();

    bool add_child(CSGNode* node) override;
    bool reparent_child(CSGNode* node, CSGNode* after = nullptr) override;
    void swap_child(CSGNode* child1, CSGNode* child2) override;

    std::list<Type> get_changable_types();
    Type get_type();
    void set_type(Type type_);

    CSGNode* main_child();
    TransformComponent* get_transform();
    // Transform get_transform_copy();
    // Transform get_transform_scaleUp_copy(const vec3& _scaleAcc); //_scaleAcc : 스케일 커질 배율

    void render();
    void render_selection_id(Material* material, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc);
    SelectionPixelObjectInfo from_selection_id(SelectionPixelIdInfo selection_id, Model* model, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc);
};
