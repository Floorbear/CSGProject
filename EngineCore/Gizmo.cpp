#include "Gizmo.h"
#include "Shader.h"
#include "Camera.h"
#include <algorithm>

Gizmo::Gizmo(TransformComponent* _parentTransform)
{
	parentTransform = _parentTransform;
	for(int i = 0 ; i<4; i++)
	{
		Mesh* newMesh = new Mesh(Mesh::cube(1));
		uiMesh.push_back(newMesh);
	}

	shader = new Shader("DefaultVertexShader.glsl", "GizmoFragmentShader.glsl");
	selectionShader = new Shader("DefaultVertexShader.glsl", "SelectionFragmentShader.glsl");

	{
		float valueBig = 1.2f;
		float valueSmall = 0.13f;
		uiMesh_scale.push_back({ valueBig,valueSmall,valueSmall });
		uiMesh_scale.push_back({valueSmall,valueBig,valueSmall });
		uiMesh_scale.push_back({ valueSmall,valueSmall,valueBig });
		uiMesh_scale.push_back({valueSmall,valueSmall,valueSmall });
	}

	{
		uiMesh_color.push_back({ 1,0,0 });
		uiMesh_color.push_back({ 0,1,0 });
		uiMesh_color.push_back({ 0,0,1 });
		uiMesh_color.push_back({ 1,1,1 });
	}

	{
		float valueBig = 0.63f;
		float valueSmall = 0.f;
		uiMesh_position.push_back({ valueBig,valueSmall,valueSmall });
		uiMesh_position.push_back({ valueSmall,valueBig,valueSmall });
		uiMesh_position.push_back({ valueSmall,valueSmall,valueBig });
		uiMesh_position.push_back({ valueSmall,valueSmall,valueSmall });
	}

}

Gizmo::~Gizmo()
{
	for (int i = 0; i < uiMesh.size(); i++)
	{
		delete uiMesh[i];
	}
	if (shader != nullptr)
	{
		delete shader;
	}
}

void Gizmo::apply(Camera* _camera)
{

}

void Gizmo::render(Camera* _camera)
{
	apply(_camera);
	shader->use();

	glDisable(GL_DEPTH_TEST);

	std::vector<int> renderOrder = get_renderOrder(_camera);
	for (int i = 0; i < renderOrder.size(); i++)
	{
		Transform newTransform = parentTransform->get_value();
		newTransform.set_scale(uiMesh_scale[renderOrder[i]]);
		newTransform.add_position(uiMesh_position[renderOrder[i]]);
		shader->set_mat4("world", newTransform.get_world_matrix());
		shader->set_mat4("view", _camera->get_view());
		shader->set_mat4("projection", _camera->get_projection());
		shader->set_vec3("gizmoColor", uiMesh_color[renderOrder[i]]);

		uiMesh[renderOrder[i]]->render();
	}
	glEnable(GL_DEPTH_TEST);
}

void Gizmo::render_selectionBuffer(Camera* _camera)
{
	apply(_camera);
	selectionShader->use();

	glDisable(GL_DEPTH_TEST);

	std::vector<int> renderOrder = get_renderOrder(_camera);
	for (int i = 0; i < uiMesh.size(); i++)
	{
		Transform newTransform = parentTransform->get_value();
		newTransform.set_scale(uiMesh_scale[renderOrder[i]]);
		newTransform.add_position(uiMesh_position[renderOrder[i]]);
		selectionShader->set_mat4("world", newTransform.get_world_matrix());
		selectionShader->set_mat4("view", _camera->get_view());
		selectionShader->set_mat4("projection", _camera->get_projection());

		selectionShader->set_uint("objectType", 1);
		selectionShader->set_uint("modelID", renderOrder[i]); // 0 : x , 1 : y , 2: z , 3 : mainDot
		selectionShader->set_uint("meshID", 0);
		uiMesh[renderOrder[i]]->render();
	}
	glEnable(GL_DEPTH_TEST);
}

void Gizmo::move(Camera* _camera,vec2 _curPos, vec2 _prevPos, int _axis)
{

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
	//parentScreenPositon.y *= -1;
	vec2 axisScreenPosition = _camera->worldPosition_to_screenPosition(parentTransform->get_position() + uiMesh_position[_axis]);
	//axisScreenPosition.y *= -1;

	vec2 screenVector = axisScreenPosition - parentScreenPositon;
	screenVector = normalize(screenVector);
	vec2 mouseVector = _curPos - _prevPos;
	mouseVector.y = -mouseVector.y;

	float moveForce = dot(screenVector, mouseVector);



	//printf("ScreenPos : %f, %f \n", screenPos.x, screenPos.y);
	//printf("ImguiPos : %f, %f \n", _curPos.x, _curPos.y);

	float speed = 5.f;
	parentTransform->add_position(speed * Utils::time_delta() * uiMesh_color[_axis] * moveForce); // Color는 축역활도 함


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

std::vector<int> Gizmo::get_renderOrder(Camera* _camera)
{
	//Set ZOrder
	std::vector<zSortStruct> newZSortStructVector;
	for (int i = 0; i < 3; i++)
	{
		zSortStruct newZSortStruct;
		newZSortStruct.length = length(uiMesh_position[i] - _camera->get_transform()->get_position());
		newZSortStruct.renderOrder = i;
		newZSortStructVector.push_back(newZSortStruct);
	}
	std::vector<int> renderOrder = zSort(newZSortStructVector);
	renderOrder.push_back(3);
	return renderOrder;
}
