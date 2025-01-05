#pragma once

#include <string>

namespace transform
{
	std::string to_lower(std::string_view str);

	std::wstring to_lower(std::wstring_view str);

	std::wstring to_wide(std::string_view str);
}
