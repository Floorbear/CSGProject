#pragma once
#include "EngineCore/EngineMinimal.h"

struct Vertex
{
	vector3 position;
	//vector3 normal
	//vector2 texcoords
	Vertex(float x, float y, float z)
	{
		position = vector3(x, y, z);
	}
};

enum class MeshType
{
	Square,
	Triangle,
	Cube
};

class EngineMesh
{
public:
	EngineMesh();
	~EngineMesh();

	void Render();
	//void RenderSquare();
	//----------------	Mesh Related	----------------------------------------------
public:
	void SetMesh(MeshType _meshType);
	//----------------	Buffer Related	----------------------------------------------
private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	std::vector<Vertex> vertex;
	std::vector<unsigned int> indices;
	//----------------	Shader Related	----------------------------------------------
private:
	class EngineShader* shader;
public:
	EngineTransform* GetTransform()
	{
		return &transform;
	}
	//----------------	Transform Related	----------------------------------------------
public:
	class EngineShader* GetShader()
	{
		return shader;
	}
private:
	EngineTransform transform;
};

