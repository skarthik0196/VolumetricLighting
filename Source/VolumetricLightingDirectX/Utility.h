#pragma once

namespace Rendering
{
	class Utility
	{
	public:
		Utility() = delete;
		~Utility() = default;

	public:
		static DirectX::XMFLOAT3 Forward;
		static DirectX::XMFLOAT3 Right;
		static DirectX::XMFLOAT3 Up;
		static DirectX::XMFLOAT3 Zero;

		static DirectX::XMFLOAT4 White;

		static DirectX::XMFLOAT4X4 IdentityMatrix;

	};
}