#include "pch.h"
#include "ModThread.hpp"
#include "Mod.hpp"

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
        // Wait until GameAssembly is found
        while (!GetModuleHandleW(L"gameassembly.dll"))
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Start the mod
        Mod mod{};

        // Wait for a signal to exit
        WaitForSingleObject(stopEvent_, INFINITE);
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
