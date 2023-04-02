#pragma once
#include "EngineMinimal.h"

class EngineShader
{
public:
	EngineShader();
	~EngineShader();

	void Use();


private:
	unsigned int vertexShader = 0;
	unsigned int fragmentShader = 0;
	unsigned int shaderProgram = 0;
};

