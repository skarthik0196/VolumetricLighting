#include "pch.h"
#include "ScreenQuad.h"

namespace Rendering
{
	ScreenQuad::ScreenQuad(ID3D11Device2* device) : ScreenQuadVS(std::make_shared<Shader>(L"ScreenQuadVertexShader.cso", Shader::ShaderType::VertexShader, device))
	{

	}


	void ScreenQuad::BindScreenQuadVertexShader(ID3D11DeviceContext2* deviceContext)
	{
		deviceContext->VSSetShader(ScreenQuadVS->GetVertexShader(), 0, 0);
		deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetInputLayout(NULL);
	}

	void ScreenQuad::DrawScreenQuad(ID3D11DeviceContext2 * deviceContext)
	{
		deviceContext->Draw(6, 0);

	}
}