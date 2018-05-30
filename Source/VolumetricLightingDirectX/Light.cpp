#include "pch.h"
#include "Light.h"

namespace Rendering
{
	Light::Light(const DirectX::XMFLOAT4& color, float intensity) : Color(color)
	{
		if (intensity != 1.0f)
		{
			SetIntensity(intensity);
		}
	}

	void Light::SetColor(const DirectX::XMFLOAT4 & color)
	{
		Color = color;
	}

	void Light::SetIntensity(float intensity)
	{
		Intensity = intensity;
		DirectX::XMStoreFloat4(&Color, DirectX::XMVectorScale(DirectX::XMLoadFloat4(&Color), intensity));
		Color.w = 1.0f;
	}

	const DirectX::XMFLOAT4& Light::GetColor() const
	{
		return Color;
	}

	float Light::GetIntensity() const
	{
		return Intensity;
	}
}