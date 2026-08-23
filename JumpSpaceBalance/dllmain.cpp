#include "pch.h"
#include "Mod/ModThread.hpp"

#ifdef JUMPSPACEBALANCE_STANDALONE
    #include <UltimateProxyDLL.h> // must be included precisely once, here
#endif

#include <string>

namespace
{
    bool IsInJumpSpace()
    {
        char path[MAX_PATH]{};
        GetModuleFileNameA(NULL, path, std::size(path) - 1);
        return std::string(path).find("Jump Space.exe") != std::string::npos;
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
    }

    if (!IsInJumpSpace() || DetourIsHelperProcess())
    {
        return TRUE;
    }

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DetourRestoreAfterWith();           // set up Detours
#ifdef JUMPSPACEBALANCE_STANDALONE
            upd::create_proxy(hModule);         // set up UltimateProxyDLL
#endif
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

#ifdef JUMPSPACEBALANCE_STANDALONE
    // version.dll exports for UltimateProxyDLL
    EXPORT(1, GetFileVersionInfoA)
    EXPORT(2, GetFileVersionInfoByHandle)
    EXPORT(3, GetFileVersionInfoExA)
    EXPORT(4, GetFileVersionInfoExW)
    EXPORT(5, GetFileVersionInfoSizeA)
    EXPORT(6, GetFileVersionInfoSizeExA)
    EXPORT(7, GetFileVersionInfoSizeExW)
    EXPORT(8, GetFileVersionInfoSizeW)
    EXPORT(9, GetFileVersionInfoW)
    EXPORT(10, VerFindFileA)
    EXPORT(11, VerFindFileW)
    EXPORT(12, VerInstallFileA)
    EXPORT(13, VerInstallFileW)
    EXPORT(14, VerLanguageNameA)
    EXPORT(15, VerLanguageNameW)
    EXPORT(16, VerQueryValueA)
    EXPORT(17, VerQueryValueW)
#endif
