#include "pch.h"
#include "PostProcessing.h"

namespace Rendering
{
	PostProcessing::PostProcessing(ID3D11Device2* device, const std::wstring& postProcessPixelShader) : ScreenQuadData(std::make_shared<ScreenQuad>(device)), PostProcessingPixelShader(std::make_shared<Shader>(postProcessPixelShader, Shader::ShaderType::PixelShader, device))
	{
	}

	PostProcessing::PostProcessing(ID3D11Device2* device, std::shared_ptr<ScreenQuad> screenQuad, const std::wstring& postProcessPixelShader) : ScreenQuadData(screenQuad), PostProcessingPixelShader(std::make_shared<Shader>(postProcessPixelShader, Shader::ShaderType::PixelShader, device))
	{
	}
}