#include "Engine.h"
#include "Window.h"
#include "App.h"
#include "ImputComponent.h"

namespace dxtk
{
	Window* Engine::pWindow = nullptr;
	InputComponent* Engine::pInput = nullptr;
	App* Engine::pApp = nullptr;

	Engine::Engine()
	{
		pWindow = new Window;
	}

	Engine::~Engine()
	{
		delete pApp;
		delete pInput;
		delete pWindow;
	}

	int Engine::run(App* aplication)
	{
		pApp = aplication;
		pWindow->create();

		pInput = new InputComponent(pWindow);

		pApp->pWindow = pWindow;
		pApp->pInput = pInput;

		SetWindowLongPtr(pWindow->id(), GWLP_WNDPROC, (LONG_PTR)Engine::engineProc);
		return loop();
	}

	LRESULT CALLBACK Engine::engineProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if(msg == WM_PAINT)
			pApp->display();

		return CallWindowProc(InputComponent::inputProc, wnd, msg, wParam, lParam);
	}

	int Engine::loop()
	{
		MSG msg = { 0 };
		pApp->init();

		do
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				pApp->update();
				pApp->draw();
			}
		} while(msg.message != WM_QUIT);

		pApp->finalize();
		return (int)msg.wParam;
	}
}