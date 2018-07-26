#include "pch.h"
#include "VolumetricLight.h"
#include "Scene.h"
#include "Direct3D.h"

namespace Rendering
{

	VolumetricLight::VolumetricLight(std::shared_ptr<Direct3D>& direct3DRenderer, const std::wstring & postProcessPixelShader) : PostProcessing(direct3DRenderer->GetDevice(), postProcessPixelShader)
									, RenderTexturePixelShader(std::make_shared<Shader>(L"RenderTexturePixelShader.cso", Shader::ShaderType::PixelShader, direct3DRenderer->GetDevice()))
	{
		Initialize(direct3DRenderer);
	}

	VolumetricLight::VolumetricLight(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring & postProcessPixelShader) : PostProcessing(direct3DRenderer->GetDevice(), screenQuad, postProcessPixelShader)
									, RenderTexturePixelShader(std::make_shared<Shader>(L"RenderTexturePixelShader.cso", Shader::ShaderType::PixelShader, direct3DRenderer->GetDevice()))
	{
		Initialize(direct3DRenderer);
	}

	void VolumetricLight::ApplyPostProcessing(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		CreateVolumetrics(scene, direct3DRenderer);
		BlendVolumetrics(scene, direct3DRenderer);
	}

	void VolumetricLight::SetShadowMap(std::shared_ptr<ShadowMap>& sMap)
	{
		SMap = sMap;
	}

	const std::shared_ptr<ShadowMap>& VolumetricLight::GetShadowMap() const
	{
		return SMap;
	}

	void VolumetricLight::SetGScattering(float GScattering)
	{
		PSData.GScattering = GScattering;
	}

	float VolumetricLight::GetGScattering() const
	{
		return PSData.GScattering;
	}

	void VolumetricLight::SetScatteringAmount(float scatterAmount)
	{
		PSData.ScatteringAmount = scatterAmount;
	}

	float VolumetricLight::GetScatteringAmount() const
	{
		return PSData.ScatteringAmount;
	}

	void VolumetricLight::CreateVolumetrics(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		PSData.CameraPosition = scene->GetCamera()->GetPositionAsFloat3();

		DirectX::XMStoreFloat4x4(&PSData.LightViewProjectionMatrix, DirectX::XMMatrixTranspose(SMap->GetShadowMapLightSource()->GetViewProjectionMatrix()));
		PSData.LightColor = SMap->GetShadowMapLightSource()->GetBaseColor();
		PSData.LightDirection = SMap->GetShadowMapLightSource()->GetDirectionAsFloat3();

		ScreenQuadData->BindScreenQuadVertexShader(deviceContext);
		deviceContext->UpdateSubresource(PSCBuffer.Get(), 0, nullptr, &PSData, 0, 0);
		deviceContext->PSSetShader(PostProcessingPixelShader->GetPixelShader(), nullptr, 0);
		//deviceContext->RSSetViewports(1, &VolumtericLightViewPort);

		ID3D11ShaderResourceView* shaderResources[] = { scene->GetGBuffer()->GetShaderResourceView(GBuffer::GBufferData::WorldPosition), SMap->GetShaderResourceView() };

		deviceContext->PSSetShaderResources(0, ARRAYSIZE(shaderResources), shaderResources);
		deviceContext->PSSetConstantBuffers(0, 1, PSCBuffer.GetAddressOf());

		deviceContext->ClearRenderTargetView(VolumetricLightRenderTarget.Get(), reinterpret_cast<float*>(&Utility::BackgroundColor));
		deviceContext->OMSetRenderTargets(1, VolumetricLightRenderTarget.GetAddressOf(), nullptr);

		ScreenQuadData->DrawScreenQuad(deviceContext);
	}

	void VolumetricLight::BlendVolumetrics(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		scene;
		direct3DRenderer;

		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		//direct3DRenderer->SetViewPort();
		direct3DRenderer->SetToneMappingRenderTarget();
		direct3DRenderer->EnableAdditiveBlending();

		deviceContext->PSSetShader(RenderTexturePixelShader->GetPixelShader(), 0, 0);
		deviceContext->PSSetShaderResources(0, 1, VolumetricLightResourceView.GetAddressOf());

		ScreenQuadData->DrawScreenQuad(deviceContext);

		direct3DRenderer->DisableBlending();

	}

	void VolumetricLight::Initialize(std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ID3D11Device* device = direct3DRenderer->GetDevice();

		D3D11_TEXTURE2D_DESC textureDescription;
		ZeroMemory(&textureDescription, sizeof(D3D11_TEXTURE2D_DESC));

		textureDescription.Width = static_cast<uint32_t>(direct3DRenderer->GetScreenWidth());
		textureDescription.Height = static_cast<uint32_t>(direct3DRenderer->GetScreenHeight());
		textureDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDescription.Usage = D3D11_USAGE_DEFAULT;
		textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDescription.MipLevels = 1;
		textureDescription.ArraySize = 1;
		textureDescription.SampleDesc.Count = direct3DRenderer->GetMultiSamplingCount();
		textureDescription.SampleDesc.Quality = direct3DRenderer->GetMultiSamplingQualityLevels() - 1;

		device->CreateTexture2D(&textureDescription, nullptr, VolumetricLightTexture.ReleaseAndGetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC resourceDescription;
		ZeroMemory(&resourceDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		resourceDescription.Format = textureDescription.Format;
		resourceDescription.Texture2D.MostDetailedMip = 0;
		resourceDescription.Texture2D.MipLevels = 1;
		resourceDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		device->CreateShaderResourceView(VolumetricLightTexture.Get(), &resourceDescription, VolumetricLightResourceView.ReleaseAndGetAddressOf());

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetDescription;
		ZeroMemory(&renderTargetDescription, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		renderTargetDescription.Format = textureDescription.Format;
		renderTargetDescription.Texture2D.MipSlice = 0;
		renderTargetDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		device->CreateRenderTargetView(VolumetricLightTexture.Get(), &renderTargetDescription, VolumetricLightRenderTarget.ReleaseAndGetAddressOf());

		D3D11_BUFFER_DESC bufferDescription;
		ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));

		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.ByteWidth = sizeof(VolumetricLightPixelData);
		bufferDescription.MiscFlags = 0;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;

		device->CreateBuffer(&bufferDescription, nullptr, PSCBuffer.ReleaseAndGetAddressOf());

		VolumtericLightViewPort.Height = direct3DRenderer->GetScreenHeight() / 2;
		VolumtericLightViewPort.Width = direct3DRenderer->GetScreenWidth() / 2;
		VolumtericLightViewPort.TopLeftX = 0;
		VolumtericLightViewPort.TopLeftY = 0;
		VolumtericLightViewPort.MinDepth = 0.0f;
		VolumtericLightViewPort.MaxDepth = 1.0f;
	}
}