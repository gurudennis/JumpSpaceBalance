#include "pch.h"
#include "Mod.hpp"
#include "../Log/Log.hpp"

#include <IL2CPP_Resolver.hpp>

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

        SetUpHooks();
        
    }

    Mod::~Mod()
    {
        hooks_.reset();

        assert(instance_);
        instance_ = nullptr;
    }

    void Mod::SetUpHooks()
    {
        JSB_LOGINF("Setting up hooks...");
        hooks_.emplace();

        // ...

        if (hooks_->Apply())
        {
            JSB_LOGINF("Hooks set up successfully.");
        }
        else
        {
            JSB_LOGERR("Failed to set up one or more hooks!");
        }
    }
}
