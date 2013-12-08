#include "dlg.h"

//Dword __fastcall iActivate(_Body_ *body,int,int par1,int par2){
//	STARTNA(__LINE__, 0)
//	int vind=body->VInd;
//	if(vind>=0){
//		void (__fastcall *fn)(int vind,int x,int y,int p1,int p2,int p3,int p4,int p5); *(Dword *)&fn=0x597870;
//		int vx=body->VX;
//		int vy=body->VY;
//		fn(vind,vx,vy,0,0,1,1,1);
//	}
//	Dword (__fastcall *fn2)(_Body_ *dlg,int,int par1,int par2); *(Dword *)&fn2=0x5FF0A0;
//	Dword ret=fn2(body,0,par1,par2);
//	if(ret==0){
//		void (__fastcall *fn3)(void); *(Dword *)&fn3=0x5978F0;
//		fn3();
//	}
//	RETURN(ret)
//}
//
//Dword __fastcall HDlg::iDisactivate(_Body_ *body,int,int par1){
//	STARTNA(__LINE__, 0)
////  int vind=*(Dword *)&dlg[0x70];
//	int vind=body->VInd;
//	if(vind>=0){
//		void (__fastcall *fn3)(void); *(Dword *)&fn3=0x5978F0;
//		fn3();
//	}
//	Dword (__fastcall *fn2)(_Body_ *dlg,int,int par1); *(Dword *)&fn2=0x5FF220;
//	Dword ret=fn2(body,0,par1);
//	RETURN(ret)
//}

struct _DlgItem_{
	void** VMT;
	void*** ParentPtr;
	_DlgItem_* NextDlgItem;
	_DlgItem_* NextVisibleItem;
	short Id;
	short field_12;
	short Flags;
	short State;
	short X;
	short Y;
	short Width;
	short Height;
	int field_20;
	int field_24;
	Byte field_28;
	Byte field_29;
	Byte field_2A;
	Byte field_2B;
	int field_2C;
};

//-------- TileDraw ---------

void __fastcall TileDraw(_DlgItem_* self, int dx, int dy, Dword defDraw)
{
	void (__fastcall *fn)(_DlgItem_*);  *(Dword*)&fn = defDraw;
	int w = self->Width;
	int h = self->Height;
	int bx = self->X;
	int by = self->Y;
	self->Height = dy;
	for (int y = 0; y < h; y += dy)
	{
		self->Y = by + y;
		if (y + dy > h)  self->Height = h - y;
		self->Width = dx;
		for (int x = 0; x < w; x += dx)
		{
			self->X = bx + x;
			if (x + dx > w)  self->Width = w - x;
			fn(self);
		}
	}
	self->X = bx;
	self->Y = by;
	self->Width = w;
	self->Height = h;
}

void __fastcall TileDef_Draw(_DlgItem_* self)
{
	int def = *(int*)((int)self + 0x30);
	if (def)
		TileDraw(self, *(int*)(def + 0x30), *(int*)(def + 0x34), 0x4EAC20);
}

int VTable_TileDef[14];


void __fastcall TilePcx_Draw(_DlgItem_* self)
{
	int pcx = *(int*)((int)self + 0x30);
	if (pcx)
		TileDraw(self, *(int*)(pcx + 0x24), *(int*)(pcx + 0x28), 0x450120);
}

int VTable_TilePcx[14];


void __fastcall TilePcx16_Draw(_DlgItem_* self)
{
	int pcx = *(int*)((int)self + 0x30);
	if (pcx)
		TileDraw(self, *(int*)(pcx + 0x24), *(int*)(pcx + 0x28), 0x450460);
}

int VTable_TilePcx16[14];


int GetTileVTables(lua_State *L)
{
	memcpy((void*)VTable_TileDef, (void*)0x63EC48, 14*4);
	VTable_TileDef[4] = (int)TileDef_Draw;
	memcpy((void*)VTable_TilePcx, (void*)0x63BA94, 14*4);
	VTable_TilePcx[4] = (int)TilePcx_Draw;
	memcpy((void*)VTable_TilePcx16, (void*)0x63BACC, 14*4);
	VTable_TilePcx16[4] = (int)TilePcx16_Draw;
	lua_pushnumber(L, (Dword)&VTable_TileDef);
	lua_pushnumber(L, (Dword)&VTable_TilePcx);
	lua_pushnumber(L, (Dword)&VTable_TilePcx16);
	return 3;
}

//-------- DrawPosButton ---------

void __fastcall DrawButtonAtPos(_DlgItem_* self, int dx, int dy)
{
	if (*(int*)((int)self + 0x30))
	{
		int bx = self->X;
		int by = self->Y;
		int w = self->Width;
		int h = self->Height;
		if ((dx >= w) || (dy >= h))  return;
		self->X = bx + dx;
		self->Y = by + dy;
		self->Width = w - dx;
		self->Height = h - dy;
		
		void (__fastcall *fn)(_DlgItem_*);  *(Dword*)&fn = 0x456620;
		fn(self);

		self->X = bx;
		self->Y = by;
		self->Width = w;
		self->Height = h;
	}
}

void __fastcall DrawPosButton_Draw(_DlgItem_* self)
{
	DrawButtonAtPos(self, *(int*)((int)self + 0x68), *(int*)((int)self + 0x6C));
}

int VTable_DrawPosButton[13];

void __fastcall DrawPosCaptionButton_Draw(_DlgItem_* self)
{
	DrawButtonAtPos(self, *(int*)((int)self + 0x70), *(int*)((int)self + 0x74));
}

int VTable_DrawPosCaptionButton[13];

int GetDrawPosVTables(lua_State *L)
{
	memcpy((void*)VTable_DrawPosButton, (void*)0x63BB54, sizeof(VTable_DrawPosButton));
	VTable_DrawPosButton[4] = (int)DrawPosButton_Draw;
	memcpy((void*)VTable_DrawPosCaptionButton, (void*)0x63BB54, sizeof(VTable_DrawPosCaptionButton));
	VTable_DrawPosCaptionButton[4] = (int)DrawPosCaptionButton_Draw;
	lua_pushnumber(L, (Dword)&VTable_DrawPosButton);
	lua_pushnumber(L, (Dword)&VTable_DrawPosCaptionButton);
	return 2;
}

//-------- Edit ---------

int __fastcall Edit_SetFocused(_DlgItem_* self, int, char focused)
{
	int (__fastcall *fn)(_DlgItem_*,int,char);  *(Dword*)&fn = 0x5BAF00;
	int ret = fn(self, 0, focused);
	LuaCall("Edit_SetFocused", (Dword)self, focused);
	return ret;
}

_MouseStr_ keycmd;

int __fastcall Edit_KeyPressed(_DlgItem_* self, int, _MouseStr_ *cmd)
{
	int (__fastcall *fn)(_DlgItem_*,int,_MouseStr_*);  *(Dword*)&fn = 0x5BB000;
	int ret = fn(self, 0, cmd);
	keycmd = *cmd;
	return LuaCall("Edit_KeyPressed", (Dword)self, (Dword)&keycmd, ret);
}

int __fastcall Edit_IsKeyIgnored(_DlgItem_* self, int, _MouseStr_ *cmd)
{
	int (__fastcall *fn)(_DlgItem_*,int,_MouseStr_*);  *(Dword*)&fn = 0x5BBDD0;
	int ret = fn(self, 0, cmd);
	return LuaCall("Edit_IsKeyIgnored", (Dword)self, (Dword)cmd, ret);
}

int VTable_Edit[19];

int GetEditVTables(lua_State *L)
{
	memcpy((void*)VTable_Edit, (void*)0x642D50, sizeof(VTable_Edit));
	VTable_Edit[14] = (int)Edit_SetFocused;
	VTable_Edit[15] = (int)Edit_KeyPressed;
	VTable_Edit[16] = (int)Edit_IsKeyIgnored;
	lua_pushnumber(L, (Dword)&VTable_Edit);
	return 1;
}

void __fastcall Scroll_Callback(int tick, Dword self)
{
	LuaCall("Scroll_Callback", self, tick);
}

__declspec(naked) void ScrollCallback_asm(){__asm
{
	mov edx, esi
	jmp Scroll_Callback
}}

/*
int __fastcall Dlg_Show(_DlgItem_* self, int, int a2, int a3)
{
	if (LuaFunction("Dlg_Show") == 0)
	{
		lua_pushnumber(Lua, (Dword)self);
		lua_pushnumber(Lua, a2);
		lua_pushnumber(Lua, a3);
		LuaPCall(3, 0);
	}

	int (__fastcall *fn)(_DlgItem_*,int,int,int);  *(Dword*)&fn = 0x5FF0A0;
	return fn(self, 0, a2, a3);
}

int __fastcall Dlg_Hide(_DlgItem_* self, int, int a2)
{
	if (LuaFunction("Dlg_Hide") == 0)
	{
		lua_pushnumber(Lua, (Dword)self);
		lua_pushnumber(Lua, a2);
		LuaPCall(2, 0);
	}

	int (__fastcall *fn)(_DlgItem_*,int,int);  *(Dword*)&fn = 0x5FF220;
	return fn(self, 0, a2);
}


int VTable_Dlg[15];

int GetDlgVTable(lua_State *L)
{
	memcpy((void*)VTable_Dlg, (void*)0x63A6A8, sizeof(VTable_Dlg));
	//VTable_Dlg[1] = (int)Dlg_Show;
	//VTable_Dlg[2] = (int)Dlg_Hide;
	lua_pushnumber(L, (Dword)&VTable_Dlg);
	return 1;
}
*/

//-------- OnDialogAction ---------

/*
int __stdcall OnDialogAction(Dword body, Dword cmd)
{
	STARTNA(__LINE__, 0)
	if (LuaFunction("OnDialogAction")) RETURN(0)
	lua_pushnumber(Lua, body);
	lua_pushnumber(Lua, cmd);
	if (LuaPCall(2, 1)) RETURN(0)
	int ret = ToInteger(Lua, -1);
	lua_remove(Lua, -1);
	RETURN(ret)
}
*/

void * __fastcall DialogVMT0(int **self,int, int del)
{
	STARTNA(__LINE__, 0)
	LuaCall("DialogVMT0", (Dword)self);
	int * my = *self - 1;
	*self = (int*)*my;
	free(my);
	void * (__fastcall *f)(int**,int, int); *(int *)&f= **self;
	RETURN(f(self,0, del));
}

Dword __fastcall DialogVMT3(void *self,int,_MouseStr_ *cmd)
{
	STARTNA(__LINE__, 0)
	RETURN(LuaCall("DialogVMT3", (Dword)self, (Dword)cmd))

/*
	Byte *mman=*(Byte **)0x6992D0;
	int  ct=getTime();
	int *nt=(int *)0x6989E8;
	if(*nt==0){ *nt=ct+100; RETURN(1) }
	int  dif=ct- *nt;
	if(dif>0){
		if(dif<100) dif=100;
		*nt+=dif;
		for(int i=0;i<10;i++){
			HDlg::_Body_::_ADef_ *adef=&body->ADef[i];
			if(adef->Item==0) continue;
			++adef->Cadre;
//__asm int 3
			DlgItem *it=adef->Item;
			int     cdr=adef->Cadre;
//          Byte *it2=*(Byte *)&it[0x30];
			__asm{
				mov   ecx, it
				mov   eax, [ecx+0x30]
				cmp   dword ptr [eax+0x28], 7 
				jle   Lab1
				mov   esi, [eax+0x2C]
				mov   edi, [esi+0x1C]
				test  edi, edi
				jz    Lab1
				mov   eax, [eax+0x1C]
				mov   eax, [eax+0x1C]
				mov   eax, [eax]
				jmp   Lab2
			}
Lab1: __asm{
				xor   eax, eax 
			}
Lab2: __asm{
				mov   edx, cdr
				cmp   edx, eax 
				jl    Lab3
//            mov   dword ptr [ebx+0x60], 0
			}
			adef->Cadre=0;
Lab3:
			cdr=adef->Cadre;
			__asm{
				mov   ecx, it
				mov   edx, cdr
				push  edx
				mov   eax,0x4EB0D0
				call  eax
			}
		}
		int vind=body->VInd;
		if(vind>=0){
			int vx=body->VX;
			int vy=body->VY;
			__asm{  
				mov   eax,0x597C90
				call  eax
				test  al, al  
				jnz   loc_m1 
				push  1
				push  1
				push  1
				push  0
				push  0
				push  vy
				mov   edx, vx
				mov   ecx, vind
				mov   eax,0x597870
				call  eax
//    call DlgItem_OpenVideo                  ; Call Procedure
			}
loc_m1:;
		}
	// animation
		__asm{
//int 3
			mov   ebx, body
			mov   eax, [ebx]
			push  0xFFFF
			push  0xFFFF0001
			push  1
			mov   ecx, ebx
			call  dword ptr [eax+0x14]
		}
	}
	switch(cmd->Type){
		// 4 is called periodically and may be used to set a cursor
		case 4:{ // show hint
			if(body->HintItemId<1) RETURN(1)
			void (__fastcall *fn2)(Byte *,int,_MouseStr_ *cmd); *(Dword *)&fn2=0x602920;
			fn2(mman,0,cmd);
			int previt=*(int *)&mman[0x3C];
			if(previt==cmd->Item) RETURN(1)
			*(int *)&mman[0x3C]=cmd->Item;
			// process item hint here
//        char *hint="This is a Hint";
			char *hint=HDlg::GetHint(body,cmd->Item);
			char *buf=(char *)0x697428;
			if(hint==NULL) buf[0]=0;
			else strncpy(buf,hint,0x1FF);
			void (__fastcall *fn3)(_Body_ *,int,int,int,int,char *buf); *(Dword *)&fn3=0x5FF400;
			fn3(body,0,0x200,3,body->HintItemId,buf);
			void (__fastcall *fn4)(_Body_ *,int,int,int,int); *(Dword *)&fn4=(*(Dword **)body->Obj)[5];
			fn4(body,0,0,7,8);
			void (__fastcall *fn5)(Byte *,int,int,int,int,int); *(Dword *)&fn5=0x603190;
			int p1=*(int *)&body->Obj[0x1C] + 0x16B;
			int p2=*(int *)&body->Obj[0x18] + 8;
			fn5(mman,0,p2,p1,0x138,0x11);
			RETURN(1)
		}
		case 0x200: // mouse push/pop
			Dlg_Cmd=cmd;
			Dlg_User_Command=0;
			DlgCallBack(body->Id,cmd->Item,cmd->SType);
			if(Dlg_User_Command){
				switch(Dlg_User_Command){
					case 1:
						cmd->Type=0x200;
						*(int *)&mman[0x38]=cmd->Item;
						cmd->Item=10;
						cmd->SType=10;
						RETURN(2)
				}
			}
			Dlg_Cmd=NULL;
			RETURN(1)
		default:
			break;
	}

	RETURN(1)
*/
}

Dword __fastcall DialogVMT8(void *self,int, char deact)
{
	return LuaCall("DialogVMT8", (Dword)self, deact);
}

/*
Dword VTable[15]={
	0x5D2900, //0 Dctor  (not like in Lvl1Dlg_Ctor)
	0x5FF0A0, //1 Activate
	0x5FF220, //2 Deactivate
	0x405610, //3 
	0x49A230, //4
	0x5FF5E0, //5
	0x5FFA20, //6
	0x5FFB30, //7
	0x5FFBB0, //8
	0x41B120, //9
	0x5FFCA0, //10
	0x5FFD50, //11
	0x5FFE90, //12
	0x4842C0, //13
	0x41B0F0  //14
};

typedef Byte DlgItem; // temp

int InitDialog(lua_State *L)
{
	byte *Body = (byte*)ToInteger(L, 1);
	*(int*)Body[0] = (int)VTable;
	int numofitems = *(int*)&Body[0x10];
	DlgItem **ip=(DlgItem **)h3new(numofitems*4); if(ip==0) RETURN(0)
	*(DlgItem ***)&Body[0x34]=ip;
	*(DlgItem ***)&Body[0x38]=ip;
	*(DlgItem ***)&Body[0x3C]=&ip[numofitems];
	lua_settop(L, 0);
	return 0;
}
*/

void __fastcall HookModalDialog(int** dlg)
{
	int vmt = (int)*dlg;
	if (vmt != (int)HDlg::VTable && vmt != (int)ShowMapSpellDlg_VTable && ((int)*dlg < 0x400000 || (int)*dlg > 0x6ACEBC))
		return;
	
	int** dest = (int**)malloc(1024);
	dest[0] = *dlg;
	memcpy((char*)++dest, (char*)*dlg, 1020);
	*dlg = (int*)dest;

	dest[0] = (int*)DialogVMT0;
	//dest[3] = (int*)DialogVMT3;
	dest[8] = (int*)DialogVMT8;
}
