#pragma once

#include "../Hooks/Hooks.hpp"

#include <optional>

namespace JSB
{
    class Mod
    {
    public:
        static Mod& GetInstance();

        void Finalize();

    private:
        Mod();
        ~Mod();

    private:
        std::optional<HookScope> hooks_;
    };
}
