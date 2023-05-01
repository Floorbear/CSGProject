#pragma once
#include "GUI_Actions.h"
#include "Task.h"

#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>



#include <iostream>

class Model;
class Renderer;
class Camera;
class WorkSpace {
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
