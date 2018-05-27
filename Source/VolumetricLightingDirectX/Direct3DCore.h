#pragma once
#include <Windows.h>
#include <wrl.h>

LRESULT CALLBACK WindowProc(HWND windHandle, UINT message, WPARAM wParam, LPARAM lParam);

namespace Rendering
{
	class Direct3DCore
	{
	public:
		Direct3DCore();
		Direct3DCore(float screenWidth, float screenHeight);

		~Direct3DCore();

		void InitializeWindow(HINSTANCE currentInstance, const std::wstring &className, const std::wstring windowTitle, int showCommand);
		void InitializeD3DCore();

		HWND GetWindowHandle();

		void SetScreenResolution(float screenWidth, float screenHeight);
		void SetRefreshRate(uint32_t refreshRate);
		void SetMultiSamplingCount(uint32_t multiSamplingCount);
		void SetBufferCount(uint32_t bufferCount);

		float GetScreenWidth();
		float GetScreenHeight();

		uint32_t GetRefreshRate() const;
		uint32_t GetMultiSamplingCount() const;
		uint32_t GetMultiSamplingQualityLevels() const;
		uint32_t GetBufferCount() const;
		
		ID3D11Device2* GetDevice() const;
		ID3D11DeviceContext2* GetDeviceContext() const;
		IDXGISwapChain2* GetSwapChain() const;

	protected:
		float ScreenWidth;
		float ScreenHeight;

		Microsoft::WRL::ComPtr<ID3D11Device2> Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext2> DeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain2> SwapChain;

	private:
		bool IsWindowed;

		uint32_t RefreshRate;
		uint32_t MultiSamplingCount;
		uint32_t MultiSamplingQualityLevels;
		uint32_t BufferCount;

		HWND WindowHandle;
		WNDCLASSEX WindowClass;

		D3D_FEATURE_LEVEL FeatureLevel;
	};
}