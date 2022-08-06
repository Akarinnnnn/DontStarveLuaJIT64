#pragma once
constexpr size_t ExportFunctionCount = 103;
extern "C" unsigned char * jmptable[ExportFunctionCount];

unsigned int* GetExportTableAddress(void* pointerInModule);
void PrepareJumpTable(unsigned int* source);