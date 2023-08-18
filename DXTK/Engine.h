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

		int run(App* aplication);

		float frameTime();

		static Window* window()
		{
			return pWindow;
		}

		static InputComponent* input()
		{
			return pInput;
		}

		static LRESULT CALLBACK engineProc(HWND, UINT, WPARAM, LPARAM);

	protected:
		int loop();

		static Window* pWindow;
		static InputComponent* pInput;
		static App* pApp;

		Timer timer;
		float fDeltaTime;
	};
}
