#pragma once
#include "Direct3DCore.h"

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

		void ClearRenderTarget(DirectX::XMFLOAT4 BGColor = DirectX::XMFLOAT4(0.0f, 0.2f, 0.4f, 1.0f));
		void ClearDepthStencilView();
		void RenderToScreen();

		void SetVSync(bool vsyncStatus);
		void SetSingleRenderTarget();
		void SetDepthDepthStencilState();

		bool GetVSyncStatus();

	private:
		uint32_t MipLevels;
		bool VSync;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthBuffer;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

	};
}