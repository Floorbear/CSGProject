#pragma once

#include <glm/glm.hpp>

#include <list>
#include <functional>

using namespace glm;

class Parameter{
protected:
    std::string label;
    std::function<void()> render_action;
    Parameter(std::string label_);

public:
    Parameter(std::string label_, std::function<void()> render_);
    void render();
};

class Vec3Parameter : public Parameter{
    bool is_edited = false;
    vec3 temp;

    std::string label_x;
    std::string label_y;
    std::string label_z;
    std::function<vec3()> get;
    std::function<void(vec3)> set;

public:
    Vec3Parameter(std::string label_, std::string label_x_, std::string label_y_, std::string label_z_, std::function<vec3()> get_, std::function<void(vec3)> set_);
};

class Component{
    std::string label;

protected:
    std::list<Parameter*> parameters;

public:
    Component(std::string label_);
    ~Component();

    void render();
    // TODO : add_component();
    // TODO : get_component();
};
