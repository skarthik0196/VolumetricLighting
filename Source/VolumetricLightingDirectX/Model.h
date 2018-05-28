#pragma once

namespace Rendering
{
	class Mesh;
	class Material;

	class Model
	{
	public:
		Model(ID3D11Device2* device, const std::string& filename, bool flipUVs = false);
		Model(const Model& rhs) = default;
		Model(Model&& rhs) = default;
		virtual ~Model() = default;

		Model& operator=(const Model& rhs) = default;
		Model& operator=(Model&& rhs) = default;

		bool HasMaterials() const;
		bool HasMeshes() const;

		const std::vector<std::shared_ptr<Mesh>>& GetMeshes();
		const std::vector<std::shared_ptr<Material>>& GetMaterials();

		Material& CreateMaterial();

	private:
		void LoadModel(ID3D11Device2* device, const std::string& fileName, bool flipUVs);

		std::vector<std::shared_ptr<Mesh>> Meshes;
		std::vector<std::shared_ptr<Material>> Materials;
	};
}