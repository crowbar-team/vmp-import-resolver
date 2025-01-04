#include "vmp_image.hpp"
#include "win_process/win_process.hpp"

#include <format>
#include <fstream>

portable_executable::image_t* vmp_image_t::image()
{
	return reinterpret_cast<portable_executable::image_t*>(this->m_buffer.data());
}

vmp_image_t::vmp_image_t(const std::uintptr_t image_base) : m_image_base(image_base)
{
}

void vmp_image_t::initialize_memory_pe(const std::size_t image_size, const win_process_t* win_process)
{
	if (!image_size)
	{
		throw std::invalid_argument("image_size cannot be 0");
	}

	if (!win_process)
	{
		throw std::invalid_argument("image_size cannot be nullptr");
	}

	this->m_buffer.resize(image_size);

	if (!win_process->read(this->m_image_base, this->m_buffer.data(), this->m_buffer.size()))
	{
		throw std::runtime_error(std::format("failed to read 0x{0:x} bytes from 0x{0:X} in remote process", this->m_buffer.size(), this->m_image_base));
	}
}

std::expected<portable_executable::section_header_t*, std::string> vmp_image_t::add_section(const std::string_view name, const std::uint32_t size, const portable_executable::section_characteristics_t section_characteristics)
{
	if (name.length() > portable_executable::section_name_size_limit)
	{
		return std::unexpected("section name cannot be longer than 8 characters");
	}

	portable_executable::image_t* image = this->image();

	if (!image || !image->dos_header()->valid() || !image->nt_headers()->valid())
	{
		return std::unexpected("invalid image loaded");
	}

	const portable_executable::section_header_t* last_section = image->get_section(image->num_sections() - 1);

	if (!last_section)
	{
		return std::unexpected("failed to find last section in image");
	}

	const auto new_section_header = reinterpret_cast<portable_executable::section_header_t*>(reinterpret_cast<std::uintptr_t>(last_section) + sizeof(portable_executable::section_header_t));

	const auto optional_header = &image->nt_headers()->optional_header;

	const auto align = [](const std::uint32_t value, const std::uint32_t alignment) -> std::uint32_t
	{
		return value + (alignment - value % alignment);
	};

	std::memcpy(new_section_header->name, name.data(), name.length());

	new_section_header->virtual_size = align(size + sizeof(std::uint32_t) + 1, optional_header->section_alignment);
	new_section_header->virtual_address = align(last_section->virtual_address + last_section->virtual_size, optional_header->section_alignment);
	new_section_header->size_of_raw_data = align(size + sizeof(std::uint32_t) + 1, optional_header->file_alignment);
	new_section_header->pointer_to_raw_data = align(last_section->pointer_to_raw_data + last_section->size_of_raw_data, optional_header->file_alignment);
	new_section_header->pointer_to_relocations = 0x0;
	new_section_header->pointer_to_linenumbers = 0x0;
	new_section_header->number_of_relocations = 0x0;
	new_section_header->number_of_linenumbers = 0x0;
	new_section_header->characteristics = section_characteristics;

	image->nt_headers()->file_header.number_of_sections++;

	const std::uint32_t old_size = optional_header->size_of_image;

	optional_header->size_of_image = align(optional_header->size_of_image + size + sizeof(std::uint32_t) + 1 + sizeof(portable_executable::section_header_t), optional_header->section_alignment);
	optional_header->size_of_headers = align(optional_header->size_of_headers + sizeof(portable_executable::section_header_t), optional_header->file_alignment);

	std::vector<std::uint8_t> new_image(optional_header->size_of_image, 0x0);

	std::memcpy(new_image.data(), this->m_buffer.data(), old_size);

	this->m_buffer = std::move(new_image);

	return image->find_section(name);
}

void vmp_image_t::dump_to_fs(const std::filesystem::path& fs_path)
{
	portable_executable::image_t* image = this->image();

	image->nt_headers()->optional_header.image_base = this->m_image_base;

	for (auto& section : image->sections())
	{
		section.pointer_to_raw_data = section.virtual_address;
		section.size_of_raw_data = section.virtual_size;
	}

	std::ofstream file_stream(fs_path, std::ios_base::out | std::ios_base::binary);

	if (!file_stream || !file_stream.is_open())
	{
		throw std::runtime_error(std::format("failed to open file stream to path {}", fs_path.string()));
	}

	file_stream.write(reinterpret_cast<const char*>(this->m_buffer.data()), static_cast<std::streamsize>(this->m_buffer.size()));
}
