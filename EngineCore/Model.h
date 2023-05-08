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
public:
    std::string name;

    Model(std::string name_);
    ~Model();
    void set_new(const Mesh& mesh);

    Model* get_parent();
    std::list<Model*> get_children();
    bool is_leaf_node();
    CSGNode* get_csg_mesh();

    void add_component(Component* component);
    std::list<Component*> get_components();
    // TODO : add_component();
    // TODO : get_component(type);

    Transform* get_transform(); // 필수적인 컴포넌트들 get

    bool is_renderable();
    void render();

    void render_selection_id(uint32_t* selection_id_model_acc);
    SelectionPixelObjectInfo from_selection_id(SelectionPixelIdInfo selection_id, uint32_t* selection_id_model_acc);

    //Transform get_world_position();
    //===== Material =====
public:
    Material* get_material();

    template <typename MaterialType>
    void set_material()
    {
        //이미 존재하는 마테리얼(Default = Color)을 삭제하고 새 마테리얼을 할당합니다.
        if (material != nullptr)
        {
            delete material;
            material = nullptr;
        }
        material = new MaterialType();
        assert(material != nullptr);//잘못된 형변환 체크
    }
private:
    Material* material = nullptr;
};