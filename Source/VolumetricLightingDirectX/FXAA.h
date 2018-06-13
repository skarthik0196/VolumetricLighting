#pragma once
#include "PostProcessing.h"

namespace Rendering
{
	class FXAA : public PostProcessing
	{
	public:
		struct FXAAPixelShaderData
		{
			DirectX::XMFLOAT2 ScreenResolution{ DirectX::XMFLOAT2(1366.0f,768.0f) };
			DirectX::XMFLOAT2 Padding;
		};

		FXAA(ID3D11Device2* device);
		FXAA(ID3D11Device2* device, std::shared_ptr<ScreenQuad> screenQuad);
		virtual ~FXAA() = default;

		void ApplyPostProcessing(std::shared_ptr<Direct3D> direct3DRenderer);
		void SetScreenResolution(float screenWidth, float screenHeight);

	private:
		void InitializeFXAA(ID3D11Device2* device);
		FXAAPixelShaderData PSData;
		Microsoft::WRL::ComPtr<ID3D11Buffer> PSCBuffer;
		bool NeedsBufferUpdate{ true };
	};
}