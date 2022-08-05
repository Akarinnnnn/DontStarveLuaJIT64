#pragma once

extern "C" size_t ExportFunctionCount;

void** GetExportTableAddress(void* pointerInModule);
void OverwriteOurEAT(void** source);