#pragma once

#include <Imgui/imgui.h>

#include <glm/glm.hpp>

#include <list>
#include <functional>

using namespace glm;

class Parameter{
protected:
    static ImGuiInputTextCallback edit_callback;

    std::string label;
    bool is_edited = false;
    std::function<void()> render_action;

    Parameter(std::string label_);

public:
    Parameter(std::string label_, std::function<void()> render_);
    void render();
};

class FloatParameter : public Parameter{
    float temp;
    std::function<float()> get;
    std::function<void(float)> set;

public:
    FloatParameter(std::string label_, std::function<float()> get_, std::function<void(float)> set_);
};

class BoolParameter : public Parameter{
    std::function<bool()> get;
    std::function<void(bool)> set;

public:
    BoolParameter(std::string label_, std::function<bool()> get_, std::function<void(bool)> set_);
};

class Vec3Parameter : public Parameter{
    std::string label_x;
    std::string label_y;
    std::string label_z;

    vec3 temp;
    std::function<vec3()> get;
    std::function<void(vec3)> set;

public:
    Vec3Parameter(std::string label_, std::string label_x_, std::string label_y_, std::string label_z_, std::function<vec3()> get_, std::function<void(vec3)> set_);
};

class ColorParameter : public Parameter{
    float temp[4];
    std::function<vec4()> get;
    std::function<void(vec4)> set;

public:
    ColorParameter(std::string label_, std::function<vec4()> get_, std::function<void(vec4)> set_);
};

class Component{
    std::string label;

protected:
    std::list<Parameter*> parameters;

public:
    static bool show_remove_button;

    Component(std::string label_);
    ~Component();

    void render();
};

class ComponentContainer{
protected:
    std::list<Component*> components;

public:
    ~ComponentContainer();

    void add_component(Component* component);
    std::list<Component*> get_components();
    // TODO : add_component();
    // TODO : get_component(type);
};