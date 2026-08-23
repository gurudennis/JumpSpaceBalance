#include "pch.h"
#include "Log.hpp"

#ifdef JUMPSPACEBALANCE_STANDALONE
    #define JSB_LOG_BANNER_ ""
#else
    #define JSB_LOG_BANNER_ "[JumpSpaceBalance] "
#endif

namespace JSB
{
    namespace
    {
        const char* StrFromLevel(Log::Level level)
        {
            switch (level)
            {
                case Log::Level::Info:
                    return "INF";
                case Log::Level::Warning:
                    return "WRN";
                case Log::Level::Error:
                    return "ERR";
            }

            return "UNK";
        }
    }

    Log& Log::GetInstance()
    {
        static Log instance;
        return instance;
    }

    Log::Log()
    {
#if defined(_DEBUG) && defined(JUMPSPACEBALANCE_STANDALONE)
        AllocConsole();
        FILE* file = nullptr;
        freopen_s(&file, "CONOUT$", "w", stdout);
        SetConsoleTitleW(L"JumpSpaceBalance Mod - Developer mode");
#endif
    }

    void Log::LogLine(Level level, const std::string& message)
    {
#ifdef _DEBUG
        printf(JSB_LOG_BANNER_ "[%s] %s\n", StrFromLevel(level), message.c_str());
#endif
    }
}
