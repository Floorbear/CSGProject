#include "Leaked_Pointers.h"

std::list<void*> Leaked_Pointers::pointers;

void Leaked_Pointers::add(void* ptr){
    pointers.push_back(ptr);
}

void Leaked_Pointers::dispose(){
    for(void* ptr : pointers){
        delete ptr;
    }
}