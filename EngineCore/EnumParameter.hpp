#include "Component.h"
#include "GUI.h"

template<typename E>
concept EnumType = std::is_enum_v<E>;

template <EnumType T>
class EnumParameter : public Parameter{
    std::function<T()> get;
    std::function<void(T)> set;
    std::function<std::list<T>()> get_values;
    const char** string_values;

public:
    EnumParameter(std::string label_, std::function<T()> get_, std::function<void(T)> set_, std::function<std::list<T>()> get_values_, const char* string_values[]);
};

template <EnumType T>
EnumParameter<T>::EnumParameter(std::string label_, std::function<T()> get_, std::function<void(T)> set_, std::function<std::list<T>()> get_values_, const char* string_values_[]) :
    Parameter(label_), get(get_), set(set_), get_values(get_values_), string_values(string_values_){
    int parameter_count = GUI::parameter_count++;
    render_action = [this, parameter_count](){ // TODO : completion 액션 추가
        ImGui::Text(label.c_str());
        std::list<T> values = get_values();
        T current = get();
        if (ImGui::BeginCombo(Utils::format("##Combo%1%", parameter_count).c_str(), string_values[(int)current], 0)){
            for (int n = 0; n < values.size(); n++){
                const bool is_selected = ((int)current == n);
                if (ImGui::Selectable(string_values[(int)n], is_selected)){
                    set((T)n);
                }
                if (is_selected){
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    };
}
