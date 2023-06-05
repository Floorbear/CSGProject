#pragma once

#include <stdint.h>

struct SelectionPixelInfo{
    const static uint32_t object_type_none;
    const static uint32_t object_type_object;
    const static uint32_t object_type_gizmo_x;
    const static uint32_t object_type_gizmo_y;
    const static uint32_t object_type_gizmo_z;
    const static uint32_t object_type_gizmo_dot;
};

struct SelectionPixelIdInfo{
    uint32_t model_id = 0;
    uint32_t mesh_id = 0;
    uint32_t object_type = SelectionPixelInfo::object_type_none;

    SelectionPixelIdInfo();
    SelectionPixelIdInfo(uint32_t object_type_);
    SelectionPixelIdInfo(uint32_t model_id_, uint32_t mesh_id_);
};

class Model;
class CSGNode;
struct SelectionPixelObjectInfo{
    Model* model = nullptr;
    CSGNode* mesh = nullptr;
    uint32_t object_type = SelectionPixelInfo::object_type_none;

    SelectionPixelObjectInfo();
    SelectionPixelObjectInfo(uint32_t object_type_);
    SelectionPixelObjectInfo(Model* model_id_, CSGNode* mesh_id_);
    bool empty();
};

