#pragma once

namespace Rendering
{
	class Frustum
	{
	public:
		Frustum();
		~Frustum();

		void ConstructFrustum(float farPlane, const DirectX::XMFLOAT4X4& projectionMatrix, const DirectX::XMFLOAT4X4& viewMatrix);

		bool CheckSphere(const DirectX::XMFLOAT3& sphereCenter, const float sphereRadius);

	private:
		DirectX::XMFLOAT4 FrustumPlanes[6];
	};
}