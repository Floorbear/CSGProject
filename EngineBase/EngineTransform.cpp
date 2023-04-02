#include "EngineTransform.h"

EngineTransform::EngineTransform()
{
	vector = glm::vec4(0.f, 0.f, 0.f, 1.f);
	transformMat = glm::mat4(1.0f);

	position = glm::vec3(0.f,0.f,0.f);
	scale = glm::vec3(1.f, 1.f, 1.f);
	rotation = glm::vec4(1.f, 1.f, 1.f, 0.f);
}

EngineTransform::EngineTransform(glm::vec4& _Vec4)
{
	vector = _Vec4;
	transformMat = glm::mat4(1.0f);

	position = glm::vec3(0.f, 0.f, 0.f);
	scale = glm::vec3(1.f, 1.f, 1.f);
	rotation = glm::vec4(1.f, 1.f, 1.f, 0.f);
}

EngineTransform::~EngineTransform()
{
}

void EngineTransform::SetPosition(const glm::vec3& _Position)
{
	position = _Position;
}

void EngineTransform::SetRotation(const glm::vec4& _Rotation)
{
	rotation = _Rotation;
}


void EngineTransform::Calculate()
{
	transformMat = glm::mat4(1.0f);
	transformMat = glm::scale(transformMat, scale);
	transformMat = glm::rotate(transformMat,glm::radians(rotation.w),glm::vec3(rotation.x,rotation.y,rotation.z));
	transformMat = glm::translate(transformMat, position);
}
