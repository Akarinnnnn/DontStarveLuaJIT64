#include "pch.hpp"
#include "pehelper.hpp"
#include "hook.hpp"
#include "fde64bin/fde64.h"
#include <vector>
#include <string>
#include <set>

using namespace std;

struct Entry
{
	string Name;
	// instruction cache
	uint8_t Cache[64];
	void* Address;
};

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

std::vector<Entry> Entries;

bool operator< (Entry& a, Entry& b) 
{
	return a.Name < b.Name;
}

bool operator== (Entry& a, Entry& b) 
{
	return a.Name == b.Name;
}

void ParseEntry(string_view name, const uint8_t* funcAddr, set<Entry> entries)
{
	Entry ent;
	memcpy(&ent.Cache, funcAddr, 64);
	fde64s inst;// ruction
	int pos = 0;
	while (decode(funcAddr, &inst) != 0 &&
		pos <= sizeof(ent.Cache))
	{
		pos += inst.len;

		// 原话：
		// 在这里我直接采用了一个较简单但是相对耗时的策略：
		// 在搜索时利用反汇编引擎XDE32一条条解析指令格式，
		// 发现需要重定位的指令，就读取地址处的值代替地址本身充当特征码
	
		// CALL
		// E8 <32 RVA>
		// A9 <32/48 Far>
		// FF 15 <绝对地址>
		if (inst.opcode == 0xE8 && inst.len == 5)
		{
			uint8_t* callee = ApplyRva<uint8_t>(funcAddr, inst.imm32);
			ent.Address = callee;
			entries.insert(ent);
		}
	}
}

void BuildFunctionList(void* hsig) 
{
	auto* expdir = GetExportDirectory((char*)hsig);
	auto* pEat = GetExportTableAddress(hsig);
	auto* namesList = ApplyRva<DWORD>(hsig, expdir->AddressOfNames);

	set<Entry> distinct;

	string_view lua = "lua_",
		luaopen = "luaopen_",
		luaL = "luaL_";

	for (DWORD i = 0; i < expdir->NumberOfNames; i++)
	{
		if (namesList[i] == 0)
			continue;

		string_view currentName{ ApplyRva<char>(hsig, namesList[i]) };
		

		if (currentName.starts_with(lua) ||
			currentName.starts_with(luaopen) ||
			currentName.starts_with(luaL))
		{
			
			ParseEntry(currentName, ApplyRva<uint8_t>(hsig, pEat[i]), distinct);
		}
	}
}

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
