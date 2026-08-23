#include "pch.h"
#include "ModThread.hpp"
#include "Mod.hpp"
#include "../Log/Log.hpp"

#include <chrono>

namespace JSB
{
    ModThread& ModThread::GetInstance()
    {
        static ModThread instance;
        return instance;
    }

    ModThread::ModThread()
    {
        stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, L"JumpSpaceBalance_StopEvent__");
        thread_ = std::thread([this](){ ThreadProc(); });
    }
    
    ModThread::~ModThread()
    {
        Finalize();
    }

    void ModThread::ThreadProc()
    {
        JSB_LOGINF("Waiting for game to initialize...");

        // Wait until GameAssembly is found
        while (!GetModuleHandleW(L"gameassembly.dll"))
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        JSB_LOGINF("Loading the mod...");

        // Start the mod
        Mod mod{};

        JSB_LOGINF("Mod loaded successfully.");

        // Wait for a signal to exit
        WaitForSingleObject(stopEvent_, INFINITE);

        JSB_LOGINF("Exiting");
    }

    void ModThread::Finalize()
    {
        HANDLE stopEvent = stopEvent_.exchange(NULL);
        if (stopEvent)
        {
            SetEvent(stopEvent);
        }

        if (thread_.joinable())
        {
            thread_.join();
        }

        if (stopEvent)
        {
            CloseHandle(stopEvent);
        }
    }
}
