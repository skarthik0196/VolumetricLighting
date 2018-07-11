#include "pch.h"
#include "CascadedShadowMap.h"
#include "Direct3D.h"
#include "Camera.h"
#include "Scene.h"
#include "Mesh.h"

namespace Rendering
{
	CascadedShadowMap::CascadedShadowMap(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<DirectionalLight>& light) : ShadowMapLight(light), ShadowMapVertexShader(std::make_shared<Shader>(L"CascadedShadowMapVertexShader.cso", Shader::ShaderType::VertexShader, direct3DRenderer->GetDevice())),
																																	ShadowMapPixelShader(std::make_shared<Shader>(L"CascadedShadowMapPixelShader.cso", Shader::ShaderType::PixelShader, direct3DRenderer->GetDevice()))
	{
		Initialize(direct3DRenderer);
	}

	void CascadedShadowMap::RenderShadowMap(std::shared_ptr<Direct3D>& direct3DRenderer, Scene * currentScene)
	{
		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();
		auto& camera = currentScene->GetCamera();

		direct3DRenderer->EnableDepthTesting();
		deviceContext->RSSetViewports(1, &ShadowMapViewPort);

		deviceContext->ClearDepthStencilView(ShadowMapDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		deviceContext->RSSetState(ShadowMapRasterizerState.Get());
		deviceContext->VSSetShader(ShadowMapVertexShader->GetVertexShader(), 0, 0);
		deviceContext->PSSetShader(NULL, 0, 0);

		ID3D11ShaderResourceView* nullPSResouce[] = { nullptr,nullptr,nullptr, nullptr };
		deviceContext->PSSetShaderResources(0, 4, nullPSResouce);

		deviceContext->OMSetRenderTargets(1, ShadowMapRenderTargetView[0].GetAddressOf(), ShadowMapDepthStencilView.Get());
		deviceContext->IASetInputLayout(ShadowMapVertexShader->GetInputLayout());

		deviceContext->VSSetConstantBuffers(0, 1, VSCBuffer.GetAddressOf());

		auto viewProjection = camera->GetPartitionProjectionMatrixByIndex(0);
		auto viewProjection2 = camera->GetPartitionProjectionMatrixByIndex(1);
		auto viewProjection3 = camera->GetPartitionProjectionMatrixByIndex(2);

		CalculateFrustumData(viewProjection, 0);
		CalculateFrustumData(viewProjection2, 1);
		CalculateFrustumData(viewProjection3, 2);


		auto& gameObjectList = currentScene->GetGameObjectList();

		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;

		for (auto& gameObject : gameObjectList)
		{
			auto& meshList = gameObject->GetObjectModel()->GetMeshes();
			auto worldMatrix = gameObject->GetWorldMatrix();
			auto wvp = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMLoadFloat4x4(&VSData.LightWorldViewProjectionMatrix[0]));

			DirectX::XMStoreFloat4x4(&VSData.LightWorldViewProjectionMatrix[0], DirectX::XMMatrixTranspose(wvp));

			wvp = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMLoadFloat4x4(&VSData.LightWorldViewProjectionMatrix[1]));
			DirectX::XMStoreFloat4x4(&VSData.LightWorldViewProjectionMatrix[1], DirectX::XMMatrixTranspose(wvp));

			wvp = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMLoadFloat4x4(&VSData.LightWorldViewProjectionMatrix[2]));
			DirectX::XMStoreFloat4x4(&VSData.LightWorldViewProjectionMatrix[2], DirectX::XMMatrixTranspose(wvp));
			deviceContext->UpdateSubresource(VSCBuffer.Get(), 0, nullptr, &VSData, 0, 0);

			for (auto& mesh : meshList)
			{
				deviceContext->IASetVertexBuffers(0, 1, mesh->GetAddressOfVertexBuffer(), &stride, &offset);
				deviceContext->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
				deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
			}
		}

		direct3DRenderer->SetRasterizerState(Direct3D::RasterizerState::BackFaceCulling);

	}

	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& CascadedShadowMap::GetShaderResourceView()
	{
		return ShadowMapResourceView;
	}

	uint32_t CascadedShadowMap::GetCascadeAmount()
	{
		return CascadeAmount;
	}

	void CascadedShadowMap::CalculateFrustumData(DirectX::XMMATRIX& cameraViewProjectionMatrix, uint32_t cascadeIndex)
	{
		DirectX::XMFLOAT3 frustumVertices[8] =
		{
			DirectX::XMFLOAT3(-1.0f,1.0f,0.0f),
			DirectX::XMFLOAT3(1.0f,1.0f,0.0f),
			DirectX::XMFLOAT3(1.0f,-1.0f,0.0f),
			DirectX::XMFLOAT3(-1.0f,-1.0f,0.0f),

			DirectX::XMFLOAT3(-1.0f,1.0f,1.0f),
			DirectX::XMFLOAT3(1.0f,1.0f,1.0f),
			DirectX::XMFLOAT3(1.0f,-1.0f,1.0f),
			DirectX::XMFLOAT3(-1.0f,-1.0f,1.0f),
		};

		auto inverseProjectionMatrix = DirectX::XMMatrixInverse(nullptr, cameraViewProjectionMatrix);
		DirectX::XMFLOAT3 frustumCenter = Utility::Zero;

		for (uint32_t i = 0; i < std::size(frustumVertices); ++i)
		{
			DirectX::XMStoreFloat3(&frustumVertices[i], DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&frustumVertices[i]), inverseProjectionMatrix));
			DirectX::XMStoreFloat3(&frustumCenter, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&frustumCenter), DirectX::XMLoadFloat3(&frustumVertices[i])));
		}

		frustumCenter.x = (frustumCenter.x / 8.0f);
		frustumCenter.y = (frustumCenter.y / 8.0f);
		frustumCenter.z = (frustumCenter.z / 8.0f);

		float radius = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&frustumVertices[0]), DirectX::XMLoadFloat3(&frustumVertices[6]))));

		//Do Texel Snapping here
		
		//float texelsPerUnit = 2048 / (radius*2.0f);
		//auto scalingMatrix = DirectX::XMMatrixScaling(texelsPerUnit, texelsPerUnit, texelsPerUnit);

		//End TexelSnapping

		auto eyePosition = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&frustumCenter), DirectX::XMVectorScale(ShadowMapLight->GetDirection(), radius * 2.0f));

		auto viewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, DirectX::XMLoadFloat3(&frustumCenter), DirectX::XMLoadFloat3(&Utility::Up));
		auto projectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(-radius, radius, -radius, radius, 1.0f, radius * 6.0f);
		DirectX::XMStoreFloat4x4(&VSData.LightWorldViewProjectionMatrix[cascadeIndex], DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(viewMatrix, projectionMatrix)));
	}

	void CascadedShadowMap::Initialize(std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ShadowMapRenderTargetView.reserve(CascadeAmount);
		DepthTexture.reserve(CascadeAmount);
		ShadowMapResourceView.reserve(CascadeAmount);

		ID3D11Device2* device = direct3DRenderer->GetDevice();

		D3D11_TEXTURE2D_DESC depthTextureDescription{ 0 };

		depthTextureDescription.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthTextureDescription.Width = 2048;
		depthTextureDescription.Height = 2048;
		depthTextureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthTextureDescription.MipLevels = 1;
		depthTextureDescription.ArraySize = 1;
		depthTextureDescription.SampleDesc.Count = 1;
		depthTextureDescription.SampleDesc.Quality = 0;
		depthTextureDescription.Usage = D3D11_USAGE_DEFAULT;

		for (uint32_t i = 0; i < CascadeAmount; ++i)
		{
			device->CreateTexture2D(&depthTextureDescription, nullptr, DepthTexture[i].ReleaseAndGetAddressOf());
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
		ZeroMemory(&depthStencilViewDescription, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

		depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDescription.Texture2D.MipSlice = 0;

		device->CreateDepthStencilView(DepthTexture[0].Get(), &depthStencilViewDescription, ShadowMapDepthStencilView.ReleaseAndGetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC depthShaderResourceViewDescription;
		ZeroMemory(&depthShaderResourceViewDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		depthShaderResourceViewDescription.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		depthShaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthShaderResourceViewDescription.Texture2D.MipLevels = depthTextureDescription.MipLevels;
		depthShaderResourceViewDescription.Texture2D.MostDetailedMip = 0;

		for (uint32_t i = 0; i < CascadeAmount; ++i)
		{
			device->CreateShaderResourceView(DepthTexture[i].Get(), &depthShaderResourceViewDescription, ShadowMapResourceView[i].ReleaseAndGetAddressOf());
		}

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

		ShadowMapViewPort.Height = 2048;
		ShadowMapViewPort.Width = 2048;
		ShadowMapViewPort.MaxDepth = 1.0f;
		ShadowMapViewPort.MinDepth = 0.0f;
		ShadowMapViewPort.TopLeftX = 0;
		ShadowMapViewPort.TopLeftY = 0;
	}

}