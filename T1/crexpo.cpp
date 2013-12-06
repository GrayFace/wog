#include <windows.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "b1.h"
//#include "casdem.h"
#include "service.h"
//#include "herospec.h"
#include "monsters.h"
//#include "anim.h"
//#include "womo.h"
//#include "sound.h"
#include "npc.h"
#include "erm.h"
#include "erm_lua.h"
//#include "ai.h"
#include "CrExpo.h"
#define __FILENUM__ 15

static TxtFile CrExpTXT;
///////////////////////////
static char _E2S_String[100];
char *Exp2String(int exp,int Limit,int HasArt){
	STARTNA(__LINE__, 0)
	int i;
	_E2S_String[0]=0;
	char *sart=""; 
	switch(HasArt){
		case 1: sart="\x19"; break;
		case 2: sart="{\x1A}"; break;
		case 3: sart="{\x1B}"; break;
		case 4: sart="{\x1C}"; break;
	}
//  if(HasArt==1) sart="\x19";
//  else if(HasArt>1) sart="{\x1A}";
	char *first="",second[20]; second[0]=0;
	if(exp>=Limit){ // > максимума
		first="{\x18\x18}"; 
	}else{
		i=CrExpMod::GetLimRank(exp,Limit);
		if(i>=5){ // есть вторая цифра
			first="{\x18}";
			i-=5;
		}
		if(i!=0){
			sprintf(second,"%c",i-1+0x14);
		}
	}
	sprintf(_E2S_String,"%s%s%s\x0A",sart,first,second);
	RETURN(_E2S_String)
}

char *Exp2String2(int exp[2],int Limit,int HasArt[2]){
	STARTNA(__LINE__, 0)
	int i/*,j,cexp*/;
	_E2S_String[0]=0;
	char *sart="",*finish=""; 
	switch(HasArt[0]){
		case 1: sart="\x19"; break;
		case 2: sart="{\x1A}"; break;
		case 3: sart="{\x1B}"; break;
		case 4: sart="{\x1C}"; break;
	}
	switch(HasArt[1]){
		case 1: finish="\x19"; break;
		case 2: finish="{\x1A}"; break;
		case 3: finish="{\x1B}"; break;
		case 4: finish="{\x1C}"; break;
	}
//  if(HasArt[0]==1) sart="\x19"; else if(HasArt[0]>1) sart="{\x1A}";
//  if(HasArt[1]==1) finish="\x19"; else if(HasArt[1]>1) finish="{\x1A}";
	char *first="",second[20]; second[0]=0;
	char *first2="",second2[20]; second2[0]=0;
	if(exp[0]>=Limit){ // > максимума
		first="{\x18\x18}"; 
	}else{
		i=CrExpMod::GetLimRank(exp[0],Limit);
		if(i>=5){ // есть вторая цифра
			first="{\x18}";
			i-=5;
		}
		if(i!=0){
			sprintf(second,"%c",i-1+0x14);
		}
	}
	if(exp[1]>=Limit){ // > максимума
		first2="{\x18\x18}"; 
	}else{
		i=CrExpMod::GetLimRank(exp[1],Limit);
		if(i>=5){ // есть вторая цифра
			first2="{\x18}";
			i-=5;
		}
		if(i!=0){
			sprintf(second2,"%c",i-1+0x14);
		}
	}
	sprintf(_E2S_String,"%s%s%s  <=>  %s%s%s",sart,first,second,first2,second2,finish);

/*
	memset(_E2S_String,0,sizeof(_E2S_String));
	for(j=0;j<2;j++){
		cexp=exp[j];
		do{
			if(cexp>=Limit){ // > максимума
				strcat((char *)_E2S_String,"{\x18\x18}");
				break;
			}
			i=CrExpMod::GetLimRank(cexp,Limit);
			if(i>5){ // есть вторая цифра
				strcat((char *)_E2S_String,"{\x18}");
				i-=5;
				if(i!=0){
					_E2S_String[strlen((char *)_E2S_String)]=(Byte)(i-1+0x14);
				}
				break;
			}
			if(i!=0) _E2S_String[strlen((char *)_E2S_String)]=(Byte)(i-1+0x14);
		}while(0);
		if(j==0) strcat((char *)_E2S_String,"  <=>  ");
	}
	_E2S_String[strlen((char *)_E2S_String)]=0;
*/
	RETURN((char *)_E2S_String)
}
static int Upgraded(int Mon){
	STARTNA(__LINE__, 0)
	int rm=-1;
	if((Mon>=0)&&(Mon<MONNUM)){
		if(MonsterUpgradeTable[Mon]==-2) rm=0;
		else rm=MonsterUpgradeTable[Mon];
	}
	if(rm==-1){
		__asm{
			mov    ecx,Mon
			mov    eax,0x47AAD0
			call   eax
			mov    rm,eax
		}
	}
	RETURN(rm)
}
///////////////////////////////
CrExpo::~CrExpo(){ Clear(); }
//CrExpo::CrExpo(CrExpo &){ TError("CrExpo: Copy Ctor is disabled!"); }
CrExpo &CrExpo::operator=(CrExpo &src){
	STARTNA(__LINE__, 0)
	memcpy((void *)this,(void *)&src,sizeof(CrExpo));
	Check4Max(); // check for max experience
	RETURN(*this)
}

CrExpo::CrExpo() { Clear(); }
void CrExpo::Check4Max(){
	// check for max experience
	STARTNA(__LINE__, 0)
	if(Expo>(EXPOMUL1*CrExpMod::CrMaxExpo(this))) Expo=EXPOMUL1*CrExpMod::CrMaxExpo(this);
	RETURNV
}
int  CrExpo::RealType(int type){
	STARTNA(__LINE__, 0)
	switch(Fl.Type){
		case CE_HERO:{
			if((Dt.Hero.Ind<0)||(Dt.Hero.Ind>=HERNUM)){ TError("CrExpo: Incorrect Hero index."); RETURN(-1) }
			if((Dt.Hero.Slot<0)||(Dt.Hero.Slot>6)){ TError("CrExpo: Incorrect Hero Slot index."); RETURN(-1) }
			_Hero_ *hp=GetHeroStr(Dt.Hero.Ind);
			if(type==-2){ // get
				RETURN(hp->Ct[Dt.Hero.Slot])
			}else{ // set
				hp->Ct[Dt.Hero.Slot]=type;
				RETURN(0)
			}
		}
		case CE_MAP:{
			_MapItem_ *mp=GetMapItem2(Dt.MixPos);
			if(mp==0){ TError("CrExpo: Cannot get map position."); RETURN(-1) }
			if(mp->OType!=54){ TError("CrExpo: Not a Monster at map position."); RETURN(-1) }
			if(type==-2){ // get
				RETURN(mp->OSType)
			}else{ // set
				mp->OSType=(Word)type;
				RETURN(0)
			}
		}
		case CE_TOWN:{
			_CastleSetup_ *cp=FindCastle(Dt.Town.X,Dt.Town.Y,Dt.Town.L);
			if(cp==0){ TError("CrExpo: Cannot find town."); RETURN(-1) }
			if((Dt.Town.Slot<0)||(Dt.Town.Slot>6)){ TError("CrExpo: Incorrect Town Slot index."); RETURN(-1) }
			if(type==-2){ // get
				RETURN(cp->GuardsT[Dt.Town.Slot])
			}else{ // set
				cp->GuardsT[Dt.Town.Slot]=type;
				RETURN(0)
			}
		}
		case CE_MINE:{
			_Mine_ *cp=FindMine(Dt.Mine.X,Dt.Mine.Y,Dt.Mine.L);
			if(cp==0){ TError("CrExpo: Cannot find mine."); RETURN(-1) }
			if((Dt.Mine.Slot<0)||(Dt.Mine.Slot>6)){ TError("CrExpo: Incorrect Mine Slot index."); RETURN(-1) }
			if(type==-2){ // get
				RETURN(cp->GType[Dt.Mine.Slot])
			}else{ // set
				cp->GType[Dt.Mine.Slot]=type;
				RETURN(0)
			}
		}
		case CE_HORN:{
			_Horn_ *cp=FindHorn(Dt.Horn.X,Dt.Horn.Y,Dt.Horn.L);
			if(cp==0){ TError("CrExpo: Cannot find horn."); RETURN(-1) }
			if((Dt.Horn.Slot<0)||(Dt.Horn.Slot>6)){ TError("CrExpo: Incorrect Horn Slot index."); RETURN(-1) }
			if(type==-2){ // get
				RETURN(cp->GType[Dt.Horn.Slot])
			}else{ // set
				cp->GType[Dt.Horn.Slot]=type;
				RETURN(0)
			}
		}
		default:
			TError("CrExpo: Unknown stack type.");
			RETURN(-1)
	}
//  RETURN(-1)
}
int  CrExpo::RealNum(int num){
	STARTNA(__LINE__, 0)
	switch(Fl.Type){
		case CE_HERO:{
			if((Dt.Hero.Ind<0)||(Dt.Hero.Ind>=HERNUM)){ TError("CrExpo: Incorrect Hero index."); RETURN(-1) }
			if((Dt.Hero.Slot<0)||(Dt.Hero.Slot>6)){ TError("CrExpo: Incorrect Hero Slot index."); RETURN(-1) }
			_Hero_ *hp=GetHeroStr(Dt.Hero.Ind);
			if(num==-2){ // get
				RETURN(hp->Cn[Dt.Hero.Slot])
			}else{
				hp->Cn[Dt.Hero.Slot]=num;
				RETURN(0)
			}
		}
		case CE_MAP:{
			_MapItem_ *mp=GetMapItem2(Dt.MixPos);
			if(mp->OType!=54){ TError("CrExpo: Not a Monster at map position."); RETURN(-1) }
			_CMonster_ *mo=(_CMonster_ *)&mp->SetUp;
			if(num==-2){ // get
				RETURN(mo->Number)
			}else{
				mo->Number=(Word)num;
				RETURN(0)
			}
		}
		case CE_TOWN:{
			_CastleSetup_ *cp=FindCastle(Dt.Town.X,Dt.Town.Y,Dt.Town.L);
			if(cp==0){ TError("CrExpo: Cannot find town."); RETURN(-1) }
			if((Dt.Town.Slot<0)||(Dt.Town.Slot>6)){ TError("CrExpo: Incorrect Town Slot index."); RETURN(-1) }
			if(num==-2){ // get
				RETURN(cp->GuardsN[Dt.Town.Slot])
			}else{ // set
				cp->GuardsN[Dt.Town.Slot]=num;
				RETURN(0)
			}
		}
		case CE_MINE:{
			_Mine_ *cp=FindMine(Dt.Mine.X,Dt.Mine.Y,Dt.Mine.L);
			if(cp==0){ TError("CrExpo: Cannot find mine."); RETURN(-1) }
			if((Dt.Mine.Slot<0)||(Dt.Mine.Slot>6)){ TError("CrExpo: Incorrect Mine Slot index."); RETURN(-1) }
			if(num==-2){ // get
				RETURN(cp->GNumb[Dt.Mine.Slot])
			}else{ // set
				cp->GNumb[Dt.Mine.Slot]=num;
				RETURN(0)
			}
		}
		case CE_HORN:{
			_Horn_ *cp=FindHorn(Dt.Horn.X,Dt.Horn.Y,Dt.Horn.L);
			if(cp==0){ TError("CrExpo: Cannot find horn."); RETURN(-1) }
			if((Dt.Horn.Slot<0)||(Dt.Horn.Slot>6)){ TError("CrExpo: Incorrect Horn Slot index."); RETURN(-1) }
			if(num==-2){ // get
				RETURN(cp->GNumb[Dt.Horn.Slot])
			}else{ // set
				cp->GNumb[Dt.Horn.Slot]=num;
				RETURN(0)
			}
		}
		default:
			TError("CrExpo: Unknown stack type.");
			RETURN(-1)
	}
//  RETURN(-1)
}
void CrExpo::SetN(int Type,CRLOC Data,int mtype,int num,int expo,int arts){
	STARTNA(__LINE__, 0)
	Fl.Act=0;
	Num=num;
	Expo=expo;
	Dt.UniData=Data;
	Fl.Type=(Word)Type;
	Fl.MType=(Word)mtype;
	DelArt(); AddArt(arts);
	switch(Type){
		case CE_HERO:
			if((Dt.Hero.Ind<0)||(Dt.Hero.Ind>=HERNUM)){ TError("CrExpo: Incorrect Hero index."); RETURNV }
			if((Dt.Hero.Slot<0)||(Dt.Hero.Slot>6)){ TError("CrExpo: Incorrect Hero Slot index."); RETURNV }
			break;
		case CE_MAP:{
			_MapItem_ *mp=GetMapItem2(Dt.MixPos);
			if(mp->OType!=54){ TError("CrExpo: Not a Monster at map position."); RETURNV }
			if(mp->OSType>=MONNUM){ TError("CrExpo: Incorrect Monster Subtype."); RETURNV }
			break;
		}
		case CE_TOWN:{
			_CastleSetup_ *cp=FindCastle(Dt.Town.X,Dt.Town.Y,Dt.Town.L);
			if(cp==0){ TError("CrExpo: Cannot find town."); RETURNV }
			if((Dt.Town.Slot<0)||(Dt.Town.Slot>6)){ TError("CrExpo: Incorrect Town Slot index."); RETURNV }
			break;
		}
		case CE_MINE:{
			_Mine_ *cp=FindMine(Dt.Mine.X,Dt.Mine.Y,Dt.Mine.L);
			if(cp==0){ TError("CrExpo: Cannot find mine."); RETURNV }
			if((Dt.Mine.Slot<0)||(Dt.Mine.Slot>6)){ TError("CrExpo: Incorrect Mine Slot index."); RETURNV }
			break;
		}
		case CE_HORN:{
			_Horn_ *cp=FindHorn(Dt.Horn.X,Dt.Horn.Y,Dt.Horn.L);
			if(cp==0){ TError("CrExpo: Cannot find horn."); RETURNV }
			if((Dt.Horn.Slot<0)||(Dt.Horn.Slot>6)){ TError("CrExpo: Incorrect Horn Slot index."); RETURNV }
			break;
		}
///    default:
///      TError("CrExpo: Unknown stack type.");
///      RETURNV
	}
	Fl.Act=1;
	RETURNV
}
void CrExpo::Clear(){
	STARTNA(__LINE__, 0)
	FillMem(this,sizeof(CrExpo),0);
	Fl.Act=0;
	RETURNV
}
void CrExpo::RecalcExp2RealNum(int n,int type){
	STARTNA(__LINE__, 0)
	if(Fl.Act==0) RETURNV
//  if(n!=Num){ Expo=Expo*n/Num; Num=n; }
	if(type!=-1){ // тип существ сменился 
		if(type!=194){ // и не верволф!!!!
			if(Fl.MType!=(unsigned)type){
				Expo=0;
				Fl.MType=(Word)type; // установим новый тип, чтобы дальше накапливалось
			}
		}else{ // верволф
			Fl.MType=(Word)type; // установим новый тип, и пересчитаем
		}
	}
	if(n==-1) n=RealNum();
	if(n==0){ Expo=0; RETURNV }//?????
//  if(Num==0) return;
	if(n<=Num){ Num=n; RETURNV }
	if(n>=Num){ Expo=Expo*Num/n; Num=n; RETURNV }
	RETURNV
}
int CrExpo::Validate(void){
	STARTNA(__LINE__, 0)
	while(1){
		if(Fl.Act==0) break;
//    if(Expo==0) break;
		if(Num<=0) break;
		if(Fl.Type==0) break;
		if(Fl.MType==(Word)-1) break;
//    RecalcExp2RealNum(-1,-1);
		Check4Max();
		RETURN(0)
	}
	Clear();
	RETURN(1)
}
/////////////////////
CrExpo  CrExpoSet::Body[MAXCRINFONUM];
//CrExpo  CrExpoSet::Cur;
CrExpo  CrExpoSet::BFBody[42];

int CrExpoSet::FindIt(int Type,CRLOC Data){
	STARTNA(__LINE__, 0)
	if(Type==0) RETURN(-1)
//  if(Data==0) RETURN(-1)
	for(int i=0;i<MAXCRINFONUM;i++){
		if(Body[i].Fl.Act==0) continue;
		if(Body[i].Fl.Type!=(unsigned)Type) continue;
		if(Body[i].Dt.UniData!=Data) continue;
		RETURN(i)
	}
	RETURN(-1)
}
CrExpo *CrExpoSet::Find(int Type,CRLOC Data){
	STARTNA(__LINE__, 0)
//asm int 3
	int i=FindIt(Type,Data);
	if(i==-1) RETURN(0)
	RETURN(&Body[i])
};
void CrExpoSet::Del(int Type,CRLOC Data){
	STARTNA(__LINE__, 0)
	int i=FindIt(Type,Data);
	if(i==-1) RETURNV
	Body[i].Clear();
	RETURNV
};
void CrExpoSet::Clear(){
	STARTNA(__LINE__, 0)
	for(int i=0;i<MAXCRINFONUM;i++) Body[i].Clear();
	// impossible
	AIMult[4]=500; AIBase[4]=400; AITMult[4]=100;
	// expert
	AIMult[3]=400; AIBase[3]=300; AITMult[3]=75;
	// hard
	AIMult[2]=300; AIBase[2]=200; AITMult[2]=50;
	// normal
	AIMult[1]=200; AIBase[1]=100; AITMult[1]=25;
	// easy
	AIMult[0]=100; AIBase[0]=50; AITMult[0]=10;
	PlayerMult=100;
	StopAllBF();
	RETURNV
}
int CrExpoSet::Load(int /*ver*/){
	STARTNA(__LINE__, 0)
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='C'||buf[1]!='R'||buf[2]!='E'||buf[3]!='X')
			{TError("CrExpoSet cannot start loading"); RETURN(1)}
	if(Loader(Body,sizeof(Body))) RETURN(1)
	if(Loader(&LastUser,sizeof(LastUser))) RETURN(1)
	if(Loader(HerExpo,sizeof(HerExpo))) RETURN(1)
	if(Loader(AIMult,sizeof(AIMult))) RETURN(1)
	if(Loader(AIBase,sizeof(AIBase))) RETURN(1)
	if(Loader(AITMult,sizeof(AITMult))) RETURN(1)
	if(Loader(&PlayerMult,sizeof(PlayerMult))) RETURN(1)
//  if(Loader(HerArts,sizeof(HerArts))) RETURN(1)
	StopAllBF();
	RETURN(0)
}
int CrExpoSet::Save(void){
	STARTNA(__LINE__, 0)
	if(Saver("CREX",4)) RETURN(1)
	if(Saver(Body,sizeof(Body))) RETURN(1)
	if(Saver(&LastUser,sizeof(LastUser))) RETURN(1)
	if(Saver(HerExpo,sizeof(HerExpo))) RETURN(1)
	if(Saver(AIMult,sizeof(AIMult))) RETURN(1)
	if(Saver(AIBase,sizeof(AIBase))) RETURN(1)
	if(Saver(AITMult,sizeof(AITMult))) RETURN(1)
	if(Saver(&PlayerMult,sizeof(PlayerMult))) RETURN(1)
//  if(Saver(HerArts,sizeof(HerArts))) RETURN(1)
	RETURN(0)
}
CrExpo *CrExpoSet::FindEmpty(void)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<MAXCRINFONUM;i++){
		if(Body[i].Fl.Act!=0) continue;
		RETURN(&Body[i])
	}
	RETURN(0) // не нашли и нет места
}
int CrExpoSet::SetN(int Type,CRLOC Data,int mtype,int num,int expo){
	STARTNA(__LINE__, 0)
	int ind=FindIt(Type,Data);
	if(ind!=-1){ // нашли - добавляем
		Body[ind].Clear();
		Body[ind].SetN(Type,Data,mtype,num,expo);
		Body[ind].Check4Max(); // check for max experience
		RETURN(ind)
	}else{
		if(Type==0) RETURN(-1) // приемный стэк неопределенного типа
		for(int i=0;i<MAXCRINFONUM;i++){
			if(Body[i].Fl.Act!=0) continue;
			Body[i].SetN(Type,Data,mtype,num,expo);
			Body[i].Check4Max(); // check for max experience
			RETURN(i)
		}
	}
	RETURN(-1) // не нашли и нет места
}
int CrExpoSet::Set(CrExpo &cr){
	STARTNA(__LINE__, 0)
	if(&cr==0) RETURN(-1)
	int ind=FindIt(cr.Fl.Type,cr.Dt.UniData);
	if(ind!=-1){ // нашли - добавляем
		Body[ind].Clear();
		Body[ind]=cr;
		RETURN(ind)
	}else{
		for(int i=0;i<MAXCRINFONUM;i++){
			if(Body[i].Fl.Act!=0) continue;
			Body[i]=cr;
			RETURN(i)
		}
	}
	RETURN(-1) // не нашли и нет места
}

// получить опыт
int CrExpoSet::GetExp(int Type,CRLOC Data){
	STARTNA(__LINE__, 0)
	CrExpo *cr=Find(Type,Data);
	if(cr==0) RETURN(0)
	if(cr->Num==0) RETURN (0)
	cr->Check4Max();
	RETURN(cr->Expo)
}
int CrExpoSet::GetExpM(int Type,CRLOC Data,int M){
	STARTNA(__LINE__, 0)
	CrExpo *cr=Find(Type,Data);
	if(cr==0) RETURN(0)
	int exp=GetExp(Type,Data);
	if(M>=10){ exp=CrExpMod::GetRank(cr); }
	RETURN(exp)
}

void CrExpoSet::AddExpo(_Hero_ *Hp,int NewHExp,int OldHExp){
/*
	int slot,side;
	Byte *bm,*mo;
	asm{
		mov  eax,0x699420
		mov  eax,[eax]
		mov  bm,eax
	}
	if((Dword)Hp==*(Dword *)&bm[0x53CC])
	for(int i=0;i<(21*2);i++){
		mo=&bm[0x54CC+0x548*i];
		if()
		slot=*(int *)&mo[0x5C];

	}
*/
	STARTNA(__LINE__, 0)
	//if(!WoG) RETURNV
	if(!PL_CrExpEnable) RETURNV
	if(PL_ExpGainDis) RETURNV
	if(Hp==0) RETURNV
// do not support AI experience yet
	if(IsAI(Hp->Owner)) RETURNV
	CrExpo* cr;
	int i,j,val;
	float AllW=0,ExpW[7];
	for(i=0;i<7;i++){
		ExpW[i]=0;
		if((Hp->Cn[i]==0)||(Hp->Ct[i]==-1)) continue; // нет стэка или умер
		switch(PL_CrExpStyle){
			case 1: // Timothy
				ExpW[i]=0.9f+(float)Hp->Cn[i]*(7-MonTable[Hp->Ct[i]].SubGroup)*CrExpMod::ExpMul(Hp->Ct[i]);
				AllW+=ExpW[i];
				break;
			case 2: // QQD
				ExpW[i]=0.9f+(float)Hp->Cn[i]*((MonTable[Hp->Ct[i]].SubGroup+1)*100+GetExp(CE_HERO,MAKEHS(Hp->Number,i))/EXPOMUL1 +10)*CrExpMod::ExpMul(Hp->Ct[i])/100;
				AllW+=ExpW[i];
				break;
			case 3: // equal
				ExpW[i]=Hp->Cn[i]*CrExpMod::ExpMul(Hp->Ct[i]);
				AllW+=ExpW[i];
				break;
			case 0: // equal without number
			default:
				ExpW[i]=CrExpMod::ExpMul(Hp->Ct[i]);
		}
//    AllW+=ExpW[i];
	}
	if(AllW==0) AllW=1;
	for(i=0;i<7;i++){
		if((Hp->Cn[i]==0)||(Hp->Ct[i]==-1)){ // нет стэка или умер
			cr=Find(CE_HERO,MAKEHS(Hp->Number,i));
			if(cr!=0 && cr->HasArt()){ // арт был, но стэк умер - передадим герою
				for(j=0;j<=cr->ArtCopy();j++) EquipArtifact(Hp,EXPARTFIRST,-1);
			}
			Del(CE_HERO,MAKEHS(Hp->Number,i));
			continue;
		}
		cr=Find(CE_HERO,MAKEHS(Hp->Number,i));
		if(cr==0){ // не было
//      Set(CE_HERO,MAKEHS(Hp->Number,i),Hp->Ct[i],Hp->Cn[i],(NewHExp-OldHExp)*Hp->Cn[i]*5);
			SetN(CE_HERO,MAKEHS(Hp->Number,i),Hp->Ct[i],Hp->Cn[i],0);
			cr=Find(CE_HERO,MAKEHS(Hp->Number,i));
		}else{ // был
			cr->RecalcExp2RealNum(-1,Hp->Ct[i]);
//      if(cr->MType!=Hp->Ct[i]){ // стэк-то другой
//        cr->Expo=0; // обнулим
//      }
//      cr->AddExpRel((NewHExp-OldHExp)*5);
		}
		switch(PL_CrExpStyle){
			case 0:
				val=CrExpMod::CapIt(cr,(int)((float)NewHExp-OldHExp)*CrExpoSet::PlayerMult*HEREXPMUL*ExpW[i]/AllW/100.0f+0.0001f);
				break;
			default:
				val=CrExpMod::CapIt(cr,((float)NewHExp-OldHExp)*CrExpoSet::PlayerMult*HEREXPMUL*ExpW[i]/AllW/Hp->Cn[i]/100.0f+0.0001f);
		}
		if(cr!=0 && cr->HasArt() && cr->GetSubArt()==5 ){ // Art Sub 5 - +50% experience
			val=val*1.5; 
		}
		cr->Expo+=val;

		cr=Find(CE_HERO,MAKEHS(Hp->Number,i));
		cr->Check4Max(); // check for max experience
	}
	RETURNV
}

static void ApplyExpo(int *Exp,int E,int M,int Type,int T,int Num,int N){
	STARTNA(__LINE__, 0)
	switch(M){
		case 0: // добавляем/устанавливаем существ, у которых задан опыт только если те же
			if(N==0){ *Exp=0; break; }
			if(Type!=T){ *Exp=E; break; }
			*Exp=(*Exp*Num+E*(N-Num))/N; break;
		case 1: // добавляем/устанавливаем существ, у которых задан опыт
			if(N==0){ *Exp=0; break; }
			*Exp=(*Exp*Num+E*(N-Num))/N; break;
		case 2: // устанавливаем всем этот опыт
			*Exp=E; break;
		case 3: // новые существа при замене берут опыт пред. существ плюс заданный
			*Exp+=E; break;
		case 4: // новые существа при замене берут опыт пред. существ плюс заданный УРОВЕНЬ
			*Exp=CrExpMod::Exp4Level(T,E,*Exp); break;
		case 5: // новые существа при замене берут опыт пред. существ плюс заданный
			*Exp=int(*Exp*CrExpMod::UpgrMul(Type)+E); break;

		case 10: // добавляем/устанавливаем существ, у которых задан УРОВЕНЬ опыта
			if((Num+N)==0){ *Exp=0; break; }
			if(Type!=T){ *Exp=CrExpMod::Exp4Level(T,E,0); break; }
			*Exp=(*Exp*Num+CrExpMod::Exp4Level(T,E,0)*(N-Num))/N; break;
		case 11: // добавляем/устанавливаем существ, у которых задан УРОВЕНЬ опыта
			if(N==0){ *Exp=0; break; }
			*Exp=(*Exp*Num+CrExpMod::Exp4Level(T,E,0)*(N-Num))/N; break;
		case 12: // устанавливаем всем этот УРОВЕНЬ опыта (0...10,11)
			*Exp=CrExpMod::Exp4Level(T,E,0); break;
		case 13: // новые существа при замене берут УРОВЕНЬ опыта пред. существ плюс заданный
			*Exp=*Exp*CrExpMod::Limit(T)/CrExpMod::Limit(Type)+E; break;
		case 14: // новые существа при замене берут УРОВЕНЬ опыта пред. существ плюс заданный УРОВЕНЬ
			*Exp=*Exp*CrExpMod::Limit(T)/CrExpMod::Limit(Type);
			*Exp=CrExpMod::Exp4Level(T,E,*Exp); break;
	}
	RETURNV
}

int CrExpoSet::Modify(int Tp,int Type,CRLOC Crloc,int E,int M,int T,int ON,int N,_MonArr_ *Ma)
{
	STARTNA(__LINE__, 0)
	CrExpo *Cr,*CrN;
	CRLOC   Crl;
	int i,/*t,tn,*/ind =FindIt(Type,Crloc);
	switch(Tp){
/*
		case 1: // upgrade E
			if(ind==-1) break;
			t=Body[ind].Fl.MType;    if(t==-1) break;
			tn=Upgraded(t);          if(tn==-1) break;
//      Body[ind].RecalcExp2RealNum(-1,-1);
			Body[ind].Expo*=CrExpMod::UprgMul(t); //recalc experience
			Body[ind].Fl.MType=(Word)tn;
			ApplyExpo(&Body[ind].Expo,E);
			Body[ind].Validate();
		case 2: // degrade T,E
			if(ind==-1) break;
//      Body[ind].RecalcExp2RealNum(-1,-1);
			Body[ind].Fl.MType=(Word)T;
			ApplyExpo(&Body[ind].Expo,E);
			Body[ind].Validate();
			break;
		case 3: // set type T,E
			if(ind==-1) break;
//      Body[ind].RecalcExp2RealNum(-1,-1);
			Body[ind].Fl.MType=(Word)T;
			ApplyExpo(&Body[ind].Expo,E);
			Body[ind].Validate();
			break;
		case 4: // set num N,E
			if(ind==-1) break;
//      Body[ind].RecalcExp2RealNum(-1,-1);
			Body[ind].Num=N;
			ApplyExpo(&Body[ind].Expo,E);
			Body[ind].Validate();
			break;
*/
		case 5: // set type and num T,N,E
			if(ind!=-1){
				ApplyExpo(&Body[ind].Expo,E,M,Body[ind].Fl.MType,T,Body[ind].Num,N);
				Body[ind].Fl.MType=(Word)T;
				Body[ind].Num=N;
				Body[ind].Validate();
			}else{
//        if(E==0) break;
				Cr=CrExpoSet::FindEmpty();
				if(Cr==0){ TError("CrExpoSet - Modify: no room in experience story."); RETURN(0) }
				Cr->Clear();
				Cr->SetN(Type,Crloc,T,N,0);
				ApplyExpo(&Cr->Expo,E,M,T,T,ON,N);
				Cr->Validate();
			}
			break;
		case 6: // add to any available stack T,N,E
			if(Ma==0) break;
			for(i=0;i<7;i++){
				if(Ma->Ct[i]!=T) continue;
				if(Type==CE_HERO){
					Crl=MAKEHS(((CrExpo::_Dt::_Hero *)&Crloc)->Ind,i);
				}
				Cr=Find(Type,Crl);
				if(Cr!=0){
					if(Cr->Fl.MType!=(Word)T){ // тип сменился
						int Exp=Cr->Expo;
						ApplyExpo(&Exp,E,M,Cr->Fl.MType,T,Cr->Num,Ma->Cn[i]);
//            Cr->Clear(); // удалим старый
						CrN=CrExpoSet::FindEmpty();
						if(CrN==0){ TError("CrExpoSet - Modify: no room in experience story."); RETURN(0) }
						CrN->Clear();
						CrN->SetN(Type,Crl,T,Ma->Cn[i],0);
						CrN->Expo=Exp;
						CrN->Validate();
						CrN->AddArt(*Cr);
						Cr->Clear(); // удалим старый
						break;
					}
//          Cr->RecalcExp2RealNum(-1,-1);
					ApplyExpo(&Cr->Expo,E,M,T,T,Cr->Num,Ma->Cn[i]);
					Cr->Num=Ma->Cn[i];
					Cr->Validate();
				}else{
					Cr=CrExpoSet::FindEmpty();
					if(Cr==0){ TError("CrExpoSet - Modify: no room in experience story."); RETURN(0) }
					Cr->Clear();
					Cr->SetN(Type,Crl,T,Ma->Cn[i],0);
					ApplyExpo(&Cr->Expo,E,M,T,T,0,Ma->Cn[i]);
					Cr->Validate();
				}
				break;
			}
			break;
		default:
			TError("CrExpoSet - Modify: unknown action type");
	}
	RETURN(0)
}

// перенести стэк из одного слота в другой
int CrExpoSet::HMove(int Type,int TypeN,CRLOC Data,CRLOC DataN,int type,int typeN,int num,int numN){
	STARTNA(__LINE__, 0)
	int ind =FindIt(Type,Data);
	int ind1=FindIt(TypeN,DataN);
	if(ind!=-1){
		Body[ind].RecalcExp2RealNum(num,type);
		Body[ind].Fl.Type=(Word)TypeN;
		Body[ind].Dt.UniData=DataN;
		if(Body[ind].Num==0){
//      Body[ind].Exp=0;
			Del(TypeN,DataN); // нет монстров - нечего хранить
		}else{
			Body[ind].Check4Max(); // check for max experience
			Body[ind].Expo=Body[ind].Expo*num/Body[ind].Num; Body[ind].Num=num;
		}
	}
	if(ind1!=-1){
		Body[ind1].RecalcExp2RealNum(numN,typeN);
		Body[ind1].Fl.Type=(Word)Type;
		Body[ind1].Dt.UniData=Data;
		if(Body[ind1].Num==0){
//      Body[ind1].Exp=0;
			Del(Type,Data); // нет монстров - нечего хранить
		}else{
			Body[ind1].Check4Max(); // check for max experience
			Body[ind1].Expo=Body[ind1].Expo*numN/Body[ind1].Num; Body[ind1].Num=numN;
		}
	}
/*
	//Ничего нет - добавим
	if(num!=0){
		for(int i=0;i<MAXCRINFONUM;i++){
			if(Body[i].Fl.Act!=0) continue;
			Body[i].Set(TypeN,DataN,num,0);
			RETURN(0)
		}
	}
	if(numN!=0){
		for(int i=0;i<MAXCRINFONUM;i++){
			if(Body[i].Fl.Act!=0) continue;
			Body[i].Set(Type,Data,numN,0);
			RETURN(0)
		}
	}
	RETURN(-1) // не нашли и нет места
*/
	RETURN(0)
}

// добавить первый стэк ко второму
int CrExpoSet::HComb(int Type,int TypeN,CRLOC Data,CRLOC DataN,int type,int typeN,int num,int numN){
	STARTNA(__LINE__, 0)
	int expo,anum/*,mtype*/;
	int ind =FindIt(Type,Data);
	int ind1=FindIt(TypeN,DataN);
	if((ind==-1)&&(ind1==-1)) RETURN(0) // нет обоих-выходим
	if(TypeN==0) RETURN(0) // приемный стэк неопределенного типа
//  if(ind!=-1 && ind1!=-1){
//    if(Body[ind].HasArt() && Body[ind1].HasArt()){ // два артифакта
//      if(Type==CE_HERO)
			//?????????????????????????
//    }
//  }
	for(int i=0;i<MAXCRINFONUM;i++){
		if(Body[i].Fl.Act!=0) continue;
		Body[i].Clear();
		expo=0;/* mtype=-1;*/
		if(ind!=-1){ // есть иточник
			Body[ind].RecalcExp2RealNum(num,type);
			Body[ind].Check4Max(); // check for max experience
			expo=Body[ind].Expo*num;
//      anum=Body[ind].Num;
//      mtype=Body[ind].Fl.MType;
		}else{
//      anum=num;
		}
		if(ind1!=-1){ // есть приемник
			Body[ind1].RecalcExp2RealNum(numN,typeN);
			Body[ind1].Check4Max(); // check for max experience
			expo+=Body[ind1].Expo*numN;
//      anum+=Body[ind1].Num;
//      mtype=Body[ind1].Fl.MType;
		}else{
//      anum+=numN;
		}
		anum=num+numN;
		Body[i].SetN(TypeN,DataN,type,anum,expo/anum);
		Body[i].Num=anum;
		// artifact
		if(ind !=-1) Body[i].AddArt(Body[ind]);
		if(ind1!=-1) Body[i].AddArt(Body[ind1]);
/*
		if(Body[ind].HasArt()){
			Body[i].CopyArt(Body[ind]);
		}
		if(Body[ind1].HasArt()){
			if(Body[i].HasArt()==0){ // уже есть
				Body[i].CopyArt(Body[ind1]);
			}
		}
*/
		if(ind !=-1) Body[ind ].Clear();
		if(ind1!=-1) Body[ind1].Clear();
		RETURN(0)
	}
	TError("CrExpoSet - HComb: cannot add new stack");
	RETURN(-1)
}
// добавить первый стэк ко второму реально (не только опыт)
int CrExpoSet::HCombReal(int Type,int TypeN,CRLOC Data,CRLOC DataN,int type,int typeN,int num,int numN){
	CrExpo tmp,tmpN;
	tmp.Clear(); tmp.SetN(Type,Data,type,num,0);
	if(num==0){
		type=tmp.RealType(-2);
		num=tmp.RealNum(-2);
		tmp.SetN(Type,Data,type,num,0);
	}
	tmpN.Clear(); tmpN.SetN(TypeN,DataN,typeN,numN,0);
	if(type!=typeN) return -1;
	CrExpoSet::HComb(Type,TypeN,Data,DataN,type,typeN,num,numN);
	tmp.RealType(-1); tmp.RealNum(0);
	tmpN.RealNum(numN+num);
	return 1;
}

// ищем - кто хозяин армии
int CrExpoSet::FindType(int x,int y,int l,int Stack,int *Type,CRLOC *Crloc,int *MType,int *MNum){
	STARTNA(__LINE__, 0)
	int i,*mt,*mn,dummy;
	if(MType==0) mt=&dummy; else mt=MType;
	if(MNum ==0) mn=&dummy; else mn=MNum;
	if((*Type==-1)||(*Type==CE_HERO)){
		// Герой?
		_Hero_ *hp;
		for(i=0;i<HERNUM;i++){
			hp=GetHeroStr(i);
			if(hp->x!=x) continue;
			if(hp->y!=y) continue;
			if(hp->l!=l) continue;
			*Type=CE_HERO; *Crloc=MAKEHS(hp->Number,Stack);
			*mt=hp->Ct[Stack]; *mn=hp->Cn[Stack];
			RETURN(1)
		}
	}
	if((*Type==-1)||(*Type==CE_TOWN)){
		// Город
		do{
			_CastleSetup_ *cst;
			cst=GetCastleBase();
			if(cst==0) break; // нет замков
			int cn=GetCastleNum(); //
			for(i=0;i<cn;i++,cst++){
				if(cst->x!=x) continue;
				if(cst->y!=y) continue;
				if(cst->l!=l) continue;
				*Type=CE_TOWN; *Crloc=MAKETS(cst->x,cst->y,cst->l,Stack);
				*mt=cst->GuardsT[Stack]; *mn=cst->GuardsN[Stack];
				RETURN(1)
			}
		}while(0);
	}
	if((*Type==-1)||(*Type==CE_MINE)){
		// Шахта
		do{
			_Mine_ *cst;
			cst=GetMineBase();
			if(cst==0) break; // нет
			int cn=GetMineNum(); //
			for(i=0;i<cn;i++,cst++){
				if(cst->xe!=x) continue;
				if(cst->ye!=y) continue;
				if(cst->le!=l) continue;
				*Type=CE_MINE; *Crloc=MAKEMS(cst->xe,cst->ye,cst->le,Stack);
				*mt=cst->GType[Stack]; *mn=cst->GNumb[Stack];
				RETURN(1)
			}
		}while(0);
	}
	if((*Type==-1)||(*Type==CE_HORN)){
		// Гарн
		do{
			_Horn_ *cst;
			cst=GetHornBase();
			if(cst==0) break; // нет
			int cn=GetHornNum(); //
			for(i=0;i<cn;i++,cst++){
				if(cst->ex!=x) continue;
				if(cst->ey!=y) continue;
				if(cst->el!=l) continue;
				*Type=CE_HORN; *Crloc=MAKEZS(cst->ex,cst->ey,cst->el,Stack);
				*mt=cst->GType[Stack]; *mn=cst->GNumb[Stack];
				RETURN(1)
			}
		}while(0);
	}
	// на карте
/*
	do{
		_MapItem_ *mp=GetMapItem2(Dt.MixPos);
			if(mp==0){ TError("CrExpo: Cannot get map position."); RETURN(-1) }
			if(mp->OType!=54){ TError("CrExpo: Not a Monster at map position."); RETURN(-1) }
			if(type==-2){ // get
				RETURN(mp->OSType)
			}else{ // set
				mp->OSType=(Word)type;
				RETURN(0)
			}
		}
*/
	RETURN(0)
}
// ищем - кто хозяин армии
int CrExpoSet::FindType(_MonArr_ *MArr,int Stack,int *Type,CRLOC *Crloc){
	STARTNA(__LINE__, 0)
	int i;
	// Герой?
	_Hero_ *hp;
	for(i=0;i<HERNUM;i++){
		hp=GetHeroStr(i);
		if(((_MonArr_ *)hp->Ct)==MArr){ *Type=CE_HERO; *Crloc=MAKEHS(hp->Number,Stack); RETURN(1) }
	}
	// Город
	do{
		_CastleSetup_ *cst;
		cst=GetCastleBase();
		if(cst==0) break; // нет замков
		int cn=GetCastleNum(); //
		for(i=0;i<cn;i++,cst++){
		 if((_MonArr_ *)cst->GuardsT==MArr){ *Type=CE_TOWN; *Crloc=MAKETS(cst->x,cst->y,cst->l,Stack); RETURN(1) }
		}
	}while(0);
	// Шахта
	do{
		_Mine_ *cst;
		cst=GetMineBase();
		if(cst==0) break; // нет
		int cn=GetMineNum(); //
		for(i=0;i<cn;i++,cst++){
		 if((_MonArr_ *)cst->GType==MArr){ *Type=CE_MINE; *Crloc=MAKEMS(cst->xe,cst->ye,cst->le,Stack); RETURN(1) }
		}
	}while(0);
	// Гарн
	do{
		_Horn_ *cst;
		cst=GetHornBase();
		if(cst==0) break; // нет
		int cn=GetHornNum(); //
		for(i=0;i<cn;i++,cst++){
		 if((_MonArr_ *)cst->GType==MArr){ *Type=CE_HORN; *Crloc=MAKEZS(cst->ex,cst->ey,cst->el,Stack); RETURN(1) }
		}
	}while(0);
	// на карте
/*
	do{
		_MapItem_ *mp=GetMapItem2(Dt.MixPos);
			if(mp==0){ TError("CrExpo: Cannot get map position."); RETURN(-1) }
			if(mp->OType!=54){ TError("CrExpo: Not a Monster at map position."); RETURN(-1) }
			if(type==-2){ // get
				RETURN(mp->OSType)
			}else{ // set
				mp->OSType=(Word)type;
				RETURN(0)
			}
		}
*/
	RETURN(0)
}
int CrExpoSet::FindType(int Ind,int *Type,CRLOC *Crloc,int *Slot)
{
	STARTNA(__LINE__, 0)
	if(Ind<0 || Ind>=MAXCRINFONUM) RETURN(0)
	if(Body[Ind].Fl.Act==0) RETURN(0)
	*Type=Body[Ind].Fl.Type;
	*Crloc=Body[Ind].Dt.UniData;
	*Slot=0;
	switch(Body[Ind].Fl.Type){
		case CE_HERO: *Slot=Body[Ind].Dt.Hero.Slot; break;
		case CE_TOWN: *Slot=Body[Ind].Dt.Town.Slot; break;
		case CE_MINE: *Slot=Body[Ind].Dt.Mine.Slot; break;
		case CE_HORN: *Slot=Body[Ind].Dt.Horn.Slot; break;
	}
	RETURN(1)
}

/////////////////////
int LoadExpTXT(void){
	STARTNA(__LINE__, 0)
	if(LoadTXT("ZCREXP.TXT",&CrExpTXT)) RETURN(1) // не может загрузить TXT
	RETURN(0)
}
/////////////////////
CrExpMod::CrExpModStr CrExpMod::Body[MAXCREXPAB];
CrExpMod::CrExpModStr CrExpMod::Cur;
CrExpMod::CrExpModStr CrExpMod::BFBody[BFNUM];
CrExpMod::CrExpModStr CrExpMod::Dummy={
	1.0,1.0,EXPOMAX2,0,100
};
int CrExpMod::Ranks[11]={
	17500,
	1000,1000,1200,1400,1600,1800,2000,2200,2500,2800
};

int  CrExpMod::Load(int /*ver*/){
	STARTNA(__LINE__, 0)
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='C'||buf[1]!='R'||buf[2]!='M'||buf[3]!='D')
			{TError("CrExpMod cannot start loading"); RETURN(1)}
	if(Loader(Body,sizeof(Body))) RETURN(1)
	RETURN(0)
}
int  CrExpMod::Save(){
	STARTNA(__LINE__, 0)
	if(Saver("CRMD",4)) RETURN(1)
	if(Saver(Body,sizeof(Body))) RETURN(1)
	RETURN(0)
}

static TxtFile CrExpModFile;
static float CrModTmp[7+1][2];
static int   CrModTmp2[7+1][3];
int  CrExpMod::Clear(){ // из TXT файла
	STARTNA(__LINE__, 0)
	float v;
	int i,j,n,t;
	// set default
//  for(i=0;i<MAXCREXPAB;i++){ Body[i].ExpMul=0.0; Body[i].UpgrMul=0.0; }
	for(j=0;j<8;j++){ // set up defaults
		CrModTmp[j][0]=1.0; CrModTmp[j][1]=0.5;
		CrModTmp2[j][0]=79400; CrModTmp2[j][1]=50; CrModTmp2[j][1]=55580;
	}
	memset(Body,0,sizeof(Body));
	if(LoadTXT("CREXPMOD.TXT",&CrExpModFile)) RETURN(1)  // не может загрузить TXT
	for(i=1;i<CrExpModFile.sn;i++){
		n=a2i(ITxt(i,0,&CrExpModFile));
		if(n<0){ n=-n-1; if(n>7) continue;
			v=Atof(ITxt(i,1,&CrExpModFile));
			CrModTmp[n][0]=v;
			v=Atof(ITxt(i,2,&CrExpModFile));
			CrModTmp[n][1]=v;
			t=Atoi(ITxt(i,3,&CrExpModFile));
			CrModTmp2[n][0]=t;
			t=Atoi(ITxt(i,4,&CrExpModFile));
			CrModTmp2[n][1]=t;
			t=Atoi(ITxt(i,5,&CrExpModFile));
			CrModTmp2[n][2]=t;
			continue;
		}
		if((n<0)||(n>=MAXCREXPAB)) continue; // неправильный номер монстра
		v=Atof(ITxt(i,1,&CrExpModFile));
		Body[n].ExpMul=v;
		v=Atof(ITxt(i,2,&CrExpModFile));
		Body[n].UpgrMul=v;
		t=Atoi(ITxt(i,3,&CrExpModFile));
		Body[n].Limit=t;
		t=Atoi(ITxt(i,4,&CrExpModFile));
		Body[n].Cap=t;
		t=Atoi(ITxt(i,5,&CrExpModFile));
		Body[n].Lvl11Exp=t;
	}
	UnloadTXT(&CrExpModFile);
	for(j=0;j<8;j++){ // set up defaults
		for(i=0;i<MAXCREXPAB;i++){
			if(Body[i].Limit!=0) continue; // уже настроен напрямую
			if(i>=MONNUM) n=7; // несуществующие - дефаулт
			else n=MonTable[i].SubGroup;
			if(n<0 || n>7) n=7;
			Body[i].ExpMul=CrModTmp[n][0];
			Body[i].UpgrMul=CrModTmp[n][1];
			Body[i].Limit=CrModTmp2[n][0];
			Body[i].Cap=CrModTmp2[n][1];
			Body[i].Lvl11Exp=CrModTmp2[n][2];
		}
	}
	RETURN(0)
}
/////////////////////
CrExpBon::CrExpBonStr CrExpBon::Body[MAXCREXPAB][20];
CrExpBon::CrExpBonStr CrExpBon::BFBody[BFNUM][20];
CrExpBon::CrExpBonStr CrExpBon::Dummy[20];
CrExpBon::CrExpBonStr CrExpBon::Cur[20];
CrExpBon::CrExpBonBFStartStat CrExpBon::BFStat[BFNUM];
int CrExpBon::IsBattle=0;


char CrExpBon::BFBodyAct[BFNUM];

int  CrExpBon::Load(int /*ver*/){
	STARTNA(__LINE__, 0)
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='C'||buf[1]!='R'||buf[2]!='B'||buf[3]!='N')
			{TError("CrExpBon cannot start loading"); RETURN(1)}
	if(Loader(Body,sizeof(Body))) RETURN(1)
	RETURN(0)
}
int  CrExpBon::Save(){
	STARTNA(__LINE__, 0)
	if(Saver("CRBN",4)) RETURN(1)
	if(Saver(Body,sizeof(Body))) RETURN(1)
	RETURN(0)
}

static int CrExpBonFileLoaded=0;
static TxtFile CrExpBonFile;
int CrExpBon::FindBon(CrExpBonStr *str,char type,int Mod,int silent){
	STARTNA(__LINE__, 0)
	int j;
	if(type!=0){
		for(j=0;j<14;j++){
			if(str[j].Fl.Act==0) continue;
			if(str[j].Type!=type) continue;
			if(Mod==-1 || str[j].Mod==Mod) RETURN(j)
		}
	}
	for(j=0;j<14;j++){ if(str[j].Fl.Act==0) break; }
	if(j==14){
		if(silent==0) TError("CREXPBON.TXT: More than 14 creature attributes. Skipped");
		RETURN(-1)
	}
	RETURN(j)
}
int CrExpBon::FindThisBonLineInd(int MType,char Bon,char Mod){
	STARTNA(__LINE__, 0)
	if(MType<-BFNUM || MType>=MAXCREXPAB) RETURN(1)
	CrExpBonStr *str=Get(MType);// &Body[MType][Ind];
//  CrExpBonStr *str=Body[MType];
	RETURN(CrExpBon::FindBon(str,Bon,Mod,1))
}

int  CrExpBon::Clear(){ // из TXT файла
	STARTNA(__LINE__, 0)
	int i,j,k,l,n;
	char c;
	CrExpBonStr *str,BodyD[8][20];
	if(CrExpBonFileLoaded==0){
		if(LoadTXT("CREXPBON.TXT",&CrExpBonFile)) RETURN(1)  // не может загрузить TXT
		else CrExpBonFileLoaded=1;
	}
	FillMem(&Body,sizeof(Body),0);
	FillMem(&BodyD,sizeof(BodyD),0);
	for(i=1;i<CrExpBonFile.sn;i++){
		n=a2i(ITxt(i,0,&CrExpBonFile));
		if((n<-8)||(n>-1)) continue; // неправильный номер дефаулта монстра
		n=-n-1; str=BodyD[n]; 
		c=ITxt(i,1,&CrExpBonFile)[0];
		if(c==' ') continue; // неизвестный тип параметра
		if(c==9) continue; // неизвестный тип параметра
		if(c==0) continue; // неизвестный тип параметра
		j=FindBon(str,0,-1); if(j==-1) continue;
		str[j].Type=c;
		str[j].Mod=ITxt(i,2,&CrExpBonFile)[0];
		if(str[j].Mod=='#'){ // number 0...255
			*(Byte *)&str[j].Mod=(Byte)Atoi(&(ITxt(i,2,&CrExpBonFile)[1]));
		}
		for(k=0;k<11;k++){
			str[j].Lvls[k]=(Byte)Atoi(ITxt(i,3+k,&CrExpBonFile));
		}
		// !!!!!!!!!!!!!!
		if(str[j].Type=='f' && str[j].Mod=='S'){ // shooting ability
			if(PL_CrExpEnable!=0){
				MonTable[n].Flags|=0x00000004;
				MonTable[n].NShots=0;
			}else{
				MonTable[n].Flags&= ~0x00000004;
			}
		}
		str[j].Fl.Act=1;
	}
	for(i=0;i<MONNUM;i++){
		str=Body[i];
		l=MonTable[i].SubGroup;
		for(j=0;j<14;j++){
			if(BodyD[l][j].Fl.Act==0) continue;
			n=FindBon(str,BodyD[l][j].Type,-1); if(n==-1) continue;
			if(str[n].Fl.Act!=0) continue; // ужу есть
			memcpy(&str[n],&BodyD[l][j],sizeof(CrExpBonStr));
		}
		for(j=0;j<14;j++){
			if(BodyD[7][j].Fl.Act==0) continue;
			n=FindBon(str,BodyD[7][j].Type,-1); if(n==-1) continue;
			if(str[n].Fl.Act!=0) continue; // ужу есть
			memcpy(&str[n],&BodyD[7][j],sizeof(CrExpBonStr));
		}
	}
	for(i=1;i<CrExpBonFile.sn;i++){
		n=a2i(ITxt(i,0,&CrExpBonFile));
		if((n<0)||(n>=MAXCREXPAB)) continue; // неправильный номер монстра
		str=Body[n];
		c=ITxt(i,1,&CrExpBonFile)[0];
		if(c==' ') continue; // неизвестный тип параметра
		if(c==9) continue; // неизвестный тип параметра
		if(c==0) continue; // неизвестный тип параметра
		switch(c){
			case 'A':
			case 'D':
			case 'H':
			case 'm':
			case 'M':
			case 'S':
				j=FindBon(str,c,-1); 
				if(j==-1){ j=FindBon(str,0,-1); if(j==-1) continue; }
				break;
			default:
				j=FindBon(str,0,-1); if(j==-1) continue;
		}
		str[j].Type=c;
		str[j].Mod=ITxt(i,2,&CrExpBonFile)[0];
		if(str[j].Mod=='#'){ // number 0...255
			*(Byte *)&str[j].Mod=(Byte)Atoi(&(ITxt(i,2,&CrExpBonFile)[1]));
		}
		for(k=0;k<11;k++){
			str[j].Lvls[k]=(Byte)Atoi(ITxt(i,3+k,&CrExpBonFile));
		}
		// !!!!!!!!!!!!!!
		if(str[j].Type=='f' && str[j].Mod=='S'){ // shooting ability
			if(PL_CrExpEnable!=0){
				MonTable[n].Flags|=0x00000004;
				MonTable[n].NShots=0;
			}else{
				MonTable[n].Flags&= ~0x00000004;
			}
		}
//    Body[n][j].Descr=ITxt(i,14,&CrExpModFile);
		str[j].Fl.Act=1;
	}
	
/*
	for(i=1;i<CrExpBonFile.sn;i++){
		n=a2i(ITxt(i,0,&CrExpBonFile));
		if((n<-8)||(n>=MAXCREXPAB)) continue; // неправильный номер монстра
		if(n<0){ n=-n-1; str=BodyD[n]; }
		else{ str=Body[n]; }
		c=ITxt(i,1,&CrExpBonFile)[0];
		if(c==' ') continue; // неизвестный тип параметра
		if(c==9) continue; // неизвестный тип параметра
		if(c==0) continue; // неизвестный тип параметра
//    for(j=0;j<14;j++){ if(str[j].Fl.Act==0) break; }
//    if(j==14){ TError("CREXPBON.TXT: More than 14 creature attributes. Skipped"); continue; }
		j=FindBon(str,0,-1); if(j==-1) continue;
		str[j].Type=c;
		str[j].Mod=ITxt(i,2,&CrExpBonFile)[0];
		if(str[j].Mod=='#'){ // number 0...255
			*(Byte *)&str[j].Mod=(Byte)Atoi(&(ITxt(i,2,&CrExpBonFile)[1]));
		}
		for(k=0;k<11;k++){
			str[j].Lvls[k]=(Byte)Atoi(ITxt(i,3+k,&CrExpBonFile));
		}
		// !!!!!!!!!!!!!!
		if(str[j].Type=='f' && str[j].Mod=='S'){ // shooting ability
			if(PL_CrExpEnable!=0){
				MonTable[n].Flags|=0x00000004;
				MonTable[n].NShots=0;
			}else{
				MonTable[n].Flags&= ~0x00000004;
			}
		}
//    Body[n][j].Descr=ITxt(i,14,&CrExpModFile);
		str[j].Fl.Act=1;
	}
//  UnloadTXT(&CrExpBonFile);
	for(i=0;i<MONNUM;i++){
		str=Body[i];
		l=MonTable[i].SubGroup;
		for(j=0;j<14;j++){
			if(BodyD[l][j].Fl.Act==0) continue;
			n=FindBon(str,BodyD[l][j].Type,-1); if(n==-1) continue;
			if(str[n].Fl.Act!=0) continue; // ужу есть
			memcpy(&str[n],&BodyD[l][j],sizeof(CrExpBonStr));
		}
		for(j=0;j<14;j++){
			if(BodyD[7][j].Fl.Act==0) continue;
			n=FindBon(str,BodyD[7][j].Type,-1); if(n==-1) continue;
			if(str[n].Fl.Act!=0) continue; // ужу есть
			memcpy(&str[n],&BodyD[7][j],sizeof(CrExpBonStr));
		}
	}
*/
	RETURN(0)
}

int CrExpBon::SetBonLine(int MType,int Ind,int Flags, int Type, int Mod, int *lvls)
{
	STARTNA(__LINE__, 0)
	if(MType<-BFNUM || MType>=MAXCREXPAB) RETURN(1)
	if(Ind<0 || Ind>=20) RETURN(1)
//  CrExpBonStr *str=&Body[MType][Ind];
	CrExpBonStr *str=&Get(MType)[Ind];// &Body[MType][Ind];
	*(int *)&str->Fl=Flags;
	str->Type=(char)Type;
	*(Byte *)&str->Mod=(Byte)Mod;
	for(int i=0;i<11;i++) str->Lvls[i]=(Byte)lvls[i];
	RETURN(0)
}

int CrExpBon::GetBonLine(int MType,int Ind,int *Flags, int *Type, int *Mod, int *lvls)
{
	STARTNA(__LINE__, 0)
	if(MType<-BFNUM || MType>=MAXCREXPAB) RETURN(1)
	if(Ind<0 || Ind>=20) RETURN(1)
//  CrExpBonStr *str=&Body[MType][Ind];
	CrExpBonStr *str=&Get(MType)[Ind];
	*Flags=*(int *)&str->Fl;
	*Type=str->Type;
	*Mod=*(Byte *)&str->Mod;
	for(int i=0;i<11;i++) lvls[i]=str->Lvls[i];
	RETURN(0)
}

int CrExpBon::GetStackExpo(Byte *Mon,int *VExp){
//  _MonArr_ *MArr;
	STARTNA(__LINE__, 0)
	Byte   *BatMan;
	int       Exp/*,side*/;
	*VExp=0;
	__asm{
		mov    eax,0x699420
		mov    eax,[eax] // bat man
		mov    BatMan,eax
	}
	if(/*WoG &&*/ PL_CrExpEnable){
/*
		int tp=*(int *)&Mon[0x34];
		if(tp==-1) return 0;
		int Stack=*(int *)&Mon[0x5C]; // номер стэка
		if(Stack==-1){ MArr=0; Stack=0; }
		else{
			side=*(int *)&Mon[0xF4]; // сторона
			MArr=(_MonArr_ *)*(Dword *)&BatMan[0x54C4+side*4];
		}
		if(MArr==0) Exp=0;
		else{
			int Type;
			CRLOC Crloc;
			if(BADistFlag){ // Дистанционная атака
				Exp=ToSendCrExpo[side][Stack].Expo;
			}else{
				if(CrExpoSet::FindType(MArr,Stack,&Type,&Crloc)){
					CrExpo *cr;
					cr=CrExpoSet::Find(Type,Crloc);
					if(cr==0) Exp=0;
					else{
						if(cr->Num==0) Exp=0;
						else Exp=cr->Expo;
					}
				}else  Exp=0;
			}
		}
*/

		CrExpo *cr=CrExpoSet::GetBF(MonIndex(Mon));
		if(cr->Num==0) Exp=0;
		else Exp=cr->Expo;
		*VExp=Exp;
		RETURN(1)
	}
	RETURN(0)
}

static void ApplyMod(float *val,char mod,int perc){
	STARTNA(__LINE__, 0)
	switch(mod){
		case '+': *val+=perc; break;
		case '-': *val-=perc; break;
		case '%': *val+=*val*perc/100.0f+0.5f; break;
		case '=': *val=float(perc); break;
	}
	RETURNV
}

static int BB_Exp,BB_Type1,BB_Pos,BB_Lvl,BB_Num;
static CrExpo *BB_Cr;
static int Prepare(Byte *Mon,int checknum=1){
	STARTNA(__LINE__, 0)
	//if(WoG==0) RETURN(0)
	if(PL_CrExpEnable==0) RETURN(0)
	if(Mon==0) RETURN(0)
	BB_Type1=*(int *)&Mon[0x34];
	BB_Pos=MonIndex(Mon);
	BB_Num=*(int *)&Mon[0x4C];
	if(IsNPCNotCreature(Mon)) RETURN(0)
	if((checknum==1) && (BB_Num<1)) RETURN(0)
	CrExpBon::MakeCur(BB_Pos,BB_Type1);
//  CrExpBon::GetStackExpo(Mon,&BB_Exp);
	BB_Cr=CrExpoSet::GetBF(BB_Pos);
	BB_Exp=BB_Cr->Expo;
//  BB_Lvl=CrExpMod::GetRank(BB_Type1,BB_Exp);
	BB_Lvl=CrExpMod::GetRank(CURMON,BB_Exp);
	RETURN(1)
}
static int Prepare2(_Hero_ *Hp,int Stack){
	STARTNA(__LINE__, 0)
	//if(WoG==0) RETURN(0)
	if(PL_CrExpEnable==0) RETURN(0)
	if(Hp==0) RETURN(0)
	BB_Exp=0;
	BB_Cr=0;
	CrExpo *cr=CrExpoSet::Find(CE_HERO,MAKEHS(Hp->Number,Stack));
	if(cr==0) RETURN(0)
	BB_Cr=cr;
	BB_Exp=cr->Expo;
	BB_Type1=Hp->Ct[Stack];
	CrExpBon::MakeCur(-1,BB_Type1);
//  BB_Lvl=CrExpMod::GetRank(BB_Type,BB_Exp);
	BB_Lvl=CrExpMod::GetRank(CURMON,BB_Exp);
	RETURN(1)
}
/*
int CrExpBon::PrepareBFExpStructure(Byte *BatMan){
	int i,j;
	Byte *Mon;
	if(BatMan==0) return 1;
	for(i=0;i<(21*2);i++){
		FillMem(BFBody[i],sizeof(struct CrExpBonStrBF)*20,0);
		// 3.58 Tower skip
		if(i==20) continue; // 3.58
		if(i==41) continue; // 3.58
		Mon=&BatMan[0x54CC+0x548*i];
		int tp=*(int *)&Mon[0x34]; if(tp==-1) continue;
		int num=*(int *)&Mon[0x4C];
		if((tp<0)||(tp>=MAXCREXPAB)) continue;
		for(j=0;j<20;j++){
			memcpy(&BFBody[i][j].Str,&Body[tp][j],sizeof(struct CrExpBonStr));
		}
		if(Prepare(Mon,0)) BFBody[i].Exp=BB_Exp; else BFBody[i].Exp=0;
		BFBody[i].Num=num;
	}
	return 0;
}
*/
static int CalcRetNumber(Byte *Mon,int BONor){
	int val;
	int counterstrike=*(int *)&Mon[0x280];
	if(BONor){ // current bonus
		if(counterstrike==0){ // no spell cast
			val=0;
		}else{ // spell cast
			val=*(int *)&Mon[0x494];
		}
	}else{ // original without bonus
		val=*(int *)&Mon[0x454];
		if(counterstrike==0){ // no spell cast
		}else{ // spell cast
			val-=*(int *)&Mon[0x494];
		}
	}
	return val;
}
// Busy:
// A,D,H,M,S,O,P,B,F,C,R,W,E,G,L,U,d,i,o,e,h,c,s,p,a,w,b,g,r,m,u,l,J,j,K,k
// f(F,S,B,L,1,2,3,P,E,I,D,R,M,U,A,G,b,c,m,s,f,r,u)
int CrExpBon::Apply(Byte *Mon){ // настройка бонусов на поле боя один раз
	STARTNA(__LINE__, 0)
	float val;
	Dword dw;
	int   ArtUsed=0,RUsed=0;
//  int Exp=0; if(GetStackExpo(Mon,&Exp)) return 0;
//  int Type=*(int *)&Mon[0x34];
//  int lvl=Exp*100/EXPOMUL/CrExpMod::Limit(Type)/10; if(lvl>10) lvl=10; if(lvl<0) lvl=0;
	if(!Prepare(Mon)) RETURN(0)
	CrExpBonBFStartStat *str=&BFStat[BB_Pos];
	for(int i=0;i<20;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'A': // attack
				if(str->fA==0){ str->aA=*(int *)&Mon[0xC8]; str->fA=1; } val=str->aA;
				if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==1 ){ // Art Sub 1 *2 attack bonus +2
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]*2); 
					val+=2;
					ArtUsed=1;
				}else{
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); 
				}
				*(int *)&Mon[0xC8]=(int)val;
				break;
			case 'D': // defence
				if(str->fD==0){ str->aD=*(int *)&Mon[0xCC]; str->fD=1; } val=str->aD;
				if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==2 ){ // Art Sub 2 *2defence bonus + 2
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]*2); 
					val+=2;
					ArtUsed=1;
				}else{
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); 
				}
				*(int *)&Mon[0xCC]=(int)val;
				break;
			case 'H': // Hit Points
//        val=(float)*(int *)&Mon[0xC0]; ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); *(int *)&Mon[0xC0]=(int)val;
				if(str->fH==0){ str->aH=*(int *)&Mon[0x6C]; str->fH=1; } val=str->aH;
				if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==0 ){ // Art Sub 0 *2health +2
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]*2); 
					val+=2;
					ArtUsed=1;
				}else{
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); 
				}
				*(int *)&Mon[0x6C]=(int)val;
				*(int *)&Mon[0xC0]=(int)val;
				break;
			case 'm': // Min Damage
				if(str->fm==0){ str->am=*(int *)&Mon[0xD0]; str->fm=1; } val=str->am;
				if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==3 ){ // Art Sub 3 *2+1
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]*2); 
					val+=1;
					ArtUsed=1;
				}else{
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); 
				}
				*(int *)&Mon[0xD0]=(int)val;
				break;
			case 'M': // Max Damage
				if(str->fM==0){ str->aM=*(int *)&Mon[0xD4]; str->fM=1; } val=str->aM;
				if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==3 ){ // Art Sub 3 *2+1
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]*2); 
					val+=1;
					ArtUsed=1;
				}else{
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); 
				}
				*(int *)&Mon[0xD4]=(int)val;
				break;
			case 'S': // Speed
				if(str->fS==0){ str->aS=*(int *)&Mon[0xC4]; str->fS=1; } val=str->aS;
				if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==4 ){ // Art Sub 4 *2+1
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]*2); 
					val+=1;
					ArtUsed=1;
				}else{
					ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); 
				}
				*(int *)&Mon[0xC4]=(int)val;
				break;
			case 'O': // Number of shots
				if(str->fO==0){ str->aO=*(int *)&Mon[0xD8]; str->fO=1; } val=str->aO;
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); *(int *)&Mon[0xD8]=(int)val;
				break;
			case 'P': // Number spells to cast
				if(str->fP==0){ str->aP=*(int *)&Mon[0xDC]; str->fP=1; } val=str->aP;
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); *(int *)&Mon[0xDC]=(int)val;
				break;
			case 'R': // количество ответов на атаку
				if(str->fR==0){ str->aR=CalcRetNumber(Mon,0); str->fR=1; } val=str->aR;
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); 
				val+=CalcRetNumber(Mon,1);
				if(BB_Cr!=0 && BB_Cr->HasArt() && (BB_Cr->GetSubArt()==8) ){ // Art Sub 8
					val+=2;
					ArtUsed=1;
				}
				RUsed=1;
				*(int *)&Mon[0x454]=(int)val;
				break;
			case 'f': // Flags
				if(ebp->Lvls[BB_Lvl]==2) break;
				dw=0;
				switch(ebp->Mod){
					case 'F': dw=0x00000002; break; // can fly
					case 'S': dw=0x00000004; break; // can shoot
					case 'B': dw=0x00000008; break; // breath attack
					case 'L': dw=0x00000010; break; // Alive
					case '1': dw=0x00000080; break; // KING 1
					case '2': dw=0x00000100; break; // KING 2
					case '3': dw=0x00000200; break; // KING 3
					case 'P': dw=0x00000400; break; // не чуствителен к псих атаке
					case 'E': dw=0x00001000; break; // no melee penalty
					case 'I': dw=0x00004000; break; // fire magic immunity
					case 'D': dw=0x00008000; break; // Double strike
					case 'R': dw=0x00010000; break; // No retailate
					case 'M': dw=0x00020000; break; // No moral penalty
					case 'U': dw=0x00040000; break; // Undead
					case 'A': dw=0x00080000; break; // round attack
					case 'G': dw=0x80000000; break; // dragon
//          case 'b': RETURN(0)
//          default: TError("CREXPBON.TXT: Unknown Type field"); break;
//          default: RETURN(0)
				}
				if(ebp->Lvls[BB_Lvl]==1){ *(Dword *)&Mon[0x84]|=dw; }
				else{                     *(Dword *)&Mon[0x84]&=~dw; }
				break;
//      default:
//        TError("CREXPBON.TXT: Unknown Type field"); break;
		}
	}
	if(RUsed==0){ // если R не было и артифакт еще мог не сработать
		// количество ответов на атаку
		if(str->fR==0){ str->aR=CalcRetNumber(Mon,0); str->fR=1; } val=str->aR;
		val+=CalcRetNumber(Mon,1);
		if(BB_Cr!=0 && BB_Cr->HasArt() && (BB_Cr->GetSubArt()==8) ){ // Art Sub 8
			val+=2;
			ArtUsed=1;
			*(int *)&Mon[0x454]=(int)val;
		}
	}
	RETURN(0)
}
int CrExpBon::Apply2(Byte *Mon){ // настройка бонусов на поле боя каждый раунд
	STARTNA(__LINE__, 0)
	int ArtUsed=0,RUsed=0;
	float val;
	if(!Prepare(Mon)) RETURN(0)
	CrExpBonBFStartStat *str=&BFStat[BB_Pos];
	for(int i=0;i<20;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'R': // количество ответов на атаку
				if(str->fR==0){ str->aR=CalcRetNumber(Mon,0); str->fR=1; } val=str->aR;
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]); 
				val+=CalcRetNumber(Mon,1);
				if(BB_Cr!=0 && BB_Cr->HasArt() && (BB_Cr->GetSubArt()==8) ){ // Art Sub 8
					val+=2;
					ArtUsed=1;
				}
				RUsed=1;
				*(int *)&Mon[0x454]=(int)val;
				break;
//      default:
//        TError("CREXPBON.TXT: Unknown Type field"); break;
		}
	}
	if(RUsed==0){
		if(str->fR==0){ str->aR=CalcRetNumber(Mon,0); str->fR=1; } val=str->aR;
		val+=CalcRetNumber(Mon,1);
		if(BB_Cr!=0 && BB_Cr->HasArt() && (BB_Cr->GetSubArt()==8) ){ // Art Sub 8
			val+=2;
			ArtUsed=1;
			*(int *)&Mon[0x454]=(int)val;
		}
	}
	RETURN(0)
}
int CrExpBon::DefenceBonus(Byte *Mon,int Def)
{
	STARTNA(__LINE__, 0)
	if(!Prepare(Mon)) RETURN(Def)
	float val=(float)Def;
	for(int i=0;i<20;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'd': // бонус при выборе защиты
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				break;
//      default:
//        TError("CREXPBON.TXT: Unknown Type field"); break;
		}
	}
	RETURN((int)val)
}

/////////////
// Blocking ability
int CrExpBon::StackBlock(Byte *Mon,int DefPerc)
{
	STARTNA(__LINE__, 0)
//  if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	int Perc=DefPerc;
	if(!Prepare(Mon)){ 
		//RETURN(0)
	}else{
		float val;
		for(int i=0;i<14;i++){
			CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
			if(ebp->Fl.Act==0) continue;
			switch(ebp->Type){
				case 'B': // Block Ability
					val=0.0; ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
					Perc=int(val+0.5f);
					break;
			}
		}
	}
/*
	if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==7 ){ // Art Sub 7
		Perc=50;
	}
*/
	if(Perc==0) RETURN(0)
	if(Random(1,100)<=Perc){
		// флаг защиты поставим
		*((Dword *)&Mon[0x84])|=0x08000000;
		__asm{ // анимация блока
			mov    ecx,0x699420
			mov    ecx,[ecx] // bat man
			mov    eax,Mon
			mov    edx,0x55 // Complete Exp Block
			push   0
			push   100
			push   eax // ->monster
			push   edx
			mov    eax,0x4963C0
			call   eax
		}
		__asm{
			xor   eax,eax // no damage
			mov   ecx,Mon
			push  eax
			mov   eax,0x443DB0
			call  eax
		}
		// флаг защиты снимем
		char *po=ITxt(12,1,&CrExpTXT);
			__asm{
				mov    eax,0x699420
				mov    eax,[eax] // combat manager
				mov    ecx,[eax+0x132FC]
				mov    eax,po
				push   0
				push   1
				push   eax
				mov    eax,0x4729D0
				call   eax
			}
		RETURN(1)
	}
	RETURN(0)
}
static char SBP_Buf[512];
 // частичный блок
int CrExpBon::StackBlockPartial(Byte *Mon,int Dam)
{
	STARTNA(__LINE__, 0)
	if(!Prepare(Mon)) RETURN(Dam)
	int val=Dam,fl=0;
	int Perc=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'L': // Partial Block Ability
				if(ebp->Mod>=100){ // фиксир вероятность
					Perc=ebp->Mod-100;
					val=ebp->Lvls[BB_Lvl];
				}else{ // фикс снятие дамэджа
					val=ebp->Mod+1;
					Perc=ebp->Lvls[BB_Lvl];
				}
				break;
		}
	}
	do{
		if(Perc>0){ // бонусный шанс
			if(Random(1,100)<=Perc) fl=1; // сработал
		}
		if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==7 ){ // Art Sub 7
			if(Random(1,100)<=25){ // и арт сработал
				if(fl==1){ // оба сработали
					if(val<50)  val=50;
				}else{ // только арт
					val=50;
					fl=1;
				}
				if(Perc<25) Perc=25; // for displaying
			}else{ // арт не сработал
			}
		}
	}while(0);
//  if(Perc==0) RETURN(Dam)
//  if(Random(1,100)<=Perc){
	if(fl){ // что-то сработало
		if(val<1) RETURN(Dam)
		__asm{ // анимация блока
			mov    ecx,0x699420
			mov    ecx,[ecx] // bat man
			mov    eax,Mon
			mov    edx,0x56 // Partial Exp Block
			push   0
			push   100
			push   eax // ->monster
			push   edx
			mov    eax,0x4963C0
			call   eax
		}
//    char *po=ITxt(256,1,&CrExpTXT);
		sprintf(SBP_Buf,ITxt(256,1,&CrExpTXT),val,Perc);
		__asm{
			mov    eax,0x699420
			mov    eax,[eax] // combat manager
			mov    ecx,[eax+0x132FC]
			lea    eax,SBP_Buf
			push   0
			push   1
			push   eax
			mov    eax,0x4729D0
			call   eax
		}
		if(val>100) val=100;
		RETURN((Dam*(100-val)/100))
	}
	RETURN(Dam)
}

/////////////
// Fear Ability (no specific chance)
int CrExpBon::Fear(Byte *Mon)
{
	STARTNA(__LINE__, 0)
//  if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	float val=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'F': // Fear Ability
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				break;
		}
	}
	RETURN((int)val)
}
// бесстрашный
int CrExpBon::Fearless(Byte *Mon)
{
	STARTNA(__LINE__, 0)
//  if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(1)
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		if(ebp->Type=='f' && ebp->Mod=='f'){
			if(ebp->Lvls[BB_Lvl]==1){ RETURN(0) }else{ RETURN(1) }
		}
	}
	RETURN(1)
}
// возраждение стэке (FireBird) число заклов - число кастов
int CrExpBon::Rebirth(Byte *Mon)
{
	STARTNA(__LINE__, 0)
	if(!Prepare(Mon,0)) RETURN(1)
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		if(ebp->Type=='f' && ebp->Mod=='r'){
			if(ebp->Lvls[BB_Lvl]==1){ RETURN(0) }else{ RETURN(1) }
		}
	}
	RETURN(1)
}
// DeathStare
int CrExpBon::DeathStare(Byte *Mon)
{
	STARTNA(__LINE__, 0)
//  if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	float val=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'E': // Chance for Death Stare per creature
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				break;
		}
	}
	RETURN((int)val)
}

/////////////
// шанс игнорир магию (dwarf type)
int CrExpBon::DwarfResist(Byte *Mon,int Res,int Spell) //
{
	STARTNA(__LINE__, 0)
//  if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(Res)
// Res=100 - complete resist
	int i;
	float val=(float)Res;
	for(i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'W': // Chance to complete resisting
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				break;
		}
	}
	Byte *SpTable;
	__asm{
		mov   eax,0x687FA8
		mov   eax,[eax]
		mov   SpTable,eax
	}
	for(i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'w': // spell specific resistance
				if(ebp->Lvls[BB_Lvl]!=1) break;
				switch(ebp->Mod){
					case '6':
					case '7':
					case '8':
					case '9':
					case ':': // immune to spell of levels
						if(*(int *)&SpTable[0x88*Spell+0x18]<ebp->Mod-'6'+2) val=100.1f;
						break;
					case '1':
					case '2':
					case '3':
					case '4':
					case '5': // immune to spell of levels
						if(*(int *)&SpTable[0x88*Spell+0x18]<ebp->Mod-'1'+2) val=100.1f;
						break;
					case 'C': // immune to cold spells (Ice Bolt, Frost Ring)
						if(Spell==16 || Spell==20) val=100.1f;
						break;
					case 'L': // immune to Lightning spells (Lightning Bolt, Chain Lightning and Titan's Lightning Bolt)
						if(Spell==17 || Spell==19 || Spell==57) val=100.1f;
						break;
					case 'R': // immune to Armageddon
						if(Spell==26) val=100.1f;
						break;
					case 'M': // immune to   Meteor Shower
						if(Spell==23) val=100.1f;
						break;
					case 'I': // immune to Implosion
						if(Spell==18) val=100.1f;
						break;
//          case 'P': // immune to Any Dispell
//            if(Spell==35) val=100.1;
//            break;
//          case 'p': // immune to Hostile Dispell
//            if(Spell==35) val=100.1;
//            break;
//          case 'N': // immune to Dispell Benefic. Spells
//            if(Spell==78) val=100.1;
//            break;
					case 'B': // immune to Blind
						if(Spell==62) val=100.1f;
						break;
					case 'K': // immune to Berserk
						if(Spell==59) val=100.1f;
						break;
					case 'H': // immune to Hypnotize
						if(Spell==60) val=100.1f;
						break;
					case 'S': // immune to Slow
						if(Spell==54) val=100.1f;
						break;
					case 'O': // immune to Fire Direct Damage Spell
//10. Immune to all Fire Damage spells (but NOT Magic Arrow). The
//specific spells are: Fire Wall, Fireball, Land Mine, Armageddon, Fire
//Shield, Inferno
						if(Spell==13 || Spell==21 || Spell==11 || Spell==26 || Spell==29 || Spell==22) val=100.1f;
						break;
					case 'D': // immune to All Direct Damage Spell
//11. And an Immune to All Direct Damage Spells might also be useful if
//it's not too much trouble. The list of spells would be: Magic Arrow,
//Lightning Bolt, Chain Lightning, Meteor Shower, Implosion, Fireball,
//Armageddon, Inferno, Ice Bolt, Frost Ring.
						if(Spell==15 || Spell==17 || Spell==19 || Spell==23 || Spell==18 || Spell==21 || Spell==26 || Spell==22 || Spell==16 || Spell==20) val=100.1f;
						break;
					case 'm': // immune to some mind spells (Blind, Berserk, Sorrow, Hypnotize and Forgetfulness)
						if(Spell==62 || Spell==59 || Spell==50 || Spell==60 || Spell==61) val=100.1f;
						break;
					case 'A': // immune to Air schools
					case 'a': // immune to Air schools
						if(*(int *)&SpTable[0x88*Spell+0x1C] & 0x1) val=100.1f;
						break;
					case 'F': // immune to Fire schools
					case 'f': // immune to Fire schools
						if(*(int *)&SpTable[0x88*Spell+0x1C] & 0x2) val=100.1f;
						break;
					case 'W': // immune to Water schools
					case 'w': // immune to Water schools
						if(*(int *)&SpTable[0x88*Spell+0x1C] & 0x4) val=100.1f;
						break;
					case 'E': // immune to Earth schools
					case 'e': // immune to Earth schools
						if(*(int *)&SpTable[0x88*Spell+0x1C] & 0x8) val=100.1f;
						break;
				}
				break;
		}
	}
	RETURN((int)val)
}
// шанс игнорир друж. магию (dwarf type)
int CrExpBon::DwarfResistFriendly(Byte *Mon,int Res,int Spell) //
{
	STARTNA(__LINE__, 0)
	if(!Prepare(Mon)) RETURN(Res)
	float val=(float)Res;
	Byte *SpTable;
	__asm{
		mov   eax,0x687FA8
		mov   eax,[eax]
		mov   SpTable,eax
	}
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'w': // spell specific resistance
				if(ebp->Lvls[BB_Lvl]!=1) break;
				switch(ebp->Mod){
					case '0': // remove all immunity to friendly spell
						val=0.0;
						break;
					case '6':
					case '7':
					case '8':
					case '9':
					case ':': // immune to spell of levels
						if(*(int *)&SpTable[0x88*Spell+0x18]<ebp->Mod-'6'+2) val=100.1f;
						break;
					case 'a': // immune to Air schools
						if(*(int *)&SpTable[0x88*Spell+0x1C] & 0x1) val=100.1f;
						break;
					case 'f': // immune to Fire schools
						if(*(int *)&SpTable[0x88*Spell+0x1C] & 0x2) val=100.1f;
						break;
					case 'w': // immune to Water schools
						if(*(int *)&SpTable[0x88*Spell+0x1C] & 0x4) val=100.1f;
						break;
					case 'e': // immune to Earth schools
						if(*(int *)&SpTable[0x88*Spell+0x1C] & 0x8) val=100.1f;
						break;
				}
				break;
		}
	}
	RETURN((int)val)
}
/////////////
// нет пенальти от дистанции
int  CrExpBon::NoDistPenalty(Byte *Mon) //
{
	STARTNA(__LINE__, 0)
//  if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(1)
	int   rval;
	float val=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'i':
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				break;
		}
	}
	if(val>0.5) rval=0; else rval=1;
	RETURN(rval)
}
/////////////
// нет пенальти от припятствий
int  CrExpBon::NoObstPenalty(Byte *Mon) //
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	float val=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'o':
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				break;
		}
	}
	RETURN((int)val)
}
/////////////
// Casting Cost Reduction
int CrExpBon::StackSubSpCost(_Hero_ *Hp,int Stack,int CostShift) // снизить цену заклинания
{
	STARTNA(__LINE__, 0)
	if(!Prepare2(Hp,Stack)) RETURN(CostShift)
	float val=(float)CostShift;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'C': // Reduce cost of casting (+,%,=)
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				break;
		}
	}
	CostShift=(int)val;
	RETURN(CostShift)
}
/////////////
// Death Blow
int CrExpBon::DeathBlow(Byte *Mon,int Chance) // бонус к защите при выборе защиты
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(Chance) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(Chance)
	float val=(float)Chance;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'e':
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				break;
		}
	}
	RETURN((int)val)
}
//////////////
int CrExpBon::PersonalHate(Byte *MonA,int DType,int mult) // первональная ненависть
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(mult) // to disable AI thinking
	if(!Prepare(MonA)) RETURN(mult)
	float val=(float)mult;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'h':
				if(*(Byte *)&ebp->Mod==DType){
					if(ebp->Lvls[BB_Lvl]>200){
						val=(float)-(ebp->Lvls[BB_Lvl]-200)*10;
					}else{
						val=(float)ebp->Lvls[BB_Lvl]*10;
					}
				}
				break;
		}
	}
	RETURN((int)val)
}
//////////////
// может скастовать спел при атаке
int CrExpBon::CastSpell(Byte *Mon,Byte *DMon,_Hero_ *hpa,_Hero_ *hpd) // может скастовать спел при атаке
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI) RETURN(-1) // to disable AI thinking
	if(!Prepare(Mon))  RETURN(-1)
	float val=0;
	int Spell=-1;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'c':
				Spell=*(Byte *)&ebp->Mod;
				val=ebp->Lvls[BB_Lvl];
				break;
		}
	}
	if(val<1) RETURN(-1)
	if(Spell<0) RETURN(-1)
	if(Spell>=MAGICS) RETURN(-1)
	if(Random(1,100)>val) RETURN(-1)
	int num=*((int *)&Mon[0x4C]);
	if(num<1) RETURN(-1)
	if(*((int *)&DMon[0x4C])<=0) RETURN(-1);
	Spell=SCastSpell(num,DMon,hpa,hpd,Spell);
	RETURN(Spell)
}
//////////////
// проверка из опыта ли кастинг плюс расчет силы, если отсюда
static Byte *ESCP_AMon,*ESCP_DMon;
static char ESCP_CastType;

int CrExpBon::ExperienceSpellCastingPower(int/* Spell*/)
{
	return 0;
//  if(ESCP_AMon==0) return 0;
//  ESCP_DMon=DMon; ESCP_CastType=Ch;
//  switch(Spell)
}

// может скастовать массовые спелы при атаке
void CrExpBon::IntCastMassSpell(Byte *Mon,Byte *DMon,char Ch) // может скастовать спел при атаке
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI) RETURNV // to disable AI thinking
	if(!Prepare(Mon))  RETURNV
	int num=*((int *)&Mon[0x4C]);
	if(num<1) RETURNV
	if(*((int *)&DMon[0x4C])<=0) RETURNV
	ESCP_AMon=Mon; ESCP_DMon=DMon; ESCP_CastType=Ch;
	float val=0;
	int Spell=-1,dummy;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		if(ebp->Type==Ch){
			Spell=*(Byte *)&ebp->Mod;
			val=ebp->Lvls[BB_Lvl];
			if(val<1) continue;
			if(Spell<0) continue;
			if(Spell>=MAGICS) continue;
			if(Random(1,100)>val) continue;
			int side=*((int *)&Mon[0xF4]);
			if(*((int *)&Mon[0x288])){ side=!side; }
			int PowerLevel=3; 
			if((Ch=='J') || (Ch=='K')) PowerLevel=2;
			if((Ch=='j') || (Ch=='k')) PowerLevel=1;
			if(Spell==14){ // Earthquake
				if(side!=0) continue; // not attacker
				__asm{
					mov   eax,0x699420
					mov   eax,[eax]
					mov   eax,[eax+0x132F4]
					mov   dummy,eax
				}
				if(dummy <1) continue; // no town 
			}
			__asm{
				mov    ecx,0x699420
				mov    ecx,[ecx]
				push   1
				push   1
				push   DMon  // -> DMon
				push   side  // Side
				push   Spell // Spell Index
				mov    eax,0x5A8950
				call   eax
				test   al, al
				jz     lCannot
				mov    edi,DMon
				mov    ecx, [edi+0x38]     // Dest position
				push   3
				push   PowerLevel  // level of creature
				push  -1
				push   1
				push   ecx
				mov    ecx, 0x699420
				mov    ecx,[ecx]
				push   Spell // Spell Index
				mov    eax,0x5A0140
				call   eax
			}
lCannot:;
		}
	}
/*
	if(val<1) RETURNV
	if(Spell<0) RETURNV
	if(Spell>=MAGICS) RETURNV
	if(Random(1,100)>val) RETURNV
	int num=*((int *)&Mon[0x4C]);
	if(num<1) RETURNV
	if(*((int *)&DMon[0x4C])<=0) RETURNV

//  Spell=SCastSpell(num,DMon,hpa,hpd,Spell);
	int side=*((int *)&Mon[0xF4]);
	if(*((int *)&Mon[0x288])){ side=!side; }
	int PowerLevel=3; 
	if((Ch=='J') || (Ch=='K')) PowerLevel=2;
	if((Ch=='j') || (Ch=='k')) PowerLevel=1;
	if(Spell==14){ // Earthquake
		if(side!=0) RETURNV // not attacker
		__asm{
			mov   eax,0x699420
			mov   eax,[eax]
			mov   eax,[eax+0x132F4]
			mov   dummy,eax
		}
		if(dummy <1) RETURNV // no town 
	}
	__asm{
		mov    ecx,0x699420
		mov    ecx,[ecx]
		push   1
		push   1
		push   DMon  // -> DMon
		push   side  // Side
		push   Spell // Spell Index
		mov    eax,0x5A8950
		call   eax
		test   al, al
		jz     lCannot
		mov    edi,DMon
		mov    ecx, [edi+0x38]     // Dest position
		push   3
		push   PowerLevel  // level of creature
		push  -1
		push   1
		push   ecx
		mov    ecx, 0x699420
		mov    ecx,[ecx]
		push   Spell // Spell Index
		mov    eax,0x5A0140
		call   eax
	}
lCannot:;
*/
	ESCP_AMon=0; ESCP_DMon=0; ESCP_CastType=0;
	RETURNV
}
void CrExpBon::CastMassSpell(Byte *Mon,Byte *DMon) // может скастовать спел при атаке
{
	STARTNA(__LINE__, 0)
	IntCastMassSpell(Mon,DMon,'p');
	IntCastMassSpell(Mon,DMon,'J');
	IntCastMassSpell(Mon,DMon,'j');
	RETURNV
}
void CrExpBon::CastMassSpell2(Byte *Mon,Byte *DMon) // может скастовать спел при атаке ПОСЛЕ УДАРА
{
	STARTNA(__LINE__, 0)
	IntCastMassSpell(Mon,DMon,'a');
	IntCastMassSpell(Mon,DMon,'K');
	IntCastMassSpell(Mon,DMon,'k');
	RETURNV
}
///////////////
// наложить заклинание в начале раунда
static float RetFloat;
void CrExpBon::ApplySpell(Byte *Mon,Byte *BatMan) // наложить заклинание в начале раунда
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI) RETURNV // to disable AI thinking
//  if(!Prepare(Mon))  RETURNV
	int i,j,Spell,Level,dummy;
	if(BatMan==0){
		__asm{
			mov   eax,0x699420
			mov   eax,[eax]
			mov   BatMan,eax
		}
	}
	for(i=0;i<14;i++){
		if(!Prepare(Mon))  RETURNV
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 's':
				Spell=*(Byte *)&ebp->Mod;
				Level=ebp->Lvls[BB_Lvl];
				if(Level<1) RETURNV
				if(Spell<0) RETURNV
				if(Spell>=MAGICS) RETURNV
				if(Level<4) SetMonMagic(Mon,Spell,999,Level,0);
				else{
					if(Level==6) SetMonMagic(Mon,Spell,999,3,0);
					int side=*((int *)&Mon[0xF4]);
					if(*((int *)&Mon[0x288])){ side=!side; }
					_Hero_ *hpa,*hpd;
					hpa=*(_Hero_ **)&BatMan[0x53CC+side*4];
					hpd=*(_Hero_ **)&BatMan[0x53CC+(1-side)*4];
					for(j=0;j<(21*2);j++){
						// 3.58 Tower skip
						if(j==20) continue; // 3.58
						if(j==41) continue; // 3.58
						Byte *mon=&BatMan[0x54CC+0x548*j];
						if(*((int *)&mon[0x288])){ if(*((int *)&mon[0xF4])==side) continue; }
						else{ if(*((int *)&mon[0xF4])!=side) continue; }
						if(*((int *)&mon[0x4C])<=0) continue;
						int tp=*(int *)&mon[0x34];
						if(tp==-1) continue;
						if(Level<5){
							SetMonMagic(mon,Spell,999,3,0);
						}else{
							__asm{
								mov    eax,hpd
								push   hpa  // _Hero_ * Owner
								push   eax
								mov    ecx,Spell
								mov    edx,tp
								mov    eax,0x44A1A0
								call   eax
								fstp   RetFloat
							}
							__asm{
								mov   eax,0x687FA8
								mov   eax,[eax]
								mov   edx,Spell
								shl   edx,4
								add   edx,Spell
								shl   edx,3
								add   eax,edx
								mov   eax,[eax]
								mov   dummy,eax
							}
							int Ret=(int)(RetFloat*100.0f);
							if(dummy<=0){
								Ret=CrExpBon::DwarfResist(mon,100-Ret,Spell);
							}else{ // дружественное
								Ret=CrExpBon::DwarfResistFriendly(mon,100-Ret,Spell);
							}
							if(Ret>=100) continue; // complete immunity
							if(Random(0,100)>Ret) SetMonMagic(mon,Spell,999,3,0);
						}
					}
				}
				break;
			case 'U':  // вызов каждый 4-й турн
			case 'u': // вызов один раз
			case 'l':{ // вызов каждый раз, начиная со второго райнда клона
				Byte *bh;
				int x,y,CodeP,Flags=0;
				if(BatMan[0x13D68]) break; // tactic phase
				if(*((int *)&Mon[0x5C])==-1) break; // already summoned
				int Type=*(Byte *)&ebp->Mod;
//        int Num=ebp->Lvls[BB_Lvl];
				float val1,val=(float)*(int *)&Mon[0x4C]; // число монстров
				val1=val;
				ApplyMod(&val,'%',ebp->Lvls[BB_Lvl]);
				int Num=(int)(val-val1);
				if(Num<1) break;
				int Day=(*(int *)&((Byte *)BatMan)[0x13D6C]);
				if(ebp->Type=='u'){
					if(Day!=1) break;
				}else if(ebp->Type=='U'){
					if((Day%4)!=1) break;
				}else{
					if(Day<1) break;
					Flags=0x00C00000;
				}
				int Redraw=0;
				if(Day>1) Redraw=1;
				int Code=*(int *)&Mon[0x38];
				int side=*((int *)&Mon[0xF4]);
				if(*((int *)&Mon[0x288])){ side=!side; }
				int Shift=0;
				if((*(Dword *)&Mon[0x84])&1) Shift=2; // double wide
				if(M2B_FindPos(Code,&x,&y)==-1) break;
				do{
					if(side){
						if(M2B_CheckPosCode(x-2-Shift,y)==0){
							if((CodeP=M2B_GetPosCode(x-2-Shift,y))!=0){
								bh=(Byte *)GetHexStr(CodeP);
								if((bh!=0) && ((bh[0x10]&0x3F)==0)){
									if(SammonCreatureEx(Type,Num,CodeP,side,-1,Redraw,Flags)) break;
								}
							}
						}
						if(M2B_CheckPosCode(x-1-Shift,y-1)==0){
							if((CodeP=M2B_GetPosCode(x-1-Shift,y-1))!=0){
								bh=(Byte *)GetHexStr(CodeP);
								if((bh!=0) && ((bh[0x10]&0x3F)==0)){
									if(SammonCreatureEx(Type,Num,CodeP,side,-1,Redraw,Flags)) break;
								}
							}
						}
						if(M2B_CheckPosCode(x-1-Shift,y+1)==0){
							if((CodeP=M2B_GetPosCode(x-1-Shift,y+1))!=0){
								bh=(Byte *)GetHexStr(CodeP);
								if((bh!=0) && ((bh[0x10]&0x3F)==0)){
									if(SammonCreatureEx(Type,Num,CodeP,side,-1,Redraw,Flags)) break;
								}
							}
						}
					}else{
						if(M2B_CheckPosCode(x+2+Shift,y)==0){
							if((CodeP=M2B_GetPosCode(x+2+Shift,y))!=0){
								bh=(Byte *)GetHexStr(CodeP);
								if((bh!=0) && ((bh[0x10]&0x3F)==0)){
									if(SammonCreatureEx(Type,Num,CodeP,side,-1,Redraw,Flags)) break;
								}
							}
						}
						if(M2B_CheckPosCode(x+1+Shift,y-1)==0){
							if((CodeP=M2B_GetPosCode(x+1+Shift,y-1))!=0){
								bh=(Byte *)GetHexStr(CodeP);
								if((bh!=0) && ((bh[0x10]&0x3F)==0)){
									if(SammonCreatureEx(Type,Num,CodeP,side,-1,Redraw,Flags)) break;
								}
							}
						}
						if(M2B_CheckPosCode(x+1+Shift,y+1)==0){
							if((CodeP=M2B_GetPosCode(x+1+Shift,y+1))!=0){
								bh=(Byte *)GetHexStr(CodeP);
								if((bh!=0) && ((bh[0x10]&0x3F)==0)){
									if(SammonCreatureEx(Type,Num,CodeP,side,-1,Redraw,Flags)) break;
								}
							}
						}
					}
				}while(0);
				break;}
		}
	}
	RETURNV
}
///////////////
int CrExpBon::IsHarpy(Byte *Mon)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	int val=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		if(ebp->Type=='f' && ebp->Mod=='b'){
			val=ebp->Lvls[BB_Lvl];
		}
	}
	RETURN(val)
}
int CrExpBon::ReduceDefence(Byte *AMon,Byte * /*DMon*/,int Def)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(Def) // to disable AI thinking
	if(!Prepare(AMon)) RETURN(Def)
	int ret=Def;
	float val=0.0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'b':
				ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				if(val<0.0) val=0.0;
				if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==6 ){ // Art Sub 6
					val+=25;
				}
				if(val>100.0) val=100.0;
				ret=int(Def*(100.0f-val)/100.0f);
				RETURN(ret)
		}
	}
//_Ok:
	if(BB_Cr!=0 && BB_Cr->HasArt() && BB_Cr->GetSubArt()==6 ){ // Art Sub 6
		ret=(int)(Def*0.75);
	}
	RETURN(ret)
}
// бонус чемпиона
int CrExpBon::Champion(Byte * Mon)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	int val=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		if(ebp->Type=='f' && ebp->Mod=='c'){
			val=ebp->Lvls[BB_Lvl];
		}
	}
	if(val==1) RETURN(11)
	else RETURN(0)
}
// сопротивление Голема (только добавляет сопротивление)
int CrExpBon::GolemResist(Byte *Mon,int Dam,int OrDam,int Spell)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(Dam) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(Dam)
	float val;
	do{
		if(Spell!=29) break; // Fire Shield
		for(int i=0;i<14;i++){
			CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
			if(ebp->Fl.Act==0) continue;
			switch(ebp->Type){
				case 'w': // spell specific resistance
					if(ebp->Lvls[BB_Lvl]!=1) break;
					switch(ebp->Mod){
						case '9':
						case ':': // immune to spell of levels
						case '4':
						case '5': // immune to spell of levels
						case 'O': // immune to Fire Direct Damage Spell
						case 'D': // immune to All Direct Damage Spell
						case 'F': // immune to Fire schools
						case 'f': // immune to Fire schools
							if(Spell==29) RETURN(0); // complete immunity
							break;
					}
			}
		}
	}while(0);
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'g':
//        ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				val=OrDam*(1.0f-ebp->Lvls[BB_Lvl]/100.0f);
				if(val>OrDam) val=(float)OrDam;
				if(val<0) val=0;
				RETURN((int)val);
		}
	}
	RETURN(Dam)
}
// иммунитет к Dispell и др.
//float CrExpBon::DispellResistAndOther(Byte *Mon,int Spell,int ASide,float Res)
int CrExpBon::DispellResist(int Spell,Byte *Mon,int ASide)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	int side=*((int *)&Mon[0xF4]);
	if(*((int *)&Mon[0x288])){ side=!side; }
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'w': // spell specific resistance
				if(ebp->Lvls[BB_Lvl]!=1) break;
				switch(ebp->Mod){
					case 'P': // immune to Any Dispell
						if(Spell==35) RETURN(1); // complete immunity
						break;
					case 'p': // immune to Hostile Dispell
						if((Spell==35)&&(ASide!=side)) RETURN(1); // complete immunity
						break;
					case 'N': // immune to Dispell Benefic. Spells
						if(Spell==78) RETURN(1); // complete immunity
						break;
				}
		}
	}
	RETURN(0)
}
 // регенерация
int CrExpBon::Regenerate(Byte *Mon,int HPloss)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	int val;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'r':
//        ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
				val=ebp->Lvls[BB_Lvl];
				if(Random(1,100)>ebp->Mod) RETURN(0)
				if(val>HPloss) val=HPloss;
				if(val<0) val=0;
				RETURN(val);
		}
	}
	RETURN(0)
}
 // позитивная мораль в битве
int CrExpBon::MinotourMoral(Byte *Mon)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	int val=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		if(ebp->Type=='f' && ebp->Mod=='m'){
			val=ebp->Lvls[BB_Lvl];
		}
	}
	if(val==1) RETURN(0x4F)
	else RETURN(0)
}
// магич. аура
int CrExpBon::UnicornAura(Byte *Mon)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	int val=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		if(ebp->Type=='f' && ebp->Mod=='u'){
			val=ebp->Lvls[BB_Lvl];
		}
	}
	if(val==1) RETURN(0x19)
	else RETURN(0)
}
// стреляет даже если противник рядом
int CrExpBon::ShootAnyTime(Byte *Mon)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	int val=0;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		if(ebp->Type=='f' && ebp->Mod=='s'){
			val=ebp->Lvls[BB_Lvl];
		}
	}
	if(val==1) RETURN(1)
	else RETURN(0)
}
// бонус существ для SG
int CrExpBon::SGBonus(Byte *Mon,int *Type,int *Num)
{
	STARTNA(__LINE__, 0)
	if(G2B_CompleteAI)  RETURN(0) // to disable AI thinking
	if(!Prepare(Mon)) RETURN(0)
	float val=(float)*Num;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Cur[i];//&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		if(ebp->Type!='G') continue;
		if(ebp->Mod=='='){
			if(ebp->Lvls[BB_Lvl]==1) *Type+=1;
		}else{
			ApplyMod(&val,ebp->Mod,ebp->Lvls[BB_Lvl]);
			*Num=(int)val;
		}
	}
	RETURN(1)
}
///////////////
// наложить массовое заклинание в начале раунда
/*
void CrExpBon::ApplyMassSpell(Byte *Mon) // наложить заклинание в начале раунда
{
	STARTC("Stack: Apply Mass Spell ability",0)
	if(G2B_CompleteAI) RETURNV // to disable AI thinking
	if(!Prepare(Mon))  RETURNV
	int Level=0;
	int Spell=-1;
	for(int i=0;i<14;i++){
		CrExpBonStr *ebp=&Body[BB_Type][i];
		if(ebp->Fl.Act==0) continue;
		switch(ebp->Type){
			case 'E':
				Spell=*(Byte *)&ebp->Mod;
				Level=ebp->Lvls[BB_Lvl];
				break;
		}
	}
	if(Level<1) RETURNV
	if(Level>3) RETURNV
	if(Spell<0) RETURNV
	if(Spell>=MAGICS) RETURNV
	SetMonMassMagic(Mon,Spell,Level);
}
*/
////////////////////////
static char MI_Bon[100];
static char MI_Buf[32];
static char *Bon2Str(int val,char mod,int mode){
	STARTNA(__LINE__, 0)
	MI_Bon[0]=0;
	switch(mode){
		case 2:
			switch(val){
				case 2: strcpy(MI_Bon,ITxt(93,1,&CrExpTXT)/*"??"*/); break;
				case 1: strcpy(MI_Bon,ITxt(94,1,&CrExpTXT)/*"YES"*/); break;
				default: strcpy(MI_Bon,ITxt(95,1,&CrExpTXT)/*"NO"*/); break;
			}
			break;
		case 1:
			switch(val){
				case 2: strcpy(MI_Bon,ITxt(96,1,&CrExpTXT)/*"No specific settings"*/); break;
				case 1: strcpy(MI_Bon,ITxt(97,1,&CrExpTXT)/*"Has"*/); break;
				default: strcpy(MI_Bon,ITxt(98,1,&CrExpTXT)/*"Does'not have"*/); break;
			}
			break;
		default:
			switch(mod){
				case '+':
					if(val>=0) strcpy(MI_Bon,ITxt(99,1,&CrExpTXT)/*"+"*/);
					else MI_Bon[0]=0;
					strcat(MI_Bon,itoa(val,MI_Buf,10));
					break;
				case '-':
					if(val>=0) strcpy(MI_Bon,ITxt(221,1,&CrExpTXT)/*"-"*/);
					else MI_Bon[0]=0;
					strcat(MI_Bon,itoa(val,MI_Buf,10));
					break;
				case '%':
					if(val>=0) strcpy(MI_Bon,ITxt(100,1,&CrExpTXT)/*""*/);
					else MI_Bon[0]=0;
					strcat(MI_Bon,itoa(val,MI_Buf,10)); strcat(MI_Bon,ITxt(101,1,&CrExpTXT)/*"%"*/);
					break;
				case '=':
					strcpy(MI_Bon,ITxt(102,1,&CrExpTXT)/*"="*/); strcat(MI_Bon,itoa(val,MI_Buf,10));
					break;
			}
	}
	RETURN(MI_Bon)
}
static _MonsterInfo MonsterInfo;
static char MI_DefName[256];
static char MI_Expo[256];
//static char MI_Caption[256];
static char MI_Levels[256];
static char IconTexts[8][256];
static char IconHintTexts[8][256];
static char SArtIcon[256];
static char SArtHint[256];
static char *RowCaptions[16],*RowCaptionHints[16],
						*TxtProperties[8]={
							IconTexts[0],IconTexts[1],IconTexts[2],IconTexts[3],IconTexts[4],IconTexts[5],IconTexts[6],IconTexts[7]
						};
static char *HintProperties[8]={
							IconHintTexts[0],IconHintTexts[1],IconHintTexts[2],
							IconHintTexts[3],IconHintTexts[4],IconHintTexts[5],
							IconHintTexts[6],IconHintTexts[7]
						};
static char *IcoProperties[8];
static char Rows[16][256];
static char RowHs[16][256];
static char *ARows[16]={Rows[0],Rows[1],Rows[2],Rows[3],Rows[4],Rows[5],Rows[6],Rows[7],
												Rows[8],Rows[9],Rows[10],Rows[11],Rows[12],Rows[13],Rows[14],Rows[15]},
						*RowHints[16]={RowHs[0],RowHs[1],RowHs[2],RowHs[3],RowHs[4],RowHs[5],RowHs[6],RowHs[7],
												RowHs[8],RowHs[9],RowHs[10],RowHs[11],RowHs[12],RowHs[13],RowHs[14],RowHs[15]};
static char *ArtIcon=".\\DATA\\ZVS\\LIB1.RES\\E_ART.bmp";
static char *MagicImmune[][2]={
/* '1' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPLVL1.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPLVL1.bmp"},
/* '2' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPLVL2.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPLVL2.bmp"},
/* '3' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPLVL3.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPLVL3.bmp"},
/* '4' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPLVL4.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPLVL4.bmp"},
/* '5' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPLVL5.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPLVL5.bmp"},
/* 'C' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPCOLD.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPCOLD.bmp"},
/* 'R' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPARM.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPARM.bmp"},
/* 'A' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPAIR.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPAIR.bmp"}, //7
/* 'F' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPFIRE.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPFIRE.bmp"}, //8
/* 'W' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPWATER.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPWATER.bmp"}, //9
/* 'E' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPEATH.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPEATH.bmp"}, //10
/* 'L' */
{".\\DATA\\ZVS\\LIB1.RES\\E_LIGHT.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_LIGHT.bmp"}, //11
/* '0' */
{".\\DATA\\ZVS\\LIB1.RES\\E_NOFRIM.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_NOFRIM.bmp"}, //12
/* 'M' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPMET.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPMET.bmp"}, //13
/* 'I' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPIMP.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPIMP.bmp"}, //14
/* 'P' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPDISP.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPDISP.bmp"}, //15
/* 'N' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPDISB.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPDISB.bmp"}, //16
/* 'B' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPBLIND.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPBLIND.bmp"}, //17
/* 'K' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPBERS.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPBERS.bmp"}, //18
/* 'H' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPHYPN.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPHYPN.bmp"}, //19
/* 'S' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPSLOW.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPSLOW.bmp"}, //20
/* 'O' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPDFIRE.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPDFIRE.bmp"}, //21
/* 'D' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPDIR.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPDIR.bmp"}, //22
/* 'p' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPDISP.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPDISP.bmp"}, //23
/* 'm' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPAWILL.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPAWILL.bmp"}, //24
/* 'a' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPAIR1.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPAIR1.bmp"}, //25
/* 'f' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPFIRE1.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPFIRE1.bmp"}, //26
/* 'w' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPWATER1.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPWATER1.bmp"}, //27
/* 'e' */
{".\\DATA\\ZVS\\LIB1.RES\\E_SPEATH1.bmp",".\\DATA\\ZVS\\LIB1.RES\\N_SPEATH1.bmp"} //28
};

static char *_IconPic[8];
static int   _IconTxt[8];
static void SetIconText(char *dst,char *hdr,char *txt){
	STARTNA(__LINE__, 0)
	strcpy(dst,hdr);
	strcat(dst,"\x0A");
	strcat(dst,txt);
	RETURNV
}
//static int SI_Expo,SI_Mon,SI_Num,SI_Trig;
static int SpecButtonPush(void);
int OnControlClick(int ContorIndex,int MouseButton,int ShiftState);
static int RI_Mon,RI_Num,RI_Expo;
void CrExpBon::PrepareInfo(int Mon,int Num,int Expo,CrExpo *Cr,int Changing)
{
	STARTNA(__LINE__, 0)
	int i,j,k,l,m,s,yes,self,hi,hk,val;
	if(Mon<0 || Mon>=MONNUM) RETURNV
	RI_Mon=Mon; RI_Num=Num; RI_Expo=Expo;
	for(i=0;i<16;i++){
		RowCaptions[i]=0;
		RowCaptionHints[i]=0;
		Rows[i][0]=0;
		RowHs[i][0]=0;
	}
	for(i=0;i<8;i++){
		TxtProperties[i][0]=0;
		HintProperties[i][0]=0;
		IcoProperties[i]=0;
		_IconPic[i]=0;
		_IconTxt[i]=-1;
	}

	if((Cr!=0) && (Cr->HasArt()!=0)){ 
		sprintf(SArtIcon,"[LODDEF]H3Sprite.lod;Artifact.def;%i;0;0",Cr->GetArt()+1);
		sprintf(SArtHint,ITxt(364,1,&CrExpTXT),ITxt(Cr->GetSubArt()+340,1,&CrExpTXT));
//    strcpy(SArtHint,ITxt(Cr->GetSubArt()+340,1,&CrExpTXT));
		if(Cr->ArtCopy()>0){ MonsterInfo.ArtCopy=Cr->ArtCopy()+1; }else{ MonsterInfo.ArtCopy=-1; }
	}else{ 
		strcpy(SArtIcon,"NONE");
		strcpy(SArtHint,ITxt(362,1,&CrExpTXT));
		MonsterInfo.ArtCopy=-1;
	}
	MonsterInfo.ArtIcon=SArtIcon;         // иконка артифакта ("NONE" - пустой)
	MonsterInfo.ArtHint=SArtHint;         // хинт для иконки артифакта
	MonsterInfo.ArtOutput=0;         // артифакт отдается - 1, оставляется - 0
	MonsterInfo.Flags=0;             // флаги 
	if(Changing) MonsterInfo.Flags|=0x00000001;

//  MonsterInfo.Caption=ITxt(13,1,&CrExpTXT);
	strcpy(MI_DefName,"[LODDEF]H3sprite.lod;TwCrPort.def;");
	strcat(MI_DefName,itoa(Mon+3,MI_Buf,10));
	strcat(MI_DefName,";-1;-1");
	MonsterInfo.Picture=MI_DefName;

//  MonsterInfo.Info="дополнительная информация о твари";
//  int Rank=Expo*100/EXPOMUL/CrExpMod::Limit(Mon)/10; if(Rank<0) Rank=0; if(Rank>10) Rank=10;
//  int Expo=CrExpoSet::Cur.Expo;
	int Rank=CrExpMod::GetRank(CURMON,Expo);
//  sprintf(MI_Caption,ITxt(13,1,&CrExpTXT),CrExpMod::GetRankExp(Mon,Rank+1)-Expo,CrExpMod::Cap(Mon),CrExpMod::ExpMul(Mon));
//  MonsterInfo.Caption=MI_Caption;
	MonsterInfo.Caption=ITxt(13,1,&CrExpTXT);

	if(Num<2){ 
		strcpy(MI_Expo,"");
	}else{
		strcpy(MI_Expo,itoa(Num,MI_Buf,10)); strcat(MI_Expo," ");
	}
	strcat(MI_Expo,ITxt(Rank+1,1,&CrExpTXT));
	strcat(MI_Expo," ");
	if(Num<2){ strcat(MI_Expo,MonTable[Mon].NameS); }else{ strcat(MI_Expo,MonTable[Mon].NameP); }
	strcat(MI_Expo,ITxt(14,1,&CrExpTXT)/*" [Level "*/);
	strcat(MI_Expo,itoa(Rank,MI_Buf,10));
	if(Num<2){ strcat(MI_Expo,ITxt(15,1,&CrExpTXT)/*"] has "*/); }else{ strcat(MI_Expo,ITxt(16,1,&CrExpTXT)/*") have "*/); }
	strcat(MI_Expo,itoa(Expo,MI_Buf,10));
	strcat(MI_Expo,ITxt(17,1,&CrExpTXT)/*" out of "*/);
//  strcat(MI_Expo,itoa(CrExpMod::MaxExpo(Mon)*EXPOMUL,MI_Buf,10));
	strcat(MI_Expo,itoa(CrExpMod::MaxExpo(CURMON),MI_Buf,10));
	strcat(MI_Expo,ITxt(18,1,&CrExpTXT)/*" Experience Points."*/);
	MonsterInfo.Info=MI_Expo;

//  MonsterInfo.ColCaptions="1      2      3      4      5      6      7      8      9      10     11     ";
	for(i=0;i<11;i++){
		strncpy(&MI_Levels[i*8],ITxt(i+1,1,&CrExpTXT),7);
	}
	for(i=0;i<8*11;i++) if(MI_Levels[i]<0x20) MI_Levels[i]=' ';
	MonsterInfo.ColCaptions=MI_Levels;
	MonsterInfo.ColHint=ITxt(122,1,&CrExpTXT);

	RowCaptions[0]=ITxt(19,1,&CrExpTXT)/*"% Exp. Ranks"*/;
	RowCaptionHints[0]=ITxt(20,1,&CrExpTXT)/*"A Percentage of Experience Ranks"*/;
	RowCaptions[1]=ITxt(21,1,&CrExpTXT)/*"# Exp. Ranks"*/;
	RowCaptionHints[1]=ITxt(22,1,&CrExpTXT)/*"An Absolute Experience Points of Experience Ranks"*/;
//  strcpy(Rows[0],"");
	for(i=0;i<8*11;i++) Rows[0][i]=' ';
//  strcpy(Rows[1],"");
	for(i=0;i<8*11;i++) Rows[1][i]=' ';
	val=CrExpMod::Limit(CURMON); if(val==0) val=1;
	for(i=0;i<11;i++){
//    strcpy(&Rows[0][i*8],itoa(i*10,MI_Buf,10));
		strcpy(&Rows[0][i*8],itoa(CrExpMod::GetRankExp(CURMON,i)*100/val,MI_Buf,10));
		strcat(&Rows[0][i*8],"%");
//    strcpy(&Rows[1][i*8],itoa(CrExpMod::Limit(Mon)*EXPOMUL*i/10,MI_Buf,10));
		strcpy(&Rows[1][i*8],itoa(CrExpMod::GetRankExp(CURMON,i),MI_Buf,10));
	}
	for(i=0;i<8*11;i++){
		if(Rows[0][i]<0x20) Rows[0][i]=' ';
		if(Rows[1][i]<0x20) Rows[1][i]=' ';
	}

	CrExpBonStr *str,*str0;
	str0=Cur;//Get(VMon,Mon);
	// if BF settings undefined, take a global
//  if(str0->Fl.Act==0) str0=&Body[Mon][0];
	for(i=k=0;i<14;i++){
		yes=0; self=0; hi=0; hk=0;
//    str=&Body[Mon][i];
		str=&str0[i];
		if(str->Fl.Act==0) continue;
		switch(str->Type){
			case 'A': RowCaptions[i+2]=ITxt(23,1,&CrExpTXT)/*"Attack"*/;
				RowCaptionHints[i+2]=ITxt(24,1,&CrExpTXT)/*"Attack Bonus that stack has."*/;
				break;
			case 'D': RowCaptions[i+2]=ITxt(25,1,&CrExpTXT)/*"Defence"*/;
				RowCaptionHints[i+2]=ITxt(26,1,&CrExpTXT)/*"Defence Bonus that stack has."*/;
				break;
			case 'H': RowCaptions[i+2]=ITxt(27,1,&CrExpTXT)/*"Hit Points"*/;
				RowCaptionHints[i+2]=ITxt(28,1,&CrExpTXT)/*"Hit Points Bonus that stack has."*/;
				break;
			case 'm': RowCaptions[i+2]=ITxt(29,1,&CrExpTXT)/*"Min Damage"*/;
				RowCaptionHints[i+2]=ITxt(30,1,&CrExpTXT)/*"Minimum Damage Bonus that stack has."*/;
				break;
			case 'M': RowCaptions[i+2]=ITxt(31,1,&CrExpTXT)/*"Max Damage"*/;
				RowCaptionHints[i+2]=ITxt(32,1,&CrExpTXT)/*"Maximum Damage Bonus that stack has."*/;
				break;
			case 'S': RowCaptions[i+2]=ITxt(33,1,&CrExpTXT)/*"Speed"*/;
				RowCaptionHints[i+2]=ITxt(34,1,&CrExpTXT)/*"Speed Bonus that stack has."*/;
				break;
			case 'O': RowCaptions[i+2]=ITxt(35,1,&CrExpTXT)/*"Shots"*/;
				RowCaptionHints[i+2]=ITxt(36,1,&CrExpTXT)/*"Number of Shots Bonus that stack has."*/;
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(237,1,&CrExpTXT));
				hi=i+2; hk=k; 
				_IconTxt[k]=365;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_SHOOTN.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_SHOOTN.bmp";
				k++;
				break;
			case 'P': RowCaptions[i+2]=ITxt(37,1,&CrExpTXT)/*"Casts"*/;
				RowCaptionHints[i+2]=ITxt(38,1,&CrExpTXT)/*"Number of Spell Casting Bonus that stack has."*/;
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(236,1,&CrExpTXT));
				hi=i+2; hk=k;
				_IconTxt[k]=366;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_CASTER.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_CASTER.bmp";
				k++;
				break;
			case 'F': RowCaptions[i+2]=ITxt(107,1,&CrExpTXT);/*Fear*/
				RowCaptionHints[i+2]=ITxt(108,1,&CrExpTXT);
				yes=1;
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(109,1,&CrExpTXT));
				hi=i+2; hk=k;
				_IconTxt[k]=367;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_FEAR.bmp";
				if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_FEAR.bmp";
				k++;
				break;
			case 'R': RowCaptions[i+2]=ITxt(110,1,&CrExpTXT);// Number of retaliations
				RowCaptionHints[i+2]=ITxt(111,1,&CrExpTXT);
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(238,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(238,1,&CrExpTXT));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=368;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_RETAIL1.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_RETAIL1.bmp";
				k++;
				break;
			case 'd': RowCaptions[i+2]=ITxt(112,1,&CrExpTXT);// take defence bonus
				RowCaptionHints[i+2]=ITxt(113,1,&CrExpTXT);
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(239,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(239,1,&CrExpTXT));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=369;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_DEFBON.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_DEFBON.bmp";
				k++;
				break;
			case 'W': RowCaptions[i+2]=ITxt(114,1,&CrExpTXT);// Dwarf Resistance
				RowCaptionHints[i+2]=ITxt(115,1,&CrExpTXT);
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(240,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(240,1,&CrExpTXT));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=370;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_DWARF.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_DWARF.bmp";
				k++;
				break;
			case 'g': RowCaptions[i+2]=ITxt(222,1,&CrExpTXT);// Golem Resistance
				RowCaptionHints[i+2]=ITxt(223,1,&CrExpTXT);
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(241,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(241,1,&CrExpTXT));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=371;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_GOLEM.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_GOLEM.bmp";
				k++;
				break;
			case 'i': RowCaptions[i+2]=ITxt(116,1,&CrExpTXT);// No Distance Penalty
				RowCaptionHints[i+2]=ITxt(117,1,&CrExpTXT);
				yes=1;
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(118,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(118,1,&CrExpTXT));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=372;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_DIST.bmp";
				if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_DIST.bmp";
				k++;
				break;
			case 'o': RowCaptions[i+2]=ITxt(119,1,&CrExpTXT);// No Distance Penalty
				RowCaptionHints[i+2]=ITxt(120,1,&CrExpTXT);
				yes=1;
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(121,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(121,1,&CrExpTXT));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=373;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_OBST.bmp";
				if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_OBST.bmp";
				k++;
				break;
			case 'f':
				yes=1;
				switch(str->Mod){
					case 'F': // can fly
						RowCaptions[i+2]=ITxt(39,1,&CrExpTXT)/*"Fly [*]"*/;
						RowCaptionHints[i+2]=ITxt(40,1,&CrExpTXT)/*"Creature can Fly."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(41,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(41,1,&CrExpTXT))/*"Can Fly (ignore obstacles)."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=374;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_FLY.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_FLY.bmp";
						k++;
						break;
					case 'S': // can shoot
						RowCaptions[i+2]=ITxt(42,1,&CrExpTXT)/*"Shoot [*]"*/;
						RowCaptionHints[i+2]=ITxt(43,1,&CrExpTXT)/*"Creature can Shoot."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(44,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(44,1,&CrExpTXT))/*"Can Shoot."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=375;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_SHOOT.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_SHOOT.bmp";
						k++;
						break;
					case 'B': // breath attack
						RowCaptions[i+2]=ITxt(45,1,&CrExpTXT)/*"Breath [*]"*/;
						RowCaptionHints[i+2]=ITxt(46,1,&CrExpTXT)/*"Creature has a Breath Attack."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(47,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(47,1,&CrExpTXT))/*"Has a Breath Attack."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=376;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_BREATH.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_BREATH.bmp";
						k++;
						break;
					case 'L': // Alive
						RowCaptions[i+2]=ITxt(48,1,&CrExpTXT)/*"Alive [*]"*/;
						RowCaptionHints[i+2]=ITxt(49,1,&CrExpTXT)/*"Creature is Alive."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(50,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(50,1,&CrExpTXT))/*"Alive."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=377;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_ALIVE.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_ALIVE.bmp";
						k++;
						break;
					case '1': // KING 1
						RowCaptions[i+2]=ITxt(51,1,&CrExpTXT)/*"KING 1 [*]"*/;
						RowCaptionHints[i+2]=ITxt(52,1,&CrExpTXT)/*"Creature belongs to KING 1 group."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(53,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(53,1,&CrExpTXT))/*"Belongs to KING 1 group."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=378;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_KING1.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_KING1.bmp";
						k++;
						break;
					case '2': // KING 2
						RowCaptions[i+2]=ITxt(54,1,&CrExpTXT)/*"KING 2 [*]"*/;
						RowCaptionHints[i+2]=ITxt(55,1,&CrExpTXT)/*"Creature belongs to KING 2 group."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(56,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(56,1,&CrExpTXT))/*"Belongs to KING 2 group."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=379;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_KING2.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_KING2.bmp";
						k++;
						break;
					case '3': // KING 3
						RowCaptions[i+2]=ITxt(57,1,&CrExpTXT)/*"KING 3 [*]"*/;
						RowCaptionHints[i+2]=ITxt(58,1,&CrExpTXT)/*"Creature belongs to KING 3 group."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(59,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(59,1,&CrExpTXT))/*"Belongs to KING 3 group."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=380;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_KING3.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_KING3.bmp";
						k++;
						break;
					case 'P': // не чуствителен к псих атаке
						RowCaptions[i+2]=ITxt(60,1,&CrExpTXT)/*"Mind [*]"*/;
						RowCaptionHints[i+2]=ITxt(61,1,&CrExpTXT)/*"Creature is immune to Mind spells."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(62,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(62,1,&CrExpTXT))/*"Immune to Mind spells."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=381;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_MIND.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_MIND.bmp";
						k++;
						break;
					case 'E': // no melee penalty
						RowCaptions[i+2]=ITxt(63,1,&CrExpTXT)/*"Melee [*]"*/;
						RowCaptionHints[i+2]=ITxt(64,1,&CrExpTXT)/*"Creature has no Melee penalty."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(65,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(65,1,&CrExpTXT))/*"Has no Melee penalty."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=382;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_MELEE.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_MELEE.bmp";
						k++;
						break;
					case 'I': // fire magic immunity
						RowCaptions[i+2]=ITxt(66,1,&CrExpTXT)/*"Fire [*]"*/;
						RowCaptionHints[i+2]=ITxt(67,1,&CrExpTXT)/*"Creature is immune to Fire."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(68,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(68,1,&CrExpTXT))/*"Immune to Fire."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=383;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_FIRE.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_FIRE.bmp";
						k++;
						break;
					case 'D': // Double strike
						RowCaptions[i+2]=ITxt(69,1,&CrExpTXT)/*"Double [*]"*/;
						RowCaptionHints[i+2]=ITxt(70,1,&CrExpTXT)/*"Creature strike twice."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(71,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(71,1,&CrExpTXT))/*"Double Strike."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=384;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_DOUBLE.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_DOUBLE.bmp";
						k++;
						break;
					case 'R': // No retailate
						RowCaptions[i+2]=ITxt(72,1,&CrExpTXT)/*"Retail [*]"*/;
						RowCaptionHints[i+2]=ITxt(73,1,&CrExpTXT)/*"Enemy cannot Retailate."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(74,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(74,1,&CrExpTXT))/*"Enemy cannot Retailate."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=385;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_RETAIL.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_RETAIL.bmp";
						k++;
						break;
					case 'M': // No moral penalty
						RowCaptions[i+2]=ITxt(75,1,&CrExpTXT)/*"Moral [*]"*/;
						RowCaptionHints[i+2]=ITxt(76,1,&CrExpTXT)/*"Creature has No Moral Penalty."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(77,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(77,1,&CrExpTXT))/*"No Moral Penalty."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=386;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_MORAL.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_MORAL.bmp";
						k++;
						break;
					case 'U': // Undead
						RowCaptions[i+2]=ITxt(78,1,&CrExpTXT)/*"Undead [*]"*/;
						RowCaptionHints[i+2]=ITxt(79,1,&CrExpTXT)/*"Creature is Undead."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(80,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(80,1,&CrExpTXT))/*"Undead."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=387;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_UNDEAD.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_UNDEAD.bmp";
						k++;
						break;
					case 'A': // round attack
						RowCaptions[i+2]=ITxt(81,1,&CrExpTXT)/*"Round [*]"*/;
						RowCaptionHints[i+2]=ITxt(82,1,&CrExpTXT)/*"Creature has a Round Attack."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(83,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(83,1,&CrExpTXT))/*"Round Attack."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=388;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_ROUND.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_ROUND.bmp";
						k++;
						break;
					case 'G': // dragon
						RowCaptions[i+2]=ITxt(84,1,&CrExpTXT)/*"Dragon [*]"*/;
						RowCaptionHints[i+2]=ITxt(85,1,&CrExpTXT)/*"Creature has a Dragon Nanure."*/;
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(86,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(86,1,&CrExpTXT))/*"Dragon Nature."*/;
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=389;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_DRAGON.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_DRAGON.bmp";
						k++;
						break;
					case 'b': // attack and return
						RowCaptions[i+2]=ITxt(149,1,&CrExpTXT);
						RowCaptionHints[i+2]=ITxt(150,1,&CrExpTXT);
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(151,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(151,1,&CrExpTXT));
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=390;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_HARPY.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_HARPY.bmp";
						k++;
						break;
					case 'c': // champion distance bonus
						RowCaptions[i+2]=ITxt(218,1,&CrExpTXT);
						RowCaptionHints[i+2]=ITxt(219,1,&CrExpTXT);
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(220,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(220,1,&CrExpTXT));
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=391;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_CHAMP.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_CHAMP.bmp";
						k++;
						break;
					case 'm': // always positive moral
						RowCaptions[i+2]=ITxt(229,1,&CrExpTXT);
						RowCaptionHints[i+2]=ITxt(230,1,&CrExpTXT);
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(231,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(231,1,&CrExpTXT));
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=392;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_MINOT.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_MINOT.bmp";
						k++;
						break;
					case 's': // стреляет даже если противник рядом
						RowCaptions[i+2]=ITxt(232,1,&CrExpTXT);
						RowCaptionHints[i+2]=ITxt(233,1,&CrExpTXT);
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(234,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(234,1,&CrExpTXT));
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=393;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_SHOOTA.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_SHOOTA.bmp";
						k++;
						break;
					case 'f': // бесстрашный
						RowCaptions[i+2]=ITxt(243,1,&CrExpTXT);
						RowCaptionHints[i+2]=ITxt(244,1,&CrExpTXT);
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(245,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(245,1,&CrExpTXT));
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=394;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_FEARL.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_FEARL.bmp";
						k++;
						break;
					case 'r': // rebirth (число заклов действует)
						RowCaptions[i+2]=ITxt(246,1,&CrExpTXT);
						RowCaptionHints[i+2]=ITxt(247,1,&CrExpTXT);
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(248,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(248,1,&CrExpTXT));
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=395;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_REBIRTH.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_REBIRTH.bmp";
						k++;
						break;
					case 'u': // Unicorn Magic Aura
						RowCaptions[i+2]=ITxt(249,1,&CrExpTXT);
						RowCaptionHints[i+2]=ITxt(250,1,&CrExpTXT);
						if(k>MAXICONNUM) break;
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(251,1,&CrExpTXT));
//            strcpy(TxtProperties[k],ITxt(251,1,&CrExpTXT));
//            strcpy(HintProperties[k],RowCaptionHints[i+2]);
						hi=i+2; hk=k;
						_IconTxt[k]=396;
						_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_UNIC.bmp";
						if(str->Lvls[Rank]!=1) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_UNIC.bmp";
						k++;
						break;
					default: RowCaptions[i+2]=ITxt(87,1,&CrExpTXT)/*"??Undef??"*/; break;
				}
				break;
			case 'B':
//        yes=1;
				RowCaptions[i+2]=ITxt(88,1,&CrExpTXT)/*"Block [*]"*/;
				RowCaptionHints[i+2]=ITxt(89,1,&CrExpTXT)/*"Complete Block of a Phisical Attack Bonus that stack has."*/;
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(90,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(90,1,&CrExpTXT))/*"Has a Chance to Block a Phisical Damage."*/;
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=397;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_BLOCK.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_BLOCK.bmp";
				k++;
				break;
			case 'C':
//        yes=1;
				RowCaptions[i+2]=ITxt(103,1,&CrExpTXT)/*"Block [*]"*/;
				RowCaptionHints[i+2]=ITxt(104,1,&CrExpTXT)/*"Complete Block of a Phisical Attack Bonus that stack has."*/;
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(105,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(105,1,&CrExpTXT))/*"Has a Chance to Block a Phisical Damage."*/;
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=398;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_MANA.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_MANA.bmp";
				k++;
				break;
//      case '': RowCaptions[i+2]=""; break;
			case 'e': // DeathBlow
				RowCaptions[i+2]=ITxt(123,1,&CrExpTXT)/*"Block [*]"*/;
				RowCaptionHints[i+2]=ITxt(124,1,&CrExpTXT)/*"Complete Block of a Phisical Attack Bonus that stack has."*/;
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(125,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(125,1,&CrExpTXT))/*"Has a Chance to Block a Phisical Damage."*/;
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=399;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_DBLOW.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_DBLOW.bmp";
				k++;
				break;
			case 'h': // Hatrade
				RowCaptions[i+2]=ITxt(126,1,&CrExpTXT)/*"Block [*]"*/;
				if(str->Lvls[Rank]>200){
					RowCaptionHints[i+2]=ITxt(144,1,&CrExpTXT)/*"Complete Block of a Phisical Attack Bonus that stack has."*/;
				}else{
					RowCaptionHints[i+2]=ITxt(127,1,&CrExpTXT)/*"Complete Block of a Phisical Attack Bonus that stack has."*/;
				}
				for(j=0;j<11;j++){
					if(str->Lvls[j]>200){
						strncpy(&Rows[i+2][j*8],Bon2Str((200-str->Lvls[j])*10,'%',0),8);
					}else{
						strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j]*10,'%',0),8);
					}
				}
				for(j=0;j<8*11;j++) if(Rows[i+2][j]<0x20) Rows[i+2][j]=' ';
				strcpy(RowHints[i+2],ITxt(129,1,&CrExpTXT));
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MONNUM) strcat(RowHints[i+2],MonTable[j].NameP);
				strcat(RowHints[i+2],ITxt(324,1,&CrExpTXT));
				if(str->Lvls[Rank]>200){
					strcat(RowHints[i+2],Bon2Str((200-str->Lvls[Rank])*10,'%',0));
					strcat(RowHints[i+2],ITxt(323,1,&CrExpTXT));
				}else{
					strcat(RowHints[i+2],Bon2Str((str->Lvls[Rank])*10,'%',0));
					strcat(RowHints[i+2],ITxt(322,1,&CrExpTXT));
				}
				if(k>MAXICONNUM) continue;
				if(str->Lvls[Rank]>200){
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(145,1,&CrExpTXT));
//          strcpy(TxtProperties[k],ITxt(145,1,&CrExpTXT));
				}else{
						SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(128,1,&CrExpTXT));
//          strcpy(TxtProperties[k],ITxt(128,1,&CrExpTXT));
				}
				strcat(TxtProperties[k],MonTable[j].NameP);
				strcpy(HintProperties[k],RowHints[i+2]);
				_IconTxt[k]=400;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_HATE.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_HATE.bmp";
				k++;
				MonsterInfo.IcoPropertiesCount=k;
				continue;
			case 'r': // Regeneration
				RowCaptions[i+2]=ITxt(224,1,&CrExpTXT)/*"Block [*]"*/;
				RowCaptionHints[i+2]=ITxt(225,1,&CrExpTXT)/*"Complete Block of a Phisical Attack Bonus that stack has."*/;
				for(j=0;j<11;j++){
					strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],'=',0),8);
				}
				for(j=0;j<8*11;j++) if(Rows[i+2][j]<0x20) Rows[i+2][j]=' ';
				strcpy(RowHints[i+2],ITxt(226,1,&CrExpTXT));
				strcat(RowHints[i+2],itoa(str->Mod,MI_Buf,10));
				strcat(RowHints[i+2],ITxt(227,1,&CrExpTXT));
				strcat(RowHints[i+2],itoa(str->Lvls[Rank],MI_Buf,10));
				strcat(RowHints[i+2],ITxt(228,1,&CrExpTXT));
				if(k>MAXICONNUM) continue;
				SetIconText(TxtProperties[k],RowCaptions[i+2],RowHints[i+2]);
//        strcpy(TxtProperties[k],RowHints[i+2]);
				strcpy(HintProperties[k],RowHints[i+2]);
				_IconTxt[k]=401;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_TROLL.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_TROLL.bmp";
				k++;
				MonsterInfo.IcoPropertiesCount=k;
				continue;
			case 'c': // Casting Spell
				self=1;
				RowCaptions[i+2]=ITxt(130,1,&CrExpTXT)/*"Block [*]"*/;
				RowCaptionHints[i+2]=ITxt(131,1,&CrExpTXT)/*"Complete Block of a Phisical Attack Bonus that stack has."*/;
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MAGICS){
					strcpy(RowHints[i+2],ITxt(132,1,&CrExpTXT));
					strcat(RowHints[i+2],GetSpellName(j));
					strcat(RowHints[i+2],ITxt(321,1,&CrExpTXT));
					strcat(RowHints[i+2],itoa(str->Lvls[Rank],MI_Buf,10));
					strcat(RowHints[i+2],ITxt(101,1,&CrExpTXT));
				}
				if(k>MAXICONNUM) continue;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(132,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(132,1,&CrExpTXT));
				strcat(TxtProperties[k],GetSpellName(j));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=402;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_CAST.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_CAST.bmp";
				k++;
				break;
			case 'p': // Casting Spell before strike (Power=3)
				self=1;
				RowCaptions[i+2]=ITxt(140,1,&CrExpTXT); // Magician
				RowCaptionHints[i+2]=ITxt(141,1,&CrExpTXT); // May Cast an Expert/Mass Version of a Spell before attack
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MAGICS){
					strcpy(RowHints[i+2],ITxt(142,1,&CrExpTXT)); // May Cast (before attack) Expert/Mass Spell : 
					strcat(RowHints[i+2],GetSpellName(j));
					strcat(RowHints[i+2],ITxt(321,1,&CrExpTXT)); //, chance is 
					strcat(RowHints[i+2],itoa(str->Lvls[Rank],MI_Buf,10));
					strcat(RowHints[i+2],ITxt(101,1,&CrExpTXT)); // %
				}
				if(k>MAXICONNUM) continue;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(142,1,&CrExpTXT)); // May Cast (before attack) Expert/Mass Spell : 
				strcat(TxtProperties[k],GetSpellName(j));
				hi=i+2; hk=k;
				_IconTxt[k]=403;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_CAST1.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_CAST1.bmp";
				k++;
				break;
			case 'J': self=1; // Casting Spell before strike Power=2
			case 'j': if(self==0){ s=329; self=1; }else{ s=326; }// Power=1
				self=1;
				RowCaptions[i+2]=ITxt(s+0,1,&CrExpTXT); // Magician
				RowCaptionHints[i+2]=ITxt(s+1,1,&CrExpTXT); // May Cast an Expert/Mass Version of a Spell before attack
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MAGICS){
					strcpy(RowHints[i+2],ITxt(s+2,1,&CrExpTXT)); // May Cast (before attack) Expert/Mass Spell : 
					strcat(RowHints[i+2],GetSpellName(j));
					strcat(RowHints[i+2],ITxt(321,1,&CrExpTXT)); //, chance is 
					strcat(RowHints[i+2],itoa(str->Lvls[Rank],MI_Buf,10));
					strcat(RowHints[i+2],ITxt(101,1,&CrExpTXT)); // %
				}
				if(k>MAXICONNUM) continue;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(s+2,1,&CrExpTXT)); // May Cast (before attack) Expert/Mass Spell : 
				strcat(TxtProperties[k],GetSpellName(j));
				hi=i+2; hk=k;
				_IconTxt[k]=404;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_CAST1.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_CAST1.bmp";
				k++;
				break;
			case 'a': // Casting Spell after strike (Power=3)
				self=1;
				RowCaptions[i+2]=ITxt(146,1,&CrExpTXT);
				RowCaptionHints[i+2]=ITxt(147,1,&CrExpTXT);
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MAGICS){
					strcpy(RowHints[i+2],ITxt(148,1,&CrExpTXT));
					strcat(RowHints[i+2],GetSpellName(j));
					strcat(RowHints[i+2],ITxt(321,1,&CrExpTXT));
					strcat(RowHints[i+2],itoa(str->Lvls[Rank],MI_Buf,10));
					strcat(RowHints[i+2],ITxt(101,1,&CrExpTXT));
				}
				if(k>MAXICONNUM) continue;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(148,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(148,1,&CrExpTXT));
				strcat(TxtProperties[k],GetSpellName(j));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=405;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_CAST2.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_CAST2.bmp";
				k++;
				break;
			case 'K': self=1;// Power=2
			case 'k': if(self==0){ s=335; self=1; }else{ s=332; }// Power=1
				RowCaptions[i+2]=ITxt(s+0,1,&CrExpTXT);
				RowCaptionHints[i+2]=ITxt(s+1,1,&CrExpTXT);
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MAGICS){
					strcpy(RowHints[i+2],ITxt(s+2,1,&CrExpTXT));
					strcat(RowHints[i+2],GetSpellName(j));
					strcat(RowHints[i+2],ITxt(321,1,&CrExpTXT));
					strcat(RowHints[i+2],itoa(str->Lvls[Rank],MI_Buf,10));
					strcat(RowHints[i+2],ITxt(101,1,&CrExpTXT));
				}
				if(k>MAXICONNUM) continue;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(s+2,1,&CrExpTXT));
				strcat(TxtProperties[k],GetSpellName(j));
				hi=i+2; hk=k;
				_IconTxt[k]=406;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_CAST2.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_CAST2.bmp";
				k++;
				break;
			case 's': // Applied Spell
				RowCaptions[i+2]=ITxt(133,1,&CrExpTXT);
				RowCaptionHints[i+2]=ITxt(134,1,&CrExpTXT);
				for(j=0;j<11;j++){
					int sl=str->Lvls[j];
					if(sl>3) strncpy(&Rows[i+2][j*8],ITxt(263+sl-4,1,&CrExpTXT),8);
					else strncpy(&Rows[i+2][j*8],ITxt(135+sl,1,&CrExpTXT),8);
				}
				for(j=0;j<8*11;j++) if(Rows[i+2][j]<0x20) Rows[i+2][j]=' ';
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MAGICS){
					if(str->Lvls[Rank]==4) strcpy(RowHints[i+2],ITxt(143,1,&CrExpTXT));
					else if(str->Lvls[Rank]==5) strcpy(RowHints[i+2],ITxt(235,1,&CrExpTXT));
					else if(str->Lvls[Rank]==6) strcpy(RowHints[i+2],ITxt(255,1,&CrExpTXT));
					else{ 
						sprintf(RowHints[i+2],ITxt(139,1,&CrExpTXT),ITxt(135+str->Lvls[Rank],1,&CrExpTXT));
//            strcpy(RowHints[i+2],ITxt(139,1,&CrExpTXT));
					}
					strcat(RowHints[i+2],GetSpellName(j));
				}
				if(k>MAXICONNUM) continue;
				if(str->Lvls[Rank]==4){
					SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(143,1,&CrExpTXT));
					strcat(TxtProperties[k],GetSpellName(j));
				}else if(str->Lvls[Rank]==5){
					SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(235,1,&CrExpTXT));
					strcat(TxtProperties[k],GetSpellName(j));
				}else if(str->Lvls[Rank]==6){
					SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(255,1,&CrExpTXT));
					strcat(TxtProperties[k],GetSpellName(j));
				}else{
//            SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(139,1,&CrExpTXT));
					 sprintf(TxtProperties[k],"%s\n%s",RowCaptions[i+2],RowHints[i+2]);
				}
//        strcat(TxtProperties[k],GetSpellName(j));
				strcpy(HintProperties[k],RowHints[i+2]);
				_IconTxt[k]=407;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_BLESS.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_BLESS.bmp";
				k++;
				MonsterInfo.IcoPropertiesCount=k;
				continue;
			case 'w':
				yes=1;
				l=0; m=408;
				switch(str->Mod){
					case '6': l=1; j=0;  m=418; break;
					case '1': l=2; j=0;  m=419; break;
					case '7': l=1; j=1;  m=420; break;
					case '2': l=2; j=1;  m=421; break;
					case '8': l=1; j=2;  m=422; break;
					case '3': l=2; j=2;  m=423; break;
					case '9': l=1; j=3;  m=424; break;
					case '4': l=2; j=3;  m=425; break;
					case ':': l=1; j=4;  m=426; break;
					case '5': l=2; j=4;  m=427; break;
					case 'C':      j=5;  m=428; break;
					case 'R':      j=6;  m=429; break;
					case 'a': l=1; j=7;  m=430; break;
					case 'A': l=2; j=25; m=431; break;
					case 'f': l=1; j=8;  m=432; break;
					case 'F': l=2; j=26; m=433; break;
					case 'w': l=1; j=9;  m=434; break;
					case 'W': l=2; j=27; m=435; break;
					case 'e': l=1; j=10; m=436; break;
					case 'E': l=2; j=28; m=437; break;
					case 'L':      j=11; m=438; break;

					case '0': j=12; m=439; break;
					case 'M': j=13; m=440; break;
					case 'I': j=14; m=441; break;
					case 'P': j=15; m=442; break;
					case 'N': j=16; m=443; break;
					case 'B': j=17; m=444; break;
					case 'K': j=18; m=445; break;
					case 'H': j=19; m=446; break;
					case 'S': j=20; m=447; break;
					case 'O': j=21; m=448; break;
					case 'D': j=22; m=449; break;
					case 'p': j=23; m=450; break;
					case 'm': j=24; m=451; break;
					default: continue;
				}
				if(j>=25 && j<=28) s=(j-25+7)*3+152;
				else if(j>20) s=(j-21)*3+266;
				else     s=j*3+152;
				RowCaptions[i+2]=ITxt(s,1,&CrExpTXT)/*"Block [*]"*/;
				RowCaptionHints[i+2]=ITxt(s+1,1,&CrExpTXT)/*"Complete Block of a Phisical Attack Bonus that stack has."*/;
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(s+2,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(s+2,1,&CrExpTXT));
				if(l==1){
					strcat(TxtProperties[k],ITxt(242,1,&CrExpTXT));
				}else if(l==2){
					strcat(TxtProperties[k],ITxt(320,1,&CrExpTXT));
				}
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=m;
				_IconPic[k]=IcoProperties[k]=MagicImmune[j][0];
				if(str->Lvls[Rank]==0) IcoProperties[k]=MagicImmune[j][1];
				k++;
				break;
			case 'b': // reduce defence
				RowCaptions[i+2]=ITxt(215,1,&CrExpTXT);
				RowCaptionHints[i+2]=ITxt(216,1,&CrExpTXT);
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(217,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(217,1,&CrExpTXT));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=409;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_RDEF.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_RDEF.bmp";
				k++;
				break;
			case 'E': // Death Stare
				RowCaptions[i+2]=ITxt(252,1,&CrExpTXT);
				RowCaptionHints[i+2]=ITxt(253,1,&CrExpTXT);
				if(k>MAXICONNUM) break;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(254,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(254,1,&CrExpTXT));
//        strcpy(HintProperties[k],RowCaptionHints[i+2]);
				hi=i+2; hk=k;
				_IconTxt[k]=410;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_DEATH.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_DEATH.bmp";
				k++;
				break;
			case 'L': // Partial Block
				RowCaptions[i+2]=ITxt(257,1,&CrExpTXT)/*"Deflect"*/;
				RowCaptionHints[i+2]=ITxt(258,1,&CrExpTXT);
				for(j=0;j<11;j++){
					strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],'%',0),8);
				}
				for(j=0;j<8*11;j++) if(Rows[i+2][j]<0x20) Rows[i+2][j]=' ';
				if(*(Byte *)&str->Mod>=100){ // фикс вер
					strcpy(RowHints[i+2],ITxt(259,1,&CrExpTXT));
					strcat(RowHints[i+2],itoa((*(Byte *)&str->Mod)-100,MI_Buf,10));
					strcat(RowHints[i+2],ITxt(260,1,&CrExpTXT));
				}else{ // фикс процентаж
					strcpy(RowHints[i+2],ITxt(261,1,&CrExpTXT));
					strcat(RowHints[i+2],itoa((*(Byte *)&str->Mod)+1,MI_Buf,10));
					strcat(RowHints[i+2],ITxt(262,1,&CrExpTXT));
				}
				if(k>MAXICONNUM) continue;
				strcpy(HintProperties[k],RowHints[i+2]);
				if(*(Byte *)&str->Mod>=100){ // фикс вер
					SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(259,1,&CrExpTXT));
//          strcpy(TxtProperties[k],ITxt(259,1,&CrExpTXT));
					strcat(TxtProperties[k],itoa((*(Byte *)&str->Mod)-100,MI_Buf,10));
					strcat(TxtProperties[k],ITxt(260,1,&CrExpTXT));
					_IconTxt[k]=411;
					_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_BLOCK1.bmp";
					if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_BLOCK1.bmp";
				}else{ // фикс процентаж
					SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(261,1,&CrExpTXT));
//          strcpy(TxtProperties[k],ITxt(261,1,&CrExpTXT));
					strcat(TxtProperties[k],itoa((*(Byte *)&str->Mod)+1,MI_Buf,10));
					strcat(TxtProperties[k],ITxt(262,1,&CrExpTXT));
					_IconTxt[k]=412;
					_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_BLOCK2.bmp";
					if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_BLOCK2.bmp";
				}
				k++;
				MonsterInfo.IcoPropertiesCount=k;
				continue;
			case 'G': // SG Bonus
				RowCaptions[i+2]=ITxt(302,1,&CrExpTXT);
				RowCaptionHints[i+2]=ITxt(303,1,&CrExpTXT);
				if(str->Mod=='='){
					for(j=0;j<11;j++){ strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],'=',2),8); }
				}else{
					for(j=0;j<11;j++){ strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],'%',0),8); }
				}
				for(j=0;j<8*11;j++) if(Rows[i+2][j]<0x20) Rows[i+2][j]=' ';
				if(str->Mod=='='){
					strcpy(RowHints[i+2],ITxt(304,1,&CrExpTXT));
				}else{
					strcpy(RowHints[i+2],ITxt(305,1,&CrExpTXT));
				}
				if(k>MAXICONNUM) continue;
				strcpy(HintProperties[k],RowHints[i+2]);
				if(str->Mod=='='){
					SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(306,1,&CrExpTXT));
//          strcpy(TxtProperties[k],ITxt(306,1,&CrExpTXT));
					_IconTxt[k]=413;
					_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_SGTYPE.bmp";
					if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_SGTYPE.bmp";
				}else{
					SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(307,1,&CrExpTXT));
//          strcpy(TxtProperties[k],ITxt(307,1,&CrExpTXT));
					_IconTxt[k]=414;
					_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_SGNUM.bmp";
					if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_SGNUM.bmp";
				}
				k++;
				MonsterInfo.IcoPropertiesCount=k;
				continue;
			case 'U': // Summon every 4th round
				RowCaptions[i+2]=ITxt(308,1,&CrExpTXT);
				RowCaptionHints[i+2]=ITxt(309,1,&CrExpTXT);
				for(j=0;j<11;j++){ strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],'%',0),8); }
				for(j=0;j<8*11;j++) if(Rows[i+2][j]<0x20) Rows[i+2][j]=' ';
				strcpy(RowHints[i+2],ITxt(310,1,&CrExpTXT));
				if(k>MAXICONNUM) continue;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(311,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(311,1,&CrExpTXT));
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MONNUM) strcat(TxtProperties[k],MonTable[j].NameP);
				strcpy(HintProperties[k],RowHints[i+2]);
				_IconTxt[k]=415;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_SUMMON.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_SUMMON.bmp";
				k++;
				MonsterInfo.IcoPropertiesCount=k;
				continue;
			case 'u': // Summon once
				RowCaptions[i+2]=ITxt(312,1,&CrExpTXT);
				RowCaptionHints[i+2]=ITxt(313,1,&CrExpTXT);
				for(j=0;j<11;j++){ strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],'%',0),8); }
				for(j=0;j<8*11;j++) if(Rows[i+2][j]<0x20) Rows[i+2][j]=' ';
				strcpy(RowHints[i+2],ITxt(314,1,&CrExpTXT));
				if(k>MAXICONNUM) continue;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(315,1,&CrExpTXT));
//        strcpy(TxtProperties[k],ITxt(315,1,&CrExpTXT));
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MONNUM) strcat(TxtProperties[k],MonTable[j].NameP);
				strcpy(HintProperties[k],RowHints[i+2]);
				_IconTxt[k]=416;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_SUMMON1.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_SUMMON1.bmp";
				k++;
				MonsterInfo.IcoPropertiesCount=k;
				continue;
			case 'l': // Summon clonned stack every round
				RowCaptions[i+2]=ITxt(316,1,&CrExpTXT);
				RowCaptionHints[i+2]=ITxt(317,1,&CrExpTXT);
				for(j=0;j<11;j++){ strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],'%',0),8); }
				for(j=0;j<8*11;j++) if(Rows[i+2][j]<0x20) Rows[i+2][j]=' ';
				strcpy(RowHints[i+2],ITxt(318,1,&CrExpTXT));
				if(k>MAXICONNUM) continue;
				SetIconText(TxtProperties[k],RowCaptions[i+2],ITxt(319,1,&CrExpTXT));
				j=*(Byte *)&str->Mod;
				if(j>=0 && j<MONNUM) strcat(TxtProperties[k],MonTable[j].NameP);
				strcpy(HintProperties[k],RowHints[i+2]);
				_IconTxt[k]=417;
				_IconPic[k]=IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\E_SUMMON2.bmp";
				if(str->Lvls[Rank]==0) IcoProperties[k]=".\\DATA\\ZVS\\LIB1.RES\\N_SUMMON2.bmp";
				k++;
				MonsterInfo.IcoPropertiesCount=k;
				continue;

			default: RowCaptions[i+2]=ITxt(91,1,&CrExpTXT)/*"??Undef??"*/; break;
		}
		for(j=0;j<11;j++){
			if(self){
				strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],'%',0),8);
			}else{
				if(yes){
					strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],str->Mod,2),8);
				}else{
					strncpy(&Rows[i+2][j*8],Bon2Str(str->Lvls[j],str->Mod,0),8);
				}
			}
		}
		for(j=0;j<8*11;j++) if(Rows[i+2][j]<0x20) Rows[i+2][j]=' ';
		MonsterInfo.IcoPropertiesCount=k;
		if(self){
			if(hi!=0) strcpy(HintProperties[hk],RowHints[hi]);
			continue;
		}
		strcpy(RowHints[i+2],ITxt(92,1,&CrExpTXT)/*"Current Bonus the Stack has : "*/);
		strcat(RowHints[i+2],Bon2Str(str->Lvls[Rank],str->Mod,yes));
		strcat(RowHints[i+2]," [ ");
		strcat(RowHints[i+2],RowCaptions[i+2]);
		strcat(RowHints[i+2]," ] ");
		if(hi!=0) strcpy(HintProperties[hk],RowHints[hi]);
	}
	do{
		if(Cr==0) break;
		if(Cr->HasArt()==0) break;
		if(k>MAXICONNUM) break;
		if(Changing) i=452; else i=453;
		SetIconText(TxtProperties[k],ITxt(i,1,&CrExpTXT),ITxt(Cr->GetSubArt()+340,1,&CrExpTXT));
		strcpy(HintProperties[k],ITxt(454,1,&CrExpTXT));
		IcoProperties[k]=ArtIcon;
		k++;
		MonsterInfo.IcoPropertiesCount=k;
	}while(0);
	MonsterInfo.TxtProperties=TxtProperties;
	MonsterInfo.HintProperties=HintProperties;
	MonsterInfo.IcoProperties=IcoProperties;
	MonsterInfo.Rows=ARows;
	MonsterInfo.RowHints=RowHints;
	MonsterInfo.RowCaptionHints=RowCaptionHints;
	MonsterInfo.RowCaptions=RowCaptions;
//  if(Rank>0) MonsterInfo.CurPropColLeft=Rank*8-1;
//  else       MonsterInfo.CurPropColLeft=Rank*8;
	MonsterInfo.CurPropColLeft=Rank*8;
	MonsterInfo.CurPropColRight=(Rank+1)*8;
	MonsterInfo.SpecButtonPush=SpecButtonPush;
	MonsterInfo.OnControlClick=OnControlClick;
	MonsterInfo.PictureHint=ITxt(455,1,&CrExpTXT);    // хинт к изображению твари
	RETURNV
}

static int SI_UpgrButton,SI_Trig,SI_MayChooseArtOpt,SI_ArtButton;
static CrExpo *SI_Cr;
void CrExpBon::ShowInfo(int Mon,int Num,int Expo,CrExpo *Cr)
{
//TestDlg(); return;
	STARTNA(__LINE__, 0)
	if(Mon<0 || Mon>=MONNUM) RETURNV
	if(Mon>=174 && Mon<=191) RETURNV // Commander
	SI_Cr=Cr;
	SI_MayChooseArtOpt=0;
	int HeroOwner=-1;
	if(Cr!=0){
		if(Cr->Fl.Type==CE_HERO){
			HeroOwner=Cr->Dt.Hero.Ind;
			if(HeroOwner<0 || HeroOwner>=HERNUM) HeroOwner=-1;
			_Hero_ *hp=GetHeroStr(HeroOwner);
			for(int i=0;i<64;i++){
				if(hp->OArt[i][0]==-1) goto _Ok;
			}
			HeroOwner=-1; // no room in a backpack
_Ok:;
		}
	}
	if(Upgraded(Mon)==-1){ // no upgrade
		MonsterInfo.ShowSpecButton=0;
		MonsterInfo.SpecButtonHint="";
	}else{
		MonsterInfo.ShowSpecButton=1;
		MonsterInfo.SpecButtonHint=ITxt(106,1,&CrExpTXT);//"Check Upgraded/Nonupgraded Creature Bonuses";
	}
	SI_Trig=1; SI_UpgrButton=1;
	do{
		if(SI_UpgrButton) SI_Trig=!SI_Trig;
		SI_UpgrButton=SI_ArtButton=0;
		if(SI_Trig==0){ 
			CrExpBon::MakeCur(-1,Mon);
			SI_MayChooseArtOpt=1;
			if(HeroOwner!=-1) PrepareInfo(Mon,Num,Expo,Cr,1); // can pass back an art
			else  PrepareInfo(Mon,Num,Expo,Cr,0);
		}else{ 
			CrExpBon::MakeCur(-1,Upgraded(Mon));
			SI_MayChooseArtOpt=0;
			PrepareInfo(Upgraded(Mon),Num,int(Expo*CrExpMod::UpgrMul(Mon)),Cr,0);
		}
		ShowUpgradeDlg(&MonsterInfo);
	}while(SI_UpgrButton!=0 || SI_ArtButton!=0);
	do{
		if(MonsterInfo.ArtOutput==0) break;
		if(HeroOwner==-1) break;
		if(Cr==0) break;
		if(Cr->HasArt()==0) break;
		_Hero_ *hp=GetHeroStr(HeroOwner);
		for(int i=0;i<64;i++){
			if(hp->OArt[i][0]==-1){
				hp->OArt[i][0]=Cr->GetArt();
				hp->OArt[i][1]=-1;
				hp->OANum=(Byte)(hp->OANum+1);
				Cr->TakeArt();
				break;
			}
		}
	}while(0);
	RETURNV
}
void CrExpBon::ShowInfo(Byte *BMon)
{
	STARTNA(__LINE__, 0)
	int Expo;
	SI_MayChooseArtOpt=0;
	int Mon=*(int *)&BMon[0x34];
	int Num=*(int *)&BMon[0x4C];
	if(Mon<0 || Mon>=MONNUM) RETURNV
	if(Mon>=174 && Mon<=191){ // Commander
		if(ShowBFNPCInfo(Mon,BMon)) RETURNV
	}
	int Ind=MonIndex(BMon);
	CrExpBon::MakeCur(Ind,Mon);
//  Expo=CrExpoSet::Cur.Expo;
	CrExpo *cr=CrExpoSet::GetBF(Ind);
	Expo=cr->Expo;
		MonsterInfo.ShowSpecButton=0;
		MonsterInfo.SpecButtonHint="";
	SI_Trig=0;
	PrepareInfo(Mon,Num,Expo,cr,0);
	ShowUpgradeDlg(&MonsterInfo);
/*
	do{
		SI_UpgrButton=0;
		if(SI_Trig==0) PrepareInfo(-Ind-1,Num,Expo);
		else PrepareInfo(Upgraded(Mon),Num,Expo*CrExpMod::UpgrMul(Mon));
		ShowUpgradeDlg(&MonsterInfo);
		SI_Trig=!SI_Trig;
	}while(SI_UpgrButton!=0);
*/
	RETURNV
}
static int SpecButtonPush(void)
{
	STARTNA(__LINE__, 0)
/*
	if(SI_Mon<0 || SI_Mon>=MONNUM) return 1;
	if(SI_Trig==0){ // show upgraded
		CrExpBon::PrepareInfo(Upgraded(SI_Mon),SI_Num,SI_Expo*CrExpMod::UprgMul(SI_Mon));
		SI_Trig=1;
	}else{ // show this
		CrExpBon::PrepareInfo(SI_Mon,SI_Num,SI_Expo);
		SI_Trig=0;
	}
	return 0;
*/
	SI_UpgrButton=1;
	RETURN(1)
}

static _CheckBoxes RBDlg;
static char OCC_Text[2048],OCC_Pic[128];
int OnControlClick(int ContorIndex,int MouseButton,int /*ShiftState*/)
{
	if(MouseButton!=1) return 0;
	//STARTNA(__LINE__, 0)
	START("Dlg::OnControlClick")
	int val,val2;
	int rank=CrExpMod::GetRank(CURMON,RI_Expo);
	switch(ContorIndex){
		case 0 : // 0 - портрет (c AVI не работает)
		case 1 : // 1 - таблица (содержимое, заголовки столбцов, заголовки строк)
		case 8 : // 8 - поле информации о твари
			if(CrExpMod::GetRankExp(CURMON,rank)==0) val=999999;
			else val=RI_Expo*RI_Num/CrExpMod::GetRankExp(CURMON,rank)-RI_Num;
			if(CrExpMod::GetRankExp(CURMON,10)==0) val2=999999;
			else val2=(CrExpMod::Lvl11(CURMON)+CrExpMod::GetRankExp(CURMON,10))*RI_Num/ CrExpMod::GetRankExp(CURMON,10)-RI_Num;
			sprintf(OCC_Text,ITxt(325,1,&CrExpTXT)
				,MonTable[RI_Mon].NameS
				,ITxt(rank+1,1,&CrExpTXT),rank
				,RI_Expo
				,CrExpMod::GetRankExp(CURMON,rank+1)-RI_Expo
				,CrExpMod::Cap(CURMON),CrExpMod::Limit(CURMON)*CrExpMod::Cap(CURMON)/100
				,RI_Num
				,val
				,CrExpMod::ExpMul(CURMON)
				,CrExpMod::UpgrMul(CURMON)
				,CrExpMod::Lvl11(CURMON)
				,val2
				);
			PicDlg_Int(OCC_Text,"[LODDEF]H3Sprite.lod;PSkill.def;5;-1;-1");
//      SendMessage(GetForegroundWindow(),WM_ACTIVATE,WA_INACTIVE,(long)GetForegroundWindow());
//      {HWND hwnd=GetForegroundWindow();
//MessageBox(hwnd,"Hi!","Title",MB_YESNOCANCEL|MB_ICONINFORMATION|MB_APPLMODAL);
//      RedrawWindow(hwnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
//      SetForegroundWindow(GetForegroundWindow());
//}
			break;
		case 2 : // 2 - дополнительные свойства 1 (иконка и текст)
		case 3 : // 3 - дополнительные свойства 2 (иконка и текст)
		case 4 : // 4 - дополнительные свойства 3 (иконка и текст)
		case 5 : // 5 - дополнительные свойства 4 (иконка и текст)
		case 6 : // 6 - дополнительные свойства 5 (иконка и текст)
		case 7 : // 7 - дополнительные свойства 6 (иконка и текст)
		case 12: // 12- дополнительные свойства 7 (иконка и текст)
		case 13: // 13- дополнительные свойства 8 (иконка и текст)
			if(ContorIndex>7) val=ContorIndex-6; else val=ContorIndex-2;
			if(_IconTxt[val]!=-1){
				sprintf(OCC_Text,"[<POSITION=C>]%s\n\n%s",TxtProperties[val],ITxt(_IconTxt[val],1,&CrExpTXT));
			}else{
				sprintf(OCC_Text,"[<POSITION=C>]%s",TxtProperties[val]);
			}
			PicDlg_Int(OCC_Text,_IconPic[val]);
			break;
		case 9 : // 9 - заголовок диалога
			break;
		case 10: // 10 - кнопка №2 (справа от кнопки "Закрыть") [левый клик не работает]
			CommonDialog(ITxt(338,1,&CrExpTXT));
			break;
		case 11: // 11 - иконка атрефакта [левый клик не работает]
			if(SI_Cr==0 || SI_Cr->HasArt()==0){
				sprintf(OCC_Pic,"[LODDEF]H3Sprite.lod;Artifact.def;%i;0;0",EXPARTFIRST+1);
				PicDlg_Int(ITxt(363,1,&CrExpTXT),OCC_Pic);
				break;
			}
			if(SI_MayChooseArtOpt && SI_Cr!=0){
//        if((ShiftState&0x00000004)==0){ // Ctrl not pressed
//          sprintf(OCC_Text,ITxt(361,1,&CrExpTXT),ITxt(SI_Cr->GetSubArt()+340,1,&CrExpTXT));
//          sprintf(OCC_Pic,"[LODDEF]H3Sprite.lod;Artifact.def;%i;0;0",SI_Cr->GetArt()+1);
//          PicDlg_Int(OCC_Text,OCC_Pic);
//          break;
//        }

				char *Text[16];
				Text[ 0]=ITxt(340,1,&CrExpTXT); Text[ 1]=ITxt(341,1,&CrExpTXT); Text[ 2]=ITxt(342,1,&CrExpTXT); Text[ 3]=ITxt(343,1,&CrExpTXT);
				Text[ 4]=ITxt(344,1,&CrExpTXT); Text[ 5]=ITxt(345,1,&CrExpTXT); Text[ 6]=ITxt(346,1,&CrExpTXT); Text[ 7]=ITxt(347,1,&CrExpTXT);
				Text[ 8]=ITxt(348,1,&CrExpTXT); Text[ 9]=ITxt(349,1,&CrExpTXT); Text[10]=ITxt(350,1,&CrExpTXT); Text[11]=ITxt(351,1,&CrExpTXT);
				Text[12]=ITxt(352,1,&CrExpTXT); Text[13]=ITxt(353,1,&CrExpTXT); Text[14]=ITxt(354,1,&CrExpTXT); Text[15]=ITxt(355,1,&CrExpTXT);
				int State[16]; for(int i=0;i<16;i++) State[i]=0;
				RBDlg.Caption=ITxt(339,1,&CrExpTXT);
				RBDlg.Number=9;
				RBDlg.OnlyOne=1;
				RBDlg.Text=Text;
				State[SI_Cr->GetSubArt()]=1;
				RBDlg.State=State;
				MultiCheckReqInt(&RBDlg);
				for(int i=0;i<16;i++){ if(State[i]==1) SI_Cr->SetSubArt(i); }
				//Called from zvslib dialog, so game-based dialog is wrong here:
				//if (LuaCallStart("RadioDialog")) break;
				//lua_pushnil(Lua);
				//lua_pushnil(Lua);
				//lua_pushnil(Lua);
				//lua_pushinteger(Lua, SI_Cr->GetSubArt());
				//for (int i = 399; i <= 348; i++)
				//	lua_pushstring(Lua, ITxt(i,1,&CrExpTXT));
				//SI_Cr->SetSubArt(LuaCallEnd());
				SI_ArtButton=1;
				RETURN(1)
			}else{
				CommonDialog(ITxt(356,1,&CrExpTXT));
			}
			break;   
	}
	RETURN(0)
}

int CrExpoSet::DropArt2Stack(_MouseStr_ *ms,_Hero_ *hp,int DropArt)
{
	STARTNA(__LINE__, 0)
	CrExpo *Cr=0;
	int Type,Stack;
	CRLOC Crloc;
	Stack=ms->Item-0x44;
	if(Stack<0 || Stack>6){ TError("DropArt2Stack - incorrect stack index."); RETURN(0) }
	switch(ms->SType){
		case 0x0C: // левый нажали
			if(/*WoG==0 ||*/ PL_CrExpEnable==0){
				Message(ITxt(357,1,&CrExpTXT),1); // cannot drop art if experience disabled
				RETURN(0)
			}
			// проверяем можно ли добавить артифакт и, если да, то добавляем
			// incorrect art
			if(DropArt<(EXPARTFIRST) || DropArt>(EXPARTLAST)){
				Message(ITxt(358,1,&CrExpTXT),1); // cannot drop this art
				RETURN(0)
			}
			// нет стэка
			if(hp->Ct[Stack]==-1 || hp->Cn[Stack]==0){ 
				RETURN(0)
			}
			if(CrExpoSet::FindType((_MonArr_ *)&hp->Ct[0],Stack,&Type,&Crloc)){
					Cr=CrExpoSet::Find(Type,Crloc);
					if(Cr==0){
						Cr=CrExpoSet::FindEmpty();
						if(Cr==0){ 
							TError("DropArt2Stack - no room in experience story."); 
							RETURN(0) 
						}
						Cr->Clear();
						Cr->SetN(Type,Crloc,hp->Ct[Stack],hp->Cn[Stack],0);
					}
					if(Cr->HasArt()){
						Message(ITxt(359,1,&CrExpTXT),1); // has art already
						RETURN(0)
					}
					if(Cr->SetArt(DropArt,0)==0){
						Message(ITxt(360,1,&CrExpTXT),1); // cannot add art (internal reason)
						RETURN(0)
					}
					// добавили
//          RedrawHeroScreen(hp);
					RETURN(1)
/*
		COCall(0,hp);
						npc->ToText(0);
						if(ShowNPC(&DlgNPC)==-1){
							npc->Used=0;
						}
						for(int j=0;j<6;j++){
							if(DlgNPC.ArtOutput[j]==1){ // передаем артифакт обратно герою
								if(hp->OANum>=64){
									Message(ITxt(214,1,&NPCTexts),1);
									break;
								}
								int Type=npc->Arts[j][ARTNUMINDEX];
								for(int i=0;i<64;i++){
									if(hp->OArt[i][0]==-1){
										hp->OArt[i][0]=Type;
										hp->OArt[i][1]=-1;
										hp->OANum=(Byte)(hp->OANum+1);
										break;
									}
								}
								for(int k=0;k<8;k++) npc->Arts[j][k]=0;
								moved=1;
							}
						}
						COCall(1,hp);
					}
*/
//          if(moved) RedrawHeroScreen(hp);
//          if(DropArt!=-1 && DropArt!=0xFF) RETURN(1)
					break;
			}
			break;
		case 0x0D: // левый отпустили
			break;
		case 0x0E: // правый нажали
/*
			switch(npc->Used){
				case -1: // запрещен
					Message(ITxt(182,1,&NPCTexts),4);
					break;
				case 0: // разрешен, но нет
					Message(ITxt(181,1,&NPCTexts),4);
					break;
				case 1: // есть
					Message(ITxt(180,1,&NPCTexts),4);
					break;
			}
*/
			break;
	}
	RETURN(0)
}

///////////////////
static void FixAllDblHeroes(void)
{
	STARTNA(__LINE__, 0)
	int i,j,k,l,o,found;
	_Hero_ *hp;
	_PlayerSetup_ *ps;
	for(i=0;i<HERNUM;i++){
CheckAgain:;
		hp=GetHeroStr(i);
		o=hp->Owner;
		if(o==-1){ // ничейный - не должен быть ни у кого
			for(j=0;j<7;j++){
				if(DoesExist(j)==0) continue;
				ps=GetPlayerSetup(j);
				for(k=0;k<ps->HasHeroes;k++){
					if(ps->Heroes[k]!=i) continue;
					// нашли - НЕ нормально, надо убрать
					for(l=k+1;l<ps->HasHeroes;l++){
						ps->Heroes[l-1]=ps->Heroes[l];
					}
					ps->Heroes[ps->HasHeroes]=-1;
					ps->HasHeroes-=(char)1;
					HideHero(hp);
					k--; // повторно проверим этот индекс, т.к. сдвинули
				}
			}
			_CastleSetup_ *cst;
			cst=GetCastleBase();
			if(cst==0) continue;
			int cn=GetCastleNum(); //
			for(j=0;j<cn;j++,cst++){
//        if(cst->Owner!=o)
				if(cst->IHero==i)  cst->IHero=-1;
				if(cst->VHero==i){ cst->VHero=-1; HideHero(hp); }
			}
		}else{
			for(j=0;j<7;j++){
				if(DoesExist(j)==0) continue;
				ps=GetPlayerSetup(j);
				if(ps->THeroL==i){ // в таверне наш герой - не нормально
					ps->THeroL=-1;
				}
				if(ps->THeroR==i){ // в таверне наш герой - не нормально
					ps->THeroR=-1;
				}
				if(ps->Owner==o){ // наш хозяин
					for(k=found=0;k<ps->HasHeroes;k++){
						if(ps->Heroes[k]==i){ // нашли - нормально
							if(found){ // нашли повторно - выкинем
								for(l=k+1;l<ps->HasHeroes;l++){
									ps->Heroes[l-1]=ps->Heroes[l];
								}
								ps->Heroes[ps->HasHeroes]=-1;
								ps->HasHeroes-=(char)1;
								HideHero(hp);
								k--; // повторно проверим этот индекс, т.к. сдвинули
							}else{
								found=1;
							}
						}
					}
					if(found) goto ToTowns;
					// не нашли в списке - смотрим в городах
					do{
						_CastleSetup_ *cst;
						cst=GetCastleBase();
						if(cst==0) break; // надо добавить, т.к. в городах нет
						int cn=GetCastleNum(); //
						for(j=0;j<cn;j++,cst++){
							if(cst->Owner==o){
								if(cst->IHero==i) goto ToTowns; // нашли - все нормально
							}
						}
					}while(0);
					// не нашли - надо добавить
					if(ps->HasHeroes<8){
						ps->Heroes[ps->HasHeroes]=i;
						ps->HasHeroes+=(char)1;
					}else{
						hp->Owner=-1;
						HideHero(hp);
						goto CheckAgain;
					}
					// не нашли - ненормально
				}else{ // НЕ наш хозяин
					for(k=0;k<ps->HasHeroes;k++){
						if(ps->Heroes[k]!=i) continue;
						// нашли - не нормально
						for(l=k+1;l<ps->HasHeroes;l++){
							ps->Heroes[l-1]=ps->Heroes[l];
						}
						ps->Heroes[ps->HasHeroes]=-1;
						ps->HasHeroes-=(char)1;
					}
					// не нашли - нормально
				}
			}
ToTowns:
			_CastleSetup_ *cst;
			cst=GetCastleBase();
			if(cst==0) continue;
			int cn=GetCastleNum(); //
			for(j=0;j<cn;j++,cst++){
				if(cst->Owner!=o){
					if(GetTeam(cst->Owner)==GetTeam(o)) continue; // одна команда - можно
					if(cst->IHero==i){  cst->IHero=-1; }
					if(cst->VHero==i){ cst->VHero=-1; HideHero(hp); }
				}else{
					if((cst->IHero==i) || (cst->VHero==i)){
						while(1){
							if(cst->x!=hp->x)break;
							if(cst->y!=hp->y)break;
							if(cst->l!=hp->l)break;
							if(cst->IHero==cst->VHero){ cst->VHero=-1; HideHero(hp); }
							goto ThisTown;
						}
						if(cst->IHero==i) cst->IHero=-1;
						if(cst->VHero==i){ cst->VHero=-1; HideHero(hp); }
ThisTown:;
					}
				}
			}
		}
//NextHero2:;
	}
	// clean up all empty heroes indexes
	for(j=0;j<7;j++){
		if(DoesExist(j)==0) continue;
		ps=GetPlayerSetup(j);
		for(k=ps->HasHeroes;k<8;k++) ps->Heroes[k]=-1;
	}
	// проверим списки городов
	do{
		_CastleSetup_ *cst;
		_MapItem_ *mp;
		cst=GetCastleBase();
		if(cst==0) break;;
		int cn=GetCastleNum(); //
		for(i=0;i<cn;i++,cst++){ // переберем все города
//      o=cst->Owner;
			mp=GetMapItem(cst->x,cst->y,cst->l);
			if(mp->OType==0x22){  // стоит герой на входе
				int hn=mp->SetUp;
				hp=GetHeroStr(hn);
				if((hp->x!=cst->x)||(hp->y!=cst->y)||(hp->l!=cst->l)){ // стоит не здесь - double hero
					mp->OType=0x62; // town
					mp->SetUp=i; // номер города
					mp->Mirror|=0x1000; // есть желтая клетка
					cst->VHero=-1; // нет героя визитера  
				}
//          if(cst->VHero!=hn){ // не тот герой на входе
//          }
			}

		}
	}while(0);
/*
	do{
		_CastleSetup_ *cst;
		cst=GetCastleBase();
		if(cst==0) break;;
		int cn=GetCastleNum(); //
		for(i=0;i<cn;i++,cst++){ // переберем все города
			o=cst->Owner;
			for(j=0;j<7;j++){ // переберем игроков
				if(DoesExist(j)==0) continue; // нет игрока
				ps=GetPlayerSetup(j);
				if(o==j){ // хозяин города
					for(k=found=0;k<ps->HasTowns;k++){
						if(ps->Towns[k]==i){ // нашли - нормально
							if(found){ // нашли повторно - выкинем
								for(l=k+1;l<ps->HasTowns;l++){
									ps->Towns[l-1]=ps->Towns[l];
								}
								ps->Towns[ps->HasTowns]=-1;
								ps->HasTowns-=(char)1;
								k--; // повторно проверим этот индекс, т.к. сдвинули
							}else{
								found=1;
							}
						}
					}
					if(found==0){
						// не нашли - надо добавить
						if(ps->HasTowns<48){
							ps->Towns[ps->HasTowns]=i;
							ps->HasTowns+=(char)1;
						}else{
							// такого не может быть - городов больше 48! 
						}
					}
				}else{  // НЕ хозяин города
					for(k=0;k<ps->HasTowns;k++){
						if(ps->Towns[k]!=i) continue;
						// нашли - НЕ нормально, надо убрать
						for(l=k+1;l<ps->HasTowns;l++){
							ps->Towns[l-1]=ps->Towns[l];
						}
						ps->Towns[ps->HasTowns]=-1;
						ps->HasTowns-=(char)1;
						k--; // повторно проверим этот индекс, т.к. сдвинули
					}
				}
			}
		}
	}while(0);
*/
	RETURNV
}

static void StoreAllHeroExp(Dword *Place)
{
	STARTNA(__LINE__, 0)
	for(int i=0;i<HERNUM;i++){
		Place[i]=GetHeroStr(i)->Exp;
	}
	RETURNV
}
/*
static void StoreAllHeroArts(Byte *Place)
{
	START("StoreAllHeroArts")
	_Hero_ *hp;
	int owner;
	for(int i=0;i<HERNUM;i++){
		hp=GetHeroStr(i)->Exp;
		owner=hp->Owner;
		if(owner==-1 || IsAI(owner)){
			
		}
		if(IsAI())
	}
	RETURNV
}
*/
int   CrExpoSet::LastUser=-1;
int   CrExpoSet::PlayerMult=100;
int   CrExpoSet::AIMult[5];
int   CrExpoSet::AIBase[5];
int   CrExpoSet::AITMult[5];
Dword CrExpoSet::HerExpo[256]; // опыт всех героев
//Byte  CrExpoSet::HerArts[256]; 

void CrExpoSet::DaylyAIExperience(int User)
{
	STARTNA(__LINE__, 0)
	int i,j, Mult,Base,Diff,AllDiff,TownMult,AllW;
	_Hero_ *hp;
	CrExpo *cr;
	FixAllDblHeroes();
	if(User==-1){
		LastUser=-1;
		StoreAllHeroExp(HerExpo);
//    StoreAllHeroArts(HerArts);
		RETURNV;
	}
	//if(!WoG) RETURNV
	if(!PL_CrExpEnable) RETURNV
	if(PL_ExpGainDis) RETURNV
	if(LastUser!=-1 && IsAI(LastUser)){
		AllDiff=0;
		for(i=0;i<HERNUM;i++){
			hp=GetHeroStr(i);
			if(hp->Owner!=LastUser) continue; // not this
			int gml=GameLevel();
			Mult=CrExpoSet::AIMult[gml]; Base=CrExpoSet::AIBase[gml]; TownMult=CrExpoSet::AITMult[gml];
			Diff=int((hp->Exp-HerExpo[i]+Base)*Mult/100.0f);
			if(Diff==0) continue;
			AllDiff+=Diff;
			AllW=0;
			for(j=0;j<7;j++){ AllW+=hp->Cn[j]; }
			if(AllW==0) continue;
			for(j=0;j<7;j++){
				cr=CrExpoSet::Find(CE_HERO,MAKEHS(i,j));
				if(cr==0){
					SetN(CE_HERO,MAKEHS(i,j),hp->Ct[j],hp->Cn[j],0);
					cr=Find(CE_HERO,MAKEHS(i,j));
				}
				if(cr==0) continue; // no more rooms
				cr->RecalcExp2RealNum(hp->Cn[j],hp->Ct[j]);
				cr->Expo+=CrExpMod::CapIt(cr,Diff*HEREXPMUL/*/AllW*/);
				cr->Check4Max(); // check for max experience
			}
		}
		_CastleSetup_ *cst;
		cst=GetCastleBase();
		if(cst!=0){
			int cn=GetCastleNum(); //
			for(i=0;i<cn;i++,cst++){
				if(cst->Owner!=LastUser) continue;
				AllW=0;  for(j=0;j<7;j++){ AllW+=cst->GuardsN[j]; }
				if(AllW==0) continue; // no creatures
				for(j=0;j<7;j++){
					cr=CrExpoSet::Find(CE_TOWN,MAKETS(cst->x,cst->y,cst->l,j));
					if(cr==0){
						SetN(CE_TOWN,MAKETS(cst->x,cst->y,cst->l,j),cst->GuardsT[j],cst->GuardsN[j],0);
						cr=Find(CE_TOWN,MAKETS(cst->x,cst->y,cst->l,j));
					}
					if(cr==0) continue; // no more rooms
					cr->RecalcExp2RealNum(cst->GuardsN[j],cst->GuardsT[j]);
					cr->Expo+=CrExpMod::CapIt(cr,AllDiff*HEREXPMUL*TownMult/*/AllW*//100);
					cr->Check4Max(); // check for max experience
				}
			}
		}
	}
	LastUser=User;
	StoreAllHeroExp(HerExpo);
	RETURNV
}

int CrExpBon::ApplyBFBonus(int TypeS,int TypeD, int Mode)
{
	STARTNA(__LINE__, 0)
	int i,tpS,tpD,tp,side0,side;
	Byte *Mon;
	if(Mode==0){ // just copy
		if(TypeD<0) CrExpBon::SetB2AIndex(-TypeD-1,-1);
		CrExpBon::Copy(CrExpBon::Get(TypeD),CrExpBon::Get(TypeS));
		CrExpMod::Copy(CrExpMod::Get(TypeD),CrExpMod::Get(TypeS));
		RETURN(0)
	}
	if(TypeS<0){ // stack at BF
		Mon=MonPos(-TypeS-1); if(Mon==0) RETURN(1)
		tpS=*(int *)&Mon[0x34];
		if(tpS<0 || tpS>= MONNUM) RETURN(1)
		Mon=MonPos(-TypeD-1); if(Mon==0) RETURN(1)
		tpD=*(int *)&Mon[0x34];
		if(tpD<0 || tpD>= MONNUM) RETURN(1)
		side0=*(int *)&Mon[0xF4]; // сторона приемного монстра
		CrExpBon::SetB2AIndex(-TypeD-1,-1);
		CrExpBon::Copy(CrExpBon::Get(TypeD),CrExpBon::Get(TypeS));
		CrExpMod::Copy(CrExpMod::Get(TypeD),CrExpMod::Get(TypeS));
		for(i=0;i<42;i++){
			if(i==20) continue; // 3.58
			if(i==41) continue; // 3.58
			Mon=MonPos(i); if(Mon==0) continue;
			tp=*(int *)&Mon[0x34];
			if(tp!=tpD) continue;
			side=*(int *)&Mon[0xF4]; // сторона
			switch(Mode){
				case 1: // copy to all friendly of this stack
					if(side!=side0) continue;
					break;
				case 2: // copy to all enemy of this stack
					if(side==side0) continue;
					break;
				case 3: // copy to all side 0
					if(side!=0) continue;
					break;
				case 4: // copy to all side 1
					if(side!=1) continue;
					break;
				case 5: // copy to all 
					break;
				default: // only this one or none
					continue;
			}
			CrExpBon::SetB2AIndex(i,-1);
			CrExpBon::Copy(CrExpBon::Get(-i-1),CrExpBon::Get(TypeS));
			CrExpMod::Copy(CrExpMod::Get(-i-1),CrExpMod::Get(TypeS));
		}
	}else{ // stat 
		if(TypeD<0){
			Mon=MonPos(-TypeD-1); if(Mon==0) RETURN(1)
			tpD=*(int *)&Mon[0x34];
		}else tpD=TypeD;
		if(tpD<0 || tpD>= MONNUM) RETURN(1)
		for(i=0;i<42;i++){
			if(i==20) continue; // 3.58
			if(i==41) continue; // 3.58
			Mon=MonPos(i); if(Mon==0) continue;
			tp=*(int *)&Mon[0x34];
			if(tp<0 || tp>= MONNUM) continue;
			if(tp!=tpD) continue;
			side=*(int *)&Mon[0xF4]; // сторона
			switch(Mode){
				case 3: // copy to all side 0
					if(side!=0) continue;
					break;
				case 4: // copy to all side 1
					if(side!=1) continue;
					break;
				case 5: // copy to all 
					break;
			}
			CrExpBon::SetB2AIndex(i,-1);
			CrExpBon::Copy(CrExpBon::Get(-i-1),CrExpBon::Get(TypeS));
			CrExpMod::Copy(CrExpMod::Get(-i-1),CrExpMod::Get(TypeS));
		}
	}
	RETURN(0)
}
/*
int CrExpBon::UpdateBonusAtBF(int Type, int Mode)
{
	int i,tp0,tp,side0,side;
	Byte *Mon;
	if(Type<0){ // stack at BF
		Mon=MonPos(-Type-1); if(Mon==0) return 1;
		tp0=*(int *)&Mon[0x34];
		if(tp0<0 || tp0>= MONNUM) return 1;
		side0=*(int *)&Mon[0xF4]; // сторона
		CrExpBon::Apply(Mon);
		CrExpBon::Apply2(Mon);
		for(i=0;i<42;i++){
			if(i==20) continue; // 3.58
			if(i==41) continue; // 3.58
			Mon=MonPos(i); if(Mon==0) continue;
			tp=*(int *)&Mon[0x34];
//      if(tp<0 || tp>= MONNUM) continue;
			if(tp!=tp0) continue;
			side=*(int *)&Mon[0xF4]; // сторона
			switch(Mode){
				case 1: // copy to all friendly of this stack
					if(side!=side0) continue;
					break;
				case 2: // copy to all enemy of this stack
					if(side==side0) continue;
					break;
				case 3: // copy to all side 0
					if(side!=0) continue;
					break;
				case 4: // copy to all side 1
					if(side!=1) continue;
					break;
				case 5: // copy to all 
					break;
				default: // only this one or none
					continue;
			}
			CrExpBon::Apply(Mon);
			CrExpBon::Apply2(Mon);
		}
	}else{ // stat 
		for(i=0;i<42;i++){
			if(i==20) continue; // 3.58
			if(i==41) continue; // 3.58
			Mon=MonPos(i); if(Mon==0) continue;
			tp=*(int *)&Mon[0x34];
			if(tp<0 || tp>= MONNUM) continue;
			if(tp!=Type) continue;
			side=*(int *)&Mon[0xF4]; // сторона
			switch(Mode){
				case 3: // copy to all side 0
					if(side!=0) continue;
					break;
				case 4: // copy to all side 1
					if(side!=1) continue;
					break;
				case 5: // copy to all 
					break;
			}
			CrExpBon::Apply(Mon);
			CrExpBon::Apply2(Mon);
		}
	}
	return 0;
}
*/
int ERM_AICrExp(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int Type,v,art,opt;
	int i,fl,Ind,Flags,Bon,Mod,lvls[11];
	float f;
	switch(sp->ParSet){
		case 1: // тип монстра
			Type=GetVarVal(&sp->Par[0]);
			if(Type<-BFNUM || Type>=MAXCREXPAB){ MError("\"!!EA\"-wrong creature type index."); RETURN(0) }
			break;
//    case 2: // установки AI 1/DifLevel
//      Type=GetVarVal(&sp->Par[1]);
//      if(Type<0 || Type>=5){ MError("\"!!EA\"-wrong creature type index."); RETURN(0) }
		default:
			MError("\"!!EA\"-unsupported syntax."); RETURN(0)
	}
	switch(Cmd){
		case 'M': // ExpMul
			f=CrExpMod::ExpMul(Type); v=(int)(f*1000.0f);
			if(Apply(&v,4,Mp,0)) break;
			if(Type<0) CrExpBon::SetB2AIndex(-Type-1,-1);
			f=v/1000.0f; CrExpMod::SExpMul(Type,f);
			if(Type<0) CrExpBon::Apply(MonPos(-Type-1));
			break;
		case 'U': // UpgrMul
			f=CrExpMod::UpgrMul(Type); v=(int)(f*1000.0f);
			if(Apply(&v,4,Mp,0)) break;
			if(Type<0) CrExpBon::SetB2AIndex(-Type-1,-1);
			f=v/1000.0f; CrExpMod::SUpgrMul(Type,f);
			if(Type<0) CrExpBon::Apply(MonPos(-Type-1));
			break;
		case 'L': // Limit
			v=CrExpMod::Limit(Type);
			if(Apply(&v,4,Mp,0)) break;
			if(Type<0) CrExpBon::SetB2AIndex(-Type-1,-1);
			CrExpMod::SLimit(Type,v);
			if(Type<0) CrExpBon::Apply(MonPos(-Type-1));
			break;
		case 'P': // Lvl11 expo value
			v=CrExpMod::Lvl11(Type);
			if(Apply(&v,4,Mp,0)) break;
			if(Type<0) CrExpBon::SetB2AIndex(-Type-1,-1);
			CrExpMod::SLvl11(Type,v);
			if(Type<0) CrExpBon::Apply(MonPos(-Type-1));
			break;
		case 'C': // Cap
			v=CrExpMod::Cap(Type);
			if(Apply(&v,4,Mp,0)) break;
			if(Type<0) CrExpBon::SetB2AIndex(-Type-1,-1);
			CrExpMod::SCap(Type,v);
			if(Type<0) CrExpBon::Apply(MonPos(-Type-1));
			break;
		case 'B': // BonusLine
			if(Num<15){ MError("\"!!EA\"-wrong number of parameters."); RETURN(0) }
//      if(Apply(&MType,4,Mp,0)) break;
			if(Apply(&Ind,4,Mp,0)) break;
			CrExpBon::GetBonLine(Type,Ind,&Flags,&Bon,&Mod,lvls);
			fl=0;
			if(Apply(&Flags,4,Mp,1)) fl=1;
			if(Apply(&Bon,4,Mp,2)) fl=1;
			if(Apply(&Mod,4,Mp,3)) fl=1;
			for(i=0;i<11;i++){
				if(Apply(&lvls[i],4,Mp,(char)(4+i))) fl=1;
			}
			if(fl) break;
			if(Type<0) CrExpBon::SetB2AIndex(-Type-1,-1);
			CrExpBon::SetBonLine(Type,Ind,Flags,Bon,Mod,lvls);
			if(Type<0){ 
				CrExpBon::Apply(MonPos(-Type-1));
				CrExpBon::ApplySpell(MonPos(-Type-1),0);
			}
//      CrExpBon::UpdateBonusAtBF(Type,0);
			break;
		case 'O':{
			int Mode=0;
			if(Apply(&v,4,Mp,0)) break; // source creature
			if(Num>1){ if(Apply(&Mode,4,Mp,1)) break; }
			if(v<-BFNUM || v>=MAXCREXPAB){ MError("\"!!EA:O\"-wrong creature type index."); RETURN(0) }
			CrExpBon::ApplyBFBonus(v,Type,Mode);
			if(Type<0){ 
				CrExpBon::Apply(MonPos(-Type-1));
				CrExpBon::ApplySpell(MonPos(-Type-1),0);
			}
//      if(Type<0) CrExpBon::SetB2AIndex(-Type-1,-1);
//      CrExpBon::Copy(CrExpBon::Get(Type),CrExpBon::Get(v));
//      CrExpMod::Copy(CrExpMod::Get(Type),CrExpMod::Get(v));
			break;}
		case 'D':
			if(Type<-BFNUM || Type>=0){ MError("\"!!EA:D\"-wrong creature type index."); RETURN(0) }
			v=CrExpBon::BFBodyAct[-Type-1];
			Apply(&v,4,Mp,0);
			break;
		case 'R':{ // add artifact R$art/$option
			CHECK_ParamsMin(2);
			if(Type<-BFNUM || Type>=0){ MError("\"!!EA:R\"-wrong creature type index."); RETURN(0) }
			CrExpo *cr=CrExpoSet::GetBF(-Type-1);
			if(cr==0){ MError("\"!!EA:R\"-internal#1."); RETURN(0) }
			if(cr->HasArt()){ art=cr->GetArt(); opt=cr->GetSubArt(); }
			else{ art=-1; opt=0; }
			fl=0;
			if(Apply(&art,4,Mp,0)) fl=1;
			if(Apply(&opt,4,Mp,1)) fl=1;
			if(fl) break;
			if(art==-1){ // no art
				cr->DelArt();
			}else{
				cr->SetArt(art,opt);
			}
			CrExpBon::Apply(MonPos(-Type-1));
			CrExpBon::ApplySpell(MonPos(-Type-1),0);
			break;}
		case 'E':{
			if(Type<-BFNUM || Type>=0){ MError("\"!!EA:E\"-wrong creature type index."); RETURN(0) }
			CHECK_ParamsMin(4);
			Type=-Type-1;
			CrExpo *cr=&CrExpoSet::BFBody[Type];
			Byte *bm,*mon; 
			int tp,nm,exp,mod=0;
			__asm{
				mov   eax,0x699420
				mov   eax,[eax]
				mov   bm,eax
			}
			mon=&bm[0x54CC+0x548*Type]; tp=*(int *)&mon[0x34]; nm=*(int *)&mon[0x4C];
			if(cr->Fl.Act==0){ cr->Clear(); }
			exp=cr->Expo;
			if(Apply(&exp,4,Mp,0)) break; // source creature
			if(Apply(&mod,4,Mp,1)) break; // source creature
			if(Apply(&tp ,4,Mp,2)) break; // source creature
			if(Apply(&nm ,4,Mp,3)) break; // source creature
			cr->Fl.MType=tp;
			cr->Num=nm;
			ApplyExpo(&cr->Expo,exp,mod,cr->Fl.MType,tp,cr->Num,nm);
			cr->Fl.Act=1;
			CrExpBon::Apply(MonPos(Type));
			CrExpBon::ApplySpell(MonPos(Type),0);
			break;}
		case 'F':
			CHECK_ParamsMin(2);
			if(Num<3){
				if(Apply(&Bon,4,Mp,0)) break;
				i=CrExpBon::FindThisBonLineInd(Type,(char)Bon,(char)-1);
				if(Apply(&i,4,Mp,1)) break;
			}else{
				if(Apply(&Bon,4,Mp,0)) break;
				if(Apply(&Mod,4,Mp,1)) break;
				i=CrExpBon::FindThisBonLineInd(Type,(char)Bon,(char)Mod);
				if(Apply(&i,4,Mp,2)) break;
			}
			break;
		case 'A': // AI Multiplier
			CHECK_ParamsMin(2);
			if(Apply(&Type,4,Mp,0)) break;
			v=CrExpoSet::AIMult[Type]; if(Apply(&v,4,Mp,1)) break; CrExpoSet::AIMult[Type]=v;
			break;
		case 'S': // AI Base
			CHECK_ParamsMin(2);
			if(Apply(&Type,4,Mp,0)) break;
			v=CrExpoSet::AIBase[Type]; if(Apply(&v,4,Mp,1)) break; CrExpoSet::AIBase[Type]=v;
			break;
		case 'T': // Town AI Multiplier
			CHECK_ParamsMin(2);
			if(Apply(&Type,4,Mp,0)) break;
			v=CrExpoSet::AITMult[Type]; if(Apply(&v,4,Mp,1)) break; CrExpoSet::AITMult[Type]=v;
			break;
		case 'H': // Human Expo Multiplier
			v=CrExpoSet::PlayerMult; if(Apply(&v,4,Mp,0)) break; CrExpoSet::PlayerMult=v;
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

static void ParseAIExpErt(_Hero_ *hp,_MonArr_ *ma,int *HasArt)
{
	STARTNA(__LINE__, 0)
	int Type;
	CRLOC Crloc;
	CrExpo *cr;
	int i,j,k,n,r,Arts;
	if(hp==0) RETURNV;
	if(IsAI(hp->Owner)==0) RETURNV; // герой АИ
	for(i=j=0;i<64;i++){ // считаем експо артифакты в рюкзаке
		if(hp->OArt[i][0]==(EXPARTFIRST)) j++;
	}
	Arts=j;
	for(i=0;i<7;i++){ // значимость стэков
		j=ma->Ct[i];
		if(j==-1) continue; // no stack
		n=ma->Cn[i];
		if(n==0) continue; // no stack
		k=MonTable[j].SubGroup+1;
		if(CrExpoSet::FindType(ma,i,&Type,&Crloc)==0) r=1;
		else{
			cr=CrExpoSet::Find(Type,Crloc);
			if(cr==0) r=1;
			else r=CrExpMod::GetRank(cr)+1;
		}
		HasArt[i]=k*k*k*r*r*n;
	}
	for(i=0;i<Arts;i++){ // определяем самых значимых
		for(j=k=0,n=-1;j<7;j++) if(HasArt[j]>k){ k=HasArt[j]; n=j; } // ищем самый большой
		if(n==-1) break; // нет больше
		HasArt[n]=-1-i;
	}
	for(i=0;i<7;i++){
		if(HasArt[i]>0) HasArt[i]=0;
		if(HasArt[i]<0) HasArt[i]=-HasArt[i];
	}
	RETURNV
}
// вызывается дважды, если Герой атакует Героя.
void CrExpoSet::StartAllBF(_MonArr_ *AMAr,_MonArr_ *DMAr)
{
	STARTNA(__LINE__, 0)
	CrExpBon::IsBattle=1;
	if(BADistFlag) RETURNV; // from distant all is transfered
	memset(BFBody,0,sizeof(BFBody));
	memset(CrExpBon::BFBody,0,sizeof(CrExpBon::BFBody));
	memset(CrExpMod::BFBody,0,sizeof(CrExpMod::BFBody));
	memset(CrExpBon::BFBodyAct,0,sizeof(CrExpBon::BFBodyAct));
	memset(CrExpBon::BFStat,0,sizeof(CrExpBon::BFStat));
	int i,j,k,o;
	int Type;
	int HasArt[2][7]; for(i=0;i<7;i++){ HasArt[0][i]=0; HasArt[1][i]=0; }
	CRLOC Crloc;
	CrExpBon::CrExpBonStr *crb;
	CrExpMod::CrExpModStr *crm;
	CrExpo *cr;
	if(AMAr==0) RETURNV
	if(DMAr==0) RETURNV
	if(G2B_HrA!=0){ // есть герой атакер
		ParseAIExpErt(G2B_HrA,AMAr,HasArt[0]);
	}
	for(i=0,k=0;i<7;i++){
		j=AMAr->Ct[i];
		if(j==-1) continue; // no stack
		if(AMAr->Cn[i]==0) continue; // no stack
		crb=CrExpBon::Get(j);
		memcpy(CrExpBon::BFBody[k],crb, sizeof(CrExpBon::CrExpBonStr)*20);
		crm=CrExpMod::Get(j);
		memcpy(&CrExpMod::BFBody[k],crm, sizeof(CrExpMod::CrExpModStr));
		if(CrExpoSet::FindType(AMAr,i,&Type,&Crloc)!=0){
			cr=CrExpoSet::Find(Type,Crloc);
			if(cr!=0){ 
				cr->Fl.MType=j;
				BFBody[k]=*cr;
			}
			if(HasArt[0][i]>0){ 
				o=Random(0,7); if(o>4) o++;
				BFBody[k].SetArt(EXPARTFIRST,o);
			}
		}
		CrExpBon::BFBodyAct[k]=i+1;
		k++;
	}
	if(G2B_HrD!=0){ // есть герой атакер
		ParseAIExpErt(G2B_HrD,DMAr,HasArt[1]);
	}
	for(i=0,k=21;i<7;i++){
		j=DMAr->Ct[i];
		if(j==-1) continue; // no stack
		if(DMAr->Cn[i]==0) continue; // no stack
		crb=CrExpBon::Get(j);
		memcpy(CrExpBon::BFBody[k],crb, sizeof(CrExpBon::CrExpBonStr)*20);
		crm=CrExpMod::Get(j);
		memcpy(&CrExpMod::BFBody[k],crm, sizeof(CrExpMod::CrExpModStr));
		if(CrExpoSet::FindType(DMAr,i,&Type,&Crloc)!=0){
			cr=CrExpoSet::Find(Type,Crloc);
			if(cr!=0){ 
				cr->Fl.MType=j;
				BFBody[k]=*cr;
			}
			if(HasArt[1][i]>0) BFBody[k].SetArt(EXPARTFIRST,Random(0,4));
		}
		CrExpBon::BFBodyAct[k]=i+7+1;
		k++;
	}
//  memset(CrExpBon::BFStat,-1,sizeof(CrExpBon::BFStat));
	RETURNV
}
void CrExpoSet::StopAllBF(void)
{
	STARTNA(__LINE__, 0)
	memset(BFBody,0,sizeof(BFBody));
	memset(CrExpBon::BFBody,0,sizeof(CrExpBon::BFBody));
	memset(CrExpMod::BFBody,0,sizeof(CrExpMod::BFBody));
	memset(CrExpBon::BFBodyAct,0,sizeof(CrExpBon::BFBodyAct));
	memset(CrExpBon::BFStat,0,sizeof(CrExpBon::BFStat));
	CrExpBon::IsBattle=0;
	RETURNV
}
/*
void CrExpoSet::AdjustStackTypes(int atDEF,_Hero_ *Hp)
{
	START("CrExpoSet::AdjustStackTypes")
	int i,j,k,Art;
//  CRLOC Crloc;
	CrExpBon::CrExpBonStr *crb;
	CrExpMod::CrExpModStr *crm;
	CrExpo *cr;
	if(Hp==0) RETURNV
	for(i=0,k=atDEF*21;i<7;i++){
		j=Hp->Ct[i];
		if(j==-1) continue; // no stack
		if(Hp->Cn[i]==0) continue; // no stack
		crb=CrExpBon::Get(j);
		memcpy(CrExpBon::BFBody[k],crb, sizeof(CrExpBon::CrExpBonStr)*20);
		crm=CrExpMod::Get(j);
		memcpy(&CrExpMod::BFBody[k],crm, sizeof(CrExpMod::CrExpModStr));
		cr=CrExpoSet::Find(CE_HERO,MAKEHS(Hp->Number,i));
		Art=0; if(BFBody[k].HasArt()) Art=1;
		if(cr!=0){ 
			cr->Fl.MType=j;
			BFBody[k]=*cr;
		}else{
			BFBody[k].Fl.MType=j;
		}
		if(Art) BFBody[k].SetArt(EXPARTFIRST,Random(0,4));
		BFBody[k].Check4Max();
		CrExpBon::BFBodyAct[k]=i+atDEF*7+1;
		k++;
	}
	RETURNV
}
static _Hero_ *SAIS_HeroA,*SAIS_HeroD;
void SwapAIStacks(void)
{
	_EDI(SAIS_HeroA);
	_EBX(SAIS_HeroD);
	START("SwapAIStacks")
	__asm pusha
	if(BADistFlag) goto _Done; // from distant all is transfered
	CrExpoSet::AdjustStackTypes(0,SAIS_HeroA);
	CrExpoSet::AdjustStackTypes(1,SAIS_HeroD);
_Done:    
	__asm popa
	STOP
	__asm{
		mov   eax,0x67F5CC
		mov   eax,[eax]
	}
}
*/