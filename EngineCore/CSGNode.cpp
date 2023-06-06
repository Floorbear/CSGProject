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

const char* CSGNode::type_string_values[] = {"Operand", "Union", "Intersection", "Difference"};

CSGNode::CSGNode(Type type_) : Component("CSG Params"), transform(CSGNodeTransform(this)){
    type = type_;
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

CSGNode::CSGNode(const Mesh& mesh) : CSGNode(Type::Operand){
    is_result_valid = true;
    result = mesh;
}

CSGNode::~CSGNode(){
    for (CSGNode* child : children){
        delete child;
    }
    Entity::~Entity();
}

bool CSGNode::add_child(CSGNode* node, CSGNode* after){
    assert(type != Type::Operand);
    if (type == Type::Difference){
        assert(children.size() < 2);
    }
    bool succeed = TreeNode<CSGNode>::add_child(node, after);
    if (succeed){
        node->transform.set_parent(get_transform(), true);
        node->model = model;
        mark_edited();
    }
    return succeed;
}

bool CSGNode::reparent_child(CSGNode* node, CSGNode* after){
    if (is_descendant_of(node)){
        return false;
    }
    if (type == Type::Difference && children.size() >= 2){
        return false;
    }
    CSGNode* this_parent = parent;
    CSGNode* node_parent = node->parent;
    Model* node_model = node->model;

    if (type == Type::Operand){ // Case 1 : Operand <- Any

        // 새 union 노드 생성
        CSGNode* union_node = new CSGNode(Type::Union);
        union_node->model = model;

        // parent에서 union 노드 추가, this 제거
        if (this_parent == nullptr){ // root
            model->set_csg_mesh(union_node);
        } else{
            union_node->get_transform()->set(*get_transform()->get_parent());
            this_parent->add_child(union_node, this);
            this_parent->children.remove(this);
        }

        union_node->add_child(this, nullptr);
        union_node->add_child(node, this);

    } else{ // Case 2 : Op <- Any
        add_child(node, after);
    }

    // node->parent에서 node 제거
    if (node_parent == nullptr){
        node_model->set_csg_mesh(nullptr);
    } else{
        node_parent->children.remove(node);
        if (node_parent->is_leaf_node()){ // not operand
            node_parent->remove_self();
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

bool CSGNode::remove_self(){
    bool succeed = TreeNode::remove_self();
    if (succeed && parent->type != Type::Operand && parent->is_leaf_node()){
        parent->remove_self();
    }
    return succeed;
}

bool CSGNode::remove_self_subtree(){
    if (is_root_node()){
        model->set_csg_mesh(nullptr);
        return true;
    } else{
        bool succeed = TreeNode::remove_self_subtree();
        if (succeed && parent->type != Type::Operand && parent->is_leaf_node()){
            parent->remove_self();
        }
        return succeed;
    }
}

bool CSGNode::unpack_to_parent(){
    return remove_self();
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

Mesh* CSGNode::get_mesh(){
    return &result;
}

TransformComponent* CSGNode::get_transform(){
    return &transform;
}

void CSGNode::calculate_mesh(){
    if (!is_result_valid){
        if (type == Type::Operand){
            printf("warning : CSGNode::calculate_mesh() : type was operand!\n");
            is_result_valid = true;
            return;
        }
        assert(children.size() != 0);
        for (CSGNode* child : children){
            child->calculate_mesh();
        }
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
        transform.calculate_matrix();
        material->set_uniform_model_transform(&transform);
        material->set_uniform_selection_id(SelectionPixelIdInfo(selection_id_model_acc, *selection_id_mesh_acc));
        material->apply_selection_id();
        result.render();
        (*selection_id_mesh_acc)++;

    } else{
        for (CSGNode* child : children){
            child->render_selection_id(material, selection_id_model_acc, selection_id_mesh_acc);
        }
    }
}

SelectionPixelObjectInfo CSGNode::from_selection_id(SelectionPixelIdInfo selection_id, Model* model_, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc){
    if (selection_group || children.empty()){ // leaf node
        SelectionPixelObjectInfo info;
        if (selection_id.model_id == selection_id_model_acc && selection_id.mesh_id == *selection_id_mesh_acc){
            return SelectionPixelObjectInfo(model_, this);
        }
        (*selection_id_mesh_acc)++;

    } else{
        for (CSGNode* child : children){
            SelectionPixelObjectInfo info = child->from_selection_id(selection_id, model_, selection_id_model_acc, selection_id_mesh_acc);
            if (!info.empty()){
                return info;
            }
        }
    }
    return SelectionPixelObjectInfo(); // null
}
