#pragma once

#include <cstdint>
#include <utility>
#include <vector>

namespace vmp
{
	std::vector<std::uintptr_t> scan_import_calls(std::uintptr_t text_base, const std::vector<std::uint8_t>& text_section, const std::vector<std::pair<std::uintptr_t, std::size_t>>& vmp_sections);
}
