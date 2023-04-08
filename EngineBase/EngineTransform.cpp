#include "EngineTransform.h"
EngineTransform::EngineTransform()
{
	vector = vector4(0.f, 0.f, 0.f, 1.f);
	transformMat = glm::mat4(1.0f);

	localPosition = vector3(0.f,0.f,0.f);
	localScale = vector3(1.f, 1.f, 1.f);
}

EngineTransform::EngineTransform(vector4& _Vec4)
{
	vector = _Vec4;
	transformMat = glm::mat4(1.0f);

	localPosition = vector3(0.f, 0.f, 0.f);
	localScale = vector3(1.f, 1.f, 1.f);
}

EngineTransform::~EngineTransform()
{
}

void EngineTransform::AddLocalPosition(const vector3& _Position)
{
	localPosition = localPosition + _Position;
	CalculateWorld();
}

void EngineTransform::AddLocalRotation(const vector3& _Rotation)
{
	localRotation = localRotation + _Rotation;
	CalculateWorld();
}

void EngineTransform::AddLocalScale(const vector3& _Scale)
{
	localScale = localScale + _Scale;
	CalculateWorld();
}

void EngineTransform::SetLocalPosition(const vector3& _Position)
{
	localPosition = _Position;
	CalculateWorld();
}

void EngineTransform::SetLocalRotation(const vector3& _Rotation)
{
	localRotation = _Rotation;
	CalculateWorld();
}

void EngineTransform::SetLocalScale(const vector3& _Scale)
{
	localScale = _Scale;
	CalculateWorld();
}


void EngineTransform::CalculateWorld()
{
	transformMat = glm::mat4(1.0f);

	if (parent != nullptr)
	{
		//transform
		{
			glm::mat4 leftMat = glm::mat4(1.0f);
			glm::mat4 rightMat = glm::mat4(1.0f);
			leftMat = glm::translate(leftMat, localPosition);
			rightMat = glm::translate(rightMat, parent->worldPosition); // ex)  Root : 2 , Root->children : 3 >> Root->children->childeren = 5 (Pos)
			leftMat = leftMat * rightMat;
			worldPosition = vector3(leftMat[3][0], leftMat[3][1], leftMat[3][2]); //mat[col][row] ??
		}
	}
	else // if this == Root >> local = world
	{
		worldPosition = localPosition;
		worldRotation = localRotation;
		worldScale = localScale;
	}
	transformMat = glm::translate(transformMat, worldPosition);
	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.x), glm::vec3(1.f, 0, 0.f));
	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.y), glm::vec3(0.f, 1.f, 0.f));
	transformMat = glm::rotate(transformMat,glm::radians(worldRotation.z),glm::vec3(0.f,0,1.f));
	transformMat = glm::scale(transformMat, worldScale);
}

void EngineTransform::UpdateTransform()
{
	CalculateWorld();
}
