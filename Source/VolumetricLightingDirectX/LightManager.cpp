#include "pch.h"
#include "LightManager.h"
#include "Scene.h"
#include "Camera.h"

namespace Rendering
{
	LightManager::LightManager(ID3D11Device2* device) : AmbientLight(std::make_shared<Light>()), DLight(std::make_shared<DirectionalLight>()), PostProcessingShader(std::make_shared<Shader>(L"PostProcessingPixelShader.cso", Shader::ShaderType::PixelShader, device))
	{
		Initialize(device);
	}

	LightManager::~LightManager()
	{
	}

	std::shared_ptr<DirectionalLight>& LightManager::GetDirectionalLight()
	{
		return DLight;
	}

	std::shared_ptr<Light>& LightManager::GetAmbientLight()
	{
		return AmbientLight;
	}

	void LightManager::Initialize(ID3D11Device2* device)
	{
		D3D11_BUFFER_DESC constantBufferDescription{ 0 };

		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDescription.ByteWidth = sizeof(DirectionalAmbientLightData);
		constantBufferDescription.Usage = D3D11_USAGE_DEFAULT;

		device->CreateBuffer(&constantBufferDescription, nullptr, DLightCBuffer.ReleaseAndGetAddressOf());

		UpdateCBufferData();
	}

	void LightManager::BindDLightCBuffer(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		DirectionalLightData.CameraPosition = scene->GetCamera()->GetPositionAsFloat3();

		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();
		deviceContext->UpdateSubresource(DLightCBuffer.Get(), 0, nullptr, &DirectionalLightData, 0, 0);
		deviceContext->PSSetConstantBuffers(0, 1, DLightCBuffer.GetAddressOf());
	}

	void LightManager::UpdateCBufferData()
	{
		DirectionalLightData.AmbientLightColor = AmbientLight->GetColor();
		DirectionalLightData.DirectionalLightColor = DLight->GetColor();
		DirectionalLightData.DirectionalLightDirection = DLight->GetDirectionAsFloat3();
	}

	std::shared_ptr<Shader>& LightManager::GetPostProcessingShader()
	{
		return PostProcessingShader;
	}
}