#include "search.hpp"
#include "pehelper.cpp"
#include "fde64bin/fde64.h"

using namespace std;

set<string_view> ExistVmFunctions = {
	"luaL_addlstring", "luaL_addstring", "luaL_addvalue", "luaL_argerror",
	"luaL_buffinit", "luaL_callmeta", "luaL_checkany", "luaL_checkinteger",
	"luaL_checklstring", "luaL_checknumber", "luaL_checkoption", "luaL_checkstack",
	"luaL_checkudata", "luaL_findtable", "luaL_getmetafield", "luaL_gsub",
	"luaL_loadbuffer", "luaL_loadfile", "luaL_loadstring", "luaL_newmetatable",
	"luaL_newstate", "luaL_openlib", "luaL_openlibs", "luaL_optinteger",
	"luaL_optlstring", "luaL_optnumber", "luaL_prepbuffer", "luaL_pushresult",
	"luaL_ref", "luaL_register", "luaL_unref", "luaL_where",
	"lua_cpcall", "lua_equal", "lua_getallocf", "lua_gethook",
	"lua_gethookcount", "lua_gethookmask", "lua_getupvalue", "lua_isuserdata",
	"lua_pushvfstring", "lua_setallocf", "lua_setupvalue", "lua_status", "luaopen_io"
};


/// <summary>
/// ����Lua����
/// </summary>
/// <param name="hsig">����������ģ��</param>
/// <param name="entries"></param>
void FindVmFunctions(void* hsig, set<Entry>& entries)
{
	auto* expdir = GetExportDirectory((char*)hsig);
	auto* pEat = GetExportTableAddress(hsig);
	auto* namesList = ApplyRva<DWORD>(hsig, expdir->AddressOfNames);

	for (DWORD i = 0; i < expdir->NumberOfNames; i++)
	{
		if (namesList[i] == 0)
			continue;

		string_view currentName{ ApplyRva<char>(hsig, namesList[i]) };

		/*if (currentName.starts_with(lua) ||
			currentName.starts_with(luaopen) ||
			currentName.starts_with(luaL))*/
		if (ExistVmFunctions.contains(currentName))
		{
			RecordCalls(currentName, ApplyRva<uint8_t>(hsig, pEat[i]), entries);
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

		// CALL, PUSH
		// E8 <32 RVA>
		// A9 <32/48 Far>
		// FF 15 <���Ե�ַ>
		if (inst.opcode == 0xE8 && inst.len == 5 || inst.flags & F_IMM32)
		{
			auto callee = *ApplyRva<uint8_t*>(funcAddr + pos, inst.imm32);
			ent.Address = callee;
			entries.insert(ent);
		}

		if (pos + inst.len < SizeOfCopy)
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
