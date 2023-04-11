#pragma once
#include "EngineMinimal.h"
#include "EngineCore/EngineMesh.h"


class EngineModel : public EngineNameObject
{
public:
	EngineModel();
	~EngineModel();

	inline EngineMesh* CreateMesh(MeshType _meshType)
	{
		EngineMesh* newMesh = new EngineMesh(_meshType);
		meshes.push_back(newMesh);
		return newMesh;
	}

	inline EngineMesh* CreateMesh(MeshType _meshType, std::string_view _name)
	{
		EngineMesh* newMesh =CreateMesh(_meshType);
		newMesh->SetName(_name.data());
		return newMesh;
	}

	inline EngineMesh* FindMesh(std::string_view _name)
	{
		EngineMesh* findMesh = nullptr;
		for (auto iter = meshes.begin(); iter != meshes.end(); ++iter)
		{
			if ((*iter)->GetNameCopy() == _name.data())
			{
				findMesh = (*iter);
				break;
			}
		}
		return findMesh;
	}

	void Render();
private:
	std::list<EngineMesh*> meshes;

	//----------------	Shader Related	----------------------------------------------
private:
	class EngineShader* shader;
public:
	class EngineShader* GetShader()
	{
		return shader;
	}

};

