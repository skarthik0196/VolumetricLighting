#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include <assimp\scene.h>
#include <Shlwapi.h>

namespace Rendering
{
	Material::Material(Model& referenceToModel) : ReferenceToModel(referenceToModel)
	{
	}

	Material::Material(Model& referenceToModel, aiMaterial* material, ID3D11Device2* D3DDevice, const std::string& modelPath) : ReferenceToModel(referenceToModel)
	{

		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		Name = name.C_Str();

		auto pos = modelPath.find_last_of("\\");
		std::string directory = modelPath.substr(0, pos) + "\\";

		for (Texture::TextureType textureType = static_cast<Texture::TextureType>(0); textureType < Texture::TextureType::End; textureType = static_cast<Texture::TextureType>(static_cast<uint32_t>(textureType) + 1))
		{
			aiTextureType mappedTextureType = static_cast<aiTextureType>(TextureTypeMapping[textureType]);
			uint32_t textureCount = material->GetTextureCount(mappedTextureType);
			if (textureCount > 0)
			{
				for(uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
				{
					aiString path;
					if (material->GetTexture(mappedTextureType, textureIndex, &path) == AI_SUCCESS)
					{
						std::string sPath = directory + path.C_Str(); 
						std::wstring wPath = StringToWideString(sPath);
						std::wstring extension = PathFindExtension(wPath.c_str());
						CreateTexture(D3DDevice, wPath, ExtensionFileTypeMapping[extension], textureType);
					}
				}
			}
		}
	}

	Model & Material::GetModel()
	{
		return ReferenceToModel;
	}

	const std::string & Material::GetName() const
	{
		return Name;
	}

	std::map<Texture::TextureType, std::vector<std::shared_ptr<Texture>>>& Material::GetTextures()
	{
		return Textures;
	}

	std::shared_ptr<Texture> Material::GetTexture(Texture::TextureType textureType, uint32_t index)
	{
		return Textures[textureType][index];
	}

	std::shared_ptr<Texture> Material::CreateTexture(ID3D11Device2* D3Ddevice, const std::wstring & filePath, Texture::TextureFileType fileType, Texture::TextureType textureType)
	{
		auto newTexture = std::make_shared<Texture>(filePath, fileType, textureType);
		Textures[textureType].push_back(newTexture);
		newTexture->InitializeTexture(D3Ddevice);

		return newTexture;
	}

	std::vector<std::shared_ptr<Texture>>& Material::GetTexturesofType(const Texture::TextureType & textureType)
	{
		return Textures[textureType];
	}

	std::map<Texture::TextureType, uint32_t> Material::TextureTypeMapping =
	{
		{ Texture::TextureType::Diffuse, aiTextureType_DIFFUSE },
		{ Texture::TextureType::Specular, aiTextureType_SPECULAR },
		{ Texture::TextureType::Ambient, aiTextureType_AMBIENT },
		{ Texture::TextureType::Emissive, aiTextureType_EMISSIVE },
		{ Texture::TextureType::HeightMap, aiTextureType_HEIGHT },
		{ Texture::TextureType::NormalMap, aiTextureType_NORMALS },
		{ Texture::TextureType::SpecularPowerMap, aiTextureType_SHININESS },
		{ Texture::TextureType::DisplacementMap, aiTextureType_DISPLACEMENT },
		{ Texture::TextureType::LightMap, aiTextureType_LIGHTMAP }
	};

	std::unordered_map<std::wstring, Texture::TextureFileType> Material::ExtensionFileTypeMapping =
	{
		{L".dds", Texture::TextureFileType::DDS},
		{L".tga", Texture::TextureFileType::TGA},
		{L".bmp", Texture::TextureFileType::WIC},
		{L".jpeg", Texture::TextureFileType::WIC },
		{L".png", Texture::TextureFileType::WIC },
		{L".tiff", Texture::TextureFileType::WIC },
		{L".gif", Texture::TextureFileType::WIC }
	};
}

std::wstring StringToWideString(const std::string& str)
{
	uint32_t sizeRequired = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<uint32_t>(str.size()), NULL, 0);
	std::wstring wstr(sizeRequired, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<uint32_t>(str.size()), &wstr[0], sizeRequired);
	return wstr;
}
