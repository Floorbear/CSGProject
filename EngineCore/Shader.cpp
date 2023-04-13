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
