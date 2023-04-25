#include "Material.h"

Material::Material() : Component("Material"){

    /*parameters.push_back(new Vec3Parameter("position", "x", "y", "z", [this](){
        return get_position();
    }, [this](vec3 value){
        set_position(value);
    }));
    parameters.push_back(new Vec3Parameter("rotation", "x", "y", "z", [this](){
        return get_rotation();
    }, [this](vec3 value){
        set_rotation(value);
    }));
    parameters.push_back(new Vec3Parameter("scale", "x", "y", "z", [this](){
        return get_scale();
    }, [this](vec3 value){
        set_scale(value);
    }));*/
}

Material::~Material(){
}
