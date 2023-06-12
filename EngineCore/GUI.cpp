#include "GUI.h"
#include "Utils.h"
#include "Camera.h"
#include "Model.h"
#include "CSGNode.h"
#include "WorkSpace.h"
#include "Core.h"
#include "FrameBuffer.h"
#include "FileSystem.h"

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
    for (WorkSpace* workspace : workspaces){
        workspace->process_input();
    }
}

void GUI::dispose(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //싱글톤 & Static 리소스는 Core::dispose에서 삭제
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
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
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
    imgui_io.ConfigWindowsMoveFromTitleBarOnly = true;

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
    // bool show_demo_window = true;
    // ImGui::ShowDemoWindow(&show_demo_window);

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
                workspace->check_init_dockspace(dockspace_id, viewport);
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(glfw_window, &display_w, &display_h);
    window_size.x = (float)display_w;
    window_size.y = (float)display_h;

    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
            if (ImGui::MenuItem("Export Scene", NULL, false, active_workspace->root_model->children_size() != 0)){
                Core::get()->task_manager.add([this](){
                    Mesh result;
                    active_workspace->root_model->make_union_mesh(result);
                    std::string default_filename = active_workspace->title + ".stl";
                    std::wstring default_filename_w;
                    default_filename_w.assign(default_filename.begin(), default_filename.end());
                    result.save(FileSystem::select_file_save(default_filename_w).get_path());
                });
            }
            if (ImGui::MenuItem("Close")){}
            if (ImGui::MenuItem("Close All")){}
            ImGui::Separator();
            if (ImGui::MenuItem("Options")){
                // TODO : 각종 설정 내용 정하기
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cleanup Memory")){
                // TODO : undo history 삭제, leaked pointers 삭제
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
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")){
            if (ImGui::MenuItem("Add View Window")){
                active_workspace->add_view_new();
            }
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

void GUI::init_shortcut(){

    // ===== 편집 기능 ===== //
    {
        shortcuts.push_back(Shortcut("save", true, false, false, ImGuiKey_S, [](){
            printf("save");
        }));
        shortcuts.push_back(Shortcut("paste", true, false, false, ImGuiKey_V, [this](){
            if (!active_workspace->selected_models.empty()){// TEST
                Transform t = active_workspace->selected_models.back()->get_transform()->get_value();
                printf("%f %f %f\n", t.get_position().x, t.get_position().y, t.get_position().z);
            }
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
            vec3 Dir = active_workspace->get_main_camera()->get_transform()->get_right_dir() * Camera::speed_move_default * Camera::speed_move_fast_multiplier;
            active_workspace->get_main_camera()->get_transform()->add_position(Dir * Utils::time_delta());
        }));
        shortcuts.push_back(Shortcut("camera move fast left", true, false, false, GLFW_KEY_A, [this](){
            vec3 Dir = active_workspace->get_main_camera()->get_transform()->get_right_dir() * -Camera::speed_move_default * Camera::speed_move_fast_multiplier;
            active_workspace->get_main_camera()->get_transform()->add_position(Dir * Utils::time_delta());
        }));
        shortcuts.push_back(Shortcut("camera move fast up", true, false, false, GLFW_KEY_Q, [this](){
            active_workspace->get_main_camera()->get_transform()->add_position({0.f, Camera::speed_move_default * Camera::speed_move_fast_multiplier * Utils::time_delta(), 0});
        }));
        shortcuts.push_back(Shortcut("camera move fast down", true, false, false, GLFW_KEY_E, [this](){
            active_workspace->get_main_camera()->get_transform()->add_position({0.f, -Camera::speed_move_default * Camera::speed_move_fast_multiplier * Utils::time_delta(), 0});
        }));
        shortcuts.push_back(Shortcut("camera move fast forward", true, false, false, GLFW_KEY_W, [this](){
            vec3 Dir = active_workspace->get_main_camera()->get_transform()->get_forward_dir() * Camera::speed_move_default * Camera::speed_move_fast_multiplier;
            active_workspace->get_main_camera()->get_transform()->add_position(Dir * Utils::time_delta());
        }));
        shortcuts.push_back(Shortcut("camera move fast back", true, false, false, GLFW_KEY_S, [this](){
            vec3 Dir = active_workspace->get_main_camera()->get_transform()->get_forward_dir() * -Camera::speed_move_default * Camera::speed_move_fast_multiplier;
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