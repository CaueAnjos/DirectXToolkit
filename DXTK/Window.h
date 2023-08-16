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
		void size(uint32_t w, uint32_t h);
		void color(Color color) { bg = color; }
		void title(const char* text) { sTitle = text; }
		void icon(uint32_t id) { wndIcon = LoadIcon(wndInstance, MAKEINTRESOURCE(id)); }
		void cursor(uint32_t id) { wndCursor = LoadCursor(wndInstance, MAKEINTRESOURCE(id)); }

		void hideCursor(bool hide) { ShowCursor(!hide); }
		void print(const char* text, int x, int y, Color color);
		void clear();
		void close() { PostMessage(hWnd, WM_DESTROY, 0, 0); }
		bool create();

		HWND id() const { return hWnd; }
		uint32_t height() const { return unHeight; }
		uint32_t width() const { return unWidth; }
		uint32_t centerX() const { return unWidth / 2; }
		uint32_t centerY() const { return unHeight / 2; }
		WINDOW_MODES mode() const { return wndMode; }
		const String& title() const { return sTitle; }
		Color color() const { return bg; }

		static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

	protected:
		uint32_t unHeight;
		uint32_t unWidth;

		int x;
		int y;

		WINDOW_MODES wndMode;
		HWND hWnd;
		Color bg;
		HICON wndIcon;
		HCURSOR wndCursor;
		HINSTANCE wndInstance;
		String sTitle;
		UINT unStyle;
	};
}
