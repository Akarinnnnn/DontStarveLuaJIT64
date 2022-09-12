// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "lua.hpp"

static long long s_data;

extern "C" bool DLLEXPORT CheckVersion()
{
    lua_State* vm = lua_open();
    size_t verlen = 0;
    luaL_openlibs(vm);

    luaL_dofile(vm, "checkVersion.lua");
    const char* version = luaL_checklstring(vm, 1, &verlen);
    printf_s(version);

    if (strncmp(version, LUA_VERSION, verlen) == 0)
        return false;
    else
        return true;

}

int main(void) 
{
    if (CheckVersion())
        printf_s("true");
    else
        printf_s("false");
}


int DLLEXPORT UnrelatedFunc1(int l, int r) 
{
    return l + r * (l & r);
}


int DLLEXPORT UnrelatedFunc2(int num)
{
    s_data += num;
    return static_cast<int>(s_data) * 2;
}

int DLLEXPORT UnrelatedFunc3(int num)
{
    UnrelatedFunc1(num, num + 200) ^ UnrelatedFunc2(num ^ 0x1f2f3f4d);
}