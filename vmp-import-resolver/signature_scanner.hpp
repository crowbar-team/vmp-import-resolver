#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace signature_scanner
{
	void buffer(const void* buffer, std::size_t size, const std::vector<std::uint8_t>& signature, const std::function<void(std::size_t offset, const std::uint8_t* address)>& callback_fn);
}
