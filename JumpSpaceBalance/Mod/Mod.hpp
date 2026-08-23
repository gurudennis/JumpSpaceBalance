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

        // Hook infrastructure
        static Mod& GetInstance();
        float Hook_ItemModuleTweakableValue_CalculateRolledValue(void* ths, float roll, int32_t upgradeLevel);

    private:
        void SetUpHooks();
        void HookFunctions();

    private:
        static inline Mod* instance_{};
        std::optional<HookScope> hooks_;
    };
}
