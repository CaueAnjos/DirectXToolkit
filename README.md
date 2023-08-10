# DXTK framework

>Status: development

>gamedev framework: 

to work properly you need to install the windows sdk, but if you are using visual studio it is installed automatically.

>seting a game project with DXTK:

DXTK is a static lib project, so you just need to link it to your game project and include:

```
#include <DXTK.h>
using namespace dxtk;
```

to make your game you need to extend an 'App':

```
//this class extend App
class WinApp : public App
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
int APIENTRY WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    Engine* engine = Engine::get();
    engine->window()->mode(WINDOWED);
    engine->window()->color(Color(255, 255, 255));
    engine->window()->size(500, 300);
    engine->window()->title("Simple App");

    int result = engine->run(new WinApp);
    return result;
}
```

this will lead you to a 500 by 300 pixel white window with "Simple App" as the title.
it is a basic window but it can be improved.
