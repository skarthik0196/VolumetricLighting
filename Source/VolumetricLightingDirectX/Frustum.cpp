#include "pch.h"
#include "Frustum.h"

namespace Rendering
{
	Frustum::Frustum()
	{
	}


	Frustum::~Frustum()
	{
	}

	void Frustum::ConstructFrustum(float farPlane, const DirectX::XMFLOAT4X4& projectionMatrix, const DirectX::XMFLOAT4X4& viewMatrix)
	{
		auto localProjectionMatrix = projectionMatrix;

		float zMinimum = -localProjectionMatrix._43 / localProjectionMatrix._33;
		float r = farPlane / (farPlane - zMinimum);

		localProjectionMatrix._33 = r;
		localProjectionMatrix._43 = -r * zMinimum;

		DirectX::XMFLOAT4X4 frustumMatrix;

		DirectX::XMStoreFloat4x4(&frustumMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&viewMatrix), DirectX::XMLoadFloat4x4(&localProjectionMatrix)));

		//Near Plane

		FrustumPlanes[0].x = frustumMatrix._14 + frustumMatrix._13;
		FrustumPlanes[0].y = frustumMatrix._24 + frustumMatrix._23;
		FrustumPlanes[0].z = frustumMatrix._34 + frustumMatrix._33;
		FrustumPlanes[0].w = frustumMatrix._44 + frustumMatrix._43;

		DirectX::XMStoreFloat4(&FrustumPlanes[0], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&FrustumPlanes[0])));

		//Far Plane

		FrustumPlanes[1].x = frustumMatrix._14 - frustumMatrix._13;
		FrustumPlanes[1].y = frustumMatrix._24 - frustumMatrix._23;
		FrustumPlanes[1].z = frustumMatrix._34 - frustumMatrix._33;
		FrustumPlanes[1].w = frustumMatrix._44 - frustumMatrix._43;

		DirectX::XMStoreFloat4(&FrustumPlanes[1], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&FrustumPlanes[1])));

		//Left Plane

		FrustumPlanes[2].x = frustumMatrix._14 + frustumMatrix._11;
		FrustumPlanes[2].y = frustumMatrix._24 + frustumMatrix._21;
		FrustumPlanes[2].z = frustumMatrix._34 + frustumMatrix._31;
		FrustumPlanes[2].w = frustumMatrix._44 + frustumMatrix._41;

		DirectX::XMStoreFloat4(&FrustumPlanes[2], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&FrustumPlanes[2])));

		//Right Plane

		FrustumPlanes[3].x = frustumMatrix._14 - frustumMatrix._11;
		FrustumPlanes[3].y = frustumMatrix._24 - frustumMatrix._21;
		FrustumPlanes[3].z = frustumMatrix._34 - frustumMatrix._31;
		FrustumPlanes[3].w = frustumMatrix._44 - frustumMatrix._41;

		DirectX::XMStoreFloat4(&FrustumPlanes[3], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&FrustumPlanes[3])));

		//Top Plane

		FrustumPlanes[4].x = frustumMatrix._14 - frustumMatrix._12;
		FrustumPlanes[4].y = frustumMatrix._24 - frustumMatrix._22;
		FrustumPlanes[4].z = frustumMatrix._34 - frustumMatrix._32;
		FrustumPlanes[4].w = frustumMatrix._44 - frustumMatrix._42;

		DirectX::XMStoreFloat4(&FrustumPlanes[4], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&FrustumPlanes[4])));

		//Bottom Plane

		FrustumPlanes[5].x = frustumMatrix._14 + frustumMatrix._12;
		FrustumPlanes[5].y = frustumMatrix._24 + frustumMatrix._22;
		FrustumPlanes[5].z = frustumMatrix._34 + frustumMatrix._32;
		FrustumPlanes[5].w = frustumMatrix._44 + frustumMatrix._42;

		DirectX::XMStoreFloat4(&FrustumPlanes[5], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&FrustumPlanes[5])));
	}

	bool Frustum::CheckSphere(const DirectX::XMFLOAT3 & sphereCenter, const float sphereRadius)
	{
		for (int i = 0; i < 6; ++i)
		{
			if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(DirectX::XMLoadFloat4(&FrustumPlanes[i]), DirectX::XMLoadFloat3(&sphereCenter))) < -sphereRadius)
			{
				return false;
			}
		}

		return true;
	}
}