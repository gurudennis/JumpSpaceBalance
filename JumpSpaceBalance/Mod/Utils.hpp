#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace JSB
{
    template <typename TChar>
    struct StringHash
    {
        using is_transparent = void;
        using StrV = std::basic_string_view<TChar>;
        using Str = std::basic_string<TChar>;

        size_t operator()(StrV sv) const
        {
            return std::hash<StrV>{}(sv);
        }
        size_t operator()(const Str& s) const
        {
            return std::hash<Str>{}(s);
        }
        size_t operator()(const TChar* str) const
        {
            return std::hash<StrV>{}(str);
        }
    };

    template <typename TChar, typename TValue>
    using StringMap = std::unordered_map<std::basic_string<TChar>, TValue, StringHash<wchar_t>, std::equal_to<>>;
}
