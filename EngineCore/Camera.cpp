#include "Camera.h"

#include "Utils.h"

using namespace glm;

float Camera::speed_move_default = 3.f;
float Camera::speed_rotate_default = 35.f;
float Camera::speed_move_fast = 6.0f;

Camera::Camera(float width_, float height_, float fov_):
    width(width_),
    height(height_),
    fov(fov_),
    parent(nullptr)
{
    view = mat4(1.0f);
    projection = mat4(1.0f);
}

Camera::~Camera()
{
}

void Camera::calculate_view()
{
    mat4 newView = mat4(1.0f);
    newView = glm::translate(newView, vec3(0.0, 0.0, 0.f));

    mat4 newProjection = mat4(1.0f);
    float fov = 45.f;
    projection = glm::perspective(glm::radians(fov), width/ height, near, far);
    projection[1][1] *= -1;

    vec3 cameraPos = transform.get_worldPosition();
    //cameraPos.x = -cameraPos.x;6

    //cameraPos.y = cameraPos.z;
    vec3 cameraRot = transform.get_worldRotation();
    cameraRot.x = clamp(cameraRot.x, -89.9f, 89.9f);

    vec3 cameraDir = normalize(Utils::get_vecFromPitchYaw(cameraRot.x, cameraRot.y));
  


    vec3 up = vec3(0, 1.f, 0);
    vec3 cameraRight = normalize(cross(up, cameraDir));

    vec3 cameraUp = cross(cameraDir, cameraRight);
    view = lookAt(cameraPos, cameraPos + cameraDir, cameraUp);
    //view[3][0] = -view[3][0];
    //view = rotate(view, radians(-90.f), vec3(0, 1, 0));
    //view = rotate(view, radians(180.f), vec3(0, 0, 1));

    //save
}
