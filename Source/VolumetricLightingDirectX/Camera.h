#pragma once
#include "Frustum.h"

namespace Rendering
{
	class Camera
	{
	public:
		Camera(float screenWidth = 1024.0f, float screenHeight =768.0f, float nearPlane = 1.0f, float farPlane = 10000.0f);

		Camera(const Camera& rhs) = default;
		Camera(Camera&& rhs) = default;

		Camera& operator=(const Camera& rhs) = default;
		Camera& operator=(Camera&& rhs) = default;
		~Camera();

		DirectX::XMVECTOR GetPosition() const;
		const DirectX::XMFLOAT3& GetPositionAsFloat3() const;

		DirectX::XMVECTOR GetRotation() const;
		const DirectX::XMFLOAT3& GetRotationAsFloat3() const;

		DirectX::XMVECTOR GetUpVector() const;
		DirectX::XMFLOAT3 GetUpVectorAsFloat3() const;

		DirectX::XMVECTOR GetUpVectorN() const;
		DirectX::XMFLOAT3 GetUpVectorAsFloat3N() const;

		DirectX::XMVECTOR GetRightVectorN() const;
		DirectX::XMFLOAT3 GetRightVectorAsFloat3N() const;

		DirectX::XMVECTOR GetForwardVectorN() const;
		DirectX::XMFLOAT3 GetForwardVectorAsFloat3N() const;

		void SetAngle(float angle);
		float GetAngle() const;

		float GetScreenWidth();
		void SetScreenWidth(float screenWidth);

		float GetScreenHeight();
		void SetScreenHeight(float screenHeight);

		void SetNearPlane(float nearPlane);
		void SetFarPlane(float farPlane);

		float GetNearPlane() const;
		float GetFarPlane() const;

		const DirectX::XMFLOAT4X4& GetViewMatrixAsFloat4x4() const;
		DirectX::XMMATRIX GetViewMatrix() const;

		const DirectX::XMFLOAT4X4& GetProjectionMatrixAsFloat4x4() const;
		DirectX::XMMATRIX GetProjectionMatrix() const;

		const DirectX::XMFLOAT4X4& GetViewProjectionMatrixAsFloat4x4();
		DirectX::XMMATRIX GetViewProjectionMatrix();

		void InitializeViewMatrix();
		void InitializePerspectiveProjectionMatrix();

		void Move(const DirectX::XMFLOAT3& translation);
		void Move(const DirectX::XMVECTOR& translation);

		void Rotate(const DirectX::XMFLOAT3& axis, float angleInDegrees);
		void Rotate(const DirectX::XMVECTOR& axis, float angleInDegrees);

		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetPosition(const DirectX::XMVECTOR& position);

		void SetRotation(const DirectX::XMVECTOR& rotation);
		void SetRotation(const DirectX::XMFLOAT3& rotation);

		void ResetCamera();

		void UpdateFrustum();

		std::shared_ptr<Frustum>& GetFrustum();

		const DirectX::XMFLOAT4X4& GetPartitionProjectionMatrixByIndexAsFloat4x4(uint32_t index) const;
		DirectX::XMMATRIX GetPartitionProjectionMatrixByIndex(uint32_t index) const;

		static const uint32_t& GetPartitionCount();

	private:

		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Target;
		DirectX::XMFLOAT3 RightVectorEndPoint;
		DirectX::XMFLOAT3 UpVectorEndPoint;

		DirectX::XMFLOAT3 Rotation;

		DirectX::XMFLOAT4X4 ViewMatrix;
		DirectX::XMFLOAT4X4 ProjectionMatrix;
		DirectX::XMFLOAT4X4 ViewProjectionMatrix;

		DirectX::XMFLOAT4X4 ParitionViewMatrix[3];
		DirectX::XMFLOAT4X4 PartitionProjectionMatrix[3];

		float FieldOfView;
		float NearPlane;
		float FarPlane;
		float ScreenWidth;
		float ScreenHeight;

		static const uint32_t PartitionCount = 3;
		static const float PartitionDepth[];

		std::shared_ptr<Frustum> CameraFrustum;
	};
}