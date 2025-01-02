#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>
#include <unicorn/unicorn.h>

class emulator_t
{
	uc_arch m_arch;
	uc_mode m_mode;
	uc_x86_reg m_rsp_reg;

	uc_engine* m_engine = nullptr;

	std::vector<uc_hook> m_hooks;

	std::uintptr_t m_stack_end = 0;

	void terminate() const;

public:
	explicit emulator_t(uc_arch arch, uc_mode mode);

	~emulator_t();

	void map_memory(std::uintptr_t address, const void* buffer, std::size_t size) const;

	void add_hook(uc_hook_type hook_type, void* user_data, void* callback_fn);

	void start(std::uintptr_t address) const;

	void stop() const;

	[[nodiscard]] std::uintptr_t stack_pointer() const;

	[[nodiscard]] std::uintptr_t read_stack(std::ptrdiff_t offset = 0) const;
};