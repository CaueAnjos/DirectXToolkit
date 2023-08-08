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
			return m_window;
		}

		InputComponent* input()
		{
			return m_input;
		}

	private:
		Window* m_window;
		InputComponent* m_input;
	};
}
