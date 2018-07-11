#pragma once
#include "GameObject.h"
#include "Direct3D.h"
#include "Camera.h"
#include "Shader.h"
#include "LightManager.h"
#include "GBuffer.h"
#include "ScreenQuad.h"
#include "PostProcessing.h"
#include "SkyBox.h"
#include "SSAO.h"

namespace Rendering
{
	class SceneManager;

	class Scene
	{
	public:
		Scene(SceneManager& sceneManagerReference);
		~Scene() = default;

		void UpdateScene();
		void DrawScene(std::shared_ptr<Direct3D>& direct3DRenderer);

		void AddGameObject(std::shared_ptr<GameObject> gameObject);
		void RemoveGameObject(std::shared_ptr<GameObject> gameObject);

		void CreateGameObject(Transform transform = Transform(), const std::string& ModelPath = "", bool flipUVs = false, ID3D11Device2* device = nullptr);

		void InitializeScene();

		std::shared_ptr<Shader>& GetDefaultVertexShader();
		std::shared_ptr<Shader>& GetDefaultPixelShader();
		std::shared_ptr<LightManager>& GetLightManager();

		ID3D11Buffer* GetVSCBufferPerObject();
		ID3D11Buffer* GetVSCBufferPerFrame();
		ID3D11Buffer* GetPSCBufferPerObject();
		ID3D11Buffer* GetPSCBufferPerFrame();

		std::shared_ptr<Camera>& GetCamera();

		std::shared_ptr<GBuffer>& GetGBuffer();

		std::vector<std::shared_ptr<GameObject>>& GetGameObjectList();

	private:
		void HandleInput();
		void AddPostProcessingEffects(ID3D11Device2* device);

		SceneManager& SceneManagerReference;
		std::vector<std::shared_ptr<GameObject>> GameObjectList;
		std::vector <std::shared_ptr<PostProcessing>> PostProcessList;

		std::shared_ptr<SSAO> SSAOPostProcess;
		
		std::shared_ptr<Camera> MainCamera;
		std::shared_ptr<LightManager> Lights;
		std::shared_ptr<GBuffer> GBuffer1;
		std::shared_ptr<ScreenQuad> ScreenQuad1;
		std::shared_ptr<SkyBox> SceneSkyBox;

		std::shared_ptr<Shader> DefaultVertexShader;
		std::shared_ptr<Shader> DefaultPixelShader;

		Microsoft::WRL::ComPtr<ID3D11Buffer> VSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> PSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> VSCBufferPerFrame;
		Microsoft::WRL::ComPtr<ID3D11Buffer> PSCBufferPerFrame;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> DefaultSamplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> ShadowMapSamplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> PointClampSamplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> PointWrapSamplerState;
	};
}

