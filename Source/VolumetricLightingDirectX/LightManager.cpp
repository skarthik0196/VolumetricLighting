#include "pch.h"
#include "LightManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"

namespace Rendering
{
	LightManager::LightManager(ID3D11Device2* device) : AmbientLight(std::make_shared<Light>()), DLight(std::make_shared<DirectionalLight>()), DirectionalLightPixelShader(std::make_shared<Shader>(L"PostProcessingPixelShader.cso", Shader::ShaderType::PixelShader, device)),
														PointLightPixelShader(std::make_shared<Shader>(L"PointLightPixelShader.cso", Shader::ShaderType::PixelShader, device)), PointLightVolume(std::make_shared<Model>(device, "Content\\Models\\Sphere.obj", true)),
														PointLightVertexShader(std::make_shared<Shader>(L"PointLightVertexShader.cso", Shader::ShaderType::VertexShader, device))
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

	std::shared_ptr<Shader>& LightManager::GetDirectionalLightPixelShader()
	{
		return DirectionalLightPixelShader;
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

		constantBufferDescription.ByteWidth = sizeof(PointLightPixelShaderData);
		device->CreateBuffer(&constantBufferDescription, nullptr, PointLightPSCBuffer.ReleaseAndGetAddressOf());

		constantBufferDescription.ByteWidth = sizeof(PointLightVertexShaderData);
		device->CreateBuffer(&constantBufferDescription, nullptr, PointLightVSCBuffer.ReleaseAndGetAddressOf());

		UpdateDirectionalCBufferData();
	}

	void LightManager::BindDLightCBuffer(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		DirectionalLightData.CameraPosition = scene->GetCamera()->GetPositionAsFloat3();

		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();
		deviceContext->UpdateSubresource(DLightCBuffer.Get(), 0, nullptr, &DirectionalLightData, 0, 0);
		deviceContext->PSSetConstantBuffers(0, 1, DLightCBuffer.GetAddressOf());
	}

	void LightManager::UpdateDirectionalCBufferData()
	{
		DirectionalLightData.AmbientLightColor = AmbientLight->GetColor();
		DirectionalLightData.DirectionalLightColor = DLight->GetColor();
		DirectionalLightData.DirectionalLightDirection = DLight->GetDirectionAsFloat3();
	}

	void LightManager::UpdatePointLightCBufferData(uint32_t index, const DirectX::XMMATRIX& viewProjectionMatrix)
	{
		auto& pointLight = PointLightList[index];
		PLightPSData.PointLightColor = pointLight->GetColor();

		auto& position = pointLight->GetPosition();
		PLightPSData.PointLightPosition = DirectX::XMFLOAT4(position.x, position.y, position.z, 1.0f);
		PLightPSData.Radius = pointLight->GetRadius();

		DirectX::XMStoreFloat4x4(&PLightVSData.WorldViewProjection, DirectX::XMMatrixTranspose(pointLight->GetWorldMatrix() * viewProjectionMatrix));
	}

	void LightManager::RenderPointLights(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer, const DirectX::XMMATRIX& viewProjectionMatrix)
	{
		static auto& pointLightVolume = PointLightVolume->GetMeshes()[0];
		static auto pointLightVB = pointLightVolume->GetAddressOfVertexBuffer();
		static auto pointLightIB = pointLightVolume->GetIndexBuffer();
		static uint32_t indexCount = pointLightVolume->GetIndexCount();
		static uint32_t stride = sizeof(Vertex);
		static uint32_t offset = 0;

		PLightPSData.CameraPosition = scene->GetCamera()->GetPositionAsFloat3();
		PLightPSData.ScreenResolution.x = scene->GetCamera()->GetScreenWidth();
		PLightPSData.ScreenResolution.y = scene->GetCamera()->GetScreenHeight();

		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		deviceContext->IASetInputLayout(PointLightVertexShader->GetInputLayout());
		deviceContext->IASetVertexBuffers(0, 1, pointLightVB, &stride, &offset);
		deviceContext->IASetIndexBuffer(pointLightIB, DXGI_FORMAT_R32_UINT, 0);
		
		deviceContext->VSSetShader(PointLightVertexShader->GetVertexShader(), 0, 0);
		deviceContext->PSSetShader(PointLightPixelShader->GetPixelShader(), 0, 0);
		deviceContext->PSSetConstantBuffers(0, 1, PointLightPSCBuffer.GetAddressOf());
		deviceContext->VSSetConstantBuffers(0, 1, PointLightVSCBuffer.GetAddressOf());

		for (uint32_t i = 0; i < PointLightList.size(); ++i)
		{
			UpdatePointLightCBufferData(i, viewProjectionMatrix);
			deviceContext->UpdateSubresource(PointLightVSCBuffer.Get(), 0, nullptr, &PLightVSData, 0, 0);
			deviceContext->UpdateSubresource(PointLightPSCBuffer.Get(), 0, nullptr, &PLightPSData, 0, 0);
			deviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}

	std::shared_ptr<PointLight> LightManager::CreatePointLight(const DirectX::XMFLOAT3 & position, const DirectX::XMFLOAT4 & color, float intensity, float radius)
	{
		std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>(position, radius, color, intensity);
		PointLightList.push_back(pointLight);

		return pointLight;
	}

	std::shared_ptr<PointLight>& LightManager::GetPointLight(uint32_t index)
	{
		return PointLightList[index];
	}
}