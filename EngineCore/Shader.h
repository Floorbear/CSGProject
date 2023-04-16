#pragma once
#include "EngineMinimal.h"

class Shader
{
public:
	Shader();
	~Shader();

	void Use();

	inline unsigned int get_shaderProgram()
	{
		return shaderProgram;
	}

private:
	unsigned int vertexShader = 0;
	unsigned int fragmentShader = 0;
	unsigned int shaderProgram = 0;
};

