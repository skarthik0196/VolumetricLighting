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

	void DirectionalLight::UpdateWorldMatrix()
	{
		DirectX::XMStoreFloat4x4(&WorldMatrix, DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(SourceScale.x, SourceScale.y, SourceScale.z), DirectX::XMMatrixTranslation(SourcePosition.x, SourcePosition.y, SourcePosition.z)));
	}
}