#include "GUI_Actions.h"
#include "Utils.h"
#include "Model.h"
#include "CSGNode.h"
#include "WorkSpace.h"

WorkSpace_Actions::WorkSpace_Actions(WorkSpace* workspace_) : workspace(workspace_){
}

void WorkSpace_Actions::delete_selected_models(){
    MultiTransactionTask* task_multi = new MultiTransactionTask(Utils::format("Delete %1% Model(s)", (int)workspace->selected_models.size()));
    for (Model* model : workspace->selected_models){
        task_multi->add_task(new TreeModifyTask("Delete Model", model->get_parent(), [this, model](){
            model->remove_self();
        }));
    }
    workspace->transaction_manager.add(task_multi);
}

void WorkSpace_Actions::add_cube_new(){
    static int count = 0;
    Model* model = new Model(Utils::format("Cube%1%", count).c_str());
    model->set_new(Mesh::Cube);

    // TEST
    Transform* newMesh = model->get_transform();
    newMesh->set_position(vec3(-count * 2.5, 0, 0)); // TODO : I believe it places objects in the center of the active viewport
    ++count;
    workspace->transaction_manager.add(new TreeModifyTask("Add Cube", workspace->root_model, [this, model](){
        workspace->root_model->add_child(model);
    }));
}

void WorkSpace_Actions::reorder_mesh_up(CSGNode* mesh){
    workspace->transaction_manager.add("Mesh Reorder", [=](){
        std::list<CSGNode*> siblings = mesh->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), mesh);
        if (it != siblings.end()){
            mesh->get_parent()->swap_child(*std::prev(it), *it);
        }
    }, [=](){
        std::list<CSGNode*> siblings = mesh->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), mesh);
        if (it != siblings.end()){
            mesh->get_parent()->swap_child(*it, *std::next(it));
        }
    });
}

void WorkSpace_Actions::reorder_mesh_down(CSGNode* mesh){
    workspace->transaction_manager.add("Mesh Reorder", [=](){
        std::list<CSGNode*> siblings = mesh->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), mesh);
        if (it != siblings.end()){
            mesh->get_parent()->swap_child(*it, *std::next(it));
        }
    }, [=](){
        std::list<CSGNode*> siblings = mesh->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), mesh);
        if (it != siblings.end()){
            mesh->get_parent()->swap_child(*std::prev(it), *it);
        }
    });
}

void WorkSpace_Actions::reorder_model_up(Model* model){
    workspace->transaction_manager.add("Model Reorder", [=](){
        std::list<Model*> siblings = model->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), model);
        if (it != siblings.end()){
            model->get_parent()->swap_child(*std::prev(it), *it);
        }
    }, [=](){
        std::list<Model*> siblings = model->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), model);
        if (it != siblings.end()){
            model->get_parent()->swap_child(*it, *std::next(it));
        }
    });
}

void WorkSpace_Actions::reorder_model_down(Model* model){
    workspace->transaction_manager.add("Model Reorder", [=](){
        std::list<Model*> siblings = model->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), model);
        if (it != siblings.end()){
            model->get_parent()->swap_child(*it, *std::next(it));
        }
    }, [=](){
        std::list<Model*> siblings = model->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), model);
        if (it != siblings.end()){
            model->get_parent()->swap_child(*std::prev(it), *it);
        }
    });
}

void WorkSpace_Actions::move_model_to_parent(Model* model){
    workspace->transaction_manager.add(new TreeModifyTask("Model Tree Edit", model->get_parent()->get_parent(), [=](){
        model->get_parent()->get_parent()->set_child(model, model->get_parent());
    }));
}

GUI_Actions::GUI_Actions(GUI* parent_) : parent(parent_){
}
