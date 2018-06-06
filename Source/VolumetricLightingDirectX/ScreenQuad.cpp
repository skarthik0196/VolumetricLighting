#include "pch.h"
#include "ScreenQuad.h"
#include "Mesh.h"

namespace Rendering
{
	ScreenQuad::ScreenQuad(ID3D11Device2* device) : ScreenQuadVS(std::make_shared<Shader>(L"ScreenQuadVertexShader.cso", Shader::ShaderType::VertexShader, device))
	{
		Vertex vertices[] =
		{
			Vertex(DirectX::XMFLOAT4(-1.0f,-1.0f,0.0f,1.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f,0.0f,-1.0f)),
			Vertex(DirectX::XMFLOAT4(-1.0f,1.0f,0.0f,1.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f,0.0f,-1.0f)),
			Vertex(DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f,0.0f,-1.0f)),

			Vertex(DirectX::XMFLOAT4(-1.0f,-1.0f,0.0f,1.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f,0.0f,-1.0f)),
			Vertex(DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f,0.0f,-1.0f)),
			Vertex(DirectX::XMFLOAT4(1.0f,-1.0f,0.0f,1.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f,0.0f,-1.0f)),
		};

		D3D11_BUFFER_DESC vertexBufferDescription;
		ZeroMemory(&vertexBufferDescription, sizeof(vertexBufferDescription));

		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDescription.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
		vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
		vertexSubresourceData.pSysMem = &vertices[0];

		device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, VertexBuffer.GetAddressOf());
	}
	void ScreenQuad::BindScreenQuadVertexShader(ID3D11DeviceContext2* deviceContext)
	{
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->VSSetShader(ScreenQuadVS->GetVertexShader(), 0, 0);

		deviceContext->IASetInputLayout(ScreenQuadVS->GetInputLayout());		//Change to NULL if creating Quad in VS
		deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);

	}

	void ScreenQuad::DrawScreenQuad(ID3D11DeviceContext2 * deviceContext)
	{
		deviceContext->Draw(6, 0);

	}
}