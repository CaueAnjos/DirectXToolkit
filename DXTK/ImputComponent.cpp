#include "ImputComponent.h"
#include "Window.h"

namespace dxtk
{
	InputComponent::InputComponent(Window* owner)
		: m_owner(owner), m_active(false)/*, m_inputBuffer(), m_mouse()*/
	{
		activate();
	}

	InputComponent::~InputComponent()
	{
		if(m_active)
		{
			deactivate();
		}
	}

	bool InputComponent::keyPressed(int vkey)
	{
		if(m_inputBuffer[vkey].ctrl)
		{
			if(keyDown(vkey))
			{
				m_inputBuffer[vkey].ctrl = false;
				return true;
			}
		}
		else if(keyUp(vkey))
		{
			m_inputBuffer[vkey].ctrl = true;
		}
		return false;
	}

	short InputComponent::MouseWheel()
	{
		auto ret = m_mouse.wheel;
		m_mouse.wheel = 0;
		return ret;
	}

	void InputComponent::activate()
	{
		m_active = true;

		auto downFunc = [this](Window* wnd, WPARAM wParam, LPARAM lParam)
		{
			switch(wParam)
			{
			case MK_MBUTTON:
				m_inputBuffer[VK_MBUTTON].down = true;
				break;

			case MK_RBUTTON:
				m_inputBuffer[VK_RBUTTON].down = true;
				break;

			case MK_LBUTTON:
				m_inputBuffer[VK_LBUTTON].down = true;
				break;

			default:
				m_inputBuffer[wParam].down = true;
				break;
			}
		};

		auto upFunc = [this](Window* wnd, WPARAM wParam, LPARAM lParam)
		{
			switch(wParam)
			{
			case MK_MBUTTON:
				m_inputBuffer[VK_MBUTTON].down = false;
				break;

			case MK_RBUTTON:
				m_inputBuffer[VK_RBUTTON].down = false;
				break;

			case MK_LBUTTON:
				m_inputBuffer[VK_LBUTTON].down = false;
				break;

			default:
				m_inputBuffer[wParam].down = false;
				break;
			}
		};

		m_owner->messager().bindAction(WM_KEYDOWN, downFunc);
		m_owner->messager().bindAction(WM_KEYUP, upFunc);
						 
		m_owner->messager().bindAction(WM_LBUTTONDOWN, downFunc);
		m_owner->messager().bindAction(WM_LBUTTONUP, upFunc);
						 
		m_owner->messager().bindAction(WM_RBUTTONDOWN, downFunc);
		m_owner->messager().bindAction(WM_RBUTTONUP, upFunc);
						 
		m_owner->messager().bindAction(WM_MBUTTONDOWN, downFunc);
		m_owner->messager().bindAction(WM_MBUTTONUP, upFunc);
						 
		m_owner->messager().bindAction(WM_MOUSEMOVE,
			[this](Window* wnd, WPARAM wParam, LPARAM lParam)
			{
				m_mouse.x = GET_X_LPARAM(lParam);
				m_mouse.y = GET_Y_LPARAM(lParam);
			});

		m_owner->messager().bindAction(WM_MOUSEWHEEL,
			[this](Window* wnd, WPARAM wParam, LPARAM lParam)
			{
				m_mouse.wheel = GET_WHEEL_DELTA_WPARAM(wParam);
			});
	}

	void InputComponent::deactivate()
	{
		m_active = false;
		m_owner->messager().unbindAction(WM_KEYDOWN);
		m_owner->messager().unbindAction(WM_KEYUP);
		m_owner->messager().unbindAction(WM_LBUTTONDOWN);
		m_owner->messager().unbindAction(WM_LBUTTONUP);
		m_owner->messager().unbindAction(WM_RBUTTONDOWN);
		m_owner->messager().unbindAction(WM_RBUTTONUP);
		m_owner->messager().unbindAction(WM_MBUTTONDOWN);
		m_owner->messager().unbindAction(WM_MBUTTONUP);
		m_owner->messager().unbindAction(WM_MOUSEMOVE);
		m_owner->messager().unbindAction(WM_MOUSEWHEEL);
	}
}