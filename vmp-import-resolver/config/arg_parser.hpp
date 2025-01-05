#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <filesystem>

namespace config::arg_parser
{
	std::expected<std::filesystem::path, std::string> parse(std::int32_t argc, const char** argv);
}
