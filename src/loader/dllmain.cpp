#include "pch.hpp"

#include "pehelper.hpp"
#include <filesystem>
#include <cassert>

extern "C" HMODULE backing = nullptr;
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
		if(lastIndex > 127 - 11)
			return FALSE;
		memcpy_s(buff + lastIndex, 128 - lastIndex, L"\\winmm.dll", 11*2);

		// 原文如此，无视加载锁
		backing = LoadLibraryW(buff);
		if(backing == nullptr)
			return FALSE;

		auto* eat = GetExportTableAddress(backing);
		PrepareJumpTable(eat);
		// OverwriteOurEAT(eat);
#ifdef _DEBUG
		(void)getchar(); // 停一下
#endif
	}

	return TRUE;
}