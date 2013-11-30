#include <string.h>
#include "structs.h"
#include "txtfile.h"
#include "service.h"
#include "common.h"
#include "erm.h"
#include "b1.h"
#include "timer.h"

#define __FILENUM__ 18

#define TSP ((_TurnTimeStruct_ *)0x69D680)

char SpecTimerON=0;

static char *TimeFormatMessage[]={
	"\t{%d}",        // seconds
//  "\t{%d : %d}",   // minutes:seconds
	"\t{%d paused}",     // seconds paused
//  "\t{%d : %d paused}" // minutes paused
};
static char Buf[128];
struct _TimeBuf_{
	struct _TimeItem_{
		char  String[0x80];
		int   StartTime;
		char  Flag;
		char _pad[3];
	} *Items;
	int    FirstItemShift;
	int    AlreadyDrawn;
	Dword _field0C;
	Dword _field10;
	int    RedrawFlag;
	Dword _field18;
	int    MaxItemsNum;
	int    LastDrawn;
	Dword _field24;
	Dword _field28;
	Byte  _field2C, _pad1[3];
	Dword _field30;
	Dword _field34;
	Dword _field38;
	Dword _field3C;
	Dword _field40;
};

typedef void (__fastcall *MPProcessTimeAndRemind_ptr)(_TurnTimeStruct_ *timer);

static Dword LastSec=0;
void __fastcall MPProcessTimeAndRemind(_TurnTimeStruct_ *timer)
{
	STARTNA(__LINE__, 0)
	int pausedshift;
	int timeleft;
	MPProcessTimeAndRemind_ptr orfun; *((Dword *)&orfun)=0x557E90;
	do{
		Dword curtime=getTime()/1000;
		if(curtime!=LastSec){
			TLCall(curtime);
			LastSec=curtime;
		}
		if(SpecTimerON==0){ orfun(timer); break; }
		if(MarkedAsChiter()) break;
		if(timer->ShowNextPeriod == 0) break;
		if(timer->TurnLimit == 0) break;
		if(timer->StartTimeMain == 0) break;
		if(timer->LastShownTime == 0) break;
		void *plstr=*((void **)0x69CCFC);
		if(plstr == 0) break;
		if(IsThisGamer2(plstr) == 0) break;
		curtime=getTime();
		if(timer->BattleStartTime != 0){
			pausedshift=1;
			timeleft=timer->TurnLimit-(timer->BattleStartTime-timer->StartTimeMain);
		}else{
			pausedshift=0;
			timeleft=timer->TurnLimit-(curtime-timer->StartTimeMain);
		}
		// force show period to 1 second
		if(timer->ShowNextPeriod > 1000) timer->ShowNextPeriod=1000;
		if((curtime-timer->LastShownTime) < timer->ShowNextPeriod) break;
		timer->LastShownTime=curtime;
//    int timeleft=timer->StartTimeMain-curtime+timer->TurnLimit;
		if(timeleft<0){ timer->ShowNextPeriod=0; break; } 
		int secleft=timeleft/1000;
		int (*sprintfpo)(char *buffer,const char *format, ...);
		*(Dword *)&sprintfpo=0x6179DE;
		sprintfpo(Buf,TimeFormatMessage[pausedshift],secleft);
		_TimeBuf_ *tbpo; *(Dword *)&tbpo=0x69D800;
//    int iind=(tbpo->FirstItemShift+tbpo->AlreadyDrawn)%tbpo->MaxItemsNum;
		while(1){
			if(tbpo->LastDrawn<0) break;
			int iind=(tbpo->FirstItemShift+tbpo->LastDrawn)%tbpo->MaxItemsNum;
//      if(iind==0) break;
			_TimeBuf_::_TimeItem_ *tipo=&tbpo->Items[iind];
			if(tipo->String[0] != '\t') break; // last is not ours
			// last is ours
			strncpy(tipo->String,Buf,0x7F);
			tipo->StartTime=0;
			tipo->Flag=1;
			tbpo->RedrawFlag=1;
			RETURNV
		}
		// last not ours

		tbpo->_field2C=1;
		__asm{
			lea  eax,Buf
			push eax
			mov  eax,tbpo
			push eax
			mov  eax,0x553C40
			call eax
			add  esp,8
		}

/*
			__asm{
				 mov  edx,secleft
				 push edx
				 lea  eax,TimeFormatMessage
				 push eax
				 push 0x69D800
				 mov  eax,0x553D60
				 call eax
				 add  esp, 0Ch
			}
*/
	}while(0);
	RETURNV
}

// Fix Time does not count until new player start turn
void MPStartTimerFix(void){
	Dword turnlimit;
	__asm{
		mov   ecx,0x69D680 // timer struct
		mov   eax,[ecx+8]  // turn limit
		mov   turnlimit,eax
		xor   eax,eax
		mov   [ecx+8],eax  // disable turn limit
		mov   eax,0x419BE0 // show "your turn message"
		call  eax 
		mov   ecx,0x69D680 // timer struct
		mov   eax,turnlimit
		mov   [ecx+8],eax  // restore turn limit
		mov   eax,0x558130 // setup timer for a new player
		call  eax
	}
}

///////////////////////////////
int ERM_TL(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp)
{
	STARTNA(__LINE__, 0)
	int v,v2;
	switch(Cmd){
		case 'E': // enable/disable show timer
			Apply(&SpecTimerON,4,Mp,0);
			break;
		case 'C': // control
			if(Apply(&v,4,Mp,0)){ MError("\"!!TL:C\"-cannot get or check control."); RETURN(0) }
			switch(v){
				case 0: // continue
					MPContinueTimer(); break;
				case 1: // pause
					MPPauseTimer(); break;
				default:
					MError("\"!!TL:C\"-incorrect command."); RETURN(0)
			}
			break;
		case 'T': // get/set times
			CHECK_ParamsMin(2);
			if(Apply(&v,4,Mp,0)){ MError("\"!!TL:T\"-cannot get or check index."); RETURN(0) }
			switch(v){
				case 0: // current time in ms (get)
					v2=getTime(); Apply(&v2,4,Mp,1); break;
				case 1: // current time in sec (get)
					v2=getTime()/1000; Apply(&v2,4,Mp,1); break;
				case 2: // Player Start Time ms (get/set)
					Apply(&TSP->StartTimeMain,4,Mp,1); break;
				case 3: // Player Start Time sec (get/set)
					v2=TSP->StartTimeMain/1000;
					if(Apply(&v2,4,Mp,1)) break;
					TSP->StartTimeMain=v2*1000;
					break;
				case 4: // Player Pause Start Time ms (get/set)
					Apply(&TSP->BattleStartTime,4,Mp,1); break;
				case 5: // Player Pause Start Time sec (get/set)
					v2=TSP->BattleStartTime/1000;
					if(Apply(&v2,4,Mp,1)) break;
					TSP->BattleStartTime=v2*1000;
					break;
				case 6: // Player Turn Limit ms (get/set)
					Apply(&TSP->TurnLimit,4,Mp,1); break;
				case 7: // Player Turn Limit sec (get/set)
					v2=TSP->TurnLimit/1000;
					if(Apply(&v2,4,Mp,1)) break;
					TSP->TurnLimit=v2*1000;
					break;
				default:
					MError("\"!!TL:C\"-incorrect command."); RETURN(0)
			}
			break;
		 
/*
		case 'S': // S#герой/#хозяин/#номер/$x/$y/$l/$значимость/$флаги
			CHECK_ParamsMin(8);
			if(Apply(&h,4,Mp,0)){ MError("\"!!AI:S\"-cannot get or check hero."); RETURN(0) }
			if(Apply(&o,4,Mp,1)){ MError("\"!!AI:S\"-cannot get or check owner."); RETURN(0) }
			if(Apply(&n,4,Mp,2)){ MError("\"!!AI:S\"-cannot get or check number."); RETURN(0) }
			ind=AIRGetThis((short)h,(short)o,(short)n);
			if(ind<0){ // нет такого
				 x=-1; y=-1; l=0;
				flags=0; AIval=500000;
			}else{
				AIval=AIRun[ind].AIValue;
				mixpos=AIRun[ind].MixPos;
				MixedPos(&x,&y,&l,mixpos);
				flags=AIRun[ind].Flags;
			}
			chk=0;
			if(Apply(&x,4,Mp,3)) chk=1; // check or get
			if(Apply(&y,4,Mp,4)) chk=1; // check or get
			if(Apply(&l,4,Mp,5)) chk=1; // check or get
			if(Apply(&AIval,4,Mp,6)) chk=1; // check or get
			if(Apply(&flags,2,Mp,7))  chk=1; // check or get
			if(chk) break;
			if(ind<0){ // нет такого - добавим
				ind=AIRAddThis((short)h,(short)o,(short)n);
				if(ind<0) break; // не установили
			}
			AIRun[ind].AIValue=AIval;
			mixpos=PosMixed(x,y,l);
			AIRun[ind].MixPos=mixpos;
			AIRun[ind].Flags=flags;
			break;
		case 'D': // D#герой/#хозяин/#номер
			CHECK_ParamsMin(3);
			if(Apply(&h,4,Mp,0)){ MError("\"!!AI:D\"-cannot get or check hero."); RETURN(0) }
			if(Apply(&o,4,Mp,1)){ MError("\"!!AI:D\"-cannot get or check owner."); RETURN(0) }
			if(Apply(&n,4,Mp,2)){ MError("\"!!AI:D\"-cannot get or check number."); RETURN(0) }
			if(n<1) AIRDelAll((short)h,(short)o);
			else    AIRDelThis((short)h,(short)o,(short)n);
			break;
*/
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}


void ResetLTimer(void){
	SpecTimerON=0;
}
int LoadLTimer(int ver)
{
	STARTNA(__LINE__, 0)
	ResetLTimer();
	if(ver<18) RETURN(0)
	// start with 3.59
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='T'||buf[1]!='I'||buf[2]!='M'||buf[3]!='R') {MError("LoadTimer cannot start loading"); RETURN(1)}
	if(Loader(&SpecTimerON,sizeof(SpecTimerON))) RETURN(1)
	RETURN(0)
}
int SaveLTimer(void)
{
	STARTNA(__LINE__, 0)
	if(Saver("TIMR",4)) RETURN(1)
	if(Saver(&SpecTimerON,sizeof(SpecTimerON))) RETURN(1)
	RETURN(0)
}

