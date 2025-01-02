#pragma once

#include <cstdint>
#include <cstddef>
#include <Zydis/Zydis.h>

namespace x86
{
    class disassembler_t
    {
        ZydisDecoder m_decoder = { };

    public:
        struct instruction_t
        {
        	std::uintptr_t runtime_address;
            ZydisDecodedInstruction info;
            ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

            [[nodiscard]] std::uintptr_t absolute_address() const;
        };

        bool initialize();

        bool decode(std::uintptr_t runtime_address, const void* buffer, std::size_t length, instruction_t& instruction) const;
    };
}