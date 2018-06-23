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

		void SetSourcePosition(const DirectX::XMFLOAT3& position);
		const DirectX::XMFLOAT3& GetSourcePosition() const;
		
		void SetSourceScale(const DirectX::XMFLOAT3 scale);
		const DirectX::XMFLOAT3& GetSourceScale() const;

		const DirectX::XMFLOAT4X4& GetWorldMatrix() const;

		DirectX::XMMATRIX GetViewProjectionMatrix();

	private:
		bool viewProjectionNeedsUpdate{ true };

		void UpdateWorldMatrix();
		void UpdateViewProjectionMatrix();

		DirectX::XMFLOAT4X4 WorldMatrix;

		DirectX::XMFLOAT3 Direction;
		DirectX::XMFLOAT3 UpVector;
		DirectX::XMFLOAT3 RightVector;

		DirectX::XMFLOAT3 SourcePosition{ DirectX::XMFLOAT3(0.0f,0.0f,0.0f) };
		DirectX::XMFLOAT3 SourceScale{ DirectX::XMFLOAT3(10.0f,10.0f,10.0f) };

		DirectX::XMFLOAT3 ShadowMapPosition{ DirectX::XMFLOAT3(0.0f,1000.0f,0.0f) };

		DirectX::XMFLOAT4X4 ViewMatrix;
		DirectX::XMFLOAT4X4 ProjectionMatrix;
	};
}