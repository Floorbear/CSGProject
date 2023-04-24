#pragma once

#include <list>

class Parameter{
public:
    Parameter();

    template<typename T> T get();
    template<typename T> void set(T value_);
    void render();
};

class Component{
    std::list<Parameter*> params;
};
