#pragma once

#include "Mesh.h"
#include "SelectionPixelInfo.h"
#include "Component.h"
#include "TreeNode.hpp"
#include "Gizmo.h"

#include <glm/glm.hpp>

#include <list>

class CSGNode;
class Material;
class Renderer;
class Model : public Entity, public TreeNode<Model>, public TransformEntity{
    CSGNode* csgmesh = nullptr; // root node
    TransformComponent* transform = nullptr;
    Material* material = nullptr;

public:
    std::string name;

    Model(std::string name_);
    ~Model();

    void set_parent(Model* parent_) override;

    CSGNode* get_csg_mesh();
    void set_csg_mesh(CSGNode* csgmesh_, bool fix_mesh_position = false);
    void set_csg_mesh_new(const Mesh& mesh);

    Model* find_model(std::string_view name_);

    // 필수적인 컴포넌트들 get set
    TransformComponent* get_transform() override;
    Material* get_material();
    void set_material(Material* material_);

    bool is_renderable();
    void render(Renderer* renderer);
    void render_monotone(Material* material_monotone);

    void render_selection_id(uint32_t* selection_id_model_acc);
    SelectionPixelObjectInfo from_selection_id(SelectionPixelIdInfo selection_id, uint32_t* selection_id_model_acc);
};
