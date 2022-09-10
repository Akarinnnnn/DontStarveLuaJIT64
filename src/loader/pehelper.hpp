#pragma once
#include <span>
constexpr size_t ExportFunctionCount = 181; // winmm.dll
extern "C" char * jmptable[ExportFunctionCount];

unsigned int* GetExportTableAddress(void* pointerInModule);
void PrepareJumpTable(unsigned int* source);
std::span<unsigned char> GetDsCodeSection(void* mod);