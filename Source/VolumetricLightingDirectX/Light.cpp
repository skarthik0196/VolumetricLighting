#include "pch.h"
#include "Light.h"

namespace Rendering
{
	Light::Light(const DirectX::XMFLOAT4& color, float intensity) : BaseColor(color)
	{
		SetIntensity(intensity);
	}

	void Light::SetColor(const DirectX::XMFLOAT4 & color)
	{
		BaseColor = color;
	}

	void Light::SetIntensity(float intensity)
	{
		Intensity = intensity;
		DirectX::XMStoreFloat4(&Color, DirectX::XMVectorScale(DirectX::XMLoadFloat4(&BaseColor), intensity));
		Color.w = 1.0f;
	}

	const DirectX::XMFLOAT4& Light::GetColor() const
	{
		return Color;
	}

	const DirectX::XMFLOAT4 & Light::GetBaseColor() const
	{
		return BaseColor;
	}

	float Light::GetIntensity() const
	{
		return Intensity;
	}
}