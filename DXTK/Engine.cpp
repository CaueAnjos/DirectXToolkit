#include "Engine.h"
#include "Window.h"
#include "App.h"
#include "ImputComponent.h"

namespace dxtk
{
	Window* Engine::s_pWindow = nullptr;
	InputComponent* Engine::s_pInput = nullptr;
	App* Engine::s_pApp = nullptr;

	Engine::Engine()
	{
		s_pWindow = new Window;
		s_pInput = new InputComponent(s_pWindow);
	}

	Engine::~Engine()
	{
		delete s_pApp;
		delete s_pInput;
		delete s_pWindow;
	}

	int Engine::run(App* aplication)
	{
		s_pApp = aplication;
		s_pWindow->create();

		s_pWindow->messager().bindAction(WM_PAINT,
			[this](Window* wnd, WPARAM wParam, LPARAM lParam)
			{
				s_pApp->display();
			});

		return loop();
	}

	int Engine::loop()
	{
		MSG msg = { 0 };
		s_pApp->init();

		do
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				s_pApp->update();
				s_pApp->draw();
			}
		} while(msg.message != WM_QUIT);

		s_pApp->finalize();
		return (int)msg.wParam;
	}
}