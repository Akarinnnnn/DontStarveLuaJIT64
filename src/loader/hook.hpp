#pragma once
#include <cstdint>
struct Writer
{
	uint8_t* buffer;

	Writer(void* ptr)
	{
		buffer = (uint8_t*)ptr;
	}

	void Advance(int length);
	void Advance(size_t length);

	template<typename T>
	T* Write() 
	{
		return (T*)buffer;
	}

	DWORD UnlockCode();
};

size_t SearchAndApply(void* hReplacement, void* hHookee);

void Apply(void* toHook, void* targetPointer);