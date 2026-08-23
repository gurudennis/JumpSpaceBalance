#include "pch.h"
#include "Mod/ModThread.hpp"

#include <UltimateProxyDLL.h> // must be included precisely once, here

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (DetourIsHelperProcess())
    {
        return TRUE;
    }

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule); // forgo per-thread notifications
            DetourRestoreAfterWith();           // set up Detours
            upd::create_proxy(hModule);         // set up UltimateProxyDLL
            JSB::ModThread::GetInstance();      // start the mod thread
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

extern "C" __declspec(dllexport) void JumpSpaceBalance_Finalize()
{
    JSB::ModThread::GetInstance().Finalize();
}
