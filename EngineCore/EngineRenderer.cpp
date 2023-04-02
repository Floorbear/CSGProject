#include "EngineRenderer.h"
#include "EngineShader.h"

EngineRenderer::EngineRenderer():
	shader(nullptr)
{
	shader = new EngineShader();
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

		float vertices[] = {

			 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
			 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
		};

		//Square
		//float vertices[] = {
		//	-0.5f, -0.5f, 0.0f, // left down  
		//	 0.5f, -0.5f, 0.0f, // right down
		//	 -0.5f,  0.5f, 0.0f,// left top
		//	 0.5f,  0.5f, 0.0f  // right top
		//};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}
	//-------------------- Init EBO ----------------------------------
	{
		//glGenBuffers(1, &EBO);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//unsigned int indices[] = {
		//	0, 1, 2,
		//	2, 3, 1
		//};
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	//Linking Vertex Attribute
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	//-------------------- Unbind Buffer ----------------------------------
	{
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

EngineRenderer::~EngineRenderer()
{
	if (shader != nullptr)
	{
		delete shader;
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void EngineRenderer::RenderTriangle()
{
	shader->Use();
	//int uniformLocation = glGetUniformLocation(shaderProgram, "ourColor");
	//float timeValue = (sin(glfwGetTime()) / 0.5f) + 0.5f;
	//glUniform4f(uniformLocation, timeValue, 0.1f, 0.1f, 1.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
