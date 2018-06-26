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
			Down,
			IncreaseAttribute,
			DecreaseAttribute,
			IncreaseExposure,
			DecreaseExposure,
			RotateDirectionalLightRightIncrease,
			RotateDirectionalLightRightDecrease,
			RotateDirectionalLightUpIncrease,
			RotateDirectionalLightUpDecrease,
			ToggleFXAA,
			ToggleHDR,
			ToggleGammaCorrection,
			Reset
		};

		InputManager(HWND windowHandle);
		~InputManager() = default;

		void ProcessInput();

		const std::map<InputActions, bool>& GetInput();
		const DirectX::XMFLOAT3& GetMouseInput();

	private:
		std::shared_ptr<DirectX::Keyboard> KeyboardComponent;
		std::shared_ptr<DirectX::Mouse> MouseComponent;

		std::map<InputActions, bool> KeyPressMap;

		DirectX::XMFLOAT3 MouseInput;

		float Sensititivity{0.1f};
	};

}