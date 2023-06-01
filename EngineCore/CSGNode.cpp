#include "CSGNode.h"
#include "Model.h"
#include "Utils.h"
#include "Material.h"
#include "EnumParameter.hpp"

CSGNode::CSGNodeTransform::CSGNodeTransform(CSGNode* csg_node_) : csg_node(csg_node_){
}

void CSGNode::CSGNodeTransform::on_local_modify(){
    if (csg_node->parent != nullptr){
        csg_node->parent->mark_edited();
    }
    TransformComponent::on_local_modify();
}

void CSGNode::mark_edited(){
    CSGNode* csg_node = this;
    while (csg_node != nullptr){
        csg_node->is_result_valid = false;
        csg_node = csg_node->parent;
    }
}

const char* CSGNode::type_string_values[] = {"None", "Union", "Intersection", "Difference"};

CSGNode::CSGNode(const Mesh& mesh) : Component("CSG Params"), result(mesh), transform(CSGNodeTransform(this)){
    type = Type::Operand;
    is_result_valid = true;

    components.push_back(&transform);
    components.push_back(this);

    parameters.push_back(new EnumParameter<Type>("type", [this](){
        return type;
    }, [this](Type value){
        set_type(value);
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
        set_type(value);
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

void CSGNode::set_parent(CSGNode* parent_){
    TreeNode::set_parent(parent_);
    if (parent != nullptr){
        transform.set_parent(parent->get_transform(), true);
    }
}

bool CSGNode::add_child(CSGNode* node){
    assert(type != Type::Operand);
    if (type == Type::Difference){
        assert(children.size() < 2);
    }
    if (TreeNode<CSGNode>::add_child(node)){
        CSGNode* parent_ = parent;
        while (parent_ != nullptr){
            parent_->mark_edited();
            parent_ = parent_->parent;
        }
        return true;
    }
    return false;
}

bool CSGNode::reparent_child(CSGNode* node, CSGNode* after){
    if (is_leaf_node()){ // type == Operand
        if (parent != nullptr && node->type == Type::Union){
            parent->reparent_child(node, this);
        } else{
            CSGNode* union_node = new CSGNode(Type::Union, this, node);
            if (parent == nullptr){ // root
                model->set_csg_mesh(union_node);
            } else{
                parent->children.insert(std::find(parent->children.begin(), parent->children.end(), this), union_node);
                parent->children.remove(this);
                union_node->set_parent(parent);
            }
            if (node->parent == nullptr){
                node->model->set_csg_mesh(nullptr);
            } else{
                node->parent->children.remove(node);
            }
            set_parent(union_node);
            node->set_parent(union_node);
        }
    } else{
        if (type == Type::Difference && children.size() >= 2){ // 더이상 자식 추가 불가
            return false;
        }
        if (type == node->type && (type == Type::Union || type == Type::Intersection)){
            auto node_children_copy = node->children;
            for (CSGNode* child : node_children_copy){
                reparent_child(child, after);
            }
            if (node->parent == nullptr){
                node->model->set_csg_mesh(nullptr);
            } else{
                node->parent->children.remove(node);
            }
        } else{
            node->transform.set_parent(&transform, true);
            return TreeNode<CSGNode>::reparent_child(node, after);
        }
    }
    return true;
}

void CSGNode::swap_child(CSGNode* child1, CSGNode* child2){
    TreeNode<CSGNode>::swap_child(child1, child2);
    if (type == Type::Difference){
        is_result_valid = false;
    }
}

std::string CSGNode::get_name(){
    return result.get_name();
}

std::vector<CSGNode::Type> CSGNode::get_changable_types(){
    if (type == Type::Operand){
        return {Type::Operand};
    }
    if (type == Type::Difference || children.size() <= 2){
        return {Type::Union, Type::Intersection, Type::Difference};
    }
    return {Type::Union, Type::Intersection};
}

CSGNode::Type CSGNode::get_type(){
    return type;
}

void CSGNode::set_type(Type type_){
    assert(Utils::contains(get_changable_types(), type_));
    if (type != type_){
        mark_edited();
    }
    type = type_;
}

TransformComponent* CSGNode::get_transform(){
    return &transform;
}

/*Transform CSGNode::get_transform_scaleUp_copy(const vec3& _scaleAcc)
{
    Transform newTransform = get_transform_copy();
    vec3 newScale = newTransform.get_scale();
    newScale.x *= _scaleAcc.x;
    newScale.y *= _scaleAcc.y;
    newScale.z *= _scaleAcc.z;
    newTransform.set_scale(newScale);
    return newTransform;
}*/

void CSGNode::calculate_mesh(){
    if (!is_result_valid){
        for (CSGNode* child : children){
            child->calculate_mesh();
        }
        assert(children.size() != 0);
        if (children.size() == 1){
            result = children.front()->result;
        } else{
            bool succeed;
            if (type == Type::Union){
                std::list<CSGNode*>::iterator child_it = children.begin();
                std::list<CSGNode*>::iterator child_next_it = std::next(child_it, 1);
                succeed = Mesh::compute_union((*child_it)->result, (*child_it)->get_transform(),
                                              (*child_next_it)->result, (*child_next_it)->get_transform(), result);
                child_it++;
                for (; child_next_it != children.end(); child_it++, child_next_it++){
                    succeed |= Mesh::compute_union(result, &Transform::identity,
                                                  (*child_next_it)->result, (*child_next_it)->get_transform(), result);
                }
            } else if (type == Type::Intersection){
                std::list<CSGNode*>::iterator child_it = children.begin();
                std::list<CSGNode*>::iterator child_next_it = std::next(child_it, 1);
                succeed = Mesh::compute_intersection((*child_it)->result, (*child_it)->get_transform(),
                                              (*child_next_it)->result, (*child_next_it)->get_transform(), result);
                child_it++;
                for (; child_next_it != children.end(); child_it++, child_next_it++){
                    succeed |= Mesh::compute_intersection(result, &Transform::identity,
                                                  (*child_next_it)->result, (*child_next_it)->get_transform(), result);
                }
            } else if (type == Type::Difference){
                succeed = Mesh::compute_difference(children.front()->result, children.front()->get_transform(),
                                                   children.back()->result, children.back()->get_transform(), result);
            }
        }
        is_result_valid = true;
    }
}

void CSGNode::render(){
    calculate_mesh();
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
    /*SelectionPixelObjectInfo info;
    if (selection_id.model_id == selection_id_model_acc){ // TEST
        info = SelectionPixelObjectInfo(model, this);
        if (!info.empty()){
            return info;
        }
    }
    return SelectionPixelObjectInfo();*/

    SelectionPixelObjectInfo info;
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
