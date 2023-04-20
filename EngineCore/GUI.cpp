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
    if (type == Type::Continuous){
        if (ctrl == ImGui::GetIO().KeyCtrl &&
            shift == ImGui::GetIO().KeyShift &&
            alt == ImGui::GetIO().KeyAlt &&
            glfwGetKey(glfw_window, continuous_key) == GLFW_PRESS){
            callback();
        }
    } else if (type == Type::Discrete){
        if (ctrl == ImGui::GetIO().KeyCtrl &&
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
    gui_initialized = false;
    gui_hierarchy = true;
    gui_inspector = true;
    gui_logs = true;
    gui_csgtree = false;
    //TODO : 여기서 도킹 설정?

    // TODO : 렌더러로 이동
    mainCamera = new Camera(parent->window_size.x, parent->window_size.y);
    mainCamera->set_parent(parent_);
    cameras.push_back(mainCamera);

    renderer.viewport_size = vec2(512, 512);//parent->window_size; // TODO : view창 만들면서 view창 크기로 지정
    renderer.set_parent(parent);
    renderer.init();
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

    for (Model* model : models){
        delete model;
    }

    for (Camera* camera : cameras){ // TODO : renderer로 이동
        delete camera;
    }
}

Model* WorkSpace::find_model(std::string_view name){
    for (Model* model : models){
        if (model->name == name){
            return model;
        }
    }
    return nullptr;
}

void WorkSpace::render(){
    static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;
    #ifdef NO_BACKGROUND
    window_flags |= ImGuiWindowFlags_NoBackground;
    #endif

    static float view_w = 512;
    static float view_h = 512;

    {// TODO : 여러 view에서 models를 참조해서 그려야함. (렌더러 개수만큼 수행)
        // https://stackoverflow.com/questions/60955993/how-to-use-opengl-glfw3-render-in-a-imgui-window
        GLuint f_tex = init_fbo(view_w, view_h);

        // TODO : 이부분 렌더러로 이동
        glViewport(0, 0, 512, 512);
        ImVec4 clear_color = ImVec4(0.03f, 0.30f, 0.70f, 1.00f);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (Camera* camera : cameras){
            camera->calculate_view();
        }
        // 여기까지
        renderer.render(models);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui::Begin(Utils::format("View##%1%", id).c_str(), 0, window_flags);
        view_w = ImGui::GetWindowSize().x;
        view_h = ImGui::GetWindowSize().y;
        renderer.viewport_size.x = view_w;
        renderer.viewport_size.y = view_h;
        ImVec2 sss = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y);
        ImGui::GetWindowDrawList()->
            AddImage((void*)f_tex, ImGui::GetWindowPos(), sss, ImVec2(0, 0), ImVec2(1, 1));
        render_popup_menu();
        ImGui::End();
    }

    if (gui_hierarchy){
        ImGui::Begin(Utils::format("Hierarchy##%1%", id).c_str(), 0, window_flags);
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Cube")){// TODO : 오브젝트 트리 적용
            if (ImGui::TreeNode((void*)(intptr_t)0, "Sphere %d", 0)){
                ImGui::TreePop();
            }
            if (ImGui::TreeNode((void*)(intptr_t)0, "Sphere %d", 1)){
                if (ImGui::TreeNode((void*)(intptr_t)0, "Cube %d", 2)){
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode((void*)(intptr_t)0, "Cube %d", 3)){
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        ImGui::End();
    }

    if (gui_inspector){
        ImGui::Begin(Utils::format("Inspector##%1%", id).c_str(), 0, window_flags);
        ImGui::Text(Utils::format("Text%1%", id).c_str());
        ImGui::End();
    }

    if (gui_logs){
        ImGui::Begin(Utils::format("Logs##%1%", id).c_str(), 0, window_flags);
        ImGui::Text(Utils::format("Text%1%", id).c_str());
        ImGui::End();
    }
}

WorkSpace* WorkSpace::create_new(GUI* parent_){
    return new WorkSpace(parent_, "test"); // TEST
}

GLuint WorkSpace::init_fbo(int w_, int _h){ // TODO : renderer로 이동
    static unsigned int fbo = 0;
    static GLuint f_tex;
    if (fbo == 0){
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        //GLuint  f_tex = CreateTexture(512, 512, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
        glGenTextures(1, &f_tex);
        glBindTexture(GL_TEXTURE_2D, f_tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_, _h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glBindTexture(GL_TEXTURE_2D, 0); // 

        glBindTexture(GL_TEXTURE_2D, f_tex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, f_tex, 0);

        GLuint depthrenderbuffer;
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w_, _h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    return f_tex;
}


// ===== Engine GUI ===== //

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
    //if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
    //    glfwSetWindowShouldClose(glfw_window, true);
    //}

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

    // fullscreen 아닐때 - 나중에 편집
    //dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    //if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode){
    //	window_flags |= ImGuiWindowFlags_NoBackground;
    //}

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
            workspaces.push_back(WorkSpace::create_new(this));
        }

        for (WorkSpace* workspace : workspaces){
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

                        ImGui::DockBuilderDockWindow(Utils::format("View##%1%", workspace->id).c_str(), dock_main_id);
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
                    active_workspace = nullptr; // 참조를 막아둔다
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

        /*if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)){ dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
        if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, true)){ dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
        */
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
            if (ImGui::MenuItem("Sphere")){}
            if (ImGui::MenuItem("Cylinder")){}
            if (ImGui::MenuItem("Cone")){}
            if (ImGui::MenuItem("Tetrahedron")){}
            if (ImGui::MenuItem("Pyramid")){}
            if (ImGui::MenuItem("Torus")){}
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Load Model")){}
        ImGui::Separator();
        if (ImGui::MenuItem("Set To Selection Group")){
            // csg연산 한거 나눠서 선택 안되게
        }
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
        shortcuts.push_back(Shortcut("camera move right", false, false, false, GLFW_KEY_D, [=](){
            vec3 Dir = active_workspace->mainCamera->get_transform()->get_rightDir() * Camera::speed_move_default;
            active_workspace->mainCamera->get_transform()->add_localPosition(Dir);
        }));
        shortcuts.push_back(Shortcut("camera move left", false, false, false, GLFW_KEY_A, [=](){
            vec3 Dir = active_workspace->mainCamera->get_transform()->get_rightDir() * -Camera::speed_move_default;
            active_workspace->mainCamera->get_transform()->add_localPosition(Dir);
        }));
        shortcuts.push_back(Shortcut("camera move up", false, false, false, GLFW_KEY_Q, [=](){
            active_workspace->mainCamera->get_transform()->add_localPosition({0.f,Camera::speed_move_default, 0});
        }));
        shortcuts.push_back(Shortcut("camera move down", false, false, false, GLFW_KEY_E, [=](){
            active_workspace->mainCamera->get_transform()->add_localPosition({0.f,-Camera::speed_move_default, 0});
        }));
        shortcuts.push_back(Shortcut("camera move forward", false, false, false, GLFW_KEY_W, [=](){
            vec3 Dir = active_workspace->mainCamera->get_transform()->get_frontDir() * Camera::speed_move_default;
            active_workspace->mainCamera->get_transform()->add_localPosition(Dir);
        }));
        shortcuts.push_back(Shortcut("camera move back", false, false, false, GLFW_KEY_S, [=](){
            vec3 Dir = active_workspace->mainCamera->get_transform()->get_frontDir() * -Camera::speed_move_default;
            active_workspace->mainCamera->get_transform()->add_localPosition(Dir);
        }));

        shortcuts.push_back(Shortcut("camera move fast right", true, false, false, GLFW_KEY_D, [=](){
            vec3 Dir = active_workspace->mainCamera->get_transform()->get_rightDir() * Camera::speed_move_fast;
            active_workspace->mainCamera->get_transform()->add_localPosition(Dir);
        }));
        shortcuts.push_back(Shortcut("camera move fast left", true, false, false, GLFW_KEY_A, [=](){
            vec3 Dir = active_workspace->mainCamera->get_transform()->get_rightDir() * -Camera::speed_move_fast;
            active_workspace->mainCamera->get_transform()->add_localPosition(Dir);
        }));
        shortcuts.push_back(Shortcut("camera move fast up", true, false, false, GLFW_KEY_Q, [=](){
            active_workspace->mainCamera->get_transform()->add_localPosition({0.f,Camera::speed_move_fast, 0});
        }));
        shortcuts.push_back(Shortcut("camera move fast down", true, false, false, GLFW_KEY_E, [=](){
            active_workspace->mainCamera->get_transform()->add_localPosition({0.f,-Camera::speed_move_fast, 0});
        }));
        shortcuts.push_back(Shortcut("camera move fast forward", true, false, false, GLFW_KEY_W, [=](){
            vec3 Dir = active_workspace->mainCamera->get_transform()->get_frontDir() * Camera::speed_move_fast;
            active_workspace->mainCamera->get_transform()->add_localPosition(Dir);
        }));
        shortcuts.push_back(Shortcut("camera move fast back", true, false, false, GLFW_KEY_S, [=](){
            vec3 Dir = active_workspace->mainCamera->get_transform()->get_frontDir() * -Camera::speed_move_fast;
            active_workspace->mainCamera->get_transform()->add_localPosition(Dir);
        }));

        shortcuts.push_back(Shortcut("camera rotate up", false, false, false, GLFW_KEY_UP, [=](){
            active_workspace->mainCamera->get_transform()->add_localRotation({Camera::speed_rotate_default, 0.0f, 0.0f});
        }));
        shortcuts.push_back(Shortcut("camera rotate down", false, false, false, GLFW_KEY_DOWN, [=](){
            active_workspace->mainCamera->get_transform()->add_localRotation({-Camera::speed_rotate_default, 0.0f, 0.0f});
        }));
        shortcuts.push_back(Shortcut("camera rotate left", false, false, false, GLFW_KEY_LEFT, [=](){
            active_workspace->mainCamera->get_transform()->add_localRotation({0.0f, -Camera::speed_rotate_default, 0.0f});
        }));
        shortcuts.push_back(Shortcut("camera rotate right", false, false, false, GLFW_KEY_RIGHT, [=](){
            active_workspace->mainCamera->get_transform()->add_localRotation({0.0f, Camera::speed_rotate_default, 0.0f});
        }));
    }
}