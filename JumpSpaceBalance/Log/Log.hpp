#pragma once

#include <string>
#include <format>

namespace JSB
{
    class Log
    {
    public:
        enum class Level
        {
            Info,
            Warning,
            Error
        };

        static Log& GetInstance();

        void LogLine(Level level, const std::string& message);

    private:
        Log();
    };
}

#ifdef _DEBUG
    #define JSB_LOG(level__, ...) \
        JSB::Log::GetInstance().LogLine(level__, std::format(__VA_ARGS__))
#else
    #define JSB_LOG(level__, ...) \
        JSB::Log::GetInstance().LogLine(level__, std::string{})
#endif

#define JSB_LOGINF(...) JSB_LOG(JSB::Log::Level::Info,    __VA_ARGS__)
#define JSB_LOGWRN(...) JSB_LOG(JSB::Log::Level::Warning, __VA_ARGS__)
#define JSB_LOGERR(...) JSB_LOG(JSB::Log::Level::Error,   __VA_ARGS__)
