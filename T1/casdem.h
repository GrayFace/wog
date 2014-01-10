#pragma once
//extern TxtFile    TDBuild;
//extern TxtFile    TDCreat;
//extern Byte CSCheck[48][10];
//extern int CSValues[48][4][2];
//extern int CS2Crash[48];
//extern int CSMonst[7*2]; // стоимость ухода существ 7 уровней

struct _BuildMask_{
	Dword Lo,Hi;
};

extern struct _BuildMask_ AllEnBuildings[TOWNNUM];
extern struct _BuildMask_ BuildDepends[TOWNNUM][44];
extern struct _BuildMask_ BuildExclusions[TOWNNUM][44];
extern Byte TownStructOnScreen[TOWNNUM][18];
extern  Byte TownStructOnScreenNum[TOWNNUM];

int LoadTXTCasDem(void); // загружаем строки
int LoadCasDem(int ver);
int SaveCasDem(void);
int ERM_CasDem(char,int,_ToDo_ *sp,Mes *Mp);

void PrepareCastle(Byte *CStructure);
void CastleCheck(int flag);
void CastleService(void);
void CastleService2(void);
void CastleExit(Dword Pointer);
void DaylyCastle(int);
void ResetCastles(void);
void __stdcall CastleServiceRedraw(Dword,Dword);
void __stdcall AIBuild(Dword);
int Enter2Castle(int GM_ai,_MapItem_ *Mi,_Hero_ *Hr,Dword /*avd.Manager*/);
void __stdcall MakeTownDefender(Dword v2,Dword v1);
void InitCastles(void);
int __fastcall CorrectTownIncome(_CastleSetup_ * town, int income);
int __fastcall CorrectTownGrowth(_CastleSetup_ * town, int income, int level);
int ERM_CastleIncome(char Cmd, int Num, _ToDo_*, Mes *Mp);
int ERM_Castle(char Cmd,int Num,_ToDo_*sp,Mes *Mp);
void SetTowerBunus(_CastleSetup_ *Castle,Byte *MonStr);
void NextTowerState(_CastleSetup_ *Castle);
void CastleTowers(void);
extern int CrChangeDialogResult[2][7];
void CrChangeDialog(_Hero_ *hp);
void CrChangeDialog(_Hero_ *hp,int Mt0,int Mn0,int Mt1,int Mn1,int Mt2,int Mn2,
	int Mt3,int Mn3,int Mt4,int Mn4,int Mt5,int Mn5,int Mt6,int Mn6);
int GetCastleNumber(_CastleSetup_ *CStructure);
// 3.58
void __stdcall SoDFix_SpaceInAlliedTown(int par1,int par2,int par3);
// 3.58f
void FixWrongTownListInfo(void);
// 3.59
// network support
void SendCastleState(int cnum,int *len,Byte **buf);
void ReceiveCastleState(int len,Byte *buf);
// new town support
void ResetNewTownStruct(void);
void AdjustNewTownStructures(void);
int SaveNewTown(void);
int LoadNewTown(int ver);


