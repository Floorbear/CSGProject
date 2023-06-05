#pragma once

#include "GUI_Actions.h"
#include "Task.h"

#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

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


class WorkSpace;
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
