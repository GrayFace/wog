#pragma once

#pragma warning ( disable : 4996 )

#include <time.h>
#include "windows.h"
#include "LogManager.h"
#include "erm_lua.h"
#define STRINGER( x ) #x

///////////////////////////////
extern  _Date_    *CurDate;
extern   int       M_MDisabled;
extern   int       M_MDisabledNext;
extern   char     *ErrStringPo;
extern   bool      DoneError;

extern   int   IDummy;
extern   Dword DDummy;
extern   char  CDummy;
extern   Byte  BDummy;
extern   Byte *BPDummy;
extern   char *CPDummy;
extern   int  *IPDummy;
extern   int   GLB_DisableMouse;

extern   char *SourceFileList[];
extern   char LastLoadedDefName[16]; // tmp, sometimes LoadDEF crashes

extern void DumpERMVars(char *Text,char *Text2);

void *Alloc(int size);
void Free(void *po);
int  MesMan(_AMes_ *mp,char *mes,int size);
void DisableMouse(void);

char *_GetErmBuildingName(void);

void Check4MessageEnable(void);
int  Message(const char *mes,int n=1,int showtime=0);
int  Message(int n);
int  a2i(char *val);
int  i2a(int val,char *str);
int  f2a(float val,char *str);
int  a2a(char *src,char *str);
_PlayerSetup_ *GetPlayerSetup(int owner);
int GetRes(int Player,int Nres);
int CheckRes(int Player,int Nres,int Vres);
void AddRes(int Player,int Nres,int Vres);
void SetRes(int Player,int Nres,int Vres);
int Request0(char *mes,int showtime=0);
int Request(char *mes,int l,int showtime=0);
int RequestPic(char *mes,int Type,int Btype,int mode,int showtime=0);
int Request3Pic(char *mes,int T1,int B1,int T2,int B2,int T3,int B3,int mode,int showtime=0,int x=-1,int y=-1);
int __inline Request2Pic(char *mes,int T1,int B1,int T2,int B2,int mode,int showtime=0) { return Request3Pic(mes, T1, B1, T2, B2, -1, 0, mode, showtime); }
int GetCurDate(void);
void SetCurDate(int day);
void SetCurDate(int Month,int Week,int Day);
int CurrentUser(void);
int QuickBattle(void);
void SetQuickBattle(int value);

void Copy(Byte *src,Byte *dst,int sz);
#define FillMem(Dst, Len, Fill)  memset(Dst, Fill, Len)
int Random(int low,int High);
int *RandomSeed();
int Abs(int v);
int StrCmp(const char *src, const char *dst);
int StrCmpOk(const char *src, const char *dst);
// StrNCmp(...) = !memcmp(...)
int __fastcall StrCopy(char *dst,int len,const char *src);
void StrCanc(char *dst,int len,char *s1,char *s2);
void ToUpper(char *str);
void ToLower(char *str);
void CutWord(char *str);
int LoadFile16k(char *name,char *mode);

void MakeLine(int x1,int y1,int x2,int y2,int l,void (*fun)(int x,int y,int l));
_MapItem_ *GetMapItem0(void);
int GetMapSize(void);
int GetMapLevels(void);
_MapItem_ *GetMapItem(int x,int y,int l);
_MapItem_ *GetMapItem2(Dword MixPos);
_MapItem_ *GetObjectEntrance(int x, int y, int l);
_MapItem_ *GetObjectEntrance(Dword mixp);
Byte *LoadPCX(char *PCXName);
int EmptyPlace(_MapItem_ *mp);
int EmptyPlaceWater(_MapItem_ *mp);
int EmptyPlaceAny(_MapItem_ *mp);
int EmptyPlace2(_MapItem_ *mp);
_Hero_ *GetHeroStr(int n);
_Hero_ *FindHeroNum(int x,int y,int l);
void KillHero(int Number);
void Hero2Town(Dword TownManager);
void Town2Hero(Dword TownManager);
void Town2Hero0(_CastleSetup_ *CStr);
//void Terminate(char *Mes);
void Terminate(char *File,int Line);
#define Exit() Terminate(__FILE__,__LINE__)
#define Error() _Error(__FILENUM__,__LINE__)
#define MError(X) _MError(__FILENUM__,__LINE__,X)
#define MError2(X) _MError(__FILENUM__,__LINE__,Format("\"%s\"-%s", ErrorCmd.Name, X))
#define TError(X) _TError(__FILENUM__,__LINE__,X)

#define EWrongCommand() MError2("unknown command.")
#define EWrongSyntax() MError2("wrong syntax.")
#define EWrongParam() MError2("invalid parameter value.")
#define EWrongParamsNum() MError2("wrong number of parameters.")

#define CHECK_ParamsNum(n) if(Num!=n) { EWrongParamsNum(); RETURN(0) }
#define CHECK_ParamsMax(n) if(Num>n) { EWrongParamsNum(); RETURN(0) }
#define CHECK_ParamsMin(n) if(Num<n) { EWrongParamsNum(); RETURN(0) }

char* __cdecl Format(const char* str, ...);
char* __cdecl Format2(const char* str, ...);
char* NewString(const char* str);
void _Error(int file,int line);
void _MError(int file,int line,char *Txt);
void _TError(int File,int Line,char *Txt);
void DumpMessage(char *txt,int offset);
char LoadCrTraits(int WoG);
int IsAI(int gamer);
int IsThis(int gamer);
int CurrentUserSecond(void);
void SetAI(int gamer,int AI_gm);
void ThisIs(int gamer,int ThisGamer);
int GetTeam(int gamer);
void SetTeam(int gamer,char team);
int IsTeam(int gamer1, int gamer2);
_CastleSetup_ *GetCastleBase(void);
int GetCastleNum(void);
_CastleSetup_ *FindCastle(int x,int y,int l);
int IsCastleSetup(_CastleSetup_ *cs);
Dword PosMixed(int x,int y,int l);
void MixedPos(int *x,int *y,int *l,Dword pos);
void Map2Coord(_MapItem_ *Mi,int *x,int *y,int *l);
void HideHero(_Hero_ *Hp);
void ShowHero(_Hero_ *Hp);
void JumpHero(_Hero_ *Hp,int nx,int ny,int nl,int flag);
int Zsprintf2(_ZPrintf_ *Str,char *Format,Dword a1,Dword a2);
int Zsprintf3(_ZPrintf_ *ZFStr,char *Format,Dword a1,Dword a2,Dword a3);
_Dwelling_ *GetDwellingBase(void);
int GetDwellingNums(void);
_Dwelling_ *FindDwelling(int x,int y,int l);
void HideArea(int X,int Y,int Level,int Owner,int Radius);
void ShowArea(int X,int Y,int Level,int Owner,int Radius);
_HeroInfo_ *GetHIBase(void);
Byte *GamerStruc(int hn);
void __fastcall RedrawMap(bool force = false);
void __fastcall RedrawRes(bool force = false);
_Mine_ *GetMineStr(int Num);
void AddExp(_Hero_ *hp);

_ArtRes_ *GetArtResBase(void);
int GetArtResNum(void);
_Dwelling_ *GetSwellingBase(void);
int GetDwellingNum(void);
_Mine_ *GetMineBase(void);
int GetMineNum(void);
_Horn_ *GetHornBase(void);
int GetHornNum(void);
_Monster_ *GetMonsterBase(void);
int GetMonsterNum(void);
_Sign_ *GetSignBase(void);
int GetSignNum(void);
_Univer_ *GetUniverBase(void);
int GetUniverNum(void);
_CrBankMap_ *GetCrBankBase(void);
int GetCrBankNum(void);

int FindEnterShift(int t,int st,int *dx,int *dy,int silence=0);
Dword PlaceObject(int x,int y,int l,int type,int subtype,int t2,int st2,int ter);
void DelObject(int x,int y,int l);
void DelFirstObjectPic(int x,int y,int l);
Dword AddStrObject(int x,int y,int l,int t,int st,int t2,int st2,int ter);
void SetMonMagic(Byte *MonStr,int Spell,int Length,int Level,_Hero_ *Hp);
int DoBattle(int *Mn,_Hero_ *Hp,int Mt,Dword PosMixed);
_Dwelling_ *FindDwellStr(_MapItem_ *Mi);
int HeroHasArt(_Hero_ *hp,int ANum);
int CheckForCreature(_Hero_ *hp,int type);
char *GetArtName(int number);
char *GetSpellName(int number);
char *GetBuildName(int town,int build);
char *GetMonName(int number,int oneMORE);
char *GetSSkillName(int number);
int TimeRandom(int Low, int High);
void SetKeyMaster(int owner,int key,int val);
int GetKeyMaster(int owner,int key);
int StrCmpExt(char *src,char *dst);
Byte *SammonCreatureEx(int type,int num,int pos,int side,int slot,int redraw,int Flags);
Byte *SammonCreature(int type,int num,int pos,int side,int slot,int redraw);
Byte *SammonCreatureIfPossible(int type,int num,int pos,int side,int slot,int redraw,int Flags);
int ArtDisabled(int art);
void ArtDisabledSet(int art,int disab);
_ArtSetUp_ *GetArtBase(void);
void FortOnHill(int hero);
int *MonInTownBase(int Town);
int AddGlbEvent(void);
int DoesExist(int Player);
void _PlaceObject(int x,int y,int l,int type,int subtype,int ter,Dword SetUpWord);
void CoorectMonsterRadius(void);
_Position_ *GetPositionBase(void);
int GetPositionNum(void);
_CastleSetup_ *GetTownBase(void);
int GetTownNum(void);
void ShowThis(int x,int y,char level,int delay);
char *GetHOTableBase(void);
int *GetHHTableBase(void);
int IsPlayerKilled(int Player,int val);
int PlaceObstacle(int object,int position);
void HitMonster(Byte *mon, int damage);
void RedrawBF(void);
void __stdcall CastSpell(int Spell,int Pos,int heroMONSTER,int Pos2,int MagicSkill,int SpellPower);
Byte *GetHexStr(int Pos);
int MapSmPos(_MouseStr_ *MouseStr,Dword *rMixPos);
int GetVisability(int x,int y,int l);
void SetVisability(int x,int y,int l,int v);
void CrChangeDialog(_Hero_ *hp,int Mt0,int Mn0,int Mt1,int Mn1,int Mt2,int Mn2,int Mt3,int Mn3,int Mt4,int Mn4,int Mt5,int Mn5,int Mt6,int Mn6);
// искать объект на корте по типу/подтипу
_MapItem_ *SearchForObject(int x,int y,int l,int type,int stype,int num);
int IsStartUp(void);
int RemoveCreature(int *Mt,int *Mn,int *Mn1,int *Mn2,int pers); // pers домножен на 100
void AddCreature(int *Mt,int *Mn,int *Mt1,int *Mn1);
void RedrawHeroScreen(_Hero_ *hp);
int GetObel(int Player,int Index);
void SetObel(int Player,int Index,int State);
char *SpellDisBase(void);
void AddMon2Hero(_Hero_ *hp,int t,int n,int HUM_ai);
int GetCurHero(void);
//3.58
void EquipArtifact(_Hero_ *hp,int ArtNum,int ArtSNum);
int GetMapItemModif(_MapItem_ *mp, int &subtype);
void RedrawTown(void);
void ShowTSHint(char *txt);
int Search4Substring(char *S,char *D);
void CastAdvSpell(int Spell);
void ChooseAdvSpell(void);
Dword GetExpo(int Lvl);
int GetLimitLvl(void);
void SetLimitLvl(int NewLvl);
void SetMem(void *Mem,int Sz,Byte Val);
int MusicVolume(void);
int EffectVolume(void);
int GameLevel(void);
void SetGameLevel(int Val);
void SplitPath(char *all,char *path,char *name);
void MakePath(char *all,char *path,char *name);
int SaveSetupState(char *Name,void *Buf,int Len);
char *Strtok(char *strToken);
char *Strerror(int errnum);
extern int sprintf(char *buffer,const char *format, ...);
void Itoa(int val,char *str,int Base);
int Atoi(char *str);
float Atof(char *str);
int GetDelay(void);
void SetDelay(int Value);
int GetAutoSave(void);
void SetAutoSave(int Value);
void Display8Dialog(char *TextPo,int (*Ar)[2],int x=-1,int y=-1);
_Mine_ *FindMine(int x,int y,int l);
_Horn_ *FindHorn(int x,int y,int l);
int GenArt(int lvl);
int  GetMerchArt(int Numb);
void SetMerchArt(int Numb,int Art);
int SetMonMassMagic(Byte *Mon,int Spell,int Level);
int WinerIs(void);
int MonIndex(Byte *Mon);
Byte *MonPos(int Index);
void SetMouseCursor(int FType, int SType);
void DelayIt(int MSec);
void ZHideCursor(void);
void ZShowCursor(void);
void SetActiveStack(int Val);
int BuildTownStruct(_CastleSetup_ *TownPo,int Str2Buld);
//void ApplyArtSet(Dword *DstAr);
//void Add2ArtSet(int Ind,int Ar,int Subtype);
//int GetArtSetNum(Dword *ArtSet);
//void GetArtInd(Dword *ArtSet,int Ind,int *Art,int *Subtype);
//void SetArtInd(Dword *ArtSet,int Ind,int Art,int Subtype);
//int CopyArtSet(Dword *ArtSet);
int DoPhysicalDamage(Byte *Mon,int Damage);
int BuildUpCombo(int CNum,int CInd,int ArtNums,int *Arts);
void SummonCreatures3(int,int,int);
Word *GetVisabilityPosByXYL(int x,int y,int l);
int StrSkipLead(char *str,int start);
int StrSkipTrailer(char *str,int start);
Byte *MonPos2(int Side,int SideIndex);
void MPPauseTimer(void);
void MPContinueTimer(void);
void AdjustAllIfPassDwell(int DwNum,int NewOwner);

void _inline *h3new(size_t sz){
	void *(*fn)(int sz); *(Dword *)&fn=0x617492;
	return(fn(sz));
}
void _inline h3delete(void *po){
	void (*fn)(void *p); *(Dword *)&fn=0x60B0F0;
	return(fn(po));
}
// Human Player Functions
typedef void*(__thiscall *_GetHumanPlayerStructure_)(void *base);
#define GetHumanPlayerStructure ((_GetHumanPlayerStructure_)(void*)0x4CE670)
typedef int (__fastcall *_GetHumanPlayerIndex_)(void *base);
#define GetHumanPlayerIndex ((_GetHumanPlayerIndex_)(void*)0x4CE6E0)
// Dialogs Functions
typedef void (__thiscall *_DlgItem_ProcessCmd4Item_)(void *dlg, int cmdType, int cmdSubType, int itemId, int newValue);
#define DlgItem_ProcessCmd4Item ((_DlgItem_ProcessCmd4Item_)(void*)0x5FF400)

void __fastcall ConvertPalettesOfAllDefs_CurPlayer(void* body);
void __fastcall ConvertPalettesOfAllDefs(void* body, int player);
// Combat Functions
typedef void (__thiscall *_CombatMan_SetCurrentMon_)(void *self, int side, int index);
#define CombatMan_SetCurrentMon ((_CombatMan_SetCurrentMon_)(void*)0x464F10)


extern char MapSavedWoG[];
//#pragma warn -inl
class PEr{
	Dword Sig;
	int   InterruptMe;
	char  AType[50]; // 0 - string, 1 - number
	char *Descr[50];
	char *Text[50];
	int   DescrCount;
	//char  Erm[100];
	char *Erm;
	static _ZPrintf_ Frmt;
	static char GlbBuf[2][30000];
	void *GetStackTop();
public:
	bool canLog;
	void *StackTop;
	PEr(){
		for(int i=0;i<50;i++){ Descr[i]=Text[i]=0; }
		Sig='PER_';
		DescrCount = 0;
		Erm = 0;
		StackTop = GetStackTop();
	}
	void Clear(void){ for(int i=0;i<50;i++){ Descr[i]=Text[i]=0; AType[i]=0; } DescrCount = 0; }
	void LastERM(char *s){ Erm = s; }//StrCopy(Erm,99,s); }
	void WrongYVarUsage(void){ 
		Zsprintf2(&Frmt,"WARNING!\n\n%s\n\n ERM Receiver is:\n\n%s",(Dword)"Trying to set Y var outside of a function.",(Dword)(Erm ? Erm : ""));
		StrCopy(GlbBuf[0],30000,Frmt.Str);
		Message(GlbBuf[0],1,-1);
	}
	int Add(char *d,char *t);
	int AddN(int d,char *t);
	static void __fastcall Del(int level);
	void Show(char *Reason,void *Address,int Flag,Dword AddPar,char *Adendum);
};
extern PEr GEr;

//#pragma warn .inl

#ifdef WOG_DIRTY

// Slava recklessly modifies EBX in a lot of places, so LAStEBX should make things safer
#define STARTC(x,y)   __asm{ pusha } int GErLeveL = GEr.Add(x,y); __asm{ popa }  int LAStEBX; __asm{ mov LAStEBX, ebx}
#define START(x)      __asm{ pusha } int GErLeveL = GEr.Add(x,0); __asm{ popa }  int LAStEBX; __asm{ mov LAStEBX, ebx}
#define STARTNA(x,y)  __asm{ pusha } int GErLeveL = GEr.AddN(__FILENUM__*1000000 + x,y); /*_CrtDumpMemoryLeaks();*/ __asm{ popa }  int LAStEBX; __asm{ mov LAStEBX, ebx}
#define STOP          {__asm{ mov ebx, LAStEBX } __asm{ pusha }  PEr::Del(GErLeveL);  __asm{ popa }}
#define RETURN(x)     {__asm{ mov ebx, LAStEBX } __asm{ pusha }  PEr::Del(GErLeveL);  __asm{ popa } return(x);}
#define RETURNV       {__asm{ mov ebx, LAStEBX } __asm{ pusha }  PEr::Del(GErLeveL);  __asm{ popa } return;}

#else

#define STARTC(x,y)   int GErLeveL = GEr.Add(x,y);
#define START(x)      int GErLeveL = GEr.Add(x,0);
#define STARTNA(x,y)  int GErLeveL = GEr.AddN(__FILENUM__*1000000 + x,y);
//#define STOP          {PEr::Del(GErLeveL);}  // use RETURNV instead
#define RETURN(x)     {PEr::Del(GErLeveL); return(x);}
#define RETURNV       {PEr::Del(GErLeveL); return;}

#endif

//  try{
//  }catch(...){ GEr.Add("ZStrNCpy fault",0); throw __LINE__; } 

// 3.59
char MarkedAsChiter(void);
Dword getTime(void);
char IsThisGamer2(void *PlStruct);
int ChooseCrToAnimDlg(void);
int HasAnyAtThisPC(void);

void ErrorMessage(const char * msg);

#define SOD_LoadDef(name) (((int (__fastcall *)(char*))0x55C9C0)(name))
#define SOD_LoadPcx(name) (((int (__fastcall *)(char*))0x55AA10)(name))
#define SOD_LoadPcx16(name) (((int (__fastcall *)(char*))0x55B1E0)(name))

#define SOD_Deref(item) (((int (__fastcall *)(void *))(((int**)item)[0][1]))((void*)item))
