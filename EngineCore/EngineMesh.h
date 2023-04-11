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


class EngineMesh : public EngineNameObject
{
	friend InitEngineMesh;
public:
	EngineMesh(MeshType _meshType = MeshType::Square);
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

public:

	//----------------	Transform Related	----------------------------------------------
public:
	EngineTransform* GetTransform()
	{
		return &transform;
	}

private:
	EngineTransform transform;

};

