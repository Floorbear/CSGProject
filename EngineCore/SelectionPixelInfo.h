#pragma once
#include "Gizmo.h"

#include <stdint.h>

struct SelectionPixelInfo{
    const static uint32_t object_type_none;
    const static uint32_t object_type_object;
    const static uint32_t object_type_gizmo;
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
    GizmoAxis gizmo_axis = GizmoAxis::None;

    SelectionPixelObjectInfo();
    SelectionPixelObjectInfo(uint32_t object_type_);
    SelectionPixelObjectInfo(GizmoAxis gizmo_axis_);
    SelectionPixelObjectInfo(Model* model_id_, CSGNode* mesh_id_);
    bool empty();
};

