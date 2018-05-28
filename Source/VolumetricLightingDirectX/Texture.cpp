#include "pch.h"
#include "Texture.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

namespace Rendering
{
	Rendering::Texture::Texture(const std::wstring& filePath, TextureFileType fileType, TextureType textureType) : FilePath(filePath), FileType(fileType), Type(textureType)
	{
	}

	Texture::TextureType Texture::GetTextureType() const
	{
		return Type;
	}

	Texture::TextureFileType Texture::GetTextureFileType() const
	{
		return FileType;
	}

	void Texture::SetTextureType(TextureType textureType)
	{
		Type = textureType;
	}

	void Texture::SetTextureFileType(TextureFileType fileType)
	{
		FileType = fileType;
	}

	ID3D11ShaderResourceView* Texture::GetShaderResourceView()
	{
		return ShaderResourceView.Get();
	}

	ID3D11ShaderResourceView** Texture::GetAddressOfShaderResourceView()
	{
		return ShaderResourceView.GetAddressOf();
	}

	const std::wstring& Texture::GetFilePath()
	{
		return FilePath;
	}

	void Texture::SetFilePath(const std::wstring & filePath)
	{
		FilePath = filePath;
	}

	void Texture::InitializeTexture(ID3D11Device2* device)
	{
		if (FileType == TextureFileType::DDS)
		{
			DirectX::CreateDDSTextureFromFile(device, FilePath.c_str(), nullptr, ShaderResourceView.GetAddressOf());
		}
		else
		{
			DirectX::CreateWICTextureFromFile(device, FilePath.c_str(), nullptr, ShaderResourceView.GetAddressOf());
		}
	}
}