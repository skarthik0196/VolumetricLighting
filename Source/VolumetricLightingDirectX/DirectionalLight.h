#pragma once
#include "Light.h"

namespace Rendering
{
	class DirectionalLight : public Light
	{

	public:
		DirectionalLight(const DirectX::XMFLOAT4& color = Utility::White, float intensity = 1.0f);
		virtual ~DirectionalLight() = default;

		DirectX::XMVECTOR GetDirection() const;
		const DirectX::XMFLOAT3& GetDirectionAsFloat3() const;

		DirectX::XMVECTOR GetUpVector() const;
		const DirectX::XMFLOAT3& GetUpVectorAsFloat3() const;

		DirectX::XMVECTOR GetRightVector() const;
		const DirectX::XMFLOAT3& GetRightVectorAsFloat3() const;

		void ApplyRotation(const DirectX::XMFLOAT3& axis, float angle);

		DirectX::XMVECTOR GetDirectionToLight() const;
		DirectX::XMFLOAT3 GetDirectionToLightAsFloat3() const;

	private:
		DirectX::XMFLOAT3 Direction;
		DirectX::XMFLOAT3 UpVector;
		DirectX::XMFLOAT3 RightVector;
	};
}