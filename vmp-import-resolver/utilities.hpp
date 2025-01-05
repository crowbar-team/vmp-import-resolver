#pragma once

#include <cstdint>
#include <string>
#include <expected>

namespace utilities
{
	std::expected<std::uint32_t, std::string> find_process_id(std::string_view process_name);
}
