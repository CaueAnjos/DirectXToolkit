#include "Messager.h"
#include "Window.h"

namespace dxtk
{
    Window* Messager::s_owner = nullptr;

    Messager::Messager(Window* owner)
    {
        s_owner = owner;
    }

    bool Messager::registred(uint32_t message) const
    {
        if(m_messageMap.find(message) != m_messageMap.end())
            return true;
        else
            return false;
    }

    bool Messager::distribute(uint32_t message, WPARAM wParam, LPARAM lParam)
    {
        const Messager& messager = s_owner->messager();
        if(messager.registred(message))
        {
            MessageAction action = messager.get(message);
            action(s_owner, wParam, lParam);
            return true;
        }
        return false;
    }

    LRESULT CALLBACK Messager::procedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        switch(Msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            distribute(Msg, wParam, lParam);
        }
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    };
}