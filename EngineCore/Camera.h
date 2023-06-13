#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"

#include <glm/glm.hpp>

#include <list>

class Camera : public Entity, Component{
public:
	static float speed_move_default;
	static float speed_rotate_default;
	static float speed_move_fast_multiplier;

	static float default_pos_z;

	Camera(float width_, float height_, float fov = 45.f);
	~Camera();

	void resize(float viewport_width, float viewport_height);

public:
	void calculate_view();
	void calculate_view_ortho();

	inline glm::mat4 get_view()
	{
		return view;
	}

	inline glm::mat4 get_projection()
	{
		return projection;
	}

	inline TransformComponent* get_transform()
	{
		return &transform;
	}

	//Screen 좌표계 왼쪽 아래가 중점...  y증가하면 위로
	//Imgui 포지션 : y증가하면 아래로
	vec3 world_position_to_ndc_position(const vec3& world_position_);
	vec3 ndc_position_to_world_position(const vec3& ndc_position_);
	vec2 world_position_to_screen_position(const vec3& world_position_);

private:
	TransformComponent transform;
public:
	float width = 0.f;
	float height = 0.f;
	float fov = 45.f;
	float near = 0.1f;
	float far = 100.f;


private:
	glm::mat4 view;
	glm::mat4 projection;
};

