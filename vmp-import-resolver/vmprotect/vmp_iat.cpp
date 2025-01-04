#include "vmp_iat.hpp"

void vmp::iat_t::add_import(const std::string& module_name, const std::string& function_name)
{
	if (const auto it = this->m_iat.find(module_name);
		it != this->m_iat.end())
	{
		if (auto& functions = it->second;
			std::ranges::find(functions, function_name) == functions.end())
		{
			functions.emplace_back(function_name);
		}
	}
	else
	{
		this->m_iat[module_name].emplace_back(function_name);
	}
}

std::size_t vmp::iat_t::size()
{
	return 0;
}
