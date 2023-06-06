#include "Shader.h"
#include "FileSystem.h"

 std::map<std::string, std::string> Shader::compileData;

Shader::Shader(std::string vertexShader_, std::string fragmentShader_)
{
	//Can make instance
	//-------------------- Init VertexShader ----------------------------------
	{
		static bool isCompileVertexShader = false;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		auto iter = compileData.find(vertexShader_);
		if(iter == compileData.end())
		{
			EnginePath NewPath = FileSystem::GetProjectPath().move("EngineResource").move("Shader").move(vertexShader_);
			std::string ShaderTEXT = NewPath.ReadFile();
			compileData.insert(std::make_pair(vertexShader_, ShaderTEXT));
			//shaderSource = ShaderTEXT.c_str();
			isCompileVertexShader = true;
		}
		const char* shaderSource = compileData[vertexShader_].c_str();
		glShaderSource(vertexShader, 1, &shaderSource, NULL);
		glCompileShader(vertexShader);

		int success = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		assert(success != 0);
	}
	//-------------------- Init FragmentShader ----------------------------------
	{
		static bool isCompileFragmentShader = false;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		auto iter = compileData.find(fragmentShader_);
		if (iter == compileData.end())
		{
			EnginePath NewPath = FileSystem::GetProjectPath().move("EngineResource").move("Shader").move(fragmentShader_);
			std::string ShaderTEXT = NewPath.ReadFile();
			compileData.insert(std::make_pair(fragmentShader_, ShaderTEXT));
			isCompileFragmentShader = true;
		}
		const char* shaderSource = compileData[fragmentShader_].c_str();
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

void Shader::use()
{
	glUseProgram(shaderProgram);
}

void Shader::set_float(std::string_view uniform_, float value_)
{
	GLuint uniformLocation = glGetUniformLocation(shaderProgram, uniform_.data());
	glUniform1f(uniformLocation, value_);
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

void Shader::set_int(std::string_view uniform_, int _int)
{
	GLuint uniformLocation = glGetUniformLocation(shaderProgram, uniform_.data());
	glUniform1i(uniformLocation, _int);
}

void Shader::set_uint(std::string_view uniform_, unsigned int _int)
{
	GLuint uniformLocation = glGetUniformLocation(shaderProgram, uniform_.data());
	glUniform1ui(uniformLocation, _int);
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
