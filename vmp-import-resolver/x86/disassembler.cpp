#include "disassembler.hpp"

std::uintptr_t x86::disassembler_t::instruction_t::absolute_address() const
{
    std::uintptr_t absolute_address = 0;

	if (!ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(&this->info, this->operands, this->runtime_address, &absolute_address)))
	{
		return 0;
	}

    return absolute_address;
}

bool x86::disassembler_t::initialize()
{
    return ZYAN_SUCCESS(ZydisDecoderInit(&this->m_decoder, ZydisMachineMode::ZYDIS_MACHINE_MODE_LONG_64, ZydisStackWidth::ZYDIS_STACK_WIDTH_64));
}

bool x86::disassembler_t::decode(const std::uintptr_t runtime_address, const void* buffer, const std::size_t length, instruction_t& instruction) const
{
    instruction.runtime_address = runtime_address;

    ZydisDecoderContext ctx;

    return  ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&this->m_decoder, &ctx, buffer, length, &instruction.info)) &&
        ZYAN_SUCCESS(ZydisDecoderDecodeOperands(&this->m_decoder, &ctx, &instruction.info, instruction.operands, instruction.info.operand_count));
}