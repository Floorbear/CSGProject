#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class EngineTransform
{
public:
	EngineTransform();
	EngineTransform(glm::vec4& _Vec4);
	~EngineTransform();

	void SetPosition(const glm::vec3& _Position);
	void SetRotation(const glm::vec4& _Rotation);


	void Calculate();

	glm::mat4* GetTransformMat()
	{
		return &transformMat;
	}

private:
	glm::vec4 vector;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec4 rotation; // w : Radian
	glm::mat4 transformMat;
};

