#pragma once
#include <list>

class Leaked_Pointers{
    static std::list<void*> pointers;
public:
    static void add(void* ptr);
    static void dispose();
};

