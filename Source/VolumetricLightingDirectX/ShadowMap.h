#pragma once
#include <wrl.h>
#include "Shader.h"
#include "Light.h"
#include "DirectionalLight.h"

namespace Rendering
{
	class Direct3D;
	class Scene;

	class ShadowMap
	{
	public:
		struct VSCBufferPerObject
		{
			DirectX::XMFLOAT4X4 LightWorldViewProjectionMatrix;
		};

		ShadowMap(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<DirectionalLight>& light);
		virtual ~ShadowMap() = default;
		
		void RenderShadowMap(std::shared_ptr<Direct3D>& direct3DRenderer, Scene* currentScene);
		
		ID3D11ShaderResourceView* GetShaderResourceView();

	private:
		void Initialize(std::shared_ptr<Direct3D>& direct3DRenderer);
		
		VSCBufferPerObject VSData;
		float Resolution;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ShadowMapRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShadowMapResourceView;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ShadowMapDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthTexture;

		Microsoft::WRL::ComPtr<ID3D11Buffer> VSCBuffer;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> ShadowMapRasterizerState;
		
		D3D11_VIEWPORT ShadowMapViewPort;

		std::shared_ptr<Shader> ShadowMapVertexShader;
		std::shared_ptr<Shader> ShadowMapPixelShader;

		std::shared_ptr<DirectionalLight>& ShadowMapLight;

		static const uint32_t ShadowMapResolution = 4096;
	};
}

