#include "Engine.h"
#include "Window.h"
#include "App.h"
#include "ImputComponent.h"
#include "Graphics.h"

namespace dxtk
{
	Engine* Engine::pCurrent = nullptr;

	Engine::Engine()
		: fDeltaTime(0.f),
		pWindow(nullptr),
		pInput(nullptr),
		pGraphic(nullptr),
		pApp(nullptr),
		bPaused(false)
	{
		pCurrent = this;

		pWindow = new Window;
		pGraphic = new Graphics;
	}

	Engine::~Engine()
	{
		delete pApp;
		delete pInput;
		delete pGraphic;
		delete pWindow;

		if(pCurrent == this)
		{
			pCurrent = nullptr;
		}
	}

	int Engine::run(App* aplication)
	{
		pApp = aplication;
		pWindow->create();

		pInput = new InputComponent(pWindow);

		pGraphic->initialize(pWindow);

		SetWindowLongPtr(pWindow->id(), GWLP_WNDPROC, (LONG_PTR)Engine::engineProc);
		return loop();
	}

	float Engine::frameTime()
	{
#if _DEBUG
		static float fTotalTime = 0.f;
		static uint32_t unFrameCount = 0;
#endif

		fDeltaTime = timer.reset();

#if _DEBUG

		fTotalTime += fDeltaTime;
		unFrameCount++;

		if(fTotalTime >= 1.f)
		{
			std::stringstream text;
			text << std::fixed;
			text.precision(3);

			text << pWindow->title().c_str() << "   "
				<< "FPS: " << unFrameCount << "   "
				<< "Frame Time: " << fDeltaTime * 1000 << "(ms)";

			SetWindowText(pWindow->id(), text.str().c_str());

			unFrameCount = 0;
			fTotalTime -= 1.f;
		}
#endif 
		return fDeltaTime;
	}

	LRESULT CALLBACK Engine::engineProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if(msg == WM_PAINT)
			pCurrent->pApp->display();

		return CallWindowProc(InputComponent::inputProc, wnd, msg, wParam, lParam);
	}

	int Engine::loop()
	{
		timer.start();

		MSG msg = { 0 };
		pApp->init();

		do
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				if(input()->keyPressed(VK_PAUSE))
				{
					if(bPaused)
						resume();
					else
						pause();
				}

				if(!bPaused)
				{
					fDeltaTime = frameTime();

					pApp->update();
					pApp->draw();
				}
				else
				{
					pApp->whilePaused();
				}
			}
		} while(msg.message != WM_QUIT);

		pApp->finalize();
		return (int)msg.wParam;
	}
}