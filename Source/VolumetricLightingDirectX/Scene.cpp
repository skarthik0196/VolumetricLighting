#include "pch.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

namespace Rendering
{
	Scene::Scene(SceneManager& sceneManagerReference) : SceneManagerReference(sceneManagerReference), MainCamera(std::make_shared<Camera>())
	{
		InitializeScene();
	}

	void Scene::InitializeScene()
	{
		ID3D11Device2* device = SceneManagerReference.GetRenderer()->GetDevice();

		DefaultVertexShader = std::make_shared<Shader>(L"DefaultVertexShader.cso", Shader::ShaderType::VertexShader, device);
		DefaultPixelShader = std::make_shared<Shader>(L"DefaultPixelShader.cso", Shader::ShaderType::PixelShader, device);

		D3D11_SAMPLER_DESC samplerDescription;
		ZeroMemory(&samplerDescription, sizeof(samplerDescription));
		samplerDescription.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
		samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		device->CreateSamplerState(&samplerDescription, DefaultSamplerState.GetAddressOf());

		D3D11_BUFFER_DESC constantBufferDescription;
		ZeroMemory(&constantBufferDescription, sizeof(D3D11_BUFFER_DESC));

		constantBufferDescription.ByteWidth = sizeof(CBufferPerObject);
		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		device->CreateBuffer(&constantBufferDescription, nullptr, VSCBufferPerObject.GetAddressOf());

		MainCamera->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 30.0f));
	}

	std::shared_ptr<Shader> Scene::GetDefaultVertexShader()
	{
		return DefaultPixelShader;
	}

	std::shared_ptr<Shader> Scene::GetDefaultPixelShader()
	{
		return DefaultPixelShader;
	}

	ID3D11Buffer* Scene::GetVSCBufferPerObject()
	{
		return VSCBufferPerObject.Get();
	}

	ID3D11Buffer* Scene::GetVSCBufferPerFrame()
	{
		return VSCBufferPerFrame.Get();
	}

	ID3D11Buffer* Scene::GetPSCBufferPerObject()
	{
		return PSCBufferPerObject.Get();
	}

	ID3D11Buffer* Scene::GetPSCBufferPerFrame()
	{
		return PSCBufferPerFrame.Get();
	}

	std::vector<std::shared_ptr<GameObject>>& Scene::GetGameObjectList()
	{
		return GameObjectList;
	}

	void Scene::UpdateScene()
	{
		//auto currentPosition = MainCamera->GetPositionAsFloat3();
		//currentPosition.x += 0.1f;
		//MainCamera->SetPosition(currentPosition);

		for (auto& gameObject : GameObjectList)
		{
			gameObject->Update(this);
		}
	}

	void Scene::DrawScene(std::shared_ptr<Direct3D> direct3DRenderer)
	{
		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		deviceContext->VSSetShader(DefaultVertexShader->GetVertexShader(), 0, 0);
		deviceContext->PSSetShader(DefaultPixelShader->GetPixelShader(), 0, 0);
		deviceContext->IASetInputLayout(DefaultVertexShader->GetInputLayout());

		deviceContext->VSSetConstantBuffers(0, 1, VSCBufferPerObject.GetAddressOf());
		
		deviceContext->PSSetSamplers(0, 1, DefaultSamplerState.GetAddressOf());

		DirectX::XMMATRIX viewProjectionMatrix = MainCamera->GetViewProjectionMatrix();

		for (auto& gameObject : GameObjectList)
		{
			gameObject->Draw(direct3DRenderer, this , viewProjectionMatrix);
		}
	}

	void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject)
	{
		GameObjectList.push_back(gameObject);
	}

	void Scene::RemoveGameObject(std::shared_ptr<GameObject> gameObject)
	{
		GameObjectList.erase(std::find(GameObjectList.begin(), GameObjectList.end(), gameObject));
	}

	void Scene::CreateGameObject(Transform transform, const std::string& ModelPath, bool flipUVs, ID3D11Device2* device)
	{
		std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(transform, ModelPath, flipUVs, device);
		AddGameObject(gameObject);
	}

}