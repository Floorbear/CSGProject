#pragma once
#include "Component.h"

class WorkSpace;
class PointLight : public Component{
    WorkSpace* parent;
    vec3 position;

public:
    PointLight(WorkSpace* parent_);
    PointLight(WorkSpace* parent_, vec3 position_);
    ~PointLight();

    vec3 get_position();
    void set_position(vec3 position_);
};

