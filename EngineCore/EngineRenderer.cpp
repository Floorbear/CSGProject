#include "EngineModel.h"
#include "EngineShader.h"

EngineModel::EngineModel():
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

		//vertex & color
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

EngineModel::~EngineModel()
{
	if (shader != nullptr)
	{
		delete shader;
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void EngineModel::RenderTriangle()
{
	shader->Use();
	//transform.AddLocalRotation(glm::vec3(1.0f, 0.f, 0.f)); //매 프레임 x축을 기준으로 1도 회전함
	//transform.SetLocalPosition(glm::vec3(sin(glfwGetTime()), 0, 0)); // 좌우로 이동
	//transform.SetLocalScale(glm::vec3(cos(glfwGetTime()), cos(glfwGetTime()), 0)); //커졌다 작아졌다.

	int transformLocation = glGetUniformLocation(shader->GetShaderProgram(),"transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(*transform.GetTransformMat()));


	

	//int uniformLocation = glGetUniformLocation(shaderProgram, "ourColor");
	//float timeValue = (sin(glfwGetTime()) / 0.5f) + 0.5f;
	//glUniform4f(uniformLocation, timeValue, 0.1f, 0.1f, 1.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
