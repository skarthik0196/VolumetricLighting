#include "pch.h"
#include "SkyBox.h"
#include "Mesh.h"
#include "Material.h"
#include "Scene.h"
#include "Direct3D.h"

namespace Rendering
{

	SkyBox::SkyBox(ID3D11Device2 * device, Camera& camera, const std::wstring & skyBoxTexturePath, Texture::TextureFileType textureFileType) : ReferenceToWorldCamera(camera), SkyBoxVertexShader(std::make_shared<Shader>(L"SkyBoxVertexShader.cso", Shader::ShaderType::VertexShader, device)), SkyBoxPixelShader(std::make_shared<Shader>(L"SkyBoxPixelShader.cso", Shader::ShaderType::PixelShader, device))
	{
		Initialize(device, skyBoxTexturePath, textureFileType);
	}

	void SkyBox::Update(Scene * currentScene)
	{
		UNREFERENCED_PARAMETER(currentScene);
		auto position = ReferenceToWorldCamera.GetPosition();
		DirectX::XMStoreFloat4x4(&WorldMatrix, DirectX::XMLoadFloat4x4(&ScalingMatrix) * DirectX::XMMatrixTranslationFromVector(position));
	}

	void SkyBox::Draw(std::shared_ptr<Direct3D>& direct3DRenderer, Scene* currentScene, DirectX::XMMATRIX& viewProjectionMatrix)
	{
		UNREFERENCED_PARAMETER(currentScene);

		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;
		auto& mesh = SkyBoxModel->GetMeshes()[0];

		auto WVP = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&WorldMatrix), viewProjectionMatrix);
		DirectX::XMStoreFloat4x4(&VSData.WorldViewProjection, DirectX::XMMatrixTranspose(WVP));

		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		deviceContext->IASetVertexBuffers(0, 1, mesh->GetAddressOfVertexBuffer(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		deviceContext->VSSetShader(SkyBoxVertexShader->GetVertexShader(), nullptr, 0);
		deviceContext->PSSetShader(SkyBoxPixelShader->GetPixelShader(), nullptr, 0);

		deviceContext->VSSetConstantBuffers(0, 1, VSCBuffer.GetAddressOf());
		deviceContext->UpdateSubresource(VSCBuffer.Get(), 0, nullptr, &VSData, 0, 0);
		deviceContext->PSSetShaderResources(0, 1, mesh->GetMaterial()->GetTextures()[Texture::TextureType::Diffuse][0]->GetAddressOfShaderResourceView());

		direct3DRenderer->DisableDepthWriting();
		direct3DRenderer->SetRasterizerState(Direct3D::RasterizerState::DisableCulling);

		deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);

		direct3DRenderer->SetRasterizerState(Direct3D::RasterizerState::BackFaceCulling);
		direct3DRenderer->EnableDepthTesting();

	}

	void SkyBox::Initialize(ID3D11Device2* device, const std::wstring & skyBoxTexturePath, Texture::TextureFileType textureFileType)
	{
		SkyBoxModel = std::make_shared<Model>(device, R"(Content\Models\Sphere.obj)", true);
		SkyBoxModel->GetMeshes()[0]->GetMaterial()->CreateTexture(device, skyBoxTexturePath, textureFileType);

		D3D11_BUFFER_DESC constantBufferDescription;
		ZeroMemory(&constantBufferDescription, sizeof(constantBufferDescription));

		constantBufferDescription.ByteWidth = sizeof(SkyBoxVertexData);
		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		device->CreateBuffer(&constantBufferDescription, nullptr, VSCBuffer.GetAddressOf());

		auto scale = ReferenceToWorldCamera.GetFarPlane() - 10.0f;
		DirectX::XMStoreFloat4x4(&ScalingMatrix, DirectX::XMMatrixScaling(scale, scale, scale));
	}
}