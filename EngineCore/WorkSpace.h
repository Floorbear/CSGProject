#pragma once
#include "GUI_Actions.h"
#include "Task.h"
#include "Gizmo.h"

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
    int dockspace_id = -1;

    std::list<PointLight*> lights;

    Renderer* main_renderer = nullptr;

    void render_view(Renderer* renderer);
    void render_hierarchy();
    void render_inspector();
    void render_logs();
    void render_popup_menu_view(Renderer* renderer);
    void render_popup_menu_inspector();

public:
    enum class SelectionMode{
        Model, Mesh
    };

    int id;
    std::string title;

    bool gui_hierarchy = true;
    bool gui_inspector = true;
    bool gui_logs = true;
    bool gui_csgtree = false;

    TransactionTaskManager transaction_manager;
    std::list<std::string> logs;

    std::list<Renderer*> renderers;
    Renderer* renderer_focused = nullptr;
    Model* root_model = nullptr;

    SelectionMode selection_mode = SelectionMode::Model;
    GizmoMode gizmo_mode = GizmoMode::Translate;
    std::list<Model*> selected_models;
    std::list<CSGNode*> selected_meshes;

    vec2 mouse_pos_left_press_raw = vec2(); // ImGui::GetIO().MouseClickedPos[ImGuiMouseButton_Left] / TODO : 구조체로 변경
    vec2 mouse_pos_left_press_view = vec2();
    vec2 mouse_pos_left_current_raw = vec2();
    vec2 mouse_pos_left_current_view = vec2();
    bool is_background_pressed = false;
    bool is_gizmo_pressed = false; // TODO : pressed_object_type으로 변경
    
    WorkSpace(GUI* parent_);
    WorkSpace(GUI* parent_, std::string title_);
    ~WorkSpace();

    Camera* get_main_camera();
    Model* find_model(std::string_view name);
    std::list<PointLight*>* get_lights();

    void render();
    void check_init_dockspace(int dockspace_id_, const ImGuiViewport* viewport);
    void process_input();

    void on_mouse_press_left();
    void on_mouse_drag_left();
    std::function<void(Camera*,vec2,vec2)> dragDelegate;
    void on_mouse_release_left();
    vec2 prevPos;

    bool check_model_selected_exact(Model* model);
    bool check_model_selected(Model* model);
    bool check_mesh_selected_exact(CSGNode* mesh);
    bool check_mesh_selected(CSGNode* mesh);

    static WorkSpace* create_new(GUI* parent_, const char* filename);
    void add_view_new();
};
