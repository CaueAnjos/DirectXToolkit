#pragma once
#include "Window.h"
#include "ImputComponent.h"

namespace dxtk
{
	class Engine;
	class App
	{
		friend Engine;
	public:
		App() : pWindow(nullptr), pInput(nullptr)
		{}

		virtual void init() = 0;
		virtual	void update() = 0;
		virtual	void finalize() = 0;

		virtual void draw() {}
		virtual void display() {}

		Window* window()
		{
			return pWindow;
		}

		InputComponent* input()
		{
			return pInput;
		}

	private:
		Window* pWindow;
		InputComponent* pInput;
	};
}
