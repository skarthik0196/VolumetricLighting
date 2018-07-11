#include "pch.h"
#include "FXAA.h"
#include "Shader.h"
#include "Scene.h"

namespace Rendering
{
	FXAA::FXAA(ID3D11Device2* device) : PostProcessing(device, L"RenderTexturePixelShader.cso"), FXAAPixelShader(std::make_shared<Shader>(L"FXAAPixelShader.cso", Shader::ShaderType::PixelShader, device)), FXAAEnabled(false)
	{
		InitializeFXAA(device);
	}

	FXAA::FXAA(ID3D11Device2* device, std::shared_ptr<ScreenQuad> screenQuad) : PostProcessing(device, screenQuad, L"RenderTexturePixelShader.cso"), FXAAPixelShader(std::make_shared<Shader>(L"FXAAPixelShader.cso", Shader::ShaderType::PixelShader, device)), FXAAEnabled(false)
	{
		InitializeFXAA(device);
	}

	void FXAA::ApplyPostProcessing(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		UNREFERENCED_PARAMETER(scene);

		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		direct3DRenderer->SetFrameBufferRenderTarget();

		ScreenQuadData->BindScreenQuadVertexShader(deviceContext);

		if (FXAAEnabled)
		{
			deviceContext->PSSetShader(FXAAPixelShader->GetPixelShader(), 0, 0);
		}
		else
		{
			deviceContext->PSSetShader(PostProcessingPixelShader->GetPixelShader(), 0, 0);
		}

		deviceContext->PSSetShaderResources(0, 1, direct3DRenderer->GetAddressOfToneMappedTextureResouceView());
		deviceContext->PSSetConstantBuffers(0, 1, PSCBuffer.GetAddressOf());

		if (NeedsBufferUpdate)
		{
			deviceContext->UpdateSubresource(PSCBuffer.Get(), 0, nullptr, &PSData, 0, 0);
			NeedsBufferUpdate = false;
		}
		ScreenQuadData->DrawScreenQuad(deviceContext);
	}

	void FXAA::SetScreenResolution(float screenWidth, float screenHeight)
	{
		PSData.NoiseScale.x = screenWidth;
		PSData.NoiseScale.y = screenHeight;
		NeedsBufferUpdate = true;
	}

	void FXAA::ToggleFXAA()
	{
		FXAAEnabled = !FXAAEnabled;
	}

	bool FXAA::IsFXAAEnabled() const
	{
		return FXAAEnabled;
	}

	void FXAA::InitializeFXAA(ID3D11Device2* device)
	{
		D3D11_BUFFER_DESC constantBufferDescription{ 0 };
		
		constantBufferDescription.ByteWidth = sizeof(FXAAPixelShaderData);
		constantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		device->CreateBuffer(&constantBufferDescription, nullptr, PSCBuffer.ReleaseAndGetAddressOf());

	}
}