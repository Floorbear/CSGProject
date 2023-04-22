#include "Shader.h"
#include "FileSystem.h"

Shader::Shader()
{
	//Can make instance
	//-------------------- Init VertexShader ----------------------------------
	{
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		EnginePath NewPath = FileSystem::GetProjectPath();
		NewPath.Move("EngineResource");
		NewPath.Move("Shader");
		NewPath.Move("DefaultVertexShader.glsl");
		std::string ShaderTEXT = NewPath.ReadFile();
		const char* shaderSource = ShaderTEXT.c_str();

		glShaderSource(vertexShader, 1, &shaderSource, NULL);
		glCompileShader(vertexShader);

		int success = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		assert(success != 0);
	}
	//-------------------- Init FragmentShader ----------------------------------
	{
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		EnginePath NewPath = FileSystem::GetProjectPath();
		NewPath.Move("EngineResource");
		NewPath.Move("Shader");
		NewPath.Move("DefaultFragmentShader.glsl");
		std::string ShaderTEXT = NewPath.ReadFile();
		const char* shaderSource = ShaderTEXT.c_str();

		glShaderSource(fragmentShader, 1, &shaderSource, NULL);
		glCompileShader(fragmentShader);
		int success = 0;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		assert(success != 0);
	}

	//-------------------- Init shaerProgram ----------------------------------
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//-------------------- Delete shader ----------------------------------
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
}

Shader::~Shader()
{
}

void Shader::Use()
{
	glUseProgram(shaderProgram);
}

void Shader::set_mat4(std::string_view uniform_, const glm::mat4& value_)
{
	GLuint uniformLocation = glGetUniformLocation(shaderProgram, uniform_.data());
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value_));
}

void Shader::set_vec3(std::string_view uniform_, const glm::vec3& value_)
{
	GLuint uniformLocation = glGetUniformLocation(shaderProgram, uniform_.data());
	glUniform3fv(uniformLocation, 1, &value_[0]);
}


//void EngineShader::RenderTri()
//{
//
//}
//
//void EngineShader::RenderSquare()
//{
//	glUseProgram(shaderProgram);
//	glBindVertexArray(VAO);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//	//glBindVertexArray(0);
//}
