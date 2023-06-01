#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "SelectionPixelInfo.h"
#include "Component.h"
#include "TreeNode.hpp"

#include <glm/glm.hpp>

#include <vector>

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
    class CSGNodeTransform : public TransformComponent{
        CSGNode* csg_node;
    public:
        CSGNodeTransform(CSGNode* csg_node_);
        void on_local_modify() override;
    };

    static const char* type_string_values[];

    Mesh result;
    bool is_result_valid = false;
    void mark_edited();

    CSGNodeTransform transform; //  TODO : 포인터로 변경?
    Type type;

public:
    Model* model = nullptr; // root mesh 조작시 사용

    bool selection_group = false;

    CSGNode(const Mesh& mesh);
    CSGNode(Type type_, CSGNode* node1, CSGNode* node2);
    ~CSGNode();

    void set_parent(CSGNode* parent_) override;
    bool add_child(CSGNode* node) override;
    bool reparent_child(CSGNode* node, CSGNode* after = nullptr) override;
    void swap_child(CSGNode* child1, CSGNode* child2) override;

    std::string get_name();
    std::vector<Type> get_changable_types();
    Type get_type();
    void set_type(Type type_);

    TransformComponent* get_transform();
    // Transform get_transform_copy();
    // Transform get_transform_scaleUp_copy(const vec3& _scaleAcc); //_scaleAcc : 스케일 커질 배율

    void calculate_mesh();
    void render();
    void render_selection_id(Material* material, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc);
    SelectionPixelObjectInfo from_selection_id(SelectionPixelIdInfo selection_id, Model* model, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc);
};
