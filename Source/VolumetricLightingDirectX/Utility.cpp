#include "pch.h"
#include "Utility.h"

namespace Rendering
{
	DirectX::XMFLOAT3 Utility::Forward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 Utility::Right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 Utility::Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	DirectX::XMFLOAT3 Utility::Zero = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	DirectX::XMFLOAT4 Utility::White = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 Utility::Red = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 Utility::Black = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMFLOAT4X4 Utility::IdentityMatrix = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	DirectX::XMFLOAT4 Utility::BackgroundColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}