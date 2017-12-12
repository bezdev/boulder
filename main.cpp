#include "OculusApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR str, int)
{   
    OculusApp app(hInstance);

    try
    {
        app.Initialize();
    }
    catch (std::exception& e)
    {
        OutputDebugString("\n");
        OutputDebugString(e.what());
        OutputDebugString("\n");

        if (IsDebuggerPresent())
        {
            DebugBreak();
        }

        exit(1);
    }

    app.Run();

    return(0);
}