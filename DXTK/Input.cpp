#include "Input.h"
#include "Window.h"

namespace dxtk
{
	std::array<Key, 256> Input::inputBuffer;
	Mouse Input::mouse;

	Input::Input(Window* owner)
		: pOwner(owner), active(true)
	{
		SetWindowLongPtr(pOwner->id(), GWLP_WNDPROC, (LONG_PTR)Input::inputProc);
	}

	Input::~Input()
	{
		SetWindowLongPtr(pOwner->id(), GWLP_WNDPROC, (LONG_PTR)Window::wndProc);
		active = false;
	}

	bool Input::keyPressed(int vkey)
	{
		if(active)
		{
			if(inputBuffer[vkey].ctrl)
			{
				if(keyDown(vkey))
				{
					inputBuffer[vkey].ctrl = false;
					return true;
				}
			}
			else if(keyUp(vkey))
			{
				inputBuffer[vkey].ctrl = true;
			}
		}
		return false;
	}

	short Input::MouseWheel()
	{
		auto ret = mouse.wheel;
		mouse.wheel = 0;
		return ret;
	}

	LRESULT CALLBACK Input::inputProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch(msg)
		{
			case WM_KEYDOWN:
				inputBuffer[wParam].down = true;
				return 0;
			case WM_KEYUP:
				inputBuffer[wParam].down = false;
				return 0;
			case WM_LBUTTONDOWN:
				inputBuffer[VK_LBUTTON].down = true;
				return 0;
			case WM_LBUTTONUP:
				inputBuffer[VK_LBUTTON].down = false;
				return 0;
			case WM_RBUTTONDOWN:
				inputBuffer[VK_RBUTTON].down = true;
				return 0;
			case WM_RBUTTONUP:
				inputBuffer[VK_LBUTTON].down = false;
				return 0;
			case WM_MBUTTONDOWN:
				inputBuffer[VK_MBUTTON].down = true;
				return 0;
			case WM_MBUTTONUP:
				inputBuffer[VK_MBUTTON].down = false;
				return 0;
			case WM_MOUSEMOVE:
				mouse.x = GET_X_LPARAM(lParam);
				mouse.y = GET_Y_LPARAM(lParam);
				return 0;
			case WM_MOUSEWHEEL:
				mouse.wheel = GET_WHEEL_DELTA_WPARAM(wParam);
				return 0;
		}
		return CallWindowProc(Window::wndProc, wnd, msg, wParam, lParam);
	}
}