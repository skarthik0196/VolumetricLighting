#include "pch.h"
#include "Texture.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <DirectXTex.h>

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

		switch (FileType)
		{
			case TextureFileType::DDS:
			{
				DirectX::CreateDDSTextureFromFile(device, FilePath.c_str(), nullptr, ShaderResourceView.GetAddressOf());
				break;
			}
			case TextureFileType::WIC:
			{
				DirectX::CreateWICTextureFromFile(device, FilePath.c_str(), nullptr, ShaderResourceView.GetAddressOf());
				break;
			}
			case TextureFileType::TGA:
			{
				auto image = std::make_unique<DirectX::ScratchImage>();
				HRESULT hr = LoadFromTGAFile(FilePath.c_str(), nullptr, *image);
				if (FAILED(hr))
				{
					throw std::runtime_error("Texture Error");
				}

				DirectX::CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), image->GetMetadata(), ShaderResourceView.GetAddressOf());
				image->Release();
				break;
			}
		}
	}
}