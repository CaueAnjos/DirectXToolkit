#pragma once
#include "Window.h"
#include "ImputComponent.h"
#include "App.h"

namespace dxtk
{
	class App;
	class Engine
	{
	public:
		Engine(const Engine&) = delete;
		~Engine();

		int run(App* aplication);

		static Engine* get()
		{
			return s_engine;
		}

		Window* window()
		{
			return m_window;
		}

		InputComponent* input()
		{
			return m_input;
		}

	protected:
		int loop();

		static Engine* s_engine;

		Window* m_window;
		InputComponent* m_input;

		App* m_app;

	private:
		Engine();
	};
}
