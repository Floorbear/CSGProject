#pragma once

#include <stdint.h>

struct SelectionPixelIdInfo{
    uint32_t model_id = 0;
    uint32_t mesh_id = 0;
    uint32_t primID = 0;

    SelectionPixelIdInfo();
    SelectionPixelIdInfo(uint32_t model_id_, uint32_t mesh_id_);
};

class Model;
class CSGNode;
struct SelectionPixelObjectInfo{
    Model* model = nullptr;
    CSGNode* mesh = nullptr;
    uint32_t primID = 0;

    SelectionPixelObjectInfo();
    SelectionPixelObjectInfo(Model* model_id_, CSGNode* mesh_id_);
    bool empty();
};

