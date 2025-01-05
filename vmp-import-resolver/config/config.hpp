#pragma once

#include <string>
#include <vector>
#include <expected>
#include <filesystem>

namespace config
{
	struct context_t
	{
		std::string process_name;
		std::string module_name;

		std::vector<std::string> vmp_sections;

		std::string iat_section_name;
		std::string stub_section_name;

		std::filesystem::path dump_path;
	};

	context_t parse_toml(const std::filesystem::path& fs_path);
}
