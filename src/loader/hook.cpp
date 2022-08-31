#include "pch.hpp"

#include "hook.hpp"

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

constexpr unsigned short Jmp64Prefix = '\x25\xff';

void Apply(void* toHook, void* targetPointer)
{
	auto w = Writer(toHook);
	w.UnlockCode();
	*w.Write<unsigned short>() = Jmp64Prefix;
	
	w.Advance(2);
	*w.Write<void*>() = targetPointer;
}
