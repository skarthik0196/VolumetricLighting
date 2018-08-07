#pragma once
#include "Direct3D.h"
#include "Shader.h"
#include "PostProcessing.h"

namespace Rendering
{
	class SSAO : public PostProcessing
	{
	private:

		static const uint32_t KernelSize = 64;
		static const uint32_t NoiseSize = 16;

	public:
		struct PixelShaderCBuffer
		{
			DirectX::XMFLOAT4X4 ViewMatrix;
			DirectX::XMFLOAT4X4 ProjectionMatrix;
			DirectX::XMFLOAT2 NoiseScale;
			float Radius{ 0.5f };
			float Intensity{ 1.0f };
			DirectX::XMFLOAT4 SSAOKernel[KernelSize];
		};

		SSAO(std::shared_ptr<Direct3D> direct3DRenderer, const std::wstring& postProcessPixelShader);
		SSAO(std::shared_ptr<Direct3D> direct3DRenderer, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring& postProcessPixelShader);

		float GetRadius() const;
		void SetRadius(float radius);

		void SetIntensity(float intensity);
		float GetIntensity() const;

		~SSAO() = default;

		void ApplyPostProcessing(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);

		ID3D11ShaderResourceView* GetFinalSSAOTextureResource() const;
		ID3D11ShaderResourceView** GetAddressOfFinalSSAOTextureResource();

	private:
		void Initialize(std::shared_ptr<Direct3D> direct3DRenderer);
		std::shared_ptr<Shader> BlurPixelShader;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> NoiseTexture;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> SSAOTexture;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> BlurTexture;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> NoiseResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SSAOTextureResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SSAOBlurResource;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> SSAORenderTarget;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BlurRenderTarget;

		Microsoft::WRL::ComPtr<ID3D11Buffer> PSCBuffer;

		std::vector<DirectX::XMFLOAT3> SSAOKernel;
		std::vector<DirectX::XMFLOAT4> Noise;
		
		PixelShaderCBuffer PSData;
	};
}