#include "Component.h"
#include "GUI.h"

template<typename E>
concept EnumType = std::is_enum_v<E>;

template<EnumType T>
class EnumParameter : public Parameter{
    std::function<T()> get;
    std::function<void(T)> set;
    std::function<std::vector<T>()> get_values;
    const char** string_values;

public:
    EnumParameter(std::string label_, std::function<T()> get_, std::function<void(T)> set_, std::function<std::vector<T>()> get_values_, const char* string_values_[]) :
        Parameter(label_, [this](){

        ImGui::Text(label.c_str());
        std::vector<T> values = get_values();
        T current = get();
        if (ImGui::BeginCombo(Utils::format("##Combo%1%", id).c_str(), string_values[(int)current], 0)){
            for (int n = 0; n < values.size(); n++){
                const bool is_selected = (current == values[n]);
                if (ImGui::Selectable(string_values[(int)values[n]], is_selected)){
                    set(values[n]);
                }
                if (is_selected){
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }), get(get_), set(set_), get_values(get_values_), string_values(string_values_){
    }

    ParameterSnapshot* make_snapshot_new() override{
        ParameterSnapshot* ret = new ParameterSnapshot(this);
        ret->set_value_(new T(get()));
        return ret;
    }

    void recover_from(ParameterSnapshot* snapshot) override{
        set(*((T*)snapshot->get_value_()));
    }
};
