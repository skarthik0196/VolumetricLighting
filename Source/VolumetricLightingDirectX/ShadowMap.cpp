#include "pch.h"
#include "ShadowMap.h"
#include "Direct3D.h"
#include "GameObject.h"
#include "Scene.h"
#include "Mesh.h"

namespace Rendering
{
	Rendering::ShadowMap::ShadowMap(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<DirectionalLight>& light) : ShadowMapVertexShader(std::make_shared<Shader>(L"ShadowMapVertexShader.cso", Shader::ShaderType::VertexShader, direct3DRenderer->GetDevice())), ShadowMapLight(light),
																															 ShadowMapPixelShader(std::make_shared<Shader>(L"ShadowMapPixelShader.cso", Shader::ShaderType::PixelShader, direct3DRenderer->GetDevice()))
	{
		Initialize(direct3DRenderer);
	}

	void ShadowMap::RenderShadowMap(std::shared_ptr<Direct3D>& direct3DRenderer, Scene * currentScene)
	{
		if (ShadowMapLight->HasBeenUpdated())
		{
			ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

			direct3DRenderer->EnableDepthTesting();
			deviceContext->RSSetViewports(1, &ShadowMapViewPort);

			deviceContext->ClearDepthStencilView(ShadowMapDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			deviceContext->RSSetState(ShadowMapRasterizerState.Get());
			deviceContext->VSSetShader(ShadowMapVertexShader->GetVertexShader(), 0, 0);
			deviceContext->PSSetShader(NULL, 0, 0);

			ID3D11ShaderResourceView* nullPSResouce[] = { nullptr,nullptr,nullptr, nullptr };
			deviceContext->PSSetShaderResources(0, 4, nullPSResouce);
			ID3D11RenderTargetView* nullRT = { 0 };

			deviceContext->OMSetRenderTargets(1, &nullRT, ShadowMapDepthStencilView.Get());
			deviceContext->IASetInputLayout(ShadowMapVertexShader->GetInputLayout());

			deviceContext->VSSetConstantBuffers(0, 1, VSCBuffer.GetAddressOf());

			auto viewProjection = ShadowMapLight->GetViewProjectionMatrix();
			auto& gameObjectList = currentScene->GetGameObjectList();

			uint32_t stride = sizeof(Vertex);
			uint32_t offset = 0;

			for (auto& gameObject : gameObjectList)
			{
				auto& meshList = gameObject->GetObjectModel()->GetMeshes();
				auto worldMatrix = gameObject->GetWorldMatrix();
				auto wvp = DirectX::XMMatrixMultiply(worldMatrix, viewProjection);

				DirectX::XMStoreFloat4x4(&VSData.LightWorldViewProjectionMatrix, DirectX::XMMatrixTranspose(wvp));
				deviceContext->UpdateSubresource(VSCBuffer.Get(), 0, nullptr, &VSData, 0, 0);

				for (auto& mesh : meshList)
				{
					deviceContext->IASetVertexBuffers(0, 1, mesh->GetAddressOfSimpleVertexBuffer(), &stride, &offset);
					deviceContext->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
					deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
				}
			}

			direct3DRenderer->SetRasterizerState(Direct3D::RasterizerState::BackFaceCulling);
		}
	}

	std::shared_ptr<DirectionalLight>& ShadowMap::GetShadowMapLightSource()
	{
		return ShadowMapLight;
	}

	ID3D11ShaderResourceView* ShadowMap::GetShaderResourceView()
	{
		return ShadowMapResourceView.Get();
	}

	ID3D11ShaderResourceView ** ShadowMap::GetAddressOfShaderResourceView()
	{
		return ShadowMapResourceView.GetAddressOf();
	}

	void ShadowMap::Initialize(std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ID3D11Device2* device = direct3DRenderer->GetDevice();

		D3D11_TEXTURE2D_DESC depthTextureDescription{ 0 };

		depthTextureDescription.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthTextureDescription.Width = ShadowMapResolution;// static_cast<uint32_t>(direct3DRenderer->GetScreenWidth());
		depthTextureDescription.Height = ShadowMapResolution;// static_cast<uint32_t>(direct3DRenderer->GetScreenHeight());
		depthTextureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthTextureDescription.MipLevels = 1;
		depthTextureDescription.ArraySize = 1;
		depthTextureDescription.SampleDesc.Count = 1;
		depthTextureDescription.SampleDesc.Quality = 0;
		depthTextureDescription.Usage = D3D11_USAGE_DEFAULT;

		device->CreateTexture2D(&depthTextureDescription, nullptr, DepthTexture.ReleaseAndGetAddressOf());

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
		ZeroMemory(&depthStencilViewDescription, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

		depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDescription.Texture2D.MipSlice = 0;

		device->CreateDepthStencilView(DepthTexture.Get(), &depthStencilViewDescription, ShadowMapDepthStencilView.ReleaseAndGetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC depthShaderResourceViewDescription;
		ZeroMemory(&depthShaderResourceViewDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		depthShaderResourceViewDescription.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		depthShaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthShaderResourceViewDescription.Texture2D.MipLevels = depthTextureDescription.MipLevels;
		depthShaderResourceViewDescription.Texture2D.MostDetailedMip = 0;

		device->CreateShaderResourceView(DepthTexture.Get(), &depthShaderResourceViewDescription, ShadowMapResourceView.ReleaseAndGetAddressOf());
		
		D3D11_BUFFER_DESC constantBufferDescription;
		ZeroMemory(&constantBufferDescription, sizeof(D3D11_BUFFER_DESC));

		constantBufferDescription.ByteWidth = sizeof(VSCBufferPerObject);
		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		device->CreateBuffer(&constantBufferDescription, nullptr, VSCBuffer.ReleaseAndGetAddressOf());

		D3D11_RASTERIZER_DESC rasterizerDescription;
		ZeroMemory(&rasterizerDescription, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerDescription.CullMode = D3D11_CULL_BACK;
		rasterizerDescription.FillMode = D3D11_FILL_SOLID;
		rasterizerDescription.DepthClipEnable = true;

		device->CreateRasterizerState(&rasterizerDescription, ShadowMapRasterizerState.ReleaseAndGetAddressOf());

		ShadowMapViewPort.Height = ShadowMapResolution;
		ShadowMapViewPort.Width = ShadowMapResolution;
		ShadowMapViewPort.MaxDepth = 1.0f;
		ShadowMapViewPort.MinDepth = 0.0f;
		ShadowMapViewPort.TopLeftX = 0;
		ShadowMapViewPort.TopLeftY = 0;
	}
}