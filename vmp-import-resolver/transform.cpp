#include "transform.hpp"

#include <algorithm>

std::string transform::to_lower(const std::string_view str)
{
	std::string result(str);

	std::ranges::transform(result, result.begin(), ::tolower);

	return result;
}

std::wstring transform::to_lower(const std::wstring_view str)
{
	std::wstring result(str);

	std::ranges::transform(result, result.begin(), ::towlower);

	return result;
}

std::wstring transform::to_wide(std::string_view str)
{
	return { str.begin(), str.end() };
}
