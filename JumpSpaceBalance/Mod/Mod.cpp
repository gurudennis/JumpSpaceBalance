#include "pch.h"
#include "Mod.hpp"
#include "../Log/Log.hpp"

#include <IL2CPP_Resolver.hpp>

#include <cassert>

namespace JSB
{
    namespace
    {
        template <typename TFunc>
        bool HookILFunction(HookScope& hooks, const char* cls, const char* func, TFunc hook, TFunc& original)
        {
            void* address = IL2CPP::Class::Utils::GetMethodPointer(cls, func);
            if (!address)
            {
                JSB_LOGERR("Failed to find address of class=\"{}\" function=\"{}\"!", cls, func);
                return false;
            }

            original = static_cast<TFunc>(address);

            if (!hooks.Add(hook, original))
            {
                JSB_LOGERR("Failed stage the hook of class=\"{}\" function=\"{}\" at address={:#x}!", cls, func, uint64_t(address));
            }

            JSB_LOGINF("Hooking class=\"{}\" function=\"{}\" at address={:#x}", cls, func, uint64_t(address));

            return true;
        }
    }

    namespace Hooks
    {
        // Keepsake.HyperSpace.System.Modifiers.ItemModule.ItemModuleTweakableValue.CalculateRolledValue
        typedef float(__fastcall* P_ItemModuleTweakableValue_CalculateRolledValue)(void* ths, float roll, int32_t upgradeLevel);
        P_ItemModuleTweakableValue_CalculateRolledValue O_ItemModuleTweakableValue_CalculateRolledValue = nullptr;
        float __fastcall ItemModuleTweakableValue_CalculateRolledValue(void* ths, float roll, int32_t upgradeLevel)
        {
            return Mod::GetInstance().Hook_ItemModuleTweakableValue_CalculateRolledValue(ths, roll, upgradeLevel);
        }
    }

    Mod::Mod()
    {
        assert(!instance_);
        instance_ = this;

        JSB_LOGINF("Initializing IL2CPP...");
        IL2CPP::Initialize();
        JSB_LOGINF("IL2CPP initialized.");

        SetUpHooks();
        
    }

    Mod::~Mod()
    {
        hooks_.reset();

        assert(instance_);
        instance_ = nullptr;
    }

    Mod& Mod::GetInstance()
    {
        assert(instance_);
        return *instance_;
    }

    void Mod::SetUpHooks()
    {
        JSB_LOGINF("Setting up hooks...");
        hooks_.emplace();

        HookFunctions();

        if (hooks_->Apply())
        {
            JSB_LOGINF("Hooks set up successfully.");
        }
        else
        {
            JSB_LOGERR("Failed to set up one or more hooks!");
        }
    }

    void Mod::HookFunctions()
    {
        HookILFunction(*hooks_, "Keepsake.HyperSpace.System.Modifiers.ItemModule.ItemModuleTweakableValue", "CalculateRolledValue",
            Hooks::ItemModuleTweakableValue_CalculateRolledValue, Hooks::O_ItemModuleTweakableValue_CalculateRolledValue);
    }

    float Mod::Hook_ItemModuleTweakableValue_CalculateRolledValue(void* ths, float roll, int32_t upgradeLevel)
    {
        const float res = Hooks::O_ItemModuleTweakableValue_CalculateRolledValue(ths, roll, upgradeLevel);
        JSB_LOGINF("ItemModuleTweakableValue_CalculateRolledValue({:#x}, {}, {}) = {}", uint64_t(ths), roll, upgradeLevel, res);
        return res;
    }
}
