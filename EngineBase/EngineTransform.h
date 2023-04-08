#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef glm::vec2 vector2;
typedef glm::vec3 vector3;
typedef glm::vec4 vector4;

class EngineTransform
{
public:
	EngineTransform();
	EngineTransform(vector4& _Vec4);
	~EngineTransform();

	void AddLocalPosition(const vector3& _Position);
	void AddLocalRotation( const vector3& _Rotation);
	void AddLocalScale(const vector3& _Scale);

	void SetLocalPosition(const vector3& _Position);
	void SetLocalRotation(const vector3& _Rotation);
	void SetLocalScale(const vector3& _Scale);

	inline vector3 GetWorldPosition()
	{
		return worldPosition;
	}


	void CalculateWorld();
	void UpdateTransform();

	glm::mat4* GetTransformMat()
	{
		return &transformMat;
	}

private:
	glm::vec4 vector;
	vector3 localPosition;
	vector3 localScale;
	vector3 localRotation = vector3(0.f, 0.f, 0.f);

	vector3 worldPosition = vector3(0.f, 0.f, 0.f);
	vector3 worldScale = vector3(1.f, 1.f, 1.f);
	vector3 worldRotation = vector3(0.f, 0.f, 0.f); // w : Radian

	EngineTransform* parent;

	glm::mat4 transformMat;
};

