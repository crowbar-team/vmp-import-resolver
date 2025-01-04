#include "vmp_iat.hpp"

#include <ranges>

#include "vmp_pe.hpp"
#include "vmp_image.hpp"
#include "vmp_utilities.hpp"

void vmp::iat_t::add_import(const std::string& module_name, const std::string& function_name)
{
	if (const auto it = this->m_iat.find(module_name);
		it != this->m_iat.end())
	{
		if (auto& functions = it->second;
			std::ranges::find(functions, function_name) == functions.end())
		{
			functions.emplace_back(function_name);
		}
	}
	else
	{
		this->m_iat[module_name].emplace_back(function_name);
	}
}

void vmp::iat_t::reconstruct(image_t* image)
{
	const auto new_section_va = image->compute_new_section_va();

	if (!new_section_va)
	{
		throw std::runtime_error(new_section_va.error());
	}

	std::vector<pe::named_import_t> named_imports;

	for (const auto& imports : this->m_iat | std::views::values)
	{
		for (const auto& import : imports)
		{
			named_imports.emplace_back(0, import);
		}
	}

	auto [named_imports_serialized, named_imports_rvas, named_imports_end] = utilities::serialize_table(named_imports, *new_section_va);

	std::vector<pe::module_name_t> module_names;
	std::vector<pe::thunk_data_x64_t> import_thunks;
	std::vector<std::size_t> first_thunk_indices;

	std::size_t name_index = 0;

	for (const auto& [module_name, imports] : this->m_iat)
	{
		first_thunk_indices.emplace_back(import_thunks.size());

		module_names.emplace_back(module_name);

		for (const auto& import : imports)
		{
			const std::uint64_t named_import_rva = named_imports_rvas[name_index];

			name_index++;

			pe::thunk_data_x64_t thunk_data = { };
			thunk_data.address = named_import_rva;

			import_thunks.emplace_back(thunk_data);
		}

		import_thunks.emplace_back();
	}

	auto [module_names_serialized, module_names_rvas, module_names_end] = utilities::serialize_table(module_names, named_imports_end);
	auto [import_thunks_serialized, import_thunks_rvas, import_thunks_end] = utilities::serialize_table(import_thunks, module_names_end);

	std::vector<pe::import_descriptor_t> import_descriptors;

	std::size_t index2 = 0;

	for (const auto& index : first_thunk_indices)
	{
		const auto thunk_rva = static_cast<std::uint32_t>(import_thunks_rvas[index]);
		const auto name_rva = static_cast<std::uint32_t>(module_names_rvas[index2]);

		pe::import_descriptor_t import_descriptor = { };
		import_descriptor.rva_name = name_rva;
		import_descriptor.rva_original_first_thunk = thunk_rva;
		import_descriptor.rva_first_thunk = thunk_rva;

		import_descriptors.push_back(import_descriptor);
		index2++;
	}

	auto [import_descriptors_serialized, import_descriptors_rvas, import_descriptors_end] = utilities::serialize_table(import_descriptors, import_thunks_end);

	this->m_import_section.insert(this->m_import_section.end(), named_imports_serialized.begin(), named_imports_serialized.end());
	this->m_import_section.insert(this->m_import_section.end(), module_names_serialized.begin(), module_names_serialized.end());
	this->m_import_section.insert(this->m_import_section.end(), import_thunks_serialized.begin(), import_thunks_serialized.end());
	this->m_import_section.insert(this->m_import_section.end(), import_descriptors_serialized.begin(), import_descriptors_serialized.end());

	const auto section_size = static_cast<std::uint32_t>(this->m_import_section.size());

	portable_executable::section_characteristics_t section_characteristics = { };
	section_characteristics.cnt_init_data = 1;
	section_characteristics.mem_read = 1;

	const auto section = image->add_section(".iat", section_size, section_characteristics);

	if (!section)
	{
		throw std::runtime_error(section.error());
	}

	std::memcpy(image->get() + (*section)->virtual_address, this->m_import_section.data(), this->m_import_section.size());

	const auto iat_va = static_cast<std::uint32_t>(import_thunks_end);
	const auto iat_size = static_cast<std::uint32_t>(import_descriptors_end - import_thunks_end);

	image->set_iat(iat_va, iat_size);
}