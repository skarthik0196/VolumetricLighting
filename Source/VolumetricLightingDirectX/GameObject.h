#pragma once
#include "Model.h"
#include "Utility.h"

namespace Rendering
{
	class Direct3D;
	class Scene;

	struct Transform
	{
		DirectX::XMFLOAT3 Position{ Utility::Zero };
		DirectX::XMFLOAT3 Rotation{ Utility::Zero };
		DirectX::XMFLOAT3 Scale{ DirectX::XMFLOAT3(1.0f,1.0f,1.0f) };

		Transform() = default;
	};

	struct CBufferPerObject
	{
		DirectX::XMFLOAT4X4 WorldMatrix;
		DirectX::XMFLOAT4X4 WorldViewProjectionMatrix;
	};

	class GameObject
	{
	
	public:
		GameObject(Transform transform = Transform(), const std::string& ModelPath = "" , bool flipUVs = false, ID3D11Device2* device = nullptr);
		GameObject(Transform transform = Transform(), std::shared_ptr<Model> model = nullptr);
		virtual ~GameObject();

		const Transform& GetTransform() const;
		const DirectX::XMFLOAT3& GetPosition() const;
		const DirectX::XMFLOAT3& GetRotation() const;
		const DirectX::XMFLOAT3& GetScale() const;

		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetRotation(const DirectX::XMFLOAT3& rotation);
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetTransform(const Transform& transform);

		DirectX::XMMATRIX GetWorldMatrix();
		DirectX::XMFLOAT4X4 GetWorldMatrixAsFloat4x4();

		virtual void Update(Scene* currentScene);
		virtual void Draw(std::shared_ptr<Direct3D>& direct3DRenderer, Scene* currentScene, DirectX::XMMATRIX viewProjectionMatrix);

		std::shared_ptr<Model>& GetObjectModel();

	protected:

		void UpdateWorldMatrix();

		Transform ObjectTransform;
		std::shared_ptr<Model> ObjectModel{ nullptr };
		DirectX::XMFLOAT4X4 WorldMatrix{ Utility::IdentityMatrix };

		CBufferPerObject VSPerObjectData;

		bool NeedsUpdate;
	};

}