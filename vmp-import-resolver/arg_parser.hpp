#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <expected>

namespace arg_parser
{
	struct context_t
	{
		std::uint32_t process_id;
		std::string module_name;
		std::vector<std::string> vmp_sections;
	};

	std::expected<context_t, std::string> parse(std::int32_t argc, const char** argv);
}
