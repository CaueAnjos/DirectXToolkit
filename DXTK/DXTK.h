#pragma once

#include "Types.h"
#include "Mesh.h"
#include "Error.h"
#include "Input.h"
#include "Window.h"
#include "App.h"
#include "BaseApp.h"
#include "Graphics.h"
#include "Engine.h"

#define START_CONFIG \
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)\
{\
    try\
    {

#define END_CONFIG \
}\
catch(const dxtk::Error& error)\
{\
    MessageBox(nullptr, error.description().c_str(), "DXTK Error", MB_OK);\
    return 0;\
}\
}
