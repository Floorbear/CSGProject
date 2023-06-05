#pragma once
#include "Transform.h"

class WorkSpace;
class GUI;
class CSGNode;
class Model;
class Mesh;

class WorkSpace_Actions{
    WorkSpace* workspace;
public:
    WorkSpace_Actions(WorkSpace* workspace_);

    // 여기부턴 gui 기능 함수들

    void delete_selected_models();
    void add_model_new(const Mesh& mesh, const Transform& transform = Transform::identity);
    void reorder_mesh_up(CSGNode* mesh);
    void reorder_mesh_down(CSGNode* mesh);
    void reorder_model_up(Model* model);
    void reorder_model_down(Model* model);
    void move_model_to_parent(Model* model);


    //delete model
    //...
};

class GUI_Actions{
    GUI* parent;
public:
    GUI_Actions(GUI* parent_);
};