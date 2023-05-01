#include "WorkSpace.h"
#include "Utils.h"
#include "Camera.h"
#include "Model.h"
#include "CSGNode.h"
#include "Renderer.h"

//#include "Imgui/imgui_internal.h"

// ===== Workspace ===== //

int WorkSpace::id_counter = 1; // 1부터 시작

WorkSpace::WorkSpace(GUI* parent_) : WorkSpace(parent_, std::string("test")) {
}

WorkSpace::WorkSpace(GUI* parent_, std::string title_) : parent(parent_), title(title_), actions(WorkSpace_Actions(this)) {
    id = id_counter++;
    //TODO : 여기서 도킹 설정?

    renderers.push_back(renderer_focused = new Renderer(512, 512));
    renderer_focused->set_parent(parent);
    renderer_focused->init();
}

WorkSpace::~WorkSpace() {
    // TODO 
    // -> ImGui::DisposeWindow(Utils::format("View##%1%", workspace.id).c_str());
    // -> ImGui::DisposeWindow(Utils::format("Objects##%1%", workspace.id).c_str());
    // -> ImGui::DisposeWindow(Utils::format("Inspector##%1%", workspace.id).c_str());
    // -> ImGui::DisposeWindow(Utils::format("Logs##%1%", workspace.id).c_str());
    // void ImGui::DisposeWindow(){
    // (windows에서 제거,, windowsbyid에서 제거. frame end에서 실행)
    //}

    for (Renderer* renderer : renderers) {
        delete renderer;
    }
    for (Model* model : models) {
        delete model;
    }
}

Camera* WorkSpace::get_main_camera() {
    assert(renderer_focused != nullptr);
    return renderer_focused->camera;
}

Model* WorkSpace::find_model(std::string_view name) {
    for (Model* model : models) {
        if (model->name == name) {
            return model;
        }
    }
    return nullptr;
}

void WorkSpace::render_view(Renderer* renderer) {
    // https://stackoverflow.com/questions/60955993/how-to-use-opengl-glfw3-render-in-a-imgui-window
    ImGui::Begin(Utils::format("View##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    renderer->resize((int)ImGui::GetWindowSize().x, (int)ImGui::GetWindowSize().y);

    ImVec2 p_min = ImGui::GetWindowPos();
    ImVec2 p_max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y);

#pragma warning(disable: 4312)
    ImGui::GetWindowDrawList()->AddImage((void*)renderer->frame_texture, p_min, p_max, ImVec2(0, 0), ImVec2(1, 1));
    ImGui::End();

}

void WorkSpace::render_hierarchy() {
    ImGui::Begin(Utils::format("Hierarchy##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    std::function<void(CSGNode*)> draw_mesh_tree = [&](CSGNode* node) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once); // TODO : 삭제
        if (ImGui::TreeNode((void*)(intptr_t)0, std::string("<mesh>").c_str())) { // TODO : 메쉬는 어떻게 표시할것인가?
            for (CSGNode* child : node->get_children()) {
                draw_mesh_tree(child);
            }
            ImGui::TreePop();
        }
    };
    std::function<void(Model*)> draw_model_tree = [&](Model* model) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode((void*)(intptr_t)0, model->name.c_str())) {
            draw_mesh_tree(model->get_csg_mesh());
            for (Model* child : model->get_children()) {
                draw_model_tree(child);
            }
            ImGui::TreePop();
        }
    };
    // TODO : 하이어라키 선택
    /*static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    static bool align_label_with_current_x_position = false;
    static bool test_drag_and_drop = false;
    ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow",       &base_flags, ImGuiTreeNodeFlags_OpenOnArrow);
    ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick", &base_flags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
    ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth",    &base_flags, ImGuiTreeNodeFlags_SpanAvailWidth); ImGui::SameLine(); HelpMarker("Extend hit area to all available width instead of allowing more items to be laid out after the node.");
    ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth",     &base_flags, ImGuiTreeNodeFlags_SpanFullWidth);
    ImGui::Checkbox("Align label with current X position", &align_label_with_current_x_position);
    ImGui::Checkbox("Test tree node as drag source", &test_drag_and_drop);
    ImGui::Text("Hello!");
    if (align_label_with_current_x_position)
    ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

    // 'selection_mask' is dumb representation of what may be user-side selection state.
    //  You may retain selection state inside or outside your objects in whatever format you see fit.
    // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
    /// of the loop. May be a pointer to your own node type, etc.
    static int selection_mask = (1 << 2);
    int node_clicked = -1;
    for (int i = 0; i < 6; i++)
    {
    // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
    // To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
    ImGuiTreeNodeFlags node_flags = base_flags;
    const bool is_selected = (selection_mask & (1 << i)) != 0;
    if (is_selected)
    node_flags |= ImGuiTreeNodeFlags_Selected;
    if (i < 3)
    {
    // Items 0..2 are Tree Node
    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    node_clicked = i;
    if (test_drag_and_drop && ImGui::BeginDragDropSource())
    {
    ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
    ImGui::Text("This is a drag and drop source");
    ImGui::EndDragDropSource();
    }
    if (node_open)
    {
    ImGui::BulletText("Blah blah\nBlah Blah");
    ImGui::TreePop();
    }
    }
    else
    {
    // Items 3..5 are Tree Leaves
    // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
    // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
    ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    node_clicked = i;
    if (test_drag_and_drop && ImGui::BeginDragDropSource())
    {
    ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
    ImGui::Text("This is a drag and drop source");
    ImGui::EndDragDropSource();
    }
    }
    }
    if (node_clicked != -1)
    {
    // Update selection state
    // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
    if (ImGui::GetIO().KeyCtrl)
    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
    else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
    selection_mask = (1 << node_clicked);           // Click to single-select
    }
    if (align_label_with_current_x_position)
    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
    ImGui::TreePop();
    }
    ImGui::TreePop();*/
    for (Model* model : models) {
        draw_model_tree(model);
    }
    ImGui::End();
}

void WorkSpace::render_inspector() {
    ImGui::Begin(Utils::format("Inspector##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    Model* selected_model_ = models.back();// TODO : 멤버의 selected model로 변경
    if (selected_model_ != nullptr) {
        for (Component* component : selected_model_->get_components()) {
            component->render();
        }
    }
    ImGui::End();
}

void WorkSpace::render_logs() {
    if (gui_logs) {
        ImGui::Begin(Utils::format("Logs##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
        ImGui::Text(Utils::format("Text%1%", id).c_str()); // TODO
        ImGui::End();
    }
}

void WorkSpace::render() {
    for (Renderer* renderer : renderers) {
        renderer->render(models);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for (Renderer* renderer : renderers) {
        render_view(renderer);
    }
    render_popup_menu();

    if (gui_hierarchy) {
        render_hierarchy();
    }
    if (gui_inspector) {
        render_inspector();
    }
    if (gui_logs) {
        render_logs();
    }
}

WorkSpace* WorkSpace::create_new(GUI* parent_, const char* filename) {
    return new WorkSpace(parent_, filename);
}

void WorkSpace::render_popup_menu() {
    if (ImGui::GetIO().MouseClicked[1]) {
        ImGui::OpenPopup("View_Popup_Edit");
    }
    if (ImGui::BeginPopup("View_Popup_Edit")) {

        if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        if (ImGui::MenuItem("Paste", "CTRL+V")) {}
        if (ImGui::MenuItem("Delete", "Del")) {} // TODO : 선택이 있는경우 제한
        ImGui::Separator();

        if (ImGui::BeginMenu("Select")) {
            if (ImGui::MenuItem("Select Parent")) {}
            if (ImGui::MenuItem("Reverse Selection")) {}
            if (ImGui::MenuItem("Filter Selection")) {}
            // TODO : 옵션 추가
            ImGui::EndMenu();
        }
        ImGui::Separator();
        // 우클릭도 선택가능.
        // TODO : if 선택된 모델이 있는경우 클릭하면 add child model
        // TODO : if 선택된 메쉬가 있는경우 add mesh_union, add mesh intersention, add mesh difference(제한적으로 활성화)
        if (ImGui::BeginMenu("Add Model")) {
            if (ImGui::MenuItem("Cube")) {
                transaction_manager.add("add cube",
                    [this]() { actions.add_cube_new(); },
                    [this]() { /*actions.delete_model();*/ });
            }
            /*if (ImGui::MenuItem("Sphere")){} TODO : 주석해제
            if (ImGui::MenuItem("Cylinder")){}
            if (ImGui::MenuItem("Cone")){}
            if (ImGui::MenuItem("Tetrahedron")){}
            if (ImGui::MenuItem("Pyramid")){}
            if (ImGui::MenuItem("Torus")){}*/
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Load Model")) {}
        ImGui::Separator();
        if (ImGui::MenuItem("Set To Selection Group")) {
            // csg연산 한거 나눠서 선택 안되게
        }

        // TODO : 메쉬 정제(csg가능하게) 연산들 - cgal 라이브러리 내에 다양한 알고리즘들 제공
        ImGui::EndPopup();
    }
}
