struct _TurnTimeStruct_{
	Dword LastShownTime;
	Dword StartTimeMain;
	Dword TurnLimit;
	Dword ShowNextPeriod;
	Dword BattleStartTime;
};

void __fastcall MPProcessTimeAndRemind(_TurnTimeStruct_ *timer);
void MPStartTimerFix(void);

int ERM_TL(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp);

void ResetLTimer(void);
int  LoadLTimer(int ver);
int  SaveLTimer(void);
