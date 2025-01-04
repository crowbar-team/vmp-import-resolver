#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace vmp::pe
{
    struct module_name_t
    {
        std::string name;

        [[nodiscard]] std::vector<std::uint8_t> bytes() const;
    };

    struct named_import_t
    {
        std::uint16_t hint;
        std::string name;

        [[nodiscard]] std::vector<std::uint8_t> bytes() const;
    };

    // only named imports supported right now
    struct thunk_data_x64_t
    {
        union
        {
            std::uint64_t forwarder_string;
            std::uint64_t function;
            std::uint64_t address;

        	struct
            {
                std::uint64_t ordinal : 16;
                std::uint64_t _reserved0 : 47;
                std::uint64_t is_ordinal : 1;
            };
        };

        [[nodiscard]] std::vector<std::uint8_t> bytes() const;
    };

    struct import_descriptor_t
    {
        union
        {
            std::uint32_t characteristics;
            std::uint32_t rva_original_first_thunk;
        };

        std::uint32_t timedate_stamp;
        std::uint32_t forwarder_chain;
        std::uint32_t rva_name;
        std::uint32_t rva_first_thunk;

        [[nodiscard]] std::vector<std::uint8_t> bytes() const;
    };
}