#include "win_process.hpp"
#include "../transform.hpp"

#include <Windows.h>
#include <Psapi.h>

win_process_t::win_process_t(const std::uint32_t process_id) : m_process_id(process_id)
{

}

win_process_t::~win_process_t()
{
	this->detach();
}

bool win_process_t::attach()
{
	return (this->m_handle = OpenProcess(PROCESS_ALL_ACCESS, false, this->m_process_id)) != nullptr;
}

void win_process_t::detach() const
{
	if (this->m_handle)
	{
		CloseHandle(this->m_handle);
	}
}

bool win_process_t::read(const std::uintptr_t address, void* buffer, const std::size_t size) const
{
	return ReadProcessMemory(this->m_handle, reinterpret_cast<void*>(address), buffer, size, nullptr);
}

bool win_process_t::write(const std::uintptr_t address, const void* buffer, const std::size_t size) const
{
	return WriteProcessMemory(this->m_handle, reinterpret_cast<void*>(address), buffer, size, nullptr);
}

std::expected<std::vector<win_process_t::win_module_t>, std::uint32_t> win_process_t::modules() const
{
	std::vector<HMODULE> modules(64);

	const auto modules_size = [&modules]() -> DWORD
	{
		return static_cast<DWORD>(modules.size() * sizeof(HMODULE));
	};

	DWORD bytes_needed = 0;

	if (!EnumProcessModulesEx(this->m_handle, modules.data(), modules_size(), &bytes_needed, LIST_MODULES_ALL))
	{
		return std::unexpected(GetLastError());
	}
	else if (bytes_needed > modules_size())
	{
		modules.resize(bytes_needed / sizeof(HMODULE));

		if (!EnumProcessModulesEx(this->m_handle, modules.data(), modules_size(), &bytes_needed, LIST_MODULES_ALL))
		{
			return std::unexpected(GetLastError());
		}
	}

	std::vector<win_module_t> win_modules = { };

	for (std::size_t i = 0; i < bytes_needed / sizeof(HMODULE); i++)
	{
		if (wchar_t module_path[MAX_PATH] = { };
			GetModuleFileNameEx(this->m_handle, modules[i], module_path,sizeof(module_path) / sizeof(wchar_t)))
		{
			MODULEINFO info = { };

			GetModuleInformation(this->m_handle, modules[i], &info, sizeof(info));

			win_modules.emplace_back(std::filesystem::path(module_path), reinterpret_cast<std::uintptr_t>(info.lpBaseOfDll), info.SizeOfImage);
		}
	}

	return win_modules;
}

std::expected<win_process_t::win_module_t, std::string> win_process_t::find_module(const std::string_view module_name) const
{
	const auto& modules = this->modules();

	if (!modules)
	{
		return std::unexpected(std::format("failed to get module list with last error {}", modules.error()));
	}

	const std::string module_name_lower = transform::to_lower(module_name);

	for (const auto& module : *modules)
	{
		if (transform::to_lower(module.path.string()).find(module_name_lower) != std::string::npos)
		{
			return module;
		}
	}

	return std::unexpected(std::format("failed to find module {} in process {}", module_name, this->m_process_id));
}
