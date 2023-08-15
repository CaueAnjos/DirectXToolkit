#include "Messager.h"
#include "Window.h"

namespace dxtk
{
    Messager* Messager::s_messagerList[MAX_SIZE] = {nullptr};
    int Messager::s_nlast = 0;

    Messager::Messager()
        : pOwner(nullptr)
    {
        if(s_nlast < MAX_SIZE)
        {
            s_messagerList[s_nlast] = this;
            bindAction(WM_DESTROY, onDestroy);
            s_nlast++;
        }
    }

    bool Messager::registred(uint32_t message) const
    {
        if(m_messageMap.find(message) != m_messageMap.end())
            return true;
        else
            return false;
    }

    void Messager::onDestroy(Window* wnd, WPARAM wParam, LPARAM lParam)
    {
        PostQuitMessage(0);
    }

    LRESULT CALLBACK Messager::procedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        for(const auto& pAt : s_messagerList)
        {
            if(pAt && pAt->registred(Msg))
            {
                pAt->call(Msg)(pAt->pOwner, wParam, lParam);
            }
        }
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    };
}