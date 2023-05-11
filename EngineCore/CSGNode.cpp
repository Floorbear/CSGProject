#include "CSGNode.h"
#include "Utils.h"
#include "Material.h"
#include "EnumParameter.hpp"

const char* CSGNode::type_string_values[] = {"None", "Union", "Intersection", "Difference"};

CSGNode::CSGNode(const Mesh& mesh) : Component("CSG Params"), result(mesh), transform(CSGNodeTransform(this)){
    type = Type::Operand;
    is_result_valid = true;

    components.push_back(&transform);
    components.push_back(this);

    parameters.push_back(new EnumParameter<Type>("type", [this](){
        return type;
    }, [this](Type value){
        type = value;
    }, [this](){
        return get_changable_types();
    }, type_string_values));

    parameters.push_back(new BoolParameter("selection group", [this](){
        return selection_group;
    }, [this](bool value){
        selection_group = value;
    }));
}

CSGNode::CSGNode(Type type_, const Mesh& mesh1, const Mesh& mesh2) : Component("CSG Params"), transform(CSGNodeTransform(this)){
    assert(type != Type::Operand);
    type = type_;
    children.push_back(new CSGNode(mesh1));
    children.push_back(new CSGNode(mesh2));
    is_result_valid = false;

    components.push_back(&transform);
    components.push_back(this);

    parameters.push_back(new EnumParameter<Type>("type", [this](){
        return type;
    }, [this](Type value){
        type = value;
    }, [this](){
        return get_changable_types();
    }, type_string_values));

    parameters.push_back(new BoolParameter("selection group", [this](){
        return selection_group;
    }, [this](bool value){
        selection_group = value;
    }));
}

CSGNode::~CSGNode(){
    for (CSGNode* child : children){
        delete child;
    }
    ComponentContainer::~ComponentContainer();
}

CSGNode* CSGNode::get_parent(){
    return parent;
}

std::list<CSGNode*> CSGNode::get_children(){
    return children;
}

bool CSGNode::is_leaf_node(){
    return children.empty(); // <=> type == Operand
}

void CSGNode::add_child(CSGNode* node){
    assert(type != Type::Operand);
    if (type == Type::Difference){
        assert(children.size() < 2);
    }
    children.push_back(node);
    CSGNode* parent_ = parent;
    while (parent_ != nullptr){
        parent_->is_result_valid = false;
        parent_ = parent_->parent;
    }
}

std::list<CSGNode::Type> CSGNode::get_changable_types(){
    if (type == Type::Operand || children.size() <= 2){
        return {Type::Operand, Type::Union, Type::Intersection, Type::Difference};
    }
    if (type == Type::Difference){
        return {Type::Union, Type::Intersection, Type::Difference};
    }
    return {Type::Union, Type::Intersection};
}

CSGNode::Type CSGNode::get_type(){
    return type;
}

void CSGNode::set_type(Type type_){
    assert(Utils::contains(get_changable_types(), type_));
    type = type_;
}

CSGNode* CSGNode::main_child(){
    if (children.empty()){
        return nullptr;
    }
    return children.front();
}

Transform* CSGNode::get_transform(){
    if (!children.empty()){
        return main_child()->get_transform();
    }
    return (Transform*)&transform;
}

void CSGNode::render(){
    if (!is_result_valid){
        if (type == Type::Union){
            result = Mesh::compute_union(children.front()->result, children.back()->result);
        } else if (type == Type::Intersection){
            result = Mesh::compute_intersection(children.front()->result, children.back()->result);
        } else if (type == Type::Difference){
            result = Mesh::compute_difference(children.front()->result, children.back()->result);
        }
        is_result_valid = true;
    }
    result.render();
}

void CSGNode::render_selection_id(Material* material, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc){
    if (selection_group || children.empty()){ // leaf node
        material->set_uniform_selection_id(SelectionPixelIdInfo(selection_id_model_acc, *selection_id_mesh_acc));
        result.render();
    }
    (*selection_id_mesh_acc)++;
    if (!selection_group){
        for (CSGNode* child : children){
            child->render_selection_id(material, selection_id_model_acc, selection_id_mesh_acc);
        }
    }
}

SelectionPixelObjectInfo CSGNode::from_selection_id(SelectionPixelIdInfo selection_id, Model* model, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc){
    SelectionPixelObjectInfo info;
    if (selection_id.model_id == selection_id_model_acc){ // TEST
        info = SelectionPixelObjectInfo(model, this);
        if (!info.empty()){
            return info;
        }
    }
    return SelectionPixelObjectInfo(); // TODO : info가 다 채워지고나면 이 위를 지우고 밑을 사용.

    if (selection_id.model_id == selection_id_model_acc && selection_id.mesh_id == *selection_id_mesh_acc){
        info = SelectionPixelObjectInfo(model, this);
    }
    (*selection_id_mesh_acc)++;
    if (!info.empty()){
        return info;
    }

    if (!selection_group){
        for (CSGNode* child : children){
            SelectionPixelObjectInfo info = child->from_selection_id(selection_id, model, selection_id_model_acc, selection_id_mesh_acc);
            if (!info.empty()){
                return info;
            }
        }
    }
    return SelectionPixelObjectInfo(); // null
}

CSGNode::CSGNodeTransform::CSGNodeTransform(CSGNode* parent_) : Transform(), parent(parent_){
}
// TODO : gui 조작을 위한 특정시점 값 저장 후 차이값 적용 기능 추가

void CSGNode::CSGNodeTransform::set_position(const vec3& value){
    vec3 delta = value - get_position();
    Transform::set_position(value);
    for (CSGNode* node : parent->children){
        node->transform.add_position(delta);
    }
}

void CSGNode::CSGNodeTransform::set_rotation(const vec3& value){
    vec3 delta = value - get_rotation();
    Transform::set_rotation(value);
    for (CSGNode* node : parent->children){
        node->transform.rotate(delta);
    }
}

void CSGNode::CSGNodeTransform::set_scale(const vec3& value){
    vec3 ratio = value / get_scale();
    Transform::set_scale(value);
    for (CSGNode* node : parent->children){
        node->transform.scale(ratio);
    }
}

void CSGNode::CSGNodeTransform::translate(const vec3& value){

}

void CSGNode::CSGNodeTransform::rotate(const vec3& value){
}

void CSGNode::CSGNodeTransform::scale(const vec3& value){
}

void CSGNode::CSGNodeTransform::add_position(const vec3& value){
    parent->transform.add_position(value);
    for (CSGNode* node : parent->children){
        node->transform.add_position(value);
    }
}