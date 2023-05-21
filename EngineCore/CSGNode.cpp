#include "CSGNode.h"
#include "Model.h"
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

CSGNode::CSGNode(Type type_, CSGNode* node1, CSGNode* node2) : Component("CSG Params"), transform(CSGNodeTransform(this)){
    type = type_;
    assert(type != Type::Operand);
    children.push_back(node1);
    children.push_back(node2);
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
    Entity::~Entity();
}

bool CSGNode::add_child(CSGNode* node){
    assert(type != Type::Operand);
    if (type == Type::Difference){
        assert(children.size() < 2);
    }
    if (TreeNode<CSGNode>::add_child(node)){
        CSGNode* parent_ = parent;
        while (parent_ != nullptr){
            parent_->is_result_valid = false;
            parent_ = parent_->parent;
        }
        return true;
    }
    return false;
}

bool CSGNode::reparent_child(CSGNode* node, CSGNode* after){
    if (is_leaf_node()){ // type == Operand
        CSGNode* union_node = new CSGNode(Type::Union, this, node);
        if (parent == nullptr){ // root
            model->set_csg_mesh(union_node);
        } else{
            parent->children.insert(std::find(parent->children.begin(), parent->children.end(), this), union_node);
            parent->children.remove(this);
        }
        if(node->parent == nullptr){
            node->model->set_csg_mesh(nullptr);
        }else{
            node->parent->children.remove(node);
        }
        parent = union_node;
        node->parent = union_node;
    } else{
        if (type == Type::Difference && children.size() >= 2){ // 더이상 자식 추가 불가
            return false;
        }
        return TreeNode<CSGNode>::reparent_child(node, after);
    }
    return true;
}

void CSGNode::swap_child(CSGNode* child1, CSGNode* child2){
    TreeNode<CSGNode>::swap_child(child1, child2);
    if (type == Type::Difference){
        is_result_valid = false;
    }
}

std::list<CSGNode::Type> CSGNode::get_changable_types(){
    if (type == Type::Operand){
        return {Type::Operand};
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

Transform CSGNode::get_transform_copy()
{
    return transform;
}

Transform CSGNode::get_transform_scaleUp_copy(const vec3& _scaleAcc)
{
    Transform newTransform = get_transform_copy();
    vec3 newScale = newTransform.get_scale();
    newScale.x *= _scaleAcc.x;
    newScale.y *= _scaleAcc.y;
    newScale.z *= _scaleAcc.z;
    newTransform.set_scale(newScale);
    return newTransform;
}

void CSGNode::render(){
    if (!is_result_valid){
        if (type == Type::Union){
            result = children.back()->result;//TODO : Mesh::compute_union(children.front()->result, children.back()->result);
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

    //SelectionPixelObjectInfo info;
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