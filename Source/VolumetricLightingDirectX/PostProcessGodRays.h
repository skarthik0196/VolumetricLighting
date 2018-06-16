#pragma once
#include "PostProcessing.h"
#include "Shader.h"

namespace Rendering
{
	class Scene;
	class Direct3D;
	class DirectionalLight;

	class PostProcessGodRays : public PostProcessing
	{
	public:

		struct GodRayPixelShaderData
		{
			DirectX::XMFLOAT2 LightPosition{ DirectX::XMFLOAT2(0.0f,0.0f) };
			float Exposure{ 0.001f };
			float Decay{ 1.0f };
			float Weight{ 5.0f };
			float Padding1;
			float Padding2;
			float Padding3;
		};
		
		PostProcessGodRays(std::shared_ptr<Direct3D>& direct3DRenderer, const std::wstring& postProcessPixelShader, std::shared_ptr<DirectionalLight> directionalLight);
		PostProcessGodRays(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring& postProcessPixelShader, std::shared_ptr<DirectionalLight> directionalLight);

		~PostProcessGodRays() = default;

		void SetExposure(float exposure);
		void SetDecay(float decay);
		void SetWeight(float weight);

		float GetExposure() const;
		float GetDecay() const;
		float GetWeight() const;

		void CreateLightShafts(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);
		void BlendLightShafts(std::shared_ptr<Direct3D>& direct3DRenderer);

		void SetDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight);

		void ApplyPostProcessing(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);

	private:
		void Initialize(std::shared_ptr<Direct3D>& direct3DRenderer);

		GodRayPixelShaderData PSData;
		std::shared_ptr<DirectionalLight> LightSource;
		std::shared_ptr<Shader> RenderTexturePixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer> GodRayPSCBuffer;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GodRayRenderTarget;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> GodRayTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GodRayResourceView;

		bool CBufferNeedsUpdate{ true };
	};
}