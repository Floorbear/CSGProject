#pragma once
#include "Utils.h"
#include "Mesh.h"

class Shader;
class Camera;
class Gizmo
{
public:
	Gizmo(TransformComponent* _parentTransform);
	~Gizmo();

private:
	std::vector<Mesh*> uiMesh; // 0 : x , 1 : y , 2: z , 3 : mainDot
	std::vector<vec3> uiMesh_scale;
	std::vector<vec3> uiMesh_position;
	std::vector<vec3> uiMesh_color;
	TransformComponent* parentTransform;
	Shader* shader;

	void apply(Camera* _camera); 
public:
	void render(Camera* _camera);
};

