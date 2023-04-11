#include "EngineMesh.h"
#include "EngineShader.h"

std::vector<MeshData> EngineMesh::basicShapes;

InitEngineMesh Inst = InitEngineMesh();

InitEngineMesh::InitEngineMesh()
{
	EngineMesh::basicShapes.resize(static_cast<int>(MeshType::Max));

	//square
	EngineMesh::basicShapes[static_cast<int>(MeshType::Square)].indices = { 0, 1, 2, 2, 3, 1 };
	EngineMesh::basicShapes[static_cast<int>(MeshType::Square)].vertex =
	{
		Vertex(-0.5f, -0.5f, 0.0f), // left down  
		Vertex(0.5f, -0.5f, 0.0f), // right down
		Vertex(-0.5f,  0.5f, 0.0f), // left top
		Vertex(0.5f,  0.5f, 0.0f) // right top
	};

	//triangle
	EngineMesh::basicShapes[static_cast<int>(MeshType::Triangle)].indices = { 0, 1, 2 };
	EngineMesh::basicShapes[static_cast<int>(MeshType::Triangle)].vertex =
	{
		Vertex(0.5f, -0.5f, 0.0f),
		Vertex(-0.5f, -0.5f, 0.0f),
		Vertex(0.0f,  0.5f, 0.0f)
	};

	//cube
	EngineMesh::basicShapes[static_cast<int>(MeshType::Cube)].indices = {
		0, 1, 3, 3, 1, 2,
		1, 5, 2, 2, 5, 6,
		5, 4, 6, 6, 4, 7,
		4, 0, 7, 7, 0, 3,
		3, 2, 7, 7, 2, 6,
		4, 5, 0, 0, 5, 1 };
	EngineMesh::basicShapes[static_cast<int>(MeshType::Cube)].vertex =
	{
			Vertex(-1, -1, -1),
			Vertex(1, -1, -1),
			Vertex(1, 1, -1),
			Vertex(-1, 1, -1),
			Vertex(-1, -1, 1),
			Vertex(1, -1, 1),
			Vertex(1, 1, 1),
			Vertex(-1, 1, 1)
	};

}

InitEngineMesh::~InitEngineMesh()
{
}

EngineMesh::EngineMesh():
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


		vertex = basicShapes[static_cast<int>(MeshType::Square)].vertex;
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);
	}
	//-------------------- Init EBO ----------------------------------
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		indices = basicShapes[static_cast<int>(MeshType::Square)].indices;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	}

	//Linking Vertex Attribute
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)0);

		//Vertex Color Data
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	//-------------------- Unbind Buffer ----------------------------------
	{
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

EngineMesh::~EngineMesh()
{
	if (shader != nullptr)
	{
		delete shader;
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void EngineMesh::Render()
{
	shader->Use();
	transform.AddLocalRotation(glm::vec3(0.05f, 0.f, 0.f)); 
	//transform.SetLocalPosition(glm::vec3(sin(glfwGetTime()), 0, 0)); //
	//transform.SetLocalScale(glm::vec3(cos(glfwGetTime()), cos(glfwGetTime()), 0)); //

	int transformLocation = glGetUniformLocation(shader->GetShaderProgram(),"transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(*transform.GetTransformMat()));
	

	//int uniformLocation = glGetUniformLocation(shaderProgram, "ourColor");
	//float timeValue = (sin(glfwGetTime()) / 0.5f) + 0.5f;
	//glUniform4f(uniformLocation, timeValue, 0.1f, 0.1f, 1.0f);
	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

}

void EngineMesh::SetMesh(MeshType _meshType)
{
	vertex = basicShapes[static_cast<int>(_meshType)].vertex;
	indices = basicShapes[static_cast<int>(_meshType)].indices;

	glBindVertexArray(VAO);

	//버퍼 바인드
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EngineMesh::SetMesh(MeshData _meshData)
{
	vertex = _meshData.vertex;
	indices = _meshData.indices;

	glBindVertexArray(VAO);

	//버퍼 바인드
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
