#pragma once

#include "GUI.h"
#include "Task.h"

#include <glm/glm.hpp>

using namespace glm;

class Core{
    static Core* instance;

    Core();
    void dispose();

public:
    GUI gui;
    TaskManager task_manager;

    static Core* get();

    void start();
};





/*class Scene{

Camera camera;
std::list<Model> models;


void render(){
camera.multiplyMatrix();
for each (model in models)	{
model.render();
}
};

/*class CgalMeshWrapper{
CgalMesh mesh;
void render(){
mesh.getVertexArray();
drawVertexArray(dfdfkfsd);
}
}//

class Model{
Transform transform;
//CgalMEshWrapper mesh;
CgalMesh mesh;
void render{
//mesh.render();
Util.drawCgalMesh(mesh);
}


}
class Camera {
Transform transform;

void multiplyMatrix(){
}

}


void CgalMeshDraw(CGAL::Mesh mesh){
dafsafsdfasfas
drawVertexBuffer(...);
}*/