#pragma once
#include "Texture.h"

namespace Rendering
{
	class BillboardedQuad
	{
	public:
		BillboardedQuad();
		~BillboardedQuad();

	private:
		std::shared_ptr<Texture> QuadTexture;

		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	};
}