#include "GUI.h"
#include "Core.h"
#include "Utils.h"
#include "Camera.h"

#include "Imgui/imgui_internal.h"

#include <glad/glad.h>

#define NO_BACKGROUND

// ===== Shortcut ===== //

Shortcut::Shortcut(const char* name_, bool ctrl_, bool shift_, bool alt_, ImGuiKey discrete_key_, std::function<void()> callback_) :
    name(name_),
    ctrl(ctrl_),
    shift(shift_),
    alt(alt_),
    discrete_key(discrete_key_),
    callback(callback_){
    type = Type::Discrete;
}

Shortcut::Shortcut(const char* name_, bool ctrl_, bool shift_, bool alt_, int glfw_continuous_key_, std::function<void()> callback_) :
    name(name_),
    ctrl(ctrl_),
    shift(shift_),
    alt(alt_),
    continuous_key(glfw_continuous_key_),
    callback(callback_){
    type = Type::Continuous;
}

void Shortcut::check_execute(GLFWwindow* glfw_window){
    bool is_input_text_active = ImGui::GetInputTextState(ImGui::GetActiveID()) != nullptr;
    is_input_text_active = is_input_text_active && !(true == ImGui::GetIO().KeyCtrl &&
                                                   false == ImGui::GetIO().KeyShift &&
                                                   false == ImGui::GetIO().KeyAlt &&
                                                   ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S))); // 저장 키는 소중하므로 저장 키만 살려두자...
    if (type == Type::Continuous){
        if (!is_input_text_active &&
            ctrl == ImGui::GetIO().KeyCtrl &&
            shift == ImGui::GetIO().KeyShift &&
            alt == ImGui::GetIO().KeyAlt &&
            glfwGetKey(glfw_window, continuous_key) == GLFW_PRESS){
            callback();
        }
    } else if (type == Type::Discrete){
        if (!is_input_text_active &&
            ctrl == ImGui::GetIO().KeyCtrl &&
            shift == ImGui::GetIO().KeyShift &&
            alt == ImGui::GetIO().KeyAlt &&
            ImGui::IsKeyPressed(ImGui::GetKeyIndex(discrete_key))){
            callback();
        }
    }
}

std::string Shortcut::to_string(){
    std::string ret;
    if (type == Type::Discrete){
        if (ctrl) ret += "Ctrl+";
        if (alt) ret += "Alt+";
        if (shift) ret += "Shift+";
        if (discrete_key == ImGuiKey_Tab) ret += "Tab";
        else if (discrete_key == ImGuiKey_Space) ret += "Space";
        else if (discrete_key == ImGuiKey_Enter) ret += "Enter";
        else if (discrete_key == ImGuiKey_Backspace) ret += "Backspace";
        else if (discrete_key == ImGuiKey_Delete) ret += "Delete";
        else if (discrete_key == ImGuiKey_Escape) ret += "ESC (None)";
        else{
            ret += (char)discrete_key;
        }
    } else if (type == Type::Continuous){
        ret += "aaaaaaaaaaaa";// TEST
    }
    return ret;
}


// ===== Workspace ===== //

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
    ImGui::Begin(Utils::format("View##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    renderer->resize((int)ImGui::GetWindowSize().x, (int)ImGui::GetWindowSize().y);

    ImVec2 p_min = ImGui::GetWindowPos();
    ImVec2 p_max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y);

    #pragma warning(disable: 4312)
    ImGui::GetWindowDrawList()->AddImage((void*)renderer->frame_texture, p_min, p_max, ImVec2(0, 0), ImVec2(1, 1));
    ImGui::End();

}

void WorkSpace::render_hierarchy(){
    ImGui::Begin(Utils::format("Hierarchy##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    std::function<void(CSGNode*)> draw_mesh_tree = [&](CSGNode* node){
        ImGui::SetNextItemOpen(true, ImGuiCond_Once); // TODO : 삭제
        if (ImGui::TreeNode((void*)(intptr_t)0, std::string("<mesh>").c_str())){ // TODO : 메쉬는 어떻게 표시할것인가?
            for (CSGNode* child : node->get_children()){
                draw_mesh_tree(child);
            }
            ImGui::TreePop();
        }
    };
    std::function<void(Model*)> draw_model_tree = [&](Model* model){
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode((void*)(intptr_t)0, model->name.c_str())){
            draw_mesh_tree(model->get_mesh());
            for (Model* child : model->get_children()){
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
    for (Model* model : models){
        draw_model_tree(model);
    }
    ImGui::End();
}

void WorkSpace::render_inspector(){
    ImGui::Begin(Utils::format("Inspector##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    Model* selected_model_ = models.back();// TODO : 멤버의 selected model로 변경
    if (selected_model_ != nullptr){
        for (Component* component : selected_model_->get_components()){
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
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for (Renderer* renderer : renderers){
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

WorkSpace* WorkSpace::create_new(GUI* parent_, const char* filename){
    return new WorkSpace(parent_, filename);
}


// ===== Engine GUI ===== //

int GUI::parameter_count = 0;

GUI::GUI() : actions(GUI_Actions(this)){
    frame_count = 0;
    glfw_window = NULL;
    active_workspace = NULL;
    window_size.x = 1280;
    window_size.y = 720;
}

void GUI::init(){
    init_glfw();
    init_gui();
    init_shortcut();

    // ===== Test ===== //
    workspaces.push_back(new WorkSpace(this, "test")); // TODO : 정리하기
}

void GUI::render(){
    if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)){
        return;
    }
    render_begin();
    render_gui();
    render_end();
}

void GUI::update(){
    if (active_workspace != NULL){
        active_workspace->transaction_manager.execute_all();
    }
}

void GUI::process_input(){
    for (Shortcut shortcut : shortcuts){
        shortcut.check_execute(glfw_window);
    }
}

void GUI::dispose(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::init_glfw(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    glfw_window = glfwCreateWindow((int)window_size.x, (int)window_size.y, "CSGProject", NULL, NULL);
    if (glfw_window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(glfw_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glViewport(0, 0, (int)window_size.x, (int)window_size.y);
    glEnable(GL_DEPTH_TEST);

    // TODO : resize handler
}

void GUI::init_gui(){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& imgui_io = ImGui::GetIO(); (void)imgui_io;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
    ImGui_ImplOpenGL3_Init();
}

void GUI::render_begin(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::render_gui(){
    //bool show_demo_window = true;
    //ImGui::ShowDemoWindow(&show_demo_window);

    static ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus;
    #ifdef NO_BACKGROUND
    window_flags |= ImGuiWindowFlags_NoBackground;
    #endif

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("MainWindow", 0, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    render_menubar();

    static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable
        | ImGuiTabBarFlags_AutoSelectNewTabs
        | ImGuiTabBarFlags_TabListPopupButton
        | ImGuiTabBarFlags_TabListPopupButton
        | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton
        | ImGuiTabBarFlags_FittingPolicyScroll;

    if (ImGui::BeginTabBar("WorkSpaces", tab_bar_flags)){
        if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)){
            if (ImGui::GetIO().MouseClicked[1]){
                ImGui::OpenPopup("WorkSpace_AddTab_Popup");
            }
            if (ImGui::BeginPopup("WorkSpace_AddTab_Popup")){
                static int count = 0;
                std::string default_filename = Utils::format("NewFile%1%", count);
                static char filename_buf[128];
                strcpy_s(filename_buf, default_filename.c_str());
                ImGui::InputTextWithHint("WorkSpace_AddTab_Filename", default_filename.c_str(), filename_buf, IM_ARRAYSIZE(filename_buf), ImGuiInputTextFlags_CharsNoBlank);
                if (ImGui::Button("Create")){
                    workspaces.push_back(WorkSpace::create_new(this, filename_buf));
                    ++count;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        for (WorkSpace* workspace : workspaces){ // TODO : 탭 우클릭하면 제목수정 팝업창
            bool is_opened = true;
            if (ImGui::BeginTabItem((workspace->title + "##" + std::to_string(workspace->id)).c_str(), &is_opened, ImGuiTabItemFlags_None)){

                active_workspace = workspace;

                ImGuiID dockspace_id = ImGui::GetID(Utils::format("WorkSpace_DockSpace_%1%", workspace->id).c_str());
                if (!workspace->gui_initialized){
                    // TODO : 이거 여깄어도 될까? workspace생성할때 TaskManager통해 한번만 수행하는걸로 변경?
                    ImGui::DockBuilderRemoveNode(dockspace_id);
                    if (ImGui::DockBuilderGetNode(dockspace_id) == NULL){
                        ImGui::DockBuilderAddNode(dockspace_id, 0);
                        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

                        ImGuiID dock_main_id = dockspace_id;
                        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);
                        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
                        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);

                        ImGui::DockBuilderDockWindow(Utils::format("View##%1%", workspace->id).c_str(), dock_main_id);// TODO : 여러 view 도킹
                        ImGui::DockBuilderDockWindow(Utils::format("Hierarchy##%1%", workspace->id).c_str(), dock_id_left);
                        ImGui::DockBuilderDockWindow(Utils::format("Inspector##%1%", workspace->id).c_str(), dock_id_right);
                        ImGui::DockBuilderDockWindow(Utils::format("Logs##%1%", workspace->id).c_str(), dock_id_bottom);
                        ImGui::DockBuilderFinish(dockspace_id);
                    }
                    workspace->gui_initialized = true;
                }

                ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(1, 1, 1, 1));
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
                ImGui::PopStyleColor();
                ImGui::EndTabItem();
            }
            if (!is_opened){
                Core::get()->task_manager.add([this, workspace](){
                    active_workspace = nullptr;
                    workspaces.remove_if([=](WorkSpace* workspace_){
                        return workspace_->id == workspace->id;
                    });
                    delete workspace;
                });
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    if (active_workspace != NULL){
        active_workspace->render();
    }
}

void GUI::render_end(){
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(glfw_window, &display_w, &display_h);
    window_size.x = (float)display_w;
    window_size.y = (float)display_h;

    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwPollEvents();
    glfwSwapBuffers(glfw_window);

    ++frame_count;
}


// ===== 기능 ===== //

void GUI::render_menubar(){
    if (ImGui::BeginMenuBar()){
        if (ImGui::BeginMenu("File")){
            if (ImGui::MenuItem("New")){
                // 빈 CSGProject 형식 생성
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")){
                // 로딩 형식 -> CSGProject, stl(새 프젝 생성후 모델하나 로딩)
            }
            if (ImGui::BeginMenu("Open Recent")){
                //ImGui::MenuItem("fish_hat.c");
                //ImGui::MenuItem("fish_hat.inl");
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")){
                // 저장 형식 -> CSGProject, stl
            }
            if (ImGui::MenuItem("Save As..")){}
            if (ImGui::MenuItem("Close")){}
            if (ImGui::MenuItem("Close All")){}
            ImGui::Separator();
            if (ImGui::MenuItem("Options")){
                // TODO : 각종 설정 내용 정하기
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")){
            if (ImGui::MenuItem(("Undo " + active_workspace->transaction_manager.undo_detail()).c_str(), "CTRL+Z", false,
                                active_workspace->transaction_manager.can_undo())){
                active_workspace->transaction_manager.undo();
            }
            if (ImGui::MenuItem(("Redo " + active_workspace->transaction_manager.redo_detail()).c_str(), "CTRL+Y", false,
                                active_workspace->transaction_manager.can_redo())){
                active_workspace->transaction_manager.redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Union Selected Objects")){}
            if (ImGui::MenuItem("Intersect Selected Objects")){}
            if (ImGui::MenuItem("Difference Selected Objects")){
                // TODO : selection에 순서가 있어야함
            }
            // ? : 굳이 편집메뉴가 중복으로 있을까 해서 우클릭메뉴 중복 내용은 제거함.
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")){
            if (ImGui::MenuItem("Add View Window")){}
            if (ImGui::MenuItem("Hierarchy", NULL, &active_workspace->gui_hierarchy)){}
            if (ImGui::MenuItem("Inspector", NULL, &active_workspace->gui_inspector)){}
            if (ImGui::MenuItem("Logs", NULL, &active_workspace->gui_logs)){}
            ImGui::Separator();
            if (ImGui::MenuItem("CSG Tree View", "CTRL+G", &active_workspace->gui_csgtree)){}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void WorkSpace::render_popup_menu(){
    if (ImGui::GetIO().MouseClicked[1]){
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

int tcnt; // TEST
void GUI::init_shortcut(){

    // ===== 편집 기능 ===== //
    {
        shortcuts.push_back(Shortcut("save", true, false, false, ImGuiKey_S, [](){
            printf("save");
        }));
        shortcuts.push_back(Shortcut("paste", true, false, false, ImGuiKey_V, [this](){
            int ttcnt = tcnt;
            active_workspace->transaction_manager.add(Utils::format("paste %1%", ttcnt), [=](){
                printf("paste %d do\n", ttcnt);
            }, [=](){
                printf("paste %d undo\n", ttcnt);
            });
            tcnt++;
        }));
        shortcuts.push_back(Shortcut("undo", true, false, false, ImGuiKey_Z, [this](){
            active_workspace->transaction_manager.undo();
        }));
        shortcuts.push_back(Shortcut("redo", true, false, false, ImGuiKey_Y, [this](){
            active_workspace->transaction_manager.redo();
        }));
    }

    // ===== 카메라 ===== //
    {
        shortcuts.push_back(Shortcut("camera move right", false, false, false, GLFW_KEY_D, [this](){
            active_workspace->get_main_camera()->get_transform()->
                translate(vec3(1, 0, 0) * Camera::speed_move_default * Utils::time_delta());
        }));
        shortcuts.push_back(Shortcut("camera move left", false, false, false, GLFW_KEY_A, [this](){
            active_workspace->get_main_camera()->get_transform()->
                translate(vec3(-1, 0, 0) * Camera::speed_move_default * Utils::time_delta());
        }));
        shortcuts.push_back(Shortcut("camera move up", false, false, false, GLFW_KEY_Q, [this](){
            active_workspace->get_main_camera()->get_transform()->
                add_position({0.f,Camera::speed_move_default * Utils::time_delta(), 0});
        }));
        shortcuts.push_back(Shortcut("camera move down", false, false, false, GLFW_KEY_E, [this](){
            active_workspace->get_main_camera()->get_transform()->
                add_position({0.f,-Camera::speed_move_default * Utils::time_delta(), 0});
        }));
        shortcuts.push_back(Shortcut("camera move forward", false, false, false, GLFW_KEY_W, [this](){
            active_workspace->get_main_camera()->get_transform()->
                translate(vec3(0, 0, 1) * Camera::speed_move_default * Utils::time_delta());
        }));
        shortcuts.push_back(Shortcut("camera move back", false, false, false, GLFW_KEY_S, [this](){
            active_workspace->get_main_camera()->get_transform()->
                translate(vec3(0, 0, -1) * Camera::speed_move_default * Utils::time_delta());
        }));

        shortcuts.push_back(Shortcut("camera move fast right", true, false, false, GLFW_KEY_D, [this](){
            vec3 Dir = active_workspace->get_main_camera()->get_transform()->get_right_dir() * Camera::speed_move_fast;
            active_workspace->get_main_camera()->get_transform()->add_position(Dir * Utils::time_delta());
        }));
        shortcuts.push_back(Shortcut("camera move fast left", true, false, false, GLFW_KEY_A, [this](){
            vec3 Dir = active_workspace->get_main_camera()->get_transform()->get_right_dir() * -Camera::speed_move_fast;
            active_workspace->get_main_camera()->get_transform()->add_position(Dir * Utils::time_delta());
        }));
        shortcuts.push_back(Shortcut("camera move fast up", true, false, false, GLFW_KEY_Q, [this](){
            active_workspace->get_main_camera()->get_transform()->add_position({0.f,Camera::speed_move_fast * Utils::time_delta(), 0});
        }));
        shortcuts.push_back(Shortcut("camera move fast down", true, false, false, GLFW_KEY_E, [this](){
            active_workspace->get_main_camera()->get_transform()->add_position({0.f,-Camera::speed_move_fast * Utils::time_delta(), 0});
        }));
        shortcuts.push_back(Shortcut("camera move fast forward", true, false, false, GLFW_KEY_W, [this](){
            vec3 Dir = active_workspace->get_main_camera()->get_transform()->get_forward_dir() * Camera::speed_move_fast;
            active_workspace->get_main_camera()->get_transform()->add_position(Dir * Utils::time_delta());
        }));
        shortcuts.push_back(Shortcut("camera move fast back", true, false, false, GLFW_KEY_S, [this](){
            vec3 Dir = active_workspace->get_main_camera()->get_transform()->get_forward_dir() * -Camera::speed_move_fast;
            active_workspace->get_main_camera()->get_transform()->add_position(Dir * Utils::time_delta());
        }));

        shortcuts.push_back(Shortcut("camera rotate up", false, false, false, GLFW_KEY_UP, [this](){
            active_workspace->get_main_camera()->get_transform()->rotate({Camera::speed_rotate_default * Utils::time_delta(), 0.0f, 0.0f});
        }));
        shortcuts.push_back(Shortcut("camera rotate down", false, false, false, GLFW_KEY_DOWN, [this](){
            active_workspace->get_main_camera()->get_transform()->rotate({-Camera::speed_rotate_default * Utils::time_delta(), 0.0f, 0.0f});
        }));
        shortcuts.push_back(Shortcut("camera rotate left", false, false, false, GLFW_KEY_LEFT, [this](){
            active_workspace->get_main_camera()->get_transform()->rotate({0.0f, -Camera::speed_rotate_default * Utils::time_delta(), 0.0f});
        }));
        shortcuts.push_back(Shortcut("camera rotate right", false, false, false, GLFW_KEY_RIGHT, [this](){
            active_workspace->get_main_camera()->get_transform()->rotate({0.0f, Camera::speed_rotate_default * Utils::time_delta(), 0.0f});
        }));
    }
}