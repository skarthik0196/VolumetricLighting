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

		D3D11_TEXTURE2D_DESC depthStencilBufferDescription;
		ZeroMemory(&depthStencilBufferDescription, sizeof(D3D11_TEXTURE2D_DESC));

		depthStencilBufferDescription.Width = static_cast<uint32_t>(ScreenWidth);
		depthStencilBufferDescription.Height = static_cast<uint32_t>(ScreenHeight);
		depthStencilBufferDescription.MipLevels = MipLevels;
		depthStencilBufferDescription.ArraySize = 1;
		depthStencilBufferDescription.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		depthStencilBufferDescription.SampleDesc.Count = GetMultiSamplingCount();
		depthStencilBufferDescription.SampleDesc.Quality = GetMultiSamplingQualityLevels() - 1;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
		ZeroMemory(&depthStencilViewDescription, sizeof(depthStencilViewDescription));

		depthStencilViewDescription.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDescription.Flags = 0;
		depthStencilViewDescription.Texture2D.MipSlice = 0;

		result = Device->CreateTexture2D(&depthStencilBufferDescription, nullptr, DepthBuffer.GetAddressOf());

		D3D11_DEPTH_STENCIL_DESC depthStencilDescription;

		depthStencilDescription.DepthEnable = true;
		depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;

		//Enable Stencil if required, remember to change format to D24_U8

		depthStencilDescription.StencilEnable = false;
		/*depthStencilDescription.StencilReadMask = 0xFF;
		depthStencilDescription.StencilWriteMask = 0xFF;

		depthStencilDescription.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDescription.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDescription.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDescription.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;*/

		result = Device->CreateDepthStencilState(&depthStencilDescription, DepthEnabledState.GetAddressOf());
		result = Device->CreateDepthStencilView(DepthBuffer.Get(), &depthStencilViewDescription, DepthStencilView.GetAddressOf());

		depthStencilDescription.DepthEnable = true;
		depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		result = Device->CreateDepthStencilState(&depthStencilDescription, DepthDisabledState.ReleaseAndGetAddressOf());

		result = GetDevice()->CreateRenderTargetView(BackBuffer.Get(), nullptr, RenderTargetView.GetAddressOf());

		CreateBlendStates();

		EnableDepthTesting();
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
		viewPortDescription.MinDepth = 0;
		viewPortDescription.MaxDepth = 1;

		DeviceContext->RSSetViewports(1, &viewPortDescription);
	}

	void Direct3D::CreateBlendStates()
	{
		D3D11_BLEND_DESC blendStateDescription;
		ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

		blendStateDescription.RenderTarget[0].BlendEnable = true;

		blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendStateDescription.AlphaToCoverageEnable = false;
		blendStateDescription.IndependentBlendEnable = false;
		Device->CreateBlendState(&blendStateDescription, FirstAdditiveBlendState.ReleaseAndGetAddressOf());

		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		Device->CreateBlendState(&blendStateDescription, AdditiveBlendState.ReleaseAndGetAddressOf());
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
		SwapChain->Present(static_cast<uint32_t>(VSync), 0);
	}

	void Direct3D::SetVSync(bool vsyncStatus)
	{
		VSync = vsyncStatus;
	}

	void Direct3D::SetSingleRenderTarget()
	{
		DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	}

	void Direct3D::EnableDepthTesting()
	{
		DeviceContext->OMSetDepthStencilState(DepthEnabledState.Get(), 1);
	}

	bool Direct3D::GetVSyncStatus()
	{
		return VSync;
	}

	void Direct3D::DisableDepthTesting()
	{
		DeviceContext->OMSetDepthStencilState(DepthDisabledState.Get(), 1);
	}

	void Direct3D::BeginAdditiveBlending()
	{
		DeviceContext->OMSetBlendState(FirstAdditiveBlendState.Get(), NULL, 0xFFFFFFFF);
	}

	void Direct3D::EnableAdditiveBlending()
	{
		DeviceContext->OMSetBlendState(AdditiveBlendState.Get(), NULL, 0xFFFFFFFF);
	}

	void Direct3D::DisableBlending()
	{
		
		DeviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	}

	ID3D11DepthStencilView* Direct3D::GetDepthStencilView()
	{
		return DepthStencilView.Get();
	}
}