//#include "stdafx.h"
#include <stdlib.h>
#include <windows.h>
extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
}
#include "RSMemHDetour.h"

typedef unsigned char byte;

static lua_State *Lua;

static int ToInteger(lua_State *L, int i)
{
	//if (lua_isnumber(L, i))  return (int)lua_tonumber(L, i);
	//if (lua_isboolean(L, i))  return lua_toboolean(L, i);
	//return 0;

	lua_Number num = lua_tonumber(L, i);
	if (num>=0)
		return (DWORD)num;
	else
		return (int)num;
}

//-------- Bits support ---------

static int BitOr(lua_State *L)
{
	int i = ToInteger(L, 1) | ToInteger(L, 2);
	lua_settop(L, 0);
	lua_pushnumber(L, i);
	return 1;
}

static int BitAnd(lua_State *L)
{
	int i = ToInteger(L, 1) & ToInteger(L, 2);
	lua_settop(L, 0);
	lua_pushnumber(L, i);
	return 1;
}

static int BitXor(lua_State *L)
{
	int i = ToInteger(L, 1) ^ ToInteger(L, 2);
	lua_settop(L, 0);
	lua_pushnumber(L, i);
	return 1;
}

static int BitAndNot(lua_State *L)
{
	int i = ToInteger(L, 1) & ~ToInteger(L, 2);
	lua_settop(L, 0);
	lua_pushnumber(L, i);
	return 1;
}

//-------- memory ---------

static int CallMultyParam(void* callback, void* callwhat, int _size, int fastcount, void* param) // fastcount: 0 - normal, 1 - thiscall, 2 - fastcall
{
	int result;
	_asm
	{
		push ebx
		mov ebx, esp
		mov eax, _size
		sub esp, eax
		push eax
		mov eax, param
		push eax
		lea eax, [esp + 8]
		push eax
		call callback
		test al, al
		jz _ret
		add esp, 12
		mov eax, fastcount
		dec eax
		jl _call
		pop ecx
		jz _call
		pop edx
_call:
		call callwhat
_ret:
		mov result, eax
		mov esp, ebx
		pop ebx
	}
	return result;
}

static int ToParam(lua_State *L, int idx, int &param)
{
	switch(lua_type(L, idx))
	{
		case LUA_TNIL:
			param = 0;
			break;
		case LUA_TNUMBER:
			param = ToInteger(L, idx);
			break;
		case LUA_TBOOLEAN:
			param = lua_toboolean(L, idx);
			break;
		case LUA_TSTRING:
			param = (int)lua_tostring(L, idx);
			break;
		case LUA_TUSERDATA:
		case LUA_TLIGHTUSERDATA:
			param = (int)lua_touserdata(L, idx);
			break;
		case LUA_TTABLE:
			lua_getfield(L, idx, "?ptr");
			if (lua_type(L, -1) == LUA_TNUMBER)
			{
				param = ToInteger(L, -1);
				lua_pop(L, 1);
				break;
			}
			else
			{
				lua_pop(L, 1);
				return 0;
			}
		default:
			//LuaLastError("Unsupported parameter type");
			return 0;
	}
	return 1;
}

static int MoveParams(int params[], lua_State *L, int idx, int n)
{
	for (int i = 0; i < n; i++, idx++)
		if (!ToParam(L, idx, params[i]))
			return 0;
	return 1;
}

static int __cdecl CallbackCall(int params[], lua_State *L)
{
	return MoveParams(params, L, 3, lua_gettop(L) - 2);
}

static int CallPtr(lua_State *L)
{
	int ret = CallMultyParam((void*)CallbackCall, (void*)ToInteger(L, 1), (lua_gettop(L)-2)*4, ToInteger(L, 2), L);
	lua_settop(L, 0);
	lua_pushinteger(L, ret);
	return 1;
}

static int Mem_toaddress(lua_State *L)
{
	int param;
	int ret = ToParam(L, 1, param);
	lua_settop(L, 0);
	if (ret)
		lua_pushnumber(L, param);
	return ret;
}

static int __inline Mem_NumSize(int t)
{
	switch(t)
	{
		case 5:  return 4;
		case 6:  return 8;
		case 7:  return 10;
		default:
			return abs(t);
	}
}

__declspec(naked) static long double __stdcall GetLongDouble(int p)
{
	_asm
	{
		mov eax, [esp + 4]
		fld tbyte ptr ds:[eax]
		ret 4
	}
}

static int Mem_GetNum(lua_State *L)
{
	int t = ToInteger(L, 1);
	int p = ToInteger(L, 2);
	lua_settop(L, 0);
	lua_Number v = 0;
	if (IsBadReadPtr((void*)p, Mem_NumSize(t))) return 0;
	switch(t)
	{
		case 8:   v = (lua_Number)*(long long*)p;                   break;
		case 4:   v = (lua_Number)*(int*)p;                         break;
		case 2:   v = (lua_Number)*(short*)p;                       break;
		case 1:   v = (lua_Number)*(char*)p;                        break;
		case -8:  v = (lua_Number)*(unsigned long long*)p;          break;
		case -4:  v = (lua_Number)*(unsigned int*)p;                break;
		case -2:  v = (lua_Number)*(unsigned short*)p;              break;
		case -1:  v = (lua_Number)*(unsigned char*)p;               break;
		case 5:   v = (lua_Number)*(float*)p;                       break;
		case 6:   v = (lua_Number)*(double*)p;                      break;
		case 7:   v = (lua_Number)GetLongDouble(p);                 break;
		//case 7:   v = (lua_Number)*(long double*)p;                 break; // MSVS doesn't support 10 bytes long double
	}
	lua_pushnumber(L, v);
	return 1;
}

__declspec(naked) static void __stdcall SetLongDouble(int p, double v)
{
	_asm
	{
		fld qword ptr ds:[esp + 8]
		mov eax, [esp + 4]
		fstp tbyte ptr ds:[eax]
		wait
		ret 0xC
	}
}

static int Mem_SetNum(lua_State *L)
{
	int t = ToInteger(L, 1);
	int p = ToInteger(L, 2);
	lua_Number v = lua_tonumber(L, 3);
	lua_settop(L, 0);
	//if (IsBadWritePtr((void*)p, Mem_NumSize(t))) return 0; // checked in the calling code
	switch(t)
	{
		case 8:   *(long long*)p = (long long)v;                    break;
		case 4:   *(int*)p = (int)v;                                break;
		case 2:   *(short*)p = (short)v;                            break;
		case 1:   *(signed char*)p = (signed char)v;                break;
		case -8:  *(unsigned long long*)p = (unsigned long long)v;  break;
		case -4:  *(unsigned int*)p = (unsigned int)v;              break;
		case -2:  *(unsigned short*)p = (unsigned short)v;          break;
		case -1:  *(unsigned char*)p = (unsigned char)v;            break;
		case 5:   *(float*)p = (float)v;                            break;
		case 6:   *(double*)p = (double)v;                          break;
		case 7:
			if (sizeof(long double) == 10)
				*(long double*)p = v;
			else
				SetLongDouble(p, v); // MSVS doesn't support 10 bytes long double
			break;
	}
	lua_pushnumber(L, v);
	return 1;
}

static int Mem_String(lua_State *L)
{
	char* p = (char*)ToInteger(L, 1);
	if (p == 0 && lua_gettop(L) == 1)
	{
		lua_pushstring(L, "");
		return 1;
	}
	if (IsBadReadPtr((void*)p, 1))
	{
		lua_settop(L, 0);
		return 0;
	}

	if (lua_gettop(L) > 1)
	{
		int n = ToInteger(L, 2);
		int i = 0;
		if (lua_gettop(L) == 2)
			for (; i < n && p[i]!= 0; i++) ;
		else
			i = n;
		lua_settop(L, 0);
		lua_pushlstring(L, p, i);
	}
	else
	{
		lua_settop(L, 0);
		lua_pushstring(L, p);
	}
	return 1;
}

static void __fastcall Mem_HookProcCall(DWORD data)
{
	lua_pushlightuserdata(Lua, &Lua);
	lua_rawget(Lua, LUA_REGISTRYINDEX);
	lua_pushnumber(Lua, (DWORD)data);
	lua_call(Lua, 1, 0);
}

static int Mem_RegisterHookCallback(lua_State *L)
{
	Lua = L;
	lua_pushlightuserdata(L, &Lua);
	lua_pushvalue(L, 1);
	lua_rawset(L, LUA_REGISTRYINDEX);
	return 0;
}

__declspec(naked) static void Mem_HookProc()
{
	__asm
	{
		pushfd
		sub esp, 4096 - 4  // if more than 4096, we must touch the stack every 4096 bytes
		//pushad
		push eax
		lea eax, [esp + 4096 + 4]
		push ecx
		push edx
		push ebx
		push eax
		push ebp
		push esi
		push edi
		push dword ptr [eax - 4]  // flags

		mov ecx, esp
		call Mem_HookProcCall
		popfd
		//popad  // чертовщина с ESP
		pop edi
		pop esi
		pop ebp
		mov ebx, [esp + 0x4]
		mov edx, [esp + 0x8]
		mov ecx, [esp + 0xC]
		mov eax, [esp + 0x10]
		mov esp, [esp]

		ret
	}
}

static int NewObj(lua_State *L)
{
	DWORD i = 0;
	if (lua_gettop(L) != 0)
		i = (DWORD)lua_tonumber(L, 1);
	lua_settop(L, 0);
	i = (DWORD)lua_newuserdata(L, i);
	if (i == 0) return 0;
	lua_pushnumber(L, i);
	return 2;
}

static const struct luaL_reg mem_internal [] =
{
	{"bit_Or", BitOr},
	{"bit_And", BitAnd},
	{"bit_Xor", BitXor},
	{"bit_AndNot", BitAndNot},
	{"NewObj", NewObj},
	{"call", CallPtr},
	{"Mem_GetNum", Mem_GetNum},
	{"Mem_SetNum", Mem_SetNum},
	{"Mem_String", Mem_String},
	{"RegisterHookCallback", Mem_RegisterHookCallback},
	{"toaddress", Mem_toaddress},
	{0, 0}
};

static void RegConst(lua_State *L, const char * name, DWORD i)
{
	lua_pushnumber(L, i);
	lua_setfield(L, -2, name);
}

void RSMemRegister(lua_State *L)
{
	lua_createtable(L, 0, 0);

	luaL_register(L, 0, mem_internal);
	RegConst(L, "malloc", (DWORD)malloc);
	RegConst(L, "free", (DWORD)free);
	RegConst(L, "realloc", (DWORD)realloc);
	RegConst(L, "new", (DWORD)malloc);
	RegConst(L, "memcpy", (DWORD)memcpy);
	RegConst(L, "memset", (DWORD)memset);
	RegConst(L, "VirtualProtect", (DWORD)VirtualProtect);
	RegConst(L, "IsBadCodePtr", (DWORD)IsBadCodePtr);
	RegConst(L, "IsBadReadPtr", (DWORD)IsBadReadPtr);
	RegConst(L, "IsBadWritePtr", (DWORD)IsBadWritePtr);
	RegConst(L, "Mem_HookProc", (DWORD)Mem_HookProc);
	RegConst(L, "VirtualAlloc", (DWORD)VirtualAlloc);
	RegConst(L, "LoadLibrary", (DWORD)LoadLibraryA);
	RegConst(L, "FreeLibrary", (DWORD)FreeLibrary);
	RegConst(L, "GetProcAddress", (DWORD)GetProcAddress);
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	RegConst(L, "PageSize", info.dwPageSize);

	lua_setglobal(L, "mem");
	RSMemHDetourRegister(L);
}