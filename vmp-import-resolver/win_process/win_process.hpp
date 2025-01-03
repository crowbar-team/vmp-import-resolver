#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <vector>

class win_process_t
{
	std::uint32_t m_process_id = 0;

	void* m_handle = nullptr;

public:
	explicit win_process_t(std::uint32_t process_id);

	~win_process_t();

	bool attach();

	void detach() const;

	bool read(std::uintptr_t address, void* buffer, std::size_t size) const;

	bool write(std::uintptr_t address, const void* buffer, std::size_t size) const;

	std::vector<std::uint8_t> dump(std::string_view module_name) const;

	struct win_module_t
	{
		std::filesystem::path path;
		std::uintptr_t address;
		std::size_t size;
	};

	[[nodiscard]] std::expected<std::vector<win_module_t>, std::uint32_t> modules() const;

	[[nodiscard]] std::expected<win_module_t, std::string> find_module(std::string_view module_name) const;
};