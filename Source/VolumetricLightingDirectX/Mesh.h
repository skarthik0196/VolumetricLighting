#pragma once
#include <Model.h>
#include <wrl.h>

struct aiMesh;
struct aiMaterial;

namespace Rendering
{
	class Material;
	class Model;

	struct Vertex
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 TextureCoordinates;
		DirectX::XMFLOAT3 Normals;

		Vertex(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& textureCoordinates, const DirectX::XMFLOAT3& normals);
	};

	class Mesh
	{
	public:

		friend class Model;

		Mesh(Model& model, aiMesh& mesh, uint32_t materialIndex, ID3D11Device2* D3Ddevice);
		virtual ~Mesh() = default;

		Model& GetModel();

		const std::vector<DirectX::XMFLOAT3>& GetVertices() const;
		const std::vector<DirectX::XMFLOAT3>& GetNormals() const;
		const std::vector<DirectX::XMFLOAT3>& GetTangents() const;
		const std::vector<DirectX::XMFLOAT3>& GetBiNormals() const;

		const std::vector<std::shared_ptr<std::vector<DirectX::XMFLOAT3>>>& GetTextureCoordinates() const;
		const std::vector<std::shared_ptr<std::vector<DirectX::XMFLOAT4>>>& GetVertexColors() const;

		uint32_t GetFaceCount() const;
		uint32_t GetIndexCount() const;

		const std::vector<uint32_t>& GetIndices() const;

		void CreateIndexBuffer(ID3D11Device2* device);
		void CreateVertexBuffer(ID3D11Device2* device);

		ID3D11Buffer* GetIndexBuffer();
		ID3D11Buffer* GetVertexBuffer();

		ID3D11Buffer** GetAddressOfVertexBuffer();
		ID3D11Buffer** GetAddressOfIndexBuffer();

		std::shared_ptr<Material> GetMaterial() const;



	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;

		Model& ReferenceToModel;

		std::string Name;
		std::vector<DirectX::XMFLOAT3> Vertices;
		std::vector<DirectX::XMFLOAT3> Normals;
		std::vector<DirectX::XMFLOAT3> Tangents;
		std::vector<DirectX::XMFLOAT3> BiNormals;
		std::vector<std::shared_ptr<std::vector<DirectX::XMFLOAT3>>> TextureCoordinates;
		std::vector<std::shared_ptr<std::vector<DirectX::XMFLOAT4>>> VertexColors;
		uint32_t FaceCount;
		std::vector<uint32_t> Indices;

		std::uint32_t MaterialIndex;
	};
}