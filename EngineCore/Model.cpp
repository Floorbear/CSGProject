#include "Model.h"
#include "CSGNode.h"
#include "Material.h"
#include "Utils.h"
#include "Leaked_Pointers.h"
#include "GUI.h"
#include "Texture.h"
#include "Renderer.h"
#include "WorkSpace.h"

Model::Model(std::string name_) : name(name_){
    components.push_back(transform = new TransformComponent());
    components.push_back(material = new ColorMaterial());
    // components.push_back(material = new TextureMaterial(Texture::create_texture(EnginePath::get_texture_path().move("rockTexture.jpg"))));
}

Model::~Model(){
    for (Model* child : children){
        delete child;
    }

    Entity::~Entity();

    if (csgmesh != nullptr){
        delete csgmesh;
    }
}

void Model::set_parent(Model* parent_){
    TreeNode::set_parent(parent_);
    transform->set_parent(parent->get_transform(), true);
}

CSGNode* Model::get_csg_mesh(){
    return csgmesh;
}

void Model::set_csg_mesh(CSGNode* csgmesh_, bool fix_mesh_position){
    csgmesh = csgmesh_;
    if (csgmesh != nullptr){
        csgmesh->model = this;
        csgmesh->get_transform()->set_parent(transform, false);
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

void Model::render(Renderer* renderer){
    if (renderer->workspace == nullptr || renderer->workspace->check_model_selected(this)){ // TODO : 이 로직이 여기 없었으면 좋겠긴한데... 파라미터로 스텐실 테스트 함수로 넘겨받는방법?
        glStencilMask(0xFF); // on
    } else{
        glStencilMask(0x00); // off
    }

    transform->calculate_matrix(); // 부모가 먼저 계산하고 자식으로 전파해야하므로 제거하지 마세요!
    if (csgmesh != nullptr){
        csgmesh->get_transform()->calculate_matrix();
        material->set_uniform_model_transform(csgmesh->get_transform());
        material->apply();
        csgmesh->render();
    }

    for (Model* child : children){
        child->material->set_uniform_camera(material->get_uniform_camera());
        child->material->set_uniform_lights(material->get_uniform_lights());
        child->render(renderer);
    }
}

void Model::render_monotone(Material* material_monotone){
    if (csgmesh != nullptr){
        material_monotone->set_uniform_model_transform(csgmesh->get_transform());
        material_monotone->apply();
        csgmesh->render();
    }

    for (Model* child : children){
        child->render_monotone(material_monotone);
    }
}


void Model::render_selection_id(uint32_t* selection_id_model_acc){
    uint32_t selection_id_mesh_acc = 1;

    if (csgmesh != nullptr){
        material->set_uniform_model_transform(csgmesh->get_transform());
        // csgmesh에서 material.apply()
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
