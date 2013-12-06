#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "b1.h"
//#include "casdem.h"
//#include "service.h"
//#include "herospec.h"
//#include "monsters.h"
//#include "anim.h"
//#include "womo.h"
//#include "sound.h"
//#include "npc.h"
#include "erm.h"
#include "erm_lua.h"
#include "string.h"
#include "prot.h"
#define __FILENUM__ 14
///////////////////////////
String::~String(){ Clear(); }
//  String(String &);
String::String(){Ind=0; Text=0;}
void String::Set(int ind,char *txt){
	STARTNA(__LINE__,txt) //  STARTC("String: Set",txt)
	if(ind<1){ MError("String: Index is below 1, Item is ignored."); }
	if(ind<1000){ // установим и z переменную также
		for(int i=0;i<512;i++){
		 ERMString[ind-1][i]=txt[i];
		 if(txt[i]==0) break;
		}
		ERMString[ind-1][511]=0;
	}
	Ind=ind;
	Text=txt;
	RETURNV
}
void String::Clear(){ if(Text) Free(Text); Text=0; Ind=0; }
int String::GetInd(){ return Ind; }
char *String::GetText(){ return Text; }

//  StringSet(StringSet &);
//  StringSet();
// ~StringSet();
//  StringSet &operator=(StringSet &);
void StringSet::Clear()
{
	LuaCall("ClearERT");
}

char *StringSet::GetText(int ind){
	return (char*)LuaCall("GetERT", ind);
}

bool StringSet::Belongs(const char *str){
	lua_getfield(Lua, LUA_REGISTRYINDEX, "StrToERT");
	lua_rawgeti(Lua, -1, (Dword)str);
	bool ret = !lua_isnil(Lua, -1);
	lua_pop(Lua, 2);
	return ret;
}

// Dynamic strings manager

_AMes_ StrMan::ERMDynString[16384];
int StrMan::NextStr;
int StrMan::Count;

int StrMan::Push(char * str, int len) // allocates a copy of str and returns its index in ERMDynString array
{
	int ret = NextStr;
	if (ret == Count)
		NextStr = Count = ret + 1;
	else
		NextStr = ERMDynString[ret].a;

	ERMDynString[ret].a = 0;
	ERMDynString[ret].m.s = 0;
	ERMDynString[ret].m.l = 0;
	if (str == 0) str = "";
	if (len < 0) len = strlen(str);
	char* (__fastcall *SetStr)(Dword self, Dword _, const char *a2, int len);  *(Dword*)&SetStr = 0x404180;
	SetStr((Dword)&ERMDynString[ret].m.s - 4, 0, str, len);
	return ret;
}

void StrMan::Remove(int Index)
{
	MesMan(&ERMDynString[Index], 0, 0);
	ERMDynString[Index].m.s = 0;
	ERMDynString[Index].m.l = -1;
	ERMDynString[Index].a = NextStr;
	NextStr = Index;
	if (Index == Count - 1)
		Count = Index;
}

void StrMan::Reset()
{
	GEr.LastERM(0);
	for (int i = NextStr - 1; i >= 0; i--)
		if (ERMDynString[i].m.s != 0)
		{
			MesMan(&ERMDynString[i], 0, 0);
			ERMDynString[i].m.s = 0;
			ERMDynString[i].m.l = -1;
		}
	
	NextStr = Count = 0;
}

int StrMan::Save()
{
	if (Saver(&Count,sizeof(Count))) return 1;
	for (int i = 0; i < Count; i++)
	{
		if (Saver(&(ERMDynString[i].m.l), 4)) return 1;
		if (ERMDynString[i].m.l > 0)
			if (Saver(ERMDynString[i].m.s, ERMDynString[i].m.l)) return 1;
	}
	return 0;
}

int StrMan::Load()
{
	char* (__fastcall *alloc)(Dword self, Dword _, int len);  *(Dword*)&alloc = 0x404B80;
	Reset();
	if (Loader(&Count,sizeof(Count))) return 1;
	int size = 0;
	for (int i = 0; i < Count; i++)
	{
		if (Loader(&size, 4)) return 1;
		if (size > 0)
		{
			ERMDynString[i].m.l = ERMDynString[i].a = 0;
			alloc((Dword)&ERMDynString[i] - 4, 0, size); // ERMString is used as a buffer bigger than possible string size
			ERMDynString[i].m.l = size;
			ERMDynString[i].m.s[size] = 0;
			if (Loader(ERMDynString[i].m.s, size)) return 1;

		} else
		{
			ERMDynString[i].m.l = size;
		}
	}

	NextStr = Count;
	for (int i = Count-1; i >= 0; i--)
		if (ERMDynString[i].m.l < 0)
		{
			ERMDynString[i].a = NextStr;
			NextStr = i;
		}
			
	return 0;
}

char *StrMan::GetStoredStr(int ind, char *backup)
{
	if (ind > 1000) return StringSet::GetText(ind);
	if (ind > 0) return ERMString[ind-1];
	if (ind < 0) return ERMDynString[-ind-1].m.s;
	return backup;
}

int inline _Apply(void *dp,char size,Mes *mp,char ind) { return Apply(dp, size, mp, ind); }

int StrMan::Apply(char* &str, char* backup, int &var, Mes* Mp, int ind) // returns 1 if the string was changed
{
	int strType = Mp->VarI[ind].Type;
	if (strType == 0 && Mp->n[ind] == 0 && Mp->VarI[ind].Check==0 && Mp->m.s[Mp->i] == STRCH)
		strType = -1;

	if (strType == 7 || strType == -1)
	{
		int i = -var-1;
		if (i < 0)  i = Push(str);
		if (strType == -1)
		{
			int L;
			MesMan(&ERMDynString[i], ERM2String(&Mp->m.s[Mp->i],0,&L), 0);
			Mp->i += L;
		}
		else		
		{
			ApplyString(Mp, ind, &ERMDynString[i]);
		}
		if (var == 0)
			if(StrCmpOk(str, ERMDynString[i].m.s)) // not changed
			{
				Remove(i);
				return 0; // not changed
			}
			else
			{
				var = -i-1;
				str = ERMDynString[i].m.s;
			}
	}
	else
	{
		int i = var;
		if (_Apply(&i, 4, Mp, ind)) return 0;
		if (i != var)
		{
			if (i < 0){ MError2("wrong z var index (1...1000+)."); return 0; }
			if (var<0) Remove(-var-1);
			if (var == i) return 0; // not changed
			var = i;
			str = GetStoredStr(i, backup);
		}
	}
	return 1;
}

int StrMan::Apply(char* str, Mes* Mp, int ind, int bufsize) // returns 1 if the string was changed
{
	int strType = Mp->VarI[ind].Type;
	if (strType == 7)
		if(Mp->VarI[ind].Check==1)
		{ // ?
			int i = GetVarIndex(&Mp->VarI[ind], false);
			if(i>0)       StrCopy(ERMString[i-1],bufsize,str);
			else if(i<0)  StrCopy(ERMLString[-i-1],bufsize,str);
			return 0;
		}
		else
		{ // set
			int i = GetVarIndex(&Mp->VarI[ind], true);
			const char * es = (i ? GetErmString(i) : 0);
			if (es) StrCopy(str, bufsize, es);
			return 1;
		}
	else
		if (strType == 0 && Mp->n[ind] == 0 && Mp->VarI[ind].Check==0 && Mp->m.s[Mp->i] == STRCH)
		{
			int L;
			StrCopy(str, bufsize, ERM2String(&Mp->m.s[Mp->i], 0, &L));
			Mp->i += L;
			return 1;
		}
	MError2("string expected.");
	return 0;
}
