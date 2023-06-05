#include "Transform.h"
#include "Utils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

Transform Transform::identity = Transform();

void Transform::on_local_modify(){
    is_modified_local = true;
    calculate_matrix();
}

Transform::Transform(){
}

Transform::~Transform(){
}

vec3 Transform::get_position(){
    return position;
}

vec3 Transform::get_rotation(){
    return rotation;
}

vec3 Transform::get_scale(){
    return scale3d;
}

vec3 Transform::get_world_position(){
    return position_world;
}

vec3 Transform::get_world_rotation(){
    return rotation_world;
}

void Transform::set_parent(Transform* parent_, bool fix_position){
    vec3 old_parent_scale = vec3(1, 1, 1);
    if (parent != nullptr){
        old_parent_scale = parent->scale3d_world;
    }
    if (fix_position){
        scale3d = scale3d_world / parent_->scale3d_world;
        rotation = rotation_world - parent_->rotation_world;
        position = (position_world - parent_->position_world) * old_parent_scale / parent_->scale3d_world;
    }
    parent = parent_;
    is_modified_local = true;
    calculate_matrix();
}

void Transform::set(const Transform& value){
    position = value.position;
    rotation = value.rotation;
    scale3d = value.scale3d;
    on_local_modify();
}

void Transform::set_position(const vec3& value){
    position = value;
    on_local_modify();
}

void Transform::set_rotation(const vec3& value){
    rotation = value;
    on_local_modify();
}

void Transform::set_scale(const vec3& value){
    scale3d = value;
    on_local_modify();
}

void Transform::translate(const vec3& value){ // rotation 적용된 상태
    add_position(get_right_dir() * value.x);
    add_position(get_up_dir() * value.y);
    add_position(get_forward_dir() * value.z);
    on_local_modify();
}

void Transform::rotate(const vec3& value){
    rotation = rotation + value;
    on_local_modify();
}

void Transform::scale(const vec3& value){
    scale3d = scale3d + value;
    scale3d = max(scale3d, vec3(epsilon<float>(), epsilon<float>(), epsilon<float>()));
    on_local_modify();
}

void Transform::add_position(const vec3& value){
    position = position + value;
    on_local_modify();
}

void Transform::calculate_matrix(){
    matrix_world = get_local_matrix();
    position_world = position;
    rotation_world = rotation;
    scale3d_world = scale3d;
    if (parent != nullptr){
        matrix_world = parent->matrix_world * matrix_world;
        position_world += parent->position_world;
        rotation_world += parent->rotation_world;
        scale3d_world *= parent->scale3d_world;
    }
}

mat4 Transform::get_local_matrix(){
    if (is_modified_local){
        matrix_local = mat4(1.0f);
        matrix_local = glm::translate(matrix_local, position);
        matrix_local = glm::rotate(matrix_local, glm::radians(rotation.x), glm::vec3(1.f, 0, 0.f));
        matrix_local = glm::rotate(matrix_local, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
        matrix_local = glm::rotate(matrix_local, glm::radians(rotation.z), glm::vec3(0.f, 0, 1.f));
        matrix_local = glm::scale(matrix_local, scale3d);
        is_modified_local = false;
    }
    return matrix_local;
}

mat4 Transform::get_world_matrix(){
    return matrix_world;
}

// https://stackoverflow.com/questions/31733811/local-variables-before-return-statements-does-it-matter
vec3 Transform::get_forward_dir(){
    return glm::normalize(Utils::get_vecFromPitchYaw(rotation.x, rotation.y));
}

vec3 Transform::get_right_dir(){
    glm::vec3 forward_dir = Utils::get_vecFromPitchYaw(rotation.x, rotation.y);
    glm::vec3 up_dir = glm::vec3(0, 1, 0);
 
    return glm::normalize(cross(forward_dir, up_dir));
}

vec3 Transform::get_up_dir(){
    return normalize(cross(get_right_dir(), get_forward_dir()));
}

TransformComponent::TransformComponent() : TransformComponent(Transform()){
}

TransformComponent::TransformComponent(Transform transform) : Component("Transform"), Transform(){
    parameters.push_back(new Vec3Parameter("position", "x", "y", "z", [this](){
        return get_position();
    }, [this](vec3 value){
        set_position(value);
    }));
    parameters.push_back(new Vec3Parameter("rotation", "x", "y", "z", [this](){
        return get_rotation();
    }, [this](vec3 value){
        set_rotation(value);
    }));
    parameters.push_back(new Vec3Parameter("scale", "x", "y", "z", [this](){
        return get_scale();
    }, [this](vec3 value){
        set_scale(value);
    }));
}

Transform TransformComponent::get_value(){
    Transform ret = *this;
    return ret;
}