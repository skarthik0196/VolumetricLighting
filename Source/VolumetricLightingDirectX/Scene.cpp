#include "pch.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

namespace Rendering
{
	Scene::Scene(SceneManager& sceneManagerReference) : SceneManagerReference(sceneManagerReference), MainCamera(std::make_shared<Camera>(sceneManagerReference.GetRenderer()->GetScreenWidth(), sceneManagerReference.GetRenderer()->GetScreenHeight())), Lights(std::make_shared<LightManager>(SceneManagerReference.GetRenderer()->GetDevice())),
														GBuffer1(std::make_shared<GBuffer>(sceneManagerReference.GetRenderer())), ScreenQuad1(std::make_shared<ScreenQuad>(sceneManagerReference.GetRenderer()->GetDevice()))
	{
		InitializeScene();
	}

	void Scene::InitializeScene()
	{
		ID3D11Device2* device = SceneManagerReference.GetRenderer()->GetDevice();

		DefaultVertexShader = std::make_shared<Shader>(L"DefaultDeferredGeometryVertexShader.cso", Shader::ShaderType::VertexShader, device);
		DefaultPixelShader = std::make_shared<Shader>(L"DefaultDeferredGeometryPixelShader.cso", Shader::ShaderType::PixelShader, device);

		D3D11_SAMPLER_DESC samplerDescription;
		ZeroMemory(&samplerDescription, sizeof(samplerDescription));
		samplerDescription.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
		samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		//samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
		//samplerDescription.MinLOD = 0;
		//samplerDescription.MipLODBias = 0.0f;
		//samplerDescription.MaxAnisotropy = 1;
		//samplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;

		device->CreateSamplerState(&samplerDescription, DefaultSamplerState.GetAddressOf());

		D3D11_BUFFER_DESC constantBufferDescription;
		ZeroMemory(&constantBufferDescription, sizeof(D3D11_BUFFER_DESC));

		constantBufferDescription.ByteWidth = sizeof(CBufferPerObject);
		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		device->CreateBuffer(&constantBufferDescription, nullptr, VSCBufferPerObject.GetAddressOf());

		// Bistro Start

		/*MainCamera->Rotate(MainCamera->GetRightVectorN(), -90.0f);
		MainCamera->Move(DirectX::XMFLOAT3(-750.0f, 0.0, -500.0f));

		Lights->GetAmbientLight()->SetIntensity(0.5f);
		Lights->GetDirectionalLight()->SetIntensity(1.0f);
		Lights->GetDirectionalLight()->ApplyRotation(Utility::Up, 30);

		std::uniform_real_distribution<float> distribution(-1500.0f, 1500.0f);
		std::uniform_real_distribution<float> colordistribution(0, 1);
		std::random_device rd;
		std::default_random_engine randomengine (rd());
		

		for (uint32_t i = 0; i < 40; ++i)
		{
			Lights->CreatePointLight(DirectX::XMFLOAT3(distribution(randomengine), distribution(randomengine), -500.0f), DirectX::XMFLOAT4(colordistribution(randomengine), colordistribution(randomengine), colordistribution(randomengine), colordistribution(randomengine)), 1.0f, 500.0f);
		}*/

		//Bistro End

		//Sponza Start

		MainCamera->Move(DirectX::XMFLOAT3(30.0f, 100.0f, 0.0f));
		MainCamera->Rotate(MainCamera->GetUpVector(), 90.0f);

		Lights->GetAmbientLight()->SetIntensity(0.2f);
		Lights->GetDirectionalLight()->SetIntensity(0.5f);
		Lights->GetDirectionalLight()->ApplyRotation(Utility::Right, 30);

		Lights->CreatePointLight(DirectX::XMFLOAT3(0.0f, 30.0f, 50.0f), Utility::Red, 1.0f, 150.0f);

		std::uniform_real_distribution<float> distribution(-250.0f, 250.0f);
		std::uniform_real_distribution<float> colordistribution(0, 1);
		std::default_random_engine randomengine;

		for (uint32_t i = 0; i < 40; ++i)
		{
			Lights->CreatePointLight(DirectX::XMFLOAT3((-750.0f + i*50.0f), distribution(randomengine), distribution(randomengine)), DirectX::XMFLOAT4(colordistribution(randomengine), colordistribution(randomengine), colordistribution(randomengine), colordistribution(randomengine)), 1.0f, 300.0f);
		}

		//Sponza End

		//Solar System Begin

		/*Lights->GetAmbientLight()->SetIntensity(0.5f);
		Lights->GetDirectionalLight()->SetIntensity(1.0f);
		Lights->GetDirectionalLight()->ApplyRotation(Utility::Up, 30);

		MainCamera->Move(DirectX::XMFLOAT3(0.0f, 0.0f, -30.0f));*/

		//Solar System End

		Lights->UpdateDirectionalCBufferData();
	}

	std::shared_ptr<Shader>& Scene::GetDefaultVertexShader()
	{
		return DefaultPixelShader;
	}

	std::shared_ptr<Shader>& Scene::GetDefaultPixelShader()
	{
		return DefaultPixelShader;
	}

	std::shared_ptr<LightManager>& Scene::GetLightManager()
	{
		return Lights;
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

		if (inputMap.at(InputManager::InputActions::IncreaseAttribute))
		{
			/*auto& pLight = Lights->GetPointLight(0);
			pLight->SetRadius(pLight->GetRadius() + 10.0f);*/
			
			MainCamera->SetNearPlane(MainCamera->GetNearPlane() +10.0f);
		}

		if (inputMap.at(InputManager::InputActions::DecreaseAttribute))
		{
			/*auto& pLight = Lights->GetPointLight(0);
			pLight->SetRadius(pLight->GetRadius() - 10.0f);*/

			MainCamera->SetNearPlane(MainCamera->GetNearPlane() - 10.0f);
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

		MainCamera->Rotate(MainCamera->GetRightVectorN(), mouseInput.x);
		MainCamera->Rotate(MainCamera->GetUpVectorN(), mouseInput.y);

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

		GBuffer1->SetRenderTargets(direct3DRenderer);
		GBuffer1->ClearRenderTargets(direct3DRenderer);

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->VSSetShader(DefaultVertexShader->GetVertexShader(), 0, 0);
		deviceContext->PSSetShader(DefaultPixelShader->GetPixelShader(), 0, 0);
		deviceContext->IASetInputLayout(DefaultVertexShader->GetInputLayout());

		deviceContext->VSSetConstantBuffers(0, 1, VSCBufferPerObject.GetAddressOf());
		
		deviceContext->PSSetSamplers(0, 1, DefaultSamplerState.GetAddressOf());

		DirectX::XMMATRIX viewProjectionMatrix = MainCamera->GetViewProjectionMatrix();

		MainCamera->UpdateFrustum();

		for (auto& gameObject : GameObjectList)
		{
			gameObject->Draw(direct3DRenderer, this , viewProjectionMatrix);
		}

		direct3DRenderer->SetSingleRenderTarget();
		direct3DRenderer->DisableDepthTesting();
		direct3DRenderer->BeginAdditiveBlending();

		ScreenQuad1->BindScreenQuadVertexShader(deviceContext);
		GBuffer1->BindGBufferData(deviceContext);

		deviceContext->PSSetShader(Lights->GetDirectionalLightPixelShader()->GetPixelShader(), 0, 0);
		Lights->BindDLightCBuffer(this, direct3DRenderer);
		ScreenQuad1->DrawScreenQuad(deviceContext);

		direct3DRenderer->EnableAdditiveBlending();

		Lights->RenderPointLights(this, direct3DRenderer, viewProjectionMatrix);

		direct3DRenderer->DisableBlending();
		direct3DRenderer->EnableDepthTesting();
		

		GBuffer1->UnBindBufferData(deviceContext);
		
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