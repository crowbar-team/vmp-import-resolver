#include "emulator.hpp"

#include <format>
#include <stdexcept>

void emulator_t::terminate() const
{
	if (this->m_engine)
	{
		for (const auto& hook : this->m_hooks)
		{
			uc_hook_del(this->m_engine, hook);
		}

		uc_close(this->m_engine);
	}
}

emulator_t::emulator_t(const uc_arch arch, const uc_mode mode) : m_arch(arch), m_mode(mode), m_rsp_reg(m_mode == UC_MODE_64 ? UC_X86_REG_RSP : UC_X86_REG_ESP)
{
	uc_err err = uc_open(this->m_arch, this->m_mode, &this->m_engine);

	if (err != UC_ERR_OK)
	{
		throw std::runtime_error(std::format("failed to initialize unicorn! error: {}", uc_strerror(err)));
	}

	constexpr std::uintptr_t stack_base = 0x40000ull;
	constexpr std::size_t stack_size = 0x10000;

	this->m_stack_end = stack_base + stack_size - 64;

	err = uc_mem_map(this->m_engine, stack_base, stack_size, UC_PROT_READ | UC_PROT_WRITE);

	if (err != UC_ERR_OK)
	{
		throw std::runtime_error(std::format("failed to setup virtual stack! error: {}", uc_strerror(err)));
	}

	err = uc_reg_write(this->m_engine, this->m_rsp_reg, &this->m_stack_end);

	if (err != UC_ERR_OK)
	{
		throw std::runtime_error(std::format("failed to write virtual stack address to stack pointer! error: {}", uc_strerror(err)));
	}
}

emulator_t::~emulator_t()
{
	this->terminate();
}

void emulator_t::map_memory(const std::uintptr_t address, const void* buffer, const std::size_t size) const
{
	const auto uc_align = [](const std::size_t value) -> std::size_t
	{
		constexpr size_t alignment = 0x1000;

		return value + alignment - 1 & ~(alignment - 1);
	};

	uc_err err = uc_mem_map(this->m_engine, address, uc_align(size), UC_PROT_ALL);

	if (err != UC_ERR_OK)
	{
		throw std::runtime_error(std::format("failed to map code! error: {}", uc_strerror(err)));
	}

	err = uc_mem_write(this->m_engine, address, buffer, size);

	if (err != UC_ERR_OK)
	{
		throw std::runtime_error(std::format("failed write code to mapped memory! error: {}", uc_strerror(err)));
	}
}

void emulator_t::add_hook(const uc_hook_type hook_type, void* user_data, void* callback_fn)
{
	uc_hook hook = 0;

	if (const uc_err err = uc_hook_add(this->m_engine, &hook, hook_type, callback_fn, user_data, 1, 0); err != UC_ERR_OK)
	{
		throw std::runtime_error(std::format("failed to add hook! error: {}", uc_strerror(err)));
	}

	this->m_hooks.emplace_back(hook);
}

void emulator_t::start(const std::uintptr_t address) const
{
	uc_err err = uc_reg_write(this->m_engine, this->m_rsp_reg, &this->m_stack_end);

	if (err != UC_ERR_OK)
	{
		throw std::runtime_error(std::format("failed to write virtual stack address to stack pointer! error: {}", uc_strerror(err)));
	}

	err = uc_emu_start(this->m_engine, address, 0, 0, 0);

	if (err != UC_ERR_OK)
	{
		throw std::runtime_error(std::format("emulation failed with error: {}", uc_strerror(err)));
	}
}

void emulator_t::stop() const
{
	uc_emu_stop(this->m_engine);
}

std::uintptr_t emulator_t::stack_pointer() const
{
	std::uintptr_t sp = 0;

	uc_reg_read(this->m_engine, this->m_rsp_reg, &sp);

	return sp;
}

std::uintptr_t emulator_t::read_stack(const std::ptrdiff_t offset) const
{
	const std::uintptr_t sp = this->stack_pointer();

	std::uintptr_t value = 0;

	uc_mem_read(this->m_engine, sp + offset, &value, sizeof(value));

	return value;
}

std::uintptr_t emulator_t::stack_displacement() const
{
	return this->m_stack_end - this->stack_pointer();
}