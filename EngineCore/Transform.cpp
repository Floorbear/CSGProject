#include "Transform.h"

Transform::Transform() : Transform(vec4(0.f, 0.f, 0.f, 1.f)){ // ??
}

Transform::Transform(const vec4& _Vec4) : Component("Transform"){
    vector = _Vec4;
    matrix = glm::mat4(1.0f);

    position = vec3(0.f, 0.f, 0.f);
    scale3d = vec3(1.f, 1.f, 1.f);

    parameters.push_back(new Vec3Parameter("position", "x", "y", "z", [this](){
        return get_position();
    }, [this](vec3 value){
        set_position(value);
    }));
    //TODO : 다른 파라미터
}

Transform::~Transform(){
}

vec3 Transform::get_position(){
    return position;
}

vec3 Transform::get_rotation(){
    return rotation;
}

vec3 Transform::get_scale(){
    return scale3d;
}

void Transform::set_position(const vec3& value){
    position = value;
    calculate_matrix();
}

void Transform::set_rotation(const vec3& value){
    rotation = value;
    calculate_matrix();
}

void Transform::set_scale(const vec3& value){
    scale3d = value;
    calculate_matrix();
}

void Transform::translate(const vec3& value){ // rotation 적용된 상태
    glm::vec3 forward_dir = Utils::get_vecFromPitchYaw(worldRotation.x, worldRotation.y);
    glm::vec3 up_dir = glm::vec3(0, 1, 0);
    add_position(get_right_dir() * value.x);
    add_position(get_up_dir() * value.y);
    add_position(get_forward_dir() * value.z);
    calculate_matrix();
}

void Transform::rotate(const vec3& value){
    rotation = rotation + value;
    calculate_matrix();
}

void Transform::scale(const vec3& value){
    scale3d = scale3d + value;
    scale3d = max(scale3d, vec3(epsilon<float>(), epsilon<float>(), epsilon<float>()));
    calculate_matrix();
}

void Transform::add_position(const vec3& value){
    position = position + value;
    calculate_matrix();
}


void Transform::calculate_matrix(){
    matrix = glm::mat4(1.0f);

    /*if (parent != nullptr)
    {
        //transform
        {
            glm::mat4 leftMat = glm::mat4(1.0f);
            glm::mat4 rightMat = glm::mat4(1.0f);
            leftMat = glm::translate(leftMat, position);
            rightMat = glm::translate(rightMat, parent->worldPosition); // ex)  Root : 2 , Root->children : 3 >> Root->children->childeren = 5 (Pos)
            leftMat = leftMat * rightMat;
            worldPosition = vec3(leftMat[3][0], leftMat[3][1], leftMat[3][2]); //mat[col][row] ??
        }
    }
    else // if this == Root >> local = world*/
    {
        worldPosition = position;
        worldRotation = rotation;
        worldScale = scale3d;
    }
    matrix = glm::translate(matrix, worldPosition);
    matrix = glm::rotate(matrix, glm::radians(worldRotation.x), glm::vec3(1.f, 0, 0.f));
    matrix = glm::rotate(matrix, glm::radians(worldRotation.y), glm::vec3(0.f, 1.f, 0.f));
    matrix = glm::rotate(matrix, glm::radians(worldRotation.z), glm::vec3(0.f, 0, 1.f));
    matrix = glm::scale(matrix, worldScale);
}

mat4* Transform::get_matrix(){
    return &matrix;
}

// https://stackoverflow.com/questions/31733811/local-variables-before-return-statements-does-it-matter
vec3 Transform::get_forward_dir(){ // TODO : local을 이용해서 구해야하는거 아님????????
    return glm::normalize(Utils::get_vecFromPitchYaw(rotation.x, rotation.y));
}

vec3 Transform::get_right_dir(){
    glm::vec3 forward_dir = Utils::get_vecFromPitchYaw(rotation.x, rotation.y);
    glm::vec3 up_dir = glm::vec3(0, 1, 0);

    return glm::normalize(-cross(forward_dir, up_dir));
}

vec3 Transform::get_up_dir(){
    return normalize(cross(get_right_dir(), get_forward_dir()));
}