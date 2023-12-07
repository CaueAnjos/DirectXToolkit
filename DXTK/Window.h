#pragma once
#include "Types.h"

enum WINDOW_MODES
{
	WINDOWED, FULLSCREEN
};

namespace dxtk
{
	class Window
	{
	public:
		Window();

		void mode(WINDOW_MODES mode);
		void size(int32_t w, int32_t h);
		void color(ColorEx color) { bg = color; }
		void title(const char* text) { sTitle = text; }
		void icon(uint32_t id) { wndIcon = LoadIcon(wndInstance, MAKEINTRESOURCE(id)); }
		void cursor(uint32_t id) { wndCursor = LoadCursor(wndInstance, MAKEINTRESOURCE(id)); }

		void hideCursor(bool hide) { ShowCursor(!hide); }
		void print(const char* text, int x, int y, ColorEx color);
		void clear();
		void close() { PostMessage(hWnd, WM_DESTROY, 0, 0); }
		bool create();

		HWND id() const { return hWnd; }
		int32_t height() const { return nHeight; }
		int32_t width() const { return nWidth; }
		int32_t centerX() const { return nWidth / 2; }
		int32_t centerY() const { return nHeight / 2; }
		WINDOW_MODES mode() const { return wndMode; }
		const String& title() const { return sTitle; }
		ColorEx color() const { return bg; }

		static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

	private:
		int32_t nHeight;
		int32_t nWidth;

		int32_t x;
		int32_t y;

		WINDOW_MODES wndMode;
		HWND hWnd;
		ColorEx bg;
		HICON wndIcon;
		HCURSOR wndCursor;
		HINSTANCE wndInstance;
		String sTitle;
		UINT unStyle;
	};
}
