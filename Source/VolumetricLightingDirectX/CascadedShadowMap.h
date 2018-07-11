#pragma once
#include <wrl.h>
#include "Shader.h"
#include "DirectionalLight.h"

namespace Rendering
{
	class Direct3D;
	class Scene;

	class CascadedShadowMap
	{
	private:
		static const uint32_t CascadeAmount = 3;

	public:
		struct VSCBufferPerObject
		{
			DirectX::XMFLOAT4X4 LightWorldViewProjectionMatrix[CascadeAmount];
		};

		CascadedShadowMap(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<DirectionalLight>& light);
		virtual ~CascadedShadowMap() = default;

		void RenderShadowMap(std::shared_ptr<Direct3D>& direct3DRenderer, Scene* currentScene);
	
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& GetShaderResourceView();

		static uint32_t GetCascadeAmount();

	private:
		void CalculateFrustumData(DirectX::XMMATRIX& cameraViewProjectionMatrix, uint32_t cascadeIndex);
		void Initialize(std::shared_ptr<Direct3D>& direct3DRenderer);

		VSCBufferPerObject VSData;
		float Resolution;

		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> ShadowMapRenderTargetView;
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> ShadowMapResourceView;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ShadowMapDepthStencilView;
		std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> DepthTexture;

		Microsoft::WRL::ComPtr<ID3D11Buffer> VSCBuffer;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> ShadowMapRasterizerState;

		D3D11_VIEWPORT ShadowMapViewPort;

		std::shared_ptr<Shader> ShadowMapVertexShader;
		std::shared_ptr<Shader> ShadowMapPixelShader;

		std::shared_ptr<DirectionalLight>& ShadowMapLight;
	};
}