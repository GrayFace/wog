#include <stdio.h>
#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "service.h"
#include "casdem.h"
#include "b1.h"
#define __FILENUM__ 16

__declspec( naked ) void ArtMove1(void)
{
	__asm{
		cmp    eax,0x1680
		jae    l_New
		sub    edx,[eax+0x67FFA0]
		jmp    l_All
	}
l_New: __asm  sub  edx,0x16
l_All:
	__asm ret;
}
__declspec( naked ) void ArtMove2(void)
{
	__asm{
		cmp    eax,0x1680
		jae    l_New
		mov    edx,[eax+0x67FFA4]
		jmp    l_All
	}
l_New: __asm  mov  edx,0x16
l_All:
	__asm ret;
}

//static AAIA_Ret;
// ecx,edx should be set
int __stdcall ArtAIAnal(Dword ArtNum,Dword par2,Dword par1)
{
	__asm  mov    eax,ArtNum
	__asm  cmp    eax,0x91
	__asm  ja     l_No
	__asm  push   par1
	__asm  push   par2
	__asm  push   ArtNum
	__asm  mov    eax,0x4336C0
	__asm  call   eax
	__asm  jmp    l_Yes
l_No:;
	__asm  xor    eax,eax
l_Yes:;
//  return   _EAX;
	__asm mov DDummy,eax
	return DDummy;
}

static Dword HA157_Ret;
// esi -> our mon, edi ->attaker mon
__declspec( naked ) void HasArt157(void)
{
	__asm{
			pusha
			mov      edx, [edi+84h]
			mov      HA157_Ret,edx

			mov      eax, [esi+0x288] // Hypnotized?
			test     eax, eax      
			jz       loc_0044585C  
			mov      ecx, [esi+0xF4]  // mon side
			mov      eax, 1
			sub      eax, ecx         // reverse it
			jmp      loc_00445862
	}
loc_0044585C: 
	__asm{
			mov      eax, [esi+0xF4]  // side
	}
loc_00445862:
	__asm{
			mov      ecx, 0x699420
			mov      ecx, [ecx]
			mov      eax, [ecx+eax*4+53CCh] // Hero Po
			test     eax, eax 
			jz       loc_004458C3   // no hero
			push     157
			mov      ecx, eax
			mov      eax, 0x4D9460
			call     eax            // HeroHasArt ?
			test     al, al               
			jz       loc_004458C3   // no
// has art
			mov      edx,HA157_Ret
			and      edx,0xFFFEFFFF // reset "no retailate bit"      
			mov      HA157_Ret,edx
	}
loc_004458C3:
	__asm{
			popa    
			mov      edx,HA157_Ret
			ret
	}
}

static float HA158_Fl=150;
// edi -> our mon esi -> defender mon
__declspec( naked ) void HasArt158(void)
{
	__asm{
			pusha
			mov     Byte ptr [esi+0x4C0], 0
			mov     Byte ptr [esi+0x4C1], 0
			mov     ecx, [esi+0x4C]     // no defending creatures
			test    ecx, ecx             
			jle     loc_00441C01         
			mov     eax, [edi+0x84]
			shr     eax, 2             // shooter?
			test    al, 1                
			jnz     loc_00441C01
			mov     eax, [edi+0x290]    // Blind?
			test    eax, eax   
			jnz     loc_00441C01 
			mov     eax, [edi+0x2B0]    // Stoned?
			test    eax, eax      
			jnz     loc_00441C01  
			mov     eax, [edi+0x2C0]    // Paralized?
			test    eax, eax          
			jnz     loc_00441C01      
			mov     eax, [edi+0x4C]     // No creatures?
			test    eax, eax          
			jle     loc_00441C01      

			mov     eax, [edi+0x288] // Hypnotized?
			test    eax, eax      
			jz      loc_0044585C  
			mov     ecx, [edi+0xF4]  // mon side
			mov     eax, 1
			sub     eax, ecx         // reverse it
			jmp     loc_00445862
	}
loc_0044585C: 
	__asm{
			mov     eax, [edi+0xF4]  // side
	}
loc_00445862:
	__asm{
			mov      ecx, 0x699420
			mov      ecx, [ecx]
			mov      eax, [ecx+eax*4+53CCh] // Hero Po
			test     eax, eax 
			jz       loc_00441C01   // no hero
			push     158
			mov      ecx, eax
			mov      eax, 0x4D9460
			call     eax            // HeroHasArt ?
			test     al, al               

			jz  short loc_00441C01            
			mov     edx, 0x69883C
			mov     edx, [edx]
			fld     Dword ptr [0x63CF7C+edx*4]
			fmul    HA158_Fl
			mov     eax,0x617F94   // ftol
			call    eax
			mov     ecx, eax
			mov     eax,0x4F8A50
			call    eax
			mov     eax, [ebp+8]
			mov     ecx, edi
			push    eax
			push    esi
			mov     eax,0x441330
			call    eax
	}
loc_00441C01:
	__asm{
			popa    
			mov ecx, [esi+0x84]
			ret
	}
}

struct _ClosedTowns_ {
	char  Closed;  // 0 -нет, длит в дн€х закрыти€
	char  HoldLen; // длит в дн€х задержки дл€ возврата ключа
	char  Owner;   // хоз€ин
	Byte  Hero;    // герой - хоз€ин ключа
	Byte  X,Y,L;   //
	Byte _r;
} ClosedTowns[48];

static char Buf[256];
void CloseCastleGate(_Hero_ *hp,_CastleSetup_ *Cstr)
{
	STARTNA(__LINE__, 0)
	_MapItem_  *MIp0,*MIp;
	int ArtSlot=-1,ex,ey,el,MapSize;
	int This=IsThis(hp->Owner);
	int AI=IsAI(hp->Owner);
	for(int i=0;i<19;i++){ if(hp->IArt[i][0]==160) ArtSlot=i; }
/*
			hp->OArt[i][0]=-1;
			hp->OArt[i][1]=-1;
			hp->OANum=(Byte)(hp->OANum-1);
		}
	}
*/  
	if(This&& !AI){ // human
		if(ArtSlot!=-1){ // has art
			if(ClosedTowns[Cstr->Number].Closed!=0) RETURNV // закрыт
			if(ClosedTowns[Cstr->Number].HoldLen!=0){ // ключ еще не возвращен
				sprintf(Buf,ITxt(238,0,&Strings),ClosedTowns[Cstr->Number].HoldLen);
				Request2Pic(Buf,22+Cstr->Type,7,8,160,1);
				RETURNV 
			}
			if(Request2Pic(ITxt(237,0,&Strings),22+Cstr->Type,7,8,160,2)){
				ClosedTowns[Cstr->Number].Closed=5;
				ClosedTowns[Cstr->Number].HoldLen=5;
				ClosedTowns[Cstr->Number].Hero=(Byte)hp->Number;
				ClosedTowns[Cstr->Number].Owner=hp->Owner;
				hp->IArt[ArtSlot][0]=-1; 
				ex=Cstr->x; ey=Cstr->y; el=Cstr->l;
				ClosedTowns[Cstr->Number].X=ex; ClosedTowns[Cstr->Number].Y=ey; ClosedTowns[Cstr->Number].L=el; 
				__asm{
					mov   ecx,BASE
					mov   ecx,[ecx]
					mov   eax,[ecx+0x1FC40]
					mov   MIp0,eax
					mov   eax,[ecx+0x1FC44]
					mov   MapSize,eax
				}
				MIp=&MIp0[ex+(ey+el*MapSize)*MapSize];
				MIp->Attrib|=0x01;  // закрываем
			}
		}
	}else{ // AI
	}
}

void DaylyCastleKey(void/*int User*/)
{
	STARTNA(__LINE__, 0)
	_CastleSetup_ *Castle, *End;
	int   i,Num,Cnum,DoIt;
	_ClosedTowns_ *csp;
	_MapItem_  *MIp0,*MIp;
	_Hero_ *hp;
	int    MapSize,ex,ey,el/*,Date*/;

	__asm{
		mov   ecx,BASE
		mov   ecx,[ecx]
		mov   eax,[ecx+0x021614]
		mov   Castle,eax
		mov   eax,[ecx+0x021618]
		mov   End,eax
	}
	Num=End-Castle;
//  Date=GetCurDate();
//__asm int 3
	for(i=0;i<Num;i++,Castle++){
		DoIt=0;
		if((Cnum=GetCastleNumber(Castle))<0) RETURNV
		csp=&ClosedTowns[Cnum];
		ex=Castle->x; ey=Castle->y; el=Castle->l;
		if(csp->Closed){ // закрыт
			csp->Closed--;
//      if(csp->Closed==1){
//        if(csp->Owner==User){ // если этот игрок закрывал
//          DoIt=1; // откроем
//        }
//      }
			if(csp->Closed==0){
				DoIt=1; // откроем
			}
		}else{
			if(csp->HoldLen){ // еще задержан
				csp->HoldLen--;
				if(csp->HoldLen==0){
					hp=GetHeroStr(csp->Hero);
					if(hp->OANum<64){ //добавим арт в рюкзак
						for(i=0;i<64;i++){
							if(hp->OArt[i][0]==-1){
								hp->OArt[i][0]=160;
								hp->OArt[i][1]=-1;
								hp->OANum=(Byte)(hp->OANum+1);
								csp->Hero=0;
								csp->Owner=0;
								break;
							}
						}
					}else{ // некуда добавить - подождем еще денек
						csp->HoldLen=1;
					}
				}
			}
		}
		if(DoIt!=0){
			__asm{
				mov   ecx,BASE
				mov   ecx,[ecx]
				mov   eax,[ecx+0x1FC40]
				mov   MIp0,eax
				mov   eax,[ecx+0x1FC44]
				mov   MapSize,eax
			}
			MIp=&MIp0[ex+(ey+el*MapSize)*MapSize];
			if(DoIt==1){
				MIp->Attrib&=0xFE;  // открываем
//      }else{
//        MIp->Attrib|=0x01;  // закрываем
			}
		}
	}
	RETURNV
}
/*
static Dword CMS_Ecx;
void CorrectMapSquares(void)
{
	_ECX(CMS_Ecx); // [BASE]
	__asm{
		mov    ecx,CMS_Ecx
		mov    eax,0x4CA0D0
		call   eax
	}
	_CastleSetup_ *Castle, *End;
	_ClosedTowns_ *csp;
	int i,Num,Cnum,ex,ey,el;
	__asm{
		mov   ecx,BASE
		mov   ecx,[ecx]
		mov   eax,[ecx+0x021614]
		mov   Castle,eax
		mov   eax,[ecx+0x021618]
		mov   End,eax
	}
	Num=End-Castle;
	for(i=0;i<Num;i++,Castle++){
		if((Cnum=GetCastleNumber(Castle))<0) RETURNV
		csp=&ClosedTowns[Cnum];
		ex=Castle->x; ey=Castle->y; el=Castle->l;
		if(csp->Closed){ // закрыт
			Word *VisWord=GetVisabilityPosByXYL(ex,ey,el);
			
		}
	}
}
*/

static Dword ASP_Ecx;
void __stdcall AdjustSquarePassability(_MapItem_ *MIp,int ADDremove)
{
	_ECX(ASP_Ecx); 
	STARTNA(__LINE__, 0)
	__asm{
		push   ADDremove
		push   MIp
		mov    ecx,ASP_Ecx
		mov    eax,0x505C60
		call   eax
	}
	int x=0,y=0,l=0; Map2Coord(MIp,&x,&y,&l);
	for(int i=0;i<48;i++){
		_ClosedTowns_ *cp=&ClosedTowns[i];
		if(cp->Closed==0) continue;
		if(cp->X!=x) continue;
		if(cp->Y!=y) continue;
		if(cp->L!=l) continue;
		MIp->Attrib|=0x01;  // закрываем
		break;
	}
	RETURNV
}

void ResetA160(void){
	STARTNA(__LINE__, 0)
	SetMem(ClosedTowns,sizeof(ClosedTowns),0);
	RETURNV
}

int LoadA160(int /*ver*/)
{
	STARTNA(__LINE__, 0)
	ResetA160();
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='A'||buf[1]!='1'||buf[2]!='6'||buf[3]!='0') {MError("LoadA160 cannot start loading"); RETURN(1)}
	if(Loader(ClosedTowns,sizeof(ClosedTowns))) RETURN(1)
	RETURN(0)
}

int SaveA160(void)
{
	STARTNA(__LINE__, 0)
	if(Saver("A160",4)) RETURN(1)
	if(Saver(ClosedTowns,sizeof(ClosedTowns))) RETURN(1)
	RETURN(0)
}

__declspec( naked ) void SoDArtMenchHintFix(void)
{
	__asm{
		mov   edx,esi
		sub   edx,0x12 // slot index in the screen
		mov   eax,0x6AAAD8 // shift of the slot according a backpack
		add   edx,[eax]
		mov   ecx,0x6AAAE0
		mov   ecx,[ecx]
		ret
	}
}