#include "utilities.hpp"
#include "transform.hpp"

#include <format>
#include <Windows.h>
#include <TlHelp32.h>

std::expected<std::uint32_t, std::string> utilities::find_process_id(const std::string_view process_name)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    const std::wstring process_name_lower = transform::to_lower(transform::to_wide(process_name));

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
	        if (std::wstring entry_name_lower = transform::to_lower(entry.szExeFile);
                entry_name_lower.find(process_name_lower) != std::wstring::npos)
            {
                std::uint32_t process_id = entry.th32ProcessID;

                CloseHandle(snapshot);

                return process_id;
            }
        }
    }

    CloseHandle(snapshot);

    return std::unexpected(std::format("failed to find process id for {}", process_name));
}
