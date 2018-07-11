#pragma once
#include "Direct3DCore.h"
#include "Utility.h"

namespace Rendering
{
	class Direct3D : public Direct3DCore
	{

	public:

		enum class RasterizerState
		{
			BackFaceCulling,
			FrontFaceCulling,
			DisableCulling,
			WireFrame
		};

		Direct3D();
		Direct3D(float screenWidth, float screenHeight);

		virtual ~Direct3D();

		void InitializeDirect3D();

		void ClearRenderTarget(DirectX::XMFLOAT4 BGColor = Utility::BackgroundColor);
		void ClearDepthStencilView();
		void RenderToScreen();

		void SetVSync(bool vsyncStatus);
		void SetFrameBufferRenderTarget();
		void SetSceneBufferRenderTarget();
		void SetToneMappingRenderTarget();

		void EnableDepthTesting();
		void DisableDepthWriting();
		void DisableDepthTesting();

		void BeginAdditiveBlending();
		void EnableAdditiveBlending();
		void DisableBlending();

		void SetViewPort();

		bool GetVSyncStatus();

		void SetRasterizerState(RasterizerState cullMode);

		ID3D11DepthStencilView* GetDepthStencilView();
		ID3D11ShaderResourceView* GetSceneTextureResourceView();
		ID3D11ShaderResourceView** GetAddressOfSceneTextureResourceView();

		ID3D11ShaderResourceView* GetToneMappedTextureResouceView();
		ID3D11ShaderResourceView** GetAddressOfToneMappedTextureResouceView();

		ID3D11ShaderResourceView** GetAddressOfSceneDepthBufferResource();
		ID3D11ShaderResourceView* GetSceneDepthBufferResource();

	private:
		void CreateViewPort();
		void CreateBlendStates();
		void CreateRasterizerStates();

		D3D11_VIEWPORT RendererViewPort;

		uint32_t MipLevels;
		bool VSync;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> FrameTexture;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>	SceneTexture;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> ToneMappedTexture;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SceneResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ToneMappedResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DepthResource;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthEnabledState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthWritingDisabledState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthDisableState;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> FrameBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> SceneTarget;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ToneMappedTarget;

		Microsoft::WRL::ComPtr<ID3D11BlendState> FirstAdditiveBlendState;
		Microsoft::WRL::ComPtr<ID3D11BlendState> AdditiveBlendState;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> FrontFaceCulling;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> BackFaceCulling;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> DisableCulling;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> WireFrameMode;

		
	};
}