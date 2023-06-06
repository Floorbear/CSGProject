#pragma once
#include "Task.h"

#include <Imgui/imgui.h>

#include <glm/glm.hpp>

#include <list>
#include <vector>
#include <functional>

using namespace glm;

class ParameterSnapshot;
class Parameter{
    static int id_counter;
protected:
    static ImGuiInputTextCallback edit_callback;
    int id = (id_counter++);

    std::string label;
    bool is_edited = false;
    std::function<void()> render_action;

public:
    Parameter(std::string label_, std::function<void()> render_);

    void render();

    virtual ParameterSnapshot* make_snapshot_new();
    virtual void recover_from(ParameterSnapshot* snapshot);
};

class ParameterSnapshot{
    Parameter* ptr;
    void* value;

public:
    ParameterSnapshot(Parameter* ptr_);
    ~ParameterSnapshot();

    template<typename T>
    void set_value(T value_);
    template<typename T>
    T get_value();

    void set_value_(void* value_); // TODO : EnumParameter에서 링킹에러가 뜨는데 해결방법 찾기!
    void* get_value_();

    void recover();
};

class FloatParameter : public Parameter{
    float temp;
    std::function<float()> get;
    std::function<void(float)> set;

public:
    FloatParameter(std::string label_, std::function<float()> get_, std::function<void(float)> set_);

    ParameterSnapshot* make_snapshot_new() override;
    void recover_from(ParameterSnapshot* snapshot) override;
};

class BoolParameter : public Parameter{
    std::function<bool()> get;
    std::function<void(bool)> set;

public:
    BoolParameter(std::string label_, std::function<bool()> get_, std::function<void(bool)> set_);

    ParameterSnapshot* make_snapshot_new() override;
    void recover_from(ParameterSnapshot* snapshot) override;
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

    ParameterSnapshot* make_snapshot_new() override;
    void recover_from(ParameterSnapshot* snapshot) override;
};

class ColorParameter : public Parameter{
    float temp[4];
    std::function<vec4()> get;
    std::function<void(vec4)> set;

public:
    ColorParameter(std::string label_, std::function<vec4()> get_, std::function<void(vec4)> set_);

    ParameterSnapshot* make_snapshot_new() override;
    void recover_from(ParameterSnapshot* snapshot) override;
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
    void set_label(std::string label_);
    std::list<Parameter*> get_parameters();
};

class Entity{
protected:
    std::list<Component*> components;

public:
    ~Entity();

    void add_component(Component* component);
    std::list<Component*> get_components();
    void clear_components();
    // TODO : add_component();
    // TODO : get_component(type);
};

class EntitySnapshot{
public:
    // TODO : entity의 component 보유 상태로 수정 - ComponentSnapshot (add/remove component가 일어나거나 컴포넌트 안에서 연계 작동하는 파라미터가 있을시 필요)
    std::list<ParameterSnapshot*> snapshots; // 모든 component의 parameter을 캡쳐해서 저장

    EntitySnapshot(Entity* ptr);
    ~EntitySnapshot();
    void recover();
};

class ParameterModifyTask : public TransactionTask{
    std::list<ParameterSnapshot*> snapshots; // 여러개의 entity 동시 수정
public:
    ParameterModifyTask(std::string detail_, std::function<bool()> work_, std::function<void()> work_undo_);
};

// TODO : undo redo에서 focus가 inputtext인지 확인