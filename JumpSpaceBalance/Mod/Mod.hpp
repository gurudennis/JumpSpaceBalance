#pragma once

#include "../Hooks/Hooks.hpp"

#include <atomic>
#include <optional>

namespace JSB
{
    class Mod
    {
    public:
        Mod();
        ~Mod();

    private:
        void SetUpHooks();

    private:
        static inline Mod* instance_{};
        std::optional<HookScope> hooks_;
    };
}
