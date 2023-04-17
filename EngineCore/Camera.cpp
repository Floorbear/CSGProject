#include "Camera.h"

#include "Utils.h"

using namespace glm;

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
    projection = glm::perspective(glm::radians(fov), width/ height, 0.1f, 100.f);

    vec3 cameraPos = -transform.get_worldPosition();

    vec3 cameraRot = transform.get_worldRotation();
    vec3 cameraDir = normalize(Utils::get_vecFromPitchYaw(-cameraRot.x, -cameraRot.y));

    vec3 up = vec3(0, 1.f, 0);
    vec3 cameraRight = normalize(cross(up, cameraDir));

    vec3 cameraUp = cross(cameraDir, cameraRight);

    view = lookAt(cameraPos, cameraPos + cameraDir, cameraUp);
}
