#include "pch.h"
#include "Mod.hpp"

#include <cassert>

namespace JSB
{
    namespace Hooks
    {
        // ...
    }

    Mod::Mod()
    {
        assert(!instance_);
        instance_ = this;

        // Set up the hooks
        hooks_.emplace();
        SetUpHooks();
        hooks_->Apply();
    }

    Mod::~Mod()
    {
        // Tear down the hooks
        hooks_.reset();

        assert(instance_);
        instance_ = nullptr;
    }

    void Mod::SetUpHooks()
    {
        // ...
    }
}
