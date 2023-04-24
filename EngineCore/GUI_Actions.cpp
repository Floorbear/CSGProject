#include "GUI_Actions.h"
#include "GUI.h"
#include "Utils.h"

WorkSpace_Actions::WorkSpace_Actions(WorkSpace* parent_) : parent(parent_){
}

void WorkSpace_Actions::add_cube_new(){
    static int count = 0;
    Model* model = new Model(Utils::format("Cube%1%", ++count).c_str());
    model->set_new(Mesh::Cube);
    parent->models.push_back(model);

    // TEST
    Transform* newMesh = model->test_get_main_transform();//->FindMesh("Cube1");
    newMesh->set_localPostition(vec3(-count * 2.5, 0, 0)); // TODO : 메쉬가 생성되는 위치는 마우스 위치
    printf("%f\n", newMesh->get_worldPosition().x);

}

GUI_Actions::GUI_Actions(GUI* parent_) : parent(parent_){
}