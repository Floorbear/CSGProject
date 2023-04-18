#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Utils.h"

using namespace glm;

class Transform{
public:
	Transform();
	Transform(vec4& _Vec4);
	~Transform();

	void add_localPosition(const vec3& _Position);
	void add_localRotation( const vec3& _Rotation);
	void add_localScale(const vec3& _Scale);

	void set_localPostition(const vec3& _Position);
	void set_localRotation(const vec3& _Rotation);
	void set_localScale(const vec3& _Scale);

	inline vec3 get_worldPosition()
	{
		return worldPosition;
	}
	inline vec3 get_worldRotation()
	{
		return worldRotation;
	}


	void calculate_world();
	void update_transform();

	glm::mat4* get_transformMat()
	{
		return &transformMat;
	}

	inline glm::vec3 get_frontDir()
	{
		glm::vec3 frontDir = Utils::get_vecFromPitchYaw(worldRotation.x, worldRotation.y);
		frontDir = glm::normalize(frontDir);
		return frontDir;
	}

	inline glm::vec3 get_rightDir()
	{
		glm::vec3 rightDir;
		glm::vec3 frontDir = Utils::get_vecFromPitchYaw(worldRotation.x, worldRotation.y);
		glm::vec3 up = glm::vec3(0, 1, 0);

		rightDir = -glm::cross(frontDir, up);

		rightDir = glm::normalize(rightDir);
		return rightDir;
	}

private:
	glm::vec4 vector;
	vec3 localPosition; // 메인 프로퍼티
	vec3 localScale;
	vec3 localRotation = vec3(0.f, 0.f, 0.f);


	vec3 worldPosition = vec3(0.f, 0.f, 0.f);
	vec3 worldScale = vec3(1.f, 1.f, 1.f);
	vec3 worldRotation = vec3(0.f, 0.f, 0.f); // w : Radian

	Transform* parent;

	glm::mat4 transformMat;
};

