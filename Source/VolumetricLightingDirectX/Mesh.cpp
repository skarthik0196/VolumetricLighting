#include "pch.h"
#include "Mesh.h"
#include <assimp\scene.h>
#include "Model.h"
#include "Material.h"

namespace Rendering
{
	Mesh::Mesh(Model& model, aiMesh& mesh, uint32_t materialIndex, ID3D11Device2* D3Ddevice) : ReferenceToModel(model), FaceCount(0), Name(mesh.mName.C_Str()), MaterialIndex(materialIndex)
	{
		Vertices.reserve(mesh.mNumVertices);
		for (uint32_t i = 0; i < mesh.mNumVertices; ++i)
		{
			Vertices.push_back(DirectX::XMFLOAT3(reinterpret_cast<float*>(&mesh.mVertices[i])));
		}

		if (mesh.HasNormals())
		{
			Normals.reserve(mesh.mNumVertices);
			for (uint32_t i = 0; i < mesh.mNumVertices; ++i)
			{
				Normals.push_back(DirectX::XMFLOAT3(reinterpret_cast<float*>(&mesh.mNormals[i])));
			}
		}

		if (mesh.HasTangentsAndBitangents())
		{
			Tangents.reserve(mesh.mNumVertices);
			BiNormals.reserve(mesh.mNumVertices);

			for (uint32_t i = 0; i < mesh.mNumVertices; ++i)
			{
				Tangents.push_back(DirectX::XMFLOAT3(reinterpret_cast<float*>(&mesh.mNormals[i])));
				BiNormals.push_back(DirectX::XMFLOAT3(reinterpret_cast<float*>(&mesh.mNormals[i])));
			}
		}

		uint32_t UVChannelCount = mesh.GetNumUVChannels();
		for (uint32_t i = 0; i < UVChannelCount; ++i)
		{
			std::shared_ptr<std::vector<DirectX::XMFLOAT3>> textureCoordinates = std::make_shared<std::vector<DirectX::XMFLOAT3>>();
			textureCoordinates->reserve(mesh.mNumVertices);
			TextureCoordinates.push_back(textureCoordinates);

			aiVector3D* aiTextureCoordinates = mesh.mTextureCoords[i];

			for (uint32_t j = 0; j < mesh.mNumVertices; ++j)
			{
				textureCoordinates->push_back(DirectX::XMFLOAT3(reinterpret_cast<float*>(&aiTextureCoordinates[j])));
			}
		}

		uint32_t ColorChannelCount = mesh.GetNumColorChannels();
		for (uint32_t i = 0; i < ColorChannelCount; ++i)
		{
			std::shared_ptr<std::vector<DirectX::XMFLOAT4>> vertexColors = std::make_shared<std::vector<DirectX::XMFLOAT4>>();
			vertexColors->reserve(mesh.mNumVertices);
			VertexColors.push_back(vertexColors);

			aiColor4D* aiVertexColors = mesh.mColors[i];

			for (uint32_t j = 0; j < mesh.mNumVertices; ++j)
			{
				vertexColors->push_back(DirectX::XMFLOAT4(reinterpret_cast<float*>(&aiVertexColors[j])));
			}
		}

		if (mesh.HasFaces())
		{
			FaceCount = mesh.mNumFaces;
			for (uint32_t i = 0; i < FaceCount; ++i)
			{
				aiFace *face = &mesh.mFaces[i];

				for (uint32_t j = 0; j < face->mNumIndices; ++j)
				{
					Indices.push_back(face->mIndices[j]);
				}
			}
		}

		CreateVertexBuffer(D3Ddevice);
		CreateIndexBuffer(D3Ddevice);
	}

	Model& Mesh::GetModel()
	{
		return ReferenceToModel;
	}

	const std::vector<DirectX::XMFLOAT3>& Mesh::GetVertices() const
	{
		return Vertices;
	}

	const std::vector<DirectX::XMFLOAT3>& Mesh::GetNormals() const
	{
		return Normals;
	}

	const std::vector<DirectX::XMFLOAT3>& Mesh::GetTangents() const
	{
		return Tangents;
	}

	const std::vector<DirectX::XMFLOAT3>& Mesh::GetBiNormals() const
	{
		return BiNormals;
	}

	const std::vector<std::shared_ptr<std::vector<DirectX::XMFLOAT3>>>& Mesh::GetTextureCoordinates() const
	{
		return TextureCoordinates;
	}

	const std::vector<std::shared_ptr<std::vector<DirectX::XMFLOAT4>>>& Mesh::GetVertexColors() const
	{
		return VertexColors;
	}

	uint32_t Mesh::GetFaceCount() const
	{
		return FaceCount;
	}

	uint32_t Mesh::GetIndexCount() const
	{
		return static_cast<uint32_t>(Indices.size());
	}

	const std::vector<uint32_t>& Mesh::GetIndices() const
	{
		return Indices;
	}

	void Mesh::CreateIndexBuffer(ID3D11Device2 *device)
	{

		D3D11_BUFFER_DESC IndexBufferDescription;
		ZeroMemory(&IndexBufferDescription, sizeof(IndexBufferDescription));

		IndexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		IndexBufferDescription.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(Indices.size());
		IndexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA IndexSubResource;
		ZeroMemory(&IndexSubResource, sizeof(IndexSubResource));
		IndexSubResource.pSysMem = &Indices[0];

		device->CreateBuffer(&IndexBufferDescription, &IndexSubResource, IndexBuffer.GetAddressOf());
	}

	void Mesh::CreateVertexBuffer(ID3D11Device2* device)
	{
		std::vector<Vertex> vertices;
		std::vector<DirectX::XMFLOAT3>& textureCoordinates = *(TextureCoordinates.at(0));

		for (uint32_t i = 0; i < Vertices.size(); ++i)
		{
			DirectX::XMFLOAT4 position = DirectX::XMFLOAT4(Vertices[i].x, Vertices[i].y, Vertices[i].z, 1.0f);
			DirectX::XMFLOAT2 uv = DirectX::XMFLOAT2(textureCoordinates[i].x, textureCoordinates[i].y);
			vertices.emplace_back(position, uv, Normals[i]);
		}

		D3D11_BUFFER_DESC vertexBufferDescription;
		ZeroMemory(&vertexBufferDescription, sizeof(vertexBufferDescription));

		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDescription.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
		vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
		vertexSubresourceData.pSysMem = &vertices[0];

		device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, VertexBuffer.GetAddressOf());

	}

	ID3D11Buffer * Mesh::GetIndexBuffer()
	{
		return IndexBuffer.Get();
	}

	ID3D11Buffer * Mesh::GetVertexBuffer()
	{
		return VertexBuffer.Get();
	}

	ID3D11Buffer ** Mesh::GetAddressOfVertexBuffer()
	{
		return VertexBuffer.GetAddressOf();
	}

	ID3D11Buffer ** Mesh::GetAddressOfIndexBuffer()
	{
		return IndexBuffer.GetAddressOf();
	}

	std::shared_ptr<Material> Mesh::GetMaterial() const
	{
		return ReferenceToModel.GetMaterials()[MaterialIndex];
	}

	Vertex::Vertex(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& textureCoordinates, const DirectX::XMFLOAT3& normals) :Position(position), TextureCoordinates(textureCoordinates), Normals(normals)
	{

	}
}