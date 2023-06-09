#include "SelectionPixelInfo.h"

const uint32_t SelectionPixelInfo::object_type_none = 0;
const uint32_t SelectionPixelInfo::object_type_object = 1;
const uint32_t SelectionPixelInfo::object_type_gizmo = 2;

SelectionPixelIdInfo::SelectionPixelIdInfo(){
}

SelectionPixelIdInfo::SelectionPixelIdInfo(uint32_t object_type_) : object_type(object_type_){
}

SelectionPixelIdInfo::SelectionPixelIdInfo(uint32_t model_id_, uint32_t mesh_id_) : model_id(model_id_), mesh_id(mesh_id_), object_type(SelectionPixelInfo::object_type_object){
}


SelectionPixelObjectInfo::SelectionPixelObjectInfo(){
}

SelectionPixelObjectInfo::SelectionPixelObjectInfo(uint32_t object_type_) : object_type(object_type_){
}

SelectionPixelObjectInfo::SelectionPixelObjectInfo(GizmoAxis gizmo_axis_) : gizmo_axis(gizmo_axis_), object_type(SelectionPixelInfo::object_type_gizmo){
}

SelectionPixelObjectInfo::SelectionPixelObjectInfo(Model* model_id_, CSGNode* mesh_id_) : model(model_id_), mesh(mesh_id_), object_type(SelectionPixelInfo::object_type_object){
    if (model == nullptr || mesh == nullptr){
        object_type = SelectionPixelInfo::object_type_none; // set empty
    }
}

bool SelectionPixelObjectInfo::empty(){
    return object_type == SelectionPixelInfo::object_type_none;
}
