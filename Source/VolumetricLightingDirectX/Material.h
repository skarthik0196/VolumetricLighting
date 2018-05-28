#pragma once
#include "Model.h"
#include "Texture.h"

struct aiMaterial;

namespace Rendering
{
	class Material
	{
	public:

		friend class Model;

		Material();
		Material(aiMaterial* material, ID3D11Device2* D3DDevice);
		Material(const Material& rhs) = default;
		Material(Material&& rhs) = default;
		~Material() = default;

		Material& operator=(const Material& rhs) = default;
		Material& operator=(Material&& rhs) = default;

		//Model& GetModel();

		const std::string& GetName() const;
		std::map<Texture::TextureType, std::vector<std::shared_ptr<Texture>>>& GetTextures();
		std::shared_ptr<Texture> GetTexture(Texture::TextureType textureType, uint32_t index);

		std::shared_ptr<Texture> CreateTexture(ID3D11Device2* D3Ddevice, const std::wstring& filePath, Texture::TextureFileType fileType, Texture::TextureType textureType = Texture::TextureType::Diffuse);
		std::vector<std::shared_ptr<Texture>>& GetTexturesofType(const Texture::TextureType& textureType);

	private:
		/// <summary>
		/// Map TextureType Enum to Assimp's types
		/// </summary>
		static std::map<Texture::TextureType, uint32_t> TextureTypeMapping;

		std::map<Texture::TextureType, std::vector<std::shared_ptr<Texture>>> Textures;
		std::string Name;
	};
}