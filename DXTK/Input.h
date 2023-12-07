#pragma once
#include "Types.h"

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

	class Input
	{
	public:
		Input(Window* owner);
		~Input();

		bool keyDown(int vkey) { return active ? inputBuffer[vkey].down : false; }
		bool keyUp(int vkey) { return active ? !(inputBuffer[vkey].down) : false; }
		bool keyPressed(int vkey);

		int MouseX() { return mouse.x; }
		int MouseY() { return mouse.y; }
		short MouseWheel();

		static LRESULT CALLBACK inputProc(HWND, UINT, WPARAM, LPARAM);

		bool active;
	protected:
		static std::array<Key, 256> inputBuffer;
		static Mouse mouse;
		Window* pOwner;
	};
}
