#include "Utils.h"

#include <boost/format.hpp>

#include <GLFW/glfw3.h>

const float Utils::Epsilon = 0.0001f; // TODO : 지수표현으로 바꾸세요

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

double Utils::time_acc(){
    return glfwGetTime();
}
