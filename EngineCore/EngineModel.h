#pragma once
#include "EngineCore/EngineMinimal.h"

struct Vertex
{
	vector3 postition;
	//vector3 normal
	//vector2 texcoords
};

class EngineModel
{
public:
	EngineModel();
	~EngineModel();

	void RenderTriangle();
	//void RenderSquare();

	EngineTransform* GetTransform()
	{
		return &transform;
	}

	class EngineShader* GetShader()
	{
		return shader;
	}

private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	class EngineShader* shader;

	std::vector<Vertex> vertex;
	std::vector<unsigned int> indices;

	EngineTransform transform;
};

