#pragma once

#include <cstdint>

#include "nt_headers.hpp"

namespace portable_executable
{
    static constexpr std::uint16_t dos_magic = 0x5A4D;

    struct dos_header_t
    {
        std::uint16_t e_magic;
        std::uint16_t e_cblp;
        std::uint16_t e_cp;
        std::uint16_t e_crlc;
        std::uint16_t e_cparhdr;
        std::uint16_t e_minalloc;
        std::uint16_t e_maxalloc;
        std::uint16_t e_ss;
        std::uint16_t e_sp;
        std::uint16_t e_csum;
        std::uint16_t e_ip;
        std::uint16_t e_cs;
        std::uint16_t e_lfarlc;
        std::uint16_t e_ovno;
        std::uint16_t e_res[4];
        std::uint16_t e_oemid;
        std::uint16_t e_oeminfo;
        std::uint16_t e_res2[10];
        std::uint32_t e_lfanew;

        [[nodiscard]] bool valid() const;

        nt_headers_t* nt_headers();

        [[nodiscard]] const nt_headers_t* nt_headers() const;
    };
}