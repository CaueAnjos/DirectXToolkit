#pragma once
#include "Types.h"
#include <functional>


namespace dxtk
{
	class Window;
	typedef std::function<void(Window* window, WPARAM wParam, LPARAM lParam)> MessageAction;

	class Messager
	{
	public:
		Messager(Window* owner);
		
		WNDPROC winProc() { return procedure; }

		void bindAction(uint32_t message, MessageAction action) { m_messageMap[message] = action; }
		void unbindAction(uint32_t message) { m_messageMap.erase(message); }

		bool registred(uint32_t message) const;
		MessageAction get(uint32_t message) const { return m_messageMap.at(message); }

	protected:
		static Window* s_owner;
		std::map<uint32_t, MessageAction> m_messageMap;

		static bool distribute(uint32_t message, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK procedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	};
}
