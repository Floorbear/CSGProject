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

	void set_float(std::string_view uniform_, float value_);
	void set_mat4(std::string_view uniform_, const glm::mat4& value_);
	void set_vec3(std::string_view uniform_, const glm::vec3& value_);

private:
	unsigned int vertexShader = 0;
	unsigned int fragmentShader = 0;
	unsigned int shaderProgram = 0;
};

