#include "pch.h"
#include "SSAO.h"
#include "Scene.h"
#include "Camera.h"
#include "GBuffer.h"

namespace Rendering
{
	SSAO::SSAO(std::shared_ptr<Direct3D> direct3DRenderer, const std::wstring & postProcessPixelShader) : PostProcessing(direct3DRenderer->GetDevice(), postProcessPixelShader)
	{
		Initialize(direct3DRenderer);
	}

	SSAO::SSAO(std::shared_ptr<Direct3D> direct3DRenderer, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring & postProcessPixelShader) : PostProcessing(direct3DRenderer->GetDevice(), screenQuad, postProcessPixelShader)
	{
		Initialize(direct3DRenderer);
	}

	float SSAO::GetRadius() const
	{
		return PSData.Radius;
	}

	void SSAO::SetRadius(float radius)
	{
		PSData.Radius = radius;
	}

	void SSAO::SetIntensity(float intensity)
	{
		PSData.Intensity = intensity;
	}

	float SSAO::GetIntensity() const
	{
		return PSData.Intensity;
	}

	void SSAO::ApplyPostProcessing(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();
		ID3D11RenderTargetView* nullRT = nullptr;

		direct3DRenderer->DisableDepthTesting();
		direct3DRenderer->DisableBlending();

		ScreenQuadData->BindScreenQuadVertexShader(deviceContext);
		PSData.ViewMatrix = scene->GetCamera()->GetViewMatrixAsFloat4x4();
		PSData.ProjectionMatrix = scene->GetCamera()->GetProjectionMatrixAsFloat4x4();

		DirectX::XMStoreFloat4x4(&PSData.ViewMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&PSData.ViewMatrix)));
		DirectX::XMStoreFloat4x4(&PSData.ProjectionMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&PSData.ProjectionMatrix)));

		deviceContext->PSSetConstantBuffers(0, 1, PSCBuffer.GetAddressOf());
		deviceContext->UpdateSubresource(PSCBuffer.Get(), 0, 0, &PSData, 0, 0);

		deviceContext->ClearRenderTargetView(SSAORenderTarget.Get(), reinterpret_cast<float*>(&Utility::BackgroundColor));
		deviceContext->PSSetShader(PostProcessingPixelShader->GetPixelShader(), nullptr, 0);

		deviceContext->OMSetRenderTargets(1, &nullRT, nullptr);
		
		auto& gBuffer = scene->GetGBuffer();
		ID3D11ShaderResourceView* shaderResources[5] = { gBuffer->GetShaderResourceView(GBuffer::GBufferData::Normal), gBuffer->GetShaderResourceView(GBuffer::GBufferData::WorldPosition), NoiseResource.Get(), direct3DRenderer->GetSceneDepthBufferResource(), nullptr };

		deviceContext->PSSetShaderResources(0, ARRAYSIZE(shaderResources), shaderResources);
		deviceContext->OMSetRenderTargets(1, SSAORenderTarget.GetAddressOf(), nullptr);
		ScreenQuadData->DrawScreenQuad(deviceContext);

		deviceContext->PSSetShader(BlurPixelShader->GetPixelShader(), nullptr, 0);

		deviceContext->ClearRenderTargetView(BlurRenderTarget.Get(), reinterpret_cast<float*>(&Utility::BackgroundColor));

		deviceContext->OMSetRenderTargets(1, &nullRT, nullptr);

		ID3D11ShaderResourceView* shaderResources2[5] = { SSAOTextureResource.Get(), nullptr, nullptr, nullptr, nullptr };

		deviceContext->PSSetShaderResources(0, ARRAYSIZE(shaderResources2), shaderResources2);
		deviceContext->OMSetRenderTargets(1, BlurRenderTarget.GetAddressOf(), direct3DRenderer->GetDepthStencilView());


		ScreenQuadData->DrawScreenQuad(deviceContext);
	}

	ID3D11ShaderResourceView* SSAO::GetFinalSSAOTextureResource() const
	{
		return SSAOTextureResource.Get();
	}

	ID3D11ShaderResourceView** SSAO::GetAddressOfFinalSSAOTextureResource()
	{
		return SSAOBlurResource.GetAddressOf();
	}

	void SSAO::Initialize(std::shared_ptr<Direct3D> direct3DRenderer)
	{
		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
		std::uniform_real_distribution<float> randomFloats2(-1.0f, 1.0f);
		std::default_random_engine generator;

		ID3D11Device2* device = direct3DRenderer->GetDevice();

		for (uint32_t i = 0; i < KernelSize; ++i)
		{
			using namespace DirectX;

			float x = randomFloats2(generator);
			float y = randomFloats2(generator);
			float z = randomFloats(generator);

			DirectX::XMFLOAT4 sample(x, y, z, 0.0f);
			auto sampleVector = DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&sample));
			DirectX::XMStoreFloat4(&PSData.SSAOKernel[i], sampleVector);

			float scale = static_cast<float>(i) / static_cast<float>(KernelSize);
			scale = 0.1f + ((scale*scale)*(1.0f - 0.1f));
			sampleVector = DirectX::XMVectorScale(sampleVector, scale);
			DirectX::XMStoreFloat4(&PSData.SSAOKernel[i], sampleVector);

			//DirectX::XMFLOAT3 sample(x, y, z);
			//auto sampleVector = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&sample));
			//sampleVector = DirectX::XMVectorScale(sampleVector, randomFloats(generator));


			//float scale = static_cast<float>(i) / KernelSize;
			//
			//auto finalScale = 0.1f + ((scale*scale)*(1.0f - 0.1f));  //DirectX::XMVectorGetX(DirectX::XMVectorLerp(DirectX::XMVectorZero(), DirectX::XMLoadFloat3(&Utility::Right), scale * scale));
			//sampleVector = DirectX::XMVectorScale(sampleVector, finalScale);
			//DirectX::XMStoreFloat3(&sample, sampleVector);

			//DirectX::XMFLOAT4 sample4Float = DirectX::XMFLOAT4(sample.x, sample.y, sample.z, 0.0f);

			//PSData.SSAOKernel[i] = sample4Float;
			//SSAOKernel.push_back(sample);
		}

		for (uint32_t i = 0; i < NoiseSize; ++i)
		{
			float x = randomFloats2(generator);
			float y = randomFloats2(generator);
			float z = 0.0f; //randomFloats2(generator);

			DirectX::XMFLOAT4 noise(x, y, z, 0.0f);
			DirectX::XMStoreFloat4(&noise, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&noise)));

			Noise.push_back(noise);
		}

		D3D11_TEXTURE2D_DESC textureDescription;
		ZeroMemory(&textureDescription, sizeof(D3D11_TEXTURE2D_DESC));

		textureDescription.Width = 4U;
		textureDescription.Height = 4U;
		textureDescription.MipLevels = 1;
		textureDescription.ArraySize = 1;
		textureDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDescription.SampleDesc.Count = direct3DRenderer->GetMultiSamplingCount();
		textureDescription.SampleDesc.Quality = direct3DRenderer->GetMultiSamplingQualityLevels() - 1;
		textureDescription.Usage = D3D11_USAGE_DEFAULT;
		textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA subResourceData;
		ZeroMemory(&subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

		subResourceData.pSysMem = &Noise[0];
		subResourceData.SysMemPitch = sizeof(DirectX::XMFLOAT4) * 4;

		device->CreateTexture2D(&textureDescription, &subResourceData, NoiseTexture.ReleaseAndGetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDescription;
		ZeroMemory(&shaderViewDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		shaderViewDescription.Format = textureDescription.Format;
		shaderViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderViewDescription.Texture2D.MostDetailedMip = 0;
		shaderViewDescription.Texture2D.MipLevels = 1;

		device->CreateShaderResourceView(NoiseTexture.Get(), &shaderViewDescription, NoiseResource.ReleaseAndGetAddressOf());

		textureDescription.Width = static_cast<uint32_t>(direct3DRenderer->GetScreenWidth());
		textureDescription.Height = static_cast<uint32_t>(direct3DRenderer->GetScreenHeight());
		textureDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		device->CreateTexture2D(&textureDescription, nullptr, SSAOTexture.ReleaseAndGetAddressOf());
		device->CreateTexture2D(&textureDescription, nullptr, BlurTexture.ReleaseAndGetAddressOf());

		shaderViewDescription.Format = textureDescription.Format;

		device->CreateShaderResourceView(SSAOTexture.Get(), &shaderViewDescription, SSAOTextureResource.ReleaseAndGetAddressOf());
		device->CreateShaderResourceView(BlurTexture.Get(), &shaderViewDescription, SSAOBlurResource.ReleaseAndGetAddressOf());

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetDescription;
		ZeroMemory(&renderTargetDescription, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		renderTargetDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetDescription.Format = textureDescription.Format;
		renderTargetDescription.Texture2D.MipSlice = 0;

		device->CreateRenderTargetView(SSAOTexture.Get(), &renderTargetDescription, SSAORenderTarget.ReleaseAndGetAddressOf());
		device->CreateRenderTargetView(BlurTexture.Get(), &renderTargetDescription, BlurRenderTarget.ReleaseAndGetAddressOf());

		D3D11_BUFFER_DESC bufferDescription;
		ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));

		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.ByteWidth = sizeof(PixelShaderCBuffer);
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;

		device->CreateBuffer(&bufferDescription, nullptr, PSCBuffer.ReleaseAndGetAddressOf());

		PSData.NoiseScale.x = direct3DRenderer->GetScreenWidth() / 4.0f;
		PSData.NoiseScale.y = direct3DRenderer->GetScreenHeight() / 4.0f;
		PSData.Radius = 1.5f;

		BlurPixelShader = std::make_shared<Shader>(L"SSAOBlurPixelShader.cso", Shader::ShaderType::PixelShader, device);
	}
}
