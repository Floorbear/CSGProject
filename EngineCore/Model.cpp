#include "Model.h"
#include "CSGNode.h"
#include "Material.h"
#include "Utils.h"
#include "Leaked_Pointers.h"
#include "GUI.h"


Model::Model(std::string name_) : name(name_){
    components.push_back(transform = new TransformComponent());
    components.push_back(material = new ColorMaterial());
}

Model::~Model(){
    for (Model* child : children){
        delete child;
    }

    Entity::~Entity();

    if (csgmesh != nullptr){
        delete csgmesh;
    }
    if (gizmo != nullptr)
    {
        delete gizmo;
    }
}

void Model::set_parent(Model* parent_){
    TreeNode::set_parent(parent_);
    transform->set_parent(parent->get_transform(), true);
}

CSGNode* Model::get_csg_mesh(){
    return csgmesh;
}

void Model::set_csg_mesh(CSGNode* csgmesh_, bool calculate_local){
    csgmesh = csgmesh_;
    if (csgmesh != nullptr){
        csgmesh->model = this;
        csgmesh->get_transform()->set_parent(transform, calculate_local);
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
    return transform;
}

Material* Model::get_material(){
    return material;
}

void Model::set_material(Material* material_){
    components.remove(material);
    Leaked_Pointers::add(material);
    components.push_back(material_);
    material = material_;
}

bool Model::is_renderable(){
    return csgmesh != nullptr;
}

void Model::render(){
    transform->calculate_matrix();
    if (csgmesh != nullptr){
        csgmesh->get_transform()->calculate_matrix();
        material->set_uniform_model_transform(csgmesh->get_transform());
        material->apply();
        csgmesh->render();
    }

    for (Model* child : children){
        child->material->set_uniform_camera(material->get_uniform_camera());
        child->material->set_uniform_lights(material->get_uniform_lights());
        child->render();
    }
}

void Model::render_outline(const vec3& _scaleAcc){
    //조금더 큰 모델을 렌더링 합니다.
    if (csgmesh != nullptr){
        Transform newTransform = csgmesh->get_transform()->get_value();
        vec3 newScale = newTransform.get_scale();
        newScale.x *= _scaleAcc.x;
        newScale.y *= _scaleAcc.y;
        newScale.z *= _scaleAcc.z;
        newTransform.set_scale(newScale);
        material->set_uniform_model_transform(&newTransform);
        material->apply_outline();

        csgmesh->render();
    }

    for (Model* child : children){
        child->material->set_uniform_camera(material->get_uniform_camera());
        child->render_outline(_scaleAcc);
    }
}

void Model::render_gizmo()
{
    //지연 초기화
    if (gizmo == nullptr)
    {
        gizmo = new Gizmo(get_transform());
    }
    gizmo->render(material_ptr->get_uniform_camera());
}


void Model::render_selection_id(uint32_t* selection_id_model_acc){
    uint32_t selection_id_mesh_acc = 1;

    if (csgmesh != nullptr){
        material->set_uniform_model_transform(csgmesh->get_transform());
        material->apply_selection_id();
        csgmesh->render_selection_id(material, *selection_id_model_acc, &selection_id_mesh_acc);
    }
    (*selection_id_model_acc)++;

    for (Model* child : children){
        child->material->set_uniform_camera(material->get_uniform_camera());
        child->render_selection_id(selection_id_model_acc);
    }
}

SelectionPixelObjectInfo Model::from_selection_id(SelectionPixelIdInfo selection_id, uint32_t* selection_id_model_acc){
    uint32_t selection_id_mesh_acc = 1;
    SelectionPixelObjectInfo info;

    if (csgmesh != nullptr){
        info = csgmesh->from_selection_id(selection_id, this, *selection_id_model_acc, &selection_id_mesh_acc);
        if (!info.empty()){
            return info;
        }
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
