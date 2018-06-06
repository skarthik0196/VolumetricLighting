#include "pch.h"
#include "GBuffer.h"
#include "Direct3D.h"
#include "Utility.h"

namespace Rendering
{
	GBuffer::GBuffer(std::shared_ptr<Direct3D> direct3DRenderer)
	{
		Initialize(direct3DRenderer);
	}

	void GBuffer::Initialize(std::shared_ptr<Direct3D> direct3DRenderer)
	{
		RenderTargetTextures.resize(static_cast<uint32_t>(GBuffer::GBufferData::End));
		RenderTargetViews.resize(static_cast<uint32_t>(GBuffer::GBufferData::End));
		ShaderResourceViews.resize(static_cast<uint32_t>(GBuffer::GBufferData::End));

		D3D11_TEXTURE2D_DESC textureDescription;
		ZeroMemory(&textureDescription, sizeof(D3D11_TEXTURE2D_DESC));

		textureDescription.Width = static_cast<uint32_t>(direct3DRenderer->GetScreenWidth());
		textureDescription.Height = static_cast<uint32_t>(direct3DRenderer->GetScreenHeight());
		textureDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDescription.Usage = D3D11_USAGE_DEFAULT;
		textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDescription.MipLevels = 1;
		textureDescription.ArraySize = 1;
		textureDescription.SampleDesc.Count = direct3DRenderer->GetMultiSamplingQualityLevels();
		textureDescription.SampleDesc.Quality = direct3DRenderer->GetMultiSamplingQualityLevels() - 1;

		ID3D11Device2* device = direct3DRenderer->GetDevice();

		for (uint32_t i = 0; i < static_cast<uint32_t>(GBufferData::End); ++i)
		{
			HRESULT hr;
			hr = device->CreateTexture2D(&textureDescription, nullptr, RenderTargetTextures[i].GetAddressOf());
		}

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescription;
		ZeroMemory(&renderTargetViewDescription, sizeof(renderTargetViewDescription));

		renderTargetViewDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDescription.Format = textureDescription.Format;
		renderTargetViewDescription.Texture2D.MipSlice = 0;

		for (uint32_t i = 0; i < static_cast<uint32_t>(GBufferData::End); ++i)
		{
			HRESULT hr;
			hr = device->CreateRenderTargetView(RenderTargetTextures[i].Get(), &renderTargetViewDescription, RenderTargetViews[i].GetAddressOf());
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDescription;
		ZeroMemory(&shaderViewDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		shaderViewDescription.Format = textureDescription.Format;
		shaderViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderViewDescription.Texture2D.MostDetailedMip = 0;
		shaderViewDescription.Texture2D.MipLevels = 1;

		for (uint32_t i = 0; i < static_cast<uint32_t>(GBufferData::End); ++i)
		{
			HRESULT hr;
			hr = device->CreateShaderResourceView(RenderTargetTextures[i].Get(), &shaderViewDescription, ShaderResourceViews[i].GetAddressOf());
		}
	}

	void GBuffer::SetRenderTargets(std::shared_ptr<Direct3D> direct3DRenderer)
	{
		ID3D11RenderTargetView* renderTargetViews[] = { RenderTargetViews[0].Get(), RenderTargetViews[1].Get(), RenderTargetViews[2].Get() };
		direct3DRenderer->GetDeviceContext()->OMSetRenderTargets(static_cast<uint32_t>(RenderTargetViews.size()), renderTargetViews, direct3DRenderer->GetDepthStencilView());
	}

	void GBuffer::ClearRenderTargets(std::shared_ptr<Direct3D> direct3DRenderer)
	{
		for (uint32_t i = 0; i < RenderTargetViews.size(); ++i)
		{
			direct3DRenderer->GetDeviceContext()->ClearRenderTargetView(RenderTargetViews[i].Get(), reinterpret_cast<float*>(&Utility::BackgroundColor));
		}
	}

	ID3D11ShaderResourceView* GBuffer::GetShaderResourceView(GBufferData resourceType)
	{
		return ShaderResourceViews[static_cast<uint32_t>(resourceType)].Get();
	}

	ID3D11ShaderResourceView** GBuffer::GetAddressOfShaderResourceView(GBufferData resourceType)
	{
		return ShaderResourceViews[static_cast<uint32_t>(resourceType)].GetAddressOf();;
	}

	void GBuffer::BindGBufferData(ID3D11DeviceContext2 * deviceContext)
	{
		ID3D11ShaderResourceView* shaderResourceViews[] = { ShaderResourceViews[0].Get(), ShaderResourceViews[1].Get(), ShaderResourceViews[2].Get() };
		deviceContext->PSSetShaderResources(0, ARRAYSIZE(shaderResourceViews), shaderResourceViews);
	}

	void GBuffer::UnBindBufferData(ID3D11DeviceContext2 * deviceContext)
	{
		ID3D11ShaderResourceView* shaderResourceViews[] = { nullptr, nullptr, nullptr };
		deviceContext->PSSetShaderResources(0, ARRAYSIZE(shaderResourceViews), shaderResourceViews);
	}
}