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

struct MeshData
{
	std::vector<Vertex> vertex;
	std::vector<unsigned int> indices;
};

enum class MeshType
{
	Square,
	Triangle,
	Cube,
	Max
};

class InitEngineMesh
{
public:
	InitEngineMesh();
	~InitEngineMesh();

private:

};


class EngineMesh
{
	friend InitEngineMesh;
public:
	EngineMesh();
	~EngineMesh();

	void Render();
	//void RenderSquare();
	//----------------	Mesh Related	----------------------------------------------
public:
	void SetMesh(MeshType _meshType);
	void SetMesh(MeshData _meshData);
	//----------------	Buffer Related	----------------------------------------------
private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	std::vector<Vertex> vertex;
	std::vector<unsigned int> indices;

public:
	static std::vector<MeshData> basicShapes;
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

