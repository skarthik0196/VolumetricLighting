#pragma once
#include <wrl.h>
#include "DirectionalLight.h"
#include "PointLight.h"
#include "ShadowMap.h"

namespace Rendering
{
	class Scene;
	class Direct3D;
	class Shader;
	class Model;
	class Texture;

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

		struct PointLightVertexShaderData
		{
			DirectX::XMFLOAT4X4 WorldViewProjection{Utility::IdentityMatrix};
		};

		struct PointLightPixelShaderData
		{
			DirectX::XMFLOAT4 PointLightColor;
			DirectX::XMFLOAT4 PointLightPosition;
			DirectX::XMFLOAT3 CameraPosition;
			float Radius;
			DirectX::XMFLOAT2 ScreenResolution;
			float Padding1;
			float Padding2;
		};

		LightManager(ID3D11Device2* device);
		~LightManager();

		std::shared_ptr<DirectionalLight>& GetDirectionalLight();
		std::shared_ptr<Light>& GetAmbientLight();

		void Initialize(ID3D11Device2* device);

		void BindDLightCBuffer(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);

		void UpdateDirectionalCBufferData();

		std::shared_ptr<Shader>& GetDirectionalLightPixelShader();

		void RenderPointLights(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer, const DirectX::XMMATRIX& viewProjectionMatrix);
		std::shared_ptr<PointLight> CreatePointLight(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, float intensity, float radius);

		std::shared_ptr<PointLight>& GetPointLight(uint32_t index);

		void RenderPointLightSourceToScreen(int pointLightIndex, Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer, const DirectX::XMMATRIX& viewProjectionMatrix);

		void RenderDirectionalLightSourceToScreen(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer, const DirectX::XMMATRIX& viewProjectionMatrix);

		void CreateDirectionalLightShadowMap(std::shared_ptr<Direct3D>& direct3DRenderer);
		void RenderDirectionalLightShadowMap(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer);

	private:
		void UpdatePointLightCBufferData(uint32_t index, const DirectX::XMMATRIX& viewProjectionMatrix);

		DirectionalAmbientLightData DirectionalLightData;
		PointLightPixelShaderData PLightPSData;
		PointLightVertexShaderData PLightVSData;

		std::shared_ptr<DirectionalLight> DLight;
		std::shared_ptr<Light> AmbientLight;
		std::vector<std::shared_ptr<PointLight>> PointLightList;
		std::shared_ptr<Model> PointLightVolume;
		std::shared_ptr<Texture> LightSourceTexture;

		Microsoft::WRL::ComPtr<ID3D11Buffer> DLightCBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> PointLightPSCBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> PointLightVSCBuffer;

		std::shared_ptr<Shader> DirectionalLightPixelShader;

		std::shared_ptr<Shader> PointLightVertexShader;
		std::shared_ptr<Shader> PointLightPixelShader;
		std::shared_ptr<Shader> LightSourcePixelShader;

		std::shared_ptr<ShadowMap> DirectionalLightShadowMap;
	};
}