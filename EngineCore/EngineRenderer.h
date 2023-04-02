#pragma once
#include "EngineCore/EngineMinimal.h"


class EngineRenderer
{
public:
	EngineRenderer();
	~EngineRenderer();

	void RenderTriangle();
	//void RenderSquare();

	EngineTransform* GetTransform()
	{
		return &transform;
	}

private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	class EngineShader* shader;

	EngineTransform transform;
};

