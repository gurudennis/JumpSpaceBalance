#pragma once

#include <detours.h>

#include <vector>

namespace JSB
{
    class HookScope
    {
    public:
        HookScope() = default;
        ~HookScope()
        {
            DoTransaction([this]()
            {
                for (Hook& hook : hooks_)
                {
                    if (!hook.applied)
                    {
                        continue;
                    }

                    if (DetourDetach(hook.original, hook.hook) != NO_ERROR)
                    {
                        return false;
                    }
                }

                return true;
            });
        }

        template <typename TFunc>
        bool Add(TFunc hook, TFunc& original)
        {
            if (!hook || !original)
            {
                return false;
            }

            hooks_.emplace_back(Hook{
                .hook = static_cast<void*>(hook),
                .original = reinterpret_cast<void**>(&original)
            });

            return true;
        }

        bool Apply()
        {
            bool ok = DoTransaction([this]()
            {
                for (Hook& hook : hooks_)
                {
                    if (hook.applied)
                    {
                        continue;
                    }

                    if (DetourAttach(hook.original, hook.hook) != NO_ERROR)
                    {
                        return false;
                    }
                }

                return true;
            });

            if (ok)
            {
                for (Hook& hook : hooks_)
                {
                    hook.applied = true;
                }
            }

            return true;
        }

    private:
        struct Hook
        {
            void* hook{};
            void** original{};
            bool applied{};
        };

        template <typename TLambda>
        bool DoTransaction(TLambda&& lambda)
        {
            LONG res = DetourTransactionBegin();
            if (res != NO_ERROR)
            {
                return false;
            }

            if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR)
            {
                return false;
            }

            if (!lambda())
            {
                DetourTransactionAbort();
                return false;
            }

            return DetourTransactionCommit() != NO_ERROR;
        }

    private:
        std::vector<Hook> hooks_;
    };
}
