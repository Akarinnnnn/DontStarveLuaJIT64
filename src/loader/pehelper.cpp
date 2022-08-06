#include "pehelper.hpp"

#define _WIN32_LEAN_AND_MEAN
#include <cassert>
#include <Windows.h>

size_t ExportFunctionCount = 103;

PIMAGE_EXPORT_DIRECTORY GetExportDirectory(char* module)
{
	IMAGE_DOS_HEADER* header = (IMAGE_DOS_HEADER*)module;
	IMAGE_OPTIONAL_HEADER64* ntheader = (IMAGE_OPTIONAL_HEADER64*)(module + header->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY eat = (PIMAGE_EXPORT_DIRECTORY)(module + ntheader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
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

void** GetExportTableAddress(void* ptr)
{
	auto mod = GetModule(ptr);
	auto exdir = GetExportDirectory(mod);
	return (void**)(mod + exdir->AddressOfFunctions);
}

void OverwriteOurEAT(void** source)
{
	auto thismod = GetModule(&OverwriteOurEAT);
	auto* thisExport = GetExportDirectory(thismod);
	auto* thiseat = GetExportTableAddress(&OverwriteOurEAT);
	auto size = (size_t)8 * thisExport->NumberOfFunctions;
	DWORD oldProtect = PAGE_READONLY;

	auto status = VirtualProtect(thiseat, size, PAGE_READWRITE, &oldProtect);
	assert(status == 0);
	memcpy(thiseat, source, size);

	VirtualProtect(thiseat, size, oldProtect, &oldProtect);
}
