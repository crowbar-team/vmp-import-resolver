#include "arg_parser.hpp"

#include <argparse/argparse.hpp>

arg_parser::context_t arg_parser::parse(const std::int32_t argc, const char** argv)
{
	argparse::ArgumentParser program("vmp-import-resolver - crowbar.team");

    program.add_argument("-p", "--process_id")
        .required()
        .implicit_value(false)
        .help("process id");

    program.add_argument("-m", "--module_name")
        .required()
        .implicit_value(false)
        .help("name of module");

    program.add_argument("-s", "--sections")
        .required()
        .implicit_value(false)
        .help("VMProtect sections")
        .nargs(argparse::nargs_pattern::at_least_one);

    program.parse_args(argc, argv);

    context_t context = { };

    context.process_id = program.get<std::uint32_t>("--process_id");
    context.module_name = program.get<std::string>("--module_name");
    context.vmp_sections = program.get<std::vector<std::string>>("--sections");

    return context;
}
