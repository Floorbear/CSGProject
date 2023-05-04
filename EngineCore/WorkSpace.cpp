#include "WorkSpace.h"
#include "Utils.h"
#include "Camera.h"
#include "Model.h"
#include "CSGNode.h"
#include "Renderer.h"

int WorkSpace::id_counter = 1; // 1부터 시작

WorkSpace::WorkSpace(GUI* parent_) : WorkSpace(parent_, std::string("test")){
}

WorkSpace::WorkSpace(GUI* parent_, std::string title_) : parent(parent_), title(title_), actions(WorkSpace_Actions(this)){
    id = id_counter++;
    //TODO : 여기서 도킹 설정?

    renderers.push_back(renderer_focused = new Renderer(512, 512));
    renderer_focused->set_parent(parent);
    renderer_focused->init();
}

WorkSpace::~WorkSpace(){
    // TODO 
    // -> ImGui::DisposeWindow(Utils::format("View##%1%", workspace.id).c_str());
    // -> ImGui::DisposeWindow(Utils::format("Objects##%1%", workspace.id).c_str());
    // -> ImGui::DisposeWindow(Utils::format("Inspector##%1%", workspace.id).c_str());
    // -> ImGui::DisposeWindow(Utils::format("Logs##%1%", workspace.id).c_str());
    // void ImGui::DisposeWindow(){
    // (windows에서 제거,, windowsbyid에서 제거. frame end에서 실행)
    //}

    for (Renderer* renderer : renderers){
        delete renderer;
    }
    for (Model* model : models){
        delete model;
    }
}

Camera* WorkSpace::get_main_camera(){
    assert(renderer_focused != nullptr);
    return renderer_focused->camera;
}

Model* WorkSpace::find_model(std::string_view name){
    for (Model* model : models){
        if (model->name == name){
            return model;
        }
    }
    return nullptr;
}

void WorkSpace::render_view(Renderer* renderer){
    // https://stackoverflow.com/questions/60955993/how-to-use-opengl-glfw3-render-in-a-imgui-window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(Utils::format("View##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    vec2 p_min = vec2(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y);
    vec2 p_max = vec2(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMax().y);
    renderer->resize(p_max - p_min);

    // Selection 렌더링
    if (ImGui::IsAnyMouseDown()){
        // TODO : 마우스 버튼별 선택 처리 추가
        vec2 mouse_position = vec2(ImGui::GetMousePos().x - p_min.x, ImGui::GetMousePos().y - p_min.y);
        SelectionPixelObjectInfo info = renderer->find_selection(models, mouse_position);
        if (!info.empty()){
            printf("%s", info.model_id->name.c_str()); // TEST
        }
    }

    // Gui 렌더링
    #pragma warning(disable: 4312)
    ImGui::GetWindowDrawList()->AddImage((void*)renderer->frame_texture, ImVec2(p_min.x, p_min.y), ImVec2(p_max.x, p_max.y), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::End();
    ImGui::PopStyleVar();

}

void WorkSpace::render_hierarchy(){
    ImGui::Begin(Utils::format("Hierarchy##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    CSGNode* mesh_clicked = nullptr;
    Model* model_clicked = nullptr;

    static std::function<void(CSGNode*)> draw_mesh_tree = [&](CSGNode* node){
        ImGuiTreeNodeFlags node_flags = base_flags;
        if (Utils::contains(selected_meshes, node)){
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }
        if (node->is_leaf_node()){
            node_flags |= ImGuiTreeNodeFlags_Leaf;
        }

        bool node_open = ImGui::TreeNodeEx((void*)node, node_flags, "<mesh>", 0);
        if (this->selection_mode == SelectionMode::Mesh_Selection && ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()){
            mesh_clicked = node;
        }
        if (ImGui::BeginDragDropSource()){
            ImGui::SetDragDropPayload("_TREENODE", NULL, 0); // TODO : 수정
            ImGui::Text("This is a drag and drop source");
            ImGui::EndDragDropSource();
        }
        if (node_open){
            for (CSGNode* child : node->get_children()){
                draw_mesh_tree(child);
            }
            ImGui::TreePop();
        }
    };

    static std::function<void(Model*)> draw_model_tree = [&](Model* model){
        ImGuiTreeNodeFlags node_flags = base_flags;
        if (Utils::contains(selected_models, model)){
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool node_open = ImGui::TreeNodeEx((void*)model, node_flags, model->name.c_str(), 0);
        if (this->selection_mode == SelectionMode::Model_Selection && ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()){
            model_clicked = model;
        }
        if (ImGui::BeginDragDropSource()){
            ImGui::SetDragDropPayload("_TREENODE", NULL, 0); // TODO : 수정
            ImGui::Text("This is a drag and drop source");
            ImGui::EndDragDropSource();
        }
        if (node_open){
            draw_mesh_tree(model->get_csg_mesh());
            for (Model* child : model->get_children()){
                draw_model_tree(child);
            }
            ImGui::TreePop();
        }
    };

    for (Model* model : models){
        draw_model_tree(model);
    }

    if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_None)
        && mesh_clicked == nullptr && model_clicked == nullptr){ // 빈칸 선택
        selected_meshes.clear();
        selected_models.clear();

    } else if (mesh_clicked != nullptr){ // 메쉬 선택
        if (ImGui::GetIO().KeyCtrl){
            if (Utils::contains(selected_meshes, mesh_clicked)){
                selected_meshes.remove(mesh_clicked);
            } else{
                selected_meshes.push_back(mesh_clicked);
            }
        } else{
            selected_meshes.clear();
            selected_meshes.push_back(mesh_clicked);
        }

    } else if (model_clicked != nullptr){ // 모델 선택
        if (ImGui::GetIO().KeyCtrl){
            if (Utils::contains(selected_models, model_clicked)){
                selected_models.remove(model_clicked);
            } else{
                selected_models.push_back(model_clicked);
            }
        } else{
            selected_models.clear();
            selected_models.push_back(model_clicked);
        }
    }

    ImGui::End();
}

void WorkSpace::render_inspector(){
    ImGui::Begin(Utils::format("Inspector##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);

    if (selected_models.empty()){ // 모델과 메쉬가 모두 컴포넌트를 가진 오브젝트니까 이부분에서 추상화를 시켜도 좋을텐데 일단 보류...
        if (!selected_meshes.empty()){
            selected_meshes.front()->get_transform()->render();
        }
    } else{
        for (Component* component : selected_models.front()->get_components()){
            component->render();
        }
    }
    ImGui::End();
}

void WorkSpace::render_logs(){
    if (gui_logs){
        ImGui::Begin(Utils::format("Logs##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
        ImGui::Text(Utils::format("Text%1%", id).c_str()); // TODO
        ImGui::End();
    }
}

void WorkSpace::render(){
    for (Renderer* renderer : renderers){
        renderer->render(models);
        render_view(renderer);
    }
    render_popup_menu();

    if (gui_hierarchy){
        render_hierarchy();
    }
    if (gui_inspector){
        render_inspector();
    }
    if (gui_logs){
        render_logs();
    }
}

void WorkSpace::process_input(){
}

void WorkSpace::on_mouse_press(vec2 position){
}

void WorkSpace::on_mouse_moved(vec2 position, vec2 position_prev){
}

void WorkSpace::on_mouse_released(vec2 position){
}

WorkSpace* WorkSpace::create_new(GUI* parent_, const char* filename){
    return new WorkSpace(parent_, filename);
}

void WorkSpace::render_popup_menu(){
    if (ImGui::GetIO().MouseReleased[1]){
        ImGui::OpenPopup("View_Popup_Edit");
    }
    if (ImGui::BeginPopup("View_Popup_Edit")){

        if (ImGui::MenuItem("Cut", "CTRL+X")){}
        if (ImGui::MenuItem("Copy", "CTRL+C")){}
        if (ImGui::MenuItem("Paste", "CTRL+V")){}
        if (ImGui::MenuItem("Delete", "Del")){} // TODO : 선택이 있는경우 제한
        ImGui::Separator();

        if (ImGui::BeginMenu("Select")){
            if (ImGui::MenuItem("Select Parent")){}
            if (ImGui::MenuItem("Reverse Selection")){}
            if (ImGui::MenuItem("Filter Selection")){}
            // TODO : 옵션 추가
            ImGui::EndMenu();
        }
        ImGui::Separator();
        // 우클릭도 선택가능.
        // TODO : if 선택된 모델이 있는경우 클릭하면 add child model
        // TODO : if 선택된 메쉬가 있는경우 add mesh_union, add mesh intersention, add mesh difference(제한적으로 활성화)
        if (ImGui::BeginMenu("Add Model")){
            if (ImGui::MenuItem("Cube")){
                transaction_manager.add("add cube",
                    [this](){ actions.add_cube_new(); },
                    [this](){ /*actions.delete_model();*/ });
            }
            /*if (ImGui::MenuItem("Sphere")){} TODO : 주석해제
            if (ImGui::MenuItem("Cylinder")){}
            if (ImGui::MenuItem("Cone")){}
            if (ImGui::MenuItem("Tetrahedron")){}
            if (ImGui::MenuItem("Pyramid")){}
            if (ImGui::MenuItem("Torus")){}*/
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Load Model")){}
        ImGui::Separator();
        if (ImGui::MenuItem("Set To Selection Group")){
            // csg연산 한거 나눠서 선택 안되게
        }

        // TODO : 메쉬 정제(csg가능하게) 연산들 - cgal 라이브러리 내에 다양한 알고리즘들 제공
        ImGui::EndPopup();
    }
}
