#include "App.h"
#include "Engine.h"

namespace dxtk
{
	App::App()
	{
		m_window = Engine::get()->window();
		m_input = Engine::get()->input();
	}
}