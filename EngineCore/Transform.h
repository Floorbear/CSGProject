#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Transform{
public:
	Transform();
	Transform(vec4& _Vec4);
	~Transform();

	void AddLocalPosition(const vec3& _Position);
	void AddLocalRotation( const vec3& _Rotation);
	void AddLocalScale(const vec3& _Scale);

	void SetLocalPosition(const vec3& _Position);
	void SetLocalRotation(const vec3& _Rotation);
	void SetLocalScale(const vec3& _Scale);

	inline vec3 GetWorldPosition()
	{
		return position;
	}


	void CalculateWorld();
	void UpdateTransform();

	glm::mat4* GetTransformMat()
	{
		return &transformMat;
	}

private:
	glm::vec4 vector;
	vec3 position; // 메인 프로퍼티
	vec3 localScale;
	vec3 localRotation = vec3(0.f, 0.f, 0.f);


	// 월드좌표 캐시
	vec3 world_position = vec3(0.f, 0.f, 0.f);
	vec3 worldScale = vec3(1.f, 1.f, 1.f);
	vec3 worldRotation = vec3(0.f, 0.f, 0.f); // w : Radian

	Transform* parent;

	glm::mat4 transformMat;
};

