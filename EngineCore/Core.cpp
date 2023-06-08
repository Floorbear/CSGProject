#include "Core.h"
#include "Utils.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Leaked_Pointers.h"

#include <GLFW/glfw3.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")

//#define CGAL_EIGEN3_ENABLED

Core* Core::instance = nullptr;

Core::Core(){
    instance = this;
}

void Core::dispose(){
    gui.dispose();
    glfwTerminate();

    FrameBuffer::dispose();
    //Texture::dispose();
    for (auto i : Texture::all_textures)
    {
        delete i;
    }
    Leaked_Pointers::dispose();
}

Core* Core::get(){
    //assert(instance != nullptr && "Inst is nullptr"); // 뒤에 주석은 무엇?
    if (instance == nullptr){
        instance = new Core();
    }
    return instance;
}

void Core::start(){
    gui.init();
    while (!glfwWindowShouldClose(gui.glfw_window)){
        Utils::time_update();
        task_manager.execute_all();
        gui.render();
        gui.process_input();
        gui.update();
    }
    dispose();
}