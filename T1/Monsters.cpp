#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "service.h"
#include "herospec.h"
#include "casdem.h"
#include "erm.h"
#include "npc.h"
#include "b1.h"
#include "CrExpo.h"
#include "monsters.h"
#include "global.h"
#define __FILENUM__ 6

//static Byte *BACall_Mon=0;
static int BACall_Day=-1;
static int BACall_Turn=-1;
int BADistFlag=0;
static Byte *CurrentMon;

static TxtFile GhostParams;

int  MonsterOfWeek[100];
int  CR100;
int CalcRand100(void)
{
	__asm  pusha
	CR100=Random(1,100);
	__asm  popa
	return CR100;
}

/////////////////////////////////////
/// ERM настройка
int ERM_MonAtr(char Cmd,int Num,_ToDo_*,Mes *Mp)
{
	int k,l;
// разрешено до WoG
//  if(WoG==0) return 0;
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	if(Num<2) RETURN(0)
	k=Mp->n[0];  // номер монстра
	if((k<0)||(k>=MONNUM)){ RETURN(0) }
	switch(Cmd){
		case 'O': // Группа O#/$ принадлежность городу (0...8,-1 = ничейный)
			if(Apply(&MonTable[k].Group,4,Mp,1)) break;
			if(MonTable[k].Group<-1) MonTable[k].Group=-1;
			if(MonTable[k].Group>8)  MonTable[k].Group=8;
			CrIsChanged(k);
			break;
		case 'L': // Уровень L#/$ (0...6[7],-1 = запрещен)
			if(Apply(&MonTable[k].SubGroup,4,Mp,1)) break;
			if(MonTable[k].SubGroup<-1) MonTable[k].SubGroup=-1;
			if(MonTable[k].SubGroup>6)  MonTable[k].SubGroup=6;
			CrIsChanged(k);
			break;
		case 'C': // Цена C#/#/$ по номерам ресурсов (0...6)
			CHECK_ParamsNum(3);
			l=Mp->n[1];  // номер ресурса
			if((l<0)||(l>6)) { MError2("resource index out of bounds (0...6)"); RETURN(0)}
			if(Apply(&MonTable[k].CostRes[l],4,Mp,2)) break;
			CrIsChanged(k);
			break;
		case 'F': // Fight F#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].Fight,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'I': // AIValue I#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].AIvalue,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'G': // Grow G#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].Grow,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'R': // Grow R#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].HGrow,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'P': // HitPoints P#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].HitPoints,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'S': // Speed S#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].Speed,4,Mp,1)) break;
			if(MonTable[k].Speed<1) MonTable[k].Speed=1;
			CrIsChanged(k);
			break;
		case 'A': // Attack A#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].Attack,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'D': // Defence D#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].Defence,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'M': // DamageL M#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].DamageL,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'E': // DamageH E#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].DamageH,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'N': // NShots N#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].NShots,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'B': // HasSpell B#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].HasSpell,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'V': // AdvMapL V#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].AdvMapL,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'H': // AdvMapH H#/$ соответствующий параметр в TXT
			if(Apply(&MonTable[k].AdvMapH,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'X': // X#/$ Флаги - спецспособности
			if(Apply(&MonTable[k].Flags,4,Mp,1)) break;
			CrIsChanged(k);
			break;
		case 'U': // Upgrade to U#/$
			if(Apply(&MonsterUpgradeTable[k],4,Mp,1)) break;
			break;
		default:
			RETURN(0)
	}
	RETURN(1)
}

////////////////////////////
// параметры привидений
static int GhostMultiplyer[MONNUM];
int LoadGhostParam(void)
{
	int   i,n,k;
	STARTNA(__LINE__, 0)
	if(LoadTXT("ZGhost.TXT",&GhostParams)) RETURN(-1) // не может загрузить TXT
	k=100; //=1.0
	for(i=1;i<GhostParams.sn;i++){
		n=a2i(ITxt(i,0,&GhostParams));
		if(n!=-1) continue;
		k=a2i(ITxt(i,1,&GhostParams));
	}
	for(i=0;i<MONNUM;i++) GhostMultiplyer[i]=k;
	for(i=1;i<GhostParams.sn;i++){
		n=a2i(ITxt(i,0,&GhostParams));
		if((n<0)||(n>=MONNUM)) continue;
		k=a2i(ITxt(i,1,&GhostParams));
		if(k==0) continue; // пустая строка
		GhostMultiplyer[n]=k;
	}
	RETURN(0)
}

////////////////////////////////////////
// ответ монстра на атаку
Byte *MKB_MonA;
Byte *MKB_MonD;
Dword MKB_Flags;
int   MKB_HPorig;
int   MKB_Killed;
int   MKB_HPdel;
Byte  MKB_OldMagic;
int   MKB_Ret;
int _MonsterKickBack(void)
{
	int  n;
	char *ns,*np;

	STARTNA(__LINE__, 0)
	//if(WoG==0) RETURN(0)
	if(*((int *)&MKB_MonA[0x34])==159){ // привидение
		*((int *)&MKB_MonA[0x34])=63;  // временно сделаем вампиром-лордом
//    *((int *)&MKB_MonA[0x60])*=3; // ограничим максимум в 3 раза за отаку
			n=*((int *)&MKB_MonA[0x4C]);
//      n+=(*((int *)&MKB_MonD[0x50])-*((int *)&MKB_MonD[0x4C])); не верно
			n+=MKB_Killed;
			*((int *)&MKB_MonA[0x60])=n; // ограничим максимум в +число убитых
		MKB_Flags=*((int *)&MKB_MonD[0x84]);  // запомним флаги D монстра
		*((int *)&MKB_MonD[0x84])|=0x10; // сделаем временно живым D монстра
		// модифицируем урон для восстановления
		MKB_HPdel=(MKB_HPdel*GhostMultiplyer[MKB_MonD[0x34]])/100;
		// имя монстров
		ns=ITxt(38,0,&Strings);
		np=ITxt(39,0,&Strings);
		__asm{
			// запомним вампира лорда
			mov    eax,63
			mov    edi,0x06747B0
			mov    edi,[edi]
			lea    ecx,[0x8*eax]
			sub    ecx,eax
			lea    eax,[eax+4*ecx]
			mov    ecx,[edi+4*eax+0x14] // ед.число
			mov    ebx,ns
			mov    ns,ecx
			mov    [edi+4*eax+0x14],ebx
			mov    ecx,[edi+4*eax+0x18] // мн.число
			mov    ebx,np
			mov    np,ecx
			mov    [edi+4*eax+0x18],ebx
		}
		// заменим фразу
/*
		phr=ITxt(33,0,&Strings);
		asm{
			mov    eax,0x6A5DC4
			mov    eax,[eax]
			mov    eax,[eax+0x20]
			mov    eax,[eax+0x5AC]
	mov    eax,[eax+000005B4]

			mov    phr,eax
//8B80AC050000   mov    eax,[eax+000005AC] // строка самого сообщения
*/
		// временно устанавливаем вампиру-логрду {0x440B9D+1,DS0(0x53),1},
		__asm{
			mov    ecx,0x440B9D+1
			mov    al,[ecx]
			mov    MKB_OldMagic,al
			mov    al,0x53
			mov    [ecx],al
		}
		__asm{
			mov    ecx,MKB_MonA
			push   MKB_HPorig
			push   MKB_Killed
			push   MKB_HPdel
			push   MKB_MonD
			call   dword ptr [OriginalCallPointer]
		}
		// восстанавливаем
		__asm{
			mov    ecx,0x440B9D+1
			mov    al,MKB_OldMagic
			mov    [ecx],al
		}
		// имя монстров
		__asm{
			mov    eax,63
			mov    edi,0x06747B0
			mov    edi,[edi]
			lea    ecx,[0x8*eax]
			sub    ecx,eax
			lea    eax,[eax+4*ecx]
			mov    ecx,ns
			mov    [edi+4*eax+0x14],ecx
			mov    ecx,np
			mov    [edi+4*eax+0x18],ecx
		}
		*((int *)&MKB_MonD[0x84])=MKB_Flags;  // восстановим флаги D монстра
		*((int *)&MKB_MonA[0x60])=*((int *)&MKB_MonA[0x4C]);
		*((int *)&MKB_MonA[0x34])=159; // восстановим
		RETURN(1)
	}
	if(*((int *)&MKB_MonA[0x34])==152){ // бог грома
		// увеличиваем вероятность молнии по голове до 100%
		__asm{
			mov    ecx,0x440EB4
			mov    al,0x13
			mov    [ecx],al
		}
		__asm{
			mov    ecx,MKB_MonA
			push   MKB_HPorig
			push   MKB_Killed
			push   MKB_HPdel
			push   MKB_MonD
			call   dword ptr [OriginalCallPointer]
		}
		// восстанавливаем
		__asm{
			mov    ecx,0x440EB4
			mov    al,0x64
			mov    [ecx],al
		}
		RETURN(1)
	}
	if(*((int *)&MKB_MonA[0x34])==157){ // гидра хаоса
		// увеличиваем вероятность кислоты до 100%
		__asm{
			mov    ecx,0x4411CE
			mov    al,0x13
			mov    [ecx],al
		}
		__asm{
			mov    ecx,MKB_MonA
			push   MKB_HPorig
			push   MKB_Killed
			push   MKB_HPdel
			push   MKB_MonD
			call   dword ptr [OriginalCallPointer]
		}
		// восстанавливаем
		__asm{
			mov    ecx,0x4411CE
			mov    al,0x64
			mov    [ecx],al
		}
		RETURN(1)
	}
	if(*((int *)&MKB_MonA[0x34])==154){ // дракон привидение
		// уменьшаем вероятность вампирства до 40%
		if(Random(1,100)>40) RETURN(1)
		__asm{
			mov    ecx,MKB_MonA
			push   MKB_HPorig
			push   MKB_Killed
			push   MKB_HPdel
			push   MKB_MonD
			call   dword ptr [OriginalCallPointer]
		}
		RETURN(1)
	}
	RETURN(0)
/*
 mov    edx,[ebp+0x8]     // D Монстр, нападающий
 mov    eax,[edx+0x0084]  // D флаги
 shr    eax,4
 test   al,1   // живой?
 je     // нет - выходим
 mov    ebx,[esi+0x4C] // A защ. монстр - число
 mov    edx,[esi+0x60] // A нач. число
 mov    eax,[ebp+0x14] // D здоровье * число = полное здоровье
 mov    ecx,[ebp+0x0C] // A нанесенный урон
 sub    edx,ebx
 cmp    eax,ecx
 mov    dword ptr [ebp-20],0

 mov    [ebp+0x14],eax
 mov    [ebp+0x0C],ecx

 lea    edi,[ebp+0x14]
 jl     1 // всех убили
 lea    edi,[ebp+0x0C]
1: // edi указывает на меньшее

 mov    ecx,[esi+0xC0] // A здоровье на одного
 mov    eax,ecx
 imul   eax,edx        // A здоровье * число умерших с начала битвы
 mov    edx,[esi+0x58] // A потери здоровья последнего монстра
 add    eax,edx        // A сумма потерянного здоровья
 mov    edx,[edi]      // D полное доступное здоровье
 mov    [ebp-10],eax
 cmp    eax,edx
 mov    [ebp+14],edx
 lea    eax,[ebp-10]
 jl     1
 lea    eax,[ebp+14]
1: // выберем меньшее

 mov    eax,[eax]      // минимум из суммы здоровья
 mov    edx,[esi+0x58] // A потери здоровья последнего монстра
 sub    edx,eax
 mov    [ebp+14],eax
 mov    [esi+58],edx
 mov    edi,edx
 jns    H32.004409A8 (004409A8)
 mov    eax,ecx
 sub    eax,edi
 dec    eax
 cdq
 idiv   ecx
 imul   ecx,eax
 add    ecx,edi
 add    ebx,eax
 mov    [ebp-20],eax
 mov    eax,[ebp+14]
 mov    [esi+58],ecx
 mov    [esi+4C],ebx
 test   eax,eax
 jle    H32.004412AB
 mov    cl,[ebp+0B]
 push   00000000
 mov    [ebp-1C],cl
 lea    ecx,[ebp-1C]
 call   H32.00404130
 mov    ecx,[ebp+08]
 mov    edi,[006747B0]
 mov    dword ptr [ebp-04],00000000
 mov    eax,[ecx+34]
 test   eax,eax
 jl     H32.00440A08 (00440A08)
 cmp    eax,00000096
 jg     H32.00440A08 (00440A08)
 mov    edx,[ecx+4C]
 mov    ebx,[ebp+10]
 add    edx,ebx
 lea    ecx,[8*eax]
 cmp    edx,00000001
 jne    H32.004409FD (004409FD)
 sub    ecx,eax
 lea    edx,[eax+4*ecx]
 mov    edx,[edi+4*edx+14]
 jmp    H32.00440A0D (00440A0D)
 sub    ecx,eax
 lea    edx,[eax+4*ecx]
 mov    edx,[edi+4*edx+18]
 jmp    H32.00440A0D (00440A0D)
 mov    edx,00691260
 mov    eax,[esi+4C]
 mov    ebx,[ebp-20]
 sub    eax,ebx
 cmp    eax,00000001
 mov    eax,[esi+34]
 jne    H32.00440A81 (00440A81)
 test   eax,eax
 jl     H32.00440A3A (00440A3A)
 cmp    eax,00000096
*/
/* имя монстров
	mov    edi,[006747B0]
	lea    ecx,[8*eax]
	sub    ecx,eax
	lea    eax,[eax+4*ecx]
	mov    ecx,[edi+4*eax+14] // ед.число
	mov    ecx,[edi+4*eax+18] // мн.число
*/
//8B80AC050000   mov    eax,[eax+000005AC] // строка самого сообщения
}

void __stdcall MonsterKickBack(Dword mon,int HPdel,int Killed,int HPorig)
{
	_ECX(MKB_MonA);
	MKB_HPorig=HPorig;
	MKB_Killed=Killed;
	MKB_HPdel=HPdel;
	MKB_MonD=(Byte *)mon;
	__asm pusha
//  C2H_HireBufPo=(C2H_HireBufStr *)_EAX;
//  C2H_TownMan=(Byte *)_EBX;
	STARTNA(__LINE__, 0)
	OriginalCallPointer=Callers[18].forig; // ! до вызова функции
	MKB_Ret=_MonsterKickBack();
	__asm popa;
	if(MKB_Ret==0){
		__asm{
			mov    ecx,MKB_MonA
			push   MKB_HPorig
			push   MKB_Killed
			push   MKB_HPdel
			push   MKB_MonD
			call   dword ptr [OriginalCallPointer]
		}
/*
		if(*(int *)&MKB_MonA[0x34]==196){ // dracolish "disable ressurection" support
			int OrCr=*(int *)&MKB_MonD[0x60];
			int CurCr=*(int *)&MKB_MonD[0x4C];
			int NewOrCr=OrCr-MKB_Killed;
			if(NewOrCr<CurCr) NewOrCr=CurCr;
			if(NewOrCr>OrCr) NewOrCr=OrCr;
			*(int *)&MKB_MonD[0x60]=NewOrCr;
		}
*/
	}
	__asm pusha
	WereWolf(MKB_MonA,MKB_MonD);
//  Dracolich(MKB_MonA,MKB_MonD);
	CrExpBon::CastMassSpell2(MKB_MonA,MKB_MonD);
	STOP
	__asm popa
}

static int MS_AtType;
static Dword MS_ECX_;
void __stdcall MonsterStrikeFirst(Dword Par1,Dword Par2)
{
	_ECX(MS_ECX_);
	__asm pusha
		MS_AtType=1;
	__asm popa
	__asm{
		push  Par2
		push  Par1
		mov   ecx,MS_ECX_
		mov   eax,0x441330
		call  eax
	}
}
void __stdcall MonsterStrikeBack(Dword Par1,Dword Par2)
{
	_ECX(MS_ECX_);
	__asm pusha
		MS_AtType=2;
	__asm popa
	__asm{
		push  Par2
		push  Par1
		mov   ecx,MS_ECX_
		mov   eax,0x441330
		call  eax
	}
}
void __stdcall MonsterStrikeSecond(Dword Par1,Dword Par2)
{
	_ECX(MS_ECX_);
	__asm pusha
		MS_AtType=3;
	__asm popa
	__asm{
		push  Par2
		push  Par1
		mov   ecx,MS_ECX_
		mov   eax,0x441330
		call  eax
	}
}
///////////////////////////////
// оставление монстров
#define MONONMAPNUM 10000
struct _MonMapInfo_{
	unsigned  Owner    : 4; // хозяин
	unsigned  TakeBack : 1; // взять монстров назад
	unsigned _u1       : 3; // резерв до байта
	signed    x        :16;
	signed    y        :16;
	signed    l        : 8;
// 3.58
/*
	struct{
		unsigned  Owner    : 4; // хозяин
		unsigned  TakeBack : 1; // взять монстров назад
		unsigned _u1       : 3; // резерв до байта
	} A;
//  char  Owner; // 1...9 - хозяин+1, 0-убит или нет
	short x,y;
	char  l;
*/
};
static _MonMapInfo_ BackwardComp;
static _MonMapInfo_ MonMapInfo[MONONMAPNUM];

int AddMonOnMap(char Owner,int x,int y,int l)
{
	int i;
	STARTNA(__LINE__, 0)
	for(i=0;i<MONONMAPNUM;i++){
		if(MonMapInfo[i]./*A.*/Owner!=0) continue;
		MonMapInfo[i]./*A.*/Owner=Owner;
		if(PL_MLeaveStyle==1) MonMapInfo[i]./*A.*/TakeBack=1;
		else MonMapInfo[i]./*A.*/TakeBack=0;
		MonMapInfo[i].x=(short)x;
		MonMapInfo[i].y=(short)y;
		MonMapInfo[i].l=(char)l;
		RETURN(1)
	}
	RETURN(0)
}

static int LeftCr[2][7];
static char CrBuffer[1024];

int CheckCreature2Leave(int Type)
{
	STARTNA(__LINE__, 0)
	if(Type==GHOSTTYPE){ // привидение
		Message(ITxt(60,0,&Strings),1);
		RETURN(1)
	}
	if((Type>=GODMONTSTRT)&&(Type<(GODMONTSTRT+GODMONTNUM))){ // боги
		Message(ITxt(61,0,&Strings),1);
		RETURN(1)
	}
	RETURN(0)
}

int PlaceCreature(void)
{
	char *zstr;
	_ECX(zstr);
	_MapItem_ *mp0,*mp;
	_EDI(mp);
	_Hero_ *hp;
	int x,y,l,zsize,n,i,j;
	int t;
	int rt;
	char ch;
	char *abkup,*mbkup;

	STARTNA(__LINE__, 0)
	//if(WoG==0) goto l_exit;
	if(PL_MLeaveStd && PL_LeaveArt==0) goto l_exit;
	for(i=0;i<7;i++){ LeftCr[0][i]=-1; LeftCr[1][i]=0; }
// проверка на возможность размещения
	if(IsThis(CurrentUser())==0) goto l_exit; // не тот игрок
	if(EmptyPlace(mp)==0) goto l_exit;
	__asm{
		mov    eax,0x69CCFC
		mov    eax,[eax]
		mov    eax,[eax+4] // номер активного героя
		mov    IDummy,eax
	}
	if(IDummy==-1) goto l_exit;  // нет активного героя
	__asm{
		mov    ecx,eax
		shl    ecx,6
		add    eax,ecx
		mov    ecx,BASE
		mov    ecx,[ecx]
		lea    eax,[eax+8*eax]
		lea    ecx,[ecx+2*eax+0x21620] // указатель на структ. акт. героя
		mov    hp,ecx
	}
	__asm{
		mov    ecx,BASE
		mov    ecx,[ecx]
		mov    eax,[ecx+0x1FC40]
		mov    mp0,eax
		mov    eax,[ecx+0x1FC44]
		mov    zsize,eax
	}
	n=mp-mp0;
	l=n/(zsize*zsize);
	n=n%(zsize*zsize);
	y=n/zsize;
	x=n%zsize;
	if(hp->l!=l) goto l_exit;
	if(Abs(hp->x-x)>1) goto l_exit;
	if(Abs(hp->y-y)>1) goto l_exit;

	for(i=0;i<512;i++){
		ch=zstr[i];
		if(ch==0) break;
		CrBuffer[i]=ch;
	}
	for(j=0;i<1023;i++,j++){
		ch=ITxt(21,0,&Strings)[j];
		CrBuffer[i]=ch;
		if(ch==0) break;
	}
	if(PL_MLeaveStd==0 && PL_LeaveArt!=0){
		abkup=GetArtBase()[75].Name;
		GetArtBase()[75].Name=ITxt(225,0,&Strings);
		mbkup=MonTable[0].NameS;
		MonTable[0].NameP=ITxt(224,0,&Strings);
		rt=Request2Pic(CrBuffer,21,0,8,75,10);
		MonTable[0].NameP=mbkup;
		GetArtBase()[75].Name=abkup;
	}else{
		if(PL_MLeaveStd==0) rt=1; // Leave troops
		else rt=2; // Leave art
	}
	if(rt==2){
		rt=ChooseArt(hp,1);
		if(rt!=-1){
			if(rt>=1000){ // scroll
				PlaceObject(x,y,l,93,rt-1000,93,rt-1000,-1);
			}else{
				PlaceObject(x,y,l,5,rt,5,rt,-1);
			}
		}
		RETURN(0);
	}else if(rt==1){
//  if(Request0(CrBuffer)){
// определить количество
l_again:
		__asm{
			mov    ecx,hp
			lea    edx,LeftCr
			push   1
			mov    eax,0x5D16B0
			call   eax
		}
		t=-1;
		for(i=0;i<7;i++){
			if(LeftCr[0][i]!=-1){ // есть монстр
				if(t==-1){
					t=LeftCr[0][i];
					n=LeftCr[1][i];
					if(CheckCreature2Leave(t)) goto l_again;
				}else{
					if(t!=LeftCr[0][i]){ // Несколько разных существ
						Message(ITxt(20,0,&Strings),1);
						goto l_again;
					}
					n+=LeftCr[1][i];
				}
			}
		}
		if(t==-1) RETURN(0)
// поставить монстров
		__asm{
			mov    ecx,t   // подтип
			mov    edx,l   //l
			mov    eax,y   //y
			mov    ebx,x   //x
			push   0
			push   ecx    // subtype
			push   0x047  // type
			push   edx    // l
			push   eax    // y
			push   ebx    // x
			mov    ecx,BASE
			mov    ecx,[ecx] // [BASE]
			mov    eax,0x4C9550
			call   eax
// случайно - число монстров
// mov    eax,[00697798]
// lea    ecx,[8*eax]
// sub    ecx,eax
// lea    edx,[eax+4*ecx]
// mov    eax,[006747B0]
// mov    ecx,[eax+4*edx+6C]
// lea    eax,[eax+4*edx]
// mov    edx,[eax+70]
// call   H3.0050C7C0
// mov    ecx,[esi]
// lea    edx,[eax+eax]
// xor    edx,ecx
// установить число
			mov    edx,n
			and    edx,0xFFF // число монстров
			mov    esi,mp
			mov    [esi],edx
// установить агрессивность
			mov    ecx,[esi]
			mov    al,0x0A   // agression
			and    eax,0x01F // агрессивность от 0x0 до 0xA
			shl    eax,0x0C
			and    ecx,0xFFFE0FFF
			or     eax,ecx
			mov    [esi],eax
// 4C8EF4 что-то не доконца ясное, но работает - устанавливает корректный радиус монстрам
			mov    edi,BASE
			mov    edi,[edi]
			mov    ecx,edi
			mov    eax,0x4CA0D0
			call   eax
/*
			push   2
			mov    ecx,edi
			mov    eax,0x4C9190
			call   eax
			push   2
			mov    ecx,edi
			mov    [edi+0x1F664],eax
			mov    eax,0x4C9190
			call   eax
			push   2
			mov    ecx,edi
			mov    [edi+0x1F668],eax
			mov    eax,0x4C9190
			call   eax
			push   4
			mov    ecx,edi
			mov    [edi+0x1F66C],eax
			mov    eax,0x4C9190
			call   eax
			push   4
			mov    ecx,edi
			mov    [edi+0x1F670],eax
			mov    eax,0x4C9190
			call   eax
			push   4
			mov    ecx,edi
			mov    [edi+0x1F674],eax
			mov    eax,0x4C9190
			call   eax
			push   8
			mov    ecx,edi
			mov    [edi+0x1F678],eax
			mov    eax,0x4C9190
			call   eax
			mov    [edi+0x1F67C],eax
*/
			mov    ecx,0x6992B8
			mov    ecx,[ecx]
			push   0
			mov    eax,0x40F7D0
			call   eax
		}
		// запомним в структурке
		AddMonOnMap((char)(hp->Owner+1),x,y,l);
		RETURN(1)
	}
	RETURN(0)
l_exit:
	//Message(zstr,4);
	RETURN(-1)
}

/////////////////////////////
// проверка на разрешение драки с монстрами
static int E2MCheck2Kill(int GM_ai,_Hero_ *Hr,_MapItem_ *Mi)
{
	STARTNA(__LINE__, 0)
	if(GM_ai==0) RETURN(0) // AI пусть разбирается
	_MapItem_ *Mi0=GetMapItem0();
	int i,x,y,l,n,size;
	size=GetMapSize();
	n=Mi-Mi0;
	l=n/(size*size);
	n=n%(size*size);
	y=n/size;
	x=n%size;
	for(i=0;i<MONONMAPNUM;i++){
		if((int)MonMapInfo[i]./*A.*/Owner!=Hr->Owner+1) continue;
		if(MonMapInfo[i].x!=(short)x) continue;
		if(MonMapInfo[i].y!=(short)y) continue;
		if(MonMapInfo[i].l!=l) continue;
		if(MonMapInfo[i]./*A.*/TakeBack==1){
			_CMonster_ *mp=(_CMonster_ *)Mi;
			mp->Agression=0x1C;
			RETURN(0)
		}else{
			// оповещаем, что монстры убегают
			RequestPic(ITxt(40,0,&Strings),0x15,Mi->OSType,1);
			DelObject(x,y,l);
		}
		RETURN(1)
	}
	RETURN(0)
}


int _E2MWoMoNoBattle(_MapItem_ *Mi,_Hero_ *Hr)
{
	STARTNA(__LINE__, 0)
	_MapItem_ *Mi0=GetMapItem0();
	int i,x,y,l,n,size;
	size=GetMapSize();
	n=Mi-Mi0;
	l=n/(size*size);
	n=n%(size*size);
	y=n/size;
	x=n%size;
	for(i=0;i<MONONMAPNUM;i++){
		if((int)MonMapInfo[i]./*A.*/Owner!=Hr->Owner+1) continue;
		if(MonMapInfo[i].x!=(short)x) continue;
		if(MonMapInfo[i].y!=(short)y) continue;
		if(MonMapInfo[i].l!=(char)l) continue;
		RETURN(1)
	}
	RETURN(0)
}
static _MapItem_ *E2MWM_Pos=0;
static _Hero_ *E2MWM_Hp=0;
void __stdcall E2MWoMoNoBattle(int,int,int,int,int,int,int,int,int,int)
{
	__asm pusha
	__asm mov    E2MWM_Pos,esi
	__asm mov    E2MWM_Hp,edi
	STARTNA(__LINE__, 0)
	if(_E2MWoMoNoBattle(E2MWM_Pos,E2MWM_Hp)==0){
		__asm{
			mov    edx,2
			mov    ecx,0x697428
			push   0
			push   0xFFFFFFFF
			push   0
			push   0xFFFFFFFF
			push   0
			push   0xFFFFFFFF
			push   0
			push   0xFFFFFFFF
			push   0xFFFFFFFF
			push   0xFFFFFFFF
			mov    eax,0x4F6C00
			call   eax
		}
	}else{
		// оповещаем, что монстры убегают
		RequestPic(ITxt(40,0,&Strings),0x15,E2MWM_Pos->OSType,1);
		__asm{
			mov    eax,0x6992D0
			mov    eax,[eax]
			mov    dword ptr [eax+38],0x7806
		}
	}
	STOP
	__asm popa
}

static Dword E2M_Ecx;
static int m_t,m_st;
void __stdcall Enter2Monster(_MapItem_ *Mi,_Hero_ *Hr,int MixedPos,int GM_ai) //ecx=avd.Manager
{
	_ECX(E2M_Ecx);
	STARTNA(__LINE__, 0)
	m_t=Mi->OType; m_st=Mi->OSType;
	if(ERM2Object(0,GM_ai,MixedPos,Mi,Hr,m_t,m_st)){ // запрещено к посещению
		goto l_exit;
	}
	if(E2MCheck2Kill(GM_ai,Hr,Mi)) goto l_exit;
	if(DoesHeroHas(Hr->Number,CURSE_NDIPL)>=0){ // проклятье присоединения
		_CMonster_ *mp=(_CMonster_ *)Mi;
//    mp->NoRun=1;
		mp->Agression=0x0A;
		if(GM_ai){
			Message(ITxt(125,0,&Strings),1);
//      goto l_exit;
		}
	}
	__asm{
		mov    ecx,E2M_Ecx
		push   GM_ai
		push   MixedPos
		push   Hr
		push   Mi
		mov    eax,0x4A7630
		call   eax
	}
l_exit:
	ERM2Object(1,GM_ai,MixedPos,Mi,Hr,m_t,m_st); // пост триггер
	M_MDisabled=0;
	M_MDisabledNext=0;
	RETURNV
}
void __stdcall Enter2Monster2(_MapItem_ *Mi,_Hero_ *Hr,int MixedPos,int GM_ai) //ecx=avd.Manager
{
	_ECX(E2M_Ecx);
	STARTNA(__LINE__, 0)
	m_t=Mi->OType; m_st=Mi->OSType;
	if(ERM2Object(0,GM_ai,MixedPos,Mi,Hr,m_t,m_st)){ // запрещено к посещению
		goto l_exit;
	}
	if(DoesHeroHas(Hr->Number,CURSE_NDIPL)>=0){ // проклятье присоединения
		if(GM_ai){
			Message(ITxt(125,0,&Strings),1);
			goto l_exit;
		}
	}
	if(E2MCheck2Kill(GM_ai,Hr,Mi)) goto l_exit;
	__asm{
		mov    ecx,E2M_Ecx
		push   GM_ai
		push   MixedPos
		push   Hr
		push   Mi
		mov    eax,0x4A73B0
		call   eax
	}
l_exit:
	ERM2Object(1,GM_ai,MixedPos,Mi,Hr,m_t,m_st); // пост триггер
	M_MDisabled=0;
	M_MDisabledNext=0;
	RETURNV
}

// сделано - отменяем
/*
///////////////////////////////
// Картинки монстров в момент Атаки и умирания.
int MMP_Index[MONNUM-MONNUM_0][2]={
	{150,13},{151,27},{152,41},{153,55},{154,69},{155,83},{156,97},{157,111},{158,131}, // 8 level
	{159,61}, // Ghost
};
int MMP_rind;
static void MakeRInd(void)
{
	int i;
	for(i=0;i<(MONNUM-MONNUM_0);i++){
		if(MMP_Index[i][0]==MMP_rind){ MMP_rind=MMP_Index[i][1]; break; }
	}
}

void MonMapPicOn(void)
{
	asm{
		pusha
		mov   MMP_rind,eax
	}
	MakeRInd();
	asm{
		popa
		mov   eax,MMP_rind
		lea   edx,[ecx][eax*2]
		mov   ecx,[edi+0xE0]
	}
}

void MonMapPicOff(void)
{
	asm{
		pusha
		mov   MMP_rind,eax
	}
	MakeRInd();
	asm{
		popa
		mov   eax,MMP_rind
		lea   edx,[ecx][eax*2]
		mov   ecx,[esi+0xE0]
	}
}
*/
////////////////////////////////////
// Божественные представители
#define GODMONNUM 1000
//#define GODMONTSTRT   (150+9+1) // после привидения
//#define GODMONTNUM     4        // количество богов
static struct _GodRepr_{
	int  SubType;  // подтип существа, 0=нет
	int  HeroInd;  // герой-хозяин, 0=нет
	int  StartDay; // день, когда появился у героя
	int  BonusVal; // Значение параметра бонуса
} GodMonInfo[GODMONNUM],GodMonInfoBack[GODMONNUM];

int       M2M_SSlot,M2M_DSlot;
_MonArr_ *M2M_SStr,*M2M_DStr;
int       M2M_SHi,M2M_DHi;
Dword     M2M_P1,M2M_P2;
Dword    *M2M_sm;
int       M2M_YesItIsGod,M2M_ret;
char      M2M_cret;

static int DoesHeroHas(int hi)
{
	STARTNA(__LINE__, 0)
	if(hi==-1) RETURN(0)
	for(int i=0;i<GODMONNUM;i++){
		if(GodMonInfo[i].SubType==0) continue;
		if(GodMonInfo[i].HeroInd==hi) RETURN(1)
	}
	RETURN(0)
}

int DoesHeroGot(_Hero_ *hr)
{
	int i,mt;
	STARTNA(__LINE__, 0)
	if(hr==0) RETURN(0)
	for(i=0;i<7;i++){
		mt=hr->Ct[i];
		if((mt>=GODMONTSTRT)&&(mt<(GODMONTSTRT+GODMONTNUM))) RETURN(mt)
	}
	RETURN(0)
}

static int AddGod(int gt,int hi)
{
	STARTNA(__LINE__, 0)
	if(hi==-1) RETURN(0)
	for(int i=0;i<GODMONNUM;i++){
		if(GodMonInfo[i].SubType!=0) continue;
		GodMonInfo[i].SubType=gt;
		GodMonInfo[i].HeroInd=hi;
		GodMonInfo[i].StartDay=GetCurDate();
		GodMonInfo[i].BonusVal=1;
//    if(gt==(GODMONTSTRT+3)){ // знания
			GetHeroStr(hi)->PSkill[gt-GODMONTSTRT]+=(char)1;
//    }
		RETURN(0)
	}
	RETURN(-1)
}

static void RemGod(int hi)
{
	STARTNA(__LINE__, 0)
	if(hi==-1) RETURNV
	for(int i=0;i<GODMONNUM;i++){
		if(GodMonInfo[i].SubType==0) continue;
		if(GodMonInfo[i].HeroInd!=hi) continue;
//    if(GodMonInfo[i].SubType==(GODMONTSTRT+3)){ // знания
			GetHeroStr(hi)->PSkill[GodMonInfo[i].SubType-GODMONTSTRT]-=(char)GodMonInfo[i].BonusVal;
//    }
		GodMonInfo[i].SubType=0;
		GodMonInfo[i].StartDay=0;
		GodMonInfo[i].BonusVal=0;
//    return 0;
	}
	RETURNV
}

static void RemGodBack(int hi)
{
	STARTNA(__LINE__, 0)
	if(hi==-1) RETURNV
	for(int i=0;i<GODMONNUM;i++){
		if(GodMonInfoBack[i].SubType==0) continue;
		if(GodMonInfoBack[i].HeroInd!=hi) continue;
		GetHeroStr(hi)->PSkill[GodMonInfoBack[i].SubType-GODMONTSTRT]-=(char)GodMonInfoBack[i].BonusVal;
	}
	RETURNV
}

void BackupGodBonus(void){
	STARTNA(__LINE__, 0)
	Copy((Byte *)GodMonInfo,(Byte *)GodMonInfoBack,sizeof(GodMonInfo));
	RETURNV
}
static _Hero_ *COGB_Hp=0;
static void _CrossOverGodBonus(void)
{
	STARTNA(__LINE__, 0)
	if(COGB_Hp==0) RETURNV
	int ind=COGB_Hp->Number;
	if((ind<0)||(ind>=HERNUM)) RETURNV
	RemGodBack(ind);
	RETURNV
}

__declspec( naked ) void CrossOverGodBonus(void){
	__asm pusha
	_EBX(COGB_Hp);
	_CrossOverGodBonus();
	__asm popa
	__asm xor   edi,edi
	__asm lea   esi,[ebx+0x12D]
	__asm ret
}

int GetGodBonus(int hi,int gt)
{
	STARTNA(__LINE__, 0)
	if(hi==-1) RETURN(0)
	for(int i=0;i<GODMONNUM;i++){
		if(gt!=0){
			if(GodMonInfo[i].SubType!=gt) continue;
		}
		if(GodMonInfo[i].HeroInd!=hi) continue;
		RETURN(GodMonInfo[i].BonusVal)
	}
	RETURN(0)
}

void DaylyGodsBonus(int Owner)
{
	int     bv,gt,hv;
	_Hero_ *hr;
	STARTNA(__LINE__, 0)
	if((GetCurDate()%7)!=1) RETURNV // не понедельник
	for(int i=0;i<GODMONNUM;i++){
		gt=GodMonInfo[i].SubType;
		if(gt==0) continue;
		hr=GetHeroStr(GodMonInfo[i].HeroInd);
		if(hr->Owner!=Owner) continue;
		bv=Random(1,3);
		hv=hr->PSkill[gt-GODMONTSTRT]+bv;
		if(hv>99){
			hv=99;
			bv=hv-hr->PSkill[gt-GODMONTSTRT];
		}
		hr->PSkill[gt-GODMONTSTRT]+=(char)bv;
		GodMonInfo[i].BonusVal+=bv;
	}
	RETURNV
}

static int IsGodMon(_MonArr_ *MonArr,int pos)
{
	STARTNA(__LINE__, 0)
	if((pos<0)||(pos>6)) RETURN(0)
	if((MonArr->Ct[pos]>=GODMONTSTRT)&&
		 (MonArr->Ct[pos]<(GODMONTSTRT+GODMONTNUM))) RETURN(MonArr->Ct[pos])
	RETURN(0)
}

static int M2MCheck(int sm) // для SwapMan sm=1
{ // M2M_SStr,M2M_SSlot, M2M_DStr,M2M_DSlot или M2M_sm
	int       i,is,id;
	_Hero_   *hps,*hpd,*hp;
	_MonArr_ *mas,*mas1;
	int       ps,ps1;

	STARTNA(__LINE__, 0)
	is=id=-1; //hps=hpd=0;
	M2M_YesItIsGod=0;
//asm int 3
	if(sm){
		hps=(_Hero_ *)M2M_sm[M2M_sm[0x48/4]+0x40/4];
		hpd=(_Hero_ *)M2M_sm[M2M_sm[0x4C/4]+0x40/4];
		for(i=0;i<HERNUM;i++){
			hp=GetHeroStr(i);
			if(hp==hps) is=i;
			if(hp==hpd) id=i;
		}
		if(is==-1) RETURN(1) // вообще-то этого быть не может
		if(id==-1) RETURN(1) // вообще-то этого быть не может
		M2M_SStr=(_MonArr_ *)hps->Ct;
		M2M_SSlot=M2M_sm[0x50/4];
		M2M_SHi=is;
		M2M_DStr=(_MonArr_ *)hpd->Ct;
		M2M_DSlot=M2M_sm[0x54/4];
		M2M_DHi=id;

		mas=(_MonArr_ *)hps->Ct;
		ps=M2M_sm[0x50/4];
		mas1=(_MonArr_ *)hpd->Ct;
		ps1=M2M_sm[0x54/4];
	}else{
		if(M2M_SStr==M2M_DStr) RETURN(0) // перемещение внутри
		for(i=0;i<HERNUM;i++){
			hp=GetHeroStr(i);
			if(((_MonArr_ *)hp->Ct)==M2M_SStr) is=i;
			if(((_MonArr_ *)hp->Ct)==M2M_DStr) id=i;
		}
/*
		if(is==-1){ // не от героя - можно
			return 0;
		}
		if(id==-1){ // передаем не герою
			Message("Sorry. Cannot hand it not to Hero",1);
			return 1;
		}
*/
//    if(is!=-1) hps=GetHeroStr(is);
//    if(id!=-1) hpd=GetHeroStr(id);
		M2M_SHi=is;
		M2M_DHi=id;

		mas=M2M_SStr;
		ps=M2M_SSlot;
		mas1=M2M_DStr;
		ps1=M2M_DSlot;
	}
	if(IsGodMon(mas,ps)==0){
		if(IsGodMon(mas1,ps1)==0){
			RETURN(0) // не боги
		}else{ // бог у получателя
			if(is==-1){ // передаем не герою - нельзя
				Message(ITxt(50,0,&Strings),1);
				RETURN(1)
			}
			if(id==is) RETURN(0) // если сам себе
			if(DoesHeroHas(is)){ // у приемника уже есть
				Message(ITxt(51,0,&Strings),1);
				RETURN(1)
			}
			if(Request0(ITxt(52,0,&Strings))==0) RETURN(1) // передумал
			//...
			M2M_YesItIsGod=2;
			RETURN(0)
		}
	}{ // бог у источника
		if(id==-1){ // передаем не герою - нельзя
			Message(ITxt(50,0,&Strings),1);
			RETURN(1)
		}
		if(id==is) RETURN(0) // если сам себе
		if(DoesHeroHas(id)){ // у приемника уже есть
			Message(ITxt(51,0,&Strings),1);
			RETURN(1)
		}
		if(Request0(ITxt(52,0,&Strings))==0) RETURN(1) // передумал
		//...
		M2M_YesItIsGod=1;
		RETURN(0)
	}
//  return 0;
}

static int M2MCheckAfter(void)
{
	int gt;
	STARTNA(__LINE__, 0)
	if(M2M_YesItIsGod==0) RETURN(0)
	if(M2M_YesItIsGod==1){
		if(M2M_SStr!=0){
			if(IsGodMon(M2M_SStr,M2M_SSlot)==0){ // бог НЕ остался у хозяина
				if(M2M_SHi!=-1) RemGod(M2M_SHi);
			}
		}
		if(M2M_DStr!=0){
			gt=IsGodMon(M2M_DStr,M2M_DSlot);
			if(gt==0) RETURN(0) // бога не донес до слота
			if(M2M_DHi!=-1) AddGod(gt,M2M_DHi);
		}
	}else{
		if(M2M_DStr!=0){
			if(IsGodMon(M2M_DStr,M2M_DSlot)==0){ // бог НЕ остался у хозяина
				if(M2M_DHi!=-1) RemGod(M2M_DHi);
			}
		}
		if(M2M_SStr!=0){
			gt=IsGodMon(M2M_SStr,M2M_SSlot);
			if(gt==0) RETURN(0) // бога не донес до слота
			if(M2M_SHi!=-1) AddGod(gt,M2M_SHi);
		}
	}
// ....
	RETURN(0)
}

int M2MRequest(void) // запрос на увольнение
{ // M2M_SStr,M2M_SSlot
	int i,is,gt;
	_Hero_ *hp;

	STARTNA(__LINE__, 0)
	M2M_YesItIsGod=0;
	gt=IsGodMon(M2M_SStr,M2M_SSlot);
	if(gt==0) RETURN(0) // не бог
	M2M_SHi=is=-1;
	for(i=0;i<HERNUM;i++){
		hp=GetHeroStr(i);
		if(((_MonArr_ *)hp->Ct)==M2M_SStr){ is=i; break; }
	}
	if(is==-1) RETURN(0)

	if(RequestPic(ITxt(49,0,&Strings),0x1F+gt-GODMONTSTRT,GetGodBonus(is,gt),2)==0) RETURN(1)
	M2M_SHi=is;
	M2M_YesItIsGod=1;
	RETURN(0)
}
////////////////////////////////////

static int M2MCrExp(int sm) // для SwapMan sm=1
{ // M2M_SStr,M2M_SSlot, M2M_DStr,M2M_DSlot или M2M_sm
	int       i,n,is,id,ts,td,ms,md,hs,hd,xs,ys,ls,xd,yd,ld;
	_Hero_   *hp;
//  _MonArr_ *mas,*mas1;
//  int       ps,ps1;
	int   types=0,typed=0,nums=0,numd=0;
	CRLOC crlocs=0,crlocd=0;
	STARTNA(__LINE__, 0)
	is=id=ts=td=ms=md=hs=hd=-1; //hps=hpd=0;
	if(sm){
		_Hero_ *hps=(_Hero_ *)M2M_sm[M2M_sm[0x48/4]+0x40/4];
		_Hero_ *hpd=(_Hero_ *)M2M_sm[M2M_sm[0x4C/4]+0x40/4];
		for(i=0;i<HERNUM;i++){
			hp=GetHeroStr(i);
			if(hp==hps) is=i;
			if(hp==hpd) id=i;
		}
		if(is==-1) RETURN(1) // вообще-то этого быть не может
		if(id==-1) RETURN(1) // вообще-то этого быть не может
		M2M_SStr=(_MonArr_ *)hps->Ct;
		M2M_SSlot=M2M_sm[0x50/4];
		M2M_SHi=is;
		M2M_DStr=(_MonArr_ *)hpd->Ct;
		M2M_DSlot=M2M_sm[0x54/4];
		M2M_DHi=id;
		types=CE_HERO; crlocs=MAKEHS(is,M2M_SSlot); nums=M2M_SStr->Cn[M2M_SSlot];
		typed=CE_HERO; crlocd=MAKEHS(id,M2M_DSlot); numd=M2M_DStr->Cn[M2M_DSlot];
		if(is!=id){
			if(M2M_DStr->Ct[M2M_DSlot]==-1){ // перемещает в пустое место
				for(i=n=0;i<7;i++){
					if(i==M2M_SSlot) continue;
					if(M2M_SStr->Ct[i]==-1) continue; // пустой слот
					++n;
				}
				if(n==0) RETURN(0) // хочет перенести последний стэк - не получится
			}
			if(M2M_SStr->Ct[M2M_SSlot]==-1){ // перемещает в пустое место
				for(i=n=0;i<7;i++){
					if(i==M2M_DSlot) continue;
					if(M2M_DStr->Ct[i]==-1) continue; // пустой слот
					++n;
				}
				if(n==0) RETURN(0) // хочет перенести последний стэк - не получится
			}
		}
		if(M2M_SStr->Ct[M2M_SSlot]==M2M_DStr->Ct[M2M_DSlot]){ // одинаковые типы
			CrExpoSet::HComb(types,typed,crlocs,crlocd,M2M_SStr->Ct[M2M_SSlot],M2M_DStr->Ct[M2M_DSlot],nums,numd);
		}else{
			CrExpoSet::HMove(types,typed,crlocs,crlocd,M2M_SStr->Ct[M2M_SSlot],M2M_DStr->Ct[M2M_DSlot],nums,numd);
		}
		RETURN(0)
	}else{
		for(i=0;i<HERNUM;i++){
			hp=GetHeroStr(i);
			if(((_MonArr_ *)hp->Ct)==M2M_SStr) is=i;
			if(((_MonArr_ *)hp->Ct)==M2M_DStr) id=i;
		}
		if((is==-1)||(id==-1)){ // должен быть город
			_CastleSetup_ *cst,*cst0;
			cst=cst0=GetCastleBase();
			if(cst==0) goto CDone; // нет замков
			int cn=GetCastleNum(); //
			for(i=0;i<cn;i++,cst++){
				if((_MonArr_ *)cst->GuardsT==M2M_SStr) ts=i;
				if((_MonArr_ *)cst->GuardsT==M2M_DStr) td=i;
			}
			if(ts!=-1){ xs=cst0[ts].x; ys=cst0[ts].y; ls=cst0[ts].l; }
			if(td!=-1){ xd=cst0[td].x; yd=cst0[td].y; ld=cst0[td].l; }
		}
CDone:
		if(((is+ts)==-2)||((id+td)==-2)){ // должен быть шахта
			_Mine_ *cst,*cst0;
			cst=cst0=GetMineBase();
			if(cst==0) goto MDone; // нет замков
			int cn=GetMineNum(); //
			for(i=0;i<cn;i++,cst++){
				if((_MonArr_ *)cst->GType==M2M_SStr) ms=i;
				if((_MonArr_ *)cst->GType==M2M_DStr) md=i;
			}
			if(ms!=-1){ xs=cst0[ms].xe; ys=cst0[ms].ye; ls=cst0[ms].le; }
			if(md!=-1){ xd=cst0[md].xe; yd=cst0[md].ye; ld=cst0[md].le; }
		}
MDone:
		if(((is+ts+ms)==-3)||((id+td+md)==-3)){ // должен быть горн
			_Horn_ *cst,*cst0;
			cst=cst0=GetHornBase();
			if(cst==0) goto HDone; // нет замков
			int cn=GetHornNum(); //
			for(i=0;i<cn;i++,cst++){
				if((_MonArr_ *)cst->GType==M2M_SStr) hs=i;
				if((_MonArr_ *)cst->GType==M2M_DStr) hd=i;
			}
			if(hs!=-1){ xs=cst0[hs].ex; ys=cst0[hs].ey; ls=cst0[hs].el; }
			if(hd!=-1){ xd=cst0[hd].ex; yd=cst0[hd].ey; ld=cst0[hd].el; }
		}
HDone:
		if(is!=-1){ // источник-герой
			types=CE_HERO; crlocs=MAKEHS(is,M2M_SSlot); nums=M2M_SStr->Cn[M2M_SSlot];
		}else if(ts!=-1){ // источник-город
			types=CE_TOWN; crlocs=MAKETS(xs,ys,ls,M2M_SSlot); nums=M2M_SStr->Cn[M2M_SSlot];
		}else if(ms!=-1){ // источник-шахта
			types=CE_MINE; crlocs=MAKEMS(xs,ys,ls,M2M_SSlot); nums=M2M_SStr->Cn[M2M_SSlot];
		}else if(hs!=-1){ // источник-горн
			types=CE_HORN; crlocs=MAKEZS(xs,ys,ls,M2M_SSlot); nums=M2M_SStr->Cn[M2M_SSlot];
		}
		if(id!=-1){ // приемник-герой
			typed=CE_HERO; crlocd=MAKEHS(id,M2M_DSlot); numd=M2M_DStr->Cn[M2M_DSlot];
		}else if(td!=-1){ // приемник-город
			typed=CE_TOWN; crlocd=MAKETS(xd,yd,ld,M2M_DSlot); numd=M2M_DStr->Cn[M2M_DSlot];
		}else if(md!=-1){ // источник-шахта
			typed=CE_MINE; crlocd=MAKEMS(xd,yd,ld,M2M_DSlot); numd=M2M_DStr->Cn[M2M_DSlot];
		}else if(hd!=-1){ // источник-горн
			typed=CE_HORN; crlocd=MAKEZS(xd,yd,ld,M2M_DSlot); numd=M2M_DStr->Cn[M2M_DSlot];
		}
		if(is!=id){
			if(is!=-1){ // Герой
				if(M2M_DStr->Ct[M2M_DSlot]==-1){ // перемещает в пустое место
					for(i=n=0;i<7;i++){
						if(i==M2M_SSlot) continue;
						if(M2M_SStr->Ct[i]==-1) continue; // пустой слот
						++n;
					}
					if(n==0) RETURN(0) // хочет перенести последний стэк - не получится
				}
			}
			if(id!=-1){ // Герой
				if(M2M_SStr->Ct[M2M_SSlot]==-1){ // перемещает в пустое место
					for(i=n=0;i<7;i++){
						if(i==M2M_DSlot) continue;
						if(M2M_DStr->Ct[i]==-1) continue; // пустой слот
						++n;
					}
					if(n==0) RETURN(0) // хочет перенести последний стэк - не получится
				}
			}
		}
		if(M2M_SStr->Ct[M2M_SSlot]==M2M_DStr->Ct[M2M_DSlot]){ // одинаковые типы
			CrExpoSet::HComb(types,typed,crlocs,crlocd,M2M_SStr->Ct[M2M_SSlot],M2M_DStr->Ct[M2M_DSlot],nums,numd);
		}else{
			CrExpoSet::HMove(types,typed,crlocs,crlocd,M2M_SStr->Ct[M2M_SSlot],M2M_DStr->Ct[M2M_DSlot],nums,numd);
		}
		RETURN(0)
	}
//  RETURN(0)
}
////////////////////////////////////
// 449B60
void __stdcall M2MSmooth(int SSlot,Dword DStr,int DSlot,Dword P2v,Dword P1v)
{ // ECX -> SStr
// экран встречи героев
	_ECX(M2M_SStr); 
	M2M_DStr=(_MonArr_ *)DStr;
	M2M_SSlot=SSlot; M2M_DSlot=DSlot;
	M2M_P1=P1v; M2M_P2=P2v;
// плавная передача существ (Shift+LMouse)
	//if(WoG){
		__asm  pusha
		M2M_ret=M2MCheck(0);
		__asm  popa
		if(M2M_ret) return;
	//}
	STARTNA(__LINE__, 0)
	__asm{
		mov    ecx,M2M_SStr
		push   P1v
		push   P2v
		push   M2M_DSlot
		push   M2M_DStr
		push   M2M_SSlot
		mov    eax,0x449B60
// вызов диалога плавной передачи монстра
		call   eax
// диалог закрылся
	}
	STOP
	//if(WoG){
		__asm  pusha
		M2MCheckAfter();
		__asm  popa
	//}
}
/*
esi -> townManager
												mov    edx,[esi+00000134] -> стр. приемника
005D520D 8B8E38010000   mov    ecx,[esi+00000138] = номер приемного стека
005D5213 8B526C         mov    edx,[edx+6C]       -> _MonArr_ приемника
005D5216 8B7C8A1C       mov    edi,[edx+4*ecx+1C] = число монстров в приемнике
005D521A 8D448A1C       lea    eax,[edx+4*ecx+1C]
005D521E 8B8E2C010000   mov    ecx,[esi+0000012C] -> стр. источника
005D5224 8B9630010000   mov    edx,[esi+00000130] = номер исходного стека
005D522A 8B496C         mov    ecx,[ecx+6C]       -> _MonArr_ приемника
005D522D?8B54911C       mov    edx,[ecx+4*edx+1C] = число монстров в источнике
005D5231 03FA           add    edi,edx
005D5233 8938           mov    [eax],edi
005D5235 8B862C010000   mov    eax,[esi+0000012C]
005D523B 8B8E30010000   mov    ecx,[esi+00000130]
005D5241 8B506C         mov    edx,[eax+6C]
005D5244 C7048AFFFFFFFF mov    dword ptr [edx+4*ecx],FFFFFFF = -1 тип
005D524B 8B8E2C010000   mov    ecx,[esi+0000012C]
005D5251 8B8630010000   mov    eax,[esi+00000130]
005D5257 8B516C         mov    edx,[ecx+6C]
005D525A 895C821C       mov    [edx+4*eax+1C],ebx =0 число
005D525E E91DFFFFFF     jmp    H3WOG.005D5180
*/

////////////////////////////////////
// 005D520D
void M2MCombine(void)
{ // ESI -> townManager
// один стэк добавляется к другому в городе
	__asm{
		mov    eax,[esi+0x12C]
		mov    eax,[eax+0x6C]
		mov    M2M_SStr,eax
		mov    eax,[esi+0x130]
		mov    M2M_SSlot,eax
		mov    eax,[esi+0x134]
		mov    eax,[eax+0x6C]
		mov    M2M_DStr,eax
		mov    eax,[esi+0x138]
		mov    M2M_DSlot,eax
	}
	STARTNA(__LINE__, 0)
	//if(WoG){
		__asm  pusha
		M2M_ret=M2MCheck(0);
		__asm  popa
		if(M2M_ret) RETURNV;
		if(PL_CrExpEnable){
			__asm  pusha
			M2M_ret=M2MCrExp(0);
			__asm  popa
			if(M2M_ret) RETURNV;
		}
	//}
	__asm  pusha
	M2M_DStr->Cn[M2M_DSlot]+=M2M_SStr->Cn[M2M_SSlot];
	M2M_SStr->Cn[M2M_SSlot]=0;
	M2M_SStr->Ct[M2M_SSlot]=-1;
	__asm  popa
	STOP
/*
	asm{
		mov    edx,[esi+0x134] //-> стр. приемника
		mov    ecx,[esi+0x138] //= номер приемного стека
		mov    edx,[edx+0x6C]  //-> _MonArr_ приемника
		mov    edi,[edx+4*ecx+0x1C] // = число монстров в приемнике
		lea    eax,[edx+4*ecx+0x1C]
		mov    ecx,[esi+0x12C] //-> стр. источника
		mov    edx,[esi+0x130] //= номер исходного стека
		mov    ecx,[ecx+0x6C]  //     -> _MonArr_ приемника
		mov    edx,[ecx+4*edx+0x1C] //= число монстров в источнике
		add    edi,edx
		mov    [eax],edi
		mov    eax,[esi+0x12C]
		mov    ecx,[esi+0x130]
		mov    edx,[eax+0x6C]
		mov    dword ptr [edx+4*ecx],-1
		mov    ecx,[esi+0x12C]
		mov    eax,[esi+0x130]
		mov    edx,[ecx+0x6C]
		mov    [edx+4*eax+0x1C],0 число
	}
*/
}
////////////////////////////////////
// 44AA30
void __stdcall M2MStack(int SSlot,Dword DStr,int DSlot)
{ // ECX -> SStr
// передача внутри города от одного героя - другому или в гарнизон
	_ECX(M2M_SStr); 
	M2M_DStr=(_MonArr_ *)DStr;
	M2M_SSlot=SSlot; M2M_DSlot=DSlot;
	//if(WoG){
		__asm  pusha
		M2M_ret=M2MCheck(0);
		__asm  popa
		if(M2M_ret) return;
		if(PL_CrExpEnable){
			__asm  pusha
			M2M_ret=M2MCrExp(0);
			__asm  popa
			if(M2M_ret) return;
		}
	//}
	STARTNA(__LINE__, 0)
	__asm{
		mov    ecx,M2M_SStr
		push   M2M_DSlot
		push   M2M_DStr
		push   M2M_SSlot
// функция вызывается при люой передачи из одного слота в другой
		mov    eax,0x44AA30
// уже выбран приемный слот - перемещаем (перерисовки нет)
		call   eax
	}
	STOP
	//if(WoG){
		__asm  pusha
		M2MCheckAfter();
		__asm  popa
	//}
}
////////////////////////////////////
// 5B10B0
void M2MSwap(void)
{ // ECX -> swapManager
// экран встречи двух героев - передача стека целиком
	_ECX(M2M_sm);
	//if(WoG){
		__asm  pusha
		M2M_ret=M2MCheck(1);
		__asm  popa
		if(M2M_ret) return;
		if(PL_CrExpEnable){
			__asm  pusha
			M2M_ret=M2MCrExp(1);
			__asm  popa
			if(M2M_ret) return;
		}
	//}
	STARTNA(__LINE__, 0)
	__asm{
		mov    ecx,M2M_sm
		mov    eax,0x5B10B0
		call   eax
	}
	STOP
	//if(WoG){
		__asm  pusha
		M2MCheckAfter(); // подготовлены ранее
		__asm  popa
	//}
}
////////////////////////////////////
static _MonArr_ M2M_MonArrBack[2];
void _M2MHero2TownBefore(void){
	STARTNA(__LINE__, 0)
	for(int i=0;i<7;i++){
		M2M_MonArrBack[0].Ct[i]=M2M_SStr->Ct[i];
		M2M_MonArrBack[0].Cn[i]=M2M_SStr->Cn[i];
		M2M_MonArrBack[1].Ct[i]=M2M_DStr->Ct[i];
		M2M_MonArrBack[1].Cn[i]=M2M_DStr->Cn[i];
	}
	RETURNV
}
void _M2MHero2Town(void){
	STARTNA(__LINE__, 0)
/*
	int i,j,k,x,y,l,id;
	int types,typed,nums,numd,mts,mtd;
	CRLOC crlocs,crlocd;
	_Hero_ *hp=(_Hero_ *)((char *)M2M_DStr-0x91); // 0x91 смещение до начала структуры героя
	_CastleSetup_ *cst=(_CastleSetup_ *)((char *)M2M_SStr-0xE0); // 0xE0 смещение до начала структуры города
	types=CE_TOWN; typed=CE_HERO;
	x=cst->x; y=cst->y; l=cst->l;
	id=hp->Number;
	for(i=0;i<7;i++){
Again:
		mts= M2M_MonArrBack[0].Ct[i];
		nums=M2M_MonArrBack[0].Cn[i];
		if((mts==-1)||(nums==0)) continue;
		crlocs=MAKETS(x,y,l,i);
		for(j=0;j<7;j++){
			mtd= M2M_MonArrBack[1].Ct[j];
			numd=M2M_MonArrBack[1].Cn[j];
			if(mtd!=mts) continue;
			// совпадают
			crlocd=MAKEHS(id,j);
			CrExpoSet::HComb(types,typed,crlocs,crlocd,M2M_MonArrBack[0].Ct[i],M2M_MonArrBack[1].Ct[j],nums,numd);
			M2M_MonArrBack[0].Ct[i]=-1;
			M2M_MonArrBack[1].Cn[j]+=M2M_MonArrBack[0].Cn[j]; M2M_MonArrBack[0].Cn[i]=0;
			goto Found;
		}
		for(j=0;j<7;j++){
			mtd= M2M_MonArrBack[1].Ct[j];
			if(mtd!=-1) continue;
			// пустой
			crlocd=MAKEHS(id,j);
			CrExpoSet::HComb(types,typed,crlocs,crlocd,mts,-1,nums,0);
			M2M_MonArrBack[1].Ct[j]=M2M_MonArrBack[0].Ct[i]; M2M_MonArrBack[0].Ct[i]=-1;
			M2M_MonArrBack[1].Cn[j]=M2M_MonArrBack[0].Cn[i]; M2M_MonArrBack[0].Cn[i]=0;
			goto Found;
		}
		for(j=0;j<7;j++){
			mtd= M2M_MonArrBack[1].Ct[j];
			for(k=j+1;k<7;k++){
				if(mtd==M2M_MonArrBack[1].Ct[k]){
					// сливаем вместе
					crlocs=MAKEHS(id,k);
					crlocd=MAKEHS(id,j);
					CrExpoSet::HComb(typed,typed,crlocs,crlocd,mtd,mtd,M2M_MonArrBack[1].Ct[k],M2M_MonArrBack[1].Ct[j]);
					M2M_MonArrBack[1].Ct[k]=-1;
					M2M_MonArrBack[1].Cn[j]+=M2M_MonArrBack[1].Cn[k]; M2M_MonArrBack[1].Cn[k]=0;
					goto Again;
				}
			}
		}
Found:;
	}
	*/
	int i,t,n,x,y,l,id;
	_MonArr_ *p0,*p1,MonArrTmp[4];
	CrExpo *cr;
	_Hero_ *hp=(_Hero_ *)((char *)M2M_DStr-0x91); // 0x91 смещение до начала структуры героя
	_CastleSetup_ *cst=(_CastleSetup_ *)((char *)M2M_SStr-0xE0); // 0xE0 смещение до начала структуры города
	x=cst->x; y=cst->y; l=cst->l;
	id=hp->Number;
	int Expo,Arts;
	// set town numbers
//__asm int 3
	for(i=0;i<7;i++){
		cr=CrExpoSet::Find(CE_TOWN,MAKETS(x,y,l,i));
		if(cr==0) Expo=0;
		else{
			cr->RecalcExp2RealNum(M2M_MonArrBack[0].Cn[i],M2M_MonArrBack[0].Ct[i]);
			if(cr->Num==0) Expo=0;
			else Expo=cr->Expo;
		}
		MonArrTmp[0].Ct[i]=MonArrTmp[2].Ct[i]=M2M_MonArrBack[0].Ct[i];
		MonArrTmp[0].Cn[i]=Expo*2*M2M_MonArrBack[0].Cn[i]+1;
		if(cr==0){ MonArrTmp[2].Cn[i]=1; }else{ MonArrTmp[2].Cn[i]=cr->GetArtNums()*16+1; }
		if(MonArrTmp[0].Ct[i]==-1){ MonArrTmp[0].Cn[i]=0; }
		if(MonArrTmp[2].Ct[i]==-1){ MonArrTmp[2].Cn[i]=0; }
	}
	// set hero numbers
	for(i=0;i<7;i++){
		cr=CrExpoSet::Find(CE_HERO,MAKEHS(id,i));
		if(cr==0) Expo=0;
		else{
			cr->RecalcExp2RealNum(M2M_MonArrBack[1].Cn[i],M2M_MonArrBack[1].Ct[i]);
			if(cr->Num==0) Expo=0;
			else Expo=cr->Expo;
		}
		MonArrTmp[1].Ct[i]=MonArrTmp[3].Ct[i]=M2M_MonArrBack[1].Ct[i];
		MonArrTmp[1].Cn[i]=Expo*2*M2M_MonArrBack[1].Cn[i]+1;
		if(cr==0){ MonArrTmp[3].Cn[i]=1; }else{ MonArrTmp[3].Cn[i]=cr->GetArtNums()*16+1; }
		if(MonArrTmp[1].Ct[i]==-1){ MonArrTmp[1].Cn[i]=0; }
		if(MonArrTmp[3].Ct[i]==-1){ MonArrTmp[3].Cn[i]=0; }
	}
	// combine experience
	p0=&MonArrTmp[0];
	p1=&MonArrTmp[1];
	__asm{
		mov   eax,p0
		mov   ecx,p1
		push  eax
//int 3
		mov   eax,0x44B2F0
		call  eax
		mov   BDummy,al
	}
	if(BDummy){ // done
		// combine arts
		p0=&MonArrTmp[2];
		p1=&MonArrTmp[3];
		__asm{
			mov   eax,p0
			mov   ecx,p1
			push  eax
			mov   eax,0x44B2F0
			call  eax
			mov   BDummy,al
		}
		for(i=0;i<7;i++){
			CrExpoSet::Del(CE_TOWN,MAKETS(x,y,l,i));
			CrExpoSet::Del(CE_HERO,MAKEHS(id,i));
			t=hp->Ct[i];
			n=hp->Cn[i];
			Expo=MonArrTmp[1].Cn[i]/2;
			Arts=MonArrTmp[3].Cn[i]/16;
			if((t!=-1)&&(n!=0)){
				cr=CrExpoSet::FindEmpty();
				if(cr==0){ MError("No more rooms in experience system"); RETURNV }
				cr->SetN(CE_HERO,MAKEHS(id,i),t,n,Expo/n,Arts);
			}
		}
	}else{ // impossible
	}
	RETURNV
}
// 44B2F0
// Add Town garrizon in a Hero's army
/*
void fun(_MonArr_ *Hma,_MonArr_ *Tma){
	int i,j,k;
	_MonArr_ hma,tma;
	for(i=0;i<7;i++){
		hma.Ct[i]=Hma->Ct[i];
		hma.Cn[i]=Hma->Cn[i];
		tma.Ct[i]=Tma->Ct[i];
		tma.Cn[i]=Tma->Cn[i];
	}
	for(i=0;i<7;i++){
ThisOneAgain:
		if(tma.Ct[i]==-1) continue;
		if(tma.Cn[i]==0) continue;
		for(j=0;j<7;j++){
			if(hma.Ct[j]==tma.Ct[i]){
				tma.Ct[i]=-1;
				hma.Cn[j]+=tma.Cn[i]; tma.Cn[i]=0;
				goto ThisDone;
			}
		}
		for(j=0;j<7;j++){
			if(hma.Ct[j]==-1){
				hma.Ct[j]=tma.Ct[i]; tma.Ct[i]=-1;
				hma.Cn[j]=tma.Cn[i]; tma.Cn[i]=0;
				goto ThisDone;
			}
		}
		for(j=0;j<7;j++){
			for(k=j+1;k<7;k++){
				if(hma.Ct[j]==hma.Ct[k]){
					tma.Ct[k]=-1;
					hma.Cn[j]+=tma.Cn[i]; tma.Cn[k]=0;
					goto ThisOneAgain;
				}
			}
		}
ThisDone:;
	}
}
*/
char __stdcall M2MHero2Town(_MonArr_ *mas){
	_ECX(M2M_DStr);
	M2M_SStr=mas;
	__asm  pusha
	STARTNA(__LINE__, 0)
	_M2MHero2TownBefore();
	__asm{
		mov   eax,M2M_SStr
		mov   ecx,M2M_DStr
		push  eax
		mov   eax,0x44B2F0
		call  eax
		mov   M2M_cret,al
	}
	if(M2M_cret){
		if(/*WoG &&*/ PL_CrExpEnable){
			_M2MHero2Town();
		}
	}
	STOP
	__asm  popa
	return M2M_cret;
}
void _M2MHero2TownAutoWoG(void)
{
	STARTNA(__LINE__, 0)
	int i,j,k,m,t,t2,n,n2,x,y,l,id,val;
	CrExpo *cr,*cr2;
	_Hero_ *hp=(_Hero_ *)((char *)M2M_DStr-0x91); // 0x91 смещение до начала структуры героя
	_CastleSetup_ *cst=(_CastleSetup_ *)((char *)M2M_SStr-0xE0); // 0xE0 смещение до начала структуры города
	x=cst->x; y=cst->y; l=cst->l;
	id=hp->Number;
	int Expo,Expo2,Arts,Arts2;
	for(i=0;i<7;i++){
		t=hp->Ct[i];
		if(t==-1) continue; // empty stack - skip
		n=hp->Cn[i];
		cr=CrExpoSet::Find(CE_HERO,MAKEHS(id,i));
		if(cr==0) Expo=0; else{ 
			cr->RecalcExp2RealNum(n,t); if(cr->Num==0) Expo=0; else Expo=cr->Expo; 
			Arts=cr->GetArtNums();
		}
		// add all town creatures if they are the same
		for(j=0;j<7;j++){
			t2=cst->GuardsT[j];
			if(t2==t){ // combine
				n2=cst->GuardsN[j];
				cr2=CrExpoSet::Find(CE_TOWN,MAKETS(x,y,l,j));
				if(cr2==0) Expo2=0; else{ 
					cr2->RecalcExp2RealNum(n2,t2); if(cr2->Num==0) Expo2=0; else Expo2=cr2->Expo; 
					Arts2=cr2->GetArtNums();
				}
				CrExpoSet::Del(CE_TOWN,MAKETS(x,y,l,j)); CrExpoSet::Del(CE_HERO,MAKEHS(id,i));
				val=Expo*n+Expo2*n2; n+=n2; if(n<=0) n=1; Expo=val/n; Arts+=Arts2;
				cr=CrExpoSet::FindEmpty();
				if(cr==0){ MError("No more rooms in experience system"); RETURNV }
				cr->SetN(CE_HERO,MAKEHS(id,i),t,n,Expo,Arts);
				hp->Cn[i]+=cst->GuardsN[j]; cst->GuardsT[j]=0xFFFFFFFF; cst->GuardsN[j]=0;
			}
		}
	}
	int Arr[7][5];
	k=-1;
	// sort out all creatures of a type
	for(j=0;j<7;j++){
		t2=cst->GuardsT[j];
		if(t2==-1) continue; // skip stack
		n2=cst->GuardsN[j];
		cr2=CrExpoSet::Find(CE_TOWN,MAKETS(x,y,l,j));
		if(cr2==0) Expo2=0; else{ 
			cr2->RecalcExp2RealNum(n2,t2); if(cr2->Num==0) Expo2=0; else Expo2=cr2->Expo; 
			Arts2=cr2->GetArtNums();
		}
		if(k==-1){ // first stack
			k=0; Arr[k][0]=t2; Arr[k][1]=n2; Arr[k][2]=Expo2; Arr[k][3]=Arts2;
			continue;
		}else{
			// search for the same
			for(m=0;m<=k;m++){
				if(Arr[m][0]!=t2) continue; // different types
				val=Arr[m][1]+n2; if(val<=0) val=1;
				Arr[m][2]=(Arr[m][2]*Arr[m][1]+Expo2*n2)/val; // expo
				Arr[m][3]+=Arts2; // arts
				Arr[m][1]=val;
				goto L_Added;
			}
			++k; Arr[k][0]=t2; Arr[k][1]=n2; Arr[k][2]=Expo2; Arr[k][3]=Arts2;
L_Added:;
		}
	}
	// calc the weight
//__asm int 3
	for(j=0;j<=k;j++){
		val=CrExpMod::GetRank(Arr[j][0],Arr[j][2]); // get rank
		// AIVal*n*(20+Lvl)/20
		Arr[j][4]=MonTable[Arr[j][0]].AIvalue*Arr[j][1]*(20+val)/20;
	}
	// put most important to hero army
	for(i=0;i<7;i++){
		if(hp->Ct[i]!=-1) continue; // NOT empty stack - skip
		hp->Cn[i]=0;
		CrExpoSet::Del(CE_HERO,MAKEHS(id,i));
		// find the max weight
		for(j=0,m=-1;j<=k;j++){
			if(m==-1){ 
				if(Arr[j][4]<=0) continue;
				val=Arr[j][4]; m=j; 
			}
			else if(Arr[j][4]>val){ val=Arr[j][4]; m=j; }
		}
		if(m==-1) break; // no more found
		t2=Arr[m][0];
		for(j=0;j<7;j++){
			if((int)cst->GuardsT[j]!=t2) continue; // not this type
			hp->Ct[i]=t2; hp->Cn[i]+=cst->GuardsN[j];
			cst->GuardsT[j]=0xFFFFFFFF; cst->GuardsN[j]=0;
			CrExpoSet::Del(CE_TOWN,MAKETS(x,y,l,j));
		}
		if(hp->Cn[i]==0) continue;
		cr=CrExpoSet::FindEmpty();
		if(cr==0){ MError("No more rooms in experience system"); RETURNV }
		cr->SetN(CE_HERO,MAKEHS(id,i),t2,Arr[m][1],Arr[m][2],Arr[m][3]);
		Arr[m][4]=0;
	}
	RETURNV
}
// перевод героя в гарнизон города при атаке
void __stdcall M2MHero2Town2(_MonArr_ *mas){
	_ECX(M2M_DStr);
	M2M_SStr=mas;
	__asm  pusha
	_M2MHero2TownAutoWoG();
	__asm  popa
}
/*
void __stdcall M2MHero2Town2(_MonArr_ *mas){
	_ECX(M2M_DStr);
	M2M_SStr=mas;
	__asm  pusha
	STARTN(__FILENUM__*1000000+__LINE__)
	_M2MHero2TownBefore();
	__asm{
		mov   eax,M2M_SStr
		mov   ecx,M2M_DStr
		push  eax
		mov   eax,0x44B4F0
		call  eax
		mov   M2M_cret,al
	}
	if(M2M_cret){
		if(WoG && PL_CrExpEnable){
			_M2MHero2Town();
		}
	}
	STOP
	__asm  popa
}
*/
////////////////////////////////////
// 44A830
void __stdcall M2MDismiss(int SSlot)
{ // ECX -> SStr
	_ECX(M2M_SStr);
	M2M_SSlot=SSlot;
	M2M_DStr=0;
	M2M_DSlot=0;
	//if(WoG){
		__asm  pusha
		M2M_ret=M2MRequest();
		__asm  popa
		if(M2M_ret) return;
	//}
	STARTNA(__LINE__, 0)
	__asm{
		mov    ecx,M2M_SStr
		push   M2M_SSlot
		mov    eax,0x44A830
		call   eax
	}
	STOP
	//if(WoG){
		__asm  pusha
		M2MCheckAfter();
		__asm  popa
	//}
}
////////////////////
int Enter2Bank(int GM_ai,_MapItem_ *Mi,_Hero_ *Hr,Dword /*avd.Manager*/,Dword* Object)
{
	int st,gt;
	// проверка на новый двелинг
//  if((CStr=FindCastleStr(Mi))==0) return -1; // не нашли дв - значит не оно
	STARTNA(__LINE__, 0)
	if(Mi->OType!=16) RETURN(-1) // НЕ CrBanck
	st=Mi->OSType;
	if((st<13)||(st>16)) RETURN(-1) // не God
	*Object=(Dword)Hr;
	if(Mi->SetUp&0x02000000){ // уже посещен
		if(GM_ai) Message(ITxt(48,0,&Strings),1);
		RETURN(1)
	}
	if(GM_ai) RequestPic(ITxt(46,0,&Strings),0x15,st+GODMONTSTRT-13,1);
	gt=DoesHeroGot(Hr);
	if(gt){ // уже есть God
		if(GM_ai) Request2Pic(ITxt(45,0,&Strings),0x15,gt,0x15,st+GODMONTSTRT-13,1);
		RETURN(1) // нечего ему здесь делать
	}
	RETURN(0)
}

void EnterOutOfBank(int GM_ai,Dword Object)
{
	int gt;
	_Hero_ *hr=(_Hero_ *)Object;
	STARTNA(__LINE__, 0)
	gt=DoesHeroGot(hr);
	if(gt){ // есть God
		AddGod(gt,hr->Number);
		if(GM_ai) RequestPic(ITxt(47,0,&Strings),0x1F+gt-GODMONTSTRT,1,1);
	}
	RETURNV
/*
	int ind,i;
	int Day=GetCurDate();
	_DwellMapInfo_ *di;

	if(dw==0) return; // вообще-то невозможно
	ind=FindDwellInfo(dw);
	if(ind==-1) return; // такого не было - такого быть не должно
	di=&DwellMapInfo[ind];
	if(di->Owner!=dw->Owner){ // хозяин сменился - захвачен
		di->Owner=dw->Owner;   // модифицируем
		di->GotDay=(short)Day;
		di->Mon2Town=0; // включится только со след недели
		if(dw->Num2Hire[0]!=0){ // не нанял
			if(GM_ai){
				RequestPic(ITxt(42,0,&Strings),0x15,150-80+dw->SType,1); // тип монстра
			}
		}
		// покажем всем
		for(i=0;i<8;i++){
			ShowArea(dw->x,dw->y,dw->l,i,1);
		}
	}
*/
}
////////////////////
static _MonArr_ *G2B_MArrD,*G2B_MArrA;
_Hero_   *G2B_HrD=0,*G2B_HrA=0;
static int       G2B_OwnerD;
static Dword     G2B_MixedPos;
static Dword     G2B_AdvMan;
static int       G2B_EnableBattle;
			 int       G2B_CompleteAI=1; // полностью AI битва (1) или нет (0)
static int FirstGodAttack;
static int FirstTowerAttack;
static int   G2B_HPA,G2B_HPD,G2B_EXA,G2B_EXD;
static float G2B_KA,G2B_KD;
static int SGBonusChance[][2]={{50,3},{100,6},{150,12},{200,24},{300,60},{500,90}};
static int SGArts[]={7,8,9,13,14,15,19,20,21,25,
										 26,27,37,38,41,42,45,46,47,48,
										 49,50,51,52,53,54,55,57,60,61,
										 63,64,65,66,67,68,73,74,76,77,
										 94,95,100,101,102,103,104,105,106,107,
										 118,119,120, 146,147,148,149,150,151,152,
										 153,154,155};

void G2B_Prepare(void)
{
//__asm int 3
	int i,t,n;
	STARTNA(__LINE__, 0)
	G2B_EnableBattle=1;
	NPCAttack0(G2B_MArrA,G2B_MArrD,G2B_HrA,G2B_HrD);
	CrExpoSet::StartAllBF(G2B_MArrA,G2B_MArrD);
	// 3.58
	ERMFlags[997]=0;
	ERMFlags[996]=0;
	do{
		if(G2B_OwnerD==-1) break; // defender is neutral (AI)
		if(IsAI(G2B_OwnerD)) break; // defender is AI
//    if(IsThis(G2B_OwnerD)) break; // defender is a Human at this PC
		_Hero_ *hp=G2B_HrA; if(hp!=0) i=hp->Owner; else break; // get attacker owner or break (nonsense)
		if(IsThis(i) && IsThis(G2B_OwnerD)) break; // (both at this PC) 3.58f fix for distant PC 
		ERMFlags[996]=1;
		if(IsAI(i)) ; // Attacker is AI (AIvsH)
		else ERMFlags[997]=1; // Attacker is Human (HvsH)
	}while(0);
	if(BADistFlag){
		BACall(50,G2B_HrA);
	}else{
		BACall(0,G2B_HrA);
	}
	BACall(52,G2B_HrA);
	NPCAttack(G2B_MArrA,G2B_MArrD,G2B_HrA,G2B_HrD);
//  CrExpoSet::StartAllBF(G2B_MArrA,G2B_MArrD);
	if(G2B_EnableBattle==0) RETURNV
	BACall_Day=-1;
	BACall_Turn=-1;
	FirstGodAttack=1; // будет сообщение при первом ударе богом.
// SG bonus
	G2B_HPA=0;
	for(i=0;i<7;i++){
		t=G2B_MArrA->Ct[i]; n=G2B_MArrA->Cn[i];
		if((t!=-1)&&(n!=0)){ G2B_HPA+=n*MonTable[t].HitPoints; }
	}
	G2B_HPD=0;
	for(i=0;i<7;i++){
		t=G2B_MArrD->Ct[i]; n=G2B_MArrD->Cn[i];
		if((t!=-1)&&(n!=0)){ G2B_HPD+=n*MonTable[t].HitPoints; }
	}
	if(G2B_HrA!=0){ G2B_EXA=G2B_HrA->Exp; }else{ G2B_EXA=0; }
	if(G2B_HrD!=0){ G2B_EXD=G2B_HrD->Exp; }else{ G2B_EXD=0; }
	G2B_KA=G2B_HPA;
	if(G2B_EXA>0){
		if(G2B_EXA>=G2B_HPA){ G2B_KA=2.0*G2B_HPA*G2B_EXA/(G2B_HPA+G2B_EXA); }
	}
	G2B_KD=G2B_HPD;
	if(G2B_EXD>0){
		if(G2B_EXD>=G2B_HPD){ G2B_KD=2.0*G2B_HPD*G2B_EXD/(G2B_HPD+G2B_EXD); }
	}
	RETURNV
}

void SGBonus(_Hero_ *p,int Ad)
{
	int i,j,chn,bon,art,arti;

	STARTNA(__LINE__, 0)
	bon=0;
	if(p->Owner==-1) RETURNV // игрок проиграл
	for(i=0;i<7;i++){
		if(p->Ct[i]==173){
			if(Ad) chn=G2B_KD*100/G2B_KA;
			else   chn=G2B_KA*100/G2B_KD;
			for(j=0;j<6;j++){
				if(chn>=SGBonusChance[j][0]) bon=SGBonusChance[j][1];
			}
			break;
		}
	}
	if(bon>0){
		if(bon>=Random(1,100)){
			// 3.58
			int AMax=62; if(PL_NoNPC) AMax=52; // No Commander artifact if they are disabled
			arti=Random(0,AMax); art=SGArts[arti];
			if(ArtDisabled(art)){
				for(i=arti+1;i<=AMax;i++){
					art=SGArts[i];
					if(ArtDisabled(art)==0) goto l_found;
				}
				for(i=0;i<arti;i++){
					art=SGArts[i];
					if(ArtDisabled(art)==0) goto l_found;
				}
				RETURNV // нет ни одного разрешенного
			}
l_found:
			for(i=0;i<64;i++){
				if(p->OArt[i][0]==-1){
					p->OArt[i][0]=art;
					p->OArt[i][1]=-1;
					p->OANum=(Byte)(p->OANum+1);
					if(CurrentUser()==p->Owner){
						Request2Pic(ITxt(195,0,&Strings),21,173,8,art,1);
					}
					break;
				}
			}
		}
	}
	RETURNV
}

void G2B_After(void)
{
	int gt;
	// восстановим старые значения атаки
	STARTNA(__LINE__, 0)
	if(G2B_HrA!=0){ // атакующий
//    for(i=0;i<4;i++){
//      G2B_HrA->PSkill[i]-=(char)G2B_PSkillAddA[i];
//    }
		gt=DoesHeroGot(G2B_HrA);
		if(gt==0) // потерял Godа
			RemGod(G2B_HrA->Number);
// SG
		SGBonus(G2B_HrA,1);
	}
	if(G2B_HrD!=0){ // защищающийся
//    for(i=0;i<4;i++){
//      G2B_HrD->PSkill[i]-=(char)G2B_PSkillAddD[i];
//    }
		gt=DoesHeroGot(G2B_HrD);
		if(gt==0) // потерял Godа
			RemGod(G2B_HrD->Number);
// SG
		SGBonus(G2B_HrD,0);
	}
	_CastleSetup_ *cstp; /*=(_CastleSetup_ *)*(Dword *)&bm[0x53C8];*/
	__asm{ 
		mov eax,0x699420; 
		mov eax,[eax]; 
		add eax,0x53C8; 
		mov eax,[eax]; 
		mov cstp,eax; 
	}
	if(IsCastleSetup(cstp)==1){ // замок
		if(PL_TowerStd==0) NextTowerState(cstp); // стандартное поведение башен
	}
	if(BADistFlag){
		BACall(51,G2B_HrA);
	}else{
		BACall(1,G2B_HrA);
	}
	BACall(53,G2B_HrA);
	CrExpoSet::StopAllBF();
	RETURNV
}

void CheckForCompleteAI(void)
{
	STARTNA(__LINE__, 0)
	G2B_CompleteAI=1;
	_Hero_ *hp;
	hp=G2B_HrD;
	while(hp!=0){
		if(hp->Owner<0) break;
		if(hp->Owner>7) break;
		if(IsAI(hp->Owner)==0) G2B_CompleteAI=0; // human
		break;
	}
	hp=G2B_HrA;
	while(hp!=0){
		if(hp->Owner<0) break;
		if(hp->Owner>7) break;
		if(IsAI(hp->Owner)==0) G2B_CompleteAI=0; // human
		break;
	}
	if(IsAI(G2B_OwnerD)==0){ // хозяин защищающихся монстров - человек
		G2B_CompleteAI=0; // human
	}
	__asm{
		mov    eax,0x691259
		mov    al,[eax]
		test   al,al
		je     l_No
		mov    eax,0x691258
		mov    al,[eax]
		test   al,al
		je     l_No
	}
	G2B_CompleteAI=0;
l_No:;
	RETURNV
}

int __stdcall Get2Battle(Dword MixedPos,_Hero_ *HrA,_MonArr_ *MArrA,
												 int OwnerD,Dword Pv4/*0*/,_Hero_ *HrD,_MonArr_ *MArrD,
												 int Pv3/*-1*/,Dword Pv2/*1*/,Dword Pv1/*0*/) // ECX=Adv.Manager
{
	_ECX(G2B_AdvMan);
	G2B_MArrD=MArrD; G2B_MArrA=MArrA;
	G2B_HrD=HrD; G2B_HrA=HrA;
	G2B_OwnerD=OwnerD;
	G2B_MixedPos=MixedPos;
	__asm  pusha
	ExpoFromBattle=1;
	CheckForCompleteAI();
	G2B_Prepare();
	__asm  popa
	if(G2B_EnableBattle==0){
		G2B_HrD=0; G2B_HrA=0;
		BADistFlag=0;
		ExpoFromBattle=0;
		return 0;
	}
	STARTNA(__LINE__, 0)
	__asm{  
		push   Pv1
		push   Pv2
		push   Pv3
		push   G2B_MArrD
		push   G2B_HrD
		push   Pv4
		push   G2B_OwnerD
		push   G2B_MArrA
		push   G2B_HrA
		push   G2B_MixedPos
		mov    ecx,G2B_AdvMan
		mov    eax,0x4AD160
		call   eax
	}
	STOP
	__asm  pusha
	G2B_After();
	G2B_HrD=0; G2B_HrA=0;
	G2B_CompleteAI=1; // 3.58 to disable AI thinking (battle simulating)
	BADistFlag=0;
	ExpoFromBattle=0;
	__asm  popa
	__asm  mov   IDummy,eax
	return IDummy;
}

//////////////////////////
Byte  *M2B_BatMan;
Byte  *M2B_Monster;
char  *M2B_Movement; // возможность встать на клетку

Byte *M2B_GetMonStr(Byte *BatMan)
{
	STARTNA(__LINE__, 0)
	__asm{
		mov    esi,BatMan
		mov    ecx,[esi+0x132B8]
		mov    ebx,[esi+0x132BC]
		lea    eax,[8*ecx]
		sub    eax,ecx
		lea    eax,[eax+2*eax]
		add    eax,ebx
		lea    ecx,[8*eax]
		sub    ecx,eax
		lea    ecx,[ecx+2*ecx]
		lea    edx,[eax+8*ecx]
		mov    ecx,esi
		lea    eax,[esi+8*edx+0x54CC]
		mov    BPDummy,eax
	}
	STOP
	return BPDummy;
}

int  M2B_GetMonNum(Byte *BatMan)
{
	STARTNA(__LINE__, 0)
	int n;
	n=(*(int *)(&BatMan[0x132B8]))*21+*(int *)(&BatMan[0x132BC]);
	RETURN(n)
}

char M2B_IsAutoMonstr(Byte *BatMan)
{
	STARTNA(__LINE__, 0)
	__asm{
		mov   ecx,BatMan
		mov   eax,0x4744D0
		call  eax
		mov   CDummy,al
	}
	STOP
	return CDummy;
/*
	int i;
	i=*((int *)&MonStr[0xF4]);     // номер игрока из двух на поле боя
	i=((int *)&AdvMan[0x54A8])[i]; // хозяин по цветам 0=никто
	if(i==-1) return 1;
	i=*((int *)&AdvMan[0x132C4]);  // 1=auto, 0=human
	if(i==0){

	}else{
	}
*/
}
int M2B_BFPos[11][15+15]={
	{0,0x01,0,0x02,0,0x03,0,0x04,0,0x05,0,0x06,0,0x07,0,0x08,0,0x09,0,0x0A,0,0x0B,0,0x0C,0,0x0D,0,0x0E,0,0x0F},
	{0x12,0,0x13,0,0x14,0,0x15,0,0x16,0,0x17,0,0x18,0,0x19,0,0x1A,0,0x1B,0,0x1C,0,0x1D,0,0x1E,0,0x1F,0,0x20,0},
	{0,0x23,0,0x24,0,0x25,0,0x26,0,0x27,0,0x28,0,0x29,0,0x2A,0,0x2B,0,0x2C,0,0x2D,0,0x2E,0,0x2F,0,0x30,0,0x31},
	{0x34,0,0x35,0,0x36,0,0x37,0,0x38,0,0x39,0,0x3A,0,0x3B,0,0x3C,0,0x3D,0,0x3E,0,0x3F,0,0x40,0,0x41,0,0x42,0},
	{0,0x45,0,0x46,0,0x47,0,0x48,0,0x49,0,0x4A,0,0x4B,0,0x4C,0,0x4D,0,0x4E,0,0x4F,0,0x50,0,0x51,0,0x52,0,0x53},
	{0x56,0,0x57,0,0x58,0,0x59,0,0x5A,0,0x5B,0,0x5C,0,0x5D,0,0x5E,0,0x5F,0,0x60,0,0x61,0,0x62,0,0x63,0,0x64,0},
	{0,0x67,0,0x68,0,0x69,0,0x6A,0,0x6B,0,0x6C,0,0x6D,0,0x6E,0,0x6F,0,0x70,0,0x71,0,0x72,0,0x73,0,0x74,0,0x75},
	{0x78,0,0x79,0,0x7A,0,0x7B,0,0x7C,0,0x7D,0,0x7E,0,0x7F,0,0x80,0,0x81,0,0x82,0,0x83,0,0x84,0,0x85,0,0x86,0},
	{0,0x89,0,0x8A,0,0x8B,0,0x8C,0,0x8D,0,0x8E,0,0x8F,0,0x90,0,0x91,0,0x92,0,0x93,0,0x94,0,0x95,0,0x96,0,0x97},
	{0x9A,0,0x9B,0,0x9C,0,0x9D,0,0x9E,0,0x9F,0,0xA0,0,0xA1,0,0xA2,0,0xA3,0,0xA4,0,0xA5,0,0xA6,0,0xA7,0,0xA8,0},
	{0,0xAB,0,0xAC,0,0xAD,0,0xAE,0,0xAF,0,0xB0,0,0xB1,0,0xB2,0,0xB3,0,0xB4,0,0xB5,0,0xB6,0,0xB7,0,0xB8,0,0xB9}
};

int M2B_FindPos(int Code,int *x,int *y)
{
	int i,j;

	STARTNA(__LINE__, 0)
	for(i=0;i<30;i++)
		for(j=0;j<11;j++)
			if(M2B_BFPos[j][i]==Code){ *x=i; *y=j; RETURN(0) }
	RETURN(-1)
}

int M2B_GetPosCode(int x,int y)
{
	STARTNA(__LINE__, 0)
	int ret;
	if(x<0) x=0; if(x>29) x=29;
	if(y<0) y=0; if(y>10) y=10;
	ret=M2B_BFPos[y][x];
	if(ret!=0) RETURN(ret)
	if(x==0){ x=1; RETURN(M2B_BFPos[y][x]) }
	if(x==29){ x=28; RETURN(M2B_BFPos[y][x]) }
	if(y==0){ if(x<15) x+=1; else x-=1; RETURN(M2B_BFPos[y][x]) }
	if(y==10){ if(x<15) x+=1; else x-=1; RETURN(M2B_BFPos[y][x]) }
	RETURN(0)
}

int M2B_CheckPosCode(int x,int y)
{
	STARTNA(__LINE__, 0)
	if(x<0)  RETURN(-1)
	if(x>29) RETURN(-1)
	if(y<0)  RETURN(-1)
	if(y>10) RETURN(-1)
	RETURN(0)
}
//int M2B_TowerAgain=0;

Byte *M2B_GetMonAtCode(int PosCode){
	STARTNA(__LINE__, 0)
	if(PosCode<0) RETURN(0)
	if(PosCode>=0xBB) RETURN(0)
	__asm{
		mov   ebx,PosCode
		mov   ecx,0x699420
		mov   ecx,[ecx]
		lea   eax,[8*ebx]
		sub   eax,ebx
		shl   eax,4
		lea   ecx,[eax+ecx+0x1C4]
		mov   eax,0x4E7230
		call  eax
		mov   BPDummy,eax
	}
	RETURN(BPDummy)
}
////////////

int SorcSpells[4][30]={
 {52,25,47,50,73,75,45,100},
 {54,20,42,40,78,60,52,70,47,80,73,90,45,100},
 {71,15,50,30,80,45,61,60,54,68,42,76,78,84,52,88,47,92,73,96,45,100},
 {71,11,50,22,80,33,61,44,54,52,42,60,78,68,52,72,47,76,73,80,45,84,60,88,62,92,59,96,75,100}
};
/*
#define SS_UNDEAD -0x00040000
#define SS_NOFIRE -0x00004000
#define SS_NOMIND -0x00000400
#define SS_NOMORL -0x00020000

struct _SrcSpellImmune{
	short  SpNum;
	long   Mons[];
} SSpellImmun[]={
// lvl1
 {SS_MISFORTUNE,{SS_NOFIRE,82,83,155,26,27,151,132,,0}},
 {SS_DISRRAY,{82,83,155,26,27,151,132,,0}},
 {SS_DISEASE,{SS_UNDEAD,,30,31,82,83,155,26,27,151,132,0}},
 {SS_WEAKNESS,{82,83,155,26,27,151,132,SS_NOMORL,0}},
// lvl2
 {SS_SLOW,{82,83,155,26,27,151,132,SS_NOMORL,0}},
 {SS_CURSE,{SS_UNDEAD,SS_NOMORL,SS_NOFIRE,82,83,155,26,27,151,132,0}},
// {SS_DISPELL,{SS_UNDEAD,SS_NOMORL,SS_NOFIRE,82,83,155,26,27,151,132,0}},
// lvl3
 {SS_POISON,{SS_UNDEAD,SS_NOMORL,30,31,82,83,155,26,27,151,132,0}},
 {SS_SORROW,{SS_UNDEAD,SS_NOMORL,SS_NOFIRE,82,83,155,26,27,151,132,0}},
 {SS_ACIDBREATH,{SS_UNDEAD,SS_NOMORL,SS_NOFIRE,82,83,155,26,27,151,132,0}},
 {SS_FORGETFUL,{SS_UNDEAD,SS_NOMORL,SS_NOFIRE,82,83,155,26,27,151,132,0}},
// lvl4
 {SS_HYPNOTIZE,{SS_UNDEAD,SS_NOMORL,SS_NOFIRE,82,83,155,26,27,151,132,0}},
 {SS_BLIND,{SS_UNDEAD,SS_NOMORL,SS_NOFIRE,82,83,155,26,27,151,132,0}},
 {SS_BERSERK,{SS_UNDEAD,SS_NOMORL,SS_NOFIRE,82,83,155,26,27,151,132,0}},
 {SS_AGE,{SS_UNDEAD,SS_NOMORL,SS_NOFIRE,82,83,155,26,27,151,132,0}},

 {0,{0}}
};
int SorceCastSpell(int SNum,Byte *TMon){
	if(SNum>=50)     { p=&SorcSpells[3][0]; }
	else if(SNum>=20){ p=&SorcSpells[2][0]; }
	else if(SNum>=10){ p=&SorcSpells[1][0]; }
	else             { p=&SorcSpells[0][0]; }
}
*/
#define SS_CURSE      42
#define SS_WEAKNESS   45
#define SS_DISRRAY    47
#define SS_SORROW     50
#define SS_MISFORTUNE 52
#define SS_SLOW       54
#define SS_BERSERK    59
#define SS_HYPNOTIZE  60
#define SS_FORGETFUL  61
#define SS_BLIND      62
#define SS_POISON     71
#define SS_DISEASE    73
#define SS_AGE        75
#define SS_DISPELL    78
#define SS_ACIDBREATH 80

#define SS_UNDEAD 0x00040000
#define SS_NOFIRE 0x00004000
#define SS_NOMIND 0x00000400
#define SS_NOMORL 0x00020000
#define SS_SHOOTR 0x00000004
int CS_Spells[50][2];
int IsElemental(int tp){
	// ALL Elementals
 while(1){
	if(tp==112)  break; // Air Elemental
	if(tp==127)  break; // Stopm Elemental
	if(tp==115)  break; // Water Elemental
	if(tp==123)  break; // Ice Elemental
	if(tp==114)  break; // Fire Elemental
	if(tp==129)  break; // Energy Elemental
	if(tp==113)  break; // Earth Elemental
	if(tp==125)  break; // Magma Elemental
	if(tp==120)  break; // Psychic Elemental
	if(tp==121)  break; // Magic Elemental
	if(tp==182)  break; // Astral Spirit
	if(tp==191)  break; // Astral Spirit
	return 0;
 }
 return 1;
}
int IsDragon(int tp)
{
	while(1){
		if(tp==26)   break; // Green Dragon
		if(tp==27)   break; // Gold Dragon
		if(tp==151)  break; // Diamond Dragon
		if(tp==82)   break; // Red Dragon
		if(tp==83)   break; // Black Dragon
		if(tp==155)  break; // Darkness Dragon
		if(tp==132)  break; // Azure Dragon
		return 0;
	}
	return 1;
}
int IsGolem(int tp)
{
	while(1){
		if(tp==32)   break; // Stone Golem
		if(tp==33)   break; // Iron Golem
		if(tp==116)  break; // Gold Golem
		if(tp==117)  break; // Diamond Golem
		return 0;
	}
	return 1;
}
int HasUsefulSpells(Byte *Mp)
{ // есть полезные спелы
	int val=0,*sp;
	sp=(int *)&Mp[0x198];
	val+=sp[53]; // Haste
	val+=sp[51]; // Fortune
	val+=sp[44]; // Precision
	val+=sp[30]; // Protection from Air
	val+=sp[28]; // Air Shield
	val+=sp[58]; // Counterstrike
	val+=sp[36]; // Magic Mirror
	val+=sp[27]; // Shield
	val+=sp[46]; // Stone Skin
	val+=sp[34]; // AntiMagic
	val+=sp[33]; // Protection from Earth
	val+=sp[43]; // Bloodlust
	val+=sp[31]; // Protection from Fire
	val+=sp[29]; // Fire Shield
	val+=sp[56]; // Frenzy
	val+=sp[55]; // Slayer
	val+=sp[41]; // Bless
	val+=sp[32]; // Protection from Water
	val+=sp[49]; // Mirth
	val+=sp[48]; // Prayer
	return val;
}

int SCastSpell(int SNum,Byte *Mp,_Hero_ *hpa,_Hero_ *hpd,int Fixed,int NoAnim)
{
	STARTNA(__LINE__, 0)
	int   i,en;
	Byte *bm;
	int NoLvl3=0,NoAll=0,YesAll=0;
	int   tp=*((int *)&Mp[0x34]); // тип монстра
	Dword fl=*((Dword *)&Mp[0x84]); // флаги монстра
// общие ограничения
	//Recanter's cloak
	if(HeroHasArt(hpa,83)) NoLvl3=1;
	if(HeroHasArt(hpd,83)) NoLvl3=1;
	//Orb of Inhibition
	if(HeroHasArt(hpa,126)) NoAll=1;
	if(HeroHasArt(hpd,126)) NoAll=1;
	//Cursed Grounds
	__asm{
		mov    eax,0x699420
		mov    eax,[eax]
		mov    bm,eax
	}
	if(*((int *)&bm[0x53C0])==2) NoAll=1;
	//Orb of Vulnerability
	if(HeroHasArt(hpa,93)) YesAll=1;
	if(HeroHasArt(hpd,93)) YesAll=1;
// if no spell may be cast - just leave (no animation)
	if(NoAll) RETURN(-1)

// иммунитет к любым заклам
	// All God's Representatives and Messangers
	if((tp>=160)&&(tp<=167)){
		if(NoAnim==0){
			__asm{ // анимация сопротивления
				mov    eax,Mp
				push   0
				push   100
				push   eax // ->monster
				push   0x4E
				mov    ecx,0x699420  // ->combat manager
				mov    ecx,[ecx]
				mov    eax,0x4963C0
				call   eax
			}
		}
		RETURN(-1)
	}
	// Magic Mirror
	if(*(int *)&Mp[0x228]){ // есть
		i=0;
		switch(*(int *)&Mp[0x36C]){ // сила
			case 0: i=20; break;
			case 1: i=20; break;
			case 2: i=30; break;
			case 3: i=40; break;
		}
		if(Random(1,50)<=i){ // надо отразить на случ стэк атакера, но пока просто блокируем ы двойной вероятностью
			if(NoAnim==0){
				__asm{ // анимация сопротивления
					mov    eax,Mp
					push   0
					push   100
					push   eax // ->monster
					push   3 //0x4E
					mov    ecx,0x699420  // ->combat manager
					mov    ecx,[ecx]
					mov    eax,0x4963C0
					call   eax
				}
			}
			RETURN(-1)
		}
	}

	if(Fixed==-1){ // Sors
// типы заклинаний по числу С
		if(SNum>=50){
			CS_Spells[0][0]=SS_HYPNOTIZE;   CS_Spells[0][1]=4;
			CS_Spells[1][0]=SS_BLIND;       CS_Spells[1][1]=4;
			CS_Spells[2][0]=SS_BERSERK;     CS_Spells[2][1]=4;
			CS_Spells[3][0]=SS_AGE;         CS_Spells[3][1]=4;
			CS_Spells[4][0]=SS_POISON;      CS_Spells[4][1]=11;
			CS_Spells[5][0]=SS_SORROW;      CS_Spells[5][1]=11;
			CS_Spells[6][0]=SS_ACIDBREATH;  CS_Spells[6][1]=11;
			CS_Spells[7][0]=SS_FORGETFUL;   CS_Spells[7][1]=11;
			CS_Spells[8][0]=SS_SLOW;        CS_Spells[8][1]=8;
			CS_Spells[9][0]=SS_CURSE;       CS_Spells[9][1]=8;
			CS_Spells[10][0]=SS_DISPELL;    CS_Spells[10][1]=8;
			CS_Spells[11][0]=SS_MISFORTUNE; CS_Spells[11][1]=4;
			CS_Spells[12][0]=SS_DISRRAY;    CS_Spells[12][1]=4;
			CS_Spells[13][0]=SS_DISEASE;    CS_Spells[13][1]=4;
			CS_Spells[14][0]=SS_WEAKNESS;   CS_Spells[14][1]=4;
			CS_Spells[15][0]=-1;
		}else if(SNum>=20){
			CS_Spells[0][0]=SS_POISON;     CS_Spells[0][1]=15;
			CS_Spells[1][0]=SS_SORROW;     CS_Spells[1][1]=15;
			CS_Spells[2][0]=SS_ACIDBREATH; CS_Spells[2][1]=15;
			CS_Spells[3][0]=SS_FORGETFUL;  CS_Spells[3][1]=15;
			CS_Spells[4][0]=SS_SLOW;       CS_Spells[4][1]=8;
			CS_Spells[5][0]=SS_CURSE;      CS_Spells[5][1]=8;
			CS_Spells[6][0]=SS_DISPELL;    CS_Spells[6][1]=8;
			CS_Spells[7][0]=SS_MISFORTUNE; CS_Spells[7][1]=4;
			CS_Spells[8][0]=SS_DISRRAY;    CS_Spells[8][1]=4;
			CS_Spells[9][0]=SS_DISEASE;    CS_Spells[9][1]=4;
			CS_Spells[10][0]=SS_WEAKNESS;  CS_Spells[10][1]=4;
			CS_Spells[11][0]=-1;
		}else if(SNum>=10){
			CS_Spells[0][0]=SS_SLOW;       CS_Spells[0][1]=20;
			CS_Spells[1][0]=SS_CURSE;      CS_Spells[1][1]=20;
			CS_Spells[2][0]=SS_DISPELL;    CS_Spells[2][1]=20;
			CS_Spells[3][0]=SS_MISFORTUNE; CS_Spells[3][1]=10;
			CS_Spells[4][0]=SS_DISRRAY;    CS_Spells[4][1]=10;
			CS_Spells[5][0]=SS_DISEASE;    CS_Spells[5][1]=10;
			CS_Spells[6][0]=SS_WEAKNESS;   CS_Spells[6][1]=10;
			CS_Spells[7][0]=-1;
		}else{
			CS_Spells[0][0]=SS_MISFORTUNE; CS_Spells[0][1]=25;
			CS_Spells[1][0]=SS_DISRRAY;    CS_Spells[1][1]=25;
			CS_Spells[2][0]=SS_DISEASE;    CS_Spells[2][1]=25;
			CS_Spells[3][0]=SS_WEAKNESS;   CS_Spells[3][1]=25;
			CS_Spells[4][0]=-1;
		}
	}else{
		CS_Spells[0][0]=Fixed;   CS_Spells[0][1]=100;
		CS_Spells[1][0]=-1;
	}
// иммунитет к mind spell
	if(HeroHasArt(hpd,49)){ //Badge of Courage
		fl|=SS_NOMIND; // immunity
	}
// иммунитет к отдельным из них
	for(i=0;i<50;i++){
		if(CS_Spells[i][0]==-1) break; // все прошли
		en=0; // default is desabling
		switch(CS_Spells[i][0]){
			case SS_MISFORTUNE :
				if(YesAll){en=1; break;} // no creature immunity
				if(IsDragon(tp))    break; // ALL Dragons
//        if(IsElemental(tp)) break; // ALL Elementals
				if(tp==121)  break; // Magic Elemental
				if(fl&SS_NOFIRE) break; // Fire ummunity
				en=1;
				break;
			case SS_DISRRAY    :
				if(YesAll){en=1; break;} // no creature immunity
				if(IsDragon(tp))    break; // ALL Dragons
//        if(IsElemental(tp)) break; // ALL Elementals
				if(tp==121)  break; // Magic Elemental
				en=1;
				break;
			case SS_DISEASE    :
				if(NoLvl3) break; // this spell cannot be cast
				if(YesAll){en=1; break;} // no creature immunity
				if(fl&SS_UNDEAD) break; // Undead
				if(IsGolem(tp))     break; // ALL Golems
				if(IsElemental(tp)) break; // ALL Elementals
				if(IsDragon(tp))    break; // ALL Dragons
				if(tp==172)  break; // Nightmare
				if(tp==30)   break; // Stone Gargoyle
				if(tp==31)   break; // Obsidian Gargoyle
				en=1;
				break;
			case SS_WEAKNESS   :
				if(YesAll){en=1; break;} // no creature immunity
				if(IsDragon(tp))    break; // ALL Dragons
				if(tp==121)  break; // Magic Elemental
				en=1;
				break;
			case SS_SLOW       :
				if(YesAll){en=1; break;} // no creature immunity
				if(IsDragon(tp))    break; // ALL Dragons
				if(tp==121)  break; // Magic Elemental
				en=1;
				break;
			case SS_CURSE      :
				if(YesAll){en=1; break;} // no creature immunity
				if(fl&SS_UNDEAD) break; // Undead
				if(IsDragon(tp))    break; // ALL Dragons
				if(tp==121)  break; // Magic Elemental
				if(fl&SS_NOFIRE) break; // Fire ummunity
				en=1;
				break;
			case SS_DISPELL    :
				if(HasUsefulSpells(Mp)==0) break; // нет полезных спелов
				if(YesAll){en=1; break;} // no creature immunity
				en=1;
				break;
			case SS_POISON     :
				if(NoLvl3) break; // this spell cannot be cast
				if(YesAll){en=1; break;} // no creature immunity
				if(IsDragon(tp))    break; // ALL Dragons
				if(fl&SS_UNDEAD)  break; // Undead
				if(IsElemental(tp)) break; // ALL Elementals
				if(IsGolem(tp))     break; // ALL Golems
				if(tp==172)  break; // Nightmare
				if(tp==30)   break; // Stone Gargoyle
				if(tp==31)   break; // Obsidian Gargoyle
				en=1;
				break;
			case SS_SORROW     :
				if(NoLvl3) break; // this spell cannot be cast
				if(fl&SS_NOMORL) break; // не подвержен низкой морали
				if(YesAll){en=1; break;} // no creature immunity
				if(fl&SS_UNDEAD) break; // Undead
				if(fl&SS_NOMIND) break; // No mind spell
				if(tp==83)   break; // Black Dragon
				if(tp==155)  break; // Darkness Dragon
				if(tp==27)   break; // Gold Dragon
				if(tp==151)  break; // Diamond Dragon
				en=1;
				break;
			case SS_ACIDBREATH :
				if(NoLvl3) break; // this spell cannot be cast
				if(YesAll){en=1; break;} // no creature immunity
				if(tp==121)  break; // Magic Elemental
				if(tp==83)   break; // Black Dragon
				if(tp==155)  break; // Darkness Dragon
				if(tp==151)  break; // Diamond Dragon
				en=1;
				break;
			case SS_FORGETFUL  :
				if((fl&SS_SHOOTR)==0) break; // non-shooting
				if(*((int *)&Mp[0xD8])<=0) break; // нет выстрелов
				if(NoLvl3) break; // this spell cannot be cast
				if(YesAll){en=1; break;} // no creature immunity
				if(fl&SS_NOMIND) break; // No mind spell
				if(fl&SS_UNDEAD) break; // Undead
				en=1;
				break;
			case SS_HYPNOTIZE  :
				if(NoLvl3) break; // this spell cannot be cast
				if(YesAll){en=1; break;} // no creature immunity
				if(fl&SS_UNDEAD) break; // Undead
				if(IsGolem(tp))     break; // ALL Golems
				if(IsElemental(tp)) break; // ALL Elementals
				if(fl&SS_NOMIND) break; // No mind spell
				if(IsDragon(tp))    break; // ALL Dragons
				en=1;
				break;
			case SS_BLIND      :
				if(YesAll){en=1; break;} // no creature immunity
				if(fl&SS_UNDEAD) break; // Undead
				if(IsGolem(tp))     break; // ALL Golems
				if(IsElemental(tp)) break; // ALL Elementals
				if(fl&SS_NOMIND) break; // No mind spell
				if(IsDragon(tp))    break; // ALL Dragons
				if(fl&SS_NOFIRE) break; // Fire ummunity
				if(tp==70)   break; //  Troglodyte
				if(tp==71)   break; //  Infernal Troglodyte
				en=1;
				break;
			case SS_BERSERK    :
				if(NoLvl3) break; // this spell cannot be cast
				if(YesAll){en=1; break;} // no creature immunity
				if(fl&SS_UNDEAD) break; // Undead
				if(IsGolem(tp))     break; // ALL Golems
				if(IsElemental(tp)) break; // ALL Elementals
				if(fl&SS_NOMIND) break; // No mind spell
				if(fl&SS_NOFIRE) break; // Fire ummunity
				if(tp==27)   break; // Gold Dragon
				if(tp==151)  break; // Diamond Dragon
				if(tp==83)   break; // Black Dragon
				if(tp==155)  break; // Darkness Dragon
				if(tp==132)  break; // Azure Dragon
				en=1;
				break;
			case SS_AGE        :
				if(NoLvl3) break; // this spell cannot be cast
				if(YesAll){en=1; break;} // no creature immunity
				if(fl&SS_UNDEAD) break; // Undead
				if(IsGolem(tp))     break; // ALL Golems
				if(IsElemental(tp)) break; // ALL Elementals
				if(tp==83)   break; // Black Dragon
				if(tp==155)  break; // Darkness Dragon
				if(tp==151)  break; // Diamond Dragon
				if(tp==30)   break; // Stone Gargoyle
				if(tp==31)   break; // Obsidian Gargoyle
				en=1;
				break;
		}
		if(en==0) CS_Spells[i][0]=-2; // запретим
	}
// нечуствительность к другим
	for(i=0;i<50;i++){
		if(CS_Spells[i][0]==-1) break; // все прошли
		if(CS_Spells[i][0]==-2) continue; // иммунитет
		if((*(int *)&Mp[CS_Spells[i][0]*4+0x198])!=0){ // длительность есть
			// видимо наложено
			CS_Spells[i][0]=-2;
		}
	}
// выбор из оставшихся
	int chance=0;
	for(i=0;i<50;i++){
		if(CS_Spells[i][0]==-1) break; // все прошли
		if(CS_Spells[i][0]==-2) continue; // иммунитет
		chance+=CS_Spells[i][1];
	}
	if(chance==0) RETURN(-1) // ни одного не нашли
	int spch=Random(1,chance);
	int spell=-1;
	for(i=0;i<50;i++){
		if(CS_Spells[i][0]==-1) break; // все прошли
		if(CS_Spells[i][0]==-2) continue; // иммунитет
		// подстрахуемся - запомним первое
		if(spell==-1) spell=CS_Spells[i][0];
		spch-=CS_Spells[i][1];
		if(spch<=0){
			spell=CS_Spells[i][0];
			break;
		}
	}
// сопротивляемость магии
	int resist=0;
	if(hpd!=0){
		switch(hpd->SSkill[26]){ // level of Resistance skill
			case 3: // Expert
				resist+=20; break;
			case 2: // Advanced
				resist+=10; break;
			case 1: // Basic
				resist+=5; break;
		}
	}
	if(hpd!=0){
		if(hpd->Number==20){ //Thorgrim
			resist+=5*hpd->ExpLevel*resist/100;
		}
	}
	if(HeroHasArt(hpd,57)){ //Garniture of Interference
		resist+=5;
	}
	if(HeroHasArt(hpd,58)){ //Surcoat of Counterpoise
		resist+=10;
	}
	if(HeroHasArt(hpd,59)){ //Boots of Polarity
		resist+=15;
	}
	do{
		if(spell==SS_POISON) break;
		if(spell==SS_ACIDBREATH) break;
//    if(spell==SS_FORGETFUL) break;
		if(spell==SS_DISPELL) break;
		if(spell==SS_AGE) break;
		if(YesAll) break; // отменяет резист существ
		switch(tp){
			case 17  : // Battle Dwarf
				resist+=40; break;
			case 16  : // Dwarf
			case 133 : // Crystal Dragon
				resist+=20; break;
			case 196 : // Dracolich
				resist+=20; break;
		}
		// apply experience system resistance
		int host;
		__asm{
			mov   eax,0x687FA8
			mov   eax,[eax]
			mov   edx,spell
			shl   edx,4
			add   edx,spell
			shl   edx,3
			add   eax,edx
			mov   eax,[eax]
			mov   host,eax
		}
		if(host<=0){ // foe
			resist=CrExpBon::DwarfResist(Mp,resist,spell);
		}else{ // friend
			resist=CrExpBon::DwarfResistFriendly(Mp,resist,spell);
		}
	}while(0);
	if(HeroHasArt(hpd,105)){ //Pendant of Free Will
		if(spell==SS_HYPNOTIZE){ // immune
			resist=200; // immunity
		}
	}
	if(HeroHasArt(hpd,101)){ //Pendant of Second Sight
		if(spell==SS_BLIND){ // immune
			resist=200; // immunity
		}
	}
	if(HeroHasArt(hpd,100)){ //Pendant of Dispassion
		if(spell==SS_BERSERK){ // immune
			resist=200; // immunity
		}
	}
	if(HeroHasArt(hpd,107)){ //Pendant of Total Recall
		if(spell==SS_FORGETFUL){ // immune
			resist=200; // immunity
		}
	}
	if(HeroHasArt(hpd,102)){ //Pendant of Holiness
		if(spell==SS_CURSE){ // immune
			resist=200; // immunity
		}
	}
	if(HeroHasArt(hpd,92)){ //Sphere of Permanence
		if(spell==SS_DISPELL){ // immune
			resist=200; // immunity
		}
	}
	if(HeroHasArt(hpd,134)){ //Power of the Dragon Father
		if(spell!=SS_AGE){ // NOT immune to AGING only
			resist=200; // immunity
		}
	}
	if(resist>0){
		if(Random(1,100)<=resist){ // сопротивление в действии
			if(NoAnim==0){
				__asm{ // анимация сопротивления
					mov    eax,Mp
					push   0
					push   100
					push   eax // ->monster
					push   0x4E
					mov    ecx,0x699420  // ->combat manager
					mov    ecx,[ecx]
					mov    eax,0x4963C0
					call   eax
				}
			}  
			RETURN(-1)
		}
	}
	RETURN(spell)
}
// анимация действия спелла на монстра - yf
//:005A234A 6A00           push   00000000
//:005A234C 6A64           push   00000064
//:005A234E 57             push   edi // ->monster
//:005A234F 6A4E           push   0000004E
//:005A2351 8BCB           mov    ecx,ebx  // ->combat manager
//:005A2353E86840EFFF     call   HEROES3.004963C0

static int SG_Shift[6][2]= {{2,0},{-2,0},{-1,1},{-1,-1},{1,1},{1,-1}};
//static int SG_Shift2[6][2]={{2,0},{-4,0},{-1,1},{-1,-1},{0,0},{0,0}};
//static int SG_Shift3[6][2]={{4,0},{-2,0},{1,1},{1,-1},{0,0},{0,0}};
void _Monster2Battle(void)
{
	int x,y,x1,y1;
	int *AType,MType,MPos,*MDest;
	STARTNA(__LINE__, 0)
	//if(WoG==0) RETURNV
	M2B_Movement=(char *)&M2B_BatMan[0x4C];
	M2B_Monster=M2B_GetMonStr(M2B_BatMan);
	AType=(int *)&M2B_BatMan[0x3C];  // тип атаки
	MType=*((int *)&M2B_Monster[0x34]); // тип монстра
	MPos=*((int *)&M2B_Monster[0x38]); // позиция монстра
	MDest=(int *)&M2B_BatMan[0x44]; // позиция назначения монстра
	_Hero_   *hpo=(_Hero_ *)*(Dword *)&M2B_BatMan[0x53CC];
	_Hero_   *hpe=(_Hero_ *)*(Dword *)&M2B_BatMan[0x53D0];
	_CastleSetup_ *cp;
	cp=(_CastleSetup_ *)*(Dword *)&M2B_BatMan[0x53C8];
//asm int 3
//  M2B_TowerAgain=0;
	if(IsCastleSetup(cp)==1){ // замок
		if(MType==0x95){        // башня
//      Dword *flpo=(Dword *)&M2B_Monster[0x84];
//      *flpo|=0x04000000;   // атаковал уже
			int side=*(int *)&M2B_Monster[0xF4]; // сторона
			_Hero_   *htmp;
//      _Hero_   *hpo=(_Hero_ *)*(Dword *)&M2B_BatMan[0x53CC];
//      _Hero_   *hpe=(_Hero_ *)*(Dword *)&M2B_BatMan[0x53D0];
			if(side!=0){ htmp=hpo; hpo=hpe; hpe=htmp; }
			if((hpo!=0)&&(HeroHasArt(hpo,0x8E))){ // свой
//        Dword *flpo=(Dword *)&M2B_Monster[0x84];
//        *(int *)&M2B_Monster[0x60]=2;
//        *flpo|=0x00008000;   // стр. дважды
			}else if((hpe!=0)&&(HeroHasArt(hpe,0x8E))){ // чужой
				*AType=3; // ничего не делает.
				if(M2B_IsAutoMonstr(M2B_BatMan)==0){ // НЕ автопилот
					if(FirstTowerAttack){
						Message(ITxt(192,0,&Strings),1);
						FirstTowerAttack=0;
					}
				}
			}
		}
	}
	if((MType<GODMONTSTRT)||(MType>=(GODMONTSTRT+GODMONTNUM))){  // не God
		if(MType==147){ // палатка первой помощи
			if(*AType!=0x0B){ // лечить палаткой
				*AType=3; // пусть стоит на месте.
			}
		}
	}else{
		if(M2B_IsAutoMonstr(M2B_BatMan)){ // автопилот
//    if(AType==6) // атака, значит кто-то близко
			*AType=3; // пусть пока стоит на месте.
		}else{
			if(*AType!=6) RETURNV
			// атака, значит уходим
			if(FirstGodAttack){
				Message(ITxt(59,0,&Strings),1);
				FirstGodAttack=0;
			}
			if(M2B_FindPos(MPos,&x,&y)) RETURNV
			if(M2B_FindPos(*MDest,&x1,&y1)) RETURNV
			if(y==y1){ // на одной линии
				if(x>x1){ x+=2; }
				else{ x-=2; }
			}else{
				if(y>y1){ y+=1; }
				else{ y-=1; }
				if(x>x1){ x+=1; }
				else{ x-=1; }
			}
			*MDest=M2B_GetPosCode(x,y);
			*AType=2;
		}
	}
	if(MType==193){ // Sorceress
		if(*AType==7){ // shoot
			do{
//asm int 3
				Byte *dmp=M2B_GetMonAtCode(*MDest);   if(dmp==0) break;
				int dside=*(int *)&dmp[0xF4]; // сторона
				_Hero_   *hdmp,*hdmp2;
				if(dside!=0){ hdmp=hpe; hdmp2=hpo; }
				else{ hdmp=hpo; hdmp2=hpe; }
				int crn=*((int *)&M2B_Monster[0x4C]); if(crn<=0) break;
				int prc=2*crn+20; // 20+2% per creature
				if(Random(1,100)>prc) break; // chance to cast
				int spell=SCastSpell(crn,dmp,hdmp2,hdmp,-1,0);
				if(spell!=-1){
					*(int *)&dmp[0xEC]=spell;
				}
			}while(0);
		}
	}
	if(MType==195){ // Hell Steed (fire horse)
		if(*AType==6){ // attack
			do{
				Byte *dmp=M2B_GetMonAtCode(*MDest);   if(dmp==0) break;
//        int dside=*(int *)&dmp[0xF4]; // сторона
//        _Hero_   *hdmp,*hdmp2;
//        if(dside!=0){ hdmp=hpe; hdmp2=hpo; }
//        else{ hdmp=hpo; hdmp2=hpe; }
/*
				if(*(Dword *)&dmp[0x84]&0x4000) break; // immune for fire
				int tp=*(int *)&dmp[0x34];
				// Magic Miggor
				if((tp==169)||(tp==134)) break; // War Zealot and Fairy Dragon
				// Dragons
				if((tp==82)  // red dragon
				 ||(tp==83)  // black dragon
				 ||(tp==155) // darkness dragon
				 ||(tp==26)  // green dragon
				 ||(tp==27)  // gold dragon
				 ||(tp==151) // diamond dragon
				 ||(tp==132))// azure dragon
					 break;
*/
//        int spell=SCastSpell(13,dmp,hdmp2,hdmp,-1,0);
//        if(spell!=-1){
//          *(int *)&dmp[0xEC]=spell;
//        }
//        if(Random(1,100)>50) break;
				*(int *)&dmp[0xEC]=13;
			}while(0);
		}
	}
//asm int 3
//  if(*AType==7 || *AType==6){ // shoot or attack
//    do{
//      Byte *dmp=M2B_GetMonAtCode(*MDest);  if(dmp==0) break;
//      WereWolf(M2B_Monster,dmp);
//    }while(0);
//  }
	if(*AType==7 || *AType==6){ // shoot or attack
		do{
			Byte *dmp=M2B_GetMonAtCode(*MDest);   if(dmp==0) break;
			int dside=*(int *)&dmp[0xF4]; // сторона
			_Hero_   *hdmp,*hdmp2;
			if(dside!=0){ hdmp=hpe; hdmp2=hpo; }
			else{ hdmp=hpo; hdmp2=hpe; }
//        int crn=*((int *)&M2B_Monster[0x4C]); if(crn<=0) break;
//        int prc=2*crn+20; // 20+2% per creature
//        if(Random(1,100)>prc) break; // chance to cast
			int spell=CrExpBon::CastSpell(M2B_Monster,dmp,hdmp2,hdmp);
			if(spell!=-1){
				*(int *)&dmp[0xEC]=spell;
			}
			CrExpBon::CastMassSpell(M2B_Monster,dmp);
		}while(0);
	}
	BACall3(0,*(int *)&((Byte *)M2B_BatMan)[0x13D6C]);
	RETURNV
}

//static int ERM_Proceed(Byte *mon,char Cmd,Mes *Mp,char Ind);
static int Tmp_BtlStackVars[100][100];
int ERM_MAction(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp)
{	
	int v/*,v1,v2,v3,v4*/;
	_Hero_ *hp;
	Byte *bm;
	char *bh;
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	if(G2B_CompleteAI){
		MError("ERROR! Attempt to use \"!!BG\" in non-human battle (use flag 1000 for checking)."); RETURN(0)
	}
	bm = combatManager;
	switch(Cmd){
		case 'A': Apply((int *)&bm[0x3C],4,Mp,0); break; // тип атаки
		case 'S': Apply((int *)&bm[0x40],4,Mp,0); break; // номер закл
		case 'D': Apply((int *)&bm[0x44],4,Mp,0); break; // позиция куда атака
		case 'X': Apply((int *)&bm[0x48],4,Mp,0); break; // доп. позиция телепорта / дертвы
		case 'Q': // (проверка) атакующая сторона
			v=*(int *)&bm[0x132C0];
			Apply(&v,4,Mp,0);
			break;
		case 'H': // текущий герой-хозяин
			v=*(int *)&bm[0x132C0];
			hp=*(_Hero_ **)&bm[0x53CC+v*4];
			if(hp==0) v=-1; else v=hp->Number;
			Apply(&v,4,Mp,0);
			break;
		case 'E': // E?v номер монстра для атаки
			v=*(int *)&bm[0x44];
//      for(i=0;i<(21*2);i++){
//        mon=&bm[0x54CC+0x548*i];
//        if((*(int *)&mon[0x38])==v){
//          v=i; Apply(&v,4,Mp,0); return 1;
//        }
//      }
			bh=(char *)GetHexStr(v);
			if(bh==0){ v=-1; }
			else{
				if(bh[0x18]==-1){ // нет живого стека
					v=-1;
				}else{
					v=(int)bh[0x18]*21+(int)bh[0x19];
				}
			}
			Apply(&v,4,Mp,0);
			break;
		case 'N': // номер монстра (0...41)
			CHECK_ParamsNum(1);
			v=M2B_GetMonNum(bm);
			if(Apply(&v,4,Mp,0)) break;
			if(v < 0 || v > 41){ MError2("incorrect stack index."); RETURN(0) }
			CombatMan_SetCurrentMon(combatManager, (v > 20) ? 1 : 0, v % 21);
			break;
		case 'V':{ // 3.59 V$stack/$varind/$varval
			CHECK_ParamsMin(3);
			int ind=0,st=0;
			Apply(&st,4,Mp,0);  if(st <0 || st >99){ MError("\"!!BG:V\"-incorrect stack index."); RETURN(0) }
			Apply(&ind,4,Mp,1); if(ind<0 || ind>99){ MError("\"!!BG:V\"-incorrect value index."); RETURN(0) }
			Apply(&Tmp_BtlStackVars[st][ind],4,Mp,2);
			break;}
		case 'C':{ // 3.59 C or C$stack - clean up all or stack vars
			CHECK_ParamsNum(1);
			int st=0;
			Apply(&st,4,Mp,0);  if(st <-1 || st >99){ MError("\"!!BG:C\"-incorrect stack index."); RETURN(0) }
			if(st==-1){
				memset(Tmp_BtlStackVars,0,sizeof(Tmp_BtlStackVars));
			}else{
				memset(Tmp_BtlStackVars[st],0,sizeof(Tmp_BtlStackVars[0]));
			}
			break;}
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

int SG_GuardsSet=0;
struct _SG_GuardsSetUp_{
	int sg_stack;
	int own_stack;
	int own_type;
	int own_speed;
} SG_GuardsSetUp[42];
void MakeSGGuards(Byte *man, Byte *mon,int SG_stack)
{
	STARTNA(__LINE__, 0)
	_Hero_   *hpo=(_Hero_ *)*(Dword *)&man[0x53CC];
	_Hero_   *hpe=(_Hero_ *)*(Dword *)&man[0x53D0];
	int (*SG_shpo)[2];
//  if(*((int *)&[0x34])==172){        // Santa Gremlin
//   if((*(Dword *)&man[0x13D6C])==1){
//    if(man[0x13D68]==0){
			Byte *gmon;
			int i,j,n,type,gm,num,x,y,x1,y1;
			int Code=*(int *)&mon[0x38];
//*(int *)&mon[0x3C]=2;
			*(int *)&mon[0x4E0]=16; // frost ring
			num=*(int *)&mon[0x60];
			gm=*(int *)&mon[0xF4];
			_Hero_   *hp;
			if(gm) hp=hpe; else hp=hpo;
			n=6;
//      SG_Shift[2][0]=SG_Shift[3][0]=-1;
//      SG_Shift[1][0]=-2; SG_Shift[0][0]=2;
			SG_shpo=SG_Shift;
			if(hp!=0){
				type=(hp->Spec/2)*14;
				if(type==14) type=16; // Кентавры на Гномов
				if(type==112) type=118; // Элементали на Пикселей
				if((hp->Spec/2)==1){ // Рампарт
					num=num/2;
					if(num==0) num=1;
				}
			}else{
				type=28; // гремлин
			}
			CrExpBon::SGBonus(mon,&type,&num);
			if(M2B_FindPos(Code,&x,&y)==-1){ Error(); RETURNV }
			for(i=0;i<n;i++){
				if((SG_shpo[i][0]==0)&&(SG_shpo[i][1]==0)) continue;
				x1=x+SG_shpo[i][0];
				y1=y+SG_shpo[i][1];
				if(M2B_CheckPosCode(x1,y1)==0){
					if((Code=M2B_GetPosCode(x1,y1))!=0){
						gmon=SammonCreature(type,num,Code,gm,-1,0);
						if(gmon!=0){
							SG_GuardsSetUp[SG_GuardsSet].sg_stack=SG_stack;
							for(j=0;j<(21*2);j++){
								// 3.58 Tower skip
								if(j==20) continue; // 3.58
								if(j==41) continue; // 3.58
								if(&man[0x54CC+0x548*j]==gmon) break;
							}
							if(j==42) j=-1;
							SG_GuardsSetUp[SG_GuardsSet].own_stack=j;
							SG_GuardsSetUp[SG_GuardsSet].own_type=type;
							SG_GuardsSetUp[SG_GuardsSet].own_speed=*(int *)&gmon[0xC4];
							++SG_GuardsSet;
							*(int *)&gmon[0xC4]=-1;
						}
					}
				}
			}
	RETURNV
}

static int  NMT_First=0;
static Byte *NMT_man;
void _NoMoreTactic(void)
{
	int i,tp;
	Byte *mon;
//asm int 3
	STARTNA(__LINE__, 0)
	if(NMT_First) RETURNV
	if(NMT_man[0x13D68]==1) RETURNV // тактика
	NMT_First=1;
	SG_GuardsSet=0;
	for(i=0;i<(21*2);i++){
		// 3.58 Tower skip
		if(i==20) continue; // 3.58
		if(i==41) continue; // 3.58
		mon=&NMT_man[0x54CC+0x548*i];
		tp=*(int *)&mon[0x34];
		if(tp==173){        // Santa Gremlin
			MakeSGGuards(NMT_man,mon,i);
		}
		if((*(int *)&mon[0x4C])==0) continue;
//    BACall_Mon=mon;
//    BACall2(3,0);
//    BACall_Mon=0;
	}
	BACall_Day=1;
	BACall_Turn=0;
	BACall2(2,0);
	RETURNV
}

void __stdcall AppearWithoutTacktic(int,int,int)
{
	STARTNA(__LINE__, 0)
	__asm{
		mov    ecx,0x6992D0
		mov    ecx,[ecx]
		push   0
		push   4
		push   0
		mov    eax,0x603210
		call   eax
	}
	__asm{
		mov    eax,0x699420
		mov    eax,[eax]
		mov    eax,[eax+0x13D68]
		mov    IDummy,eax
	}
	if(IDummy==1) RETURNV // тактика
	__asm pusha
	BACall_Day=0;
	BACall_Turn=0;
	BACall2(2,0);
	__asm popa
	RETURNV
}
void NoMoreTactic(void)
{
	STARTNA(__LINE__, 0)
	__asm pusha
	_EBX(NMT_man);
	_NoMoreTactic();
	__asm popa
	__asm mov   eax,0x554450
	__asm call  eax
	STOP
}
void NoMoreTactic1(void)
{
	STARTNA(__LINE__, 0)
	__asm pusha
	_ECX(NMT_man);
	_NoMoreTactic();
	__asm popa
	__asm mov   eax,0x475800
	__asm call  eax
	STOP
}

void Check4Fairy(void)
{
	__asm{
		xor eax,eax
		cmp   dword ptr [edi+0x34],0x86
		je    lyes
		cmp   dword ptr [edi+0x34],173 // santa gremlin
		je    lyes
		cmp   dword ptr [edi+0x34],174 // NPC1
		jb    lyes            // не NPC - переход с НЕ РАВНО
		cmp   dword ptr [edi+0x34],191 // NPC1
		ja    lyes            // не NPC - переход с НЕ РАВНО
		cmp   eax,eax
		je    lyes            // NPC - переход с РАВНО
	}
lyes:;
}

static int FP_ret;
void FairyPower(void)
{
	__asm{
		mov   eax,[esi+0x4C] // количество
		cmp   dword ptr [esi+0x34],0x86
		jne   l_not
		lea   eax,[eax+4*eax]
		jmp   l_yes
	}
l_not:
	__asm{
		cmp   dword ptr [esi+0x34],173 // santa gremlin
		jne   l_not2
		shr   eax,6 // num/64
		inc   eax
	}
l_not2:
	__asm{
		cmp   dword ptr [esi+0x34],174 // NPC
		jb    l_not3
		cmp   dword ptr [esi+0x34],191 // NPC
		ja    l_not3
		mov   FP_ret,esi
		pusha
	}
	GetNPCMagicPower((Byte *)FP_ret);
	__asm{
		mov   FP_ret,eax
		popa
		mov   eax,FP_ret
	}
l_not3:
l_yes:;
}

static int MAB_ret;
void _MonsterAfterBattle(void)
{
	__asm pusha
//asm int 3
	_EAX(MAB_ret);
	STARTNA(__LINE__, 0)
	if((*(int *)&((Byte *)M2B_BatMan)[0x132F8])==1){
//    for(int i=0;i<(21*2);i++) *(Dword *)&M2B_BatMan[0x54CC+0x548*i+0x84]|=0x00200000;
		CheckForAliveNPCAfterBattle((Byte *)M2B_BatMan);
		MAB_ret=2;
	}
//  BACall3(1,*(int *)&((Byte *)M2B_BatMan)[0x13D6C]);
	BACall3(1,BACall_Day);
	STOP
	__asm popa
	__asm mov  eax,MAB_ret
}

void __stdcall Monster2Battle(Dword Pv2,Dword Pv1)
{
	_ECX(M2B_BatMan);
	_Monster2Battle();
	STARTNA(__LINE__, 0)
	__asm  push   Pv1
	__asm  push   Pv2
	__asm  mov    ecx,M2B_BatMan
	__asm  mov    eax,0x4786B0
	__asm  call   eax
	STOP
	_MonsterAfterBattle();
}

// показывает книгу с закл при навидении мыши
Dword SMBM_MonPo;
Dword SMBM_RetPo;
Byte  SMBM_MonNum;
int __stdcall ShowMagicBookMouse(Dword position)
{
	__asm  mov  SMBM_MonPo,ecx
	STARTNA(__LINE__, 0)
	SMBM_MonNum=((Byte *)SMBM_MonPo)[0x34];
// архангел
	if(((Byte *)SMBM_MonPo)[0x34]==150) ((Byte *)SMBM_MonPo)[0x34]=13;
	__asm{
		mov    eax,position   // инд. поз на поле
		mov    ecx,SMBM_MonPo // структ монстра
		push   eax
		mov    eax,0x4470F0
		call   eax
		mov    SMBM_RetPo,eax
	}
	((Byte *)SMBM_MonPo)[0x34]=SMBM_MonNum;
	STOP
//  __asm mov  eax,SMBM_RetPo
	return SMBM_RetPo;
}

// снижение защиты чудищем //Уменьшение вражеской защиты
Byte *RD_Def;
Byte *RD_Att;
Dword RD_Ret,RD_Ret2,RD_OrDef;
int __stdcall ReduceDefence(Dword Att,Dword One_or_Zero)
{
	__asm  mov  RD_Def,ecx
	RD_Att=(Byte *)Att;
	STARTNA(__LINE__, 0)
	__asm{
		mov    eax,RD_Att
		mov    ecx,RD_Def
		push   One_or_Zero
		push   eax
		mov    eax,0x4422B0
		call   eax
		mov    RD_Ret,eax
	}
// чудище - снимает 100% защиты
	if(RD_Att!=0){
		__asm pusha
		if((*(int *)&RD_Att[0x34]>=174)&&(*(int *)&RD_Att[0x34]<=191)){
			RD_Ret2=NPCReduceDefence(RD_Att,RD_Ret);/*RD_Ret*20/100*/
		}else{
			if(RD_Ret>0){
				RD_OrDef=RD_Ret;
				switch(*(int *)&RD_Att[0x34]){
					case 96: RD_OrDef+=*(int *)&RD_Def[0xCC]*0.4; break;
					case 97: RD_OrDef+=*(int *)&RD_Def[0xCC]*0.8; break;
//          case 156:
				}
				RD_Ret2=CrExpBon::ReduceDefence((Byte *)RD_Att,(Byte *)RD_Def,RD_OrDef);
				if(RD_Ret2==RD_OrDef){
					RD_Ret2=RD_Ret;
					if(*(int *)&RD_Att[0x34]==156){
						RD_Ret2=0/*RD_Ret*20/100*/;
					}
				}
			}
		}
		__asm popa
	}else{
		RD_Ret2=RD_Ret;
	}
	STOP
//  __asm mov  eax,RD_Ret2
	return RD_Ret2;
}

//  AI просчитывает магию монстра
Byte AIMagicJumper[MONNUM-13]={
/* 13*/ 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/* 20*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/* 30*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x03, 0x03,
/* 40*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/* 50*/ 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/* 60*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/* 70*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/* 80*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/* 90*/ 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/*100*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/*110*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/*120*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/*130*/ 0x03, 0x03, 0x03, 0x03, 0x02,
/*135*/ 0x03, 0x03, 0x03, 0x03, 0x03,
/*140*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/*150*/ 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/*160*/ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/*170*/ 0x03, 0x03, 0x03, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
/*180*/ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
/*190*/ 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
};

int AIJmpAdr [] = {0x421484, 0x42149F, 0x4214BA, 0x4214D5, (int)&AICastMagic};

// Магия Чародея
//E1 86 5A 00 Armaggedon
//0F 87 5A 00 Anti-Magic
//77 87 5A 00 Ressurection, Animation Dead
//33 88 5A 00 Sacrifice
//73 86 5A 00 Hypnotize
//2A 87 5A 00 Clone
//A2 88 5A 00 no
//26 = 00 06 06 06
//30 = 06 06 06 06 01 06 06 06 02 02
//40 = 03 06 06 06 06 06 06 06 06 06
//50 = 06 06 06 06 06 06 06 06 06 06
//60 = 04 06 06 06 06 05
// Магия Чародея
//A3 78 44 00 Shield, Fire Shield
//4E 79 44 00 Air Shield
//F3 77 44 00 Prot from Air, Fire, Water, Earth, Anti-Magic, Magic Mirror
//20 78 44 00 Cure
//D3 7A 44 00 Bloodlust
//F9 79 44 00 Precission
//2E 78 44 00 Prayer
//40 78 44 00 Slayer
//82 7B 44 00 no
//27 = 00 01 00
//30 = 02 02 02 02 02 08 02 03 08 08
//40 = 08 08 08 04 05 08 08 08 06 08
//50 = 08 08 08 08 08 07

// Магия Сказ Драк:
// 43c326 - переход по проверке действия магии на клетку, если там есть что-то

// Герой думает что колдовать:
// 43C551
//0043C551 33FF           xor    edi,edi = тип заклинания
//0043C553 33F6           xor    esi,esi = смещ типа в табл закл
//0043C555897DF0         mov    [ebp-10],edi = тип заклинания
//0043C558 8975F8         mov    [ebp-08],esi = смещ типа в табл закл
//0043C55B 8B4B04         mov    ecx,[ebx+04] ECX -> структ. Героя (+430 заклы)
//0043C55E 8A843930040000 mov    al,[ecx+edi+00000430] есть закл?
//0043C565 84C0           test   al,al есть закл?
//0043C567 0F847C010000   je     NextSpell  H3WOG.0043C6E9 нет
//0043C56D 8B15A87F6800   mov    edx,[00687FA8] -> табл. заклов
//0043C573 8B44160C       mov    eax,[esi+edx+0C] = аттриб. закла
//0043C577 A801           test   al,01 аттриб 0001
//0043C579 0F846A010000   je     NextSpell  H3WOG.0043C6E9 нет
//0043C57F 8A5508         mov    dl,[ebp+08] =0/1 ??? (0)
//0043C582 84D2           test   dl,dl
//0043C584 7409           je     Over1 H3WOG.0043C58F (0043C58F)
//0043C586 F6C402         test   ah,02 аттриб 0200
//0043C589 0F845A010000   je     NextSpell  H3WOG.0043C6E9 нет
//Over1:
//0043C58F A120946900     mov    eax,[00699420] -> CombatManager
//0043C594 8B80C0530000   mov    eax,[eax+000053C0] = модиф. битвы (маг земли и т.п.)
//0043C59A 83F802         cmp    eax,00000002 модиф 0002 ?
//0043C59D 7511           jne    Over2 H3WOG.0043C5B0 (0043C5B0)
//0043C59F 8B15A87F6800   mov    edx,[00687FA8] -> табл. заклов
//0043C5A5 837C161801     cmp    dword ptr [esi+edx+18],00001 = категор закла 0001 ?
//0043C5AA 0F8F39010000   jg     NextSpell  H3WOG.0043C6E9 нет
//Over2:
//0043C5B0 8A55FF         mov    dl,[ebp-01] есть ограничение по макс уровню спеллов?
//0043C5B3 84D2           test   dl,dl Recanter's Cloak
//0043C5B5 7411           je     AnyLevel H3WOG.0043C5C8 (0043C5C8) нет
//0043C5B7 8B15A87F6800   mov    edx,[00687FA8] табл. заклов
//0043C5BD 837C161802     cmp    dword ptr [esi+edx+18],00002 уровень закла > 2
//0043C5C2 0F8F21010000   jg     NextSpell  H3WOG.0043C6E9 >2 - нельзя
//AnyLevel:
//0043C5C8 50             push   eax модиф. поверхности
//0043C5C9 57             push   edi номер закла
//                ECX -> структ. героя
//0043C5CA E8218D0A00     call   H3WOG.004E52F0 ???
//   +54C4  dd -> монстры, если нет первого героя
//   +54C8  dd -> монстры, если нет второго героя
//0043C5CF 8B55EC         mov    edx,[ebp-14] -> выше на монстров оппонента
//0043C5D2 8945E8         mov    [ebp-18],eax что ф-я возвращает
//0043C5D5 A120946900     mov    eax,[00699420] -> CombatManager
//   +53C0  dd = модеф битвы (-1) 1=маг. земля
//0043C5DA 8B88C0530000   mov    ecx,[eax+000053C0]
//0043C5E0 51             push   ecx
//0043C5E1 8B4B04         mov    ecx,[ebx+04] -> структ. героя
//0043C5E4 52             push   edx -> на монстров оппонента
//0043C5E5 57             push   edi номер закла
//0043C5E6 E8C58E0A00     call   H3WOG.004E54B0 маны на закл
//0043C5EB 8BF0           mov    esi,eax маны на закл
//0043C5ED 8B4304         mov    eax,[ebx+04] -> структ. героя
//0043C5F0 0FBF4818       movsx  ecx,word ptr [eax+18] = мана
//0043C5F4 3BF1           cmp    esi,ecx хватает маны?
//0043C5F6 0F8FEA000000   jg     NextSpell  H3WOG.0043C6E6 нет
//0043C5FC 8A45FE         mov    al,[ebp-02] ??? (0)
//0043C5FF 84C0           test   al,al ???
//0043C601 7415           je     Over3   H3WOG.0043C618 (0043C618)
// разр только Ressurection или Animate Dead
//0043C603 8B45F8         mov    eax,[ebp-08] смещ на закл в табл
//0043C606 3D30140000     cmp    eax,00001430 Ressurection?
//0043C60B 740B           je     H3WOG.0043C618 (0043C618) да
//0043C60D 3DB8140000     cmp    eax,000014B8 Animate Dead
//0043C612 0F85CE000000   jne    H3WOG.0043C6E6 нет
//Over3:
//0043C618 8B55E8         mov    edx,[ebp-18] ранее устан функцией
// [ebp-20] = [eax+4*ecx+000053D4] eax->CombatManager, ecx=тек. сторона
//0043C61B 8B45E0         mov    eax,[ebp-20] = доп. длительность ??? (8)
//0043C61E 8B4DF4         mov    ecx,[ebp-0C] = доп. длительность ??? (8)
//0043C621 8955C0         mov    [ebp-40],edx
//0043C624 8945C4         mov    [ebp-3C],eax
//0043C627 8D55BC         lea    edx,[ebp-44]
//0043C62A 894DC8         mov    [ebp-38],ecx
//0043C62D 83C8FF         or     eax,FFFFFFFF
//0043C630 52             push   edx
// edx -> 9 dd
// +00 dd = номер закла
// +04 dd = 0 ???
// +08 dd = 8 длит. закла ???
// +0С dd = 8 длит. закла ???
// +10 db = 1/0
// +14 dd = позиция на поле (код)
// +18 dd = -1 ???
// +1C dd = вес закла ???
// +20 dd = 1/0
//0043C631 8BCB           mov    ecx,ebx
//0043C633 897DBC         mov    [ebp-44],edi
//0043C636 C645CC01       mov    byte ptr [ebp-34],01
//0043C63A C745D800000000 mov    dword ptr [ebp-28],00000000
//0043C641 8945D0         mov    [ebp-30],eax
//0043C644 8945D4         mov    [ebp-2C],eax
//0043C647 C645DC00       mov    byte ptr [ebp-24],00
//0043C64B E820F1FFFF     call   H3WOG.0043B770 ???
//0043C650 8B4DD8         mov    ecx,[ebp-28] = полный HP стека для атаки ???
//0043C653 85C9           test   ecx,ecx
//0043C655 0F8E8B000000   jle    NextSpell  H3WOG.0043C6E6 нет
//0043C65B 8B4304         mov    eax,[ebx+04] -> структ. героя
//0043C65E 8D14F500000000 lea    edx,[8*esi] esi = сколько маны на закл
//0043C665 2BD6           sub    edx,esi мана*7
//0043C667 0FBF4018       movsx  eax,word ptr [eax+18] = мана героя
//0043C66B 3BC2           cmp    eax,edx  маны меньше, чем надо*7 ?
//0043C66D 7C0A           jl     Over4  H3WOG.0043C679 (0043C679)
//0043C66F 8D0489         lea    eax,[ecx+4*ecx] HP*5
//0043C672 99             cdq ???
//0043C673 2BC2           sub    eax,edx ???
//0043C675 D1F8           sar    eax,1  делить на 2
//0043C677 EB2A           jmp    Over5  H3WOG.0043C6A3 (0043C6A3)
//Over4:
//0043C679 99             cdq ???
//0043C67A F7FE           idiv   esi мана героя/мана на закл
//0043C67C 8945E8         mov    [ebp-18],eax
//0043C67F DB45E8         fild   dword ptr[ebp-18]
//0043C682 DD5DE4         fstp   qword ptr[ebp-1C]
//0043C685 8B45E8         mov    eax,[ebp-18]
//0043C688 8B4DE4         mov    ecx,[ebp-1C]
//0043C68B 50             push   eax
//0043C68C 51             push   ecx
//0043C68D E8C2BA1D00     call   H3WOG.00618154
//0043C692 DB45D8         fild   dword ptr[ebp-28] полный HP стэка
//0043C695 83C408         add    esp,00000008
//0043C698 DD5DE4         fstp   qword ptr[ebp-1C]
//0043C69B DC4DE4         fmul   qword ptr[ebp-1C]
//0043C69E E8F1B81D00     call   H3WOG.00617F94 сохраняет верш стэка в eax
//Over5:
// eax хранит "весовой коэф" закла
//0043C6A3 BA64000000     mov    edx,00000064 = 100
//0043C6A8 B94B000000     mov    ecx,0000004B =  75
//0043C6AD 8945D8         mov    [ebp-28],eax
//0043C6B0 E80B010D00     call   H3WOG.0050C7C0 случ. число от 75 до 100
//0043C6B5 8BC8           mov    ecx,eax
//0043C6B7 B81F85EB51     mov    eax,51EB851F
//0043C6BC 0FAF4DD8       imul   ecx,[ebp-28]
//0043C6C0 F7E9           imul   ecx
//0043C6C2 C1FA05         sar    edx,05
//0043C6C5 8BC2           mov    eax,edx
//0043C6C7 C1E81F         shr    eax,1F
//0043C6C7 C1E81F         shr    eax,1F
//0043C6CA 03D0           add    edx,eax
//0043C6CC 8B45B4         mov    eax,[ebp-4C]
//0043C6CF 3BD0           cmp    edx,eax
// проверка на ранее расчитанный, больше ли вес этого (+1C)
//0043C6D1 8955D8         mov    [ebp-28],edx
//0043C6D4 7E10           jle    H3WOG.0043C6E6 (0043C6E6)
//0043C6D6 B909000000     mov    ecx,00000009
//0043C6DB 8D75BC         lea    esi,[ebp-44]
//0043C6DE 8D7D98         lea    edi,[ebp-68]
//0043C6E1 F3A5           rep movsd   пересылаем 9 дв. слов
//0043C6E3 8B7DF0         mov    edi,[ebp-10] = тип залинания
//0043C6E6 8B75F8         mov    esi,[ebp-08] = смещ типа в табл закл
//NextSpell:
//0043C6E9 81C688000000   add    esi,00000088 переходим к след типу заклинания
//0043C6EF 47             inc    edi переходим к след типу заклинания
//0043C6F0 81FE30250000   cmp    esi,00002530 все заклинания?
//0043C6F6 897DF0         mov    [ebp-10],edi = тип залинания
//0043C6F9 8975F8         mov    [ebp-08],esi = смещ типа в табл закл
//0043C6FC 0F8C59FEFFFF   jl     H3WOG.0043C55B все закл. рассмотрели

//0043C702 8B4598         mov    eax,[ebp-68]
//0043C705 5F             pop    edi
//0043C706 5E             pop    esi
//0043C707 83F8FF         cmp    eax,FFFFFFFF
//0043C70A 5B             pop    ebx  был хоть один закл ?
//0043C70B 7443           je     NoSpell  H3WOG.0043C750 (0043C750)
//0043C70D 8A4DB8         mov    cl,[ebp-48] (+20)
//0043C710 84C9           test   cl,cl
//0043C712 7507           jne    Found  H3WOG.0043C71B (0043C71B)
//0043C714 8A4D08         mov    cl,[ebp+08] ??? (0)
//0043C717 84C9           test   cl,cl
//0043C719 7435           je     NoSpell  H3WOG.0043C750 (0043C750)
//Found:
//0043C71B 8B0D20946900   mov    ecx,[00699420] -> CombatManager
//0043C721 C7413C01000000 mov    dword ptr [ecx+3C],00000001 тип атаки
//0043C728 8B1520946900   mov    edx,[00699420] -> CombatManager
//0043C72E 8B4DAC         mov    ecx,[ebp-54] позиц на поле для атаки (+14)
//0043C731 894240         mov    [edx+40],eax
//0043C734 A120946900     mov    eax,[00699420]
//0043C739 894844         mov    [eax+44],ecx
//0043C73C 8B1520946900   mov    edx,[00699420]
//0043C742 8B45B0         mov    eax,[ebp-50]
//0043C745 894248         mov    [edx+48],eax
//0043C748 B001           mov    al,01
//0043C74A 8BE5           mov    esp,ebp
//0043C74C 5D             pop    ebp
//0043C74D C20400         ret    0004
// NoSpell:
// нет закла
//0043C750 32C0           xor    al,al
//0043C752 8BE5           mov    esp,ebp
//0043C754 5D             pop    ebp
//0043C755 C20400         ret    0004

// вызов для определения акции героя
//0042304D 8B96C0320100   mov    edx,[esi+000132C0] = тек. сторона
//00423053 6A00           push   00000000
//00423055 52             push   edx
//00423056 56             push   esi
//00423057 8D8DE4FBFFFF   lea    ecx,[ebp-0000041C]
//0042305D E8AE350100     call   H3WOG.00436610
//00423062 8BCE           mov    ecx,esi
//00423064 C745FC00000000 mov    dword ptr [ebp-04],00000000
//0042306B E880B6FFFF     call   H3WOG.0041E6F0
//00423070 50             push   eax
//00423071 8D8DE4FBFFFF   lea    ecx,[ebp-0000041C]
//00423077E8D4930100     call   H3WOG.0043C450
//0042307C 84C0           test   al,al
//0042307E 7423           je     H3WOG.004230A3 (004230A3)
//00423080 8D8DE4FBFFFF   lea    ecx,[ebp-0000041C]
//00423086 C745FCFFFFFFFF mov    dword ptr [ebp-04],FFFFFFFF
//0042308D E8EE370100     call   H3WOG.00436880

// просчет заклинания для героя
//0043B783 8D47F2         lea    eax,[edi-0E]
//0043B786 83F837         cmp    eax,00000037
//0043B789 0F87F5010000   ja     H3WOG.0043B984
//0043B78F 33C9           xor    ecx,ecx
//0043B7918A8834BB4300   mov    cl,[eax+0043BB34]
//0043B797 FF248D0CBB4300 jmp    [4*ecx+0043BB0C]

// решает что колдовать герою, если возможно
//004781AD E86EADFAFF     call   H3WOG.00422F20

// переход по типу существа для показа книжки
Byte MoJumper1[MONNUM-13]={ // Diakon
/* 13*/ 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 20*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 30*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x01, 0x08, 0x08,
/* 40*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 50*/ 0x08, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 60*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 70*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 80*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 90*/ 0x08, 0x02, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*100*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*110*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*120*/ 0x08, 0x08, 0x08, 0x03, 0x08, 0x04, 0x08, 0x05, 0x08, 0x06,
/*130*/ 0x08, 0x08, 0x08, 0x08, 0x07,
/*135*/ 0x08, 0x08, 0x08, 0x08, 0x08,
/*140*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*150*/ 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*160*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*170*/ 0x08, 0x08, 0x08, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
/*180*/ 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
/*190*/ 0x07, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

int MoJmpAdr1[]={//Diakon
	0x447480, 0x447511, 0x4476A3, 0x4475F6, 0x44766E, 0x4475B7,
	0x447635, 0x44756E, 0x4476F2, (int)&MagicBookManager
};

Byte MagHint [MONNUM-13]={
	/*13*/0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	/*29*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x01, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	/*45*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x02, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	/*61*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	/*77*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x03, 0x09,
	/*93*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	/*109*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x09,
	/*125*/0x05, 0x09, 0x06, 0x09, 0x07, 0x09, 0x09, 0x09, 0x09, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	/*141*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	/*157*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	/*173*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	/*189*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09
};

int MagHintJmp[]={0x492A6E, 0x492AAB, 0x492AF6, 0x492CE5, 0x492C03, 0x492C91, 0x492BC9,
				  0x492C57, 0x492B31, 0x492E3B, (int)&ManageHintCast};

// переход по самой магии
Byte MoJumper2[MONNUM-13]={
/* 13*/ 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 20*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 30*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x01, 0x09, 0x09,
/* 40*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 50*/ 0x09, 0x02, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 60*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 70*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 80*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 90*/ 0x09, 0x03, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*100*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*110*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*120*/ 0x09, 0x09, 0x09, 0x04, 0x09, 0x05, 0x09, 0x06, 0x09, 0x07,
/*130*/ 0x09, 0x09, 0x09, 0x09, 0x08,
/*135*/ 0x09, 0x09, 0x09, 0x09, 0x09,
/*140*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*150*/ 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*160*/ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*170*/ 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*180*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*190*/ 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
}; // таблица переходов 448460

int MagJmp [] = {0x44826A, 0x448351, 0x448308, 0x4483C4, 0x448394, 
				 0x4483B4, 0x448384, 0x4483A4, 0x44835E, 0x4483DD, (int)&MagicCastManager};

// переход по спецатаке
Byte MoJumper3[]={
/* 63*/ 0x00, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
/* 70*/ 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
/* 80*/ 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
/* 90*/ 0x05, 0x05, 0x05, 0x01, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
/*100*/ 0x05, 0x05, 0x05, 0x02, 0x03, 0x03, 0x05, 0x05, 0x05, 0x05,
/*110*/ 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
/*120*/ 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
/*130*/ 0x05, 0x05, 0x05, 0x05, 0x05, 0x04,
/*136*/ 0x05, 0x05, 0x05, 0x05,
/*140*/ 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
/*150*/ 0x05, 0x05, 0x01, 0x05, 0x00, 0x05, 0x05, 0x04, 0x05, 0x05,
/*160*/ 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
/*170*/ 0x05, 0x05, 0x02, 0x05, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
/*180*/ 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
/*190*/ 0x02, 0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
};
// 440921  440EA5  440BD6  4410E4  4411BF  4412AB

// переход по спецмагии монстра
Byte MoJumper4[]={
/* 22*/0x00, 0x00, 0x01, 0x01, 0x09, 0x09, 0x09, 0x09,
/* 30*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 40*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 50*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x02,
/* 60*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x03, 0x03, 0x09, 0x04,
/* 70*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x05, 0x05, 0x09, 0x09,
/* 80*/0x09, 0x06, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/* 90*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*100*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x05, 0x05, 0x09, 0x07,
/*110*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*120*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*130*/0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x09, 0x09, 0x09, 0x09,
/*140*/0x09, 0x03,
/*142*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*150*/0x09, 0x01, 0x09, 0x05, 0x04, 0x09, 0x09, 0x08, 0x09, 0x09,
/*160*/0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
/*170*/0x09, 0x09, 0x09, 0x09, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
/*180*/0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
/*190*/0x06, 0x06, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
};
// кастинг спела после атаки/героем
//00468CCF 8B964CFFFFFF   mov    edx,[esi-000000B4] = позиция назначения
//00468CD5 8B4DF8         mov    ecx,[ebp-08] = combat manager
//00468CD8 6A03           push   00000003
//00468CDA 6A00           push   00000000
//00468CDC 6AFF           push   FFFFFFFF
//00468CDE 6A01           push   00000001
//00468CE0 52             push   edx
//00468CE1 50             push   eax = номер закла
//00468CE2E859741300     call   H3WOG.005A0140
//00468CE7 C706FFFFFFFF   mov    dword ptr [esi],FFFFFFFF сбросить закл


void GDMagic(void);
Dword MoJumper4po[]={
0x4403A1,0x44032D,0x4402B8,0x440422,0x440242,
0x440496,0x4405C0,0x44053A,0x44050A,0x4402AE,
};

void ManaAdd2Hero(int side)
{
	STARTNA(__LINE__, 0)
	__asm{
		mov    ebx,0x699420
		mov    ebx,[ebx]
		mov    eax,side
//    mov    eax,[ebx+0x132C0] // ebx->CombatManager
		neg    eax                // атакующая сторона
		sbb    eax,eax
		and    al,0xF0
		add    eax,0x10
		mov    ecx,ebx
		push   0
		push   0x128 // 0x64
		push   eax
		push   0x4B
		mov    eax,0x496590
		call   eax
	}
	RETURNV
}

static _Hero_ *BCHero;
static char    BCRet;
char BalistaControl(void)
{
	__asm  mov   BCHero,eax
	__asm  mov   al,[eax+0xDD]
	__asm  mov   BCRet,al
	__asm  pusha
	STARTNA(__LINE__, 0)
	BCRet=NPCBalistaControl(BCHero);
	STOP
	__asm  popa
	return BCRet;
}

static Byte *FATMon;
static float FATRet;
static float (*FATor)(void)=(float (*)(void))0x4E4B90;
float FirstAidTentHealths(void)
{
	__asm  mov   FATMon,edi
	FATRet=FATor();
	FATRet*=NPCFirstAid(FATMon);
	return FATRet;
}

int __stdcall CastHeroOrCreature(int p3,int p2,int p1)
{
	__asm   xor   eax,eax
	__asm   or    ecx,ecx
	__asm   je    l_Exit
	__asm   push  p1
	__asm   push  p2
	__asm   push  p3
	__asm   mov   eax,0x4E6260
	__asm   call  eax
	__asm   mov   IDummy,eax
l_Exit:
	return IDummy;
}


static int   R4DS_Ret,R4DS_EBX,R4DS_Num,R4DS_Yes;
static Byte *R4DS_AMon,*R4DS_DMon;
int Random4DeathStare(void)
{
	__asm pusha
	__asm mov   R4DS_EBX,ebx
	__asm mov   R4DS_DMon,ecx
	__asm mov   R4DS_AMon,esi
	STARTNA(__LINE__, 0)
	R4DS_Yes=0;
	R4DS_Num=NPCDeathStare(R4DS_AMon,R4DS_DMon);
	if(R4DS_Num>0){
		R4DS_EBX=R4DS_Num-1;
		R4DS_Ret=2; // yes
		R4DS_Yes=1;
	}else if(R4DS_Num==0){
		R4DS_Ret=99; // no
	}else{
	 // return % per creature to kill a creature (std=10%)
	 R4DS_Num=CrExpBon::DeathStare(R4DS_AMon);
	 if(R4DS_Num>0){
		 if(Random(1,100)<=R4DS_Num) R4DS_Ret=1; // yes
		 else R4DS_Ret=99; // no
	 }else{
		 __asm{
			mov   edx,100
			mov   ecx,1
			mov   eax,0x50C7C0
			call  eax
			mov   R4DS_Ret,eax
		 }
	 }
	}
//l_Exit:
	STOP
	__asm popa
	__asm mov   ecx,R4DS_EBX
	return R4DS_Ret;
}
void DeathStarePost(void)
{
	__asm  mov  ecx,R4DS_Yes
	__asm  or   ecx,ecx
	__asm  je   l_Std
	__asm  mov  edx,eax
	__asm  jmp  l_Ret
l_Std:
	__asm  mov  ecx,edx
	__asm  shr  ecx,0x1F
	__asm  add  edx,ecx
l_Ret:;
}

static int PC_Ret;
int ParalizeChance(void)
{
	__asm pusha
	__asm mov   eax,[esi+0x34] // type
	__asm cmp   eax,174
	__asm jb    l_NotNPC
	__asm cmp   eax,191
	__asm ja    l_NotNPC
	STARTNA(__LINE__, 0)
	PC_Ret=NPC_Paralize();
	STOP
	__asm popa
	__asm mov   eax,PC_Ret
	__asm jmp   l_Exit
l_NotNPC:
	__asm popa
	__asm mov   eax,0x50C7C0
	__asm call  eax
l_Exit:
	__asm mov   IDummy,eax
	return IDummy;
}
// переход по стрелялке и другим спец картинкам монстра
// для спец анимации ставить тип 0 и добавить код в LoadShootingDef ниже
Byte MoJumper5[]={
/*  2*/0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x01, 0x01,
/* 10*/0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x02, 0x02,
/* 20*/0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x03,
/* 30*/0x10, 0x10, 0x10, 0x10, 0x04, 0x04, 0x10, 0x10, 0x10, 0x10,
/* 40*/0x10, 0x05, 0x10, 0x10, 0x06, 0x06, 0x10, 0x10, 0x10, 0x10,
/* 50*/0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
/* 60*/0x10, 0x10, 0x10, 0x10, 0x07, 0x07, 0x10, 0x10, 0x10, 0x10,
/* 70*/0x10, 0x10, 0x10, 0x10, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10,
/* 80*/0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x09, 0x09,
/* 90*/0x10, 0x10, 0x10, 0x10, 0x0A, 0x0A, 0x10, 0x10, 0x10, 0x10,
/*100*/0x0B, 0x0B, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
/*110*/0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
/*120*/0x10, 0x10, 0x10, 0x0C, 0x10, 0x10, 0x10, 0x05, 0x10, 0x10,
/*130*/0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x04, 0x02, 0x0D, 0x10,
/*140*/0x10, 0x10, 0x10, 0x10, 0x10, 0x0E, 0x0F, 0x10, 0x10, 0x10,
/*150*/0x10, 0x10, 0x05, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
/*160*/0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x07,
/*170*/0x04, 0x06, 0x10, 0x10, 0x00, 0x01, 0x08, 0x06, 0x01, 0x06,
/*180*/0x02, 0x06, 0x04, 0x00, 0x01, 0x08, 0x06, 0x01, 0x06, 0x02,
/*190*/0x06, 0x04, 0x02, 0x01, 0x10, 0x10, 0x00, 0x10, 0x10, 0x10,
};
Dword MoJumper5po[]={
0x43DAAA, 0x43DAB1, 0x43DAB8, 0x43DABF, 0x43DB05,
0x43DB13, 0x43DACD, 0x43DAD4, 0x43DADB, 0x43DAE2,
0x43DAE9, 0x43DAF0, 0x43DB0C, 0x43DAC6, 0x43DAFE,
0x43DAF7, 0x43DB1A
};

// возможность действие магии героя на монстра
Byte MoJumper6[]={
/* 16*/0x00, 0x01, 0x08, 0x08,
/* 20*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x02, 0x03, 0x08, 0x08,
/* 30*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 40*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 50*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 60*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 70*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 80*/0x08, 0x08, 0x02, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/* 90*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*100*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*110*/0x08, 0x08, 0x05, 0x06, 0x08, 0x07, 0x08, 0x08, 0x08, 0x08,
/*120*/0x08, 0x04, 0x08, 0x07, 0x08, 0x06, 0x08, 0x05, 0x08, 0x08,
/*130*/0x08, 0x08, 0x02, 0x00,
/*134*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*140*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*150*/0x08, 0x04, 0x08, 0x08, 0x08, 0x04, 0x08, 0x08, 0x08, 0x08,
/*160*/0x04, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*170*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*180*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*190*/0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};
Dword MoJumper6po[]={
0x44A4BE, 0x44A4E5, 0x44A54C, 0x44A565,
0x44A56F, 0x44A4EE, 0x44A50A, 0x44A530,
0x44A4C5
};

//Кавалерийский бонус
static Byte *IC_AMon,*IC_DMon;
static int   IC_Ret,IC_AType;
int IsChampion(void)
{
	__asm pusha
	__asm mov   IC_AMon,ebx
	__asm mov   IC_DMon,edi
	__asm mov   eax,[ebx+0x34]
	__asm mov   IC_AType,eax
	STARTNA(__LINE__, 0)
	if(IC_AType==10) IC_Ret=11;
	else if(IC_AType==11) IC_Ret=11;
	else IC_Ret=NPCChampion(IC_AMon);
	if(IC_Ret!=11) IC_Ret=CrExpBon::Champion(IC_AMon);
	STOP
	__asm popa
//  __asm mov   eax,IC_Ret
	return IC_Ret;
}

static int   MR_Trigger=0;
static int   MR_Dam, MR_Type, MR_Spell, MR_Ret;
static byte *MR_Mon;
int __fastcall MagicResist(int dmg, int spell, int Type)
{
// 3.58 если G2B_CompleteAI=0, то EDI -> структ монстра под атакой
	_EDI(MR_Mon);
	int lastTrig = MR_Trigger, lastDmg = MR_Dam, lastSpell = MR_Spell, lastType = MR_Type, lastRet = MR_Ret;
	Byte *lastMon = CurrentMon;
	MR_Dam = dmg;
	MR_Spell = spell;
	CurrentMon = MR_Mon;
	MR_Type=Type;
	__asm pusha
	STARTNA(__LINE__, 0)
	MR_Trigger=1;
	MR_Ret=MR_Dam;
	MRCall(0); // call !?MR0; - before
	if((MR_Type>=174)&&(MR_Type<=191)){
		MR_Ret=NPC_Resist(MR_Type,MR_Dam,MR_Mon);
	}else{
		__asm{
			mov    eax,MR_Dam
			mov    ecx,MR_Type
			mov    edx,MR_Spell
			push   ecx
			mov    ecx,eax
			mov    eax,0x44B180 // считает ослабление удара от магии для существ
			call   eax
			mov    MR_Ret,eax
		}
	}
	MR_Ret=CrExpBon::GolemResist(MR_Mon,MR_Ret,MR_Dam,MR_Spell);
	MRCall(1); // call !?MR1; - after
	STOP
	int ret = MR_Ret; // Diakon
	MR_Trigger = lastTrig; MR_Dam = lastDmg; MR_Spell = lastSpell; MR_Type = lastType; MR_Ret = lastRet; CurrentMon = lastMon;
	__asm popa
	return ret;
}

static float MR_RetFloat;
static int *MR_Fr2EnFlag;
void _MagicDwarfResistance(void)
{
	STARTNA(__LINE__, 0)
	MR_Dam=0;
//  MR_Spell=-1;
	MR_Ret=MR_RetFloat*100.0;
	__asm{
		mov   eax,0x687FA8
		mov   eax,[eax]
		mov   edx,MR_Spell
		shl   edx,4
		add   edx,MR_Spell
		shl   edx,3
		add   eax,edx
		mov   MR_Fr2EnFlag,eax
	}
	if(*MR_Fr2EnFlag<=0){
		if(MR_Mon!=0){
			if(*(int *)&MR_Mon[0x34]==196){ // Dracolich)
				MR_Ret-=20; // remove 20% from possible damage
			}
		}
		MR_Ret=CrExpBon::DwarfResist(MR_Mon,100-MR_Ret,MR_Spell);
	}else{ // дружественное
		MR_Ret=CrExpBon::DwarfResistFriendly(MR_Mon,100-MR_Ret,MR_Spell);
	}
	if(MR_Ret>100) MR_Ret=100;
	if(MR_Ret<0)   MR_Ret=0;
	MR_Trigger=1;
	MRCall(2); // call !?MR2; - Dwarf res
	MR_RetFloat=1-MR_Ret/100.0;
	STOP
}
// вызывается только для дамэджевых спелов
static Dword Or_Flags=0;
float __fastcall MagicDwarfResistance3(int spell, int monType, int p2,int p1)
{
	_EDI(MR_Mon);
	int lastTrig = MR_Trigger, lastDmg = MR_Dam, lastSpell = MR_Spell, lastType = MR_Type, lastRet = MR_Ret;
	Byte *lastMon = CurrentMon;
	MR_Spell = spell;
	CurrentMon = MR_Mon;
	MR_Type = monType; //*(int*)&CurrentMon[0x34];
	STARTNA(__LINE__, 0)
		Or_Flags=MonTable[MR_Type].Flags;
		MonTable[MR_Type].Flags=*(Dword *)&MR_Mon[0x84];
	__asm{
		mov    eax,p2
		push   p1  // _Hero_ * Owner
		push   eax
		mov    edx, monType
		mov    ecx, spell
		mov    eax,0x44A1A0
		call   eax
//    mov    MR_Ret,eax
		fstp   MR_RetFloat
// 0.0 - полный резист
// 1.0 - полный дамэдж
	}
	MonTable[MR_Type].Flags=Or_Flags;
	_MagicDwarfResistance();
	STOP
	MR_Trigger = lastTrig; MR_Dam = lastDmg; MR_Spell = lastSpell; MR_Type = lastType; MR_Ret = lastRet; CurrentMon = lastMon;
	return MR_RetFloat;
//  asm mov    eax,MR_Ret
//  return _EAX;
}
/*
static Dword SMR_Ecx;
//static Byte *SMR_Mon;
//static int SMR_Side,SMR_Spell;
static float SMR_Ret;
float pascal SpecMagicResiatance(int p1,int p2,int p3,Byte *Mon,int Side,int Spell)
{
	asm mov  SMR_Ecx,ecx
	asm{
			push  p1
			push  p2
			push  p3
			push  Mon
			push  Side
			push  Spell
			mov   ecx,SMR_Ecx
			mov   eax,0x5A83A0
			call  eax
			fstp  SMR_Ret
	}
	asm pusha
	SMR_Ret=CrExpBon::DispellResistAndOther(Mon,Spell,Side,SMR_Ret);
	asm popa
	return SMR_Ret;
}
*/
char __stdcall DispellResist(int Side,_Hero_ *DHero)
{
	int  Spell;
	_EAX(Spell);
	Byte *Mon;
	_EDI(Mon);
	if(CrExpBon::DispellResist(Spell,Mon,Side)) return 1;
	if(DHero==0) return 0;
	__asm{
		mov   ecx,DHero
		push  92
		mov   eax,0x4D9460
		call  eax
		mov   CDummy,al
	}
	return CDummy;
}
/*
int MagicDwarfResistance(void)
{
	asm{
		mov    eax,0x617F94
		call   eax
		mov    MR_Ret,eax
		mov    MR_Mon,edi
		mov    eax,[edi+0x34]
		mov    MR_Type,eax
	}
	asm pusha
	_MagicDwarfResistance();
	asm popa
	asm mov    eax,MR_Ret
	return _EAX;
}
int MagicDwarfResistance2(void)
{
	asm{
		mov    eax,0x617F94
		call   eax
		mov    MR_Ret,eax
		mov    MR_Mon,esi
		mov    eax,[esi+0x34]
		mov    MR_Type,eax
	}
	asm pusha
	_MagicDwarfResistance();
	asm popa
	asm mov    eax,MR_Ret
	return _EAX;
}
*/
int ERM_MonRes(char Cmd,int/*Num*/,_ToDo_* /*sp*/,Mes *Mp) //!!MR
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	if(MR_Trigger==0){
		MError("ERROR! Attempt to call \"!!MR\" not in !?MR section.");
		RETURN(0)
	}
	switch(Cmd){
		case 'N': // номер монстра (0...41)
			if(G2B_CompleteAI){
				MError("ERROR! Attempt to use \"!!MR:N\" in non-human battle (use flag 1000 for checking).");
				RETURN(0)
			}
			v = *((int*)0x699420) + 0x54CC; // the first monster address
			v = ((int)MR_Mon - v)/0x548;
			if (v < 0 || v > 41)  v = -1; // a copy of real monster was passed to !?MR
			Apply(&v,4,Mp,0);
			break;
		case 'S': // номер спелла
			Apply(&MR_Spell,4,Mp,0); break;
		case 'M': // тип монстра
			Apply(&MR_Type,4,Mp,0); break;
		case 'D': // начальный урон
			Apply(&MR_Dam,4,Mp,0); break;
		case 'F': // конечный урон
			Apply(&MR_Ret,4,Mp,0); break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

static int SP_Ret;
static Byte *SP_Mon;
void ShooterPower(void)
{
	__asm{
		mov    SP_Mon,edi
		mov    eax,[edi][0x34]
		cmp    eax,0x95 // стрелк. башня
		je     l_yes
		cmp    eax,0x89 // щутер
		je     l_yes
		cmp    eax,170 // белый щутер
		je     l_yes
		cmp    eax,171 // красный щутер
		je     l_yes
		pusha
		mov    SP_Ret,eax
	}
	STARTNA(__LINE__, 0)
	SP_Ret=NPCNoShootPenalty(SP_Ret,0,SP_Mon); // NPC с луком?
	if(SP_Ret==1) SP_Ret=CrExpBon::NoDistPenalty(SP_Mon);
	STOP
	__asm{
		popa
		mov    eax,SP_Ret
		jmp    l_Ok
	}
//_Not:
	__asm{
		mov    eax,1
		jmp    l_Ok
	}
l_yes:
	__asm{
		xor    eax,eax
	}
l_Ok:;
}

void UnicornMagic(void)
{
	__asm{
		mov    eax,[esi+0x34]
		cmp    eax,151 // алм.дракон
		je     l_Ok
		mov    eax,0x50C7C0  // 20%
		call   eax
		jmp    l_exit
	}
l_Ok:
	__asm{
		mov    edx,40    // 50%
		mov    eax,0x50C7C0
		call   eax
	}
l_exit:;
}

void MedusaMagic(void)
{
	__asm{
		mov    eax,[esi+0x34]
		cmp    eax,153 // дьявол
		je     l_Ok
		mov    eax,0x50C7C0  // 20%
		call   eax
		jmp    l_Exit
	}
l_Ok:
	__asm{
		mov    edx,40    // 50%
		mov    eax,0x50C7C0
		call   eax
	}
l_Exit:;
}

void GDMagic(void)
{
	__asm pusha
	__asm{
		mov    ecx,[esi+0x34]
		cmp    ecx,154
		jne    l_Next
		mov    ecx,0x440254
		mov    byte ptr [ecx],0x28 // 50%
	}
l_Next:
	__asm{
		mov    ecx,0x440254
		mov    byte ptr [ecx],0x64 // 20%
	}
	__asm popa
	__asm mov    edx,[edi+0x84]
}
/*
void CheckForWrite(void)
{
	asm pusha
	asm{
		mov    ecx,[ebx+0x34]
		cmp    ecx,157
		jne   _Next
		mov    ecx,0x4650D2
		mov    byte ptr [ecx],157
		jmp    l_Exit
	}
_Next:
	asm{
		mov    ecx,0x4650D2
		mov    byte ptr [ecx],0x3D
	}
l_Exit:
	asm popa
	asm mov  byte ptr [eax+esi+0x54B0],0
}
*/

//Регенирация
static int IT_Ret,IT_Edx;
static Byte *IT_Mon;
void IsTroll(void)
{
	__asm{
		mov    IT_Mon,esi
		mov    edx,50 // умолчательное восстановление здоровья
		mov    IT_Edx,edx
		cmp    eax,0x3C
		je     l_Ok
		cmp    eax,0x3D
		je     l_Ok
		cmp    eax,0x90
		je     l_Ok
		cmp    eax,157 // гидра
		je     l_Ok_check
//    jmp   l_Exit
		pusha
	}
	__asm mov IDummy,eax
	STARTNA(__LINE__, 0)
	IT_Ret=CanNPCRegenerate(IDummy,IT_Mon);
	if(IT_Ret!=-1){
		IT_Ret=CrExpBon::Regenerate(IT_Mon,*(int *)&IT_Mon[0x58]);
		if(IT_Ret==0){ // no reg
			IT_Ret=0;
		}else{ // yes
			IT_Edx=IT_Ret;
			IT_Ret=-1;
		}
	}
	STOP
	__asm{
		 popa
		 mov    eax,IT_Ret
//    je    _Ok_check
		 jmp    l_Exit
	}
l_Ok_check:
	CalcRand100();
	__asm{
		cmp    eax,40  // 40%
		ja     l_Exit
	}
l_Ok:
	__asm mov    eax,-1
l_Exit:
	__asm mov    edx,IT_Edx
}

static int FDD_ret,FDD_RetExp/*,FDD_pos*/;
static Byte *FDD_mon;
void FixDDBug(void)
{
	_EDI(FDD_ret);
	_EBX(FDD_mon);
//  if((*((Dword *)&FDD_mon[0x84])&0x1){ // double wide
		if(*((int *)&FDD_mon[0xF4])==0){ // левый
			if(*((int *)&FDD_mon[0x44])!=0) return;
			++FDD_ret;
		}else{ // правый
			if(*((int *)&FDD_mon[0x44])==0) return;
			--FDD_ret;
		}
//  }
}
int ExpoHarpy(void)
{
	_EDI(FDD_ret);
	_EBX(FDD_mon);
	STARTNA(__LINE__, 0)
	if(CrExpBon::IsHarpy(FDD_mon)==0) RETURN(0)
	if((*((Dword *)&FDD_mon[0x84]))&0x1){ // double wide
		if(*((int *)&FDD_mon[0xF4])==0){ // левый
			if(*((int *)&FDD_mon[0x44])!=0) RETURN(1)
			++FDD_ret;
		}else{ // правый
			if(*((int *)&FDD_mon[0x44])==0) RETURN(1)
			--FDD_ret;
		}
	}
	RETURN(1)
}

// Атака с возвратом
static int CMA_Harpy[50];

HARPYATACK HarpyStr [MONNUM]; // Diakon
void IsHarpy(void)
{
// ebx -> monster structure
	int num_m; // Diakon
	__asm{
		/*cmp    eax,0x48
		je     l_Ok
		cmp    eax,0x49
		je     l_Ok
		cmp    eax,155 // черн. дракон
		je     l_OkDD */ 
		mov num_m, eax

	}
	if (HarpyStr[num_m].IsHave) //Diakon
		__asm jmp  l_OkDD;  

	__asm  pusha
	FDD_RetExp=ExpoHarpy();
	__asm  popa
	__asm{
		mov  eax,FDD_RetExp
		or   eax,eax
		je   l_Not
		pusha
		mov   eax,FDD_ret;
		mov   [esp],eax
		popa
		jmp   l_Ok
	}
l_OkDD:
	__asm  pusha
	STARTNA(__LINE__, 0)
	FixDDBug();
	STOP
	__asm{
		mov   eax,FDD_ret;
		mov   [esp],eax
		popa
//    mov   edi,FDD_ret;
		jmp    l_Ok
	}
l_Not:
	__asm{
		mov    eax,-1
		jmp    l_Exit
	}
l_Ok:
		// 3.58 check for binded
	__asm{
		mov    eax,[ebx+0x2B8]
		or     eax,eax
		je    _NoBind
		mov    eax,-1
		jmp    l_Exit
	}
_NoBind:
	__asm{
		mov    eax,[esi+0x132B8]
		or     eax,eax
		je     l_Side
		mov    eax,21
	}
l_Side:
	__asm{
		add    eax,[esi+0x132BC]
		lea    edx,CMA_Harpy
		mov    eax,[edx+eax*4]
		or     eax,eax
		je     l_Done
		mov    eax,-1
		jmp    l_Exit
	 }
l_Done:
	__asm{
		xor    eax,eax
	}
l_Exit:;
}
/*
static void _IsPhoenix(void)
{
	asm{
		mov    eax,ebx
		or     eax,eax
		jne   _Ok
		inc    eax
	}
_Ok:
	asm  mov    ecx,[esi+0x84]
}
*/
static Byte *IFB_Mon;
static int IFB_Ret;
void IsFireBird(void)
{
	__asm{
		mov    IFB_Mon,esi
		cmp    dword ptr [esi+0x34],0x83
		je     l_Ok
		cmp    dword ptr [esi+0x34],158
		je     l_Ok1
	}
	__asm pusha
	IFB_Ret=CrExpBon::Rebirth(IFB_Mon);
	__asm popa
	__asm{
		mov    eax,IFB_Ret
		jmp    l_Exit
	}
l_Ok1:
	__asm{
		mov    eax,0x4690E4
		mov    dword ptr [eax  ],0x90430475
		mov    word  ptr [eax+4],0x9090
		jmp    l_Done
	}
l_Ok:
//004690E2 85DB           test   ebx,ebx
//004690E4 0F84A8000000   je     H3.00469192
//004690EA 8B8E84000000   mov    ecx,[esi+00000084]
	__asm{
		mov    eax,0x4690E4
		mov    dword ptr [eax  ],0x00A8840F
		mov    word  ptr [eax+4],0x0000
	}
l_Done:
	__asm{
		xor    eax,eax
	}
l_Exit:;
}

// настройка монстров перед битвой
Dword SUBB_BatMan;
static void _SetUpBeforeBattle(void)
{
	STARTNA(__LINE__, 0)
	int       i;
	int       m150,m156,m153,m154,m196has;
	Byte     *bm=(Byte *)SUBB_BatMan;
	_Hero_   *hp1=(_Hero_ *)*(Dword *)&bm[0x53CC];
	_Hero_   *hp2=(_Hero_ *)*(Dword *)&bm[0x53D0];
	_MonArr_ *mp1=(_MonArr_ *)*(Dword *)&bm[0x54C4];
	_MonArr_ *mp2=(_MonArr_ *)*(Dword *)&bm[0x54C8];
	m150=m156=m153=m154=m196has=0;
	FirstTowerAttack=1;
	if(hp1!=0){ // есть герой
		for(i=0;i<7;i++){
			if(hp1->Ct[i]==150){ // верховный архангел
				if(m150==0){ m150=1; hp1->DMorale1+=(char)1; }
			}
			if(hp1->Ct[i]==153){ // дьявол
				if(m153==0){ if(hp2!=0){ m153=1; hp2->DLuck-=(char)1; if(hp2->DLuck<0) hp2->DLuck=0; } }
			}
			if(hp1->Ct[i]==154){ // некро дракон
				if(m154==0){ if(hp2!=0){ m154=1; hp2->DMorale1-=(char)2; } }
			}
			if(hp1->Ct[i]==156){ // чудище
				if(m156==0){ m156=1; hp1->DLuck+=(char)2; }
			}
			if(hp1->Ct[i]==196){ // dracolish
				m196has=1;
			}
		}
		if(m196has){
			if(m154==0){
				if(hp2!=0){ hp2->DMorale1-=(char)1; }
			}
		}
	}else{      // нет героя
		for(i=0;i<7;i++){
			if(mp1->Ct[i]==153){ // дьявол
				if(m153==0){ if(hp2!=0){ m153=1; hp2->DLuck-=(char)1; if(hp2->DLuck<0) hp2->DLuck=0;} }
			}
			if(mp1->Ct[i]==154){ // некро дракон
				if(m154==0){ if(hp2!=0){ m154=1; hp2->DMorale1-=(char)2; } }
			}
			if(mp1->Ct[i]==196){ // dracolish
				m196has=1;
			}
		}
		if(m196has){
			if(m154==0){
				if(hp2!=0){ hp2->DMorale1-=(char)1; }
			}
		}
	}
	m150=m156=m153=m154=m196has=0;
	if(hp2!=0){ // есть герой
		for(i=0;i<7;i++){
			if(hp2->Ct[i]==150){ // верховный архангел
				if(m150==0){ m150=1; hp2->DMorale1+=(char)1; }
			}
			if(hp2->Ct[i]==153){ // дьявол
				if(m153==0){ if(hp1!=0){ m153=1; hp1->DLuck-=(char)1; if(hp1->DLuck<0) hp1->DLuck=0;} }
			}
			if(hp2->Ct[i]==154){ // некро дракон
				if(m154==0){ if(hp1!=0){ m154=1; hp1->DMorale1-=(char)2; } }
			}
			if(hp2->Ct[i]==156){ // чудище
				if(m156==0){ m156=1; hp2->DLuck+=(char)2; }
			}
			if(hp2->Ct[i]==196){ // dracolish
				m196has=1;
			}
		}
		if(m196has){
			if(m154==0){
				if(hp1!=0){ hp1->DMorale1-=(char)1; }
			}
		}
	}else{      // нет героя
		for(i=0;i<7;i++){
			if(mp2->Ct[i]==153){ // дьявол
				if(m153==0){ if(hp1!=0){ m153=1; hp1->DLuck-=(char)1; if(hp1->DLuck<0) hp1->DLuck=0;} }
			}
			if(mp2->Ct[i]==154){ // некро дракон
				if(m154==0){ if(hp1!=0){ m154=1; hp1->DMorale1-=(char)2; } }
			}
			if(mp2->Ct[i]==196){ // dracolish
				m196has=1;
			}
		}
		if(m196has){
			if(m154==0){
				if(hp1!=0){ hp1->DMorale1-=(char)1; }
			}
		}
	}
	STOP
}

static int CMA_Shoots[50];
static void _SetUpNewRoundOnce(void);
Dword SUNRO_BatMan;
static void _SetUpBeforeBattle2(void)
{
	STARTNA(__LINE__, 0)
	int       i,j,tp;
	Byte     *bm=(Byte *)SUBB_BatMan;
	Byte     *mon;
//  _Hero_   *hp1=(_Hero_ *)*(Dword *)&bm[0x53CC];
//  _Hero_   *hp2=(_Hero_ *)*(Dword *)&bm[0x53D0];
	NMT_First=0; // Сброс для Санта Гремлина и других, кто иниц. после тактики
///////////  PlaceNPCAtBattleStart(bm);
//  CrExpBon::PrepareBFExpStructure(bm);
//  if(bm[0x13D68]==0){  // NO tactic phase
		*(int *)&((Byte *)bm)[0x13D6C]=0; // clear the turn index
//  }
	for(i=0;i<(21*2);i++){
		mon=&bm[0x54CC+0x548*i];
		*(int *)&mon[0x288]=0; // not being hypnotized
		*(int *)&mon[0xF4] =i/21; // initial side
		// 3.58 Tower skip
		if(i==20) continue; // 3.58
		if(i==41) continue; // 3.58
		mon=&bm[0x54CC+0x548*i];
//    AdjustStackParams(/*bm,*/mon,0); // 3.58 Creature experience
		CrExpBon::Apply(mon);
		tp=*(int *)&mon[0x34];
		if((tp>=174)&&(tp<=191)){
			 AddMagic2NPC(mon,bm);
		}
		if(tp==152){ // титан
			SetMonMagic(mon,28,1000,3,0); // воздушный щит
		}
		if(tp==156){ // чудище
			*(int *)&mon[0x454]=2;
		}
		if(tp==158){ // феникс
			SetMonMagic(mon,0x1D,1000,1,0); // огненный щит
			SetMonMagic(mon,55,1000,3,0); // палач
		}
		if(tp==195){ // Hell Steed (fire horse)
			SetMonMagic(mon,0x1D,1000,1,0); // огненный щит
		}
		if((tp>=GODMONTSTRT)&&(tp<(GODMONTSTRT+GODMONTNUM))){ // бог
			*(int *)&mon[0x454]=0; // не отвечает на атаку
		}
		if(tp==164){ // огненный голем
			SetMonMagic(mon,31,1000,3,0); // защита от огня
		}
		if(tp==165){ // огненный голем
			SetMonMagic(mon,33,1000,3,0); // защита от земли
		}
		if(tp==166){ // огненный голем
			SetMonMagic(mon,30,1000,3,0); // защита от воздуха
		}
		if(tp==167){ // огненный голем
			SetMonMagic(mon,32,1000,3,0); // защита от воды
		}
		if(tp==169){ // белый монах
			SetMonMagic(mon,36,1000,3,0); // волшебное зеркало
		}
		if(tp==170){ // белый шутер
			SetMonMagic(mon,28,1000,3,0); // воздушный щит
		}
		if(tp==171){ // красный шутер
			SetMonMagic(mon,0x1D,1000,1,0); // огненный щит
		}
		if(tp==192){ // Centaur-Elf
			SetMonMagic(mon,49,1000,3,0); // Mirth
		}
		for(j=0;j<100;j++){
			if(MonsterOfWeek[j]==-1) break;
			if(tp==MonsterOfWeek[j]){ // монстр недели
				SetMonMagic(mon,48,1000,3,0); // воздушный щит
			}
		}
		CrExpBon::ApplySpell(mon,bm);
//    CrExpBon::ApplyMassSpell(mon);
		if(tp==0x95){        // башня
			_CastleSetup_ *cp;
			cp=(_CastleSetup_ *)*(Dword *)&bm[0x53C8];
			if(IsCastleSetup(cp)==1){ // замок
				SetTowerBunus(cp,mon);
/*
				int side=*(int *)&mon[0xF4]; // сторона
				_Hero_   *htmp;
				_Hero_   *hpo=(_Hero_ *)*(Dword *)&bm[0x53CC];
				_Hero_   *hpe=(_Hero_ *)*(Dword *)&bm[0x53D0];
				if(side!=0){ htmp=hpo; hpo=hpe; hpe=htmp; }
				if((hpo!=0)&&(HeroHasArt(hpo,0x8E))){ // свой
//          *(int *)&mon[0x4C]=99;
				}
*/
			}
		}

/*
		//asm int 3
			Byte *gmon;
			int i,n,type,gm,num,x,y,x1,y1;
			int Code=*(int *)&mon[0x38];
			num=*(int *)&mon[0x60];
			gm=*(int *)&mon[0xF4];
			_Hero_   *hp;
			if(gm) hp=hp2; else hp=hp1;
			n=6;
			if(hp!=0){
				type=(hp->Spec/2)*14;
//        if((hp->Spec/2)==1) n=4;
			}else{
				type=28; // гремлин
			}
			if(M2B_FindPos(Code,&x,&y)==-1){ Error(); return; }
			for(i=0;i<n;i++){
				x1=x+SG_Shift[i][0];
				y1=y+SG_Shift[i][1];
				if(M2B_CheckPosCode(x1,y1)==0){
					if((Code=M2B_GetPosCode(x1,y1))!=0){
						gmon=SammonCreature(type,num,Code,gm,-1,0);
						if(gmon!=0){
							*(int *)&gmon[0xC4]=-1;
//              *(int *)&gmon[0x270]=1;
//              *(Dword *)&gmon[0x84]|=0x40;
						}
					}
				}
			}
		}
*/
//    if((*(int *)&mon[0x4C])==0) continue;
//    BACall_Mon=mon;
//    BACall2(3,*(int *)&bm[0x13D6C]);
//    BACall_Mon=0;
		// Adjust additional Commander info
		ApplyCmdMonChanges(mon);
	}
	SUNRO_BatMan=SUBB_BatMan;
	_SetUpNewRoundOnce();
	for(i=0;i<50;i++) CMA_Shoots[i]=CMA_Harpy[i]=0;
	BACall2(2,-1);
	STOP
}

static Byte *PNPC2_Bm;
static int   PNPC2_Side;
static int   PNPC2_Placed;
void PlaceNPC2(void)
{
	__asm  mov   PNPC2_Bm,ebx
	__asm  mov   PNPC2_Side,edi
	__asm  mov   PNPC2_Placed,esi
	__asm  pusha
	STARTNA(__LINE__, 0)
	PNPC2_Placed=PlaceNPCAtBattleStart2(PNPC2_Bm,PNPC2_Side,PNPC2_Placed);
	STOP
	__asm  popa
	__asm  mov   esi,PNPC2_Placed
	__asm  mov   [ebx+4*edi+0x54BC],esi
}

void __stdcall SetUpBeforeBattle(Dword position)
{
	__asm  mov  SUBB_BatMan,ecx
	__asm pusha
	STARTNA(__LINE__, 0)
	_SetUpBeforeBattle();
	__asm popa
	__asm{
		mov    eax,position
		mov    ecx,SUBB_BatMan
		push   eax
		mov    eax,0x4631E0
		call   eax
	}
	__asm pusha
	_SetUpBeforeBattle2();
	STOP
	__asm popa
}

Dword SUNR_BatMan;
Dword SUNR_Mon;
static void  _SetUpNewRound(void)
{
	STARTNA(__LINE__, 0)
	int j;
	Byte *sgp,*bmp=(Byte *)SUNR_BatMan;
	Byte *mon=(Byte *)SUNR_Mon;
	int   tp=*(int *)&mon[0x34];
//  Byte     *bm=(Byte *)SUNR_BatMan;
//  _Hero_   *hp1=(_Hero_ *)*(Dword *)&bm[0x53CC];
//  _Hero_   *hp2=(_Hero_ *)*(Dword *)&bm[0x53D0];

	if((tp>=174)&&(tp<=191)){
		 AddMagic2NPC(mon,bmp);
	}
	CrExpBon::Apply2(mon);
	if(tp==152){ // титан
		SetMonMagic(mon,28,1000,3,0); // воздушный щит
	}
	if(tp==156){ // чудище
		*(int *)&mon[0x454]=2; // отвечает дважды
	}
	if(tp==158){ // феникс
		SetMonMagic(mon,0x1D,1000,1,0); // огненный щит
		SetMonMagic(mon,55,1000,3,0); // палач
	}
	if(tp==195){ // Hell Steed (fire horse)
		SetMonMagic(mon,0x1D,1000,1,0); // огненный щит
	}
	for(j=0;j<100;j++){
		if(MonsterOfWeek[j]==-1) break;
		if(tp==MonsterOfWeek[j]){ // монстр недели
			SetMonMagic(mon,48,1000,3,0); // воздушный щит
		}
	}
	if((tp>=GODMONTSTRT)&&(tp<(GODMONTSTRT+GODMONTNUM))){ // бог
		*(int *)&mon[0x454]=0; // не отвечает на атаку
	}
	if(tp==164){ // огненный голем
		SetMonMagic(mon,31,1000,3,0); // защита от огня
	}
	if(tp==165){ // огненный голем
		SetMonMagic(mon,33,1000,3,0); // защита от земли
	}
	if(tp==166){ // огненный голем
		SetMonMagic(mon,30,1000,3,0); // защита от воздуха
	}
	if(tp==167){ // огненный голем
		SetMonMagic(mon,32,1000,3,0); // защита от воды
	}
	if(tp==169){ // белый монах
		SetMonMagic(mon,36,1000,3,0); // волшебное зеркало
	}
	if(tp==170){ // белый шутер
		SetMonMagic(mon,28,1000,3,0); // воздушный щит
	}
	if(tp==171){ // красный шутер
		SetMonMagic(mon,0x1D,1000,1,0); // огненный щит
	}
	if(tp==192){ // Centaur-Elf
		SetMonMagic(mon,49,1000,3,0); // Mirth
	}
	CrExpBon::ApplySpell(mon,bmp);
	for(j=0;j<SG_GuardsSet;j++){
	 if(SG_GuardsSetUp[j].own_stack!=-1){
		 sgp=&bmp[0x54CC+0x548*SG_GuardsSetUp[j].sg_stack];
		 if((*(int *)&sgp[0x4C])<=0){
			 sgp=&bmp[0x54CC+0x548*SG_GuardsSetUp[j].own_stack];
			 *(int *)&sgp[0xC4]=SG_GuardsSetUp[j].own_speed;
		 }
	 }
 }
/*
		if(tp==172){        // Santa Gremlin
//asm int 3
		 if((*(Dword *)&bm[0x13D6C])==1){
			Byte *gmon;
			int i,n,type,gm,num,x,y,x1,y1;
			int Code=*(int *)&mon[0x38];
			num=*(int *)&mon[0x60];
			gm=*(int *)&mon[0xF4];
			_Hero_   *hp;
			if(gm) hp=hp2; else hp=hp1;
			n=6;
			if(hp!=0){
				type=(hp->Spec/2)*14;
//        if((hp->Spec/2)==1) n=4;
			}else{
				type=28; // гремлин
			}
			if(M2B_FindPos(Code,&x,&y)==-1){ Error(); return; }
			for(i=0;i<n;i++){
				x1=x+SG_Shift[i][0];
				y1=y+SG_Shift[i][1];
				if(M2B_CheckPosCode(x1,y1)==0){
					if((Code=M2B_GetPosCode(x1,y1))!=0){
						gmon=SammonCreature(type,num,Code,gm,-1,0);
						if(gmon!=0){
							*(int *)&gmon[0xC4]=-1;
//              *(int *)&gmon[0x270]=1;
//              *(Dword *)&gmon[0x84]|=0x40;
						}
					}
				}
			}
		 }
		}
*/
//  BACall_Mon=(Byte *)SUNR_Mon;
//  BACall2(3,*(int *)&((Byte *)SUNR_BatMan)[0x13D6C]);
//  BACall_Mon=0;
	if(BACall_Day!=(*(int *)&((Byte *)SUNR_BatMan)[0x13D6C])){
		BACall_Day=(*(int *)&((Byte *)SUNR_BatMan)[0x13D6C]);
		if(BACall_Turn<0) --BACall_Turn; else ++BACall_Turn;
		BACall2(2,BACall_Turn);
	}
	STOP
}
int ChechPosition(int Pos)
{
	STARTNA(__LINE__, 0)
	int i;
	Byte *bh;
	i=Pos%17;
	if(i==0) RETURN(-1) // крайняя левая
	if(i==16) RETURN(-1) // крайняя правая
	bh=GetHexStr(Pos);
	if(bh==0) RETURN(-1)
	if((bh[0x10]&0x3F)!=0) RETURN(-1) // припятствие
	if(bh[0x18]!=0xFF) RETURN(-1) // есть живой стек
	if(bh[0x1C]!=0) RETURN(-1) // есть мертвый стэк
	RETURN(0)
}

int QuickSand(int herMON,int Num,int Pos,int Draw)
{
	STARTNA(__LINE__, 0)
	int i,v,v2;
	Byte *bm,*tmon;
	_Hero_ *hp;
	Dword *pqb;
	__asm{
		mov    eax,0x699420
		mov    eax,[eax]
		mov    bm,eax
	}
	if(ChechPosition(Pos)) RETURN(-1)
	if(herMON){ // monster
		tmon=&bm[0x54CC+0x548*Num];
		if((*(int *)&tmon[0x4C])==0) RETURN(-1) // монстр уже убит
	}else{ // hero
		hp=*(_Hero_ **)&bm[0x53CC+Num*4];
		if(hp==0) RETURN(-1)
	}
//  asm{mov eax,0x642224; mov dword ptr [eax],1; mov dword ptr [eax+4],1; mov dword ptr [eax+8],1; mov dword ptr [eax+12],1 }
	__asm{
		mov eax,0x5A0664; 
		mov dword ptr [eax],0x000001BF; 
		mov word ptr [eax+4],0x9000; 
		mov byte ptr [eax+6],0x90 
	}
	__asm{
		mov eax,0x5A06AD; 
		mov byte ptr [eax],0xB8; 
		mov edx,Pos; 
		mov [eax+1],edx 
	}
	__asm{
		mov eax,0x6987CC; 
		mov pqb,eax 
	}
	i=*pqb; if(Draw==0){ if(i==0) *pqb=1; }
	if(herMON){ // monster
		v= *(int *)&bm[0x132B8]; *(int *)&bm[0x132B8]=Num/21;
		v2=*(int *)&bm[0x132BC]; *(int *)&bm[0x132BC]=Num%21;
		CastSpell(10,Pos,1,-1,3,3);
		*(int *)&bm[0x132B8]=v;
		*(int *)&bm[0x132BC]=v2;
	}else{ // hero
		v=*(int *)&bm[0x132C0]; *(int *)&bm[0x132C0]=Num;
		CastSpell(10,Pos,0,-1,-1,3);
		*(int *)&bm[0x132C0]=v;
	}
	*pqb=i;
	__asm{
		mov eax,0x5A06AD; 
		mov byte ptr [eax],0xE8; 
		mov dword ptr [eax+1],0xFFF6C27E 
	}
	__asm{
		mov eax,0x5A0664; 
		mov dword ptr [eax],0x24B53C8B; 
		mov word ptr [eax+4],0x6422; 
		mov byte ptr [eax+6],0 
	}
//  asm{mov eax,0x642224; mov dword ptr [eax],4; mov dword ptr [eax+4],4; mov dword ptr [eax+8],6; mov dword ptr [eax+12],8 }
	RETURN(0)
}

int LandMine(int herMON,int Num,int Pos,int Draw)
{
	STARTNA(__LINE__, 0)
	int i,v,v2;
	Byte *bm,*tmon;
	_Hero_ *hp;
	Dword *pqb;
	__asm{
		mov    eax,0x699420
		mov    eax,[eax]
		mov    bm,eax
	}
	if(ChechPosition(Pos)) RETURN(-1)
	if(herMON){ // monster
		tmon=&bm[0x54CC+0x548*Num];
		if((*(int *)&tmon[0x4C])==0) RETURN(-1) // монстр уже убит
	}else{ // hero
		hp=*(_Hero_ **)&bm[0x53CC+Num*4];
		if(hp==0) RETURN(-1)
	}
	__asm{
		mov eax,0x5A084B; 
		mov dword ptr [eax],0x000001BA; 
		mov word ptr [eax+4],0x9000; 
		mov byte ptr [eax+6],0x90 
	}
	__asm{
		mov eax,0x5A08C5; 
		mov byte ptr [eax],0xB8; 
		mov edx,Pos; 
		mov [eax+1],edx 
	}
	__asm{
		mov eax,0x6987CC; 
		mov pqb,eax 
	}
	i=*pqb; if(Draw==0){ if(i==0) *pqb=1; }
	if(herMON){ // monster
		v= *(int *)&bm[0x132B8]; *(int *)&bm[0x132B8]=Num/21;
		v2=*(int *)&bm[0x132BC]; *(int *)&bm[0x132BC]=Num%21;
		CastSpell(11,Pos,1,-1,3,3);
		*(int *)&bm[0x132B8]=v;
		*(int *)&bm[0x132BC]=v2;
	}else{ // hero
		v=*(int *)&bm[0x132C0]; *(int *)&bm[0x132C0]=Num;
		CastSpell(11,Pos,0,-1,-1,3);
		*(int *)&bm[0x132C0]=v;
	}
	*pqb=i;
	__asm{
		mov eax,0x5A08C5; 
		mov byte ptr [eax],0xE8; 
		mov dword ptr [eax+1],0xFFF6C066 
	}
	__asm{
		mov eax,0x5A084B; 
		mov dword ptr [eax],0x34B5148B; 
		mov word ptr [eax+4],0x6422; 
		mov byte ptr [eax+6],0 
	}
	RETURN(0)
}

void PlayAnimation(byte *bm, byte *mon, int animId, int badEffect = 0)
{
	PlayingBM_V = true;
	__asm{ // анимация (сопротивление 0x4E)
		mov    ecx,bm
		mov    eax,mon
		mov    edx,animId
		push   badEffect
		push   100
		push   eax // ->monster
		push   edx
		mov    eax,0x4963C0
		call   eax
	}
	PlayingBM_V = false;
}

//Dword Glb_LastSummoned=0;
int ERM_BRound(char Cmd,int Num,_ToDo_*sp,Mes *Mp) //!!BM
{
	int Spell,MSkill,HSkill,Pos,Redraw,Check,st;
	int i,val,val2,val4;
	Dword val3;
	Byte *bm,*mon,*tmon;
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	if(G2B_CompleteAI){
		MError("ERROR! Attempt to use \"!!BM\" in non-human battle (use flag 1000 for checking).");
		RETURN(0)
	}
	int mn=GetVarVal(&sp->Par[0]);
	//  if(Num<2){ MError("\"!!BR:\"-wrong syntax."); return 0; }
	if((mn<-1)||(mn>41)){
		MError("\"!!BM:\"-monster index is incorrect (-1, 0...41).");
		RETURN(0)
	}
	bm = combatManager;
	if (mn == -1)
		if (CurrentMon)
			mon = CurrentMon;
		else
			mn = M2B_GetMonNum(bm);

	if (mn >= 0)
		mon=&bm[0x54CC+0x548*mn];

	switch(Cmd){
		case 'T': Apply((int *)&mon[0x034],4,Mp,0); break;
		case 'N': Apply((int *)&mon[0x04C],4,Mp,0); break;
//    case '': Apply((int *)&mon[0x050],4,Mp,0); break;
		case 'L': Apply((int *)&mon[0x058],4,Mp,0); break;
		case 'B': Apply((int *)&mon[0x060],4,Mp,0); break;
		case 'E': Apply((int *)&mon[0x0DC],4,Mp,0); break;
		case 'I': Apply((int *)&mon[0x0F4],4,Mp,0); break;
		case 'A': Apply((int *)&mon[0x0C8],4,Mp,0); break;
		case 'D': Apply((int *)&mon[0x0CC],4,Mp,0); break;
		case 'H': 
			if(Apply((int *)&mon[0x0C0],4,Mp,0)) break; 
			*(int *)&mon[0x06C]=*(int *)&mon[0x0C0];
			break;
		case 'S': Apply((int *)&mon[0x0C4],4,Mp,0); break;
		case 'F': Apply((int *)&mon[0x084],4,Mp,0); break;
		case 'O': Apply((int *)&mon[0x05C],4,Mp,0); break;
		case 'R': Apply((int *)&mon[0x454],4,Mp,0); break;
		case 'J': Apply((int *)&mon[0x194],4,Mp,0); break;
		case 'U': // 3.58 U#/...
			switch(Mp->n[0]){
				case 1: // U1/$ - min damage
					Apply((int *)&mon[0x0D0],4,Mp,1); 
					break;
				case 2: // U2/$ - max damage
					Apply((int *)&mon[0x0D4],4,Mp,1); 
					break;
				case 3: // U3/$ - number of shots
					Apply((int *)&mon[0x0D8],4,Mp,1); 
					break;
				case 4: // U4/$ - тип спела для каста
					Apply((int *)&mon[0x4E0],4,Mp,1); // чушь
					break;   
				case 5: // U5/$ - номер стэка - копии данного (-1 = нет)
					Apply((int *)&mon[0x28],4,Mp,1); 
					break;
				default:
					EWrongSyntax(); RETURN(0)
			}
			break;
		case 'G':
			CHECK_ParamsMin(3);
			if(Apply(&Spell,4,Mp,0)){ MError("\"!!BM:G\"-par 1 may be set only."); RETURN(0) }
			//  Но то, что проверки нет, уже используется в скриптах
			if(Spell*4+0x198 < 0 || Spell*4+0x198 >= 0x548){ MError("\"!!BM:G\"-spell index is incorrect."); RETURN(0) }
			val = *(int*)&mon[Spell*4+0x198]; // last duration
			Apply((int *)&mon[Spell*4+0x198],4,Mp,1); // длительность
			if (Spell*4+0x2DC >= 0x548) break; // Использование в качестве хака для получения/изменения параметров монстра
			val2 = *(int*)&mon[Spell*4+0x2DC]; // last power
			Apply((int *)&mon[Spell*4+0x2DC],4,Mp,2); // сила
			if ((Spell>=0) && (Spell<=80) && (val != 0) && (*(int*)&mon[Spell*4+0x198] == 0)) // убираем, если длительность = 0
			{
				*(int*)&mon[Spell*4+0x198] = val;
				*(int*)&mon[Spell*4+0x2DC] = val2;
				_asm{
					push Spell
					mov ecx, mon
					mov eax, 0x444230
					call eax
				}
			}
			break;
		case 'C': // Cspell/pos/Mskill/HSkill/Check4TargetMonster
			CHECK_ParamsMin(5);
			if(Apply(&Spell,4,Mp,0)){ MError("\"!!BM:C\"-par 1 may be set only."); RETURN(0) }
			if(Apply(&Pos,4,Mp,1))  { MError("\"!!BM:C\"-par 2 may be set only."); RETURN(0) }
			if(Apply(&MSkill,4,Mp,2)) { MError("\"!!BM:C\"-par 3 may be set only."); RETURN(0) }
			if(Apply(&HSkill,4,Mp,3)) { MError("\"!!BM:C\"-par 4 may be set only."); RETURN(0) }
			if(Apply(&Check,4,Mp,4)) { MError("\"!!BM:C\"-par 5 may be set only."); RETURN(0) }
			if((*(int *)&mon[0x4C])==0) break; // монстр уже убит
			if(Check){ // если надо проверять
				for(i=0,st=-1;i<(21*2);i++){
					// 3.58 Tower skip
					if(i==20) continue; // 3.58
					if(i==41) continue; // 3.58
					tmon=&bm[0x54CC+0x548*i];
					if((*(int *)&tmon[0x38])==Pos){ st=i; break; }
				}
				if(st==-1) break;
				tmon=&bm[0x54CC+0x548*st];
				if((*(int *)&tmon[0x4C])==0) break; // монстр уже убит
			}
			val= *(int *)&bm[0x132B8]; *(int *)&bm[0x132B8]=mn/21;
			val2=*(int *)&bm[0x132BC]; *(int *)&bm[0x132BC]=mn%21;
//      Glb_LastSummoned=0;
			CastSpell(Spell,Pos,1,-1,MSkill,HSkill);
			*(int *)&bm[0x132B8]=val;
			*(int *)&bm[0x132BC]=val2;
			break;
		case 'K': // Kdamage
			if(Apply(&val,4,Mp,0)) break;
			HitMonster(mon,val);
			break;
		case 'M': // MSpell/Length/Level
			if(Apply(&val ,4,Mp,0)) break;
			if(Apply(&val2,4,Mp,1)) break;
			if(Apply(&val4,4,Mp,2)) break;
			SetMonMagic(mon,val,val2,val4,0);
//    SetMonMagic(mon,33,1000,3,0); // защита от земли
			break;
		case 'P':
			if(Mp->VarI[0].Check==1){ // ?
				Apply((int *)&mon[0x038],4,Mp,0);
			}else{
				Apply(&val,4,Mp,0);
				val2=*(int *)&mon[0x0C4]; // запомним скорость
				*(int *)&mon[0x0C4]=999; // установим гигантскую скорость
				val3=*(Dword *)&mon[0x084]; // запомним флаги
				*(Dword *)&mon[0x084]|=2; // установим летучесть
				__asm{
					mov    eax,0x4F8980
					mov    byte ptr [eax],0xC3
//          mov    eax,0x4468E7
//          mov    dword ptr [eax],0x000001B9
//          mov    word ptr [eax+4],0x9000
					mov    eax,val
					mov    ecx,mon
					push   1
					push   eax
					mov    eax,0x445A30
					call   eax
					mov    eax,0x4F8980
					mov    byte ptr [eax],0x55
//          mov    eax,0x4468E7
//          mov    dword ptr [eax],0x89E80D8B
//          mov    word ptr [eax+4],0x0069
				}
				*(Dword *)&mon[0x084]=val3; // восстановим флаги
				*(int *)&mon[0x0C4]=val2; // восстановим скорость
			}
			break;
		case 'Q': // Q#/pos/redraw
			CHECK_ParamsMin(3);
			if(Apply(&val,4,Mp,0)) break;
			if(Apply(&Pos,4,Mp,1)) break;
			if(Apply(&Redraw,4,Mp,2)) break;
			switch(val){
				case 0: QuickSand(1,mn,Pos,Redraw); break;
				case 1: LandMine(1,mn,Pos,Redraw); break;
				default:
					EWrongSyntax(); RETURN(0)
			}
			break;
		case 'V': //V# -animate magic # 3.58
			CHECK_ParamsMax(3);
			if(Num == 3)
			{
				char* oldDef = MagicAni[0].DefName;
				char* oldName = MagicAni[0].Name;
				int oldType = MagicAni[0].Type;
				MagicAni[0].Type = Mp->n[0];
				MagicAni[0].DefName = GetErmText(Mp, 2);
				MagicAni[0].Name = MagicAni[0].DefName;
				*(int*)(bm + 78572) = -1; // last effect index (force .def loading)
				PlayAnimation(bm, mon, 0, Mp->n[1]);
				*(int*)(bm + 78572) = -1; // last effect index (force .def loading)
				MagicAni[0].DefName = oldDef;
				MagicAni[0].Name = oldName;
				MagicAni[0].Type = oldType;
				break;
			}
			else
			{
				PlayAnimation(bm, mon, Mp->n[0], (Num == 2 ? Mp->n[1] : 0));
			}
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

int ERM_BHero(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	int i,v,Spell,Pos,Skill,Check,st,Redraw;
	_Hero_ *hp;
	Byte *bm,*tmon;
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	if(G2B_CompleteAI){
		MError("ERROR! Attempt to use \"!!BH\" in non-human battle (use flag 1000 for checking).");
		RETURN(0)
	}
	int hn=GetVarVal(&sp->Par[0]);
	if((hn<0)||(hn>1)){
		MError("\"!!BH:\"-hero side index is incorrect (0...1).");
		RETURN(0)
	}
	__asm{
		mov    eax,0x699420
		mov    eax,[eax]
		mov    bm,eax
	}
	switch(Cmd){
		case 'N': // N?v номер героя
			hp=*(_Hero_ **)&bm[0x53CC+hn*4];
			if(hp==0) v=-1; else v=hp->Number;
			Apply(&v,4,Mp,0);
			break;
		case 'M': // Mv кастовал герой или нет
			Apply((int *)&bm[0x54B4+hn*4],4,Mp,0);
			break;
		case 'C': // Cspell/pos/Hskill/Check4TargetMonster
			CHECK_ParamsMin(4);
			if(Apply(&Spell,4,Mp,0)) break;
			if(Apply(&Pos,4,Mp,1)) break;
			if(Apply(&Skill,4,Mp,2)) break;
			if(Apply(&Check,4,Mp,3)) break;
			hp=*(_Hero_ **)&bm[0x53CC+hn*4];
			if(hp==0) break;
			if(Check){ // если надо проверять
				for(i=0,st=-1;i<(21*2);i++){
					// 3.58 Tower skip
					if(i==20) continue; // 3.58
					if(i==41) continue; // 3.58
					tmon=&bm[0x54CC+0x548*i];
					if((*(int *)&tmon[0x38])==Pos){ st=i; break; }
				}
				if(st==-1) break;
				tmon=&bm[0x54CC+0x548*st];
				if((*(int *)&tmon[0x4C])==0) break; // монстр уже убит
			}
			v=*(int *)&bm[0x132C0]; *(int *)&bm[0x132C0]=hn;
			CastSpell(Spell,Pos,0,-1,-1,Skill);
			*(int *)&bm[0x132C0]=v;
			break;
		case 'Q': // Q#/pos/redraw
			CHECK_ParamsMin(3);
			if(Apply(&v,4,Mp,0)) break;
			if(Apply(&Pos,4,Mp,1)) break;
			if(Apply(&Redraw,4,Mp,2)) break;
			switch(v){
				case 0: QuickSand(0,hn,Pos,Redraw); break;
				case 1: LandMine(0,hn,Pos,Redraw); break;
				default: EWrongSyntax(); RETURN(0)
			}
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}
int ERM_BUniversal(char Cmd,int Num,_ToDo_*,Mes *Mp)
{
	int v,st;
	int type,num,pos,side,slotnum,redraw;
	Byte *bm;
	char *bh;
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	if(G2B_CompleteAI){
		MError("ERROR! Attempt to use \"!!BU\" in non-human battle (use flag 1000 for checking).");
		RETURN(0)
	}
	__asm{
		mov    eax,0x699420
		mov    eax,[eax]
		mov    bm,eax
	}
	switch(Cmd){
		case 'M': // добавить сообщение
		 {
			int sind;
			char *txtp;
			CHECK_ParamsNum(1);
			if(Mp->VarI[0].Type!=7){ MError("\"!!BU:\"- not a Z variable."); RETURN(0) }
			sind=GetVarVal(&Mp->VarI[0]);
			if(BAD_INDEX_LZ(sind)||(sind>1000)){ MError("\"!!BU:\"-wrong z (destination) index (-10...-1,1...1000)."); RETURN(0) }
			if(Mp->VarI[0].Check!=0){ MError("\"!!BU:\"- can use only set syntax."); RETURN(0) }
			if(sind>0) txtp=ERMString[sind-1];
			else       txtp=ERMLString[-sind-1];
			__asm{
				mov    ecx,0x699420
				mov    ecx,[ecx]
				mov    ecx,[ecx+0x132FC]
				mov    eax,txtp
				push   0
				push   1
				push   eax
				mov    eax,0x4729D0
				call   eax
			}
			break;
		 }
		case 'E': // Epos/?v номер монстра в позиции
			CHECK_ParamsMin(2);
			if(Apply(&v,4,Mp,0)) break;
			bh=(char *)GetHexStr(v);
			if(bh==0){ MError("\"!!BU:E\"-incorrect hex number."); RETURN(0) }
			if(bh[0x18]==-1){ // нет живого стека
				st=-1;
			}else{
				st=(int)bh[0x18]*21+(int)bh[0x19];
			}
			Apply(&st,4,Mp,1);
			break;
		case 'D': // Dpos/?v номер последнего убитого монстра в позиции
			CHECK_ParamsMin(2);
			if(Apply(&v,4,Mp,0)) break;
			bh=(char *)GetHexStr(v);
			if(bh==0){ MError("\"!!BU:D\"-incorrect hex number."); RETURN(0) }
			if(bh[0x18]==-1){ // нет живого стека
//        if((bh[0x10]&2)==0){ // не припятствие
				v=bh[0x1C]-1;
				if(v<0){ st=-1; // нет вообще стеков
				}else{   st=(int)bh[0x20+v]*21+(int)bh[0x2E+v]; }
			}else{ st=-2; // есть живой стек
			}
			Apply(&st,4,Mp,1);
			break;
		case 'O': // Opos/?v есть припятствие или нет
			CHECK_ParamsMin(2);
			if(Apply(&v,4,Mp,0)) break;
			bh=(char *)GetHexStr(v);
			if(bh==0){ MError("\"!!BU:O\"-incorrect hex number."); RETURN(0) }
/*
			if(bh[0x10]&2){ // припятствие
				st=1;
			}else{
				st=0;
			}
*/
			st=bh[0x10]&0x3F; // припятствие
			Apply(&st,4,Mp,1);
			break;
		case 'S': // Stype/num/pos/side/slotnum/redraw вызвать существ на поле
			CHECK_ParamsMin(6);
			if(Apply(&type,4,Mp,0)) break;
			if(Apply(&num,4,Mp,1)) break;
			if(Apply(&pos,4,Mp,2)) break;
			if(Apply(&side,4,Mp,3)) break;
			if(Apply(&slotnum,4,Mp,4)) break;
			if(Apply(&redraw,4,Mp,5)) break;
			/*gmon=*/SammonCreature(type,num,pos,side,slotnum,redraw);
			break;
		case 'T':
			Apply(&bm[0x13D68],1,Mp,0);
			break;
		case 'C':
			Apply(&bm[0x132F8],1,Mp,0);
			break;
		case 'R': RedrawBF(); break;
		case 'G': // 3.58
			Apply(&bm[0x53C0],4,Mp,0);
			break;
		case 'V': //3.58 выиграть  стороне
			if(Apply(&v,4,Mp,0)) break;
			__asm{
				mov   ecx,bm
				mov   eax,v
				push  v
				mov   eax,0x468F80
				call  eax
			}
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}
/*
static Byte *GetMonPo(Byte *mall,int n)
{
	return(&mall[n*0x548]);
}
*/
/*
static void CorMon(_Hero_ *hp,Byte *mall,int side)
{
	int i,j,t,speed,has;
	Dword flags;
	Byte *mp;
	int power=hp->SSkill[11]; // Eagle Eye
	int sp[20][2]; // распред по скоростям
	for(i=0;i<20;i++){ sp[i][0]=0; sp[i][1]=0; }
	for(has=i=0;i<20;i++){
		mp=GetMonPo(mall,i);
		if((*(int *)&mp[0x34])==-1) continue; // нет монстра
		if((*(int *)&mp[0x4C])==0) continue; // все убиты
		if((*(int *)&mp[0xF4])!=side) continue; // не та сторона
		has=1;
		speed=*(int *)&mp[0xC4];
		for(j=0;j<20;j++){
			if(speed==sp[j][0]){ ++sp[j][1]; break; }  
			if(sp[j][0]==0){ sp[j][0]=speed; sp[j][1]=1; break; }
		}
	}
	if(has==0) return; // нет монстров вообще
	for(i=0;i<20;i++){ // сортировка
		if(sp[i][0]==0) break;
		for(j=0;j<19;j++){
			if(sp[j+1][0]==0) break;
			if(sp[j][0]>sp[j+1][0]){ // меняем
				t=sp[j][0]; sp[j][0]=sp[j+1][0]; sp[j+1][0]=t;
				t=sp[j][1]; sp[j][1]=sp[j+1][1]; sp[j+1][1]=t;
			}
		}
	}
	j=0; while(sp[j][0]!=0) ++j; --j; // индекс самого быстрого
	switch(power){
		case 3: // expert
			speed=sp[j][0];
			if(sp[j][1]!=1) j=Random(0,sp[j][1]-1); else j=0;
			break;
		case 2: // adv
			j=j*2/3;
			speed=sp[j][0];
			if(sp[j][1]!=1) j=Random(0,sp[j][1]-1); else j=0;
			break;
		case 1: // bas
			j=j/3;
			speed=sp[j][0];
			if(sp[j][1]!=1) j=Random(0,sp[j][1]-1); else j=0;
			break;
		default: //0
			speed=sp[0][0];
			if(sp[0][1]!=1) j=Random(0,sp[0][1]-1); else j=0;
	}
	for(i=0;i<20;i++){
		mp=GetMonPo(mall,i);
		if((*(int *)&mp[0x34])==-1) continue; // нет монстра
		if((*(int *)&mp[0x4C])==0) continue; // все убиты
		if((*(int *)&mp[0xF4])!=side) continue; // не та сторона
		if(speed==*(int *)&mp[0xC4]){
			if(j==0){
				flags=*(Dword *)&mp[0x84];
//asm int 3
//        flags|=0x02000000; // уже ждал
//flags|=0x04000000; // уже ходил
				*(Dword *)&mp[0x84]=flags;
				break;
			}
			--j;
		}
	}
}
*/
static void _SetUpNewRoundOnce(void)
{
/*
	Byte     *bm=(Byte *)SUNRO_BatMan;
	_Hero_   *hp1=(_Hero_ *)*(Dword *)&bm[0x53CC];
	_Hero_   *hp2=(_Hero_ *)*(Dword *)&bm[0x53D0];
	Byte     *mall=&bm[0x54CC];
	if(hp1!=0){ 
		if(HeroHasArt(hp1,0x8E)){ // есть арт (+1)
//      CorMon(hp1,mall,0);
			for(int i=0;i<20;i++){
				Byte *mp=GetMonPo(mall,i);
				if((*(int *)&mp[0x34])==-1) continue; // нет монстра
				if((*(int *)&mp[0x4C])==0) continue; // все убиты
				*(Dword *)&mp[0x84]|=0x02000000;;
			}
		}
	}
	if(hp2!=0){
		if(HeroHasArt(hp2,0x8E)){ // есть арт (+1)
//      CorMon(hp2,mall,1);
			for(int i=0;i<20;i++){
				Byte *mp=GetMonPo(mall,i);
				if((*(int *)&mp[0x34])==-1) continue; // нет монстра
				if((*(int *)&mp[0x4C])==0) continue; // все убиты
				*(Dword *)&mp[0x84]|=0x02000000;;
			}
		}
	}
*/
}
void SetUpNewRoundOnce(void)
{
	_ECX(SUNRO_BatMan);
	__asm{
		mov    ecx,SUNRO_BatMan
		mov    eax,0x475800
		call   eax
	}
	__asm pusha
	_SetUpNewRoundOnce();
	__asm popa
}

static Byte *TS_BatMan;
static Byte *TS_TowerStr;
static int   TS_ShotsNum;
void __stdcall TowerShot(Dword MonNum)
{
	__asm mov  TS_BatMan,ecx
	__asm mov  TS_TowerStr,esi
	__asm pusha
	STARTNA(__LINE__, 0)
	TS_ShotsNum=1;
	_CastleSetup_ *cp;
	cp=(_CastleSetup_ *)*(Dword *)&TS_BatMan[0x53C8];
	if(IsCastleSetup(cp)==1){ // замок
		int side=*(int *)&TS_TowerStr[0xF4]; // сторона
//    _Hero_   *htmp;
		_Hero_   *hpo=(_Hero_ *)*(Dword *)&TS_BatMan[0x53CC];
		_Hero_   *hpe=(_Hero_ *)*(Dword *)&TS_BatMan[0x53D0];
		if(side!=0){ /*htmp=hpo;*/ hpo=hpe; /*hpe=htmp;*/ }
		if((hpo!=0)&&(HeroHasArt(hpo,0x8E))){ // свой
			TS_ShotsNum++; //арт
			switch(hpo->SSkill[10]){ // балистика
				case 3: TS_ShotsNum++;
				case 2: TS_ShotsNum++;
				case 1: TS_ShotsNum++;
					break;
			}
		}
	}
//  _TowerShot();
	STOP
	__asm popa
	while(TS_ShotsNum--){
		__asm{
			mov    eax,MonNum
			mov    ecx,TS_BatMan
			push   eax
			mov    eax,0x4656B0
			call   eax
		}
	}
}

void DoNotFly(void)
{
	__asm{
		cmp   eax,0x36
		je    l_Ok
		cmp   eax,0x37
		je    l_Ok
		cmp   eax,153 // архидьявол
		je    l_Ok
		cmp   eax,160 // б.п.1
		je    l_Ok
		cmp   eax,161 // б.п.2
		je    l_Ok
		cmp   eax,162 // б.п.3
		je    l_Ok
		cmp   eax,163 // б.п.4
		je    l_Ok
		xor   eax,eax   // не летает
		jmp   l_Exit
	}
l_Ok:
	__asm{
		mov   eax,0x37 // летает
	}
l_Exit:;
}

void NoMagic(void)
{
	__asm{
		mov    eax,[esi+0x34]
		cmp    eax,0x97
		je     l_Ok
		cmp    eax,0x9B
		je     l_Ok
		mov    eax,0x50C7C0
		call   eax
		jmp    l_Exit
	}
l_Ok:
	__asm{
		mov    eax,0x50C7C0
		call   eax
		mov    eax,0x64
	}
l_Exit:;
}

static Byte *F1_Mon_Po;
static int F1_Ret;
void Fear1(void)
{
	__asm{
		mov    F1_Mon_Po,edi
		cmp    dword ptr [edi+0x34],0x84
		je     l_Yes
		cmp    dword ptr [edi+0x34],150
		jl     l_Check // все новые не бояться
		cmp    dword ptr [edi+0x34],192
		jl     l_Yes // все новые не бояться
	}
l_Check:
	__asm pusha
	STARTNA(__LINE__, 0)
	// 0-fearless, 1-not
	F1_Ret=CrExpBon::Fearless(F1_Mon_Po);
	STOP
	__asm popa
	__asm{
		mov    eax,F1_Ret
		jmp    l_Exit
	}
l_Yes:
	__asm{
		xor    eax,eax
	}
l_Exit:;
}

static int F2_Ret,F2_Type;
static Byte *F2_Mon_Po;
void Fear2(void)
{
l_Next:
	__asm{
//    mov    ebx,00000084
// cmp    [ecx-18],ebx
		 mov    ebx,ecx  // 3.58
		 sub    ebx,0x4C // 3.58
		 mov    F2_Mon_Po,ebx // 3.58
		mov    ebx,[ecx-0x18]
		 mov    F2_Type,ebx
		cmp    ebx,0x84
		je     l_Ok
		cmp    ebx,153
		je     l_Ok
		cmp    ebx,155
		je     l_Ok
//    cmp    ebx,174
//    jb    _Not
//    cmp    ebx,191
//    ja    _Not
		pusha
	}
	STARTNA(__LINE__, 0)
	if(F2_Type>=174 && F2_Type<=191) F2_Ret=HasNPCFear(F2_Mon_Po);
	else F2_Ret=CrExpBon::Fear(F2_Mon_Po);
	STOP
	__asm{
		popa
		mov    ebx,F2_Ret
		or     ebx,ebx
		je     l_Not
	}
l_Ok:
	__asm add    edx,[ecx]
l_Not:
	__asm{
		add    ecx,0x548
		dec    eax
		jne    l_Next
	}
}
static int TW_Ret;
static Byte *TW_Mon;
void ThroughWall(void)
{
	__asm{ // <>0 for yes, =0 for no
		 mov  TW_Mon,eax
		 mov  eax, [eax+0x34] // 3.58
		 cmp  eax, 0x22       // 3.58
		 je   l_exit            // 3.58
		 cmp  eax, 0x23       // 3.58
		 je   l_exit            // 3.58
		 cmp  eax, 0x88       // 3.58
		 je   l_exit            // 3.58
		cmp  eax,0x89 // шарпшутер
		je   l_exit
		cmp  eax,0x95 // стр. башня
		je   l_exit
		cmp  eax,170  // новые шарпшутеры
		je   l_exit
		cmp  eax,171
		je   l_exit
		pusha
		mov    TW_Ret,eax
	}
	STARTNA(__LINE__, 0)
	TW_Ret=NPCNoShootPenalty(TW_Ret,1,TW_Mon); // NPC с луком?
	if(TW_Ret==0) TW_Ret=CrExpBon::NoObstPenalty(TW_Mon);
	STOP
	__asm{
		popa
		mov    eax,TW_Ret
//    jmp   l_exit
	}
//  asm{
//    xor  eax,eax
//  }
l_exit:;
}

void SetUpNewRound(void)
{
	_ECX(SUNR_Mon);
	_ESI(SUNR_BatMan);
	__asm{
		mov    ecx,SUNR_Mon
		mov    eax,0x446E40
		call   eax
	}
	__asm pusha
	_SetUpNewRound();
	__asm popa
}

// -1 к удаче врага
void IsDevil(void)
{
	__asm{
		xor    ecx,ecx
	}
l_next:
	__asm{
		cmp    dword ptr [edx],0x37
		je     l_Ok
		cmp    dword ptr [edx],153
		je     l_Ok
		inc    ecx
		add    edx,4
		cmp    ecx,7
		jl     l_next
		mov    edx,1
		jmp    l_exit
	}
l_Ok: __asm xor    edx,edx
l_exit:;
}
void IsDevil1(void)
{
	__asm{
		cmp    eax,0x36
		je     l_Ok
		cmp    eax,0x37
		je     l_Ok
		cmp    eax,153
		je     l_Ok
	}
l_Ok:;
}
void IsDevil2(void)
{
l_next:
	__asm{
		cmp    dword ptr [ecx],0x37
		je     l_Ok
		cmp    dword ptr [ecx],153
		je     l_Ok
		inc    eax
		add    ecx,4
		cmp    eax,7
		jl     l_next
	}
	return;
l_Ok: __asm xor   eax,eax
}


// + 1 к морале
void IsAngel(void)
{
l_next:
	__asm{
		cmp    dword ptr [ecx],0x0D
		je     l_Ok
		cmp    dword ptr [ecx],150
		je     l_Ok
		inc    eax
		add    ecx,4
		cmp    eax,7
		jl     l_next
		mov    eax,1
	}
	return;
l_Ok:
	__asm xor  eax,eax
}
void IsAngel1(void)
{
l_next:
	__asm{
		cmp    dword ptr [edx],0x0D
		je     l_Ok
		cmp    dword ptr [edx],150
		je     l_Ok
		inc    ecx
		add    edx,4
		cmp    ecx,7
		jl     l_next
		mov    eax,1
	}
	return;
l_Ok:
	__asm xor  eax,eax
}

////////////////////
void DaylyMonstr(void)
{
	STARTNA(__LINE__, 0)
	int     i,j,k,l,day;
	_Hero_ *hr;
	day=GetCurDate();
	if((day%7)==1){ // начало недели
		for(j=0;j<100;j++) MonsterOfWeek[j]=-1;
		j=0;
		for(i=0;i<HERNUM;i++){
			hr=GetHeroStr(i);
			if(hr->Owner==-1) continue; // ничей
			if(HeroHasArt(hr,0x8F)==0) continue;
			for(k=0,l=0;k<7;k++) if(hr->Ct[k]!=-1) ++l;
			if(l==0) continue; // нет монстров вообще
			l=Random(1,l)-1;
			for(k=0;k<7;k++){
				if(hr->Ct[k]==-1) continue;
				if(l==0){ MonsterOfWeek[j++]=hr->Ct[k]; break; }
				else --l;
			}
		}
	}
	STOP
}

static Dword WD_Ecx;
static Word  WD_Accum[4];
static Dword WD_GAccum[4];
void __stdcall WeeklyDwellingGrow(int)
{
	__asm{
		pusha
	}
	STARTNA(__LINE__, 0)
	__asm{
		or    ecx,ecx
		je    l_exit
// Accumulate creatures (before)
		mov   WD_Ecx,ecx
		mov   eax,[ecx+0x14]
		mov   dword ptr [WD_Accum+0],eax
		mov   eax,[ecx+0x18]
		mov   dword ptr [WD_Accum+4],eax
// Guards
		mov   eax,[ecx+0x38]
		mov   [WD_GAccum+0],eax
		mov   eax,[ecx+0x3C]
		mov   [WD_GAccum+4],eax
		mov   eax,[ecx+0x40]
		mov   [WD_GAccum+8],eax
		mov   eax,[ecx+0x44]
		mov   [WD_GAccum+12],eax
		mov   eax,[ecx+0x48]
		mov   [WD_GAccum+16],eax
		mov   eax,[ecx+0x4C]
		mov   [WD_GAccum+20],eax
		mov   eax,[ecx+0x50]
		mov   [WD_GAccum+24],eax
		mov   eax,[ecx+4]
// SG dwelling?
		mov   eax,[ecx+4]
		cmp   eax,173
		je    l_exit
// Call original
		mov   eax,0x4B8760
		push  0
		call  eax
// Accumulate creatures (add that was before)
		lea   eax,PL_WoGOptions//[0][7];PL_DwellAccum
		mov   eax,[eax+7*4]
		or    eax,eax
		je    l_exit1
		mov   ecx,WD_Ecx
// 3.58f Lvl 8 creatures may not be accumulated
		mov   eax,[ecx+4]
		cmp   eax,150
		jb    _MNot8Lvl
		cmp   eax,158
		ja    _MNot8Lvl
		jmp   l_exit1
	}
_MNot8Lvl:
	__asm{
		mov   ax,[WD_Accum+0]
		add   [ecx+0x14],ax
		mov   ax,[WD_Accum+2]
		add   [ecx+0x16],ax
		mov   ax,[WD_Accum+4]
		add   [ecx+0x18],ax
		mov   ax,[WD_Accum+6]
		add   [ecx+0x1A],ax
	}
l_exit1:
// Accumulate guards
	__asm{
		lea   eax,PL_WoGOptions//[0][7];PL_DwellAccum
		mov   eax,[eax+8*4]
		or    eax,eax
		je    l_exit
		mov   ecx,WD_Ecx
		mov   eax,[WD_GAccum+0]
		add   [ecx+0x38],eax
		mov   eax,[WD_GAccum+4]
		add   [ecx+0x3C],eax
		mov   eax,[WD_GAccum+8]
		add   [ecx+0x40],eax
		mov   eax,[WD_GAccum+12]
		add   [ecx+0x44],eax
		mov   eax,[WD_GAccum+16]
		add   [ecx+0x48],eax
		mov   eax,[WD_GAccum+20]
		add   [ecx+0x4C],eax
		mov   eax,[WD_GAccum+24]
		add   [ecx+0x50],eax
	}
l_exit:
	STOP
	__asm{
		popa
	}
}

void DaylySG(int)
{
	STARTNA(__LINE__, 0)
 do{
	_Dwelling_ *gdb=GetDwellingBase();
	if(gdb==0) break;
	int i,gdn=GetDwellingNums();
	for(i=0;i<gdn;i++){
		if(gdb[i].Mon2Hire[0]==173){ // +04 dd*7 = тип монстра для найма (-1=нет)
			if(gdb[i].Owner==-1) continue; // ничей
			if(gdb[i].Num2Hire[0]<10000){ // +14 dw*4 = количество монстров для найма (0=нет)
				gdb[i].Num2Hire[0]+=(Word)MonTable[173].Grow;
			}
		}
	}
 }while(0);
//  3.58
 do{
	int i,j;
	_Hero_ *hp;
	//if(WoG==0) break; // не WoG
	if(!PL_CentElf) break; // Horse Elf disabled
	for(i=0;i<HERNUM;i++){
		hp=GetHeroStr(i);
		if(hp==0) break; //???
		if(IsAI(hp->Owner)==0) continue; // человек
		for(j=0;j<7;j++){
			if(hp->Ct[j]==19) hp->Ct[j]=192; // Grand Elf - Horse Elf
		}
	}
 }while(0);
 STOP
}

////////////////////
static char BFBackGrUDef[256];

int MonInfoDlgPopUpSetting=2;
// 0 = always, 1 = never, 2 = when Ctrl is pressed

int SaveMapMon(void)
{
	STARTNA(__LINE__, 0)
	int i;
	if(Saver("LMMN",4)) RETURN(1)
	if(Saver(MonMapInfo,sizeof(MonMapInfo))) RETURN(1)
	if(Saver(GodMonInfo,sizeof(GodMonInfo))) RETURN(1)
	if(Saver(MonsterOfWeek,sizeof(MonsterOfWeek))) RETURN(1)
	for(i=0;i<MONNUM;i++){
		if(Saver(&MonTable[i].Group,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].SubGroup,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].Flags,sizeof(Dword))) RETURN(1)
		if(Saver(&MonTable[i].CostRes[0],sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].CostRes[1],sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].CostRes[2],sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].CostRes[3],sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].CostRes[4],sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].CostRes[5],sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].CostRes[6],sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].Fight,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].AIvalue,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].Grow,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].HGrow,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].HitPoints,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].Speed,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].Attack,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].Defence,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].DamageL,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].DamageH,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].NShots,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].HasSpell,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].AdvMapL,sizeof(long))) RETURN(1)
		if(Saver(&MonTable[i].AdvMapH,sizeof(long))) RETURN(1)
	}
// +26.02.02 v8
	if(Saver(BFBackGrUDef,sizeof(BFBackGrUDef))) RETURN(1)
	// 3.59
	if(Saver(&MonInfoDlgPopUpSetting,sizeof(MonInfoDlgPopUpSetting))) RETURN(1)
	RETURN(0)
}

static int RMT_RunOnce=0;
void ResetMonTable(void)
{
	STARTNA(__LINE__, 0)
	if(RMT_RunOnce==0){
		Copy((Byte *)MonTable,(Byte *)MonTableBack,sizeof(MonTable));
		RMT_RunOnce=1;
	}
	Copy((Byte *)MonTableBack,(Byte *)MonTable,sizeof(MonTable));
	STOP
}

int LoadMapMon(int ver)
{
	STARTNA(__LINE__, 0)
	int i,MonNum;
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='L'||buf[1]!='M'||buf[2]!='M'||buf[3]!='N')
			{MError("LoadMapMon cannot start loading"); RETURN(1)}
	if(Loader(MonMapInfo,sizeof(MonMapInfo))) RETURN(1)
//__asm int 3 
//MonMapInfo[0].Owner=MonMapInfo[1].Owner;
//MonMapInfo[0].x=MonMapInfo[1].x;
//MonMapInfo[0].y=MonMapInfo[1].y;
//MonMapInfo[0].l=MonMapInfo[1].l;
	if(Loader(GodMonInfo,sizeof(GodMonInfo))) RETURN(1)
	if(Loader(MonsterOfWeek,sizeof(MonsterOfWeek))) RETURN(1)
	ResetMonTable();
//    else if(ver<15) MonNum=(150+9+1+4+4+5+1+9*2);
	MonNum=MONNUM;
	for(i=0;i<MonNum;i++){
			if(Loader(&MonTable[i].Group,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].SubGroup,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].Flags,sizeof(Dword))) RETURN(1)
			if(Loader(&MonTable[i].CostRes[0],sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].CostRes[1],sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].CostRes[2],sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].CostRes[3],sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].CostRes[4],sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].CostRes[5],sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].CostRes[6],sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].Fight,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].AIvalue,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].Grow,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].HGrow,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].HitPoints,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].Speed,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].Attack,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].Defence,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].DamageL,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].DamageH,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].NShots,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].HasSpell,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].AdvMapL,sizeof(long))) RETURN(1)
			if(Loader(&MonTable[i].AdvMapH,sizeof(long))) RETURN(1)
		}
	if(Loader(BFBackGrUDef,sizeof(BFBackGrUDef))) RETURN(1)
	G2B_HrD=0; G2B_HrA=0; // сбросим
	BADistFlag=0;
	if(ver<SAVEWOG359){
		MonInfoDlgPopUpSetting=2;
	}else{
		if(Loader(&MonInfoDlgPopUpSetting,sizeof(MonInfoDlgPopUpSetting))) RETURN(1)
	}
	RETURN(0)
}

void ResetMapMon(void)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<MONONMAPNUM;i++){
		MonMapInfo[i]./*A.*/Owner=0;
		MonMapInfo[i]./*A.*/TakeBack=0;
		MonMapInfo[i].x=0;
		MonMapInfo[i].y=0;
		MonMapInfo[i].l=0;
	}
	for(i=0;i<100;i++){ MonsterOfWeek[i]=-1; }
	ResetMonTable();
	for(i=0;i<GODMONNUM;i++){
		GodMonInfo[i].SubType=0;
		GodMonInfo[i].HeroInd=0;
		GodMonInfo[i].StartDay=0;
		GodMonInfo[i].BonusVal=0;
	}
	for(i=0;i<256;i++) BFBackGrUDef[i]=0;
	G2B_HrD=0; G2B_HrA=0; // сбросим
	BADistFlag=0;
	MonInfoDlgPopUpSetting=2;
	STOP
}

static int BF_Prepare=0;
static int BF_BatFieldNum=-1;
void PrepareBF(void)
{
	Byte *cmpo;
	_ECX(cmpo);
	BF_Prepare=1;
	BF_BatFieldNum=-1;
	__asm pusha
	STARTNA(__LINE__, 0)
	BFCall();
	STOP
	__asm popa
	if(BF_Prepare==1){
		__asm{
			mov   ecx,cmpo
			mov   eax,0x465E70
			call  eax
		}
	}
//  PlaceObstacle(0x54,37);
}
Dword NoCliff(void)
{
	__asm{
		mov    eax,BF_Prepare
		or     eax,eax
		jne    l_cont
		mov    eax,-1
		mov    [esi+0x53A0],eax
	}
l_cont:
	__asm mov    eax,[esi+0x53A0]
	__asm mov    DDummy,eax
	return DDummy;
}
static char *BFBackGr[]={BFBackGrUDef,
/*+ 1*/"CmBkBch.pcx","CmBkDes.pcx","CmBkDrDd.pcx","CmBkDrMt.pcx","CmBkDrTr.pcx",
			 "CmBkGrMt.pcx","CmBkGrTr.pcx","CmBkLava.pcx","CmBkMag.pcx","CmBkSnMt.pcx",
			 "CmBkSnTr.pcx","CmBkSub.pcx","CmBkSwmp.pcx",
/*+14*/"CmBkFF.pcx","CmBkRK.pcx","CmBkMC.pcx","CmBkLP.pcx","CmBkHG.pcx",
			 "CmBkCF.pcx","CmBkEF.pcx","CmBkFW.pcx","CmBkCur.pcx","CmBkRgh.pcx",
/*+24*/"CmBkBoat.pcx","CmBkDeck.pcx"
};
Dword SetBFBack(void)
{
	if(BF_BatFieldNum==-1){
		__asm{
			mov   eax,0x4642B0
			call  eax
		}
//  }else if(BF_BatFieldNum==-2){
	}else{
		__asm{
			lea   edx,BFBackGr
			mov   eax,BF_BatFieldNum
			mov   eax,[4*eax+edx]
		}
	}
	__asm mov  DDummy,eax
	return DDummy;
}

int ERM_BattleField(char Cmd,int Num,_ToDo_*,Mes *Mp)
{
	int i,j,v,t;
	Byte *bh;
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	if(G2B_CompleteAI){
		MError("ERROR! Attempt to use \"!!BF\" in non-human battle (use flag 1000 for checking).");
		RETURN(0)
	}
	switch(Cmd){
		case 'C': // C - очистить поле (не заполнять)
			BF_Prepare=0;
			break;
		case 'O': // Oposition/object - поместить
			CHECK_ParamsMin(2);
			if(Apply(&v,4,Mp,0)){ MError("\"!!BF:O\"-cannot be got or checked."); RETURN(0) }
			if(Apply(&t,4,Mp,1)){ MError("\"!!BF:O\"-cannot be got or checked."); RETURN(0) }
			if(v==-1){ // просто непроходимая клетка
				bh=GetHexStr(t);
				if(bh==0){ MError("\"!!BF:O\"-wrong position number."); RETURN(0) }
				if((bh[0x10]&0x3D)!=0) break; // припятствие
//        if(bh[0x18]!=0xFF) break; // есть живой стек
//        if(bh[0x1C]!=0) break; // есть мертвый стэк
				bh[0x10]|=2;
			}else{
				PlaceObstacle(v,t);
			}
			break;
		case 'M': // M#/#/#/#/#/#/#/#/#/#/#(/#) - биты доступности
			CHECK_ParamsMin(11);
			if(Num>11) t=Mp->n[11]; else t=0x54;
			for(i=0;i<11;i++){
				for(j=0;j<17;j++){
					v=1<<j;
					if(Mp->n[i]&v) PlaceObstacle(t,j+i*17);
				}
			}
			BF_Prepare=0;
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}

int ERM_Battle(char Cmd,int Num,_ToDo_*,Mes *Mp)
{
	int ind,val,slot,x,y,l,v,vv;
	_Hero_   *hp;
	_MonArr_ *ma;
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	switch(Cmd){
		case 'H': // H0...1/$ - герой
			CHECK_ParamsMin(2);
			ind=Mp->n[0];
			if((ind<0)||(ind>1)){ MError("\"!!BA:H\"-side out of range (0,1)."); RETURN(0) }
			if(ind){ hp=G2B_HrD; }else{ hp=G2B_HrA; }
			if(hp!=0) val=hp->Number; else val=-2;
			if(Apply(&val,4,Mp,1)) break;
//      Apply(&val,4,Mp,1);
			if((val>=0)&&(val<HERNUM)) hp=GetHeroStr(val); else hp=0;
			if(ind) G2B_HrD=hp; else G2B_HrA=hp;
			break;
		case 'M': // M0..1/0...6/type/number - монстры
			CHECK_ParamsMin(4);
			ind=Mp->n[0];  if((ind<0)||(ind>1)){ MError("\"!!BA:M\"-side out of range (0,1)."); RETURN(0) }
			slot=Mp->n[1]; if((slot<0)||(slot>6)){ MError("\"!!BA:M\"-slot out of range (0...6)."); RETURN(0) }
			if(ind) ma=G2B_MArrD; else ma=G2B_MArrA;
			if(ma==0){ MError("\"!!BA:M\"-mon pointer=0(internal error)."); RETURN(0) }
			v=0;
			if(Apply(&ma->Ct[slot],4,Mp,2)) v=1;
			if(Apply(&ma->Cn[slot],4,Mp,3)) v=1;
			if(v==1) break;
			if(PL_CrExpEnable){
				int Type;
				CRLOC Crloc;
				CrExpo *cr=0;
				if(BADistFlag){ // Дистанционная атака
//          cr=&ToSendCrExpo[ind][slot];
					cr=&CrExpoSet::BFBody[ind*21+slot];
				}else{
					if(CrExpoSet::FindType(ma,slot,&Type,&Crloc)){
						cr=CrExpoSet::Find(Type,Crloc);
					}
				}
				if(cr!=0){
					cr->Fl.MType=ma->Ct[slot];
					cr->Num=ma->Cn[slot];
					int BIndex=CrExpBon::B2AIndex(ind*7+slot+1);
					if(BIndex!=-1){ 
						CrExpBon::Copy(CrExpBon::Get(-BIndex-1),CrExpBon::Get(ma->Ct[slot]));
						CrExpMod::Copy(CrExpMod::Get(-BIndex-1),CrExpMod::Get(ma->Ct[slot]));
					}
				}
			}
//      if(Num>4){ // experience
//        if(Apply(&expo,4,Mp,4)) break;
//        if(cr!=0)
//      }
			break;
		case 'P': // Px/y/l - позиция
			CHECK_ParamsMin(3);
			MixedPos(&x,&y,&l,G2B_MixedPos);
			Apply(&x,4,Mp,0);
			Apply(&y,4,Mp,1);
			Apply(&l,4,Mp,2);
			G2B_MixedPos=PosMixed(x,y,l);
			break;
		case 'O': // O?ownerA/?ownerD хозяева атаки и защиты
			CHECK_ParamsMin(2);
			hp=G2B_HrA;
			if(hp!=0) val=hp->Owner; else val=-1;
			Apply(&val,4,Mp,0);
			val=G2B_OwnerD;
			Apply(&val,4,Mp,1);
			break;
		case 'E': // E?extern  real MP battle (0=no, 1=HvsH, 2=AIvsH)
			val=0;
			do{
				if(G2B_OwnerD==-1) break; // defender is neutral (AI)
				if(IsAI(G2B_OwnerD)) break; // defender is AI
				if(IsThis(G2B_OwnerD)) break; // defender is a Human at this PC
				hp=G2B_HrA; if(hp!=0) val=hp->Owner; else break; // get attacker owner or break (nonsense)
				if(IsAI(val)) val=2; // Attacker is AI (AIvsH)
				else val=1; // Attacker is Human (HvsH)
			}while(0);
			Apply(&val,4,Mp,0);
			break;
		case 'D': // D# - запретить=1/разрешить=0 битву
			CHECK_ParamsMin(1);
			if(Mp->n[0]!=0) G2B_EnableBattle=0; else G2B_EnableBattle=1;
			break;
		case 'A': // A?# - Полностью автобитва или нет
			CHECK_ParamsMin(1);
			val=G2B_CompleteAI;
			Apply(&val,4,Mp,0);
			break;
		case 'B': // B# - установить бэкграунд
			CHECK_ParamsMin(1);
			v=Mp->n[0];
			if(v==0){ // копируем строку
				StrCopy(BFBackGrUDef,255,ERM2String(&Mp->m.s[Mp->i],0,&vv));
				Mp->i+=vv;
			}else{
				if((v<-1)||(v>25)){ MError("\"!!BA:B\"-wrong BA background index."); RETURN(0) }
			}
			BF_BatFieldNum=v;
			break;
		case 'Q': // Q$ - 3.58 quick battle (1=yes/0=no)
			CHECK_ParamsMin(1);
			val=QuickBattle();
			if(Apply(&val,4,Mp,0)) break;
			SetQuickBattle(val);
			break;
		case 'S': // S?# - осада замка или нет
			CHECK_ParamsMin(1);
			__asm{
				mov   eax,0x699420 // -> combatManager
				mov   eax,[eax]
				mov   eax,[eax+0x132F4]
				mov   val,eax
			}
			Apply(&val,4,Mp,0);
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}

void ChooseMonAttack(void)
{
//  Message("Here you will be able to choose an attack type",1);
	STARTNA(__LINE__, 0)
	int v,sl,n;
	_Hero_ *hp;
	_MonInfo_ *mi;
	Byte *bm,*mon;
// Поддерживается во всех типах игры
//  if(WoG==0) return; // поддерживается только в WoG
	// 3.58f no network battle support
/*
	__asm{ 
		 mov eax,0x69959C
		 mov eax,[eax]; 
		 mov v,eax; 
	}
*/
	if(ERMFlags[996]!=0){
		Message("This feature does not work in Human vs Human network based battle",1);
		RETURNV
	}
	__asm{
		mov    eax,0x699420
		mov    eax,[eax]
		mov    bm,eax
	}
	// текущий герой-хозяин
	v=*(int *)&bm[0x132C0];
	hp=*(_Hero_ **)&bm[0x53CC+v*4];
	if(hp==0) RETURNV // нет героя
	if(IsThis(hp->Owner)==0) RETURNV // не перед экраном
	if((*(char *)&bm[0x13D68])==1) RETURNV // тактика
	if((*(int  *)&bm[0x132C4])==1) RETURNV // автобитва
	sl=M2B_GetMonNum(bm); // номер монстра
	mon=&bm[0x54CC+0x548*sl];
	v=*(int  *)&mon[0x34]; // тип
	mi=&MonTable[v];
	_PopUpRadioButtons PopUpRadioButtons;
	PopUpRadioButtons.DlgLeft = -15;
	PopUpRadioButtons.DlgTop = -15 - 44;
	PopUpRadioButtons.DlgWidth = 300;
	if(mi->Flags&0x4){ // может стрелять
		n=*(int  *)&mon[0xD8]; // число выстрелов
		if(n!=0) CMA_Shoots[sl]=n;
//    PopUpRadioButtons.Caption="Choose an Attack type";
		PopUpRadioButtons.Caption=ITxt(227,0,&Strings);
		if(CrExpBon::IsHarpy(mon)){
			PopUpRadioButtons.Number=3;
//      char* Text[]={"Melee","Melee (with return)","Ranged"};
			char *Text[3];
			Text[0]=ITxt(228,0,&Strings);
			Text[1]=ITxt(229,0,&Strings);
			Text[2]=ITxt(230,0,&Strings);
			PopUpRadioButtons.Text=Text;
			if (!CMA_Harpy[sl])
				PopUpRadioButtons.SelIndex = (*(int  *)&mon[0xD8] == 0 ? 1 : 2);
			ShowRadio(&PopUpRadioButtons);
			CMA_Harpy[sl]=0; // attack and back
			switch(PopUpRadioButtons.SelIndex){
				case 0: // melee
					CMA_Harpy[sl]=1; // attack
				case 1:
//          CMA_Harpy[sl]=0; // attack and back
					if((v>=174)&&(v<=191)){ // Commander
						*(Dword *)&mon[0x84]&=0xFFFFFFFB; // НЕ стреляет
					}else{
						*(int  *)&mon[0xD8]=0; // число выстрелов
					}
					break;
				case 2: // стрельба
					if((v>=174)&&(v<=191)){ // Commander
						*(Dword *)&mon[0x84]|=0x4; // стреляет
					}else{
						*(int  *)&mon[0xD8]=CMA_Shoots[sl]; // число выстрелов
					}
					break;
			}
		}else{
			PopUpRadioButtons.Number=2;
//      char* Text[]={"Melee","Ranged"};
			char *Text[2];
			Text[0]=ITxt(228,0,&Strings);
			Text[1]=ITxt(230,0,&Strings);
			PopUpRadioButtons.Text=Text;
			PopUpRadioButtons.SelIndex = (*(int  *)&mon[0xD8] == 0 ? 0 : 1);
			ShowRadio(&PopUpRadioButtons);
			if(PopUpRadioButtons.SelIndex==0){ // melee
				if((v>=174)&&(v<=191)){ // Commander
					*(Dword *)&mon[0x84]&=0xFFFFFFFB; // НЕ стреляет
				}else{
					*(int  *)&mon[0xD8]=0; // число выстрелов
				}
			}else{
				if((v>=174)&&(v<=191)){ // Commander
					*(Dword *)&mon[0x84]|=0x4; // стреляет
				}else{
					*(int  *)&mon[0xD8]=CMA_Shoots[sl]; // число выстрелов
				}
			}
		}
	}else if((v==0x48)||(v==0x49)||(v==155)||(CrExpBon::IsHarpy(mon))){ // гарпии
//    PopUpRadioButtons.Caption="Choose an Attack type";
		PopUpRadioButtons.Caption=ITxt(227,0,&Strings);
		PopUpRadioButtons.Number=2;
//    char* Text[]={"Attack and Back","Attack and Stay"};
		char *Text[2];
		Text[0]=ITxt(231,0,&Strings);
		Text[1]=ITxt(232,0,&Strings);
		PopUpRadioButtons.Text=Text;
		PopUpRadioButtons.SelIndex = (CMA_Harpy[sl] ? 1 : 0);
		ShowRadio(&PopUpRadioButtons);
		if(PopUpRadioButtons.SelIndex==0){ // Attack and Back
			CMA_Harpy[sl]=0;
		}else{
			CMA_Harpy[sl]=1;
		}
	}else if(((*(int  *)&mon[0xDC])!=0)||(CMA_Shoots[sl]!=0)){ // может колдовать
		n=*(int  *)&mon[0xDC]; // число выстрелов
		if(n!=0) CMA_Shoots[sl]=n;
//    PopUpRadioButtons.Caption="Choose an Attack type";
		PopUpRadioButtons.Caption=ITxt(227,0,&Strings);
		PopUpRadioButtons.Number=2;
//    char* Text[]={"Melee","May Cast Spell"};
		char *Text[2];
		Text[0]=ITxt(228,0,&Strings);
		Text[1]=ITxt(233,0,&Strings);
		PopUpRadioButtons.Text=Text;
		PopUpRadioButtons.SelIndex = (*(int  *)&mon[0xDC] == 0 ? 0 : 1);
		ShowRadio(&PopUpRadioButtons);
		if(PopUpRadioButtons.SelIndex==0){ // melee
//      *(Dword *)&mon[0x84]&=0xFFFFFFFB; // НЕ стреляет
			*(int  *)&mon[0xDC]=0; // число выстрелов
		}else{
//      *(Dword *)&mon[0x84]|=0x4; // стреляет
			*(int  *)&mon[0xDC]=CMA_Shoots[sl]; // число выстрелов
		}
	}
	RETURNV
}
///////////////////////////////////
int __stdcall AIattackNPConly(int p2,int p1)
{
	__asm    mov  eax,p2
	__asm    or   eax,eax
	__asm    jne  l_Ok
	__asm    mov  eax,1
l_Ok:
	__asm{
//    mov    eax,p2
		push   p1
		push   eax
		mov    eax,0x42DD70
		call   eax
		mov    IDummy,eax
	}
	return IDummy;
}

///////////////////////////////////
static int NOIC_hn, NOIC_ret;
int NPCOnlyInCastle(void)
{
	__asm    cmp  eax,-1
	__asm    je   l_Set
	__asm    add  eax,0x1A
	__asm    mov  eax,[eax]
l_Set:
	__asm    mov  NOIC_hn,eax
	__asm    mov  eax,0x449370
	__asm    call eax
	__asm    mov  NOIC_ret,eax
	__asm    pusha
	if(NOIC_ret==0){
		if(NOIC_hn!=-1) NOIC_ret=IsNPCDead(NOIC_hn);
	}
	__asm    popa
//  __asm    mov  eax,NOIC_ret
	return NOIC_ret;
}
int NPCOnlyInCastle2(void)
{
	__asm    sub  eax,0x91
	__asm    add  eax,0x1A
	__asm    mov  eax,[eax]
	__asm    mov  NOIC_hn,eax
	__asm    mov  eax,0x449370
	__asm    call eax
	__asm    mov  NOIC_ret,eax
	__asm    pusha
	if(NOIC_ret==0) NOIC_ret=IsNPCDead(NOIC_hn);
	__asm    popa
//  __asm    mov  eax,NOIC_ret
	return NOIC_ret;
}
///////////////////////////////////
// Multiplayer Support
///////////////////////////////////
void SendMonOfWeek(int *len,Byte **buf)
{
	*len=sizeof(MonsterOfWeek);
	*buf=(Byte *)MonsterOfWeek;
}
void ReceiveMonOfWeek(int len,Byte *buf)
{
	STARTNA(__LINE__, 0)
	int i;
	if(len!=sizeof(MonsterOfWeek)){ TError("Wrong length of sent MonOfWeek vars"); RETURNV };
	for(i=0;i<100;i++) MonsterOfWeek[i]=((int *)buf)[i];
	RETURNV
}
//CrExpo ToSendCrExpo[2][7];
//  memset(CrExpoSet::BFBody,0,sizeof(BFBody));
//  memset(CrExpBon::BFBody,0,sizeof(CrExpBon::BFBody));
//  memset(CrExpMod::BFBody,0,sizeof(CrExpMod::BFBody));
void SendExpo1(int *len,Byte **buf)
{
	STARTNA(__LINE__, 0)
	*len=sizeof(CrExpoSet::BFBody);
	*buf=(Byte *)CrExpoSet::BFBody;
	RETURNV
}
void ReceiveExpo1(int len,Byte *buf)
{
	STARTNA(__LINE__, 0)
//  int i;
	if(len!=sizeof(CrExpoSet::BFBody)){ TError("Wrong length of sent MonExpo1"); RETURNV }
	memcpy(CrExpoSet::BFBody,buf,sizeof(CrExpoSet::BFBody));
	RETURNV
}
void SendExpo2(int *len,Byte **buf)
{
	STARTNA(__LINE__, 0)
	*len=sizeof(CrExpBon::BFBody);
	*buf=(Byte *)CrExpBon::BFBody;
	RETURNV
}
void ReceiveExpo2(int len,Byte *buf)
{
	STARTNA(__LINE__, 0)
//  int i;
	if(len!=sizeof(CrExpBon::BFBody)){ TError("Wrong length of sent MonExpo2"); RETURNV }
	memcpy(CrExpBon::BFBody,buf,sizeof(CrExpBon::BFBody));
	RETURNV
}
void SendExpo3(int *len,Byte **buf)
{
	STARTNA(__LINE__, 0)
	*len=sizeof(CrExpMod::BFBody);
	*buf=(Byte *)CrExpMod::BFBody;
	RETURNV
}
void ReceiveExpo3(int len,Byte *buf)
{
	STARTNA(__LINE__, 0)
//  int i;
	if(len!=sizeof(CrExpMod::BFBody)){ TError("Wrong length of sent MonExpo3"); RETURNV }
	memcpy(CrExpMod::BFBody,buf,sizeof(CrExpMod::BFBody));
	RETURNV
}

CrExpo *CrPo,ArmyExpo[7];
void SendExpo4(int *len,Byte **buf,int Hero)
{
	STARTNA(__LINE__, 0)
	FillMem(ArmyExpo,sizeof(ArmyExpo),0);
	int i;
	if(Hero!=-1){
		for(i=0;i<7;i++){
			CrPo=CrExpoSet::Find(CE_HERO,MAKEHS(Hero,i));    
			if(CrPo!=0) ArmyExpo[i]=*CrPo;
		}
	}
	*len=sizeof(ArmyExpo);
	*buf=(Byte *)ArmyExpo;
	RETURNV
}
void ReceiveExpo4(int len,Byte *buf)
{
	STARTNA(__LINE__, 0)
//  int i;
	if(len!=sizeof(ArmyExpo)){ TError("Wrong length of sent MonExpo4"); RETURNV }
	memcpy(ArmyExpo,buf,sizeof(ArmyExpo));
	for(int i=0;i<7;i++){
		if(ArmyExpo[i].Fl.Act) CrExpoSet::Set(ArmyExpo[i]);
	}
	RETURNV
}

/*
void SendExpo(int *len,Byte **buf)
{
	START("Send Mon Expo")
	int i,Type;
	CRLOC Crloc;
	CrExpo *cr;
	memset(ToSendCrExpo,0,sizeof(ToSendCrExpo));
	do{
		if(G2B_MArrA==0) break;
		for(i=0;i<7;i++){
			if(CrExpoSet::FindType(G2B_MArrA,i,&Type,&Crloc)==0) continue;
			cr=CrExpoSet::Find(Type,Crloc);
			if(cr==0) continue;
			ToSendCrExpo[0][i]=*cr;
		}
	}while(0);
	do{
		if(G2B_MArrD==0) break;
		for(i=0;i<7;i++){
			if(CrExpoSet::FindType(G2B_MArrD,i,&Type,&Crloc)==0) continue;
			cr=CrExpoSet::Find(Type,Crloc);
			if(cr==0) continue;
			ToSendCrExpo[1][i]=*cr;
		}
	}while(0);
	*len=sizeof(ToSendCrExpo);
	*buf=(Byte *)ToSendCrExpo;
	RETURNV
}
void ReceiveExpo(int len,Byte *buf)
{
	START("Receive Mon Expo")
	int i,j;
	if(len!=sizeof(ToSendCrExpo)){ MError("Wrong length of sent MonExpo"); RETURNV }
	memcpy(ToSendCrExpo,buf,sizeof(ToSendCrExpo));
	for(j=0;j<2;j++){
		for(i=0;i<7;i++){
			CrExpoSet::Set(ToSendCrExpo[j][i]);
		}
	}
	RETURNV
}
*/
static _ZPrintf_ MP_Buffer;
int CheckMpWoGVersion(int wver,int ever)
{
	STARTNA(__LINE__, 0)
	if(wver!=WOG_VERSION){
		Zsprintf2(&MP_Buffer ,"WoG(mp): You have incompatible WoG versions: %i (yours) and %i",(Dword)WOG_VERSION,(Dword)wver);
		Message(MP_Buffer.Str,1);
		RETURN(1)
	}
	if(ever!=ERM_VERSION){
		Zsprintf2(&MP_Buffer ,"WoG(mp): You have incompatible ERM versions: %i (yours) and %i",(Dword)ERM_VERSION,(Dword)ever);
		Message(MP_Buffer.Str,1);
		RETURN(1)
	}
	RETURN(0)
}

// 1005: esi+4
// +  0h - Header
// + 14h = dd MixedPos - battle square
// + 18h = db has AHero
// + 19h = db has DHero
// + 1Ah = db has DTown
// +      1Bh = db reserv
// + 1Bh  (1Ch)  = dd ???
// + 1Fh  (20h) = db (dd) ???
// + 20h  (24h) = db ???
// + 21h  (25h) = db ???
// +      26h = dw reserv
// + 22h  (28h) = db (dd) owner of AHero 
// + 23h  (2Ch) = dd gold of owner AHero
// + 27h  (30h) = db (dd) owner of DHero (if present)
// + 28h  (34h) = dd gold of owner DHero (if present)
// + 2Ch  (38h) = _MonArr_ ??? (38h)
// + 64h  (70h) = _MonArr_ ??? (38h)
// +      A8h = dd reserv
// + 9Ch  (ACh) = _Castle_ ??? (185h [168h])
// +221h (214h) = _Hero_ ??? (~446h [492h])
//           +  0 dw = x
//           +  2 dw = y
//           +  4 dw = l
//           +  6 db = Visible
//           +  7 dd = PlMapItem
//           +  B dd = PlOType   
//           +  F db = Pl0Cflag
//           + 10 dd = PlSetUp
//           + 14 db = Sex
//           + 15 db = fl_D
//           + 16 dd = Bio.m.l
//           + 18 db*Bio.m.l = Bio.m.s
//           + XX db = Owner
//           + XX+  1 db = DMorale1
//           + XX+  2 db = DLuck
//           + XX+  3 db = OANum
//           + XX+  4 db = +10Eh ???
//           + XX+  5 db = +112h ???
//           + XX+  6 db = DMorale
//           + XX+  7 db = +10Dh ???
//           + XX+  8 db = +128h ???
//           + XX+  9 db = Number
//           + XX+  A db = Spec
//           + XX+  B db = DMorale1
//           ...
// +~667h (6A6h) = _Hero_ ??? (~446h [492h])
// +~AADh (B38h)
struct _NetHeader_{
	int    SendOwner;
	Dword _un1;
	int    DataType;
	int    DataLen;
	Dword _un2;
};
struct _Net_1005_Str_{
	Dword  VTable;
	_NetHeader_ Header;
	Dword       BattlePosition; // MixedPos
	char        HasAHero;
	char        HasDHero;
	char        HasDTown;
	Byte       _align1;
	Dword      _un2;
	Byte       _un3;
	Byte       _align2[3]; 
	Byte       _un4;
	Byte       _un5;
	Byte       _align3[2]; 
	char        AOwner;
	Byte       _align4[3]; 
	int         AGold;
	char        DOwner;
	Byte       _align5[3]; 
	int         DGold;
	_MonArr_    AMonArr;
	_MonArr_    DMonArr;
	Dword      _align6;
	_CastleSetup_ Town;
	_Hero_      AHero;
	_Hero_      DHero;
} *SP1005;

void SendExpo5(int *len,Byte **buf)
{
	STARTNA(__LINE__, 0)
	FillMem(ArmyExpo,sizeof(ArmyExpo),0);
	int i;
	if(SP1005->HasDTown){ // есть городской гарнизон
		_CastleSetup_ *csp=&SP1005->Town;
		for(i=0;i<7;i++){
			CrPo=CrExpoSet::Find(CE_TOWN,MAKETS(csp->x,csp->y,csp->l,i));    
			if(CrPo!=0) ArmyExpo[i]=*CrPo;
		}
	}
	*len=sizeof(ArmyExpo);
	*buf=(Byte *)ArmyExpo;
	RETURNV
}
void ReceiveExpo5(int len,Byte *buf)
{
	STARTNA(__LINE__, 0)
//  int i;
	if(len!=sizeof(ArmyExpo)){ TError("Wrong length of sent MonExpo5"); RETURNV }
	memcpy(ArmyExpo,buf,sizeof(ArmyExpo));
	for(int i=0;i<7;i++){
		if(ArmyExpo[i].Fl.Act) CrExpoSet::Set(ArmyExpo[i]);
	}
	RETURNV
}

#define FIXEDSIZE 0xB00
static int S2D_Edx,S2D_p2,S2D_p3,S2D_Ecx;
static Byte *S2D_Esi;
static Byte S2D_Buf[101000+FIXEDSIZE];
void Add2Send(void)
{
	STARTNA(__LINE__, 0)
	int i,first,last,len,ind;
	Byte *buf;
	first=*(int *)&S2D_Buf[0x0C];
	last=first;

	*((int *)&S2D_Buf[last])=WOG_VERSION; last+=sizeof(int);
	*((int *)&S2D_Buf[last])=ERM_VERSION; last+=sizeof(int);
	while(1){
		if(SP1005->HasAHero) ind=SP1005->AHero.Number; else ind=-1;
		SendNPC(&len,&buf,ind); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;
		SendExpo4(&len,&buf,ind); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;

		if(SP1005->HasDHero) ind=SP1005->DHero.Number; else ind=-1;
		SendNPC(&len,&buf,ind); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;
		SendExpo4(&len,&buf,ind); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;

		SendExpo5(&len,&buf); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;
		// 3.59
		if(SP1005->HasDTown) ind=SP1005->Town.Number; else ind=-1; // we have a city
		SendCastleState(ind,&len,&buf); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;

		SendERMVars(&len,&buf); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;

		SendCreatures(&len,&buf); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;

		SendMonOfWeek(&len,&buf); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;

		SendExpo1(&len,&buf); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;
		SendExpo2(&len,&buf); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;
		SendExpo3(&len,&buf); if((last+len)>100000) break;
		*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
		for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
		last+=len;

		*((int *)&S2D_Buf[last])=first; last+=4;
		S2D_Buf[last]='Z'; S2D_Buf[last+1]='V'; S2D_Buf[last+2]='S'; last+=3;
		*(int *)&S2D_Buf[0x0C]=last;
		// all went fine
		RETURNV
	}
	// there was an error
	TError("WoG(mp): Too many data to send (>100000)");
	RETURNV
}
void Get4Receive(Byte *Buf)
{
	STARTNA(__LINE__, 0)
	int w,e,len,first,last;
	last=*(int *)&Buf[0x0C];
	if((Buf[last-3]!='Z')||(Buf[last-2]!='V')||(Buf[last-1]!='S')){
		Message("WoG(mp): Data expected but was not sent.\nThis may happen if some of players have SoD or WoG version 3.57 (non multiplayer) or earlier.\n All players should have at least WoG 3.57m version",1);
		RETURNV
	}// не передавали
	first=*(int *)&Buf[last-7];

	w=*(int *)&Buf[first]; first+=sizeof(int);
	e=*(int *)&Buf[first]; first+=sizeof(int);
	if(CheckMpWoGVersion(w,e)) RETURNV
	while(1){
		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveNPC(len,&Buf[first]);
		first+=len; if(first>last) break;

		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveExpo4(len,&Buf[first]);
		first+=len; if(first>last) break;

		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveNPC(len,&Buf[first]);
		first+=len; if(first>last) break;

		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveExpo4(len,&Buf[first]);
		first+=len; if(first>last) break;

		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveExpo5(len,&Buf[first]);
		first+=len;
		if(first>last) break;

		// 3.59
		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveCastleState(len,&Buf[first]);
		first+=len; if(first>last) break;

		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveERMVars(len,&Buf[first]);
		first+=len;
		if(first>last) break;

		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveCreatures(len,&Buf[first]);
		first+=len; if(first>last) break;

		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveMonOfWeek(len,&Buf[first]);
		first+=len;
		if(first>last) break;

		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveExpo1(len,&Buf[first]);
		first+=len; if(first>last) break;
		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveExpo2(len,&Buf[first]);
		first+=len; if(first>last) break;
		len=*(int *)&Buf[first]; first+=sizeof(int);
		ReceiveExpo3(len,&Buf[first]);
		first+=len; if(first>last) break;

		if(first!=(last-7)) break;
		// all is fine
		RETURNV
	}
	// error is found
	Message("WoG(mp): Wrong Transferred data size",1); 
	RETURNV
}

int Prepare2Send(void)
{
	STARTNA(__LINE__, 0)
	int i,len;
	len=((int *)S2D_Ecx)[3];
	for(i=0;i<100000;i++) S2D_Buf[i]=0;
	for(i=0;i<len;i++) S2D_Buf[i]=((Byte *)S2D_Ecx)[i];
	if(((int *)S2D_Ecx)[2]!=1005) RETURN(0)
	SP1005=(_Net_1005_Str_ *)S2D_Esi;
	RETURN(1)
}
int __stdcall Send2Defender(int p2,int p3)
{
	__asm    mov S2D_Ecx,ecx
	__asm    mov S2D_Edx,edx
	__asm    mov S2D_Esi,esi
	__asm    mov eax,p2
	__asm    mov S2D_p2,eax
	__asm    mov eax,p3
	__asm    mov S2D_p3,eax
	__asm    pusha
	STARTNA(__LINE__, 0)
	if(Prepare2Send()){
		TriggerIP(0);
		Add2Send();
	}
	STOP
	__asm    popa
	__asm{
		push   S2D_p3
		push   S2D_p2
		mov    edx,S2D_Edx
		lea    ecx,S2D_Buf
		mov    eax,0x5549E0
		call   eax
		mov    IDummy,eax
	}
	return IDummy;
}

static int R4A_p1,R4A_Ecx;
int __stdcall Receive4Attacker(int p1)
{
	__asm    mov R4A_Ecx,ecx
	__asm    mov eax,p1
	__asm    mov R4A_p1,eax
	__asm    pusha
//  ReceiveNPC((Byte *)R4A_p1);
	STARTNA(__LINE__, 0)
	Get4Receive((Byte *)R4A_p1);
	TriggerIP(1);
	BADistFlag=1; // здесь подчиненный PC
	STOP
	__asm    popa
	__asm{
		push   R4A_p1
		mov    ecx,R4A_Ecx
		mov    eax,0x513010
		call   eax
		mov    IDummy,eax
	}
	return IDummy;
}
// 3.58 - встреча двух героев
/*
void Add2Send358_1(void)
{
	STARTN(__FILENUM__*1000000+__LINE__)
	int i,first,last,len,ind;
	Byte *buf;
	_Hero_ *Hero1,*Hero2;
	first=*(int *)&S2D_Buf[0x0C];
	Hero1=(_Hero_ *)&S2D_Buf[0x2E]; // первый герой в буфере
	Hero2=(_Hero_ *)&S2D_Buf[0x4C0]; // второй герой в буфере
	last=first;

	*((int *)&S2D_Buf[last])=WOG_VERSION; last+=sizeof(int);
	*((int *)&S2D_Buf[last])=ERM_VERSION; last+=sizeof(int);

	ind=Hero1->Number;
	SendNPC(&len,&buf,ind);
	if((last+len)>100000){ MError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
	last+=len;

	ind=Hero2->Number;
	SendNPC(&len,&buf,ind);
	if((last+len)>100000){ MError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
	last+=len;

	SendERMVars(&len,&buf);
	if((last+len)>100000){ MError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
	last+=len;

	SendCreatures(&len,&buf);
	if((last+len)>100000){ MError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
	last+=len;

	SendMonOfWeek(&len,&buf);
	if((last+len)>100000){ MError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
	last+=len;

	SendExpo1(&len,&buf);
	if((last+len)>100000){ MError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
	last+=len;
	SendExpo2(&len,&buf);
	if((last+len)>100000){ MError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
	last+=len;
	SendExpo3(&len,&buf);
	if((last+len)>100000){ MError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
	last+=len;

	*((int *)&S2D_Buf[last])=first; last+=4;
	S2D_Buf[last]='Z'; S2D_Buf[last+1]='V'; S2D_Buf[last+2]='S'; last+=3;
	*(int *)&S2D_Buf[0x0C]=last;
	RETURNV
}
void Get4Receive358_1(Byte *Buf)
{
	STARTN(__FILENUM__*1000000+__LINE__)
	int w,e,len,first,last;
	last=*(int *)&Buf[0x0C];
	if((Buf[last-3]!='Z')||(Buf[last-2]!='V')||(Buf[last-1]!='S')){
		Message("WoG(mp): Data expected but was not sent.\nThis may happen if some of players have SoD or WoG version 3.57 (non multiplayer) or earlier.\n All players should have at least WoG 3.57m version",1);
		RETURNV
	}// не передавали
	first=*(int *)&Buf[last-7];

	w=*(int *)&Buf[first]; first+=sizeof(int);
	e=*(int *)&Buf[first]; first+=sizeof(int);
	if(CheckMpWoGVersion(w,e)) RETURNV

	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveNPC(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }

	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveNPC(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }

	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveERMVars(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }

	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveCreatures(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }

	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveMonOfWeek(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }

	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveExpo1(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }
	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveExpo2(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }
	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveExpo3(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }

	if(first!=(last-7)){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }

	RETURNV
}

int __stdcall Send358_1(int p2,int p3)
{
	__asm    mov S2D_Ecx,ecx
	__asm    mov S2D_Edx,edx
	__asm    mov eax,p2
	__asm    mov S2D_p2,eax
	__asm    mov eax,p3
	__asm    mov S2D_p3,eax
	__asm    pusha
	STARTN(__FILENUM__*1000000+__LINE__)
	if(Prepare2Send()){
		TriggerIP(0);
		Add2Send358_1();
	}
	STOP
	__asm    popa
	__asm{
		push   S2D_p3
		push   S2D_p2
		mov    edx,S2D_Edx
		lea    ecx,S2D_Buf
		mov    eax,0x5549E0
		call   eax
		mov    IDummy,eax
	}
	return IDummy;
}

static Dword R358_Esi;
int __stdcall Receive358_1(void) // 40624D
{
	__asm    mov R358_Esi,esi
	__asm    pusha
	STARTN(__FILENUM__*1000000+__LINE__)
	Get4Receive358_1((Byte *)R358_Esi);
	TriggerIP(1);
//????
//  BADistFlag=1; // здесь подчиненный PC
	STOP
	__asm    popa
	__asm{
		shl    eax,6
		add    eax,ecx
		mov    IDummy,eax
	}
	return IDummy;
}
*/
///////////////////////////////
#define FIXEDSIZE2 0x3C
static Byte D2D_Buf[101000+FIXEDSIZE2];
void Add2Defender(int HeroNum)
{
	STARTNA(__LINE__, 0)
	int i,first,last,len;
	Byte *buf;
	first=*(int *)&D2D_Buf[0x0C];
	last=first;

	if((HeroNum<0)||(HeroNum>HERNUM)) { TError("WoG(mp): Incorrect Hero Number"); RETURNV }

	*((int *)&D2D_Buf[last])=WOG_VERSION; last+=sizeof(int);
	*((int *)&D2D_Buf[last])=ERM_VERSION; last+=sizeof(int);

	SendNPC(&len,&buf,HeroNum);
	if((last+len)>100000){ TError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&D2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) D2D_Buf[last+i]=buf[i];
	last+=len;

	SendERMVars(&len,&buf);
	if((last+len)>100000){ TError("WoG(mp): Too many data to send (>100000)"); RETURNV }
	*((int *)&D2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) D2D_Buf[last+i]=buf[i];
	last+=len;
/*
	SendCreatures(&len,&buf);
	if((last+len)>10000){ MError("Too many data to send (>10000)"); return; }
	*((int *)&S2D_Buf[last])=len; last+=sizeof(int);
	for(i=0;i<len;i++) S2D_Buf[last+i]=buf[i];
	last+=len;
*/
	*((int *)&D2D_Buf[last])=first; last+=4;
	D2D_Buf[last]='Z'; D2D_Buf[last+1]='V'; D2D_Buf[last+2]='S'; last+=3;
	*(int *)&D2D_Buf[0x0C]=last;
	RETURNV
}
void Get4Defender(Byte *Buf)
{
	STARTNA(__LINE__, 0)
	int w,e,len,first,last;
	last=*(int *)&Buf[0x0C];
	if((Buf[last-3]!='Z')||(Buf[last-2]!='V')||(Buf[last-1]!='S')){
		Message("WoG(mp): Data expected but was not sent.\nThis may happen if some of players have SoD or WoG version 3.57 (non multiplayer) or earlier.\n All players should have at least WoG 3.57m version",1);
		RETURNV
	}// не передавали
	first=*(int *)&Buf[last-7];

	w=*(int *)&Buf[first]; first+=sizeof(int);
	e=*(int *)&Buf[first]; first+=sizeof(int);
	if(CheckMpWoGVersion(w,e)) RETURNV

	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveNPC(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }

	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveERMVars(len,&Buf[first]);
	first+=len;
	if(first>last){ Message("WoG(mp): Wrong Transferred data size",1); RETURNV }
/*
	len=*(int *)&Buf[first]; first+=sizeof(int);
	ReceiveCreatures(len,&Buf[first]);
	first+=len;
	if(first!=(last-7)){ Message("WoG: Wrong Transferred data size",1); return; }
*/
	RETURNV
}

static int D2D_Edx,D2D_p2,D2D_p3,D2D_Ecx;
int Check2Def2(void)
{
	STARTNA(__LINE__, 0)
	int i,len;
	len=((int *)D2D_Ecx)[3];
	for(i=0;i<100000;i++) D2D_Buf[i]=0;
	for(i=0;i<len;i++) D2D_Buf[i]=((Byte *)D2D_Ecx)[i];
	if(((int *)D2D_Ecx)[2]!=1011) RETURN(0)
	RETURN(1)
}
int Prepare2Defender(void)
{
	STARTNA(__LINE__, 0)
//  int i,len;
//  len=((int *)D2D_Ecx)[3];
//  for(i=0;i<100000;i++) D2D_Buf[i]=0;
//  for(i=0;i<len;i++) D2D_Buf[i]=((Byte *)D2D_Ecx)[i];
	RETURN(((int *)D2D_Ecx)[5]) // номер героя
}
int __stdcall Send4Defender(int p2,int p3)
{
	__asm    mov D2D_Ecx,ecx
	__asm    mov D2D_Edx,edx
	__asm    mov eax,p2
	__asm    mov D2D_p2,eax
	__asm    mov eax,p3
	__asm    mov D2D_p3,eax
	__asm    pusha
	STARTNA(__LINE__, 0)
	if(Check2Def2()){
		TriggerIP(2);
		Add2Defender(Prepare2Defender());
	}
	STOP
	__asm    popa
	__asm{
		push   D2D_p3
		push   D2D_p2
		mov    edx,D2D_Edx
		lea    ecx,D2D_Buf
		mov    eax,0x5549E0
		call   eax
		mov    IDummy,eax
	}
	return IDummy;
}

static int   R4D_p1,R4D_p2;
static Byte *R4D_Buf;
int R4D_Check4Ours(void)
{
	STARTNA(__LINE__, 0)
	Dword *buf=(Dword *)R4D_Buf;
	if(R4D_Buf==0) RETURN(0)
	if(buf[2]!=1011) RETURN(0)
	RETURN(1)
}
// передача скилов героя победителя на текущий PC (атака на удаленный PC)
int __stdcall Receive4DefenderAfter(int p2,int p1)
{
	__asm{
		mov    eax,p1
		mov    R4D_p1,eax
		mov    eax,p2
		mov    R4D_p2,eax
		push   R4D_p1
		push   R4D_p2
		mov    eax,0x553440
		call   eax
		mov    R4D_Buf,eax
	}
	__asm    pusha
	STARTNA(__LINE__, 0)
	if(R4D_Check4Ours()){
		Get4Defender(R4D_Buf);
		TriggerIP(3);
	}
	STOP
	__asm    popa
	__asm    mov   IDummy,eax
	return IDummy;
}

int R4D_Check4Ours2(void)
{
	STARTNA(__LINE__, 0)
	Dword *buf=(Dword *)R4D_Buf;
	if(R4D_Buf==0) RETURN(0)
	if(buf[2]!=1005) RETURN(0)
	RETURN(1)
}
int __stdcall Receive4DefenderAfter2(int p2,int p1)
{
	__asm{
		mov    eax,p1
		mov    R4D_p1,eax
		mov    eax,p2
		mov    R4D_p2,eax
		push   R4D_p1
		push   R4D_p2
		mov    eax,0x553440
		call   eax
		mov    R4D_Buf,eax
	}
	__asm    pusha
	STARTNA(__LINE__, 0)
	if(R4D_Check4Ours2()){
		Get4Receive(R4D_Buf);
		TriggerIP(3);
	}
	STOP
	__asm    popa
	__asm    mov   IDummy,eax
	return IDummy;
}

static _Hero_ *FHSSS_Hp;
void AdjustSSShow(void)
{
	STARTNA(__LINE__, 0)
	int  i;
	Byte j;
	for(i=0;i<28;i++) FHSSS_Hp->SShow[i]=0;
	j=1;
	for(i=0;i<28;i++){ if(FHSSS_Hp->SSkill[i]==3){ FHSSS_Hp->SShow[i]=j; j++; }}
	for(i=0;i<28;i++){ if(FHSSS_Hp->SSkill[i]==2){ FHSSS_Hp->SShow[i]=j; j++; }}
	for(i=0;i<28;i++){ if(FHSSS_Hp->SSkill[i]==1){ FHSSS_Hp->SShow[i]=j; j++; }}
	RETURNV
}
void FixHeroSecScillsShow(void)
{
	__asm  pusha
	__asm  mov   [eax+0x101],edx
	_EAX(FHSSS_Hp);
	AdjustSSShow();
	__asm  popa
}

///////////////////////
static Dword SE_Ecx;
static int   SE_BColor,SE_Color,SE_x,SE_y,SE_dx,SE_dy;
static char *SE_Buf,*SE_Font;
static char  SE_MyBuf[1024];
void _ShowExpo(int Expo,int Limit,int HasArt)
{
	STARTNA(__LINE__, 0)
	StrCopy(SE_MyBuf,1023,Exp2String(Expo,Limit,HasArt));
//  StrCopy(SE_MyBuf,1023,"{\xB7}\xB6\xB5\xB4\xB3\x0A");
	StrCanc(SE_MyBuf,1023,SE_MyBuf,SE_Buf);
//  SE_y-=20;
	SE_y-=16;
	SE_dy+=16;
	SE_Buf=SE_MyBuf;
//  SE_Font="ZVerd10b.fnt";
	RETURNV
}
static _MonArr_ *SE_MArr=0;
static int SE_Stack=0,
					 SE_SideBF=0/*уст. ТОЛЬКО для поля битвы*/,
					 SE_StackBF=0/*уст. ТОЛЬКО для поля битвы*/,
					 SE_Exp=0,SE_Limit=EXPOMAX2,
					 SE_HasArt=0;
static Byte *SE_Mon=0;
static int SE_MonWOExpo=-1;/*уст только для монстров без экспы и не сохраняемые*/

int _ExpDetailedInfo(void){
	STARTNA(__LINE__, 0)
	CrExpo *cr=0;
	if(/*WoG &&*/ PL_CrExpEnable){
	 if(SE_Mon){
		 CrExpBon::ShowInfo(SE_Mon);
		 RETURN(1)
	 }else if(SE_MonWOExpo!=-1){
			int t=SE_MonWOExpo;
			if(t>=174 && t<=191)/* ShowNPC(&DlgNPC)*/;
			else CrExpBon::ShowInfo(t,1,0,0);
	 }else{
		if(SE_MArr==0){ SE_Exp=0; SE_Limit=EXPOMAX2; }
		else{
			int Type;
			CRLOC Crloc;
/*
			if(BADistFlag){ // Дистанционная атака
//        cr=&ToSendCrExpo[SE_Side][SE_Stack];
				cr=&CrExpoSet::BFBody[SE_Side*21+SE_Stack];
				if(cr->Num==0) SE_Exp=0;
				else SE_Exp=cr->Expo;
				SE_Limit=CrExpMod::CrLimit(cr);
			}else{
*/
				if(CrExpoSet::FindType(SE_MArr,SE_Stack,&Type,&Crloc)){
					cr=CrExpoSet::Find(Type,Crloc);
					if(cr==0) SE_Exp=0;
					else{
						if(cr->Num==0) SE_Exp=0;
						else SE_Exp=cr->Expo;
					}
					SE_Limit=CrExpMod::CrLimit(cr);
				}else{ SE_Exp=0; SE_Limit=EXPOMAX2; }
//      }
			int t=SE_MArr->Ct[SE_Stack],n=SE_MArr->Cn[SE_Stack];
			if(t>=174 && t<=191)/* ShowNPC(&DlgNPC)*/;
			else CrExpBon::ShowInfo(t,n,SE_Exp,cr);
		}
//    FillMem(SE_MyBuf,15,' '); SE_MyBuf[15]=0;
//    itoa(SE_Exp,SE_MyBuf,10); SE_MyBuf[strlen(SE_MyBuf)]=' ';
//    itoa(SE_Exp*100/SE_Limit/EXPOMUL,&SE_MyBuf[10],10); SE_MyBuf[strlen(SE_MyBuf)]=' '; SE_MyBuf[13]='%';
//    Message(SE_MyBuf,1);
		RETURN(1)
	 }
	}
	RETURN(0)
}
//005F4C5D?8B4608         mov    eax,[esi+08]
//005F4C60 8BCB           mov    ecx,ebx
//005F4C62 50             push   eax - тип поля, куда кликнули
//005F4C63 E8F8FDFFFF     call   H3WOG.005F4A60 - возвр -1, если нечего выводить
static int EDI_Ecx,EDI_Ret;
int __stdcall ExpDetailedInfo(int Field){
	__asm mov EDI_Ecx,ecx
	__asm pusha
	STARTNA(__LINE__, 0)
	do{
		EDI_Ret=-1;
		if((Field==0xC9 || Field==0xCC) && _ExpDetailedInfo()) break;
		__asm{
			mov    ecx,EDI_Ecx
			mov    eax,Field
			push   eax
			mov    eax,0x5F4A60
			call   eax
			mov    EDI_Ret,eax
		}
	}while(0);
	STOP
	__asm popa
	return EDI_Ret;
}
/*
static Dword EDI_Ecx,EDI_Edx;
void pascal ExpDetailedInfo(int,int,int,int,int,int,int,int,int,int)
{
	asm mov  EDI_Ecx,ecx
	asm mov  EDI_Edx,edx
	asm pusha
asm int 3
	if(_ExpDetailedInfo()==0){
		asm{
			push  0
			push -1
			push  0
			push -1
			push  0
			push -1
			push  0
			push -1
			push -1
			push -1
			mov  ecx,EDI_Ecx
			mov  edx,EDI_Edx
			mov  eax,0x4F6C00
			call eax
		}
	}
	asm popa
}
*/
int __stdcall ShowExpo(int x,int y,int dx,int dy,
											 char *Buf,char *Font,int Color,
											 int par4,int par3,int BColor,int par1)
{
	_ECX(SE_Ecx);
	STARTNA(__LINE__, 0)
	SE_BColor=BColor;
	SE_Color=Color;
	SE_Buf=Buf;
	SE_Font=Font;
	SE_x=x;
	SE_y=y;
	SE_dx=dx;
	SE_dy=dy;
	if(/*WoG &&*/ PL_CrExpEnable){
		CrExpo *cr;
//    if(SE_MArr==0){ SE_Exp=0; SE_Limit=EXPOMAX2; }
//    else{
			int Type;
			CRLOC Crloc;
//      if(BADistFlag){ // Дистанционная атака
//        cr=&ToSendCrExpo[SE_Side][SE_Stack];
			if(CrExpBon::IsBattle){
//        cr=&CrExpoSet::BFBody[SE_Side*21+SE_Stack];
//        CrExpBon::MakeCur(SE_Side*21+SE_Stack,Mon);
				cr=CrExpoSet::GetBF(SE_StackBF); 
//        if(cr->Num==0) SE_Exp=0;
//        else SE_Exp=cr->Expo;
				SE_Exp=cr->Expo;
//        SE_HasArt=cr->HasArt();
				if(cr->HasArt()){ SE_HasArt=cr->ArtCopy()+1; }
				else SE_HasArt=0;
				SE_Limit=CrExpMod::CrLimit(cr);
			}else{
				if(SE_MArr==0){ SE_Exp=0; SE_Limit=EXPOMAX2; SE_HasArt=0; }
				else{
					if(CrExpoSet::FindType(SE_MArr,SE_Stack,&Type,&Crloc)){
						cr=CrExpoSet::Find(Type,Crloc);
						if(cr==0){ 
							SE_Exp=0;
							SE_HasArt=0;
						}else{
							if(cr->Num==0) SE_Exp=0;
							else SE_Exp=cr->Expo;
							if(cr->HasArt()){ SE_HasArt=cr->ArtCopy()+1; }
							else SE_HasArt=0;
						}
						SE_Limit=CrExpMod::CrLimit(cr);
					}else{ SE_Exp=0; SE_Limit=EXPOMAX2;  SE_HasArt=0; }
				}
			}
		}
		_ShowExpo(SE_Exp,SE_Limit,SE_HasArt);
//  }
	__asm{
		push  par1
		push  SE_BColor
		push  par3
		push  par4
		push  SE_Color
		push  SE_Font
		push  SE_Buf
		push  SE_dy
		push  SE_dx
		push  SE_y
		push  SE_x
		mov   ecx,SE_Ecx
		mov   eax,0x5Bc6A0
		call  eax
		mov   IDummy,eax
	}
	RETURN(IDummy)
}

// вызов диалога монстра (по левому и правому клику)
static Dword SEP_Ecx;
int __stdcall ShowExpoPrep(_MonArr_ *MArr,int Stack,_Hero_ *Hp,Dword p7,Dword p6,Dword p5,Dword p4,Dword p3,Dword p2,Dword p1){
	__asm mov SEP_Ecx,ecx
	STARTNA(__LINE__, 0)
	__asm pusha
	SE_MArr=MArr; SE_Stack=Stack; SE_StackBF=0;
	SE_Mon=0; // not a BF
	SE_MonWOExpo=-1;
	__asm popa
	__asm{ 
		push   p1
		push   p2
		push   p3
		push   p4
		push   p5
		push   p6
		push   p7
		push   Hp
		push   Stack
		push   MArr
		mov    ecx,SEP_Ecx
		mov    eax,0x5F3EF0
		call   eax
		mov    IDummy,eax
	}
	RETURN(IDummy)
}
// окно с инфой для монстров без хозяина и опыта
static Dword SEPU_Ecx;
int __stdcall ShowExpoPrepUnknown(int MType,Dword p3,Dword p2,Dword p1)
{
	__asm mov SEPU_Ecx,ecx
	STARTNA(__LINE__, 0)
	__asm pusha
	SE_Mon=0; // not a BF
	SE_MonWOExpo=MType;
	__asm popa
	__asm{
		push   p1
		push   p2
		push   p3
		push   MType
		mov    ecx,SEPU_Ecx
		mov    eax,0x5F45B0
		call   eax
		mov    IDummy,eax
	}
	RETURN(IDummy)
}
// окно с инфой о монстре в битве
static Byte *SEPB_Mon,*SEPB_BatMan;
static Dword SEPB_Ecx;
int __stdcall ShowExpoPrepBattle(Dword p4,Dword p3,Dword p2,Dword p1){
	__asm mov SEPB_Ecx,ecx
	STARTNA(__LINE__, 0)
	__asm pusha
	__asm mov eax,0x699420
	__asm mov eax,[eax]
	__asm mov SEPB_BatMan,eax
		SEPB_Mon=(Byte *)p4;
		SE_Mon=SEPB_Mon;
		SE_MonWOExpo=-1;
//    _Hero_   *hp;
//asm int 3
		SE_Stack=*(int *)&SEPB_Mon[0x5C]; // номер стэка в армии героя
		SE_StackBF=MonIndex(SEPB_Mon);
		SE_SideBF=*(int *)&SEPB_Mon[0xF4]; // сторона
		if(SE_Stack==-1){ SE_MArr=0; SE_Stack=0; }
		else{
			int side=*(int *)&SEPB_Mon[0xF4]; // сторона
//      if(side==0) hp=(_Hero_ *)*(Dword *)&SEPB_BatMan[0x53CC];
//      else        hp=(_Hero_ *)*(Dword *)&SEPB_BatMan[0x53D0];
//      if(hp==0){ // монстры
				SE_MArr=(_MonArr_ *)*(Dword *)&SEPB_BatMan[0x54C4+side*4];
//      }else{     // герой
//        SE_MArr=(_MonArr_ *)*(Dword *)&SEPB_BatMan[0x53C4+side*4];
//      }
		}
	__asm popa
	__asm{ 
		push   p1
		push   p2
		push   p3
		push   p4
		mov    ecx,SEPB_Ecx
		mov    eax,0x5F3700
		call   eax
		mov    IDummy,eax
	}
	RETURN(IDummy)
}
///////////////////////////////////
// картинка в окне героя
static Dword  SNM_Eax,SNM_Ecx,SNM_Edx;
void _ShowNumberMid(void)
{
	STARTNA(__LINE__, 0)
	int     StNum=-1,hv,hi,Expo=0,Limit=EXPOMAX2;
	char   *Text=(char *)SNM_Eax;
	Word   *Info=(Word *)SNM_Ecx;
	_Hero_ *hp;
	_CastleSetup_ *cp;
	CrExpo *cr=0;
	switch(Info[12]){
// экран героя
		case 0x0F: // стэк 1
			if(Info[8]!=0x3D) break;
			StNum=0; break;
		case 0x51: // стэк 2
			if(Info[8]!=0x3E) break;
			StNum=1; break;
		case 0x93: // стэк 3
			if(Info[8]!=0x3F) break;
			StNum=2; break;
		case 0xD5: // стэк 4
			if(Info[8]!=0x40) break;
			StNum=3; break;
		case 0x117: // стэк 5
			if(Info[8]!=0x41) break;
			StNum=4; break;
		case 0x159: // стэк 6
			if(Info[8]!=0x42) break;
			StNum=5; break;
		case 0x19B: // стэк 7
			if(Info[8]!=0x43) break;
			StNum=6; break;
// экран города
		case 0x131:
			if(Info[8]==0x6C) StNum=7;  // внутри
			if(Info[8]==0x85) StNum=14; // визитер
			break;
		case 0x16F:
			if(Info[8]==0x6D) StNum=8;
			if(Info[8]==0x86) StNum=15;
			break;
		case 0x1AD:
			if(Info[8]==0x6E) StNum=9;
			if(Info[8]==0x87) StNum=16;
			break;
		case 0x1EB:
			if(Info[8]==0x6F) StNum=10;
			if(Info[8]==0x88) StNum=17;
			break;
		case 0x229:
			if(Info[8]==0x70) StNum=11;
			if(Info[8]==0x89) StNum=18;
			break;
		case 0x267:
			if(Info[8]==0x71) StNum=12;
			if(Info[8]==0x8A) StNum=19;
			break;
		case 0x2A5:
			if(Info[8]==0x72) StNum=13;
			if(Info[8]==0x8B) StNum=20;
			break;
// гарнизоны и шахты
// не герой: 6C,5C,0AF,14
		case 0x5C: // стэк 1
			if(Info[8]==0x6C) StNum=28;  // внутри
			if(Info[8]==0x85) StNum=21; // визитер
			break;
		case 0x9A: // стэк 2
			if(Info[8]==0x6D) StNum=29;
			if(Info[8]==0x86) StNum=22;
			break;
		case 0xD8: // стэк 3
			if(Info[8]==0x6E) StNum=30;
			if(Info[8]==0x87) StNum=23;
			break;
		case 0x116: // стэк 4
			if(Info[8]==0x6F) StNum=31;
			if(Info[8]==0x88) StNum=24;
			break;
		case 0x154: // стэк 5
			if(Info[8]==0x70) StNum=32;
			if(Info[8]==0x89) StNum=25;
			break;
		case 0x192: // стэк 6
			if(Info[8]==0x71) StNum=33;
			if(Info[8]==0x8A) StNum=26;
			break;
		case 0x1D0: // стэк 7
			if(Info[8]==0x72) StNum=34;
			if(Info[8]==0x8B) StNum=27;
			break;
	}
	if(StNum==-1) RETURNV // нет стэка
	if(StNum<7){ // экран героя
		__asm{
			mov  eax,0x698B70
			mov  eax,[eax]
			mov  hp,eax
		}
		if(hp==0) RETURNV // нет героя
		Info[13]=(Word)(0x0216-16);
		Info[15]=(Word)(0x12+16);
		cr=CrExpoSet::Find(CE_HERO,MAKEHS(hp->Number,StNum));
		if(cr==0) Expo=0;
		else{
			cr->RecalcExp2RealNum(hp->Cn[StNum],hp->Ct[StNum]);
			if(cr->Num==0) Expo=0;
			else Expo=cr->Expo;
			Limit=CrExpMod::CrLimit(cr);
		}
	}
	if((StNum>6)&&(StNum<21)){ // экран города внутри
		__asm{
			mov  eax,0x69954C
			mov  eax,[eax]
			mov  eax,[eax+0x38]
			mov  cp,eax
		}
		if(cp==0) RETURNV // нет города
	}
	if((StNum>6)&&(StNum<14)){ // экран города охранник
		Info[13]=(Word)(0x01B4-16);
		Info[15]=(Word)(0x14+16);
		hi=cp->IHero;
		if(hi==-1){ // город
			cr=CrExpoSet::Find(CE_TOWN,MAKETS(cp->x,cp->y,cp->l,StNum-7));
		}else{ //герой
			cr=CrExpoSet::Find(CE_HERO,MAKEHS(hi,StNum-7));
		}
		if(cr==0) Expo=0;
		else{
			if(hi==-1) cr->RecalcExp2RealNum(cp->GuardsN[StNum-7],cp->GuardsT[StNum-7]);
			else       cr->RecalcExp2RealNum(GetHeroStr(hi)->Cn[StNum-7],GetHeroStr(hi)->Ct[StNum-7]);
			if(cr->Num==0) Expo=0;
			else Expo=cr->Expo;
			Limit=CrExpMod::CrLimit(cr);
		}
	}
	if((StNum>13)&&(StNum<21)){ // экран города визитер
		Info[13]=(Word)(0x0214-16);
		Info[15]=(Word)(0x14+16);
		hv=cp->VHero;
		if(hv==-1){ // город
			cr=CrExpoSet::Find(CE_TOWN,MAKETS(cp->x,cp->y,cp->l,StNum-14));
		}else{ //герой
			cr=CrExpoSet::Find(CE_HERO,MAKEHS(hv,StNum-14));
		}
		if(cr==0) Expo=0;
		else{
			if(hv==-1) cr->RecalcExp2RealNum(-1,-1);
			else       cr->RecalcExp2RealNum(GetHeroStr(hv)->Cn[StNum-14],GetHeroStr(hv)->Ct[StNum-14]);
			if(cr->Num==0) Expo=0;
			else Expo=cr->Expo;
			Limit=CrExpMod::CrLimit(cr);
		}
	}
	if((StNum>20)&&(StNum<28)){ // гарнизон и шахты - визитер
		hp=HeroEntered2Object;
		if(hp==0) RETURNV // нет героя
		Info[13]=(Word)(0x010F-16);
		Info[15]=(Word)(0x14+16);
		cr=CrExpoSet::Find(CE_HERO,MAKEHS(hp->Number,StNum-21));
		if(cr==0) Expo=0;
		else{
			cr->RecalcExp2RealNum(hp->Cn[StNum-21],hp->Ct[StNum-21]);
			if(cr->Num==0) Expo=0;
			else Expo=cr->Expo;
			Limit=CrExpMod::CrLimit(cr);
		}
	}
	if((StNum>27)&&(StNum<35)){ // гарнизон и шахты - хозяин
		_MonArr_ *ma;
		hp=HeroEntered2Object;
		if(hp==0) RETURNV // нет героя
		int Found=0;
		do{
			_Mine_ *mn; mn=GetMineBase(); if(mn==0) goto MChecked; // нет шахт
			int cn=GetMineNum(); //
			for(int i=0;i<cn;i++,mn++){
				if(mn->xe!=hp->x) continue;
				if(mn->ye!=hp->y) continue;
				if(mn->le!=hp->l) continue;
				cr=CrExpoSet::Find(CE_MINE,MAKEMS(mn->xe,mn->ye,mn->le,StNum-28));
				ma=(_MonArr_ *)mn->GType;
				Found=1;
				goto AllChecked;
			}
		}while(0);
MChecked:
		do{
			_Horn_ *hn; hn=GetHornBase(); if(hn==0) goto HChecked; // нет шахт
			int cn=GetHornNum(); //
			for(int i=0;i<cn;i++,hn++){
				if(hn->ex!=hp->x) continue;
				if(hn->ey!=hp->y) continue;
				if(hn->el!=hp->l) continue;
				cr=CrExpoSet::Find(CE_HORN,MAKEZS(hn->ex,hn->ey,hn->el,StNum-28));
				ma=(_MonArr_ *)hn->GType;
				Found=1;
				goto AllChecked;
			}
		}while(0);
HChecked:
AllChecked:
		if(Found==0) RETURNV
		Info[13]=(Word)(0x0AF-16);
		Info[15]=(Word)(0x14+16);
		if(cr==0) Expo=0;
		else{
			cr->RecalcExp2RealNum(ma->Cn[StNum-28],ma->Ct[StNum-28]);
			if(cr->Num==0) Expo=0;
			else Expo=cr->Expo;
			Limit=CrExpMod::CrLimit(cr);
		}
	}
//  if(cr!=0) cr->RecalcExp2RealNum(-1);
	if(cr!=0){ 
		if(cr->HasArt()){
			SE_HasArt=cr->ArtCopy()+1; 
		}else SE_HasArt=0;
	}else SE_HasArt=0;
	StrCopy(SE_MyBuf,1023,Exp2String(Expo,Limit,SE_HasArt));
	StrCanc(SE_MyBuf,1023,SE_MyBuf,Text);
	SNM_Eax=(Dword)SE_MyBuf;
	RETURNV
}
void __stdcall ShowNumberMid(void)
{
	__asm{
		mov    eax,[eax][0x18]
		mov    edx,[ecx]
		mov    SNM_Eax,eax
		mov    SNM_Ecx,ecx
		mov    SNM_Edx,edx
	}
#if (WOG_VERSION==999)
#else
/////////////////
//PL_CrExpEnable=1;
/////////////////
	if(PL_CrExpEnable){
		_ShowNumberMid();
	}  
#endif
	__asm{
		mov    eax,SNM_Eax
		mov    ecx,SNM_Ecx
		mov    edx,SNM_Edx
		push   eax
		call   [edx][0x34]
	}
}
// перемещение монстров из стэка в стэк
static CrExpo SH_CrS, SH_CrD;
static int SH_Bnums, SH_Bnumd;
static Dword SH_p1,SH_p2,SH_p3;
char  *SH_Man;
static void SH_PrepIt(void){
	STARTNA(__LINE__, 0)
	int Left,Right;
	int Exp[2],Arts[2];
	__asm{
		mov  eax,0x6938C8
		mov  eax,[eax]
		mov  SH_Man,eax
	}
	if(SH_Man==0) RETURNV
	Left =*(int *)&SH_Man[0x70];
	Right=*(int *)&SH_Man[0x74];
//  SE_MyBuf[0]='!'; SE_MyBuf[1]=0;
	if(Left>SH_Bnums){
//    Exp[0]=(SH_CrS.Exp+SH_CrD.Exp*(Left-SH_Bnums)/SH_CrD.Num)/Left;
		Exp[0]=(SH_CrS.Expo*SH_Bnums+SH_CrD.Expo*(Left-SH_Bnums))/Left;
		if(SH_CrD.Num==0) Exp[1]=0;
		else Exp[1]=SH_CrD.Expo;
	}else{
		Exp[0]=SH_CrS.Expo;
		if(Right==0) Exp[1]=0;
//    else Exp[1]=(SH_CrD.Exp+SH_CrS.Exp*(Right-SH_Bnumd)/SH_CrS.Num)/Right;
		else Exp[1]=(SH_CrD.Expo*SH_Bnumd+SH_CrS.Expo*(Right-SH_Bnumd))/Right;
	}
	if(SH_CrS.HasArt()){ Arts[0]=SH_CrS.ArtCopy()+1; }else Arts[0]=0;
	if(SH_CrD.HasArt()){ Arts[1]=SH_CrD.ArtCopy()+1; }else Arts[1]=0;
	StrCopy(SE_MyBuf,1023,Exp2String2(Exp,CrExpMod::CrLimit(&SH_CrS),Arts));
	RETURNV
}
void _ShowHint(void)
{
	STARTNA(__LINE__, 0)
	SH_PrepIt();
	__asm{
		lea   eax,SE_MyBuf
		push  eax
		push  8
		push  3
		push  0x200
		mov   ecx,SH_Man
		mov   eax,0x5FF400
		call  eax
	}
	STOP
}

Dword ShowHint(Dword p1,Dword p2,Dword p3)
{
	SH_p1=p1; SH_p2=p2; SH_p3=p3;
	_ShowHint();
	__asm{
		push p3
		push p2
		push p1
		mov  eax,0x6179DE
		call eax
		add  esp,0xC
		mov  DDummy,eax
	}
	return DDummy;
}
void __stdcall ShowHint1(Dword,Dword,Dword,char *Text)
{
	Dword LEcx;
	_ECX(LEcx);
#if (WOG_VERSION==999)
#else
	if(Text[0]==0){
		SH_PrepIt();
		Text=SE_MyBuf;
//    SE_MyBuf[0]='!'; SE_MyBuf[1]=0;
//    StrCopy(SE_MyBuf,1023,"{\xB7}\xB6\xB5\xB4\xB3  <=>  {\xB7}\xB6\xB5\xB4\xB3");
	}
#endif
	__asm{
		mov   ecx,LEcx
		mov   eax,Text
		push  eax
		push  8
		push  3
		push  0x200
		mov   eax,0x5FF400
		call  eax
	}
}
// eax,???,ecx,stack...
void __stdcall SmoothPassInternalFun(int SrcStack,_MonArr_ *MonArrD,int DstStack,int P1,int P2)
{
	_MonArr_ *MonArrS; _ECX(MonArrS);
	STARTNA(__LINE__, 0)
	int i;
	int is,id,ts,td,ms,md,hs,hd;
	int xs,ys,ls,xd,yd,ld;
	int exps,expd,nums,numd;
	int   types=0,typed=0;
	CRLOC crlocs=0,crlocd=0;
	_Hero_ *hp;
	CrExpo *cr;
	is=id=ts=td=ms=md=hs=hd=-1;
	for(i=0;i<HERNUM;i++){
		hp=GetHeroStr(i);
		if(((_MonArr_ *)hp->Ct)==MonArrS) is=i;
		if(((_MonArr_ *)hp->Ct)==MonArrD) id=i;
	}
	if((is==-1)||(id==-1)){ // должен быть город
		_CastleSetup_ *cst,*cst0;
		cst=cst0=GetCastleBase();
		if(cst==0) goto CDone; // нет замков
		int cn=GetCastleNum(); //
		for(i=0;i<cn;i++,cst++){
			if((_MonArr_ *)cst->GuardsT==MonArrS) ts=i;
			if((_MonArr_ *)cst->GuardsT==MonArrD) td=i;
		}
		if(ts!=-1){ xs=cst0[ts].x; ys=cst0[ts].y; ls=cst0[ts].l; }
		if(td!=-1){ xd=cst0[td].x; yd=cst0[td].y; ld=cst0[td].l; }
	}
CDone:
	if(((is+ts)==-2)||((id+td)==-2)){ // должен быть шахта
		_Mine_ *cst,*cst0;
		cst=cst0=GetMineBase();
		if(cst==0) goto MDone; // нет замков
		int cn=GetMineNum(); //
		for(i=0;i<cn;i++,cst++){
			if((_MonArr_ *)cst->GType==MonArrS) ms=i;
			if((_MonArr_ *)cst->GType==MonArrD) md=i;
		}
		if(ms!=-1){ xs=cst0[ms].xe; ys=cst0[ms].ye; ls=cst0[ms].le; }
		if(md!=-1){ xd=cst0[md].xe; yd=cst0[md].ye; ld=cst0[md].le; }
	}
MDone:
	if(((is+ts+ms)==-3)||((id+td+md)==-3)){ // должен быть горн
		_Horn_ *cst,*cst0;
		cst=cst0=GetHornBase();
		if(cst==0) goto HDone; // нет замков
		int cn=GetHornNum(); //
		for(i=0;i<cn;i++,cst++){
			if((_MonArr_ *)cst->GType==MonArrS) hs=i;
			if((_MonArr_ *)cst->GType==MonArrD) hd=i;
		}
		if(hs!=-1){ xs=cst0[hs].ex; ys=cst0[hs].ey; ls=cst0[hs].el; }
		if(hd!=-1){ xd=cst0[hd].ex; yd=cst0[hd].ey; ld=cst0[hd].el; }
	}
HDone:
	if(is!=-1){ // источник-герой
		types=CE_HERO; crlocs=MAKEHS(is,SrcStack);
	}else if(ts!=-1){ // источник-город
		types=CE_TOWN; crlocs=MAKETS(xs,ys,ls,SrcStack);
	}else if(ms!=-1){ // источник-шахта
		types=CE_MINE; crlocs=MAKEMS(xs,ys,ls,SrcStack);
	}else if(hs!=-1){ // источник-горн
		types=CE_HORN; crlocs=MAKEZS(xs,ys,ls,SrcStack);
	}
	if(id!=-1){ // приемник-герой
		typed=CE_HERO; crlocd=MAKEHS(id,DstStack);
	}else if(td!=-1){ // приемник-город
		typed=CE_TOWN; crlocd=MAKETS(xd,yd,ld,DstStack);
	}else if(md!=-1){ // источник-шахта
		typed=CE_MINE; crlocd=MAKEMS(xd,yd,ld,DstStack);
	}else if(hd!=-1){ // источник-горн
		typed=CE_HORN; crlocd=MAKEZS(xd,yd,ld,DstStack);
	}
	SH_Bnums=MonArrS->Cn[SrcStack];
	SH_Bnumd=MonArrD->Cn[DstStack];
	cr=CrExpoSet::Find(types,crlocs);
	if(cr && (MonArrS->Ct[SrcStack]!=-1) && SH_Bnums>0) SH_CrS=*cr; else SH_CrS.SetN(types,crlocs,MonArrS->Ct[SrcStack],SH_Bnums,0);
	cr=CrExpoSet::Find(typed,crlocd);
	if(cr && (MonArrD->Ct[DstStack]!=-1) && SH_Bnumd>0) SH_CrD=*cr; else SH_CrD.SetN(typed,crlocd,/*MonArrD->Ct[DstStack]*/MonArrS->Ct[SrcStack],SH_Bnumd,0);
	SH_CrS.RecalcExp2RealNum(SH_Bnums,MonArrS->Ct[SrcStack]);
	SH_CrD.RecalcExp2RealNum(SH_Bnumd,MonArrS->Ct[SrcStack]);
	__asm{
		push  P2
		push  P1
		push  DstStack
		push  MonArrD
		push  SrcStack
		mov   ecx,MonArrS
		mov   eax,0x449B60
		call  eax
	}
	nums=MonArrS->Cn[SrcStack];
	numd=MonArrD->Cn[DstStack];
	if(nums==0){ 
		exps=0;
		if(SH_CrS.HasArt()){ SH_CrD.AddArt(SH_CrS); }
	}else if(nums>SH_Bnums){
//    exps=SH_CrS.Exp+SH_CrD.Exp*(nums-SH_Bnums)/SH_CrD.Num;
		exps=(SH_CrS.Expo*SH_Bnums+SH_CrD.Expo*(nums-SH_Bnums))/nums;
	}else{
//    exps=SH_CrS.Exp*nums/SH_CrS.Num;
		exps=SH_CrS.Expo;
	}
	if(numd==0){ 
		expd=0;
		if(SH_CrD.HasArt()){ SH_CrS.AddArt(SH_CrD); }
	}else if(numd>SH_Bnumd){
//    expd=SH_CrD.Exp+SH_CrS.Exp*(numd-SH_Bnumd)/SH_CrS.Num;
		expd=(SH_CrD.Expo*SH_Bnumd+SH_CrS.Expo*(numd-SH_Bnumd))/numd;
	}else{
//    expd=SH_CrD.Exp*numd/SH_CrD.Num;
		expd=SH_CrD.Expo;
	}
	SH_CrS.Expo=exps; SH_CrS.Num=nums;
	if(SH_CrS.Fl.Type!=0) CrExpoSet::Set(SH_CrS);
	SH_CrD.Expo=expd; SH_CrD.Num=numd;
	if(SH_CrD.Fl.Type!=0) CrExpoSet::Set(SH_CrD);
	RETURNV
}
/////////////////////
static _Hero_ *MSHA_Eax;
static Dword MSHA_Edx,MSHA_Edi;
void MoveStackInHeroArmy(void)
{
	__asm{
		mov   MSHA_Eax,eax
		mov   MSHA_Edx,edx
		mov   MSHA_Edi,edi
	}
	__asm pusha
	M2M_SStr=(_MonArr_ *)MSHA_Eax->Ct; M2M_DStr=(_MonArr_ *)MSHA_Eax->Ct;
	M2M_SSlot=MSHA_Edx; M2M_DSlot=MSHA_Edi;
	//if(WoG){
		if(PL_CrExpEnable){
			M2M_ret=M2MCrExp(0);
			if(M2M_ret) return;
		}
	//}
	__asm popa
	__asm{
		mov    edx,[eax+4*edx+0x0000AD]
		lea    edi,[eax+4*edi+0x0000AD]
		mov    eax,[edi]
		add    eax,edx
		mov    [edi],eax
	}
}
void __stdcall MoveStackInHeroArmy2(/*int,int,_MonArr_ *MonArrS,*/int StackD,_MonArr_ *MonArrD,int StackS)
{
	_MonArr_ *MonArrS; _ECX(MonArrS);
	__asm pusha
	M2M_SStr=MonArrS; M2M_DStr=MonArrD;
	M2M_SSlot=StackS; M2M_DSlot=StackD;
	//if(WoG){
		if(PL_CrExpEnable){
			M2M_ret=M2MCrExp(0);
			if(M2M_ret) return;
		}
	//}
	__asm popa
	__asm{
		push   edx
		push   eax
		push   edi
		mov    ecx,eax
		mov    eax,0x44AA30
		call   eax
	}
}
///////////////////
// Upgrade stack in HillFort
// :004E86E3 8984B991000000 mov    [ecx+4*edi+00000091],eax
static Dword UH_Ecx,UH_Edi,UH_Ebx,UH_Eax;
static void _UpgrHill(void){
	STARTNA(__LINE__, 0)
	_Hero_ *hp=(_Hero_ *)UH_Ecx;
	int     st=(int)UH_Edi;
	int     nm=(int)UH_Eax;
	CrExpo *cr;
	cr=CrExpoSet::Find(CE_HERO,MAKEHS(hp->Number,st));
	if(cr==0) RETURNV
	if((int)cr->Fl.MType!=hp->Ct[st]) RETURNV
	cr->Fl.MType=(Word)nm; // new type
	cr->RecalcExp2RealNum(-1,-1);
	cr->Expo*=CrExpMod::UpgrMul(hp->Ct[st]); //recalc experience
	RETURNV
}
void UpgrHill(void){
	__asm mov  UH_Edi,edi
	__asm mov  UH_Ecx,ecx
	__asm mov  UH_Eax,eax
	__asm pusha
	_UpgrHill();
	__asm popa
	__asm mov [ecx+4*edi+0x91],eax
}
// Upgrade in creature info screen
// :004C6BD9 891C88         mov    [eax+4*ecx],ebx
void _UpgrInd(void){
	STARTNA(__LINE__, 0)
	Dword *po=(Dword *)UH_Eax;
	_MonArr_ *mar=(_MonArr_ *)po[0];
	_Hero_   *hp;
	int       st=(int)po[1];
	int       nm=(int)UH_Ebx;
	int       i;
	CrExpo   *cr=0;

	for(i=0;i<HERNUM;i++){
		hp=GetHeroStr(i);
		if(((_MonArr_ *)hp->Ct)==mar){
			cr=CrExpoSet::Find(CE_HERO,MAKEHS(hp->Number,st));
			if(cr==0) RETURNV
			break;
		}
	}
	if(cr==0){
		_CastleSetup_ *cst;
		cst=GetCastleBase();
		if(cst==0) RETURNV // нет замков
		int cn=GetCastleNum(); //
		for(i=0;i<cn;i++,cst++){
			if((_MonArr_ *)cst->GuardsT==mar){
				cr=CrExpoSet::Find(CE_TOWN,MAKETS(cst->x,cst->y,cst->l,st));
				if(cr==0) RETURNV
				break;
			}
		}
	}
	if(cr==0) RETURNV
	if((int)cr->Fl.MType!=mar->Ct[st]) RETURNV
	cr->Fl.MType=(Word)nm; // new type
	cr->RecalcExp2RealNum(-1,-1);
	cr->Expo*=CrExpMod::UpgrMul(mar->Ct[st]); //recalc experience
	RETURNV
}
void UpgrInd(void){
	__asm mov  UH_Ebx,ebx
//  asm mov  UH_Ecx,ecx
	__asm mov  UH_Eax,eax
	__asm pusha
	_UpgrInd();
	__asm popa
	__asm mov    ecx,[eax+4]
	__asm mov    eax,[eax]
	__asm mov    [eax+4*ecx],ebx
//  asm test   esi,esi // IMPORTANT !!! it is checked in SoD code
}
///////////////////
// Universal monster feature receiver
//
static int   MF_Trigger=0;
static int   MF_Dam, MF_Cor;
static int   MF_Enable, MF_Ret;
static Byte *MF_Mon;
static int   MF_RetAddr;
//static int   MF_Exp;
int ERM_MonFeature(char Cmd,int/*Num*/,_ToDo_* /*sp*/,Mes *Mp) //!!MR
{
	int   v,j;
	Byte *bm;
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	if(MF_Trigger==0){
		MError("ERROR! Attempt to call \"!!MF\" not in !?MF section.");
		RETURN(0)
	}
	__asm{
		mov    eax,0x699420
		mov    eax,[eax]
		mov    bm,eax
	}
	switch(Cmd){
		case 'N': // номер монстра (0...41)
			if(MF_Mon==0){
				MError("\"!!MF:N\": not a monster structure (internal)");
			}
//      v=M2B_GetMonNum(MF_Mon);
			v=-1;
			for(j=0;j<(21*2);j++){
				// 3.58 Tower skip
				if(j==20) continue; // 3.58
				if(j==41) continue; // 3.58
				if(&bm[0x54CC+0x548*j]==MF_Mon){
					v=j;
					break;
				}
			}
			Apply(&v,4,Mp,0);
			break;
		case 'D': // начальный урон
			Apply(&MF_Dam,4,Mp,0); break;
		case 'F': // конечный урон
			Apply(&MF_Cor,4,Mp,0); break;
		case 'E': // разрешить акцию или нет
			Apply(&MF_Enable,4,Mp,0); break;
		case 'W': // кто ударяет
			v=0;
			if(MF_RetAddr==0x469A98) v=1;
			if(MF_RetAddr==0x465964) v=2;
			Apply(&v,4,Mp,0); 
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

///////////////////
// blocking ability
// entries phisical:
// 43F95B,43FA5E,43FD3D,4400DF,440858,440E70,441048,44124C,441739,44178A
// entries ???:
// 46595F,469A93
// entries magical:
// 59FF34,5A009F,5A0D02,5A0DA8,5A0E6B,5A0EDB,5A1065,5A1262,5A1D17,5A4DED,5A4FA8,5A6886
// call:
// push  damage
// mov   ecx,-> mon struct
// call  0x443DB0 (pascal)
// returns number of killed monsters
void _ApplyDamage(void)
{
	STARTNA(__LINE__, 0)
	MF_Cor=MF_Dam;
	MF_Trigger=1; MF_Enable=1;
	MFCall(1); // call !?MF1; - Block ability
	MF_Trigger=0;
	STOP
}

int __stdcall ApplyDamage(int dmg)
{
	__asm mov  MF_Mon,ecx
	__asm mov  eax,dmg
	__asm mov  MF_Dam,eax
	__asm mov  eax,[ebp+4]
	__asm mov  MF_RetAddr,eax
	__asm pusha
	STARTNA(__LINE__, 0)
	_ApplyDamage();
	if(MF_Enable){
		if(CommanderBlock(MF_Mon,*(int *)&MF_Mon[0x34],MF_Dam)){
			MF_Ret=0;
		}else if(CrExpBon::StackBlock(MF_Mon,*(int *)&MF_Mon[0x34]==196?20:0)){
			MF_Ret=0;
		}else{
			MF_Cor=CrExpBon::StackBlockPartial(MF_Mon,MF_Cor);
			__asm{
				mov   eax,MF_Cor
				mov   ecx,MF_Mon
				push  eax
				mov   eax,0x443DB0
				call  eax
				mov   MF_Ret,eax
			}
		}
	}else{ // disabled
		MF_Ret=0;
	}
	STOP
	__asm popa
//  __asm mov  eax,MF_Ret
	return MF_Ret;
}

///////////////////
//10 = 00 01 02 03 04 05 06 07 08 09
//20 = 0A 0B 0C 0D 0E 0F 10 11 11 11
//30 = 11 11 11 11 11 12 11 13 14 14
//40 = 15 11 11 16 11 11 11 17 11 11
//50 = 11 11 11 11 11 11 11 07 11 18
//60 = 11 11 11 19 1A 1B 1C 1D 1E 1F
//70 = 20 21 22 11 11 22 25 25 23 25
//80 = 24

// 0x5A0664 = Quicksand
// 0x5A0846 =
// 0x5A0A69 = 0x5A0B31
//x04 - 0x5A0C8D 0x5A0C9A 0x5A0D40 0x5A0E1C
//x08 - 0x5A0EA9 0x5A0F19 0x5A0F35 0x5A0F4C
//x0C - 0x5A0F63 0x5A0F7A 0x5A0F91 0x5A118E
//x10 - 0x5A137E 0x5A138F 0x5A18DE 0x5A1AF6
//x14 - 0x5A1C17 0x5A1CAC 0x5A1503 0x5A1D63
//x18 - 0x5A2090 0x5A1E5D 0x5A1F14 0x5A202B
//x1C - 0x5A203C 0x5A2051 0x5A2066 0x5A207B
//x20 - 0x5A1401 0x5A18A0 0x5A18D7 0x5A1A8E
//x24 - 0x5A2180 0x5A2368

// стоимость заклов для героя в зависимости от монстров, что он имеет
//004E554C 6A14           push   00000014
//004E554E 8BCF           mov    ecx,edi
//004E5550 E8FB52F6FF     call   H3WOG.0044A850
//004E5555 84C0           test   al,al
//004E5557 750D           jne    H3WOG.004E5566 (004E5566)
//004E5559 6A15           push   00000015
//004E555B 8BCF           mov    ecx,edi
//004E555D E8EE52F6FF     call   H3WOG.0044A850
//004E5562 84C0           test   al,al
//004E5564 7403           je     H3WOG.004E5569 (004E5569)
//004E5566 83C602         add    esi,00000002
//004E5569 8DBB91000000   lea    edi,[ebx+00000091]
//004E556F?6A22           push   00000022
//004E5571 8BCF           mov    ecx,edi
//004E5573 E8D852F6FF     call   H3WOG.0044A850
//004E5578 84C0           test   al,al
//004E557A 750D           jne    H3WOG.004E5589 (004E5589)
//004E557C 6A23           push   00000023
//004E557E 8BCF           mov    ecx,edi
//004E5580 E8CB52F6FF     call   H3WOG.0044A850
//004E5585 84C0           test   al,al
//004E5587 7403           je     H3WOG.004E558C (004E558C)
//004E5589 83EE02         sub    esi,00000002
int __stdcall CheckSpellCostReduction(int Cost,_Hero_ *Hp)
{
	STARTNA(__LINE__, 0)
	int i,tp,nm,CostSub=0,val;
	if(Hp==0) RETURN(Cost)
	if(G2B_CompleteAI){ // to disable AI thinking
		if(IsThis(CurrentUser())==0) RETURN(Cost)
	}
	_MonArr_ *MArr=(_MonArr_ *)Hp->Ct;
	if(MArr==0) RETURN(Cost)
	for(i=0;i<7;i++){
		tp=MArr->Ct[i];
		nm=MArr->Cn[i];
		if(tp==-1) continue;
		if(nm==0) continue;
		if(/*WoG==0 ||*/ PL_CrExpEnable==0){
			if(tp==34 || tp==35) CostSub=2;
			continue;
		}
		val=0;
		if(tp==34 || tp==35) val=2;
		val=CrExpBon::StackSubSpCost(Hp,i,val);
		if(val>CostSub) CostSub=val;
	}
	RETURN(Cost-CostSub)
//  int i,CostSub=0,CostAdd=0,val;
//  Byte *bm;
//  if(WoG==0 || PL_CrExpEnable==0){
/*
	asm{
		mov    eax,0x699420
		mov    eax,[eax]
		mov    bm,eax
	}
	for(i=0;i<(21*2);i++){
		if(i==20) continue; // 3.58
		if(i==41) continue; // 3.58
		mon=&bm[0x54CC+0x548*i];
		int tp=*(int *)&mon[0x34];
		int num=*(int *)&mon[0x4C];
		int side=*(int *)&mon[0xF4]; // сторона
		_MonArr_ *MArr=(_MonArr_ *)*(Dword *)&bm[0x54C4+side*4];
//    int stack=*(int *)&Mon[0x5C]; // номер стэка
		if(num==0) continue;
		if(tp==-1) continue;
//    if(stack==-1) continue;
		if(MArr==DArr){ // caster side
			if(WoG==0 || PL_CrExpEnable==0){
				if(tp==34 || tp==35) CostSub=2;
				continue;
			}
			val=0;
			if(tp==34 || tp==35) val=2;
			val=CrExpBon::StackSubSpCost(Byte *Mon,val);
			if(val>CostSub) CostSub=val;
		}else{
			if(WoG==0 || PL_CrExpEnable==0){
				if(tp==20 || tp==21) CostAdd=2;
				continue;
			}
			val=0;
			if(tp==20 || tp==21) val=2;
			val=CrExpBon::StackAddSpCost(Byte *Mon,val);
			if(val>CostAdd) CostAdd=val;
		}
	}
	Cost=Cost+CostAdd-CostSub;
*/
}

// модификация бонуса защиты при выборе защиты
static Byte *DB_Mon;
static int   DB_Def;
void _DefenseBonus(void)
{
	STARTNA(__LINE__, 0)
	DB_Def=CrExpBon::DefenceBonus(DB_Mon,DB_Def);
	STOP
}
void DefenseBonus(void)
{
	__asm mov   DB_Mon,ebx
	__asm mov   DB_Def,ecx
	__asm pusha
	_DefenseBonus();
	__asm popa
	__asm mov   ecx,DB_Def
	__asm mov   [ebx+0x4DC],ecx
}

//004435A3 8B4634         mov    eax,[esi+34]
//004435A6 83E843         sub    eax,00000043
//004435A9 0F8418010000   je     H3WOG.004436C7
static Byte *DB1_Mon;
static int   DB1_Ret,DB1_Chance,DB1_RetC;
void _DeathBlowChance(void)
{
	STARTNA(__LINE__, 0)
	int chance=0;
	if(DB1_Mon==0) RETURNV
	if(*(int *)&DB1_Mon[0x34]==0x43) chance=20;
	DB1_Chance=CrExpBon::DeathBlow(DB1_Mon,chance);
	RETURNV
}
void _DeathBlow(void)
{
	STARTNA(__LINE__, 0)
	_DeathBlowChance();
	if(DB1_Chance>0) DB1_Ret=0x43;
	RETURNV
}
void _DeathBlowChanceRnd(void)
{
	STARTNA(__LINE__, 0)
	_DeathBlowChance();
	if(DB1_RetC>DB1_Chance) DB1_RetC=99; else DB1_RetC=1;
	RETURNV
}
__declspec( naked ) int DeathBlow(void)
{
	__asm mov DB1_Mon,esi
	__asm mov eax,[esi+0x34]
	__asm mov DB1_Ret,eax
	__asm pusha
	_DeathBlow();
	__asm popa
	__asm mov eax,DB1_Ret
	__asm sub eax,0x43 // MUST BE THE LAST COMMAND !!!!!!!!!!!!!!!
//  __asm mov IDummy,eax
//  return IDummy;
	__asm ret
}
int DeathBlowChance(void)
{
	__asm mov   DB1_Mon,esi
	__asm mov   eax,0x50C7C0
	__asm call  eax
	__asm mov   DB1_RetC,eax
	__asm pusha
	_DeathBlowChanceRnd();
	__asm popa
//  __asm mov   eax,DB1_RetC
	return DB1_RetC;
}

// Ненависть друг к другу
//00443195 8B4734         mov    eax,[edi+34]
//00443198 83C1F4         add    ecx,FFFFFFF4
//0044319B 83F947         cmp    ecx,00000047
//0044319E 0F876C020000   ja     H3WOG.00443410
//004431A4 33D2           xor    edx,edx
//004431A6 8A910C354400   mov    dl,[ecx+0044350C]
//004431AC FF2495F0344400 jmp    [4*edx+004434F0]
// Табл. Переходов
// /*12*/ 0,0,6,6,6,6,6,6,
// /*20*/ 6,6,6,6,6,6,6,6,6,6,
// /*30*/ 6,6,6,6,6,6,1,1,6,6,
// /*40*/ 6,2,6,6,6,6,6,6,6,6,
// /*50*/ 6,6,3,3,4,4,6,6,6,6,
// /*60*/ 6,6,6,6,6,6,6,6,6,6,
// /*70*/ 6,6,6,6,6,6,6,6,6,6,
// /*80*/ 6,6,6,5
// Адреса Переходов
//       dd offset loc_004431B3
//       dd offset loc_004431E3
//       dd offset loc_004431F8
//       dd offset loc_004431D3
//       dd offset loc_004431C3
//       dd offset loc_004431F3
//       dd offset loc_00443410

static Byte *PH_MonA,*PH_MonD;
static int   PH_Ret,PH_Dmg;
static int _PersonalHate(void)
{
	STARTNA(__LINE__, 0)
	int mult=0;
	int amtype=*(int *)&PH_MonA[0x34];
	int dmtype=*(int *)&PH_MonD[0x34];
	switch(amtype){
		case 12: // Angel
		case 13: // Archangel
			if(dmtype==54 || dmtype==55) mult=50; break;
		case 36: // Genie
		case 37: // Master Genie
			if(dmtype==52 || dmtype==53) mult=50; break;
		case 41: // Titan
			if(dmtype==83) mult=5; break;
		case 52: // Efreeti
		case 53: // Efreet Sultan
			if(dmtype==36 || dmtype==37) mult=50; break;
		case 54: // Devil
		case 55: // Arch Devil
			if(dmtype==12 || dmtype==13) mult=50; break;
		case 83: // Black Dragon
			if(dmtype==41) mult=5; break;
	}
	mult=CrExpBon::PersonalHate(PH_MonA,dmtype,mult);
	RETURN(PH_Dmg*mult/100.0)
}
int PersonalHate(void)
{
	__asm mov PH_MonA,ebx
	__asm mov PH_MonD,edi
	__asm mov PH_Dmg,ecx
	__asm pusha
	PH_Ret=_PersonalHate();
	__asm popa
//  __asm mov eax,PH_Ret
	return PH_Ret;
}

////////////////////
// SoD bug fix (set level creatures for random hero)
static _Hero_ *SURH_Hp;
void _SetUpRandomCreatures(void)
{
	STARTNA(__LINE__, 0)
	if(SURH_Hp==0) RETURNV
	_MonArr_ *mp=(_MonArr_ *)SURH_Hp->Ct;
	for(int i=0;i<7;i++){
		if(mp->Ct[i]<-1){
			if(mp->Cn[i]==0){ mp->Ct[i]=-1; continue; }
//      int lvl=-mp->Ct[i]-2;
//      int up=lvl%2;
//      lvl=lvl/2;
			mp->Ct[i]=-mp->Ct[i]-2+((SURH_Hp->Spec/2)*14);
		}
	}
	RETURNV
}
Byte SetUpRandomCreatures(void)
{
	__asm pusha
	__asm mov SURH_Hp,esi
	_SetUpRandomCreatures();
	__asm popa
	__asm mov al,[ebx+0x31C]
	__asm mov BDummy,al
	return BDummy;
}

int HasDragoHeartArt(Byte *Mon)
{
	STARTNA(__LINE__, 0)
	int side=*(int *)&Mon[0xF4]; // side 0,1
	Byte *cm;
	__asm{
		mov   eax,0x699420
		mov   eax,[eax]
		mov   cm,eax
	}
	_Hero_ *hp=(_Hero_ *)(((Dword *)&cm[0x53CC])[side]);
	if(hp==0) RETURN(0)
	if(HeroHasArt(hp,/*7*/159)==0) RETURN(0)
	Dword fl=*(Dword *)&Mon[0x84];
	if(fl&0x80000000) RETURN(1) // dragon
//
	RETURN(0)
}

static Byte *MM_Mon;
static int   MM_Ret;

// Всегда положительная мораль
int MinotourMoral(void)
{
	__asm mov   MM_Mon,edi
	__asm pusha
	STARTNA(__LINE__, 0)
	int montp=*(int *)&MM_Mon[0x34];
	if((montp==0x4E)||(montp==0x4F)) MM_Ret=0x4F;
	else{
		if(HasDragoHeartArt(MM_Mon)) MM_Ret=0x4F;
		else MM_Ret=CrExpBon::MinotourMoral(MM_Mon);
	}
	STOP
	__asm popa
//  __asm mov   eax,MM_Ret
	return MM_Ret;
}

static Byte *SAT_Mon;
static char  SAT_Ret;
char __stdcall ShootAnyTime(int Art)
{
	__asm mov   SAT_Mon,esi
	__asm push  Art
	__asm mov   eax,0x4D9460
	__asm call  eax
	__asm mov   SAT_Ret,al
	__asm pusha
	if(SAT_Ret==0) SAT_Ret=(char)CrExpBon::ShootAnyTime(SAT_Mon);
	__asm popa
//  __asm mov   al,SAT_Ret
	return SAT_Ret;
}
char __stdcall ShootAnyTime2(int Art)
{
	__asm mov   SAT_Mon,edi
	__asm push  Art
	__asm mov   eax,0x4D9460
	__asm call  eax
	__asm mov   SAT_Ret,al
	__asm pusha
	if(SAT_Ret==0) SAT_Ret=(char)CrExpBon::ShootAnyTime(SAT_Mon);
	__asm popa
//  __asm mov   al,SAT_Ret
	return SAT_Ret;
}

////////////////
/*
static Byte *CPAS_AMon,*CPAS_DMon;
void _CastPostAttackSpell(void)
{
	CrExpBon::CastMassSpell2(CPAS_AMon,CPAS_DMon);
}

void CastPostAttackSpell(void)
{
	asm mov   CPAS_AMon,esi
	asm mov   CPAS_DMon,edi
	asm pusha
	_CastPostAttackSpell();
	asm popa
	asm mov fs:[0],ecx
}
*/


//Магическая Аура
static Byte *UA_Mon;
static int   UA_Ret;
int UnicornAura(void)
{
	__asm mov   UA_Mon,esi
	__asm pusha
	STARTNA(__LINE__, 0)
	if(*(int *)&UA_Mon[0x34]==0x18) UA_Ret=0x19;
	else{
		UA_Ret=CrExpBon::UnicornAura(UA_Mon);
	}
	STOP
	__asm popa
//  __asm mov   eax,UA_Ret
	return UA_Ret;
}
int UnicornAura2(void)
{
	__asm mov   UA_Mon,edi
	__asm pusha
	STARTNA(__LINE__, 0)
	if(*(int *)&UA_Mon[0x34]==0x18) UA_Ret=0x19;
	else{
		UA_Ret=CrExpBon::UnicornAura(UA_Mon);
	}
	STOP
	__asm popa
//  __asm mov   eax,UA_Ret
	return UA_Ret;
}

static int HTF_Ret;
int HeroTacticFix(void)
{
	__asm pusha
	__asm{
		mov    eax,[ebx+0x53CC]
		mov    ecx,[ebx+0x53D0]
		xor    dl,dl
		or     eax,eax
		je     NoAH
		mov    dl, [eax+0xDC]
	}
NoAH:
	__asm{
		xor    dh,dh
		or     ecx,ecx
		je     NoDH
		mov    dh, [ecx+0xDC]
	}
NoDH:
	__asm{
		cmp    dl,dh
		ja     AtH
		je     YesT
		mov    al, [ecx+0x48]
		test   al,2
		je     lNoT
		jmp    YesT
	}
AtH:
	__asm{
		mov    al, [eax+0x48]
		test   al,2
		je     lNoT
		jmp    YesT
	}
YesT:
	__asm mov  eax,1
	__asm mov  HTF_Ret,eax
	__asm jmp  lExit
lNoT:
	__asm xor  eax,eax
	__asm mov  HTF_Ret,eax
lExit:
	__asm popa
//  __asm mov  eax,HTF_Ret
	return HTF_Ret;
}

void WrongRelictPictureFix(int Buffer,int FormatString,int ArtNum)
{
	if(ArtNum>127) ++ArtNum; // skip Pandora Box Picture
	__asm{
		push  ArtNum
		push  FormatString
		push  Buffer
		mov   eax,0x6179DE
		call  eax
		add   esp,0x0C
	}
}

__declspec( naked ) void IsAngel3(void)
{
	__asm{
		mov    eax,[esi+0x34]
		cmp    eax,0x0D
		je     l_Ok
		cmp    eax,150
		je     l_Ok
		mov    esi,[esi+0x4C]
		ret
	}
l_Ok:
	__asm{
		mov    eax,0x0D
		mov    esi,[esi+0x4C]
		ret
	}
}
/*
int _PrepInfoToShow(Byte * Mon,int Modifier)
{
	int i=MonIndex(Mon);
}
void __cdecl PrepInfoToShow(Byte * Mon,int Modifier)
{
// 476445 call
	__asm pusha
	_PrepInfoToShow(Mon,Modifier);
	__asm popa
}
*/
static Byte *RS_Mon;
void __stdcall ResetSpell(int Spell)
{
 //__asm int 3
	__asm mov  RS_Mon,ecx
	__asm pusha
	if(*(int *)&RS_Mon[0x34]!=194){
		__asm{
			mov   ecx,RS_Mon
			mov   eax,Spell
			push  eax
			mov   eax,0x444230
			call  eax
		}
	}
	__asm popa
}

static char WW_Buf[256];
void WereWolf(Byte *MonA,Byte *MonD)
{
	STARTNA(__LINE__, 0)
	int Lec=0;
	int TypeA=*(int *)&MonA[0x34];
	if(TypeA!=194) RETURNV
	int TypeD=*(int *)&MonD[0x34];
	int Lday=GetCurDate()%28;
	do{
	 if(Lday!=14 && Lday!=15 && Lday!=16) break;
	 if(MS_AtType!=1 && MS_AtType!=3) break;
	 Lec=1;
//    *(int *)&MonA[0xEC]=59;
		int sideA=*((int *)&MonA[0xF4]);
		if(*((int *)&MonA[0x288])){ sideA=!sideA; }
		 __asm{
				mov    ecx,0x699420
				mov    ecx,[ecx]
				push   1
				push   1
				push   MonA  // -> DMon
				push   sideA  // Side
				push   59 // Spell Index
				mov    eax,0x5A8950
				call   eax
				test   al, al
				jz     lCannot
				mov    edi,MonA
				mov    ecx, [edi+0x38]     // Dest position
				push   3
				push   0  // level of creature
				push  -1
				push   1
				push   ecx
				mov    ecx, 0x699420
				mov    ecx,[ecx]
				push   59 // Spell Index
				mov    eax,0x5A0140
				call   eax
			}
			//*(int *)&MonA[0x194];
			*(int *)&MonA[0x284]=2;
			//*(int *)&MonA[0x3C8]=2;
lCannot:;    
	}while(0);
	if(TypeD==194) RETURNV // itself immune
	if(TypeD>=132 && TypeD<=135) RETURNV
	if(TypeD>=150 && TypeD<=158) RETURNV
	if(TypeD==160 && TypeD<=163) RETURNV
	if(TypeD==174 && TypeD<=191) RETURNV
	
	Dword Flags=*(Dword *)&MonD[0x84];
	if((Flags&0x00000010) == 0) RETURNV // not alive target
	if(Random(1,100)>(20+20*Lec)) RETURNV // chance to work out
	int NumA=*(int *)&MonA[0x4C];
	int PosD=*(int *)&MonD[0x38];
//  if(PosA<0 || PosA>=0xBB) return;
	int SideD=*(int *)&MonD[0xF4];
	int SlotD=*(int *)&MonD[0x5C];
	int HealthD=*(int *)&MonD[0xC0];
	int NumD=*(int *)&MonD[0x4C];
	if(NumD<1) RETURNV
	int BFPosD=MonIndex(MonD);
	int HealthPoint=HealthD*NumD;
	*(int *)&MonD[0xDC]=0; // 3.58f фикс воскрешения фениксов и т.п.
	DoPhysicalDamage(MonD,HealthPoint+1);
	Flags|=0x10000000; 
	*(Dword *)&MonD[0x84]=Flags;
	MonD[0x20]=1;
	int HealthN=MonTable[194].HitPoints;
	int NumN=HealthPoint/HealthN; if(NumN<1) NumN=1;
	Flags&=0x0E000000;
	// add text to hint here
	int txtind; char *who,*whom,*to;
	if(NumA==1){ txtind=235; who=MonTable[194].NameS; }else{ txtind=236; who=MonTable[194].NameP; }
	if(NumD==1){ whom=MonTable[TypeD].NameS; }else{ whom=MonTable[TypeD].NameP; }
	if(NumN==1){ to=MonTable[194].NameS; }else{ to=MonTable[194].NameP; }
	sprintf(WW_Buf,ITxt(txtind,0,&Strings),who,whom,to);
	__asm{
		mov    ecx,0x699420
		mov    ecx,[ecx]
		mov    ecx,[ecx+0x132FC]
		lea    eax,WW_Buf
		push   0
		push   1
		push   eax
		mov    eax,0x4729D0
		call   eax
	}
	// redraw
	__asm{
		push    1
		push    0x33
		mov     ecx,0x699420
		mov     ecx,[ecx]
		mov     eax,0x468570
		call    eax
	}
	Byte *MonN=SammonCreatureEx(194,NumN,PosD,SideD,SlotD,1,Flags);
		// add text to hint here
	if(MonN==0) RETURNV // cannot summon
	int BFPosN=MonIndex(MonN);
	CrExpo *cr=CrExpoSet::GetBF(BFPosN);
	*cr=*CrExpoSet::GetBF(BFPosD);
	cr->Fl.MType=194;
//  if(SlotD!=-1){
//    cr=CrExpoSet::Find(Type,Crloc);Type=CE_HERO; Crloc=MAKEHS(hind,Slot);
//    if(cr!=0){
//      cr->Fl.MType=194;
//    }
//  }
	RETURNV
}
/*
__declspec( naked ) int DracolishRandom(void)
{ // 
	__asm{
		mov     eax, [esi+0x34]   // cr type
		cmp     eax, 196 // dracolish
		je     _Drac
		mov     eax, 0x50C7C0
		call    eax
		jmp    _LeaveIt
	}
_Drac:
	__asm{
		xor     eax, eax
	}
_LeaveIt:
	__asm{
		ret
	}
}

static Dword DR_Mult0;
static float DR_Mult=0.8f;
void Dracolich(Byte *MonA,Byte *MonD)
{
	int TypeA=*(int *)&MonA[0x34];
	if(TypeA!=196) return; // not Dracolich
//  int TypeD=*(int *)&MonD[0x34];
	Dword Flags=*(Dword *)&MonD[0x84];
	if((Flags&0x00000010) == 0) return; // not alive target
	////if(Random(1,100)>20) return; // chance to work out
//    int NumA=*(int *)&MonA[0x4C];
//    int PosD=*(int *)&MonD[0x38];
//  if(PosA<0 || PosA>=0xBB) return;
//  int SideD=*(int *)&MonD[0xF4];
//    int SlotD=*(int *)&MonD[0x5C];
//    int HealthD=*(int *)&MonD[0xC0];
	int NumD=*(int *)&MonD[0x4C];
	if(NumD<1) return;
//  int BFPosD=MonIndex(MonD);
//    int HealthPoint=HealthD*NumD;
//  __asm{
//    mov   ecx,0x63B8CC
//    mov   eax,[ecx]
//    mov   DR_Mult0,eax
//    mov   eax,DR_Mult
//    mov   [ecx],eax
//  }
	SetMonMagic(MonD,0x4B,1,0,0);
//  __asm{
//    mov   ecx,0x63B8CC
//    mov   eax,DR_Mult0
//    mov   [ecx],eax
//  }
	__asm{ // анимация
		mov    eax,MonD
		push   0
		push   100
		push   eax // ->monster
		push   0x4B
		mov    ecx,0x699420  // ->combat manager
		mov    ecx,[ecx]
		mov    eax,0x4963C0
		call   eax
	}
}
*/
static int Dragon2Summon[20][20][2]={
// terrain type byte
//  0   Dirt 
	{{68/*Bone*/,45},{69/*Ghost*/,25},{154/*Blood*/,20},{196/*Dracolich*/,10},{0,0}},
//  1   Sand 
	{{26/*Green*/,10},{82/*Red*/,10},{83/*Black*/,10},{27/*Gold*/,10},{68/*Bone*/,10},{69/*Ghost*/,10},{155/*Darkness*/,5},
	{151/*Diamond*/,5},{154/*Blood*/,5},{196/*Dracolich*/,5},{133/*Crystal*/,5},{134/*Faerie*/,5},{135/*Rust*/,5},{132/*Azure*/,5},{0,0}},
//  2   Grass 
	{{26/*Green*/,70},{27/*Gold*/,20},{151/*Diamond*/,10},{0,0}},
//  3   Snow 
	{{68/*Bone*/,25},{69/*Ghost*/,20},{132/*Azure*/,20},{133/*Crystal*/,15},{151/*Diamond*/,10},{135/*Rust*/,10},{0,0}},
//  4   Swamp 
	{{168/*Gorynych*/,50},{26/*Green*/,35},{135/*Rust*/,15},{0,0}},
//  5   Rough 
	{{26/*Green*/,10},{82/*Red*/,10},{83/*Black*/,10},{27/*Gold*/,10},{68/*Bone*/,10},{69/*Ghost*/,10},{155/*Darkness*/,5},
	{151/*Diamond*/,5},{154/*Blood*/,5},{196/*Dracolich*/,5},{133/*Crystal*/,5},{134/*Faerie*/,5},{135/*Rust*/,5},{132/*Azure*/,5},{0,0}},
//  6   Underground 
	{{82/*Red*/,60},{83/*Black*/,20},{155/*Darkness*/,10},{135/*Rust*/,10},{0,0}},
//  7   Lava 
	{{82/*Red*/,60},{83/*Black*/,20},{155/*Darkness*/,10},{135/*Rust*/,10},{0,0}},
//  8   Water 
	{{26/*Green*/,10},{82/*Red*/,10},{83/*Black*/,10},{27/*Gold*/,10},{68/*Bone*/,10},{69/*Ghost*/,10},{155/*Darkness*/,5},
	{151/*Diamond*/,5},{154/*Blood*/,5},{196/*Dracolich*/,5},{133/*Crystal*/,5},{134/*Faerie*/,5},{135/*Rust*/,5},{132/*Azure*/,5},{0,0}},
//  9   Rock - impossible though
	{{26/*Green*/,10},{82/*Red*/,10},{83/*Black*/,10},{27/*Gold*/,10},{68/*Bone*/,10},{69/*Ghost*/,10},{155/*Darkness*/,5},
	{151/*Diamond*/,5},{154/*Blood*/,5},{196/*Dracolich*/,5},{133/*Crystal*/,5},{134/*Faerie*/,5},{135/*Rust*/,5},{132/*Azure*/,5},{0,0}},
// terrain modifiers  -1 - ничего
//  0 - побережье (только рисунок)
	{{26/*Green*/,10},{82/*Red*/,10},{83/*Black*/,10},{27/*Gold*/,10},{68/*Bone*/,10},{69/*Ghost*/,10},{155/*Darkness*/,5},
	{151/*Diamond*/,5},{154/*Blood*/,5},{196/*Dracolich*/,5},{133/*Crystal*/,5},{134/*Faerie*/,5},{135/*Rust*/,5},{132/*Azure*/,5},{0,0}},
//  1 - Magic Plains
	{{134/*Faerie*/,100},{0,0}},
//  2 - Cursed Grounds
	{{68/*Bone*/,45},{69/*Ghost*/,25},{154/*Blood*/,20},{196/*Dracolich*/,10},{0,0}},
//  3 - Holy Grounds
	{{26/*Green*/,70},{27/*Gold*/,20},{151/*Diamond*/,10},{0,0}},
//  4 - Evil Fog
	{{68/*Bone*/,45},{69/*Ghost*/,25},{154/*Blood*/,20},{196/*Dracolich*/,10},{0,0}},
//  5 - Clover Field
	{{134/*Faerie*/,30},{135/*Rust*/,25},{133/*Crystal*/,25},{132/*Azure*/,20},{0,0}},
//  6 - Lucid Pools
	{{26/*Green*/,10},{82/*Red*/,10},{83/*Black*/,10},{27/*Gold*/,10},{68/*Bone*/,10},{69/*Ghost*/,10},{155/*Darkness*/,5},
	{151/*Diamond*/,5},{154/*Blood*/,5},{196/*Dracolich*/,5},{133/*Crystal*/,5},{134/*Faerie*/,5},{135/*Rust*/,5},{132/*Azure*/,5},{0,0}},
//  7 - Fiery Fields
	{{82/*Red*/,60},{83/*Black*/,20},{155/*Darkness*/,10},{135/*Rust*/,10},{0,0}},
//  8 - Rockland
	{{135/*Rust*/,100},{0,0}},
//  9 - Magic Clouds
	{{134/*Faerie*/,100},{0,0}}
};
static int AnyDragon2Summon[15]={
	26/*Green*/, 27/*Gold*/, 151/*Diamond*/, 82/*Red*/, 83/*Black*/, 
	155/*Darkness*/, 68/*Bone*/, 69/*Ghost*/, 154/*Blood*/, 196/*Dracolich*/, 
	168/*Gorynych*/, 134/*Faerie*/, 135/*Rust*/, 133/*Crystal*/, 132/*Azure*/};
static Dword C4DHA_Hero;
char Check4DragonHeartArt(int Art)
{
	_ECX(C4DHA_Hero);
	__asm pusha
	STARTNA(__LINE__, 0)
	__asm{
		push    159 //7
		mov     ecx,C4DHA_Hero
		mov     eax,0x4D9460
		call    eax
		test    al,al     
		je     _DoesNotHave
	}
	_Hero_ *hp=(_Hero_ *)C4DHA_Hero;
	Byte   *cm;
	__asm{
		mov     eax,0x699420
		mov     eax,[eax]
		mov     cm,eax
	}
	int tp=0;
	if(Random(1,100)>90){ // random
		tp=AnyDragon2Summon[Random(0,14)];
	}else{ // terrain based
		int mod=*(int *)&cm[0x53C0];
		if(mod==-1){ // has modifier
			if(mod<0 || mod>9) mod=0;
			int chance=Random(0,100);
			for(int i=0,j=0;i<20;i++){
				if(Dragon2Summon[mod+10][i][1]==0) break;
				tp=Dragon2Summon[mod+10][i][0];
				j+=Dragon2Summon[mod+10][i][1];
				if(chance<j) break;
			}
		}else{ // terrain based
			_MapItem_ *mp=*(_MapItem_ **)&cm[0x53BC];
			int mod=mp->Land;
			if(mod<0 || mod>9) mod=9;
			int chance=Random(0,100);
			for(int i=0,j=0;i<20;i++){
				if(Dragon2Summon[mod][i][1]==0) break;
				tp=Dragon2Summon[mod][i][0];
				j+=Dragon2Summon[mod][i][1];
				if(chance<j) break;
			}
		}
	}
	int HPpoints=hp->ExpLevel*100;
	int Hp0=MonTable[tp].HitPoints; if(Hp0<1) Hp0=1;
	int num=HPpoints/Hp0; if(num<1) num=1;
//  SummonCreatures3(tp,hp->PSkill[2],0/*level*/);
	int side=-1;
	if(*(_Hero_ **)&cm[0x53CC]==hp) side=0;
	if(*(_Hero_ **)&cm[0x53D0]==hp) side=1;
	if(side!=-1){ 
//    if(side==0) pos=89; else pos=97;
//    SammonCreatureEx(tp,num,pos,side,-1,1,0);
		if(side==0){ 
			do{
				if(SammonCreatureIfPossible(tp,num, 89,side,-1,1,0)) break;
				if(SammonCreatureIfPossible(tp,num, 55,side,-1,1,0)) break;
				if(SammonCreatureIfPossible(tp,num,123,side,-1,1,0)) break;
				if(SammonCreatureIfPossible(tp,num, 21,side,-1,1,0)) break;
				if(SammonCreatureIfPossible(tp,num,157,side,-1,1,0)) break;
			}while(0);
		}else{ 
			do{
				if(SammonCreatureIfPossible(tp,num, 97,side,-1,1,0)) break;
				if(SammonCreatureIfPossible(tp,num, 63,side,-1,1,0)) break;
				if(SammonCreatureIfPossible(tp,num,131,side,-1,1,0)) break;
				if(SammonCreatureIfPossible(tp,num, 29,side,-1,1,0)) break;
				if(SammonCreatureIfPossible(tp,num,165,side,-1,1,0)) break;
			}while(0);
		}
	}
_DoesNotHave:
	STOP
	__asm popa
	__asm{
		mov     eax,Art
		mov     ecx,C4DHA_Hero
		push    eax
		mov     eax,0x4D9460
		call    eax
		mov     CDummy,al
	}
	return CDummy;
}

__declspec( naked ) void DracolichShoot(void)
{
	__asm{
		cmp   eax, 0x40 // Lich
		je   _Yes
		cmp   eax, 0x41 // Power Lich
		je   _Yes
		cmp   eax, 196  // Dracolich
		je   _Yes
		mov   eax, 0
		ret
	}
_Yes:
	__asm{
		mov   eax,0x41
		ret
	}
}

static char DAISS_Name[]="ZShot195.def";
void DisableAIStackSwitching(int){ return; }
// 3.58f commander no shooting animation crash
__declspec( naked ) void LoadShootingDef4Commanders(void){
	__asm{
		mov     eax,[ebx+0x34]
		cmp     eax,174
		jb     _MStd
		cmp     eax,191
		ja     _MStd
		mov     eax,4 // can load shooting animation for any case
		ret
	}
_MStd:
	__asm{
		mov     eax,[ebx+0x84]
		ret
	}
}

__declspec( naked ) void LoadShootingDef(void){
	__asm{
		mov   eax,[ebx+0x34]
		cmp   eax,196  // Dracolich
		je   _Draco
		mov   ecx,0x6609B0
		jmp   short _Exit
	}
_Draco:
	__asm{
		mov   ecx,offset DAISS_Name
	}
_Exit:;
	__asm{
		ret
	}
}

static Byte *FSENH_BatMan;
static char *FSENH_Ret;
void _FixSummonElementalNoHero(void)
{
	int side=*(int *)&FSENH_BatMan[0x132C0];
	_Hero_ *hp=*(_Hero_ **)&FSENH_BatMan[0x53CC+side*4];
	if(hp==0){
		FSENH_Ret="";
	}else{
		FSENH_Ret=hp->Name;
	}
}
__declspec( naked ) void FixSummonElementalNoHero(void)
{
	__asm  pusha
	_EBX(FSENH_BatMan);
	_FixSummonElementalNoHero();
	__asm  popa
	__asm  mov   edx,FSENH_Ret
	__asm  ret
}
__declspec( naked ) void FixDeathRippleNoHero(void)
{
	__asm  pusha
	_ECX(FSENH_BatMan);
	_FixSummonElementalNoHero();
	__asm  popa
	__asm  mov   ecx,FSENH_Ret
	__asm  ret
}

Dword DRR_Ecx;
float DRR_RetFloat;
float __stdcall DeathRippleResist(int Spell,int ASide,Byte* DMon,int Mod1,int Mod2,int heroMONSTR)
{
	_ECX(DRR_Ecx);
	if(heroMONSTR==1){ // monstr casts
		if(*(int *)&DMon[0xF4]==ASide){ // attacking side monstr
			return 0.0;
		}
	}
	__asm{
		push   heroMONSTR
		push   Mod2
		push   Mod1
		push   DMon
		push   ASide
		push   Spell
		mov    ecx,DRR_Ecx
		mov    eax,0x5A83A0
		call   eax
		fstp   DRR_RetFloat
	}
	return DRR_RetFloat;
}

static _Hero_ *FNH4SLC_Hp;
int __stdcall FixNoHero4SpellLevelCalc(int Scool,int GroundModifier)
{
//__asm int 3
	_ECX(FNH4SLC_Hp);
	if(FNH4SLC_Hp==0){ // no hero - monster
		int var=0;
		switch(GroundModifier){
			case 1: var=0x0F; break;
			case 6: var=0x04; break;
			case 7: var=0x02; break;
			case 8: var=0x08; break;
			case 9: var=0x01; break;
		}
		if(var & Scool) DDummy=3; else DDummy=0;
	}else{ // hero - standard
		__asm{
			mov   ecx,FNH4SLC_Hp
			push  GroundModifier
			push  Scool
			mov   eax,0x4E5370
			call  eax
			mov   DDummy,eax
		}
	}
	return DDummy;
}

static _Hero_ *FNH4SP_Hp;
int __stdcall FixNoHero4SpellPower(int Spell,int Par2,int Par3)
{
//__asm int 3
	_ECX(FNH4SP_Hp);
	if(FNH4SP_Hp==0){ // no hero - monster
		STARTNA(__LINE__, 0)
		Byte *AMon;
		int mside,msideind,var=0;
		switch(Spell){
			case 60: // Hypnotize
				__asm{
					mov    eax,0x699420
					mov    edx,[eax]
					mov    eax,[edx+0x132B8]
					mov    mside,eax
					mov    eax,[edx+0x132BC]
					mov    msideind,eax
				}
				AMon=MonPos2(mside,msideind);
				int Health=*(int *)&AMon[0xC0];
				int Num=*(int *)&AMon[0x4C];
				var=Health*Num;
				break;
		}
		STOP
		DDummy=var;
	}else{ // hero - standard
		__asm{
			mov   ecx,FNH4SP_Hp
			push  Par3
			push  Par2
			push  Spell
			mov   eax,0x4E6260
			call  eax
			mov   DDummy,eax
		}
	}
	return DDummy;
}

static _Hero_ *FNH4SD_Hp;
int FixNoHero4SpellDuration(void)
{
	_ECX(FNH4SD_Hp);
	STARTNA(__LINE__, 0)
	if(FNH4SD_Hp!=0){ // There is a Hero
		__asm{
			mov    ecx,FNH4SD_Hp
			mov    eax,0x4E5020
			call   eax
			mov    DDummy,eax
		}
		RETURN(DDummy)
	}else{ // No Hero owner
		RETURN(5)
	}
}

__declspec( naked ) void FixWrongDwIndKingdomOverview(void)
{
	__asm{
		xor   edx,edx
		mov   dl,[ecx+edi]   // type
		cmp   dl,0x14  // extended dwelling
		je   _ExtDwell
		mov   dl,[ecx+edi+1] // subtype
//    cmp   edx,80
//    jae  _NewDwelling
//    test  eax,eax
		jmp  _Exit1
	}
_ExtDwell:
	__asm{
		mov   dl,[ecx+edi+1] // subtype
		add   dl,DWNUM
		jmp  _Exit1
	}
//_NewDwelling:
//  __asm{
//    add   edx,8
//    test  eax,eax
//    jmp  _Exit1
//  }
_Exit1:
	__asm{
		test  eax,eax
		ret
	}
}

static Byte *FSBFPUMI_BatMan;
static int ParMod;
void __stdcall FixShowBFPopUpMonInfo(Byte *Mon,int)
{
	 _ECX(FSBFPUMI_BatMan);
	STARTNA(__LINE__, 0)
	 ParMod=1;
	 do{
		 if(/*MonInfoDlgPopUpSetting == 1 || MonInfoDlgPopUpSetting == 2*/ MonInfoDlgPopUpSetting &&
			    GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_MENU) >= 0 && GetKeyState(VK_SHIFT) >= 0) break;
		 if(Mon==0) break;
		 int v=*(int *)&Mon[0xF4]; // сторона - хозяин монстра
		 if(v<0) break;
		 if(v>1) break;
//__asm int 3
		 if(v==*(int *)&FSBFPUMI_BatMan[0x132C0]){ // та же сторона, что и атакует 
			 v=*(int *)&FSBFPUMI_BatMan[v*4+0x54A8];
			 if(IsThis(v)) break;    
		 }else{ // other
//       v=*(int *)&FSBFPUMI_BatMan[v*4+0x54A8];
		 }
/*
		 v=*(int *)&FSBFPUMI_BatMan[v*4+0x54A8];
		 if(v<0){ ParMod=0; break; }

		 __asm{
			 mov    ecx,BASE
			 mov    ecx,[ecx]
			 push   v
			 mov    eax,0x4CE600
			 call   eax
			 mov    BDummy,al
		 }
		 if(BDummy!=0) break; // this player
//     if(IsThis(v)) break; 
*/
		 ParMod=0;
	 }while(0);
 __asm{
	 mov    ecx,FSBFPUMI_BatMan
	 push   ParMod
	 push   Mon
	 mov    eax,0x468440
	 call   eax
	}
	RETURNV
}

__declspec( naked ) int FixShootTwiceOneShot(void){
	__asm{
		mov   eax,[esi+0xD8] // количество выстрелов
		or    eax,eax
		ja   _HasMore
		mov   eax,[esi+0x84]  
		and   eax,0xFFFF7FFF // reset shoot twice
		ret
	}
_HasMore:
	__asm{
		mov   eax,[esi+0x84] // return as is 
		ret
	}
}
void NetworkMessageParse(Dword RetAddress,char *buffer, const char *format,int p1,int p2,int p3,int p4,int p5,int p6)
{
	if(RetAddress==0x5FEAFC) return; // какой-то ненормальный вызов
	if(RetAddress==0x553196) return; // DPlay Receive error [%s]
//  __asm int 3
	sprintf(buffer,format,p1,p2,p3,p4,p5,p6);
	Message(buffer,1);
}

static Byte *TWEF_TownMan=0; 
void TownWrongExpoFix(void){
	_ECX(TWEF_TownMan);
	__asm pusha
	STARTNA(__LINE__, 0)
	_CastleSetup_ *cp=*(_CastleSetup_ **)&TWEF_TownMan[0x38];
	if(cp!=0){
//    if(cp->IHero==-1){ // город
			for(int i=0;i<7;i++){
				if(cp->GuardsT[i]==-1){ 
					cp->GuardsN[i]=0;
					CrExpoSet::Del(CE_TOWN,MAKETS(cp->x,cp->y,cp->l,i));
					continue;
				}
				if(cp->GuardsN[i]<=0){ 
					cp->GuardsT[i]=0xFFFFFFFF; cp->GuardsN[i]=0;
					CrExpoSet::Del(CE_TOWN,MAKETS(cp->x,cp->y,cp->l,i));
					continue;
				}
			}
//    }
	}
	STOP
	__asm popa
	__asm{
		mov    ecx,TWEF_TownMan
		mov    eax,0x5D5810
		call   eax
	}

}

__declspec( naked ) void NoClone4CommandersFix(void){
	__asm{
		push   eax
		mov    eax,[edi+0x34]
		cmp    eax,174 // NPC 
		jb    _NotNPC
		cmp    eax,191 // NPC 
		ja    _NotNPC
		mov    edx,8  // level out of range (cannot cast clone)   
	}
_NotNPC:
	__asm{
		pop    eax
		mov    eax,[ecx+4*eax+0x22BC]
		ret
	}
}
/*
Dword __stdcall GameSummonCreature(int Redraw,int CrBits,int PosCode,int MonNum,int MonType,int Side){
// ecx = combat man
	__asm{
		push  Redraw
		push  CrBits
		push  PosCode
		push  MonNum
		push  MonType
		push  Side
		mov   eax,0x479A30
		call  eax
		mov   DDummy,eax
		mov   Glb_LastSummoned,eax
	}
	return DDummy;
}
*/
///////////////////////////////////////////////
static int Send2Player=-1;
NetData NData;

// send ERM v vars
void SendERMVars(int from,int to){
	if(from>to){int t=from; from=to; to=t;}
	if(from<0) from=0;
	if(to>9999) to=9999;
	if(from>to) return;
	NetData nd = NetData(NM_VARS,Send2Player,0,to-from+3);
	int *data = (int*)nd.Data();
	for(int i=from,j=2;i<=to;i++,j++) data[j]=ERMVar2[i];
	data[0]=from;
	data[1]=to;
	nd.SendTo();
}
void ReceiveERMVars(void){
	int *data=(int *)NData.Data();
	int strt=data[0];
	int stop=data[1];
	for(int i=strt,j=2;i<=stop;i++,j++) ERMVar2[i]=data[j];
	NData=NetData();
}
// send ERM flags
void SendERMFlags(int from,int to){
	if(from>to){int t=from; from=to; to=t;}
	if(from<0) from=0;
	if(to>999) to=999;
	if(from>to) return;
	NetData nd=NetData(NM_FLAGS,Send2Player,0,to-from+3);
	int *data = (int*)nd.Data();
	for(int i=from,j=2;i<=to;i++,j++) data[j]=ERMFlags[i];
	data[0]=from;
	data[1]=to;
	nd.SendTo();
}
void ReceiveERMFlags(void){
	int *data=(int *)NData.Data();
	int strt=data[0];
	int stop=data[1];
	for(int i=strt,j=2;i<=stop;i++,j++) ERMFlags[i]=data[j];
	NData=NetData();
}
// send ERM w vars
void SendERMWVars(int hind,int from,int to){
	if(from>to){int t=from; from=to; to=t;}
	if(from<0) from=0;
	if(to>199) to=199;
	if(from>to) return;
	NetData nd=NetData(NM_WVARS,Send2Player,0,to-from+4);
	int *data = (int*)nd.Data();
	for(int i=from,j=3;i<=to;i++,j++) data[j]=ERMVarH[hind][i];
	data[0]=hind;
	data[1]=from;
	data[2]=to;
	nd.SendTo();
}
void ReceiveERMWVars(void){
	int *data=(int *)NData.Data();
	int hind=data[0];
	int strt=data[1];
	int stop=data[2];
	for(int i=strt,j=3;i<=stop;i++,j++) ERMVarH[hind][i]=data[j];
	NData=NetData();
}
// set random seed
void SyncRGSeed(void){
	Dword val = timeGetTime();
	NetData nd=NetData(NM_RGSEED,Send2Player,&val,1);
	nd.SendTo();
	__asm{
		mov   ecx,val;
		mov   eax,0x50C7B0
		call  eax // set random seed
	}
}
void ReceiveRGSeed(void){
	 Dword seed=*(Dword *)NData.Data();
		NData=NetData();
	__asm{
		mov   ecx,seed;
		mov   eax,0x50C7B0
		call  eax // set random seed
	}
}
// custom request

void SendCustomRequest(int ind,int *var,int num){
	NetData nd=NetData(NM_CUSTOM,Send2Player,0,num+2);
	int *data = (int*)nd.Data();
	for(int i=0;i<num;i++) data[i+2]=var[i]; // x vars
	data[0]=ind; // function index
	data[1]=num; // number of x vars
	nd.SendTo();
}
void ReceiveCustomRequest(void){
	int *data=(int *)NData.Data();
	int ind=data[0];
	int num=data[1];
	int vars[16];
	for(int i=0;i<num;i++) vars[i]=data[i+2];
	NData=NetData();
	Call_Function(ind,vars,num);
}
///////////////////////////////////////////////////
Dword *RNBFC_Ecx=0;
void ReceiveNetBFCommand(void){
	_ECX(RNBFC_Ecx);
	__asm pusha
		if(RNBFC_Ecx[2]==MYNETMES){ // my
			NData=NetData(RNBFC_Ecx);
//      TriggerIP(4);
//__asm int 3
			int whom=NData.Whom(); 
			if((whom==-1)||(IsThis(whom))){
				switch(NData.Command()){
					case NM_VARS:   ReceiveERMVars(); break;
					case NM_WVARS:  ReceiveERMWVars(); break;
					case NM_FLAGS:  ReceiveERMFlags(); break;
					case NM_RGSEED: ReceiveRGSeed(); break;
					case NM_CUSTOM: ReceiveCustomRequest(); break;
					///////
				}
			}
		}
		__asm{
			mov    ecx,RNBFC_Ecx
			mov    eax,0x555D00
			call   eax
		}
	__asm popa
}
void ReceiveNetAMCommand(void){
	_EDI(RNBFC_Ecx);
	__asm pusha
		if(RNBFC_Ecx[2]==MYNETMES){ // my
			NData=NetData(RNBFC_Ecx);
//      TriggerIP(4);
//__asm int 3
			int whom=NData.Whom(); 
			if((whom==-1)||(IsThis(whom))){
				switch(NData.Command()){
					case NM_VARS:   ReceiveERMVars(); break;
					case NM_WVARS:  ReceiveERMWVars(); break;
					case NM_FLAGS:  ReceiveERMFlags(); break;
					case NM_RGSEED: ReceiveRGSeed(); break;
					case NM_CUSTOM: ReceiveCustomRequest(); break;
					///////
				}
			}
		}
		__asm{
			mov    eax,RNBFC_Ecx
			push   eax
			mov    eax,0x60B0F0
			call   eax
			add    esp,4
		}
	__asm popa
}

int ERM_NetworkService(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	switch(Cmd){
		case 'D': // set to whom send a command (-1 to all)
			if(Apply(&Send2Player,4,Mp,0)) break;
			break;
		case 'V':{ // send vars
			CHECK_ParamsMin(2);
			int strt,stop;
			if(Apply(&strt,4,Mp,0)) break;
			if(Apply(&stop,4,Mp,1)) break;
			SendERMVars(strt-1,stop-1);
			break;}
		case 'W':{ // send w vars
			CHECK_ParamsMin(3);
			int hind,strt,stop;
			if(Apply(&hind,4,Mp,0)) break;
			if(Apply(&strt,4,Mp,1)) break;
			if(Apply(&stop,4,Mp,2)) break;
			SendERMWVars(hind,strt-1,stop-1);
			break;}
		case 'F':{ // send flags
			CHECK_ParamsMin(2);
			int strt,stop;
			if(Apply(&strt,4,Mp,0)) break;
			if(Apply(&stop,4,Mp,1)) break;
			SendERMFlags(strt-1,stop-1);
			break;}
		case 'R': // sync random generator
		 SyncRGSeed();
		 break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

/////////////////////////////////////////////////////////////////
int NumOfMagic; // номер используемого заклинания Diakon
int NumOfMagLevel; // уровнь используемого заклинания Diakon
int NumOfResistMonstr; // номер монстра, на которого кастуют закл Diakon
int TypeMag; // тип магии
int JmpDefaultResist = 0x44A4C5;
int JmpResistAllMag = 0x44A5AF;
float EqResistens;
RESISTSTRUCT MonResStr[MONNUM];

__declspec( naked )void ResistMagicManager () // Diakon менеджер управления сопративлением
{
	__asm{ 
		pushad;
		mov eax, dword ptr [ebp - 0x10];
		mov NumOfMagic, eax;
		mov eax, dword ptr [esi + 0x18];
		mov NumOfMagLevel, eax;
		mov eax, dword ptr [esi + 0x1C];
		mov TypeMag, eax;
		mov NumOfResistMonstr, edx
	} // asm

	EqResistens = 1;

	if (MonResStr[NumOfResistMonstr].AllResist.FullResistEq != 0){
			EqResistens = 1 - (float)MonResStr[NumOfResistMonstr].AllResist.FullResistEq/100;

	}//if

	if (MonResStr[NumOfResistMonstr].SpelLevelResist[NumOfMagLevel].LevelResistEq != 0){
			EqResistens = 1 - (float)MonResStr[NumOfResistMonstr].SpelLevelResist[NumOfMagLevel].LevelResistEq/100;
	}

	if (MonResStr[NumOfResistMonstr].TypeResist[TypeMag].TypeResistEq != 0){
			EqResistens = 1 - (float)MonResStr[NumOfResistMonstr].TypeResist[TypeMag].TypeResistEq/100;
	}

	if (MonResStr[NumOfResistMonstr].Magic[NumOfMagic].ResistEq != 0){
			EqResistens = 1 - (float)MonResStr[NumOfResistMonstr].Magic[NumOfMagic].ResistEq/100;
	}

	if (MonResStr[NumOfResistMonstr].MyMagic)
		__asm jmp allRes;
	else
		__asm jmp defaultRes;

defaultRes:
	__asm popad;
	__asm mov eax, EqResistens; 
	__asm mov dword ptr [ebp + 0xC], eax;
	__asm jmp JmpDefaultResist;

allRes:
	__asm popad;
	__asm fld EqResistens;
	__asm jmp JmpResistAllMag;

} // ResistMagicManager
//--------------------------------------------------------------//
__declspec (naked) void InsertResistFunc()//функция врезка Diakon
{	
	__asm jmp ResistMagicManager;

}//InsertResistFunc
//--------------------------------------------------------------//
void ResetLoadDefaultResist() // Diakon Устанавливаем стандартные св-ва моснтров по сопративлению
{
	ZeroMemory((void*)&MonResStr, sizeof(MonResStr));

	// Черные Драконы и Магический Элементаль - полное сопративление
	MonResStr[0x53].AllResist.FullResistEq = 100;
	MonResStr[0x79].AllResist.FullResistEq = 100;

	// Гном и Кристальный Дракон имеют сопративление ко всей магии 20%
	MonResStr[0x10].AllResist.FullResistEq = 20;
	MonResStr[0x85].AllResist.FullResistEq = 20;

	// Боевой Гном сопративление 40 %
	MonResStr[0x11].AllResist.FullResistEq = 40;

	// Воздушный элементаль и элементаль шторма имеют имунитет к метеоритный дождь и слепота
	MonResStr[0x70].Magic[0x17].ResistEq = 100;
	MonResStr[0x70].Magic[0x3E].ResistEq = 100;
	MonResStr[0x7F].Magic[0x17].ResistEq = 100;
	MonResStr[0x7F].Magic[0x3E].ResistEq = 100;

	// Элементаль Земли и Элементаль Магмы имеют иммунитет к Удар Молнии, Цепная молния, Армагеддон, Гром Титанов
	MonResStr[0x71].Magic[0x11].ResistEq = 100;
	MonResStr[0x71].Magic[0x13].ResistEq = 100;
	MonResStr[0x71].Magic[0x1A].ResistEq = 100;
	MonResStr[0x71].Magic[0x39].ResistEq = 100;
	MonResStr[0x7D].Magic[0x11].ResistEq = 100;
	MonResStr[0x7D].Magic[0x13].ResistEq = 100;
	MonResStr[0x7D].Magic[0x1A].ResistEq = 100;
	MonResStr[0x7D].Magic[0x39].ResistEq = 100;

	// Элементаль Воды и Элементаль Льда имеют имунитет к кольцу холода и Ледяной стреле
	MonResStr[0x73].Magic[0x10].ResistEq = 100;
	MonResStr[0x73].Magic[0x14].ResistEq = 100;
	MonResStr[0x7B].Magic[0x10].ResistEq = 100;
	MonResStr[0x7B].Magic[0x14].ResistEq = 100;

	// Зеленый Дракон, Красный Дракон, Лазурный Дракон имеют имунитет к 1,2,3 ур-ям магии
	MonResStr[0x1A].SpelLevelResist[0].LevelResistEq = 100;
	MonResStr[0x1A].SpelLevelResist[1].LevelResistEq = 100;
	MonResStr[0x1A].SpelLevelResist[2].LevelResistEq = 100;
	MonResStr[0x52].SpelLevelResist[0].LevelResistEq = 100;
	MonResStr[0x52].SpelLevelResist[1].LevelResistEq = 100;
	MonResStr[0x52].SpelLevelResist[2].LevelResistEq = 100;
	MonResStr[0x84].SpelLevelResist[0].LevelResistEq = 100;
	MonResStr[0x84].SpelLevelResist[1].LevelResistEq = 100;
	MonResStr[0x84].SpelLevelResist[2].LevelResistEq = 100;

	// Золотой Дракон имеет имунитет к 1..4 ур-ям магии
	MonResStr[0x1B].SpelLevelResist[1].LevelResistEq = 100;
	MonResStr[0x1B].SpelLevelResist[2].LevelResistEq = 100;
	MonResStr[0x1B].SpelLevelResist[3].LevelResistEq = 100;
	MonResStr[0x1B].SpelLevelResist[4].LevelResistEq = 100;


}//ResetLoadDefaultResist

// настройка монстров с атакой гарпий

void ResetLoadHarpyAtack() { // Diakon
	ZeroMemory((void*)&HarpyStr, sizeof(HarpyStr));
	// гарпии
	HarpyStr[0x48].IsHave = TRUE;

	// гарпии ведьмы
	HarpyStr[0x49].IsHave = TRUE;

	// темный дракон
	HarpyStr[0x9B].IsHave = TRUE;

};

int nMonstr; //Diakon
bool ifcast; //Diakon
CASTMAGIC MagicCast [MONNUM]; //Diakon
int Cast = 0x4476DD; //Diakon
int notCast = 0x4476E9; //Diakon
int EnFrFunc = 0x5A3F90; //Diakon
int nMagic=0; // налогаемая магия Diakon
bool fMagic = FALSE; // флаг магии Diakon

__declspec (naked) void MagicBookManager(){ // Diakon

	__asm cmp edi, 0;
	__asm jnz next;
	__asm pushad;
	__asm xor eax, eax;
	__asm mov fMagic, al;
	__asm popad;
	__asm jmp notCast;

next:
	__asm{
		pushad;
		add ecx, 13;
		mov nMonstr, ecx;
		pushad;
		call RandMagic;
		popad;
		mov eax, dword ptr [esi + 0xF4];
		mov ecx, nMagic;
		push 1;
		push 1;
		push edi;
		push eax;
		push ecx;
		mov ecx, 0x699420;
		mov ecx, [ecx];
		call EnFrFunc;
		mov ifcast, al;
	}

	if (ifcast){
		__asm popad;
		__asm jmp Cast;

	}
	__asm popad;
	__asm jmp notCast;

}

int CallHint1 = 0x4E7230; 
int CallHint2 = 0x6179DE;
int JmpDefaultHint = 0x492E3B;
int CallHint3 = 0x43FE20;

__declspec (naked) void ManageHintCast(){//Diakon
	__asm{
		mov edx, 0x699420;
		mov edx, [edx];
		lea ecx, dword ptr [eax*8];
		sub ecx, eax;
		shl ecx, 4;
		lea ecx, dword ptr [ecx + edx + 0x1C4];
		call CallHint1;
		test eax, eax;
		jnz Jmp1;
		mov eax, nMagic;
		mov ecx, 0x687FA8;
		mov ecx, [ecx]
		imul eax, eax, 0x88;
		mov edx, dword ptr [eax + ecx + 0x10];
		mov eax, 0x6A5DC4;
		mov eax, [eax];
		push edx;
		mov ecx, dword ptr [eax + 0x20];
		mov edx, dword ptr [ecx + 0x6C];
		push edx;
		push 0x697428;
		call CallHint2;
		add esp, 0x0C;
		jmp JmpDefaultHint;
Jmp1:
		mov edx, dword ptr[eax + 0x4C];
		mov ecx, dword ptr[eax + 0x34];
		call CallHint3;
		push eax;
		mov eax, nMagic;
		imul eax, eax, 0x88;
		mov ecx, 0x687FA8;
		mov ecx, [ecx];
		mov edx, dword ptr [eax+ecx+0x10];
		mov eax, 0x6A5DC4;
		mov eax, [eax];
		push edx;
		mov ecx, dword ptr [eax + 0x20];
		mov edx, dword ptr [ecx + 0x70];
		push edx;
		push 0x697428;
		call CallHint2;
		add esp, 0x10;
		jmp JmpDefaultHint;
	}
}

int JmpManager1 = 0x4483DD;
int JmpExitManager = 0x4483D2;

__declspec (naked) void MagicCastManager(){//Diakon
	__asm{
		pushad;
		xor eax, eax;
		mov fMagic, al;
		popad;
		mov ecx, dword ptr [ebp+8];
		test ecx, ecx;
		jl fJmpManager1;
		cmp ecx, 0xBB;
		jge fJmpManager1;
		mov eax, dword ptr [esi + 0x4C];
		lea eax, dword ptr [eax + eax*4];
		push eax;
		push 2;
		push -1;
		push 1;
		push ecx;
		mov ecx, nMagic;
		push ecx;
		jmp JmpExitManager;
fJmpManager1:
		jmp JmpManager1;
	}
};

CASTMAGIC StrCastMag [MONNUM]; //Diakon

void RandMagic(){// Diakon

	int RandMag = rand()%StrCastMag[nMonstr].NumOfMag;
	int i = 0;

	if (!fMagic){
	while (RandMag > -1)
	{
		if (StrCastMag[nMonstr].SpellCast[i] == TRUE)
		{
			nMagic = i;
			RandMag--;
		}

		i++;
	}
	fMagic = TRUE;
	}

}//RandMagic

void RandMagicAI(){// Diakon

	int RandMag = rand()%StrCastMag[nMonstr].NumOfMag;
	int i = 0;


	while (RandMag > -1)
	{
		if (StrCastMag[nMonstr].SpellCast[i] == TRUE)
		{
			nMagic = i;
			RandMag--;
		}

		i++;
	}

}//RandMagic

int jmpCaseAI = 0x42147D; //Diakon

bool AISide;
int MagEnemy [] = {15, 16, 17, 18, 19, 20, 21, 22, 23,
42, 45, 47, 50, 52, 54, 59, 60, 61, 62};

int i;
__declspec (naked) void AICastMagic(){//Diakon

	__asm{
		pushad;
		add ecx, 13;
		mov nMonstr, ecx;
		push eax;
		call RandMagicAI;
		pop eax;
		mov ebx, nMagic;
		mov dword ptr[eax+0x4E0],ebx;
		
	}

	for (i=0; i<sizeof(MagEnemy)/sizeof(int); i++){
		if (nMagic == MagEnemy[i]){
			__asm popad;
			__asm mov dl, 2;
			__asm jmp jmpCaseAI;
		}
	}

	__asm popad;
	__asm mov dl, 1;
	__asm jmp jmpCaseAI;	
}


//////////////////////////////////////////////////////////////////