#pragma once
#include "Engine.h"

namespace dxtk
{
	class Window;
	class Input;
	class Graphics;

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

		Input* input()
		{
			return Engine::pCurrent->input();
		}

		Graphics* graphics()
		{
			return Engine::pCurrent->graphic();
		}
	};
}
