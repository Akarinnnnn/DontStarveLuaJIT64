#include "search.hpp"
#include "pehelper.cpp"
#include "fde64bin/fde64.h"

using namespace std;

void FindVmFunctions(void* hvm, set<Entry>& entries)
{
	auto* expdir = GetExportDirectory((char*)hvm);
	auto* pEat = GetExportTableAddress(hvm);
	auto* namesList = ApplyRva<DWORD>(hvm, expdir->AddressOfNames);


	string_view lua = "lua_",
		luaopen = "luaopen_",
		luaL = "luaL_";

	for (DWORD i = 0; i < expdir->NumberOfNames; i++)
	{
		if (namesList[i] == 0)
			continue;

		string_view currentName{ ApplyRva<char>(hvm, namesList[i]) };


		if (currentName.starts_with(lua) ||
			currentName.starts_with(luaopen) ||
			currentName.starts_with(luaL))
		{

			RecordCalls(currentName, ApplyRva<uint8_t>(hvm, pEat[i]), entries);
		}
	}
}


void RecordCalls(string_view name, const uint8_t* funcAddr, set<Entry>& entries)
{
	Entry ent;
	memcpy(&ent.InstructionCopy, funcAddr, 64);
	fde64s inst;// insruction
	int pos = 0;
	while (decode(funcAddr + pos, &inst) != 0 &&
		pos <= sizeof(ent.InstructionCopy) &&
		inst.opcode != 0xCC && // int 3
		inst.opcode != 0xC3) // retn
	{
		// 原话：
		// 在这里我直接采用了一个较简单但是相对耗时的策略：
		// 在搜索时利用反汇编引擎XDE32一条条解析指令格式，
		// 发现需要重定位的指令，就读取地址处的值代替地址本身充当特征码

		// CALL
		// E8 <32 RVA>
		// A9 <32/48 Far>
		// FF 15 <绝对地址>
		if (inst.opcode == 0xE8 && inst.len == 5 || inst.flags & F_IMM32)
		{
			auto callee = *ApplyRva<uint8_t*>(funcAddr + pos, inst.imm32);
			ent.Address = callee;
			entries.insert(ent);
		}

		if (pos < SizeOfCopy)
			pos += inst.len;
		// 省略lengthHist
	}

	memcpy(ent.InstructionCopy, funcAddr, pos);
	ent.CopyLength = pos;
}

bool operator< (Entry& a, Entry& b)
{
	return a.Name < b.Name;
}

bool operator< (Entry& a, Entry& b)
{
	return a.Name > b.Name;
}

bool operator== (Entry& a, Entry& b)
{
	return a.Address == b.Address;
}
