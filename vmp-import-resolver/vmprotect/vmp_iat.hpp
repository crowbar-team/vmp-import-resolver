#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace vmp
{
	class iat_t
	{
		// module name - vector of function names
		std::unordered_map<std::string, std::vector<std::string>> m_iat;

	public:
		explicit iat_t() = default;

		void add_import(const std::string& module_name, const std::string& function_name);

	};
}
