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
    std::vector<uint8_t> bytes;

    bytes.resize(sizeof(this->m_hint) + this->m_name.size() + 1);

    std::memcpy(bytes.data(), &this->m_hint, sizeof(this->m_hint));
    std::memcpy(bytes.data() + sizeof(this->m_hint), this->m_name.data(), this->m_name.size());

    return bytes;
}
