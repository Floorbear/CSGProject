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
    csgmesh->render();
}

void Model::render_leaf_meshes(){
    // TODO : 그룹 관련 추가
}
