#include "signature_scanner.hpp"

void signature_scanner::buffer(const void* buffer, const std::size_t size, const std::vector<std::uint8_t>& signature, const std::function<void(std::size_t offset, const std::uint8_t* address)>& callback_fn)
{
	for (std::size_t i = 0; i < size - signature.size(); i++)
	{
		const std::uint8_t* temp_buffer = static_cast<const std::uint8_t*>(buffer) + i;

		bool found = true;

		for (std::size_t j = 0; j < signature.size(); j++)
		{
			if (signature[j] != 0x00 && signature[j] != temp_buffer[j])
			{
				found = false;

				break;
			}
		}

		if (found)
		{
			callback_fn(i, temp_buffer);
		}
	}
}