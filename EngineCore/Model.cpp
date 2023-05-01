#include "Model.h"
#include "CSGNode.h"
#include "Material.h"
// ===== Model ===== //

Model::Model(std::string name_) : name(name_) {
}

Model::~Model() {
    if (csgmesh != nullptr) {
        delete csgmesh;
    }
    delete material;
}

void Model::set_new(const Mesh& mesh) {
    csgmesh = new CSGNode(mesh);
    // TODO : 기존 transform 제거
    components.push_back(csgmesh->get_transform());
    components.push_back(material = new Material());
}

Model* Model::get_parent() {
    return parent;
}

std::list<Model*> Model::get_children() {
    return children;
}

CSGNode* Model::get_csg_mesh() {
    return csgmesh;
}

std::list<Component*> Model::get_components() {
    return components;
}

Transform* Model::get_transform() {
    if (csgmesh == nullptr) {
        return nullptr;
    }
    return csgmesh->get_transform();
}

Material* Model::get_material() {
    return material;
}

bool Model::is_renderable() {
    return csgmesh != nullptr;
}

void Model::render(Renderer* renderer) {
    csgmesh->render();
}

