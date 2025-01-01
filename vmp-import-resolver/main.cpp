#include <cstdint>
#include <iostream>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "arg_parser.hpp"
#include "win_process/win_process.hpp"

std::int32_t main(const std::int32_t argc, const char** argv)
{
	arg_parser::context_t context;

	try
	{
		context = arg_parser::parse(argc, argv);
	}
	catch (std::runtime_error& error)
	{
		spdlog::error("failed to parse arguments with error: {}", error.what());

		return EXIT_FAILURE;
	}

	win_process_t win_process(context.process_id);

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

	return EXIT_SUCCESS;
}
