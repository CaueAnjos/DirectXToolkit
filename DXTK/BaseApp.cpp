#include "BaseApp.h"

void dxtk::BaseApp::update()
{
    if(input()->keyDown(VK_ESCAPE))
        window()->close();
}
