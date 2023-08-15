#pragma once
#include "Types.h"
#include <functional>

namespace dxtk
{
	constexpr int MAX_SIZE = 3;

	class Window;
	typedef std::function<void(Window* window, WPARAM wParam, LPARAM lParam)> MessageAction;

	class Messager
	{
	public:
		Messager();
		
		WNDPROC winProc() { return procedure; }

		void bindAction(uint32_t message, MessageAction action) { m_messageMap[message] = action; }
		void unbindAction(uint32_t message) { m_messageMap.erase(message); }

		bool registred(uint32_t message) const;
		MessageAction call(uint32_t message) const { return m_messageMap.at(message); }

		static void onDestroy(Window* wnd, WPARAM wParam, LPARAM lParam);

		Window* pOwner;

	protected:
		std::map<uint32_t, MessageAction> m_messageMap;

		static Messager* s_messagerList[MAX_SIZE];
		static int s_nlast;
		static LRESULT CALLBACK procedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	};
}
