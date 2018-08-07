#pragma once
#include "PostProcessing.h"

namespace Rendering
{
	class ScreenSpaceReflections : public PostProcessing
	{
	public:
		struct PixelShaderCBuffer
		{
			DirectX::XMFLOAT4X4 ViewMatrix;
			DirectX::XMFLOAT4X4 ProjectionMatrix;
		};

		ScreenSpaceReflections(std::shared_ptr<Direct3D>& direct3DRenderer, const std::wstring& postProcessPixelShader);
		ScreenSpaceReflections(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring& postProcessPixelShader);

		virtual ~ScreenSpaceReflections() = default;

		virtual void ApplyPostProcessing(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);

	private:
		void Initialize(std::shared_ptr<Direct3D>& direct3DRenderer);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> SSRTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SSRResource;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> SSRRenderTarget;
		Microsoft::WRL::ComPtr<ID3D11Buffer> PSCBuffer;

		PixelShaderCBuffer PSData;
	};
}