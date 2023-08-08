#include "Engine.h"

namespace dxtk
{
	Engine* Engine::s_engine = new Engine;

	Engine::Engine()
		: m_window(nullptr), m_input(nullptr), m_app(nullptr)
	{
		m_window = new Window;
		m_input = new InputComponent(m_window);
	}

	Engine::~Engine()
	{
		delete m_app;
		delete m_input;
		delete m_window;
	}

	int Engine::run(App* aplication)
	{
		m_app = aplication;
		m_window->create();

		m_window->messager().bindAction(WM_PAINT,
			[this](Window* wnd, WPARAM wParam, LPARAM lParam)
			{
				m_app->display();
			});

		return loop();
	}

	int Engine::loop()
	{
		MSG msg = { 0 };
		m_app->init();

		do
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				m_app->update();
				m_app->draw();
			}
		} while(msg.message != WM_QUIT);

		m_app->finalize();
		return (int)msg.wParam;
	}
}