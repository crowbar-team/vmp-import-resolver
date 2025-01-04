#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../portable_executable/imports_directory.hpp"

namespace vmp
{
	class iat_t
	{
		// module name - vector of function names
		std::unordered_map<std::string, std::vector<std::string>> m_iat;

	public:
		explicit iat_t() = default;

		void add_import(const std::string& module_name, const std::string& function_name);

		std::size_t size();
	};
}
