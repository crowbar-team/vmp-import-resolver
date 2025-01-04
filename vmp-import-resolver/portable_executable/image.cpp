#include "image.hpp"

#include <vector>

portable_executable::dos_header_t* portable_executable::image_t::dos_header()
{
	return &this->m_dos_header;
}

const portable_executable::dos_header_t* portable_executable::image_t::dos_header() const
{
	return &this->m_dos_header;
}

portable_executable::nt_headers_t* portable_executable::image_t::nt_headers()
{
	return this->dos_header()->nt_headers();
}

const portable_executable::nt_headers_t* portable_executable::image_t::nt_headers() const
{
	return this->dos_header()->nt_headers();
}

std::uint16_t portable_executable::image_t::num_sections() const
{
	const nt_headers_t* nt_headers = this->nt_headers();

	if (!nt_headers)
	{
		return 0;
	}

	return nt_headers->file_header.number_of_sections;
}

portable_executable::section_header_t* portable_executable::image_t::find_section(const std::string_view name)
{
	for (auto& section : this->sections())
	{
		if (name == section.name)
		{
			return &section;
		}
	}

	return nullptr;
}

const portable_executable::section_header_t* portable_executable::image_t::find_section(const std::string_view name) const
{
	for (const auto& section : this->sections())
	{
		if (section.to_str().find(name) != std::string::npos)
		{
			return &section;
		}
	}

	return nullptr;
}

portable_executable::section_header_t* portable_executable::image_t::get_sections()
{
	const nt_headers_t* nt_headers = this->nt_headers();

	if (!nt_headers)
	{
		return nullptr;
	}

	return reinterpret_cast<section_header_t*>(reinterpret_cast<std::uintptr_t>(&nt_headers->optional_header) + nt_headers->file_header.sizeof_optional_header);
}

const portable_executable::section_header_t* portable_executable::image_t::get_sections() const
{
	const nt_headers_t* nt_headers = this->nt_headers();

	if (!nt_headers)
	{
		return nullptr;
	}

	return reinterpret_cast<section_header_t*>(reinterpret_cast<std::uintptr_t>(&nt_headers->optional_header) + nt_headers->file_header.sizeof_optional_header);
}

portable_executable::section_header_t* portable_executable::image_t::get_section(const std::size_t index)
{
	const nt_headers_t* nt_headers = this->nt_headers();

	if (!nt_headers)
	{
		return nullptr;
	}

	return index >= nt_headers->file_header.number_of_sections ? nullptr : this->get_sections() + index;
}

const portable_executable::section_header_t* portable_executable::image_t::get_section(const std::size_t index) const
{
	const nt_headers_t* nt_headers = this->nt_headers();

	if (!nt_headers)
	{
		return nullptr;
	}

	return index >= nt_headers->file_header.number_of_sections ? nullptr : this->get_sections() + index;
}

std::uint8_t* portable_executable::image_t::find_export(const std::string_view name) const
{
	for (const auto& [export_name, export_address] : this->exports())
	{
		if (export_name.find(name) != std::string::npos)
		{
			return export_address;
		}
	}

	return nullptr;
}

std::uint8_t* portable_executable::image_t::signature_scan(const std::string_view signature) const
{
	auto pattern_to_bytes = [](const std::string_view pattern) -> std::vector<std::int32_t>
	{
		std::vector<std::int32_t> bytes;

		const char* start = pattern.data();
		const char* end = start + pattern.size();

		for (auto current = const_cast<char*>(start); current < end; current++)
		{
			if (*current == '?')
			{
				current++;

				if (*current == '?')
				{
					current++;
				}

				bytes.push_back(-1);
			}
			else
			{
				bytes.push_back(static_cast<std::int32_t>(std::strtoul(current, &current, 16)));
			}
		}

		return bytes;
	};

	const std::vector<std::int32_t> pattern_bytes = pattern_to_bytes(signature);
	const std::size_t pattern_bytes_size = pattern_bytes.size();

	for (const auto& section : this->sections())
	{
		const std::uint8_t* section_start = this->as<const std::uint8_t*>() + section.virtual_address;
		const std::uint8_t* section_end = section_start + section.virtual_size;

		for (const std::uint8_t* byte = section_start; byte < (section_end - pattern_bytes_size); byte++)
		{
			bool found = true;

			for (std::size_t j = 0; j < pattern_bytes_size; j++)
			{
				if (pattern_bytes[j] != -1 && pattern_bytes[j] != byte[j])
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				return const_cast<std::uint8_t*>(byte);
			}
		}
	}

	return nullptr;
}

std::uint8_t* portable_executable::image_t::signature_scan(const std::uint8_t* pattern, const std::size_t pattern_size) const
{
	for (const auto& section : this->sections())
	{
		const std::uint8_t* section_start = this->as<const std::uint8_t*>() + section.virtual_address;
		const std::uint8_t* section_end = section_start + section.virtual_size;

		for (const std::uint8_t* byte = section_start; byte < (section_end - pattern_size); byte++)
		{
			bool found = true;

			for (std::size_t j = 0; j < pattern_size; j++)
			{
				if (pattern[j] != 0x00 && pattern[j] != byte[j])
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				return const_cast<std::uint8_t*>(byte);
			}
		}
	}

	return nullptr;
}
