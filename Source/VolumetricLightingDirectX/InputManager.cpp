#include "pch.h"
#include "InputManager.h"

namespace Rendering
{
	InputManager::InputManager(HWND windowHandle) : KeyboardComponent(std::make_shared<DirectX::Keyboard>()), MouseComponent(std::make_shared<DirectX::Mouse>())
	{
		MouseComponent->SetWindow(windowHandle);
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

		if (kb.Q)
		{
			KeyPressMap[InputActions::Up] = true;
		}
		else
		{
			KeyPressMap[InputActions::Up] = false;
		}

		if (kb.E)
		{
			KeyPressMap[InputActions::Down] = true;
		}
		else
		{
			KeyPressMap[InputActions::Down] = false;
		}
	}
	const std::map<InputManager::InputActions, bool>& InputManager::GetInput()
	{
		return KeyPressMap;
	}
}