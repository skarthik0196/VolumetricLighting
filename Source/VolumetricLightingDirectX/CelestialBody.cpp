#include "pch.h"
#include "CelestialBody.h"
#include "Direct3D.h"

#define PositionScaleFactor 100
#define RotationScaleFactor 5
#define OrbitalVelocityScaleFactor 0.01f

namespace Rendering
{
	CelestialBody::CelestialBody(std::shared_ptr<Model> model, const std::wstring& texturePath, ID3D11Device2* device, float AU, float relativeScale, float axialTilt, float rotationalPeriod, float orbitalVelocity,  Transform transform) : GameObject(transform, model) , AxialTilt(axialTilt),
			RelativeDistance(AU), RelativeScale(relativeScale), LocalForwardVector(Utility::Forward), LocalUpVector(Utility::Up), LocalRightVector(Utility::Right), RotationalPeriod(rotationalPeriod), OrbitalVelocity(orbitalVelocity)
	{

		ObjectTransform.Position = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		ObjectTransform.Position.z *= (AU * PositionScaleFactor);
		
		SetScale(DirectX::XMFLOAT3(RelativeScale, RelativeScale, RelativeScale));
		RotateAboutAxis(Utility::Forward, AxialTilt);
		
		UpdateWorldMatrix();

		if (!texturePath.empty() && device != nullptr)
		{
			SetPlanetTexture(texturePath, device);
		}
	}

	void CelestialBody::Update(Scene * currentScene)
	{
		UNREFERENCED_PARAMETER(currentScene);
		//SetPosition(DirectX::XMFLOAT3(ObjectTransform.Position.x, ObjectTransform.Position.y, ObjectTransform.Position.z + 1.0f));

		CurrentAngle += (RotationScaleFactor * RotationalPeriod );
		if (CurrentAngle > 360.0f)
		{
			CurrentAngle = CurrentAngle - 360.0f;
		}

		RotateAboutAxis(DirectX::XMLoadFloat3(&LocalUpVector), CurrentAngle);

		if (Parent != nullptr)
		{
			OrbitAboutPoint(Parent->GetPosition());
		}
		else
		{
			OrbitAboutPoint(OrbitPoint);
		}
	}

	void CelestialBody::Draw(std::shared_ptr<Direct3D>& direct3DRenderer, Scene * currentScene, DirectX::XMMATRIX viewProjectionMatrix)
	{
		direct3DRenderer->GetDeviceContext()->PSSetShaderResources(0, 1, CelestialTexture->GetAddressOfShaderResourceView());
		GameObject::Draw(direct3DRenderer, currentScene, viewProjectionMatrix);
	}

	void CelestialBody::SetPlanetTexture(std::wstring filePath, ID3D11Device2* device)
	{
		CelestialTexture = std::make_shared<Texture>(filePath);
		CelestialTexture->InitializeTexture(device);
	}

	void CelestialBody::SetPlanetTexture(std::shared_ptr<Texture> texture)
	{
		CelestialTexture = texture;
	}

	void CelestialBody::SetParent(std::shared_ptr<CelestialBody>& parent)
	{
		Parent = parent;
		SetOrbitPoint(parent->GetPosition());
	}

	void CelestialBody::RotateAboutAxis(const DirectX::XMFLOAT3& axis, float angleInDegrees)
	{
		RotateAboutAxis(DirectX::XMLoadFloat3(&axis), angleInDegrees);
	}

	void CelestialBody::RotateAboutAxis(DirectX::XMVECTOR axis, float angleInDegrees)
	{
		if (DirectX::XMVector3Equal(DirectX::XMVectorZero(), axis) || angleInDegrees == 0.0f)
		{
			return;
		}

		auto rotationMatrix = DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angleInDegrees));
		
		if (ObjectTransform.Rotation.y > 180.0f)
		{
			ObjectTransform.Rotation.y = ObjectTransform.Rotation.y - 360.0f;
		}
		
		if (ObjectTransform.Rotation.y < -180.0f)
		{
			ObjectTransform.Rotation.y = ObjectTransform.Rotation.y + 360.0f;
		}

		if (ObjectTransform.Rotation.x > 180.0f)
		{
			ObjectTransform.Rotation.x = ObjectTransform.Rotation.x - 360.0f;
		}

		if (ObjectTransform.Rotation.x < -180.0f)
		{
			ObjectTransform.Rotation.x = ObjectTransform.Rotation.x + 360.0f;
		}

		if (ObjectTransform.Rotation.z > 180.0f)
		{
			ObjectTransform.Rotation.z = ObjectTransform.Rotation.z - 360.0f;
		}

		if (ObjectTransform.Rotation.z < -180.0f)
		{
			ObjectTransform.Rotation.z = ObjectTransform.Rotation.z + 360.0f;
		}

		DirectX::XMStoreFloat3(&LocalForwardVector, DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&LocalForwardVector), rotationMatrix)));
		DirectX::XMStoreFloat3(&LocalUpVector, DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&LocalUpVector), rotationMatrix)));
		DirectX::XMStoreFloat3(&LocalRightVector, DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&LocalRightVector), rotationMatrix)));

		DirectX::XMFLOAT4X4 rotation;
		DirectX::XMStoreFloat4x4(&rotation, rotationMatrix);

		float pitch = asinf(-rotation._32);
		float yaw, roll;

		if (cosf(pitch) > 0.0001f)
		{
			yaw = atan2f(rotation._31, rotation._33);
			roll = atan2f(rotation._12, rotation._22);
		}
		else
		{
			yaw = 0.0f;
			roll = atan2f(--rotation._21, rotation._11);
		}
		
		SetRotation(DirectX::XMFLOAT3(DirectX::XMConvertToDegrees(pitch), DirectX::XMConvertToDegrees(yaw), DirectX::XMConvertToDegrees(roll)));

		NeedsUpdate = true;
	}

	void CelestialBody::SetOrbitPoint(const DirectX::XMFLOAT3 & orbitPoint)
	{
		OrbitPoint = orbitPoint;
		OrbitRadius = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&ObjectTransform.Position), DirectX::XMLoadFloat3(&OrbitPoint))));
	}

	void CelestialBody::OrbitAboutPoint(const DirectX::XMFLOAT3& point)
	{
		//float radius = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&ObjectTransform.Position), DirectX::XMLoadFloat3(&point))));

		CurrentOrbitalVelocity += OrbitalVelocity * OrbitalVelocityScaleFactor;

		if (CurrentOrbitalVelocity > 360.0f)
		{
			CurrentOrbitalVelocity = CurrentOrbitalVelocity - 360.0f;
		}

		float x = point.x + (OrbitRadius * sinf(DirectX::XMConvertToRadians(CurrentOrbitalVelocity)));
		float z = point.z + (OrbitRadius * cosf(DirectX::XMConvertToRadians(CurrentOrbitalVelocity)));

		ObjectTransform.Position.x = x;
		ObjectTransform.Position.z = z;
		NeedsUpdate = true;
	}
}