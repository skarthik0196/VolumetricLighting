// VolumetricLightingDirectX.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "VolumetricLightingDirectX.h"


#include "Direct3D.h"

std::wstring WindowClassName = L"VolumetricLighting.DirectX";
std::wstring WindowTitleName = L"Volumetric Lighting";
float ScreenWidth = 1024;
float ScreenHeight = 768;

using namespace Rendering;

int WINAPI WinMain(HINSTANCE CurrentInstance, HINSTANCE PreviousInstance, LPSTR CommandLine, int ShowCommand)
{
	UNREFERENCED_PARAMETER(PreviousInstance);
	UNREFERENCED_PARAMETER(CommandLine);

	std::shared_ptr<Direct3D> Direct3DRenderer = std::make_shared<Direct3D>(ScreenWidth, ScreenHeight);

	Direct3DRenderer->InitializeWindow(CurrentInstance, WindowClassName, WindowTitleName, ShowCommand);
	Direct3DRenderer->InitializeDirect3D();
	Direct3DRenderer->SetVSync(true);

	MSG Message;

	while (true)
	{
		if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
			if (Message.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			Direct3DRenderer->ClearRenderTarget();
			Direct3DRenderer->ClearDepthStencilView();
			Direct3DRenderer->RenderToScreen();
		}
	}

	return 0;
}