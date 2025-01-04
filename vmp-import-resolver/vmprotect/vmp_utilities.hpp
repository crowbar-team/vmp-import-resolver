#pragma once

#include <cstdint>
#include <vector>

namespace vmp::utilities
{
	std::vector<std::uintptr_t> scan_import_calls(std::uintptr_t text_base, const std::vector<std::uint8_t>& text_section);

	template <typename T>
	std::tuple<std::vector<std::uint8_t>, std::vector<std::size_t>, std::size_t> serialize_table(const std::vector<T>& table_entries, const std::size_t offset_base = 0)
	{
        std::vector<std::uint8_t> result_bytes;
        std::vector<std::size_t> result_offsets;

        for (const T& entry : table_entries)
        {
            result_offsets.push_back(result_bytes.size() + offset_base);

            std::vector<uint8_t> entry_bytes = entry.bytes();

            result_bytes.insert(result_bytes.end(), entry_bytes.begin(), entry_bytes.end());
        }

        return { result_bytes, result_offsets, result_bytes.size() + offset_base };
	}
}
