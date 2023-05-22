#include "Model.h"
#include "CSGNode.h"
#include "Material.h"
#include "Utils.h"
#include "Leaked_Pointers.h"

Model::Model(std::string name_) : name(name_){
    components.push_back(material_ptr = new ColorMaterial());
}

Model::~Model(){
    for (Model* child : children){
        delete child;
    }
    components.remove(get_transform()); // Transform만은 미리 삭제하면 안됨!
    Entity::~Entity();
    if (csgmesh != nullptr){
        delete csgmesh;
    }
}

CSGNode* Model::get_csg_mesh(){
    return csgmesh;
}

void Model::set_csg_mesh(CSGNode* csgmesh_){
    if (csgmesh != nullptr){
        components.remove(csgmesh->get_transform());
    }
    csgmesh = csgmesh_;
    if(csgmesh != nullptr){
        components.push_front(csgmesh->get_transform());
        csgmesh->model = this;
    }
}

void Model::set_csg_mesh_new(const Mesh& mesh){
    set_csg_mesh(new CSGNode(mesh));
}

Model* Model::find_model(std::string_view name_){
    if (name == name_){
        return this;
    }
    for (Model* child : children){
        Model* temp = child->find_model(name_);
        if (temp != nullptr){
            return temp;
        }
    }
    return nullptr;
}

TransformComponent* Model::get_transform(){
    if (csgmesh == nullptr){
        return nullptr;
    }
    return csgmesh->get_transform();
}

/*Transform Model::get_transform_copy()
{
    assert(csgmesh != nullptr);
    return csgmesh->get_transform_copy();
}*/

/*Transform Model::get_transform_scaleUp_copy(const vec3& _scaleAcc)
{
    assert(csgmesh != nullptr);
    return csgmesh->get_transform_scaleUp_copy(_scaleAcc);
}*/

Material* Model::get_material(){
    return material_ptr;
}

void Model::set_material(Material* material_){
    components.remove(material_ptr);
    Leaked_Pointers::add(material_ptr);
    components.push_back(material_);
    material_ptr = material_;
}

bool Model::is_renderable(){
    return csgmesh != nullptr;
}

void Model::render(){
    if (csgmesh != nullptr){
        material_ptr->set_uniform_model_transform(get_transform());
        material_ptr->apply();
        csgmesh->render();
    }

    for (Model* child : children){
        child->material_ptr->set_uniform_camera(material_ptr->get_uniform_camera());
        child->material_ptr->set_uniform_lights(material_ptr->get_uniform_lights());
        child->render();
    }
}

void Model::render_outline(const vec3& _scaleAcc)
{
    //조금더 큰 모델을 렌더링 합니다.
    if (csgmesh != nullptr) {
        Transform newTransform = get_transform()->get_value();
        vec3 newScale = newTransform.get_scale();
        newScale.x *= _scaleAcc.x;
        newScale.y *= _scaleAcc.y;
        newScale.z *= _scaleAcc.z;
        newTransform.set_scale(newScale);
        material_ptr->set_uniform_model_transform(&newTransform);
        material_ptr->apply_outline();

        csgmesh->render();
    }

    for (Model* child : children) {
        child->material_ptr->set_uniform_camera(material_ptr->get_uniform_camera());
        child->render_outline(_scaleAcc);
    }
}

void Model::render_selection_id(uint32_t* selection_id_model_acc){
    uint32_t selection_id_mesh_acc = 1;

    material_ptr->set_uniform_model_transform(get_transform());
    material_ptr->apply_selection_id();
    csgmesh->render_selection_id(material_ptr, *selection_id_model_acc, &selection_id_mesh_acc);
    (*selection_id_model_acc)++;

    for (Model* child : children){
        child->material_ptr->set_uniform_camera(material_ptr->get_uniform_camera());
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
