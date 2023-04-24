#pragma once
#include "Component.h"
#include "Utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Transform : Component{
	mat4 matrix;

	vec3 position;
	vec3 rotation = vec3(0.f, 0.f, 0.f);
	vec3 scale3d;

public:
	Transform();
	Transform(vec4& _Vec4);
	~Transform();

	void set_position(const vec3& _Position);
	void set_rotation(const vec3& _Rotation);
	void set_scale(const vec3& _Scale);

	void translate(const vec3& value);
	void rotate(const vec3& value);
	void scale(const vec3& value);
	void add_position(const vec3& _Position);

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

private:
	glm::vec4 vector;


	vec3 worldPosition = vec3(0.f, 0.f, 0.f);
	vec3 worldScale = vec3(1.f, 1.f, 1.f);
	vec3 worldRotation = vec3(0.f, 0.f, 0.f); // w : Radian

	//Transform* parent; 굳이 필요??
};

