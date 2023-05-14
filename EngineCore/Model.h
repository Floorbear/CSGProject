#pragma once

#include "Mesh.h"
#include "SelectionPixelInfo.h"
#include "Component.h"

#include <glm/glm.hpp>

#include <list>

class CSGNode;
class Material;
class Model : public Entity{
    Model* parent = nullptr;
    std::list<Model*> children;

    CSGNode* csgmesh = nullptr; // root node
    Material* material_ptr = nullptr;

public:
    std::string name;

    Model(std::string name_);
    ~Model();
    
    Model* get_parent();
    std::list<Model*> get_children();
    void add_child(Model* model);
    void set_child(Model* model);
    void swap_child(Model* child1, Model* child2);
    bool is_leaf_node();
    CSGNode* get_csg_mesh();

    void set_new(const Mesh& mesh);
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