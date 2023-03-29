#include "EngineShader.h"

EngineShader::EngineShader()
{
}

EngineShader::~EngineShader()
{
}

void EngineShader::Init()
{
	//Can make instance
	//-------------------- Init VertexShader ----------------------------------
	{
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		EnginePath NewPath = EngineFilesystem::GetProjectPath();
		NewPath.Move("EngineResource");
		NewPath.Move("Shader");
		NewPath.Move("EngineVertexShader.glsl");
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

		EnginePath NewPath = EngineFilesystem::GetProjectPath();
		NewPath.Move("EngineResource");
		NewPath.Move("Shader");
		NewPath.Move("EngineFragmentShader.glsl");
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



	//-------------------- Can make Instance ----------------------------------
	//-------------------- Init VAO ----------------------------------
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
	}
	//-------------------- Init VBO ----------------------------------
	{
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		//int VerticesSize = sizeof(vertices.size() * 4);
//		float vertices[] = {
//-0.5f, -0.5f, 0.0f, // left  
// 0.5f, -0.5f, 0.0f, // right 
// 0.0f,  0.5f, 0.0f  // top   
//		};

		//Square
		float vertices[] = {
			-0.5f, -0.5f, 0.0f, // left down  
			 0.5f, -0.5f, 0.0f, // right down
			 -0.5f,  0.5f, 0.0f,// left top
			 0.5f,  0.5f, 0.0f  // right top
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}
	//-------------------- Init EBO ----------------------------------
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 1
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	//Linking Vertex Attribute
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
	//-------------------- Unbind Buffer ----------------------------------
	{
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}
}

void EngineShader::RenderTri()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void EngineShader::RenderSquare()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
}
