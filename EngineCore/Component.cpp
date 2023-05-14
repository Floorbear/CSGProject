#include "Component.h"
#include "GUI.h"
#include "Utils.h"

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

Parameter::Parameter(std::string label_) : label(label_){
}

Parameter::Parameter(std::string label_, std::function<void()> render_) : label(label_), render_action(render_){
}

void Parameter::render(){
    render_action();
}

FloatParameter::FloatParameter(std::string label_, std::function<float()> get_, std::function<void(float)> set_) :
    Parameter(label_), get(get_), set(set_){
    int parameter_count = GUI::parameter_count++;
    render_action = [this, parameter_count](){ // TODO : completion 액션 추가
        temp = get();
        ImGui::Text(label.c_str());
        ImGui::InputFloat(Utils::format("##InputFloat%1%", parameter_count).c_str(), &temp, 0, 0, "%.3f", ImGuiInputTextFlags_CallbackEdit, edit_callback, (void*)this);
        if (is_edited){ // TODO : 리팩토링
            if (set != nullptr){
                set(temp);
            }
        }
    };
}

BoolParameter::BoolParameter(std::string label_, std::function<bool()> get_, std::function<void(bool)> set_) :
    Parameter(label_), get(get_), set(set_){
    int parameter_count = GUI::parameter_count++;
    render_action = [this, parameter_count](){ // TODO : completion 액션 추가
        bool temp = get();
        if (ImGui::Checkbox((label + Utils::format("##CheckBox%1%", parameter_count)).c_str(), &temp)){
            printf("aa");
            set(temp);
        }
    };
}


Vec3Parameter::Vec3Parameter(std::string label_, std::string label_x_, std::string label_y_, std::string label_z_, std::function<vec3()> get_, std::function<void(vec3)> set_) :
    Parameter(label_), label_x(label_x_), label_y(label_y_), label_z(label_z_), get(get_), set(set_){
    int parameter_count = GUI::parameter_count++; // TODO : parameter id로 리팩토링
    render_action = [this, parameter_count](){ // TODO : completion 액션 추가
        temp = get();
        ImGui::Text(label.c_str());
        if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_SizingStretchSame)){
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("x");
            ImGui::SameLine(0, 3);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputFloat(Utils::format("##InputFloat%1%", parameter_count).c_str(), &temp.x, 0, 0, "%.3f", ImGuiInputTextFlags_CallbackEdit, edit_callback, (void*)this);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("y");
            ImGui::SameLine(0, 3);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputFloat(Utils::format("##InputFloat%1%", parameter_count + 1).c_str(), &temp.y, 0, 0, "%.3f", ImGuiInputTextFlags_CallbackEdit, edit_callback, (void*)this);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("z");
            ImGui::SameLine(0, 3);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputFloat(Utils::format("##InputFloat%1%", parameter_count + 2).c_str(), &temp.z, 0, 0, "%.3f", ImGuiInputTextFlags_CallbackEdit, edit_callback, (void*)this);
            ImGui::EndTable();
        }
        if (is_edited){ // TODO : 리팩토링
            if (set != nullptr){
                set(temp);
            }
        }
    };
}

ColorParameter::ColorParameter(std::string label_, std::function<vec4()> get_, std::function<void(vec4)> set_) :
    Parameter(label_), get(get_), set(set_){
    int parameter_count = GUI::parameter_count++;
    render_action = [this, parameter_count](){ // TODO : completion 액션 추가
        vec4 temp_vec = get();
        temp[0] = temp_vec.x;
        temp[1] = temp_vec.y;
        temp[2] = temp_vec.z;
        temp[3] = temp_vec.w;
        ImGui::Text(label.c_str());
        // ImGuiColorEditFlags_PickerHueWheel
        ImGui::ColorEdit4(Utils::format("##ColorEdit%1%", parameter_count).c_str(), temp, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoOptions);
        is_edited = true;
        if (is_edited){
            if (set != nullptr){
                set(vec4(temp[0], temp[1], temp[2], temp[3]));
            }
        }
    };
}

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

/*
* TODO : undo redo에서 focus가 inputtext인지 확인
*
            static char buf1[64] = ""; ImGui::InputText("default",     buf1, 64);
            static char buf2[64] = ""; ImGui::InputText("decimal",     buf2, 64, ImGuiInputTextFlags_CharsDecimal);
            static char buf3[64] = ""; ImGui::InputText("hexadecimal", buf3, 64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
            static char buf4[64] = ""; ImGui::InputText("uppercase",   buf4, 64, ImGuiInputTextFlags_CharsUppercase);
            static char buf5[64] = ""; ImGui::InputText("no blank",    buf5, 64, ImGuiInputTextFlags_CharsNoBlank);
            static char buf6[64] = ""; ImGui::InputText("\"imgui\" letters", buf6, 64, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterImGuiLetters);


            static float col1[3] = { 1.0f, 0.0f, 0.2f };
            static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            ImGui::ColorEdit3("color 1", col1);
            ImGui::SameLine(); HelpMarker(
                "Click on the color square to open a color picker.\n"
                "Click and hold to use drag and drop.\n"
                "Right-click on the color square to show options.\n"
                "CTRL+click on individual component to input value.\n");

            ImGui::ColorEdit4("color 2", col2);


            ImGui::DragInt("drag int", &i1, 1);
            ImGui::SameLine(); HelpMarker(
                "Click and drag to edit value.\n"
                "Hold SHIFT/ALT for faster/slower edit.\n"
                "Double-click or CTRL+click to input value.");

            ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);

            static float f1 = 1.00f, f2 = 0.0067f;
            ImGui::DragFloat("drag float", &f1, 0.005f);
            ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");


            ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            IMGUI_DEMO_MARKER("Widgets/Basic/InputInt, InputFloat");
            static int i0 = 123;
            ImGui::InputInt("input int", &i0);

            static float f0 = 0.001f;
            ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static float f1 = 1.e10f;
            ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            ImGui::SameLine(); HelpMarker(
                "You can input value using the scientific notation,\n"
                "  e.g. \"1e+8\" becomes \"100000000\".");

*/