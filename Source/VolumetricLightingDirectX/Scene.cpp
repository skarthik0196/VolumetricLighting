#include "pch.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

namespace Rendering
{
	Scene::Scene(SceneManager& sceneManagerReference) : SceneManagerReference(sceneManagerReference), MainCamera(std::make_shared<Camera>()), Lights(std::make_shared<LightManager>(SceneManagerReference.GetRenderer()->GetDevice()))
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

		Lights->GetAmbientLight()->SetIntensity(0.5f);
		Lights->GetDirectionalLight()->SetIntensity(1.5f);
		Lights->GetDirectionalLight()->ApplyRotation(Utility::Right, -30);

		Lights->UpdateCBufferData();
	}

	std::shared_ptr<Shader>& Scene::GetDefaultVertexShader()
	{
		return DefaultPixelShader;
	}

	std::shared_ptr<Shader>& Scene::GetDefaultPixelShader()
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

	std::shared_ptr<Camera>& Scene::GetCamera()
	{
		return MainCamera;
	}

	std::vector<std::shared_ptr<GameObject>>& Scene::GetGameObjectList()
	{
		return GameObjectList;
	}

	void Scene::UpdateScene()
	{
		HandleInput();

		//MainCamera->Rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), 0.5f);
		//auto currentRotation = MainCamera->GetRotationAsFloat3();

		//currentRotation.y += 0.5f;

		//MainCamera->SetRotation(currentRotation);

		for (auto& gameObject : GameObjectList)
		{
			gameObject->Update(this);
		}
	}

	void Scene::HandleInput()
	{
		static const float speed = 5.0f;
		static const float rotationGain = 0.25f;

		const std::map<InputManager::InputActions, bool>& inputMap = SceneManagerReference.GetInputManager()->GetInput();

		auto currentPosition = MainCamera->GetPosition();
		if (inputMap.at(InputManager::InputActions::Forward))
		{
			currentPosition = DirectX::XMVectorAdd(currentPosition, DirectX::XMVectorScale(MainCamera->GetForwardVectorN(), speed));
		}

		if (inputMap.at(InputManager::InputActions::Backward))
		{
			currentPosition = DirectX::XMVectorAdd(currentPosition, DirectX::XMVectorScale(DirectX::XMVectorNegate(MainCamera->GetForwardVectorN()), speed));
		}

		if (inputMap.at(InputManager::InputActions::StrafeLeft))
		{
			currentPosition = DirectX::XMVectorAdd(currentPosition, DirectX::XMVectorScale(DirectX::XMVectorNegate(MainCamera->GetRightVectorN()), speed));
		}

		if (inputMap.at(InputManager::InputActions::StrafeRight))
		{
			currentPosition = DirectX::XMVectorAdd(currentPosition, DirectX::XMVectorScale(MainCamera->GetRightVectorN(), speed));
		}

		if (inputMap.at(InputManager::InputActions::Up))
		{
			currentPosition = DirectX::XMVectorAdd(currentPosition, DirectX::XMVectorScale(MainCamera->GetUpVectorN(), speed));
		}

		if (inputMap.at(InputManager::InputActions::Down))
		{
			currentPosition = DirectX::XMVectorAdd(currentPosition, DirectX::XMVectorScale(DirectX::XMVectorNegate(MainCamera->GetUpVectorN()), speed));
		}

		MainCamera->SetPosition(currentPosition);

		if (inputMap.at(InputManager::InputActions::Reset))
		{
			MainCamera->ResetCamera();
		}

		auto currentRotation = MainCamera->GetRotationAsFloat3();

		auto mouseInput = SceneManagerReference.GetInputManager()->GetMouseInput();
		mouseInput.x *= rotationGain;
		mouseInput.y *= rotationGain;

		MainCamera->Rotate(MainCamera->GetRightVectorN(), -mouseInput.x);
		MainCamera->Rotate(MainCamera->GetUpVectorN(), -mouseInput.y);

		/*currentRotation.x -= mouseInput.x;
		currentRotation.y -= mouseInput.y;

		float limit = 90.0f;
		currentRotation.x = std::max<float>(-limit, currentRotation.x);
		currentRotation.x= std::min<float>(+limit, currentRotation.x);

		if (currentRotation.y > 180.0f)
		{
			currentRotation.y -= 360.0f;
		}
		else if (currentRotation.y < -180.0f)
		{
			currentRotation.y += 360.0f;
		}

		MainCamera->SetRotation(currentRotation);*/
	}

	void Scene::DrawScene(std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->VSSetShader(DefaultVertexShader->GetVertexShader(), 0, 0);
		deviceContext->PSSetShader(DefaultPixelShader->GetPixelShader(), 0, 0);
		deviceContext->IASetInputLayout(DefaultVertexShader->GetInputLayout());

		deviceContext->VSSetConstantBuffers(0, 1, VSCBufferPerObject.GetAddressOf());
		
		deviceContext->PSSetSamplers(0, 1, DefaultSamplerState.GetAddressOf());

		Lights->BindDLightCBuffer(this, direct3DRenderer);

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