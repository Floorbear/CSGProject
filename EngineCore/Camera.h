#pragma once

#include "Mesh.h"
#include "Shader.h"

#include <glm/glm.hpp>

#include <list>
#include "EngineCore/Transform.h"

class Camera
{
public:
	static float speed_move_default;
	static float speed_rotate_default;
	static float speed_move_fast;

	Camera(float width_, float height_, float fov = 45.f);
	~Camera();

	void resize(float viewport_width, float viewport_height);

public:
	void calculate_view();

	inline glm::mat4 get_view()
	{
		return view;
	}

	inline glm::mat4 get_projection()
	{
		return projection;
	}

	inline Transform* get_transform()
	{
		return &transform;
	}

private:
	Transform transform;

	float width = 0.f;
	float height = 0.f;
	float fov = 45.f;
	float near = 0.1f;
	float far = 100.f;


	glm::mat4 view;
	glm::mat4 projection;
};

