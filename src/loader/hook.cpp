#include "pch.hpp"
#include "pehelper.hpp"
#include "hook.hpp"
#include "fde64bin/fde64.h"
#include "search.hpp"

using namespace std;

// constexpr auto SizeOfCalls = 16;


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
	VirtualProtect(buffer, SizeOfCopy, PAGE_EXECUTE_READWRITE, &old); // rwx
	return old;
}

// FF 25 <i4 RVA>
constexpr unsigned short Jmp64Prefix = '\x25\xff';


size_t SearchAndApply(void* hReplacement, void* hHookee, void* hSignerature)
{
	size_t hooksCount = 0;

	set<Entry> signerature{}, replacement{};
	FindVmFunctions(hReplacement, replacement);
	FindVmFunctions(hSignerature, signerature);

	auto codesect = GetCodeSection(hHookee);
}

void Apply(void* toHook, void* targetPointer)
{
	auto w = Writer(toHook);
	w.UnlockCode();
	*w.Write<unsigned short>() = Jmp64Prefix;
	
	w.Advance(2);
	*w.Write<int>() = ToRva(toHook, targetPointer);
}
