#include "Window.h"

namespace dxtk
{
    Window::Window()
    {
        hWnd = NULL;

        bg = ColorRGB(255, 255, 255);

	    wndIcon = NULL;
	    wndCursor = NULL;
	    wndInstance = GetModuleHandle(NULL);

	    sTitle = "WindosApp";

        unStyle = WS_POPUP | WS_VISIBLE;
        wndMode = FULLSCREEN;

        nHeight = GetSystemMetrics(SM_CYSCREEN);
        nWidth = GetSystemMetrics(SM_CXSCREEN);
    }

    void Window::mode(WINDOW_MODES mode)
    {
        mode = mode;
        if(mode == WINDOWED)
        {
            unStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE;
        }
        else
        {
            unStyle = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE;
        }
    }

    void Window::size(int32_t w, int32_t h)
    {
        nWidth = w;
        nHeight = h;

        x = (GetSystemMetrics(SM_CXSCREEN) / 2 ) - (nWidth / 2);
        y = (GetSystemMetrics(SM_CYSCREEN) / 2 ) - (nHeight / 2);
    }

    void Window::print(const char* text, int x, int y, ColorEx color)
    {
        HDC xdc = GetDC(hWnd);
        SetTextColor(xdc, color.getRGB()());
        SetBkMode(xdc, TRANSPARENT);
        TextOut(xdc, x, y, text, (int)strlen(text));
        ReleaseDC(hWnd, xdc);
    }

    void Window::clear()
    {
        HDC xdc = GetDC(hWnd);
        RECT area;
        GetClientRect(hWnd, &area);
        FillRect(xdc, &area, CreateSolidBrush(bg.getRGB()()));
        ReleaseDC(hWnd, xdc);
    }

    bool Window::create()
    {
        WNDCLASSEX wc;
        wc.cbSize = sizeof WNDCLASSEX;
        wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = wndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = wndInstance;
        wc.hIcon = wndIcon;
        wc.hCursor = wndCursor;
        wc.hbrBackground = CreateSolidBrush(bg.getRGB()());
        wc.lpszMenuName = NULL;
        wc.lpszClassName = sTitle.c_str();
        wc.hIconSm = wndIcon;

        if(!RegisterClassEx(&wc))
        {
            return false;
        }

        // Create the window.

        hWnd = CreateWindowEx(
            0,                              // Optional window styles.
            sTitle.c_str(),                     // Window class
            sTitle.c_str(),    // Window text
            unStyle,            // Window style

            // Size and position
            x, y, nWidth, nHeight,

            NULL,       // Parent window    
            NULL,       // Menu
            wndInstance,  // Instance handle
            NULL        // Additional application data
        );

        if(wndMode == WINDOWED)
        {
            RECT client = { 0, 0, (long)nWidth, (long)nHeight };
            AdjustWindowRectEx(&client, 
                GetWindowStyle(hWnd), 
                GetMenu(hWnd) != NULL, 
                GetWindowExStyle(hWnd));

            x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (nWidth / 2);
            y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (nHeight / 2);

            MoveWindow(hWnd, x, y, 
                client.right - client.left, 
                client.bottom - client.top, 
                TRUE);
        }

        if(hWnd == NULL)
        {
            return false;
        }
        return true;
    }

    LRESULT Window::wndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch(msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(wnd, msg, wParam, lParam);
    }
}
