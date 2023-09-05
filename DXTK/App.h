#pragma once
#include "Engine.h"

namespace dxtk
{
	class Window;
	class InputComponent;

	class App
	{
	public:
		virtual void init() = 0;
		virtual	void update() = 0;
		virtual	void finalize() = 0;

		virtual void draw() {}
		virtual void display() {}
		virtual void whilePaused() {}

		Window* window()
		{
			return Engine::pCurrent->window();
		}

		InputComponent* input()
		{
			return Engine::pCurrent->input();
		}
	};
}
