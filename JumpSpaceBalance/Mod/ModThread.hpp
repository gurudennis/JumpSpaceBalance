#pragma once

#include <atomic>
#include <thread>

namespace JSB
{
    class ModThread
    {
    public:
        static ModThread& GetInstance();

        void Finalize();

    private:
        ModThread();
        ~ModThread();

        void ThreadProc();

    private:
        std::thread thread_;
        std::atomic<HANDLE> stopEvent_{};
    };
}
