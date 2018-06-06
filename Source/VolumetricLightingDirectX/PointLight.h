#pragma once
#include "Light.h"

namespace Rendering
{
	class PointLight : public Light
	{
	public:
		PointLight(const DirectX::XMFLOAT3 position = Utility::Zero, float radius = 0.0f, const DirectX::XMFLOAT4& color = Utility::White, float intensity = 1.0f);
		virtual ~PointLight() = default;

		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetRadius(const float& radius);
		
		const DirectX::XMFLOAT3& GetPosition() const;
		float GetRadius() const;

		const DirectX::XMFLOAT4X4& GetWorldMatrixAsFloat4x4() const;
		DirectX::XMMATRIX GetWorldMatrix();

	private:
		void UpdateWorldMatrix();

		DirectX::XMFLOAT4X4 WorldMatrix;
		DirectX::XMFLOAT3 Position;
		float Radius;

	};
}