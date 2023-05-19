#pragma once

#include "Mesh.h"
#include "SelectionPixelInfo.h"
#include "Component.h"
#include "TreeNode.hpp"

#include <glm/glm.hpp>

#include <list>

class CSGNode;
class Material;
class Model : public Entity, public TreeNode<Model>{
    CSGNode* csgmesh = nullptr; // root node
    Material* material_ptr = nullptr;

public:
    std::string name;

    Model(std::string name_);
    ~Model();
    
    CSGNode* get_csg_mesh();
    void set_csg_mesh(CSGNode* csgmesh_);
    void set_csg_mesh_new(const Mesh& mesh);
    Model* find_model(std::string_view name_);

    Transform* get_transform(); // 필수적인 컴포넌트들 get set
    Material* get_material();
    void set_material(Material* material_);

    bool is_renderable();
    void render();

    void render_selection_id(uint32_t* selection_id_model_acc);
    SelectionPixelObjectInfo from_selection_id(SelectionPixelIdInfo selection_id, uint32_t* selection_id_model_acc);

    //Transform get_world_position();
};