#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "SelectionPixelInfo.h"
#include "Transform.h"
#include "Component.h"
#include "TreeNode.hpp"

#include <glm/glm.hpp>

#include <vector>

class Material;
class CSGNode : public Entity, Component, public TreeNode<CSGNode>, public TransformEntity{
public:
    enum class Type{
        Operand, // children 제한 : 0개
        Union,
        Intersection,
        Difference // children 제한 : 2개
    };
    static const char* type_string_values[];

private:
    class CSGNodeTransform : public TransformComponent{
        CSGNode* csg_node;
    public:
        CSGNodeTransform(CSGNode* csg_node_);
        void on_local_modify() override;
    };

    Mesh result;
    bool is_result_valid = false;

    CSGNodeTransform transform; //  TODO : 포인터로 변경?
    Type type;


public:
    Model* model = nullptr; // root mesh 조작시 사용
    bool selection_group = false;

    CSGNode(const Mesh& mesh);
    CSGNode(Type type_);
    ~CSGNode();

    void set_parent(CSGNode* parent_) override;
    bool add_child(CSGNode* node, CSGNode* after = nullptr);
    bool reparent_child(CSGNode* node, CSGNode* after = nullptr);
    void swap_child(CSGNode* child1, CSGNode* child2);
    bool remove_self();
    bool remove_self_subtree();
    bool unpack_to_parent();

    void mark_edited();

    std::string get_name();
    std::vector<Type> get_changable_types();
    Type get_type();
    void set_type(Type type_);
    Mesh* get_mesh();

    TransformComponent* get_transform() override;

    void calculate_mesh();
    void render();
    void render_operands(Material* material_mesh_overlay);
    void render_monotone(Material* material_monotone);
    void render_selection_id(Material* material, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc);
    SelectionPixelObjectInfo from_selection_id(SelectionPixelIdInfo selection_id, Model* model, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc);
};
