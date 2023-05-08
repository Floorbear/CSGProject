#include "PointLight.h"
#include "WorkSpace.h"

PointLight::PointLight(WorkSpace* parent_) : PointLight(parent_, vec3(0, 0, 0)){
}

PointLight::PointLight(WorkSpace* parent_, vec3 position_) : Component("PointLight"), parent(parent_), position(position_){
    parameters.push_back(new Vec3Parameter("position", "x", "y", "z", [this](){
        return position;
    }, [this](vec3 value){
        position = value;
    }));
    parent->get_lights()->push_back(this);
}

PointLight::~PointLight(){
    parent->get_lights()->remove(this);
}

vec3 PointLight::get_position(){
    return position;
}

void PointLight::set_position(vec3 position_){
    position = position_;
}
