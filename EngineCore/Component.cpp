#include "Component.h"
#include "GUI.h"
#include "Utils.h"

// ===== Parameter ===== //

int Parameter::id_counter = 0;

ImGuiInputTextCallback Parameter::edit_callback = [](ImGuiInputTextCallbackData* data){
    Parameter* parent = (Parameter*)data->UserData;
    parent->is_edited = true;
    return 0;
};

/*ImGuiInputTextCallback completion_callback = [](ImGuiInputTextCallbackData* data){// TODO : transaction 등록
    Parameter* parent = (Parameter*)data->UserData;
    parent->is_edited = true;
    return 0;
};*/

Parameter::Parameter(std::string label_, std::function<void()> render_) : label(label_), render_action(render_){
}

void Parameter::render(){
    render_action();
}

ParameterSnapshot* Parameter::make_snapshot_new(){
    ParameterSnapshot* ret = new ParameterSnapshot(this);
    // ret->set_value(get());
    return ret;
}

void Parameter::recover_from(ParameterSnapshot* snapshot){
    // set(snapshot->get_value());
}


// ===== ParameterSnapshot ===== //

ParameterSnapshot::ParameterSnapshot(Parameter* ptr_) : ptr(ptr_){
}

ParameterSnapshot::~ParameterSnapshot(){
    delete value;
}

template<typename T>
void ParameterSnapshot::set_value(T value_){
    value = new T(value_);
}

template<typename T>
T ParameterSnapshot::get_value(){
    return *((T*)value);
}

void ParameterSnapshot::set_value_(void* value_){
    value = value_;
}
void* ParameterSnapshot::get_value_(){
    return value;
}

void ParameterSnapshot::recover(){
    ptr->recover_from(this);
}


// ===== FloatParameter ===== //

// TODO : completion 액션 추가
FloatParameter::FloatParameter(std::string label_, std::function<float()> get_, std::function<void(float)> set_) : Parameter(label_, [this](){
    temp = get();
    ImGui::Text(label.c_str());
    ImGui::DragFloat(Utils::format("##InputFloat%1%", id).c_str(), &temp, 0.01f, 0, 0, "%.3f", 0, edit_callback, (void*)this);
    if (is_edited){ // TODO : 리팩토링
        if (set != nullptr){
            set(temp);
        }
        is_edited = false;
    }
}), get(get_), set(set_){
}

ParameterSnapshot* FloatParameter::make_snapshot_new(){
    ParameterSnapshot* ret = new ParameterSnapshot(this);
    ret->set_value<float>(get());
    return ret;
}

void FloatParameter::recover_from(ParameterSnapshot* snapshot){
    set(snapshot->get_value<float>());
}


// ===== BoolParameter ===== //

BoolParameter::BoolParameter(std::string label_, std::function<bool()> get_, std::function<void(bool)> set_) : Parameter(label_, [this](){
    bool temp = get();
    if (ImGui::Checkbox((label + Utils::format("##CheckBox%1%", id)).c_str(), &temp)){
        if (is_edited){ // TODO : 리팩토링
            if (set != nullptr){
                set(temp);
            }
            is_edited = false;
        }
    }
}), get(get_), set(set_){
}

ParameterSnapshot* BoolParameter::make_snapshot_new(){
    ParameterSnapshot* ret = new ParameterSnapshot(this);
    ret->set_value<bool>(get());
    return ret;
}

void BoolParameter::recover_from(ParameterSnapshot* snapshot){
    set(snapshot->get_value<bool>());
}


// ===== Vec3Parameter ===== //

Vec3Parameter::Vec3Parameter(std::string label_, std::string label_x_, std::string label_y_, std::string label_z_, std::function<vec3()> get_, std::function<void(vec3)> set_) : Parameter(label_, [this](){
    temp = get();
    ImGui::Text(label.c_str());
    if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_SizingStretchSame)){
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("x");
        ImGui::SameLine(0, 3);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragFloat(Utils::format("##InputFloat%1%", id).c_str(), &temp.x, 0.01f, 0, 0, "%.3f", 0, edit_callback, (void*)this);
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("y");
        ImGui::SameLine(0, 3);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragFloat(Utils::format("##InputFloat%1%", id + 1).c_str(), &temp.y, 0.01f, 0, 0, "%.3f", 0, edit_callback, (void*)this);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("z");
        ImGui::SameLine(0, 3);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragFloat(Utils::format("##InputFloat%1%", id + 2).c_str(), &temp.z, 0.01f, 0, 0, "%.3f", 0, edit_callback, (void*)this);
        ImGui::EndTable();
    }
    if (is_edited){ // TODO : 리팩토링
        if (set != nullptr){
            set(temp);
        }
        is_edited = false;
    }
}), label_x(label_x_), label_y(label_y_), label_z(label_z_), get(get_), set(set_){
}

ParameterSnapshot* Vec3Parameter::make_snapshot_new(){
    ParameterSnapshot* ret = new ParameterSnapshot(this);
    ret->set_value<vec3>(get());
    return ret;
}

void Vec3Parameter::recover_from(ParameterSnapshot* snapshot){
    set(snapshot->get_value<vec3>());
}


// ===== ColorParameter ===== //

ColorParameter::ColorParameter(std::string label_, std::function<vec4()> get_, std::function<void(vec4)> set_) : Parameter(label_, [this](){
    vec4 temp_vec = get();
    temp[0] = temp_vec.x;
    temp[1] = temp_vec.y;
    temp[2] = temp_vec.z;
    temp[3] = temp_vec.w;
    ImGui::Text(label.c_str());
    // ImGuiColorEditFlags_PickerHueWheel
    ImGui::ColorEdit4(Utils::format("##ColorEdit%1%", id).c_str(), temp, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoOptions);
    is_edited = true;
    if (is_edited){ // TODO : 리팩토링
        if (set != nullptr){
            set(vec4(temp[0], temp[1], temp[2], temp[3]));
        }
        is_edited = false;
    }
}), get(get_), set(set_){
}

ParameterSnapshot* ColorParameter::make_snapshot_new(){
    ParameterSnapshot* ret = new ParameterSnapshot(this);
    ret->set_value<vec4>(get());
    return ret;
}

void ColorParameter::recover_from(ParameterSnapshot* snapshot){
    set(snapshot->get_value<vec4>());
}


// ===== Component ===== //

bool Component::show_remove_button = false;

Component::Component(std::string label_) : label(label_){
}

Component::~Component(){
    for (Parameter* parameter : parameters){
        delete parameter;
    }
}
void Component::render(){
    ImGui::SeparatorText(label.c_str());
    if (show_remove_button){
        ImGui::SameLine();
        if (ImGui::Button(("x##" + this->label).c_str())){
            printf("remove component %s\n", this->label.c_str()); // TODO : 구현, Entity* parent 사용해야함!
        }
    }
    for (Parameter* parameter : parameters){
        parameter->render();
    }
}

void Component::set_label(std::string label_){
    label = label_;

}

std::list<Parameter*> Component::get_parameters(){
    return parameters;
}


// ===== Entity ===== //

Entity::~Entity(){
    for (Component* component : components){
        delete component;
    }
}

void Entity::add_component(Component* component){
    components.push_back(component);
}

std::list<Component*> Entity::get_components(){
    return components;
}

void Entity::clear_components(){
    components.clear();
}


// ===== EntitySnapshot ===== //

EntitySnapshot::EntitySnapshot(Entity* ptr){
    for (Component* component : ptr->get_components()){
        for (Parameter* parameter : component->get_parameters()){
            snapshots.push_back(parameter->make_snapshot_new());
        }
    }
}

EntitySnapshot::~EntitySnapshot(){
    for (ParameterSnapshot* snapshot : snapshots){
        delete snapshot;
    }
}

void EntitySnapshot::recover(){
    for (ParameterSnapshot* snapshot : snapshots){
        snapshot->recover();
    }
}


// ===== ParameterModifyTask ==== // 

ParameterModifyTask::ParameterModifyTask(std::string detail_, std::function<bool()> work_, std::function<void()> work_undo_) : TransactionTask(detail_, [=, this](){
    // snapshot1 = recovery_root->make_snapshot_new();
    return work_();
}, [=, this](){
    //snapshot1->recover();
    work_undo_();
}){
}