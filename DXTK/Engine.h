#pragma once
#include "Types.h"
#include "Timer.h"

namespace dxtk
{
	class App;
	class Window;
	class InputComponent;
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
			return pWindow;
		}

		InputComponent* input()
		{
			return pInput;
		}

		Graphics* graphic()
		{
			return pGraphic;
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

		Window* pWindow;
		InputComponent* pInput;
		Graphics* pGraphic;
		App* pApp;

		Timer timer;
		float fDeltaTime;

		bool bPaused;
	};
}
