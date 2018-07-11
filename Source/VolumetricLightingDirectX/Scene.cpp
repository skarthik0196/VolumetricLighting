#include "pch.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "FXAA.h"
#include "PostProcessGodRays.h"
#include "HDRToneMapping.h"

namespace Rendering
{
	Scene::Scene(SceneManager& sceneManagerReference) : SceneManagerReference(sceneManagerReference), MainCamera(std::make_shared<Camera>(sceneManagerReference.GetRenderer()->GetScreenWidth(), sceneManagerReference.GetRenderer()->GetScreenHeight())), Lights(std::make_shared<LightManager>(SceneManagerReference.GetRenderer()->GetDevice())),
														GBuffer1(std::make_shared<GBuffer>(sceneManagerReference.GetRenderer())), ScreenQuad1(std::make_shared<ScreenQuad>(sceneManagerReference.GetRenderer()->GetDevice())),
														SceneSkyBox(std::make_shared<SkyBox>(sceneManagerReference.GetRenderer()->GetDevice(), *MainCamera, L"Content\\Textures\\SkyBoxTest.dds", Texture::TextureFileType::DDS))
	{
		InitializeScene();
	}

	void Scene::InitializeScene()
	{
		ID3D11Device2* device = SceneManagerReference.GetRenderer()->GetDevice();

		D3D11_INPUT_ELEMENT_DESC InputElementDescription[] =
		{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURECOORDINATE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		DefaultVertexShader = std::make_shared<Shader>(L"DefaultDeferredGeometryVertexShader.cso", Shader::ShaderType::VertexShader, device, InputElementDescription, static_cast<uint32_t>(ARRAYSIZE(InputElementDescription)));
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

		device->CreateSamplerState(&samplerDescription, DefaultSamplerState.ReleaseAndGetAddressOf());

		samplerDescription.Filter = D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;

		device->CreateSamplerState(&samplerDescription, PointWrapSamplerState.ReleaseAndGetAddressOf());

		samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		device->CreateSamplerState(&samplerDescription, PointClampSamplerState.ReleaseAndGetAddressOf());

		samplerDescription.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDescription.ComparisonFunc = D3D11_COMPARISON_LESS;

		device->CreateSamplerState(&samplerDescription, ShadowMapSamplerState.ReleaseAndGetAddressOf());

		D3D11_BUFFER_DESC constantBufferDescription;
		ZeroMemory(&constantBufferDescription, sizeof(D3D11_BUFFER_DESC));

		constantBufferDescription.ByteWidth = sizeof(CBufferPerObject);
		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		device->CreateBuffer(&constantBufferDescription, nullptr, VSCBufferPerObject.GetAddressOf());

		AddPostProcessingEffects(device);

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
		}


		Lights->GetDirectionalLight()->SetSourcePosition(DirectX::XMFLOAT3(-3000.0f, -1000.0f, -5000.0f));
		Lights->GetDirectionalLight()->SetSourceScale(DirectX::XMFLOAT3(20.0f, 20.0f, 20.0f));*/

		//Bistro End

		//Sponza Start

		MainCamera->Move(DirectX::XMFLOAT3(130.0f, 100.0f, 0.0f));
		MainCamera->Rotate(MainCamera->GetUpVector(), 90.0f);

		Lights->GetAmbientLight()->SetIntensity(0.01f);
		DirectX::XMFLOAT4 dLightColor = DirectX::XMFLOAT4(255.0f / 255.0f, 156.0f / 255.0f, 43.0f / 255.0f, 1.0f);
		Lights->GetDirectionalLight()->SetColor(dLightColor);
		Lights->GetDirectionalLight()->SetIntensity(32.0f);
		Lights->GetDirectionalLight()->ApplyRotation(Utility::Right, 75.0f);
		//Lights->GetDirectionalLight()->ApplyRotation(Utility::Up, 180.0f);

		auto PLight = Lights->CreatePointLight(DirectX::XMFLOAT3(0.0f, 30.0f, 50.0f), Utility::Red, 3.0f, 250.0f);
		PLight->SetIntensity(4.0f);

		/*std::uniform_real_distribution<float> distribution(-250.0f, 250.0f);
		std::uniform_real_distribution<float> colordistribution(0, 1);
		std::uniform_real_distribution<float> intensityDistribution(1.0f, 5.0f);
		std::default_random_engine randomengine;

		for (uint32_t i = 0; i < 40; ++i)
		{
			Lights->CreatePointLight(DirectX::XMFLOAT3((-750.0f + i*50.0f), distribution(randomengine), distribution(randomengine)), DirectX::XMFLOAT4(colordistribution(randomengine), colordistribution(randomengine), colordistribution(randomengine), colordistribution(randomengine)), intensityDistribution(randomengine), 300.0f);
		}*/

		Lights->GetDirectionalLight()->SetSourcePosition(DirectX::XMFLOAT3(0.0f, 500.0f, 0.0f));
		Lights->GetDirectionalLight()->SetSourceScale(DirectX::XMFLOAT3(20.0f, 20.0f, 20.0f));

		//Sponza End

		Lights->UpdateDirectionalCBufferData();
		Lights->CreateDirectionalLightShadowMap(SceneManagerReference.GetRenderer());
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

	std::shared_ptr<GBuffer>& Scene::GetGBuffer()
	{
		return GBuffer1;
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

		SceneSkyBox->Update(this);
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
			//auto& pLight = Lights->GetPointLight(0);
			//pLight->SetIntensity(pLight->GetIntensity() + 0.1f);
			//pLight->SetRadius(pLight->GetRadius() + 10.0f);
			
			//MainCamera->SetNearPlane(MainCamera->GetNearPlane() +10.0f);

			auto& dLight = Lights->GetDirectionalLight();
			//auto& sourcePosition = dLight->GetSourcePosition();

			//dLight->SetSourcePosition(DirectX::XMFLOAT3(sourcePosition.x , sourcePosition.y , sourcePosition.z + 10.0f));

			//dynamic_cast<PostProcessGodRays&>(*PostProcessList[1]).SetExposure(dynamic_cast<PostProcessGodRays&>(*PostProcessList[1]).GetExposure() + 0.0001f);

			//Lights->GetDirectionalLight()->ApplyRotation(Utility::Right, 0.2f);

			dLight->SetIntensity(dLight->GetIntensity() + 1.0f);

			SSAOPostProcess->SetRadius(SSAOPostProcess->GetRadius() + 0.01f);
		}

		if (inputMap.at(InputManager::InputActions::DecreaseAttribute))
		{
			//auto& pLight = Lights->GetPointLight(0);
			//pLight->SetIntensity(pLight->GetIntensity() - 0.1f);
			//pLight->SetRadius(pLight->GetRadius() - 10.0f);

			//MainCamera->SetNearPlane(MainCamera->GetNearPlane() - 10.0f);

			auto& dLight = Lights->GetDirectionalLight();
			//auto& sourcePosition = dLight->GetSourcePosition();

			//dLight->SetSourcePosition(DirectX::XMFLOAT3(sourcePosition.x , sourcePosition.y , sourcePosition.z - 10.0f));

			//dynamic_cast<PostProcessGodRays&>(*PostProcessList[1]).SetExposure(dynamic_cast<PostProcessGodRays&>(*PostProcessList[1]).GetExposure() - 0.0001f);

			//Lights->GetDirectionalLight()->ApplyRotation(Utility::Right, -0.2f);

			dLight->SetIntensity(dLight->GetIntensity() - 1.0f);

			SSAOPostProcess->SetRadius(SSAOPostProcess->GetRadius() - 0.01f);
		}

		if (inputMap.at(InputManager::InputActions::RotateDirectionalLightRightIncrease))
		{
			Lights->GetDirectionalLight()->ApplyRotation(Utility::Right, 0.2f);
		}

		if (inputMap.at(InputManager::InputActions::RotateDirectionalLightRightDecrease))
		{
			Lights->GetDirectionalLight()->ApplyRotation(Utility::Right, -0.2f);
		}

		if (inputMap.at(InputManager::InputActions::RotateDirectionalLightUpIncrease))
		{
			Lights->GetDirectionalLight()->ApplyRotation(Utility::Up, 0.2f);
		}

		if (inputMap.at(InputManager::InputActions::RotateDirectionalLightUpDecrease))
		{
			Lights->GetDirectionalLight()->ApplyRotation(Utility::Up, -0.2f);
		}

		if (inputMap.at(InputManager::InputActions::ToggleFXAA))
		{
			std::dynamic_pointer_cast<FXAA>(PostProcessList.back())->ToggleFXAA();
		}

		if (inputMap.at(InputManager::InputActions::ToggleHDR))
		{
			std::dynamic_pointer_cast<HDRToneMapping>(PostProcessList.front())->ToggleHDR();
		}

		if (inputMap.at(InputManager::InputActions::ToggleGammaCorrection))
		{
			std::dynamic_pointer_cast<HDRToneMapping>(PostProcessList.front())->ToggleGammaCorrection();
		}

		if (inputMap.at(InputManager::InputActions::IncreaseExposure))
		{
			//auto hdr = std::dynamic_pointer_cast<HDRToneMapping>(PostProcessList.front());
			//hdr->SetExposure(hdr->GetExposure() + 0.1f);

			SSAOPostProcess->SetIntensity(SSAOPostProcess->GetIntensity() + 0.01f);

		}

		if (inputMap.at(InputManager::InputActions::DecreaseExposure))
		{
			//auto hdr = std::dynamic_pointer_cast<HDRToneMapping>(PostProcessList.front());
			//hdr->SetExposure(hdr->GetExposure() - 0.1f);

			SSAOPostProcess->SetIntensity(SSAOPostProcess->GetIntensity() - 0.01f);

		}

		MainCamera->SetPosition(currentPosition);

		if (inputMap.at(InputManager::InputActions::Reset))
		{
			MainCamera->ResetCamera();
			auto hdr = std::dynamic_pointer_cast<HDRToneMapping>(PostProcessList.front());
			hdr->SetExposure(1.0f);
		}

		auto currentRotation = MainCamera->GetRotationAsFloat3();

		auto mouseInput = SceneManagerReference.GetInputManager()->GetMouseInput();
		mouseInput.x *= rotationGain;
		mouseInput.y *= rotationGain;

		MainCamera->Rotate(MainCamera->GetRightVectorN(), mouseInput.x);
		MainCamera->Rotate(Utility::Up/*MainCamera->GetUpVectorN()*/, mouseInput.y);

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

	void Scene::AddPostProcessingEffects(ID3D11Device2* device)
	{

		std::shared_ptr<HDRToneMapping> HDR = std::make_shared<HDRToneMapping>(device, ScreenQuad1);
		HDR->ToggleGammaCorrection();
		HDR->ToggleHDR();
		PostProcessList.push_back(HDR);

		//std::shared_ptr<PostProcessGodRays> LightShafts = std::make_shared<PostProcessGodRays>(SceneManagerReference.GetRenderer(), ScreenQuad1, L"PostProcessLightShaftsPixelShader.cso", Lights->GetDirectionalLight());
		//PostProcessList.push_back(LightShafts);
		//LightShafts->SetExposure(0.0f);

		std::shared_ptr<FXAA> PostProcessFXAA = std::make_shared<FXAA>(device, ScreenQuad1);
		PostProcessFXAA->SetScreenResolution(SceneManagerReference.GetRenderer()->GetScreenWidth(), SceneManagerReference.GetRenderer()->GetScreenHeight());;
		PostProcessList.push_back(PostProcessFXAA);

		SSAOPostProcess = std::make_shared<SSAO>(SceneManagerReference.GetRenderer(), ScreenQuad1, L"SSAOPixelShader.cso");

	}

	void Scene::DrawScene(std::shared_ptr<Direct3D>& direct3DRenderer)
	{
		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		GBuffer1->ClearAllRenderTargets(direct3DRenderer);

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Lights->RenderDirectionalLightShadowMap(this, direct3DRenderer);

		direct3DRenderer->SetViewPort();

		deviceContext->VSSetShader(DefaultVertexShader->GetVertexShader(), 0, 0);

		deviceContext->IASetInputLayout(DefaultVertexShader->GetInputLayout());

		deviceContext->VSSetConstantBuffers(0, 1, VSCBufferPerObject.GetAddressOf());
		deviceContext->PSSetConstantBuffers(0, 1, VSCBufferPerObject.GetAddressOf());
		
		static ID3D11SamplerState* samplerStates[4] = { DefaultSamplerState.Get(), ShadowMapSamplerState.Get(), PointClampSamplerState.Get(), PointWrapSamplerState.Get() };

		deviceContext->PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		DirectX::XMMATRIX viewProjectionMatrix = MainCamera->GetViewProjectionMatrix();

		MainCamera->UpdateFrustum();

		GBuffer1->SetAllRenderTargets(direct3DRenderer);
		deviceContext->PSSetShader(DefaultPixelShader->GetPixelShader(), 0, 0);
		for (auto& gameObject : GameObjectList)
		{
			gameObject->Draw(direct3DRenderer, this , viewProjectionMatrix);
		}

		//GBuffer1->SetRenderTarget(GBuffer::GBufferData::OcclusionMap, direct3DRenderer);
		//Lights->RenderDirectionalLightSourceToScreen(this, direct3DRenderer, viewProjectionMatrix);

		SSAOPostProcess->ApplyPostProcessing(this, direct3DRenderer);

		direct3DRenderer->SetSceneBufferRenderTarget();
		direct3DRenderer->DisableDepthWriting();
		direct3DRenderer->BeginAdditiveBlending();

		ScreenQuad1->BindScreenQuadVertexShader(deviceContext);
		//GBuffer1->BindGBufferData(deviceContext);
		auto shaderResources = GBuffer1->GetAllShaderResourceViews();

		shaderResources.push_back(Lights->GetDirectionalLightShadowMap()->GetShaderResourceView());
		shaderResources.push_back(SSAOPostProcess->GetFinalSSAOTextureResource());
		deviceContext->PSSetShaderResources(0, static_cast<uint32_t>(shaderResources.size()), shaderResources.data());

		deviceContext->PSSetShader(Lights->GetDirectionalLightPixelShader()->GetPixelShader(), 0, 0);
		Lights->BindDLightCBuffer(this, direct3DRenderer);
		ScreenQuad1->DrawScreenQuad(deviceContext);

		GBuffer1->BindGBufferData(deviceContext);
		direct3DRenderer->EnableAdditiveBlending();

		Lights->RenderPointLights(this, direct3DRenderer, viewProjectionMatrix);

		direct3DRenderer->DisableBlending();
		GBuffer1->UnBindBufferData(deviceContext);

		SceneSkyBox->Draw(direct3DRenderer, this, viewProjectionMatrix);
		direct3DRenderer->DisableDepthTesting();

		for (auto& postProcess : PostProcessList)
		{
			postProcess->ApplyPostProcessing(this, direct3DRenderer);
		}

		direct3DRenderer->EnableDepthTesting();
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