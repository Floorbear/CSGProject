#include "SelectionPixelInfo.h"

SelectionPixelIdInfo::SelectionPixelIdInfo(){
}

SelectionPixelIdInfo::SelectionPixelIdInfo(uint32_t model_id_, uint32_t mesh_id_) : model_id(model_id_), mesh_id(mesh_id_){
}


SelectionPixelObjectInfo::SelectionPixelObjectInfo(){
}

SelectionPixelObjectInfo::SelectionPixelObjectInfo(Model* model_id_, CSGNode* mesh_id_) : model(model_id_), mesh(mesh_id_){
}

bool SelectionPixelObjectInfo::empty(){
    return model == nullptr || mesh == nullptr;
}
