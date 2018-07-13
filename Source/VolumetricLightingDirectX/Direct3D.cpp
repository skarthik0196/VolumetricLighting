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

		result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(FrameTexture.GetAddressOf()));
		D3D11_TEXTURE2D_DESC depthBufferDescription;
		FrameTexture->GetDesc(&depthBufferDescription);

		D3D11_TEXTURE2D_DESC depthStencilBufferDescription;
		ZeroMemory(&depthStencilBufferDescription, sizeof(D3D11_TEXTURE2D_DESC));

		depthStencilBufferDescription.Width = static_cast<uint32_t>(ScreenWidth);
		depthStencilBufferDescription.Height = static_cast<uint32_t>(ScreenHeight);
		depthStencilBufferDescription.MipLevels = MipLevels;
		depthStencilBufferDescription.ArraySize = 1;
		depthStencilBufferDescription.Format = DXGI_FORMAT_R32_TYPELESS;
		depthStencilBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
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

		D3D11_SHADER_RESOURCE_VIEW_DESC depthResourceDescription;
		ZeroMemory(&depthResourceDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		depthResourceDescription.Format = DXGI_FORMAT_R32_FLOAT;
		depthResourceDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthResourceDescription.Texture2D.MipLevels = depthStencilBufferDescription.MipLevels;
		depthResourceDescription.Texture2D.MostDetailedMip = 0;

		result = Device->CreateShaderResourceView(DepthBuffer.Get(), &depthResourceDescription, DepthResource.ReleaseAndGetAddressOf());

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
		result = Device->CreateDepthStencilState(&depthStencilDescription, DepthWritingDisabledState.ReleaseAndGetAddressOf());

		depthStencilDescription.DepthEnable = false;
		result = Device->CreateDepthStencilState(&depthStencilDescription, DepthDisableState.ReleaseAndGetAddressOf());

		result = GetDevice()->CreateRenderTargetView(FrameTexture.Get(), nullptr, FrameBuffer.GetAddressOf());

		D3D11_TEXTURE2D_DESC sceneTextureDescription;
		ZeroMemory(&sceneTextureDescription, sizeof(D3D11_TEXTURE2D_DESC));

		sceneTextureDescription.Width = static_cast<uint32_t>(ScreenWidth);
		sceneTextureDescription.Height = static_cast<uint32_t>(ScreenHeight);
		sceneTextureDescription.MipLevels = MipLevels;
		sceneTextureDescription.ArraySize = 1;
		sceneTextureDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		sceneTextureDescription.SampleDesc.Count = GetMultiSamplingCount();
		sceneTextureDescription.SampleDesc.Quality = GetMultiSamplingQualityLevels() - 1;
		sceneTextureDescription.Usage = D3D11_USAGE_DEFAULT;
		sceneTextureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		result = Device->CreateTexture2D(&sceneTextureDescription, nullptr, SceneTexture.ReleaseAndGetAddressOf());
		result = Device->CreateTexture2D(&sceneTextureDescription, nullptr, ToneMappedTexture.ReleaseAndGetAddressOf());

		D3D11_RENDER_TARGET_VIEW_DESC sceneTargetDescription;
		ZeroMemory(&sceneTargetDescription, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		sceneTargetDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		sceneTargetDescription.Format = sceneTextureDescription.Format;
		sceneTargetDescription.Texture2D.MipSlice = 0;

		result = Device->CreateRenderTargetView(SceneTexture.Get(), &sceneTargetDescription, SceneTarget.ReleaseAndGetAddressOf());
		result = Device->CreateRenderTargetView(ToneMappedTexture.Get(), &sceneTargetDescription, ToneMappedTarget.ReleaseAndGetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC sceneResourceDescription;
		ZeroMemory(&sceneResourceDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		sceneResourceDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		sceneResourceDescription.Format = sceneTextureDescription.Format;
		sceneResourceDescription.Texture2D.MostDetailedMip = 0;
		sceneResourceDescription.Texture2D.MipLevels = MipLevels;

		result = Device->CreateShaderResourceView(SceneTexture.Get(), &sceneResourceDescription, SceneResource.ReleaseAndGetAddressOf());
		result = Device->CreateShaderResourceView(ToneMappedTexture.Get(), &sceneResourceDescription, ToneMappedResource.ReleaseAndGetAddressOf());

		CreateBlendStates();
		CreateRasterizerStates();
		EnableDepthTesting();
		SetFrameBufferRenderTarget();
		CreateViewPort();

		Query::InitializeDisjointQuery(Device.Get());
		FrameQuery = std::make_shared<Query>(Device.Get());
	}

	void Direct3D::CreateViewPort()
	{

		RendererViewPort.TopLeftX = 0;
		RendererViewPort.TopLeftY = 0;
		RendererViewPort.Width = static_cast<float>(ScreenWidth);
		RendererViewPort.Height = static_cast<float>(ScreenHeight);
		RendererViewPort.MinDepth = 0;
		RendererViewPort.MaxDepth = 1;

		DeviceContext->RSSetViewports(1, &RendererViewPort);
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

	void Direct3D::CreateRasterizerStates()
	{
		D3D11_RASTERIZER_DESC rasterizerStateDescription;
		ZeroMemory(&rasterizerStateDescription, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerStateDescription.CullMode = D3D11_CULL_FRONT;
		rasterizerStateDescription.FillMode = D3D11_FILL_SOLID;
		Device->CreateRasterizerState(&rasterizerStateDescription, FrontFaceCulling.ReleaseAndGetAddressOf());
		
		rasterizerStateDescription.CullMode = D3D11_CULL_BACK;
		Device->CreateRasterizerState(&rasterizerStateDescription, BackFaceCulling.ReleaseAndGetAddressOf());

		rasterizerStateDescription.CullMode = D3D11_CULL_NONE;
		Device->CreateRasterizerState(&rasterizerStateDescription, DisableCulling.ReleaseAndGetAddressOf());

		rasterizerStateDescription.CullMode = D3D11_CULL_BACK;
		rasterizerStateDescription.FillMode = D3D11_FILL_WIREFRAME;
		Device->CreateRasterizerState(&rasterizerStateDescription, WireFrameMode.ReleaseAndGetAddressOf());
	}

	void Direct3D::ClearRenderTarget(DirectX::XMFLOAT4 BGColor)
	{
		DeviceContext->ClearRenderTargetView(FrameBuffer.Get(), reinterpret_cast<const float*>(&BGColor));
		DeviceContext->ClearRenderTargetView(SceneTarget.Get(), reinterpret_cast<const float*>(&BGColor));
	}

	void Direct3D::ClearDepthStencilView()
	{
		DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Direct3D::RenderToScreen()
	{
		SwapChain->Present(static_cast<uint32_t>(VSync), 0);
		FrameQuery->EndQuery(DeviceContext.Get());
		Query::EndDisjointQuery(DeviceContext.Get());
		Query::SwapQueryBuffers();
		Query::GetDisjointTimeStamp(DeviceContext.Get());
		FrameTime = FrameQuery->GetQueryTime(DeviceContext.Get());
		DeviceContext->ClearState();
	}

	void Direct3D::SetVSync(bool vsyncStatus)
	{
		VSync = vsyncStatus;
	}

	void Direct3D::SetFrameBufferRenderTarget()
	{
		DeviceContext->OMSetRenderTargets(1, FrameBuffer.GetAddressOf(), DepthStencilView.Get());
	}

	void Direct3D::SetSceneBufferRenderTarget()
	{
		DeviceContext->OMSetRenderTargets(1, SceneTarget.GetAddressOf(), DepthStencilView.Get());
	}

	void Direct3D::SetToneMappingRenderTarget()
	{
		DeviceContext->OMSetRenderTargets(1, ToneMappedTarget.GetAddressOf(), DepthStencilView.Get());
	}

	void Direct3D::EnableDepthTesting()
	{
		DeviceContext->OMSetDepthStencilState(DepthEnabledState.Get(), 1);
	}

	bool Direct3D::GetVSyncStatus()
	{
		return VSync;
	}

	void Direct3D::BeginFrameQuery()
	{
		Query::BeginDisjointQuery(DeviceContext.Get());
		FrameQuery->BeginQuery(DeviceContext.Get());
	}

	void Direct3D::SetRasterizerState(RasterizerState cullMode)
	{
		switch (cullMode)
		{
			case RasterizerState::BackFaceCulling:
			{
				DeviceContext->RSSetState(BackFaceCulling.Get());
				break;
			}
			case RasterizerState::FrontFaceCulling:
			{
				DeviceContext->RSSetState(FrontFaceCulling.Get());
				break;
			}
			case RasterizerState::DisableCulling:
			{
				DeviceContext->RSSetState(DisableCulling.Get());
				break;
			}
			case RasterizerState::WireFrame:
			{
				DeviceContext->RSSetState(WireFrameMode.Get());
				break;
			}
			default:
			{
				DeviceContext->RSSetState(BackFaceCulling.Get());
				break;
			}
		}
	}

	void Direct3D::DisableDepthWriting()
	{
		DeviceContext->OMSetDepthStencilState(DepthWritingDisabledState.Get(), 1);
	}

	void Direct3D::DisableDepthTesting()
	{
		DeviceContext->OMSetDepthStencilState(DepthDisableState.Get(), 1);
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

	void Direct3D::SetViewPort()
	{
		DeviceContext->RSSetViewports(1, &RendererViewPort);
	}

	ID3D11DepthStencilView* Direct3D::GetDepthStencilView()
	{
		return DepthStencilView.Get();
	}

	ID3D11ShaderResourceView * Direct3D::GetSceneTextureResourceView()
	{
		return SceneResource.Get();
	}
	ID3D11ShaderResourceView ** Direct3D::GetAddressOfSceneTextureResourceView()
	{
		return SceneResource.GetAddressOf();
	}

	ID3D11ShaderResourceView * Direct3D::GetToneMappedTextureResouceView()
	{
		return ToneMappedResource.Get();
	}

	ID3D11ShaderResourceView ** Direct3D::GetAddressOfToneMappedTextureResouceView()
	{
		return ToneMappedResource.GetAddressOf();
	}

	ID3D11ShaderResourceView ** Direct3D::GetAddressOfSceneDepthBufferResource()
	{
		return DepthResource.GetAddressOf();
	}

	ID3D11ShaderResourceView * Direct3D::GetSceneDepthBufferResource()
	{
		return DepthResource.Get();
	}
}