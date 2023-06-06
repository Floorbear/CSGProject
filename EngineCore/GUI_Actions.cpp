#include "GUI_Actions.h"
#include "Utils.h"
#include "Model.h"
#include "CSGNode.h"
#include "WorkSpace.h"

WorkSpace_Actions::WorkSpace_Actions(WorkSpace* workspace_) : workspace(workspace_){
}

void WorkSpace_Actions::delete_selected_meshes(){
    MultiTransactionTask* task_multi = new MultiTransactionTask(Utils::format("Delete %1% Model(s)", (int)workspace->selected_meshes.size()));
    for (CSGNode* selected_mesh : workspace->selected_meshes){
        task_multi->add_task(new TreeEntityModifyTask<CSGNode>("Delete Mesh", [=](){
            selected_mesh->remove_self_subtree();
            return true;
        }, selected_mesh->model->get_csg_mesh(), [=](CSGNode* root){
            selected_mesh->model->set_csg_mesh(root, true);
        })); // TODO : undo에 선택 복구 (task가 selected_... 들고있어야함)
    }
    workspace->transaction_manager.add(task_multi);
    workspace->selected_meshes.clear();
    workspace->selected_models.clear();
}

void WorkSpace_Actions::delete_selected_models(){
    MultiTransactionTask* task_multi = new MultiTransactionTask(Utils::format("Delete %1% Model(s)", (int)workspace->selected_models.size()));
    for (Model* selected_model : workspace->selected_models){
        task_multi->add_task(new TreeEntityModifyTask<Model>("Delete Model", selected_model->get_parent(), [=](){
            selected_model->remove_self();
            return true;
        })); // TODO : undo에 선택 복구 (task가 selected_... 들고있어야함)
    }
    workspace->transaction_manager.add(task_multi);
    workspace->selected_meshes.clear();
    workspace->selected_models.clear();
}

void WorkSpace_Actions::add_mesh_new(CSGNode* parent_node, const Mesh& mesh, const Transform& transform){
    CSGNode* node = new CSGNode(mesh);
    node->get_transform()->set(transform);

    workspace->transaction_manager.add(new TreeEntityModifyTask<CSGNode>("Add Mesh " + mesh.get_name(), parent_node, [=, this](){
        return parent_node->add_child(node);
    }));
}

void WorkSpace_Actions::add_model_new(const Mesh& mesh, const Transform& transform){
    static int count = 0;
    Model* model = new Model(Utils::format("Model%1%", count).c_str());
    model->set_csg_mesh_new(&mesh);
    model->get_transform()->set_position(vec3(count * 0.7, count * 0.7, count * 0.7)); // TODO : I believe it places objects in the center of the active viewport
    count++;

    workspace->transaction_manager.add(new TreeEntityModifyTask<Model>("Add Model " + mesh.get_name(), workspace->root_model, [=, this](){
        if (workspace->root_model->add_child(model)){
            model->get_csg_mesh()->get_transform()->set(transform);
        }
        return true;
    }));
}

void WorkSpace_Actions::reorder_mesh_up(CSGNode* mesh){
    workspace->transaction_manager.add("Reorder Mesh", [=](){
        std::list<CSGNode*> siblings = mesh->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), mesh);
        if (it != siblings.end()){
            mesh->get_parent()->swap_child(*std::prev(it), *it);
        }
        mesh->get_parent()->mark_edited();
        return true;
    }, [=](){
        std::list<CSGNode*> siblings = mesh->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), mesh);
        if (it != siblings.end()){
            mesh->get_parent()->swap_child(*it, *std::next(it));
        }
        mesh->get_parent()->mark_edited();
    });
}

void WorkSpace_Actions::reorder_mesh_down(CSGNode* mesh){
    workspace->transaction_manager.add("Reorder Mesh", [=](){
        std::list<CSGNode*> siblings = mesh->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), mesh);
        if (it != siblings.end()){
            mesh->get_parent()->swap_child(*it, *std::next(it));
        }
        mesh->get_parent()->mark_edited();
        return true;
    }, [=](){
        std::list<CSGNode*> siblings = mesh->get_parent()->get_children();
        auto it = std::find(siblings.begin(), siblings.end(), mesh);
        if (it != siblings.end()){
            mesh->get_parent()->swap_child(*std::prev(it), *it);
        }
        mesh->get_parent()->mark_edited();
    });
}

void WorkSpace_Actions::reorder_model_up(Model* model){
    workspace->transaction_manager.add("Reorder Model", [=](){
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
    workspace->transaction_manager.add("Reorder Model", [=](){
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
    workspace->transaction_manager.add(new TreeEntityModifyTask<Model>("Move Model To Parent", model->get_parent()->get_parent(), [=](){
        Model* parent = model->get_parent();
        Model* ancestor = parent->get_parent();
        return ancestor->reparent_child(model, parent);
    }));
}

void WorkSpace_Actions::create_boolean_opertation_of_selected_models(CSGNode::Type type){
    Model* new_model = new Model("Union Model");
    new_model->get_transform()->set(workspace->selected_models.front()->get_transform()->get_value());
    new_model->set_csg_mesh(new CSGNode(type));

    TransactionTask* task = new TreeEntityModifyTask<Model>(std::string("Create ") + CSGNode::type_string_values[(int)type] + " Of Selected Models", workspace->root_model, [=, this](){
        return workspace->root_model->add_child(new_model);
    });

    for (Model* selected_model : workspace->selected_models){
        task = task->link(new TreeEntityModifyTask<CSGNode>("Reparent Mesh", [=](){
            return new_model->get_csg_mesh()->reparent_child(selected_model->get_csg_mesh());
        }, new_model->get_csg_mesh(),
            [=](CSGNode* root){new_model->set_csg_mesh(root, true);
        }, selected_model->get_csg_mesh(),
            [=](CSGNode* root){selected_model->set_csg_mesh(root, true); }));
    }

    std::list<Model*> selected_models_copy = workspace->selected_models;
    workspace->transaction_manager.add(task->link(new TreeEntityModifyTask<Model>("Delete Empty Models", workspace->root_model, [=](){
        for (Model* selected_model : selected_models_copy){
            selected_model->remove_self();
        }
        return true;
    })));

    workspace->selected_meshes.clear();
    workspace->selected_models.clear();
}

GUI_Actions::GUI_Actions(GUI* parent_) : parent(parent_){
}
