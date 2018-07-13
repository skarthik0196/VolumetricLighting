#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Query.h"
#include "SpriteFont.h"

namespace Rendering
{
	class SceneManager
	{
	public:
		enum class QueryTypes
		{
			ShadowMap,
			GeometryPass,
			LightingPass,
			DirectionalLighting,
			PointLighting,
			SSAO,
			PostProcessing,
			End
		};

		SceneManager(const std::string& filePath, float screenWidth, float screenHeight);
		SceneManager(std::shared_ptr<Direct3D>& direct3DRenderer);
		SceneManager(std::shared_ptr<Direct3D>& direct3DRenderer, const std::string& filePath);
		~SceneManager() = default;

		void AddScene(std::shared_ptr<Scene> newScene);
		void RemoveScene(std::shared_ptr<Scene> scene);

		std::shared_ptr<Scene> GetCurrentScene();

		void CreateSolarSytemScene();
		void LoadScene(const std::string& filePath);
		void CreateScene();

		void UpdateSceneManager();
		void DrawCurrentScene();

		void BeginQuery(QueryTypes queryType);
		void EndQuery(QueryTypes queryType);

		void InitializeQueries();
		void CollectQueryStats();

		void RenderQueryData();

		std::shared_ptr<Direct3D>& GetRenderer();
		std::shared_ptr<InputManager>& GetInputManager();

	private:
		float Times[static_cast<uint32_t>(QueryTypes::End)];
		std::vector<std::shared_ptr<Query>> QueryList;

		std::shared_ptr<Direct3D> Direct3DRenderer;
		std::shared_ptr<InputManager> Input;
		std::shared_ptr<DirectX::SpriteFont> Font;
		std::shared_ptr<DirectX::SpriteBatch> SpriteBatcher;

		std::vector<std::shared_ptr<Scene>> SceneList;
		uint32_t CurrentSceneIndex;
	};
}