#pragma once
#include<Keyboard.h>
#include<Mouse.h>

namespace Rendering
{
	class InputManager
	{
	public:
		enum class InputActions
		{
			Forward,
			Backward,
			StrafeLeft,
			StrafeRight,
			Up,
			Down
		};

		InputManager(HWND windowHandle);
		~InputManager() = default;

		void ProcessInput();

		const std::map<InputActions, bool>& GetInput();

	private:
		std::shared_ptr<DirectX::Keyboard> KeyboardComponent;
		std::shared_ptr<DirectX::Mouse> MouseComponent;

		std::map<InputActions, bool> KeyPressMap;
	};

}