#include "EngineShader.h"

EngineShader::EngineShader()
{
}

EngineShader::~EngineShader()
{
}

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


void EngineShader::Init()
{


	//-------------------- Init VertexShader ----------------------------------
	{
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		EnginePath NewPath = EngineFilesystem::GetProjectPath();
		NewPath.Move("EngineResource");
		NewPath.Move("Shader");
		NewPath.Move("EngineVertexShader.glsl");
		std::string ShaderTEXT = NewPath.ReadFile();
		const char* shaderSource = ShaderTEXT.c_str();

		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
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

		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		int success = 0;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		assert(success != 0);
	}

	//-------------------- Init shaderProgram ----------------------------------
	{
		int success = 0;
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		assert(success != 0);

		//glUseProgram(shaderProgram);
	}

	//-------------------- Delete shader ----------------------------------
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	//-------------------- Init VAO ----------------------------------
	{
		glGenVertexArrays(1, &VAO);

	}
	//-------------------- Init VBO ----------------------------------
	{
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		//int VerticesSize = sizeof(vertices.size() * 4);
		float vertices[] = {
-0.5f, -0.5f, 0.0f, // left  
 0.5f, -0.5f, 0.0f, // right 
 0.0f,  0.5f, 0.0f  // top   
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}

	//Linking Vertex Attribute
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void EngineShader::RenderTri()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
