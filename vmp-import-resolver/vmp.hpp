#pragma once

#include <memory>
#include <vector>
#include <utility>

#include "x86/disassembler.hpp"
#include "emulator/emulator.hpp"

#include "portable_executable/image.hpp"

namespace vmp
{
	struct context_t
	{
		std::unique_ptr<x86::disassembler_t> disassembler;
		std::unique_ptr<emulator_t> emulator;

		std::vector<std::pair<std::uintptr_t, std::size_t>> sections;
	};

	inline std::unique_ptr<context_t> context;

	void construct_context(bool is_x64 = true);

	void compute_sections(const std::vector<std::string>& vmp_sections, std::uintptr_t module_base, const portable_executable::image_t* image);

	void map_sections(const std::vector<std::pair<std::uintptr_t, std::vector<std::uint8_t>>>& sections);

	void process_import_calls(const std::vector<std::uintptr_t>& import_calls, std::uintptr_t module_base, std::vector<std::uint8_t> dumped_binary);
}
