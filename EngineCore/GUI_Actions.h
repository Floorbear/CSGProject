#pragma once

class WorkSpace;
class GUI;

class WorkSpace_Actions{
    WorkSpace* parent;
public:
    WorkSpace_Actions(WorkSpace* parent_);

    // 여기부턴 gui 기능 함수들
    void add_cube_new();


    //delete model
    //reorder model
    //...
};

class GUI_Actions{
    GUI* parent;
public:
    GUI_Actions(GUI* parent_);
};