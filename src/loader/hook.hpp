#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct Writer
{
	uint8_t* ptr;
	uint8_t buffer[32];
	Writer(void* ptr)
	{
		this->ptr = (uint8_t*)ptr;
	}

	void Advance(int length);
	void Advance(size_t length);

	template<typename T>
	T* Write() 
	{
		static_assert(sizeof(T) < 32)
		return (T*)buffer;
	}

	DWORD UnlockCode();
};



// return the number of hooked trunks
size_t SearchAndApply(void* hReplacement, void* hHookee, void* hSignerature);

void Apply(void* toHook, void* targetPointer);