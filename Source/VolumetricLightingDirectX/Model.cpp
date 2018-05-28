#include "pch.h"
#include "Model.h"
#include "Material.h"
#include "Mesh.h"
#include <fstream>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <Shlwapi.h>

namespace Rendering
{
	Model::Model(ID3D11Device2 * device, const std::string & filename, bool flipUVs)
	{
		LoadModel(device, filename, flipUVs);
	}

	//bool Model::HasMaterials() const
	//{
	//	return (Materials.size() > 0);
	//}

	bool Model::HasMeshes() const
	{
		return (Meshes.size() > 0);
	}

	const std::vector<std::shared_ptr<Mesh>>& Model::GetMeshes()
	{
		return Meshes;
	}

	//const std::vector<std::shared_ptr<Material>>& Model::GetMaterials()
	//{
	//	return Materials;
	//}

	//Material & Model::CreateMaterial()
	//{
	//	std::shared_ptr<Material> newMaterial = std::make_shared<Material>(*this);
	//	Materials.push_back(newMaterial);
	//	return *newMaterial;
	//}

	void Model::LoadModel(ID3D11Device2* device, const std::string& fileName, bool flipUVs)
	{
		WCHAR PathBuffer[MAX_PATH];
		GetModuleFileName(nullptr, PathBuffer, MAX_PATH);
		PathRemoveFileSpec(PathBuffer);
		std::wstring Pathh(PathBuffer);
		SetCurrentDirectory(Pathh.c_str());

		Assimp::Importer importer;

		uint32_t flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder;
		if (flipUVs)
		{
			flags |= aiProcess_FlipUVs;
		}

		const aiScene* scene = importer.ReadFile(fileName, flags);
		if (scene == nullptr)
		{
			throw std::exception("Invalid File");
		}

		//if (scene->HasMaterials())
		//{
		//	for (uint32_t i = 0; i < scene->mNumMaterials; i++)
		//	{
		//		Materials.push_back(std::make_shared<Material>(Material(*this, scene->mMaterials[i], device)));
		//	}
		//}

		if (scene->HasMeshes())
		{
			for (uint32_t i = 0; i < scene->mNumMeshes; i++)
			{
				Meshes.push_back(std::make_shared<Mesh>(*this, *(scene->mMeshes[i]), *(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]), device));
			}
		}
	}
}