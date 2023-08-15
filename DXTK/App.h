#pragma once
#include "Window.h"
#include "ImputComponent.h"

namespace dxtk
{
	class App
	{
	public:
		App();

		virtual void init() = 0;
		virtual	void update() = 0;
		virtual	void finalize() = 0;

		virtual void draw() {}
		virtual void display() {}

		Window* window()
		{
			return s_pWindow;
		}

		InputComponent* input()
		{
			return s_pInput;
		}

	private:
		Window* s_pWindow;
		InputComponent* s_pInput;
	};
}
