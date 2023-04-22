#pragma once

#include <string>
#include <list>
#include <glm/glm.hpp>


class Utils{
    static double time_prev_frame; //...
public:
    const static float Epsilon;

    static std::string format(const char* format_str, int num);
    static std::string format(const char* format_str, const std::list<int> nums);

   
    //====== Time ======
    //static double time(); // 시계 (절대)
    static float time_acc(); // 프로그램 시작부터 누적
    static float time_delta(); // 프레임 사이 간격
    static void time_update(); // TODO : 메인루프 에서 호출
private:
    static float last_frameTime;
    static float deltaTime;


public:
    static glm::vec3 get_vecFromPitchYaw(float pitch_, float yaw_); //pitch : xAxis , yaw : yAxis

};
