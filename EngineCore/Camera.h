#pragma once

#include "Mesh.h"
#include "Shader.h"

#include <glm/glm.hpp>

#include <list>
#include "EngineCore/Transform.h"

class GUI;
class Camera
{
public:
	static float speed_move_default;
	static float speed_rotate_default;
	static float speed_move_fast;

	Camera(float width_, float height_, float fov = 45.f);
	~Camera();

public:
	void calculate_view();
	inline void set_parent(GUI* parent_)
	{
		assert(parent_ != nullptr);
		parent = parent_;
	}

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
	GUI* parent;
	Transform transform;

	float width = 0.f;
	float height = 0.f;
	float fov = 45.f;


	glm::mat4 view;
	glm::mat4 projection;
};

