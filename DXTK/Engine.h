#pragma once

namespace dxtk
{
	class App;
	class Window;
	class InputComponent;

	class Engine
	{
	public:
		Engine();
		Engine(const Engine&) = delete;
		~Engine();

		int run(App* aplication);

		static Window* window()
		{
			return s_pWindow;
		}

		static InputComponent* input()
		{
			return s_pInput;
		}

	protected:
		int loop();

		static Window* s_pWindow;
		static InputComponent* s_pInput;
		static App* s_pApp;
	};
}
