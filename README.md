# DXTK framework

>Status: development

## Required: 

to work properly you need to install the windows sdk, but if you are using visual studio it is installed automatically.

## Seting a game project with DXTK:

DXTK is a static lib project, so you just need to link it to your game project and include:

```
#include <DXTK.h>
```

to make your game you need to extend an 'App':

```
//this class extend App
class WinApp : public dxtk::App
{
public:
    //is called when the engine initialize the app
    void init()
    {}

    //is called every frame
    void update()
    {
        if(input()->keyPressed(VK_ESCAPE))
        {
            window()->close();
        }
    }

    //is called when the window is destroyed
    void finalize()
    {}


    //is called every time the window need to be redrawn
    //if you need to drawn every frame use 'drawn()' instead
    void display()
    {
        window()->print("hello", 0, 0, Color());
    }
};
```

and, on WinMain, you need to configure the window, and just run the aplication:

```
START_CONFIG
    dxtk::Engine* pEngine = new dxtk::Engine;
    pEngine->window()->mode(WINDOWED);
    pEngine->window()->color(Color(255, 255, 255));
    pEngine->window()->size(500, 300);
    pEngine->window()->title("Simple App");
    
    int result = pEngine->run(new WinApp);
    
    delete pEngine;
    return result;
END_CONFIG
```

this will lead you to a 500 by 300 pixel white window with "Simple App" as the title.
it is a basic window but it can be improved.
