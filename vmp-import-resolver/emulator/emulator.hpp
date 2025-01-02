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

	void terminate() const;

public:
	explicit emulator_t(uc_arch arch, uc_mode mode);

	~emulator_t();

	void initialize();

	void map_memory(std::uintptr_t address, const void* buffer, std::size_t size) const;

	void add_hook(uc_hook_type hook_type, void* callback_fn);

	void start(std::uintptr_t address) const;

	void stop() const;

	[[nodiscard]] std::uintptr_t stack_pointer() const;
};