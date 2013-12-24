#include <windows.h>
#include <string.h>
#include "structs.h"
#include "erm_lua.h"
#include "dxproxy.h"
#include "common.h"
#include "global.h"
#include "town.h"
#include "erm.h"
#include "casdem.h"
#include "service.h"
#include "ai.h"
#include "lod.h"
#define __FILENUM__ 22

typedef Byte FAR *LPBYTE;

LPPX_DIRECTDRAW		px_DxObj				=	NULL;
const	DWORD		oldMainProcAddress		=	0x4F8290;
//const char  (__thiscall * oldAnalizeEnteredTextProc)(char *,int)= (const char (__thiscall *)(char *,int))0x4022E0;


const	DWORD		newExtendedWindowSize	=	0x300;//768 - Height
char	      AppPath[MAX_PATH];
char	*			MapFastLoadingDir  = "maps";
char	*			MapFastLoadingName  = NULL;

bool PlayingBM_V = 0;
int InGame;

Byte *WogInstStart;
Byte *WogInstEnd;

//static const int HeroesGameType = 3; // 3 = SoD + AB, 2 = only SoD
//static const int HeroesGameTypeStdPo = 0x67F554;


//queue=-1 reserved to debug mode
//queue=-2 reserved to town mode
//queue=-200 reserved to map mode

#define H_PASS1	0x00000000//First pass(default)
#define H_PASS2	0x00000001//Second pass

#define H_CALL		0x00000002//Use call with intellect methods(default?:))
#define H_JMP		0x00000004//Use jump with intellect methods
#define H_ADRO		0x00000008//Write only address

#define H_ADRB		0x00000000//Write back address(default)
#define H_ADRP		0x00000010//Write prime address
#define H_RADR		0x00000020//Write real address(don't set by default)

#define H_ADR0		0x00000000//Write address with 0 byte offset(default)
#define H_ADR1		0x00000040//Write address with 1 byte offset(use if set H_ADR flags)
#define H_ADR2		0x00000080//Write address with 2 byte offset(use if set H_ADR flags)

#define H_CALL5	0x00000100//Use 5 byte-size call
//#define H_CALL6	0x00000200//Use 6 byte-size call(not implemented)
#define H_JMP2		0x00000400//Use 2 byte-size jump if it's possible (EB00, don't use if you don't how many bytes to offset)
#define H_JMP5		0x00000800//Use 5 byte-size jump (E900000000)

#define H_INSR3	0x00002000//Use 3 byte-size instruction
#define H_INSR5	0x00008000//Use 5 byte-size instruction
#define H_INSR6	0x00010000//Use 6 byte-size instruction


#define H_NOP		0x00020000//Write nops up to (ptr)
#define H_PTR		0x00040000//what points to variable which value should be used
//#define H_INSRU	0x00080000//Use instruction field

#define H_COPY		0x00100000//Instead of copiers (uses strings of kind "10 5F76 89AF", spaces between bytes are ignored)
#define H_STR		0x00200000//Instead of copiers for strings (doesn't copy trailing 0)
#define H_STR0		0x00400000//Instead of copiers for strings (copies trailing 0 too)

#define H_BYTE		0x00800000//Write 1 byte as is
#define H_WORD		0x01000000//Write 2 bytes as is
#define H_LONG		0x02000000//Write 4 bytes as is
#define H_DWORD	0x02000000//same


extern struct __newCallers
{
  long  where;
  long  what;
  long  flag;
  long  queue;
  //Byte  instruction;
} newCallers[];

extern struct __newCopiers
{
  long	 where;
  char * what;
  long   len;
  long   flag;
  long   queue;
} newCopiers[];

__inline Byte *FindHInstance(void *p)
{
	MEMORY_BASIC_INFORMATION info;
	VirtualQuery(p, &info, sizeof(info));
	return (Byte*)info.AllocationBase;
}

void newGlobalInitSub()
{
	clearlog();
	//LoadTownsNames();
	newWriteInMemory();
	if(!ReadIntINI(1, "No32Bit"))
		newWriteInMemory(-3);
	if(MapFastLoadingName)
		newWriteInMemory(-200);

	// Find start and end of static memory
	WogInstStart = FindHInstance(&WogInstStart);
	for (WogInstEnd = (Byte*)(((DWORD)&WogInstStart) & ~0xFFF); FindHInstance(WogInstEnd) == WogInstStart; )
		WogInstEnd += 0x1000;

	// AppPath
	GetModuleFileName(0, AppPath, sizeof(AppPath));
	int len = strlen(AppPath) - 1;
	for(; AppPath[len]!='\\' && AppPath[len]!='/'; len--) ;
	AppPath[len + 1] = 0;
	SetCurrentDirectory(AppPath);
}

void newFileLoader()
{
	//if(LoadTownsNames()) Exit();
	//if(LoadBlackSmithDesc()) Exit();
	//newWriteInMemory(-2);
}


HRESULT WINAPI px_DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter )
{
	HRESULT hRes;
	hRes=DirectDrawCreate(lpGUID,lplpDD,pUnkOuter);
	px_DxObj=new px_IDirectDraw(*lplpDD);
	*lplpDD=(LPDIRECTDRAW)px_DxObj;
	return hRes;
}

//int FullScreenChanging = 0;

LRESULT __stdcall doNewMainProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_KEYUP && wp == VK_SNAPSHOT)
		px_DxObj->MakeScreenShot();

	return CallWindowProc((WNDPROC)oldMainProcAddress, wnd, msg, wp, lp);
}

LRESULT CALLBACK newMainProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
	return LuaCall("WindowProc", (Dword)wnd, msg, wp, lp, (Dword)doNewMainProc);
}

//_declspec( naked ) void OnChangeMode()
//{
//	_asm
//	{
//		xor eax, 1
//		mov FullScreenChanging, eax
//		mov eax, 0x601AB0
//		call eax
//		xor ecx, ecx
//		mov FullScreenChanging, ecx
//		ret
//	}
//}


void ParseCommandLine()
{
	 //004F0F36  7414                        		jz 	L004F0F4C
	int argc=0,i=0;
	char **argv;
	argv=CommandLineToArgvA(GetCommandLineA(),&argc);
	if(!argv)//Parse error 
		return;

	for(i=1; i<argc; i++)
	{
		if(i<argc-1 && stricmp(argv[i],"/mapdir")==0)
			MapFastLoadingDir=argv[++i];
		if(i<argc-1 && stricmp(argv[i],"/mapname")==0)
			MapFastLoadingName=argv[++i];
	}
}

byte charToInt(byte ch)
{
	if (ch >= 'A') return ch - 'A' + 10;
	if (ch >= 'a') return ch - 'a' + 10;
	return ch - '0';
}

void doWriteInMemory(int queue, int pass2)
{
	int i,j;
	for(i=0;;i++)
	{
		if(newCopiers[i].where==0 || newCopiers[i].what==0)
			break;
		if(newCopiers[i].queue==queue && (newCopiers[i].flag&H_PASS2)==pass2)
		{
			j=newCopiers[i].len;
			do
			{ 
				*LPBYTE(newCopiers[i].where++)=*newCopiers[i].what++; 
				--j;  
			}
			while(j!=0);
		}
	}
	for(i=0;;i++)
	{
		if(newCallers[i].where==0 || newCallers[i].what==0) 
			break;
		if(newCallers[i].queue==queue && (newCallers[i].flag&H_PASS2)==pass2)
		{
			long flag = newCallers[i].flag;
			long where = newCallers[i].where;
			long WhatPtr;
			if(flag&H_PTR)
				WhatPtr=*((long*)newCallers[i].what);
			else
				WhatPtr=newCallers[i].what;

			Dword OldProtect;
			VirtualProtect((void*)where, 6, PAGE_EXECUTE_READWRITE, &OldProtect); // !!! dirty
			if(flag&H_CALL)
			{
				switch(*((unsigned char*)where))
				{
					case 0xFF:
						*((byte*)where+5)=0x90;
					case 0xE9:
						*((byte*)where)=0xE8;
					case 0xE8:
						*((long*)(where+1))=WhatPtr-where-5;
						break;
					/*
					case 0xFF:
						*((char*)where)=0x90;
						*((char*)where+1)=0xE8;
						*((long*)(where+2))=WhatPtr-where-5;
						break;
					*/	
				}
			}
			else if(flag&H_JMP)
				switch(*((unsigned char*)where))
				{
					case 0xFF:
						*((byte*)where+5)=0x90;
					case 0xE8:
						*((byte*)where)=0xE9;
					case 0xE9:
						*((long*)(where+1))=WhatPtr-where-5;
						break;
					/*
					case 0xFF:
						*((char*)where)=0x90;
						*((char*)where+1)=0xE9;
						*((long*)(where+2))=WhatPtr-where-5;
					*/
				}
			else if(flag&H_ADRO)
			{
				long off=0;
				if(flag&H_ADR1)
					off=1;
				else if(flag&H_ADR2)
					off=2;
				long addr=WhatPtr-where-off-sizeof(long);
				if(flag&H_RADR)
					addr=WhatPtr;
				if(flag&H_ADRP)
					addr = (addr<<24) + ((addr>>8)<<16) + ((addr>>16)<<8) + (addr>>24);
				*((long*)where+off)=addr;
				//0x11223344
				//0x44000000
				//0x00000033
			}
			else if(flag&H_CALL5)
			{
				*((byte*)where)=0xE8;
				*((long*)(where+1))=WhatPtr-where-5;
				if(flag&H_INSR6)
					*((byte*)where+5)=0x90;
			}
			else if(flag&H_JMP5)
			{
				*((byte*)where)=0xE9;
				*((long*)(where+1))=WhatPtr-where-5;
				if(flag&H_INSR6)
					*((byte*)where+5)=0x90;
			}
			else if(flag&H_JMP2)
			{
				*((byte*)where)=0xEB;
				*((char*)(where+1))=char(WhatPtr-where-2);
				if(flag&H_INSR3)
					*((byte*)where+2)=0x90;
				if(flag&H_INSR5)
					*((byte*)where+4)=0x90;
				if(flag&H_INSR6)
					*((byte*)where+5)=0x90;
			}
			else if(flag&H_COPY)
			{
				byte* s = (byte*)WhatPtr;
				for(byte* t = (byte*)where; ; t++)
				{
					while(*s == ' ') s++;
					if(*s == 0) break;
					byte b = charToInt(*s)*16;
					s++;
					*t = b + charToInt(*s);
					s++;
				}
			}
			else if(flag&H_STR)
			{
				char* d = (char*)where;
				for(char* s = (char*)WhatPtr; *s; s++, d++)
					*d = *s;

			}
			else if(flag&H_STR0)
			{
				char* d = (char*)where;
				for(char* s = (char*)WhatPtr; (*d = *s) != 0; s++, d++) ;
			}
			else if(flag&H_NOP)
			{
				for(long d = where; d < WhatPtr; d++)
					*(byte*)d = 0x90;
			}
			else if(flag&H_BYTE)
				*(byte*)where = (byte)WhatPtr;
			else if(flag&H_WORD)
				*(WORD*)where = (WORD)WhatPtr;
			else if(flag&H_LONG)
				*(long*)where = (long)WhatPtr;
			else
			{
				*((byte*)where)=0x90;
				*((byte*)where+1)=0x90;
				if(flag&H_INSR3)
					*((byte*)where+2)=0x90;
				if(flag&H_INSR5)
					*((byte*)where+4)=0x90;
				if(flag&H_INSR6)
					*((byte*)where+5)=0x90;
			}
			VirtualProtect((void*)where, 6, OldProtect, &OldProtect);
			//del=Callers[i].fnew-Callers[i].where-5;
			//*(long *)(Callers[i].where+1)=del;
		}
	}
}

void newWriteInMemory(int queue)
{
	doWriteInMemory(queue, 0);
	doWriteInMemory(queue, H_PASS2);
}


// bad hook code, causes crash in multiplayer
//const	DWORD		oldAnalizeEnteredTextAddress = 0x4022E0;

//const char __stdcall HookCmdLineEnteredText(char *string, int var)
//{
//	int e;__asm{mov e,esi};char r;
//	LogManager.AddDynEntry(TAB_MSG,string);  
//	__asm
//	{
//		push var
//		push string
//		mov ecx,e
//		call oldAnalizeEnteredTextAddress
//		mov r,al
//	}	
//	//addlog("oldAnalizeEnteredTextAddress",r) ;
//	return r;
//}

PCHAR* CommandLineToArgvA(PCHAR CmdLine,int* _argc)
{
	PCHAR* argv;
	PCHAR _argv;
	ULONG  len;
	ULONG  argc;
	CHAR   a;
	ULONG  i, j;
	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;
	len = strlen(CmdLine);
	i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);
	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,i + (len+2)*sizeof(CHAR));
	_argv = (PCHAR)(((PUCHAR)argv)+i);
	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;
	while((a=CmdLine[i])!=0)
	{
		if(in_QM) 
			if(a == '\"') 
				in_QM = FALSE;
			else 
			{
				_argv[j] = a;
				j++;
			}
		else 
		{
			switch(a) 
			{
				case '\"':
					in_QM = TRUE;
					in_TEXT = TRUE;
					if(in_SPACE) 
					{
						argv[argc] = _argv+j;
						argc++;
					}
					in_SPACE = FALSE;
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					if(in_TEXT) 
					{
						_argv[j] = '\0';
						j++;
					}
					in_TEXT = FALSE;
					in_SPACE = TRUE;
					break;
				default:
					in_TEXT = TRUE;
					if(in_SPACE) 
					{
						argv[argc] = _argv+j;
						argc++;
					}
					_argv[j] = a;
					j++;
					in_SPACE = FALSE;
					break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;
	(*_argc) = argc;
	return argv;
}

__declspec(naked) static void GetTerrainOverlayEnd(){__asm
{
	mov edx, [ecx+3Ch] // subtype
	pop edi
	pop esi
	pop ebx
	ret
}}

__declspec(naked) static void GetTerrainOverlaySwitch(){__asm
{
	cmp eax, 2Eh
	jz  _found

	cmp eax, 40 // new WoG overlays are type 40, subtype 10000+
	jnz _others
	mov eax, [ecx+3Ch]
	cmp eax, 10000
	mov eax, [ecx+38h]
	jnb _others

_found:
	pop edx
	jmp GetTerrainOverlayEnd
_others:
	ret
}}

/*
__declspec(naked) static void DisplayDialogHook(){__asm
{
	mov al, HookHintWindow
	test al, al
	jz _normal
	xor al, al
	mov HookHintWindow, al

	cmp edx, 4 // only change the right-mouse click dialog
	jnz _normal

	xor edx, edx
	call HintTrigger // HintTrigger(msg, 0)
	mov ecx, eax
	mov edx, 4

_normal:
	pop eax // return address
	push 0x4F6C05 // code overwritten by hook
	jmp eax
}}
*/

/*
void __fastcall ShowItem(_MapItem_ *mi)
{
	int x,y,l;
	Map2Coord(mi, &x, &y, &l);
	Message(Format("%d%d%d", x, y, l));
}

__declspec(naked) static void BeforeHeroMove(){__asm
{
	push ecx
	mov ecx, [esp+8]
	call ShowItem
	mov ecx, [esp+12]
	call ShowItem
	mov ecx, [esp+16]
	call ShowItem
	mov ecx, [esp+24]
	call ShowItem
	pop ecx

	pop eax
	push ebp
	mov ebp, esp
	sub esp, 48h
	inc eax
	jmp eax
}}
*/

__declspec(naked) static void MonDlgBorderColorBug(){__asm
{
	push ecx
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	mov eax, 0x48F940
	call eax // Dlg_0048F940 (this + 5 params)
	pop ecx
	call ConvertPalettesOfAllDefs_CurPlayer
	ret 14h
}}

__declspec(naked) static void GarrisonDlgBorderColorBug(){__asm
{
	mov eax, 0x44FFA0
	call eax // DlgPcx_BuildAndLoadPcx

	push eax

	mov ecx, eax
	mov edx, [ebp+8] // garrison struct
	xor eax, eax
	mov al, [edx] // owner
	cmp eax, 8
	jb _adjust

	mov ecx, BASE
	mov ecx, [ecx]
	//mov ecx, dword ptr [BASE]
	mov eax, 0x4CE6E0
	call eax // GetHumanPlayerIndex
	mov ecx, [esp]
_adjust:
	push eax
	mov eax, 0x4501D0
	call eax // DlgItem_AjustBorderColor2Player

	pop eax
	push 0x530D37
	ret
}}

int DefGetCadresCount(int def, int group)
{
	if (def == 0)
		return 0;

	if (*(int*)(def + 40) > group && *(*(int**)(def + 44) + group)) // number of groups is enough and the group exists
		return **(*(int***)(def + 28) + group);
	else
		return 0;
}

void __fastcall MoveMonster(int mon, bool random)
{
	if (*(int*)(mon + 0x4C) == 0) return; // doesn't exist
	if (*((int*)(mon + 0x198) + 70) || *((int*)(mon + 0x198) + 74)) return; // stoned or paralyzed

	int anim = *(int*)(mon + 0x3C);
	if (anim != 2 && (anim != 1 || !PlayingBM_V))  return;

	int* cadre = (int*)(mon + 0x40);
	int n = DefGetCadresCount(*(int*)(mon + 356), anim);
	if (n <= 1)  return;
	if (random && anim == 2)
	{
		*cadre = rand()%n;
	}
	else
	{
		*cadre = (*cadre + 1) % n;
		if (anim == 1 && *cadre == 0)
			*(int*)(mon + 0x3C) = 2;
	}
}

Dword lastMove;
int EnableMovingMonsters = 0;

int __fastcall MakeMoving()
{
	//*(int*)0x6989E8 = 0; // !!! possible option: faster combat (in MovingMonsters, not here)
	if (!EnableMovingMonsters) return 0;

	Dword t = timeGetTime();
	if (t - lastMove < 100) return 0;

	bool random = (t - lastMove >= 500);
	lastMove = t;
	int combatMan = *(int*)0x699420;
	int mon = combatMan + 0x54CC;
	for (int i = 0; i < 42; i++, mon+=0x548)
		MoveMonster(mon, random);
	// Set redraw rect to full screen
	memcpy((char*)(combatMan + 81208), (char*)0x694F68, 16);
	return 1;
}

void __fastcall MonFixMouseover(int mon)
{
	// bugfix: no mouseover animation when stoned/paralized
	if (( *((int*)(mon + 0x198) + 70) || *((int*)(mon + 0x198) + 74) ) && (*(int*)(mon + 0x3C) == 1))
	{
		*(int*)(mon + 0x3C) = 2;
		*(int*)(mon + 0x40) = 0;
	}
}

//void __fastcall LogMovingMonsters(int ret)
//{
//	addlog("MovingMonsters call:", ret);
//}

// called when any monster is about to be drawn
__declspec(naked) static void MovingMonsters(){__asm
{
	//mov ecx, [ebp + 4]
	//call LogMovingMonsters

	mov ecx, ebx
	call MonFixMouseover

	mov eax, [ebp + 4] // AnimationStep is a special case, handled in MovingMonsters2
	cmp eax, 0x49446D
	jz _std
	cmp eax, 0x494B4E
	jz _std
	cmp eax, 0x494BFE
	jz _std

	call MakeMoving
_std:
	mov eax, [ebx+100h]
	inc [esp]
	ret
}}

// handle AnimationStep
__declspec(naked) static void MovingMonsters2(){__asm
{
	push ecx
	call MakeMoving
	pop ecx

	pop eax
	push ebp
	mov ebp, esp
	sub esp, 20h
	inc eax
	jmp eax
}}

// prevent drawing shadow twice while monster is moving and after Catapult shot
__declspec(naked) static void MovingMonsters3(){__asm
{
	mov [esp+20], 1
	push 0x493FC0
	ret
}}

// show animation while flying
__declspec(naked) static void MovingMonsters4(){__asm
{
	push ecx
	
	push 0
	push 1
	push 0
	push 0
	push 0
	push 0
	mov ecx, 0x699420
	mov ecx, [ecx]
	mov eax, 0x493FC0 // AnimationStep
	call eax

	pop ecx

	push 0x43DE60
	ret
}}

__declspec(naked) static void DoAnimationStep(){__asm
{
	push ecx

	call MakeMoving
	test eax, eax
	jz _std

	mov ecx, 0x699420
	mov ecx, [ecx]
	lea eax, [ecx + 0x13D30]
	mov dword ptr [eax], 1

	push 1
	push 1
	push 0
	push 0
	push 0
	push 1
	mov eax, 0x493FC0 // AnimationStep
	call eax

	mov ecx, 0x699420
	mov ecx, [ecx]
	lea eax, [ecx + 0x13D30]
	mov dword ptr [eax], 0

_std:
	pop ecx
	ret
}}

// show animation during any kind of shooting, except ray shooting
__declspec(naked) static void MovingMonsters5(){__asm
{
	call DoAnimationStep
	push 0x47B610
	ret
}}

__declspec(naked) static void DoSpellTargetScreen(){__asm
{
	mov PlayingBM_V, 1
	call DoAnimationStep
	mov PlayingBM_V, 0

	ret
}}

// teleport select monster
__declspec(naked) static void MovingMonsters6(){__asm
{
	call DoSpellTargetScreen
	push 0x5A3880
	ret
}}

// teleport select place
__declspec(naked) static void MovingMonsters7(){__asm
{
	call DoSpellTargetScreen
	push 0x5A3AE0
	ret
}}

// sacrifice select whom to revieve
__declspec(naked) static void MovingMonsters8(){__asm
{
	call DoSpellTargetScreen
	push 0x5A3010
	ret
}}

// sacrifice select whom to kill
__declspec(naked) static void MovingMonsters9(){__asm
{
	call DoSpellTargetScreen
	push 0x5A31A0
	ret
}}

//void __fastcall LogRedrawPartOfScreen(int ret)
//{
//	addlog("RedrawPartOfScreen call:", ret);
//}
//
//__declspec(naked) static void onRedrawPartOfScreen()
//{
//	_asm
//	{
//		push ecx
//		mov ecx, [esp + 8]
//		cmp ecx, 6286244
//		jnz _ok
//		mov ecx, [ebp]
//		mov ecx, [ecx + 4]
//		call LogRedrawPartOfScreen
//		mov ecx, [ebp]
//		mov ecx, [ecx]
//		mov ecx, [ecx + 4]
//		call LogRedrawPartOfScreen
//		mov ecx, [ebp]
//		mov ecx, [ecx]
//		mov ecx, [ecx]
//		mov ecx, [ecx + 4]
//		call LogRedrawPartOfScreen
//_ok:
//		pop ecx
//		pop eax
//		push ebp
//		mov ebp, esp
//		sub esp, 10h
//		inc eax
//		jmp eax
//	}
//}

__declspec(naked) static void ShortCombatDelays(){__asm
{
	ret
}}

//void __fastcall LogWavWait(int ret)
//{
//	addlog("WavWait", ret);
//}

volatile long WaitForWav; // used to postpone MP3 start 'till wav finishes
volatile long WaitForWavTimeout;

void __fastcall SetWaitWav(int timeout)
{
	InterlockedExchange(&WaitForWavTimeout, timeGetTime() + (timeout < 0 ? 4000 : timeout));
	InterlockedIncrement(&WaitForWav);
}

__declspec(naked) static void NoWavWait(){__asm
{
	//push ecx
	//mov ecx, [esp+4]
	//call LogWavWait
	//pop ecx
	cmp [esp], 0x462C30
	jnz _nowait
	push ecx
	call SetWaitWav
	pop ecx
	ret 8
_nowait:
	ret 8
}}

__declspec(naked) static void NoWavWait2(){__asm
{
	mov edx, -1
	push 3
	mov eax, 0x59A890 // LoadWAVplayAsync
	call eax
	xor eax, eax
	ret
}}

// Play Ballista shot explosion, Quicksand and LandMine sounds asynchronously

char *lastSincSnd;

__declspec(naked) static void NoWavWait3a(){__asm
{
	mov lastSincSnd, ecx
	xor eax, eax
	ret
}}

__declspec(naked) static void NoWavWait3b(){__asm
{
	mov ecx, lastSincSnd
	mov edx, -1
	push 3
	mov eax, 0x59A890 // LoadWAVplayAsync
	call eax
	xor eax, eax
	ret 4
}}

__declspec(naked) static void CompleteCombatStartSound(){__asm
{
	inc [esp]
	ret 4
}}

const int ProcessMessages = 0x4F8980;

__declspec(naked) static void CombatStartDelay(){__asm
{
	call ds:[0x63A354]
	lea ecx, [eax + 200]
	call ProcessMessages
	mov edx, 4
	ret
}}

void MP3Process(void); // in sound.cpp

__declspec(naked) static void OnLoadMP3(){__asm
{
	call MP3Process
	push 0x61A56C
	ret
}}

char* __fastcall SoundProcess(char *name); // in sound.cpp

__declspec(naked) static void OnLoadWav(){__asm
{
	call SoundProcess
	mov ecx, eax

	pop eax
	push ebp
	mov ebp, esp
	sub esp, 1Ch
	inc eax
	jmp eax
}}

__declspec(naked) static void MessagesLoop(){__asm
{
	inc InGame
	pop eax
	push _after
	push ebp
	mov ebp, esp
	sub esp, 48h
	inc eax
	jmp eax
_after:
	dec InGame
	cmp InGame, 1
	jz _mainMenu
	ret
_mainMenu:
	dec InGame // decrease it twice the last time, 'cause it was also increased by FindERM
	call OnExitToMainMenu
	ret
}}

/*
__declspec(naked) static void OnModalDialog(){__asm
{
	push ecx
	mov ecx, [ebp + 8]
	lea edx, [ebp + 12]
	call HookModalDialog
	pop ecx
	mov eax, [0x6AAD88]
	ret
}}

__declspec(naked) static void _OnDialogAction(){__asm
{
	pop edx // ret addr
	mov eax, [esp] // cmd
	push ecx // dlg
	push edx // ret addr

	push eax // cmd
	push _after // new return
	// erased code
	push ebp
	mov ebp, esp
	push ebx
	mov ebx, eax

	push 0x41B127
	ret
_after:
	test eax,eax
	jnz _noprocess
	jmp OnDialogAction
_noprocess:
	ret 8
}}
*/

void __fastcall OnShowDialog(int dlg, bool draw)
{
	HookModalDialog((int**)dlg);
	LuaCall("OnShowDialog", dlg, draw);
}

__declspec(naked) static void _OnShowDialog1(){__asm
{
	mov ecx, [ebp + 8]
	mov edx, [ebp + 16]
	call OnShowDialog
	pop ebx
	pop ebp
	ret 0xC
}}

__declspec(naked) static void _OnShowDialog2(){__asm
{
	mov [ebx+58h], edi
	mov [ebx+5Ch], edx
	mov ecx, edi
	mov edx, [ebp + 16]
	push 0x602A50
	jmp OnShowDialog
}}

void __fastcall OnHideDialog(int dlg)
{
	LuaCall("OnHideDialog", dlg);
}

__declspec(naked) static void _OnHideDialog(){__asm
{
	mov esi, [ebp + 8]
	xor ebx, ebx
	cmp esi, ebx
	jz _skip
	push ecx
	mov ecx, esi
	call OnHideDialog
	pop ecx
_skip:
	ret
}}

int __fastcall OnDialogCallback(Dword cmd, Dword manager, Dword callback)
{
	return LuaCall("DialogCallback", cmd, manager, callback);
}

__declspec(naked) static void _OnDialogCallback(){__asm
{
	push [ebp + 0xC]
	call OnDialogCallback
	cmp eax, 2
	push 0x602C5C
	ret
}}

void __fastcall OnChangeCursor(int cadre, int type, int caller)
{
	addlog(Format("SetMouseCursor(%d, %d) at %x", cadre, type, caller));
}

__declspec(naked) static void _OnChangeCursor(){__asm
{
	push ecx
	mov ecx, [ebp + 8]
	mov edx, [ebp + 12]
	push [ebp + 4]
	call OnChangeCursor
	pop ecx
	pop eax
	push 0x62E828
	jmp eax
}}

static int __stdcall CanStart(void*, int, int, char* name)
{
	if (ReadIntINI(0, "AllowMultipleCopies")) return 1;
	HANDLE ret = *(HANDLE*)0x69965C = CreateEvent(0, 0, 0, name);
	return (ret != 0) && GetLastError() != 0xB7;
}

__declspec(naked) static void _OnStart(){__asm
{
	call CanStart
	test eax, eax
	jz _quit
	call InitLua
	push 0x4F8104
	ret
_quit:
	push 0x4F825A
	ret
}}

void __fastcall OnBattleStart(int log)
{
	void (__fastcall *f)(int); *(Dword *)&f=0x554460;
	f(log);
	BACall(54, ERM_HeroStr);
}

__declspec(naked) static void _DrawDef1ToMap1(){__asm
{
	mov eax, [ebp + 8]
	mov [esp + 4], eax
	push 0x47C300
	ret
}}

__declspec(naked) static void _DrawDef1ToMap2(){__asm
{
	mov eax, [ebp + 8]
	mov [esp + 4], eax
	push 0x47BE90
	ret
}}

int GetVKey(int key)
{
	if (key >= '0' && key <= '9' || key >= 'A' && key <= 'Z')
		return key;

	switch (key)
	{
		case '`':  return 192;  break;
		case '-':  return 189;  break;
		case '=':  return 187;  break;
		case '[':  return 219;  break;
		case ']':  return 221;  break;
		case ';':  return 186;  break;
		case '\'': return 222;  break;
		case ',':  return 188;  break;
		case '.':  return 190;  break;
		case '/':  return 191;  break;
		case '\\': return 220;  break;
		default:
			return 0;
	}
}

int __fastcall TranslateKey(int key, int shift)
{
	int code = GetVKey(key);
	if (!code) return key;

	byte keys[256];
	WORD ret;
	GetKeyboardState(keys);
	keys[VK_SHIFT] = (shift ? 128 : 0);
	if (ToAscii(code, MapVirtualKey(code, 0), keys, &ret, 0) == 1 && ret >= ' ')
		return (byte)ret;
	else
		return key;
}

__declspec(naked) static void _TranslateKey(){__asm
{
	push eax
	mov ecx, [eax + 4]
	call TranslateKey
	mov edx, eax
	pop eax
	mov [eax + 4], edx
	pop ebp
	ret 4
}}

__declspec(naked) static void _TranslateKeyPart(){__asm
{
	mov edx, [eax + 4]
	cmp edx, 41
	jz _mine
	cmp edx, 0x3B
	push 0x4EC7CC
	ret
_mine:
	mov [eax + 4], '`'
	mov edx, [eax + 0xC]
	and edx, 3
	jmp _TranslateKey
}}

__declspec(naked) static void _GetTownIncome(){__asm
{
	push ecx
	push [esp + 8]
	push _after
	push ebp
	mov ebp, esp
	push ebx
	mov ebx, ds:[0x66CDF0]
	push 0x5BFA0A
	ret

_after:
	pop ecx
	mov edx, eax
	call CorrectTownIncome
	ret 4
}}

__declspec(naked) static void _GetTownGrowth(){__asm
{
	push ecx
	push [esp + 8]
	push _after
	push ebp
	mov ebp, esp
	sub esp, 0xC
	push 0x5BFF66
	ret

_after:
	pop ecx
	mov edx, eax
	jmp CorrectTownGrowth
}}

static int __fastcall MessagePictureInit(Dword data)
{
	return LuaCall("MessagePictureInit", data);
}

__declspec(naked) static void _MessagePictureInit(){__asm
{
	jnz _stdnorm
	
	call MessagePictureInit
	mov ecx, ebx
	xor edx, edx
	mov edi, [ebx]
	mov [ebp + 8], edi
	test eax, eax
	mov eax, [ebx + 4]
	mov [ebp + 0xC], eax
	jz _std
	
	push 0x4F6229
	ret

_std:
	cmp edi, esi
	jnz _stdnorm
	push 0x4F6388
	ret

_stdnorm:
	push 0x4F558A
	ret
}}

static int __fastcall MessagePictureHint(Dword data)
{
	return LuaCall("MessagePictureHint", data);
}

__declspec(naked) static void _MessagePictureHint(){__asm
{
	jna _stdnorm

	call MessagePictureHint
	push 0x4F15C0
	ret

_stdnorm:
	push 0x4F11ED
	ret
}}

static void __fastcall AfterBuildResBar(Dword panel, int IsSmall)
{
	LuaCall("AfterBuildResBar", panel, IsSmall);
}

__declspec(naked) static void _BuildResBar(){__asm
{
	push [esp + 8]
	push [esp + 8]
	push _after
	push ebp
	mov ebp, esp
	push -1
	push 0x558DF5
	ret

_after:
	mov edx, [esp + 8]
	push eax
	mov ecx, eax
	call AfterBuildResBar
	pop eax
	ret 8
}}

__declspec(naked) static void _GetFlagColor(){__asm
{
	push ecx
	push _after
	mov edx, [ecx+1Eh]
	mov ecx, [ecx]
	push 0x40FC85
	ret

_after:
	pop ecx
	mov edx, eax
	call ChangeFlagColor
	ret
}}

__declspec(naked) static void _GetFlagColor1(){__asm
{
	cmp eax, 54
	mov eax, 0x410EC1
	jnz _ok
	mov eax, 0x41104A
_ok:
	jmp eax
}}

__declspec(naked) static void _GetFlagColor2(){__asm
{
	cmp eax, 54
	mov eax, 0x4125C4
	jnz _ok
	mov eax, 0x41272D
_ok:
	jmp eax
}}

__declspec(naked) static void _DigGrail(){__asm
{
	mov edx, 0
	jz _no
	inc edx
_no:
	mov ecx, [ebp - 0x10]  // hero
	push [ebp + 0x10]  // L
	push ebx  // Y
	push esi  // X
	call DigGrailTrigger

	cmp eax, 1
	jb _fail
	jz _win
// no standard reaction:
	push 0x40F080
	ret
_win:
	push 0x40EEE5
	ret
_fail:
	mov ecx, ds:[0x69CCFC]
	push 0x40F051
	ret
}}

__declspec(naked) static void _DigGrailFail(){__asm
{
	cmp eax, eax
	jmp _DigGrail
}}

__declspec(naked) static void _PostGainLevel(){__asm
{
	mov ecx, [ebp - 0x1C]
	mov edx, [ebp - 0x18]
	call PostGainLevel
	mov ax, [ebx + 0x55]
	mov edx, [ebp - 0x10]
	push 0x4DAF0D
	ret
}}

__declspec(naked) static void _PostGainLevelChosen(){__asm
{
	mov eax, [esp + 4]
	mov GL_SSkillResult, eax
	push 0x4E2540
	ret
}}

__declspec(naked) static void _GetAIMapPosValue(){__asm
{
	pop eax // ret
	push ds:[edx]
	push edx
	push eax // ret
	push ecx
	push [esp + 16] // param
	push _after

	push ebp
	mov ebp, esp
	sub esp, 6Ch
	push 0x528526
	ret

_after:
	mov ecx, eax
	pop edx
	jmp GetAIMapPosValue
}}

//void BaseFileLoader(); // in _B1.cpp

static void OnLodsLoaded()
{
	STARTNA(__LINE__, 0)
	if(DeveloperPath[0])
	{
		char *s = Format("%sData\\h3std.lod", DeveloperPath);
		if (DoesFileExist(s, 10) && (LodTypes::Load(6, s) || LodTypes::Add2List(6)))
		{
			Message(Format("Cannot load %s", s));
			Exit();
		}

		if(LodTypes::Load(4, Format("%sData\\h3wog.lod", DeveloperPath)) || LodTypes::Add2List(4))
		{
			Message("Cannot load h3wog.lod from DeveloperPath location.");
			Exit();
		}
		// prioritize h3custom.lod over h3wog.lod
		LodTypes::Del4List(5);
		LodTypes::Add2List(5);
	}
	//BaseFileLoader();
	LuaCall("LoadScripts", "global");
	RETURNV
}

__declspec(naked) static void _OnLodsLoaded(){__asm
{
	push 0x5B9460
	jmp OnLodsLoaded
}}


int BinTree_Ctor = 0x55D4D0;
int BinTree_Dtor = 0x559680;
int BinTree_Find = 0x55E560;
int BinTree_Add = 0x55DDF0;

static void __fastcall OnLoadDef(char *name)
{
	strncpy(LastLoadedDefName, name, sizeof(LastLoadedDefName) - 1);
}

__declspec(naked) static void _OnLoadDef(){__asm
{
	pop eax
	push 0 // alloc BinTree
	push 0
	push 0
	push 0
	push _after // new ret address

	push ebp
	mov ebp, esp
	push -1
	push eax
	push ecx
	call OnLoadDef
	pop ecx
	ret

	// Logic: create a BinTree for frames on stack on demand. Destroy in _after.
_after:
	cmp [esp + 4], 0
	jz _done
	mov ecx, esp
	push eax
	call BinTree_Dtor
	pop eax
_done:
	add esp, 0x10
	ret
}}

__declspec(naked) static void _OnLoadDefFindCadre(){__asm
{
	cmp [ebp + 8 + 4], 0
	jnz _find
	lea ecx, [ebp + 8]
	push ecx
	push ecx
	call BinTree_Ctor
_find:
	lea ecx, [ebp + 8]
	call BinTree_Find
	mov eax, [eax]
	mov ecx, [ebp + 8 + 4]
	push 0x55CDEF
	ret
}}

__declspec(naked) static void _OnLoadDefAddCadre(){__asm
{
	lea ecx, [ebp + 8]
	jmp BinTree_Add
}}

// Destroy without touching BinTree
__declspec(naked) static void _OnDefCadreDeref(){__asm
{
	test ecx, ecx
	jz _done
	sub [ecx + 0x18], 1
	jg _done
	mov eax, ds:[ecx]
	push 1
	call ds:[eax]
_done:
	ret
}}

void __fastcall OnNoNewMessage(int *p)
{
	if (p[13] != 1 || p[526] == p[527])
		MsgWaitForMultipleObjects(0, 0, 0, 1, QS_ALLINPUT);
	((void (*)())0x4EDB20)();
}

__declspec(naked) static void _FixParseCmdLine(){__asm
{
	cmp ebx, 1
	jz _good
	mov al, [ebx + ecx - 1]
	cmp al, ' '
	jz _good
	cmp al, '\"'
	jnz _bad
_good:
	mov cl, [ebx + ecx]
	cmp cl, '/'
	ret
_bad:
	mov dword ptr ds:[esp], 0x4F0F27
	ret
}}

static void OnFastMapLoad()
{
	strcpy_s((char*)BaseStruct + 0x1F7D4, 251, MapFastLoadingDir);
	strcpy_s((char*)BaseStruct + 0x1F6D9, 100, MapFastLoadingName);
	*(int*)0x4EEF4D = 0x1DF4F; // restore original code
}

__declspec(naked) static void _OnFastMapLoad(){__asm
{
	pop eax
	pop eax
	pop eax

	mov ebx, 1
	push 1
	push 1
	mov ecx, ds:[0x6992B0]
	mov eax, 0x50CEA0
	call eax

	mov eax, 0x4ED9E0
	call eax

	mov cl, 1
	mov eax, 0x4ED930
	call eax
	
	push 0x4EF43E
	jmp OnFastMapLoad
}}

__declspec(naked) static void _MyStatMemCheck(){__asm
{
	mov esi, [esp + 0x0C]
	cmp esi, WogInstStart
	jb _Fine
	cmp esi, WogInstEnd
	jnb _Fine
	xor esi, esi
_Fine:
	test esi, esi
	ret
}}

__declspec(naked) static void _FixTextLinesCount(){__asm
{
	mov edi, [ebp - 0x8] // stdcode
	mov ecx, [ebp - 0xC] // stdcode
	cmp esi, [ebp - 0x4] // i < LineStart
	jnl _std
	mov esi, [ebp - 0x4] // i = LineStart
_std:
	ret
}}

__declspec(naked) static void _FixLoadActiveHero(){__asm
{
	cmp eax, -1
	jz _skip
	mov ecx, ds:[0x69CCFC]
	mov ecx, [ecx + 4]
	cmp ecx, 0
	jnl _skip
	ret
_skip:
	mov dword ptr ds:[esp], 0x407721
	ret
}}

/*
int tmpLast;

void __stdcall tmpDoLog(int tolog)
{
	addlog("QuestLog", tolog);
}

__declspec(naked) static void tmpLog(){__asm
{
	mov eax, [esp]
	push ecx
	push eax
	call tmpDoLog
	pop ecx
	jmp tmpLast
}}

int tmpVTable[20];
void __stdcall tmpQuestLogDo(void **obj)
{
	CopyMem((char*)tmpVTable, (char*)*obj, 20*4);
	*obj = tmpVTable;
	tmpLast = tmpVTable[9];
	tmpVTable[9] = (int)tmpLog;
}

__declspec(naked) static void tmpQuestLog(){__asm
{
	pop eax
	mov eax, 0x455BD0
	call eax
	push eax
	push eax
	call tmpQuestLogDo
	pop eax
	push 0x4017FE
	ret		
}}
*/
#include "global_hooks.h" // must be in the end