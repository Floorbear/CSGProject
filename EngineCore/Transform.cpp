#include "Transform.h"

Transform::Transform()
{
	vector = vec4(0.f, 0.f, 0.f, 1.f);
	matrix = glm::mat4(1.0f);

	position = vec3(0.f,0.f,0.f);
	scale3d = vec3(1.f, 1.f, 1.f);
}

Transform::Transform(vec4& _Vec4)
{
	vector = _Vec4;
	matrix = glm::mat4(1.0f);

	position = vec3(0.f, 0.f, 0.f);
	scale3d = vec3(1.f, 1.f, 1.f);
}

Transform::~Transform(){
}

void Transform::set_position(const vec3& value)
{
	position = value;
	calculate_matrix();
}

void Transform::set_rotation(const vec3& value)
{
	rotation = value;
	calculate_matrix();
}

void Transform::set_scale(const vec3& value)
{
	scale3d = value;
	calculate_matrix();
}

void Transform::translate(const vec3& value){
	position = position + vec3(vec4(value, 1.0) * matrix);
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


void Transform::calculate_matrix()
{
	matrix = glm::mat4(1.0f);

	/*if (parent != nullptr) 이러면 안됨. 한번에 한 행렬만 적용되는게 아니기때문!
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
	matrix = glm::rotate(matrix,glm::radians(worldRotation.z),glm::vec3(0.f,0,1.f));
	matrix = glm::scale(matrix, worldScale);
}

mat4* Transform::get_matrix(){
	return &matrix;
}

// https://stackoverflow.com/questions/31733811/local-variables-before-return-statements-does-it-matter
vec3 Transform::get_forward_dir(){ // TODO : local을 이용해서 구해야하는거 아님????????
	return glm::normalize(Utils::get_vecFromPitchYaw(worldRotation.x, worldRotation.y));
}

vec3 Transform::get_right_dir(){
	glm::vec3 forward_dir = Utils::get_vecFromPitchYaw(worldRotation.x, worldRotation.y);
	glm::vec3 up_dir = glm::vec3(0, 1, 0);

	return glm::normalize(-cross(forward_dir, up_dir));
}