#include "pch.h"
#include "DirectionalLight.h"

namespace Rendering
{
	DirectionalLight::DirectionalLight(const DirectX::XMFLOAT4& color, float intensity) : Light(color, intensity), Direction(Utility::Forward), UpVector(Utility::Up), RightVector(Utility::Right)
	{
		UpdateWorldMatrix();
	}

	DirectX::XMVECTOR DirectionalLight::GetDirection() const
	{
		return DirectX::XMLoadFloat3(&Direction);
	}

	const DirectX::XMFLOAT3& DirectionalLight::GetDirectionAsFloat3() const
	{
		return Direction;
	}

	DirectX::XMVECTOR DirectionalLight::GetUpVector() const
	{
		return DirectX::XMLoadFloat3(&UpVector);
	}

	const DirectX::XMFLOAT3 & DirectionalLight::GetUpVectorAsFloat3() const
	{
		return UpVector;
	}

	DirectX::XMVECTOR DirectionalLight::GetRightVector() const
	{
		return DirectX::XMLoadFloat3(&RightVector);
	}

	const DirectX::XMFLOAT3 & DirectionalLight::GetRightVectorAsFloat3() const
	{
		return RightVector;
	}

	void DirectionalLight::ApplyRotation(const DirectX::XMFLOAT3& axis, float angle)
	{
		auto rotationMatrix = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&axis), DirectX::XMConvertToRadians(angle));

		DirectX::XMVECTOR directionVector = DirectX::XMLoadFloat3(&Direction);
		DirectX::XMVECTOR upVector = DirectX::XMLoadFloat3(&UpVector);

		directionVector = DirectX::XMVector3TransformNormal(directionVector, rotationMatrix);
		directionVector = DirectX::XMVector3Normalize(directionVector);

		upVector = DirectX::XMVector3TransformNormal(upVector, rotationMatrix);
		upVector = DirectX::XMVector3Normalize(upVector);

		DirectX::XMStoreFloat3(&RightVector, DirectX::XMVector3Cross(directionVector, upVector));
		DirectX::XMStoreFloat3(&UpVector, upVector);
		DirectX::XMStoreFloat3(&Direction, directionVector);

		viewProjectionNeedsUpdate = true;
	}

	DirectX::XMVECTOR DirectionalLight::GetDirectionToLight() const
	{
		auto temp = GetDirectionToLightAsFloat3();
		return DirectX::XMLoadFloat3(&temp);
	}

	DirectX::XMFLOAT3 DirectionalLight::GetDirectionToLightAsFloat3() const
	{
		return DirectX::XMFLOAT3(-Direction.x, -Direction.y, Direction.z);
	}

	void DirectionalLight::SetSourcePosition(const DirectX::XMFLOAT3 & position)
	{
		SourcePosition = position;
		UpdateWorldMatrix();
	}

	const DirectX::XMFLOAT3& DirectionalLight::GetSourcePosition() const
	{
		return SourcePosition;
	}

	void DirectionalLight::SetSourceScale(const DirectX::XMFLOAT3 scale)
	{
		SourceScale = scale;
		UpdateWorldMatrix();
	}

	const DirectX::XMFLOAT3 & DirectionalLight::GetSourceScale() const
	{
		return SourceScale;
	}

	const DirectX::XMFLOAT4X4& DirectionalLight::GetWorldMatrix() const
	{
		return WorldMatrix;
	}

	DirectX::XMMATRIX DirectionalLight::GetViewProjectionMatrix()
	{
		if (viewProjectionNeedsUpdate == true)
		{
			UpdateViewProjectionMatrix();
			viewProjectionNeedsUpdate = false;
		}
		auto wvp = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&ViewMatrix), DirectX::XMLoadFloat4x4(&ProjectionMatrix));
		return wvp;
	}

	void DirectionalLight::UpdateWorldMatrix()
	{
		DirectX::XMStoreFloat4x4(&WorldMatrix, DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(SourceScale.x, SourceScale.y, SourceScale.z), DirectX::XMMatrixTranslation(SourcePosition.x, SourcePosition.y, SourcePosition.z)));
	}

	void DirectionalLight::UpdateViewProjectionMatrix()
	{
		//DirectX::XMStoreFloat4x4(&ProjectionMatrix, DirectX::XMMatrixOrthographicLH(20, 20, 1.0f, 10000.0f));
		DirectX::XMStoreFloat4x4(&ViewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMVectorZero(), GetDirection(), DirectX::XMLoadFloat3(&Utility::Up)));
		DirectX::XMStoreFloat4x4(&ProjectionMatrix, DirectX::XMMatrixOrthographicOffCenterLH(-5.0, 5.0f, -5.0f, 20.0f, 1.0f, 15.0f));

		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 50.0f, 0.0f);
		DirectX::XMFLOAT3 lookAt = DirectX::XMFLOAT3(0.0f, 50.0f, 1.0f);
		//DirectX::XMStoreFloat4x4(&ViewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&Utility::Forward), DirectX::XMLoadFloat3(&Utility::Up)));
		DirectX::XMStoreFloat4x4(&ViewMatrix, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&lookAt), DirectX::XMLoadFloat3(&Utility::Up)));
		DirectX::XMStoreFloat4x4(&ProjectionMatrix, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), (1366.0f / 768.0f), 2.0f, 100.0f));
	}
}