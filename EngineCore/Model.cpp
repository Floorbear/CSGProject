#include "Model.h"
#include "CSGNode.h"
#include "Material.h"
#include "Utils.h"

Model::Model(std::string name_) : name(name_){
}

Model::~Model(){
    if (csgmesh != nullptr){
        delete csgmesh;
    }
    delete material;
}

void Model::set_new(const Mesh& mesh){
    csgmesh = new CSGNode(mesh);
    // TODO : 기존 transform 제거
    components.push_back(csgmesh->get_transform());
    components.push_back(material = new Material());
}

Model* Model::get_parent(){
    return parent;
}

std::list<Model*> Model::get_children(){
    return children;
}

bool Model::is_leaf_node(){
    return children.empty();
}

CSGNode* Model::get_csg_mesh(){
    return csgmesh;
}

std::list<Component*> Model::get_components(){
    return components;
}

Transform* Model::get_transform(){
    if (csgmesh == nullptr){
        return nullptr;
    }
    return csgmesh->get_transform();
}

Material* Model::get_material(){
    return material;
}

bool Model::is_renderable(){
    return csgmesh != nullptr;
}

void Model::render(){
    // TODO : csg 연산 젹용해서 지연 연산
    material->set_uniform_model_transform(get_transform());
    material->apply();
    csgmesh->render();

    for (Model* child : children){
        child->material->set_uniform_camera(material->get_uniform_camera());
        child->material->set_uniform_lights(material->get_uniform_lights());
        child->render();
    }
}

void Model::render_selection_id(uint32_t* selection_id_model_acc){
    uint32_t selection_id_mesh_acc = 1;

    material->set_uniform_model_transform(get_transform());
    material->apply_selection_id();
    csgmesh->render_selection_id(material, *selection_id_model_acc, &selection_id_mesh_acc);
    (*selection_id_model_acc)++;

    for (Model* child : children){
        child->material->set_uniform_camera(material->get_uniform_camera());
        child->render_selection_id(selection_id_model_acc);
    }
}

SelectionPixelObjectInfo Model::from_selection_id(SelectionPixelIdInfo selection_id, uint32_t* selection_id_model_acc){
    uint32_t selection_id_mesh_acc = 1;

    SelectionPixelObjectInfo info = csgmesh->from_selection_id(selection_id, this, *selection_id_model_acc, &selection_id_mesh_acc);
    if (!info.empty()){
        return info;
    }
    (*selection_id_model_acc)++;

    for (Model* child : children){
        info = child->from_selection_id(selection_id, selection_id_model_acc);
        if (!info.empty()){
            return info;
        }
    }
    return SelectionPixelObjectInfo(); // null
}
