#include "Component.h"
#include "GUI.h"

#include <Imgui/imgui.h>

Parameter::Parameter(std::string label_) : label(label_){
}

Parameter::Parameter(std::string label_, std::function<void()> render_) : label(label_), render_action(render_){
}

void Parameter::render(){
    render_action();
}

Vec3Parameter::Vec3Parameter(std::string label_, std::string label_x_, std::string label_y_, std::string label_z_, std::function<vec3()> get_, std::function<void(vec3)> set_) :
    Parameter(label_), label_x(label_x_), label_y(label_y_), label_z(label_z_), get(get_), set(set_){
    int parameter_count = GUI::parameter_count++;
    static ImGuiInputTextCallback callback = [](ImGuiInputTextCallbackData* data){// TODO : Completion에는 transaction 등록
        Vec3Parameter* parent = (Vec3Parameter*)data->UserData;
        parent->is_edited = true;
        return 0;
    };
    render_action = [this, parameter_count](){
        std::string n = "##" + std::to_string(parameter_count);
        temp = get();
        ImGui::Text(label.c_str());
        ImGui::InputFloat((label_x + n).c_str(), &temp.x, 0, 0, "%.3f", ImGuiInputTextFlags_CallbackEdit, callback, (void*)this);
        ImGui::SameLine();
        ImGui::InputFloat((label_y + n).c_str(), &temp.y, 0, 0, "%.3f", ImGuiInputTextFlags_CallbackEdit, callback, (void*)this);
        ImGui::SameLine();
        ImGui::InputFloat((label_z + n).c_str(), &temp.z, 0, 0, "%.3f", ImGuiInputTextFlags_CallbackEdit, callback, (void*)this);
        if(is_edited){
            if (set != nullptr){
                set(temp);
            }
        }
    };
}

Component::Component(std::string label_) : label(label_){
}

Component::~Component(){
    for (Parameter* parameter : parameters){
        delete parameter;
    }
}
void Component::render(){
    ImGui::SeparatorText(label.c_str());
    for (Parameter* parameter : parameters){
        parameter->render();
    }
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
