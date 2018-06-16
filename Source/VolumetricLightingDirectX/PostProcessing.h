#pragma once
#include<wrl.h>
#include"Direct3D.h"
#include"ScreenQuad.h"
#include"Shader.h"

namespace Rendering
{
	class Scene;
	class PostProcessing
	{
	public:
		PostProcessing(ID3D11Device2* device, const std::wstring& postProcessPixelShader);
		PostProcessing(ID3D11Device2* device, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring& postProcessPixelShader);

		virtual ~PostProcessing() = default;

		virtual void ApplyPostProcessing(Scene* scene, std::shared_ptr<Direct3D>& direct3DRenderer) = 0;

	protected:
		std::shared_ptr<ScreenQuad> ScreenQuadData;
		std::shared_ptr<Shader> PostProcessingPixelShader;

	};
}