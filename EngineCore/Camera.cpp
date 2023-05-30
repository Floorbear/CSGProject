#include "Camera.h"
#include "Utils.h"

using namespace glm;

float Camera::speed_move_default = 7.f;
float Camera::speed_rotate_default = 55.f;
float Camera::speed_move_fast_multiplier = 2.0f;

Camera::Camera(float width_, float height_, float fov_) : Component("Camera"),
    width(width_),
    height(height_),
    fov(fov_){
    view = mat4(1.0f);
    projection = mat4(1.0f);

    components.push_back(&transform);
    components.push_back(this);

    parameters.push_back(new FloatParameter("Near", [this](){
        return near;
    }, [this](float value){
        near = value;
    }));
    parameters.push_back(new FloatParameter("Far", [this](){
        return far;
    }, [this](float value){
        far = value;
    }));
    // TODO : add separator
    parameters.push_back(new FloatParameter("Keyboard Move Speed", [this](){
        return speed_move_default;
    }, [this](float value){
        speed_move_default = value;
    }));
    parameters.push_back(new FloatParameter("Keyboard Rotate Speed", [this](){
        return speed_rotate_default;
    }, [this](float value){
        speed_rotate_default = value;
    }));
}

Camera::~Camera()
{
}

void Camera::calculate_view(){
    transform.calculate_matrix();

    mat4 newView = mat4(1.0f);
    newView = glm::translate(newView, vec3(0.0, 0.0, 0.f));

    mat4 newProjection = mat4(1.0f);
    float fov = 45.f;
    projection = glm::perspective(glm::radians(fov), width/ height, near, far);
    projection[1][1] *= -1;

    vec3 cameraPos = transform.get_world_position();
    vec3 cameraRot = transform.get_world_rotation();
    cameraRot.x = clamp(cameraRot.x, -89.9f, 89.9f);

    vec3 cameraDir = normalize(Utils::get_vecFromPitchYaw(cameraRot.x, cameraRot.y));

    vec3 up = vec3(0, 1.f, 0);
    vec3 cameraRight = normalize(cross(up, cameraDir));

    vec3 cameraUp = cross(cameraDir, cameraRight);
    view = lookAt(cameraPos, cameraPos + cameraDir, cameraUp);
}


vec2 Camera::worldPosition_to_screenPosition(const vec3& _worldPosition) 
{

    // Calculate the screen-space position
    vec4 screenPosition = get_projection() * get_view() * glm::vec4(_worldPosition, 1.0f);

    // Normalize the coordinates
    glm::vec3 normalizedScreenPosition = glm::vec3(
        screenPosition.x / screenPosition.w,
        screenPosition.y / screenPosition.w,
        screenPosition.z / screenPosition.w
    );

    // Convert to screen-space position
    glm::vec2 screenSpacePosition = glm::vec2(
        (normalizedScreenPosition.x + 1.0f) / 2.0f * width,
        (1.0f - normalizedScreenPosition.y) / 2.0f * height
    );
   
    return screenSpacePosition;
}


void Camera::resize(float viewport_width, float viewport_height){
    width = viewport_width;
    height = viewport_height;
}