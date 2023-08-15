#include "App.h"
#include "Engine.h"

namespace dxtk
{
	App::App()
	{
		s_pWindow = Engine::window();
		s_pInput = Engine::input();
	}
}