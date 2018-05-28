#include "pch.h"
#include "SceneManager.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "GameObject.h"

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