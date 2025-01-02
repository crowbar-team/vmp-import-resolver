#include <cstdint>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "arg_parser.hpp"
#include "emulator/emulator.hpp"
#include "win_process/win_process.hpp"

#include "portable_executable/file.hpp"
#include "portable_executable/image.hpp"

std::int32_t main(const std::int32_t argc, const char** argv)
{
	const auto& context = arg_parser::parse(argc, argv);

	if (!context)
	{
		spdlog::error(context.error());

		return EXIT_FAILURE;
	}

	win_process_t win_process(context->process_id);

	if (!win_process.attach())
	{
		spdlog::error("failed to attach to process id {}", context->process_id);

		return EXIT_FAILURE;
	}

	const auto& module = win_process.find_module(context->module_name);

	if (!module)
	{
		spdlog::error(module.error());

		return EXIT_FAILURE;
	}

	portable_executable::file_t file(module->path);

	if (!file.load())
	{
		spdlog::error("failed to load file from path {}", module->path.string());

		return EXIT_FAILURE;
	}

	const auto& image = file.image();

	emulator_t emulator(UC_ARCH_X86, UC_MODE_64);

	try
	{
		emulator.initialize();
	}
	catch (std::runtime_error& error)
	{
		spdlog::error(error.what());

		return EXIT_FAILURE;
	}

	const portable_executable::section_header_t* text_section = image->find_section(".text");

	if (!text_section)
	{
		spdlog::error("failed to find section .text");

		return EXIT_FAILURE;
	}

	// map .text into emulator
	{
		std::vector<std::uint8_t> temp_buffer(text_section->virtual_size);

		const std::uintptr_t address = module->address + text_section->virtual_address;

		if (!win_process.read(address, temp_buffer.data(), temp_buffer.size()))
		{
			spdlog::error("failed to read from remote process");

			return EXIT_FAILURE;
		}

		try
		{
			emulator.map_memory(address, temp_buffer.data(), temp_buffer.size());
		}
		catch (std::runtime_error& error)
		{
			spdlog::error(error.what());

			return EXIT_FAILURE;
		}
	}

	// map vmp sections into emulator
	for (const auto& vmp_section : context->vmp_sections)
	{
		const portable_executable::section_header_t* section_header = image->find_section(vmp_section);

		if (!section_header)
		{
			spdlog::error("failed to find section {}", vmp_section);

			return EXIT_FAILURE;
		}

		std::vector<std::uint8_t> temp_buffer(section_header->virtual_size);

		const std::uintptr_t address = module->address + section_header->virtual_address;

		if (!win_process.read(address, temp_buffer.data(), temp_buffer.size()))
		{
			spdlog::error("failed to read from remote process");

			return EXIT_FAILURE;
		}

		try
		{
			emulator.map_memory(address, temp_buffer.data(), temp_buffer.size());
		}
		catch (std::runtime_error& error)
		{
			spdlog::error(error.what());

			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
