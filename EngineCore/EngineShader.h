#pragma once
#include "EngineMinimal.h"

class EngineShader
{
public:
	EngineShader();
	~EngineShader();

	void Init();

	void RenderTri();


private:
	unsigned int VBO = 0;
	unsigned int VAO = 0;
	unsigned int vertexShader = 0;
	unsigned int fragmentShader = 0;
	unsigned int shaderProgram = 0;

	//std::vector<float> vertices = { -0.5f,-0.5f,0.0f,0.5f,-0.5f,0.0f,0.0f,0.5f,0.0f };
};

