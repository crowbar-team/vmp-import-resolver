#include "arg_parser.hpp"

#include <argparse/argparse.hpp>

std::expected<std::filesystem::path, std::string> config::arg_parser::parse(std::int32_t argc, const char** argv)
{
    argparse::ArgumentParser program("vmp-import-resolver - crowbar.team");

    std::string fs_path;

    program.add_argument("-p", "--path")
        .required().store_into(fs_path)
        .help("path to config file");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (std::runtime_error& error)
    {
        return std::unexpected(std::format("failed to parse arguments: {}", error.what()));
    }

    return fs_path;
}
