#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "InputManager.h"

namespace Rendering
{
	class SceneManager
	{
	public:
		SceneManager(const std::string& filePath, float screenWidth, float screenHeight);
		SceneManager(std::shared_ptr<Direct3D> direct3DRenderer);
		SceneManager(std::shared_ptr<Direct3D> direct3DRenderer, const std::string& filePath);
		~SceneManager() = default;

		void AddScene(std::shared_ptr<Scene> newScene);
		void RemoveScene(std::shared_ptr<Scene> scene);

		std::shared_ptr<Scene> GetCurrentScene();

		void CreateSolarSytemScene();
		void LoadScene(const std::string& filePath);
		void CreateScene();

		void UpdateSceneManager();
		void DrawCurrentScene();

		std::shared_ptr<Direct3D>& GetRenderer();
		std::shared_ptr<InputManager>& GetInputManager();

	private:

		std::shared_ptr<Direct3D> Direct3DRenderer;
		std::shared_ptr<InputManager> Input;
		std::vector<std::shared_ptr<Scene>> SceneList;
		uint32_t CurrentSceneIndex;
	};
}