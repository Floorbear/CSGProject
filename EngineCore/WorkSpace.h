#pragma once
#include "GUI_Actions.h"
#include "Task.h"

#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>

#include <list>

using namespace glm;

class CSGNode;
class Model;
class Renderer;
class Camera;
class PointLight;
class WorkSpace {
    static int id_counter;

    GUI* parent; // WorkSpace는 GUI에 종속
    WorkSpace_Actions actions;

    std::list<PointLight*> lights;

    void render_view(Renderer* renderer);
    void render_hierarchy();
    void render_inspector();
    void render_logs();
    void render_popup_menu();

public:
    enum class SelectionMode{
        Model_Selection, Mesh_Selection
    };

    int id;
    std::string title;

    bool gui_initialized = false;
    bool gui_hierarchy = true;
    bool gui_inspector = true;
    bool gui_logs = true;
    bool gui_csgtree = false;

    TransactionTaskManager transaction_manager;

    std::list<Renderer*> renderers;
    std::list<Model*> models;

    SelectionMode selection_mode = SelectionMode::Model_Selection;
    std::list<Model*> selected_models;
    std::list<CSGNode*> selected_meshes;

    vec2 mouse_pos_left_press_raw;
    vec2 mouse_pos_left_press_view;
    vec2 mouse_pos_left_current_raw;
    vec2 mouse_pos_left_current_view;
    Renderer* renderer_focused = nullptr;
    
    WorkSpace(GUI* parent_);
    WorkSpace(GUI* parent_, std::string title_);
    ~WorkSpace();

    Camera* get_main_camera();
    Model* find_model(std::string_view name);
    std::list<PointLight*>* get_lights();

    void render();
    void process_input();

    void on_mouse_press_left();
    void on_mouse_drag_left();
    void on_mouse_release_left();

    static WorkSpace* create_new(GUI* parent_, const char* filename);

    void add_view_new();
};
