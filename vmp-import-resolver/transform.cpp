#include "transform.hpp"

#include <algorithm>

std::string transform::to_lower(const std::string_view str)
{
	std::string result(str);

	std::ranges::transform(result, result.begin(), ::tolower);

	return result;
}