#include <string.h>
#include <stdio.h>
#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "string.h"
#include "service.h"
#include "erm.h"
#include "b1.h"
#include "erm_lua.h"
#include "global.h"
#include "wogsetup.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lupvaluejoin.h"
#include "RSMem.h"
#include "luasocket.h"
//#include "afx.h"
#define __FILENUM__ 25

// set panic (&error) function, error function
// specials: FU:E; IF:M,Q;

#define GameLoaded  MainWindow
lua_State *Lua;
Word CmdCmd;
_ToDo_ CmdToDo;
Mes CmdMessage;

int ErrfIndex;
char *LuaErrorString = "Lua call";
#define CHECK_V    (1)
#define CHECK_Z    (2)
#define CHECK_PTR  (3)

static int ToInteger(lua_State *L, int i)
{
	//if (lua_isnumber(L, i))  return (int)lua_tonumber(L, i);
	//if (lua_isboolean(L, i))  return lua_toboolean(L, i);
	//return 0;

	double num = lua_tonumber(L, i);
	if (num>=0)
		return (DWORD)num;
	else
		return (int)num;
}

static const char* ToString(lua_State *L, int i)
{
	const char* str = lua_tostring(L, i);
	if (str)  return str;

	int type = lua_type(L, i);
	if (type == LUA_TBOOLEAN)  return (lua_toboolean(L, i) ? "true" : "false");
	return lua_typename(L, lua_type(L, i));
}

static int LuaError(char *text, int level)
{
	LuaCall("SetErrorLevel", level + 1);
	luaL_where(Lua, level);
	lua_pushstring(Lua, text);
	lua_concat(Lua, 2);
	return lua_error(Lua);
}

void ErrorMessage(const char * msg)
{
	FILE* f=fopen("WOGLUALOG.TXT", "w"); fprintf(f,"%s\n",msg); fclose(f);
	if (GameLoaded)
		Message(Format("{Error}\n%s", msg));
	else
		MessageBox(0, msg, "Error", 0);
}

static void ErrorMessageExe(lua_State *L)
{
	ErrorMessage(ToString(L, -1));			
	lua_pop(L, 1);
}

static void ErrorMessage(lua_State *L)
{
	int param = lua_gettop(L);

	lua_getfield(L, LUA_REGISTRYINDEX, "ErrorMessage"); // get 'internal.ErrorMessage'
	lua_pushvalue(L, param);
	if (lua_pcall(L, 1, 0, 0) == 0)
	{
		lua_settop(L, param - 1);
		return;
	}
	lua_settop(L, param);
	ErrorMessageExe(L);
}

static int ErrorMessageLua(lua_State *L)
{
	luaL_checkany(L, -1);
	ErrorMessage(L);
	return 0;
}

const char *WrongParamsError = "Invalid parameters";
char LastErmError[1000];
bool WasErmError = false;
void LuaLastError(const char *error)
{
	if(WasErmError && LastErmError[0] != 0) return;
	WasErmError = true;
	StrCopy(LastErmError, 1000, error);
}

int LuaGetLastError(lua_State *L)
{
	if (!WasErmError) return 0;
	lua_pushstring(L, LastErmError);
	return 1;
}

// Params: "IF", PostFlag, trig_num, InsertIndex, n[0], n[1], ... Returns: 0 if ok, 1 or -1 in case of error
static int ERM_Trigger(lua_State *L)
{
	STARTNA(__LINE__, 0)

	DoneError = false;
	GEr.LastERM(LuaErrorString);
	ErrStringInfo LastErrString;
	NewErrStringInfo(LuaErrorString, &LastErrString);
	WasErmError = false;
	LuaLastError(WrongParamsError);

	const char *CmdName = lua_tostring(L, 1);
	/*
	CmdToDo.Type = *(Word*)CmdName;
	CmdToDo.Self.s = "Lua call";
	CmdToDo.Self.l = 8;
	CmdToDo.Com.s = "Lua call";
	CmdToDo.Com.l = 8;
	*/

	Mes M;
	FillMem((char*)&M, sizeof(Mes), 0);
	M.m.s = &ErrorCmd.Name[4];
	M.m.l = 8;
	M.c[0] = ';';

	int n = lua_gettop(L);

	for (int i = 0; i <= n-5; i++)
		M.VarI[i].Num = M.n[i] = ToInteger(L, i+5);

	n -= 4;
	if (n == 0) n = 1;
	int insertPos = ToInteger(L, 4);
	WasErmError = false;
	int ret = InitTrigger(M, *(Word*)CmdName, n, lua_toboolean(L, 2), insertPos);
	ErrString = LastErrString;

	if (ret == 0)
	{
		LastAddedTrigger->Efl[0][0][0].Type = 255;
		LastAddedTrigger->Efl[0][0][0].Num = ToInteger(L, 3);
		lua_settop(L, 0);
		lua_pushnumber(L, insertPos == -1 ? insertPos + TriggerCount : insertPos);
		RETURN(1)
	}
	//else
	//{
	//	lua_settop(L, 0);
	//	if (ret == 1)
	//		lua_pushboolean(L, false);
	//	else
	//		lua_pushnil(L);
	//}
	RETURN(0)
}

bool specialV;
int specV;
bool specialZ;
char specZ[512];

// Params: "IF", n[0], n[1], ... Returns: 0 if ok, 1 or -1 in case of error
static int ERM_Reciever(lua_State *L)
{
	STARTNA(__LINE__, 0)

	DoneError = false;
	GEr.LastERM(LuaErrorString);
	ErrStringInfo LastErrString;
	NewErrStringInfo(LuaErrorString, &LastErrString);
	WasErmError = false;
	LuaLastError(WrongParamsError);

	const char *CmdName = lua_tostring(L, 1);
	CmdToDo.Type = *(Word*)CmdName;
	CmdToDo.Self.s = ErrorCmd.Name;
	CmdToDo.Self.l = 8;
	CmdToDo.Com.s = ErrorCmd.Name;
	CmdToDo.Com.l = 8;

	Mes M;
	FillMem((char*)&M, sizeof(Mes), 0);
	M.m.s = &ErrorCmd.Name[4];
	M.m.l = 8;
	M.c[0] = ':';
	specialV = specialZ = 0;

	int n = lua_gettop(L);

	if (CmdToDo.Type == 'RV') // VR special case
	{
		switch (lua_type(L, 2))
		{
			case LUA_TSTRING:
				M.VarI[0].Type = vtZ;
				M.VarI[0].Num = -1;
				StrCopy(specZ, 512, lua_tostring(L, 2));
				specialZ = true;
				break;
			default:
				M.VarI[0].Type = vtV;
				M.VarI[0].Num = VAR_COUNT_V + 1;
				specV = ToInteger(L, 2);
				specialV = true;
		}
	}
	else
	{
		for (int i = 0; i < n-1; i++)
			M.VarI[i].Num = M.n[i] = ToInteger(L, i+2);
	}

	lua_settop(L, 0);

	int Num = n - 1;
	if (Num == 0) Num = 1;
	WasErmError = false;
	lua_pushnumber(L, InitReciever(M, CmdToDo.Type, Num, CmdToDo.Pointer, CmdToDo.ParSet, &CmdToDo.Par[0]));

	ErrString = LastErrString;
	RETURN(1);
}

#define ERM_Call_ErrorLevel (3)

// Params: "IF", {"?", 0}
static int ERM_Call(lua_State *L)
{
	if (lua_gettop(L) > 17)
		return LuaError(Format("\"%s:%s\"-too many parameters.", CmdToDo.Type, lua_tostring(L, 1)), ERM_Call_ErrorLevel);

	STARTNA(__LINE__, 0)

	const char *str = lua_tostring(L, 1);
	char cmd = *str;

	int n = lua_gettop(L);

	CmdMessage.m.s = ErrorCmd.Name;
	CmdMessage.m.l = 8;
	CmdMessage.i = 0;
	
	int backV[16];
	char StrVar[VAR_COUNT_LZ][512];
	for (int i = 0; i < VAR_COUNT_LZ; i++)
		StrVar[i][0] = 0;

	int indexV = VAR_COUNT_V;
	int countV = 0;
	int countZ = 0;
	const char *paramZ;
	int paramV;

	if (specialV) // special VR reciever case
	{
		backV[countV] = ERMVar2[indexV];
		ERMVar2[indexV] = specV;
		indexV++; countV++;
	}
	if (specialZ)
	{
		memcpy(StrVar[countZ++], specZ, 512);
	}

	// Prepare Message Parameters

	CmdMessage.VarI[0].Check = 0;
	CmdMessage.VarI[0].Num = 0;
	CmdMessage.VarI[0].Type = 0;
	CmdMessage.VarI[0].IType = 0;
	CmdMessage.n[0] = 0;
	CmdMessage.f[0] = 0;
	int k = 0;
	int j, ltype;
	for (int i = 2; i <= n; k++)
	{
		CmdMessage.VarI[k].Check = 0;
		j = i;
		ltype = lua_type(L, i);
		if (ltype == LUA_TTABLE)
		{
			CmdMessage.f[k] = 1;
			lua_rawgeti(L, i, 1);
			ltype = lua_type(L, -1);
			j = -1;
		}
		else
		{
			CmdMessage.f[k] = 0;
		}
		i++;

		switch (ltype)
		{
			case LUA_TLIGHTUSERDATA:
				switch ((DWORD)lua_topointer(L, j))
				{
					case CHECK_PTR:
						CmdMessage.f[k] = 1;
						// fall through
					case CHECK_V:
						CmdMessage.VarI[k].Check = 1;
						goto _number;
					case CHECK_Z:
						CmdMessage.VarI[k].Check = 1;
						goto _string;
				}
				goto _error;

			case LUA_TNIL:
				CmdMessage.f[k] = 1;
				paramV = 0;
				goto _number;

			case LUA_TBOOLEAN:
				paramV = (lua_toboolean(L, j) ? 1 : 0);
				goto _number;

			case LUA_TNUMBER:
				paramV = ToInteger(L, j);
_number:
				if (CmdMessage.VarI[k].Check) // get
				{
					CmdMessage.VarI[k].Type = 3;
					backV[countV] = ERMVar2[indexV];
					ERMVar2[indexV] = 0;
					indexV++;
					CmdMessage.VarI[k].Num = CmdMessage.n[k] = indexV;				
					countV++;
				}else
				{
					CmdMessage.VarI[k].Type = 0;
					CmdMessage.VarI[k].Num = CmdMessage.n[k] = paramV;
				}
				break;

			case LUA_TSTRING:
				paramZ = lua_tostring(L, j);
_string:
				CmdMessage.VarI[k].Type = 7;
				// allocate str in Z range
				if (CmdMessage.VarI[k].Check != 1) StrCopy(StrVar[countZ], 512, paramZ);
				countZ++;
				CmdMessage.VarI[k].Num = -countZ;
				CmdMessage.n[k] = -countZ;
				break;

			default:
_error:
				if (countV)  memcpy((char*)&ERMVar2[indexV - countV], (char*)&backV[0], 4*countV);
				RETURN(LuaError(Format("Invalid parameter type: %s", lua_typename(L, ltype)), ERM_Call_ErrorLevel))
		}

	}

	// Call ERM

	lua_settop(L, 0);

	GEr.LastERM(LuaErrorString);
	ErrStringInfo LastErrString;
	NewErrStringInfo(LuaErrorString, &LastErrString);
	WasErmError = false;
	char (*LastStrVar)[512] = ERMLString;
	ERMLString = StrVar;
	//__try
	//{
	int failed = ProcessMes(&CmdToDo, CmdMessage, cmd, k == 0 ? 1 : k);
	if (failed && !WasErmError)
		MError2("unknown error.");
	//}
	//__finally
	//{
	ErrString = LastErrString;
	ERMLString = LastStrVar;
	//}

	// Process Get Parameters

	int retCount = 0;
	if (specialV) { retCount++; lua_pushnumber(L, ERMVar2[VAR_COUNT_V]); }
	if (specialZ) { retCount++; lua_pushfstring(L, StrVar[0]); }
	for (int i = 0; i < k; i++)
		if (CmdMessage.VarI[i].Check)
		{
			retCount++;
			int ind = CmdMessage.VarI[i].Num;
			if (CmdMessage.VarI[i].Type == 7)
				lua_pushstring(L, StrVar[-ind-1]);
			else
				lua_pushnumber(L, ERMVar2[ind-1]);
		}

	if (countV)  memcpy((char*)&ERMVar2[indexV - countV], (char*)&backV[0], 4*countV);

	if (failed)
		RETURN(LuaError(LastErmError, ERM_Call_ErrorLevel))

	RETURN(retCount)
}

// Params: "v", 5, [new_value]
// Returns value if new_value isn't specified, returns 'true' if new_value is specified
// Return nil in case of error
static int ERM_Var(lua_State *L)
{
	STARTNA(__LINE__, 0)

	GEr.LastERM(LuaErrorString);
	ErrStringInfo LastErrString;
	NewErrStringInfo(LuaErrorString, &LastErrString);
	WasErmError = false;
	long long oldCmd = ErrorCmd.Cmd;

	int vtype;
	char c = *ToString(L, 1);
	int i;

	//__try
	//{
		if (c == '$') // macro
		{
			char dest[16];
			const char *src = ToString(L, 2);
			for (i = 0; i < 16 && src[i]; i++) dest[i] = src[i];
			for (; i < 16; i++) dest[i] = 0;
			VarNum *vpn = GetMacro(dest);
			StrCopy(ErrorCmd.Name, sizeof(ErrorCmd.Name), Format("ERM.$%s$", src));
			if (vpn == 0) { lua_settop(L, 0); MError2("macro not found."); RETURN(0) }
			i = vpn->Num;
			vtype = vpn->Type;
			if (vtype == 0)
			{
				if(lua_gettop(L) >= 3){ lua_settop(L, 0); MError2("cannot change constant macro (use MC:S instead)."); RETURN(0) }
				lua_settop(L, 0);
				lua_pushnumber(L, vpn->Num);
				RETURN(1)
			}
			if (vtype == vtZ)  goto _string;
			goto _normVar;
		}

		i = ToInteger(L, 2);
		StrCopy(ErrorCmd.Name, sizeof(ErrorCmd.Name), (c != 0) ? Format("ERM.%c%d", c, i) : Format("ERM[%d]", c, i));

		switch (c)
		{
			case 0: // flag
				if((i<1)||(i>1000)){ lua_settop(L, 0); MError2("wrong flag index (1...1000)."); RETURN(0) }
				if(lua_gettop(L) < 3)
				{
					lua_settop(L, 0);
					lua_pushboolean(L, ERMFlags[i-1]);
				}
				else
				{
					ERMFlags[i-1] = lua_toboolean(L, 3);
					lua_settop(L, 0);
					lua_pushboolean(L, 1);
				}
				RETURN(1)
			case 'z':
_string:
				if(lua_gettop(L) < 3)
				{
					lua_settop(L, 0);
					if((i<-VAR_COUNT_LZ)||(i==0)){ MError2("wrong z var index (-20...-1,1...1000+)."); RETURN(0) }
					char buf[512];
					StrCopy(buf, 512, GetErmString(i));
					lua_pushstring(L, buf);
				}
				else
				{ // set
					const char* str = lua_tostring(L, 3);
					if((i<-VAR_COUNT_LZ)||(i==0)||(i>1000)){ lua_settop(L, 0); MError("wrong z var index (-20...-1,1...1000)."); RETURN(0) }
					StrCopy(GetPureErmString(i),512,str);
					lua_settop(L, 0);
					lua_pushboolean(L, 1);
				}
				RETURN(1)
			case 'e':
				if((i<-100)||(i==0)||(i>100)){ lua_settop(L, 0); MError("wrong e var index (-100...-1,1...100)."); RETURN(0) }
				float *var;
				if(i>0) var = &ERMVarF[i-1];
				else    var = &ERMVarFT[-i-1];
				if(lua_gettop(L) < 3)
				{
					lua_settop(L, 0);
					lua_pushnumber(L, *var);
				}
				else
				{
					*var = lua_tonumber(L, 3);
					lua_settop(L, 0);
					lua_pushboolean(L, 1);
				}
				RETURN(1)
			case 'c':
				StrCopy(ErrorCmd.Name, sizeof(ErrorCmd.Name), Format("ERM.%c", c));
				if(lua_gettop(L) < 3)
				{
					lua_settop(L, 0);
					lua_pushnumber(L, GetCurDate());
				}
				else
				{
					SetCurDate(ToInteger(L, 3));
					lua_settop(L, 0);
					lua_pushboolean(L, true);
				}
				RETURN(1)
			case 'v':  vtype = 3;  break;
			case 'w':  vtype = 4;  break;
			case 'x':  vtype = 5;  break;
			case 'y':  vtype = 6;  break;
			default:
				StrCopy(ErrorCmd.Name, sizeof(ErrorCmd.Name), Format("ERM.%c", c));
				vtype = 2;
				i = c - 'e';
		}

_normVar:
		int* p = GetVarAddress(i, vtype);
		if (p == 0) RETURN(0)

		if (lua_gettop(L) < 3)
		{
			lua_settop(L, 0);
			lua_pushnumber(L, *p);
		}
		else
		{
			*p = ToInteger(L, 3);
			lua_settop(L, 0);
			lua_pushboolean(L, 1);
		}
	//}
	//__finally
	//{
		ErrorCmd.Cmd = oldCmd;
		ErrString = LastErrString;
	//}
	RETURN(1)
}

static bool StrCmp16(const char *str1, const char *str2)
{
	for (int i = 0; i<16; i++)
	{
		if (str1[i] != str2[i]) return false;
		if (str1[i] == 0) return true;
	}
	return true;
}

static int ERM_SetMacro(lua_State *L)
{
	const char *src = ToString(L, 1);
	char c = *ToString(L, 2);
	int i = ToInteger(L, 3);
	char *dest;

	switch (c)
	{
		case 0: // flag
		{
			int j = 0;
			while (j < 1000 && ERMMacroName[j][0] != 0 && !StrCmp16(src, ERMMacroName[j])) j++;
			if(j==1000){ lua_settop(L, 0); lua_pushstring(L, "cannot declare more macros for numbers (1000)."); return 1; }
			dest = ERMMacroName[j];
			ERMMacroVal[j] = i;
			break;
		}
		case 'v':
			if((i<1)||(i>VAR_COUNT_V)){ lua_settop(L, 0); lua_pushstring(L, "var is out of set (v1...v10000)."); return 1; }
			dest = ERMVar2Macro[i-1];
			break;
		
		case 'w':
			if((i<1)||(i>200)){ lua_settop(L, 0); lua_pushstring(L, "var is out of set (w1...w200)."); return 1; }
			dest = ERMVarHMacro[i-1];
			break;

		case 'z':
			if((i<1)||(i>1000)){ lua_settop(L, 0); lua_pushstring(L, "var is out of set (z1...z1000)."); return 1; }
			dest = ERMStringMacro[i-1];
			break;

		default:
			i = c - 'f';
			if ((i<1)||(i>15)){ lua_settop(L, 0); lua_pushstring(L, "var is out of set (f...t)."); return 1; }
			dest = ERMVarMacro[i-1];
	}
	
	for (i = 0; i < 16 && src[i]; i++) dest[i] = src[i];
	for (; i < 16; i++) dest[i] = 0;
	lua_settop(L, 0);
	return 0;
}

static int ERM_CheckScript(lua_State *L)
{
	Mes M;
	M.m.s = (char*)(Dword)luaL_checknumber(L, 1);
	M.m.l = (long)luaL_checknumber(L, 2);
	lua_pushboolean(L, M.m.s != 0 && CheckERM(&M));
	return 1;
}

static int ERM_DynString(lua_State *L)
{
	lua_pushnumber(L, StrMan::Push((char*)(Dword)luaL_checknumber(L, 1), (int)luaL_checknumber(L, 2)));
	return 1;
}

static int ERM_RunScript(lua_State *L)
{
	int i = ToInteger(L, 1);
	if (i < 0 || i >= StrMan::Count)
		luaL_error(L, "Internal error. Script index (%d) out of bounds (0..%d)", i, StrMan::Count-1);
	Mes M;
	M.i = 0;
	M.m = StrMan::ERMDynString[i].m;
	lua_pushboolean(L, CheckERM(&M) && !ParseERM(M));
	StrMan::ERMDynString[i].m.l = M.m.l;
	return 1;
}

static int ERM_CheckWogify(lua_State *L)
{
	lua_pushboolean(L, CheckWogify(lua_toboolean(L, 1), lua_toboolean(L, 2), lua_toboolean(L, 3), lua_toboolean(L, 4)));
	return 1;
}

//-------- Messages ---------

int PanicMessage(lua_State *L)
{
	const char* str = "nil";
	if (lua_gettop(L) != 0)
	{
		str = ToString(L, 1);
	}
	ErrorMessage(str);
	lua_settop(L, 0);
	return 0;
}

static int LuaMessage(lua_State *L)
{
	int ret = 0;
	if (GameLoaded)
	{
		ret = Request3Pic((char*)ToString(L, 1), ToInteger(L, 3), ToInteger(L, 4), ToInteger(L, 5), ToInteger(L, 6),
			ToInteger(L, 7), ToInteger(L, 8), ToInteger(L, 2), ToInteger(L, 11), ToInteger(L, 9), ToInteger(L, 10));
	}
	else
	{
		ret = (MessageBox(0, ToString(L, 1), ToString(L, 12), (ToInteger(L, 2) == 2 ? MB_YESNO : 0)) == IDYES);
	}
	lua_settop(L, 0);
	lua_pushnumber(L, ret);
	return 1;
}

static int MultiPicMessage(lua_State *L)
{
	Dword D8D_Struct[4];
	int pics[100];
	int n = lua_gettop(L) - 3;
	n = n > 100 ? 100 : n;
	const char *msg = ToString(L, 1);
	int x = ToInteger(L, 2);
	int y = ToInteger(L, 3);
	for (int i = 0; i < n; i++)
		pics[i] = ToInteger(L, i + 4);
	STARTNA(__LINE__, 0)
	D8D_Struct[0]=1;
	D8D_Struct[1]=(Dword)&pics[0];
	D8D_Struct[2]=(Dword)&pics[n];
	D8D_Struct[3]=(Dword)&pics[n];
	__asm{
		mov   ecx,msg
		lea   edx,D8D_Struct
		push  15000  // 0,15000
		push  y
		push  x
		mov   eax,0x4F7D20
		call  eax
	}
	lua_settop(L, 0);
	RETURN(0)
}

//-------- Error Function ---------

static int ErrorFunction(lua_State *L)
{
	lua_checkstack(L, 2);
	lua_rawgeti(L, LUA_REGISTRYINDEX, ErrfIndex);
	if (lua_type(L, -1) == LUA_TNIL)
	{
		ErrorMessage("Fatal Lua error: error function is missing");
		lua_pop(L, 1);
		return 1;
	}
	lua_insert(L, -2);
	lua_pcall(L, 1, 1, 0);
	return 1;
}

//-------- Remove Error Function ---------

//static int RemoveDefErrf(lua_State *L)
//{
//	luaL_checktype(L, 1, LUA_TTHREAD);
//	lua_deferrfunc(lua_tothread(L, 1), 0);
//	lua_settop(L, 0);
//	return 0;
//}

//-------- xpcall ---------

static int lua_xpcall (lua_State *L) {
	luaL_checktype(L, 2, LUA_TFUNCTION);

	// switch function & error function
	lua_pushvalue(L, 1);
	lua_pushvalue(L, 2);
	lua_replace(L, 1);
	lua_replace(L, 2);

	// call
	lua_pushboolean(L, 0 == lua_pcall(L, lua_gettop(L) - 2, LUA_MULTRET, 1));
	lua_replace(L, 1);
	return lua_gettop(L);
}

//-------- random support ---------

int DelphiRSeed;

_declspec( naked ) int __fastcall DelphiRand(int /*range*/)
{
	_asm
	{
		mov eax, ecx
		push ebx
		xor ebx, ebx
		imul edx, DelphiRSeed[ebx], 0x08088405
		inc edx
		mov DelphiRSeed, edx
		mul edx
		mov eax, edx
		pop ebx
		ret
	}
}

//-------- os.find support ---------

typedef struct MY_WIN32_FIND_DATA
{
	WIN32_FIND_DATA d;
	HANDLE h;
	DWORD attr; //, attrF;
	bool started;
} *PMY_WIN32_FIND_DATA;

static int LuaFindStart(lua_State *L)
{
	char file[MAX_PATH];
	strncpy(file, ToString(L, 1), sizeof(file) - 1);
	file[sizeof(file) - 1] = 0;
	DWORD attr = (lua_toboolean(L, 2) ? FILE_ATTRIBUTE_DIRECTORY : 0);
	lua_settop(L, 0);

	PMY_WIN32_FIND_DATA data = (PMY_WIN32_FIND_DATA)lua_newuserdata(L, sizeof(MY_WIN32_FIND_DATA));
	if (data == 0) return 0;

	GetFullPathName(file, sizeof(file), file, 0);
	HANDLE fh = FindFirstFile(file, &data->d);
	if (fh == INVALID_HANDLE_VALUE) return 0;

	// Push userdata
	data->h = fh;
	//data->attrF = FILE_ATTRIBUTE_DIRECTORY;
	data->attr = attr;
	data->started = false;
	lua_newtable(L);
	lua_setmetatable(L, 1);

	// Push directory of the files
	int i = strlen(file) - 1;
	while ((i >= 0) && (file[i] != '/') && (file[i] != '\\'))  i--;
	lua_pushlstring(L, file, i+1);
	lua_pushnumber(L, (DWORD)&data->d);

	return 3;
}

static int DoFindNext(PMY_WIN32_FIND_DATA data)
{
	if (data->h == 0)
		return 0;

	if (FindNextFile(data->h, &data->d) == 0)
	{
		FindClose(data->h);
		data->h = 0;
		return 0;
	}
	return 1;
}

static int LuaFindNext(lua_State *L)
{
	PMY_WIN32_FIND_DATA data = (PMY_WIN32_FIND_DATA)lua_touserdata(L, 1);
	lua_settop(L, 0);
	if (data->h == 0) return 0;
	DWORD attr = data->attr;
	const DWORD attrF = FILE_ATTRIBUTE_DIRECTORY; // data->attrF;
	if (data->started && DoFindNext(data) == 0)
		return 0;
	data->started = true;
	while ((data->d.dwFileAttributes & attrF) != attr || *(WORD*)data->d.cFileName == '.' || (*(DWORD*)data->d.cFileName & 0xFFFFFF) == '.' + '.'*256)
		if (DoFindNext(data) == 0)
			return 0;
	lua_pushstring(L, data->d.cFileName);
	lua_pushnumber(L, (lua_Number)(((long long)data->d.nFileSizeHigh) << 32) + (long long)data->d.nFileSizeLow);
	return 2;
}

static int LuaFindClose(lua_State *L)
{
	PMY_WIN32_FIND_DATA data = (PMY_WIN32_FIND_DATA)lua_touserdata(L, 1);
	if (data->h)
	{
		FindClose(data->h);
		data->h = 0;
	}
	lua_settop(L, 0);
	return 0;
}

//-------- Misc ---------

#ifdef _DEBUG_LUA

#define Loader(P,L) Dbg_Loader(P,L)
#define Saver(P,L) Dbg_Saver(P,L)

HANDLE fh = INVALID_HANDLE_VALUE;
int lastSeek = 0;

int Dbg_Loader(void *Po,Dword Len)
{
	if (fh == INVALID_HANDLE_VALUE)
		fh = CreateFile("c:\\aa.dat", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fh == INVALID_HANDLE_VALUE)
		ErrorMessage(Format("%d", GetLastError()));
	if (ReadFile(fh, Po, Len, &Len, 0) == 0)
		ErrorMessage(Format("%d", GetLastError()));
	lastSeek += Len;
	return lastSeek;
}

int Dbg_Saver(const void *Po,Dword Len)
{
	if (fh == INVALID_HANDLE_VALUE)
		fh = CreateFile("c:\\aa.dat", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (fh == INVALID_HANDLE_VALUE)
		ErrorMessage(Format("%d", GetLastError()));
	if (WriteFile(fh, Po, Len, &Len, 0) == 0)
		ErrorMessage(Format("%d", GetLastError()));
	lastSeek += Len;
	return lastSeek;
}
#endif

//-------- Persistance support ---------

static int MySaver(lua_State *L)
{
	size_t n;
	const char * str = lua_tolstring(L, 1, &n);
	Saver((void*)&n, 4);
	Saver((void*)str, n);
	lua_settop(L, 0);
	return 0;
}

static int MyLoader(lua_State *L)
{
	size_t n;
	Loader((void*)&n, 4);
	void* str = malloc(n);
	Loader(str, n);
	lua_pushlstring(L, (char*)str, n);
	free(str);
	return 1;
}

//-------- Debug Console ---------

char * (__stdcall *DebugDialog)(HWND wnd, const char * text, int top);
void (__stdcall *DebugDialogResize)(long w, long h);

static int DebugConsole(lua_State *L)
{
	const char * s;
	if (lua_isstring(L, 1))
	{
		s = lua_tostring(L, 1);
		if (s == 0)
			s = "";
	}
	else
	{
		s = 0;
	}
	ShowCursor(true);
	s = DebugDialog(MainWindow, lua_tostring(L, 1), lua_toboolean(L, 2));
	ShowCursor(false);
	lua_settop(L, 0);
	lua_pushstring(L, s);
	return 1;
}

//-------- Delphi Memory Manager ---------

void * ReallocMem; // function MyReallocMem(p: ptr; size: int): ptr;

__declspec(naked) void * LuaAlloc(void *, void *, size_t, size_t){__asm
{
	mov eax, [esp + 8]
	mov edx, [esp + 16]
	jmp ReallocMem
}}

//-------- Other packages ---------

#include "luaW_dlg.h"
#undef __FILENUM__
#define __FILENUM__ 25

int LuaGetHeroGod(lua_State *L);
int LuaHeroHasBlessCurse(lua_State *L);
int LuaITxt(lua_State *L);

//-------- ERM to Lua calls ---------

int LuaCallTop;

int LuaCallStart(lua_State *L, const char *name)
{
	lua_getfield(L, LUA_REGISTRYINDEX, name); // get 'internal'.name
	if (lua_type(L, -1) != LUA_TFUNCTION)
	{
		lua_pushstring(L, "Fatal Lua error: \"");
		lua_pushstring(L, name);
		lua_pushstring(L, "\" internal function is missing");
		lua_concat(L, 3);
		ErrorMessageExe(L);
		lua_pop(L, 1);
		return 1;
	}
	LuaCallTop = lua_gettop(L);

	return 0;
}

int LuaCallStart(const char *name)
{
	return LuaCallStart(Lua, name);
}

int LuaPCall(int nArgs, int nResults)
{
	if (lua_pcall(Lua, nArgs, nResults, 0))
	{
		ErrorMessage(Lua);
		return 1;
	}
	return 0;
}

int LuaCallEnd(lua_State *L, int code, int n)
{
	if (code) return 0;
	if (LuaPCall(n, 1)) return 0;
	int r = 0;
	if (lua_type(L, -1) == LUA_TNUMBER)
		r = ToInteger(L, -1);
	else if (lua_isboolean(L, -1))
		r = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return r;
}

int LuaCallEnd()
{
	return LuaCallEnd(Lua, 0, lua_gettop(Lua) - LuaCallTop);
}

int DoFile(const char *path)
{
	if (luaL_loadfile(Lua, path))
	{
		ErrorMessage(Lua);
		return 1;
	}
	return LuaPCall(0, 0);
}

void LuaInternalConst(const char *name, int val)
{
	lua_pushnumber(Lua, val);
	lua_setfield(Lua, LUA_REGISTRYINDEX, name);
}

void LuaInternalConst(const char *name, const char *val)
{
	lua_pushstring(Lua, val);
	lua_setfield(Lua, LUA_REGISTRYINDEX, name);
}

void LuaInternalIniPath(char *name, char *def)
{
	char buf[MAX_PATH];
	LoadIniPath(buf, name, def);
	lua_pushstring(Lua, buf);
	lua_setfield(Lua, LUA_REGISTRYINDEX, name);
}

const char* LuaPushERMInfo(char *erm, bool OneLine)
{
	LuaCallStart("ERMErrorInfo");
	lua_pushnumber(Lua, (Dword)erm);
	lua_pushboolean(Lua, OneLine);
	LuaPCall(2, 1);
	return lua_tostring(Lua, -1);
}

//-------- Actual calls ---------

void CallLuaTrigger(int index)
{
	LuaCall("CallTrigger", 0.0, index);
	WasErmError = false;
}

static const struct luaL_reg LuaLib_general [] =
{
	{"xpcall", lua_xpcall},
	{0, 0}
};

static const struct luaL_reg LuaLib_internal [] =
{
	{"ERM_Reciever", ERM_Reciever},
	{"ERM_Call", ERM_Call},
	{"ERM_Trigger", ERM_Trigger},
	{"ERM_Var", ERM_Var},
	{"ERM_SetMacro", ERM_SetMacro},
	{"ERM_CheckScript", ERM_CheckScript},
	{"ERM_DynString", ERM_DynString},
	{"ERM_RunScript", ERM_RunScript},
	{"ERM_CheckWogify", ERM_CheckWogify},
	//{"ERM_LoadERS", ERM_LoadERS},
	{"LuaGetLastError", LuaGetLastError},
	{"Saver", MySaver},
	{"Loader", MyLoader},
	{"FindStart", LuaFindStart},
	{"FindNext", LuaFindNext},
	{"FindClose", LuaFindClose},
	{"LuaMessage", LuaMessage},
	{"MultiPicMessage", MultiPicMessage},
	{"ErrorMessage", ErrorMessageLua},
	{"DebugConsole", DebugConsole},
	{"GetTileVTables", GetTileVTables},
	{"GetDrawPosVTables", GetDrawPosVTables},
	{"GetEditVTables", GetEditVTables},
	{"GetHeroGod", LuaGetHeroGod},
	{"HeroHasBlessCurse", LuaHeroHasBlessCurse},
	{"ITxt", LuaITxt},
	{0, 0}
};

void InitLua()
{
	char Path[MAX_PATH];
	Path[0] = 0;
	sprintf_s(Path, "%sWogDialogs.dll", (DeveloperPath[0] ? DeveloperPath : ""));
	*(PROC*)&DebugDialog = DllImport(Path, "DebugDialog", true);
	*(PROC*)&DebugDialogResize = DllImport(Path, "DebugDialogResize", true);
	*(PROC*)&ReallocMem = DllImport(Path, "ReallocMem", true);
	DebugDialogResize(800-40, 600-40);

	for (int i = 0; i < 16; i++)
		CmdMessage.c[i] = 1;

	lua_State* L = Lua = lua_newstate(LuaAlloc, 0);  // Open Lua
	lua_atpanic(L, PanicMessage);
	lua_ignorelocale(L);
	lua_alnum(L)['?'] = 1;
	lua_alnum(L)['$'] = 1;

	luaL_openlibs(L); // Open all standard libraries
	luaopen_upvaluejoin(L); // for more RSPersist power that isn't actually used
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, 0, LuaLib_general);
	lua_pop(L, 1);
	lua_pushvalue(L, LUA_REGISTRYINDEX);
	luaL_register(L, 0, LuaLib_internal);
	lua_pop(L, 1);
	RSMemRegister(L);

	// load luasocket
	lua_getfield(L, LUA_GLOBALSINDEX, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, luaopen_socket_core);
	lua_setfield(L, -2, "socket.core");
	lua_pop(L, 2);

	// coroutine.main
	lua_getglobal(L, "coroutine");
	lua_pushthread(L);
	lua_setfield(L, -2, "main");
	lua_pop(L, 1);

	lua_pushlightuserdata(L, (void*)CHECK_V);
	lua_setglobal(L, "?v");
	lua_pushlightuserdata(L, (void*)CHECK_Z);
	lua_setglobal(L, "?z");
	lua_pushlightuserdata(L, (void*)CHECK_PTR);
	lua_setglobal(L, "?ptr");

	LuaInternalConst("GetCurrentDirectory", (int)GetCurrentDirectory);
	LuaInternalConst("SetCurrentDirectory", (int)SetCurrentDirectory);
	LuaInternalConst("CreateDirectory", (int)CreateDirectory);
	LuaInternalConst("GetLastErrorPtr", (int)GetLastError);
	LuaInternalConst("GetKeyStatePtr", (int)GetKeyState);
	LuaInternalConst("ReadFile", (int)ReadFile);
	LuaInternalConst("SetFilePointer", (int)SetFilePointer);

	LuaInternalConst("VidArr", (int)&VidArr);
	LuaInternalConst("ResetERMMainMenuPtr", (int)&ResetERMMainMenu);
	LuaInternalConst("ErmDynString", (int)&StrMan::ERMDynString);
	LuaInternalConst("ScrollCallbackPtr", (int)&ScrollCallback_asm);
	LuaInternalConst("HasAnyAtThisPCPtr", (int)&HasAnyAtThisPC);
	LuaInternalConst("WogOptionsPtr", (int)PL_WoGOptions);
	LuaInternalConst("EnableMovingMonstersPtr", (int)&EnableMovingMonsters);
	//LuaInternalConst("PL_ApplyWoG", (int)&PL_ApplyWoG);
	//LuaInternalConst("timeGetTime", (int)DllImport("winmm.dll", "timeGetTime", true));
	LuaInternalConst("WogVersion", WOG_STRING_VERSION);
	LuaInternalConst("DeveloperPath", DeveloperPath);
	LuaInternalConst("ModsPath", ModsPath);
	LuaInternalConst("ERM_HeroStr", (int)&ERM_HeroStr);

	// Scripts path
	LoadIniPath(Path, "LuaScriptsPath", (DeveloperPath[0] ? Format("%sLua\\", DeveloperPath) : "Data\\zvs\\Lua\\"));
	lua_pushstring(Lua, Path);
	lua_setfield(Lua, LUA_REGISTRYINDEX, "CoreScriptsPath");

	// Error function
	if (luaL_loadfile(L, Format("%sErrorFunction.lua", Path)) || lua_pcall(L, 0, 1, 0))
	{
		ErrorMessage(Lua);
		ExitProcess(0);
	}
	ErrfIndex = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_deferrfunc(L, ErrorFunction);

	// main.lua
	if (DoFile(Format("%smain.lua", Path))) exit(0);
	//LoadLibrary("MallocHook.dll");
	SetCurrentDirectory(AppPath);
}
