#pragma once

#include "Renderer.h"
#include "Task.h"

#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>

#include <iostream>

using namespace glm;

class Shortcut{
public:
    std::string name;
    bool ctrl;
    bool alt;
    bool shift;
    ImGuiKey key;
    std::function<void()> callback;

    Shortcut(const char* name_, bool ctrl_, bool alt_, bool shift_, ImGuiKey key_, std::function<void()> callback_);
    bool is_pressed();
    std::string to_string();
};

class WorkSpace{
    static int id_counter;

    GUI* parent; // WorkSpace는 GUI에 종속

public:
    int id;
    std::string title;

    bool gui_initialized;
    bool gui_hierarchy;
    bool gui_inspector;
    bool gui_logs;
    bool gui_csgtree;
    Renderer renderer; // view 담당. TODO : 여러개 가능하게

    TransactionTaskManager transaction_manager;

    std::list<Model*> models;
    //std::list<Mesh*> selected_meshes;

    WorkSpace(GUI* parent_);
    WorkSpace(GUI* parent_, std::string title_);
    ~WorkSpace();

    Model* find_model(std::string_view name);

    void render();

    static WorkSpace* create_new(GUI* parent_);

    GLuint init_fbo(int w_, int _h);
};

class GUI{
    int frame_count;

    void init_glfw();
    void init_gui();
    void init_shortcut();
    void render_begin();
    void render_gui();
    void render_end();
    void render_menubar();

public:
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

