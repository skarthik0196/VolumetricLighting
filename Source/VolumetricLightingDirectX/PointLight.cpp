#include "pch.h"
#include "PointLight.h"

namespace Rendering
{
	PointLight::PointLight(const DirectX::XMFLOAT3 position, float radius, const DirectX::XMFLOAT4 & color, float intensity) : Light(color, intensity), Position(position), Radius(radius)
	{
		UpdateWorldMatrix();
	}

	void PointLight::SetPosition(const DirectX::XMFLOAT3 & position)
	{
		Position = position;
		UpdateWorldMatrix();
	}

	void PointLight::SetRadius(const float & radius)
	{
		Radius = radius;
		UpdateWorldMatrix();
	}

	const DirectX::XMFLOAT3 & PointLight::GetPosition() const
	{
		return Position;
	}

	float PointLight::GetRadius() const
	{
		return Radius;
	}

	const DirectX::XMFLOAT4X4 & PointLight::GetWorldMatrixAsFloat4x4() const
	{
		return WorldMatrix;
	}

	DirectX::XMMATRIX PointLight::GetWorldMatrix()
	{
		return DirectX::XMLoadFloat4x4(&WorldMatrix);
	}

	void PointLight::UpdateWorldMatrix()
	{
		float scale = Radius / (1.0f * 5.0f);
		DirectX::XMStoreFloat4x4(&WorldMatrix, DirectX::XMMatrixScaling(scale, scale, scale) * DirectX::XMMatrixTranslation(Position.x, Position.y, Position.z));
	}

}