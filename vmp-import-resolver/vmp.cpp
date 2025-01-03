#include "vmp.hpp"
#include <asmjit/asmjit.h>

#include <format>
#include <stdexcept>
#include <stack>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

struct import_stub_data_t
{
	std::uintptr_t real_import_address;
	std::uintptr_t address_calling_import;
};

struct user_data_t
{
	std::stack<import_stub_data_t> stubs_needed;
};

static void code_hook(uc_engine* uc, const std::uintptr_t address, const std::uint32_t size, user_data_t* user_data)
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

			import_stub_data_t& stub_data = user_data->stubs_needed.top();

			stub_data.real_import_address = return_address;

			spdlog::info("resolved import at 0x{:X} to 0x{:X}, fallback to 0x{:X}", stub_data.address_calling_import, return_address, vmp::context->emulator->read_stack(8));
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

std::vector<std::uint8_t> get_stub_assembler(std::uint64_t import_to_call, std::uint32_t stack_offset)
{
	asmjit::JitRuntime jit_runtime;
	asmjit::CodeHolder code_holder;

	code_holder.init(jit_runtime.environment());
	asmjit::x86::Assembler assembler(&code_holder);

	assembler.add(asmjit::x86::rsp, asmjit::imm(stack_offset));
	assembler.jmp(import_to_call);

	std::uint8_t* jit = nullptr;

	if (jit_runtime.add(&jit, &code_holder) != 0)
	{
		return { };
	}

	return std::vector<std::uint8_t>(jit, jit + code_holder.codeSize());
}

void vmp::process_import_calls(const std::vector<std::uintptr_t>& import_calls, std::uintptr_t module_base, std::vector<std::uint8_t> dumped_binary)
{
	user_data_t user_data = { };

	context->emulator->add_hook(UC_HOOK_CODE, &user_data, reinterpret_cast<void*>(code_hook));

	for (const auto& import_call : import_calls)
	{
		user_data.stubs_needed.push({ .real_import_address = import_call });

		spdlog::info("starting emulation for possible import call at 0x{0:X}...", import_call);

		context->emulator->start(import_call);
	}

	portable_executable::image_t* binary_image = reinterpret_cast<portable_executable::image_t*>(dumped_binary.data());

	std::uint64_t stub_size = get_stub_assembler(binary_image->as<std::uintptr_t>(), 8).size();

	std::uint64_t stub_section_size = stub_size * user_data.stubs_needed.size();

	spdlog::info("secondary iat stub section size 0x{0:X}", stub_section_size);

	// rwx
	dumped_binary = binary_image->add_section(".stub", static_cast<std::uint32_t>(stub_section_size), 0x40);

	while (user_data.stubs_needed.empty() == false)
	{
		import_stub_data_t& stub_data = user_data.stubs_needed.top();

		// call stub at address_calling_import
		// stub must call real_import_address
		// and fix up stack misalignment

		user_data.stubs_needed.pop();
	}
}
