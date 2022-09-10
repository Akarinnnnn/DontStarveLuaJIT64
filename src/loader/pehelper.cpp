#include "pehelper.hpp"
#define _WIN32_LEAN_AND_MEAN 1

#include <cassert>
#include <memory>
#include <Windows.h>
#include <ImageHlp.h>

char* jmptable[ExportFunctionCount] {};

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

unsigned int* GetExportTableAddress(void* ptr)
{
	auto mod = GetModule(ptr);
	auto exdir = GetExportDirectory(mod);
	return (unsigned int*)(mod + exdir->AddressOfFunctions);
}

void PrepareJumpTable(unsigned int* source)
{
	auto mod = GetModule(source);
	auto size = ExportFunctionCount * 8;
	
	// DWORD oldProtect = PAGE_READONLY;
	auto* ntheader = GetOptHeader(mod);
	char* expdir = (char*)GetExportDirectory((char*)source),
		*expend = expdir + ntheader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	uint32_t* eat = (uint32_t*)(mod + ((PIMAGE_EXPORT_DIRECTORY)expdir)->AddressOfFunctions);
	std::fill(jmptable, jmptable + ExportFunctionCount - 1, (unsigned char*)source);

	char fwdLib[64];

	for (size_t i = 0; i < ExportFunctionCount; i++)
	{
		jmptable[i] += eat[i];
		if (jmptable[i] > expdir || jmptable[i] < expend)
		{
			// 函数转发，特殊处理
			// ntdll.FunctionName
			strcpy(fwdLib, jmptable[i]);
			char* dotpos = strchr(fwdLib, '.');
			*dotpos = 0;
			jmptable[i] = (char*)GetProcAddress(GetModuleHandleA(fwdLib), dotpos + 1);
		}
	}


}

std::span<unsigned char> GetDsCodeSection(void* modDstExecutable)
{
	auto* opt = GetOptHeader(modDstExecutable);
	// auto* fileheader = (IMAGE_FILE_HEADER*)((char*)opt - sizeof(IMAGE_FILE_HEADER));

	unsigned char* pCodeSection = (unsigned char*)opt->ImageBase + opt->BaseOfCode;
	return std::span<unsigned char>(pCodeSection, opt->SizeOfCode);
}
