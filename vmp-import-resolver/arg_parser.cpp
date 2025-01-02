#include "arg_parser.hpp"

#include <argparse/argparse.hpp>

std::expected<arg_parser::context_t, std::string> arg_parser::parse(const std::int32_t argc, const char** argv)
{
	argparse::ArgumentParser program("vmp-import-resolver - crowbar.team");

    context_t context = { };

    program.add_argument("-p", "--process_id")
        .required().store_into(context.process_id)
        .help("process id");

    program.add_argument("-m", "--module_name")
        .required().store_into(context.module_name)
        .help("name of module");

    program.add_argument("-s", "--sections")
        .required()
        .nargs(argparse::nargs_pattern::at_least_one)
		.store_into(context.vmp_sections)
        .help("VMProtect sections");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (std::runtime_error& error)
    {
        return std::unexpected(std::format("failed to parse arguments: {}", error.what()));
    }

    return context;
}
