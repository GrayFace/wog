#define VAR_COUNT_V 10000
#define VAR_COUNT_LZ 10
#define BAD_INDEX_V(vi) ((vi<1) || (vi > VAR_COUNT_V + (ErrString.str == LuaErrorString ? 16 : 0) ))
#define BAD_INDEX_LZ(vi) ((vi < -VAR_COUNT_LZ - (ErrString.str == LuaErrorString ? 16 : 0)) || (vi==0))
#define BAD_INDEX_LZ_ALLOW_0(vi) (vi < -VAR_COUNT_LZ - (ErrString.str == LuaErrorString ? 16 : 0))

extern int NextWeekOf;   // недел€ кого-то
extern int NextWeekMess; // сообщение
extern int NextMonthOf;  // тип мес€ца
extern int NextMonthOfMonster; // монстр *2
extern int MonsterUpgradeTable[MONNUM];
extern int ERMVar2[];  extern char ERMVar2Macro[VAR_COUNT_V][16];  extern char ERMVarMacro[20][16];
extern int   ERMVarX[16];
extern int   ERMVarY[100],ERMVarYT[100];
extern float ERMVarF[100],ERMVarFT[100];

extern int ERMVarH[HERNUM][200];  extern int ERMW;  extern char ERMVarHMacro[200][16];
extern char ERMString[1000][512];
extern char ERMLString[][512];  extern char ERMStringMacro[1000][16];
extern Dword pointer;
extern int ERM_GM_ai;   // кто посетил объект
extern _Hero_ *ERM_HeroStr;  // герой, посетивший объект
extern int ERM_PosX, ERM_PosY, ERM_PosL;
extern int EnableMithrill;
#define PL_WONUM 1000
#define PL_ExtDwellStd PL_WoGOptions[0][0]// 0-стандартные двелинги 8-го уровн€
#define PL_TowerStd    PL_WoGOptions[0][1]
#define PL_MLeaveStd   PL_WoGOptions[0][2]
#define PL_NoNPC       PL_WoGOptions[0][3]
#define PL_NoTownDem   PL_WoGOptions[0][4]
#define PL_ApplyWoG    PL_WoGOptions[0][5]
#define PL_NPC2Hire    PL_WoGOptions[0][6]
#define PL_DwellAccum  PL_WoGOptions[0][7]
#define PL_GuardAccum  PL_WoGOptions[0][8]
#define PL_CentElf     PL_WoGOptions[0][9]
#define PL_MLeaveStyle PL_WoGOptions[0][10]
#define PL_CrExpEnable PL_WoGOptions[0][900]
#define PL_CrExpStyle  PL_WoGOptions[0][901]
#define PL_LeaveArt    PL_WoGOptions[0][902]
#define PL_CheatDis    PL_WoGOptions[0][903]
#define PL_ERMErrDis   PL_WoGOptions[0][904]
#define PL_ERMError    PL_WoGOptions[0][905]
#define PL_ExpGainDis  PL_WoGOptions[0][906]
#define PL_NewHero     PL_WoGOptions[0][907]
extern int PL_WoGOptions[2][PL_WONUM]; // все опции
extern int EnableChest[];
extern int MithrillVal[8];
extern union _ErrorCmd_{
	char       Name[32];
	long long  Cmd;
} ErrorCmd;

int CheckPlace4Hero(_MapItem_ *mip,Dword **stp);
int Apply(void *dp,char size,Mes *mp,char ind);
int NewMesMan(Mes *ms,_AMes_ *ap,int ind);
char *ERM2String(char *mes,int zstr,int *len);
int *GetVarAddress(int vi, int vtype);
int GetVarIndex(VarNum *vnp, bool allowBigZ);
int GetVarVal(VarNum *vnp);
int SetVarVal(VarNum *vnp,int Val); // 0 means error, 1 means ok
Dword GetDinMixPos(_ToDo_ *sp);
//Dword GetDinMixPos(_ToDo_ *sp,int *number);
extern char  ERMFlags[];
extern char  ERMMacroName[1000][16];
extern int   ERMMacroVal[1000];
VarNum *GetMacro(char *macro);
/////////////////////
int ApplyString(Mes *Mp,int ind,_AMes_ *amp);
char *GetInternalErmString(int index);
int GetPureErmString(char* &var, int index);
char *GetPureErmString(int index);
int GetErmString(char* &var, int index);
char *GetErmString(int index);
/////////////////////
extern _Cmd_ *FirstTrigger;
extern _Cmd_ *LastAddedTrigger; // последний триггер кучи
extern int TriggerCount;
int InitTrigger(Mes &M, Word Id, int Num, Byte PostFlag, int InsertIndex = -1); // returns 1 if trigger is unknown or -1 in case of error
int InitReciever(Mes &M, Word Id, int Num, Dword &ToDoPo, int &ParSet, VarNum *Par); // returns 1 if reciever is unknown or -1 in case of error
void ProcessERM(bool needLocals = true);
int CheckERM(Mes *ms, bool start = true);
int ParseERM(Mes &M); // Doesn't check for ZVSE (use CheckERM for that)
int ProcessMes(_ToDo_ *sp, Mes &M, char Cmd, int Num); // returns 1 in case of error
void ResetERMMainMenu();
void StepWater(int x,int y,int l);
void PathOverWater(int x1,int y1,int x2,int y2,int l);
//int  ERM2Object(int,Dword,_MapItem_ *,_Hero_ *);
int ERM2Object(int PREpost,int GM_ai,Dword MixPos,_MapItem_ *Mi,_Hero_ *Hr,int,int);
void RunTimer(int Owner);
/////////////////////////////////////////
void FUCall(int n, Mes *Mp = 0, int Num = 0, bool needLocals = true);
int ERM_Universal(char Cmd,int Num,_ToDo_*,Mes *Mp);
Dword __stdcall HintControl0(Dword p3,Dword p2,Dword p1);
void HintControl();
void HintWindow();
int WeekOf(void);
int MonthOf(void);
int MonthOf2(void);
void ApplySphinx(int GM_ai,_Hero_ *Hr,_MapItem_ * /*Mi*/);
void MagicWonder(_Hero_ *hp);
void BACall(Dword n,_Hero_ *hp);
void HeroMove(_Hero_ *hp, int NewX, int NewY, int NewL);
void GainLevel(void);
int __fastcall GainLevelSSkill(_Hero_ * hp, int minLev, int maxLev, int PrevSkill);
void __fastcall PostGainLevel(int skill1, int skill2);
extern int GL_SSkillResult;
void MakeQuestLog(void);
Dword ArtifacsDescription(void);
int CalcObjects(int t,int st);
int FindObjects(int t,int st,int i,int *xv,int *yv,int *lv);
void BFCall(void);
void BACall2(Dword,int);
void BACall3(int,int);
void ArtifactOff(void);
void ArtifactOn(void);
void TriggerIP(Dword n);
void COCall(Dword n,_Hero_ *hp);
// 3.58
void AdvMagicManager(void);
void AdvMagicManagerSkill(void);
void PostInstrCall(void);
int CheatEnabled(void);
int ParseCheatMenuCommand(void);
void EnterTownHall(void);
// 3.59
int __fastcall ChangeFlagColor(_MapItem_ *mip, int r);
int __fastcall DigGrailTrigger(_Hero_ * hero, int r, int x, int y, int z);
////////////////////////////////////////
bool CheckWogify(int HasInternalScripts, int YesERMFile, int HasLua);
void FindERM(void);
int SaveERM(void);
int LoadERM(int ver);
void OnExitToMainMenu();
void ResetERM(int game = 1);
////////////////////////////////////////
void __stdcall ERMDebug(int,int,int,int,int,int,int,int,int,int);
int FOH_CanBeUpgraded(void);
int FOH_UpgradeTo(void);
char __stdcall MouseClick(_MouseStr_ *msp);
int MouseClickTown(void);
int MouseClickHero(_MouseStr_ *msp,_Hero_ *hp);
int MouseClick2Hero(void);
void MouseClickBattle(void);
_Scope_ *FindScope(Word ScopeNumber);
void FreeAllScopes(void);
int AddScope(Word ScopeNumber);
int CalculateScopes(void);
_Scope_ *GetScopeNum(int SNum);
void __stdcall EventPrepareMithril(Dword par1);
int EventMakeUpMithril(void);
int Check4NewSpecPic(int hn);
void SendERMVars(int *len,Byte **buf);
void ReceiveERMVars(int len,Byte *buf);
void SendCreatures(int *len,Byte **buf);
void ReceiveCreatures(int len,Byte *buf);
void DaylyMonChanged(void);
void CrIsChanged(int Num);
void ClearCrChanged(void);
char *ERM2String2(int ind,char *mes);
// 3.58
void SoundCall(void);
void MRCall(int beforeAFTER);
void MFCall(int CallType);
void __stdcall MouseOverTown(_MouseStr_ *MStr);
int __stdcall  MouseClick2(int p4,int p3,int p2,int p1);
int __stdcall MouseClick3(int p3,int p2,int p1);
void SaveManager(void);
void GameAfterLoad(void);
void GameBeforeSave(void);
void DumpERMVars(char *Text,char *Text2);
int Call_Function(int ind,int *var,int num);
// 3.59
void TLCall(Dword timesec);
void DlgCallBack(int dlg,int item,int action);
char *GetErmText(Mes *Mp,int ind);


