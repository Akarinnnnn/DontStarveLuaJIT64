#include "pehelper.hpp"

#define _WIN32_LEAN_AND_MEAN
#include <cassert>
#include <Windows.h>

size_t ExportFunctionCount = 103;

void** GetExportTableAddress(void* ptr)
{
	
	char* module = nullptr;
	auto successful = GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCWSTR)ptr,
		(HMODULE*)&module);
	assert(successful);

	IMAGE_DOS_HEADER* header = (IMAGE_DOS_HEADER*)module;
	IMAGE_OPTIONAL_HEADER64* ntheader = (IMAGE_OPTIONAL_HEADER64*)(module + header->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY eat = (PIMAGE_EXPORT_DIRECTORY)(module + ntheader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	return (void**)(module + eat->AddressOfFunctions);
}

void OverwriteOurEAT(void** source)
{
	
}
