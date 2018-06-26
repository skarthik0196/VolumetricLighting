#include "pch.h"
#include "InputManager.h"

namespace Rendering
{
	InputManager::InputManager(HWND windowHandle) : KeyboardComponent(std::make_shared<DirectX::Keyboard>()), MouseComponent(std::make_shared<DirectX::Mouse>())
	{
		MouseComponent->SetWindow(windowHandle);
		MouseComponent->SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
	}

	void InputManager::ProcessInput()
	{
		DirectX::Keyboard::State kb = KeyboardComponent->GetState();

		if (kb.Escape)
		{
			PostQuitMessage(0);
		}

		DirectX::Mouse::State mouse = MouseComponent->GetState();

		if (kb.Up || kb.W)
		{
			KeyPressMap[InputActions::Forward] = true;
		}
		else
		{
			KeyPressMap[InputActions::Forward] = false;
		}

		if (kb.Down || kb.S)
		{
			KeyPressMap[InputActions::Backward] = true;
		}
		else
		{
			KeyPressMap[InputActions::Backward] = false;
		}

		if (kb.Right || kb.D)
		{
			KeyPressMap[InputActions::StrafeRight] = true;
		}
		else
		{
			KeyPressMap[InputActions::StrafeRight] = false;
		}

		if (kb.Left || kb.A)
		{
			KeyPressMap[InputActions::StrafeLeft] = true;
		}
		else
		{
			KeyPressMap[InputActions::StrafeLeft] = false;
		}

		if (kb.E)
		{
			KeyPressMap[InputActions::Up] = true;
		}
		else
		{
			KeyPressMap[InputActions::Up] = false;
		}

		if (kb.Q)
		{
			KeyPressMap[InputActions::Down] = true;
		}
		else
		{
			KeyPressMap[InputActions::Down] = false;
		}

		if (kb.R)
		{
			KeyPressMap[InputActions::Reset] = true;
		}
		else
		{
			KeyPressMap[InputActions::Reset] = false;
		}

		if (kb.Z)
		{
			KeyPressMap[InputActions::IncreaseAttribute] = true;
		}
		else
		{
			KeyPressMap[InputActions::IncreaseAttribute] = false;
		}

		if (kb.C)
		{
			KeyPressMap[InputActions::DecreaseAttribute] = true;
		}
		else
		{
			KeyPressMap[InputActions::DecreaseAttribute] = false;
		}

		if (kb.I)
		{
			KeyPressMap[InputActions::RotateDirectionalLightRightIncrease] = true;
		}
		else
		{
			KeyPressMap[InputActions::RotateDirectionalLightRightIncrease] = false;
		}

		if (kb.O)
		{
			KeyPressMap[InputActions::RotateDirectionalLightRightDecrease] = true;
		}
		else
		{
			KeyPressMap[InputActions::RotateDirectionalLightRightDecrease] = false;
		}

		if (kb.K)
		{
			KeyPressMap[InputActions::RotateDirectionalLightUpIncrease] = true;
		}
		else
		{
			KeyPressMap[InputActions::RotateDirectionalLightUpIncrease] = false;
		}

		if (kb.L)
		{
			KeyPressMap[InputActions::RotateDirectionalLightUpDecrease] = true;
		}
		else
		{
			KeyPressMap[InputActions::RotateDirectionalLightUpDecrease] = false;
		}

		if (kb.D1)
		{
			KeyPressMap[InputActions::ToggleFXAA] = true;
		}
		else
		{
			KeyPressMap[InputActions::ToggleFXAA] = false;
		}

		if (kb.D2)
		{
			KeyPressMap[InputActions::ToggleHDR] = true;
		}
		else
		{
			KeyPressMap[InputActions::ToggleHDR] = false;
		}

		if (kb.D3)
		{
			KeyPressMap[InputActions::ToggleGammaCorrection] = true;
		}
		else
		{
			KeyPressMap[InputActions::ToggleGammaCorrection] = false;
		}

		if (kb.G)
		{
			KeyPressMap[InputActions::IncreaseExposure] = true;
		}
		else
		{
			KeyPressMap[InputActions::IncreaseExposure] = false;
		}

		if (kb.F)
		{
			KeyPressMap[InputActions::DecreaseExposure] = true;
		}
		else
		{
			KeyPressMap[InputActions::DecreaseExposure] = false;
		}

		MouseInput = DirectX::XMFLOAT3(static_cast<float>(mouse.y), static_cast<float>(mouse.x), 0.0f);

		/*if (mouse.leftButton)
		{
			MouseInput.y = 0;
		}
		else
		{
			MouseInput.x = 0;
		}*/

	}

	const std::map<InputManager::InputActions, bool>& InputManager::GetInput()
	{
		return KeyPressMap;
	}

	const DirectX::XMFLOAT3 & InputManager::GetMouseInput()
	{
		return MouseInput;
	}
}