#pragma once
#include "Types.h"
#include "Timer.h"

namespace dxtk
{
	class App;
	class Window;
	class InputComponent;
	class Timer;

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

		static LRESULT CALLBACK engineProc(HWND, UINT, WPARAM, LPARAM);

	protected:
		int loop();

		Window* pWindow;
		InputComponent* pInput;
		App* pApp;

		Timer timer;
		float fDeltaTime;
	};
}
