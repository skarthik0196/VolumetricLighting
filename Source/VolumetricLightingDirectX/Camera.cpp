#include "pch.h"
#include "Camera.h"
#include "Utility.h"

namespace Rendering
{
	Camera::Camera(float nearPlane, float farPlane) : NearPlane(nearPlane), FarPlane(farPlane), Position(Utility::Zero), Target(Utility::Forward), FieldOfView(1.0f), ScreenWidth(1024), ScreenHeight(768)
	{
		DirectX::XMFLOAT3 temp(Utility::Up);
		DirectX::XMVECTOR upVector = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Position), DirectX::XMLoadFloat3(&temp));
		DirectX::XMStoreFloat3(&UpVectorEndPoint, upVector);

		InitializeViewMatrix();
		InitializePerspectiveProjectionMatrix();
	}

	Camera::~Camera()
	{
	}

	DirectX::XMVECTOR Camera::GetUpVector() const
	{
		return DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&UpVectorEndPoint), DirectX::XMLoadFloat3(&Position));
	}

	DirectX::XMFLOAT3 Camera::GetUpVectorAsFloat3() const
	{
		DirectX::XMFLOAT3 temp;
		DirectX::XMStoreFloat3(&temp, GetUpVector());
		return temp;
	}

	DirectX::XMVECTOR Camera::GetPosition() const
	{
		return DirectX::XMLoadFloat3(&Position);
	}

	const DirectX::XMFLOAT3& Camera::GetPositionAsFloat3() const
	{
		return Position;
	}

	void Camera::SetAngle(float angle)
	{
		FieldOfView = angle;
	}

	float Camera::GetAngle() const
	{
		return FieldOfView;
	}

	float Camera::GetScreenWidth()
	{
		return ScreenWidth;
	}

	void Camera::SetScreenWidth(float screenWidth)
	{
		ScreenWidth = screenWidth;
	}

	float Camera::GetScreenHeight()
	{
		return ScreenHeight;
	}

	void Camera::SetScreenHeight(float screenHeight)
	{
		ScreenHeight = screenHeight;
	}

	void Camera::SetNearPlane(float nearPlane)
	{
		NearPlane = nearPlane;
	}

	void Camera::SetFarPlane(float farPlane)
	{
		FarPlane = farPlane;
	}

	float Camera::GetNearPlane() const
	{
		return NearPlane;
	}

	float Camera::GetFarPlane() const
	{
		return FarPlane;
	}

	const DirectX::XMFLOAT4X4 & Camera::GetViewMatrixAsFloat4x4() const
	{
		return ViewMatrix;
	}

	DirectX::XMMATRIX Camera::GetViewMatrix() const
	{
		return DirectX::XMLoadFloat4x4(&ViewMatrix);
	}

	const DirectX::XMFLOAT4X4 & Camera::GetProjectionMatrixAsFloat4x4() const
	{
		return ProjectionMatrix;
	}

	DirectX::XMMATRIX Camera::GetProjectionMatrix() const
	{

		return DirectX::XMLoadFloat4x4(&ProjectionMatrix);
	}

	const DirectX::XMFLOAT4X4 & Camera::GetViewProjectionMatrixAsFloat4x4()
	{
		DirectX::XMStoreFloat4x4(&ViewProjectionMatrix, DirectX::XMLoadFloat4x4(&ViewMatrix) * DirectX::XMLoadFloat4x4(&ProjectionMatrix));
		return ViewProjectionMatrix;
	}

	DirectX::XMMATRIX Camera::GetViewProjectionMatrix()
	{
		DirectX::XMStoreFloat4x4(&ViewProjectionMatrix, DirectX::XMLoadFloat4x4(&ViewMatrix) * DirectX::XMLoadFloat4x4(&ProjectionMatrix));
		return DirectX::XMLoadFloat4x4(&ViewProjectionMatrix);
	}

	void Camera::InitializeViewMatrix()
	{
		DirectX::XMStoreFloat4x4(&ViewMatrix, DirectX::XMMatrixLookAtRH(DirectX::XMLoadFloat3(&Position), DirectX::XMLoadFloat3(&Target), GetUpVector()));
	}

	void Camera::InitializePerspectiveProjectionMatrix()
	{
		DirectX::XMStoreFloat4x4(&ProjectionMatrix, DirectX::XMMatrixPerspectiveFovRH(FieldOfView, (ScreenWidth / ScreenHeight), 0.01f, FarPlane));

		//DirectX::XMStoreFloat4x4(&ViewProjectionMatrix, DirectX::XMLoadFloat4x4(&ViewMatrix) * DirectX::XMLoadFloat4x4(&ProjectionMatrix));
	}

	void Camera::Move(const DirectX::XMFLOAT3& translation)
	{
		DirectX::XMVECTOR temp = DirectX::XMLoadFloat3(&translation);
		Move(temp);
	}

	void Camera::Move(const DirectX::XMVECTOR& translation)
	{
		DirectX::XMVECTOR temp = DirectX::XMLoadFloat3(&Position);
		temp = DirectX::XMVector3Transform(temp, DirectX::XMMatrixTranslation(DirectX::XMVectorGetX(translation), DirectX::XMVectorGetY(translation), DirectX::XMVectorGetZ(translation)));
		DirectX::XMStoreFloat3(&Position, temp);

		temp = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&Target), DirectX::XMMatrixTranslation(DirectX::XMVectorGetX(translation), DirectX::XMVectorGetY(translation), DirectX::XMVectorGetZ(translation)));
		DirectX::XMStoreFloat3(&Target, temp);

		temp = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&UpVectorEndPoint), DirectX::XMMatrixTranslation(DirectX::XMVectorGetX(translation), DirectX::XMVectorGetY(translation), DirectX::XMVectorGetZ(translation)));
		DirectX::XMStoreFloat3(&UpVectorEndPoint, temp);

		InitializeViewMatrix();
	}

	void Camera::Rotate(const DirectX::XMFLOAT3& axis, float angleInDegrees)
	{
		DirectX::XMVECTOR temp = DirectX::XMLoadFloat3(&axis);
		Rotate(temp, angleInDegrees);
	}

	void Camera::Rotate(const DirectX::XMVECTOR& axis, float angleInDegrees)
	{
		if ((DirectX::XMVector3Equal(DirectX::XMVectorZero(), axis)) || (angleInDegrees == 0.0f))
		{
			return;
		}

		DirectX::XMVECTOR targetLookAt = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&Target), DirectX::XMLoadFloat3(&Position));
		DirectX::XMVECTOR upVectorLookAt = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&UpVectorEndPoint), DirectX::XMLoadFloat3(&Position));

		targetLookAt = DirectX::XMVector3Transform(targetLookAt, DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angleInDegrees)));
		upVectorLookAt = DirectX::XMVector3Transform(upVectorLookAt, DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angleInDegrees)));

		DirectX::XMStoreFloat3(&Target, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Position), targetLookAt));
		DirectX::XMStoreFloat3(&UpVectorEndPoint, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Position), upVectorLookAt));

		InitializeViewMatrix();
	}

	void Camera::SetPosition(const DirectX::XMFLOAT3& position)
	{
		DirectX::XMVECTOR temp = DirectX::XMLoadFloat3(&position);
		SetPosition(temp);
	}

	void Camera::SetPosition(const DirectX::XMVECTOR& position)
	{
		DirectX::XMVECTOR moveVector = DirectX::XMVectorSubtract(position, DirectX::XMLoadFloat3(&Position));
		Move(moveVector);
	}
}