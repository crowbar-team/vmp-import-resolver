#include "vmp.hpp"
#include "x86/disassembler.hpp"
#include "signature_scanner.hpp"

std::vector<std::uintptr_t> vmp::scan_import_calls(std::uintptr_t text_base, const std::vector<std::uint8_t>& text_section, const std::vector<std::pair<std::uintptr_t, std::size_t>>& vmp_sections)
{
	x86::disassembler_t disassembler;

	if (!disassembler.initialize())
	{
		return { };
	}

	std::vector<std::uintptr_t> import_calls = { };

	const std::vector<std::uint8_t> call_sig = { 0xE8, 0x00, 0x00, 0x00, 0x00 };

	signature_scanner::buffer(text_section.data(), text_section.size(), call_sig, [&vmp_sections, &disassembler, &text_base, &import_calls](const std::size_t offset, const std::uint8_t* address) -> void
	{
		std::uintptr_t call_address = text_base + offset;

		x86::disassembler_t::instruction_t instruction = { };

		if (!disassembler.decode(call_address, address, ZYDIS_MAX_INSTRUCTION_LENGTH, instruction))
		{
			return;
		}

		const std::uintptr_t target_address = instruction.absolute_address();

		if (!target_address)
		{
			return;
		}

		for (const auto& [vmp_section_address, vmp_section_size] : vmp_sections)
		{
			if (target_address >= vmp_section_address && target_address <= vmp_section_address + vmp_section_size)
			{
				import_calls.emplace_back(call_address);

				break;
			}
		}
	});

	return import_calls;
}
