#pragma once
#include "PostProcessing.h"

namespace Rendering
{
	class HDRToneMapping : public PostProcessing
	{
	public:
		struct HDRCBuffer
		{
			float Exposure;
			float GammaCorrection;
			uint32_t ApplyHDR;
			uint32_t ApplyGammaCorrection;
		};

		HDRToneMapping(ID3D11Device2* device);
		HDRToneMapping(ID3D11Device2* device, std::shared_ptr<ScreenQuad> screenQuad);
		~HDRToneMapping() = default;

		virtual void ApplyPostProcessing(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);

		void ToggleSSAO();
		void ToggleHDR();
		
		bool IsHDR() const;
		bool IsGammaCorrected() const;

		void SetExposure(float exposure);
		float GetExposure() const;

		void SetGammaCorrection(float gammaCorrection);
		float GetGammeCorrection() const;


	private:
		void InitializeHDR(ID3D11Device2* device);
		void UpdateCBuffer(ID3D11DeviceContext2* deviceContext);

		Microsoft::WRL::ComPtr<ID3D11Buffer> PSCbuffer;

		float Exposure;
		float GammaCorrection;

		bool ApplyHDR;
		bool ApplyGammaCorrection;
		bool UpdateBuffer;

		HDRCBuffer PSData;
	};
}