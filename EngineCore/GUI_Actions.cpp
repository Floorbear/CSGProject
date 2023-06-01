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
        task_multi->add_task(new TreeModifyTask("Delete Model", model->get_parent(), [=](){
            model->remove_self();
            return true;
        })); // TODO : undo에 선택 복구
    }
    workspace->selected_meshes.clear();
    workspace->selected_models.clear();
}

void WorkSpace_Actions::add_model_new(const Mesh& mesh){
    static int count = 0;
    Model* model = new Model(Utils::format("Model%1%", count++).c_str());
    model->set_csg_mesh_new(&mesh);
    model->get_transform()->set_position(vec3(count * 0.7, count * 0.7, count * 0.7)); // TODO : I believe it places objects in the center of the active viewport

    workspace->transaction_manager.add(new TreeModifyTask("Add " + mesh.get_name(), workspace->root_model, [this, model](){
        return workspace->root_model->add_child(model);
    }));
}

void WorkSpace_Actions::reorder_mesh_up(CSGNode* mesh){
    workspace->transaction_manager.add("Mesh Reorder", [=](){
        std::list<CSGNode*> siblings = mesh->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), mesh);
        if (it != siblings.end()){
            mesh->get_parent()->swap_child(*std::prev(it), *it);
        }
        return true;
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
        return true;
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
        return true;
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
        return true;
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
        return model->get_parent()->get_parent()->reparent_child(model, model->get_parent());
    }));
}

GUI_Actions::GUI_Actions(GUI* parent_) : parent(parent_){
}
