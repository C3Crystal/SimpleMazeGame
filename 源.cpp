#include <Windows.h>
#include "d2d1.h"
#include <chrono>
using namespace std;
using namespace chrono;
#include <cmath>
#include "Game.h"

ID2D1Factory * DFactory = NULL;
ID2D1HwndRenderTarget * DRenderTarget = NULL;
ID2D1SolidColorBrush * DBrush = NULL;
RECT RectWnd = {};

game TheGame(4);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hInstancePrev, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	LPCTSTR WindowClassName = (LPCTSTR)L"ProjectËæ»úÃÔ¹¬2.1";
	wcex.lpszClassName = WindowClassName;
	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(WindowClassName, (LPCTSTR)L"   ", 
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 
		500, 200, 600, 600, NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	GetClientRect(hWnd, &RectWnd);
	static D2D1_SIZE_U DSizeWnd = D2D1::SizeU(RectWnd.right, RectWnd.bottom);
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &DFactory);
	DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, DSizeWnd), &DRenderTarget);
	DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f, 1.f), &DBrush);

	srand(time(NULL));

	TheGame.GenMaze();
	TheGame.Paint();


	MSG Msg = {};

	auto TimeUpdate = high_resolution_clock::now();

	while (true)
	{
		if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		if (Msg.message == WM_QUIT)
			break;
		if (duration_cast<microseconds>(high_resolution_clock::now() - TimeUpdate).count() > 1. / 60.)
		{
			TheGame.CamFollowSnake();
			TheGame.Paint();
			TimeUpdate = high_resolution_clock::now();
		}
	}
	return (int)Msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_PAINT:
	{
		
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:
		{
			TheGame.SnakeMove(-1);
			break;
		}
		case VK_RIGHT:
		{
			TheGame.SnakeMove(+1);
			break;
		}
		case VK_UP:
		{
			TheGame.SnakeMove(-TheGame.nN);
			break;
		}
		case VK_DOWN:
		{
			TheGame.SnakeMove(+TheGame.nN);
			break;
		}
		/*case 'W':
		{
			TheGame.CamPosY -= 0.1f;
			break;
		}
		case 'S':
		{
			TheGame.CamPosY += 0.1f;
			break;
		}
		case 'A':
		{
			TheGame.CamPosX -= 0.1f;
			break;
		}
		case 'D':
		{
			TheGame.CamPosX += 0.1f;
			break;
		}*/
		}
		TheGame.Paint();
		break;
	}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}