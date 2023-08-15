#include "Window.h"

namespace dxtk
{
    Window::Window()
    {
        m_messager.pOwner = this;
        m_hWnd = NULL;

	    m_bg.r = 255;
	    m_bg.g = 255;
	    m_bg.b = 255;

	    m_icon = NULL;
	    m_cursor = NULL;
	    m_instance = GetModuleHandle(NULL);

	    m_sTitle = "WindosApp";

        m_style = WS_POPUP | WS_VISIBLE;
        m_mode = FULLSCREEN;

        m_height = GetSystemMetrics(SM_CYSCREEN);
        m_width = GetSystemMetrics(SM_CXSCREEN);
    }

    void Window::mode(WINDOW_MODES mode)
    {
        m_mode = mode;
        if(mode == WINDOWED)
        {
            m_style = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE;
        }
        else
        {
            m_style = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE;
        }
    }

    void Window::size(uint32_t w, uint32_t h)
    {
        m_width = w;
        m_height = h;

        m_x = (GetSystemMetrics(SM_CXSCREEN) / 2 ) - (m_width / 2);
        m_y = (GetSystemMetrics(SM_CYSCREEN) / 2 ) - (m_height / 2);
    }

    void Window::print(const char* text, int x, int y, Color color)
    {
        HDC xdc = GetDC(m_hWnd);
        SetTextColor(xdc, color());
        SetBkMode(xdc, TRANSPARENT);
        TextOut(xdc, x, y, text, (int)strlen(text));
        ReleaseDC(m_hWnd, xdc);
    }

    void Window::clear()
    {
        HDC xdc = GetDC(m_hWnd);
        RECT area;
        GetClientRect(m_hWnd, &area);
        FillRect(xdc, &area, CreateSolidBrush(m_bg()));
        ReleaseDC(m_hWnd, xdc);
    }

    bool Window::create()
    {
        WNDCLASSEX wc;
        wc.cbSize = sizeof WNDCLASSEX;
        wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = m_messager.winProc();
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = m_instance;
        wc.hIcon = m_icon;
        wc.hCursor = m_cursor;
        wc.hbrBackground = CreateSolidBrush(m_bg());
        wc.lpszMenuName = NULL;
        wc.lpszClassName = m_sTitle.c_str();
        wc.hIconSm = m_icon;

        if(!RegisterClassEx(&wc))
        {
            return false;
        }

        // Create the window.

        m_hWnd = CreateWindowEx(
            0,                              // Optional window styles.
            m_sTitle.c_str(),                     // Window class
            m_sTitle.c_str(),    // Window text
            m_style,            // Window style

            // Size and position
            m_x, m_y, m_width, m_height,

            NULL,       // Parent window    
            NULL,       // Menu
            m_instance,  // Instance handle
            NULL        // Additional application data
        );

        if(m_mode == WINDOWED)
        {
            RECT client = { 0, 0, (long)m_width, (long)m_height };
            AdjustWindowRectEx(&client, 
                GetWindowStyle(m_hWnd), 
                GetMenu(m_hWnd) != NULL, 
                GetWindowExStyle(m_hWnd));

            m_x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (m_width / 2);
            m_y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (m_height / 2);

            MoveWindow(m_hWnd, m_x, m_y, 
                client.right - client.left, 
                client.bottom - client.top, 
                TRUE);
        }

        if(m_hWnd == NULL)
        {
            return false;
        }
        return true;
    }
}
