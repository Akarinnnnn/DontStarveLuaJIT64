#include "pch.hpp"
#include "pehelper.hpp"
#include "hook.hpp"
#include "fde64bin/fde64.h"
#include "search.hpp"

#include <unordered_map>
#include <functional>


using namespace std;

// constexpr auto SizeOfCalls = 16;
namespace
{
	// FF 25 <i4 RVA>
	constexpr unsigned short Jmp64Prefix = '\x25\xff';

	struct ReplacingFunctionEntry
	{
		function<void()> ApplyPatch;
		string_view Name;
		string_view HexPattern;
	};

	unordered_map<string_view, ReplacingFunctionEntry> functions{ 128 };

}
void Writer::Advance(int length)
{
	Advance((size_t)length);
}

void Writer::Advance(size_t length)
{
	SIZE_T written;
	WriteProcessMemory(GetCurrentProcess(), ptr, buffer, length, &written);
	ptr += written;
}

DWORD Writer::UnlockCode()
{
	DWORD old;
	VirtualProtect(buffer, SizeOfCopy, PAGE_READWRITE, &old); // rw
	return old;
}



size_t SearchAndApply(void* hexReplacement, void* hexHookee, void* hexSignature)
{
	size_t hooksCount = 0;

	set<Entry> signature{}, replacement{};
	FindVmFunctions(hexReplacement, replacement);
	FindVmFunctions(hexSignature, signature);

	auto codesect = GetCodeSection(hexHookee);
}

void Apply(void* toHook, void* targetPointer)
{
	auto w = Writer(toHook);
	w.UnlockCode();
	*w.Write<unsigned short>() = Jmp64Prefix;
	
	w.Advance(2);
	*w.Write<int>() = ToRva(toHook, targetPointer);
}
