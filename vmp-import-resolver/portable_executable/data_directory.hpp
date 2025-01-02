#pragma once

#include <cstddef>
#include <cstdint>

namespace portable_executable
{
    static constexpr std::size_t max_data_directories = 16;

    struct data_directory_t
    {
        std::uint32_t virtual_address;
        std::uint32_t size;

        [[nodiscard]] bool present() const;
    };

    struct data_directories_t
    {
        union
        {
            struct
            {
                data_directory_t export_directory;
                data_directory_t import_directory;
                data_directory_t resource_directory;
                data_directory_t exception_directory;
                data_directory_t security_directory;
                data_directory_t basereloc_directory;
                data_directory_t debug_directory;
                data_directory_t architecture_directory;
                data_directory_t globalptr_directory;
                data_directory_t tls_directory;
                data_directory_t load_config_directory;
                data_directory_t bound_import_directory;
                data_directory_t iat_directory;
                data_directory_t delay_import_directory;
                data_directory_t com_descriptor_directory;
                data_directory_t _reserved0;
            };

            data_directory_t entries[max_data_directories];
        };
    };
}