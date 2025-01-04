#include "vmp.hpp"

#include <format>
#include <stdexcept>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

static void code_hook(uc_engine* uc, const std::uintptr_t address, const std::uint32_t size, void* user_data)
{
	std::uint8_t instruction_buffer[ZYDIS_MAX_INSTRUCTION_LENGTH] = { };

	if (const uc_err err = uc_mem_read(uc, address, instruction_buffer, size);
		err != UC_ERR_OK)
	{
		return;
	}

	x86::disassembler_t::instruction_t instruction = { };

	if (!vmp::context->disassembler->decode(address, instruction_buffer, size, instruction))
	{
		return;
	}

	if (instruction.info.mnemonic == ZYDIS_MNEMONIC_RET)
	{
		const std::uintptr_t return_address = vmp::context->emulator->read_stack();

		bool leads_into_vmp_section = false;

		for (const auto& [vmp_section_address, vmp_section_size] : vmp::context->sections)
		{
			if (return_address >= vmp_section_address && return_address <= vmp_section_address + vmp_section_size)
			{
				leads_into_vmp_section = true;

				break;
			}
		}

		if (!leads_into_vmp_section)
		{
			vmp::context->emulator->stop();

			std::uintptr_t calling_address = *static_cast<std::uintptr_t*>(user_data);

			// fallback address is located right after return address in the stack
			std::uintptr_t fallback_address = vmp::context->emulator->read_stack(sizeof(std::uintptr_t));

			std::uintptr_t stack_displacement = vmp::context->emulator->stack_displacement();

			vmp::context->imports.emplace_back(calling_address, return_address, fallback_address, stack_displacement);

			spdlog::info("resolved import at 0x{:X} to 0x{:X}, fallback to 0x{:X}, stack displacement 0x{:X}", calling_address, return_address, fallback_address, stack_displacement);
		}
	}
}

void vmp::construct_context(const bool is_x64)
{
    context = std::make_unique<context_t>();

    context->disassembler = std::make_unique<x86::disassembler_t>(	is_x64 ? ZYDIS_MACHINE_MODE_LONG_64 : ZYDIS_MACHINE_MODE_LONG_COMPAT_32,
																	is_x64 ? ZYDIS_STACK_WIDTH_64 : ZYDIS_STACK_WIDTH_32);
    context->emulator = std::make_unique<emulator_t>(UC_ARCH_X86,	is_x64 ? UC_MODE_64 : UC_MODE_32);
}

void vmp::compute_sections(const std::vector<std::string>& vmp_sections, const std::uintptr_t module_base, const portable_executable::image_t* image)
{
	for (const auto& vmp_section : vmp_sections)
	{
		const portable_executable::section_header_t* section_header = image->find_section(vmp_section);

		if (!section_header)
		{
			throw std::runtime_error(std::format("failed to find section {}", vmp_section));
		}

		const std::uintptr_t address = module_base + section_header->virtual_address;

		context->sections.emplace_back(address, section_header->virtual_size);
	}
}

void vmp::map_sections(const std::vector<std::pair<std::uintptr_t, std::vector<std::uint8_t>>>& sections)
{
	for (const auto& [address, buffer] : sections)
	{
		context->emulator->map_memory(address, buffer.data(), buffer.size());
	}
}

void vmp::process_import_calls(const std::vector<std::uintptr_t>& import_calls, std::uintptr_t module_base)
{
	std::uintptr_t calling_address = 0;

	context->emulator->add_hook(UC_HOOK_CODE, &calling_address, reinterpret_cast<void*>(code_hook));

	for (const auto& import_call : import_calls)
	{
		calling_address = import_call;

		spdlog::info("starting emulation for possible import call at 0x{0:X}...", import_call);

		try
		{
			context->emulator->start(import_call);
		}
		catch (std::runtime_error& error)
		{
			spdlog::error(error.what());
		}
	}
}
