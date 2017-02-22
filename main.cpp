#include "OculusApp.h"

#ifndef VALIDATE
    #define VALIDATE(x, msg) if (!(x)) { MessageBoxA(NULL, (msg), "OculusRoomTiny", MB_ICONERROR | MB_OK); exit(-1); }
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR str, int)
{
    // Initializes LibOVR, and the Rift
    ovrInitParams initParams = { ovrInit_RequestVersion, OVR_MINOR_VERSION, NULL, 0, 0 };
    ovrResult result = ovr_Initialize(&initParams);
    auto ovrShutdown = scope_exit([] { ovr_Shutdown(); });
    VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

    VALIDATE(OVR_SUCCESS(result), "Failed to create libOVR.");
    
    OculusApp app(hInstance);

    try
    {
        app.Initialize();
    }
    catch (std::exception& e)
    {
        OutputDebugString(e.what());
        OutputDebugString("\n");

        exit(1);
    }

    app.Run();

    return(0);
}