#include "pch.h"
#include "PostProcessGodRays.h"
#include "Scene.h"
#include "Direct3D.h"

namespace Rendering
{
	PostProcessGodRays::PostProcessGodRays(std::shared_ptr<Direct3D>& direct3DRenderer, const std::wstring & postProcessPixelShader, std::shared_ptr<DirectionalLight> directionalLight) : PostProcessing(direct3DRenderer->GetDevice(), postProcessPixelShader), LightSource(directionalLight)
							,RenderTexturePixelShader(std::make_shared<Shader>(L"RenderTexturePixelShader.cso", Shader::ShaderType::PixelShader, direct3DRenderer->GetDevice()))
	{
		Initialize(direct3DRenderer);
	}

	PostProcessGodRays::PostProcessGodRays(std::shared_ptr<Direct3D>& direct3DRenderer, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring & postProcessPixelShader, std::shared_ptr<DirectionalLight> directionalLight) : PostProcessing(direct3DRenderer->GetDevice(), screenQuad, postProcessPixelShader), LightSource(directionalLight)
							,RenderTexturePixelShader(std::make_shared<Shader>(L"RenderTexturePixelShader.cso", Shader::ShaderType::PixelShader, direct3DRenderer->GetDevice()))
	{
		Initialize(direct3DRenderer);
	}

	void PostProcessGodRays::SetExposure(float exposure)
	{
		PSData.Exposure = exposure;
		CBufferNeedsUpdate = true;
	}

	void PostProcessGodRays::SetDecay(float decay)
	{
		PSData.Decay = decay;
		CBufferNeedsUpdate = true;
	}

	void PostProcessGodRays::SetWeight(float weight)
	{
		PSData.Weight = weight;
		CBufferNeedsUpdate = true;
	}

	float PostProcessGodRays::GetExposure() const
	{
		return PSData.Exposure;
	}

	float PostProcessGodRays::GetDecay() const
	{
		return PSData.Decay;
	}

	float PostProcessGodRays::GetWeight() const
	{
		return PSData.Weight;
	}

	void PostProcessGodRays::CreateLightShafts(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		auto LightPosition = DirectX::XMVector3Project(DirectX::XMVectorZero(), 0, 0, direct3DRenderer->GetScreenWidth(), direct3DRenderer->GetScreenHeight(), 0.0f, 1.0f, scene->GetCamera()->GetProjectionMatrix(), scene->GetCamera()->GetViewMatrix(), DirectX::XMLoadFloat4x4(&LightSource->GetWorldMatrix()));
		DirectX::XMStoreFloat2(&PSData.LightPosition, LightPosition);
		PSData.LightPosition.x = PSData.LightPosition.x / direct3DRenderer->GetScreenWidth();
		PSData.LightPosition.y = PSData.LightPosition.y / direct3DRenderer->GetScreenHeight();

		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();
		ScreenQuadData->BindScreenQuadVertexShader(deviceContext);
		deviceContext->PSSetShader(PostProcessingPixelShader->GetPixelShader(), 0, 0);

		deviceContext->PSSetConstantBuffers(0, 1, GodRayPSCBuffer.GetAddressOf());
		deviceContext->PSSetShaderResources(0, 1, scene->GetGBuffer()->GetAddressOfShaderResourceView(GBuffer::GBufferData::OcclusionMap));
		deviceContext->UpdateSubresource(GodRayPSCBuffer.Get(), 0, 0, &PSData, 0, 0);

		deviceContext->ClearRenderTargetView(GodRayRenderTarget.Get(), reinterpret_cast<float*>(&Utility::Black));
		deviceContext->OMSetRenderTargets(1, GodRayRenderTarget.GetAddressOf(), direct3DRenderer->GetDepthStencilView());

		ScreenQuadData->DrawScreenQuad(deviceContext);
	}

	void PostProcessGodRays::BlendLightShafts(std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();
		direct3DRenderer->SetToneMappingRenderTarget();
		direct3DRenderer->EnableAdditiveBlending();

		deviceContext->PSSetShader(RenderTexturePixelShader->GetPixelShader(), 0, 0);
		deviceContext->PSSetShaderResources(0, 1, GodRayResourceView.GetAddressOf());

		ScreenQuadData->DrawScreenQuad(deviceContext);
		
		direct3DRenderer->DisableBlending();
	}

	void PostProcessGodRays::SetDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight)
	{
		LightSource = directionalLight;
	}

	void PostProcessGodRays::ApplyPostProcessing(Scene * scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		CreateLightShafts(scene, direct3DRenderer);
		BlendLightShafts(direct3DRenderer);
	}

	void PostProcessGodRays::Initialize(std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ID3D11Device2* device = direct3DRenderer->GetDevice();
		D3D11_BUFFER_DESC constantBufferDescription{ 0 };

		constantBufferDescription.ByteWidth = sizeof(GodRayPixelShaderData);
		constantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		device->CreateBuffer(&constantBufferDescription, nullptr, GodRayPSCBuffer.ReleaseAndGetAddressOf());

		D3D11_TEXTURE2D_DESC textureDescription;
		ZeroMemory(&textureDescription, sizeof(D3D11_TEXTURE2D_DESC));

		textureDescription.Width = static_cast<uint32_t>(direct3DRenderer->GetScreenWidth());
		textureDescription.Height = static_cast<uint32_t>(direct3DRenderer->GetScreenHeight());
		textureDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDescription.Usage = D3D11_USAGE_DEFAULT;
		textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDescription.MipLevels = 1;
		textureDescription.ArraySize = 1;
		textureDescription.SampleDesc.Count = direct3DRenderer->GetMultiSamplingCount();
		textureDescription.SampleDesc.Quality = direct3DRenderer->GetMultiSamplingQualityLevels() - 1;

		HRESULT hr;
		hr = device->CreateTexture2D(&textureDescription, nullptr, GodRayTexture.GetAddressOf());

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescription;
		ZeroMemory(&renderTargetViewDescription, sizeof(renderTargetViewDescription));

		renderTargetViewDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDescription.Format = textureDescription.Format;
		renderTargetViewDescription.Texture2D.MipSlice = 0;

		hr = device->CreateRenderTargetView(GodRayTexture.Get(), &renderTargetViewDescription, GodRayRenderTarget.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDescription;
		ZeroMemory(&shaderViewDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		shaderViewDescription.Format = textureDescription.Format;
		shaderViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderViewDescription.Texture2D.MostDetailedMip = 0;
		shaderViewDescription.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(GodRayTexture.Get(), &shaderViewDescription, GodRayResourceView.GetAddressOf());
	}
}