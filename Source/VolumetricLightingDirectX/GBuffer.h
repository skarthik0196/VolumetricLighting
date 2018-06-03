#pragma once
#include <wrl.h>

namespace Rendering
{
	class Direct3D;

	class GBuffer
	{
	public:

		enum class GBufferData
		{
			TexturedColor,
			Normal,
			WorldPosition,
			End
		};

		GBuffer(std::shared_ptr<Direct3D> direct3DRenderer);
		~GBuffer() = default;

		void Initialize(std::shared_ptr<Direct3D> direct3DRenderer);

		void SetRenderTargets(std::shared_ptr<Direct3D> direct3DRenderer);
		void ClearRenderTargets(std::shared_ptr<Direct3D> direct3DRenderer);

		ID3D11ShaderResourceView* GetShaderResourceView(GBufferData resourceType);
		ID3D11ShaderResourceView** GetAddressOfShaderResourceView(GBufferData resourceType);

		void BindGBufferData(ID3D11DeviceContext2* deviceContext);

	private:

		std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> RenderTargetTextures;
		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> RenderTargetViews;
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> ShaderResourceViews;

	};
}