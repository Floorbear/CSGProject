#include "EngineModel.h"
#include "EngineCore/EngineShader.h"

EngineModel::EngineModel()
{
	shader = new EngineShader();

}

EngineModel::~EngineModel()
{
	for (auto iter = meshes.begin(); iter != meshes.end(); ++iter)
	{
		delete* iter;
		//remove key
	}

	if (shader != nullptr)
	{
		delete shader;
	}
}

void EngineModel::Render()
{
	for (auto iter = meshes.begin(); iter != meshes.end(); ++iter)
	{

		int transformLocation = glGetUniformLocation(shader->GetShaderProgram(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(*(*iter)->GetTransform()->GetTransformMat()));
		(*iter)->Render();
	}
}
