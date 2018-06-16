#pragma once
#include "Texture.h"
#include "GameObject.h"

namespace Rendering
{
	class CelestialBody : public GameObject
	{
	public:
		CelestialBody(std::shared_ptr<Model> model, const std::wstring& texturePath = L"", ID3D11Device2* device = nullptr,  float AU = 1.0f, float relativeScale = 1.0f, float axialTilt = 0.0f, float rotationalPeriod =1.0f, float orbitalVelocity = 1.0f, Transform transform = Transform());
		virtual ~CelestialBody() = default;

		virtual void Update(Scene* currentScene);
		virtual void Draw(std::shared_ptr<Direct3D>& direct3DRenderer, Scene* currentScene, DirectX::XMMATRIX viewProjectionMatrix);
		void SetPlanetTexture(std::wstring filePath, ID3D11Device2* device);
		void SetPlanetTexture(std::shared_ptr<Texture> texture);

		void SetParent(std::shared_ptr<CelestialBody>& parent);

		void RotateAboutAxis(const DirectX::XMFLOAT3& axis, float angleInDegrees);
		void RotateAboutAxis(DirectX::XMVECTOR axis, float angleInDegrees);

		void SetOrbitPoint(const DirectX::XMFLOAT3& orbitPoint);
		
		void OrbitAboutPoint(const DirectX::XMFLOAT3& point);


	private:

		std::shared_ptr<CelestialBody> Parent{ nullptr };

		DirectX::XMFLOAT3 LocalForwardVector;
		DirectX::XMFLOAT3 LocalUpVector;
		DirectX::XMFLOAT3 LocalRightVector;

		DirectX::XMFLOAT3 OrbitPoint{ Utility::Zero };

		std::shared_ptr<Texture> CelestialTexture;

		float RotationalPeriod;
		
		float CurrentAngle{ 0.0f };
		float AxialTilt;
		float RelativeDistance;
		float RelativeScale;

		float OrbitalVelocity{ 1.0f };
		float CurrentOrbitalVelocity{ 0.0f };
		float OrbitRadius{ 0.0f };
	};
}