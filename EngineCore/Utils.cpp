#include "Utils.h"

#include <boost/format.hpp>

#include <GLFW/glfw3.h>

float Utils::last_frameTime = 0.0f;
float Utils::deltaTime = 0.0f;

template <typename T>
T Utils::get(std::list<T> _list, int _index){
    typename std::list<T>::iterator it = _list.begin();
    std::advance(it, _index);
    return *it;
}

template <typename T>
void insert(std::list<T> _list, T item, int _index){
    typename std::list<T>::iterator it = _list.begin();
    std::advance(it, _index);
    _list.insert(it, item);
}

std::string Utils::format(const char* format_str, int num){
    return (boost::format(format_str) % num).str();
}

std::string Utils::format(const char* format_str, const std::list<int> nums){
    boost::format format_str_temp = boost::format(format_str);
    for (int num : nums){
        format_str_temp = format_str_temp % num;
    }
    return format_str_temp.str();
}

float Utils::time_acc(){
    return static_cast<float>(glfwGetTime());
}

float Utils::time_delta(){
    return deltaTime;
}

void Utils::time_update(){
    float current_frameTime = (float)glfwGetTime();
    deltaTime = current_frameTime - last_frameTime;
    last_frameTime = current_frameTime;
}

glm::vec3 Utils::get_vecFromPitchYaw(float pitch_, float yaw_){
    glm::vec3 newVec;
    newVec.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
    newVec.y = sin(glm::radians(pitch_));
    newVec.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));

    return newVec;
}
