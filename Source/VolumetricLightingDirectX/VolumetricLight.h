#pragma once
#include "ShadowMap.h"
#include "PostProcessing.h"

namespace Rendering
{
	class VolumetricLight : public PostProcessing
	{
	public:

		struct VolumetricLightPixelData
		{
			DirectX::XMFLOAT4X4 LightViewProjectionMatrix;
			DirectX::XMFLOAT3 CameraPosition;
			float GScattering{ 0.0f };
			DirectX::XMFLOAT4 LightColor;
			DirectX::XMFLOAT3 LightDirection;
			float ScatteringAmount{ 1.0f };
		};

		VolumetricLight(std::shared_ptr<Direct3D>& direct3DRenderer, const std::wstring& postProcessPixelShader);
		VolumetricLight(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring& postProcessPixelShader);

		VolumetricLight() = default;

		void ApplyPostProcessing(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);

		void SetShadowMap(std::shared_ptr<ShadowMap>& sMap);
		const std::shared_ptr<ShadowMap>& GetShadowMap() const;

		void SetGScattering(float GScattering);
		float GetGScattering() const;

		void SetScatteringAmount(float scatterAmount);
		float GetScatteringAmount() const;

	private:
		void CreateVolumetrics(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);
		void BlendVolumetrics(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);
		void BlurVolumetrics(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);
		void Initialize(std::shared_ptr<Direct3D>& direct3DRenderer);

		VolumetricLightPixelData PSData;
		std::shared_ptr<ShadowMap> SMap;
		std::shared_ptr<Shader> RenderTexturePixelShader;
		std::shared_ptr<Shader> BlurXShader;
		std::shared_ptr<Shader> BlurYShader;

		Microsoft::WRL::ComPtr<ID3D11Buffer> PSCBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> VolumetricLightRenderTarget;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> VolumetricLightResourceView;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BlurXRenderTarget;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> BlurXResourceView;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BlurYRenderTarget;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> BlurYResourceView;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> VolumetricLightTexture;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> BlurXTexture;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> BlurYTexture;

		D3D11_VIEWPORT VolumtericLightViewPort;
	};
}
