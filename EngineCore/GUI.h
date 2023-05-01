#pragma once

#include "GUI_Actions.h"
#include "Renderer.h"
#include "Task.h"

#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

using namespace glm;

class Shortcut{ // TODO : 공통 부모를 가진 두 클래스로 쪼개기
    enum class Type{
        Continuous, Discrete
    };
    Type type;
public:
    std::string name;
    bool ctrl;
    bool shift;
    bool alt;
    ImGuiKey discrete_key = ImGuiKey::ImGuiKey_None;
    int continuous_key = 0;
    std::function<void()> callback;

    Shortcut(const char* name_, bool ctrl_, bool shift_, bool alt_, ImGuiKey discrete_key_, std::function<void()> callback_);
    Shortcut(const char* name_, bool ctrl_, bool shift_, bool alt_, int glfw_continuous_key_, std::function<void()> callback_);
    void check_execute(GLFWwindow* glfw_window);
    std::string to_string();
};

class Camera;
class WorkSpace{
    static int id_counter;

    GUI* parent; // WorkSpace는 GUI에 종속
    WorkSpace_Actions actions;

    void render_view(Renderer* renderer);
    void render_hierarchy();
    void render_inspector();
    void render_logs();
    void render_popup_menu();

public:
    int id;
    std::string title;

    bool gui_initialized = false;
    bool gui_hierarchy = true;
    bool gui_inspector = true;
    bool gui_logs = true;
    bool gui_csgtree = false;
    std::list<Renderer*> renderers;
    Renderer* renderer_focused = nullptr;

    TransactionTaskManager transaction_manager;

    std::list<Model*> models;
    //std::list<Mesh*> selected_meshes;

    WorkSpace(GUI* parent_);
    WorkSpace(GUI* parent_, std::string title_);
    ~WorkSpace();

    Camera* get_main_camera();
    Model* find_model(std::string_view name);

    void render();

    static WorkSpace* create_new(GUI* parent_, const char* filename);
    void add_view_new();
};

class GUI{
    GUI_Actions actions;
    int frame_count;

    void init_glfw();
    void init_gui();
    void init_shortcut();
    void render_begin();
    void render_gui();
    void render_end();
    void render_menubar();

public:
    static int parameter_count;

    GLFWwindow* glfw_window;
    vec2 window_size;

    std::list<Shortcut> shortcuts;

    std::list<WorkSpace*> workspaces;
    WorkSpace* active_workspace;

    GUI();
    void init();
    void render();
    void update();
    void process_input();
    void dispose();
};

