#pragma once

#include <cstdint>
#include <utility>
#include <vector>

namespace vmp_utilities
{
	std::vector<std::uintptr_t> scan_import_calls(std::uintptr_t text_base, const std::vector<std::uint8_t>& text_section);
}
