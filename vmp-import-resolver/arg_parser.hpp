#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace arg_parser
{
	struct context_t
	{
		std::uint32_t process_id;
		std::string module_name;
		std::vector<std::string> vmp_sections;
	};

	context_t parse(std::int32_t argc, const char** argv);
}
