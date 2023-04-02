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
		// ԭ����
		// ��������ֱ�Ӳ�����һ���ϼ򵥵�����Ժ�ʱ�Ĳ��ԣ�
		// ������ʱ���÷��������XDE32һ��������ָ���ʽ��
		// ������Ҫ�ض�λ��ָ��Ͷ�ȡ��ַ����ֵ�����ַ����䵱������

		// CALL
		// E8 <32 RVA>
		// A9 <32/48 Far>
		// FF 15 <���Ե�ַ>
		if (inst.opcode == 0xE8 && inst.len == 5 || inst.flags & F_IMM32)
		{
			auto callee = *ApplyRva<uint8_t*>(funcAddr + pos, inst.imm32);
			ent.Address = callee;
			entries.insert(ent);
		}

		if (pos < SizeOfCopy)
			pos += inst.len;
		// ʡ��lengthHist
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
