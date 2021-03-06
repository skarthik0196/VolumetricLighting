// VolumetricLightingDirectX.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "VolumetricLightingDirectX.h"
#include "SceneManager.h"
#include "Direct3D.h"
#include <Shlwapi.h>

std::wstring WindowClassName = L"VolumetricLighting.DirectX";
std::wstring WindowTitleName = L"Volumetric Lighting";
float ScreenWidth = 1366;
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

	WCHAR PathBuffer[MAX_PATH];
	GetModuleFileName(nullptr, PathBuffer, MAX_PATH);
	PathRemoveFileSpec(PathBuffer);
	std::wstring Pathh(PathBuffer);
	SetCurrentDirectory(Pathh.c_str());

	std::shared_ptr<SceneManager> SceneManager1 = std::make_shared<SceneManager>(Direct3DRenderer, "Content\\Models\\sponza.obj");
	//std::shared_ptr<SceneManager> SceneManager1 = std::make_shared<SceneManager>(Direct3DRenderer, "Content\\Models\\Bistro\\Bistro_Research_Exterior.fbx");

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
			SceneManager1->UpdateSceneManager();
			SceneManager1->DrawCurrentScene();
		}
	}

	return 0;
}