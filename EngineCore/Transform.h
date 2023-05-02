#pragma once
#include "Component.h"

#include <glm/glm.hpp>

using namespace glm;

class Transform : public Component{
	mat4 matrix;

	vec3 position;
	vec3 rotation = vec3(0.f, 0.f, 0.f);
	vec3 scale3d;

public:
	Transform();
	Transform(const vec4& _Vec4);
	~Transform();

	vec3 get_position();
	vec3 get_rotation();
	vec3 get_scale();

	virtual void set_position(const vec3& value);
	virtual void set_rotation(const vec3& value);
	virtual void set_scale(const vec3& value);

	virtual void translate(const vec3& value);
	virtual void rotate(const vec3& value);
	virtual void scale(const vec3& value);
	virtual void add_position(const vec3& value);

	inline vec3 get_worldPosition()
	{
		return worldPosition;
	}
	inline vec3 get_worldRotation()
	{
		return worldRotation;
	}


	void calculate_matrix();
	mat4* get_matrix();

	vec3 get_forward_dir();
	vec3 get_right_dir();
	vec3 get_up_dir();

private:
	glm::vec4 vector;


	vec3 worldPosition = vec3(0.f, 0.f, 0.f);
	vec3 worldScale = vec3(1.f, 1.f, 1.f);
	vec3 worldRotation = vec3(0.f, 0.f, 0.f); // w : Radian

	//Transform* parent; 굳이 필요??
};

