#pragma once
#include "Utility.h"

namespace Rendering
{
	class Light
	{
	public:
		Light(const DirectX::XMFLOAT4& color = Utility::White, float intensity = 1.0f);
		virtual ~Light() = default;

		void SetColor(const DirectX::XMFLOAT4& color);
		void SetIntensity(float intensity);

		const DirectX::XMFLOAT4& GetColor() const;
		float GetIntensity() const;

	private:

		DirectX::XMFLOAT4 Color;
		float Intensity;
	};
}