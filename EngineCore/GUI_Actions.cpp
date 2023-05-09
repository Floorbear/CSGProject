#include "GUI_Actions.h"
#include "Utils.h"
#include "Model.h"
#include "WorkSpace.h"

WorkSpace_Actions::WorkSpace_Actions(WorkSpace* workspace_) : workspace(workspace_){
}

void WorkSpace_Actions::delete_selected(){
    if (!workspace->selected_models.empty()){
        MultiTransactionTask task_multi = MultiTransactionTask(Utils::format("delete %1% objects", (int)workspace->selected_models.size()));
        for (Model* model : workspace->selected_models){
            task_multi.add_task(TransactionTask(std::string("delete ") + model->name, [this, model](){ // TODO : 구현 ㅠㅠㅠ
                //model->remove_self();
                //DeleteData = (model, index)
            }, [this](){
                //workspace->models.push_back(new Model(model_data)); 트리형태에서 인덱스까지 고려해야함
            }));
        }
        workspace->transaction_manager.add(task_multi);
    }
}

void WorkSpace_Actions::add_cube_new(){
    static int count = 0;
    Model* model = new Model(Utils::format("Cube%1%", count).c_str());
    model->set_new(Mesh::Cube);
    workspace->root_model->add_child(model);

    // TEST
    Transform* newMesh = model->get_transform();
    newMesh->set_position(vec3(-count * 2.5, 0, 0)); // TODO : I believe it places objects in the center of the active viewport
    ++count;

}

GUI_Actions::GUI_Actions(GUI* parent_) : parent(parent_){
}
