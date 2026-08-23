#include "pch.h"
#include "Mod.hpp"
#include "../Log/Log.hpp"

#include <IL2CPP_Resolver.hpp>

#include <cassert>
#include <codecvt>

// Uncomment this to print debug info
// #define JSB_PRINT_DEBUG_INFO

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

        template <typename T>
        T Read(const void* addr, size_t offset = 0)
        {
            return *reinterpret_cast<const T*>(static_cast<const std::byte*>(addr) + offset);
        }

        const wchar_t* ReadStr(const void* addr, size_t offset = 0)
        {
            const Unity::System_String* str = *(const Unity::System_String**)(static_cast<const std::byte*>(addr) + offset);
            return str->m_wString;
        }

        std::string ToNarrow(const std::wstring& wstr) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            return converter.to_bytes(wstr);
        }

        float FixModifier(float value, float minValue, float maxValue, float boundary)
        {
            if (boundary > 0.0f)
            {
                value = std::ceil(value / boundary) * boundary;
            }

            if (minValue >= 0.0f && value < minValue)
            {
                value = minValue;
            }

            if (maxValue >= 0.0f && value > maxValue)
            {
                value = maxValue;
            }

            return value;
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
        float res = Hooks::O_ItemModuleTweakableValue_CalculateRolledValue(ths, roll, upgradeLevel);

#ifdef JSB_PRINT_DEBUG_INFO
        JSB_LOGINF("ItemModuleTweakableValue_CalculateRolledValue({:#x}, {}, {}) = {}: n={}, mm={}-{}, p={}, i={}, r={}",
            uint64_t(ths), roll, upgradeLevel, res, ToNarrow(ReadStr(ths, 0x0)), Read<float>(ths, 0x8),
            Read<float>(ths, 0xc), Read<float>(ths, 0x10), Read<int32_t>(ths, 0x14), Read<float>(ths, 0x18));
#endif

        struct Modifier
        {
            const wchar_t* name{};
            float modifier{};
            bool honorMinMax{};
        };
        static Modifier modifiers[] =
        {
            // Chain (both on-foot and ship)
            Modifier{ L"Amount of bounces", 0.33, true },
            Modifier{ L"Amount of damage per jump", 0.33, false },

            // Frag (on-foot)
            Modifier{ L"Projectiles", 0.5, false }, // also: L"Damage reduction", L"Spread"

            // Frag (ship)
            Modifier{ L"Pelletss"/*(sic)*/, 0.5, false }, // also: L"Spread", L"Damage modifier"
        };

        const wchar_t* name = ReadStr(ths, 0x0);
        if (*name == L'A' || *name == L'P') // optimization that happens to hold for now (!)
        {
            for (const Modifier& modifier : modifiers)
            {
                if (wcscmp(name, modifier.name) == 0)
                {
                    const float minV = modifier.honorMinMax ? Read<float>(ths, 0x8) : -1.0f;
                    const float maxV = modifier.honorMinMax ? Read<float>(ths, 0xc) : -1.0f;
                    const float boundary = Read<float>(ths, 0x18);
                    res = FixModifier(res * modifier.modifier, minV, maxV, boundary);
                    break;
                }
            }
        }

        return res;
    }
}
