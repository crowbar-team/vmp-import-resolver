#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace vmp::pe
{
    class module_name_t
    {
        std::string m_name;

    public:
        explicit module_name_t(std::string_view name);

        [[nodiscard]] std::vector<std::uint8_t> bytes() const;
    };

    class named_import_t
    {
        std::uint16_t m_hint;
        std::string m_name;

    public:
        explicit named_import_t(std::string_view name);

        [[nodiscard]] std::vector<std::uint8_t> bytes() const;
    };
}