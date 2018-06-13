#pragma once
#include <wrl.h>
#include "Camera.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include "Utility.h"

namespace Rendering
{
	class Direct3D;
	class Scene;

	class SkyBox
	{
	public:
		struct SkyBoxVertexData
		{
			DirectX::XMFLOAT4X4 WorldViewProjection{ Utility::IdentityMatrix };
		};

		SkyBox(ID3D11Device2* device, Camera& camera, const std::wstring& skyBoxTexturePath, Texture::TextureFileType  textureFileType= Texture::TextureFileType::DDS);
		~SkyBox() = default;

		void Update(Scene* currentScene);
		void Draw(std::shared_ptr<Direct3D>& direct3DRenderer, Scene* currentScene, DirectX::XMMATRIX& viewProjectionMatrix);

	private:
		void Initialize(ID3D11Device2* device, const std::wstring& skyBoxTexturePath, Texture::TextureFileType textureFileType = Texture::TextureFileType::DDS );
		Camera& ReferenceToWorldCamera;
		SkyBoxVertexData VSData;

		std::shared_ptr<Model> SkyBoxModel;
		std::shared_ptr<Shader> SkyBoxVertexShader;
		std::shared_ptr<Shader> SkyBoxPixelShader;

		Microsoft::WRL::ComPtr<ID3D11Buffer> VSCBuffer;

		DirectX::XMFLOAT4X4 WorldMatrix;
		DirectX::XMFLOAT4X4 ScalingMatrix;
	};
}