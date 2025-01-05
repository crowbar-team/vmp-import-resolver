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

    // only named imports supported right now
    class thunk_data_x64_t
    {
        union
        {
            std::uint64_t forwarder_string;
            std::uint64_t function;
            std::uint64_t address;

        	struct  // NOLINT(clang-diagnostic-nested-anon-types)
            {
	            // ReSharper disable once CppInconsistentNaming
	            std::uint64_t ordinal : 16;
	            // ReSharper disable once CppInconsistentNaming
	            std::uint64_t _reserved0 : 47;
	            // ReSharper disable once CppInconsistentNaming
	            std::uint64_t is_ordinal : 1;
            };
        };

    public:
        explicit thunk_data_x64_t(std::uint64_t address);

        [[nodiscard]] std::vector<std::uint8_t> bytes() const;
    };

    class import_descriptor_t
    {
        std::uint32_t m_rva_original_first_thunk;  // NOLINT(clang-diagnostic-unused-private-field)
        std::uint32_t m_time_date_stamp = 0;  // NOLINT(clang-diagnostic-unused-private-field)
        std::uint32_t m_forwarder_chain = 0;  // NOLINT(clang-diagnostic-unused-private-field)
        std::uint32_t m_rva_name;  // NOLINT(clang-diagnostic-unused-private-field)
        std::uint32_t m_rva_first_thunk;  // NOLINT(clang-diagnostic-unused-private-field)

    public:
        explicit import_descriptor_t(std::uint32_t thunk_rva, std::uint32_t name_rva);

        [[nodiscard]] std::vector<std::uint8_t> bytes() const;
    };
}