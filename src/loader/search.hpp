#pragma once
#include <set>
#include "hook.hpp"

constexpr auto SizeOfCopy = 64;

struct Entry
{
	string Name;
	// instruction cache
	uint8_t InstructionCopy[SizeOfCopy];
	vector<unsigned int> Calls;
	size_t CopyLength;
	void* Address;
};

void FindVmFunctions(void* hmod, std::set<Entry>& entries);