#include "pch.hpp"

#include <cassert>
#include <memory>

#include "pehelper.hpp"

char* jmptable[ExportFunctionCount + 1] {};

auto GetOptHeader(void* mod) {
	IMAGE_DOS_HEADER * header = (IMAGE_DOS_HEADER*)mod;
	IMAGE_OPTIONAL_HEADER64* ntheader = (IMAGE_OPTIONAL_HEADER64*)((char*)mod + header->e_lfanew + sizeof(IMAGE_FILE_HEADER) + 4);
	return ntheader;
}

PIMAGE_EXPORT_DIRECTORY GetExportDirectory(char* module)
{
	auto ntheader = GetOptHeader(module);
	return (PIMAGE_EXPORT_DIRECTORY)(module + ntheader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
}

char* GetModule(void* ptr)
{
	char* ret = nullptr;
	auto successful = GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCWSTR)ptr,
		(HMODULE*)&ret);
	assert(successful);
	return ret;
}

uint32_t* GetExportTableAddress(void* ptr)
{
	auto mod = GetModule(ptr);
	auto exdir = GetExportDirectory(mod);
	return ApplyOffset<uint32_t>(mod, GetExportDirectory(mod)->AddressOfFunctions);
}

void PrepareJumpTable(unsigned int* source)
{
	constexpr auto size = ExportFunctionCount * sizeof(intptr_t);
	
	auto mod = GetModule(source);
	auto* eat = GetExportTableAddress(source);

	for (size_t i = 1; i < ExportFunctionCount; i++)
	{
		jmptable[i] = *ApplyOffset<char*>(mod, eat[i]);
	}


}

std::span<unsigned char> GetCodeSection(void* modDstExecutable)
{
	auto* opt = GetOptHeader(modDstExecutable);

	return std::span<unsigned char>(
		ApplyOffset<unsigned char>(
			modDstExecutable,
			opt->ImageBase + opt->BaseOfCode),
		opt->SizeOfCode);
}
