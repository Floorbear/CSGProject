#pragma once
#include "Component.h"

#include <glm/glm.hpp>

using namespace glm;

class Transform{
protected:
    Transform* parent = nullptr;

    vec3 position = vec3(0.f, 0.f, 0.f);
    vec3 rotation = vec3(0.f, 0.f, 0.f);
    vec3 scale3d = vec3(1.f, 1.f, 1.f); // w : Radian;

    mat4 matrix_local = glm::mat4(1.0f);
    mat4 matrix_world = glm::mat4(1.0f);
    vec3 position_world = vec3(0.f, 0.f, 0.f);
    vec3 rotation_world = vec3(0.f, 0.f, 0.f);
    vec3 scale3d_world = vec3(1.f, 1.f, 1.f);

    bool is_modified_local = true;

    virtual void on_local_modify();
public:
    static Transform identity; // const

    Transform();
    ~Transform();

    vec3 get_position();
    vec3 get_rotation();
    vec3 get_scale();

    vec3 get_world_position();
    vec3 get_world_rotation();

    Transform* get_parent();
    void set_parent(Transform* parent_, bool fix_position);

    void set(const Transform& value);
    void set_position(const vec3& value);
    void set_rotation(const vec3& value);
    void set_scale(const vec3& value);

    void translate(const vec3& value);
    void rotate(const vec3& value);
    void scale(const vec3& value);
    void add_position(const vec3& value);

    void calculate_matrix();
    mat4 get_local_matrix();
    mat4 get_world_matrix();
    Transform get_world_transform();

    vec3 get_forward_dir();
    vec3 get_right_dir();
    vec3 get_up_dir();
};

class TransformComponent : public Transform, public Component{
public:
    TransformComponent();
    TransformComponent(Transform transform);

    Transform get_value();
};

class TransformEntity{
public:
    virtual TransformComponent* get_transform() = 0;
};
