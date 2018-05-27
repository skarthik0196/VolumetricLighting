#include "pch.h"
#include "Direct3D.h"

namespace Rendering
{
	Direct3D::Direct3D() : Direct3DCore(), MipLevels(1) , VSync(false)
	{
	}

	Direct3D::Direct3D(float screenWidth, float screenHeight) : Direct3DCore(screenWidth, screenHeight),  MipLevels(1), VSync(false)
	{
	}


	Direct3D::~Direct3D()
	{
	}

	void Direct3D::InitializeDirect3D()
	{
		InitializeD3DCore();

		HRESULT result;

		result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(BackBuffer.GetAddressOf()));
		D3D11_TEXTURE2D_DESC depthBufferDescription;
		BackBuffer->GetDesc(&depthBufferDescription);

		//D3D11_TEXTURE2D_DESC depthBufferDescription;
		//ZeroMemory(&depthBufferDescription, sizeof(D3D11_TEXTURE2D_DESC));

		//depthBufferDescription.Width = static_cast<uint32_t>(ScreenWidth);
		//depthBufferDescription.Height = static_cast<uint32_t>(ScreenHeight);
		//depthBufferDescription.MipLevels = MipLevels;
		//depthBufferDescription.ArraySize = 1;
		//depthBufferDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		//depthBufferDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		//depthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		//depthBufferDescription.SampleDesc.Count = 1;
		//depthBufferDescription.SampleDesc.Quality = 0;
		//depthBufferDescription.CPUAccessFlags = 0;
		//depthBufferDescription.MiscFlags = 0;

		//result = Device->CreateTexture2D(&depthBufferDescription, nullptr, BackBuffer.GetAddressOf());

		D3D11_TEXTURE2D_DESC depthStencilBufferDescription;
		ZeroMemory(&depthStencilBufferDescription, sizeof(D3D11_TEXTURE2D_DESC));

		depthStencilBufferDescription.Width = static_cast<uint32_t>(ScreenWidth);
		depthStencilBufferDescription.Height = static_cast<uint32_t>(ScreenHeight);
		depthStencilBufferDescription.MipLevels = MipLevels;
		depthStencilBufferDescription.ArraySize = 1;
		depthStencilBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		depthStencilBufferDescription.SampleDesc.Count = 1;
		depthStencilBufferDescription.SampleDesc.Quality = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
		ZeroMemory(&depthStencilViewDescription, sizeof(depthStencilViewDescription));

		depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		depthStencilViewDescription.Flags = 0;
		depthStencilViewDescription.Texture2D.MipSlice = 0;

		result = Device->CreateTexture2D(&depthStencilBufferDescription, nullptr, DepthBuffer.GetAddressOf());
		result = Device->CreateDepthStencilView(DepthBuffer.Get(), &depthStencilViewDescription, DepthStencilView.GetAddressOf());

		result = GetDevice()->CreateRenderTargetView(BackBuffer.Get(), nullptr, RenderTargetView.GetAddressOf());
		SetSingleRenderTarget();
		CreateViewPort();
	}

	void Direct3D::CreateViewPort()
	{
		D3D11_VIEWPORT viewPortDescription;
		ZeroMemory(&viewPortDescription, sizeof(viewPortDescription));

		viewPortDescription.TopLeftX = 0;
		viewPortDescription.TopLeftY = 0;
		viewPortDescription.Width = static_cast<float>(ScreenWidth);
		viewPortDescription.Height = static_cast<float>(ScreenHeight);

		DeviceContext->RSSetViewports(1, &viewPortDescription);
	}

	void Direct3D::ClearRenderTarget(DirectX::XMFLOAT4 BGColor)
	{
		DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), reinterpret_cast<const float*>(&BGColor));
	}

	void Direct3D::ClearDepthStencilView()
	{
		DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Direct3D::RenderToScreen()
	{
		if (VSync)
		{
			SwapChain->Present(1, 0);
		}
		else
		{
			SwapChain->Present(0, 0);
		}
	}

	void Direct3D::SetVSync(bool vsyncStatus)
	{
		VSync = vsyncStatus;
	}

	void Direct3D::SetSingleRenderTarget()
	{
		DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	}

	bool Direct3D::GetVSyncStatus()
	{
		return VSync;
	}
}