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

	// ===== 기즈모 외관 관련 =====
private:
	std::vector<Mesh*> uiMesh; // 0 : x , 1 : y , 2: z , 3 : mainDot
	std::vector<vec3> uiMesh_scale;
	std::vector<vec3> uiMesh_position;
	std::vector<vec3> uiMesh_color;
	TransformComponent* parentTransform;
	Shader* shader;
public:
	void apply(Camera* _camera); 
	void render(Camera* _camera);


	// ===== 셀렉션 관련 =====
private:
	Shader* selectionShader;
public:
	void render_selectionBuffer(Camera* _camera);
	//void render()

	// ===== Move 조작 관련 =====
public:
	void move(Camera* _camera,vec2 _curPos, vec2 _prevPos, int _axis);

	void move_dot(Camera* _camera, glm::vec2& _curPos, glm::vec2& _prevPos);


	// ===== ZSort =====
private:
	struct zSortStruct
	{
		int renderOrder = 0;
		float length = 0;
	};

	std::vector<int> zSort(std::vector<zSortStruct> _vector);
	std::vector<int> get_renderOrder(Camera* _camera);

};

