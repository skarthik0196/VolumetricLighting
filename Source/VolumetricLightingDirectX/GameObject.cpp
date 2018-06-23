#include "pch.h"
#include "GameObject.h"
#include "Model.h"
#include "Direct3D.h"
#include "Scene.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

namespace Rendering
{
	GameObject::GameObject(Transform transform, const std::string& ModelPath, bool flipUVs, ID3D11Device2* device) : ObjectTransform(transform)
	{
		if (!ModelPath.empty() && device != nullptr)
		{
			ObjectModel = std::make_shared<Model>(device, ModelPath, flipUVs);
		}
	}

	GameObject::GameObject(Transform transform, std::shared_ptr<Model> model) : ObjectTransform(transform)
	{
		if (model != nullptr)
		{
			ObjectModel = model;
		}
	}

	GameObject::~GameObject()
	{
	}

	const Transform& GameObject::GetTransform() const
	{
		 return ObjectTransform;
	}

	const DirectX::XMFLOAT3& GameObject::GetPosition() const
	{
		return ObjectTransform.Position;
	}

	const DirectX::XMFLOAT3& GameObject::GetRotation() const
	{
		return ObjectTransform.Rotation;
	}

	const DirectX::XMFLOAT3& GameObject::GetScale() const
	{
		return ObjectTransform.Scale;
	}

	void GameObject::SetPosition(const DirectX::XMFLOAT3& position)
	{
		ObjectTransform.Position = position;
		NeedsUpdate = true;
	}

	void GameObject::SetRotation(const DirectX::XMFLOAT3& rotation)
	{
		ObjectTransform.Rotation = rotation;
		NeedsUpdate = true;
	}

	void GameObject::SetScale(const DirectX::XMFLOAT3& scale)
	{
		ObjectTransform.Scale = scale;
		NeedsUpdate = true;
	}

	void GameObject::SetTransform(const Transform& transform)
	{
		ObjectTransform = transform;
		NeedsUpdate = true;
	}

	DirectX::XMMATRIX GameObject::GetWorldMatrix()
	{
		if (NeedsUpdate)
		{
			UpdateWorldMatrix();
		}
		return DirectX::XMLoadFloat4x4(&WorldMatrix);
	}

	DirectX::XMFLOAT4X4 GameObject::GetWorldMatrixAsFloat4x4()
	{
		if (NeedsUpdate)
		{
			UpdateWorldMatrix();
		}
		return WorldMatrix;
	}

	void GameObject::Update(Scene* currentScene)
	{
		UNREFERENCED_PARAMETER(currentScene);
	}

	void GameObject::Draw(std::shared_ptr<Direct3D>& direct3DRenderer, Scene* currentScene, DirectX::XMMATRIX viewProjectionMatrix)
	{
		UNREFERENCED_PARAMETER(direct3DRenderer);
		UNREFERENCED_PARAMETER(currentScene);

		int unculledMeshes = 0;

		auto& frustum = currentScene->GetCamera()->GetFrustum();

		ID3D11DeviceContext2* deviceContext = direct3DRenderer->GetDeviceContext();

		DirectX::XMMATRIX worldMatrix = GetWorldMatrix();
		DirectX::XMMATRIX WVP = worldMatrix * viewProjectionMatrix;

		WVP = DirectX::XMMatrixTranspose(WVP);
		worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);

		DirectX::XMStoreFloat4x4(&VSPerObjectData.WorldMatrix, worldMatrix);
		DirectX::XMStoreFloat4x4(&VSPerObjectData.WorldViewProjectionMatrix, WVP);

		deviceContext->UpdateSubresource(currentScene->GetVSCBufferPerObject(), 0, nullptr, &VSPerObjectData, 0, 0);

		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;

		std::vector<std::shared_ptr<Mesh>> meshList = ObjectModel->GetMeshes();

		for (auto& mesh : meshList)
		{
			if (frustum->CheckSphere(mesh->GetAverageCenter(), mesh->GetBoundingSphereRadius()))
			{
				unculledMeshes++;
				deviceContext->IASetVertexBuffers(0, 1, mesh->GetAddressOfVertexBuffer(), &stride, &offset);
				deviceContext->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

				if (mesh->GetMaterial() != nullptr)
				{
					auto textures = mesh->GetMaterial()->GetTextures();
					if (textures[Texture::TextureType::Diffuse].size() > 0)
					{
						deviceContext->PSSetShaderResources(0, 1, textures[Texture::TextureType::Diffuse][0]->GetAddressOfShaderResourceView());
					}
					if (textures[Texture::TextureType::NormalMap].size() > 0)
					{
						int a;
						a = 10;
					}
				}
				deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
			}
		}		
	}

	std::shared_ptr<Model>& GameObject::GetObjectModel()
	{
		return ObjectModel;
	}

	void GameObject::UpdateWorldMatrix()
	{
		DirectX::XMMATRIX worldMatrix;
		worldMatrix = DirectX::XMLoadFloat4x4(&Utility::IdentityMatrix) * DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&ObjectTransform.Scale))
					* DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(ObjectTransform.Rotation.x), DirectX::XMConvertToRadians(ObjectTransform.Rotation.y), DirectX::XMConvertToRadians(ObjectTransform.Rotation.z))
					* DirectX::XMMatrixTranslation(ObjectTransform.Position.x, ObjectTransform.Position.y, ObjectTransform.Position.z);

		DirectX::XMStoreFloat4x4(&WorldMatrix, worldMatrix);

		NeedsUpdate = false;
	}
}