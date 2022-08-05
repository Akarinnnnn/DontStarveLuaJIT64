#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "pehelper.hpp"
#include <filesystem>
#include <cassert>

extern "C" HMODULE backing = nullptr;
extern "C" void** funclist = nullptr;
using namespace std::filesystem;

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpvReserved) 
{
	if (fdwReason == DLL_PROCESS_ATTACH) 
	{
		wchar_t buff[128];
		// path backingPath = std::filesystem::
		auto lastIndex = GetSystemDirectoryW(buff, 128);
		assert(lastIndex < 127 - 11);
		memcpy_s(buff + lastIndex, 128, L"\\winmm.dll", 11);

		// 原文如此，无视加载锁
		backing = LoadLibraryW(buff);
		assert(backing != nullptr);

		auto* eat = GetExportTableAddress(backing);
		funclist = eat;
		OverwriteOurEAT(eat);

	}
}