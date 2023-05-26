#include "Gizmo.h"
#include "Shader.h"
#include "Camera.h"

Gizmo::Gizmo(TransformComponent* _parentTransform)
{
	parentTransform = _parentTransform;
	for(int i = 0 ; i<4; i++)
	{
		Mesh* newMesh = Mesh::Cube.clone_new();
		uiMesh.push_back(newMesh);
	}

	shader = new Shader("DefaultVertexShader.glsl", "GizmoFragmentShader.glsl");

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
	for (int i = 0; i < uiMesh.size(); i++)
	{
		Transform newTransform = parentTransform->get_value();
		newTransform.set_scale(uiMesh_scale[i]);
		newTransform.add_position(uiMesh_position[i]);
		shader->set_mat4("world", *(newTransform.get_matrix()));
		shader->set_mat4("view", _camera->get_view());
		shader->set_mat4("projection", _camera->get_projection());
		shader->set_vec3("gizmoColor", uiMesh_color[i]);

		uiMesh[i]->render();
	}
	glEnable(GL_DEPTH_TEST);

}
