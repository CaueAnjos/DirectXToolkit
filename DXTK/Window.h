#pragma once
#include "Types.h"
#include "Messager.h"

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
		void color(Color color) { m_bg = color; }
		void title(const char* text) { m_sTitle = text; }
		void icon(uint32_t id) { m_icon = LoadIcon(m_instance, MAKEINTRESOURCE(id)); }
		void cursor(uint32_t id) { m_cursor = LoadCursor(m_instance, MAKEINTRESOURCE(id)); }

		void hideCursor(bool hide) { ShowCursor(!hide); }
		void print(const char* text, int x, int y, Color color);
		void clear();
		void close() { PostMessage(m_hWnd, WM_DESTROY, 0, 0); }
		bool create();

		HWND id() const { return m_hWnd; }
		uint32_t height() const { return m_height; }
		uint32_t width() const { return m_width; }
		uint32_t centerX() const { return m_width / 2; }
		uint32_t centerY() const { return m_height / 2; }
		WINDOW_MODES mode() const { return m_mode; }
		const String& title() const { return m_sTitle; }
		Color color() const { return m_bg; }

		Messager& messager() { return m_messager; }

	protected:
		Messager m_messager;

		uint32_t m_height;
		uint32_t m_width;

		int m_x;
		int m_y;

		WINDOW_MODES m_mode;
		HWND m_hWnd;
		Color m_bg;
		HICON m_icon;
		HCURSOR m_cursor;
		HINSTANCE m_instance;
		String m_sTitle;
		UINT m_style;
	};
}
