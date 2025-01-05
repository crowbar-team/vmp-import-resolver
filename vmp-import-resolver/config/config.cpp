#include "config.hpp"

#include <toml++/toml.hpp>

config::context_t config::parse_toml(const std::filesystem::path& fs_path)
{
    context_t context = { };

    const toml::table table = toml::parse_file(fs_path.string());

    {
        const auto process_information = table["process_information"];

        if (const auto process_name = process_information["process_name"].value<std::string>())
        {
            context.process_name = *process_name;
        }
        else
        {
            throw std::runtime_error("process_information!process_name was not specified");
        }

        if (const auto module_name = process_information["module_name"].value<std::string>())
        {
            context.module_name = *module_name;
        }
        else
        {
            throw std::runtime_error("process_information!module_name was not specified");
        }
    }

    {
        const auto vmprotect = table["vmprotect"];

        if (auto sections_array = vmprotect["sections"].as_array())
        {
            for (const auto& name : *sections_array)
            {
                if (auto str_value = name.as_string())
                {
                    context.vmp_sections.emplace_back(*str_value);
                }
            	else
                {
                    throw std::runtime_error("invalid section name was specified");
                }
            }
        }
        else
        {
            throw std::runtime_error("vmprotect!sections was not specified");
        }
    }

    {
        const auto new_sections = table["new_sections"];

        if (const auto iat = new_sections["iat"].value<std::string>())
        {
            context.iat_section_name = *iat;
        }
        else
        {
            throw std::runtime_error("new_sections!iat was not specified");
        }

        if (const auto stub = new_sections["stub"].value<std::string>())
        {
            context.stub_section_name = *stub;
        }
        else
        {
            throw std::runtime_error("new_sections!stub was not specified");
        }
    }

    {
        const auto dump = table["dump"];

        if (const auto path = dump["path"].value<std::string>())
        {
            context.dump_path = std::filesystem::path(*path);
        }
        else
        {
            throw std::runtime_error("dump!path was not specified");
        }
    }

    return context;
}
