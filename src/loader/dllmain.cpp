#include "pch.hpp"

#include "pehelper.hpp"
#include "hook.hpp"
#include <filesystem>
#include <cassert>

extern "C" HMODULE backing = nullptr;
using namespace std::filesystem;

const char* Dst64ModName = "dontstarve_steam_x64.exe";
const char* SigFileName = "LuaSig51.dll";
const char* NewVmName = "ReplacementVM.dll";


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
		memcpy_s(buff + lastIndex, rsize_t(128) - lastIndex, L"\\winmm.dll", static_cast<rsize_t>(11) * 2);

		// 原文如此，无视加载锁
		backing = LoadLibraryW(buff);
		if(backing == nullptr)
			return FALSE;

		auto* eat = GetExportTableAddress(backing);
		PrepareJumpTable(eat);
		
		// 获取饥荒模块
		void* ds64 = GetModuleHandleA(Dst64ModName);
		void* sig = LoadLibraryA(SigFileName);
		void* replace = LoadLibraryA(NewVmName);

		size_t count = SearchAndApply(replace, ds64, replace);

#ifdef _DEBUG
		(void)getchar(); // 停一下
#endif
	}

	return TRUE;
}