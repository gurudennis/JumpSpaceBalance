#include "pch.h"
#include "Mod.hpp"
#include "Utils.hpp"
#include "Log/Log.hpp"

#include <IL2CPP_Resolver.hpp>

#include <cassert>
#include <codecvt>
#include <vector>

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

    namespace TweakableValues
    {
        struct Signature
        {
            float minV{};
            float maxV{};
            float perLevel{};
            int32_t idx{};
            float boundary{};

            bool operator==(const Signature& other) const
            {
                return minV == other.minV && maxV == other.maxV &&
                    perLevel == other.perLevel && idx == other.idx &&
                    boundary == other.boundary;
            }
        };

        struct Modifier
        {
            const wchar_t* name{};
            std::optional<Signature> signature{};
            float modifier{};
            bool honorMinMax{};
        };
        static constexpr Modifier Modifiers[] =
        {
            // Chain (both on-foot and ship)
            Modifier{ L"Amount of bounces", std::nullopt, 0.01f, false }, // basically always 1 regardless of level
            Modifier{ L"Amount of damage per jump", std::nullopt, 0.33f, false },

            // Frag (on-foot)
            Modifier{ L"Projectiles", std::nullopt, 0.33f, false }, // also: L"Damage reduction", L"Spread"

            // Frag (ship)
            Modifier{ L"Pelletss"/*(sic)*/, std::nullopt, 0.33f, false }, // also: L"Spread", L"Damage modifier"

            // Random status effect (on-foot)
            Modifier{ L"Proc chance", std::nullopt, 0.1f, false },

            // Engine max speed
            Modifier{ L"Downtime Percentage Change"/*(sic)*/, Signature{ 0.125f, 0.0f, 0.12f, 0, 0.025f }, 0.5f, false },

            // Materia cost reduction
            Modifier{ L"Cost reduction", std::nullopt, 0.5f, false },
        };
        StringMap<wchar_t, std::vector<Modifier>> ModifierMap;

        void MakeModifierMap()
        {
            if (!ModifierMap.empty())
            {
                return;
            }

            ModifierMap.reserve(std::size(Modifiers));
            for (const Modifier& modifier : Modifiers)
            {
                ModifierMap[modifier.name].emplace_back(modifier);
            }
        }

        Signature ReadSignature(const void* ths)
        {
            return Signature
            {
                Read<float>(ths, 0x8),
                Read<float>(ths, 0xc),
                Read<float>(ths, 0x10),
                Read<int32_t>(ths, 0x14),
                Read<float>(ths, 0x18)
            };
        }
    }

    Mod::Mod()
    {
        assert(!instance_);
        instance_ = this;

        JSB_LOGINF("Initializing IL2CPP...");
        IL2CPP::Initialize();
        JSB_LOGINF("IL2CPP initialized.");

        JSB_LOGINF("Pre-caching values...");
        TweakableValues::MakeModifierMap();
        JSB_LOGINF("Values pre-cached.");

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

        const wchar_t* const name = ReadStr(ths, 0x0);
        if (!name || !*name)
        {
            return res;
        }

        const TweakableValues::Signature signature = TweakableValues::ReadSignature(ths);

#ifdef JSB_PRINT_DEBUG_INFO
        JSB_LOGINF("ItemModuleTweakableValue_CalculateRolledValue({:#x}, {}, {}) = {}: n={}, mm={}-{}, p={}, i={}, r={}",
            uint64_t(ths), roll, upgradeLevel, res, ToNarrow(name), signature.minV, signature.maxV,
            signature.perLevel, signature.idx, signature.boundary);
#endif

        auto iter = TweakableValues::ModifierMap.find(name);
        if (iter == TweakableValues::ModifierMap.end())
        {
            return res;
        }

        for (const TweakableValues::Modifier& modifier : iter->second)
        {
            if (modifier.signature && signature != *modifier.signature)
            {
                continue;
            }

            const float minV = modifier.honorMinMax ? signature.minV : -1.0f;
            const float maxV = modifier.honorMinMax ? signature.maxV : -1.0f;
            res = FixModifier(res * modifier.modifier, minV, maxV, signature.boundary);

            break;
        }

        return res;
    }
}
