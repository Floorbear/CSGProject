#pragma once

#include "Utils.h"
#include "Mesh.h"
#include "Transform.h"

// ===== 기즈모 외관 관련 =====
enum class GizmoMode{
	Translate,
	Scale,
	Rotation,
	Max
};

enum class GizmoAxis{
	X = 0, Y = 1, Z = 2, XY = 4, YZ = 5, ZX = 6, XYZ = 3
};

class Shader;
class Camera;
class Gizmo
{
public:
	Gizmo(TransformComponent* _parentTransform);
	~Gizmo();


private:
	// ===== Transform Mesh =====
	std::vector<Mesh*> transformMesh; // 0 : x , 1 : y , 2: z , 3 : mainDot
	std::vector<vec3> transformMesh_scale;
	std::vector<vec3> transformMesh_position;

	// ===== Scale Mesh =====
	std::vector<Mesh*> scaleMesh;
	std::vector<vec3> scaleMesh_scale;
	std::vector<vec3> scaleMesh_position;

	// ===== Rotation Mesh =====
	std::vector<Mesh*> rotationMesh;
	std::vector<vec3> rotationMesh_scale;
	std::vector<vec3> rotationMesh_position;
	std::vector<vec3> rotationMesh_rotation;

	// ===== 컬러 렌더링 관련 ======
	std::vector<vec3> mesh_color;
	TransformComponent* parentTransform;
	Shader* shader;
public:
	void render(Camera* _camera);
private:
	void render_color(Camera* _camera, TransformComponent* _parentTransform, std::vector<Mesh*> _mesh, Shader* _shader
		, std::vector<vec3> _mesh_scale, std::vector<vec3> _mesh_position,
		std::vector<vec3> _mesh_color, int _selectedAxis , std::vector<vec3> _mesh_rotation = {});

	void render_transformMesh(Camera* _camera);
	void render_scaleMesh(Camera* _camera);
	void render_rotationMesh(Camera* _camera);



	// ===== 셀렉션 렌더링 관련 =====
public:
	void render_selectionBuffer(Camera* _camera);

private:
	Shader* selectionShader;
	void render_selection(Camera* _camera, TransformComponent* _parentTransform, std::vector<Mesh*> _mesh, Shader* _shader
		, std::vector<vec3> _mesh_scale, std::vector<vec3> _mesh_position,
		std::vector<vec3> _mesh_rotation = {}
	);
	void render_transformMesh_selection(Camera* _camera);
	void render_scaleMesh_selection(Camera* _camera);
	void render_rotationMesh_selection(Camera* _camera);
	
	// ===== 클릭 이펙트 관련 ======
private:
	vec3 selectedAxis_color = { 1.f,1.f,0.f };
	int selectedAxis_index = -1; // -1 : Not Selected, 0 ~ 3  Axis
	int get_selectedAxis();

public:
	void set_selectedAxis(GizmoAxis _axis);

	// ===== Move 조작 관련 =====
public:
	void move(Camera* _camera,vec2 _curPos, vec2 _prevPos, int _axis);
	void move_dot(Camera* _camera, glm::vec2& _curPos, glm::vec2& _prevPos);


	// ===== ZSort 관련 =====
private:
	struct zSortStruct
	{
		int renderOrder = 0;
		float length = 0;
	};

	std::vector<int> zSort(std::vector<zSortStruct> _vector);
	std::vector<int> get_renderOrder(Camera* _camera, std::vector<vec3> _positionVector);

	// ===== 렌더 모드 관련 ======
	static GizmoMode gizmoMode;
	std::vector<std::function<void(Camera* _camera)>> renderColorFunc;
	std::vector<std::function<void(Camera* _camera)>> renderSelectionFunc;
public:
	inline static void set_gizmoMode(GizmoMode _mode)
	{
		gizmoMode = _mode;
	}
};

