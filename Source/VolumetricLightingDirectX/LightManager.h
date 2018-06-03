#pragma once
#include "DirectionalLight.h"
#include <wrl.h>

namespace Rendering
{
	class Scene;
	class Direct3D;
	class Shader;

	class LightManager
	{
	public:
		struct DirectionalAmbientLightData
		{
			DirectX::XMFLOAT4 AmbientLightColor;
			DirectX::XMFLOAT4 DirectionalLightColor;
			DirectX::XMFLOAT3 DirectionalLightDirection;
			float Padding;
			DirectX::XMFLOAT3 CameraPosition;
			float Padding2;
		};

		LightManager(ID3D11Device2* device);
		~LightManager();

		std::shared_ptr<DirectionalLight>& GetDirectionalLight();
		std::shared_ptr<Light>& GetAmbientLight();

		void Initialize(ID3D11Device2* device);

		void BindDLightCBuffer(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);
		void UpdateCBufferData();

		std::shared_ptr<Shader>& GetPostProcessingShader();


	private:
		DirectionalAmbientLightData DirectionalLightData;

		std::shared_ptr<DirectionalLight> DLight;
		std::shared_ptr<Light> AmbientLight;

		Microsoft::WRL::ComPtr<ID3D11Buffer> DLightCBuffer;

		std::shared_ptr<Shader> PostProcessingShader;

	};
}