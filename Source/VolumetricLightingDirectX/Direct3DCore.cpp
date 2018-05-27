#include "pch.h"
#include "Direct3DCore.h"

LRESULT CALLBACK WindowProc(HWND windHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(windHandle, message, wParam, lParam);
}

namespace Rendering
{
	Direct3DCore::Direct3DCore() : ScreenWidth(1024), ScreenHeight(768), MultiSamplingCount(4), BufferCount(2), RefreshRate(60), IsWindowed(true)
	{
		
	}

	Direct3DCore::Direct3DCore(float screenWidth, float screenHeight) : ScreenWidth(screenWidth), ScreenHeight(screenHeight), MultiSamplingCount(4), BufferCount(2), RefreshRate(60), IsWindowed(true)
	{

	}

	Direct3DCore::~Direct3DCore()
	{
	}

	void Direct3DCore::InitializeWindow(HINSTANCE currentInstance, const std::wstring &className, const std::wstring windowTitle, int showCommand)
	{
		ZeroMemory(&WindowClass, sizeof(WindowClass));
		WindowClass.cbSize = sizeof(WNDCLASSEX);
		WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		WindowClass.lpfnWndProc = WindowProc;
		WindowClass.hInstance = currentInstance;
		WindowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		WindowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		WindowClass.lpszClassName = className.c_str();

		RegisterClassEx(&WindowClass);

		RECT windowRect = { 0,0,static_cast<LONG>(ScreenWidth) , static_cast<LONG>(ScreenHeight) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		WindowHandle = CreateWindow(className.c_str(), windowTitle.c_str(), WS_OVERLAPPEDWINDOW, 200, 200, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, currentInstance, nullptr);

		int errNo;
		errNo = GetLastError();

		if (WindowHandle == nullptr)
		{
			throw std::runtime_error("Unable to Create Window");
		}

		ShowWindow(WindowHandle, showCommand);
		UpdateWindow(WindowHandle);
		
	}

	void Direct3DCore::InitializeD3DCore()
	{
		D3D_FEATURE_LEVEL supportedFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };

		HRESULT result;
		ID3D11Device *device;
		ID3D11DeviceContext *deviceContext;

		DWORD createDeviceFlags = 0;

#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, supportedFeatureLevels, ARRAYSIZE(supportedFeatureLevels), D3D11_SDK_VERSION, &device, &FeatureLevel, &deviceContext);
		result = device->QueryInterface(__uuidof(ID3D11Device2), reinterpret_cast<void**>(Device.GetAddressOf()));
		result = deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext2), reinterpret_cast<void**>(DeviceContext.GetAddressOf()));

		device->Release();
		deviceContext->Release();

		result = Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, MultiSamplingCount, &MultiSamplingQualityLevels);

		if (MultiSamplingQualityLevels <= 0)
		{
			throw std::runtime_error("Multisampling Unsupported");
		}

		DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
		ZeroMemory(&swapChainDescription, sizeof(swapChainDescription));

		swapChainDescription.Width = static_cast<uint32_t>(ScreenWidth);
		swapChainDescription.Height = static_cast<uint32_t>(ScreenHeight);
		swapChainDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDescription.Stereo = false;
		swapChainDescription.SampleDesc.Count = 1;	//MultiSamplingQualityLevels;								
		swapChainDescription.SampleDesc.Quality = 0;	//MultiSamplingQualityLevels - 1;
		swapChainDescription.BufferCount = BufferCount;
		swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		Microsoft::WRL::ComPtr<IDXGIDevice1> DxgiDevice = nullptr;
		result = Device->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(DxgiDevice.GetAddressOf()));

		Microsoft::WRL::ComPtr<IDXGIAdapter> DxgiAdapter = nullptr;
		result = DxgiDevice->GetAdapter(DxgiAdapter.GetAddressOf());

		Microsoft::WRL::ComPtr<IDXGIFactory2> DxgiFactory = nullptr;
		result = DxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(DxgiFactory.GetAddressOf()));

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDescription;
		ZeroMemory(&fullScreenDescription, sizeof(fullScreenDescription));

		fullScreenDescription.RefreshRate.Numerator = RefreshRate;
		fullScreenDescription.RefreshRate.Denominator = 1;
		fullScreenDescription.Windowed = IsWindowed;

		IDXGISwapChain1* swapChain;

		result = DxgiFactory->CreateSwapChainForHwnd(DxgiDevice.Get(), WindowHandle, &swapChainDescription, &fullScreenDescription, nullptr, &swapChain);
		result = swapChain->QueryInterface(__uuidof(IDXGISwapChain2), reinterpret_cast<void**>(SwapChain.GetAddressOf()));
	}

	HWND Direct3DCore::GetWindowHandle()
	{
		return WindowHandle;
	}

	void Direct3DCore::SetScreenResolution(float screenWidth, float screenHeight)
	{
		ScreenWidth = screenWidth;
		ScreenHeight = screenHeight;
	}

	void Direct3DCore::SetRefreshRate(uint32_t refreshRate)
	{
		RefreshRate = refreshRate;
	}

	void Direct3DCore::SetMultiSamplingCount(uint32_t multiSamplingCount)
	{
		MultiSamplingCount = multiSamplingCount;
	}

	void Direct3DCore::SetBufferCount(uint32_t bufferCount)
	{
		BufferCount = bufferCount;
	}

	float Direct3DCore::GetScreenWidth()
	{
		return ScreenWidth;
	}

	float Direct3DCore::GetScreenHeight()
	{
		return ScreenHeight;
	}

	uint32_t Direct3DCore::GetRefreshRate() const
	{
		return RefreshRate;
	}

	uint32_t Direct3DCore::GetMultiSamplingCount() const
	{
		return MultiSamplingCount;
	}

	uint32_t Direct3DCore::GetMultiSamplingQualityLevels() const
	{
		return MultiSamplingQualityLevels;
	}

	uint32_t Direct3DCore::GetBufferCount() const
	{
		return BufferCount;
	}

	ID3D11Device2* Direct3DCore::GetDevice() const
	{
		return Device.Get();
	}

	ID3D11DeviceContext2* Direct3DCore::GetDeviceContext() const
	{
		return DeviceContext.Get();
	}

	IDXGISwapChain2 * Direct3DCore::GetSwapChain() const
	{
		return nullptr;
	}


}