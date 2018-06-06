#pragma once
#include "Shader.h"

namespace Rendering
{
	class ScreenQuad
	{

	public:
		ScreenQuad(ID3D11Device2* device);
		~ScreenQuad() = default;

		void BindScreenQuadVertexShader(ID3D11DeviceContext2* deviceContext);
		void DrawScreenQuad(ID3D11DeviceContext2* deviceContext);


	private:

		std::shared_ptr<Shader> ScreenQuadVS;
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
				
	};

}