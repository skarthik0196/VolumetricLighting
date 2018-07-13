#include "pch.h"
#include "SceneManager.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "GameObject.h"
#include "CelestialBody.h"

namespace Rendering
{
	SceneManager::SceneManager(const std::string& filePath, float screenWidth, float screenHeight) : Direct3DRenderer(std::make_shared<Direct3D>(screenWidth, screenHeight)), Input(std::make_shared<InputManager>(Direct3DRenderer->GetWindowHandle())), Font(std::make_shared<DirectX::SpriteFont>(Direct3DRenderer->GetDevice(), L"Content\\Fonts\\CourierNew.spritefont"))
																									,SpriteBatcher(std::make_shared<DirectX::SpriteBatch>(Direct3DRenderer->GetDeviceContext()))
	{
		LoadScene(filePath);
		InitializeQueries();
	}

	SceneManager::SceneManager(std::shared_ptr<Direct3D>& direct3DRenderer) : Direct3DRenderer(direct3DRenderer), Input(std::make_shared<InputManager>(Direct3DRenderer->GetWindowHandle())), Font(std::make_shared<DirectX::SpriteFont>(direct3DRenderer->GetDevice(), L"Content\\Fonts\\CourierNew.spritefont"))
																			, SpriteBatcher(std::make_shared<DirectX::SpriteBatch>(Direct3DRenderer->GetDeviceContext()))
	{
		CreateScene();
		InitializeQueries();
	}

	SceneManager::SceneManager(std::shared_ptr<Direct3D>& direct3DRenderer, const std::string & filePath) : Direct3DRenderer(direct3DRenderer), Input(std::make_shared<InputManager>(Direct3DRenderer->GetWindowHandle())), Font(std::make_shared<DirectX::SpriteFont>(direct3DRenderer->GetDevice(), L"Content\\Fonts\\CourierNew.spritefont"))
																											, SpriteBatcher(std::make_shared<DirectX::SpriteBatch>(Direct3DRenderer->GetDeviceContext()))
	{
		LoadScene(filePath);
		InitializeQueries();
	}

	void SceneManager::AddScene(std::shared_ptr<Scene> newScene)
	{
		SceneList.push_back(newScene);
	}

	void SceneManager::RemoveScene(std::shared_ptr<Scene> scene)
	{
		SceneList.erase(std::find(SceneList.begin(), SceneList.end(), scene));
	}

	std::shared_ptr<Scene> SceneManager::GetCurrentScene()
	{
		return SceneList[CurrentSceneIndex];
	}

	void SceneManager::CreateSolarSytemScene()
	{
		std::shared_ptr<Model> sphereModel= std::make_shared<Model>(Direct3DRenderer->GetDevice(), "Content\\Models\\Sphere.obj");

		std::shared_ptr<CelestialBody> earth= std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Earth.jpg", Direct3DRenderer->GetDevice(), 1.0f, 1.0f, -24.0f, 1.0f, 29.8f);

		auto& currentScene = SceneList.back();

		currentScene->AddGameObject(earth);

		std::shared_ptr<CelestialBody> venus = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Venus.jpg", Direct3DRenderer->GetDevice(), 0.723f, 0.949f, -177.4f, 0.004f, 35.0f);
		currentScene->AddGameObject(venus);

		std::shared_ptr<CelestialBody> mercury = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Mercury.jpg", Direct3DRenderer->GetDevice(), 0.387f, 0.382f, 0.0f, 0.0172f, 47.4f);
		currentScene->AddGameObject(mercury);

		std::shared_ptr<CelestialBody> mars = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Mars.jpg", Direct3DRenderer->GetDevice(), 1.524f, 0.532f, -2.0f, 0.625f, 24.1f);
		currentScene->AddGameObject(mars);

		std::shared_ptr<CelestialBody> jupiter = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Jupiter.jpg", Direct3DRenderer->GetDevice(), 5.203f, 11.19f, -3.04f, 2.5f, 13.1f);
		currentScene->AddGameObject(jupiter);

		std::shared_ptr<CelestialBody> saturn = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Saturn.jpg", Direct3DRenderer->GetDevice(), 9.582f, 9.26f, -27.0f, 2.40f, 9.6f);
		currentScene->AddGameObject(saturn);

		std::shared_ptr<CelestialBody> uranus = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Uranus.jpg", Direct3DRenderer->GetDevice(), 19.20f, 4.01f, -97.9f, 1.42f, 6.8f);
		currentScene->AddGameObject(uranus);

		std::shared_ptr<CelestialBody> neptune = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Neptune.jpg", Direct3DRenderer->GetDevice(), 30.05f, 3.88f, -29.6f, 1.5f, 5.4f);
		currentScene->AddGameObject(neptune);

		std::shared_ptr<CelestialBody> pluto = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Pluto.jpg", Direct3DRenderer->GetDevice(), 39.48f, 0.18f, -122.0f, 0.16f, 4.74f);
		currentScene->AddGameObject(pluto);

		std::shared_ptr<CelestialBody> sun = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Sun.jpg", Direct3DRenderer->GetDevice(), -0.01f);
		sun->SetScale(DirectX::XMFLOAT3(3.0f, 3.0f, 3.0f));

		std::shared_ptr<CelestialBody> moon = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Moon.jpg", Direct3DRenderer->GetDevice(), 1.1f, 0.27f, 0.0f, 1.0f, 5.5f);
		currentScene->AddGameObject(moon);

		auto& orbitPoint = sun->GetPosition();

		moon->SetParent(earth);
		earth->SetOrbitPoint(orbitPoint);

		mercury->SetOrbitPoint(orbitPoint);
		venus->SetOrbitPoint(orbitPoint);
		
		mars->SetOrbitPoint(orbitPoint);
		jupiter->SetOrbitPoint(orbitPoint);
		saturn->SetOrbitPoint(orbitPoint);
		uranus->SetOrbitPoint(orbitPoint);
		neptune->SetOrbitPoint(orbitPoint);
		pluto->SetOrbitPoint(orbitPoint);

		currentScene->AddGameObject(sun);
		currentScene->GetLightManager()->CreatePointLight(sun->GetPosition(), Utility::White, 1.0f, 100000.0f);
		
	}

	void SceneManager::LoadScene(const std::string& filePath)
	{
		std::shared_ptr<GameObject> sceneObject = std::make_shared<GameObject>(Transform(), filePath, true, Direct3DRenderer->GetDevice());
		
		std::shared_ptr<Scene> newScene = std::make_shared<Scene>(*this);
		newScene->AddGameObject(sceneObject);
		AddScene(newScene);
		CurrentSceneIndex = static_cast<uint32_t>(SceneList.size()) - 1;

		//sceneObject = std::make_shared<GameObject>(Transform(), filePath, true, Direct3DRenderer->GetDevice());
		//newScene->AddGameObject(sceneObject);
		//sceneObject->SetPosition(DirectX::XMFLOAT3(5.0f, 0.0f, 10.0f));
		////Move Out?
		//SceneList[CurrentSceneIndex]->GetGameObjectList()[0]->GetObjectModel()->GetMeshes()[0]->GetMaterial()->CreateTexture(Direct3DRenderer->GetDevice(), L"Content//Textures//EarthComposite.dds", Texture::TextureFileType::DDS, Texture::TextureType::Diffuse);
		//SceneList[CurrentSceneIndex]->GetGameObjectList()[1]->GetObjectModel()->GetMeshes()[0]->GetMaterial()->CreateTexture(Direct3DRenderer->GetDevice(), L"Content//Textures//EarthComposite.dds", Texture::TextureFileType::DDS, Texture::TextureType::Diffuse);
	}

	void SceneManager::CreateScene()
	{
		std::shared_ptr<Scene> newScene = std::make_shared<Scene>(*this);
		AddScene(newScene);
		CurrentSceneIndex = static_cast<uint32_t>(SceneList.size()) - 1;
	}

	void SceneManager::UpdateSceneManager()
	{
		Input->ProcessInput();
		SceneList[CurrentSceneIndex]->UpdateScene();
	}

	void SceneManager::DrawCurrentScene()
	{
		CollectQueryStats();
		Direct3DRenderer->BeginFrameQuery();
		Direct3DRenderer->ClearRenderTarget();
		Direct3DRenderer->ClearDepthStencilView();
		
		SceneList[CurrentSceneIndex]->DrawScene(Direct3DRenderer);

		RenderQueryData();

		Direct3DRenderer->RenderToScreen();
	}

	void SceneManager::BeginQuery(QueryTypes queryType)
	{
		if (queryType != QueryTypes::End)
		{
			QueryList[static_cast<uint32_t>(queryType)]->BeginQuery(Direct3DRenderer->GetDeviceContext());
		}
	}

	void SceneManager::EndQuery(QueryTypes queryType)
	{
		if (queryType != QueryTypes::End)
		{
			QueryList[static_cast<uint32_t>(queryType)]->EndQuery(Direct3DRenderer->GetDeviceContext());
		}
	}


	void SceneManager::InitializeQueries()
	{
		ID3D11Device2* device = Direct3DRenderer->GetDevice();

		for (QueryTypes i = QueryTypes::ShadowMap; i < QueryTypes::End; i = static_cast<QueryTypes>(static_cast<uint32_t>(i) + 1))
		{
			QueryList.push_back(std::make_shared<Query>(device));
		}
	}

	void SceneManager::CollectQueryStats()
	{
		for (QueryTypes i = QueryTypes::ShadowMap; i < QueryTypes::End; i = static_cast<QueryTypes>(static_cast<uint32_t>(i) + 1))
		{
			float time;
			time = QueryList[static_cast<uint32_t>(i)]->GetQueryTime(Direct3DRenderer->GetDeviceContext());
			//time = static_cast<float>(static_cast<int32_t>((time * 100 + 0.5f)));
			//time = (float)time / 100;
			Times[static_cast<uint32_t>(i)] = time;
		}
	}

	void SceneManager::RenderQueryData()
	{
		using namespace DirectX;
		static DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(50.0f, 50.0f);
		static DirectX::XMFLOAT4 scale = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);

		std::wstring queryDataString;
		queryDataString = L"Geomentry Pass : " + std::to_wstring(Times[static_cast<uint32_t>(QueryTypes::GeometryPass)]);
		queryDataString += L"\nShadowMap Pass : " + std::to_wstring(Times[static_cast<uint32_t>(QueryTypes::ShadowMap)]);
		queryDataString += L"\nSSAO Pass : " + std::to_wstring(Times[static_cast<uint32_t>(QueryTypes::SSAO)]);
		queryDataString += L"\nLighting Pass : " + std::to_wstring(Times[static_cast<uint32_t>(QueryTypes::LightingPass)]);
		queryDataString += L"\nDirectional Lighting Pass : " + std::to_wstring(Times[static_cast<uint32_t>(QueryTypes::DirectionalLighting)]);
		queryDataString += L"\nPoint Lighting Pass : " + std::to_wstring(Times[static_cast<uint32_t>(QueryTypes::PointLighting)]);
		queryDataString += L"\nPostProcessing : " + std::to_wstring(Times[static_cast<uint32_t>(QueryTypes::PostProcessing)]);;
		


		//for (QueryTypes i = QueryTypes::ShadowMap; i < QueryTypes::End; i = static_cast<QueryTypes>(static_cast<uint32_t>(i) + 1))
		//{
		//	queryDataString += std::to_wstring(Times[static_cast<uint32_t>(i)]);
		//}

		//ID3D11RasterizerState *rasterizerState;
		//Direct3DRenderer->GetDeviceContext()->RSGetState(&rasterizerState);

		SpriteBatcher->Begin();
		auto origin = Font->MeasureString(queryDataString.c_str()) / 2.0f;

		Font->DrawString(SpriteBatcher.get(), queryDataString.c_str(), DirectX::XMLoadFloat2(&position), DirectX::XMLoadFloat4(&Utility::White), 0.0f, DirectX::XMVectorZero(), DirectX::XMLoadFloat4(&scale));

		SpriteBatcher->End();
		
		//Direct3DRenderer->GetDeviceContext()->RSSetState(rasterizerState);
	}

	std::shared_ptr<Direct3D>& SceneManager::GetRenderer()
	{
		return Direct3DRenderer;
	}

	std::shared_ptr<InputManager>& SceneManager::GetInputManager()
	{
		return Input;
	}
}