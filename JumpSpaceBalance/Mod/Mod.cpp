#include "pch.h"
#include "Mod.hpp"

namespace JSB
{
    Mod& Mod::GetInstance()
    {
        static Mod mod;
        return mod;
    }

    Mod::Mod()
    {
    }

    Mod::~Mod()
    {
        Finalize();
    }

    void Mod::Finalize()
    {
    }
}
