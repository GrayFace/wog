#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "service.h"
#include "string.h"
#include "b1.h"
#include "erm.h"

#define __FILENUM__ 21
////////////////////////
static int ZVars[SPELLNUM][7]; // name,Abr Name,Descr[4],SoundName
static _Spell_ SpellsBackUp[SPELLNUM];

static char *GetText(int ind,int tp){
	STARTNA(__LINE__, 0)
	char *po=NULL;
	int vv=ZVars[ind][tp];
	if(vv) po = StrMan::GetStoredStr(vv);
	RETURN(po)
}

int ERM_Spell(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__, 0)
	int   ind,vv;
	//char *txt;
	_Spell_ *Spell=NULL;
	ind=GetVarVal(&sp->Par[0]);
	if(ind<0 || ind>=SPELLNUM){ MError("\"SS\"- incorrect spell index."); RETURN(0) }
	Spell=&Spells[ind];
//  ERMFlags[0]=0;
	switch(Cmd){
//  int   FRposNOzerENneg; // +0 -1=enemy, 0=square/global/area, 1=friend
		case 'O': // O$
			Apply(&Spell->FRposNOzerENneg,4,Mp,0);
			break;
//  char *SoundFileName;   // +4 
		case 'W': // W$
			StrMan::Apply(Spell->SoundFileName, SpellsBackUp->SoundFileName, ZVars[ind][6], Mp, 0);
			//if(Apply(&ZVars[ind][6],4,Mp,0)) break;
			//txt=GetText(ind,6); if(txt!=NULL) Spell->SoundFileName=txt;
			break;
//  int   DefIndex;        // +8 used to get DefName
		case 'X': // X$
			Apply(&Spell->DefIndex,4,Mp,0);
			break;
//  int   Flags;            // +C
		case 'F': // F$
			Apply(&Spell->Flags,4,Mp,0);
			break;
//  char  *Name;            // +10h
		case 'N': // N$
			StrMan::Apply(Spell->Name, SpellsBackUp->Name, ZVars[ind][0], Mp, 0);
			//if(Apply(&ZVars[ind][0],4,Mp,0)) break;
			//txt=GetText(ind,0); if(txt!=NULL) Spell->Name=txt;
			break;
//  char  *AbbrName;        // +14h
		case 'A': // A$
			StrMan::Apply(Spell->AbbrName, SpellsBackUp->AbbrName, ZVars[ind][1], Mp, 0);
			//if(Apply(&ZVars[ind][1],4,Mp,0)) break;
			//txt=GetText(ind,0); if(txt!=NULL) Spell->AbbrName=txt;
			break;
//  int    Level;           // +18h
		case 'L': // L$
			Apply(&Spell->Level,4,Mp,0);
			break;
//  int    SchoolBits;      // +1Ch Air=1,Fire=2,Water=4,Earth=8
		case 'S': // S$
			Apply(&Spell->SchoolBits,4,Mp,0);
			break;
//  int    Cost[4];         // +20h cost mana per skill level
		case 'C': // C#/$
			CHECK_ParamsMin(2);
			Apply(&vv,4,Mp,0);
			if(vv<0 || vv>3){ MError("\"SS:C\"- wrong index."); RETURN(0) }
			Apply(&Spell->Cost[vv],4,Mp,1);
			break;
//  int    Eff_Power;       // +30h
		case 'P': // P$
			Apply(&Spell->Eff_Power,4,Mp,0);
			break;
//  int    Effect[4];       // +34h effect per skill level
		case 'E': // E#/$
			CHECK_ParamsMin(2);
			Apply(&vv,4,Mp,0);
			if(vv<0 || vv>3){ MError("\"SS:E\"- wrong index."); RETURN(0) }
			Apply(&Spell->Effect[vv],4,Mp,1);
			break;
//  int    Chance2GetVar[9];// +44h chance per class
		case 'H': // H#/$
			CHECK_ParamsMin(2);
			Apply(&vv,4,Mp,0);
			if(vv<0 || vv>8){ MError("\"SS:H\"- wrong index."); RETURN(0) }
			Apply(&Spell->Chance2GetVar[vv],4,Mp,1);
			break;
//  int    AIValue[4];      // +68h 
		case 'I': // I#/$
			CHECK_ParamsMin(2);
			Apply(&vv,4,Mp,0);
			if(vv<0 || vv>3){ MError("\"SS:I\"- wrong index."); RETURN(0) }
			Apply(&Spell->AIValue[vv],4,Mp,1);
			break;
//  char  *Descript[4];     // +78h
		case 'D': //D#/$
			CHECK_ParamsMin(2);
			Apply(&vv,4,Mp,0);
			if(vv<0 || vv>3){ MError("\"SS:D\"- wrong index."); RETURN(0) }
			StrMan::Apply(Spell->Descript[vv], SpellsBackUp->Descript[vv], ZVars[ind][2+vv], Mp, 1);
			//if(Apply(&ZVars[ind][2+vv],4,Mp,1)) break;
			//txt=GetText(ind,2+vv); if(txt!=NULL) Spell->Descript[vv]=txt;
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

/*
	int   FRposNOzerENneg; // +0 -1=enemy, 0=square/global/area, 1=friend
	char *SoundFileName;   // +4 
	int   DefIndex;        // +8 used to get DefName
	int   Flags;            // +C
//0x00000001 - BF spell
//0x00000002 - MAP spell
//0x00000004 - Has a time scale (3 rounds or so)
//0x00000008 - Creature Spell
//0x00000010 - target - single stack
//0x00000020 - target - single shooting stack (???)
//0x00000040 - has a mass version at expert level
//0x00000080 - target - any location
//0x00000100 - 
//0x00000200 - 
//0x00000400 - Mind spell
//0x00000800 - friendly and has mass version
//0x00001000 - cannot be cast on SIEGE_WEAPON
//0x00002000 - Spell from Artifact
//0x00004000 -
//0x00008000 - AI 
//0x00010000 - AI area effect
//0x00020000 - AI
//0x00040000 - AI
//0x00080000 - AI number/ownership of stacks may be changed 
//0x00100000 - AI
	char  *Name;            // +10h
	char  *AbbrName;        // +14h
	int    Level;           // +18h
	int    SchoolBits;      // +1Ch Air=1,Fire=2,Water=4,Earth=8
	int    Cost[4];         // +20h cost mana per skill level
	int    Eff_Power;       // +30h
	int    Effect[4];       // +34h effect per skill level
	int    Chance2GetVar[9];// +44h chance per class
	int    AIValue[4];      // +68h 
	char  *Descript[4];     // +78h
*/

static int SpTraitsLoaded=0;
static void ParseSpell(TxtFile *fl,int ind,int line){
	STARTNA(__LINE__, 0)
	_Spell_ *sp=&Spells[ind];
	char *txt;
	int i,len;
	txt=ITxt(line, 0,fl); len=strlen(txt);
	sp->Name=new char[len+1]; strncpy(sp->Name,txt,len); sp->Name[len]=0;
	txt=ITxt(line, 1,fl); len=strlen(txt);
	sp->AbbrName=new char[len+1]; strncpy(sp->AbbrName,txt,len); sp->AbbrName[len]=0;
	txt=ITxt(line, 2,fl); sp->Level=atoi(txt);
	txt=ITxt(line, 3,fl); if(txt[0]!=0 && txt[0]!=' ') sp->SchoolBits |= 8;
	txt=ITxt(line, 4,fl); if(txt[0]!=0 && txt[0]!=' ') sp->SchoolBits |= 4;
	txt=ITxt(line, 5,fl); if(txt[0]!=0 && txt[0]!=' ') sp->SchoolBits |= 2;
	txt=ITxt(line, 6,fl); if(txt[0]!=0 && txt[0]!=' ') sp->SchoolBits |= 1;
	for(i=0;i<4;i++){ txt=ITxt(line,7+i,fl); sp->Cost[i]=atoi(txt); }
	txt=ITxt(line,11,fl); sp->Eff_Power=atoi(txt);
	for(i=0;i<4;i++){ txt=ITxt(line,12+i,fl); sp->Effect[i]=atoi(txt); }
	for(i=0;i<9;i++){ txt=ITxt(line,16+i,fl); sp->Chance2GetVar[i]=atoi(txt); }
	for(i=0;i<4;i++){ txt=ITxt(line,25+i,fl); sp->AIValue[i]=atoi(txt); }
	for(i=0;i<4;i++){
		txt=ITxt(line,29+i,fl); len=strlen(txt);
		sp->Descript[i]=new char[len+1]; strncpy(sp->Descript[i],txt,len); sp->Descript[i][len]=0;
	}
	RETURNV
}

int ParseSpTraitsTxt(void){
	STARTNA(__LINE__, 0)
	int i;
	if(SpTraitsLoaded){
		for(i=0;i<SPELLNUM;i++) Spells[i]=SpellsBackUp[i];
		RETURN(1)
	}
	TxtFile fl;
	if(LoadTXT("sptraits.txt",&fl)==-1){ MError("Cannot load Sptraits.txt"); RETURN(0) }
	if(fl.sn < (SPELLNUM+5+3+3+3)){ MError("Sptraits.txt - not enough spells"); UnloadTXT(&fl); RETURN(0) }
//  char *txt;
	// Adventure Spells
	for(i= 0;i<10;i++) ParseSpell(&fl,i,i+5);
	// Combat Spells
	for(i=10;i<70;i++) ParseSpell(&fl,i,i+5+3);
	// Creature Abilities
	for(i=70;i<81;i++) ParseSpell(&fl,i,i+5+3+3);
	// WoG Spells
	for(i=82;i<SPELLNUM;i++) ParseSpell(&fl,i,i+5+3+3+3);
	UnloadTXT(&fl);
	for(i=0;i<SPELLNUM;i++) SpellsBackUp[i]=Spells[i];
	SpTraitsLoaded=1;
	RETURN(1)
}
void ResetSpells(void){
	STARTNA(__LINE__, 0)
//  memset(Spells,0,sizeof(Spells));
//  memcpy(Spells,(void *)0x6854A0,sizeof(_Spell_)*SPELLNUM_0);
/*
	__asm{ // Reload Sptraits
		mov  eax,0x59E390
		call eax
	}
*/
	ParseSpTraitsTxt();
	memset(ZVars,0,sizeof(ZVars));
	RETURNV
}

int LoadSpells(int /*ver*/)
{
	STARTNA(__LINE__, 0)
	ResetSpells();
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='S'||buf[1]!='P'||buf[2]!='L'||buf[3]!='0') {MError("LoadSpell cannot start loading"); RETURN(1)}
	int i,j;
	char *txt;
	for(i=0;i<SPELLNUM;i++){
		_Spell_ *spl=&Spells[i];
		if(Loader(&spl->FRposNOzerENneg,sizeof(int))) RETURN(1)
		if(Loader(&spl->DefIndex,sizeof(int))) RETURN(1)
		if(Loader(&spl->Flags,sizeof(int))) RETURN(1)
		if(Loader(&spl->Level,sizeof(int))) RETURN(1)
		if(Loader(&spl->SchoolBits,sizeof(int))) RETURN(1)
		for(j=0;j<4;j++) if(Loader(&spl->Cost[j],sizeof(int))) RETURN(1)
		if(Loader(&spl->Eff_Power,sizeof(int))) RETURN(1)
		for(j=0;j<4;j++) if(Loader(&spl->Effect[j],sizeof(int))) RETURN(1)
		for(j=0;j<9;j++) if(Loader(&spl->Chance2GetVar[j],sizeof(int))) RETURN(1)
		for(j=0;j<4;j++) if(Loader(&spl->AIValue[j],sizeof(int))) RETURN(1)
		if(Loader(ZVars,sizeof(ZVars))) RETURN(1)
		txt=GetText(i,0); if(txt!=NULL) spl->Name=txt;
		txt=GetText(i,1); if(txt!=NULL) spl->AbbrName=txt;
		for(j=0;j<4;j++){
			txt=GetText(i,2+j); if(txt!=NULL) spl->Descript[j]=txt;
		}
		txt=GetText(i,6); if(txt!=NULL) spl->SoundFileName=txt;
//    if(Loader(spl-,sizeof(int))) RETURN(1)
	}
	RETURN(0)
}

int SaveSpells(void)
{
	STARTNA(__LINE__, 0)
	if(Saver("SPL0",4)) RETURN(1)
	int i,j;
//  char *txt;
	for(i=0;i<SPELLNUM;i++){
		_Spell_ *spl=&Spells[i];
		if(Saver(&spl->FRposNOzerENneg,sizeof(int))) RETURN(1)
		if(Saver(&spl->DefIndex,sizeof(int))) RETURN(1)
		if(Saver(&spl->Flags,sizeof(int))) RETURN(1)
		if(Saver(&spl->Level,sizeof(int))) RETURN(1)
		if(Saver(&spl->SchoolBits,sizeof(int))) RETURN(1)
		for(j=0;j<4;j++) if(Saver(&spl->Cost[j],sizeof(int))) RETURN(1)
		if(Saver(&spl->Eff_Power,sizeof(int))) RETURN(1)
		for(j=0;j<4;j++) if(Saver(&spl->Effect[j],sizeof(int))) RETURN(1)
		for(j=0;j<9;j++) if(Saver(&spl->Chance2GetVar[j],sizeof(int))) RETURN(1)
		for(j=0;j<4;j++) if(Saver(&spl->AIValue[j],sizeof(int))) RETURN(1)
		if(Saver(ZVars,sizeof(ZVars))) RETURN(1)
	}
	RETURN(0)
}
