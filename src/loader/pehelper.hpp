#pragma once
#include <Windows.h>
#include <span>
constexpr size_t ExportFunctionCount = 181; // winmm.dll
extern "C" char * jmptable[ExportFunctionCount];

unsigned int* GetExportTableAddress(void* pointerInModule);
PIMAGE_EXPORT_DIRECTORY GetExportDirectory(char* module);
void PrepareJumpTable(unsigned int* source);
std::span<unsigned char> GetCodeSection(void* mod);

template<typename TTo, typename TFrom>
TTo* ApplyRva(TFrom* base, int rva)
{
	return (TTo*)((char*)base + rva);
}

template<typename TTo, typename TFrom>
int ToRva(TFrom* base, TTo* dest)
{
	return ((char*)base - (char*)dest);
}
