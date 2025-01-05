#include "vmp_pe.hpp"

vmp::pe::module_name_t::module_name_t(const std::string_view name) : m_name(name)
{
}

std::vector<std::uint8_t> vmp::pe::module_name_t::bytes() const
{
    std::vector<uint8_t> bytes(this->m_name.begin(), this->m_name.end());

    bytes.push_back('\0');

    return bytes;
}

vmp::pe::named_import_t::named_import_t(const std::string_view name) : m_hint(0), m_name(name)
{

}

std::vector<std::uint8_t> vmp::pe::named_import_t::bytes() const
{
    std::vector<uint8_t> bytes(sizeof(this->m_hint) + this->m_name.size() + 1, 0x0);

    std::memcpy(bytes.data(), &this->m_hint, sizeof(this->m_hint));
    std::memcpy(bytes.data() + sizeof(this->m_hint), this->m_name.data(), this->m_name.size());

    return bytes;
}

vmp::pe::thunk_data_x64_t::thunk_data_x64_t(const std::uint64_t address) : address(address)
{
    this->is_ordinal = false;
}

std::vector<std::uint8_t> vmp::pe::thunk_data_x64_t::bytes() const
{
    std::vector<uint8_t> bytes(sizeof(*this));

    std::memcpy(bytes.data(), this, sizeof(*this));

    return bytes;
}

vmp::pe::import_descriptor_t::import_descriptor_t(const std::uint32_t thunk_rva, const std::uint32_t name_rva) :
	m_rva_original_first_thunk(thunk_rva), m_rva_name(name_rva), m_rva_first_thunk(thunk_rva)
{
}

std::vector<std::uint8_t> vmp::pe::import_descriptor_t::bytes() const
{
    std::vector<uint8_t> bytes(sizeof(*this));

    std::memcpy(bytes.data(), this, sizeof(*this));

    return bytes;
}
