#include "Gizmo.h"
#include "Shader.h"
#include "Camera.h"
#include "FileSystem.h"

#include <glad/glad.h>

#include <algorithm>

GizmoMode Gizmo::gizmoMode = GizmoMode::Translate;

Gizmo::Gizmo(TransformComponent* _parentTransform)
{
	parentTransform = _parentTransform;


	shader = new Shader("DefaultVertexShader.glsl", "GizmoFragmentShader.glsl");
	selectionShader = new Shader("DefaultVertexShader.glsl", "SelectionFragmentShader.glsl");

	float dotScale = 0.13f;
	//트랜스폼 메쉬 초기화
	{
		for (int i = 0; i < 4; i++)
		{
			Mesh* newMesh = new Mesh(Mesh::cube(1));
			transformMesh.push_back(newMesh);
		}

		float valueBig = 1.2f;
		float valueSmall = 0.13f;
		transformMesh_scale.push_back({ valueBig,valueSmall,valueSmall });
		transformMesh_scale.push_back({valueSmall,valueBig,valueSmall });
		transformMesh_scale.push_back({ valueSmall,valueSmall,valueBig });
		transformMesh_scale.push_back({ dotScale,dotScale,dotScale });
	}

	{
		mesh_color.push_back({ 1,0,0 });
		mesh_color.push_back({ 0,1,0 });
		mesh_color.push_back({ 0,0,1 });
		mesh_color.push_back({ 1,1,1 });
	}

	{
		float valueBig = 0.63f;
		float valueSmall = 0.f;
		transformMesh_position.push_back({ valueBig,valueSmall,valueSmall });
		transformMesh_position.push_back({ valueSmall,valueBig,valueSmall });
		transformMesh_position.push_back({ valueSmall,valueSmall,valueBig });
		transformMesh_position.push_back({ valueSmall,valueSmall,valueSmall });
	}

	//스케일 메쉬 초기화
	{
		for (int i = 0; i < 4; i++)
		{
			Mesh* newMesh = new Mesh(Mesh::cube(1));
			scaleMesh.push_back(newMesh);
		}

		float valueBig = 0.3f;
		float valueSmall = 0.3f;
		scaleMesh_scale.push_back({ valueBig,valueSmall,valueSmall });
		scaleMesh_scale.push_back({ valueSmall,valueBig,valueSmall });
		scaleMesh_scale.push_back({ valueSmall,valueSmall,valueBig });
		scaleMesh_scale.push_back({ dotScale,dotScale,dotScale });
	}

	{
		float valueBig = 1.2f;
		float valueSmall = 0.f;
		scaleMesh_position.push_back({ valueBig,valueSmall,valueSmall });
		scaleMesh_position.push_back({ valueSmall,valueBig,valueSmall });
		scaleMesh_position.push_back({ valueSmall,valueSmall,valueBig });
		scaleMesh_position.push_back({ valueSmall,valueSmall,valueSmall });
	}


	//렌더 함수 초기화
	renderColorFunc.resize(static_cast<int>(GizmoMode::Max));
	renderColorFunc[static_cast<int>(GizmoMode::Translate)] = std::bind(&Gizmo::render_transformMesh, this, std::placeholders::_1);
	renderColorFunc[static_cast<int>(GizmoMode::Scale)] = std::bind(&Gizmo::render_scaleMesh, this,std::placeholders::_1);

	renderSelectionFunc.resize(static_cast<int>(GizmoMode::Max));
	renderSelectionFunc[static_cast<int>(GizmoMode::Translate)] = std::bind(&Gizmo::render_transformMesh_selection, this, std::placeholders::_1);
	renderSelectionFunc[static_cast<int>(GizmoMode::Scale)] = std::bind(&Gizmo::render_scaleMesh_selection, this, std::placeholders::_1);
}

Gizmo::~Gizmo()
{
	for (int i = 0; i < transformMesh.size(); i++)
	{
		delete transformMesh[i];
	}
	if (shader != nullptr)
	{
		delete shader;
	}
}


void Gizmo::render(Camera* _camera)
{
	shader->use();

	glDisable(GL_DEPTH_TEST);
	renderColorFunc[static_cast<int>(gizmoMode)](_camera);
	glEnable(GL_DEPTH_TEST);
}

void Gizmo::render_color(Camera* _camera, TransformComponent* _parentTransform, std::vector<Mesh*> _mesh, Shader* _shader, std::vector<vec3> _mesh_scale, std::vector<vec3> _mesh_position, std::vector<vec3> _mesh_color, int _selectedAxis)
{ 

	std::vector<int> renderOrder = get_renderOrder(_camera, _mesh_position);
	for (int i = 0; i < renderOrder.size(); i++)
	{
		Transform newTransform = _parentTransform->get_value();
		newTransform.set_rotation({ 0,0,0 });
		newTransform.set_scale(_mesh_scale[renderOrder[i]]);
		newTransform.add_position(_mesh_position[renderOrder[i]]);
		_shader->set_mat4("world", newTransform.get_world_matrix());
		_shader->set_mat4("view", _camera->get_view());
		_shader->set_mat4("projection", _camera->get_projection());
		if (renderOrder[i] == _selectedAxis) // 선택된 녀석이면 색깔을 노란색으로
		{
			_shader->set_vec3("gizmoColor", selectedAxis_color);
		}
		else
		{
			_shader->set_vec3("gizmoColor", _mesh_color[renderOrder[i]]);
		}
		_mesh[renderOrder[i]]->render();
	}
}

void Gizmo::render_transformMesh(Camera* _camera)
{
	//선택 이펙트
	int SelectedAxis_index = get_selectedAxis();
	render_color(_camera, parentTransform, transformMesh, shader, transformMesh_scale, transformMesh_position, mesh_color, SelectedAxis_index);
}

void Gizmo::render_scaleMesh(Camera* _camera)
{
	//선택 이펙트
	int SelectedAxis_index = get_selectedAxis();
	render_color(_camera, parentTransform, transformMesh, shader, transformMesh_scale, transformMesh_position, mesh_color, SelectedAxis_index);
	render_color(_camera, parentTransform, scaleMesh, shader, scaleMesh_scale, scaleMesh_position, mesh_color, SelectedAxis_index);
}

void Gizmo::render_selection(Camera* _camera, TransformComponent* _parentTransform, std::vector<Mesh*> _mesh, Shader* _shader, std::vector<vec3> _mesh_scale, std::vector<vec3> _mesh_position)
{
	std::vector<int> renderOrder = get_renderOrder(_camera, _mesh_position);
	for (int i = 0; i < renderOrder.size(); i++)
	{
		Transform newTransform = _parentTransform->get_value();
		newTransform.set_rotation({ 0,0,0 });
		newTransform.set_scale(_mesh_scale[renderOrder[i]]);
		newTransform.add_position(_mesh_position[renderOrder[i]]);
		_shader->set_mat4("world", newTransform.get_world_matrix());
		_shader->set_mat4("view", _camera->get_view());
		_shader->set_mat4("projection", _camera->get_projection());
		
		_shader->set_uint("objectType", renderOrder[i] + 2);// 2 : x , 3 : y , 4: z , 5 : mainDot
		_shader->set_uint("modelID", 0); 
		_shader->set_uint("meshID", 0);
		_mesh[renderOrder[i]]->render();
	}
}

void Gizmo::render_transformMesh_selection(Camera* _camera)
{
	render_selection(_camera, parentTransform, transformMesh, selectionShader, transformMesh_scale, transformMesh_position);
}

void Gizmo::render_scaleMesh_selection(Camera* _camera)
{
	render_selection(_camera, parentTransform, transformMesh, selectionShader, transformMesh_scale, transformMesh_position);
	render_selection(_camera, parentTransform, scaleMesh, selectionShader, scaleMesh_scale, scaleMesh_position);
}

void Gizmo::render_selectionBuffer(Camera* _camera)
{
	selectionShader->use();

	glDisable(GL_DEPTH_TEST);
	renderSelectionFunc[static_cast<int>(gizmoMode)](_camera);
	glEnable(GL_DEPTH_TEST);
}

int Gizmo::get_selectedAxis()
{
	if (selectedAxis_index == -1)
	{
		return selectedAxis_index;
	}

	int CopyValue = selectedAxis_index;
	selectedAxis_index = -1;
	return CopyValue;
}

void Gizmo::set_selectedAxis(GizmoAxis _axis)
{
	selectedAxis_index = (int)_axis;
}

void Gizmo::move(Camera* _camera,vec2 _curPos, vec2 _prevPos, int _axis)
{
	set_selectedAxis((GizmoAxis)_axis);
	if (abs(length(_curPos) - length(_prevPos)) < 0.01f)
	{
		return;
	}

	if (_axis == 3)
	{
		move_dot(_camera, _curPos, _prevPos);
		return;
	}
	//Screen좌표계 : 
	vec2 parentScreenPositon = _camera->worldPosition_to_screenPosition(parentTransform->get_position());
	vec2 axisScreenPosition = _camera->worldPosition_to_screenPosition(parentTransform->get_position() + transformMesh_position[_axis]);

	vec2 screenVector = axisScreenPosition - parentScreenPositon;
	screenVector = normalize(screenVector);
	vec2 mouseVector = _curPos - _prevPos;
	mouseVector.y = -mouseVector.y;

	float moveForce = dot(screenVector, mouseVector);




	float speed = 5.f;
	switch (gizmoMode)
	{
	case GizmoMode::Translate:
		parentTransform->add_position(speed * Utils::time_delta() * mesh_color[_axis] * moveForce); // Color는 축역활도 함
		break;
	case GizmoMode::Scale:
		parentTransform->scale(speed * Utils::time_delta() * mesh_color[_axis] * moveForce); // Color는 축역활도 함
		break;
	case GizmoMode::Max:
		break;
	default:
		break;
	}


}

void Gizmo::move_dot(Camera* _camera, glm::vec2& _curPos, glm::vec2& _prevPos)
{
	vec3 moveRightVector = _camera->get_transform()->get_right_dir();
	vec3 moveUpVecotr = _camera->get_transform()->get_up_dir();

	vec2 mouseVector = _curPos - _prevPos;
	mouseVector.y = -mouseVector.y;

	vec3 moveDirVector = moveRightVector * mouseVector.x + moveUpVecotr * mouseVector.y;

	//카메라가 멀어지면 이동값이 크고 카메라와 오브젝트가 가까우면 이동값이 작아야한다.
	//이때 이동값이 일정하기 위해서는 내적을 해야한다.!
	float speed = 0.8f * dot(parentTransform->get_position() - _camera->get_transform()->get_position(), _camera->get_transform()->get_forward_dir());

	parentTransform->add_position(Utils::time_delta() * moveDirVector * speed);
}

std::vector<int> Gizmo::zSort(std::vector<zSortStruct> _vector)
{
	std::sort(_vector.begin(), _vector.end(), [](zSortStruct _a, zSortStruct _b)
		{
			return _a.length > _b.length;
		});

	std::vector<int> result;
	for (int i = 0; i < 3; i++)
	{
		result.push_back(_vector[i].renderOrder);
	}
	return result;
}

std::vector<int> Gizmo::get_renderOrder(Camera* _camera, std::vector<vec3> _positionVector)
{
	//Set ZOrder
	std::vector<zSortStruct> newZSortStructVector;
	for (int i = 0; i < 3; i++)
	{
		zSortStruct newZSortStruct;
		newZSortStruct.length = length(_positionVector[i] - _camera->get_transform()->get_position());
		newZSortStruct.renderOrder = i;
		newZSortStructVector.push_back(newZSortStruct);
	}
	std::vector<int> renderOrder = zSort(newZSortStructVector);
	renderOrder.push_back(3);
	return renderOrder;
}
