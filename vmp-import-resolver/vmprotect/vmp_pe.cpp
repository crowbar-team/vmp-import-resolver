#include "vmp_pe.hpp"

std::vector<std::uint8_t> vmp::pe::module_name_t::bytes() const
{
    std::vector<uint8_t> bytes(this->name.begin(), this->name.end());

    bytes.push_back('\0');

    return bytes;
}

std::vector<std::uint8_t> vmp::pe::named_import_t::bytes() const
{
    std::vector<uint8_t> bytes(sizeof(this->hint) + this->name.size() + 1, 0x0);

    std::memcpy(bytes.data(), &this->hint, sizeof(this->hint));
    std::memcpy(bytes.data() + sizeof(this->hint), this->name.data(), this->name.size());

    return bytes;
}

std::vector<std::uint8_t> vmp::pe::thunk_data_x64_t::bytes() const
{
    std::vector<uint8_t> bytes(sizeof(*this));

    std::memcpy(bytes.data(), this, sizeof(*this));

    return bytes;
}

std::vector<std::uint8_t> vmp::pe::import_descriptor_t::bytes() const
{
    std::vector<uint8_t> bytes(sizeof(*this));

    std::memcpy(bytes.data(), this, sizeof(*this));

    return bytes;
}
