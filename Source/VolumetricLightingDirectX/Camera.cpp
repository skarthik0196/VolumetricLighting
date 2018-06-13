#include "pch.h"
#include "Camera.h"
#include "Utility.h"

namespace Rendering
{
	Camera::Camera(float screenWidth, float screenHeight, float nearPlane, float farPlane) : NearPlane(nearPlane), FarPlane(farPlane), Position(Utility::Zero), Target(Utility::Forward), RightVectorEndPoint(Utility::Right), Rotation(Utility::Zero), FieldOfView(45.0f), ScreenWidth(screenWidth), ScreenHeight(screenHeight),
														CameraFrustum(std::make_shared<Frustum>())
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

	DirectX::XMVECTOR Camera::GetRotation() const
	{
		return DirectX::XMLoadFloat3(&Rotation);
	}

	const DirectX::XMFLOAT3& Camera::GetRotationAsFloat3() const
	{
		return Rotation;
	}

	DirectX::XMVECTOR Camera::GetUpVectorN() const
	{
		return DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&UpVectorEndPoint), DirectX::XMLoadFloat3(&Position)));
	}

	DirectX::XMFLOAT3 Camera::GetUpVectorAsFloat3N() const
	{
		DirectX::XMFLOAT3 temp;
		DirectX::XMStoreFloat3(&temp, GetUpVectorN());
		return temp;
	}

	DirectX::XMVECTOR Camera::GetRightVectorN() const
	{
		return DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&RightVectorEndPoint), DirectX::XMLoadFloat3(&Position)));
	}

	DirectX::XMFLOAT3 Camera::GetRightVectorAsFloat3N() const
	{
		DirectX::XMFLOAT3 temp;
		DirectX::XMStoreFloat3(&temp, GetRightVectorN());
		return temp;
	}

	DirectX::XMVECTOR Camera::GetForwardVectorN() const
	{
		return DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&Target), DirectX::XMLoadFloat3(&Position)));
	}

	DirectX::XMFLOAT3 Camera::GetForwardVectorAsFloat3N() const
	{
		DirectX::XMFLOAT3 temp;
		DirectX::XMStoreFloat3(&temp, GetForwardVectorN());
		return temp;
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
		InitializePerspectiveProjectionMatrix();
	}

	void Camera::SetFarPlane(float farPlane)
	{
		FarPlane = farPlane;
		InitializePerspectiveProjectionMatrix();
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
		DirectX::XMStoreFloat4x4(&ViewMatrix, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&Position), DirectX::XMLoadFloat3(&Target), GetUpVector()));
	}

	void Camera::InitializePerspectiveProjectionMatrix()
	{
		DirectX::XMStoreFloat4x4(&ProjectionMatrix, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FieldOfView), (ScreenWidth / ScreenHeight), NearPlane, FarPlane));

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

		temp = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&RightVectorEndPoint), DirectX::XMMatrixTranslation(DirectX::XMVectorGetX(translation), DirectX::XMVectorGetY(translation), DirectX::XMVectorGetZ(translation)));
		DirectX::XMStoreFloat3(&RightVectorEndPoint, temp);

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

		DirectX::XMStoreFloat3(&Rotation, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Rotation), DirectX::XMVectorScale(axis, angleInDegrees)));

		//if (Rotation.y > 180.0f)
		//{
		//	Rotation.y = Rotation.y - 360.0f;
		//}
		//
		//if (Rotation.y < -180.0f)
		//{
		//	Rotation.y = Rotation.y + 360.0f;
		//}

		//if (Rotation.x > 180.0f)
		//{
		//	Rotation.x = Rotation.x - 360.0f;
		//}

		//if (Rotation.x < -180.0f)
		//{
		//	Rotation.x = Rotation.x + 360.0f;
		//}

		auto position = DirectX::XMLoadFloat3(&Position);

		DirectX::XMVECTOR targetLookAt = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&Target), position);
		DirectX::XMVECTOR upVectorLookAt = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&UpVectorEndPoint), position);
		DirectX::XMVECTOR rightVectorLookAt = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&RightVectorEndPoint), position);

		auto rotationMatrix = DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angleInDegrees));

		targetLookAt = DirectX::XMVector3Transform(targetLookAt, rotationMatrix);
		upVectorLookAt = DirectX::XMVector3Transform(upVectorLookAt, rotationMatrix);
		rightVectorLookAt = DirectX::XMVector3Transform(rightVectorLookAt, rotationMatrix);

		DirectX::XMStoreFloat3(&Target, DirectX::XMVectorAdd(position, targetLookAt));
		DirectX::XMStoreFloat3(&UpVectorEndPoint, DirectX::XMVectorAdd(position, upVectorLookAt));
		DirectX::XMStoreFloat3(&RightVectorEndPoint, DirectX::XMVectorAdd(position, rightVectorLookAt));

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

	void Camera::SetRotation(const DirectX::XMVECTOR& rotation)
	{
		DirectX::XMVECTOR requiredRotation = DirectX::XMVectorSubtract(rotation, DirectX::XMLoadFloat3(&Rotation));
		Rotate(GetUpVectorAsFloat3N(), DirectX::XMVectorGetY(requiredRotation));
		Rotate(GetRightVectorAsFloat3N(), DirectX::XMVectorGetX(requiredRotation));
		Rotate(GetForwardVectorAsFloat3N(), DirectX::XMVectorGetZ(requiredRotation));
	}

	void Camera::SetRotation(const DirectX::XMFLOAT3& rotation)
	{
		DirectX::XMVECTOR requiredRotation = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&rotation), DirectX::XMLoadFloat3(&Rotation));
		Rotate(GetUpVectorAsFloat3N(), DirectX::XMVectorGetY(requiredRotation));
		Rotate(GetRightVectorAsFloat3N(), DirectX::XMVectorGetX(requiredRotation));
		Rotate(GetForwardVectorAsFloat3N(), DirectX::XMVectorGetZ(requiredRotation));
	}


	void Camera::ResetCamera()
	{
		Position = Utility::Zero;
		UpVectorEndPoint = Utility::Up;
		RightVectorEndPoint = Utility::Right;
		Target = Utility::Forward;

		Rotation = Utility::Zero;

		InitializeViewMatrix();
	}

	void Camera::UpdateFrustum()
	{
		CameraFrustum->ConstructFrustum(FarPlane, ProjectionMatrix, ViewMatrix);
	}

	std::shared_ptr<Frustum>& Camera::GetFrustum()
	{
		return CameraFrustum;
	}
}