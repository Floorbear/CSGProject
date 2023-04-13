#pragma once

#include <string>
#include <list>

class Utils{
    static double time_prev_frame; //...
public:
    const static float Epsilon;

    static std::string format(const char* format_str, int num);
    static std::string format(const char* format_str, const std::list<int> nums);

    static double time(); // 시계 (절대)
    static double time_acc(); // 프로그램 시작부터 누적
    static double time_delta(); // 프레임 사이 간격
    static double time_update(); // TODO : 메인루프 에서 호출

};
