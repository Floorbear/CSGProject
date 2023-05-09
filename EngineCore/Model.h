#pragma once

#include "Mesh.h"
#include "SelectionPixelInfo.h"

#include <glm/glm.hpp>

#include <list>

class CSGNode;
class Material;
class Model {
    Model* parent = nullptr;
    std::list<Model*> children;

    std::list<Component*> components;
    CSGNode* csgmesh = nullptr; // root node
    Material* material_ptr;

public:
    std::string name;

    Model(std::string name_);
    ~Model();
    
    Model* get_parent();
    std::list<Model*> get_children();
    void add_child(Model* model);
    bool is_leaf_node();
    CSGNode* get_csg_mesh();

    void set_new(const Mesh& mesh);
    Model* find_model(std::string_view name_);

    void add_component(Component* component);
    std::list<Component*> get_components();
    // TODO : add_component();
    // TODO : get_component(type);

    Transform* get_transform(); // 필수적인 컴포넌트들 get set
    Material* get_material();
    void set_material(Material* material_);

    bool is_renderable();
    void render();

    void render_selection_id(uint32_t* selection_id_model_acc);
    SelectionPixelObjectInfo from_selection_id(SelectionPixelIdInfo selection_id, uint32_t* selection_id_model_acc);

    //Transform get_world_position();
};