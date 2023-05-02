#pragma once

#include "Mesh.h"

#include <glm/glm.hpp>

#include <list>

class CSGNode;
class Material;
class Model {
    Model* parent = nullptr;
    std::list<Model*> children;

    std::list<Component*> components;
    CSGNode* csgmesh = nullptr; // root node
    Material* material = nullptr;

public:
    std::string name;

    Model(std::string name_);
    ~Model();
    void set_new(const Mesh& mesh);

    Model* get_parent();
    std::list<Model*> get_children();
    bool is_leaf_node();
    CSGNode* get_csg_mesh();

    std::list<Component*> get_components();
    // TODO : add_component();
    // TODO : get_component(type);

    Transform* get_transform(); // 필수적인 컴포넌트들 get
    Material* get_material();
    //Transform get_world_position();

    bool is_renderable();
    void render(class Renderer* renderer);
};