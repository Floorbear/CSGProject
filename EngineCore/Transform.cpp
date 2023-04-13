#include "Transform.h"

Transform::Transform()
{
	vector = vec4(0.f, 0.f, 0.f, 1.f);
	transformMat = glm::mat4(1.0f);

	position = vec3(0.f,0.f,0.f);
	localScale = vec3(1.f, 1.f, 1.f);
}

Transform::Transform(vec4& _Vec4)
{
	vector = _Vec4;
	transformMat = glm::mat4(1.0f);

	position = vec3(0.f, 0.f, 0.f);
	localScale = vec3(1.f, 1.f, 1.f);
}

Transform::~Transform()
{
}

void Transform::AddLocalPosition(const vec3& _Position)
{
	position = position + _Position;
	CalculateWorld();
}

void Transform::AddLocalRotation(const vec3& _Rotation)
{
	localRotation = localRotation + _Rotation;
	CalculateWorld();
}

void Transform::AddLocalScale(const vec3& _Scale)
{
	localScale = localScale + _Scale;
	CalculateWorld();
}

void Transform::SetLocalPosition(const vec3& _Position)
{
	position = _Position;
	CalculateWorld();
}

void Transform::SetLocalRotation(const vec3& _Rotation)
{
	localRotation = _Rotation;
	CalculateWorld();
}

void Transform::SetLocalScale(const vec3& _Scale)
{
	localScale = _Scale;
	CalculateWorld();
}


void Transform::CalculateWorld()
{
	transformMat = glm::mat4(1.0f);

	if (parent != nullptr)
	{
		//transform
		{
			glm::mat4 leftMat = glm::mat4(1.0f);
			glm::mat4 rightMat = glm::mat4(1.0f);
			leftMat = glm::translate(leftMat, position);
			rightMat = glm::translate(rightMat, parent->world_position); // ex)  Root : 2 , Root->children : 3 >> Root->children->childeren = 5 (Pos)
			leftMat = leftMat * rightMat;
			world_position = vec3(leftMat[3][0], leftMat[3][1], leftMat[3][2]); //mat[col][row] ??
		}
	}
	else // if this == Root >> local = world
	{
		world_position = position;
		worldRotation = localRotation;
		worldScale = localScale;
	}
	transformMat = glm::translate(transformMat, world_position);
	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.x), glm::vec3(1.f, 0, 0.f));
	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.y), glm::vec3(0.f, 1.f, 0.f));
	transformMat = glm::rotate(transformMat,glm::radians(worldRotation.z),glm::vec3(0.f,0,1.f));
	transformMat = glm::scale(transformMat, worldScale);
}

void Transform::UpdateTransform()
{
	CalculateWorld();
}
