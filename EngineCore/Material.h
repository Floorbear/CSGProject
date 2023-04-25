#pragma once
#include "Component.h"

#include <glm/glm.hpp>

class Material : public Component{

public:
    Material();
    ~Material();
    vec3 objectColor = { 1.0f,1.0f,0.0f };
    float ambient = 0.1f;

};

