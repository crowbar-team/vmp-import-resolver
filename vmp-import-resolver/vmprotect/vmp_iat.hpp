#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../portable_executable/imports_directory.hpp"

namespace vmp
{
	class image_t;

	class iat_t
	{
		// module name - vector of function names
		std::unordered_map<std::string, std::vector<std::string>> m_iat;

		std::vector<std::uint8_t> m_import_section;

	public:
		explicit iat_t() = default;

		void add_import(const std::string& module_name, const std::string& function_name);

		void reconstruct(image_t* image);
	};
}
