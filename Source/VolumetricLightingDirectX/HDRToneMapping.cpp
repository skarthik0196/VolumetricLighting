#include "pch.h"
#include "HDRToneMapping.h"

namespace  Rendering 
{
	HDRToneMapping::HDRToneMapping(ID3D11Device2 * device) : PostProcessing(device, L"HDRPixelShader.cso"), ApplyHDR(true), ApplyGammaCorrection(true), Exposure(1.0f), GammaCorrection(2.2f), UpdateBuffer(true)
	{
		InitializeHDR(device);
	}

	HDRToneMapping::HDRToneMapping(ID3D11Device2 * device, std::shared_ptr<ScreenQuad> screenQuad) : PostProcessing(device, screenQuad, L"HDRPixelShader.cso"), ApplyHDR(true), ApplyGammaCorrection(true), Exposure(1.0f), GammaCorrection(2.2f), UpdateBuffer(true)
	{
		InitializeHDR(device);
	}

	void HDRToneMapping::ApplyPostProcessing(Scene * scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		UNREFERENCED_PARAMETER(scene);

		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		direct3DRenderer->SetToneMappingRenderTarget();

		ScreenQuadData->BindScreenQuadVertexShader(deviceContext);

		deviceContext->PSSetShader(PostProcessingPixelShader->GetPixelShader(), nullptr, 0);
		deviceContext->PSSetShaderResources(0, 1, direct3DRenderer->GetAddressOfSceneTextureResourceView());
		
		deviceContext->PSSetConstantBuffers(0, 1, PSCbuffer.GetAddressOf());
		if (UpdateBuffer)
		{
			UpdateCBuffer(deviceContext);
		}

		ScreenQuadData->DrawScreenQuad(deviceContext);

	}

	void HDRToneMapping::ToggleSSAO()
	{
		ApplyGammaCorrection = !ApplyGammaCorrection;
		UpdateBuffer = true;
	}

	void HDRToneMapping::ToggleHDR()
	{
		ApplyHDR = !ApplyHDR;
		UpdateBuffer = true;
	}

	bool HDRToneMapping::IsHDR() const
	{
		return ApplyHDR;
	}

	bool HDRToneMapping::IsGammaCorrected() const
	{
		return ApplyGammaCorrection;
	}

	void HDRToneMapping::SetExposure(float exposure)
	{
		Exposure = exposure;
		UpdateBuffer = true;
	}

	float HDRToneMapping::GetExposure() const
	{
		return Exposure;
	}

	void HDRToneMapping::SetGammaCorrection(float gammaCorrection)
	{
		GammaCorrection = gammaCorrection;
		UpdateBuffer = true;
	}

	float HDRToneMapping::GetGammeCorrection() const
	{
		return GammaCorrection;
	}

	void HDRToneMapping::InitializeHDR(ID3D11Device2 * device)
	{
		D3D11_BUFFER_DESC constantBufferDescription{ 0 };

		constantBufferDescription.ByteWidth = sizeof(HDRCBuffer);
		constantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		device->CreateBuffer(&constantBufferDescription, nullptr, PSCbuffer.ReleaseAndGetAddressOf());
	}

	void HDRToneMapping::UpdateCBuffer(ID3D11DeviceContext2* deviceContext)
	{
		PSData.ApplyGammaCorrection = ApplyGammaCorrection;
		PSData.ApplyHDR = ApplyHDR;
		PSData.Exposure = Exposure;
		PSData.GammaCorrection = GammaCorrection;

		deviceContext->UpdateSubresource(PSCbuffer.Get(), 0, nullptr, &PSData, 0, 0);
	}
}