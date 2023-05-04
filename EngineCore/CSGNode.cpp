#include "CSGNode.h"
#include "Utils.h"
#include "Material.h"

CSGNode::CSGNode(const Mesh& mesh) : result(mesh), transform(CSGNodeTransform(this)){
    type = Type::Operand;
}

CSGNode::CSGNode(Type type_, const Mesh& mesh1, const Mesh& mesh2) : transform(CSGNodeTransform(this)){
    assert(type != Type::Operand);
    type = type_;
    children.push_back(new CSGNode(mesh1));
    children.push_back(new CSGNode(mesh2));
}

CSGNode::~CSGNode(){
    for (CSGNode* child : children){
        delete child;
    }
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

CSGNode* CSGNode::main_child(){
    if (children.empty()){
        return nullptr;
    }
    return children.front();
}

Transform* CSGNode::get_transform(){
    if (type == Type::Operand){
        return (Transform*)&transform;
    }
    if (!children.empty()){
        return main_child()->get_transform();
    }
    return NULL;
}

void CSGNode::render(){
    result.render();
}

void CSGNode::render_selection_id(Material* material, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc){
    if (children.empty()){ // leaf node
        material->set_uniform_selection_id(SelectionPixelIdInfo(selection_id_model_acc, *selection_id_mesh_acc));
        result.render();
    }
    (*selection_id_mesh_acc)++;
    for (CSGNode* child : children){
        child->render_selection_id(material, selection_id_model_acc, selection_id_mesh_acc);
    }
    // TODO : 그룹 관련 추가
}

SelectionPixelObjectInfo CSGNode::from_selection_id(SelectionPixelIdInfo selection_id, Model* model, uint32_t selection_id_model_acc, uint32_t* selection_id_mesh_acc){
    SelectionPixelObjectInfo info;
    if (selection_id.model_id == selection_id_model_acc){ // TEST
        info = SelectionPixelObjectInfo(model, this);
        if(!info.empty()){
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

    for (CSGNode* child : children){
        SelectionPixelObjectInfo info = child->from_selection_id(selection_id, model, selection_id_model_acc, selection_id_mesh_acc);
        if (!info.empty()){
            return info;
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