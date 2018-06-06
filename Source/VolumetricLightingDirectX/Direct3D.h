#pragma once
#include "Direct3DCore.h"
#include "Utility.h"

namespace Rendering
{
	class Direct3D : public Direct3DCore
	{

	public:
		Direct3D();
		Direct3D(float screenWidth, float screenHeight);

		virtual ~Direct3D();

		void InitializeDirect3D();
		void CreateViewPort();
		void CreateBlendStates();

		void ClearRenderTarget(DirectX::XMFLOAT4 BGColor = Utility::BackgroundColor);
		void ClearDepthStencilView();
		void RenderToScreen();

		void SetVSync(bool vsyncStatus);
		void SetSingleRenderTarget();

		void EnableDepthTesting();
		void DisableDepthTesting();

		void BeginAdditiveBlending();
		void EnableAdditiveBlending();
		void DisableBlending();

		bool GetVSyncStatus();

		ID3D11DepthStencilView* GetDepthStencilView();

	private:
		uint32_t MipLevels;
		bool VSync;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthBuffer;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthEnabledState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthDisabledState;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

		Microsoft::WRL::ComPtr<ID3D11BlendState> FirstAdditiveBlendState;
		Microsoft::WRL::ComPtr<ID3D11BlendState> AdditiveBlendState;
	};
}