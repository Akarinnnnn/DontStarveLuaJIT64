#include "pehelper.hpp"

#define _WIN32_LEAN_AND_MEAN
#include <cassert>
#include <memory>
#include <Windows.h>

unsigned char* jmptable[ExportFunctionCount] {};


PIMAGE_EXPORT_DIRECTORY GetExportDirectory(char* module)
{
	IMAGE_DOS_HEADER* header = (IMAGE_DOS_HEADER*)module;
	IMAGE_OPTIONAL_HEADER64* ntheader = (IMAGE_OPTIONAL_HEADER64*)(module + header->e_lfanew + sizeof(IMAGE_FILE_HEADER) + 4);
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

unsigned int* GetExportTableAddress(void* ptr)
{
	auto mod = GetModule(ptr);
	auto exdir = GetExportDirectory(mod);
	return (unsigned int*)(mod + exdir->AddressOfFunctions);
}

void PrepareJumpTable(unsigned int* source)
{
	auto targetmod = (unsigned char*)GetModule(&PrepareJumpTable);
	auto size = ExportFunctionCount * 8;
	DWORD oldProtect = PAGE_READONLY;

	std::fill(jmptable, jmptable + ExportFunctionCount - 1, targetmod);

	for (size_t i = 0; i < ExportFunctionCount; i++)
	{
		jmptable[i] += source[i];
	}
}
