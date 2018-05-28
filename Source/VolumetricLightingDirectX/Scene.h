#pragma once
#include "GameObject.h"
#include "Direct3D.h"
#include "Camera.h"
#include "Shader.h"

namespace Rendering
{
	class SceneManager;

	class Scene
	{
	public:
		Scene(SceneManager& sceneManagerReference);
		~Scene() = default;

		void UpdateScene();
		void DrawScene(std::shared_ptr<Direct3D> direct3DRenderer);

		void AddGameObject(std::shared_ptr<GameObject> gameObject);
		void RemoveGameObject(std::shared_ptr<GameObject> gameObject);

		void CreateGameObject(Transform transform = Transform(), const std::string& ModelPath = "", bool flipUVs = false, ID3D11Device2* device = nullptr);

		void InitializeScene();

		std::shared_ptr<Shader> GetDefaultVertexShader();
		std::shared_ptr<Shader> GetDefaultPixelShader();

		ID3D11Buffer* GetVSCBufferPerObject();
		ID3D11Buffer* GetVSCBufferPerFrame();
		ID3D11Buffer* GetPSCBufferPerObject();
		ID3D11Buffer* GetPSCBufferPerFrame();

		std::vector<std::shared_ptr<GameObject>>& GetGameObjectList();

	private:
		void HandleInput();

		std::vector<std::shared_ptr<GameObject>> GameObjectList;

		std::shared_ptr<Camera> MainCamera;

		std::shared_ptr<Shader> DefaultVertexShader;
		std::shared_ptr<Shader> DefaultPixelShader;

		Microsoft::WRL::ComPtr<ID3D11Buffer> VSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> PSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> VSCBufferPerFrame;
		Microsoft::WRL::ComPtr<ID3D11Buffer> PSCBufferPerFrame;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> DefaultSamplerState;

		SceneManager& SceneManagerReference;
	};
}

