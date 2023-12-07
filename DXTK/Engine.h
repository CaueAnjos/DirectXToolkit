#pragma once
#include "Types.h"
#include "Timer.h"

namespace dxtk
{
	class App;
	class Window;
	class Input;
	class Timer;
	class Graphics;

	class Engine
	{
	public:
		Engine();
		Engine(const Engine&) = delete;
		~Engine();

		static Engine* pCurrent;

		int run(App* aplication);

		float frameTime();

		Window* window()
		{
			return pWindow.get();
		}

		Input* input()
		{
			return pInput.get();
		}

		Graphics* graphic()
		{
			return pGraphic.get();
		}

		static LRESULT CALLBACK engineProc(HWND, UINT, WPARAM, LPARAM);

		void  pause()
		{
			pCurrent->bPaused = true;
			pCurrent->timer.stop();
		}

		void resume()
		{
			pCurrent->bPaused = false;
			pCurrent->timer.start();
		}

	protected:
		int loop();

		std::unique_ptr<Window> pWindow;
		std::unique_ptr<Input> pInput;
		std::unique_ptr<Graphics> pGraphic;
		App* pApp;

		Timer timer;
		float fDeltaTime;

		bool bPaused;
	};
}
