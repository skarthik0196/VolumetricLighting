#include "pch.h"
#include "SceneManager.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "GameObject.h"
#include "CelestialBody.h"

namespace Rendering
{
	SceneManager::SceneManager(const std::string& filePath, float screenWidth, float screenHeight) : Direct3DRenderer(std::make_shared<Direct3D>(screenWidth, screenHeight)), Input(std::make_shared<InputManager>(Direct3DRenderer->GetWindowHandle()))
	{
		LoadScene(filePath);
	}

	SceneManager::SceneManager(std::shared_ptr<Direct3D> direct3DRenderer) : Direct3DRenderer(direct3DRenderer), Input(std::make_shared<InputManager>(Direct3DRenderer->GetWindowHandle()))
	{
		CreateScene();
	}

	SceneManager::SceneManager(std::shared_ptr<Direct3D> direct3DRenderer, const std::string & filePath) : Direct3DRenderer(direct3DRenderer), Input(std::make_shared<InputManager>(Direct3DRenderer->GetWindowHandle()))
	{
		LoadScene(filePath);
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
		std::shared_ptr<GameObject> earth= std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Earth.jpg", Direct3DRenderer->GetDevice());

		auto& currentScene = SceneList.back();

		currentScene->AddGameObject(earth);

		std::shared_ptr<GameObject> venus = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Venus.jpg", Direct3DRenderer->GetDevice(), 0.723f, 0.949f);
		currentScene->AddGameObject(venus);

		std::shared_ptr<GameObject> mercury = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Mercury.jpg", Direct3DRenderer->GetDevice(), 0.387f, 0.382f);
		currentScene->AddGameObject(mercury);

		std::shared_ptr<GameObject> mars = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Mars.jpg", Direct3DRenderer->GetDevice(), 1.524f, 0.532f);
		currentScene->AddGameObject(mars);

		std::shared_ptr<GameObject> jupiter = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Jupiter.jpg", Direct3DRenderer->GetDevice(), 5.203f, 11.19f);
		currentScene->AddGameObject(jupiter);

		std::shared_ptr<GameObject> saturn = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Saturn.jpg", Direct3DRenderer->GetDevice(), 9.582f, 9.26f);
		currentScene->AddGameObject(saturn);

		std::shared_ptr<GameObject> uranus = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Uranus.jpg", Direct3DRenderer->GetDevice(), 19.20f, 4.01f);
		currentScene->AddGameObject(uranus);

		std::shared_ptr<GameObject> neptune = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Neptune.jpg", Direct3DRenderer->GetDevice(), 30.05f, 3.88f);
		currentScene->AddGameObject(neptune);

		std::shared_ptr<GameObject> pluto = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Pluto.jpg", Direct3DRenderer->GetDevice(), 39.48f, 0.18f);
		currentScene->AddGameObject(pluto);

		std::shared_ptr<GameObject> sun = std::make_shared<CelestialBody>(sphereModel, L"Content\\Textures\\PlanetTextures\\Sun.jpg", Direct3DRenderer->GetDevice(), -0.01f);
		currentScene->AddGameObject(sun);
		sun->SetScale(DirectX::XMFLOAT3(3.0f, 3.0f, 3.0f));
		//currentScene->GetLightManager()->CreatePointLight(sun->GetPosition(), Utility::White, 1.0f, 100000.0f);
		
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
		Direct3DRenderer->ClearRenderTarget();
		Direct3DRenderer->ClearDepthStencilView();
		
		SceneList[CurrentSceneIndex]->DrawScene(Direct3DRenderer);

		Direct3DRenderer->RenderToScreen();
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