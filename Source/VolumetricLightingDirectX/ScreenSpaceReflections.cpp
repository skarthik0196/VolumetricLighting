#include "pch.h"
#include "ScreenSpaceReflections.h"

namespace Rendering
{
	ScreenSpaceReflections::ScreenSpaceReflections(std::shared_ptr<Direct3D>& direct3DRenderer, const std::wstring & postProcessPixelShader) : PostProcessing(direct3DRenderer->GetDevice(), postProcessPixelShader)
	{
		Initialize(direct3DRenderer);
	}

	ScreenSpaceReflections::ScreenSpaceReflections(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring & postProcessPixelShader) : PostProcessing(direct3DRenderer->GetDevice(), screenQuad, postProcessPixelShader)
	{
		Initialize(direct3DRenderer);
	}

	void ScreenSpaceReflections::ApplyPostProcessing(Scene * scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		scene;
		direct3DRenderer;
	}

	void ScreenSpaceReflections::Initialize(std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ID3D11Device2* device = direct3DRenderer->GetDevice();

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

		device->CreateTexture2D(&textureDescription, nullptr, SSRTexture.ReleaseAndGetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC resourceDescription;
		ZeroMemory(&resourceDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		resourceDescription.Format = textureDescription.Format;
		resourceDescription.Texture2D.MostDetailedMip = 0;
		resourceDescription.Texture2D.MipLevels = 1;
		resourceDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		device->CreateShaderResourceView(SSRTexture.Get(), &resourceDescription, SSRResource.ReleaseAndGetAddressOf());

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetDescription;
		ZeroMemory(&renderTargetDescription, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		renderTargetDescription.Format = textureDescription.Format;
		renderTargetDescription.Texture2D.MipSlice = 0;
		renderTargetDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		device->CreateRenderTargetView(SSRTexture.Get(), &renderTargetDescription, SSRRenderTarget.ReleaseAndGetAddressOf());

		D3D11_BUFFER_DESC bufferDescription;
		ZeroMemory(&renderTargetDescription, sizeof(D3D11_BUFFER_DESC));

		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.ByteWidth = sizeof(PixelShaderCBuffer);
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;

		device->CreateBuffer(&bufferDescription, nullptr, PSCBuffer.ReleaseAndGetAddressOf());
	}
}
