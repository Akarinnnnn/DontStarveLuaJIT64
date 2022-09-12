#include "pch.hpp"
#include "pehelper.hpp"
#include "hook.hpp"
#include "fde64bin/fde64.h"

void Writer::Advance(int length)
{
	buffer += length;
}

void Writer::Advance(size_t length)
{
	buffer += length;
}

DWORD Writer::UnlockCode()
{
	DWORD old;
	VirtualProtect(buffer, 4096, PAGE_EXECUTE_READWRITE, &old); // rwx
	return old;
}

// FF 25 <i4 RVA>
constexpr unsigned short Jmp64Prefix = '\x25\xff';

size_t SearchAndApply(void* hReplacement, void* hHookee, void* hSignerature)
{
	size_t hooksCount = 0;
	auto codesect = GetCodeSection(hHookee);
}

void Apply(void* toHook, void* targetPointer)
{
	auto w = Writer(toHook);
	w.UnlockCode();
	*w.Write<unsigned short>() = Jmp64Prefix;
	
	w.Advance(2);
	*w.Write<void*>() = targetPointer;
}
