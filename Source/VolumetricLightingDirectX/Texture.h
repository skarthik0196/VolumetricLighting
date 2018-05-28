#pragma once
#include <wrl.h>

namespace Rendering
{
	class Texture
	{
	public:

		enum class TextureType
		{
			Diffuse = 0,
			Specular,
			Ambient,
			Emissive,
			HeightMap,
			NormalMap,
			SpecularPowerMap,
			DisplacementMap,
			LightMap,
			End
		};

		enum class TextureFileType
		{
			DDS,
			WIC
		};

		Texture(const std::wstring& filePath, TextureFileType fileType = TextureFileType::WIC, TextureType textureType = TextureType::Diffuse);
		~Texture() = default;

		TextureType GetTextureType() const;
		TextureFileType GetTextureFileType() const;

		void SetTextureType(TextureType textureType);
		void SetTextureFileType(TextureFileType fileType);

		ID3D11ShaderResourceView* GetShaderResourceView();
		ID3D11ShaderResourceView** GetAddressOfShaderResourceView();

		const std::wstring& GetFilePath();
		void SetFilePath(const std::wstring& filePath);

		void InitializeTexture(ID3D11Device2* device);

	private:

		std::wstring FilePath;
		TextureType Type;
		TextureFileType FileType;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;

	};
}