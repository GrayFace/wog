//#include "stdafx.h"
#include <stdlib.h>
#include <windows.h>
extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
}
#include "hde32.h"

static DWORD ToDWORD(lua_State *L, int i)
{
	lua_Number num = lua_tonumber(L, i);
	if (num>=0)
		return (DWORD)num;
	else
		return (int)num;
}

static int DoGetHookSize(lua_State *L, int lim)
{
	DWORD p = ToDWORD(L, 1);
	lua_settop(L, 0);
	DWORD n = 0;
	bool jumps = false;
	while (n < lim)
	{
		hde32s a;
		hde32_disasm((void*)(p + n), &a);
		// attempt to recognize some jump/call instructions
		n += a.len;
		jumps = jumps || (a.opcode >= 0x70 && a.opcode <= 0x7F  // conditional jump short
			|| a.opcode == 0x0F && a.opcode2 >= 0x80 && a.opcode2 <= 0x8F  // conditional jump near
			|| a.opcode == 0xE8 || a.opcode == 0xE9 || a.opcode == 0xEB || a.opcode == 0xE3);
	}
	lua_pushnumber(L, n);
	lua_pushboolean(L, jumps);
	return 2;
}

static int GetHookSize(lua_State *L)
{
	return DoGetHookSize(L, 5);
}

static int GetInstructionSize(lua_State *L)
{
	return DoGetHookSize(L, 1);
}

void RSMemHDetourRegister(lua_State *L)
{
	lua_getglobal(L, "mem");
	lua_pushcfunction(L, GetHookSize);
	lua_setfield(L, -2, "GetHookSize");
	lua_pushcfunction(L, GetInstructionSize);
	lua_setfield(L, -2, "GetInstructionSize");
	lua_pop(L, 1);
}
