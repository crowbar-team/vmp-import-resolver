#include <cstdint>
#include <iostream>

#include "win_process/win_process.hpp"

std::int32_t main(std::int32_t argc, const char** argv)
{
	win_process_t win_process(8716);

	if (!win_process.attach())
	{
		return 1;
	}

	if (const auto& modules = win_process.modules())
	{
		std::cout << modules->size() << '\n';

		for (const auto& [fs_path, image_base, size] : *modules)
		{
			std::cout << fs_path << '\n';
		}
	}
	else
	{
		std::cout << modules.error() << '\n';
	}

	return 0;
}
