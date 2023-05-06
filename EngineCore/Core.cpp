#include "Core.h"
#include "Utils.h"
#include <GLFW/glfw3.h>
#include "Texture.h"
#include "FrameBuffer.h"

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")

//#define CGAL_EIGEN3_ENABLED

Core* Core::instance = nullptr;

Core::Core(){
    instance = this;
}

void Core::dispose(){
    //Texture::dispose();
    gui.dispose();
    glfwTerminate();
    //프레임버퍼
    for (auto i : FrameBuffer::all_frameBuffer)
    {
        delete i;
    }
    //텍스처
    for (auto i : Texture::all_textures)
    {
        delete i;
    }


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
        gui.process_input();
        gui.update();
        gui.render();
    }
    dispose();
}