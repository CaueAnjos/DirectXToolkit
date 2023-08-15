#pragma once

namespace dxtk
{
	class Window;
	struct Key
	{
		bool down, ctrl;
	};

	struct Mouse
	{
		int x, y;
		short wheel;
	};

	class InputComponent
	{
	public:
		InputComponent(Window* owner);
		~InputComponent();

		bool keyDown(int vkey) { return m_inputBuffer[vkey].down; }
		bool keyUp(int vkey) { return !(m_inputBuffer[vkey].down); }
		bool keyPressed(int vkey);

		int MouseX() { return m_mouse.x; }
		int MouseY() { return m_mouse.y; }
		short MouseWheel();

		void activate();
		void deactivate();
		bool active() { return m_active; }

	protected:
		Key m_inputBuffer[256];
		Mouse m_mouse;
		bool m_active;
		Window* m_owner;
	};
}
