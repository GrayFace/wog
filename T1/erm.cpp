#include <stddef.h>
#include <stdio.h>
#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "b1.h"
#include "casdem.h"
#include "service.h"
#include "herospec.h"
#include "monsters.h"
#include "anim.h"
#include "womo.h"
#include "sound.h"
#include "npc.h"
#include "erm.h"
#include "ai.h"
#include "CrExpo.h"
//#include "wogsetup.h"
#include "timer.h"
#define __FILENUM__ 1
#include "string.h"
#include "stdlib.h"
#include "animdlg.h"
#include "lod.h"
#include "dlg.h"
#include "spell.h"
#include "lod.h"
#include "erm_lua.h"
#include "global.h"

bool IsLuaCall = false;

_Cmd_     *Heap;
Dword      ERMHeapSize=0;
// _Date_    *CurDate;
_Main_    *Main;
////Word CurrentScope=0;
////_Scope_ *FirstERMScope=0;
////_Scope_ *ERMScope=0;
//int        ERMEnabled=0;
int        ERM_RetVal=0;
int        ERM_PosX=0,ERM_PosY=0,ERM_PosL=0;
int        ERM_LastX=0,ERM_LastY=0,ERM_LastL=0;
int        ERM_GM_ai=-1;   // кто посетил объект
//int        ERM_This=-1;    // это тот, кто перед монитором?
_Hero_    *ERM_HeroStr=0;  // герой, посетивший объект
int   PL_OptionReset[11];
#define PL_OptionReset2Size (8)
int   PL_OptionReset2[PL_OptionReset2Size];
int   PL_NPCOptionWasSet=0;
int   PL_CustomOptionWasSet=0; // 3.58
int   PL_DisableWogifyWasSet=0; // 3.58
int   PL_ExpoWasSet=0; // 3.58
int   WoGSpecMap=0; // карта random_... или wogify_...
int   PL_WoGOptions[2][PL_WONUM]; // все опции
// на самом деле используются с 1 по 20 вкл
char  ERMMacroName[1000][16];
int   ERMMacroVal[1000];
int   ERMVar[20];           char ERMVarMacro[20][16];
char  ERMVarUsed[20];
char  ERMFlags[1000];
char  ERMFlagsUsed[1000];
int   ERMVar2[VAR_COUNT_V + 16];       char ERMVar2Macro[VAR_COUNT_V][16];
char  ERMVar2Used[VAR_COUNT_V];
int   ERMW=0;
int   ERMVarH[HERNUM][200]; char ERMVarHMacro[200][16];
char  ERMVarHUsed[200];
char  ERMString[1000][512];  char ERMStringMacro[1000][16];
char  ERMStringUsed[1000];
char  ERMLString[VAR_COUNT_LZ + 16][512];
char  ERMFunUsed[30000];
char  ERMTimerUsed[100];
int   ERMVarUsedStore=0; //делать ли дамп использования переменных
int   ERMVarCheck4DblStore=0; // проверять на дублирование
int   NextMessShowTime=0; // show time for the next message (self reset, not saved)
int   NextMessX=-1;
int   NextMessY=-1;
Word  GlobalCurrentScope=0; // 3.59 current trigger scope
struct _Square_{
	unsigned Hero    :8; // последний посетивший герой
	signed   Owner   :4; // хозяин
	unsigned Number  :4; // пользовательское число 0...15
	unsigned NumberT :8; // 0...255
	unsigned NumberS :8; // 0...255
} Square[144][144][2];
struct _Square2_{
	short S[4];
	long  L[2];
} Square2[144][144][2];

int NextWeekOf=0;   // неделя кого-то
int NextWeekMess=0; // сообщение
int NextMonthOf=0;  // тип месяца
int NextMonthOfMonster=0; // монстр *2
int AI_Delay=0x1000;
int AutoSaveFlag=1;
//char Load2Path[MAX_PATH]; // 3.58 alternate path for loading scripts
struct _Timer_{
	Word FirstDay;
	Word LastDay;
	Word Period;
	Word Owners; // побитово
} ERMTimer[200];
// не надо запоминать
int   ERMVarX[16];
int   ERMVarY[100],ERMVarYT[100];
float ERMVarF[100],ERMVarFT[100];

int EnableMithrill=0;
int EnableChest[20]={0,0,0,0,0};
int MithrillVal[8]={0,0,0,0,0,0,0,0};

int MonsterUpgradeTable[MONNUM];

struct _ArtNames_{
	int NameVar; // номер z переменной (0-оригинальный)
	int DescVar;
	int PickUp;
} ArtNames[ARTNUM];
int ArtSetUpBackStored=0;
_ArtSetUp_ ArtSetUpBack[ARTNUM];
char * ArtPickUpBack[ARTNUM];

int HTableBackStored=0;
_HeroInfo_ HTableBack[HERNUM+8];

struct _SSAllNames_{
// 0-Имя, 1-Баз.Опис, 2- Продв.Опис, 3-Эксп.Опис
	int Var[4];   // номер z переменной (0-оригинальный)
} SSAllNames[SSNUM];
int SSNameBackStored=0;
_SSName_ SSNameBack[SSNUM];
int SSDescBackStored=0;
_SSDesc_ SSDescBack[SSNUM];
int SSNameDescBackStored=0;
_SSNameDesc_ SSNameDescBack[SSNUM];

struct _MonNames_{
// 0-Имя один, 1-Имя много, 2-Специальность, 3-def, 4-sound prefix
	int Var[5];   // номер z переменной (0-оригинальный)
} MonNames[MONNUM];
int MonNameBackStored=0;
_MNameS_  MonTable2Back[MONNUM];
_MNameP_  MonTable3Back[MONNUM];
_MNameSp_ MonTable4Back[MONNUM];

struct _HSpecNames_{
	int PicNum; // номер картинки специализации +1
// 0-короткое, 1-длинное, 2-описание
	int Var[3];   // номер z переменной (0-оригинальный)
} HSpecNames[HERNUM];
int HSpecBackStored=0;
_HeroSpec_ HSpecBack[HERNUM];

char ArtSkill[0x2D][4]={
	{3,3,3,6},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,0,0},
	{3,0,0,0},{4,0,0,0},{5,0,0,0},{6,0,0,0},{12,-3,0,0},{0,2,0,0},{0,3,0,0},{0,4,0,0},
	{0,5,0,0},{0,6,0,0},{-3,12,0,0},{0,0,0,1},{0,0,0,2},{0,0,0,3},{0,0,0,4},{0,0,0,5},
	{0,0,-2,10},{0,0,1,0},{0,0,2,0},{0,0,3,0},{0,0,4,0},{0,0,5,0},{0,0,10,-2},{1,1,1,1},
	{2,2,2,2},{3,3,3,3},{4,4,4,4},{5,5,5,5},{6,6,6,6},{1,1,0,0},{2,2,0,0},{3,3,0,0},
	{4,4,0,0},{0,0,1,1},{0,0,2,2},{0,0,3,3},{0,0,4,4}
};
Byte Ar0 [11][2]={{0x23,0x13},{0x19,0x15},{0x13,0x14},{0x10,0x16},{0x0D,0x17},
 {0x0A,0x7B},{0x07,0x18},{0x05,0x2C},{0x03,0x7C},{0x01,0x24},{0,0}};
Byte Ar1 [10][2]={{0x19,0x44},{0x13,0x3A},{0x0F,0x63},{0x0C,0x6D},{0x09,0x2A},
 {0x07,0x53},{0x05,0x37},{0x03,0x4E},{0x01,0x48},{0,0}};
Byte Ar2 [18][2]={{0x2C,0x66},{0x29,0x42},{0x26,0x39},{0x23,0x65},{0x20,0x68},
 {0x1D,0x6C},{0x1A,0x69},{0x17,0x6A},{0x14,0x67},{0x11,0x64},
 {0x0E,0x6B},{0x0B,0x36},{0x08,0x61},{0x06,0x47},{0x04,0x4C},
 {0x02,0x2B},{0x01,0x21},{0,0}};
Byte Ar3 [10][2]={{0x21,0x07},{0x1B,0x09},{0x15,0x08},{0x11,0x0A},{0x0D,0x0B},
 {0x09,0x0C},{0x06,0x26},{0x03,0x23},{0x01,0x80},{0,0}};
Byte Ar4 [ 9][2]={{0x1F,0x0D},{0x19,0x0F},{0x12,0x0E},{0x0E,0x10},{0x0A,0x11},
 {0x06,0x12},{0x03,0x27},{0x01,0x22},{0,0}};
Byte Ar5 [ 9][2]={{0x11,0x19},{0x0C,0x1B},{0x09,0x1A},{0x07,0x1C},{0x05,0x1D},
 {0x03,0x1E},{0x02,0x28},{0x01,0x1F},{0,0}};
Byte Ar67[11][2]={{0x1B,0x43},{0x15,0x46},{0x11,0x71},{0x0E,0x6E},{0x0B,0x45},
 {0x08,0x4D},{0x06,0x2D},{0x04,0x25},{0x02,0x5F},{0x01,0x5E},{0,0}};
Byte Ar8 [ 7][2]={{0x11,0x3B},{0x0C,0x62},{0x09,0x29},{0x06,0x38},{0x03,0x20},
 {0x01,0x5A},{0,0}};
Byte ArC [28][2]={{0x57,0x72},{0x53,0x73},{0x4F,0x74},{0x4B,0x75},{0x47,0x70},
 {0x43,0x5D},{0x3F,0x5C},{0x3B,0x5B},{0x37,0x4F},{0x33,0x50},
 {0x2F,0x51},{0x2B,0x52},{0x27,0x3C},{0x24,0x3D},{0x21,0x3E},
 {0x1E,0x6F},{0x1B,0x60},{0x18,0x49},{0x15,0x4A},{0x12,0x4B},
 {0x0F,0x7D},{0x0C,0x7E},{0x09,0x56},{0x07,0x57},{0x05,0x58},
 {0x03,0x59},{0x01,0x7F},{0,0}};

Byte ArtSlots[]={
// номер,+атака,+защита,+сила,+знания, занятые слоты ...
	0x81,21,21,21,21, 0,2,4,5,7,
	0x82,0,0,0,0,     2,7,
	0x83,0,0,0,0,     6,6,
	0x84,3,3,2,2,     0,3,4,
	0x85,0,0,0,0,     8,8,8,8,
	0x86,16,16,16,16, 0,1,2,3,4,6,6,7,
	0x87,9,9,8,8,     0,4,5,
	0x88,0,0,0,0,     2,
	0x89,0,0,0,0,     8,8,
	0x8A,0,0,0,0,     8,8,
	0x8B,0,0,0,0,     1,2,
	0x8C,0,0,0,0,     1,6,6,
	0x99 // последний
};
static char ME_Buf2[1000000];

_ErrorCmd_ ErrorCmd;

int SetErrorCmd(Word recId, char cmdId)
{
	DoneError = false;
	long long last = ErrorCmd.Cmd;
	*(Word*)ErrorCmd.Name = recId;
	ErrorCmd.Name[2] = ':';
	ErrorCmd.Name[3] = cmdId;
	ErrorCmd.Name[4] = 0;
	return last;
}

int SetErrorTrigger(Word trigId, char triggerSign)
{
	DoneError = false;
	long long last = ErrorCmd.Cmd;
	ErrorCmd.Name[0] = '!';
	ErrorCmd.Name[1] = triggerSign;
	*(Word*)&ErrorCmd.Name[2] = trigId;
	ErrorCmd.Name[4] = (triggerSign == '!') ? ':' : 0;
	ErrorCmd.Name[5] = 0;
	return last;
}

void DumpERMVars(char *Text,char *Text2)
{
	// 3.58
	STARTNA(__LINE__, 0)
	int hout;
	char *p = ME_Buf2;
	char *p2 = p + sizeof(ME_Buf2);
	p += sprintf_s(p, p2-p, "-----------------------\n%s\n-----------------------\n",Text);
	if(Text2!=0){
		p += sprintf_s(p, p2-p, "-----Context-----\n%.200s.....\n-----------------\n",Text2);
	}
	if(ErrString.str!=0){
		p += sprintf_s(p, p2-p, "ERM stack traceback:\n");
		for(ErrStringInfo *e = &ErrString; e && e->str; e = e->last){
			p += sprintf_s(p, p2-p, "\n%s", LuaPushERMInfo(e->str));
			lua_pop(Lua, 1);
		}
		p += sprintf_s(p, p2-p, "\n-----------------\n");
	}
	LuaCallStart("traceback");
	LuaPCall(0, 1);
	p += sprintf_s(p, p2-p, "Lua %s\n-----------------\n", lua_tostring(Lua, -1));
	lua_pop(Lua, 1);

	p += sprintf_s(p, p2-p, "COMMON VARS\n");
	p += sprintf_s(p, p2-p, "f=%i\ng=%i\nh=%i\ni=%i\nj=%i\nk=%i\nl=%i\nm=%i\nn=%i\no=%i\np=%i\nq=%i\nr=%i\ns=%i\nt=%i\n",
		ERMVar[0],ERMVar[1],ERMVar[2],ERMVar[3],ERMVar[4],
		ERMVar[5],ERMVar[6],ERMVar[7],ERMVar[8],ERMVar[9],
		ERMVar[10],ERMVar[11],ERMVar[12],ERMVar[13],ERMVar[14]);
	int i,j;
	p += sprintf_s(p, p2-p, "Common flags (1...1000)\n");
	for(i=0;i<1000;i++){ 
		if(ERMFlags[i]==0) continue;
		p += sprintf_s(p, p2-p, "flag%i=%i\n",i+1,ERMFlags[i]); 
	}
	p += sprintf_s(p, p2-p, "Common v vars (v1...v10000)\n");
	for(i=0;i<VAR_COUNT_V;i++){ 
		if(ERMVar2[i]==0) continue;
		p += sprintf_s(p, p2-p, "v%i=%i\n",i+1,ERMVar2[i]); 
	}
	p += sprintf_s(p, p2-p, "Hero's vars (w1...w200)\n");
	for(i=0;i<HERNUM;i++){
		hout=0;
		for(j=0;j<200;j++){ 
			if(ERMVarH[i][j]==0) continue;
			if(hout==0){ p += sprintf_s(p, p2-p, "Hero#=%i\n",i); hout=1; }
			p += sprintf_s(p, p2-p, "w%i=%i\n",j+1,ERMVarH[i][j]); 
		}
	}
	p += sprintf_s(p, p2-p, "\nTRIGGER BASED VARS\n");
	p += sprintf_s(p, p2-p, "Trigger y vars (y-1...y-100)\n");
	for(i=0;i<100;i++){ 
		if(ERMVarYT[i]==0) continue;
		p += sprintf_s(p, p2-p, "y-%i=\"%i\"\n",i+1,ERMVarYT[i]); 
	}
	p += sprintf_s(p, p2-p, "Trigger e vars (e-1...e-100)\n");
	for(i=0;i<100;i++){ 
		if(ERMVarFT[i]==0.0) continue;
		p += sprintf_s(p, p2-p, "e-%i=\"%f\"\n",i+1,ERMVarFT[i]); 
	}

	p += sprintf_s(p, p2-p, "\nFUNCTION BASED VARS\n");
	p += sprintf_s(p, p2-p, "Parameters x vars (x1...x16)\n");
	for(i=0;i<16;i++){ 
		if(ERMVarX[i]==0) continue;
		p += sprintf_s(p, p2-p, "x%i=\"%i\"\n",i+1,ERMVarX[i]); 
	}
	p += sprintf_s(p, p2-p, "Local y vars (y1...y100)\n");
	for(i=0;i<100;i++){ 
		if(ERMVarY[i]==0) continue;
		p += sprintf_s(p, p2-p, "y%i=\"%i\"\n",i+1,ERMVarY[i]); 
	}
	p += sprintf_s(p, p2-p, "Local e vars (e1...e100)\n");
	for(i=0;i<100;i++){ 
		if(ERMVarF[i]==0.0) continue;
		p += sprintf_s(p, p2-p, "e%i=\"%f\"\n",i+1,ERMVarF[i]); 
	}
	p += sprintf_s(p, p2-p, "\nSTRING VARS\n");
	p += sprintf_s(p, p2-p, "Common z vars (z1...z1000)\n");
	for(i=0;i<1000;i++){ 
		if(ERMString[i][0]==0) continue;
		p += sprintf_s(p, p2-p, "z%i=\"%s\"\n",i+1,ERMString[i]); 
	}
	p += sprintf_s(p, p2-p, "Local z vars (z-1...z-10)\n");
	for(i=0;i<VAR_COUNT_LZ;i++){ 
		if(ERMLString[i][0]==0) continue;
		p += sprintf_s(p, p2-p, "z-%i=\"%s\"\n",i+1,ERMLString[i]); 
	}
	SaveSetupState("WOGERMLOG.TXT",ME_Buf2,strlen(ME_Buf2));
	RETURNV
}

Byte  HeapArray[HEAPSIZE];

int Apply(void *dp,char size,Mes *mp,char ind);
int NewMesMan(Mes *ms,_AMes_ *ap,int ind);
char *ERM2String(char *mes,int zstr,int *len);
void DelObjPos(Dword MixPos);
void ResetCustomSettings(char *FName);


char *GetInternalErmString(int index)
{
	if(BAD_INDEX_LZ(index))
	{
		MError2("wrong z var index (-10...-1,1...1000+).");
		return 0;
	}

	if (index>1000)
		return StringSet::GetText(index);
	else if(index>0)
		return ERMString[index-1];
	else
		return ERMLString[-index-1];
}

char *GetPureErmString(int index)
{
	char *str = GetInternalErmString(index);
	if (str == 0) return 0;
	return (index > 1000) ? ERM2String(str, 1, 0) : str;
}

int GetPureErmString(char* &var, int index)
{
	char *str = GetInternalErmString(index);
	if (str == 0) return 1;
	var = (index > 1000) ? ERM2String(str, 1, 0) : str;
	return 0;
}

int GetErmString(char* &var, int index)
{
	char *str = GetInternalErmString(index);
	if (str == 0) return 1;
	var = ERM2String(str, 1, 0);
	return 0;
}

char* GetErmString(int index)
{
	char *str = GetInternalErmString(index);
	return str == 0 ? 0 : ERM2String(str, 1, 0);
}


struct StoredLocalVars
{
	int    OldY[100];
	float  OldF[100];
	char   OldString[10][512];
};

void _DoStoreVars(void *loc, void *var, int size, bool restore, bool fill)
{
	if (restore)
	{
		memcpy((char*)var, (char*)loc, size);
	}
	else
	{
		memcpy((char*)loc, (char*)var, size);
		if (fill) FillMem((char*)var, size, 0);
	}
}

int TriggerDepth = 0;

void StoreVars(StoredLocalVars *vars, bool fu, bool restore = false)
{
	TriggerDepth += (restore ? -1 : 1);
	if (!fu && TriggerDepth == (restore ? 0 : 1)) return;
	_DoStoreVars(vars->OldY, (fu ? ERMVarY : ERMVarYT), 4*100, restore, fu);
	_DoStoreVars(vars->OldF, (fu ? ERMVarF : ERMVarFT), 4*100, restore, fu);
	_DoStoreVars(vars->OldString, ERMLString, 10*512, restore, false);
}

///////////////////////////
//void FUCall(int n);

static char LCT_s1[]="..\\MAPS\\";

char *GetErmText(Mes *Mp,int ind){
	STARTNA(__LINE__, 0)
	char *ret = 0;
	int vv;
	if(Mp->VarI[ind].Check!=0){ MError2("cannot use get or check syntax."); RETURN(0) }

	int strType = Mp->VarI[ind].Type;
	if (strType == 7)
	{
		int i = GetVarIndex(&Mp->VarI[ind], true);
		ret = (i ? GetErmString(i) : 0);
	}
	else
		if (strType == 0 && Mp->n[ind] == 0 && Mp->m.s[Mp->i] == STRCH)
		{
			ret = ERM2String(&Mp->m.s[Mp->i], 0, &vv);
			Mp->i += vv;
		}
		else
		{
			MError2("string expected.");
		}
	RETURN(ret);
}

static int TriggerBreak=0;
static int TriggerGoTo=0;
static int YVarInsideFunction=1; // default is OFF to check
Mes *LastFUMes;
int LastFUNum;

int ERM_Function(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int i,v;
	int   _lYVarInsideFunction;
	int   OldX[16];
	int   NewX[16];
//  VarNum *vnp=(VarNum *)&sp->Pointer;
	switch(Cmd){
		case 'P': // вызвать функцию
			v=GetVarVal(&sp->Par[0]);
			for(i=0;i<16;i++) { OldX[i]=ERMVarX[i]; }
			//for(i=0;i<10;i++) for(j=0;j<512;j++) { OldString[i][j]=ERMLString[i][j]; }
			for(i=0;i<16;i++){ ERMVarX[i]=0; }
			for(i=0;i<Num;i++){ ERMVarX[i]=Mp->n[i]; }
			_lYVarInsideFunction=YVarInsideFunction;
			YVarInsideFunction=1;
			FUCall(v, Mp, Num);
			YVarInsideFunction=_lYVarInsideFunction;
			// 3.58 return values
			// first restore y vars (and others)
			for(i=0;i<16;i++) { NewX[i]=ERMVarX[i]; ERMVarX[i]=OldX[i]; }
			// now if used ?y or ?x, it will be stored correctly
			for(i=0;i<Num;i++)
				if(Mp->VarI[i].Check==1) // ?
					Apply(&NewX[i],4,Mp,(char)i);
			break;
		case 'D': // 3.58 distant call
			int vars[16];
			for(i=0;i<Num;i++){ vars[i]=Mp->n[i]; }
			SendCustomRequest(GetVarVal(&sp->Par[0]),vars,Num);
			break;
		case 'E': // покинуть триггер
			TriggerBreak=1;
			TriggerGoTo=0;
			CHECK_ParamsMax(1);
			if(Num == 1)  TriggerGoTo = Mp->n[0];
			if(TriggerGoTo<-256){ MError2("cannot jump more than 256 triggers back."); RETURN(0) }
			break;
		case 'X':
		{
			CHECK_ParamsNum(2);
			v = Mp->n[0] - 1;
			if((v<0)||(v>=16)){ MError2("x var index out of range (1...16)."); RETURN(0) }
			int check = 0;
			if (LastFUMes && v < LastFUNum)
			{
				check = LastFUMes->VarI[v].Check;
				if (check == 0)
					check = -LastFUMes->f[v];
				else if (check == 2 && LastFUMes->f[v] != 0)
					check = -2;
			}
			Apply(&check, 4, Mp, 1);
			break;
		}
		case 'C': // 3.58f check wrong Y var usage outside of trigger
			YVarInsideFunction=Mp->n[0];
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
// call from a network
int Call_Function(int ind,int *var,int num){
	STARTNA(__LINE__, 0)
	int i;
	int   _lYVarInsideFunction;
	int   OldX[16];
	//if(WoG==0) RETURN(0) // поддерживается только в WoG
	for(i=0;i<16;i++) { OldX[i]=ERMVarX[i]; }
	for(i=0;i<num;i++){ ERMVarX[i]=var[i]; }
	_lYVarInsideFunction=YVarInsideFunction;
	YVarInsideFunction=1;
	FUCall(ind, 0, 0);
	YVarInsideFunction=_lYVarInsideFunction;
	for(i=0;i<16;i++) { ERMVarX[i]=OldX[i]; }
	RETURN(1)
}

int ERM_Do(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int i,v,from,to,step;
//  Cycle *cp=(Cycle *)&sp->Pointer[1];
	int   _lYVarInsideFunction;
	int   OldX[16];
	int   NewX[16];
	StoredLocalVars vars;
	int   haveEqual; // have a = parameter (P=v1;)
	switch(Cmd){
		case 'P': // вызвать функцию
			v=GetVarVal(&sp->Par[0]);
			from=GetVarVal(&sp->Par[1]);
			to=GetVarVal(&sp->Par[2]);
			step=GetVarVal(&sp->Par[3]);
			StoreVars(&vars, true);
			for(i=0;i<16;i++) { OldX[i]=ERMVarX[i]; }
			for(i=0;i<16;i++){ ERMVarX[i]=0; }
			for(i=0;i<Num;i++){ ERMVarX[i]=Mp->n[i]; }
			haveEqual = 0;
			for(i=0;i<Num;i++)
				if(Mp->VarI[i].Check==2) haveEqual = 1;

			_lYVarInsideFunction=YVarInsideFunction;
			YVarInsideFunction=1;
			for(ERMVarX[15]=from;ERMVarX[15]<=to;ERMVarX[15]+=step){
				// 3.58 return values
				for(i=0;i<Num;i++)
					if(Mp->VarI[i].Check==1) // ?
						ERMVarX[i]=Mp->n[i];
				
				if (haveEqual)
					for(i=0;i<Num;i++)
						if(Mp->VarI[i].Check==2) // =
							ERMVarX[i]=GetVarVal(&Mp->VarI[i]);

				FUCall(v, Mp, Num, false);
				if (haveEqual){
					for(i=0;i<Num;i++){
						if(Mp->VarI[i].Check!=1) continue; // не ?
						int t = Mp->VarI[i].IType;
						if(t == 0) t = Mp->VarI[i].Type;
						if(t == 6 && Mp->n[i] > 0 || t == 7 && Mp->n[i] < 0) continue; // не y1..y100, z-1..z-10
						Apply(&ERMVarX[i],4,Mp,(char)i);
					}
				}
			}
			YVarInsideFunction=_lYVarInsideFunction;
			for(i=0;i<16;i++) { NewX[i]=ERMVarX[i]; ERMVarX[i]=OldX[i]; }
			StoreVars(&vars, true, true);
			// now if ?y or ?x is used, it will be stored correctly
			for(i=0;i<Num;i++){
				if(Mp->VarI[i].Check!=1) continue; // не ?
				Apply(&NewX[i],4,Mp,(char)i);
			}
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

int CheckVarIndex(int vi, int vtype, bool allowBigZ)
{
	STARTNA(__LINE__, 0)
	switch(vtype){
		case 0: // число
			break;
		case 1:
			if((vi<1)||(vi>1000)){ MError2("Flag is out of set (1...1000) in CheckVarIndex."); RETURN(0) }
			break;
		case 2: // f...t
			if((vi<1)||(vi>15)){ MError2("Var is out of set (f...t) in CheckVarIndex."); RETURN(0) }
			break;
		case 3: // v1...1000
			if(BAD_INDEX_V(vi)){ MError2("Var is out of set (v1...v10000) in CheckVarIndex."); RETURN(0) }
			break;
		case 4: // w1...100
			if((vi<1)||(vi>200)){ MError2("Var is out of set (w1...w200) in CheckVarIndex."); RETURN(0) }
			break;
		case 5: // x1...16
			if((vi<1)||(vi>16)){ MError2("Var is out of set (x1...x16) in CheckVarIndex."); RETURN(0) }
			break;
		case 6: // y1...100
			if((vi<-100)||(vi==0)||(vi>100)){ MError2("Var is out of set (y-100...y-1,y1...y100) as index in CheckVarIndex."); RETURN(0) }
			break;
		case 7: // z1...500
			if (allowBigZ)
			{
				if(BAD_INDEX_LZ(vi)){ MError2("Var is out of set (z-10...z-1,z1...z1000+) in CheckVarIndex."); RETURN(0) }
			}
			else
			{
				if(BAD_INDEX_LZ(vi)||(vi>1000)){ MError2("Var is out of set (z-10...z-1,z1...z1000) in CheckVarIndex."); RETURN(0) }
			}
			break;
		case 8: // e1...100
			if((vi<-100)||(vi==0)||(vi>100)){ MError2("Var is out of set (e-100...e-1,e1...e100) in CheckVarIndex."); RETURN(0) }
			break;
		default:
			MError2("Incorrect variable in CheckVarIndex."); RETURN(0)
	}
	RETURN(1)
}

int * GetVarAddress(int vi, int vtype)
{
	if (CheckVarIndex(vi, vtype, true) == 0)
		return 0;

	switch(vtype){
		case 2: // f...t
			return &ERMVar[vi-1];
		case 3: // v1...1000
			return &ERMVar2[vi-1];
		case 4: // w1...100
			return &ERMVarH[ERMW][vi-1];
		case 5: // x1...16
			return &ERMVarX[vi-1];
		case 6: // y1...100, y-1...-100
			if(vi<0) return &ERMVarYT[-vi-1];
			else     return &ERMVarY[vi-1];
		default:
			return 0;
	}
}

int GetVarIndex(VarNum *vnp, bool allowBigZ)
{
	STARTNA(__LINE__, 0)
	int it = vnp->IType;
	int vi = vnp->Num;
	if (it != 8 && it != 7 && CheckVarIndex(vi, it, false) == 0)
		RETURN(0)

	if (it)
	{
		int *p;
		p = GetVarAddress(vi, it);
		if (p == 0) { MError2("Incorrect index variable in GetVarIndex."); RETURN(0) }
		vi = *p;
	}
	RETURN( CheckVarIndex(vi, vnp->Type, allowBigZ) ? vi : 0 )
}

int GetVarVal(VarNum *vnp)
{
	STARTNA(__LINE__, 0)
	int *p;
	int vi = GetVarIndex(vnp, false);
	if(vnp->Type != 0 && vi == 0) RETURN(0)
	switch(vnp->Type){
		case 0: // number
		case 7: // z1...500
			// возвращаем сам индекс
			break;
		case 8: // e1...100
			if(vi<0) vi=(int)ERMVarFT[-vi-1];
			else     vi=(int)ERMVarF[vi-1];
			break;
		default:
			p = GetVarAddress(vi, vnp->Type);
			if(p == 0) { MError2("Incorrect Var GetVarVal."); RETURN(0) }
			vi = *p;
	}
	RETURN(vi)
}

// 0 means error, 1 means ok
int SetVarVal(VarNum *vnp,int Val)
{
	STARTNA(__LINE__, 0)
	char *txt;
	int *p;
	int vi = GetVarIndex(vnp, true);
	if(vnp->Type != 0 && vi == 0) RETURN(0)
	switch(vnp->Type){
		case 7: // z-10..z-1,z1...1000
			if(BAD_INDEX_LZ(Val)){ MError2("Var z is out of set (-10...-1,1...1000+) as source in SetVarVal."); RETURN(0) }
			if(Val>1000) txt=ERM2String(StringSet::GetText(Val),1,0);
			else if(Val<0) txt=ERMLString[-Val-1];
			else txt=ERMString[Val-1];
			if(vi>0) StrCopy(ERMString[vi-1],512,txt);
			else     StrCopy(ERMLString[-vi-1],512,txt);
			break;
		case 8: // e1...100
			if(vi<0) ERMVarFT[-vi-1]=(float)Val;
			else     ERMVarF[vi-1]=(float)Val;
			break;
		default:
			p = GetVarAddress(vi, vnp->Type);
			if(p == 0) { MError2("Incorrect Var SetVarVal."); RETURN(0) }
			*p = Val;
	}
	RETURN(1)
}

float GetVarValF(VarNum *vnp)
{
	STARTNA(__LINE__, 0)
	if (vnp->Type != 8)
		RETURN((float)GetVarVal(vnp))
	int vi = GetVarIndex(vnp, false);
	if(vi == 0) RETURN(0)

	if(vi<0) RETURN(ERMVarFT[-vi-1])
	else     RETURN(ERMVarF[vi-1])
}
int SetVarValF(VarNum *vnp,float Val)
{
	STARTNA(__LINE__, 0)
	if(vnp->Type != 8)
		RETURN(SetVarVal(vnp, (int)Val))
	int vi = GetVarIndex(vnp, true);
	if(vi == 0) RETURN(0)

	if(vi<0) ERMVarFT[-vi-1]=Val;
	else     ERMVarF[vi-1]=Val;
	RETURN(1)
}
////////////////////////////////////
// хождение по воде
static int SW_Set;
void StepWater(int x,int y,int l)
{
	STARTNA(__LINE__, 0)
	_MapItem_ *MIp;
	MIp=GetMapItem(x,y,l);
	if(SW_Set){
		MIp->Land=4; // болото
		PlaceObject(x,y,l,208,1,208,1,-1);
	}else{
//    DelObject(x,y,l); // не работает - удалим все, что на этой клетке
		DelFirstObjectPic(x,y,l);
		MIp->Land=8; // вода
	}
	RETURNV
}

void PathOverWater(int x1,int y1,int x2,int y2,int l)
{
	STARTNA(__LINE__, 0)
	MakeLine(x1,y1,x2,y2,l,StepWater);
	RETURNV
}
////////////////////////////////////
int CheckPlace4Hero(_MapItem_ *mip,Dword **stp)
{
	STARTNA(__LINE__, 0)
	int tp=mip->OType;
	if(tp==0x22){ // герой стоит на объекте
		int v=*(int *)mip;
		_Hero_ *hp=GetHeroStr(v);
		tp=hp->PlOType;
		if(stp!=0) *stp=&hp->PlSetUp;
	}
	RETURN(tp)
}

int IsObjectType(_MapItem_ *mip,int t,int st)
{
	STARTNA(__LINE__, 0)
	int stp=mip->OSType,tp=mip->OType;
	Byte at=mip->Attrib;
	if(tp==0x22){ // герой стоит на объекте
		int v=*(int *)mip;
		_Hero_ *hp=GetHeroStr(v);
		tp=hp->PlOType;
//    stp=hp->PlOSType;
//    at=(Byte)(hp->Pl0Cflag>>8);
	}
	if(tp==t){
		if(at&0x10){
			if(st==-1){ RETURN(1) }
			else{ if(stp==st){ RETURN(1) } }
		}else{ // 3.58f Pleced Events fix
			if(tp==26){ // local event
				if(st==-1){ RETURN(1) }
				else{ if(stp==st){ RETURN(1) } }
			}
		}
	}
	RETURN(0)
}
int CalcObjects(int t,int st)
{
	STARTNA(__LINE__, 0)
	int x,y,l,n,v,v2;
	_MapItem_ *mp;
	n=0; v=GetMapSize(); v2=GetMapLevels();
	for(l=0;l<=v2;l++){
		for(y=0;y<v;y++){
			for(x=0;x<v;x++){
				mp=GetMapItem(x,y,l);
				if(IsObjectType(mp,t,st)) ++n;
/*
				if(mp->OType==t){
					if(mp->Attrib&0x10){
						if(st==-1){ ++n; }
						else{ if(mp->OSType==st){ ++n; } }
					}
				}
*/
			}
		}
	}
	RETURN(n)
}
int FindObjects(int t,int st,int i,int *xv,int *yv,int *lv)
{
	STARTNA(__LINE__, 0)
	int x,y,l,n,m,v,v2;
	_MapItem_ *mp/*,*mp0*/;
	m=CalcObjects(t,st);
	if(m==0) RETURN(-1)
	v=GetMapSize(); v2=GetMapLevels();
	n=0;
//  mp0=GetMapItem(0,0,0); // 3.58f 
	for(l=0;l<=v2;l++){
		for(y=0;y<v;y++){
			for(x=0;x<v;x++){
				mp=GetMapItem(x,y,l);
//        mp=&mp0[x+(y+l*v)*v];
				if(IsObjectType(mp,t,st)==0) continue;
/*
				if(mp->OType==t){
					if(mp->Attrib&0x10){
						if(st==-1){ ++n; }
						else{ if(mp->OSType==st){ ++n; } }
					}
				}
*/
				++n;
				if(n>m) RETURN(-1)
				if(n==i){
					*xv=x; *yv=y; *lv=l;
					RETURN(0)
				}
			}
		}
	}
	RETURN(-1)
}
int FindNextObjects(int t,int st,int *xv,int *yv,int *lv,int direction)
{
	STARTNA(__LINE__, 0)
	int x,y,l/*,m*/,v,v2,fl=0;
	_MapItem_ *mp,*mp0;
//  m=CalcObjects(t,st);
//  if(m==0) RETURN(-1)
	v=GetMapSize(); v2=GetMapLevels();
	mp0=GetMapItem(0,0,0); // 3.58f
	int sx,sy,sl;
	if(*xv==-1){ sx=0; sy=0; sl=0; } // serch from start
	else if(*xv==-2){ sx=v-1; sy=v-1; sl=v2; } // search from end
	else { 
		sx=*xv; sy=*yv; sl=*lv; 
		if(direction==-2) --sx; else ++sx;
	}
	
	if(direction==-2){ //backward
		for(l=v2;l>=0;l--){
			for(y=v-1;y>=0;y--){
				for(x=v-1;x>=0;x--){
					if(fl==0){
						x=sx; y=sy;l=sl;
						if(x<0){ --y; x=v-1; }
						if(y<0){ --l; y=v-1; }
						if(l<0){ RETURN(-1) }
						fl=1;
					}
					mp=&mp0[x+(y+l*v)*v];
					if(IsObjectType(mp,t,st)==0) continue;
					*xv=x; *yv=y; *lv=l;
					RETURN(0)
				}
			}
		}
	}else{ // forward
		for(l=0;l<=v2;l++){
			for(y=0;y<v;y++){
				for(x=0;x<v;x++){
					if(fl==0){
						x=sx; y=sy;l=sl;
						if(x>=v){ ++y; x=0; }
						if(y>=v){ ++l; y=0; }
						if(l>v2){ RETURN(-1) }
						fl=1;
					}
					mp=&mp0[x+(y+l*v)*v];
					if(IsObjectType(mp,t,st)==0) continue;
					*xv=x; *yv=y; *lv=l;
					RETURN(0)
				}
			}
		}
	}
	RETURN(-1)
}

void ClearVarUsedList(void)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<20;i++)    ERMVarUsed[i]=0;
	for(i=0;i<VAR_COUNT_V;i++) ERMVar2Used[i]=0;
	for(i=0;i<200;i++)   ERMVarHUsed[i]=0;
	for(i=0;i<1000;i++)  ERMStringUsed[i]=0;
	for(i=0;i<1000;i++)  ERMFlagsUsed[i]=0;
	for(i=0;i<30000;i++) ERMFunUsed[i]=0;
	for(i=0;i<100;i++)   ERMTimerUsed[i]=0;
	RETURNV
}
void StartVarUsed(void){ ERMVarUsedStore=1; }
void StopVarUsed(void){ ERMVarUsedStore=0; }

void StartDblVarUsed(void){ ERMVarCheck4DblStore=1; }
void StopDblVarUsed(void){ ERMVarCheck4DblStore=0; }

char VarUsedLog[300000]; // 300 kb
void WriteVarUsed(void)
{
	STARTNA(__LINE__, 0)
	int i,len;
	char *cp=VarUsedLog;
	char buf[128];

	StrCopy(cp,64,"Common rules for cross-reverence:\r\n - used in both parts [*].\r\n"); cp+=63;
	StrCopy(cp,109,"Common rules for Flags, v,w,z vars:\r\n - used as parameter [p];\r\n - in & section [&];\r\n - in | section [|].\r\n"); cp+=108;
	StrCopy(cp,55,"Single letter variables used in the left part [f...t]:"); cp+=54;
	*cp++=0x0D; *cp++=0x0A;
	for(i=0;i<20;i++){
		if(ERMVarUsed[i]==0) continue;
		*cp++=(char)('f'+i); *cp++=' ';
		if(ERMVarUsed[i]&1) *cp++='p';
		if(ERMVarUsed[i]&2) *cp++='&';
		if(ERMVarUsed[i]&4) *cp++='|';
		if(ERMVarUsed[i]&0x40){ *cp++=' '; *cp++='*'; }
		*cp++=0x0D; *cp++=0x0A;
	}
	StrCopy(cp,40,"Flags used in the left part [1...1000]:"); cp+=39;
	*cp++=0x0D; *cp++=0x0A;
	for(i=0;i<1000;i++){
		if(ERMFlagsUsed[i]==0) continue;
		len=i2a(i+1,buf); buf[len]=0;
		StrCopy(cp,len+1,buf); cp+=len; *cp++=' ';
		if(ERMFlagsUsed[i]&1) *cp++='p';
		if(ERMFlagsUsed[i]&2) *cp++='&';
		if(ERMFlagsUsed[i]&4) *cp++='|';
		if(ERMFlagsUsed[i]&0x40){ *cp++=' '; *cp++='*'; }
		*cp++=0x0D; *cp++=0x0A;
	}
	StrCopy(cp,44,"v vars used in the left part [v1...v10000]:"); cp+=43;
	*cp++=0x0D; *cp++=0x0A;
	for(i=0;i<VAR_COUNT_V;i++){
		if(ERMVar2Used[i]==0) continue;
		*cp++='v';
		len=i2a(i+1,buf); buf[len]=0;
		StrCopy(cp,len+1,buf); cp+=len; *cp++=' ';
		if(ERMVar2Used[i]&1) *cp++='p';
		if(ERMVar2Used[i]&2) *cp++='&';
		if(ERMVar2Used[i]&4) *cp++='|';
		if(ERMVar2Used[i]&0x40){ *cp++=' '; *cp++='*'; }
		*cp++=0x0D; *cp++=0x0A;
	}
	StrCopy(cp,42,"w vars used in the left part [w1...w200]:"); cp+=41;
	*cp++=0x0D; *cp++=0x0A;
	for(i=0;i<200;i++){                                 
		if(ERMVarHUsed[i]==0) continue;
		*cp++='w';
		len=i2a(i+1,buf); buf[len]=0;
		StrCopy(cp,len+1,buf); cp+=len; *cp++=' ';
		if(ERMVarHUsed[i]&1) *cp++='p';
		if(ERMVarHUsed[i]&2) *cp++='&';
		if(ERMVarHUsed[i]&4) *cp++='|';
		if(ERMVarHUsed[i]&0x40){ *cp++=' '; *cp++='*'; }
		*cp++=0x0D; *cp++=0x0A;
	}
	StrCopy(cp,43,"z vars used in the left part [z1...z1000]:"); cp+=42;
	*cp++=0x0D; *cp++=0x0A;
	for(i=0;i<1000;i++){
		if(ERMStringUsed[i]==0) continue;
		*cp++='z';
		len=i2a(i+1,buf); buf[len]=0;
		StrCopy(cp,len+1,buf); cp+=len; *cp++=' ';
		if(ERMStringUsed[i]&1) *cp++='p';
		if(ERMStringUsed[i]&2) *cp++='&';
		if(ERMStringUsed[i]&4) *cp++='|';
		if(ERMStringUsed[i]&0x40){ *cp++=' '; *cp++='*'; }
		*cp++=0x0D; *cp++=0x0A;
	}
	StrCopy(cp,66,"Timers triggers [t] and receivers/instructions [r] [TM1...TM100]:"); cp+=65;
	*cp++=0x0D; *cp++=0x0A;
	for(i=0;i<100;i++){
		if(ERMTimerUsed[i]==0) continue;
		*cp++='T'; *cp++='M';
		len=i2a(i+1,buf); buf[len]=0;
		StrCopy(cp,len+1,buf); cp+=len; *cp++=' ';
		if(ERMTimerUsed[i]&1) *cp++='t';
		if(ERMTimerUsed[i]&2) *cp++='r';
		if(ERMTimerUsed[i]&0x40){ *cp++=' '; *cp++='*'; }
		*cp++=0x0D; *cp++=0x0A;
	}
	StrCopy(cp,81,"Functions triggers [t] and receivers/instructions [r,d] [FU(DO)1...FU(DO)30000]:"); cp+=80;
	*cp++=0x0D; *cp++=0x0A;
	for(i=0;i<30000;i++){
		if(ERMFunUsed[i]==0) continue;
		*cp++='F'; *cp++='U';
		len=i2a(i+1,buf); buf[len]=0;
		StrCopy(cp,len+1,buf); cp+=len; *cp++=' ';
		if(ERMFunUsed[i]&1) *cp++='t';
		if(ERMFunUsed[i]&2) *cp++='r';
		if(ERMFunUsed[i]&4) *cp++='d';
		if(ERMFunUsed[i]&0x40){ *cp++=' '; *cp++='*'; }
		*cp++=0x0D; *cp++=0x0A;
	}
	if(SaveSetupState("ERMVarsUsed.LOG",VarUsedLog,strlen(VarUsedLog))){
		Message(/*ITxt(125,1,&WoGTexts)*/"Cannot write LOG file",1);
	}
	RETURNV
}
int LogERMVarNumParse(VarNum *vn,char w,int flag=ERMVarCheck4DblStore)
{
	STARTNA(__LINE__, 0)
	int j;
	switch(vn->IType){ // indexed
			case 0: //=нет,
				switch(vn->Type){ // тип перем
					case 0: //=число,
						break;
					case 1: //=флаг,
						j=vn->Num;
						if((j<1)||(j>1000)){ Message("Cannot LOG index of flag",1); RETURN(1) }
						if(flag) if(ERMFlagsUsed[j-1]!=0) ERMFlagsUsed[j-1]|=0x40;
						ERMFlagsUsed[j-1]|=w;
						break;
					case 2: //=f...t,
						j=vn->Num;
						if((j<1)||(j>20)){ Message("Cannot LOG index of f...t var",1); RETURN(1) }
						if(flag) if(ERMVarUsed[j-1]!=0) ERMVarUsed[j-1]|=0x40;
						ERMVarUsed[j-1]|=w;
						break;
					case 3: //=v1...10000,
						j=vn->Num;
						if((j<1)||(j>VAR_COUNT_V)){ Message("Cannot LOG index of v var",1); RETURN(1) }
						if(flag) if(ERMVar2Used[j-1]!=0) ERMVar2Used[j-1]|=0x40;
						ERMVar2Used[j-1]|=w;
						break;
					case 4: //=w1...100,
						j=vn->Num;
						if((j<1)||(j>200)){ Message("Cannot LOG index of w var",1); RETURN(1) }
						if(flag) if(ERMVarHUsed[j-1]!=0) ERMVarHUsed[j-1]|=0x40;
						ERMVarHUsed[j-1]|=w;
						break;
					case 5: //=x1...100,
						break;
					case 6: //=y1...100
						break;
					case 7: //=z1...500
						j=vn->Num;
						if(j>1000) break;
						if((j<-VAR_COUNT_LZ)||(j==0)){ Message("Cannot LOG index of z var",1); RETURN(1) }
						if(j<1) break;
						if(flag) if(ERMStringUsed[j-1]!=0) ERMStringUsed[j-1]|=0x40;
						ERMStringUsed[j-1]|=w;
						break;
				}
				break;
			case 1: //=флаг,
				j=vn->Num;
				if((j<1)||(j>1000)){ Message("Cannot LOG index of flag [indexed???]",1); RETURN(1) }
				if(flag) if(ERMFlagsUsed[j-1]!=0) ERMFlagsUsed[j-1]|=0x40;
				ERMFlagsUsed[j-1]|=w;
				break;
			case 2: //=f...t,
				j=vn->Num;
				if((j<1)||(j>20)){ Message("Cannot LOG index of f...t var [indirected]",1); RETURN(1) }
				if(flag) if(ERMVarUsed[j-1]!=0) ERMVarUsed[j-1]|=0x40;
				ERMVarUsed[j-1]|=w;
				break;
			case 3: //=v1...1000,
				j=vn->Num;
				if((j<1)||(j>VAR_COUNT_V)){ Message("Cannot LOG index of v var [indirected]",1); RETURN(1) }
				if(flag) if(ERMVar2Used[j-1]!=0) ERMVar2Used[j-1]|=0x40;
				ERMVar2Used[j-1]|=w;
				break;
			case 4: //=w1...100,
				j=vn->Num;
				if((j<1)||(j>200)){ Message("Cannot LOG index of w var [indirected]",1); RETURN(1) }
				if(flag) if(ERMVarHUsed[j-1]!=0) ERMVarHUsed[j-1]|=0x40;
				ERMVarHUsed[j-1]|=w;
				break;
			case 5: //=x1...100,
				break;
			case 6: //=y1...100
				break;
		}
	RETURN(0)
}
void LogERMVarUsed(_ToDo_ *dp,char *Line)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<dp->ParSet;i++){
		if(LogERMVarNumParse(&dp->Par[i],1)) Message(Line,1);
	}
	for(i=0;i<16;i++){
		if(LogERMVarNumParse(&dp->Efl[0][i][0],2)) Message(Line,1); // & left
		if(LogERMVarNumParse(&dp->Efl[0][i][1],2)) Message(Line,1); // & right
		if(LogERMVarNumParse(&dp->Efl[1][i][0],4)) Message(Line,1); // | left
		if(LogERMVarNumParse(&dp->Efl[1][i][1],4)) Message(Line,1); // | right
	}
	RETURNV
}
void LogTriggerConditions(_Cmd_ *cp,char *Line)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<16;i++){
		if(LogERMVarNumParse(&cp->Efl[0][i][0],2)) Message(Line,1); // & left
		if(LogERMVarNumParse(&cp->Efl[0][i][1],2)) Message(Line,1); // & right
		if(LogERMVarNumParse(&cp->Efl[1][i][0],4)) Message(Line,1); // | left
		if(LogERMVarNumParse(&cp->Efl[1][i][1],4)) Message(Line,1); // | right
	}
	RETURNV
}
void LogERMFunctionTrigger(int fn,char *Line,int flag=ERMVarCheck4DblStore) // 1...30000
{
	STARTNA(__LINE__, 0)
	if((fn<1)||(fn>30000)){
		Message("Cannot LOG index of function [trigger]",1);
		Message(Line,1);
		RETURNV
	}
	if(flag) if(ERMFunUsed[fn-1]!=0) ERMFunUsed[fn-1]|=0x40;
	ERMFunUsed[fn-1]|=1; // trigger
	RETURNV
}
void LogERMTimerTrigger(int tn,char *Line,int flag=ERMVarCheck4DblStore) // 1...100
{
	STARTNA(__LINE__, 0)
	if((tn<1)||(tn>100)){
		Message("Cannot LOG index of timer [trigger]",1);
		Message(Line,1);
		RETURNV
	}
	if(flag) if(ERMTimerUsed[tn-1]!=0) ERMTimerUsed[tn-1]|=0x40;
	ERMTimerUsed[tn-1]|=1; // trigger
	RETURNV
}
void LogERMAnyReceiver(Word Id,VarNum Par,char * /*Line*/,int flag=ERMVarCheck4DblStore) // any trigger switched
{
	STARTNA(__LINE__, 0)
	int ind;
//  ind=GetVarVal(&Par);
	switch(Par.IType){ // indexed
		case 0: //=нет,
			switch(Par.Type){ // тип перем
				case 0: //=число,
					ind=Par.Num;
					break;
				default:
					ind=-1;
					break;
			}
			break;
		default:
			ind=-1;
			break;
	}
	switch(Id){
		case 'UF':// VC 'FU':
			if((ind<1)||(ind>30000)){
//        MError("Function receiver or instruction cannot be logged (variable value is not defined yet).");
//        Message(Line,1);
//        просто пропустем
				break;
			}
			if(flag) if(ERMFunUsed[ind-1]!=0) ERMFunUsed[ind-1]|=0x40;
			ERMFunUsed[ind-1]|=2; // receiver or instruction
			break;
		case 'OD':// VC 'DO':
			if((ind<1)||(ind>30000)){
//        MError("Cycle receiver or instruction cannot be logged (variable value is not defined yet).");
//        Message(Line,1);
//        просто пропустем
				break;
			}
			if(flag) if(ERMFunUsed[ind-1]!=0) ERMFunUsed[ind-1]|=0x40;
			ERMFunUsed[ind-1]|=4; // receiver or instruction
			break;
		case 'MT':// VC 'TM':
			if((ind<1)||(ind>100)){
//        MError("Timer receiver or instruction cannot be logged (variable value is not defined yet).");
//        Message(Line,1);
//        просто пропустем
				break;
			}
			if(flag) if(ERMTimerUsed[ind-1]!=0) ERMTimerUsed[ind-1]|=0x40;
			ERMTimerUsed[ind-1]|=2; // receiver or instruction
			break;
	}
	RETURNV
}
int ERM_VarControl(char Cmd,int /*Num*/,_ToDo_*,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	switch(Cmd){
		case 'C': // очистить список
			ClearVarUsedList();
			break;
		case 'B': // начать собирать переменные
			StartVarUsed();
			break;
		case 'E': // приостановить сбор переменных
			StopVarUsed();
			break;
		case 'Y': // проверять на дублирующие
			StartDblVarUsed();
			break;
		case 'N': // проверять на дублирующие
			StopDblVarUsed();
			break;
		case 'W': // записать дамп в файл
			WriteVarUsed();
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}


const int TextConstCount = 2933;
char** TextConst = (char**)0x6A5338;
int TextConstVars[TextConstCount];
char* TextConstBackup[TextConstCount];
int TextConstBackupDone;
char TextConstValid[TextConstCount];

void _TextFiles_SetValid()
{
	for (int i = 0; i < TextConstCount; i++)  TextConstValid[i] = 1;
	TextConstValid[22] = 0; // Ё-d
	TextConstValid[52] = 0; // (not a pointer)
	TextConstValid[53] = 0; // (zero pointer)
	TextConstValid[64] = 0; // Ё-d
	TextConstValid[314] = 0; // Ё-d
	TextConstValid[357] = 0; // (zero pointer)
	TextConstValid[445] = 0; // (zero pointer)
	TextConstValid[597] = 0; // ґ-d
	TextConstValid[605] = 0; // (not a pointer)
	TextConstValid[660] = 0; // (not a pointer)
	TextConstValid[674] = 0; // (not a pointer)
	TextConstValid[675] = 0; // Ё-d
	TextConstValid[720] = 0; // (zero pointer)
	TextConstValid[740] = 0; // ґ-d
	TextConstValid[759] = 0; // Ё-d
	TextConstValid[811] = 0; // Ё-d
	TextConstValid[860] = 0; // ґ-d
	TextConstValid[861] = 0; // Ё-d
	TextConstValid[1006] = 0; // ґ-d
	TextConstValid[1007] = 0; // Ё-d
	TextConstValid[1008] = 0; // Ё-d
	TextConstValid[1013] = 0; // (not a pointer)
	TextConstValid[1190] = 0; // ґ-d
	TextConstValid[1191] = 0; // (zero pointer)
	TextConstValid[1282] = 0; // (zero pointer)
	TextConstValid[1283] = 0; // (zero pointer)
	TextConstValid[1284] = 0; // (zero pointer)
	TextConstValid[1285] = 0; // (zero pointer)
	TextConstValid[1286] = 0; // (zero pointer)
	TextConstValid[1287] = 0; // (zero pointer)
	TextConstValid[1288] = 0; // (zero pointer)
	TextConstValid[1289] = 0; // (zero pointer)
	TextConstValid[1290] = 0; // (zero pointer)
	TextConstValid[1291] = 0; // (zero pointer)
	TextConstValid[1292] = 0; // (zero pointer)
	TextConstValid[1293] = 0; // (zero pointer)
	TextConstValid[1294] = 0; // (zero pointer)
	TextConstValid[1295] = 0; // (zero pointer)
	TextConstValid[1296] = 0; // (zero pointer)
	TextConstValid[1297] = 0; // (zero pointer)
	TextConstValid[1298] = 0; // (zero pointer)
	TextConstValid[1299] = 0; // (zero pointer)
	TextConstValid[1300] = 0; // (zero pointer)
	TextConstValid[1301] = 0; // (zero pointer)
	TextConstValid[1302] = 0; // (zero pointer)
	TextConstValid[1303] = 0; // (zero pointer)
	TextConstValid[1304] = 0; // (zero pointer)
	TextConstValid[1305] = 0; // (zero pointer)
	TextConstValid[1306] = 0; // (zero pointer)
	TextConstValid[1307] = 0; // (zero pointer)
	TextConstValid[1308] = 0; // (zero pointer)
	TextConstValid[1309] = 0; // (zero pointer)
	TextConstValid[1310] = 0; // (zero pointer)
	TextConstValid[1311] = 0; // (zero pointer)
	TextConstValid[1312] = 0; // (zero pointer)
	TextConstValid[1313] = 0; // (zero pointer)
	TextConstValid[1314] = 0; // (zero pointer)
	TextConstValid[1315] = 0; // (zero pointer)
	TextConstValid[1316] = 0; // (zero pointer)
	TextConstValid[1317] = 0; // (zero pointer)
	TextConstValid[1318] = 0; // (zero pointer)
	TextConstValid[1319] = 0; // (zero pointer)
	TextConstValid[1320] = 0; // (zero pointer)
	TextConstValid[1321] = 0; // (zero pointer)
	TextConstValid[1322] = 0; // (zero pointer)
	TextConstValid[1323] = 0; // (zero pointer)
	TextConstValid[1324] = 0; // (zero pointer)
	TextConstValid[1325] = 0; // (zero pointer)
	TextConstValid[1326] = 0; // (zero pointer)
	TextConstValid[1327] = 0; // (zero pointer)
	TextConstValid[1328] = 0; // (zero pointer)
	TextConstValid[1329] = 0; // (zero pointer)
	TextConstValid[1330] = 0; // (zero pointer)
	TextConstValid[1331] = 0; // (zero pointer)
	TextConstValid[1332] = 0; // (zero pointer)
	TextConstValid[1333] = 0; // (zero pointer)
	TextConstValid[1334] = 0; // (zero pointer)
	TextConstValid[1335] = 0; // (zero pointer)
	TextConstValid[1336] = 0; // (zero pointer)
	TextConstValid[1337] = 0; // (zero pointer)
	TextConstValid[1338] = 0; // (zero pointer)
	TextConstValid[1339] = 0; // (zero pointer)
	TextConstValid[1340] = 0; // (zero pointer)
	TextConstValid[1341] = 0; // (zero pointer)
	TextConstValid[1342] = 0; // (zero pointer)
	TextConstValid[1343] = 0; // (zero pointer)
	TextConstValid[1344] = 0; // (zero pointer)
	TextConstValid[1345] = 0; // (zero pointer)
	TextConstValid[1346] = 0; // (zero pointer)
	TextConstValid[1347] = 0; // (zero pointer)
	TextConstValid[1348] = 0; // (zero pointer)
	TextConstValid[1349] = 0; // (zero pointer)
	TextConstValid[1350] = 0; // (zero pointer)
	TextConstValid[1351] = 0; // (zero pointer)
	TextConstValid[1352] = 0; // (zero pointer)
	TextConstValid[1353] = 0; // (zero pointer)
	TextConstValid[1354] = 0; // (zero pointer)
	TextConstValid[1355] = 0; // (zero pointer)
	TextConstValid[1356] = 0; // (zero pointer)
	TextConstValid[1357] = 0; // (zero pointer)
	TextConstValid[1358] = 0; // (zero pointer)
	TextConstValid[1359] = 0; // (zero pointer)
	TextConstValid[1360] = 0; // (zero pointer)
	TextConstValid[1361] = 0; // (zero pointer)
	TextConstValid[1362] = 0; // (zero pointer)
	TextConstValid[1363] = 0; // (zero pointer)
	TextConstValid[1364] = 0; // (zero pointer)
	TextConstValid[1365] = 0; // (zero pointer)
	TextConstValid[1366] = 0; // (zero pointer)
	TextConstValid[1367] = 0; // (zero pointer)
	TextConstValid[1368] = 0; // (zero pointer)
	TextConstValid[1369] = 0; // (zero pointer)
	TextConstValid[1370] = 0; // (zero pointer)
	TextConstValid[1371] = 0; // (zero pointer)
	TextConstValid[1372] = 0; // (zero pointer)
	TextConstValid[1373] = 0; // (zero pointer)
	TextConstValid[1374] = 0; // (zero pointer)
	TextConstValid[1375] = 0; // (zero pointer)
	TextConstValid[1376] = 0; // (zero pointer)
	TextConstValid[1377] = 0; // (zero pointer)
	TextConstValid[1378] = 0; // (zero pointer)
	TextConstValid[1379] = 0; // (zero pointer)
	TextConstValid[1380] = 0; // (zero pointer)
	TextConstValid[1381] = 0; // (zero pointer)
	TextConstValid[1382] = 0; // (zero pointer)
	TextConstValid[1383] = 0; // (zero pointer)
	TextConstValid[1384] = 0; // (zero pointer)
	TextConstValid[1385] = 0; // (zero pointer)
	TextConstValid[1386] = 0; // (zero pointer)
	TextConstValid[1387] = 0; // (zero pointer)
	TextConstValid[1388] = 0; // (zero pointer)
	TextConstValid[1389] = 0; // (zero pointer)
	TextConstValid[1390] = 0; // (zero pointer)
	TextConstValid[1391] = 0; // (zero pointer)
	TextConstValid[1392] = 0; // (zero pointer)
	TextConstValid[1393] = 0; // (zero pointer)
	TextConstValid[1394] = 0; // (zero pointer)
	TextConstValid[1395] = 0; // (zero pointer)
	TextConstValid[1396] = 0; // (zero pointer)
	TextConstValid[1397] = 0; // (zero pointer)
	TextConstValid[1398] = 0; // (zero pointer)
	TextConstValid[1399] = 0; // (zero pointer)
	TextConstValid[1400] = 0; // (zero pointer)
	TextConstValid[1401] = 0; // (zero pointer)
	TextConstValid[1402] = 0; // (zero pointer)
	TextConstValid[1403] = 0; // (zero pointer)
	TextConstValid[1404] = 0; // (zero pointer)
	TextConstValid[1405] = 0; // (zero pointer)
	TextConstValid[1406] = 0; // (zero pointer)
	TextConstValid[1407] = 0; // (zero pointer)
	TextConstValid[1408] = 0; // (zero pointer)
	TextConstValid[1409] = 0; // (zero pointer)
	TextConstValid[1410] = 0; // (zero pointer)
	TextConstValid[1411] = 0; // (zero pointer)
	TextConstValid[1412] = 0; // (zero pointer)
	TextConstValid[1413] = 0; // (zero pointer)
	TextConstValid[1414] = 0; // (zero pointer)
	TextConstValid[1415] = 0; // (zero pointer)
	TextConstValid[1416] = 0; // (zero pointer)
	TextConstValid[1417] = 0; // (zero pointer)
	TextConstValid[1418] = 0; // (zero pointer)
	TextConstValid[1419] = 0; // (zero pointer)
	TextConstValid[1420] = 0; // (zero pointer)
	TextConstValid[1421] = 0; // (zero pointer)
	TextConstValid[1422] = 0; // (zero pointer)
	TextConstValid[1423] = 0; // (zero pointer)
	TextConstValid[1424] = 0; // (zero pointer)
	TextConstValid[1425] = 0; // (zero pointer)
	TextConstValid[1426] = 0; // (zero pointer)
	TextConstValid[1427] = 0; // (zero pointer)
	TextConstValid[1428] = 0; // (zero pointer)
	TextConstValid[1429] = 0; // (zero pointer)
	TextConstValid[1430] = 0; // (zero pointer)
	TextConstValid[1431] = 0; // (zero pointer)
	TextConstValid[1432] = 0; // (zero pointer)
	TextConstValid[1433] = 0; // (zero pointer)
	TextConstValid[1434] = 0; // (zero pointer)
	TextConstValid[1435] = 0; // (zero pointer)
	TextConstValid[1436] = 0; // (zero pointer)
	TextConstValid[1437] = 0; // (zero pointer)
	TextConstValid[1438] = 0; // (zero pointer)
	TextConstValid[1439] = 0; // (zero pointer)
	TextConstValid[1440] = 0; // (zero pointer)
	TextConstValid[1441] = 0; // (zero pointer)
	TextConstValid[1442] = 0; // (zero pointer)
	TextConstValid[1443] = 0; // (zero pointer)
	TextConstValid[1444] = 0; // (zero pointer)
	TextConstValid[1445] = 0; // (zero pointer)
	TextConstValid[1468] = 0; // (not a pointer)
	TextConstValid[1609] = 0; // Ё-d
	TextConstValid[1630] = 0; // Ё-d
	TextConstValid[1631] = 0; // (zero pointer)
	TextConstValid[1642] = 0; // Ё-d
	TextConstValid[1643] = 0; // (zero pointer)
	TextConstValid[2144] = 0; // Ё-d
	TextConstValid[2145] = 0; // (zero pointer)
	TextConstValid[2194] = 0; // ґ-d
	TextConstValid[2195] = 0; // Ё-d
	TextConstValid[2208] = 0; // Ё-d
	TextConstValid[2209] = 0; // (zero pointer)
	TextConstValid[2219] = 0; // (zero pointer)
	TextConstValid[2318] = 0; // Ё-d
	TextConstValid[2319] = 0; // Ё-d
	TextConstValid[2335] = 0; // (zero pointer)
	TextConstValid[2352] = 0; // (not a pointer)
	TextConstValid[2357] = 0; // (not a pointer)
	TextConstValid[2366] = 0; // Ё-d
	TextConstValid[2367] = 0; // (zero pointer)
	TextConstValid[2374] = 0; // ґ-d
	TextConstValid[2735] = 0; // ґ-d
	TextConstValid[2736] = 0; // Ё-d
	TextConstValid[2741] = 0; // (zero pointer)
	TextConstValid[2752] = 0; // (not a pointer)
	TextConstValid[2753] = 0; // (zero pointer)
	TextConstValid[2801] = 0; // (zero pointer)
	TextConstValid[2828] = 0; // Ё-d
	TextConstValid[2829] = 0; // (not a pointer)
}

static Dword GrailEffectsList[9][9] = {
	{0x4DCB4D, 0x74, 0x4E3E49, 0x74, 0},
	{0x4DD31E, 0x74, 0x4E3BAD, 0x74, 0},
	{0x463943, 0x840F, 0x4C61B9, 0x74, 0x4C7A59, 0x74, 0},
	{0x4C85D9, 0x75, 0},
	{0x4E4118, 0x74, 0},
	{0x46399E, 0x840F, 0},
	{0x4639CC, 0x840F, 0},
	{0x463A5A, 0x74, 0},
	{0x5BE50A, 0x74, 0x5CE635, 0x74, 0x5CE842, 0x840F, 0x5D745F, 0x74, 0}
};

//char BUUF[15000000];

int ERM_UniversalEx(char Cmd,int Num,_ToDo_*,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	switch(Cmd)
	{
		case 'K': // K#/?$ get/check/set key state
		{
			CHECK_ParamsNum(2);
			v = (GetKeyState(Mp->n[0]) < 0 ? 1 : 0);
			if (Apply(&v, 4, Mp, 1)) break;
			//keybd_event(Mp->n[0], 0, (v? 0 : KEYEVENTF_KEYUP), 0);
			break;
		}
		case 'M': // M$/$ get/check/set mouse position
		{
			CHECK_ParamsNum(2);
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(MainWindow, &pt);
			int check = Apply(&pt.x, 4, Mp, 0);
			if (Apply(&pt.y, 4, Mp, 1) && !check) break;
			ClientToScreen(MainWindow, &pt);
			SetCursorPos(pt.x, pt.y);
			break;
		}
		case 'T': // T$ GetTickCount
		{
			CHECK_ParamsNum(1);
			v = timeGetTime();
			if (Apply(&v, 4, Mp, 0)) break;
			Message(Format("Time delta: %d", timeGetTime() - v));
			break;
		}

		case 'S': // S#/$ get/check/set standard string
		{ 
			CHECK_ParamsNum(2);
			int n = Mp->n[0];
			if(n < 0 || n >= TextConstCount || TextConstValid[n] == 0){ MError2("wrong index."); RETURN(0) }
			StrMan::Apply(TextConst[n], TextConstBackup[n], TextConstVars[n], Mp, 1);
			break;
		}

		case 'G': // Gtown/$ get/check/set standard grail effect enabled
		{
			CHECK_ParamsNum(2);
			int n = Mp->n[0];
			if(n < 0 || n > 8){ MError2("town type out of range (0...8)."); RETURN(0) }
			Dword* list = GrailEffectsList[n];
			v = (*(unsigned char *)*list == (*(list + 1) & 0xFF));
			if (Apply(&v, 4, Mp, 1)) break;
			while (*list)
			{
				Dword ptr = *list++;
				Dword std = *list++;
				if (std <= 0xFF)
					if (v)
						*(byte*)ptr = std;
					else
						*(byte*)ptr = 0xEB;
				else
					if (v)
						*(Word*)ptr = std;
					else
						*(Word*)ptr = 0xE990;
			}
			break;
		}

		/** /
		case 'A':
		{
			int i = 0;
			BUUF[0] = 0;
			for (int j = 0; j < TextConstCount; j++)
			{
				char* p = TextConst[j];
				//if (p == 0) p = &tmp;
				if ((int)p == 0) p = "(zero pointer)";
				if ((int)p < 0x1000) p = "(not a pointer)";
				sprintf(&BUUF[i = i + strlen(&BUUF[i])], "%i\t%s\r\n", j, p);
				//Message(&BUUF[i], 1);
			}
			SaveSetupState("TX_T.TXT", BUUF, strlen(BUUF));
			break;
		}

		case 'V':
		{
			int i = 0;
			BUUF[0] = 0;
			for (int j = 0; j < TextConstCount; j++)
			{
				char* p = TextConst[j];
				bool store = 0;
				if ((int)p == 0) { p = "(zero pointer)"; store = 1; }
				if ((int)p < 0x1000) { p = "(not a pointer)"; store = 1; }
				if (strlen(p) == 3) store = 1;

				if (store) sprintf(&BUUF[i = i + strlen(&BUUF[i])], "\t\tcase %i: // %s\r\n", j, p);
				//Message(&BUUF[i], 1);
			}
			SaveSetupState("textss.TXT", BUUF, strlen(BUUF));
			break;
		}
		/**/

		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}

static char UN_Buf[32];
int ERM_Universal(char Cmd,int Num,_ToDo_*,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int x,y,l,t,st,srf,t2,st2,v,v2,zi,redraw;
	int *mt,MType,MSType;
	char *str,*str2,*str3;
	_MapItem_ *mp;
	switch(Cmd){
		case 'W': // проложить путь по воде
			CHECK_ParamsMin(5); // #x1/#y1/#x2/#y2/#l
			if(Num==6) SW_Set=Mp->n[5]; else SW_Set=1;
			PathOverWater(Mp->n[0],Mp->n[1],Mp->n[2],Mp->n[3],Mp->n[4]);
			RedrawMap();
			break;
		case 'D': // сделать отдельную клетку проходимой по воде
			CHECK_ParamsMin(3); // #x/#y/#l
			if(Num==4) SW_Set=Mp->n[3]; else SW_Set=1;
			StepWater(Mp->n[0],Mp->n[1],Mp->n[2]);
			RedrawMap();
			break;
		case 'S': // открыть поверхность
			CHECK_ParamsMin(5); // #x/#y/#l/#o/#r
			ShowArea(Mp->n[0],Mp->n[1],Mp->n[2],Mp->n[3],Mp->n[4]);
			break;
		case 'H': // закрыть поверхность
			CHECK_ParamsMin(5); // #x/#y/#l/#o/#r
			HideArea(Mp->n[0],Mp->n[1],Mp->n[2],Mp->n[3],Mp->n[4]);
			break;
		case 'R': // обновление экрана
			switch(Mp->n[0]){
				case 1: RedrawMap(); break;
				case 2: RedrawRes(); break;
				case 3:
					if(Num<2){ MError("\"!!UN:R3\"-insufficient parameters."); RETURN(0) } // #x/#y/#l/#o/#r
					v=Mp->n[1];
					if((v<-1)||(v>=HERNUM)){ MError("\"!!UN:R3\"-wrong hero number."); RETURN(0) } // #x/#y/#l/#o/#r
					if(v==-1) RedrawHeroScreen(ERM_HeroStr);
					else RedrawHeroScreen(GetHeroStr(v));
					break;
				case 4: RedrawTown(); break;
				case 5: // установить вид курсора
					if(Num<3){ MError("\"!!UN:R5\"-insufficient parameters."); RETURN(0) } // #x/#y/#l/#o/#r
					if(Apply(&MType,4,Mp,1)) break;
					if(Apply(&MSType,4,Mp,2)) break;
					SetMouseCursor(MSType,MType);
					break;
				case 6:
					if(Num<2){ MError("\"!!UN:R6\"-insufficient parameters."); RETURN(0) } // #x/#y/#l/#o/#r
					if(Apply(&MType,4,Mp,1)) break;
					DelayIt(MType); // delay for MType msec
					break;
				case 7: // enable/disable show/hide cursor
					if(Num<2){ MError("\"!!UN:R7\"-insufficient parameters."); RETURN(0) } // #x/#y/#l/#o/#r
					if(Apply(&MType,4,Mp,1)) break;
					if(MType){ // hide
						ZHideCursor();
					}else{
						ZShowCursor();
					}
					if(Num>2) Apply(&GLB_DisableMouse,4,Mp,2);
					break;        
				default: MError("\"!!UN:R\"-wrong parameter."); RETURN(0)
			}
			break;
		case 'E': // проверить на доступность размещения
			CHECK_ParamsMin(3); // #x/#y/#l
			mp=GetMapItem(Mp->n[0],Mp->n[1],Mp->n[2]);
			if(mp==0){ MError("\"!!UN:E\"-wrong map position."); RETURN(0) }
			if(EmptyPlace2(mp)==0){ //занято
				ERMFlags[0]=1;
			}else{
				ERMFlags[0]=0;
			}
			break;
		case 'I': // Ix/y/l/t/s,Ix/y/l/t/s/ter, Ix/y/l/t/s/t2/st2/ter - поместить объект
			redraw=1;
			CHECK_ParamsMin(5);
			x=Mp->n[0]; y=Mp->n[1]; l=Mp->n[2];
			t=Mp->n[3]; st=Mp->n[4];
			if(Num==5){ srf=-1; }
			if(Num==6){ srf=Mp->n[5]; if((srf<-1)||(srf>8)){ MError("\"!!UN:I\"-wrong surface type (-1...8)."); RETURN(0) }}
			t2=t; st2=st;
			if(Num>6){ t2=Mp->n[5]; st2=Mp->n[6]; }
			if(Num==7){ srf=-1; }
			if(Num>7) { srf=Mp->n[7]; if((srf<-1)||(srf>8)){ MError("\"!!UN:I\"-wrong surface type (-1...8)."); RETURN(0) }}
			if(Num==9)  redraw=Mp->n[8];
			PlaceObject(x,y,l,t,st,t2,st2,srf);
			if(redraw) RedrawMap();
			break;
		case 'O': // Ox/y/l(t/redr) удалить объект
			redraw=1;
			CHECK_ParamsMin(3);
			if(Num>3) t=Mp->n[3]; else t=0;
			if(Num==5) redraw=Mp->n[4];
			DelObject(Mp->n[0],Mp->n[1],Mp->n[2]);
			if(t!=0) DelObjPos(PosMixed(Mp->n[0],Mp->n[1],Mp->n[2]));
			if(redraw) RedrawMap();
			break;
		case 'F': // Fx/y/l/dx/dy поджечь лес
			CHECK_ParamsNum(5);
			ProvokeAnimation(Mp->n[0],Mp->n[1],Mp->n[2],Mp->n[3],Mp->n[4],1,OAM_AUTO);
			break;
		case 'K': // K#/$ установить неделю...
			CHECK_ParamsMin(2);
			switch(Mp->n[0]){
				case 1: // K1/$ 1-все недели никого, 0-обычно
					Apply(&NextWeekOf,4,Mp,1);
					break;
				case 2: // K2/$ номер строки для вывода (1...500)
					//Apply(&NextWeekMess,4,Mp,1);
					str = StrMan::GetStoredStr(NextWeekMess);
					StrMan::Apply(str, 0, NextWeekMess, Mp, 1);
					break;
				default: MError("\"!!UN:I\"-wrong first parameter."); RETURN(0)
			}
			break;
		case 'M': // M#/$ установить месяц...
			switch(Mp->n[0]){
				case 0: // M0 месяц как обычно
					NextMonthOf=0;
					break;
				case 1: // M1 месяц никого
					NextMonthOf=1;
					break;
				case 2: // M2/$ - месяц монстра*2
					CHECK_ParamsMin(2);
					NextMonthOf=2;
					Apply(&NextMonthOfMonster,4,Mp,1);
					break;
				case 3: // M3 месяц чумы
					NextMonthOf=3;
					break;
//        case 4: // M4/$1/$2 - тип монстра*2 в позиции $1
//          if(Num<3){ Error(); return 0; }
//          if((Mp->n[1]<0)||(Mp->n[1]>11)){ Error(); return 0; }
//          Apply(&NewMonthMonTypes[Mp->n[1]],1,Mp,2);
//          break;
				default: MError("\"!!UN:M\"-wrong first parameter."); RETURN(0)
			}
			break;
		case 'N': // N#/z/subtype/...
			CHECK_ParamsMin(3);
			zi=0; Apply(&zi,4,Mp,1);
			if(BAD_INDEX_LZ(zi)||(zi>1000)){ MError("\"!!UN:N\"-z var index out of range (-10...-1,1...1000)."); RETURN(0) }
			v=-1; Apply(&v,4,Mp,2);
			if(v<0){ MError("\"!!UN:N\"-incorrect type of object (<0)."); RETURN(0) }
			str = GetPureErmString(zi);
			switch(Mp->n[0]){
				case 0: // N0/z/st get name of artefact
					StrCopy(str,512,GetArtName(v));
					break;
				case 1: // N1/z/st get name of spell
					StrCopy(str,512,GetSpellName(v));
					break;
				case 2: // N2/z/st/sst  //  имя строения в определенном городе
					CHECK_ParamsMin(4);
					 v2=-1; Apply(&v2,4,Mp,3);
					if(v2<0){ MError("\"!!UN:N\"-incorrect additioal type of object (<0)."); RETURN(0) }
					StrCopy(str,512,GetBuildName(v,v2));
					break;
				case 3: // N3/z/st/sP  //  имя монстра(ов)
					CHECK_ParamsMin(4);
					 v2=-1; Apply(&v2,4,Mp,3);
					if(v2<0){ MError("\"!!UN:N\"-incorrect additioal type of object (<0)."); RETURN(0) }
					StrCopy(str,512,GetMonName(v,v2));
					break;
				case 4: // N4/z/st get name of secondary skill
					StrCopy(str,512,GetSSkillName(v));
					break;
				case 5: // N5/z/optind save a value to INI file
					if(Num<3){ MError("\"!!UN:N5\"-insufficient parameters."); RETURN(0) }
					if(Num>3){
						zi=0; Apply(&zi,4,Mp,3);
						if(GetErmString(str2, zi)) RETURN(0)
					}else str2="Common";
					if(Num>4){
						zi=0; Apply(&zi,4,Mp,4);
						if(GetErmString(str3, zi)) RETURN(0)
					}else str3=".\\WoG.ini";
					WriteStrINI(str,itoa(v,UN_Buf,10),str2,str3);
					break;
				case 6: // N5/z/optind read a value to INI file
					if(Num<3){ MError("\"!!UN:N6\"-insufficient parameters."); RETURN(0) }
					if(Num>3){
						zi=0; Apply(&zi,4,Mp,3);
						if(GetErmString(str2, zi)) RETURN(0)
					}else str2="Common";
					if(Num>4){
						zi=0; Apply(&zi,4,Mp,4);
						if(GetErmString(str3, zi)) RETURN(0)
					}else str3=".\\WoG.ini";
					ReadStrINI(str,512,str,itoa(v,UN_Buf,10),str2,str3);
					break;
				default: MError("\"!!UN:N\"-wrong first parameter."); RETURN(0)
			}
			break;
		case 'C': // Cadr/syze/data модеф. память
			CHECK_ParamsNum(3);
//      Apply((void *)Mp->n[0],(char)Mp->n[1],Mp,2);
			if(Apply(&v ,4,Mp,0)){ MError("\"!!UN:C\"-get or check syntax is not acceptable."); RETURN(0) }
			if(Apply(&v2,4,Mp,1)){ MError("\"!!UN:C\"-get or check syntax is not acceptable."); RETURN(0) }
			__asm{ 
				mov eax,v 
				mov eax,[eax]
				mov t,eax
			}
			if(Apply(&t,(char)v2,Mp,2)) break;
			switch(v2){
				case 1: __asm{ 
					mov edx,v
					mov eax,t 
					mov [edx], al 
								} 
					break;
				case 2: __asm{ 
					mov edx,v 
					mov eax,t 
					mov [edx], ax 
								} 
					break;
				case 4: __asm{ 
					mov edx,v 
					mov eax,t 
					mov [edx],eax 
								} 
					break;
				default: EWrongSyntax(); RETURN(0)
			}
			break;
		case 'X': // X?x/?l - получить размеры карты
			CHECK_ParamsNum(2);
			if(Mp->VarI[0].Check==0){ MError("\"!!UN:X\"-try to set X(Y)."); RETURN(0) }
			if(Mp->VarI[1].Check==0){ MError("\"!!UN:X\"-try to set L."); RETURN(0) }
			v=GetMapSize();
			v2=GetMapLevels();
			Apply(&v,4,Mp,0);
			Apply(&v2,4,Mp,1);
			break;
		case 'V': // V?wog/?erm - получить версии WoG и ERM
			CHECK_ParamsMin(2);
			if(Mp->VarI[0].Check==0){ MError("\"!!UN:V\"-try to set WoG version."); RETURN(0) }
			if(Mp->VarI[1].Check==0){ MError("\"!!UN:V\"-try to set ERM version."); RETURN(0) }
			v=WOG_VERSION;
			v2=ERM_VERSION;
			Apply(&v,4,Mp,0);
			Apply(&v2,4,Mp,1);
			if(Num>2){
//CHECK_ParamsMin(5);
				if(Mp->VarI[2].Check==0){ MError("\"!!UN:V\"-try to set parameter."); RETURN(0) }
				if(Mp->VarI[3].Check==0){ MError("\"!!UN:V\"-try to set parameter."); RETURN(0) }
				if(Mp->VarI[4].Check==0){ MError("\"!!UN:V\"-try to set parameter."); RETURN(0) }
				__asm{ 
					mov eax,0x6992C4 
					mov eax,[eax] 
					mov v,eax 
				}
				Apply(&v,4,Mp,2);
				__asm{ 
					mov eax,0x69959C
					mov eax,[eax]; 
					mov v,eax; 
				}
				Apply(&v,4,Mp,3);
				__asm{ 
					mov eax,0x698A40; 
					mov eax,[eax]; 
					mov v,eax; 
				}
				Apply(&v,4,Mp,4);
				if(Num>5){
					CHECK_ParamsMin(7);
					__asm{ 
						mov eax,0x698A84; 
						mov eax,[eax]; 
						mov v,eax; 
					}
					Apply(&v,4,Mp,5);
					v=0;
					__asm{
						xor eax,eax
						mov edx,BASE;
						mov edx,[edx]
						mov al,[edx+0x1F69C];
						mov v,eax;
					}
					Apply(&v,4,Mp,6);
				}
			}
			break;
		case 'A': // Cart/var разрешен арт (0) или нет (1)
			CHECK_ParamsMin(2);
			if(Num==2){
				v=ArtDisabled(Mp->n[0]);
				if(Apply(&v,4,Mp,1)==0){
					ArtDisabledSet(Mp->n[0],v);
				}
				break;
			}else if(Num==3){
				_ArtSetUp_ *artsetup;
				v2=-1; Apply(&v2,4,Mp,0);
				if((v2<0)||(v2>=ARTNUM)){ MError("\"!!UN:A\"-wrong artifact number (internal)."); RETURN(0) }
				artsetup=&GetArtBase()[v2];
				v=0; Apply(&v,4,Mp,1);
				switch(v){
					case 1: // cost
						v=artsetup->Cost; Apply(&v,4,Mp,2); artsetup->Cost=(Dword)v;
						break;
					case 2: // position
						v=artsetup->Position; Apply(&v,4,Mp,2); artsetup->Position=v;
						break;
					case 3: // type
						v=artsetup->Type; Apply(&v,4,Mp,2); artsetup->Type=v;
						break;
					case 4: // SuperNumber
						v=artsetup->SuperN; Apply(&v,4,Mp,2); artsetup->SuperN=v;
						break;
					case 5: // Part of Super
						v=artsetup->PartOfSuperN; Apply(&v,4,Mp,2); artsetup->PartOfSuperN=v;
						break;
					case 7: // Disable;
						v=artsetup->Disable; Apply(&v,4,Mp,2); artsetup->Disable=(Byte)v;
						break;
					case 8: // NewSpell;
						v=artsetup->NewSpell; Apply(&v,4,Mp,2); artsetup->NewSpell=(Byte)v;
						break;
					case 9: // Name 2 z
						StrMan::Apply(artsetup->Name, ArtSetUpBack[v2].Name, ArtNames[v2].NameVar, Mp, 2);
						break;
					case 10: // Descr 2 z
						StrMan::Apply(artsetup->Description, ArtSetUpBack[v2].Description, ArtNames[v2].DescVar, Mp, 2);
						break;
					case 11: // Pick up message
						StrMan::Apply(ArtPickUp[v2], ArtPickUpBack[v2], ArtNames[v2].PickUp, Mp, 2);
						break;
					default: //0
						MError("\"UN:A\"-wrong syntax (A$/$/$)."); RETURN(0)
				}
				break;
			}else if(Num>=4){
				int Arts[14];
				int CInd=-1; Apply(&CInd,4,Mp,0);
				if((CInd<0)||(CInd>=32)){ MError("\"!!UN:A\"-wrong combo artifact index."); RETURN(0) }
				int CNum=CArtSetup[CInd].Index; if(Apply(&CNum,4,Mp,1)) break;
				if((CNum<0)||(CNum>=ARTNUM)){ MError("\"!!UN:A\"-wrong combo artifact number."); RETURN(0) }
				for(int i=2;i<Num;i++){ Arts[i-2]=-1; Apply(&Arts[i-2],4,Mp,i); }
				ERMFlags[0]=BuildUpCombo(CNum,CInd,Num-2,Arts);
			}
			break;
		case 'U': // Utype/subtype/?N  или Utype/subtype/i/varnumber
			CHECK_ParamsMin(2);
			if((Apply(&t,4,Mp,0))||(Apply(&st,4,Mp,1))){ MError("\"!!UN:U\"-cannot check or get Type and Subtype."); RETURN(0) }
			if(Num==3){
				srf=CalcObjects(t,st);
				if(Apply(&srf,4,Mp,2)==0){ MError("\"!!UN:U\"-cannot set number of objects."); RETURN(0) }
			}else{
				if(Apply(&srf,4,Mp,2)){ MError("\"!!UN:U\"-cannot get or check number of object."); RETURN(0) }
				if(srf==0){ MError("\"!!UN:U\"-wrong object number (0). Must be>0, -1 or -2"); RETURN(0) }
				if(Apply(&v,4,Mp,3)){ MError("\"!!UN:U\"-cannot get or check number of V variable."); RETURN(0) }
				if((v<1)||(v>9998)){ MError("\"!!UN:U\"-wrong V var number (1...9998)."); RETURN(0) }
//        if(srf==-1){
				if(srf<0){
					x=ERMVar2[v-1];
					y=ERMVar2[v];
					l=ERMVar2[v+1];
					if(FindNextObjects(t,st,&x,&y,&l,srf)){ MError("\"!!UN:U\"-cannot find more objects."); RETURN(0) }
				}else{
					if(FindObjects(t,st,srf,&x,&y,&l)){ MError("\"!!UN:U\"-cannot get object coordinates."); RETURN(0) }
				}
				ERMVar2[v-1]=x;
				ERMVar2[v]=y;
				ERMVar2[v+1]=l;
			}
			break;
		case 'T': // Ttown/dwel/upgr/montype - тип монстров для найма
			CHECK_ParamsMin(4);
			t=-1; Apply(&t,4,Mp,0); if((t<0)||(t>8)){ MError("\"!!UN:T\"-wrong town number(0...8)."); RETURN(0) }
			mt=MonInTownBase(t);
			st=-1; Apply(&st,4,Mp,1); if((st<0)||(st>6)){ MError("\"!!UN:T\"-wrong dwelling number(0...6)."); RETURN(0) }
			t2=-1; Apply(&t2,4,Mp,2); if((t2<0)||(t2>1)){ MError("\"!!UN:T\"-wrong upgrade(0,1)."); RETURN(0) }
			v=mt[t2*7+st];
			if(Apply(&v,4,Mp,3)) break;
			if((v<0)||(v>=MONNUM)){ MError("\"!!UN:T\"-wrong monster type."); RETURN(0) }
			mt[t2*7+st]=v;
			break;
		case 'L': // переместить в позицию
			if(Num==4){ ShowThis(Mp->n[0],Mp->n[1],(char)Mp->n[2],Mp->n[3]); }
			else if(Num==3){
				if((Mp->n[0]>=0)&&(Mp->n[1]>=0)){
					t=GetObel(Mp->n[0],Mp->n[1]);
				}else t=0;
				if(Apply(&t,4,Mp,2)) break;
				SetObel(Mp->n[0],Mp->n[1],t);
			}else{ EWrongParamsNum(); RETURN(0) }// #x/#y/#l/#d
			break;
		case 'P': // параметры настройки
			if(Num<2){
				Apply(&PL_ExtDwellStd,4,Mp,0);
				PL_OptionReset[0] = PL_WoGOptions[0][0];
				break;
			}
			if((Mp->n[0]<0)||(Mp->n[0]>=PL_WONUM)){ MError("\"!!UN:P\"-wrong first parameter."); RETURN(0) }
			{
				int isCheck = Apply(&PL_WoGOptions[0][Mp->n[0]],4,Mp,1);
				if(Mp->n[0] >= 0 && Mp->n[0] <= 10)
					PL_OptionReset[Mp->n[0]] = PL_WoGOptions[0][Mp->n[0]];

				if(Mp->n[0] >= 900 && Mp->n[0] < 900 + PL_OptionReset2Size)
					PL_OptionReset2[Mp->n[0]] = PL_WoGOptions[0][Mp->n[0]];

				switch(Mp->n[0]){
					case 3: // NPC
					case 6: // NPC2Hire
						PL_NPCOptionWasSet=1;
						if(isCheck) break;
						if(PL_NoNPC) DisableNPC(-1); else EnableNPC(-1,!PL_NPC2Hire);
						break;
					case 5: // PL_ApplyWoG
						PL_DisableWogifyWasSet=1;
						break;
					case 900: // PL_CrExpEnable
					case 901: // PL_CrExpStyle
					case 906: // PL_ExpGainDis
						PL_ExpoWasSet=1;
						break;
				}
			}
			break;
		case 'B': // B#/$ настройка сундуков и мифрила
			CHECK_ParamsMin(2);
			switch(Mp->n[0]){
				case 0: // мифрил
					Apply(&EnableMithrill,4,Mp,1);
					break;
				default: // сундук 1..., камни 7...10 
					if((Mp->n[0]<0)||(Mp->n[0]>19)){ MError("\"!!UN:B\"-wrong first parameter."); RETURN(0) }
					Apply(&EnableChest[Mp->n[0]],4,Mp,1);
					break;
			}
			break;
		case 'Q': // Q# проиграть (0) или выиграть (1)
			if(Num>1){ // 3.58
				if(Mp->n[0]){ // выиграть
					__asm{
						mov   ecx,1
						mov   eax,0x4F3370
						call  eax
					}
				}else{ // проиграть
					__asm{
						mov   ecx,2
						mov   eax,0x4F3370
						call  eax
					}
				}
			}else{
				if(Mp->n[0]){ // выиграть
					__asm{
						mov    eax,0x697308
						mov    dword ptr [eax],1
						mov    eax,0x699560
						mov    dword ptr [eax],1
					}
				}else{ // проиграть
					__asm{
						mov    eax,0x697308
						mov    dword ptr [eax],1
						mov    eax,0x699560
						mov    dword ptr [eax],0
					}
				}
			}
			break;
		case 'G': // G#/#/#/$ имена
			CHECK_ParamsMin(4);
			switch(Mp->n[0]){
				case 0:  // вторичных скилов 0/номер_скила/тип_текста/zvar
					t=Mp->n[1];  if((t<0)||(t>28)){ MError("\"!!UN:G\"-wrong secondary skill number (0...27)."); RETURN(0) }
					t2=Mp->n[2]; if((t2<0)||(t2>3)){ MError("\"!!UN:G\"-wrong secondary skill text type number (0...3)."); RETURN(0) }
					switch(t2){
						case 0:
							if (StrMan::Apply(SSNAME[t].Name, SSNameBack[t].Name, SSAllNames[t].Var[t2], Mp, 3))
							{
								if (SSAllNames[t].Var[t2] == 0)
									SSNAMEDESC[t].Name[t2] = SSNameDescBack[t].Name[t2];
								else
									SSNAMEDESC[t].Name[t2] = SSNAME[t].Name;
							}
							break;
						default:
							if (StrMan::Apply(SSDESC[t].Name[t2-1], SSDescBack[t].Name[t2-1], SSAllNames[t].Var[t2], Mp, 3))
							{
								if (SSAllNames[t].Var[t2] == 0)
									SSNAMEDESC[t].Name[t2] = SSNameDescBack[t].Name[t2];
								else
									SSNAMEDESC[t].Name[t2] = SSDESC[t].Name[t2-1];
							}
							break;
					}
					break;
				case 1:  // монстры 1/номер_монстра/тип_текста/zvar
					t=Mp->n[1];  if((t<0)||(t>=MONNUM)){ MError("\"!!UN:G\"-wrong monster number."); RETURN(0) }
					t2=Mp->n[2]; if((t2<0)||(t2>4)){ MError("\"!!UN:G\"-wrong monster text type number (0...4)."); RETURN(0) }
					switch(t2){
						case 0: // single
							if (StrMan::Apply(MonTable2[t].NameS, MonTable2Back[t].NameS, MonNames[t].Var[t2], Mp, 3))
								MonTable[t].NameS = MonTable2[t].NameS;
							break;
						case 1: // plural
							if (StrMan::Apply(MonTable3[t].NameP, MonTable3Back[t].NameP, MonNames[t].Var[t2], Mp, 3))
								MonTable[t].NameP = MonTable3[t].NameP;
							break;
						case 2: // spec
							if (StrMan::Apply(MonTable4[t].Spec, MonTable4Back[t].Spec, MonNames[t].Var[t2], Mp, 3))
								MonTable[t].Spec = MonTable4[t].Spec;
							break;
						case 3: // def
							StrMan::Apply(MonTable[t].DefName, MonTableBack[t].DefName, MonNames[t].Var[t2], Mp, 3);
							break;
						case 4: // sound prefix
							StrMan::Apply(MonTable[t].SoundPrefix, MonTableBack[t].SoundPrefix, MonNames[t].Var[t2], Mp, 3);
							break;
					}
					break;
				case 2:  // Спец.Героев 2/номер_героя/тип_текста(3-картинка)/zvar(номер картинки спец+1)
					t=Mp->n[1];  if((t<0)||(t>=HERNUM)){ MError("\"!!UN:G\"-wrong hero number."); RETURN(0) }
					t2=Mp->n[2]; if((t2<0)||(t2>3)){ MError("\"!!UN:G\"-wrong herospec text type number (0...3)."); RETURN(0) }
					if(t2==3){ // картинка
						v=HSpecNames[t].PicNum-1;
						if(Apply(&v,4,Mp,3)) break;
						HSpecNames[t].PicNum=v+1;
						break;
					}
					switch(t2){
						case 0: // short
							StrMan::Apply(HSpecTable[t].SpShort, HSpecBack[t].SpShort, HSpecNames[t].Var[t2], Mp, 3);
							break;
						case 1: // full
							StrMan::Apply(HSpecTable[t].SpFull, HSpecBack[t].SpFull, HSpecNames[t].Var[t2], Mp, 3);
							break;
						case 2: // descr
							StrMan::Apply(HSpecTable[t].SpDescr, HSpecBack[t].SpDescr, HSpecNames[t].Var[t2], Mp, 3);
					}
					break;
				default: MError("\"!!UN:G\"-wrong first parameter."); RETURN(0)
			}
			break;
		case 'J': // J#/#/$ разное
			switch(Mp->n[0]){
				case 0: // доступность спеллов
					CHECK_ParamsMin(3);
					t=Mp->n[1];  if((t<0)||(t>=0x46)){ MError("\"!!UN:J\"-wrong spell number (0...69)."); RETURN(0) }
					v=SpellDisBase()[t];
					if(Apply(&v,4,Mp,2)) break;
					SpellDisBase()[t]=(char)v;
					break;
				case 1: // ограничение уровня опыта героев
					CHECK_ParamsMin(3);
					v=GetLimitLvl();
					if(Apply(&v,4,Mp,1)){ // check,get
					}else{ // set
						SetLimitLvl(v);
					}
					v=(int)GetExpo(v);
					Apply(&v,4,Mp,2);
					break;
				case 2: // сложность игры
					CHECK_ParamsMin(2);
					v=GameLevel();
					if(Apply(&v,4,Mp,1)) break; // check,get
					SetGameLevel(v);
					break;
				case 3: // J3/^text^ or J3/# загрузка пользовательских опций
					{ char *d;
						if(Num!=2){ MError("\"!!UN:J3\"-wrong number of parameters."); RETURN(0) }
						if(Mp->n[1]!=0){ // аргумент - z var
							if(Apply(&v,4,Mp,1)){ EWrongSyntax(); RETURN(0) }
							if(BAD_INDEX_LZ(v)){ MError("\"!!UN:J3\"- z var out of range (-10...-1,1...1000+)."); RETURN(0) }
							d=GetErmString(v);
						}else{ // аргумент - ^текст^
							d=ERM2String(&Mp->m.s[Mp->i],0,&v);
							Mp->i+=v;
						}
						ResetCustomSettings(d);
						break;
					}
				case 4: // 3.58 set the AI thinking delay
					if(Num<2){ MError("\"!!UN:J4\"-insufficient parameters."); RETURN(0) }
					v=GetDelay();
					if(Apply(&v,4,Mp,1)) break; // check,get
					AI_Delay=v;
					SetDelay(v);
					break;
				case 5: // 3.58 set auto save flag
					if(Num<2){ MError("\"!!UN:J5\"-insufficient parameters."); RETURN(0) }
					v=GetAutoSave();
					if(Apply(&v,4,Mp,1)) break; // check,get
					AutoSaveFlag=v;
					SetAutoSave(v);
					break;
				case 6: // J6/#/$; 3.58 get a random artifact
					if(Num<3){ MError("\"!!UN:J6\"-insufficient parameters."); RETURN(0) }
					if(Apply(&v,4,Mp,1)){ MError("\"!!UN:J6\"-cannot check art level."); RETURN(0) }
					v=GenArt(v);
					Apply(&v,4,Mp,2);
					break;
				case 7: // J7/#/$; 3.58 get/set Merchant artifact
					if(Num<3){ MError("\"!!UN:J7\"-insufficient parameters."); RETURN(0) }
					if(Apply(&v,4,Mp,1)){ MError("\"!!UN:J7\"-cannot check art slot."); RETURN(0) }
					v2=GetMerchArt(v);
					if(Apply(&v2,4,Mp,2)) break;
					SetMerchArt(v,v2);
					break;
				case 8: // J8/#(modifier)/#(z var) or J8/#/^text^ set flag 1 - if file exists
					{ char *d;
						if(Num!=3){ MError("\"!!UN:J8\"-wrong number of parameters."); RETURN(0) }
						if(Apply(&t,4,Mp,1)){ EWrongSyntax(); RETURN(0) }
						if(Mp->n[2]!=0){ // аргумент - z var
							if(Apply(&v,4,Mp,2)){ EWrongSyntax(); RETURN(0) }
							if(BAD_INDEX_LZ(v)){ MError("\"!!UN:J8\"- z var out of range (-10...-1,1...1000+)."); RETURN(0) }
							d=GetErmString(v);
						}else{ // аргумент - ^текст^
							d=ERM2String(&Mp->m.s[Mp->i],0,&v);
							Mp->i+=v;
						}
						ERMFlags[0]=DoesFileExist(d,t);
						break;
					}
				case 9: // J9/#(modifier)/#(z var) - get path to z var
					{ char *d;
						if(Num!=3){ MError("\"!!UN:J9\"-wrong number of parameters."); RETURN(0) }
						if(Apply(&t,4,Mp,1)){ EWrongSyntax(); RETURN(0) }
						if(Mp->n[2]!=0){ // аргумент - z var
							Apply(&v,4,Mp,2);
							if(BAD_INDEX_LZ(v)||(v>1000)){ MError("\"!!UN:J9\"- z var out of range (-10...-1,1...1000)."); RETURN(0) }
							d = GetPureErmString(v);
						}else{ // аргумент - ^текст^
							MError("\"!!UN:J9\"- must be z var."); 
							RETURN(0)
						}
						StrCopy(d,512,GetFolder(t));
						break;
					}
				case 10: // J10 - write variable dump
					DumpERMVars("SCRIPT BASED LOGGING (by UN:J10 command)",0);
					break;
				case 11: // 3.59 J11/$ - запретить залипание диалога инфы о монстрах в битве
					Apply(&MonInfoDlgPopUpSetting,4,Mp,1);
					break;
				case 12:
					{ char *d;
						char *MapName="";
						__asm{
							mov   eax,BASE
							mov   eax,[eax]
							add   eax,0x1F6D9
							mov   MapName,eax
						}
						if(Mp->n[1]!=0){ // аргумент - z var
							Apply(&v,4,Mp,1);
							if(GetPureErmString(d, v)) RETURN(0)
						}else{ // аргумент - ^текст^
							MError("\"!!UN:J12\"- must be z var."); 
							RETURN(0)
						}
						strncpy(d,MapName,511);
					}
					break;
				default: MError("\"!!UN:J\"-wrong first parameter."); RETURN(0)
			}
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}

//---- Flag Color

struct _FCstruct_
{
	//_MapItem_ * mip;
	int Type;
	Dword SType;
	int Color;
};

_FCstruct_ * FCstruct;

int __fastcall ChangeFlagColor(_MapItem_ *mip, int r)
{
	STARTNA(__LINE__, 0)
	_FCstruct_ fc;
	_FCstruct_ * old = FCstruct;

	fc.Type = CheckPlace4Hero(mip, 0);
	fc.SType = mip->OSType;
	fc.Color = r;
	FCstruct = &fc;

	Map2Coord(mip, &ERM_PosX, &ERM_PosY, &ERM_PosL);
	pointer = 30375;
	ProcessERM();

	FCstruct = old;
	RETURN(fc.Color)
}

int ERM_FlagColor(char Cmd, int Num, _ToDo_*, Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	if (FCstruct == 0) { MError2("not in !?FC trigger"); RETURN(0) }
	switch(Cmd)
	{
		case 'T':
			CHECK_ParamsNum(1);
			Apply(&FCstruct->Type, 4, Mp, 0);
			break;
		case 'U':
			CHECK_ParamsNum(1);
			Apply(&FCstruct->SType, 4, Mp, 0);
			break;
		case 'C':
			CHECK_ParamsNum(1);
			Apply(&FCstruct->Color, 4, Mp, 0);
			break;
		default:
			EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

//---- Dig Grail

struct _DGstruct_
{
	//_MapItem_ * mip;
	int x,y,z;
	int Grail;
	int Std;
};

_DGstruct_ * DGstruct;

int __fastcall DigGrailTrigger(_Hero_ * hero, int r, int x, int y, int z)
{
	STARTNA(__LINE__, 0)
	_DGstruct_ a;
	_DGstruct_ * old = DGstruct;

	a.x = x;
	a.y = y;
	a.z = z;
	a.Grail = r;
	a.Std = 1;
	DGstruct = &a;

	ERM_GM_ai = IsThis(hero->Owner);
	ERM_HeroStr = hero;
	ERM_PosX = x; ERM_PosY = y; ERM_PosL = z;
	pointer = 30376;
	ProcessERM();

	DGstruct = old;
	RETURN(!a.Std ? 2 : (a.Grail ? 1 : 0))
}

int ERM_DigGrail(char Cmd, int Num, _ToDo_*, Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	if (DGstruct == 0) { MError2("not in !?DG trigger"); RETURN(0) }
	switch(Cmd)
	{
		case 'P':
			CHECK_ParamsNum(3);
			Apply(&DGstruct->x, 4, Mp, 0);
			Apply(&DGstruct->y, 4, Mp, 1);
			Apply(&DGstruct->z, 4, Mp, 2);
			break;
		case 'G':
			CHECK_ParamsNum(1);
			Apply(&DGstruct->Grail, 4, Mp, 0);
			break;
		case 'R':
			CHECK_ParamsNum(1);
			Apply(&DGstruct->Std, 4, Mp, 0);
			break;
		default:
			EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

//static char FM_buf[8];
static char FM_buf[18];
char *FindMacro(Mes *mp,int Set)
{
	STARTNA(__LINE__,&mp->m.s[mp->i])
	int  e,s,i,j;
	char ch,cch;

	if(Set) cch='@'; else cch='$';
	for(;mp->i<mp->m.l;mp->i++){
		ch=mp->m.s[mp->i];
		if(ch==' ') continue;
		if(ch== 9 ) continue;
		if(ch!=cch) RETURN(0)
		break;
	}
	for(s=mp->i,e=mp->i+1;e<mp->m.l;e++){
		ch=mp->m.s[e];
		if(ch==0){ MError("Macro was not found."); RETURN(0) }
		if(ch==cch){
			for(i=0,j=s+1; (j<e)&&(i<16); i++,j++) FM_buf[i]=mp->m.s[j];
			if(j!=e){ MError("Macro is too long (>16 characters)."); RETURN(0) }
			for(;i<18;i++) FM_buf[i]=0;
			mp->i=e+1;
			RETURN(FM_buf)
		}
	}
	RETURN(0)
}
char *FindMacro2(char *str,int Set, int *shift)
{
	STARTNA(__LINE__,str)
	int  i,j;
	char ch,cch;

	if(Set) cch='@'; else cch='$';
	for(i=0;;i++){
		ch=str[i];
		if(ch== 0 ){ MError("Macro was not found."); RETURN(0) }
		if(ch==' ') continue;
		if(ch== 9 ) continue;
		if(ch!=cch) RETURN(0)
		break;
	}
	i+=1;
	for(j=0;;i++,j++){
		ch=str[i];
		if(ch==0){ MError("Macro was not found."); RETURN(0) }
		if(ch==cch){
			*shift=i+1;
			for(;j<18;j++) FM_buf[j]=0;
			RETURN(FM_buf)
		}
		if(j>16){ MError("Macro is too long (>16 characters)."); *shift=i+1; RETURN(0) }
		FM_buf[j]=ch;
	}
//  RETURN(0)
//  return 0;
}

int _inline CheckMacro(char *src, char *dst){
	long *src1 = (long*)src, *dst1 = (long*)dst;
	if (*(src1++) != *(dst1++)) return 1;
	if (*(src1++) != *(dst1++)) return 1;
	if (*(src1++) != *(dst1++)) return 1;
	if (*src1 != *dst1) return 1;
	return 0;
}

static VarNum GM_vn;
VarNum *GetMacro(char *macro)
{
	STARTNA(__LINE__,macro)
	int i;
	if(macro==0) RETURN(0)
	GM_vn.Check=0;
	for(i=0;i<1000;i++){
		if(ERMMacroName[i][0]==0) break;
		if(CheckMacro(macro,ERMMacroName[i])) continue;
		GM_vn.Type=0; GM_vn.Num=ERMMacroVal[i];
		RETURN(&GM_vn)
	}
	for(i=1;i<=15;i++){
		if(CheckMacro(macro,ERMVarMacro[i-1])) continue;
		GM_vn.Type=2; GM_vn.Num=i;
		RETURN(&GM_vn)
	}
	for(i=1;i<=VAR_COUNT_V;i++){
		if(CheckMacro(macro,ERMVar2Macro[i-1])) continue;
		GM_vn.Type=3; GM_vn.Num=i;
		RETURN(&GM_vn)
	}
	for(i=1;i<=1000;i++){
		if(CheckMacro(macro,ERMStringMacro[i-1])) continue;
		GM_vn.Type=7; GM_vn.Num=i;
		RETURN(&GM_vn)
	}
	for(i=1;i<=200;i++){
		if(CheckMacro(macro,ERMVarHMacro[i-1])) continue;
		GM_vn.Type=vtW; GM_vn.Num=i;
		RETURN(&GM_vn)
	}
	//...
	RETURN(0)
}

void SetMacro(char *src, char *dst){
	STARTNA(__LINE__,src)
	for(int i=0;i<16;i++) dst[i]=src[i];
	RETURNV
}
////////////////////
int ERM_Macro(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  vi, i;
	char *str;
	CHECK_ParamsNum(1);
	VarNum *vnp=&sp->Par[0];
	VarNum *vnp2=&sp->Par[1];
//  if(*(Dword * )vnp2==0) vi=vnp->Num;
	if(vnp2->Num==0 && vnp2->Type==0 && vnp2->IType==0 && vnp2->Check==0) vi=vnp->Num;
	else vi=GetVarVal(vnp2);
	str=FindMacro(Mp,1);
	if(str==0){ MError("Macro defined incorrectly."); RETURN(0) }
	switch(Cmd){
		case 'S': // установить
			switch(vnp->Type){
				case 0: // число
					for (i = 0; (i < 1000) && (ERMMacroName[i][0] != 0) && CheckMacro(ERMMacroName[i], str); i++) ;
					if (i == 1000) { MError2("cannot declare more macros for numbers (1000)."); RETURN(0) }
					SetMacro(str,ERMMacroName[i]);
					ERMMacroVal[i] = vi;
					break;
				case 2: // f...t
					if((vi<1)||(vi>15)){ MError("\"!!MC:S\"-var is out of set (f...t)."); RETURN(0) }
					SetMacro(str,ERMVarMacro[vi-1]);
					break;
				case 3: // v1...1000
					if(BAD_INDEX_V(vi)){ MError("\"!!MC:S\"-var is out of set (v1...v10000)."); RETURN(0) }
					SetMacro(str,ERMVar2Macro[vi-1]);
					break;
				case 4: // w1...100
					if((vi<1)||(vi>200)){ MError("\"!!MC:S\"-var is out of set (w1...w200)."); RETURN(0) }
					SetMacro(str,ERMVarHMacro[vi-1]);
					break;
//        case 5: // v1...1000
//        case 6: // v1...1000
				case 7: // z1...500
					if((vi<1)||(vi>1000)){ MError("\"!!MC:S\"-var is out of set (z1...z1000)."); RETURN(0) }
					SetMacro(str,ERMStringMacro[vi-1]);
					break;
				default: MError("\"!!MC:S\"-Wrong var type."); RETURN(0)
			}
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

int ERM_Variable(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	char *txt;
	int  vv,vv2,i,ind;
	VarNum *vnp=&sp->Par[0];
	switch(Cmd){
		case 'S': // установить
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(vnp->Type==7){  // строка
				if(Mp->n[0]!=0){ // копируем одну в другую
					vv=GetVarVal(vnp);
					if(Apply(&vv,4,Mp,0)) break;
					SetVarVal(vnp,vv);
				}else{           // копируем сам текст
					ind=GetVarVal(vnp);
					if((ind<-VAR_COUNT_LZ)||(ind==0)||(ind>1000)){ MError("\"!!VR:S\"-var is out of set (z-10...-1,1...z1000)."); RETURN(0) }
					StrCopy(GetPureErmString(ind),512,ERM2String(&Mp->m.s[Mp->i],0,&vv));
					Mp->i+=vv;
				}
			}else if(vnp->Type==8){  // floating
				if(Mp->VarI[0].Check!=0){ MError("\"!!VR:S\"-you cannot use get or check syntax for float variable."); RETURN(0) }
				float vf=GetVarValF(&Mp->VarI[0]);
				SetVarValF(vnp,vf);
			}else{
				vv=GetVarVal(vnp);
				if(Apply(&vv,4,Mp,0)) break;
				SetVarVal(vnp,vv);
			}
			break;
		case 'R': // установить случ генератор
			if(Num>2){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(Num==1){
				Apply(&vv,4,Mp,0);
				vv=GetVarVal(vnp)+Random(0,vv);
				SetVarVal(vnp,vv);
			}else{
				int *pseed = RandomSeed();
				Apply(pseed,4,Mp,1);
				*(int*)0x67FBE4 = *pseed;
			}
			break;
		case 'T': // случ генератор по времени
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			Apply(&vv,4,Mp,0);
			vv=GetVarVal(vnp)+TimeRandom(0,vv);
			SetVarVal(vnp,vv);
			break;
		case '+': // добавить
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(vnp->Type==7){  // строка
				ind = GetVarVal(vnp);
				if(BAD_INDEX_LZ(ind)||(ind>1000)){ MError("\"!!VR:+\"-var is out of set (z-10...-1,1...z1000)."); RETURN(0) }
				txt = GetPureErmString(ind);
				if(Mp->n[0]!=0){ // добавляем одну к другой
					vv=ind;
					if(Apply(&vv,4,Mp,0)) break;
					if(BAD_INDEX_LZ(vv)){ MError("\"!!VR:+\"-var is out of set (z-10...-1,1...z1000+)."); RETURN(0) }
					StrCanc(txt, 512, txt, GetPureErmString(vv));
				}else{           // добавляем сам текст
					StrCanc(txt, 512, txt, ERM2String(&Mp->m.s[Mp->i],0,&vv));
					Mp->i+=vv;
				}
			}else if(vnp->Type==8){  // floating
				float vf=GetVarValF(&Mp->VarI[0]);
				vf=GetVarValF(vnp)+vf;
				SetVarValF(vnp,vf);
			}else{
				Apply(&vv,4,Mp,0);
				vv=GetVarVal(vnp)+vv;
				SetVarVal(vnp,vv);
			}
			break;
		case '-': // вычесть
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(vnp->Type==8){  // floating
				float vf=GetVarValF(&Mp->VarI[0]);
				vf=GetVarValF(vnp)-vf;
				SetVarValF(vnp,vf);
				break;
			}
			Apply(&vv,4,Mp,0);
			vv=GetVarVal(vnp)-vv;
			SetVarVal(vnp,vv);
//      ERMVar[vi-1]-=vv;
			break;
		case '*': // умножить
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(vnp->Type==8){  // floating
				float vf=GetVarValF(&Mp->VarI[0]);
				vf=GetVarValF(vnp)*vf;
				SetVarValF(vnp,vf);
				break;
			}
			Apply(&vv,4,Mp,0);
			vv=GetVarVal(vnp)*vv;
			SetVarVal(vnp,vv);
//      ERMVar[vi-1]*=vv;
			break;
		case ':': // делить
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(vnp->Type==8){  // floating
				float vf=GetVarValF(&Mp->VarI[0]);
				if(vf==0.0){ Message("Sorry. Division by zero :-)",1); RETURN(0) }
				vf=GetVarValF(vnp)/vf;
				SetVarValF(vnp,vf);
				break;
			}
			Apply(&vv,4,Mp,0);
			if(vv==0){ Message("Sorry. Division by zero :-)",1); RETURN(0) }
			vv=GetVarVal(vnp)/vv;
			SetVarVal(vnp,vv);
//      ERMVar[vi-1]/=vv;
			break;
		case '%': // остаток от деления
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			Apply(&vv,4,Mp,0);
			if(vv==0){ Message("Sorry. Division by zero :-)",1); RETURN(0) }
			vv=GetVarVal(vnp)%vv;
			SetVarVal(vnp,vv);
//      ERMVar[vi-1]%=vv;
			break;
		case '&': // логич и
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			Apply(&vv,4,Mp,0);
			vv=(Dword)GetVarVal(vnp)&*(Dword *)&vv;
			SetVarVal(vnp,vv);
//      *(Dword *)(&ERMVar[vi-1])&=*(Dword *)&vv;
			break;
		case '|': // логич или
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			Apply(&vv,4,Mp,0);
			vv=(Dword)GetVarVal(vnp)|*(Dword *)&vv;
			SetVarVal(vnp,vv);
//      *(Dword *)(&ERMVar[vi-1])|=*(Dword *)&vv;
			break;
		case '^': // логич искл. или
			Message("WARNING!\n The \'!!VR:^$\' command should be changed to \'!!VR:X$\' command.",1);
		case 'X': // логич искл. или
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(Apply(&vv,4,Mp,0)) break;
			vv=(Dword)GetVarVal(vnp)^*(Dword *)&vv;
			SetVarVal(vnp,vv);
//      *(Dword *)(&ERMVar[vi-1])^=*(Dword *)&vv;
			break;
		case 'H': // H# проверить строку на отсутствие текста во флаг #
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(vnp->Type==7){  // строка
				if((Mp->n[0]<1)||(Mp->n[0]>1000)){ MError("\"!!VR:H\"-flag number out of range (1...1000)."); RETURN(0) }
				ind=GetVarVal(vnp);
				ERMFlags[Mp->n[0]-1] = (HasText(GetPureErmString(ind)) ? 1 : 0);
			}else{
				MError("\"!!VR:H\"-not a string variable (z#)."); RETURN(0)
			}
			break;
		case 'C': // C$/$/$/$/$.... установить
			if (vnp->Type >= 2 && vnp->Type <= 6)
			{
				int *p;
				ind = GetVarIndex(vnp, false);
				if (ind == 0) RETURN(0)
				for(i=0; i<Num; i++,ind++)
				{
					p = GetVarAddress(ind, vnp->Type);
					if (p == 0) RETURN(0)
					Apply(p,4,Mp,i);
				}
			}
			else{
				MError2("wrong variable type (must be integer)."); RETURN(0)
			}
			break;
		case 'U': // search for
		 { char *s,*d;
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(vnp->Type==7){  // строка
				ind=GetVarVal(vnp);
				if(GetPureErmString(s, ind)) RETURN(0)
				if(Mp->n[0]!=0){ // аргумент - z var
					if(Apply(&vv,4,Mp,0)) break;
					if(BAD_INDEX_LZ(vv)){ MError("\"!!VR:U\"- z var out of range (-10...-1,1...)."); RETURN(0) }
					d=GetErmString(vv);
				}else{ // аргумент - ^текст^
					d=ERM2String(&Mp->m.s[Mp->i],0,&vv);
					Mp->i+=vv;
				}
				ERMFlags[0]=(char)Search4Substring(s,d);
			}else{
				MError("\"!!VR:U\"-not Z var."); RETURN(0)
			}
			break;
		 }
		case 'M': // 3.58 string process
		 { char *s,*d;
			if(vnp->Type!=7){  // не строка
				MError("\"!!VR:M\"-not Z var."); RETURN(0)
			}
			CHECK_ParamsMin(2);
			ind=GetVarVal(vnp);
			if(BAD_INDEX_LZ(ind)||ind>1000){ MError("\"!!VR:M\"- z var out of range (-10...-1,1...1000)."); RETURN(0) }
			if(GetPureErmString(d, ind)) RETURN(0)
			switch(Mp->n[0]){
				case 1: // взять подстроку из z2 с #3 длинной #4
					if(Num<4){ MError("\"!!VR:M1\"-wrong number of parameters."); RETURN(0) }
					if(Apply(&vv,4,Mp,1)) break; // z var source
					if(GetPureErmString(s, vv)) RETURN(0)
					vv=0;  if(Apply(&vv,4,Mp,2)) break; // start
					vv2=0; if(Apply(&vv2,4,Mp,3)) break; // len
					if(vv>511) vv=511;
					if((vv2+vv)>511) vv2=511-vv;
					for(i=0;i<vv2;i++) d[i]=s[vv+i];
					d[vv2]=0;
					break;
				case 2: // взять токен номер #3 из z2
				 {
					char *tmp,buf[512];
					if(Num<3){ MError("\"!!VR:M2\"-wrong number of parameters."); RETURN(0) }
					if(Apply(&vv,4,Mp,1)) break; // z var source
					if(GetPureErmString(s, vv)) RETURN(0)
					StrCopy(buf,512,s);
					tmp=buf;
					vv=0; if(Apply(&vv,4,Mp,2)) break; // token
					d[0]=0;
					for(i=0;i<=vv;i++){
						s=Strtok(tmp);
						if(s==0) break; // no more tokens
						tmp=0;
					}
					StrCopy(d,512,s);
					break;
				 }
				case 3: // преобразовать число из #2 по основанию #3 в строку
					if(Num<2){ MError("\"!!VR:M3\"-wrong number of parameters."); RETURN(0) }
					vv=0; Apply(&vv,4,Mp,1);
					i=10;
					if(Num>2) Apply(&i,4,Mp,2);
					Itoa(vv,d,i);
					break;
				case 4: // VRxxx:M4/$ посчитать длину строки в символах
					if(Num<2){ MError("\"!!VR:M4\"-wrong number of parameters."); RETURN(0) }
					vv=strlen(d); Apply(&vv,4,Mp,1);
					break;
				case 5: // VRxxx:M5/$ первый значимый символ в строке
					if(Num<2){ MError("\"!!VR:M5\"-wrong number of parameters."); RETURN(0) }
					vv=StrSkipLead(d,0); Apply(&vv,4,Mp,1);
					break;
				case 6: // VRxxx:M6/$ последний значимый символ в строке
					if(Num<2){ MError("\"!!VR:M6\"-wrong number of parameters."); RETURN(0) }
					vv=StrSkipTrailer(d,strlen(d)); Apply(&vv,4,Mp,1);
					break;
				default:
					MError("\"!!VR:M\"-wrong first parameter."); RETURN(0)
			}
			break;
		 }
		case 'V': // 3.58 установить
		 {
			float vf;
			char *s;
			if(Num!=1){ MError("\"!!VR\"-wrong number of parameters."); RETURN(0) }
			if(Apply(&vv,4,Mp,0)) break; // z var source
			if(GetPureErmString(s, vv)) RETURN(0)
			switch(vnp->Type){
				case 8:  // floating
//          MError("\"!!VR:V\"-floating point not supported yet.");
//          RETURN(0)
					vf=Atof(s); SetVarValF(vnp,vf);
					break;
				case 2:
				case 3:
				case 4:
				case 5:
				case 6: // all integer
					vv=Atoi(s); SetVarVal(vnp,vv);
					break;
				default:
					MError("\"!!VR:V\"-wrong type of var (fl or int only)."); RETURN(0)
			}
			break;
		 }
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}

int ERM_Timer(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  ti,vv;
	VarNum *vnp=&sp->Par[0];
	switch(Cmd){
		case 'S': // установить Sfirst/last/period/owner
			CHECK_ParamsMin(4);
			ti=GetVarVal(vnp);
			if((ti<1)||(ti>100)){ MError("\"!!TM:S\"-timer number is out of range (1...100)."); RETURN(0) }
			vv=ERMTimer[ti-1].FirstDay; Apply(&vv,4,Mp,0); ERMTimer[ti-1].FirstDay=(Word)vv;
			vv=ERMTimer[ti-1].LastDay;  Apply(&vv,4,Mp,1); ERMTimer[ti-1].LastDay=(Word)vv;
			vv=ERMTimer[ti-1].Period;   Apply(&vv,4,Mp,2); ERMTimer[ti-1].Period=(Word)vv;
			vv=ERMTimer[ti-1].Owners;   Apply(&vv,4,Mp,3); ERMTimer[ti-1].Owners=(Word)vv;
			break;
		case 'E': // разрешить игроку Eowner
			CHECK_ParamsNum(1);
			ti=GetVarVal(vnp);
			if((ti<1)||(ti>100)){ MError("\"!!TM:E\"-timer number is out of range (1...100)."); RETURN(0) }
			ERMTimer[ti-1].Owners|=(Word)(1<<Mp->n[0]);
			break;
		case 'D': // запретить игроку Downer
			CHECK_ParamsNum(1);
			ti=GetVarVal(vnp);
			if((ti<1)||(ti>100)){ MError("\"!!TM:S\"-timer index is out of range (1...100)."); RETURN(0) }
			ERMTimer[ti-1].Owners&=(Word)~(1<<Mp->n[0]);
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}

int LastAutoTimer = 99;
int AddTimer(Word fd, Word ld, Word p, Word o)
{
	int i = 100;
	for (; i<=LastAutoTimer; i++)
		if (ERMTimer[i].FirstDay == fd && ERMTimer[i].LastDay == ld && ERMTimer[i].Period == p && ERMTimer[i].Owners == o)
			break;
	if (i > LastAutoTimer)
	{
		if (i == 200) { MError2("too many timers"); return -1; }
		LastAutoTimer = i;
		ERMTimer[i].FirstDay = fd;
		ERMTimer[i].LastDay = ld;
		ERMTimer[i].Period = p;
		ERMTimer[i].Owners = o;
	}
	return i;
}

#define ERMOBJECTS 1000
static struct _ERM_Object_{
	char  Used;        // 1-есть, 0-свободно
	short HintVar;     // переменная подсказки 0-нет,1...
	char  Disabled[8]; // 1-dis 0-en по игрокам
	char  AMessage[8][2]; // =1-запрещено, след состояние
// 0x00000000 - позиция
// 0x1tttssss - тип объекта
// 0x200pttss - переменная позиция
// 0x4w - переменная w
// 0x8v - переменная v
////// 0x6a - артифакт
	Dword PosMix;      // смешанная позиция на карте
} ERM_Object[ERMOBJECTS];

_ERM_Object_ *FindObj(Dword MixPos)
{
	STARTNA(__LINE__, 0)
	int i;

	for(i=0;i<ERMOBJECTS;i++){
		if(ERM_Object[i].Used==0) continue;
		if(ERM_Object[i].PosMix==MixPos) RETURN(&ERM_Object[i])
	}
	RETURN(0)
}

_ERM_Object_ TempObj;
_ERM_Object_ *GetObj(Dword MixPos)
{
	STARTNA(__LINE__, 0)
	_ERM_Object_ *ret = FindObj(MixPos);
	if (ret == 0)
	{
		ret = &TempObj;
		FillMem((char*)ret, sizeof(TempObj), 0);
		TempObj.PosMix = MixPos;
		//TempObj.Used = 1;
	}
	RETURN(ret)
}

_ERM_Object_ *FindObjExt(_MapItem_ *Mp,int x,int y,int l)
{
	STARTNA(__LINE__, 0)
	int wnum,wval,vnum,vval;
	int i,c,ind;
	_Square_ *sqp;
	int alltype=((Mp->OType&0xFF)<<8)+(Mp->OSType&0xFF);
	_ERM_Object_ *op;
	for(c=0;c<2;c++){
		if(c==1) alltype=((Mp->OType&0xFF)<<8)+0xFF; // для всех подтипов
		for(i=0;i<ERMOBJECTS;i++){
			op=&ERM_Object[i];
			if(op->Used==0) continue;
			switch(op->PosMix&0xF0000000){
				case 0: break;
				case 0x20000000: // position->number (0...15)
					if(alltype!=(int)(op->PosMix&0x0000FFFF)) continue; // другой
					sqp=&Square[x][y][l];
					if(sqp->Number==((op->PosMix&0x000F0000)>>16)) RETURN(op)
					break;
				case 0x40000000: // w (0...99) = (0...32)
					if(alltype!=(int)(op->PosMix&0x0000FFFF)) continue; // другой
					wnum=(op->PosMix&0x007F0000)>>16;
					if(wnum>99){ MError("Hint W variable out of range (>100)"); RETURN(0)}
					wval=(op->PosMix&0x0F800000)>>23;
//          asm{ mov eax,0x698B70; mov eax,[eax]; }
//          hp=(_Hero_ *)_EAX;
//          if(hp==0) continue;
//          if(ERMVarH[hp->Number][wnum]==wval) return op;
					ind=CurrentUser()*0x168;
					__asm{
						mov   eax,0x699538
						mov   eax,[eax]
						add   eax,0x20AD4
						add   eax,ind
						mov   eax,[eax]
						mov   ind,eax
					}
					if(ind==-1) continue; // нет активного героя
					if(ERMVarH[ind][wnum]==wval) RETURN(op)
					break;
				case 0x80000000: // v  (1...1000) = (0...3)
					if(alltype!=(int)(op->PosMix&0x0000FFFF)) continue; // другой
					vnum=(op->PosMix&0x03FF0000)>>16;
					if(vnum>=VAR_COUNT_V){ MError("Hint V variable out of range (>10000)"); RETURN(0)}
					vval=(op->PosMix&0x0C000000)>>26;
					if(ERMVar2[vnum]==vval) RETURN(op)
					break;
				case 0x10000000: // объект по типу/подтипу
					if(alltype!=(int)(op->PosMix&0x0000FFFF)) continue; // другой
					RETURN(op)
			}
		}
	}
	RETURN(0)
}
/*
_ERM_Object_ *FindObjExtArt(int artnum)
{
	int i;
	_ERM_Object_ *op;
	for(i=0;i<ERMOBJECTS;i++){
		op=&ERM_Object[i];
		if(op->Used==0) continue;
		switch(op->PosMix&0xF0000000){
			case 0x60000000: // артифакт
				if(artnum!=(op->PosMix&0x0000FFFF)) continue; // другой
				return op;
		}
	}
	return 0;
}
*/
_ERM_Object_ *AddObj(Dword MixPos)
{
	STARTNA(__LINE__, 0)
	int i,j;
	_ERM_Object_ *obj;

	obj=FindObj(MixPos);
	if(obj!=0) RETURN(obj)
	for(i=0;i<ERMOBJECTS;i++){
		if(ERM_Object[i].Used!=0) continue;
		for(j=0;j<8;j++) ERM_Object[i].Disabled[j]=0;
		ERM_Object[i].PosMix=MixPos;
		ERM_Object[i].Used=1;
		RETURN(&ERM_Object[i])
	}
	MError("Cannot add more objects.");
	RETURN(0)
}

void DelObj(_ERM_Object_ *obj)
{
	STARTNA(__LINE__, 0)
	int j;

	if(obj==0) RETURNV
	obj->Used=0;
	obj->HintVar=0;
	for(j=0;j<8;j++){
		obj->Disabled[j]=0;
		obj->AMessage[j][0]=0;
		obj->AMessage[j][1]=0;
	}
	obj->PosMix=0;
	RETURNV
}

void DelObjPos(Dword MixPos)
{
	STARTNA(__LINE__, 0)
	DelObj(FindObj(MixPos));
	RETURNV
}

void CheckDelObj(_ERM_Object_ *obj)
{
	STARTNA(__LINE__, 0)
	int j;

	if (obj == 0) RETURNV
	if (obj->HintVar != 0) RETURNV
	if (obj->Used == 0) RETURNV
	for (j=0; j<8; j++)
	{
		if (obj->Disabled[j] != 0) RETURNV
		if (obj->AMessage[j][0] != 0) RETURNV
		if (obj->AMessage[j][1] != 0) RETURNV
	}
	DelObj(obj);
	RETURNV
}

/*
Dword GetDinMixPos(_ToDo_ *sp)
{
	Dword MixPos;
	int x,y,l,ind;
	if(sp->Pointer2==1){
		VarNum *vnp=(VarNum *)&sp->Pointer;
		ind=GetVarVal(vnp);
		if((ind<1)||(ind>(1000-2))){ MError("Index of var for Dinamic position is out of range (1...998)."); return 0; }
		x=ERMVar2[ind-1];
		y=ERMVar2[ind];
		l=ERMVar2[ind+1];
		MixPos=PosMixed(x,y,l);
	}else{
		MixPos=sp->Pointer;
	}
	return MixPos;
}
*/
Dword GetDinMixPos(_ToDo_ *sp)
{
	STARTNA(__LINE__, 0)
	Dword MixPos;
	int x,y,l,ind;
	switch(sp->ParSet){
		case 1: // косвенная ссылка
//      VarNum *vnp=&sp->Par[0];
			ind=GetVarVal(&sp->Par[0]);
			if((ind<1)||(ind>(VAR_COUNT_V-2))){ MError("Index of var for Dinamic position is out of range (1...9998)."); RETURN(0) }
			x=ERMVar2[ind-1]; y=ERMVar2[ind]; l=ERMVar2[ind+1];
			MixPos=PosMixed(x,y,l);
			break;
		case 3: // x/y/l
			x=GetVarVal(&sp->Par[0]);
			y=GetVarVal(&sp->Par[1]);
			l=GetVarVal(&sp->Par[2]);
			MixPos=PosMixed(x,y,l);
			break;
		default: // t/st
			EWrongParamsNum(); RETURN(0)
/*
		VarNum *vnp=(VarNum *)&sp->Par[0];
		ind=GetVarVal(vnp);
		MixPos=ind<<16;
		vnp=(VarNum *)&sp->Par[1];
		ind=GetVarVal(vnp);
		MixPos+=ind&0xFFFF;
*/
		}
	RETURN(MixPos)
}

int ObjChangeHint(int MixPos, Mes *Mp, int ind) // 1 = ok, 0 = failed
{
	STARTNA(__LINE__, 0)
	_ERM_Object_ *obj;
	if (Mp->VarI[ind].Check == 0)
		obj = AddObj(MixPos);
	else
		obj = GetObj(MixPos);
	if(obj==0){ MError("Cannot add more objects"); RETURN(0) }
	char *tmpStr = 0;
	int var = obj->HintVar;
	StrMan::Apply(tmpStr, 0, var, Mp, ind);
	obj->HintVar = var;
	CheckDelObj(obj);
	RETURN(1)
}

int ERM_HintType(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	Dword alltype;
	int ind,val;
	if (sp->ParSet != 2) { MError("\"!!HT:\"-wrong syntax (the command requires 2 parameters)."); RETURN(0) }
	VarNum *vnp=&sp->Par[0];
	int type=GetVarVal(vnp);
	type&=0xFF;
	vnp=&sp->Par[1];
	int subtype=GetVarVal(vnp);
	if(subtype==-1) subtype=0xFF;
	subtype&=0xFF;
	switch(Cmd){
		case 'T': // Tz (z=0-delete)
//      if(Num<1){ EWrongParamsNum(); return 0; }
			alltype=(type<<8)+subtype+0x10000000;
			RETURN(ObjChangeHint(alltype, Mp, 0))
		case 'P': // Pval/z (z=0-delete) position->number (0...15)
			CHECK_ParamsMin(2);
			if(Apply(&val,4,Mp,0)) RETURN(0)
			if((val<0)||(val>15)){ MError("\"!!HT:P\"-Number value out of range (0...15)."); RETURN(0) }
			alltype=(type<<8)+subtype+0x20000000+((val&0x0F)<<16);
			RETURN(ObjChangeHint(alltype, Mp, 1))
		case 'W': // Wind/val/z (z=0-delete) w (0...99) = (0...32)
			CHECK_ParamsMin(3);
			if(Apply(&ind,4,Mp,0)) RETURN(0)
			if((ind<1)||(ind>100)){ MError("\"!!HT:W\"- W index value out of range (1...100)."); RETURN(0) }
			if(Apply(&val,4,Mp,1)) RETURN(0)
			if((val<0)||(val>32)){ MError("\"!!HT:W\"- W value out of range (0...32)."); RETURN(0) }
			alltype=(type<<8)+subtype+0x40000000+(((ind-1)&0x7F)<<16)+((val&0x1F)<<23);
			RETURN(ObjChangeHint(alltype, Mp, 2))
		case 'V': // Vind/val/z (z=0-delete) v  (1...1000) = (0...3)
			CHECK_ParamsMin(3);
			if(Apply(&ind,4,Mp,0)) RETURN(0)
			if((ind<1)||(ind>1000)){ MError("\"!!HT:V\"- V index value out of range (1...1000)."); RETURN(0) }
			if(Apply(&val,4,Mp,1)) RETURN(0)
			if((val<0)||(val>3)){ MError("\"!!HT:V\"- V value out of range (0...3)."); RETURN(0) }
			alltype=(type<<8)+subtype+((((ind-1)&0x3FF)<<16)&0x3FF0000)+(((val&0x3)<<26)&0x0C000000);
			alltype|=0x80000000;
			RETURN(ObjChangeHint(alltype, Mp, 2))
		default:
			EWrongCommand();
			RETURN(0)
	}
}

int ERM_SetObject(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	char en,fl;
	int  i,owner;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_ERM_Object_ *obj=0;

	switch(Cmd){
		case 'T': // T$ тип
			if (mip->OType == 34 && Num >= 2 && Mp->n[1] == 1)
				Apply(&GetHeroStr(mip->SetUp)->PlOType,4,Mp,0);
			else
				Apply(&mip->OType,2,Mp,0);
			break;
		case 'U': // U$ подтип
			Apply(&mip->OSType,2,Mp,0);
			break;
		case 'C': // C$ упр.слово
			if (mip->OType == 34 && Num >= 2 && Mp->n[1] == 1)
				Apply(&GetHeroStr(mip->SetUp)->PlSetUp,4,Mp,0);
			else
				Apply(&mip->SetUp,4,Mp,0);
			break;
		default:
//      obj=AddObj(MixPos); if(obj==0) RETURN(0)
			en=0;
			switch(Cmd){
				case 'M': // Mowner/auto/next
					CHECK_ParamsMin(3);
					owner=Mp->n[0];
					if((owner<-1)||(owner>7)){ MError("\"!!OB:M\"-owner out of range (-1...7)."); RETURN(0) }
					char ames[8][2]; FillMem(ames,sizeof(ames),0);
					fl=0;
					if(owner==-1){ // для всех
						for(i=0;i<8;i++){
							if(Apply(&ames[i][0],1,Mp,1)) fl=1;
							if(Apply(&ames[i][1],1,Mp,2)) fl=1;
						}
						if(fl) break;
						// 3.59 fix we are her only if not check syntax
						obj=AddObj(MixPos); if(obj==0) RETURN(0)
						for(i=0;i<8;i++){
							obj->AMessage[i][0]=ames[i][0];
							obj->AMessage[i][1]=ames[i][1];
						}
					}else{ // for a player
						if(Apply(&ames[owner][0],1,Mp,1)) fl=1;
						if(Apply(&ames[owner][1],1,Mp,2)) fl=1;
						if(fl) break;
						// 3.59 fix we are her only if not check syntax
						obj=AddObj(MixPos); if(obj==0) RETURN(0)
						obj->AMessage[owner][0]=ames[owner][0];
						obj->AMessage[owner][1]=ames[owner][1];
					}
					break;
				case 'D': // D# запретить
					en=1;
				case 'E': // E# разрешить
					CHECK_ParamsNum(1);
					owner=Mp->n[0];
					if(owner==-1) owner=CurrentUser();
					if((owner<0)||(owner>7)){ MError("\"!!OB:E\" or \"!!OB:D\"-owner out of range (-1...7)."); RETURN(0) }
					obj=AddObj(MixPos); if(obj==0) RETURN(0)
					obj->Disabled[owner]=en;
					break;
				case 'S': // S запретить всем
					en=1;
				case 'R': // R разрешить всем
//          if(Num!=1){ Error(); return 0; }
					obj=AddObj(MixPos); if(obj==0) RETURN(0)
					for(i=0;i<8;i++) obj->Disabled[i]=en;
					break;
				case 'H': // H$ установить подсказку
					RETURN(ObjChangeHint(MixPos, Mp, 0))
				case 'B': // B удалить подсказку
					obj = FindObj(MixPos);
					if (obj->HintVar < 0) StrMan::Remove(-obj->HintVar - 1);
					obj->HintVar = 0;
					CheckDelObj(obj);
//          DelObj(obj);
					break;
				default:
					EWrongCommand();
					RETURN(0)
			}
	}
	RETURN(1)
}
////////////////////////
static struct _ERM_Hero_{
	short HintVar;     // переменная подсказки 0-нет,1...
	char  Disabled[8]; // 1-dis 0-en по игрокам
} ERM_Hero[HERNUM];

int ERM_SetHero(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	char en;
	int  i,owner;
	int  ind=GetVarVal(&sp->Par[0]);

	if((ind<0)||(ind>=HERNUM)){ MError("\"!!HO:\"-number of hero out of range."); RETURN(0) }
	en=0;
	switch(Cmd){
		case 'D': // D# запретить
			en=1;
		case 'E': // E# разрешить
			if(Num!=1){ MError("\"!!HO:D\" or \"!!HO:E\"-insufficient parameters."); RETURN(0) }
			owner=Mp->n[0];
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!HO:D\" or \"!!HO:E\"-owner out of range (-1...7)."); RETURN(0) }
			ERM_Hero[ind].Disabled[owner]=en;
			break;
		case 'S': // S запретить всем
			en=1;
		case 'R': // R разрешить всем
			for(i=0;i<8;i++) ERM_Hero[ind].Disabled[i]=en;
			break;
		case 'H': // H$ установить подсказку
		{
			//Apply(&ERM_Hero[ind].HintVar,2,Mp,0);
			char *tmp = 0;
			int var = ERM_Hero[ind].HintVar;
			StrMan::Apply(tmp, 0, var, Mp, 0);
			ERM_Hero[ind].HintVar = var;
			break;
		}
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}
////////////////////////
int ERM_Owner(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int   i,j,owner,res,val,ind,act;
	_Hero_ *hp;
	_PlayerSetup_ *ps;
	char *pho=GetHOTableBase();
	Byte *str,vb;

	switch(Cmd){
		case 'R': // R#1/#2/$3 ресурсы #1(0...7,-1-текущий)
			CHECK_ParamsMin(3);
			owner=Mp->n[0]; res=Mp->n[1];
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:R\"-owner out of range (-1...7)."); RETURN(0) }
			if(res<0 || res>7){ MError("\"!!OW:R\"-resourse out of range."); RETURN(0) }
			if(res==7){ // Mithrill
				Apply(&MithrillVal[owner],4,Mp,2);
			}else{
				val=GetRes(owner,res);
				if(Apply(&val,4,Mp,2)) break;
				SetRes(owner,res,val);
			}
			break;
		case 'D': // D#1/#2 ресурсы #1(0...7,-1-текущий,-2-авто)
							//  #2 = 0...127,-1 -отменить
			CHECK_ParamsMin(2);
			owner=Mp->n[0];
			vb=(Byte)Mp->n[1];
			if(owner==-2){ //для всех
				//004F356B C60307 mov  byte ptr [ebx],07
				__asm{
					mov  ecx,0x4F356D
					mov  al,vb
					mov  [ecx],al
				}
			}else{ //конкретно
				str=GamerStruc(owner);
				str[0x3D]=vb;
			}
			break;
		case 'I': // Iowner/AIgm(/Killed) - AI или человек
			CHECK_ParamsMin(2);
			owner=Mp->n[0];
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:I\"-owner out of range (-1...7)."); RETURN(0) }
			val=IsAI(owner);
			Apply(&val,4,Mp,1);
			SetAI(owner,val);
			if(Num>2){
				val=IsPlayerKilled(owner,-1);
				if(Apply(&val,4,Mp,2)) break;
				IsPlayerKilled(owner,val);
			}
			break;
		case 'G': // Gowner/this - текущий игрок - тот, что за дисплаем или нет
			CHECK_ParamsNum(2);
			owner=Mp->n[0];
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:G\"-owner out of range (-1...7)."); RETURN(0) }
			val=IsThis(owner); Apply(&val,4,Mp,1); ThisIs(owner,val);
			break;
		case 'T': // Towner/team - команда
			CHECK_ParamsNum(2);
			owner=Mp->n[0];
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:T\"-owner out of range (-1...7)."); RETURN(0) }
			val=GetTeam(owner); Apply(&val,4,Mp,1); SetTeam(owner,(char)val);
			break;
		case 'H': // Howner/ind - посчитать и запомнить героев игрока
			CHECK_ParamsMin(2);
			owner=Mp->n[0];
			ind=Mp->n[1];
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:H\"-owner out of range (-1...7)."); RETURN(0) }
			if((ind<1)||(ind>VAR_COUNT_V)){ MError("\"!!OW:H\"-var index out of range (1...10000)."); RETURN(0) }
			if(Num==3){
				j=Mp->n[2];
				if(j<0){ MError("\"!!OW:H\"-third parameter below zero."); RETURN(0) }
				if(j==0){
					for(i=val=0;i<HERNUM;i++){
						hp=GetHeroStr(i); if(hp->Owner!=owner) continue;
						++val;
					}
					ERMVar2[ind-1]=val;
				}else{
					for(i=val=0;i<HERNUM;i++){
						hp=GetHeroStr(i); if(hp->Owner!=owner) continue;
						++val;
						if(val==j){ ERMVar2[ind-1]=i; break; }
					}
				}
			}else{
				for(i=val=0,j=ind+1;i<HERNUM;i++){
					hp=GetHeroStr(i);
					if(hp->Owner!=owner) continue;
					if(j>VAR_COUNT_V){ MError("\"!!OW:H\"-var index goes out of range (1...10000)."); RETURN(0) }
					ERMVar2[j-1]=i; ++j;
					++val;
				}
				ERMVar2[ind-1]=val;
			}
			break;
		case 'C': // C?owner - текущий игрок
			if(Num!=1) { MError("\"!!OW:C\"-wrong parameters number."); RETURN(0) }
			val=CurrentUser();
			Apply(&val,4,Mp,0);
			break;
		case 'K': // Kowner/keymaster/state - посещение кеймастеров
			if(Num<3){ MError("\"!!OW:K\"-wrong parameters number."); RETURN(0) }
			if(Apply(&owner,4,Mp,0)){ MError("\"!!OW:K\"-cannot get or check owner."); RETURN(0) }
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:K\"-owner out of range (-1...7)."); RETURN(0) }
			if(Apply(&ind,4,Mp,1)){ MError("\"!!OW:K\"-cannot get or check keymaster number."); RETURN(0) }
			if((ind<0)||(ind>7)){ MError("\"!!OW:K\"-keymaster out of range (-1...7)."); RETURN(0) }
			val=GetKeyMaster(owner,ind); Apply(&val,4,Mp,2); SetKeyMaster(owner,ind,val);
			break;
		case 'O': // Oowner/number/#1/#2/#3/#4/#5/#6/#7/#8
			if(Apply(&owner,4,Mp,0)){ MError("\"!!OW:O\"-cannot get or check owner."); RETURN(0) }
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:O\"-owner out of range (-1...7)."); RETURN(0) }
			ps=GetPlayerSetup(owner);
			if(ps==0){ MError("\"!!OW:O\"-cannot get PlayerSetup structure."); RETURN(0) }
			if(Num==4){ // O#owner/#action/#hero_ind_to be/#dummy first
				if(Apply(&act,4,Mp,1)) break; // action
				if(Apply(&ind,4,Mp,2)) break; // index
				__asm{
					mov    eax,0x6992B8
					mov    eax,[eax]
					mov    eax,[eax+0x44]
					mov    eax,[eax+0x64]
					add    ind,eax
				}
				if(ind>=ps->HasHeroes) break;
				if(ind<0) break;
				switch(act){
					case 1: // to first
						val=ps->Heroes[ind];
						for(i=ind+1;i<ps->HasHeroes;i++){ // delete
							ps->Heroes[i-1]=ps->Heroes[i];
						}
						for(i=ps->HasHeroes-1;i>0;i--){ // add
							ps->Heroes[i]=ps->Heroes[i-1];
						}
						ps->Heroes[0]=val;
						break;
					case 2: // one slot up
						if(ind<1) break;
						val=ps->Heroes[ind];
						ps->Heroes[ind]=ps->Heroes[ind-1];
						ps->Heroes[ind-1]=val;
						break;
					case 3: // one slot down
						if(ind>=(ps->HasHeroes-1)) break;
						val=ps->Heroes[ind];
						ps->Heroes[ind]=ps->Heroes[ind+1];
						ps->Heroes[ind+1]=val;
						break;
				}
				break;
			}else if(Num==3){ // O#owner/#ind/?h
				if(Apply(&ind,4,Mp,1)) break; // index
				__asm{
					mov    eax,0x6992B8
					mov    eax,[eax]
					mov    eax,[eax+0x44]
					mov    eax,[eax+0x64]
					add    ind,eax
				}
//        ind+=ps->TopHeroInd;
				if(ind>=ps->HasHeroes) val=-1; // no
				else{
					val=ps->Heroes[ind];
				}
				Apply(&val,4,Mp,2); 
				break;
			}else CHECK_ParamsMin(10);
			Apply(&ps->HasHeroes,1,Mp,1);
			for(i=0;i<8;i++){
				Apply(&ps->Heroes[i],4,Mp,(char)(i+2));
			}
			break;
		case 'V': // Vowner/heroleft/heroright to hire in a tavern
			CHECK_ParamsNum(3);
			owner=Mp->n[0];
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:V\"-owner out of range (-1...7)."); RETURN(0) }
			ps=GetPlayerSetup(owner);
			i=ps->THeroL; j=ps->THeroR;
			if(i!=-1) pho[i]=-1; if(j!=-1) pho[j]=-1;
			Apply(&i,4,Mp,1); Apply(&j,4,Mp,2);
			if(i!=-1) pho[i]=0x40; if(j!=-1) pho[j]=0x40;
			ps->THeroL=i; ps->THeroR=j;
			break;
		case 'A': // A#/$ (0...N,-1) Hero number
			CHECK_ParamsMin(2);
			if(Apply(&owner,4,Mp,0)){ MError("\"!!OW:A\"-cannot get or check owner."); RETURN(0) }
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:A\"-owner out of range (-1...7)."); RETURN(0) }
			ps=GetPlayerSetup(owner);
			if(ps==0){ MError("\"!!OW:A\"-cannot get PlayerSetup structure."); RETURN(0) }
			Apply(&ps->CurHero,4,Mp,1);
			break;
		case 'N': // N#/$ (0...N,-1) Town number
			if(Apply(&owner,4,Mp,0)){ MError("\"!!OW:N\"-cannot get or check owner."); RETURN(0) }
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:N\"-owner out of range (-1...7)."); RETURN(0) }
			ps=GetPlayerSetup(owner);
			if(ps==0){ MError("\"!!OW:N\"-cannot get PlayerSetup structure."); RETURN(0) }
			if(Num==4){ // N#owner/#action/#town_ind_to be first/dummy
				if(Apply(&act,4,Mp,1)) break; // action
				if(Apply(&ind,4,Mp,2)) break; // index
				__asm{
					mov    eax,0x6992B8
					mov    eax,[eax]
					mov    eax,[eax+0x44]
					mov    eax,[eax+0x68]
					add    ind,eax
				}
				if(ind>=ps->HasTowns) break;
				if(ind<0) break;
				switch(act){
					case 1: // to first
						val=ps->Towns[ind];
						for(i=ind+1;i<ps->HasTowns;i++){ // delete
							ps->Towns[i-1]=ps->Towns[i];
						}
						for(i=ps->HasTowns-1;i>0;i--){ // add
							ps->Towns[i]=ps->Towns[i-1];
						}
						ps->Towns[0]=val;
						break;
					case 2: // one slot up
						if(ind<1) break;
						val=ps->Towns[ind];
						ps->Towns[ind]=ps->Towns[ind-1];
						ps->Towns[ind-1]=val;
						break;
					case 3: // one slot down
						if(ind>=(ps->HasTowns-1)) break;
						val=ps->Towns[ind];
						ps->Towns[ind]=ps->Towns[ind+1];
						ps->Towns[ind+1]=val;
						break;
				}
				break;
			}else if(Num==3){ // N#owner/#ind/?t
				if(Apply(&ind,4,Mp,1)) break; // index
				__asm{
					mov    eax,0x6992B8
					mov    eax,[eax]
					mov    eax,[eax+0x44]
					mov    eax,[eax+0x68]
					add    ind,eax
				}
//        ind+=ps->TopTownInd;
				if(ind>=ps->HasTowns) val=-1; // no
				else{
					val=ps->Towns[ind];
				}
				Apply(&val,4,Mp,2);
				break;
			}else CHECK_ParamsMin(2);
			Apply(&ps->CurTown,1,Mp,1);
			break;
		case 'W': // W#/$(/$) Town number by position and full towns number
			CHECK_ParamsMin(2);
			if(Apply(&owner,4,Mp,0)){ MError("\"!!OW:W\"-cannot get or check owner."); RETURN(0) }
			if(owner==-1) owner=CurrentUser();
			if((owner<0)||(owner>7)){ MError("\"!!OW:W\"-owner out of range (-1...7)."); RETURN(0) }
			ps=GetPlayerSetup(owner);
			if(ps==0){ MError("\"!!OW:W\"-cannot get PlayerSetup structure."); RETURN(0) }
			if(Num==2){ // полное количество городов
				Apply(&ps->HasTowns,1,Mp,1);
			}else{ // >2 номер конкретного города (по порядку)
				if(Apply(&i,4,Mp,1)){ MError("\"!!OW:W\"-cannot get or check town list number."); RETURN(0) }
				if((i<0)||(i>=48)){ MError("\"!!OW:W\"-town list number out of range (0...47)."); RETURN(0) }
				Apply(&ps->Towns[i],1,Mp,2);
			}
			break;
		case 'S': // S#/#
			if(Apply(&ind,4,Mp,0)) break;
			switch(ind){
				case 1: // S1/#spell - действие спела на карте для тек героя
					if(Apply(&val,4,Mp,1)) break;
					CastAdvSpell(val);
					break;
				case 2: // S2/X - выбор спела на карте для текущего героя
					ChooseAdvSpell();
					break;
			}
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}

int ERM_Mine(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  owner;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CMine_ *msp=(_CMine_ *)mip;
	_Mine_  *mp;

	int tp=CheckPlace4Hero(mip,(Dword **)&msp);
	if((tp!=53)&&(tp!=42)){ MError("\"!!MN:\"-not a mine."); RETURN(0) }
	mp=GetMineStr(msp->Num);
	if(mp==0){ MError("\"!!MN:\"-mine is not found in internal structures."); RETURN(0) }
	switch(Cmd){
		case 'O': // O$ Owner O$/1 no redraw
			if(Mp->VarI[0].Check==0){ // установка
				owner=Mp->n[0]; if(owner==-2) owner=CurrentUser();
				if((owner<-1)||(owner>7)){ MError("\"!!MN:O\"-Owner out of range (-1...7)."); RETURN(0) }
				Mp->n[0]=owner;
			}
			Apply(&mp->Owner,1,Mp,0);
			if(Num<2) RedrawMap();
			break;
		case 'R': // R$ Resource type R$/1
			if(Mp->VarI[0].Check==0){ // установка
				if(((Mp->n[0]<0)||(Mp->n[0]>6))&&(Mp->n[0]!=100)){ MError("\"!!MN:R\"-Resource type out of range (0...6,100)."); RETURN(0) }
			}
			Apply(&mp->ResNum,1,Mp,0);
			if(Num<2) RedrawMap();
			break;
		case 'M': // M#/$/$ Monsters
			CHECK_ParamsMin(3);
			if((Mp->n[0]<0)||(Mp->n[0]>7)){ MError("\"!!MN:M\"-wrong slot number (0...7)."); RETURN(0) }
			Apply(&mp->GType[Mp->n[0]],4,Mp,1);
			Apply(&mp->GNumb[Mp->n[0]],4,Mp,2);
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}
int ERM_SetScoolar(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CSchoolar_ *stp=(_CSchoolar_ *)mip;
	if(mip->OType!=0x51/*)||(mip->OType!=0x22)*/){ MError("\"!!SC:\"-not a schoolar."); RETURN(0) }
	switch(Cmd){
		case 'T': // T# - тип 1,2,3 (3)
			v=stp->Type; Apply(&v,4,Mp,0); stp->Type=(Word)v; break;
		case 'P': // P# - перв. умение (3)
			v=stp->PSkill; Apply(&v,4,Mp,0); stp->PSkill=(Word)v; break;
		case 'S': // S# - втор. умение (7)
			v=stp->SSkill; Apply(&v,4,Mp,0); stp->SSkill=(Word)v; break;
		case 'L': // L# - заклинание (11)
			v=stp->Spell;  Apply(&v,4,Mp,0); stp->Spell=(Word)v;  break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetChest(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CChest_ *stp=(_CChest_ *)mip;
	if(mip->OType!=101){ MError("\"!!CH:\"-not a chest."); RETURN(0) }
	switch(Cmd){
		case 'S': // S# артефакт=1 или ресурс=0
			v=stp->HasArt; Apply(&v,4,Mp,0); stp->HasArt=(Word)v; break;
		case 'A': // A# номер арт.
			v=stp->ArtNum; Apply(&v,4,Mp,0); stp->ArtNum=(Word)v; break;
		case 'B': // B# кол-во бонуса
			v=stp->Bonus; Apply(&v,4,Mp,0);  stp->Bonus=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetWTomb(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CWTomb_ *stp=(_CWTomb_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x6C){ MError("\"!!WT:\"-not a warrior tomb."); RETURN(0) }
	switch(Cmd){
		case 'A': // A# номер арт. (10)
			v=stp->ArtNum; Apply(&v,4,Mp,0); stp->ArtNum=(Word)v; break;
		case 'S': // S# арт. есть=1 или нет=0 (1)
			v=stp->HasArt; if(Apply(&v,4,Mp,0)) break; stp->HasArt=(Word)v; stp->Whom=0; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetKTree(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CKTree_ *stp=(_CKTree_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x66){ MError("\"!!KT:\"-not a knowledge tree."); RETURN(0) }
	switch(Cmd){
		case 'S': // S# 0,1,2 (2)
			v=stp->Type; if(Apply(&v,4,Mp,0)) break; stp->Type=(Word)v; stp->Whom=0; break;
		case 'N': // N# номер (5)
			v=stp->Number; Apply(&v,4,Mp,0); stp->Number=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetFire(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CFire_ *stp=(_CFire_ *)mip;
	if(mip->OType!=0xC){ MError("\"!!FR:\"-not a campfire."); RETURN(0) }
	switch(Cmd){
		case 'B': // B#1/#2 (4)(>5)
			CHECK_ParamsMin(2);
			v=stp->ResType; Apply(&v,4,Mp,0); stp->ResType=(Word)v;
			v=stp->ResVal;  Apply(&v,4,Mp,1); stp->ResVal=(Word)v;
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetLean(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CLean_ *stp=(_CLean_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x27){ MError("\"!!LN:\"-not a lean."); RETURN(0) }
	switch(Cmd){
		case 'B': // B#1/#2 (4)(4)
			CHECK_ParamsMin(2);
			v=stp->ResType; Apply(&v,4,Mp,0); stp->ResType=(Word)v;
			v=stp->ResVal;  Apply(&v,4,Mp,1); stp->ResVal=(Word)v;
			break;
		case 'N': // N# номер (5)
			v=stp->Number;  Apply(&v,4,Mp,0); stp->Number=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetStone(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CLStone_ *stp=(_CLStone_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x64){ MError("\"!!ST:\"-not a stone."); RETURN(0) }
	switch(Cmd){
		case 'N': // N# номер камня (5)
			v=stp->Number; Apply(&v,4,Mp,0); stp->Number=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetWagon(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CWagon_ *stp=(_CWagon_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x69){ MError("\"!!WG:\"-not a wagon."); RETURN(0) }
	switch(Cmd){
		case 'S': // S - есть что-то=1 или нет ничего=0 (1)
			v=stp->HasBon; if(Apply(&v,4,Mp,0)) break; stp->HasBon=(Word)v; stp->Whom=0; break;
		case 'B': // B - арт.=1 или ресурс=0 (1)
			v=stp->HasArt; Apply(&v,4,Mp,0); stp->HasArt=(Word)v; break;
		case 'A': // A#1 (10)
			v=stp->ArtNum; Apply(&v,4,Mp,0); stp->ArtNum=(Word)v; break;
		case 'R': // R#1/#2 (4)(5)
			CHECK_ParamsMin(2);
			v=stp->ResType; Apply(&v,4,Mp,0); stp->ResType=(Word)v;
			v=stp->ResVal;  Apply(&v,4,Mp,1); stp->ResVal=(Word)v;
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_Pyramid(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Word b;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CPyram_  *stp=(_CPyram_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x3F){ MError("\"!!PM:\"-not a Pyramid or New Object."); RETURN(0) }
	switch(Cmd){
		case 'V': // V - непосещено=1 или посещено=0
			v=stp->Visited; Apply(&v,4,Mp,0); stp->Visited=(Word)v; break;
		case 'P': // P#/$ - кем посещено
			CHECK_ParamsMin(2);
			if((Mp->n[0]<0)||(Mp->n[0]>7)){ MError("\"!!PM:P\"-wrong player number (0...7)."); RETURN(0) }
			b=(Word)(1<<Mp->n[0]);
			if(b&stp->Whom) v=1; else v=0;
			if(Apply(&v,4,Mp,1)) break;
			if(v) stp->Whom|=b; else stp->Whom&=(Word)(~b);
			break;
		case 'S': // S$ заклинание
			v=stp->Spell; Apply(&v,4,Mp,0); stp->Spell=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetSkelet(char Cmd,int/*Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CSkelet_ *stp=(_CSkelet_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x16){ MError("\"!!SK:\"-not a skeleton."); RETURN(0) }
	switch(Cmd){
		case 'A': // A# артефакт (10)
			v=stp->ArtNum; Apply(&v,4,Mp,0); stp->ArtNum=(Word)v; break;
		case 'N': // N# установить номер (5)
			v=stp->Number; Apply(&v,4,Mp,0); stp->Number=(Word)v; break;
		case 'S': // S# есть арт.=1 или нет=0 (1)
			v=stp->HasArt; Apply(&v,4,Mp,0); stp->HasArt=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetSpring(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CMSpring_ *stp=(_CMSpring_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x30){ MError("\"!!SP:\"-not a spring."); RETURN(0) }
// НАСТРОЙКА 2-х соседних клеток
	switch(Cmd){
		case 'S': // S# заряжен=1 или нет=0
			v=stp->Power; Apply(&v,4,Mp,0); stp->Power=(Word)v; break;
		case 'N': // N# установить номер (5)
			v=stp->Number; Apply(&v,4,Mp,0); stp->Number=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetWMill(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CWMill_ *stp=(_CWMill_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x6D){ MError("\"!!WM:\"-not a water weel."); RETURN(0) }
	switch(Cmd){
		case 'B': // B# кол-во золота (5)
			v=stp->Bonus; if(Apply(&v,4,Mp,0)) break; stp->Bonus=(Word)v; stp->Whom=0; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetSwan(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CSSwan_ *stp=(_CSSwan_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x0E){ MError("\"!!SW:\"-not a swan pond."); RETURN(0) }
	switch(Cmd){
		case 'B': // B# кол-во удачи 1...3 (4)
			v=stp->BonLuck; if(Apply(&v,4,Mp,0)) break; stp->BonLuck=(Word)v; stp->Whom=0; break;
		case 'N': // N# установить номер (5)
			v=stp->Number; Apply(&v,4,Mp,0); stp->Number=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetMonolit(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CMonol_ *stp=(_CMonol_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x39){ MError("\"!!MT:\"-not a monolith."); RETURN(0) }
	switch(Cmd){
		case 'N': // N# установить номер (5)
			v=stp->Number; Apply(&v,4,Mp,0); stp->Number=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetGarden(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CMGarden_ *stp=(_CMGarden_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x37){ MError("\"!!GD:\"-not a garden."); RETURN(0) }
	switch(Cmd){
		case 'B': // B# тип бонуса (4) [обычно = 5]
			v=stp->ResType; Apply(&v,4,Mp,0); stp->ResType=(Word)v; break;
		case 'T': // T# ресурс=1 или золото=0 (1)
			v=stp->HasRes; Apply(&v,4,Mp,0); stp->HasRes=(Word)v; break;
		case 'N': // N# установить номер (5)
			v=stp->Number; Apply(&v,4,Mp,0); stp->Number=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetMill(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CMill_ *stp=(_CMill_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=0x70){ MError("\"!!ML:\"-not a mill."); RETURN(0) }
	switch(Cmd){
		case 'B': // B#/# тип бонуса    (4)(4)
			CHECK_ParamsMin(2);
			v=stp->ResType; Apply(&v,4,Mp,0); stp->ResType=(Word)v;
			v=stp->ResVal; Apply(&v,4,Mp,1); stp->ResVal=(Word)v;
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetDwelling(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  i;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	int *stp=(int *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if((tp!=17)&&(tp!=20)){ MError("\"!!DW:\"-not a Dwelling."); RETURN(0) }
	if((*stp<0)||(*stp>=GetDwellingNum())){ MError("\"!!DW\"-incorrect internal Dwelling number."); RETURN(0) }
	_Dwelling_ *dp=GetDwellingBase(); if(dp==0){ MError("\"!!DW:\"-Dwelling is not found in internal structures."); RETURN(0) }
	dp=&dp[*stp];
	switch(Cmd){
		case 'M': // M#1(0...3)/$2type/$3num монстр для наема
			CHECK_ParamsMin(3);
			i=Mp->n[0]; if((i<0)||(i>3)){ MError("\"!!DW:M\"-wrong monster slot (0...3)."); RETURN(0) }
			Apply(&dp->Mon2Hire[i],4,Mp,1);
			Apply(&dp->Num2Hire[i],2,Mp,2);
			break;
		case 'G': // M#1(0...7)/$2type/$3num монстр для охраны
			CHECK_ParamsMin(3);
			i=Mp->n[0]; if((i<0)||(i>7)){ MError("\"!!DW:G\"-wrong monster slot (0...7)."); RETURN(0) }
			Apply(&dp->GType[i],4,Mp,1);
			Apply(&dp->GNum[i],4,Mp,2);
			break;
		case 'O': // O$ хозяин (-1=ничей)
			if(Num<2){
				Apply(&dp->Owner,1,Mp,0);
			}else{
				CHECK_ParamsNum(2);
				i=dp->Owner; if(Apply(&i,1,Mp,0)) break;
				AdjustAllIfPassDwell(*stp,i);
//        if(Apply(&dp->Owner,1,Mp,0)) break;
			}
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetCrBank(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  i,ind,val,ar,retfl;
//  if(ERMVar2[0]==25 && ERMVar2[1]==20 && ERMVar2[2]==1){
//    __asm int 3
//  }
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CCrBank_ *stp=(_CCrBank_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=16 && tp!=25 && tp!=24 && tp!=84 && tp!=85){ MError("\"!!CB:\"-not a CrBank."); RETURN(0) }
	ind=stp->Ind;
	if((ind<0)||(ind>=GetCrBankNum())){ MError("\"!!CB\"-incorrect internal CrBank number."); RETURN(0) }
	_CrBankMap_ *dp=GetCrBankBase(); if(dp==0){ MError("\"!!CB:\"-Dwelling is not found in internal structures."); RETURN(0) }
	dp=&dp[ind];
	switch(Cmd){
		case 'M': // M$1type/$2num монстр для наема
			CHECK_ParamsMin(2);
			// 3.58f fix incorrect settings
//      Apply(&dp->BMonsterType,4,Mp,0);
//      Apply(&dp->BMonsterNum,1,Mp,1);
			val=dp->BMonsterType; Apply(&val,4,Mp,0); if(val<0) val=0; if(val>=MONNUM) val=MONNUM-1; dp->BMonsterType=val;
			val=dp->BMonsterNum;  Apply(&val,1,Mp,1); if(val<0) val=0; if(val>127) val=127; dp->BMonsterNum=val;
			break;
		case 'G': // M#1(0...7)/$2type/$3num монстр для охраны
			CHECK_ParamsMin(3);
			i=Mp->n[0]; if((i<0)||(i>7)){ MError("\"!!CB:G\"-wrong monster slot (0...7)."); RETURN(0) }
			Apply(&dp->DMonsterType[i],4,Mp,1);
			Apply(&dp->DMonsterNum[i],4,Mp,2);
			break;
		case 'R': // R#1(0...7)/$2num ресурсный бонус
			CHECK_ParamsMin(2);
			i=Mp->n[0]; if((i<0)||(i>7)){ MError("\"!!CB:R\"-wrong resource index (0...7)."); RETURN(0) }
			Apply(&dp->Res[i],4,Mp,1);
			break;
		case 'A': // A#ind/...
			i=Mp->n[0]; if((i<1)||(i>5)){ MError("\"!!CB:A\"-wrong subcommand (1...5)."); RETURN(0) }
			switch(i){
				case 1: // A1/?var - number of arts
					if(Num<2){ MError("\"!!CB:A1\"-wrong syntax."); RETURN(0) }
					val=dp->Arts.GetNum();
					Apply(&val,4,Mp,1);
					break;
				case 2: // A2/#/$ - get/set art and subart at position #
					if(Num<3){ MError("\"!!CB:A2\"-wrong syntax."); RETURN(0) }
					retfl=0;
					if(Apply(&val,4,Mp,1)) break;
					ar=dp->Arts.Get(val);
					if(Apply(&ar,4,Mp,2)) retfl=1;
					if(retfl) break;
					dp->Arts.Set(val,ar);
					break;
				case 3: // A3/$ - add artifact
					if(Num<2){ MError("\"!!CB:A3\"-wrong syntax."); RETURN(0) }
					if(Apply(&val,4,Mp,1)) break;
					dp->Arts.Add(val);
					break;
				case 4: // A4/$ - del art
					if(Num<2){ MError("\"!!CB:A4\"-wrong syntax."); RETURN(0) }
					if(Apply(&val,4,Mp,1)) break;
					dp->Arts.Del(val);
					break;
				default:
					EWrongSyntax(); RETURN(0) 
			}
			break;
		case 'T': // T$ взят уже (1) или нет (0)
			i=stp->Taken;
			if(Apply(&i,4,Mp,0)) break;
			if(i) stp->Taken=1; else stp->Taken=0;
			break;
		case 'V': // V$ посещен игроком (1) или нет (0) [битовые маски]
			i=stp->Whom;
			if(Apply(&i,4,Mp,0)) break;
			stp->Whom=i;
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_SetWHat(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CWHat_ *stp=(_CWHat_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=113){ MError("\"!!WH:\"-not a witch hut."); RETURN(0) }
	switch(Cmd){
		case 'S': // S$ втор.навык
//      if((Mp->n[0]<0)||(Mp->n[0]>=28)){ MError("\"!!WH:S\"-wrong skill index (0...27)."); return 0; }
			v=stp->SSkill; Apply(&v,4,Mp,0); stp->SSkill=v;
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_Shipyard(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CShipyard_ *stp=(_CShipyard_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=87){ MError("\"!!SY:\"-not a shipyard."); RETURN(0) }
	switch(Cmd){
		case 'O': // O$ хозяин
//      if((Mp->n[0]<-1)||(Mp->n[0]>7)){ MError("\"!!SY:O\"-wrong owner (-1...7)."); return 0; }
			v=stp->Owner; Apply(&v,4,Mp,0); stp->Owner=(Word)v;
			break;
		case 'P': // P$/$ позиция для лодки
			CHECK_ParamsMin(2);
			v=stp->xs; Apply(&v,4,Mp,0); stp->xs=(Word)v;
			v=stp->ys; Apply(&v,4,Mp,1); stp->ys=(Word)v;
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_Garrison(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  i,v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	int *stp=(int *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=33){ MError("\"!!GR:\"-not a garrison."); RETURN(0) }
	if((*stp<0)||(*stp>=GetHornNum())){ MError("\"!!GR\"-wrong internal Garrison index."); RETURN(0) }
	_Horn_ *dp=GetHornBase(); if(dp==0){ MError("\"!!GR:\"-Garrison is not found in internal structures."); RETURN(0) }
	dp=&dp[*stp];
	switch(Cmd){
		case 'O': // O$ хозяин
//      if((Mp->n[0]<-1)||(Mp->n[0]>7)){ MError("\"!!GR:O\"-wrong owner (-1...7)."); return 0; }
			Apply(&dp->Owner,1,Mp,0);
			break;
		case 'G': // M#1(0...6)/$2type/$3num монстр для охраны
			CHECK_ParamsMin(3);
			i=Mp->n[0]; if((i<0)||(i>=7)){ MError("\"!!GR:G\"-wrong slot number (0...6)."); RETURN(0) }
			Apply(&dp->GType[i],4,Mp,1);
			Apply(&dp->GNumb[i],4,Mp,2);
			break;
		case 'F': // F$ нельзя забирать
			v=dp->NotRem; Apply(&v,4,Mp,0); dp->NotRem=(char)v;
			break;
//    case 'N': // 3.58 N$ номер гарнизона в списке - только проверка
//      v=*stp; Apply(&v,4,Mp,0);
//      break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_Shrine(char Cmd,int/* Num*/,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CShrine_ *stp=(_CShrine_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if((tp!=88)&&(tp!=89)&&(tp!=90)){ MError("\"!!SR:\"-not a shrine."); RETURN(0) }
	switch(Cmd){
		case 'S': // S$ заклинание
			v=stp->Spell; Apply(&v,4,Mp,0); stp->Spell=(Word)v; break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_Sign(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	int *stp=(int *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if((tp!=59)&&(tp!=91)){ MError("\"!!SG:\"-not a sign or bottle."); RETURN(0) }
	if((*stp<0)||(*stp>=GetSignNum())){ MError("\"!!SG:\"-wrong internal sign or bottle index."); RETURN(0) }
	_Sign_ *dp=GetSignBase(); if(dp==0){ MError("\"!!SG:\"-Sign or Bottle is not found in internal structures."); RETURN(0) }
	dp=&dp[*stp];
	switch(Cmd){
		case 'M': // M^text^ , M1/$
			if(Num==2){ // с переменной
				if(Mp->n[0]==1){
					if(Mp->n[1]==-1){ // M1/-1
						MesMan(&dp->Mes,0,0);
						dp->HasMess=0;
					}else{ // M1/$
						ApplyString(Mp,1,&dp->Mes);
					}
				}else{ EWrongSyntax(); RETURN(0) }
			}else{
				if(NewMesMan(Mp,&dp->Mes,0)){ MError("\"!!SG:M\"-cannot set message."); RETURN(0) }
				if(dp->Mes.m.s!=0) dp->HasMess=1; // есть сообщение
				else dp->HasMess=0; // нет сообщения
			}
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_Univer(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int i;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	_CUniver_ *stp=(_CUniver_ *)mip;
	int tp=CheckPlace4Hero(mip,(Dword **)&stp);
	if(tp!=104){ MError("\"!!UR:\"-not a univercity."); RETURN(0) }
	if((int)stp->Num>=GetUniverNum()){ MError("\"!!UR:\"-wrong internal univercity index."); RETURN(0) }
	_Univer_ *dp=GetUniverBase(); if(dp==0){ MError("\"!!UR:\"-University is not found in internal structures."); RETURN(0) }
	dp=&dp[stp->Num];
	switch(Cmd){
		case 'S': // S$/$/$/$
			CHECK_ParamsNum(4);
			for(i=0;i<4;i++){ Apply(&dp->SSkill[i],4,Mp,(char)i); }
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

int ERM_Terrain(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v,x,y,l;
	Dword MixPos=GetDinMixPos(sp);
	_MapItem_ *mip=GetMapItem2(MixPos);
	switch(Cmd){
		case 'G': //G?var - получить модиф. поверхности 3.58
			CHECK_ParamsMax(2);
			int stype;
			v=GetMapItemModif(mip, stype);
			Apply(&v,4,Mp,0);
			if (Num>1) Apply(&stype,4,Mp,1);
			break;
		case 'T': // Tl/lt/r/rt/d/dt/m/mt - тип пов
			CHECK_ParamsNum(8);
			Apply(&mip->Land,1,Mp,0);
			Apply(&mip->LType,1,Mp,1);
			Apply(&mip->River,1,Mp,2);
			Apply(&mip->RType,1,Mp,3);
			Apply(&mip->Road,1,Mp,4);
			Apply(&mip->RdType,1,Mp,5);
			Apply(&mip->Mirror,1,Mp,6);
			Apply(&mip->Attrib,1,Mp,7);
			break;
		case 'P': // P$ - проходимость
			CHECK_ParamsNum(1);
			if(mip->Attrib&0x01) v=0; else v=1;
			if(Apply(&v,4,Mp,0)) break;
			if(v==0) mip->Attrib|=0x01;
			else     mip->Attrib&=~(0x01);
			break;
		case 'E': // E$ - вход
			if (Num == 3)
			{
				mip = GetObjectEntrance(MixPos);
				Map2Coord(mip, &x, &y, &l);
				Apply(&x, 4, Mp, 0);
				Apply(&y, 4, Mp, 1);
				Apply(&l, 4, Mp, 2);
				break;
			}
			CHECK_ParamsNum(1);
			if(mip->Attrib&0x10) v=0; else v=1;
			if(Apply(&v,4,Mp,0)) break;
			if(v==0) mip->Attrib|=0x10;
			else     mip->Attrib&=~(0x10);
			break;
		case 'V': // V$ - видимость
			MixedPos(&x,&y,&l,MixPos);
			v=GetVisability(x,y,l);
			if(Apply(&v,4,Mp,0)) break;
			SetVisability(x,y,l,v);
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
int ERM_Position(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  i,j,k,sz,lv,v,x,y,l,num;
	Dword w;
	Dword MixPos=GetDinMixPos(sp);
	MixedPos(&x,&y,&l,MixPos);
	sz=GetMapSize(); lv=GetMapLevels();
	if((x<0)||(x>=sz)){ MError("\"!!PO\"-wrong position (x)."); RETURN(0) }
	if((y<0)||(y>=sz)){ MError("\"!!PO\"-wrong position (y)."); RETURN(0) }
	if((l<0)||(l>lv)) { MError("\"!!PO\"-wrong position (l)."); RETURN(0) }
	_Square_ *sqp=&Square[x][y][l];
	_Square2_ *sqp2=&Square2[x][y][l];
	switch(Cmd){
		case 'H': // H$ герой 0...255
			CHECK_ParamsNum(1);
			w=sqp->Hero; Apply(&w,4,Mp,0); sqp->Hero=(Word)w;
			break;
		case 'O': // O$ хозяин -8...7
			CHECK_ParamsNum(1);
			v=sqp->Owner; Apply(&v,4,Mp,0); sqp->Owner=v;
			break;
		case 'N': // N$ число 0...15
			CHECK_ParamsNum(1);
			w=sqp->Number; Apply(&w,4,Mp,0); sqp->Number=(Word)w;
			break;
		case 'T': // T$ число 0...255 тип
			CHECK_ParamsNum(1);
			w=sqp->NumberT; Apply(&w,4,Mp,0); sqp->NumberT=(Word)w;
			break;
		case 'S': // S$ число 0...255 подтип
			CHECK_ParamsNum(1);
			w=sqp->NumberS; Apply(&w,4,Mp,0); sqp->NumberS=(Word)w;
			break;
		case 'C': // Ct/st/h/o/n (-1 - no matter)
			CHECK_ParamsNum(5);
			num=0;
			for(i=0;i<sz;i++){
				for(j=0;j<sz;j++){
					for(k=0;k<=lv;k++){
						sqp=&Square[i][j][k];
						if(Mp->n[0]!=-1){ if((int)sqp->NumberT!=Mp->n[0]) continue; }
						if(Mp->n[1]!=-1){ if((int)sqp->NumberS!=Mp->n[1]) continue; }
						if(Mp->n[2]!=-1){ if((int)sqp->Hero!=Mp->n[2]) continue; }
						if(Mp->n[3]!=-1){ if((int)sqp->Owner!=Mp->n[3]) continue; }
						if(Mp->n[4]!=-1){ if((int)sqp->Number!=Mp->n[4]) continue; }
						++num;
					}
				}
			}
			ERMVar2[0]=num;
			break;
		case 'V': // V#/$ число short -32000...32000
			CHECK_ParamsNum(2);
			if(Apply(&i,4,Mp,0)){ MError("\"!!PO:V\"-you cannot use get or check syntax for the first argument."); RETURN(0) }
			if((i<0)||(i>3)){ MError("\"!!PO:V\"-wrong index (0...3)."); RETURN(0) }
			v=sqp2->S[i]; Apply(&v,4,Mp,1); sqp2->S[i]=(short)v;
			break;
		case 'B': // B#/$ число long -4g...4g
			CHECK_ParamsNum(2);
			if(Apply(&i,4,Mp,0)){ MError("\"!!PO:B\"-you cannot use get or check syntax for the first argument."); RETURN(0) }
			if((i<0)||(i>1)){ MError("\"!!PO:B\"-wrong index (0...1)."); RETURN(0) }
			v=sqp2->L[i]; Apply(&v,4,Mp,1); sqp2->L[i]=v;
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

#define QWESTLOGNUM 1000
struct _QuestLog_{
	int  num; //0-free
	int  z;
	int  hero;
	int  owner;
}QuestLog[QWESTLOGNUM];

_QuestLog_ *FindQuest(int num,int hero,int owner)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<QWESTLOGNUM;i++){
		if(QuestLog[i].num!=num) continue;
		if(QuestLog[i].hero!=hero) continue;
		if(QuestLog[i].owner!=owner) continue;
		RETURN(&QuestLog[i])
	}
	RETURN(0)
}

_QuestLog_ *AddQuest(int num,int hero,int owner)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<QWESTLOGNUM;i++){
		if(QuestLog[i].num!=0) continue;
		QuestLog[i].num=num;
		QuestLog[i].hero=hero;
		QuestLog[i].owner=owner;
		RETURN(&QuestLog[i])
	}
	RETURN(0)
}

void DelQuest(_QuestLog_ *qlp)
{
	STARTNA(__LINE__, 0)
	qlp->num=0;
	qlp->hero=0;
	qlp->owner=0;
	qlp->z=0;
	RETURNV
}
_QuestLog_ *GetQuest(int hero,int owner,int *ind)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=*ind;i<QWESTLOGNUM;i++){
		if(QuestLog[i].num==0) continue;
		if((QuestLog[i].hero!=hero)&&(QuestLog[i].hero!=-2)) continue;
		if((QuestLog[i].owner!=owner)&&(QuestLog[i].owner!=-2)) continue;
		*ind=i+1;
		RETURN(&QuestLog[i])
	}
	RETURN(0)
}

int ERM_Qwest(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int num,hero,owner;
	_QuestLog_ *qlp;
	switch(Cmd){
		case 'A': // Anum/hero/owner/zvar;
		{
			CHECK_ParamsNum(4);
			if(Apply(&num,4,Mp,0)) { MError("\"!!QW:A\"-cannot get or check num."); RETURN(0) }
			if(Apply(&hero,4,Mp,1)) { MError("\"!!QW:A\"-cannot get or check hero number."); RETURN(0) }
			if(Apply(&owner,4,Mp,2)) { MError("\"!!QW:A\"-cannot get or check owner."); RETURN(0) }
			if(num<1){ MError("\"!!QW:A\"-Qwest number is incorrect (>0)."); RETURN(0) }
			if((hero<-2)||(hero>=HERNUM)){ MError("\"!!QW:A\"-hero number is incorrect."); RETURN(0) }
			if(hero==-1){
				hero=ERM_HeroStr->Number;
				if((hero<0)||(hero>=HERNUM)){ MError("\"!!QW:A\"-no current hero."); RETURN(0) }
			}
			if((owner<-2)||(owner>7)){ MError("\"!!QW:A\"-owner is incorrect (<-2 or >7)."); RETURN(0) }
			if(owner==-1) owner=CurrentUser();
			qlp=FindQuest(num,hero,owner);
			if(qlp==0) qlp=AddQuest(num,hero,owner);
			if(qlp==0){ MError("\"!!QW:A\"-cannot add one more quest."); RETURN(0) }
			if (Mp->VarI[3].Type != 7 && GetVarVal(&Mp->VarI[3]) == -1) // StrMan::Apply treats 0 as "restore original", not -1
				Mp->VarI[3].IType = Mp->VarI[3].Type = Mp->VarI[3].Num = Mp->n[3] = 0;

			char * str = StrMan::GetStoredStr(qlp->z);
			StrMan::Apply(str, 0, qlp->z, Mp, 3);
			if(qlp->z==0) DelQuest(qlp);
			break;
		}
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

int ERM_StackExperience(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int x,y,l,v,m,n,fl,hind,art,opt;
	int Type,Slot,MType=-1,MNum=0;
	CRLOC Crloc;
	CrExpo *Cr,*Cr2,CrV;
//  ERMFlags[0]=0;
	// should set Type,Crloc,Slot
	switch(sp->ParSet){
//    case 1: // индекс стэка в структуре опыта
//      Ind=GetVarVal(&sp->Par[0]);
//      if(CrExpoSet::FindType(Ind,&Type,&Crloc,&Slot)==0){ MError("\"!!EX\"- incorrect index"); RETURN(0) }
//      break;
		case 2: // герой/стэк
			hind=GetVarVal(&sp->Par[0]);
			if(hind<-1 || hind>=HERNUM){ MError("\"!!EX\"- incorrect hero index"); RETURN(0) }
			if(hind==-1){
				if(ERM_HeroStr==0){ MError("\"!!EX\"- default hero is not available here"); RETURN(0) }
				else hind=ERM_HeroStr->Number;
			}
			Slot=GetVarVal(&sp->Par[1]);
			if(Slot<0 || Slot>=7){ MError("\"!!EX\"- incorrect index"); RETURN(0) }
			Type=CE_HERO; Crloc=MAKEHS(hind,Slot);
			MType=GetHeroStr(hind)->Ct[Slot];
			MNum =GetHeroStr(hind)->Cn[Slot];
//      Ind=CrExpoSet::FindIt(Type,Crloc);
//      if(Ind<0){ MError("\"!!EX\"- incorrect index"); RETURN(0) }
			break;
		case 4: // something at x/y/l/n(/type)
		case 5: 
			if(sp->ParSet==5){
				Type=GetVarVal(&sp->Par[4]);
				switch(Type){
					case 1:  Type=CE_HERO; break;
					case 2:  Type=CE_TOWN; break;
					case 3:  Type=CE_MINE; break;
					case 4:  Type=CE_HORN; break;
					default: Type=-1;
				}
			}else{
				Type=-1; // undefined
			}
			x=GetVarVal(&sp->Par[0]);
			y=GetVarVal(&sp->Par[1]);
			l=GetVarVal(&sp->Par[2]);
			Slot=GetVarVal(&sp->Par[3]);
			if(Slot<0 || Slot>=7){ ERMFlags[0]=1; RETURN(0) }
			if(CrExpoSet::FindType(x,y,l,Slot,&Type,&Crloc,&MType,&MNum)==0){ MError("\"!!EX\"- cannot find object at location"); RETURN(0) }
			break;
		case 0:
		case 1: // индекс стэка в структуре опыта
		case 3: // type/index/slot
			MError("\"!!EX\"-unsupported syntax."); RETURN(0)
	}
	Cr=CrExpoSet::Find(Type,Crloc);
	// Cr may be 0 !!!!
	if(Cr==0){
		Cr2=CrExpoSet::FindEmpty();
		if(Cr2==0){ MError("\"!!EX\"-no more room in experience story."); RETURN(0) }
		CrV.Clear();
		CrV.SetN(Type,Crloc,MType,MNum,0);
	}else{
		Cr2=Cr;
		CrV=*Cr;
	}
	switch(Cmd){
		case 'E': // Experience
			v=CrV.Expo; if(Apply(&v,4,Mp,0)) break;
			CrV.Expo=v; *Cr2=CrV;
			break;
		case 'N': // Number of creatures
			v=CrV.Num;  if(Apply(&v,4,Mp,0)) break;
			CrV.Num=v; *Cr2=CrV;
			break;
		case 'T': // Monster Type
			v=CrV.Fl.MType; if(Apply(&v,4,Mp,0)) break;
			CrV.Fl.MType=(Word)v; *Cr2=CrV;
			break;
//    case 'O': // Owner Type
//      if(Cr==0) v=0; else v=Cr->Fl.Type;
//      if(Apply(&v,4,Mp,0)) break;
//      if(Cr==0){ Cr=Cr2; Cr->SetN(Type,Crloc,0,0,0); }
//      Cr->Fl.Type=v;
//      break;
		case 'A': // all info: MType/MNum/Expo
			CHECK_ParamsMin(3);
			m=CrV.Fl.MType; n=CrV.Num; v=CrV.Expo;
			fl=0;
			if(Apply(&m,4,Mp,0)) ++fl;
			if(Apply(&n,4,Mp,1)) ++fl;
			if(Apply(&v,4,Mp,2)) ++fl;
			if(fl==3) break;
//      if(m==-1){ // не задан тип - можем только уст опыт для стэка, если он уже был
			CrV.Fl.MType=(Word)m;
			CrV.Num=n;
			CrV.Expo=v;
			*Cr2=CrV;
			break;
		case 'R': // art/subart
			CHECK_ParamsMin(2);
			if(Num==4){ // new syntax - has/art/subart/copies
				int h,a,s,c,fl;
				h=CrV.HasArt();
				a=CrV.GetArt();
				s=CrV.GetSubArt();
				c=CrV.ArtCopy();
				fl=4;
				if(Apply(&h,4,Mp,0)) --fl;
				if(Apply(&a,4,Mp,1)) --fl;
				if(Apply(&s,4,Mp,2)) --fl;
				if(Apply(&c,4,Mp,3)) --fl;
				if(fl==0) break;
				CrV.SetArtAll(h,a,s,c);
				*Cr2=CrV;
				break;
			}
			if(CrV.HasArt()){ art=CrV.GetArt(); opt=CrV.GetSubArt(); }
			else{ art=-1; opt=0; }
			fl=0;
			if(Apply(&art,4,Mp,0)) fl=1;
			if(Apply(&opt,4,Mp,1)) fl=1;
			if(fl) break;
			if(art==-1){ // no art
				CrV.DelArt();
			}else{
				CrV.SetArt(art,opt);
			}
			*Cr2=CrV;
			break;
		case 'C': // combine with other stack
			{
				int nomes=0,lt,lhind,lx,ly,ll,lslot,ltype,lmtype,lmnum;
				CRLOC lcrloc;
//  CrExpo *Cr,*Cr2,CrV;
				if(Apply(&lt,4,Mp,0)) break;
				ERMVar2[0]=0;
				switch(lt){
					case 0: // 0/герой/стэк(/nomes)
					CHECK_ParamsMin(3);
					 if(Apply(&lhind,4,Mp,1)) RETURN(1)
					 if(lhind<-1 || lhind>=HERNUM){ MError("\"!!EX:C\"- incorrect hero index"); RETURN(0) }
					 if(lhind==-1){
							if(ERM_HeroStr==0){ MError("\"!!EX:C\"- default hero is not available here"); RETURN(0) }
							else lhind=ERM_HeroStr->Number;
					 } 
					 if(Apply(&lslot,4,Mp,2)) RETURN(1)
					 ltype=CE_HERO; lcrloc=MAKEHS(lhind,lslot);
					 if(CrExpoSet::Find(Type,Crloc)==0){
						 // not found - nothing to do
//             RETURN(1)
						 lmtype=MType; lmnum=0; // undefined!!!
					 }
					 lmtype=GetHeroStr(lhind)->Ct[lslot];
					 lmnum =GetHeroStr(lhind)->Cn[lslot];
					 if(Num>3) Apply(&nomes,4,Mp,3);
					 if(lmtype!=MType){ 
						 ERMVar2[0]=-1;
						 if(nomes){ 
							 RETURN(1)
						 }else{
							 MError("\"!!EX:C\"- stacks have a different creatures"); RETURN(0) 
						 }
					 }
					 break;
				 case 1: // hero at 1/x/y/l/n(/nomes)
				 case 2: // town at 2/x/y/l/n(/nomes)
				 case 3: // mine at 3/x/y/l/n(/nomes)
				 case 4: // garn at 4/x/y/l/n(/nomes)
					CHECK_ParamsMin(5);
					 if(Apply(&lx,4,Mp,1)) RETURN(1)
					 if(Apply(&ly,4,Mp,2)) RETURN(1)
					 if(Apply(&ll,4,Mp,3)) RETURN(1)
					 if(Apply(&lslot,4,Mp,4)) RETURN(1)
					 if(lslot<0 || lslot>=7){ MError("\"!!EX:C\"- incorrect slot index"); RETURN(0) }
					 switch(lt){
						 case 1:  ltype=CE_HERO; break;
						 case 2:  ltype=CE_TOWN; break;
						 case 3:  ltype=CE_MINE; break;
						 case 4:  ltype=CE_HORN; break;
						 default: ltype=-1;
					 }
					 if(CrExpoSet::FindType(lx,ly,ll,lslot,&ltype,&lcrloc,&lmtype,&lmnum)==0){  
						 // not found - nothing to do
//             RETURN(1)
						 lmtype=MType; lmnum=0; // undefined!!!
					 }
					 if(Num>5) Apply(&nomes,4,Mp,5);
					 if(lmtype!=MType){ 
						 ERMVar2[0]=-1;
						 if(nomes){ 
							 RETURN(1)
						 }else{
							 MError("\"!!EX:C\"- stacks have a different creatures"); RETURN(0) 
						 }
					 }
					 break;
				 default:
					 MError("\"!!EX:C\"-unsupported syntax."); RETURN(0)
				}
				// add this stack to main
				ERMVar2[0]=CrExpoSet::HCombReal(ltype,Type,lcrloc,Crloc,lmtype,MType,lmnum,MNum);
			}
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}


int ERM_HeroGainLevel(char Cmd,int Num,_ToDo_*sp,Mes *Mp);
int ERM_MouseClick(char Cmd,int Num,_ToDo_*sp,Mes *Mp);
int ERM_MouseMove(char Cmd,int Num,_ToDo_*sp,Mes *Mp);
int ERM_HintDisplay(char Cmd,int Num,_ToDo_*sp,Mes *Mp);

///////////////////////////////////////////////
/// ERM ERM ERM
///////////////////////////////////////////////
struct _ERM_Trigger_{ // triggers with 1 or no parameters
	Word Id;
	int Event; // the start of events range
	int paramMin;
	int paramMax;
	Byte post;
} ERM_Triggers[]=
{
	{'IP',30330,0,3},          // IP#;
	{'BA',30300,0,1},          // BA#;
	{'BA',30350,50,54},        // BA#;
	{'BF',30800,0,1},          // BF#;
	{'BR',30302},              // BR;
	{'MM',30317,0,1},          // MM; перем мауса (0-битва,1-город)
	{'AE',30315,0,1},          // AE; одевание артифакта (0-снять,1-одеть)
	{'BG',30303,0,1},          // BG;
	{'CM',30310,0,4},          // CM;
	{'CM',30319,5,5},          // CM;
	{'MW',30305,0,1},          // MW#;
	{'TH',30324,0,1},          // TH#;
	{'HE',30100,0,HERNUM-1},   // HE#;
	{'HM',30400,-1,HERNUM-1},  // HM#; -1 - every hero moves
	{'HL',30600,-1,HERNUM-1},  // HL#; -1 - every hero gains level
	{'CO',30340,0,3},          // CO
	{'MP',30320},              // MP; переключение MP3
	{'MG',30322,0,1},          // MG; magic casting on adv. map
	{'SN',30321},              // SN; переключение Sound
// 3.58
	{'MR',30307,0,2},          // MR; Magic Resistance in Battle
	{'MF',30801,0,1},          // MF; Monster abilities in a Battle
	{'GM',30360,0,1},          // GM#; After Load(0), Before Save(1) game trigger
	{'PI',30370},              // PI; post instruction call
// 3.59
	{'TL',30900,0,4},          // TL#; timer # is number
	{'DL',30371},              // DL; dialog call back
	{'HD',30372},              // HD; Hint Display - Get Hint Text
	{'CI',30373,0,1},          // CI; Castle Income
	{'FC',30375},              // FC; Flag Color
	{'DG',30376},              // DG; Dig Grail
	{'HL',31200,-1,HERNUM-1,true},  // HL#; -1 - every hero gains level post-trigger
	{'AI',30377},              // AI; Get Map Position Importance
	{0,0}
};

//////////////////////////////
// 0-30000 functions
// 30000-30100 timers
// 30100-30300 heroes (!!! must change for new town)
// 30300 to battle
// 30301 from battle
// 30302 next battle turn
// 30303 monster's action
// 30304 monster's after action
// 30305 WM reach dest
// 30306 WM killed
// 30307 MR Magic Resistance in Battle (pre-trigger)
// 30308 MR Magic Resistance in Battle (post-trigger)
// 30309 MR Dwarf Magic Resistance
// 30310 mouse click на карте (правый) !?CM0
// 30311 mouse click в Городе !?CM1
// 30312 mouse click in hero screen !?CM2
// 30313 mouse click it two heroes screen !?CM3
// 30314 mouse click in battlefield !?CM4
// 30315 игрок снял артифакт
// 30316 игрок одел артифакт
// 30317 mouse move over battlefield
// 30318 mouse move over townscreen
// 30319 mouse click на карте (левый) !?CM5
// 30320 MP3 music
// 30321 WAV and M82 Sound 3.58
// 30322 Magic cast (adv. map) (pre-trigger)
// 30323 Magic cast (adv. map) (post-trigger)
// 30324 Go to Town Hall Screen 3.58 !?TH0
// 30325 Leave Town Hall Screen 3.58 !?TH1
// 30330 MP before sending data before battle
// 30331 MP after receiving data before battle
// 30332 MP before sending data after battle
// 30333 MP after receiving data after battle
// 30334 MP received data during a battle
// 30340 CO Перед открытием диалога Командира
// 30341 CO После закрытия диалога Командира
// 30342 CO После покупки Командира в городе
// 30343 CO После воскрешения Командира в городе
// 30350 to battle
// 30351 from battle
// 30352 universal to battle
// 30353 universal from battle
// 30354 on show battlefield (!?BA54)
// 30360 Load Game
// 30361 Save Game
// 30370 Post Instruction call (только при старте новой карты, но не загрузки)
// 30371 Dlg CallBack
// 30372 Hint text callback (!?HD)
// 30373 Castle Income (!?CI0)
// 30374 Castle Monsters Growth (!?CI1)
// 30375 Flag Color (!?FC)
// 30376 Dig Grail (!?DG)
// 30377 Get Map Position Importance for AI (!?AI)
// ...
// 30400-30600 hero every movement (!!! must change for new town)
// 30600-30800 hero gain level (!!! must change for new town)
// 30800 setup battle field
// 30801 MFCall(0); !?MF0; - Defence coefficient
// 30802 MFCall(1); !?MF1; - Block ability
// 30803 MFCall(2); !?MF2; - Hate trigger - control hate and basic damage done to a creature (Defence not included, any effects that depend on chance not included)
// ...
// 30900-30904 ; TL - timer #-seconds to call
// 31000-31099 - 100 local functions
// 31100-31199 - autotimers (!?TM#1/#2/#3/#4)
// 31200-31400 - hero gain level post-trigger (!!! must change for new town)
// ...
// 0x04000000 ... макс используемый тип - уровень
// 0x10000000+ OB position enter
// 0x20000000+ LE position
// 0x40000000+ OB type/subtype enter
// 0x08000000+ OB leave
//      +  0x10000000+ OB position leave
//      +  0x40000000+ OB type/subtype leavestruct _ERM_Trigger_{ // triggers with 1 or no parameters
//////////////////////////////

struct _ERM_Addition_{
	Word   Id;
	int  (*Fun)(char,int,_ToDo_ *,Mes *);
	int    Type;
} ERM_Addition[]={
	{'CD',ERM_CasDem,0},
	{'MA',ERM_MonAtr,0},
	{'UN',ERM_Universal,0},
	{'VR',ERM_Variable,1}, // скопировать переменную VRi
	{'TM',ERM_Timer,1},    // скопировать переменную TMi
	{'OB',ERM_SetObject,2},   // 2-x/y/l в MixedPos
	{'OW',ERM_Owner,0},
	{'MN',ERM_Mine,2},
	{'SC',ERM_SetScoolar,2},   // 2-x/y/l в MixedPos
	{'CH',ERM_SetChest,2},     // 2-x/y/l в MixedPos
	{'WT',ERM_SetWTomb,2},     // 2-x/y/l в MixedPos
	{'KT',ERM_SetKTree,2},     // 2-x/y/l в MixedPos
	{'FR',ERM_SetFire,2},      // 2-x/y/l в MixedPos
	{'LN',ERM_SetLean,2},      // 2-x/y/l в MixedPos
	{'ST',ERM_SetStone,2},     // 2-x/y/l в MixedPos
	{'WG',ERM_SetWagon,2},     // 2-x/y/l в MixedPos
	{'SK',ERM_SetSkelet,2},    // 2-x/y/l в MixedPos
	{'SP',ERM_SetSpring,2},    // 2-x/y/l в MixedPos
	{'WM',ERM_SetWMill,2},     // 2-x/y/l в MixedPos
	{'SW',ERM_SetSwan,2},      // 2-x/y/l в MixedPos
	{'MT',ERM_SetMonolit,2},   // 2-x/y/l в MixedPos
	{'GD',ERM_SetGarden,2},    // 2-x/y/l в MixedPos
	{'ML',ERM_SetMill,2},      // 2-x/y/l в MixedPos
	{'DW',ERM_SetDwelling,2},  // 2-x/y/l в MixedPos
	{'WH',ERM_SetWHat,2},      // 2-x/y/l в MixedPos
	{'SY',ERM_Shipyard,2},     // 2-x/y/l в MixedPos
	{'GR',ERM_Garrison,2},     // 2-x/y/l в MixedPos
	{'SR',ERM_Shrine,2},       // 2-x/y/l в MixedPos
	{'SG',ERM_Sign,2},         // 2-x/y/l в MixedPos
	{'UR',ERM_Univer,2},       // 2-x/y/l в MixedPos
	{'MC',ERM_Macro,1},        // скопировать переменную VRi
	{'FU',ERM_Function,3},     // скопировать значение M.n[0]
	{'DO',ERM_Do,4},           // значение M.n[0], цикл M.n[1],M.n[2],M.n[3]
	{'CA',ERM_Castle,2},       // 2-x/y/l в MixedPos
	{'TR',ERM_Terrain,2},      // 2-x/y/l в MixedPos
	{'HO',ERM_SetHero,3},      // скопировать значение M.n[0]
	{'PO',ERM_Position,2},     // 2-x/y/l в MixedPos
	{'BA',ERM_Battle,0},
	{'BF',ERM_BattleField,0},
	{'BM',ERM_BRound,1},
//  {'BM',ERM_BMonster,0},
	{'BH',ERM_BHero,1},
	{'BU',ERM_BUniversal,0},
	{'BG',ERM_MAction,0},
	{'QW',ERM_Qwest,0},
	{'HL',ERM_HeroGainLevel,0},
	{'HT',ERM_HintType,5},      // две переменные v1/v2
	{'PM',ERM_Pyramid,2},       // 2-x/y/l в MixedPos
	{'MW',ERM_WMon,1},          // скопировать переменную MWi
	{'CM',ERM_MouseClick,0},
	{'MM',ERM_MouseMove,0},
	{'MP',ERM_MP3,0},
	{'AI',ERM_AIRun,0},
	{'CO',ERM_NPC,1},
	{'VC',ERM_VarControl,0},
// 3.58
	{'SN',ERM_Sound,0},
	{'MR',ERM_MonRes,0},
	{'MF',ERM_MonFeature,0},
	{'EX',ERM_StackExperience,2},
	{'EA',ERM_AICrExp,2},
	{'CB',ERM_SetCrBank,2},  // 2-x/y/l в MixedPos
	{'IP',ERM_NetworkService,3},      // скопировать значение M.n[0]
// 3.59
	{'TL',ERM_TL,0}, 
	{'DL',ERM_Dlg,1},          // скопировать переменную VRi
	{'SS',ERM_Spell,1}, 
	{'LD',ERM_LODs,0}, 
	{'HD',ERM_HintDisplay,0},
	{'UX',ERM_UniversalEx,0},
	{'CI',ERM_CastleIncome,0},
	{'FC',ERM_FlagColor, 0},
	{'DG',ERM_DigGrail, 0},
	{'if',0,0},
	{'el',0,0},
	{'en',0,0},
	{'la',0,0},
	{'go',0,0},

	{0,0}
};

//////////////////////////////

#define TriggerListOld ((_Cmd_*)-1)
_Cmd_ * TriggerLists[0x10000];

inline int TriggerIndex(Dword id)
{
	return id&0xFFFF;
}

_Cmd_ * GetTriggerList(Dword id)
{
	int index = TriggerIndex(id);
	if (TriggerLists[index] == TriggerListOld)
	{
		_Cmd_ * cp = FirstTrigger;
		_Cmd_ ** last = &TriggerLists[index];
		while(cp != 0)
		{
			if (TriggerIndex(cp->Event) == index)
			{
				*last = cp;
				last = &cp->ListNext;
			}
			cp = cp->Next;
		}
		*last = 0;
	}
	return TriggerLists[index];
}

//////////////////////////////////

int SkipNumbers(char *str)
{
	STARTNA(__LINE__,str)
	int i;
	for(i=0;;i++){
		if((str[i]=='+')||(str[i]=='-')) continue;
		break;
	}
	for(;;i++){
		if((str[i]>='0')&&(str[i]<='9')) continue;
		RETURN(i)
	}
}

static char ERMMesBuf[5][10000];
char *_Message2ERM(char *str)
{
	if (IsLuaCall && ((Dword)str < (Dword)&ERMString[0] || (Dword)str >= (Dword)&ERMString[0] + sizeof(ERMString)) 
	    && ((Dword)str < (Dword)&ERMLString[0] || (Dword)str >= (Dword)&ERMLString[VAR_COUNT_LZ])
			&& (!StringSet::Belongs(str)))
	{
		StrCopy(ERMMesBuf[4], 10000, str);
		return ERMMesBuf[4];
	}
	STARTNA(__LINE__,str)
	int  i,j,k,l,v;
	char ch,*s,*stro;
	VarNum *vnt;
	_Date_ *p=CurDate;

 for(l=0;l<5;l++){
	if(l>0) str=ERMMesBuf[l-1];
	stro=ERMMesBuf[l];
	for(i=j=0;j<9999;i++,j++){
		ch=str[i];
		switch(ch){
			case '%':
				ch=str[++i];
				switch(ch){
					case '$':
						vnt=GetMacro(FindMacro2(&str[i],0,&k));
						i+=k-1;
						if(vnt==0){ MError("\"String 2ERM\"-wrong macro $...$."); break; }
						if(vnt->Type==7){
							j+=a2a(ERMString[vnt->Num-1],&/*ERMMesBuf*/stro[j])-1;
							break;
////            }else if(vnt->Type==9){
////              if(CheckScope()) break;
////              j+=a2a(ERMScope->String[vnt->Num-1],&ERMMesBuf[j])-1;
////              break;
						}else{
							v = vnt->Num;
							switch(vnt->Type){
								case 0: break; // number
								case 2: v=ERMVar[v-1]; break; // f...t
								case 3: v=ERMVar2[v-1]; break; // v
								case 4: v=ERMVarH[ERMW][v-1]; break; // w
								case 5: v=ERMVarX[v-1]; break; // x
								case 6: // y
									if(v<0) v=ERMVarYT[-v-1];
									else    v=ERMVarY[v-1];
									break;
//                case 8: v=ERMVarF[vnt->Num-1]; break; // e
							}
							j+=i2a(v,&/*ERMMesBuf*/stro[j])-1;
						}
						break;
					case '%': /*ERMMesBuf*/stro[j]='%'; break;
					case 'V': // V# or Vf...t
						ch=str[++i];
						if((ch>='f')&&(ch<='t')){ // переменная
							j+=i2a(ERMVar[ch-'e'-1],&/*ERMMesBuf*/stro[j])-1;
						}else{
							v=a2i(&str[i]); i+=SkipNumbers(&str[i])-1;
							if((v>=1)&&(v<=VAR_COUNT_V)){ // переменная
								j+=i2a(ERMVar2[v-1],&/*ERMMesBuf*/stro[j])-1;
							}else{ MError("\"String 2ERM\"-wrong %V# number (1...10000)."); }
						}
						break;
					case 'W': // W#
						++i;
						v=a2i(&str[i]); i+=SkipNumbers(&str[i])-1;
						if((v>=1)&&(v<=200)){ // w#
							j+=i2a(ERMVarH[ERMW][v-1],&/*ERMMesBuf*/stro[j])-1;
						}else{ MError("\"String 2ERM\"-wrong %W# number (1...200)."); }
						break;
					case 'X': // X#
						++i;
						v=a2i(&str[i]); i+=SkipNumbers(&str[i])-1;
						if((v>=1)&&(v<=16)){ // x#
							j+=i2a(ERMVarX[v-1],&/*ERMMesBuf*/stro[j])-1;
						}else{ MError("\"String 2ERM\"-wrong %X# number (1...16)."); }
						break;
					case 'Y': // Y#
						++i;
						v=a2i(&str[i]); i+=SkipNumbers(&str[i])-1;
						if((v>=1)&&(v<=100)){ // y#
							j+=i2a(ERMVarY[v-1],&/*ERMMesBuf*/stro[j])-1;
						}else if((v<=-1)&&(v>=-100)){ // y#
							j+=i2a(ERMVarYT[-v-1],&/*ERMMesBuf*/stro[j])-1;
						}else{ MError("\"String 2ERM\"-wrong %Y# number (-100...-1,1...100)."); }
						break;
					case 'E': // E#
						++i;
						v=a2i(&str[i]); i+=SkipNumbers(&str[i])-1;
						if((v>=1)&&(v<=100)){ // e#
							j+=f2a(ERMVarF[v-1],&/*ERMMesBuf*/stro[j])-1;
						}else if((v<=-1)&&(v>=-100)){ // e-#
							j+=f2a(ERMVarFT[-v-1],&/*ERMMesBuf*/stro[j])-1;
						}else{ MError("\"String 2ERM\"-wrong %E# number (-100...-1,1...100)."); }
						break;
					case 'Z': // Z#
						++i;
						v=a2i(&str[i]); i+=SkipNumbers(&str[i])-1;
						if((v<-VAR_COUNT_LZ)||(v==0)){ MError("\"String 2ERM\"-wrong %Z# number (-10...-1,1...1000)."); }
						if(v>1000)   j+=a2a(StringSet::GetText(v),&/*ERMMesBuf*/stro[j])-1;
						else if(v>0) j+=a2a(ERMString[v-1],&/*ERMMesBuf*/stro[j])-1;
						else         j+=a2a(ERMLString[-v-1],&/*ERMMesBuf*/stro[j])-1;
						break;
////          case 'B': // B#
////            ++i;
////            if(CheckScope()) break;
////            v=a2i(&str[i]); i+=SkipNumbers(&str[i])-1;
////            if((v>=1)&&(v<=500)){ // z#
////              j+=a2a(ERMScope->String[v-1],&ERMMesBuf[j])-1;
////            }else{ MError("\"String 2ERM\"-wrong %B# number (1...500)."); }
////            break;
////          case 'A': // A#
////            ++i;
////            if(CheckScope()) break;
////            v=a2i(&str[i]); i+=SkipNumbers(&str[i])-1;
////            if((v>=1)&&(v<=1000)){ // x#
////                j+=i2a(ERMVar2[v-1],&ERMMesBuf[j])-1;
////              }else{ MError("\"String 2ERM\"-wrong %A# number (1...1000)."); }
////            break;
					case 'F': // F# флаги
						++i;
						v=a2i(&str[i]); i+=SkipNumbers(&str[i])-1;
						if((v>=1)&&(v<=1000)){ // z#
							j+=i2a(ERMFlags[v-1],&/*ERMMesBuf*/stro[j])-1;
						}else{ MError("\"String 2ERM\"-wrong %F# number (1...1000)."); }
						break;
					case 'D': // Dd or Dw or Dm or Da
						ch=str[++i];
						switch(ch){
							case 'd': j+=i2a(p->Day,&/*ERMMesBuf*/stro[j])-1; break;
							case 'w': j+=i2a(p->Week,&/*ERMMesBuf*/stro[j])-1; break;
							case 'm': j+=i2a(p->Month,&/*ERMMesBuf*/stro[j])-1; break;
							case 'a': j+=i2a(GetCurDate(),&/*ERMMesBuf*/stro[j])-1; break;
							default : MError("\"String 2ERM\"-wrong %D$ syntax.");
						}
						break;
					case 'G': // G# or Gc
						ch=str[++i];
						if(ch=='c') v=CurrentUser();
//            else{ v=a2i(&str[i]); i+=SkipNumbers(&str[i]); }
//            if((v<0)||(v>7)){ Error(); break; }
						else{ MError("\"String 2ERM\"-wrong %G# syntax."); break; }
						s=ITxt(62+v,0,&Strings);
						while((ch=*s++)!=0) /*ERMMesBuf*/stro[j++]=ch;
						--j;
						break;
					default:
//            MError("\"String 2ERM\"-wrong %$ syntax."); return str;
							/*ERMMesBuf*/stro[j++]='%'; /*ERMMesBuf*/stro[j]=ch;
							break;
				}
				break;
			case   0:
				/*ERMMesBuf*/stro[j]=0; goto _ok;
			default :
				/*ERMMesBuf*/stro[j]=ch;
		}
	}
_ok:
	/*ERMMesBuf*/stro[9999]=0;
 }
 RETURN(ERMMesBuf[4])
}

void Mess(Mes *m)
{
	STARTNA(__LINE__,&m->m.s[m->i])
//  DATAIT;
//  Message("{ERM} Command has wrong syntax. Skipped.",1);
	char last = m->m.s[m->m.l];
	m->m.s[m->m.l] = 0;

	Message(Format("%s\n\n%s", ITxt(24,0,&Strings), LuaPushERMInfo(m->m.s, false)),1);
	lua_pop(Lua, 1);

	m->m.s[m->m.l] = last;
	++m->i;
	RETURNV
}

//static char NoERMString[]="*** ERM String Error ***";
char *ERM2String(char *mes,int zstr,int *len)
{
	STARTNA(__LINE__,mes)
	int i,j,fl;

	ERMMesBuf[0][0]=0;
	if(len!=0) *len=0;
	if(zstr){
		_Message2ERM(mes);
	}else{
		for(fl=i=j=0;i<10000;i++){
			if(mes[i]==STRCH){
				if(fl==1){
					mes[i]=0;
					_Message2ERM(&mes[j]);
					mes[i]=STRCH;
					*len=i+1;
					break;
				}else{
					j=i+1;
					fl=1;
				}
			}
		}
	}
	RETURN(ERMMesBuf[4])
}

int ApplyString(Mes *Mp,int ind,_AMes_ *amp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int sind;
	if(Mp->VarI[ind].Check==1){ // ?
		sind=GetVarVal(&Mp->VarI[ind]);
		if(BAD_INDEX_LZ(sind)||(sind>1000)){ MError2("wrong z (destination) index (-10...-1,1...1000) in ApplyString."); RETURN(0) }
		if(sind>0) StrCopy(ERMString[sind-1],512,amp->m.s);
		else       StrCopy(ERMLString[-sind-1],512,amp->m.s);
		RETURN(1)
	}else{
		sind=Mp->n[ind];
		if(BAD_INDEX_LZ(sind)){ MError2("wrong z (sourse) index (-10...-1,1...1000) in ApplyString."); RETURN(0) }
		MesMan(amp, GetErmString(sind), 0);
		RETURN(0)
	}
}

int PrepareArtTable(void){ return 0; }

Dword InitEnters(void)
{
	STARTNA(__LINE__, 0)
	Heap=(struct _Cmd_ *)HeapArray;
	ERMHeapSize=HEAPSIZE;
	RETURN(0)
}

//////////// Put all functions here

int SkipUntil(Mes *ms)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	char *s=ms->m.s; long  l=ms->m.l; long  i=ms->i; char  c=ms->c[0];
	int fl=0;
	char ch;
	if(c==1){ // до первого символа
		while(i<l){
			c=s[i];
			switch(c){
				case ' ':
				case 0x0A:
				case 0x0D:
				case 0x09: break;
//        case '"' : fl!=fl; break;
				default: ms->i=i; RETURN(0)
			}
			++i;
		}
	}else{
		while(i<l){
			ch=s[i];
//      if(ch==STRCH){ fl!=fl;}
			// VC
			if(ch==STRCH){ fl=!fl;}
			else{
				if(fl==0){
					if(ch==c){ ms->i=i+1; RETURN(0)}// пропустим найденный
				}
			}
			++i;
		}
	}
	RETURN(-1)
}

// skip all chars (comment part)
int SkipUntil2(Mes *ms)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	char *s=ms->m.s; long  l=ms->m.l; long  i=ms->i; char  c=ms->c[0];
	char ch;
	while(i<l){
		ch=s[i];
		if(ch==c){ ms->i=i+1; RETURN(0)}// пропустим найденный
		++i;
	}
	RETURN(-1)
}

int CheckERM(Mes *ms, bool start)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	if (start) { ms->i=0; }
	ms->c[0]=1;
	if(SkipUntil(ms)) RETURN(0)
	if (ms->m.s[ms->i] == 'Z' && ms->m.s[ms->i+1] == 'V' && ms->m.s[ms->i+2] == 'S' && ms->m.s[ms->i+3] == 'E')
	{
		ms->i+=4;
		RETURN(1)
	}
	RETURN(0)
}

/////////////////////////////////
int GetSubNum(Mes *ms,int ind)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	char *s=ms->m.s; long l=ms->m.l; long i=ms->i;
	char c=ms->c[ind]; int *n=&ms->n[ind]; //char *df=&ms->f[ind];
	char ch,sign=0,day=0;
	*n=0;

//  *df=0;
	if(c==1){
		do{
			ch=s[i];
			switch(ch){
				case ' ': if(sign==0){i++; continue;}else goto _over;
//        case 'd': if(sign==0){*df=1; ++i; continue;}else goto _ok;
				case 'c': if(day==0) {day=1; ++i; continue;}else goto _ok;
				case '-': if(sign==0){sign=-1; ++i; continue;}else goto _ok;
				case '+': if(sign==0){sign=1; ++i; continue;}else goto _ok;
				default:
					if((ch<'0')||(ch>'9')) goto _ok;
					if(sign==0) sign=1;
					 if(day==0) day=-1;
					*n*=10; *n+=(int)ch-'0';
					++i;
					break;
			}
		}while(1);
	}else{
		while(((ch=s[i])!=c)&&(i<l)){
			switch(ch){
				case ' ': ++i; continue;
//        case 'd': if(sign==0){*df=1; ++i; continue;}else return -1;
				case 'c': if(day==0) { day=1; ++i; continue;}else RETURN(-1)
				case '-': if(sign==0){ sign=-1; ++i; continue;}else RETURN(-1)
				case '+': if(sign==0){ sign=1; ++i; continue;}else RETURN(-1)
				default:
					if((ch<'0')||(ch>'9')) RETURN(-1)
					if(sign==0) sign=1;
					if(day==0) day=-1;
					*n*=10; *n+=(int)ch-'0';
					++i;
					break;
			}
		}
		ms->i=i+1; // пропустим запрашиваемый символ
		goto _ok2;
	}
_ok:
	ms->i=i;
_ok2:
	*n*=sign;
	if(day==1) *n+=GetCurDate();
	RETURN(0)
_over: // пропустим завершающие пробелы
	while(s[i]==' ') ++i;
	goto _ok;
}

void VNCopy(VarNum *src,VarNum *dst)
{
	STARTNA(__LINE__, 0)
	if(src==0){ MError("\"VNCopy\"-internal error (source=0). Possible reason is wrong macro syntax."); RETURNV }
	if(dst==0){ MError("\"VNCopy\"-internal error (destination=0). Possible reason is wrong macro syntax."); RETURNV }
//  dst->SetNum(src->GetNum());
	*dst=*src;
//  dst->Num=src->Num;
//  dst->Type=src->Type;
//  dst->IType=src->IType;
//  dst->Check=src->Check;
	RETURNV
}
int GetCmpCode(char cn)
{
	STARTNA(__LINE__, 0)
	switch(cn){
		case 0: RETURN(0)
		case 1: RETURN(5) // <
		case 2: RETURN(2) // =
		case 3: RETURN(7) // <=
		case 4: RETURN(4) // >
		case 5: RETURN(3) // <>
		case 6: RETURN(6) // >=
		case 7:
		default: MError("\"GetCmpCode\"-type of comparison is incorrect."); RETURN(0)
	}
}

void SkipSpaces(const char *s, long *ind)
{
	int i = *ind;
	while(s[i]==' '||(s[i]==0x0A)||(s[i]==0x0D)||(s[i]==0x09)) ++i;
	*ind = i;
}

int GetNum(Mes *ms,int ind, int immed)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	char   *s=ms->m.s; long l=ms->m.l; long i=ms->i;
	char    c=ms->c[ind]; int *n=&ms->n[ind]; char *df=&ms->f[ind];
	char    cn;
	VarNum *mvi=&ms->VarI[ind],*vnt;
	char    ch,set,vf,ivf/*,*macro*/;
	int     vi;

	long long oldCmd = ErrorCmd.Cmd;
	StrCopy(ErrorCmd.Name, 8, "GetNum");

	*n=0; *df=0; cn=0;
	mvi->Num=0; mvi->Type=0; mvi->IType=0; mvi->Check=0;
	vf=ivf=0; set=0;
	do{
		ch=s[i];
		switch(ch){
			case    0: goto _ok;
			case  'd': *df=1; ++i; continue;
			case  '<': cn|=1; ++i; continue;
			case  '=': cn|=2; ++i; continue;
			case  '>': cn|=4; ++i; continue;
			case  '?': set=1; ++i; continue;
			case  'v':  if(vf!=0) ivf=3; else vf=3; ++i; continue;
			case  'w':  if(vf!=0) ivf=4; else vf=4; ++i; continue;
			case  'x':  if(vf!=0) ivf=5; else vf=5; ++i; continue;
			case  'y':  if(vf!=0) ivf=6; else vf=6; ++i; continue;
			case  'z':  if(vf!=0) ivf=7; else vf=7; ++i; continue;
			case  'e':  if(vf!=0) ivf=8; else vf=8; ++i; continue;
////      case  'a':  if(vf!=0) ivf=8; else vf=8; ++i; continue;
////      case  'b':  if(vf!=0) ivf=9; else vf=9; ++i; continue;
			case  ' ':
//      case  '[':
//      case  ']':
			/*
			case  '^':
				do { i++; } while ( s[i]!='^' && s[i]!=0 );
				if (s[i]!=0) ++i;
				while ( s[i] == ' ' || s[i] == 0x0D || s[i] == 0x0A || s[i] == 0x09) ++i;
				ch = s[i];
				mvi->Type = 0;
				mvi->IType = 0;
				mvi->Check = 0;
				if (set || cn!=0){ MError("\"GetNum\"-cannot set or check a string constant."); goto _over; }
				goto _ok;
			*/

			case 0x0D:
			case 0x0A:
			case 0x09: ++i; continue;
			case  '$': // макро
				ms->i=i;
				vnt=GetMacro(FindMacro(ms,0));
				i=ms->i;
				if(vnt==0){ MError("\"GetNum\"-cannot find macro."); goto _over; }
				if(vnt->Type)
					if(vf) ivf=vnt->Type; else vf=vnt->Type;

				mvi->Num = vnt->Num;
				mvi->Type=vf;
				mvi->IType=ivf;
				if(set){
					if(cn!=0){ MError("\"GetNum\"-cannot set and check both."); goto _over; }
					mvi->Check=1; //?
					if(vnt->Type!=vtSimple) *n = mvi->Num;
					goto _ok;
				}else{
					*n = vi = GetVarIndex(mvi, true);
					if(vi==0 && mvi->Type) goto _over;
					switch(mvi->Type){
						case 2: *n=ERMVar[vi-1]; break; // f...t
						case 3: *n=ERMVar2[vi-1]; break; // v
						case 4: *n=ERMVarH[ERMW][vi-1]; break; // w
						case 5: *n=ERMVarX[vi-1]; break; // x
						case 6: // y
							if(*n<0) *n=ERMVarYT[-vi-1];
							else     *n=ERMVarY[vi-1];
							break;
						case 8: // e
							if(vi<0) *n=(int)ERMVarFT[-vi-1];
							else     *n=(int)ERMVarF[vi-1];
							break; 
					}
					mvi->Check=(Word)GetCmpCode(cn);
					goto _ok;
				}
			default:
				if((ch>='f')&&(ch<='t')){ // переменные
					vi=ch-'e'; ++i;
					mvi->Num=vi;
					if(vf!=0){
						ivf=2;
						mvi->Type=vf;
						mvi->IType=2;
					}else{
						vf=2;
						mvi->Type=2;  // f...t
					}
					if(set){ // запомнить
						if(cn!=0){ MError("\"GetNum\"-cannot set and check both (?f...t var)."); goto _over; }
						mvi->Check=1; //?
//            if(ivf!=0){ Error(); goto _over; }
						goto _ok;
					}else{   // извлеч и использовать
						if(immed){
							if(mvi->IType!=0) *n=vi=ERMVar[vi-1];  // если есть индексирование
							if(!CheckVarIndex(vi, vf, true)) goto _over;
							switch(vf){
								case 2: *n=ERMVar[vi-1]; break; // f...t
								case 3: *n=ERMVar2[vi-1]; break; // v
								case 4: *n=ERMVarH[ERMW][vi-1]; break; // w
								case 5: *n=ERMVarX[vi-1]; break; // x
								case 6:
									if(vi<0) *n=ERMVarYT[-vi-1];
									else     *n=ERMVarY[vi-1];
									break; // y
								case 7: *n=vi; break; // z
								case 8:
									if(vi<0) *n=(int)ERMVarFT[-vi-1];
									else     *n=(int)ERMVarF[vi-1];
									break; // e
							}
						}
						mvi->Check=(Word)GetCmpCode(cn);
						goto _ok;
					}
				}else{
					ms->i=i;
					if(GetSubNum(ms,ind)){ MError("\"GetNum\"-cannot parse a number."); goto _over; }
					i=ms->i;
					if(vf){
						vi=*n;
						mvi->Num=vi;
						mvi->Type=vf;
						mvi->IType=ivf;
						if(set){
							if(cn!=0){ MError("\"GetNum\"-cannot set and check both."); goto _over; }
							mvi->Check=1; //?
							goto _ok;
						}else{
							if(immed){
								if(ivf!=0)
								{
									*n = vi = GetVarIndex(mvi, true);
									if(vi==0) goto _over;
								}
								else
									if(!CheckVarIndex(vi, vf, true)) goto _over;
								
								switch(vf){
									case 3: *n=ERMVar2[vi-1]; break; // v
									case 4: *n=ERMVarH[ERMW][vi-1]; break; // w
									case 5: *n=ERMVarX[vi-1]; break; // x
									case 6:
										if(vi<0) *n=ERMVarYT[-vi-1];
										else     *n=ERMVarY[vi-1];
										break; // y
									case 7: *n=vi; break; // z
									case 8:
										if(vi<0)  *n=(int)ERMVarFT[-vi-1];
										else      *n=(int)ERMVarF[vi-1];
										break; // e
								}
							}
							mvi->Check=(Word)GetCmpCode(cn);
							goto _ok;
						}
					}else{
						mvi->Num=*n;
						mvi->Type=0;
						mvi->IType=0;
						mvi->Check=(Word)GetCmpCode(cn);
						if(set){ MError("\"GetNum\"-cannot get flag."); goto _over;} // нельзя флаг запоминать
					}
					goto _ok;
				}
		}
_ok:
		SkipSpaces(s, &i);
		if(c==1) goto _ok3;
		if(ch==0){ MError("\"GetNum\"-unexpected end of line."); goto _over; }
		if((ch!=c)&&(i<l)) continue;
		ms->i=i+1; // пропустим запрашиваемый символ
		SkipSpaces(s, &ms->i);
		goto _ok2;
	}while(1);
_ok3:
	ms->i=i;
_ok2:
	ErrorCmd.Cmd = oldCmd;
	RETURN(0)
_over:
	SkipSpaces(s, &i); // пропустим завершающие пробелы
	if(s[i]!=0)
		ms->i=i+1; // пропустим запрашиваемый символ
	SkipSpaces(s, &ms->i);
	ErrorCmd.Cmd = oldCmd;
	RETURN(1)
	//goto _ok;
}

int GetFlags(Mes *m)
{
	STARTNA(__LINE__,&m->m.s[m->i])
	Mes  M;
	int  i,ind;

	M.m.s=m->m.s;
	M.m.l=m->m.l;
	M.i=m->i;
	for(i=0;i<16;i++){
		FillMem(&m->Efl[0][i][0],sizeof(VarNum),0);
		FillMem(&m->Efl[0][i][1],sizeof(VarNum),0);
		FillMem(&m->Efl[1][i][0],sizeof(VarNum),0);
		FillMem(&m->Efl[1][i][1],sizeof(VarNum),0);
//    *(long *)&m->Efl[0][i][0]=0; // не установлен
//    *(long *)&m->Efl[0][i][1]=0; // не установлен
//    *(long *)&m->Efl[1][i][0]=0; // не установлен
//    *(long *)&m->Efl[1][i][1]=0; // не установлен
/*
		m->Efl[0][i][0].Type=0; // не установлен
		m->Efl[0][i][0].IType=0; // не установлен
		m->Efl[0][i][1].Check=0; // не установлен
		m->Efl[1][i][0].Type=0; // не установлен
		m->Efl[1][i][0].IType=0; // не установлен
		m->Efl[1][i][1].Check=0; // не установлен
*/
	}
	if(M.m.s[M.i]=='&'){
		++M.i;
		for(i=0;i<16;i++){
			M.n[i]=0; M.c[i]=1;
			if(GetNum(&M,i,0)) RETURN(-1)
			if(M.VarI[i].Check!=0){ MError("\"GetFlags\"-cannot get or campare first argument in \"&...\" part."); RETURN(-1) }
			if(M.VarI[i].Type!=0){ // проверяем на переменную
				if(M.VarI[i].Num==0){ MError("\"GetFlags\"-wrong var index or syntax in \"&...\" part."); RETURN(-1) }
				VNCopy(&M.VarI[i],&m->Efl[0][i][0]);
				if(GetNum(&M,i,0)){ MError("\"GetFlags\"-cannot get number in \"&...\" part."); RETURN(-1) }
				if(M.VarI[i].Check<2){ MError("\"GetFlags\"-cannot set or get second argument in \"&...\" part (only check)."); RETURN(-1) }
				if(M.VarI[i].Type==0){  // сравниваем с числом
					M.VarI[i].Num=M.n[i]; // запомним его
				}
				VNCopy(&M.VarI[i],&m->Efl[0][i][1]);
			}else{
				ind=M.n[i];
				if(ind<0){
					ind=-ind;
					M.VarI[i].Check=3;
				}else{
					M.VarI[i].Check=2;
				}
				if((ind<1)||(ind>1000)){ MError("\"GetFlags\"-wrong index of flag in \"&...\" part."); RETURN(-1) }
				M.VarI[i].Num=ind;
				M.VarI[i].Type=1; // флаг
				M.VarI[i].IType=0;
				VNCopy(&M.VarI[i],&m->Efl[0][i][0]);
			}
			if(M.m.s[M.i]==SLCH) ++M.i;
			else break;
		}
		m->i=M.i;
	}
	if(M.m.s[M.i]=='|'){
		++M.i;
		for(i=0;i<16;i++){
			M.n[i]=0; M.c[i]=1;
			if(GetNum(&M,i,0)) RETURN(-1)
			if(M.VarI[i].Check!=0){ MError("\"GetFlags\"-cannot get or campare first argument in \"|...\" part."); RETURN(-1) }
			if(M.VarI[i].Type!=0){ // проверяем на переменную
				if(M.VarI[i].Num==0){ MError("\"GetFlags\"-wrong var index or syntax in \"|...\" part."); RETURN(-1) }
				VNCopy(&M.VarI[i],&m->Efl[1][i][0]);
				if(GetNum(&M,i,0)){ MError("\"GetFlags\"-cannot get number in \"|...\" part."); RETURN(-1) }
				if(M.VarI[i].Check<2){ MError("\"GetFlags\"-cannot set or get second argument in \"|...\" part (only check)."); RETURN(-1) }
				if(M.VarI[i].Type==0){  // сравниваем с числом
					M.VarI[i].Num=M.n[i]; // запомним его
				}
				VNCopy(&M.VarI[i],&m->Efl[1][i][1]);
			}else{
				ind=M.n[i];
				if(ind<0){
					ind=-ind;
					M.VarI[i].Check=3;
				}else{
					M.VarI[i].Check=2;
				}
				if((ind<1)||(ind>1000)){ MError("\"GetFlags\"-wrong index of flag in \"|...\" part."); RETURN(-1) }
				M.VarI[i].Num=ind;
				M.VarI[i].Type=1; // флаг
				M.VarI[i].IType=0;
				VNCopy(&M.VarI[i],&m->Efl[1][i][0]);
			}
			if(M.m.s[M.i]==SLCH) ++M.i;
			else break;
		}
		m->i=M.i;
	}
	RETURN(0)
}
//#undef MError

int GetNumAutoSelf(Mes *ms,int fl,int immed)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	int  i;
	char che;
//  ms->efl=ms->dfl=0;
	che=ms->c[0];
	for(i=0;i<16;i++){
		ms->n[i]=0; ms->c[i]=1;
		if(GetNum(ms,i,immed)) RETURN(0)
		if(ms->m.s[ms->i]==SLCH) ++ms->i;
		else{
			if(fl){ if(GetFlags(ms)) RETURN(-2) }
			if(che!=1)
			{
				if(ms->m.s[ms->i]==che) ++ms->i;
				SkipSpaces(ms->m.s, &ms->i);
			}
			RETURN(i+1)
		}
	}
	RETURN(i)
}

int GetNumAuto(Mes *ms)  { return GetNumAutoSelf(ms,0,1); }
int GetNumAutoFl(Mes *ms){ return GetNumAutoSelf(ms,1,0); }

int GetSpecNum(Mes *ms)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	char *s=ms->m.s; long l=ms->m.l; long i=ms->i;
	if(s == 0) RETURN(0)
	int *n=&ms->n[0];

	char ch;
	*n=0;
	do{
		if(i>l) RETURN(-1)
		ch=s[i];
		if((ch<1)||(ch>0x0F)){ ms->i=i; RETURN(0)}
		if(ch!=0x0F){
			if(ch==0x0B) ch=0;
			*n*=10; *n+=(int)ch;
		}
		++i;
	}while(1);
}
void PutSpecNum(Mes *ms,int len)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	char *s=ms->m.s; long l=ms->m.l; int n=ms->n[0];
	if(s == 0) RETURNV

	int k,j,d;
	char ch,ds[10];

	for(k=0;k<10;k++){
		d=n%10;
		ds[9-k]=(char)d;
		n/=10;
	}
	for(k=0;k<10;k++){ if(ds[k]!=0) break; }
	for(j=0;k<10;k++,j++){
			ch=ds[k]; if(ch==0) ch=0x0B;
			s[j]=ch;
	}
	for(;j<len;j++) s[j]=0x0F;
	ms->i=l;
	RETURNV
}

int MakeSpec(_Mes_ *m,int nn)
{
	STARTNA(__LINE__, 0)
	int lind;
	Mes M;
	M.m.s=m->s; M.m.l=m->l; M.i=0;
	if(M.m.s == 0) RETURN(0)

	if(M.m.s[0]<0x0F){ // контрольные символы
		if(GetSpecNum(&M)) RETURN(0)
		if(M.n[0]!=nn) RETURN(0) // не наш
		RETURN(1);
	}else{
		M.c[0]=1;
		if(GetNum(&M,0,1)) RETURN(0)
		if(M.n[0]!=nn) RETURN(0) // не наш
		lind=M.i; M.i=0;
		PutSpecNum(&M,lind);
		RETURN(1)
	}
}

//////////////////////////////////////////////////////////////
#define SPECSKILL 353
static int FHSI_0[SSNUM]={300,0,23,302,295,3,297,304,41,299,294,13,69,4,145,292,305,296,301,303,293,298,102,16,26,62,20,8};
static int FHSI_C[MONNUM]={
/*  0*/  158,159, 1,160, 2,161, 5,162, 12,163, 7,164, 165,166,
/* 14*/  167,168, 17,169, 21,170, 31,171, 19,172, 22,173, 174,175,
/* 28*/  176,177, 32,178, 34,179, 44,180, 33,181, 37,182, 183,184,
/* 42*/  51,185, 53,186, 48,187, 50,188, 55,189, 49,190, 155,191,
/* 56*/  71,192, 64,193, 67,194, 65,195, 66,196, 68,197, 198,199,
/* 70*/  87,200, 80,201, 83,202, 203,204, 82,205, 86,206, 207,208,
/* 84*/  100,209, 103,210, 98,211, 101,212, 99,213, 96,214, 215,149,
/* 98*/  113,216, 114,217, 116,220, 117,218, 112,219, 119,221, 222,223,
/*112*/  226,129,130,131,234,235,
/*118*/  224,225, 128,231,SPECSKILL,228,SPECSKILL,230,SPECSKILL,227,SPECSKILL,229, 232,233,
/*132*/  236,237,238,239,147,148,240,241,
/*140*/  242,243,244,245,246,247,6,248,249,356,
/*150*/  250,251,252,253,254,255,256,257,258,
/*159*/  259,
/*160*/  260,261,262,263,
/*164*/  264,265,266,267,
/*168*/  268,269,270,271,272,273,
/*174*/  274,275,276,277,278,279,280,281,282,
/*183*/  274,275,276,277,278,279,280,281,282,
/*192*/  283,284,285,286,355
};
static int FHSI_R[10]={287,38,288,60,94,111,15,/*Mithril*/290,/*wood+ore*/289,/*all other res*/291};
static int FHSI_S[80]={
/* 0*/  342,348,318,354,312,309,314,350,311,325,
/*10*/  326,339,323,136,321,138,30,315,324,45,
/*20*/  11,57/*Fireball*/,63/*Inferno*/,73,72,313,330,329,307,336,
/*30*/  306,334,351,320,319,344,316,25,88,76,
/*40*/  341,9,333,338,108,10,77,141,14,346,
/*50*/  328,29,340,46,327,24,335,317,310,332,
/*60*/  40,345,331,349,347,343,337,322,352,308,
/*70*/  0,0,0,0,0,0,0,0,0,0
};
static int FHSI_5[1]={144};
static int FHSI_7[1]={151};
static int FHSI_8[2]={156,157};
int FindHeroSpecIcon(int Hn)
{
	STARTNA(__LINE__, 0)
	Dword *dsp=(Dword *)&HSpecTable[Hn];
//  if(Hn==79) RETURN(FHSI_8[0]+1); // Nagash
//  if(Hn==91) RETURN(FHSI_8[1]+1); // Jeddit
	switch(dsp[0]){
		case 0: // 0/$ спец во вторичном навыке.
			if(dsp[1]<0 || dsp[1]>=SSNUM) RETURN(SPECSKILL+1)
			RETURN(FHSI_0[dsp[1]]+1)
		case 1: // 1/$ спец по существам
			if(dsp[1]<0 || dsp[1]>=MONNUM) RETURN(SPECSKILL+1)
			RETURN(FHSI_C[dsp[1]]+1)
		case 2: // 2/$ спец по ресурсам
			if(dsp[1]<0 || dsp[1]>=10) RETURN(SPECSKILL+1)
			RETURN(FHSI_R[dsp[1]]+1)
		case 3: // 3/$ спец по заклинанию
			if(dsp[1]<0 || dsp[1]>=80) RETURN(SPECSKILL+1)
			RETURN(FHSI_S[dsp[1]]+1)
		case 4: // 4/$/$at/$df/$damage спец по мощным существам
			if(dsp[1]<0 || dsp[1]>=MONNUM) RETURN(SPECSKILL+1)
			RETURN(FHSI_C[dsp[1]]+1)
		case 5: // 5/$ спец по скорости и др?
			if(dsp[1]<2 || dsp[1]>=3) RETURN(SPECSKILL+1)
			RETURN(FHSI_5[dsp[1]-2]+1)
		case 6: // 6/$1/$2/$3 спец по апгрейду
			if(dsp[6]<0 || dsp[6]>=MONNUM) RETURN(SPECSKILL+1)
			RETURN(FHSI_C[dsp[6]]+1)
		case 7: // 7/$a/$d спец по драконам (7/???/at/df/damage)
			RETURN(FHSI_7[0]+1)
		case 8: // 8/$ новые спец.
			if(dsp[1]<1 || dsp[1]>=3 ) RETURN(SPECSKILL+1)
			RETURN(FHSI_8[dsp[1]-1]+1)
		case -1: // Adrienne - expert Fire Magic
			RETURN(145+1)
		default:
			break;
	}
	RETURN(SPECSKILL+1)
}

int MakeHeroSpec(int Hn,Mes *M,int Num)
{
	STARTNA(__LINE__,&M->m.s[M->i])
//  Dword *dsp=(Dword *)(0x678420+0x28*Hn);
	Dword *dsp=(Dword *)&HSpecTable[Hn];
	if(Num<2) RETURN(-1)
	if((M->n[0]<0)||(M->n[0]>8)) RETURN(-1)
	dsp[0]=M->n[0]; dsp[1]=dsp[1]*M->f[1]+M->n[1];
//              if(M.n[0]==0){ // 0/$ спец во вторичном навыке.
//              }else if(M.n[0]==1){ // 1/$ спец по существам
//              }else if(M.n[0]==2){ // 2/$ спец по ресурсам
//              }else if(M.n[0]==3){ // 3/$ спец по заклинанию
	if(M->n[0]==4){ // 4/$/$a/$d/$h спец по мощным существам
		if(Num<5) RETURN(-1)
		dsp[2]=dsp[2]*M->f[2]+M->n[2]; dsp[3]=dsp[3]*M->f[3]+M->n[3]; dsp[4]=dsp[4]*M->f[4]+M->n[4];
	}else if(M->n[0]==5){ // 5/$ спец по скорости и др?
	}else if(M->n[0]==6){ // 6/$1/$2/$3 спец по апгрейду
		if(Num<4) RETURN(-1)
		dsp[5]=dsp[5]*M->f[2]+M->n[2]; dsp[6]=dsp[6]*M->f[3]+M->n[3];
	}else if(M->n[0]==7){ // 7/$a/$d спец по драконам
		dsp[2]=dsp[2]*M->f[2]+M->n[2];
	}else if(M->n[0]==8){ // 8/$ новые спец.
	}
	RETURN(0)
}

long SetMes(_Mes_ *mt,_Mes_ *mf,long sf)
{
	STARTNA(__LINE__, 0)
	long i=sf,i1,lt,lf;
	char ch,*to,*from;
	to=mt->s;  lt=mt->l;
	if(mf!=0){ from=mf->s; lf=mf->l; }else{ from=0; lf=0; }
	for(i1=0;i1<lt;i1++){
		if(to[i1]>=0x0F) goto _ok0; // прпустим контрольную информацию
	}
	RETURN(-1)
_ok0:
	if(from==0) goto _ok2;
	if(sf==0){ // копия полного сообщения - разделители не нужны
		for(i=0;i<lf;i++){
			if(from[i]>=0x0F) goto _ok1; // прпустим контрольную информацию
		}
		RETURN(-1)
	}else{
		while((i<lf)){ ++i; if(from[i-1]==STRCH) goto _ok1;}
		RETURN(-1)
	}
_ok1:
	for(;i<lf;i++){
		ch=from[i];
		if((sf!=0)&&(ch==STRCH)) goto _ok2;
		to[i1++]=ch;
		if(i1==lt) goto _ok3; // уже вся строка заполнена
	}
//  return -1;
_ok2:
	for(;i1<lt;i1++) to[i1]=0x0F; // записываем невидемые символы
_ok3:
	RETURN(i+1)
}

int MesManOld(Mes *ms,_Mes_ *p)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	_Mes_  m,*mp;
	struct _Main_     *MS;
	struct _GlbEvent_ *GEp0,*GEp1,*GEp;
	int     i,n=ms->n[0];

	switch(n){
		case -1:
			if(SetMes(p,0,0)==-1) RETURN(-1) else RETURN(0)
		case  0:
			m.s=ERM2String(&ms->m.s[ms->i],0,&i);
			m.l=strlen(m.s); //
			if(SetMes(p,&m,0)==-1) RETURN(-1)
			ms->i+=i;
			RETURN(0)
		default:
			MS=Main;
			GEp0=MS->GEp0;  //  GEp0=(_GlbEvent_ *)  *(Dword *)(Esi_+0x84);
			GEp1=MS->GEp1;  //  GEp1=(_GlbEvent_ *)  *(Dword *)(Esi_+0x88);
			for(GEp=GEp0;GEp<GEp1;GEp++){
				mp=&GEp->Mes.m;
				if(MakeSpec(&GEp->Mes.m,n)){
					if(SetMes(p,mp,0)==-1) RETURN(-1) else RETURN(0)
				}
			}
			RETURN(-1)
	}
}

int NewMesMan(Mes *ms,_AMes_ *ap,int ind)
{
	STARTNA(__LINE__,&ms->m.s[ms->i])
	_Mes_  *mp;
	struct _Main_     *MS;
	struct _GlbEvent_ *GEp0,*GEp1,*GEp;
	int     i;

	switch(ms->n[ind]){
		case -1: MesMan(ap,0,0); RETURN(0)
		case  0:
			if((ms->VarI[ind].Type==7)/*||(ms->VarI[ind].Type==9)*/){
				ApplyString(ms,ind,ap);
			}else{
				MesMan(ap,ERM2String(&ms->m.s[ms->i],0,&i),0);
				ms->i+=i;
			}
			RETURN(0)
		default:
			if((ms->VarI[ind].Type==7)/*||(ms->VarI[ind].Type==9)*/){
				ApplyString(ms,ind,ap);
				RETURN(0)
			}else{
				MS=Main;
				GEp0=MS->GEp0;  //  GEp0=(_GlbEvent_ *)  *(Dword *)(Esi_+0x84);
				GEp1=MS->GEp1;  //  GEp1=(_GlbEvent_ *)  *(Dword *)(Esi_+0x88);
				for(GEp=GEp0;GEp<GEp1;GEp++){
					mp=&GEp->Mes.m;
					if(MakeSpec(&GEp->Mes.m,ms->n[ind])){
						MesMan(ap,mp->s,0); RETURN(0)
					}
				}
			}
			RETURN(-1)
	}
}

int ProcessDisable(_ToDo_ *sp,int z)
{
	STARTNA(__LINE__, 0)
	_Cmd_  *cp;
	//_ToDo_ *td;
	//int     i;
	Dword   event,object;
	Byte    pr,cur;

	if((z<0)||(z>7)){MError2("Z command has a wrong parameter (Z4...Z7)."); RETURN(1) }
	if((z>=0)&&(z<=3)){MError2("Z0...Z3 commands are obsolete and are not supported anymore."); RETURN(1) }
	object=sp->Pointer;
	cp=FirstTrigger;
	while(cp!=0 && cp->Event!=0){
		//if(z>3){ // генератор

			if(cp->Event&0x20000000){ // LE
				event=cp->Event&0x0FFFFFFF;
//        object=sp->Pointer2;
			}else{                    // NO
				event=cp->Event;
//        object=sp->Pointer;
			}
			if(event==object){ // ! нашли
//        if(sp->Pointer2!=0){MError("!!LE:Z command is obsolete and works only with LEx/y/l trigger/receiver.\n For new style LE$ you should use conditional execution."); goto l_exit; }
				pr=cp->DisabledPrev;
				cp->DisabledPrev=cp->Disabled;
				switch(z){
					case  7: cur=pr; break;
					case  6: cur=!pr; break;
					case  5: cur=1; break;
					default: cur=0; break;
				}
				cp->Disabled=cur;
			}
		/*
		}else{ // приемник
			for(i=0;i<cp->Num;i++){
				td=&cp->ToDo[i];
				if(td->Pointer==object){ // ! нашли
					pr=td->DisabledPrev;
					td->DisabledPrev=td->Disabled;
					switch(z){
						case  3: cur=pr; break;
						case  2: cur=!pr; break;
						case  1: cur=1; break;
						default: cur=0; break;
					}
					td->Disabled=cur;
				}
			}
		}
		*/
		cp=cp->Next;
	}
	RETURN(0)
}

int CheckFlags(VarNum (*Efl)[16][2])
{
	STARTNA(__LINE__, 0)
	int i,j,vi,tp,n,n2,fl,zind,zind2,evar=0;
	float v1,v2;
	char *txt1,*txt2;
	if((Efl[0][0][0].Type==0)&&(Efl[1][0][0].Type==0)) goto _ret0;
	long long oldCmd = ErrorCmd.Cmd;
	strcpy(ErrorCmd.Name, "CheckFlags");
	for(j=0;j<2;j++){
		for(i=0;i<16;i++){
			zind=zind2=0;
			tp=Efl[j][i][0].Type;
			if(tp==0) continue; // не задан

			vi = GetVarIndex(&Efl[j][i][0], true);
			if (vi == 0) goto _ret1;
			switch(tp){
				case 1: // флаг
					if(Efl[j][i][0].Check==3){ // должен быть сброшен
						if(j==0){ if(ERMFlags[vi-1]==1) goto ToOr; /*return 1;*/ }
						else{     if(ERMFlags[vi-1]==0) goto _ret0; }
					}else{ // должен быть установлен
						if(j==0){ if(ERMFlags[vi-1]==0) goto ToOr; /*return 1;*/ }
						else{     if(ERMFlags[vi-1]==1) goto _ret0; }
					}
					continue; // к следующему элементу
				case 2: // f...t
					n=ERMVar[vi-1];
					break;
				case 3: // v1...1000
					n=ERMVar2[vi-1];
					break;
				case 4: // w1...100
					n=ERMVarH[ERMW][vi-1];
					break;
				case 5: // x1...16
					n=ERMVarX[vi-1];
					break;
				case 6: // y1...100
					if(vi<0) n=ERMVarYT[-vi-1];
					else     n=ERMVarY[vi-1];
					break;
				case 7: // z1...500
					zind=vi; n=zind;
					break;
				case 8: // e1...100
					evar=1;
					v1 = (vi < 0 ? ERMVarFT[-vi-1] : ERMVarF[vi-1]);
					n = 0;
					break;
////        case 8: // a1...1000
////          if(CheckScope()) return 1;
////          n=ERMScope->Var[vi-1];
////          break;
////        case 9: // b1...500
////          if(CheckScope()) return 1;
////          zind=-vi; n=zind;
////          break;
				default: MError("\"CheckFlags\"-wrong var index in first argument of \"&|...\" part (1...[1000])."); goto _ret1;
			}
			vi = GetVarIndex(&Efl[j][i][1], true);
			if (Efl[j][i][1].Type != 0 && vi == 0) goto _ret1;
			switch(Efl[j][i][1].Type){
				case 0: // число
					n2=vi;
					break;
				case 2: // f...t
					n2=ERMVar[vi-1];
					break;
				case 3: // v1...1000
					n2=ERMVar2[vi-1];
					break;
				case 4: // w1...100
					n2=ERMVarH[ERMW][vi-1];
					break;
				case 5: // x1...16
					n2=ERMVarX[vi-1];
					break;
				case 6: // y1...100
					if(vi<0) n2=ERMVarYT[-vi-1];
					else     n2=ERMVarY[vi-1];
					break;
				case 7: // z1...500
					zind2=vi; n2=zind;
					break;
				case 8: // e1...100
					evar+=2;
					v2 = (vi < 0 ? ERMVarFT[-vi-1] : ERMVarF[vi-1]);
					n2 = 0;
					break;
////        case 8: // a1...1000
////          if(CheckScope()) return 1;
////          n2=ERMScope->Var[vi-1];
////          break;
////        case 9: // b1...500
////          if(CheckScope()) return 1;
////          zind2=-vi; n2=zind;
////          break;
				default: MError("\"CheckFlags\"-wrong var in second argument of \"&|...\" part (#,f...t,v,w,x,y)."); goto _ret1;
			}
			fl=0;
			if((zind!=0)&&(zind2!=0)){ // сравниваем строковые переменные
				if((zind<-VAR_COUNT_LZ)||(zind==0)||(zind2<-VAR_COUNT_LZ)||(zind2==0)){
					MError("\"CheckFlags\"-wrong z var index in \"&|...\" part.");
					goto _ret1;
				}
				if((txt1 = GetInternalErmString(zind)) == 0) goto _ret1;
				if((txt2 = GetInternalErmString(zind2)) == 0) goto _ret1;
				fl=StrCmpExt(txt1,txt2);
				switch(Efl[j][i][1].Check){
					case 2: // =
						break;
					case 3: // <>,>< (!=)
						if(fl) fl=0; else fl=1;
						break;
					default: MError("\"CheckFlags\"-Z vars may be compared for = or <> only in \"&|...\" part (<,=,<=,=<,>,<>,><,>=,=>)."); goto _ret1;
				}
			}else{
				if(evar!=0){
					if ((evar&1) == 0) v1 = n;
					if ((evar&2) == 0) v2 = n2;
					switch(Efl[j][i][1].Check){
						case 5: // <
							if(v1<v2) fl=1;
							break;
						case 2: // =
							if(v1==v2) fl=1;
							break;
						case 7: // <=,=<
							if(v1<=v2) fl=1;
							break;
						case 4: // >
							if(v1>v2) fl=1;
							break;
						case 3: // <>,>< (!=)
							if(v1!=v2) fl=1;
							break;
						case 6: // >=,=>
							if(v1>=v2) fl=1;
							break;
						default: MError("\"CheckFlags\"-wrong comparison type in \"&|...\" part (<,=,<=,=<,>,<>,><,>=,=>)."); break;
					}
				}else{
					switch(Efl[j][i][1].Check){
						case 5: // <
							if(n<n2) fl=1;
							break;
						case 2: // =
							if(n==n2) fl=1;
							break;
						case 7: // <=,=<
							if(n<=n2) fl=1;
							break;
						case 4: // >
							if(n>n2) fl=1;
							break;
						case 3: // <>,>< (!=)
							if(n!=n2) fl=1;
							break;
						case 6: // >=,=>
							if(n>=n2) fl=1;
							break;
						default: MError("\"CheckFlags\"-wrong comparison type in \"&|...\" part (<,=,<=,=<,>,<>,><,>=,=>)."); break;
					}
				}
			}
			if(j==0){ if(fl==0) goto ToOr; /*return 1;*/ }  // & all must be
			else{     if(fl==1) goto _ret0; }
		}
		if(j==0){
			if(Efl[0][0][0].Type!=0) goto _ret0; // если есть & часть и все условия прошли
		}
ToOr:; // AND не прошел
	}
	// и OR не было
_ret1:
	ErrorCmd.Cmd = oldCmd;
	RETURN(1)
_ret0:;
	ErrorCmd.Cmd = oldCmd;
	RETURN(0)
}

void PutVal(void *dp,char size,Mes *mp,char ind)
{
	STARTNA(__LINE__,&mp->m.s[mp->i])
	int   *dip;
	short *dsp;
	char  *dcp;
	unsigned int   *duip;
	unsigned short *dusp;
	unsigned char  *ducp;
	switch(size){
		//3.58 unsigned support
		case -4:
			duip=(unsigned int *)dp;
			*duip=(*duip)*mp->f[ind]+mp->n[ind];
			break;
		case -2:
			dusp=(unsigned short *)dp;
			*dusp=(unsigned short)((*dusp)*mp->f[ind]+mp->n[ind]);
			break;
		case -1:
			ducp=(unsigned char *)dp;
			*ducp=(unsigned char)((*ducp)*mp->f[ind]+mp->n[ind]);
			break;
		//
		case 4:
			dip=(int *)dp;
			*dip=(*dip)*mp->f[ind]+mp->n[ind];
			break;
		case 2:
			dsp=(short *)dp;
			*dsp=(short)((*dsp)*mp->f[ind]+mp->n[ind]);
			break;
		default: // 1
			dcp=(char *)dp;
			*dcp=(char)((*dcp)*mp->f[ind]+mp->n[ind]);
			break;
	}
	RETURNV
}

int GetVal(void *dp,char size)
{
	STARTNA(__LINE__, 0)
	int    v;
	switch(size){
		//3.58 unsigned support
		case -4: v=*((unsigned int *)dp);   break;
		case -2: v=*((unsigned short *)dp); break;
		case -1: v=*((unsigned char *)dp);  break;
		//
		case 4:  v=*((int *)dp);   break;
		case 2:  v=*((short *)dp); break;
		default: v=*((char *)dp);  break;
	}
	RETURN(v)
}

char ApplyCheck(int v, int cmpWith, int chk)
{
	int p = cmpWith;
	switch(chk){
		case 2: // =
			return (v==p) ? 1 : 0;
		case 3: // <>, ><
			return (v!=p) ? 1 : 0;
		case 4: // >
			return (v>p) ? 1 : 0;
		case 5: // <
			return (v<p) ? 1 : 0;
		case 6: // >=,=>
			return (v>=p) ? 1 : 0;
		case 7: // <=,=<
			return (v<=p) ? 1 : 0;
		default:
			return 0;
	}
}

int Apply(void *dp,char size,Mes *mp,char ind)
{
	STARTNA(__LINE__,&mp->m.s[mp->i])
	int v,ret,vi,chk;
	int *p;
	ret=0;
	chk=mp->VarI[ind].Check;
	if(chk!=0){
		ret=1; // были вопросы
		v=GetVal(dp,size);
		if(mp->VarI[ind].Type!=0 && chk==1) // ?переменная
		{
			vi=GetVarIndex(&mp->VarI[ind], true);
			if(vi == 0) RETURN(-1)
			switch(mp->VarI[ind].Type)
			{
				case 8:
					if(vi<0)  ERMVarFT[-vi-1]=(float)v;
					else      ERMVarF[vi-1]=(float)v;
					break; // e
				case 7: //3.58 z vars
					PutVal(dp,size,mp,ind);
					break;
				default:
					p = GetVarAddress(vi, mp->VarI[ind].Type);
					if (p == 0) { MError2("wrong var type (f...t,v,w,x,y,e)."); RETURN(-1) }
					*p = v;
					if (mp->f[ind])
					{
						if ((Dword)dp + 512 - (Dword)&size > 1024)
							*p = (int)dp;
						else
							*p = 0;
					}
			}
		}
		else
		{
			ERMFlags[0]=(char)ApplyCheck(v, mp->n[ind], chk);
		}
	}else{
		PutVal(dp,size,mp,ind);
	}
	RETURN(ret)
}

//
static char ERMExtDBuf[40][1000];
// [0...32] - диалоги
// 33 - WeekOfText
char *ERM2String2(int ind,char *mes){
	STARTNA(__LINE__,mes)
	ERMExtDBuf[0][0]=0;
	if((ind<0)||(ind>39)){ MError("\"ERM2String2\"-wrong index (internal)"); RETURN(ERMExtDBuf[0]) }
	_Message2ERM(mes);
	for(int i=0;i<1000;i++) ERMExtDBuf[ind][i]=ERMMesBuf[4][i];
	ERMExtDBuf[ind][999]=0;
	RETURN(ERMExtDBuf[ind])
}

static int IF_N_Ar[9][2];

//////////////////////////////////////////////////////////////
int ProcessMes(_ToDo_ *sp, Mes &M, char Cmd, int Num) // returns 1 in case of error
{
	STARTNA(__LINE__, 0)
	Word  Id=sp->Type;   // тип объекта изменения
	int   ns2,i,j,k,cfl;
	int   x,y,l,val;
//  _Main_     *MS;
	_MapItem_  /* *MIp0,*/*MIp;
	_CMonster_ *mp/*, *mp0*/;

//  MS=Main;
//  MIp0=MS->MIp0;   // корта положения объектов
//  MapSize=MS->MapSize; // dx=dy

	long long oldCmd = SetErrorCmd(Id, Cmd);

	switch(Id){
		case 0x4547: // GE
			{
				struct _GlbEvent_ *p=(struct _GlbEvent_ *)sp->Pointer;
				if(Cmd=='M'){ // M# уст подсообщение
					if(Num==2){ // с переменной
						if(M.n[0]==1){
							if(M.n[1]==-1){ // M1/-1
								MesMan(&p->Mes,0,0);
							}else{ // M1/$
								ApplyString(&M,1,&p->Mes);
							}
						}else{ MError("\"GE:M\"-wrong syntax (M1/$)"); goto l_exit; }
					}else{ // старый стиль
						 if(MesManOld(&M,&p->Mes.m)){ MError("\"GE:M\"-cannot set message (M$)"); goto l_exit; }
					}
					break;
				}else if(Cmd=='F'){ // F$ день первого появления (с 1-го)
					Apply(&p->FirstDate,2,&M,0);
				}else if(Cmd=='R'){ // R$ периодичность появления
					Apply(&p->Repeater,2,&M,0);
				}else if(Cmd=='B'){ // B#1/$2 ресурсы
					if(Num<2){ EWrongParamsNum(); goto l_exit; }
					if((M.n[0]<0)||(M.n[0]>6)){ EWrongParam(); goto l_exit; }
					Apply(&p->Res[M.n[0]],4,&M,1);
				}else if(Cmd=='E'){ // E$ разрешить игрокам цветов
					if(M.f[0]) p->Colors|=(Byte)(M.n[0]&0x0FF);
					else p->Colors=(Byte)(M.n[0]&0x0FF);
				}else if(Cmd=='N'){ // N$ запретить игрокам цветов
						M.n[0]=~M.n[0];
						if(M.f[0]) p->Colors&=(Byte)(M.n[0]&0x0FF);
						else p->Colors=(Byte)(M.n[0]&0x0FF);
				}else if(Cmd=='H'){ // H# разрешить человеку
					Apply(&p->HEnable,1,&M,0);
				}else if(Cmd=='Q'){ // H# разрешить человеку
					Apply(&p->AIEnable,1,&M,0);
				}else if(Cmd=='D'){ // D#1/#2
					if(Num<2){ MError("\"GE:D\"-wrong syntax (D$/$)"); goto l_exit; }
					if((M.n[0]<0)||(M.n[0]>7)){ MError("\"GE:D\"-wrong owner (0...7)"); goto l_exit; }
					if(M.n[1]){ // запретить
						p->Colors&=(Byte)~((Byte)(((Byte)1)<<M.n[0]));
					}else{ // разрешить
						p->Colors|=(Byte)(((Byte)1)<<M.n[0]);
					}
				}else if(Cmd==' '){ // пробелы могут быть
				}else{
					EWrongCommand(); goto l_exit;
				}
			}
			break;
		case 0x454C: // LE
			{
				struct _Event_ *p=0;
				Dword MixPos=GetDinMixPos(sp);
				_MapItem_ *mi=GetMapItem2(MixPos);
				_EventPos_ *pos=(_EventPos_ *)mi;
				int tp=CheckPlace4Hero(mi,(Dword **)&pos);
				ERMFlags[995]=0;
//          if(tp!=0x1A){ ERMFlags[998]=1; break; }
				if(tp==6){} // Pandora's Box
				else if(tp!=0x1A){ ERMFlags[1]=1; break; }
				__asm{
					mov    eax,BASE
					mov    eax,[eax]
					add    eax,0x1FBC4 // ->events
					mov    eax,[eax]
					mov    p,eax
				}
				if(p==0){ MError("\"LE\"-cannot find event."); goto l_exit; }
				p=&p[pos->Number];
				if(p==0){ MError("\"LE\"-cannot find event."); goto l_exit; }
				if(Cmd=='M'){
					if(Num==2){ // с переменной
						if(M.n[0]==1){
							if(M.n[1]==-1){ // M1/-1
								MesMan(&p->Mes,0,0);
							}else{ // M1/$
								ApplyString(&M,1,&p->Mes);
							}
						}else{ MError("\"LE:M\"-wrong syntax (M1/$)."); goto l_exit; }
					}else{// старый стиль
						if(NewMesMan(&M,&p->Mes,0)){ MError("\"LE:M\"-cannot set message (M$)."); goto l_exit; }
					}
				}else if(Cmd=='G'){ // G#1/$2/$3 охранник в поз #1 тип #2(-1 нет) кол-во #3
					if(Num<3) goto l_exit;
					if((M.n[0]<0)||(M.n[0]>6)) goto l_exit;
					Apply(&p->Guard.Ct[M.n[0]],4,&M,1);
					Apply(&p->Guard.Cn[M.n[0]],4,&M,2);
				}else if(Cmd=='X'){
//            if(M.n[0]!=0) p->fl_G=1; else p->fl_G=0;
					 val=p->fl_G&0xFF; // 3.58
					 if(Apply(&val,4,&M,0)==0){ // set syntax
							if(val) p->fl_G=1; else p->fl_G=0;
					 }
				}else if(Cmd=='E'){ // E$ опыт
					Apply(&p->Exp,4,&M,0);
				}else if(Cmd=='P'){ // P$ баллы заклинаний
					Apply(&p->SpPow,4,&M,0);
				}else if(Cmd=='O'){ // O$ мораль
					Apply(&p->Moral,1,&M,0);
				}else if(Cmd=='U'){ // U$ удача
					Apply(&p->Luck,1,&M,0);
				}else if(Cmd=='R'){ // R#1/$2 ресурсы
					if(Num<2) goto l_exit;
					if((M.n[0]<0)||(M.n[0]>7)) goto l_exit;
					if(M.n[0]==7) Apply(&(((short *)p->Res)[1]),2,&M,1);
					else if(M.n[0]==0) Apply(&p->Res[0],2,&M,1);
					else Apply(&p->Res[M.n[0]],4,&M,1);
				}else if(Cmd=='F'){ // F$1/$2/$3/$4 первичные навыки
					if(Num<4) goto l_exit;
					Apply(&p->First[0],1,&M,0);
					Apply(&p->First[1],1,&M,1);
					Apply(&p->First[2],1,&M,2);
					Apply(&p->First[3],1,&M,3);
				}else if(Cmd=='N'){ // N#1/$2/$3 заменить в табл втор навыков поз1 навык2 умение3
					if(Num==1){ // 3.58
						val=(long)p->SecondE-(long)p->Second;
						val/=8;
						Apply(&val,4,&M,0);
					}else if(Num<3){ goto l_exit;
					}else{
						if((M.n[0]<0)||(p->Second[M.n[0]]>p->SecondE[0])) goto l_exit;
						Apply(&p->Second[M.n[0]][0],4,&M,1);
						Apply(&p->Second[M.n[0]][1],4,&M,2);
					}
				}else if(Cmd=='A'){ // A#1/$2 заменить в табл арт поз1 арт2
					if(Num==1){ // 3.58
//              val=(long)p->ArtifE-(long)p->Artif;
//              val/=4;
						val=p->Arts.GetNum();
						Apply(&val,4,&M,0);
					}else if(Num<2){ goto l_exit;
					}else{
//              if((M.n[0]<0)||(&p->Artif[M.n[0]]>p->ArtifE)) goto l_exit;
//              Apply(&p->Artif[M.n[0]],4,&M,1);
						do{
							if(Apply(&val,4,&M,0)) break;
							int ar=p->Arts.Get(val);
							if(Apply(&ar,4,&M,1)) break;
							p->Arts.Set(val,ar);
						}while(0);
					}
				}else if(Cmd=='B'){ // 3.58 alternative for A
					int ar;
					int st=M.n[0]; if((st<1)||(st>5)){ MError("\"!!LE:B\"-wrong subcommand (1...5)."); goto l_exit; }
					switch(st){
						case 1: // A1/?var - number of arts
							if(Num<2){ MError("\"!!LE:B1\"-wrong syntax."); goto l_exit; }
							val=p->Arts.GetNum();
							Apply(&val,4,&M,1);
							break;
						case 2: // A2/#/$ - get/set art and subart at position #
							if(Num<3){ MError("\"!!LE:B2\"-wrong syntax."); goto l_exit; }
							if(Apply(&val,4,&M,1)) break;
							ar=p->Arts.Get(val);
							if(Apply(&ar,4,&M,2)) break;
							p->Arts.Set(val,ar);
							break;
						case 3: // A3/$ - add artifact
							if(Num<2){ MError("\"!!LE:B3\"-wrong syntax."); goto l_exit; }
							if(Apply(&val,4,&M,1)) break;
							p->Arts.Add(val);
							break;
						case 4: // A4/$ - del art
							if(Num<2){ MError("\"!!LE:B4\"-wrong syntax."); goto l_exit; }
							if(Apply(&val,4,&M,1)) break;
							p->Arts.Del(val);
							break;
						default:
							EWrongSyntax(); goto l_exit; 
					}
				}else if(Cmd=='S'){ // S#1/$2 заменить в табл закл поз1 закл2
					if(Num==1){ // 3.58
						val=(long)p->SpellE-(long)p->Spell;
						val/=4;
						Apply(&val,4,&M,0);
					}else if(Num<2){ goto l_exit;
					}else{
						if((M.n[0]<0)||(&p->Spell[M.n[0]]>p->SpellE)) goto l_exit;
						Apply(&p->Spell[M.n[0]],4,&M,1);
					}
				}else if(Cmd=='C'){ // C#1/$2/$3 существа
					if(Num<3) goto l_exit;
					if((M.n[0]<0)||(M.n[0]>6)) goto l_exit;
					Apply(&p->Mon.Ct[M.n[0]],4,&M,1);
					Apply(&p->Mon.Cn[M.n[0]],4,&M,2);
				}else if(Cmd=='D'){ // D#1/#2
					if(Num<2){ MError("\"LE:D\"-wrong syntax (D$/$)."); goto l_exit; }
					if((M.n[0]<0)||(M.n[0]>7)){ MError("\"LE:D\"-wrong owner (0...7)."); goto l_exit; }
					if(M.n[1]){ // запретить
						pos->Enabled4&=(Word)~((Word)(1<<M.n[0]));
					}else{ // разрешить
						pos->Enabled4|=(Word)(1<<M.n[0]);
					}
				}else if(Cmd=='I'){ // I#
					pos->AIEnable=(Word)M.n[0];
				}else if(Cmd=='L'){ // L#
					pos->OneVisit=(Word)M.n[0];
				}else if(Cmd==' '){
				}else{
					EWrongCommand(); goto l_exit;
				}
			}
			break;
		case 0x4543: // CE
			{
				struct _CastleEvent_ *p=(struct _CastleEvent_ *)sp->Pointer;
				if(Cmd=='M'){
				// M^text^ , M1/$
					if(Num==2){ // с переменной
						if(M.n[0]==1){
							if(M.n[1]==-1){ // M1/-1
								MesMan(&p->Mes,0,0);
							}else{ // M1/$
								ApplyString(&M,1,&p->Mes);
							}
						}else{ MError("\"CE:M\"-wrong syntax (M1/$)."); goto l_exit; }
					}else{// старый стиль
						 if(MesManOld(&M,&p->Mes.m)){ MError("\"CE:M\"-cannot set message (M$)."); goto l_exit; }
					}
				}else if(Cmd=='F'){ // F$ день первого появления (с 1-го)
					Apply(&p->FirstDate,2,&M,0);
				}else if(Cmd=='R'){ // R$ периодичность появления
					Apply(&p->Repeater,2,&M,0);
				}else if(Cmd=='B'){ // B#1/$2 ресурсы
					if(Num<2) goto l_exit;
					if((M.n[0]<0)||(M.n[0]>6)) goto l_exit;
					Apply(&p->Res[M.n[0]],4,&M,1);
				}else if(Cmd=='E'){ // E$ разрешить игрокам цветов
					if(M.f[0]) p->Colors|=(Byte)(M.n[0]&0x0FF);
					else p->Colors=(Byte)(M.n[0]&0x0FF);
				}else if(Cmd=='N'){ // N$ запретить игрокам цветов
					M.n[0]=~M.n[0];
					if(M.f[0]) p->Colors&=(Byte)(M.n[0]&0x0FF);
					else p->Colors=(Byte)(M.n[0]&0x0FF);
				}else if(Cmd=='H'){ // H# разрешить человеку
					Apply(&p->HEnable,1,&M,0);
				}else if(Cmd=='Q'){ // H# разрешить человеку
					Apply(&p->AIEnable,1,&M,0);
				}else if(Cmd=='C'){ // C#1/$2 существа
					if(Num<2) goto l_exit;
					if((M.n[0]<0)||(M.n[0]>6)) goto l_exit;
					Apply(&p->Cr[M.n[0]],2,&M,1);
				}else if(Cmd=='U'){ // U# строить здания #-номер бита-здания
					if((M.n[0]<0)||(M.n[0]>47)) goto l_exit;
					ns2=M.n[0]/8; p->BuildIt[ns2]|=(Byte)(1<<(M.n[0]%8));
				}else if(Cmd=='D'){ // D# НЕ строить здания #-номер бита-здания
					if((M.n[0]<0)||(M.n[0]>47)) goto l_exit;
					ns2=M.n[0]/8; p->BuildIt[ns2]&=(Byte)~(1<<(M.n[0]%8));
				}else if(Cmd==' '){
				}else{
					EWrongCommand(); goto l_exit;
				}
			}
			break;
		case 0x4F4D: // MO
			{
				int mn,v;
				struct _Monster_  *p=0;
				Dword MixPos=GetDinMixPos(sp);
				MIp=GetMapItem2(MixPos);
				mp=(_CMonster_ *)MIp;
				if((p==0)&&(mp->HasSetUp)){ // неопределен - поищем
					mn=mp->SetUpInd;
					if(mn>GetMonsterNum()){ MError("\"MO\"-cannot find monster by num (internal)."); goto l_exit; }
					p=GetMonsterBase(); if(p==0){ MError("\"MO\"-cannot find monster base(internal)."); goto l_exit; }
					p=&p[mn];
				}
				if(Cmd=='G'){ // G$ количество
					v=mp->Number; if(Apply(&v,4,&M,0)) break; mp->Number=(Word)v;
				}else if(Cmd=='O'){ // O# не ростет
					v=mp->NoMore; if(Apply(&v,4,&M,0)) break; mp->NoMore=(Word)v;
				}else if(Cmd=='U'){ // U# не убегает
					v=mp->NoRun; if(Apply(&v,4,&M,0)) break; mp->NoRun=(Word)v;
				}else if(Cmd=='R'){ // R# агрессия
					if(Num>1){ // новый стиль 3.57m
						v=mp->Agression; if(v==0x1C) v=0;
						if(Apply(&v,4,&M,0)) break;
						mp->Agression=(Word)v;
						if(v==0) mp->Agression=0x1C;
					}else{
						if(mp->Agression==0x0A) v=1; else v=0;
						if(Apply(&v,4,&M,0)) break;
						if(v) mp->Agression=0x0A; else mp->Agression=0x1C;
					}
				}else{
					if(Cmd=='M'){
						if(p==0){ MError("\"MO:M\"-monster has no setup."); goto l_exit; }
						if(NewMesMan(&M,&p->Mes,0)) goto l_exit;
						p->Set=1; // настройка опций
					}else if(Cmd=='B'){ // R#1/$2 ресурсы
						if(p==0){ MError("\"MO:M\"-monster has no setup."); goto l_exit; }
						if(Num<2){ MError("\"MO:B\"-wrong syntax (R$/$)."); goto l_exit; }
						if((M.n[0]<0)||(M.n[0]>6)){ MError("\"MO:B\"-incorrect resource (0...7)."); goto l_exit; }
						Apply(&p->Res[M.n[0]],4,&M,1);
						p->Set=1; // настройка опций
					}else if(Cmd=='A'){ // артифакт
						if(p==0){ MError("\"MO:A\"-monster has no setup."); goto l_exit; }
						Apply(&p->Artefact,4,&M,0);
						p->Set=1; // настройка опций
//            }else if(Cmd=='W'){ // бродячий
//              MakeWoMoPos(MIp,M.n[0]);
					}else if(Cmd==' '){
					}else{
						EWrongCommand(); goto l_exit;
					}
				}
			}
			break;
		case 0x5241: // AR
			{
				_ArtRes_  *p=0/*(_ArtRes_ *)sp->Pointer*/;
				 Dword MixPos=GetDinMixPos(sp);
				_MapItem_ *mp;
				_CRes_    *vp;
//          _ArtSetUp_ *artsetup;
				int v/*,vi*/;
				mp=GetMapItem2(MixPos);
				if((mp->OType!=79)&&(mp->OType!=5)&&(mp->OType!=93)){ MError("\"AR\"-not an artefact or resource."); goto l_exit; } // нет ресурса
				vp=(_CRes_ *)mp;
				if(vp->HasSetUp){
					v=vp->SetUpNum;
					if(v>=GetArtResNum()){ MError("\"AR\"-cannot find artefact or resource (internal)."); goto l_exit; }
					p=GetArtResBase();
					if(p!=0) p=&p[v];
				}
				if(Cmd=='V'){//V$ количестро ресурса
					v=vp->Value; if(Apply(&v,4,&M,0)) break; vp->Value=(Word)v;
					break;
				}
				if(p!=0){
					if(Cmd=='M'){
						if(MixPos==0){ if(MesManOld(&M,&p->Mes.m)) goto l_exit; }
						else{ // M^text^ , M1/$
							if(Num==2){ // с переменной
								if(M.n[0]==1){
									if(M.n[1]==-1){ // M1/-1
										MesMan(&p->Mes,0,0);
									}else{ // M1/$
										ApplyString(&M,1,&p->Mes);
									}
								}else{ MError("\"AR:M\"-wrong syntax (M1/$)."); goto l_exit; }
							}else{
								if(NewMesMan(&M,&p->Mes,0)){ MError("\"AR:M\"-cannot set message (M$)."); goto l_exit; }
							}
						}
					}else if(Cmd=='G'){ // G#1/$2/$3 охранник в поз #1 тип #2(-1 нет) кол-во #3
						if(Num<3) goto l_exit;
						if((M.n[0]<0)||(M.n[0]>6)) goto l_exit;
						Apply(&p->Gt[M.n[0]],4,&M,1);
						Apply(&p->Gn[M.n[0]],4,&M,2);
					}else if(Cmd=='X'){
						if(M.n[0]!=0) p->fl_G=1; else p->fl_G=0;
					}else if(Cmd==' '){
					}else{ EWrongCommand(); }
					break;
				}
				MError("\"AR\"-cannot find artefact or resource (syntax error).");
				goto l_exit;
			}
		case 0x4548: // 'HE' HE$ HE$/$/$
			{ //ABCDEF HI KLMNOPRSTUVWXY
				int n;
				struct _Hero_  *p;
				switch(sp->ParSet){
					case 1: // номер
						n=GetVarVal(&sp->Par[0]);
						if(n==-1) p=ERM_HeroStr;
						else if(n==-10) p=G2B_HrA;
						else if(n==-20) p=G2B_HrD;
						else p=GetHeroStr(n);
						if(p==0){ MError("\"HE$\"-cannot find hero (out of range)."); goto l_exit; }
						break;
					case 3: // позиция
						MixedPos(&x,&y,&l,GetDinMixPos(sp));
						p=FindHeroNum(x,y,l);
						if(p==0){ MError("\"HE$/$/$\"-cannot find hero at position."); goto l_exit; }
						break;
					default: // ???
						MError("\"HE???\"-incorrect syntax.");
						goto l_exit;
				}
				if(Cmd=='O'){ // O$(/#) сменить хозяина
					_PlayerSetup_ *ps;
					char *pho=GetHOTableBase();
					if(M.VarI[0].Check==0){ // устанавливаем
						if((M.n[0]<-1)||(M.n[0]>7)){ MError("\"HE:O\"-owner out of range (-1...7)."); goto l_exit; }
						if(Num>1){ // 3.58 change an owner without check
							Apply(&p->Owner,1,&M,0); break;
						}
						if(M.n[0]!=-1){
							ps=GetPlayerSetup(M.n[0]); if(ps==0){ MError("\"!!HE:O\"-cannot get PlayerSetup structure."); goto l_exit; }
							if(ps->HasHeroes>=8){ break;} // нельзя больше добавить
						}
						if(p->Owner!=-1){
							ps=GetPlayerSetup(p->Owner); if(ps==0){ MError("\"!!HE:O\"-cannot get PlayerSetup structure."); goto l_exit; }
							for(i=0;i<8;i++){ if(ps->Heroes[i]==p->Number) goto Ok_11; }
							MError("\"!!HE:O\"-cannot find hero (internal)."); goto l_exit;
Ok_11:
							++i; for(;i<8;i++){ ps->Heroes[i-1]=ps->Heroes[i]; }
							ps->Heroes[7]=-1;
							ps->HasHeroes-=(char)1;
							if(ps->CurHero==p->Number){
								if(ps->HasHeroes>0) ps->CurHero=ps->Heroes[0]; else ps->CurHero=-1;
							}
						}
						if(M.n[0]!=-1){
							ps=GetPlayerSetup(M.n[0]); if(ps==0){ MError("\"!!HE:O\"-cannot get PlayerSetup structure."); goto l_exit; }
							ps->Heroes[ps->HasHeroes]=p->Number;
							ps->HasHeroes+=(char)1;
						}
						pho[p->Number]=(char)M.n[0];
						Apply(&p->Owner,1,&M,0);
					}else{
						Apply(&p->Owner,1,&M,0);
					}
				}else if(Cmd=='P'){ // P$1/P$2/$3[/flag] переместить в позицию
					if(Num<3) goto l_exit;
					cfl=0;
					if(M.VarI[0].Check){ cfl=1; Apply(&p->x,2,&M,0); }
					if(M.VarI[1].Check){ cfl=1; Apply(&p->y,2,&M,1); }
					if(M.VarI[2].Check){ cfl=1; Apply(&p->l,2,&M,2); }
					if(cfl==0){
						if(Num==4) cfl=M.n[3]; else cfl=0;
						JumpHero(p,p->x*M.f[0]+M.n[0],(p->y*M.f[1]+M.n[1]),(p->l*M.f[2]+M.n[2]),cfl);
					}
				}else if(Cmd=='I'){ // I$ очки заклинаний
					Apply(&p->SpPoints,2,&M,0);
					if(Num<2) RedrawMap();
				}else if(Cmd=='X'){ // специализация
					if(Num<7){
						if(MakeHeroSpec(p->Number,&M,Num)) goto l_exit;
					}else{
						_HeroSpec_ *dsp=&HSpecTable[p->Number];
						int p0,p1,p2,p3,p4,p5,p6;
						p0=dsp->Spec; p1=dsp->Spec1; p2=dsp->Spec2; p3=dsp->Spec3; p4=dsp->Spec4; p5=dsp->Spec5; p6=dsp->Spec6;
						Apply(&p0,4,&M,0);
						Apply(&p1,4,&M,1);
						Apply(&p2,4,&M,2);
						Apply(&p3,4,&M,3);
						Apply(&p4,4,&M,4);
						Apply(&p5,4,&M,5);
						Apply(&p6,4,&M,6);
						dsp->Spec=p0; dsp->Spec1=p1; dsp->Spec2=p2; dsp->Spec3=p3; dsp->Spec4=p4; dsp->Spec5=p5; dsp->Spec6=p6;
					}
// начиная с 3.57m записывается автоматически в файл данных
//            if(PostInstr==0){ // не дублировать инструкции в начале.
//              if(AddCommand(sp,Start,M.i)) goto l_exit;
//            }
				}else if(Cmd=='U'){ // U$1/$2/$3 сменить точку и привязку радиуса бега
					if(Num<3) goto l_exit;
					Apply(&p->x0,1,&M,0);
					Apply(&p->y0,1,&M,1);
					Apply(&p->Run,1,&M,2);
				}else if(Cmd=='C'){ // C модефикация существ
					if(Num<4){ EWrongParamsNum(); goto l_exit; }
					if(Num==14){
						CrChangeDialog(p,M.n[0],M.n[1],M.n[2],M.n[3],M.n[4],M.n[5],M.n[6],M.n[7],
														 M.n[8],M.n[9],M.n[10],M.n[11],M.n[12],M.n[13]);
					}else{
						int check=0,Exp=0,Mod=0,OldNum;
						if(Num>5) Apply(&Mod,4,&M,5);
						if(M.n[0]==0){ // C0/#2/$3/$4{/$5{/$6}} модефикация существ по позиции
							{
								if((M.n[1]<0)||(M.n[1]>7)){ EWrongParam(); goto l_exit; }
								if(M.f[2]==0){
									// 3.59 fix -1 only if a constant
									if((M.n[2]==-1)&&(M.VarI[2].Type==0)){ // апгрейд
										p->Ct[M.n[1]]=p->Ct[M.n[1]]&0xFFFFFFFE +1;
									// 3.59 fix -2 only if a constant
									}else if((M.n[2]==-2)&&(M.VarI[2].Type==0)){ // дегрейд
										p->Ct[M.n[1]]=p->Ct[M.n[1]]&0xFFFFFFFE;
									}else{
										if(Apply(&p->Ct[M.n[1]],4,&M,2)) check=1;
									}
								}
								OldNum=p->Cn[M.n[1]];
								if(Apply(&p->Cn[M.n[1]],4,&M,3)) check=1;
								if(p->Cn[M.n[1]]<=0){ p->Ct[M.n[1]]=-1; p->Cn[M.n[1]]=0; }
								// 3.58 cr experience
								if(Num>4){
//                    Exp=CrExpoSet::GetExp(CE_HERO,MAKEHS(p->Number,M.n[1]));
									// 3.59 - return exp or exp level (M>=10)
									Exp=CrExpoSet::GetExpM(CE_HERO,MAKEHS(p->Number,M.n[1]),Mod);
									if(Apply(&Exp,4,&M,4)) check=1;
								}
								if(check==0){
									CrExpoSet::Modify(5,CE_HERO,MAKEHS(p->Number,M.n[1]),Exp,Mod,p->Ct[M.n[1]],OldNum,p->Cn[M.n[1]]);
								}
							}
						}else if(M.n[0]==1){ // C1/#2/$3/$4{/$5} модефикация существ по типу
							{
								for(i=0;i<7;i++){
									if(p->Ct[i]==M.n[1]){
										if(Apply(&p->Ct[i],4,&M,2)) check=1;
										OldNum=p->Cn[i];
										if(Apply(&p->Cn[i],4,&M,3)) check=1;
										if(p->Cn[i]==0) p->Ct[i]=-1;
										// 3.58 cr experience
										if(Num>4){
											Exp=CrExpoSet::GetExp(CE_HERO,MAKEHS(p->Number,i));
											if(Apply(&Exp,4,&M,4)) check=1;
										}
										if(check==0){
											CrExpoSet::Modify(5,CE_HERO,MAKEHS(p->Number,i),Exp,Mod,p->Ct[i],OldNum,p->Cn[i]);
										}
									}
								}
							}
						}else if(M.n[0]==2){ // C2/тип/число/H_ai{/$5} добавить существ
							AddMon2Hero(p,M.n[1],M.n[2],M.n[3]);
							// 3.58 cr experience
							if(Num>4){
								if(Apply(&Exp,4,&M,4)) check=1;
							}
							if(check==0){
								CrExpoSet::Modify(6,CE_HERO,MAKEHS(p->Number,0),Exp,Mod,M.n[1],0,M.n[2],(_MonArr_ *)&p->Ct[0]);
							}
						}
					}
				}else if(Cmd=='S'){ // S#1/$2 вторичные скилы
					if(Num==3){ // показ/скил/1
						int sind=0,skill=0;
						if(Apply(&sind,4,&M,0)){ // ?/#
							if(Apply(&skill,4,&M,1)){ // ?/? - wrong
								MError("\"HE:S?/?/1\"- is a wrong syntax."); goto l_exit;
							}
							if((skill<0)||(skill>27)){ MError("\"HE:S?/#/1\"- skill number is out of range (0...27)."); goto l_exit; }
							sind=p->SShow[skill];
							Apply(&sind,4,&M,0);
						}else{ // #/? or #/#
							if((sind<0)||(sind>8)){ MError("\"HE:S#/#/1\"- skill show index is out of range (0...8)."); goto l_exit; }
							skill=-1;
							if(Apply(&skill,4,&M,1)){ // #/?
								for(i=0;i<28;i++){
									if(p->SShow[i]==sind){
										skill=i;
										Apply(&skill,4,&M,1);
										break;
									}
								}
							}else{ // #/#
								if((skill<0)||(skill>27)){ MError("\"HE:S#/#/1\"- skill number is out of range (0...27)."); goto l_exit; }
								for(i=0;i<28;i++){
									if(p->SShow[i]==sind) p->SShow[i]=0;
								}
								p->SShow[skill]=(Byte)sind;
							}
						}
					}else if(Num==1){
						Apply(&p->SSNum,4,&M,0);
					}else if(Num==2){
						if((M.n[0]<0)||(M.n[0]>27)) goto l_exit;
						if(Apply(&p->SSkill[M.n[0]],1,&M,1)) goto _ok101;
						if(p->SSkill[M.n[0]]==0){ // удалить
							if(p->SShow[M.n[0]]!=0){ // уже есть
								if(p->SShow[M.n[0]]!=p->SSNum){
									for(i=0;i<28;i++) if(p->SShow[i]==p->SSNum){
										p->SShow[i]=p->SShow[M.n[0]];
										goto _ok1;
									}
									goto l_exit;
_ok1:;
								}
								p->SShow[M.n[0]]=0;
								--p->SSNum;
							}else{ // еще нет
							}
						}else{
							if(p->SShow[M.n[0]]!=0){ // уже есть
							}else{
								if(p->SSNum<8){ // можно еще добавить
									++p->SSNum;
									p->SShow[M.n[0]]=(Byte)p->SSNum;
								}else{ // нет места
								}
							}
						}
					}else goto l_exit;
_ok101:;
				}else if(Cmd=='A'){ // A#1
					if(Num>1){ // расш синтакс
						int aflag,anum,art=0,arts=-1;
						if(M.n[0]!=5 && M.VarI[1].Check==0){
							// check only if used SET syntax
							art=M.n[1]; arts=-1;
							if(art<0){ MError("\"HE:A\"-wrong artifact number."); goto l_exit; }
							if(art==1000) art=0;
							if(art>1000){ arts=art-1001; art=1; }
						}
						switch(M.n[0]){
							case 1: // A1/art/pos - поместить арт
//                  if((M.n[2]<0)||(M.n[2]>18)){ MError("\"HE:A1\"-wrong slot number."); goto l_exit; }
								if((M.n[2]<0)||(M.n[2]>82)){ MError("\"HE:A1\"-wrong slot number."); goto l_exit; }
								if(M.VarI[1].Check!=0){ // проверка на наличие
									if(M.n[2]>18){ // 3.58 backpack
										art=p->OArt[M.n[2]-19][0];
										if(art==0) art=1000;
										if(art==1) art=p->OArt[M.n[2]-19][1]+1001;
										Apply(&art,4,&M,1);
										break;
									}else{
										art=p->IArt[M.n[2]][0];
										if(art==0) art=1000;
										if(art==1) art=p->IArt[M.n[2]][1]+1001;
										Apply(&art,4,&M,1);
										break;
									}
								}
								// добавление
								if(M.n[2]>18){ // 3.58 backpack
									if(p->OArt[M.n[2]-19][0]==-1){ // свободен
										p->OArt[M.n[2]-19][0]=art;
										p->OArt[M.n[2]-19][1]=arts;
										ERMFlags[0]=1;
									}else ERMFlags[0]=0; // оповестим, что не удалось
								}else{
									if(p->IArt[M.n[2]][0]==-1){ // свободен
										p->IArt[M.n[2]][0]=art;
										p->IArt[M.n[2]][1]=arts;
										if(((art>0)&&(art<=0x2C))||(art==0x80)){
											if(art==0x80) k=0; else k=art;
											for(j=0;j<4;j++){
												p->PSkill[j]+=(char)ArtSkill[k][j];
											}
										}
										ERMFlags[0]=1;
									}else ERMFlags[0]=0; // оповестим, что не удалось
								}
								break;
							case 2: // A2/art/num/act - посчитать арт
								aflag=anum=0;
								for(i=0;i<19;i++){
									if((p->IArt[i][0]==art)&&(p->IArt[i][1]==arts)){
										++aflag; ++anum;
									}
								}
								for(i=0;i<64;i++){
									if((p->OArt[i][0]==art)&&(p->OArt[i][1]==arts)){ ++anum; }
								}
								Apply(&anum,4,&M,2);
								Apply(&aflag,4,&M,3);
								break;
							case 3: // A3/art/num/act - отнять арт
								anum=0;  Apply(&anum,4,&M,2);
								aflag=0; Apply(&aflag,4,&M,3);
								if(aflag==0){ // удаляем в первую очередь из рюкзака
									for(i=0;i<64;i++){
										if((p->OArt[i][0]==art)&&(p->OArt[i][1]==arts)){
											if(anum==0) break;
											p->OArt[i][0]=p->OArt[i][1]=-1;
											--anum;
										}
									}
								}
								for(i=0;i<19;i++){
									if((p->IArt[i][0]==art)&&(p->IArt[i][1]==arts)){
										if(anum==0) break;
										p->IArt[i][0]=p->IArt[i][1]=-1;
										--anum;
										if(M.n[0]>0x80){ // составной
											for(j=0;;j++){ // убираем замочки
												if(ArtSlots[j]==M.n[0]){
													for(k=0,j=j+1;k<4;k++){
														p->PSkill[k]-=(char)ArtSlots[j+k];
													}
													for(j=j+4;;j++){
														k=ArtSlots[j];
														if(k>0x80) break; // дошли до конца
														p->LockedSlot[k]=(Byte)(p->LockedSlot[k]-1);
													}
												}
												if(ArtSlots[j]==0x99) break; // все
											}
										}
									}
								}
								if(aflag){ // удаляем во вторую очередь одетые
									for(i=0;i<64;i++){
										if((p->OArt[i][0]==art)&&(p->OArt[i][1]==arts)){
											if(anum==0) break;
											p->OArt[i][0]=p->OArt[i][1]=-1;
											--anum;
										}
									}
								}
								break;
							case 4: // A4/art - поместить арт (внутр функция)
								EquipArtifact(p,art,arts);
								break;
							case 5: // A5/slot/locks_count/free_count/max_count - замки на слотах
								if(M.n[1]<0 || M.n[1]>13){ MError2("wrong slot lock index (0...13)."); goto l_exit; }
								Apply(&p->LockedSlot[M.n[1]], 1, &M, 2);
								if(Num>3){
									i = k = 0;
									for(j = 0; j < 19; j++)
										if(LockGroups(j) == M.n[1])
										{
											if(p->IArt[j][0] < 0)  i++;
											k++;
										}
									Apply(&i, 4, &M, 3);
									if(Num>4)
										Apply(&k, 4, &M, 4);
								}
								break;
							default: EWrongSyntax(); goto l_exit;
						}
					}else{
						int art,arts;
						if(M.n[0]>=0){
							art=M.n[0]; arts=-1;
							if(art==1000) art=0;
							if(art>1000){ arts=art-1001; art=1; }
							if(M.VarI[0].Check!=0){ // проверка на наличие
								if(M.VarI[0].Check!=2){ MError("\"HE:A\"-wrong syntax (second argument)."); goto l_exit; } // можно только сравнивать
								ERMFlags[0]=0;
								for(i=0;i<19;i++){
									if((p->IArt[i][0]==art)&&(p->IArt[i][1]==arts)){
										ERMFlags[0]=1;
										goto _ok103;
									}
								}
								for(i=0;i<64;i++){
									if((p->OArt[i][0]==art)&&(p->OArt[i][1]==arts)){
										ERMFlags[0]=1;
										goto _ok103;
									}
								}
							}else{// добавить в рюкзак
								for(i=0;i<64;i++){
									if(p->OArt[i][0]==-1){
										p->OArt[i][0]=art;
										p->OArt[i][1]=arts;
										p->OANum=(Byte)(p->OANum+1);
										break;
									}
								}
							}
						}else{ // отнять
							art=-M.n[0]; arts=-1;
							if(art==1000) art=0;
							// для поддержки старого синтаксиса - удалить все свитки с закл
							if(art==1){ arts=-2; }
							if(art>1000){ arts=art-1001; art=1; }
							for(i=0;i<19;i++){
								if((p->IArt[i][0]==art)&&((p->IArt[i][1]==arts)||(arts==-2))){
									p->IArt[i][0]=p->IArt[i][1]=-1;
									if(art>0x80){ // составной
										for(j=0;;j++){ // убираем замочки
											if(ArtSlots[j]==art){
												for(k=0,j=j+1;k<4;k++){
													p->PSkill[k]-=(char)ArtSlots[j+k];
												}
												for(j=j+4;;j++){
													k=ArtSlots[j];
													if(k>0x80) break; // дошли до конца
													p->LockedSlot[k]=(Byte)(p->LockedSlot[k]-1);
												}
											}
											if(ArtSlots[j]==0x99) break; // все
										}
									}
									// здесь только вычитание хар. для обычных артифактов
									if(((art>0)&&(art<=0x2C))||(art==0x80)){
										if(art==0x80) k=0; else k=art;
										for(j=0;j<4;j++){
											p->PSkill[j]-=ArtSkill[k][j];
										}
									}
								}
							}
							for(i=0;i<64;i++){
								if((p->OArt[i][0]==art)&&((p->OArt[i][1]==arts)||(arts==-2))){
									p->OArt[i][0]=p->OArt[i][1]=-1;
									p->OANum=(Byte)(p->OANum-1);
								}
							}
						}
					}
_ok103:;
				}else if(Cmd=='M'){ // M#1/$2 заклинания
					char  fl;
					if(Num<2){ EWrongParamsNum(); goto l_exit; }
					if(M.VarI[0].Check!=0){ // проверка на наличие
						if(M.VarI[0].Check!=2){ MError("\"HE:M\"-wrong syntax (second argument)."); goto l_exit; } // можно только сравнивать
						if(p->Spell[M.n[0]]==0) fl=0; else fl=1;
						ERMFlags[0]=fl;
					}else{
						if((M.n[0]<0)||(M.n[0]>69)) goto l_exit;
//              p->LSpell[M.n[0]]=(Byte)(p->LSpell[M.n[0]]*M.f[1]+M.n[1]);
						if(Apply(&p->LSpell[M.n[0]],1,&M,1)) break;
						if(p->LSpell[M.n[0]]==0){ // удалить
							p->Spell[M.n[0]]=0;
						}else{
							p->Spell[M.n[0]]=1;
						}
					}
				}else if(Cmd=='F'){ // F$1/$2/$3/$4 первичные навыки
					if(Num<4){ EWrongParamsNum(); goto l_exit; }
					if(Num==4){
						Apply(&p->PSkill[0],1,&M,0);
						Apply(&p->PSkill[1],1,&M,1);
						Apply(&p->PSkill[2],1,&M,2);
						Apply(&p->PSkill[3],1,&M,3);
					}else if(Num==5){ // F$1/$2/$3/$4/1 чистые первичные навыки
						int art,ps[4];
						ps[0]=p->PSkill[0]; ps[1]=p->PSkill[1]; ps[2]=p->PSkill[2]; ps[3]=p->PSkill[3];
						switch(M.n[4]){
							case 1:
								for(i=0;i<19;i++){
									art=p->IArt[i][0];
									if(art==-1) continue;
									if(art>0x80){ // составной
										for(j=0;;j++){ // убираем замочки
											if(ArtSlots[j]==art){
												for(k=0,j=j+1;k<4;k++){
													ps[k]-=(char)ArtSlots[j+k];
												}
												for(j=j+4;;j++){
													k=ArtSlots[j];
													if(k>0x80) break; // дошли до конца
												}
											}
											if(ArtSlots[j]==0x99) break; // все
										}
									}
									// здесь только вычитание хар. для обычных артифактов
									if(((art>0)&&(art<=0x2C))||(art==0x80)){
										if(art==0x80) k=0; else k=art;
										for(j=0;j<4;j++){
											ps[j]-=ArtSkill[k][j];
										}
									}
								}
								Apply(&ps[0],4,&M,0);
								Apply(&ps[1],4,&M,1);
								Apply(&ps[2],4,&M,2);
								Apply(&ps[3],4,&M,3);
								break;
							default:
								EWrongSyntax(); goto l_exit;
						}
					}else{ EWrongParamsNum(); goto l_exit; }
				}else if(Cmd=='N'){ // N# можно только прочитать
					int v=p->Number;
					Apply(&v,4,&M,0); // можно только прочитать
				}else if(Cmd=='L'){ // L#^Name.pcx^
					if((M.n[0]<0)||(M.n[0]>5)){ EWrongSyntax(); goto l_exit; }
					if(M.n[0]==3){ // сбросить ориг
						ChangeHeroPic(p->Number,0,0);
						RedrawMap();
					}else if(M.n[0]==4){
						ChangeHeroPicN(p->Number,M.n[1]);
						RedrawMap();
					}else if(M.n[0]==0){ // установить оба в заранее заданный
						if(Num<2){ MError("\"HE:L0\"-wrong syntax."); goto l_exit; }
						CustomHPic(M.n[1],p->Number,0);
						RedrawMap();
					}else if(M.n[0]==5){ // установить оба в строковые переменные (0-не уст.)
						if(Num<3){ MError("\"HE:L5\"-wrong syntax."); goto l_exit; }
						if(BAD_INDEX_LZ_ALLOW_0(M.n[1])){ MError("\"HE:L5\"-wrong z var index (-10...-1,(0),1...1000)."); goto l_exit; }
						if(BAD_INDEX_LZ_ALLOW_0(M.n[2])){ MError("\"HE:L5\"-wrong z var index (-10...-1,(0),1...1000)."); goto l_exit; }
						char *s1=0,*s2=0;
						if(M.n[1]!=0) s1 = GetInternalErmString(M.n[1]);
						if(M.n[2]!=0) s2 = GetInternalErmString(M.n[2]);
						ChangeHeroPic(p->Number,s1,s2);
						RedrawMap();
					}else{
						for(int fl=j=0,i=M.i;i<M.m.l;i++){
							if(M.m.s[i]==STRCH){
								if(fl==1){
									M.m.s[i]=0;
									if(M.n[0]==1) // S
										ChangeHeroPic(p->Number,0,&M.m.s[j]);
									else // L
										ChangeHeroPic(p->Number,&M.m.s[j],0);
									RedrawMap();
									M.m.s[i]=STRCH;
									ERMFlags[M.n[0]-1]=(char)fl;
									M.i=i+1;
									break;
								}else{
									j=i+1;
									fl=1;
								}
							}
						}
					}
				}else if(Cmd=='W'){ // W$1 пункты перемещений
					Apply(&p->Movement,4,&M,0);
					if(Num<2) RedrawMap();
				}else if(Cmd=='G'){ // 3.58 G$1 пункты перемещений начальные
					Apply(&p->Movement0,4,&M,0);
//            if(Num<2) RedrawMap();
				}else if(Cmd=='K'){ // K убить героя
					KillHero(p->Number);
					if(Num<2) RedrawMap();
				}else if(Cmd=='E'){ // E$(/$?level) добавить опыта
					int retflag=0;
					if(Num>1){ Apply(&p->ExpLevel,2,&M,1); retflag=1; }
					if(Apply(&p->Exp,4,&M,0)) retflag=1;
					if(retflag) break; // if check first or second
					AddExp(p);
					if(Num<3) RedrawMap();
				}else if(Cmd=='V'){
				 {
					Dword mask;
					int v;
					if((M.n[0]<0)||(M.n[0]>=10)){ MError("\"HE:V\"-wrong object type index (0...10)."); goto l_exit; }
					if(Num==2){       // V#/# посещенность объектов
						Apply(&p->Visited[M.n[0]],4,&M,1);
					}else if(Num==3){ // V#/#/# посещенность объектов
						if((M.n[1]<0)||(M.n[1]>=32)){ MError("\"HE:V\"-wrong object index (0...32)."); goto l_exit; }
						mask=1;
						for(i=0;i<M.n[1];i++) mask<<=1;
						if(p->Visited[M.n[0]]&mask) v=1; else v=0;
						if(Apply(&v,4,&M,2)) break;
						if(v) p->Visited[M.n[0]]|=mask; // установим
						else  p->Visited[M.n[0]]&=~mask;
					}
				 }
				}else if(Cmd=='B'){ // имя или биос или класс
					if(Num<2){ EWrongParamsNum(); goto l_exit; }
					switch(M.n[0]){
						case 0: // B0/$ - имя
							StrMan::Apply(p->Name, &M, 1, 13);
							break;
						case 1: // B1/$ - биос
							ApplyString(&M,1,&p->Bibl);
							if(M.VarI[1].Check==0) p->fl_B=1;
							break;
						case 2: // B2/$ - класс
							Apply(&p->Spec,4,&M,1);
							break;
						case 3: // B3/$ - получить умолчательный биос
							if(M.VarI[1].Check==1){ // ?
								if(M.VarI[1].Type != 7){ MError("\"HE:B3\"-z var required."); goto l_exit; }
								int sind = GetVarIndex(&M.VarI[1], false);
								char * str = sind ? GetPureErmString(sind) : 0;
								if (str == 0) goto l_exit;
								StrCopy(str, 512, HBiosTable[p->Number].HBios);
							}else{
								MError("\"HE:B3\"-cannot use set syntax."); goto l_exit;
							}
							break;
						default:
							EWrongSyntax(); goto l_exit;
					}
				}else if(Cmd=='R'){ //
					int v,en,posred; char *pho=GetHOTableBase(); int *phh=GetHHTableBase();
					if(Num<2){ EWrongParamsNum(); goto l_exit; }
					posred=1;
					switch(M.n[0]){
						case 0: // R0/$ - мораль
							if(Apply(&p->DMorale1,1,&M,1)) posred=0;
							break;
						case 1: // R1/$ - удача
							if(Apply(&p->DLuck,1,&M,1)) posred=0;
							break;
						case 2: // R2/$ - пол
							if(Apply(&p->Sex,1,&M,1)) posred=0;
							break;
						case 3:
							if(Num<3){ // R3/$ - разрешен(1)/запрещен(0)
								en=0xFF;
							}else{ // R3/$1/$2 - разрешен(1)/запрещен(0) для игроков (1бит=1цвет)
								en=phh[p->Number]; // всем
								if(Apply(&en,4,&M,2)){ posred=0; break; }
							}
							if(M.VarI[1].Check==0){ // устанавливаем
								if(M.n[1]!=0){ // разрешаем
									if(pho[p->Number]==0x40) pho[p->Number]=-1;
									phh[p->Number]=en;
								}else{ // запрещаем
									if(pho[p->Number]==0x40){ break; }
									if(pho[p->Number]==-1){ pho[p->Number]=0x40; break; }
									// 3.57m2 если герой на карте, то его нельзя убить
									// KillHero(p->Number);
									// RedrawMap();
									pho[p->Number]=0x40;
								}
							}else{ // проверяем
								v=1; if(pho[p->Number]==0x40) v=0;
								Apply(&v,4,&M,1);
							}
							break;
						case 4: // R4/$ - разрешена тактика или нет
							if(p->Flags&0x02) v=1; else v=0;
							if(Apply(&v,4,&M,1)) posred=0;
							if(v) p->Flags|=0x02; else p->Flags&=0xFD;
							break;
						case 5: // R5/$ бит максимум морали (временный)
							posred=0;
							if(p->TempMod&0x800000) v=1; else v=0;
							if(Apply(&v,4,&M,1)) break;
							if(v) p->TempMod|=0x800000; else p->TempMod&= ~0x800000;
							break;
						case 6: // R6/$ бит максимум удачи (временный)
							posred=0;
							if(p->TempMod&0x400000) v=1; else v=0;
							if(Apply(&v,4,&M,1)) break;
							if(v) p->TempMod|=0x400000; else p->TempMod&= ~0x400000;
							break;
						case 7: // 3.59 R7/$ все модификаторы в одном (временный)
							posred=0;
							v=p->TempMod; if(Apply(&v,4,&M,1)) break; p->TempMod=v;
							break;
						default: EWrongSyntax(); goto l_exit;
					}
					if(Num>2){ // есть бит перерисовки
						v=1; Apply(&v,4,&M,2);
						if(v==1) RedrawMap();
					}else{
						if(posred) RedrawMap();
					}
				}else if(Cmd=='Y'){ // Ynum/power/length/flag проклятие
					// flag = 1-set,0-del,2-add/sub,3=del all
					if(ERM_Curse(M, Num, p->Number)) goto l_exit;
					RedrawMap();
				}else if(Cmd=='T'){ // битва Tx/y/l/mtype/mnum
					if(Num<5){ EWrongParamsNum(); goto l_exit; }
						if(DoBattle(&M.n[4],p,M.n[3],PosMixed(M.n[0],M.n[1],M.n[2])))
							ERMFlags[0]=1; else ERMFlags[0]=0;
				}else if(Cmd=='H'){ // Hslot/type/min/max настройки начальной армии героя
					if(Num<4){ EWrongParamsNum(); goto l_exit; }
					if((M.n[0]<0)||(M.n[0]>2)){ MError("\"HE:H\"-slot number out of range (0...2)."); goto l_exit; }
					Apply(&HTable[p->Number].ArmyType[M.n[0]],4,&M,1);
					Apply(&HTable[p->Number].ArmyNum[M.n[0]][0],4,&M,2);
					Apply(&HTable[p->Number].ArmyNum[M.n[0]][1],4,&M,3);
				}else if(Cmd=='D'){ // D creature upgrade
					FortOnHill(p->Number);
					RedrawRes();
/*
				}else if(Cmd=='G'){ // Gx/y/l/sound go to Boat
					if(Num<4){ EWrongParamsNum(); goto l_exit; }
					int x,y,l,sound,pdx,pdy;
					if(Apply(&x,4,&M,0)) break;
					if(Apply(&y,4,&M,1)) break;
					if(Apply(&l,4,&M,2)) break;
					if(Apply(&sound,4,&M,3)) break;
//            p->x=x; p->y; p->l=l;
//            JumpHero(p,x,y,l,sound);
					pdx=x-p->x; pdy=y-p->y;
					__asm{
int 3
						push   pdy
						push   pdx
						push   2
						push   p
						mov    ecx,0x6992B8
						mov    ecx,[ecx]
						mov    eax,0x47FCA0
						call   eax
					}
					_MapItem_ *mapitem=GetMapItem(x,y,l);
					__asm{
						mov   eax,mapitem
						mov   ecx,p
						push  eax
						push  ecx
						mov   ecx,0x6992B8
						mov   ecx,[ecx]
						mov   eax,0x4A0C80
						call  eax
					}
*/
				}else if(Cmd==' '){
				}else{
					EWrongCommand(); goto l_exit;
				}
			}
			break;
		case 0x4649: // IF 'IF'
			{
				int  fl,pt2,ps2,pt3,ps3,type,sind;
// unfortunately work only in MP mode 4F703E
//          if(Cmd=='T'){
//            Apply(&NextMessShowTime,4,&M,0);
//          }else 
				int showtime = NextMessShowTime;
				int x = NextMessX;
				int y = NextMessY;
				switch(Cmd)
				{
					case 'N': if (Num<4) break;
					case 'M':
					case 'Q':
					// for ZVSLib dialigs too?
						NextMessShowTime = 0;
						NextMessX = -1;
						NextMessY = -1;
				}
					

				if(Cmd=='Q'){ // 1000 флагов
					if((M.n[0]<1)||(M.n[0]>1000)){ MError("\"IF:Q\"-wrong flag number (1...1000)."); goto l_exit; }
					if(Num==1){ // обычный вопрос
						ERMFlags[M.n[0]-1]=(char)Request0(ERM2String(&M.m.s[M.i],0,&i),showtime);
						M.i+=i;
						break;
					}else if(Num==2){ // вопрос с переменной
						char *str = GetErmText(&M, 1);
						if (str == 0) goto l_exit;
						ERMFlags[M.n[0]-1] = (char)Request0(str, showtime/*,x,y*/);
					}else{
						if(Num<6){ // + одна картинка Qfl/pict/pics^text^
							type=M.n[3]; pt2=-1; ps2=0; pt3=-1;ps3=0; sind=M.n[4];
						}else if(Num<8){ // + две картинки
							// Qfl/pic1t/pic1s/pic2t/pic2s/type^text^
							type=M.n[5]; pt2=M.n[3]; ps2=M.n[4]; pt3=-1;ps3=0; sind=M.n[6];
						}else if(Num<10){ // + 3 картинки
							// Qfl/pic1t/pic1s/pic2t/pic2s/pic3t/pic3s/type^text^
							type=M.n[7]; pt2=M.n[3]; ps2=M.n[4]; pt3=M.n[5];ps3=M.n[6]; sind=M.n[8];
						}else{ EWrongParamsNum(); goto l_exit; }
						if(Num&1){ // нечетный - переменная
							char *str = GetErmString(sind);
							if (str == 0) goto l_exit;
							fl = Request3Pic(str, M.n[1], M.n[2], pt2, ps2, pt3, ps3, type, showtime,x,y);
						}else{ // четный - есть сообщение
						 fl=Request3Pic(ERM2String(&M.m.s[M.i],0,&i),M.n[1],M.n[2],pt2,ps2,pt3,ps3,type,showtime,x,y);
						 M.i+=i;
					 }
					 if(type==10){ // 3.58+
						 ERMVar2[M.n[0]-1]=fl;
					 }else if(type!=1){
						 ERMFlags[M.n[0]-1]=(char)fl;
					 }
					 break;
				 }
				}else if(Cmd=='M'){ // M^text^ , M1/$
					if(Num==3){ // с типом текстом или переменной
						int var=1; Apply(&var,4,&M,1);              
						//  1-сообщение
						//  2-запрос 
						//  4-инфа по правой мышке
						//  7-просьба выбрать
						// 10-можно и выбрать и отказаться
						char *str = GetErmText(&M, 2);
						if (str == 0) goto l_exit;
						var=Message(str,var,showtime/*,x,y*/);
						Apply(&var,4,&M,0);
					}else if(Num==2){ // с переменной
						if(M.n[0]==1){
							char *str = GetErmString(M.n[1]);
							if (str == 0) goto l_exit;
							Message(str, 1, showtime/*,x,y*/);
						}else{ MError("\"IF:M\"-wrong syntax (M1/$)."); goto l_exit; }
					}else{
						if (M.VarI[0].Type == 7)
						{
							char *str = GetErmString(M.n[0]);
							if (str == 0) goto l_exit;
							Message(str,1,showtime/*,x,y*/);
						}else
						{
							Message(ERM2String(&M.m.s[M.i],0,&i),1,showtime/*,x,y*/);
							M.i+=i;
						}
					}
					break;
				}else{
					switch(Cmd){
						/*
						case 'T':
							if(Num!=1) goto l_exit;
							Apply(&NextMessShowTime, 4, &M, 0);
							break;
						*/
						case 'V': // 1000 флагов
							if(Num<2) goto l_exit;
							if((M.n[0]<1)||(M.n[0]>1000)){ MError("\"IF:V\"-wrong flag index."); goto l_exit; }
							if(M.n[1]) fl=1; else fl=0; // установлен бит
							ERMFlags[M.n[0]-1]=(char)fl;
							break;
						case 'A': // только для флагов с 0 до 10
						case 'S': //
						case 'R': //
							for(i=9;i>=0;i--,M.n[0]/=10){
								fl=M.n[0]&1; // установлен бит
								if((Cmd=='S')&&(fl==0)) continue;
								if(Cmd=='R'){ if(fl==0) continue; else fl=0; }
								ERMFlags[i]=(char)fl;
							}
							break;
						case 'W': // установить индекс героя для переменных
							if(M.n[0]==-1){ // текущий герой
								struct _Hero_ *p=ERM_HeroStr;
								if(p==0){ MError("\"IF:W\"-cannot find hero."); goto l_exit; }
								M.n[0]=p->Number;
							}
							if((M.n[0]<0)||(M.n[0]>=HERNUM)){ MError("\"IF:W\"-hero number out of range."); goto l_exit; }
							ERMW=M.n[0];
							break;
						case 'P': // картинка/видео
							CustomPic(M.n[0],0);
							break;
						case 'E': // запрос
							if(Num==2){
								char *str;
								if((M.n[0]<1)||(M.n[0]>VAR_COUNT_V)){ MError("\"IF:E\"-wrong var index (1...10000)."); goto l_exit; }
								ERMVar2[M.n[0]-1]=CustomReq(M.n[1],0,&str);
								StrCopy(ERMString[0],512,str);
							}else{
								CustomReq(M.n[0],0,0);
							}
							break;
						case 'X': // X$ стандартный сфинкс номер
							if(M.n[0]==-1) M.n[0]=Random(0,TXTSphinx.sn-1);
							ERMFlags[0]=(char)SphinxReq(M.n[0]);
							break;
						case 'B': // B#/pic/rep установить картинку
							if(Num<3){ EWrongParamsNum(); goto l_exit; }
							if(BAD_INDEX_LZ(M.n[1])){ MError("\"IF:B\"-wrong z var index (-10...-1,1...1000)."); goto l_exit; }
							if(M.n[1]>1000)                AddExtCMDPic(M.n[0],ERM2String2(0,StringSet::GetText(M.n[1])),M.n[2]);
							else if(M.n[1]>0)              AddExtCMDPic(M.n[0],ERMString[M.n[1]-1],M.n[2]);
							else if(M.n[1]>=-VAR_COUNT_LZ) AddExtCMDPic(M.n[0],ERMLString[-M.n[1]-1],M.n[2]);
							else                           AddExtCMDPic(M.n[0],ERM2String2(0,ERMLString[-M.n[1]-1]),M.n[2]);
							break;
						case 'F': // F#/... установить диалог +...
						 {
							 int   tv;
							 char  HasCancel;
							 char *bth1,*bth2,*bth3,*bth4;
							 if(Num!=6){ EWrongParamsNum(); goto l_exit; }
							 tv=0;
							 if(Apply(&tv,4,&M,5)){ MError("\"IF:F\"-cannot get or check var."); goto l_exit; }
							 if(tv!=0) HasCancel=1; else HasCancel=0;

							 if(Apply(&tv,4,&M,4)){ MError("\"IF:F\"-cannot get or check var."); goto l_exit; }
							 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:F\"-wrong z var index (-10,...-1,(0),1...1000)."); goto l_exit; }
							 bth4 = (tv ? ERM2String2(1,GetPureErmString(tv)) : 0);

							 if(Apply(&tv,4,&M,3)){ MError("\"IF:F\"-cannot get or check var."); goto l_exit; }
							 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:F\"-wrong z var index (-10,...-1,(0),1...1000)."); goto l_exit; }
							 bth3 = (tv ? ERM2String2(2,GetPureErmString(tv)) : 0);

							 if(Apply(&tv,4,&M,2)){ MError("\"IF:F\"-cannot get or check var."); goto l_exit; }
							 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:F\"-wrong z var index (-10,...-1,(0),1...1000)."); goto l_exit; }
							 bth2 = (tv ? ERM2String2(3,GetPureErmString(tv)) : 0);

							 if(Apply(&tv,4,&M,1)){ MError("\"IF:F\"-cannot get or check var."); goto l_exit; }
							 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:F\"-wrong z var index (-10,...-1,(0),1...1000)."); goto l_exit; }
							 bth1 = (tv ? ERM2String2(4,GetPureErmString(tv)) : 0);

							 if(Apply(&tv,4,&M,0)){ MError("\"IF:F\"-cannot get or check var."); goto l_exit; }
							 if(AddExtCMDRequest(tv,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
								 bth1,bth2,bth3,bth4,HasCancel)==0){ MError("\"IF:F\"-cannot show dialog."); goto l_exit; }
							}
							break;
						case 'G': // GchRAD/var/state/header/txt1... диалог с ключиками
						{
							int   v,state,j,chRAD;
							char *htxt,*txt[12];
							if(Num<5){ EWrongParamsNum(); goto l_exit; }
							if(!CheckVarIndex(M.n[1], 3, false)) break;
							if(Apply(&chRAD,4,&M,0)){ MError("\"IF:G\"-cannot get or check var."); goto l_exit; }
							if(Apply(&state,4,&M,2)){ MError("\"IF:G\"-cannot get or check var."); goto l_exit; }
							if(Apply(&v,4,&M,3)){ MError("\"IF:G\"-cannot get or check var."); goto l_exit; }
							if(BAD_INDEX_LZ_ALLOW_0(v)){ MError("\"IF:G\"-wrong z var index (-10...-1,(0),1...1000+)."); goto l_exit; }
							htxt = (v ? ERM2String2(5, GetPureErmString(v)) : "");
							for(j=0;j<(Num-4);j++){
								if(Apply(&v,4,&M,(char)(j+4))){ MError("\"IF:G\"-cannot get or check var."); goto l_exit; }
								// 3.57f check
								if(j>11){ MError("\"IF:G\"-too many items."); goto l_exit; }
								if(v==0) txt[j]=0;
								else{
									if(BAD_INDEX_LZ(v)){ MError("\"IF:G\"-wrong z var index (-10...-1,1...1000+)."); goto l_exit; }
									txt[j] = ERM2String2(6+j,GetPureErmString(v));
								}
							}
							for(;j<12;j++) txt[j]=0;
							if (LuaCallStart("CheckBoxesDialog")) break;
							lua_pushboolean(Lua, chRAD);
							lua_pushnumber(Lua, state);
							lua_pushstring(Lua, htxt);
							for (int i = 0; i < 12; i++){
								if (txt[i])
									lua_pushstring(Lua, txt[i]);
								else
									lua_pushnil(Lua);
							}
							ERMVar2[M.n[1]-1] = LuaCallEnd();
							break;
						}
						case 'D': // D#/... установить диалог...
						 {
							 int   tv;
							 char  HasCancel;
							 char *txt1,*txt2,*txt3,
										*pic1,*pic2,*pic3,*pic4,
										*pch1,*pch2,*pch3,*pch4,
										*but1,*but2,*but3,*but4,
										*bth1,*bth2,*bth3,*bth4;
							 if(Num<1){ EWrongParamsNum(); goto l_exit; }
							 tv=0;
							 if(Num>15){
								 if(Apply(&tv,4,&M,15)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var index1(-10...-1,(0),1...1000+)."); goto l_exit; }
								 but4 = (tv ? ERM2String2(18,GetPureErmString(tv)) : 0);
							 }else but4=0;
							 if(Num>14){
								 if(Apply(&tv,4,&M,14)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var index2(-10...-1,(0),1...1000+)."); goto l_exit; }
								 but3 = (tv ? ERM2String2(19,GetPureErmString(tv)) : 0);
							 }else but3=0;
							 if(Num>13){
								 if(Apply(&tv,4,&M,13)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var index3(-10...-1,(0),1...1000+)."); goto l_exit; }
								 but2 = (tv ? ERM2String2(20,GetPureErmString(tv)) : 0);
							 }else but2=0;
							 if(Num>12){
								 if(Apply(&tv,4,&M,12)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var index4(-10...-1,(0),1...1000+)."); goto l_exit; }
								 but1 = (tv ? ERM2String2(21,GetPureErmString(tv)) : 0);
							 }else but1=0;
							 if(Num>11){
								 if(Apply(&tv,4,&M,11)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var index5(-10...-1,(0),1...1000+)."); goto l_exit; }
								 pch4 = (tv ? ERM2String2(22,GetPureErmString(tv)) : 0);
							 }else pch4=0;
							 if(Num>10){
								 if(Apply(&tv,4,&M,10)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var index6(-10...-1,(0),1...1000+)."); goto l_exit; }
								 pch3 = (tv ? ERM2String2(23,GetPureErmString(tv)) : 0);
							 }else pch3=0;
							 if(Num>9){
								 if(Apply(&tv,4,&M,9)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var index7(-10...-1,(0),1...1000+)."); goto l_exit; }
								 pch2 = (tv ? ERM2String2(24,GetPureErmString(tv)) : 0);
							 }else pch2=0;
							 if(Num>8){
								 if(Apply(&tv,4,&M,8)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var index8(-10...-1,(0),1...1000+)."); goto l_exit; }
								 pch1 = (tv ? ERM2String2(25,GetPureErmString(tv)) : 0);
							 }else pch1=0;
							 if(Num>7){
								 if(Apply(&tv,4,&M,7)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var index9(-10...-1,(0),1...1000+)."); goto l_exit; }
								 pic4 = (tv ? ERM2String2(26,GetPureErmString(tv)) : 0);
							 }else pic4=0;
							 if(Num>6){
								 if(Apply(&tv,4,&M,6)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var indexA(-10...-1,(0),1...1000+)."); goto l_exit; }
								 pic3 = (tv ? ERM2String2(27,GetPureErmString(tv)) : 0);
							 }else pic3=0;
							 if(Num>5){
								 if(Apply(&tv,4,&M,5)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var indexB(-10...-1,(0),1...1000+)."); goto l_exit; }
								 pic2 = (tv ? ERM2String2(28,GetPureErmString(tv)) : 0);
							 }else pic2=0;
							 if(Num>4){
								 if(Apply(&tv,4,&M,4)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var indexC(-10...-1,(0),1...1000+)."); goto l_exit; }
								 pic1 = (tv ? ERM2String2(29,GetPureErmString(tv)) : 0);
							 }else pic1=0;
							 if(Num>3){
								 if(Apply(&tv,4,&M,3)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var indexD(-10...-1,(0),1...1000+)."); goto l_exit; }
								 txt3 = (tv ? ERM2String2(30,GetPureErmString(tv)) : 0);
							 }else txt3=0;
							 if(Num>2){
								 if(Apply(&tv,4,&M,2)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var indexE(-10...-1,(0),1...1000+)."); goto l_exit; }
								 txt2 = (tv ? ERM2String2(31,GetPureErmString(tv)) : 0);
							 }else txt2=0;
							 if(Num>1){
								 if(Apply(&tv,4,&M,1)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
								 if(BAD_INDEX_LZ_ALLOW_0(tv)){ MError("\"IF:D\"-wrong z var indexF(-10...-1,(0),1...1000+)."); goto l_exit; }
								 txt1 = (tv ? ERM2String2(32,GetPureErmString(tv)) : 0);
							 }else txt1=0;
							 if(Apply(&tv,4,&M,0)){ MError("\"IF:D\"-cannot get or check var."); goto l_exit; }
							 bth1=but1;bth2=but2;bth3=but3;bth4=but4;HasCancel=1;
							 if(AddExtCMDRequest(tv,txt1,txt2,txt3,pic1,pic2,pic3,pic4,
								 pch1,pch2,pch3,pch4,but1,but2,but3,but4,
								 bth1,bth2,bth3,bth4,HasCancel)==0){ MError("\"IF:D\"-cannot show dialog."); goto l_exit; }
							}
							break;
						case 'N':{ // 3.58
							char *txt;
							if(Num<4){ // show
								// N^text^ N1/$
								if(Num==2){ // с переменной
									if(M.n[0]==1){
										if (GetPureErmString(txt, M.n[0])) goto l_exit;
										txt = ERM2String2(32,txt);
									}else{ MError("\"IF:N\"-wrong syntax (M1/$)."); goto l_exit; }
								}else{
									txt=ERM2String(&M.m.s[M.i],0,&i);
									M.i+=i;
								}
								Display8Dialog(txt,IF_N_Ar,x,y);
							}else{ // prepare
							 // Np1t/p1s/p2t/p2s/p3t/p3s/p4t/p4s/p5t/p5s/p6t/p6s/p7t/p7s/p8t/p8s
								for(i=0;i<9;i++){ IF_N_Ar[i][0]=-1; IF_N_Ar[i][1]=0; }
								for(i=0;i<Num/2;i++){
									if(Apply(&IF_N_Ar[i][0],4,&M,(char)(i*2))){ MError("\"IF:N\"-cannot get or check var."); goto l_exit; }
									if(Apply(&IF_N_Ar[i][1],4,&M,(char)(i*2+1))){ IF_N_Ar[i][0] = -1; MError("\"IF:N\"-cannot get or check var."); goto l_exit; }
								}
							}
						}
						break;
						case 'L':{ // L#   3.59 - put message to log
							char *lcstr="";
							if(BAD_INDEX_LZ_ALLOW_0(M.n[0])){ MError("\"IF:L\"-wrong z var index (-10...-1,1...1000)."); goto l_exit; }
							if(M.n[0]!=0)   lcstr=GetErmString(M.n[0]);
							else{ lcstr=ERM2String(&M.m.s[M.i],0,&i); M.i+=i; }
							_asm{
								mov  eax,lcstr
								push eax
								mov  eax,0x69D800
								push eax
								mov  eax,0x553C40
								call eax
								add  esp,8
							}
						}
						break;
						default:
							EWrongCommand(); goto l_exit;
					}
				}
			}
			break;

		default:
			if(Cmd==' ') break;
			if(Cmd==0x0A) break;
			Word id1 = (Id << 8) + (Id >> 8);
			for(j=0;;j++){
				if(ERM_Addition[j].Id==0) break;
				if(ERM_Addition[j].Id!=id1) continue;
				if(ERM_Addition[j].Fun(Cmd,Num,sp,&M)) goto l_ok;
			}
			goto l_exit;
	}
l_ok:
	ErrorCmd.Cmd = oldCmd;
	RETURN(0)
l_exit:
	ErrorCmd.Cmd = oldCmd;
	RETURN(1)
}



void ProcessCmd(_ToDo_ *sp)
{
	STARTNA(__LINE__, 0)
	Mes   M;
	int   Ind,Num,Start;
	char  Cmd;

	long long oldCmd = ErrorCmd.Cmd;
	M.m.s=sp->Com.s;   // начало строки-команды в описании (конец - ;)
	GEr.LastERM(sp->Self.s); // 3.58
	M.m.l=sp->Com.l;    // длина строки-команды
	M.i=0;
// disable to process
	do{
		Start=Ind=M.i;
		ErrString.str=&M.m.s[Ind]; // 3.57f extended error log
		Cmd=M.m.s[M.i++];
		M.c[0]=1;
		if((Num=GetNumAuto(&M))==0) goto l_exit;
		if(Cmd=='Z' && (sp->Type == 'EG' || sp->Type == 'EL')){
			SetErrorCmd(sp->Type, Cmd);
			if(Num!=1){ EWrongParamsNum(); goto l_exit; }
			if(ProcessDisable(sp,M.n[0])) goto l_exit;
			continue;
		}
		if(sp->Disabled) RETURNV
		
		if(ProcessMes(sp, M, Cmd, Num)) goto l_exit;

	} while((M.m.s[M.i]!=';')&&(M.i<M.m.l));
	RETURNV

l_exit:
	ErrorCmd.Cmd = oldCmd;
	M.i=Ind;
	if(PL_ERMErrDis==0 && ErrString.str!=LuaErrorString){
		Mess(&M);
	}
	RETURNV
}

//////////////////////////////////////////////////////////////

//static Dword Esi_;
static Word ScriptIndex4ERM=0;
static int   GameWasLoaded;
_Cmd_ *FirstTrigger;
_Cmd_ *LastAddedTrigger; // последний триггер в куче
_Cmd_ *lastTrigger; // последний триггер в списке исполнения
int TriggerCount;

// восстанавливает настройки для карт WoG без скриптов
// (считаем, что такие карты не требуют demolition'а, усиленных башен и оставления армии)
void ResetWoGNoScripts(void)
{
	STARTNA(__LINE__, 0)
	PL_ExtDwellStd = PL_OptionReset[0];
	PL_NoNPC = PL_OptionReset[3];
	for(int i = 7; i <= 10; i++)
		PL_WoGOptions[0][i] = PL_OptionReset[i];

	for(int i = 0; i < PL_OptionReset2Size; i++)
		PL_WoGOptions[0][900 + i] = PL_OptionReset2[i];

	ResetNPC();
	RETURNV
}
// восстанавливает настройки для карт со старыми скриптами
void ResetWoG356(void)
{
	STARTNA(__LINE__, 0)
	for(int i = 0; i <= 10; i++)
		PL_WoGOptions[0][i] = PL_OptionReset[i];

	for(int i = 0; i < PL_OptionReset2Size; i++)
		PL_WoGOptions[0][900 + i] = PL_OptionReset2[i];

	ResetNPC();
	RETURNV
}
// восстанавливает настройки для классических карт
void ResetNoWoG(void)
{
	ResetWoG356();
	PL_TowerStd = 1;
	PL_MLeaveStd = 1;
	PL_NoTownDem = 1;
	PL_NewHero = 0;
}
// устанавливает настройки для ВоГифицированных карт
void ResetWogify(void)
{
	STARTNA(__LINE__, 0)
	for(int i=0;i<PL_WONUM;i++){
		PL_WoGOptions[0][i]=PL_WoGOptions[1][i];
	}
	//PL_CrExpStyle=0; 
	ResetNPC();
	RETURNV
}
// 3.58 как 3.57
// устанавливает настройки для ВоГифицированных карт СО СКРИПТАМИ и 3.57 стиль
void ResetWoG358_357(void)
{
	STARTNA(__LINE__, 0)
	// сбросим все новые опции
	for(int i = 0; i < PL_OptionReset2Size; i++)
		PL_WoGOptions[0][900 + i] = PL_OptionReset2[i];
	RETURNV
}
// Сброс установок 3.59 до 3.58
void ResetWoG359(void)
{
	STARTNA(__LINE__, 0)
	PL_NewHero = PL_OptionReset2[7];
	RETURNV
}
// 3.58
void ResetCustomSettings(char *FName)
{
	STARTNA(__LINE__, 0)
	// грузим только первую половину
	int k=LoadSetupState(FName,PL_WoGOptions,sizeof(PL_WoGOptions)/2);
	if(k<0){
		Message(ITxt(200,0,&Strings),1);
		RETURNV
	}
	//if(CountERMScripts()==0) LoadERMTXT(1);
	PL_CustomOptionWasSet=1;//CountERMScripts();
	ResetNPC();
	RETURNV
}

int InitTrigger(Mes &M, Word Id, int Num, Byte PostFlag, int InsertIndex)
{
	STARTNA(__LINE__, 0)

	int Ind = M.i;
	_Cmd_ *cp = LastAddedTrigger;
	if(cp!=0)
		cp = (_Cmd_ *)((Dword)cp + (Dword)sizeof(_Cmd_)+sizeof(_ToDo_)*(cp->Num-1));
	else
		cp = Heap;

	long long oldCmd = SetErrorTrigger(Id, PostFlag ? '$' : '?');

	Word id1 = (Id << 8) + (Id >> 8); // reversed
	PostFlag = PostFlag ? true : 0;
	switch(id1)
	{
		case 'LE': // LE##/##/#;
			if(Num!=3){ EWrongParamsNum(); goto l_exit; }
			cp->Event=0x20000000|PosMixed(M.n[0],M.n[1],M.n[2]);
			if(PostFlag) cp->Event|=0x08000000;
//              cp->Par[0]=M.VarI[0];
//              cp->Par[1]=M.VarI[1];
//              cp->Par[2]=M.VarI[2];
//              cp->ParSet=3;
			break;
		case 'GE': // GE#;
		{
			if(Num!=1){ EWrongParamsNum(); goto l_exit; }
			if(M.n[0]<0){ MError2("wrong event index"); goto l_exit; }
			_GlbEvent_ *GEp1 = Main->GEp1;
			_GlbEvent_ *GEp = Main->GEp0;
			for(;GEp<GEp1;GEp++){
				if(MakeSpec(&GEp->Mes.m,M.n[0])) goto _found5;
			}
			MError2("wrong event index"); goto l_exit;
_found5:
			cp->Event=(Dword)GEp; // адр. структуры события-генератора
//              cp->ParSet=0;
			break;
		}
		case 'OB': // OB#x/#y/#l; OB#t; OB#t/#st;
			switch(Num){
				case 1: // OB#t;
					cp->Event=0x40000000|(M.n[0]<<12);
					if(PostFlag) cp->Event|=0x08000000;
//                  cp->Par[0]=M.VarI[0]; cp->ParSet=1;
					break;
				case 2: // OB#t/#st;
					cp->Event=0x40000000|((M.n[0]<<12)+(M.n[1]+1));
					if(PostFlag) cp->Event|=0x08000000;
//                  cp->Par[0]=M.VarI[0]; cp->Par[1]=M.VarI[1]; cp->ParSet=2;
					break;
				case 3: // OB#x/#y/#l;
					cp->Event=0x10000000|PosMixed(M.n[0],M.n[1],M.n[2]);
					if(PostFlag) cp->Event|=0x08000000;
//                  cp->Par[0]=M.VarI[0]; cp->Par[1]=M.VarI[1]; cp->Par[2]=M.VarI[2]; cp->ParSet=3;
					break;
				default: EWrongParamsNum(); goto l_exit;
			}
			break;
		case 'FU':{ // FU#;
			if(Num!=1){ EWrongParamsNum(); goto l_exit; }
			if((M.n[0]<-100)||(M.n[0]==0)||(M.n[0]>30000)){ MError2("wrong function index (-100...30000)."); goto l_exit; }
			int funind=M.n[0];
			if(funind<0) funind=-funind+31000-1; // local functions
			cp->Event=funind;
//              cp->Par[0]=M.VarI[0]; cp->ParSet=1;
			if(ERMVarUsedStore==1) LogERMFunctionTrigger(M.n[0],&M.m.s[M.i]);
			break;}
		case 'TM': // TM#;
			if(Num==4)
			{
				int i = AddTimer(M.n[0], M.n[1], M.n[2], M.n[3]);
				if (i<0) goto l_exit;
				cp->Event = i + 31000;
				break;
			}
			if(Num!=1){ EWrongParamsNum(); goto l_exit; }
			if((M.n[0]<1)||(M.n[0]>100)){ MError2("wrong timer index (1...100)."); goto l_exit; }
			cp->Event=M.n[0]-1+30000; // сдвигаем
//              cp->Par[0]=M.VarI[0]; cp->ParSet=1;
			if(ERMVarUsedStore==1) LogERMTimerTrigger(M.n[0],&M.m.s[M.i]);
			break;
		default:
			char *foundRange = "";
			int foundMin, foundMax;
			bool found = false;
			int i = 0;
			for (; true; i++)
			{
				_ERM_Trigger_ trig = ERM_Triggers[i];
				if (trig.Id == 0)
				{
					if (found)
					{
						foundRange = Format((foundMax == foundMin? "%s%d" : "%s%d...%d"), foundRange, foundMin, foundMax);
						MError2(Format("wrong index (%s).", foundRange));
						goto l_exit;
					}
					else
					{
						MError2("wrong or not yet implemented trigger type.");
						goto _cont;
					}
				}
				if (trig.Id == id1 && trig.post == PostFlag)
				{
					if(M.n[0] >= trig.paramMin && M.n[0] <= trig.paramMax)
						break;

					if(!found || trig.paramMin != foundMax + 1)
					{
						if (found)
							foundRange = Format((foundMax == foundMin? "%s%d," : "%s%d...%d,"), foundRange, foundMin, foundMax);
						foundMin = trig.paramMin;
						foundMax = trig.paramMax;
						found = true;
					}
					else
					{
						foundMax = trig.paramMax;
					}
				}
			}
			if(Num!=1){ EWrongParamsNum(); goto l_exit; }
			cp->Event = ERM_Triggers[i].Event - ERM_Triggers[i].paramMin + M.n[0];
	}

	LastAddedTrigger = cp;
	cp->Type=Id;
	cp->Scope = ScriptIndex4ERM; // 3.59

	cp->Num=0;
	for(int k=0;k<16;k++){
		VNCopy(&M.Efl[0][k][0],&cp->Efl[0][k][0]);
		VNCopy(&M.Efl[0][k][1],&cp->Efl[0][k][1]);
		VNCopy(&M.Efl[1][k][0],&cp->Efl[1][k][0]);
		VNCopy(&M.Efl[1][k][1],&cp->Efl[1][k][1]);
	}
	cp->Disabled=cp->DisabledPrev=0;
	if(ERMVarUsedStore==1) LogTriggerConditions(cp,&M.m.s[Ind]); // !!! check what this does

	// Insert cp in triggers linked list
	if (InsertIndex < 0)
		InsertIndex = TriggerCount + 1 + InsertIndex;

	if (InsertIndex > 0 && InsertIndex < TriggerCount)
	{
		InsertIndex--;
		_Cmd_ *after = FirstTrigger;
		for (; InsertIndex > 0; InsertIndex--) after = after->Next;
		cp->Next = after->Next;
		after->Next = cp;
	}else
	{
		if (InsertIndex <= 0)
		{
			cp->Next = FirstTrigger;
			FirstTrigger = cp;
		}
		if (InsertIndex >= TriggerCount)
		{
			if (lastTrigger) lastTrigger->Next = cp;
			lastTrigger = cp;
		}
	}
	TriggerCount++;
	TriggerLists[TriggerIndex(cp->Event)] = TriggerListOld;

	ErrorCmd.Cmd = oldCmd;
	RETURN(0);
_cont:
	ErrorCmd.Cmd = oldCmd;
	RETURN(1);
l_exit:
	ErrorCmd.Cmd = oldCmd;
	RETURN(-1);
}

int InitReciever(Mes &M, Word Id, int Num, Dword &ToDoPo, int &ParSet, VarNum *Par)
{
	STARTNA(__LINE__, 0)
	int i;
	int Ind=M.i;
	long long oldCmd = SetErrorTrigger(Id, '!');
	Word id1 = (Id << 8) + (Id >> 8); // reversed
	switch(id1){
		case 'GE': // GE#:
		{
			if(Num!=1) { MError2("wrong syntax."); goto l_exit; }
			if(M.n[0]<0) { MError2("wrong syntax."); goto l_exit; }
			_GlbEvent_ *GEp1 = Main->GEp1;
			_GlbEvent_ *GEp = Main->GEp0;
			for(;GEp<GEp1;GEp++){
				if(MakeSpec(&GEp->Mes.m,M.n[0])) goto _found;
			}
			{ MError2("global event with this index doesn't exist."); goto l_exit; }
_found:
			ToDoPo=(Dword)GEp;
			ParSet=0;
			break;
		}
		case 'LE': // LE$ LE##/##/#:
			if(Num==3){
				ToDoPo=PosMixed(M.n[0],M.n[1],M.n[2]);
				Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; Par[2]=M.VarI[2]; ParSet=3;
			}else if(Num==1){
				ToDoPo=*(Dword *)&M.VarI[0];
				Par[0]=M.VarI[0]; ParSet=1;
			}else{ MError("\"!!LE:\"-wrong syntax (LE$/$/$,LE$)."); goto l_exit; }
			break;
		case 'MO': // MO$ MO##/##/#: or MOx/y/l/1 -dynamic
			if(Num>=3){ // может быть 4
				ToDoPo=PosMixed(M.n[0],M.n[1],M.n[2]);
				Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; Par[2]=M.VarI[2]; ParSet=3;
			}else if(Num==1){
				ToDoPo=*(Dword *)&M.VarI[0];
				Par[0]=M.VarI[0]; ParSet=1;
			}else{ MError("\"!!MO:\"-wrong syntax (MO$/$/$(/$),MO$)."); goto l_exit; }
			break;
		case 'CE': // CE##:
		{
			if(Num!=1) { MError2("wrong syntax."); goto l_exit; }
			if(M.n[0]<0) goto l_exit;
			_CastleEvent_ *CEp = Main->CEp0;
			_CastleEvent_ *CEp1 = Main->CEp1;
			for(; CEp<CEp1; CEp++){
				if(MakeSpec(&CEp->Mes.m,M.n[0])) goto _found1;
			}
			goto l_exit;
_found1:
			ToDoPo=(Dword)CEp;
			ParSet=0;
			break;
		}
		case 'AR': // AR#:
			if(Num==1){
//                MError("\"!!AR$:\"-disabled old style syntax."); goto l_exit;
/*
				if(M.n[0]<0) goto l_exit;
				for(Ap=Ap0;Ap<Ap1;Ap++){
					if(MakeSpec(&Ap->Mes.m,M.n[0])) goto _found3;
				}
				goto l_exit;
_found3:
				ToDoPo=(Dword)Ap;
				ParSet=0;
*/
				 ToDoPo=0;
				 Par[0]=M.VarI[0]; ParSet=1;
		 }else if(Num==3){
				ToDoPo=0;
//                ToDoPo2=PosMixed(M.n[0],M.n[1],M.n[2]);
				Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; Par[2]=M.VarI[2]; ParSet=3;
			}else{ MError2("wrong syntax."); goto l_exit; }
			break;
		case 'HE': // HE#: или HE#1/#2/#3: или HE-1: или HEv:
			switch(Num){
				case 1:
					if(M.n[0]==-1){ // игрок, наступивший на объект
						ToDoPo=0;
						Par[0]=M.VarI[0]; ParSet=1;
					}else{
//                    if(M.VarI[0].Num!=0){
							ToDoPo=1;
							Par[0]=M.VarI[0]; ParSet=1;
//                      ToDoPo2=*(Dword *)&M.VarI[0]/*M.vi[0]*//*GetVar(&M,0)*/;
//                      if(ToDoPo2==0){ MError("\"HE#:\"-hero not found."); goto l_exit; }
//                    }else{
//                      if((M.n[0]<0)||(M.n[0]>=Hn)) goto l_exit;
//                      ToDoPo =(Dword)&Hp0 [M.n[0]];
//                      ToDoPo2=(Dword)&THp0[M.n[0]];
//                    }
					}
					break;
				case 3:
					ToDoPo=0;
//                  ToDoPo2=PosMixed(M.n[0],M.n[1],M.n[2]);
					Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; Par[2]=M.VarI[2]; ParSet=3;
					break;
				default:
					MError2("wrong syntax."); goto l_exit;
			}
			break;
		case 'IF': // IF:
			if(Num!=1) { MError2("wrong syntax."); goto l_exit; }
			ParSet=0;
			break;
		default:
			for(i=0;;i++){
				if(ERM_Addition[i].Id==0) break;
				if(ERM_Addition[i].Id==id1) goto _addfound;
			}
//__asm int 3
//              Message("Unknown Receiver or Instruction");
			{
				LuaPushERMInfo(&M.m.s[M.i-4], ErrString.str == LuaErrorString);
				if (ErrString.str == LuaErrorString)
					LuaLastError(Format("Unknown Reciever or Instruction: %s", lua_tostring(Lua, -1)));
				else if (PL_ERMErrDis == 0 && GameWasLoaded == 0){
					DumpMessage(Format("Unknown Reciever or Instruction:\n\n%s", lua_tostring(Lua, -1)),0);
				}
				lua_pop(Lua, 1);
			}
			goto _next;
_addfound:
			ToDoPo=0;
			switch(ERM_Addition[i].Type){
				case 6: // переменная.переменная, либо без переменных
					if(Num>2){ MError2("wrong syntax (t=6)."); goto l_exit; }
					Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; ParSet=Num;
					break;
				case 5: // переменная.переменная
					if(Num!=2){ MError2("wrong syntax (t=5)."); goto l_exit; }
//                  ToDoPo=*(Dword *)&M.VarI[0];
//                  ToDoPo2=*(Dword *)&M.VarI[1];
					Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; ParSet=2;
					break;
				case 4: // цикл
				 {
//                  Cycle *cp;
					if(Num!=4){ MError2("wrong syntax for receiver (loop)."); goto l_exit; }
//                  if((M.n[1]<-1000)||(M.n[1]>1000)){ MError("\"!!$$:\"-wrong value (cycle from -1000...1000)."); goto l_exit; }
//                  if((M.n[2]<-1000)||(M.n[2]>1000)){ MError("\"!!$$:\"-wrong value (cycle to -1000...1000)."); goto l_exit; }
//                  if((M.n[3]<-500)||(M.n[3]>500)){ MError("\"!!$$:\"-wrong value (cycle step -500...500)."); goto l_exit; }
//                  ToDoPo=M.n[0];
//                  cp=(Cycle *)&ToDoPo2;
//                  cp->From=M.n[1];
//                  cp->To=M.n[2];
//                  cp->Step=M.n[3];
					Par[0]=M.VarI[0]; Par[1]=M.VarI[1];
					Par[2]=M.VarI[2]; Par[3]=M.VarI[3];
					ParSet=4;
					break;
				 }
				case 3: // скопируем число
					if(Num!=1){ MError2("wrong syntax (t=3)."); goto l_exit; }
					ToDoPo=M.n[0];
					Par[0]=M.VarI[0]; ParSet=1;
					break;
				case 2: // скопируем позицию/ номер(тип)/тип и подтип
					switch(Num){
						case 1:
							Par[0]=M.VarI[0]; ParSet=1;
							break;
						case 2:
							Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; ParSet=2;
							break;
						case 3:
							Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; Par[2]=M.VarI[2]; ParSet=3;
							break;
						case 4: //3.58
							Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; Par[2]=M.VarI[2]; Par[3]=M.VarI[3]; ParSet=4;
							break;
						case 5: //3.58
							Par[0]=M.VarI[0]; Par[1]=M.VarI[1]; Par[2]=M.VarI[2]; Par[3]=M.VarI[3]; Par[4]=M.VarI[4]; ParSet=5;
							break;
						default:
							MError2("wrong syntax (t=2)."); goto l_exit;
					}
					break;
				case 1: // скопируем число/переменную   ...$
//                  ToDoPo=(Dword)M.m.s[M.i++];
					if(Num!=1){ MError2("wrong syntax (t=1)."); goto l_exit; }
//                  ToDoPo=*(Dword *)&M.VarI[0];
//                  ToDoPo2=0;
					Par[0]=M.VarI[0]; ParSet=1;
					break;
				case 0:
					if(Num!=1) goto l_exit;
//                  ToDoPo=M.n[0];
//                  ToDoPo2=0;
					Par[0]=M.VarI[0]; ParSet=1;
					break;
				default: MError2("wrong syntax (t=unknown)."); goto l_exit;
			}
			if(ERMVarUsedStore==1){ LogERMAnyReceiver(Id,Par[0],&M.m.s[Ind]); }
	}
	ErrorCmd.Cmd = oldCmd;
	RETURN(0);
_next:
	ErrorCmd.Cmd = oldCmd;
	RETURN(1);
l_exit:
	ErrorCmd.Cmd = oldCmd;
	RETURN(-1);
}

struct _IfStruct_
{
	int Ghost;
	int Total;
	int IsFalse;
};

int CheckConditions(_ToDo_ *dp, _IfStruct_ *a)
{
	DoneError = false;
	Word cmd = dp->Type;
	if(cmd=='fi'){  // if
		a->Total++;
		if (a->Ghost || a->IsFalse)
			a->Ghost++; // ghost if
		else
			a->IsFalse = CheckFlags(dp->Efl);
		return 1;
	}
	if(cmd=='le'){ // else
		if (!a->Ghost)
			if (!a->IsFalse) // previous section was true - no more processing - move the if into ghost mode
			{
				if (!a->Total) { MError("\"el\" - no IF for ELSE"); return 1; }
				a->Ghost++;
				a->IsFalse = 0;
			}else
				a->IsFalse = CheckFlags(dp->Efl);
		return 1;
	}
	if(cmd=='ne'){ // endif
		if (--a->Total < 0) { MError("\"en\" - no IF for ENDIF"); return 1; }
		if (a->Ghost)
			a->Ghost--;
		else
			a->IsFalse = 0;
		return 1;
	}
	if(a->Ghost || a->IsFalse) // this part of IF is not active
		return 1;
	return CheckFlags(dp->Efl);
}

// Doesn't check for ZVSE (use CheckERM for that)
int ParseERM(Mes &M)
{
	STARTNA(__LINE__, 0)
	long    self,ind,k;
	Word    Id;
	Dword   ToDoPo/*,ToDoPo2*/;
	int     Ind,Num;
	Byte    InstrFlag,PostFlag;
	int     ParSet;
	bool    PostInst = false;
	VarNum  Par[16];
	_ToDo_  ltd;
	_ToDo_ *dp;
	_Cmd_  *cp0 = Heap;
	_IfStruct_    TrigIf;
	Word OldScope = GlobalCurrentScope;
	ErrStringInfo LastErrString;
	NewErrStringInfo(&M.m.s[M.i], &LastErrString);

	GlobalCurrentScope = ScriptIndex4ERM++;
	TrigIf.Ghost = TrigIf.IsFalse = TrigIf.Total = 0; // init if-el-en
	TriggerBreak=0;

	do{ // найти генератор действия
		M.c[0]='!';
		// 3.58 correction to skip ^ in comments
		if(SkipUntil2(&M) || M.i>=(M.m.l-4)) // нет генератора или нет места для команды - к след событию
		{
			if (TrigIf.Total)  MError("no ENDIF for IF");
			break;
		}

		ErrString.str = &M.m.s[M.i];		
//    GEr.LastERM(sp->Self.s); // 3.58
		if((M.m.s[M.i]=='?')||(M.m.s[M.i]=='$'))  // событие-генератор ****************************
		{
			if(M.m.s[M.i]=='$') PostFlag=1; else PostFlag=0;
			++M.i;
			Id=*(Word *)&M.m.s[M.i]; M.i+=2;
			M.c[0]=';';
			Ind=M.i;
			Num = GetNumAutoFl(&M);

			switch (InitTrigger(M, Id, Num, PostFlag))
			{
				case  1: goto _cont;
				case -1: goto l_exit;
			}
		}else
		if((M.m.s[M.i]=='!')||(M.m.s[M.i]=='#'))  // событие-приемник или инструкция **************
		{
			if(M.m.s[M.i]=='#') InstrFlag=1;
			else InstrFlag=0;
			++M.i;
			self=M.i;
			Id=*(Word *)&M.m.s[M.i]; M.i+=2;
			ParSet=0;
			M.c[0]=':';
			Ind=M.i;
			Num=GetNumAutoFl(&M);

			switch (InitReciever(M, Id, Num, ToDoPo, ParSet, &Par[0]))
			{
				case  1: goto _next;
				case -1: goto l_exit;
			}
			if(InstrFlag==0){
				if(LastAddedTrigger==0) goto l_exit;
				dp=&LastAddedTrigger->ToDo[LastAddedTrigger->Num];
			}else{
				dp=&ltd;
			}
			dp->Type=Id;
			for(k=0;k<16;k++){
				VNCopy(&M.Efl[0][k][0],&dp->Efl[0][k][0]);
				VNCopy(&M.Efl[0][k][1],&dp->Efl[0][k][1]);
				VNCopy(&M.Efl[1][k][0],&dp->Efl[1][k][0]);
				VNCopy(&M.Efl[1][k][1],&dp->Efl[1][k][1]);
			}
			dp->Disabled=dp->DisabledPrev=0;
			dp->Pointer=ToDoPo;
//          dp->Pointer[1]=ToDoPo2;
			dp->ParSet=ParSet;
			for(k=0;k<ParSet;k++) dp->Par[k]=Par[k];
			for(   ;k<16    ;k++) /**(Dword *)&dp->Par[k]=0;*/ FillMem(&dp->Par[k],sizeof(VarNum),0);
			dp->Com.s=&M.m.s[M.i];
			dp->Self.s=&M.m.s[self];
			dp->Self.l=M.i-self;
			if(ERMVarUsedStore==1) LogERMVarUsed(dp,&M.m.s[Ind]);
			ind=M.i; M.c[0]=';';
			if(SkipUntil(&M)) goto l_exit;
			dp->Com.l=(Word)(M.i-ind);
			if(InstrFlag){
				if((GameWasLoaded==0)||PostInst){ // не выполнять обычные инструкции при загрузке
					if(CheckConditions(dp, &TrigIf)==0) // условное выполнение
						ProcessCmd(dp); // если новая игра, а не загрузка
				}
				if (TriggerBreak)
				{
					M.m.s[M.i] = 0;
					M.m.l = M.i;
					break;
				}
//            InstrFlag=0;
			}else{
				LastAddedTrigger->Num++;
				// проверяем на выход вне памяти
				if(((Dword)&LastAddedTrigger->ToDo[LastAddedTrigger->Num])-((Dword)cp0) >= ERMHeapSize){
					Message("Sorry, your script is too big and there is not enough internal memory (30Mbytes!)",1);
					goto l_exit2;
				}
			}

_next:;
		}else if(M.m.s[M.i]=='@'){ // пост обработка
			++M.i;
			if (GameWasLoaded == 0 || !CheckERM(&M, false))
				goto _contnext;
			PostInst = true;
		}else{
			++M.i;
		}
_cont:
		continue;
	}while(1);
			
_contnext:
	GlobalCurrentScope = OldScope;
	ErrString = LastErrString;
	RETURN(0)

l_exit:
//  Message("{ERM} has wrong syntax. Disabled.",1);

	Message(Format("%s\n\n%s", ITxt(26,0,&Strings), LuaPushERMInfo(&M.m.s[Ind], false)),1);
	lua_pop(Lua, 1);

l_exit2:
	GlobalCurrentScope = OldScope;
	ErrString = LastErrString;
	RETURN(1)
}

bool CheckWogify(int HasInternalScripts, int YesERMFile, int HasLua)
{
	// сделаем ее типа ВоГ
	__asm{
		mov   eax,BASE
		mov   eax,[eax]
		add   eax,0x1F86C
		mov   dword ptr [eax],SOGMAPTYPE
	}

	if(YesERMFile || HasInternalScripts){ // есть внутренние скрипты или файл *.ERM
		if(PL_CustomOptionWasSet!=0){ // 3.58 custom options loaded
		}else if((PL_ApplyWoG==0 && WoGSpecMap==0)||((PL_DisableWogifyWasSet==0) && (Request0(ITxt(YesERMFile ? 198 : 197,0,&Strings)))==0)){
			// если не сказано их ВоГифицировать или сказал не надо вогифицировать
			if(HasLua){ // опции для 3.59 должны быть - ничего не меяем в установках
			}else if(PL_ExpoWasSet!=0){ // опции для 3.58 должны быть
				ResetWoG359();
			}else{  // опции для 3.57 - надо сбросить опции старше 3.57
				if(PL_NPCOptionWasSet==0){ // NPC не были включены или не были выключены
					ResetWoG356(); // если есть скрипты, но старые, то 356 настройки в 0
				}else{ // скрипты есть и новые
					ResetWoG358_357(); // опции 3.57 стиля
				}
			}
			return false;
		}
	}else{ // без скриптов совсем
		if((PL_ApplyWoG==0 && WoGSpecMap==0) || (PL_ApplyWoG==3 && Request0(ITxt(226,0,&Strings))==0)){
			if (MapWoG)
				ResetWoGNoScripts();
			else
				ResetNoWoG();
			return false;
		}
	}

	//WoG = 1;
	return true;
}

void FindERM(void)
{
	STARTNA(__LINE__, 0)
// 3.58
	__asm{
		mov  eax,BASE
		mov  eax,[eax]
		add  eax,0x1FB70
		mov  Main,eax
	}
	//ERMEnabled=0;
	InGame = 1;
	// 3.56 теперь ERM только на WoG картах
	//if(WoG==0) RETURNV
	// очищаем доп память
	ZeroMemory(Heap, ERMHeapSize);
	Heap->Event=0;
	FirstTrigger = LastAddedTrigger = lastTrigger = 0;
	TriggerCount = 0;
	if(GameWasLoaded==0) ResetWogify(); // настройки (копирует установленные)

	PL_NPCOptionWasSet=0; // если будет установлен, то не сбрасывать опции в 3.56
	// усли будут загружены пользовательские опции, то установится в число скриптов
	PL_CustomOptionWasSet=0; // 3.58
	PL_DisableWogifyWasSet=0;
	PL_ExpoWasSet=0; // 3.58

	ZeroMemory(PL_OptionReset, sizeof(PL_OptionReset));
	PL_OptionReset[3] = 1; // PL_NoNPC
	PL_OptionReset[5] = PL_ApplyWoG;
	PL_OptionReset[6] = PL_NPC2Hire;
	ZeroMemory(PL_OptionReset2, sizeof(PL_OptionReset2));
	PL_OptionReset2[1] = PL_CrExpStyle;
	PL_OptionReset2[3] = PL_CheatDis;
	PL_OptionReset2[7] = 1;

	ScriptIndex4ERM = 0;
	LuaCall("EnterMap", GameWasLoaded);
	ClearCrChanged();
	RETURNV
}

void InitERM(void)
{
	STARTNA(__LINE__, 0)
	// проверяем WoG
	__asm{
		mov   eax,BASE
		mov   eax,[eax]
//    add   eax,0x1F86C+IDSHIFT
//    movzx eax,byte ptr [eax]
//    mov   WoG,eax
		add   eax,0x1F86C
		mov   eax,[eax]
		mov   IDummy,eax
	}
	if(IDummy==SOGMAPTYPE) MapWoG=1; else MapWoG=0;
	//WoGatLoad=WoG;
	//if(PL_ApplyWoG>1){
	//	WoG=1; // 3.59 now lua scripts are loaded on any map, so ERM commands must always work
	//}
	// иниацмализируем структуры
	ResetAll();
// инициализация всего что надо сбросить для новой игры
	InitCastles();
	RETURNV
}
/* 3.58
void FindERMn(void)
{
	asm pusha;
	asm mov Esi_,esi
	asm mov GameWasLoaded,0
	STARTC("Find ERM in New Game",0)
	InitERM();
	LoadERMTXT(1);
	FindERM();
// настроим анимацию
	StartAnimation();
	STOP
	asm popa;
}
*/

void FindERMn(void)
{
	__asm pusha;
	__asm mov GameWasLoaded,0
	STARTNA(__LINE__, 0)
	InitERM();
	FindERM();
// настроим анимацию
	StartAnimation();
	STOP
	__asm popa;
}

void FindERMl()
{
	__asm pusha;
//  asm mov Esi_,esi
	__asm mov GameWasLoaded,1
	InitERM();
//  LoadERMTXT(1); // подождем прочтения переменных
//  FindERM();
	__asm popa;
}

int GetGoToId(_ToDo_ *a, int *j)
{
	*j = a->Par[0].Num;
	if(*j < 0 || *j > 49)  { MError2("wrong goto label (0...49)"); return 1; }
	return 0;
}

Dword pointer;

void ProcessERM(bool needLocals)
{
	//if (InGame <= 0 || ERMEnabled==0) return; // в меню или ERM не включен
	struct _Cmd_ *cp;
	Dword  ev;
	int    i, j, n;
	StoredLocalVars vars;
	int    OldScope;
	struct _Cmd_ *OldTriggers[256]; // FU:E goto support
	int    OldTrigIndex = 0;
	int    OldTrigDone = 0;
	bool   WasLuaCall;
	bool   LocalsStored = 0;
	int    GM_ai = ERM_GM_ai, PosX = ERM_PosX, PosY = ERM_PosY, PosL = ERM_PosL;
	int    LastX = ERM_LastX, LastY = ERM_LastY, LastL = ERM_LastL;
	_IfStruct_    TrigIf;
	int    labels[50];
	char   labelIf[50];
	int    needLabel = -1;
	ErrStringInfo LastErrString;
	NewErrStringInfo("ProcessERM", &LastErrString);

	STARTNA(__LINE__, 0)
	ev=pointer; // ук на стр евента
	cp=GetTriggerList(ev);
	TriggerGoTo = 0;
	WasLuaCall = IsLuaCall;
	IsLuaCall = false;
	while(cp!=0 && cp->Event!=0){
		if(cp->Event==ev) // нашли тригер
		{
			if((ev>=31000)&&(ev<31100)) // local function should be in the same scope
				if(cp->Scope!=GlobalCurrentScope) goto _Cont;

			if(--TriggerGoTo >= 0) goto _Cont;
			TriggerGoTo=0;
			TriggerBreak=0;
			OldTriggers[OldTrigIndex] = cp;
			OldTrigIndex = (OldTrigIndex + 1)%256;
			OldTrigDone++;

			if(cp->Disabled==0) // разрешен генератор?
			{
				ErrString.str = "(error in trigger conditions)";

				if(GM_ai>=0) ERMFlags[999]=(char)GM_ai;
				else ERMFlags[999]=(char)(!IsAI(CurrentUser()));
				ERMFlags[998]=(char)IsThis(CurrentUser());
				// позиция события
				ERMVar2[997] = ERM_LastX = ERM_PosX = PosX;
				ERMVar2[998] = ERM_LastY = ERM_PosY = PosY;
				ERMVar2[999] = ERM_LastL = ERM_PosL = PosL;

				if(cp->Efl[0][0][0].Type == 255) // Lua trigger
				{
					ErrString.str = "Lua trigger";
					CallLuaTrigger(cp->Efl[0][0][0].Num);
					goto _Cont;
				}

				if(CheckFlags(cp->Efl)==0) // условное выполнение
				{
					OldScope=GlobalCurrentScope;
					GlobalCurrentScope=cp->Scope;

					if(!LocalsStored && needLocals)
					{
						LocalsStored = true;
						StoreVars(&vars, (cp->Event<30000)||((cp->Event>=31000)&&(cp->Event<31100)));
					}
					// backward compatibility - zero out y-1..y-100, f-1..f-100
					if(cp->Event >= 30000 && (cp->Event < 31000 || cp->Event >= 31100))
					{
						for(i=0;i<100;i++){ ERMVarYT[i]=0; }
						for(i=0;i<100;i++){ ERMVarFT[i]=0; }
					}
					TrigIf.Ghost = TrigIf.IsFalse = TrigIf.Total = 0; // init if-el-en
					for(i=0;i<50;i++) { labels[i]=-1; } // init goto

					// execute
					for(i=0, n=cp->Num; i<n; i++){
						//if(ERMEnabled==0) RETURNV // ERM был выключен
						ErrString.str=cp->ToDo[i].Com.s;
						Word cmd = cp->ToDo[i].Type;

						if(cmd=='al'){ // la - label
							if(GetGoToId(&cp->ToDo[i], &j)) goto _Cont2;

							labels[j] = i;
							labelIf[j] = TrigIf.Total;
							if(j == needLabel)
								needLabel = -1;
							continue;
						}

						if(needLabel >= 0 || CheckConditions(&cp->ToDo[i], &TrigIf)) continue; // условное выполнение

						if(cmd=='og'){ // go - goto
							if(GetGoToId(&cp->ToDo[i], &j)) goto _Cont2;

							if(labels[j] >= 0)
							{
								i = labels[j];
								TrigIf.Total = labelIf[j];
								TrigIf.Ghost = TrigIf.IsFalse = 0;
							}else
								needLabel = j;
							continue;
						}
						ProcessCmd(&cp->ToDo[i]);

						if(TriggerBreak) goto _Cont2;
					}

					if (TrigIf.Total)  MError("no ENDIF for IF");
_Cont2:;
					GlobalCurrentScope=OldScope;
				}
			}

_Cont:;
			if (TriggerGoTo < -OldTrigDone)
				TriggerGoTo = -OldTrigDone;
			if(TriggerGoTo<0)
			{
				OldTrigIndex = (OldTrigIndex + TriggerGoTo + 256)%256;
				cp = OldTriggers[OldTrigIndex];
				continue;
			}
		}

		cp=cp->ListNext;
	}

	if (LocalsStored)
	{
		StoreVars(&vars, (ev<30000)||((ev>=31000)&&(ev<31100)), true);
	}
	IsLuaCall = WasLuaCall;
	ErrString = LastErrString;
	TriggerBreak = TriggerGoTo = 0;
	ERM_LastX = ERM_PosX = LastX;
	ERM_LastY = ERM_PosY = LastY;
	ERM_LastL = ERM_PosL = LastL;
	RETURNV
}

void FUCall(int n, Mes *Mp, int Num, bool needLocals)
{
	STARTNA(__LINE__, 0)
	Mes *lastMp = LastFUMes;
	int lastNum = LastFUNum;
	LastFUMes = Mp;
	LastFUNum = Num;
	if(n>30000){ MError("Fuction Index out of range (1...30000)"); RETURNV }
	if(n<0){ // 3.59
		if(n<-100){ MError("Local Fuction Index out of range (-1...-100)"); RETURNV }
		n=-n+31000-1;
	}
	pointer=n;
	ProcessERM(needLocals);
	LastFUMes = lastMp;
	LastFUNum = lastNum;
	RETURNV
}

// 3.58 post instruction call
void _PostInstrCall(void){
	STARTNA(__LINE__, 0)
	if(GameWasLoaded==0){ // если только новая игра
		pointer=30370;
		ProcessERM();
	}
	RETURNV
}
void PostInstrCall(void){
	__asm pusha
	__asm mov ecx,0x698450
	__asm xor eax,eax
	__asm mov [ecx],eax
	_PostInstrCall();
	__asm popa
}
// Commander
void COCall(Dword n,_Hero_ *hp)
{
	STARTNA(__LINE__, 0)
	pointer=n+30340;
	ERM_GM_ai=IsThis(hp->Owner);
	ERM_HeroStr=hp;
	ERM_PosX=hp->x; ERM_PosY=hp->y; ERM_PosL=hp->l;
	ProcessERM();
	RETURNV
}
// battle
void BACall(Dword n,_Hero_ *hp)
{
	STARTNA(__LINE__, 0)
	pointer=n+30300;
	ERM_GM_ai=!G2B_CompleteAI;
//  ERM_PosX=0;ERM_PosY=0;ERM_PosL=0;
	ERM_HeroStr=hp; // 3.57f 29.01.03
	ProcessERM();
	RETURNV
}
// Timer - 3.59
void TLCall(Dword timesec)
{
	STARTNA(__LINE__, 0)
	if(timesec%60 == 0){ pointer=30904; ProcessERM(); }
	if(timesec%10 == 0){ pointer=30903; ProcessERM(); }
	if(timesec%5 == 0){ pointer=30902; ProcessERM(); }
	if(timesec%2 == 0){ pointer=30901; ProcessERM(); }
	pointer=30900; ProcessERM();
	RETURNV
}

void DlgCallBack(int dlg,int item,int action)
{
	STARTNA(__LINE__, 0)
	pointer=30371; 
	ERM_PosX=dlg; ERM_PosY=item; ERM_PosL=action;
	ProcessERM();
	RETURNV
}
/*
void DlgSpellCallBack(int dlg,int x,int y,int action)
{
	STARTNA(__LINE__, 0)
	pointer=30372; 
	ERM_PosX=x; ERM_PosY=y; ERM_PosL=action;
	ProcessERM();
	RETURNV
}
*/
// multiplayer
void TriggerIP(Dword n)
{
	STARTNA(__LINE__, 0)
	pointer=n+30330;
	ProcessERM();
	RETURNV
}
// battle каждый раунд
void BACall2(Dword /*n*/,int Day)
{
	STARTNA(__LINE__, 0)
//  pointer=n+30300;
	pointer=30302;
//  ERM_PosX=0;ERM_PosY=0;ERM_PosL=0;
	ERMVar2[996]=Day;
	ProcessERM();
	RETURNV
}

void MP3Call(void)
{
	STARTNA(__LINE__, 0)
	pointer=30320;
	ProcessERM();
	RETURNV
}

void SoundCall(void)
{
	STARTNA(__LINE__, 0)
	pointer=30321;
	ProcessERM();
	RETURNV
}

void AdvMagicCastCall(int beforeAFTER)
{
	STARTNA(__LINE__, 0)
	pointer=30322+beforeAFTER;
	ProcessERM();
	RETURNV
}

static int AMM_Skill=-1;
void AdvMagicManagerSkill(void)
{
	__asm{
		mov   eax, 0x59CBF0
		call  eax
	}
	__asm   pusha
	__asm{
				mov   eax,0x6992D0
				mov   eax,[eax]
				mov   eax,[eax+0x38]
	}
	_EAX(AMM_Skill);
	__asm   popa
}

void AdvMagicManager(void)
{
	__asm   pusha
	AMM_Skill=-1;
	AdvMagicCastCall(0);
	__asm   popa
	__asm   mov   eax,0x41C470
	__asm   call  eax
	__asm   pusha
	ERMVar2[996]=AMM_Skill;
	if(ERMVar2[996]==0x7801) ERMVar2[996]=-1;
	AdvMagicCastCall(1);
	__asm   popa
}

void BACall3(int beforeAFTER,int Day)
{
	STARTNA(__LINE__, 0)
//  pointer=n+30300;
	pointer=30303+beforeAFTER;
//  ERM_PosX=0;ERM_PosY=0;ERM_PosL=0;
	ERMVar2[996]=Day;
	ProcessERM();
	RETURNV
}
// 3.58 Mon Resist
void MRCall(int beforeAFTER)
{
	STARTNA(__LINE__, 0)
	pointer=30307+beforeAFTER;
	ERM_GM_ai=!G2B_CompleteAI;
	ProcessERM();
	RETURNV
}
void MFCall(int CallType)
{
	STARTNA(__LINE__, 0)
	pointer=30801+CallType;
	ERM_GM_ai=!G2B_CompleteAI;
	ProcessERM();
	RETURNV
}
// battle field
void BFCall(void)
{
	STARTNA(__LINE__, 0)
	pointer=30800;
	ProcessERM();
	RETURNV
}

int IsDisabled(Dword MixPos,int Owner)
{
	STARTNA(__LINE__, 0)
	_ERM_Object_ *obj;
	if((Owner<0)||(Owner>8)) RETURN(1)
	if((MixPos&0x10000000)==0){ // герой
		RETURN(ERM_Hero[MixPos-30100].Disabled[Owner])
	}else{
		MixPos&=0x0FFFFFFF; // восстановим нормальный вид
		obj=FindObj(MixPos);
		if(obj==0) RETURN(0)
		M_MDisabled=obj->AMessage[Owner][0];
		M_MDisabledNext=obj->AMessage[Owner][1];
		obj->AMessage[Owner][0]=obj->AMessage[Owner][1];
		RETURN(obj->Disabled[Owner])
	}
}

/*
int ERM2Object(int GM_ai,Dword MixPos,_MapItem_ *Mi,_Hero_ *Hr)
{
	ERM_GM_ai=GM_ai;
	ERM_HeroStr=Hr;
	Dword lpointer,dpointer;
	int ret;

	MixedPos(&ERM_PosX,&ERM_PosY,&ERM_PosL,MixPos);
	if(Mi->OType!=0x22){ // объект
		dpointer=MixPos&0x07FF03FF;
		dpointer|=0x10000000;
	}else{ // герой
		dpointer=Mi->SetUp+30100;
	}
//  IsDisabled(dpointer,Hr->Owner);
	if(Mi->OType!=0x22){ // объект
		lpointer=Mi->OType<<12;
		lpointer|=0x40000000;
		pointer=lpointer;
		ProcessERM();
		lpointer=(Mi->OType<<12)+(Mi->OSType+1);
		lpointer|=0x40000000;
		pointer=lpointer;
		ProcessERM();
	}
	pointer=dpointer;
	ProcessERM();
	ret=IsDisabled(dpointer,Hr->Owner);
	return ret;
}
*/
int ERM2Object(int prePOST,int GM_ai,Dword MixPos,_MapItem_ *Mi,_Hero_ *Hr,int o_t,int o_st)
{
	STARTNA(__LINE__, 0)
	ERM_GM_ai=GM_ai;
	ERM_HeroStr=Hr;
	Dword lpointer;
	int ret;

	MixedPos(&ERM_PosX,&ERM_PosY,&ERM_PosL,MixPos);
	MixedPos(&ERM_LastX,&ERM_LastY,&ERM_LastL,MixPos);
// Не знаю какую другую обработку сделать
	if((o_t==0x22)&&(Mi->OType!=0x22)) RETURN(0) // Пост тригер для героя, а он умер
	if(o_t == 0x22 && (int)Mi->SetUp == Hr->Number) // Герой сам с собой - значит надо объект выяснять
	{
		o_t = Hr->PlOType;
		o_st = Mi->OSType;
	}
	if(o_t!=0x22){ // объект
		lpointer = 0x40000000; if(prePOST) lpointer|=0x08000000;
		pointer = lpointer;
		ProcessERM();
		pointer = lpointer + (o_t<<12);
		ProcessERM();
		pointer = lpointer + (o_t<<12) + (o_st+1);
		ProcessERM();
	}
	if(o_t!=0x22){ // объект
		lpointer=MixPos&0x07FF03FF;
		lpointer|=0x10000000; if(prePOST) lpointer|=0x08000000;
	}else{ // герой
		lpointer=Mi->SetUp+30100;
	}
	pointer=lpointer;
	ProcessERM();
//  Dword setup=MIp->SetUp; Word otype=MIp->OType;
//  if(otype==0x22){ // герой "встал" при ретрансляции
//    _Hero_ *hp=GetHeroStr(setup);
//    MIp->SetUp=hp->PlSetUp; MIp->OType=(Word)hp->PlOType;
//  }
	pointer=lpointer;
//asm int 3
	ret=IsDisabled(pointer,Hr->Owner);
	RETURN(ret)
}

void HeroMove(_Hero_ *hp, int NewX, int NewY, int NewL)
{
	STARTNA(__LINE__, 0)
	if(hp==0){MError("\"HeroMoveTrigger:\"- cannot find hero."); RETURNV}
	ERM_GM_ai=-1;
	ERM_HeroStr=hp;
	ERM_PosX=NewX; ERM_PosY=NewY; ERM_PosL=NewL;
	// любой герой
	pointer=30400;
	ProcessERM();
	// конкретный герой
	pointer=30401+hp->Number;
	ProcessERM();
	RETURNV
}

typedef int (__fastcall *_GL_GetSSkill_)(_Hero_ * hp, int minLev, int maxLev, int PrevSkill);
#define GL_GetSSkill ((_GL_GetSSkill_)(void*)0x4DAF70)

static int     GL_PSkill;
static int     GL_SSkill[2];
int            GL_SSkillResult;
static _Hero_ *GL_Hp;
void DoGainLevel(void)
{
	STARTNA(__LINE__, 0)
	if(GL_Hp==0){MError("\"HeroGainLevel:\"- cannot find hero."); RETURNV}
	GL_SSkill[0] = GL_GetSSkill(GL_Hp, 1, 3, -1);
	if (GL_SSkill[0] == -1)
		GL_SSkill[0] = GL_GetSSkill(GL_Hp, 0, 3, -1);
	GL_SSkill[1] = GL_GetSSkill(GL_Hp, 0, 1, GL_SSkill[0]);
	if (GL_SSkill[1] == -1)
		GL_SSkill[1] = GL_GetSSkill(GL_Hp, 0, 3, GL_SSkill[0]);
	GL_SSkillResult = -1;
//  ERM_GM_ai=-1;
	ERM_GM_ai=!IsAI(GL_Hp->Owner);
	ERM_HeroStr=GL_Hp;
//  ERM_PosX=NewX; ERM_PosY=NewY; ERM_PosL=NewL;
	// любой герой
	pointer=30600;
	ProcessERM();
	// конкретный герой
	pointer=30601+GL_Hp->Number;
	ProcessERM();
	if(GL_SSkill[0] == -1 && GL_SSkill[1] != -1)
	{
		int i = GL_SSkill[0];
		GL_SSkill[0] = GL_SSkill[1];
		GL_SSkill[1] = i;
	}
	GL_SSkillResult = -1;
	RETURNV
}

__declspec(naked) void GainLevel(void){__asm
{
	mov GL_PSkill, esi
	mov GL_Hp, ebx
	pusha
	call DoGainLevel
	popa
	mov    eax,GL_PSkill
	mov    dl,[ebx+eax+0x476]
	inc    dl
	mov    [ebx+eax+0x476],dl
	ret
}}

int ERM_HeroGainLevel(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	switch(Cmd){
		case 'S': // Mfirst(0...3)/SSkil1(-1,...)/SSkil2(-1,...)
			CHECK_ParamsMin(3);
			Apply(&GL_PSkill,4,Mp,0);
			Apply(&GL_SSkill[0],4,Mp,1);
			Apply(&GL_SSkill[1],4,Mp,2);
			break;
		case 'R': // SSkillResult
			CHECK_ParamsNum(1);
			Apply(&GL_SSkillResult,4,Mp,0);
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}

int __fastcall GainLevelSSkill(_Hero_ * hp, int minLev, int maxLev, int PrevSkill)
{
	STARTNA(__LINE__, 0)
	if(PrevSkill==-1){ // пред или вообще не надо
		if(GL_SSkill[0]!=-2){ // надо менять первое
			RETURN(GL_SSkill[0])
		}
	}else{
		if(GL_SSkill[1]!=-2){ // надо менять второе
			RETURN(GL_SSkill[1])
		}
	}
	RETURN(GL_GetSSkill(hp, minLev, maxLev, PrevSkill))
}

void __fastcall PostGainLevel(int skill1, int skill2)
{
	if(GL_Hp==0) return;
	STARTNA(__LINE__, 0)
	GL_SSkill[0] = skill1;
	GL_SSkill[1] = skill2;
	if (skill2 == -1)
		GL_SSkillResult = skill1;
	ERM_GM_ai = !IsAI(GL_Hp->Owner);
	ERM_HeroStr = GL_Hp;
	// любой герой
	pointer = 31200;
	ProcessERM();
	// конкретный герой
	pointer = 31201 + GL_Hp->Number;
	ProcessERM();
	RETURNV
}

// 49F830 edi=адрес структуры евента 
// Вызывается при наступлении на евент.
/*
void EventERM(void)
{
	asm pusha;
	asm mov   pointer,edi
	asm mov   eax,-1
	asm mov   ERM_GM_ai,eax
	asm mov   eax,0
	asm mov   ERM_HeroStr,ebx
	ProcessERM();
	asm popa;
}
*/
//00481056 8B08
//00481058 52             push   edx  ?
//00481059 8B55EC
//0048105C 51             push   ecx  MixPos
//0048105D 52             push   edx  ->MIp
//0048105E 56             push   esi  ->Hp
//0048105F 8BCF           mov    ecx,edi ->advManager
//00481061E8DAFA0100     call   H3.004A0B40
//00481066 8B45F4
//static EE_AdvMan;
static _Event_ *EMUM_Ep;
static Dword    EMUM_First;
void __stdcall EventPrepareMithril(Dword par1)
{
	__asm    mov   eax,par1
	__asm    push  edi
	__asm    mov   EMUM_Ep,edi
	__asm    push  eax
	__asm    mov   eax,0x404130
	__asm    call  eax
	__asm    pop   edi
}
int EventMakeUpMithril(void)
{
	__asm    mov   eax,EMUM_Ep
	__asm    push  edx
	__asm    mov   edx,[eax+0x78]
	__asm    mov   EMUM_First,edx
	__asm    movsx edx,word ptr[eax+0x5E]
	__asm    cmp   edx,-1
	__asm    jne  _Ok
	__asm    xor   edx,edx
_Ok:
	__asm    mov   [eax+0x78],edx
	__asm    movsx edx,word ptr[eax+0x5C]
	__asm    mov   [eax+0x5C],edx
	__asm    pop   edx
	__asm    add   eax,0x5C
	__asm    mov   IDummy,eax
	return IDummy;
}
void __stdcall EventERM(_Hero_ *Hp,_MapItem_ *MIp,Dword MixPos,Dword _n1)
{ // ecx= -> advManager
//  Dword LEpo;
	Dword AdvMan;
	_ECX(AdvMan);
	STARTNA(__LINE__, 0)
	__asm{
		pusha
//    mov    eax,MIp
//    mov    esi,AdvMan
//    mov    eax,[eax]
//    and    eax,0x3FF // номер события
//    lea    ecx,[eax+8*eax]
//    lea    eax,[eax+2*ecx]
//    lea    edx,[eax+2*eax]
//    mov    eax,[esi+0x5C]
//    mov    ecx,[eax+0x54]
//    lea    eax,[ecx+4*edx]
	}
//  LEpo=_EAX;
//  pointer=LEpo;
	ERM_GM_ai=-1;
	ERM_HeroStr=Hp;
	MixedPos(&ERM_PosX,&ERM_PosY,&ERM_PosL,MixPos);
	MixedPos(&ERM_LastX,&ERM_LastY,&ERM_LastL,MixPos);
	pointer=MixPos&0x07FF03FF;
	pointer|=0x20000000;
	ProcessERM();
	Dword setup=MIp->SetUp; Word otype=MIp->OType;
	if(otype==0x22){ // герой "встал" при ретрансляции
		_Hero_ *hp=GetHeroStr(setup);
		MIp->SetUp=hp->PlSetUp; MIp->OType=(Word)hp->PlOType;
	}
	EMUM_Ep=0;
	__asm{
		push  _n1
		push   MixPos
		push   MIp
		push   Hp
		mov    ecx,AdvMan
		mov    eax,0x4A0B40
		call   eax
	}
	if(EMUM_Ep!=0){ // была адоптация к Мифрилу - восстановим
		__asm    mov   eax,EMUM_Ep
		__asm    mov   ecx,[eax+0x78]
		__asm    mov   [eax+0x5E],cx
		__asm    mov   ecx,EMUM_First
		__asm    mov   [eax+0x78],ecx
	}
	if(otype==0x22){ // герой "встал" при ретрансляции
		MIp->SetUp=setup; MIp->OType=otype;
	}
	pointer=MixPos&0x07FF03FF;
	pointer|=0x28000000;
	ProcessERM();
	__asm{
		popa
	}
	RETURNV
}
__declspec( naked ) void GlobalEvent(void)
{
	__asm push eax;
	__asm mov  eax,[esp+8];
	__asm mov  pointer,eax;
	__asm mov   eax,-1
	__asm mov   ERM_GM_ai,eax
	__asm mov   eax,0
	__asm mov   ERM_HeroStr,eax
	__asm pop  eax
	__asm pusha;
	ProcessERM();
	pointer=Callers[3].forig;
	__asm popa;
	__asm jmp dword ptr [pointer]
}

void RunTimer(int Owner)
{
	STARTNA(__LINE__, 0)
	int  i,day=GetCurDate();
	int  del;
	Word msk;
	msk=(Word)(1<<Owner);
	for(i=0;i<=LastAutoTimer;i++){
		if(ERMTimer[i].Owners==0) continue;
		if((ERMTimer[i].Owners&msk)==0) continue;
		if(ERMTimer[i].FirstDay>day) continue;
		if(ERMTimer[i].LastDay<day) continue;
		del=day-ERMTimer[i].FirstDay;
		if((del%ERMTimer[i].Period)!=0) continue;
		ERM_GM_ai=!IsAI(Owner);
		pointer = i + (i < 100 ? 30000 : 31000);
		ProcessERM();
	}
	RETURNV
}
/////// Hint //////////
int CheckObjHint(_MapItem_ *Mp,char *Buf,_MapItem_ *MpOrig)
{
	STARTNA(__LINE__, 0)
	int   x,y,l,hv,ind,flag;
	Dword mp;
	_ERM_Object_ *op;

	if(MpOrig->OType==0x22){ // герой
		ind=MpOrig->SetUp;
		hv=ERM_Hero[ind].HintVar;
		if(hv==0) RETURN(0) // не задано
		if((hv<-StrMan::Count)||(hv>32000)){ MError("CheckObjHint - wrong hero hint value"); RETURN(0) }
		if (hv < 0)
			StrCopy(Buf,512,StrMan::GetStoredStr(hv));
		else
			StrCopy(Buf,512,ERM2String(StrMan::GetStoredStr(hv),1,0));
		RETURN(1)
	}
	// объект
	Map2Coord(Mp,&x,&y,&l);
	mp=PosMixed(x,y,l);
	op=FindObj(mp);
// 3.52 +3.56 (если объект просто добавлен, то хинт равен 0)
	if((op==0)||(op->HintVar==0)){
//    mp=((Mp->OType&0xFFF)<<16)+(Mp->OType&0xFFFF)+0x10000000;
		flag=0;
		if(Mp->OType==0x22){ // герой стоит на входе объекта
			int v=*(int *)Mp; _Hero_ *hp=GetHeroStr(v); Mp->OType=(Word)hp->PlOType; flag=1;
		}
		op=FindObjExt(Mp,x,y,l);
		if(flag==1) Mp->OType=0x22;
		if(op==0) RETURN(0) // не нашли
	}
// if(op==0) return 0; // не нашли
	hv=op->HintVar;
	if(hv==0) RETURN(0) // не задано
	if((hv<-StrMan::Count)||(hv>32000)){ MError("CheckObjHint - wrong object hint value"); RETURN(0) }
	if (hv < 0)
		StrCopy(Buf,512,StrMan::GetStoredStr(hv));
	else
		StrCopy(Buf,512,ERM2String(StrMan::GetStoredStr(hv),1,0));
	RETURN(1)
}

_MapItem_ *HC_Map, *HC_MapEnter;
Dword __stdcall HintControl0(Dword p3,Dword p2,Dword p1)
{
	__asm{
		mov eax, p1
		push eax
		mov eax, p2
		push eax
		mov eax, p3
		mov HC_Map, eax
		push eax
		mov    eax, 0x40AF10
		call   eax
		mov    HC_MapEnter,eax
		mov    DDummy,eax
	}
	return  DDummy;
}

char *HC_Buffer = (char*)0x697428;
struct _HC_MsgParams_{
	int x;
	int y;
	int t1;
	int st1;
	int t2;
	int st2;
	int unk;
	int showtime;
	int t3;
	int st3;
} *HC_MsgParams;
int HC_Customized;

int ERM_HintDisplay(char Cmd,int Num,_ToDo_*,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	switch(Cmd)
	{
		case 'M': // message
			CHECK_ParamsNum(1);
			StrMan::Apply(HC_Buffer, Mp, 0);
			break;
		case 'T': // type
		{
			CHECK_ParamsNum(1);
			int right = (HC_MsgParams ? 1 : 0);
			Apply(&right, 4, Mp, 0);
			break;
		}
		case 'P': // pictures
			CHECK_ParamsMax(6);
			if (HC_MsgParams == 0) break; // ignore
			Apply(&HC_MsgParams->t1, 4, Mp, 0);
			if (Num > 1) Apply(&HC_MsgParams->st1, 4, Mp, 1);
			if (Num > 2) Apply(&HC_MsgParams->t2, 4, Mp, 2);
			if (Num > 3) Apply(&HC_MsgParams->st2, 4, Mp, 3);
			if (Num > 4) Apply(&HC_MsgParams->t3, 4, Mp, 4);
			if (Num > 5) Apply(&HC_MsgParams->st3, 4, Mp, 5);
			break;
		case 'C': // customized
			CHECK_ParamsNum(1);
			Apply(&HC_Customized, 4, Mp, 0);
			break;
		default:
			EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

void __fastcall HintTrigger(_HC_MsgParams_ *MsgParams)
{
	if (HC_Map == 0) return; // Uncharted Territory
	STARTNA(__LINE__, 0)

	HC_Customized = CheckObjHint(HC_Map, HC_Buffer, HC_MapEnter);
	if(HC_Customized == 0){
		if((HC_Map->OType==63)&&(HC_Map->OSType==0)){ // пирамида
			StrCopy(HC_Buffer,512,ITxt(194,0,&Strings));
		}
		if(HC_Map->OType==63){
			if((HC_Map->OSType>=10)&&(HC_Map->OSType<=13)){
				StrCopy(HC_Buffer,512,ITxt(196,0,&Strings));
			}
		}
		if(HC_Map->OType==101){
			if((HC_Map->OSType>=7)&&(HC_Map->OSType<=10)){
				StrCopy(HC_Buffer,512,ITxt(201+HC_Map->OSType-7,0,&Strings));
			}
		}
	}

	pointer = 30372;
	ERM_GM_ai = -1;
	Map2Coord(HC_Map, &ERM_PosX, &ERM_PosY, &ERM_PosL);
	HC_MsgParams = MsgParams;
	HC_Map = 0;
	ProcessERM();
	HC_MsgParams = 0;

	RETURNV;
}

__declspec(naked) void HintControl(){__asm
{
	push ecx
	xor ecx, ecx
	call HintTrigger
	pop ecx
	push 0x5FF400
	ret
}}

__declspec(naked) void HintWindow(){__asm
{
	call PlaceCreature
	cmp eax, 0
	jl _norm
	ret 0x28
_norm:
	lea ecx, [esp + 4]
	call HintTrigger
	mov ecx, HC_Buffer
	mov edx, 4
	push 0x4F6C00
	ret
}}


int AS_CGood[][3]={{12}, {5,1,3},{7,1,3},{9,50,200},{15,1,6},{16,1,6},
									 {17,1,2},{18,1,2},{19,1,2},{20,1,2},{21,100,500},
									 {64,1,4},{65,100,500}};
int AS_CBad[][3]={{48}, {1,0,0},{2,-1,-1},{3,0,0},{4,100,500},{6,1,3},
									 {8,0,0},{10,100,300},{22,0,0},{23,0,0},{24,0,0},
									 {25,0,0},{26,0,0},{27,0,0},{28,0,0},{29,0,0},
									 {30,0,0},{31,0,0},{32,0,0},{33,0,0},{34,0,0},
									 {35,0,0},{36,0,0},{37,0,0},{38,0,0},{39,0,0},
									 {40,0,0},{41,0,0},{42,0,0},{43,0,0},{44,0,0},
									 {45,0,0},{46,0,0},{47,0,0},{48,0,0},{49,0,0},
									 {50,0,0},{51,0,0},{52,0,0},{53,0,0},{54,0,0},
									 {55,0,0},{56,0,0},{57,0,0},{58,0,0},{59,0,0},
									 {60,0,0},{61,0,0},{62,0,0}};

#define SPHMOVEPOINTS 0x500
void ApplySphinx(int GM_ai,_Hero_ *Hr,_MapItem_ * /*Mi*/)
{
	STARTNA(__LINE__, 0)
	int num,val,len,P_n;
	num=Random(0,TXTSphinx.sn-1);
	if(Hr->Movement<SPHMOVEPOINTS){ // есть свободные движения у Героя
		if(GM_ai){
			Message(ITxt(18,0,&Strings),1);
			RETURNV
		}
	}
	Hr->Movement-=SPHMOVEPOINTS; if(Hr->Movement<0) Hr->Movement=0;
	if(GM_ai){
		if(SphinxReq(num)) P_n=1; else P_n=0;
	}else{
		P_n=1; // AI
//    Mi->SetUp&=0xFFFFFFFE; // visited
	}
	if(P_n){
		num=AS_CGood[0][0]; num=Random(1,num);
		if(AS_CGood[num][1]==AS_CGood[num][2]) val=AS_CGood[num][1];
		else val=Random(AS_CGood[num][1],AS_CGood[num][2]);
		num=AS_CGood[num][0];
	}else{
		num=AS_CBad[0][0]; num=Random(1,num);
		if(AS_CBad[num][1]==AS_CBad[num][2]) val=AS_CBad[num][1];
		else val=Random(AS_CBad[num][1],AS_CBad[num][2]);
		num=AS_CBad[num][0];
	}
	len=Random(1,7);
	if(AddCurse(num,val,len,2,Hr->Number)){ Error(); RETURNV }
	if(GM_ai){
		if(P_n){
			Message(ITxt(190,0,&Strings),1);
		}else{
			Message(ITxt(191,0,&Strings),1);
		}
	}
	RedrawMap();
	RETURNV
}

void MagicWonder(_Hero_ *hp)
{
	STARTNA(__LINE__, 0)
	int    i,j,s[2];
	int    olda,a[2],*dp[2],rnd;
	char   (*sk)[4],*en;
	Byte   (*ar[2])[2];

//  fl=DGET(MWEnabled);
//  if(fl==0) return; // MW не включен
	sk=ArtSkill;
	for(i=0;i<19;i++){
		if(hp->IArt[i][0]==0x8D) goto _found; // есть вп
	}
	RETURNV
_found:
	if(i==9){ // слот 1
		s[0]=0; s[1]=2; ar[0]=Ar0; ar[1]=Ar2;
	}else if(i==10){ // слот 2
		s[0]=3; s[1]=4; ar[0]=Ar3; ar[1]=Ar4;
	}else if(i==11){ // слот 3
		s[0]=6; s[1]=7; ar[0]=Ar67; ar[1]=Ar67;
	}else if(i==12){ // слот 4
		s[0]=5; s[1]=8; ar[0]=Ar5; ar[1]=Ar8;
	}else if(i==18){ // слот 5
		s[0]=1; s[1]=12; ar[0]=Ar1; ar[1]=ArC;
	}else RETURNV
	dp[0]=hp->IArt[s[0]]; dp[1]=hp->IArt[s[1]];
	a[0]=*dp[0]; a[1]=*dp[1];
	for(j=0;j<2;j++){
		if(a[j]==-1) continue; // нет
		if(a[j]<=0x80){ // не составной
			olda=a[j];
//      for(i=0,rnd=0,p=(Dword *)Edi_;i<0xF5;i++) rnd+=p[i];
//      rnd+=DGET(MWRandom); DGET(MWRandom)=rnd;
//      rnd+=(rnd>>16);
//      rnd+=(Byte)(rnd>>8);
//      rnd&=0xFFF;
			rnd=Random(0,0xFFF);
			rnd=rnd%ar[j][0][0];
			for(i=1;ar[i][1]!=0;i++){
				if(rnd>=ar[j][i][0]) break;
			}
			a[j]=ar[j][i-1][1];
			__asm{
				mov  eax,BASE
				mov  eax,[eax]
				add  eax,0x4E2B4
				mov  en,eax
			}
			if(en[a[j]]!=0) RETURNV // арт запрещен
			hp->IArt[s[j]][0]=a[j];
			if(olda==0x80) olda=0;
			if(olda<=0x2C){
				for(i=0;i<4;i++){
					hp->PSkill[i]-=sk[olda][i];
				}
			}
			if(a[j]==0x80) a[j]=0;
			if(a[j]<=0x2C){
				for(i=0;i<4;i++){
					hp->PSkill[i]+=sk[a[j]][i];
				}
			}
		}
	}
	RETURNV
}

static _Hero_ *MQL_hp;
static Dword   MQL_ecx;
static char MQL_MesBuf[30000];
int _MakeQuestLog(void)
{
	STARTNA(__LINE__, 0)
	int ind,hero,owner;
	_QuestLog_ *qlp;
	hero=MQL_hp->Number;
	owner=MQL_hp->Owner;
	ind=0; MQL_MesBuf[0]=0;
	if(WoGType){ StrCanc(MQL_MesBuf,30000,MQL_MesBuf,"{Папирус}\n\n"); }
	else{ StrCanc(MQL_MesBuf,30000,MQL_MesBuf,"{Papyrus}\n\n"); }
	while((qlp=GetQuest(hero,owner,&ind))!=0){
		if (qlp->z > 0)
			StrCanc(MQL_MesBuf,30000,MQL_MesBuf,ERM2String(StrMan::GetStoredStr(qlp->z),1,0));
		else
			StrCanc(MQL_MesBuf,30000,MQL_MesBuf,StrMan::GetStoredStr(qlp->z));
		StrCanc(MQL_MesBuf,30000,MQL_MesBuf,"\n-------------------------------------------\n");
	}
	if(ind!=0){ // что-то было
		if(WoGType){ StrCanc(MQL_MesBuf,30000,MQL_MesBuf,"\n{Хотите} {посмотреть} {QuestLog?}"); }
		else{ StrCanc(MQL_MesBuf,30000,MQL_MesBuf,"\n{Do you want} {to} {see} {the} {QuestLog?}"); }
		RETURN(Request0(MQL_MesBuf))
	}
	RETURN(1)
}

void MakeQuestLog(void)
{
	__asm pusha
	_EAX(MQL_hp); 
	_ECX(MQL_ecx);
	if(_MakeQuestLog()){
		__asm{
			mov   ecx,MQL_ecx
			mov   eax,0x52E910
			call  eax
		}
	}
	__asm popa
}

//int   AD_artnum,AD_ecx;
//Dword AD_retval;
//void FindArtHint(void) // called when artifact description is requested
//{
//	STARTNA(__LINE__, 0)
//	int   hv;
//	_ERM_Object_ *op;
//	__asm{
//		mov   eax,AD_artnum
//		shl   eax,5
//		add   eax,AD_ecx
//		add   eax,0x10
//		mov   eax,[eax]
//		mov   AD_retval,eax
//	}
//	op=FindObj((5<<8)+AD_artnum+0x10000000);
//	if(op==0) RETURNV
//	hv=op->HintVar;
//	if(hv==0) RETURNV //  
//	if (hv < 0)
//		StrCopy(AD_Buf, 512, StrMan::GetStoredStr(hv));
//	else
//		StrCopy(AD_Buf, 512, ERM2String(StrMan::GetStoredStr(hv),1,0));
//	AD_retval=(Dword)AD_Buf;
//	RETURNV
//}

//Dword ArtifacsDescription(void)
//{
//	__asm{
//		pusha
//		mov   AD_artnum,eax
//		mov   AD_ecx,ecx
//	}
//	FindArtHint();
//	__asm{
//		popa
//		mov   eax,AD_retval
//		mov   DDummy,eax
//	}
//	return DDummy;
//}

//static int Called=0;
void __stdcall ERMDebug(int,int,int,int,int,int,int,int,int,int)
{
	STARTNA(__LINE__, 0)
 if(Request0(ITxt(193,0,&Strings))){ // отладка
/*
			asm{
				mov  eax,BASE
				mov  eax,[eax]
				int 3
			}
*/
//    ShowDiplom(0);
//    if(Called==0) WoMoPlace(5,5,0,39);
//    else WoMoMove(5,5-1+Called,0,5,5+Called,0);
//    Called++;
//    MakeWoMo(5,5,0,1);
		Message(MapSavedWoG,1);
		GEr.Show(0,0,0,0,"");
		MError("Test ERM Error Message.");
//    ChooseArt(ERM_HeroStr);
	}
	RETURNV
}

int FOH_Monstr;
int FOH_ret;
int FOH_CanBeUpgraded(void)
{
	STARTNA(__LINE__, 0)
	__asm pusha
	__asm mov  FOH_Monstr,ecx
	FOH_ret=-1;
	if((FOH_Monstr>=0)&&(FOH_Monstr<MONNUM)){
		if(MonsterUpgradeTable[FOH_Monstr]==-2) FOH_ret=0;
		if(MonsterUpgradeTable[FOH_Monstr]>=0) FOH_ret=1;
	}
	if(FOH_ret==-1){
		__asm{
			mov    ecx,FOH_Monstr
			mov    eax,0x47AA50
			call   eax
			mov    FOH_ret,eax
		}
	}
	__asm popa
//  __asm mov   eax,FOH_ret
	RETURN(FOH_ret)
}

int FOH_UpgradeTo(void)
{
	STARTNA(__LINE__, 0)
	__asm pusha
	__asm mov  FOH_Monstr,ecx
	FOH_ret=-1;
	if((FOH_Monstr>=0)&&(FOH_Monstr<MONNUM)){
		if(MonsterUpgradeTable[FOH_Monstr]==-2) FOH_ret=0;
		else FOH_ret=MonsterUpgradeTable[FOH_Monstr];
	}
	if(FOH_ret==-1){
		__asm{
			mov    ecx,FOH_Monstr
			mov    eax,0x47AAD0
			call   eax
			mov    FOH_ret,eax
		}
	}
	__asm popa
//  __asm mov   eax,FOH_ret
	RETURN(FOH_ret)
}
/////////////////////////////
//00408951 8B4E44         mov    ecx,[esi+44]
//00408954 57             push   edi
//00408955E816A5FFFF     call   H3WOG.00402E70
//0040895A 84C0           test   al,al
//0040895C 0F854F010000   jne    H3WOG.00408AB1
static _MouseStr_ *MC_msp,MC_MouseStr;
static Dword MC_MixPos;
static int MC_x,MC_y,MC_l,MC_Std,MC_changed;
void _MouseClick(int type)
{
	STARTNA(__LINE__, 0)
	Copy((Byte *)MC_msp,(Byte *)&MC_MouseStr,sizeof(_MouseStr_));
	MC_changed=0;
	if(MapSmPos(&MC_MouseStr,&MC_MixPos)==1){
		if(MC_MouseStr.Item==0){ MC_MouseStr.Item=37; MC_changed=1; }
	}else{
		if(MC_MouseStr.Item==37){ MC_MouseStr.Item=0; MC_changed=1; }
	}
	MixedPos(&MC_x,&MC_y,&MC_l,MC_MixPos);
	MC_Std=1;
	if(type){ // левый клик
		pointer=30319;
	}else{ // правый клик
		pointer=30310;
	}
	ProcessERM();
	if(MC_changed==1){
		if(MC_MouseStr.Item==0) MC_MouseStr.Item=37;
		else{ if(MC_MouseStr.Item==37) MC_MouseStr.Item=0; }
	}
	Copy((Byte *)&MC_MouseStr,(Byte *)MC_msp,sizeof(_MouseStr_));
/*
AnimDlg *dlg=new AnimDlg();
__asm int 3
dlg->BuildDlg();
for(int i=0;i<1000;i++){
	int j=i;
}
delete dlg;
*/
	RETURNV
}

char __stdcall MouseClick(_MouseStr_ *msp)
{
	MC_msp=msp;
	__asm pusha
	_MouseClick(0);
	if(MC_Std==0){ // не надо стандартной реакции
		__asm{ 
			popa
			mov al,1 
		}
	}else{ // надо стандартную
		__asm{
			popa
			mov    ecx,[esi+0x44]
			push   edi
			mov    eax,0x402E70
			call   eax
		}
	}
	__asm mov CDummy,al
	return CDummy;
}
int __stdcall MouseClick2(int p4,int p3,int p2,int p1)
{
	_EDI(MC_msp);
	__asm pusha
	_MouseClick(1);
	if(MC_Std==0){ // не надо стандартной реакции
		__asm{ 
			popa; 
			mov eax,1 
		}
	}else{ // надо стандартную
		__asm{
			popa
			push   p1
			push   p2
			push   p3
			push   p4
			mov    eax,0x4099D0
			call   eax
		}
	}
	__asm mov  IDummy,eax
	return IDummy;
}
int __stdcall MouseClick3(int p3,int p2,int p1)
{
	_EDI(MC_msp);
	__asm pusha
	_MouseClick(1);
	if(MC_Std==0){ // не надо стандартной реакции
		__asm{ 
			popa; 
			mov eax,1 
		}
	}else{ // надо стандартную
		__asm{
			popa
			push   p1
			push   p2
			push   p3
			mov    eax,0x409740
			call   eax
		}
	}
	__asm mov  IDummy,eax
	return IDummy;
}
static char TSBuffer[512];
static int MC_lhp,MC_rhp;
int ERM_MouseClick(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int v;
	switch(Cmd){
		case 'T': // T$ - тип
			v=MC_MouseStr.Type; if(Apply(&v,4,Mp,0)) break; MC_MouseStr.Type=v; break;
		case 'S': // S$ -
			v=MC_MouseStr.SType; if(Apply(&v,4,Mp,0)) break; MC_MouseStr.SType=v; break;
		case 'I': // I$ -
			v=MC_MouseStr.Item; if(Apply(&v,4,Mp,0)) break; MC_MouseStr.Item=v; break;
		case 'F': // F$ -
			v=MC_MouseStr.Flags; if(Apply(&v,4,Mp,0)) break; MC_MouseStr.Flags=v; break;
		case 'A': // A$/$ -
			CHECK_ParamsMin(2);
			v=MC_MouseStr.Xabs; if(Apply(&v,4,Mp,0)==0) MC_MouseStr.Xabs=v;
			v=MC_MouseStr.Yabs; if(Apply(&v,4,Mp,1)==0) MC_MouseStr.Yabs=v;
			break;
		case 'P': // P$/$/$ -
			CHECK_ParamsMin(3);
			v=MC_x; if(Apply(&v,4,Mp,0)==0){ MError("\"!!CM:P\"-cannot set position."); RETURN(0) }
			v=MC_y; if(Apply(&v,4,Mp,1)==0){ MError("\"!!CM:P\"-cannot set position."); RETURN(0) }
			v=MC_l; if(Apply(&v,4,Mp,2)==0){ MError("\"!!CM:P\"-cannot set position."); RETURN(0) }
			break;
		case 'R': // R$ - станд поведение (1) или нет (0)
			v=MC_Std; if(Apply(&v,4,Mp,0)) break; MC_Std=v; break;
		case 'H': // A?$/?$ - номера героев
			CHECK_ParamsMin(2);
			v=MC_lhp; if(Apply(&v,4,Mp,0)==0){ MError("\"!!CM:H\"-cannot set hero number."); RETURN(0) }
			v=MC_rhp; if(Apply(&v,4,Mp,1)==0){ MError("\"!!CM:H\"-cannot set hero number."); RETURN(0) }
			break;
		case 'D': // D$ - код позиция на поле битвы куда кликнули
			__asm{
				mov    eax,0x699420
				mov    eax,[eax]
				mov    eax,[eax+0x132D4]
				mov    v,eax
			}
			if(Apply(&v,4,Mp,0)) break;
			__asm{
				mov    eax,0x699420
				mov    eax,[eax]
				mov    ebx,v
				mov    [eax+0x132D4],ebx
			}
			break;
		case 'M': // 3.58 Mz#, M message text
			CHECK_ParamsNum(1);
			if(Mp->VarI[0].Type!=7){ MError2("not a Z variable."); RETURN(0) }
			if(Mp->VarI[0].Check!=0){ MError2("cannot get or check text."); RETURN(0) }
			if (StrMan::Apply(TSBuffer, Mp, 0))
				ShowTSHint(TSBuffer);
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
// клики мышью в городе
int GetRealItem(Byte *TownMan,_MouseStr_ *MouseStr)
{
	STARTNA(__LINE__, 0)
	int retval=MouseStr->Item;
	do{
		if(TownMan==0) break;
		if(retval<0) break;
		if(retval>0x2B) break;
		__asm{
			mov esi, MouseStr
			mov eax, [esi+0x14]
			mov edx, [esi+0x10]
			lea eax, [eax+eax*4]   
			lea ecx, [eax+eax*4] 
			shl ecx, 5 
			add ecx, edx         
			mov edx, [ebx+0x118]
			mov eax, [edx+0x50]
			xor edx, edx 
			mov dx,  [eax+ecx*2]
			dec edx
			mov retval, edx
		}
	}while(0);
	RETURN(retval)
}
static _MouseStr_ *MCT_msp;
static Byte *MCT_TownMan;
static int MCT_Ret;

int _MouseClickTown(void)
{
	STARTNA(__LINE__, 0)
	// 3.58f fix click in allied town
	while(1){
		// if click to garrisoned or visiting hero
		if(MCT_msp->Item!=123) break;
		if(MCT_msp->Item==125) break;
		// left mouse click with any Shift/Alt modifier
		if(MCT_msp->Flags&512) break;
		int hi=-1;
		_Hero_ *hp=0;
		_CastleSetup_ *tp;
		__asm  mov   eax,MCT_TownMan
		__asm  mov   eax,[eax+0x38]
		__asm  mov   tp,eax
		if(tp==0) break;
		if(MCT_msp->Item==123) hi=tp->IHero;
		if(MCT_msp->Item==125) hi=tp->VHero;
		if(hi==-1) break;
		hp=GetHeroStr(hi);
		if(hp==0) break;
		if(hp->Owner==CurrentUser()) break;
		// not this owner - disable action
		RETURN(5)
	}
	Copy((Byte *)MCT_msp,(Byte *)&MC_MouseStr,sizeof(_MouseStr_));
	MC_MouseStr.Item=GetRealItem(MCT_TownMan,&MC_MouseStr);
	MC_Std=1;
	pointer=30311; //
	ProcessERM();
	Copy((Byte *)&MC_MouseStr,(Byte *)MCT_msp,sizeof(_MouseStr_));
	if(MC_Std==0){ // не надо стандартной реакции
		RETURN(5)
	}
	RETURN(MCT_msp->SType-10)
}
int MouseClickTown(void)
{
	__asm pusha
	_ESI(MCT_msp);
	_EBX(MCT_TownMan);
	MCT_Ret=_MouseClickTown();
	__asm popa
	return MCT_Ret;
}

int _MouseOverTown(void)
{
	STARTNA(__LINE__, 0)
	Copy((Byte *)MCT_msp,(Byte *)&MC_MouseStr,sizeof(_MouseStr_));
	MC_Std=1;
	pointer=30318; //
	ProcessERM();
	Copy((Byte *)&MC_MouseStr,(Byte *)MCT_msp,sizeof(_MouseStr_));
	RETURN(MC_Std)
}
void __stdcall MouseOverTown(_MouseStr_ *MStr)
{
	__asm{
		mov    ecx,0x69954C
		mov    ecx,[ecx]
		mov    eax,MStr
		push   eax
		mov    eax,0x5C7BA0
		call   eax
	}
	__asm pusha
	MCT_msp=MStr;
	_MouseOverTown();
	__asm popa
}

void _EnterTownHall(int Type)
{
	STARTNA(__LINE__, 0)
	pointer=30324+Type; //
	ProcessERM();
	RETURNV
}
void EnterTownHall(void)
{
	__asm pusha
	_EnterTownHall(0);
	__asm popa
	__asm mov   eax,0x5D34D0
	__asm call  eax
	__asm pusha
	_EnterTownHall(1);
	__asm popa
}

int MouseClickHero(_MouseStr_ *msp,_Hero_ *hp)
{
	STARTNA(__LINE__, 0)
	Copy((Byte *)msp,(Byte *)&MC_MouseStr,sizeof(_MouseStr_));
	MC_Std=1;
	ERM_HeroStr=hp;
	MC_lhp=hp->Number;
	pointer=30312; //
	ProcessERM();
	Copy((Byte *)&MC_MouseStr,(Byte *)msp,sizeof(_MouseStr_));
	if(MC_Std==0){ // не надо стандартной реакции
		RETURN(1)
	}
	// 3.58 эльфокентавры
	int i,n,tp,ind=MC_MouseStr.Item;
	if((ind>=68)&&(ind<=74)){  // проверяем на центавров
	 while(1){
		//if(WoG==0) break; // не WoG
		if(!PL_CentElf) break; // Horse Elf disabled
		ind-=68;
		if(MC_MouseStr.Flags&0x200) break; // right mouse click
		if((MC_MouseStr.Flags&0x04)==0) break; // Cntrl
		if(hp==0) break;
		if(IsThis(hp->Owner)==0) break;
		tp=0;
		if(hp->Ct[ind]==15){ // Centaur Captain
			tp=19;
		}
		if(hp->Ct[ind]==19){ // Grand Elf
			tp=15;
		}
		if(tp==0) break;
		for(i=n=0;i<7;i++){
			if(hp->Ct[i]!=tp) continue;
			n+=hp->Cn[i]; // считаем число Эльфов
		}
		if(n>hp->Cn[ind]) n=hp->Cn[ind]; // берем минимум
		if(n==0) break;
//    if(Request3Pic(
//    "{Grand} {Elves} and {Centaur} {Captains} are happy to meet each other in your army and want to be together forever...
//    \nDo you agree (Their Experience will be set to 0)?",21,15,21,19,21,192,2)){
		if(Request3Pic(ITxt(234,0,&Strings),21,15,21,19,21,192,2)){
			if(n==hp->Cn[ind]){
				hp->Ct[ind]=192;
				for(i=0;i<7;i++){
					if(n==0) break;
					if(hp->Ct[i]!=tp) continue;
					n-=hp->Cn[i]; // вычитаем число Эльфов
					if(n<0){
						hp->Cn[i]=-n;
						break;
					}
					hp->Ct[i]=-1;
					hp->Cn[i]=0;
				}
			}else{
				hp->Cn[ind]-=n;
				for(i=0;i<7;i++){
					if(hp->Ct[i]!=tp) continue;
					hp->Ct[i]=192;
				}
			}
			RedrawHeroScreen(hp);
			RETURN(1)
		}
		RETURN(0)
	 }
	}
	RETURN(0)
}
// встреча на Эльбе
static _MouseStr_ *MC2H_msp;
static int MC2H_Ret;
static Dword *MC2H_sm;
void _MouseClick2Hero(void)
{
	STARTNA(__LINE__, 0)
	Copy((Byte *)MC2H_msp,(Byte *)&MC_MouseStr,sizeof(_MouseStr_));
	MC_Std=1;
//  ERM_HeroStr=hp;
	MC_lhp=((_Hero_ *)MC2H_sm[0x40/4])->Number;
	MC_rhp=((_Hero_ *)MC2H_sm[0x44/4])->Number;
	pointer=30313; //
	ProcessERM();
	Copy((Byte *)&MC_MouseStr,(Byte *)MC2H_msp,sizeof(_MouseStr_));
	if(MC_Std==0){ // не надо стандартной реакции
		MC2H_Ret=0x0100; // >0xD7
	}else{
		MC2H_Ret=MC2H_msp->Item;
	}
	RETURNV
}

int MouseClick2Hero(void)
{
	_EDI(MC2H_msp);
	_EBX(MC2H_sm);
	__asm  pusha
	_MouseClick2Hero();
	__asm  popa
//__asm  mov    eax,MC2H_Ret
	__asm  xor    edx,edx
	return MC2H_Ret;
}

static _MouseStr_ *MCB_msp;
void _MouseClickBattle(void)
{
	STARTNA(__LINE__, 0)
	Copy((Byte *)MCB_msp,(Byte *)&MC_MouseStr,sizeof(_MouseStr_));
	MC_Std=1;
	pointer=30314; //
	ProcessERM();
	Copy((Byte *)&MC_MouseStr,(Byte *)MCB_msp,sizeof(_MouseStr_));
	if(MC_Std==0){ // не надо стандартной реакции
		MCB_msp->Item=0x07D0; //
	}
	RETURNV
}
void MouseClickBattle(void)
{
	__asm mov    MCB_msp,eax
	__asm pusha
	_MouseClickBattle();
	__asm popa
	__asm mov    ecx,[eax+0x0C]
	__asm test   ch,0x02
}
////////////////////////
// 30317 mouse move over battlefield
void _MouseMoveBattle(void)
{
	STARTNA(__LINE__, 0)
	pointer=30317;
	ProcessERM();
	RETURNV
}
void __stdcall MouseMoveBattle(int p3,int p2,int p1) // ecx - also set
{
	__asm pusha
	_MouseMoveBattle();
	__asm popa
	__asm push   p1
	__asm push   p2
	__asm push   p3
	__asm mov    eax,0x4729D0
	__asm call   eax
}

int ERM_MouseMove(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	char *orig=(char *)0x697428;
	int   v;
	switch(Cmd){
		case 'M': // Mz#, M?z# message text
			CHECK_ParamsNum(1);
			StrMan::Apply(orig, Mp, 0);
			break;
		case 'S': // Sz# add message text
		{
			CHECK_ParamsNum(1);
			char *po = GetErmText(Mp, 0);
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
			break;
		}
		case 'D': // D$ - код позиция на поле битвы куда навели мышь
			CHECK_ParamsNum(1);
			__asm{
				mov    eax,0x699420
				mov    eax,[eax]
				mov    eax,[eax+0x132D4]
				mov    v,eax
			}
			if(Apply(&v,4,Mp,0)) break;
			__asm{
				mov    eax,0x699420
				mov    eax,[eax]
				mov    ebx,v
				mov    [eax+0x132D4],ebx
			}
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
/////////////////////////////
static void GameSaveLoadTR(int st){
	STARTNA(__LINE__, 0)
	pointer=30360+st;
	ProcessERM();
	RETURNV
}
void GameBeforeSave(void){ GameSaveLoadTR(1); }
void GameAfterLoad(void){ GameSaveLoadTR(0); }

/////////////////////////////
void ClearQuestLog(void)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<QWESTLOGNUM;i++){
		QuestLog[i].num=0;
		QuestLog[i].num=0;
		QuestLog[i].hero=0;
		QuestLog[i].owner=0;
		QuestLog[i].z=0;
	}
	RETURNV
}
/////////////////////////////
static _Hero_ *ART_hp;
static int     ART_art;
static int     ART_pos;
void ArtifactOnOff(int ONoff)
{
	STARTNA(__LINE__, 0)
	ERM_HeroStr=ART_hp;
	ERM_PosX=ART_art; ERM_PosY=ART_pos;
	pointer=30315+ONoff; //
	ProcessERM();
	RETURNV
}
void ArtifactOff(void)
{
	__asm  mov    eax,[edi+8*edx+0x12D]
	__asm  mov    ART_art,eax
	__asm  mov    eax,-1
	__asm  mov    ART_pos,edx
	__asm  mov    ART_hp,edi
	__asm  pusha
	ArtifactOnOff(0);
	__asm  popa
	__asm  mov    [edi+8*edx+0x12D],eax
}
void ArtifactOn(void)
{
	__asm  mov    ART_art,eax
	__asm  mov    ART_pos,esi
	__asm  mov    ART_hp,edi
	__asm  pusha
	ArtifactOnOff(1);
	__asm  popa
	__asm  mov    [edi+8*esi+0x12D],eax
}

int Check4NewSpecPic(int hn)
{
	STARTNA(__LINE__, 0)
	if((hn<0)||(hn>=HERNUM)){ MError("Incorrect Hero index in get spec pic."); RETURN(0) }
	// 3.58
	if(HSpecNames[hn].PicNum==0){ // не установлено ERM
		// попробуем определить автоматически
		RETURN(FindHeroSpecIcon(hn))
	}
	RETURN(HSpecNames[hn].PicNum)
}

void SendERMVars(int *len,Byte **buf)
{
	STARTNA(__LINE__, 0)
	*len=1000*sizeof(int);
	*buf=(Byte *)&ERMVar2[9000];
	RETURNV
}
void ReceiveERMVars(int len,Byte *buf)
{
	STARTNA(__LINE__, 0)
	int i;
	if(len!=(1000*sizeof(int))){ MError("Wrong length of sent ERM vars"); RETURNV }
	for(i=0;i<1000;i++) ERMVar2[9000+i]=((int *)buf)[i];
	RETURNV
}

/////////////
struct _CrChanged_{
	int DayOfSet;
} CrChanged[MONNUM];
void DaylyMonChanged(void)
{
	STARTNA(__LINE__, 0)
	int day=GetCurDate();
	for(int i=0;i<MONNUM;i++){
		if(CrChanged[i].DayOfSet==0) continue;
		if((day-CrChanged[i].DayOfSet)>1) CrChanged[i].DayOfSet=0;
	}
	RETURNV
}
void CrIsChanged(int Num)
{
	STARTNA(__LINE__, 0)
	if((Num<0)||(Num>=MONNUM)){ MError("Creature index out of range"); RETURNV }
	CrChanged[Num].DayOfSet=GetCurDate();
	RETURNV
}

void ClearCrChanged(void)
{
	for(int i=0;i<MONNUM;i++) CrChanged[i].DayOfSet=0;
}

struct _CrState_{
	int   Index;
	long  Group;     //  0...8,-1 - neutral
	long  SubGroup;  //  0...6,-1 - not available (423D87, 42A0FC)
	Dword Flags;     //  (424354, 42C6C0)
	long  CostRes[7];//  (42B73A)
	long  Fight;     //
	long  AIvalue;   //
	long  Grow;      //  начальное количество монстров для найма
	long  HGrow;     //
	long  HitPoints; //
	long  Speed;     //
	long  Attack;    //
	long  Defence;   //
	long  DamageL;   //
	long  DamageH;   //
	long  NShots;    //
	long  HasSpell;  //  - сколько раз колдует
	long  AdvMapL;   //
	long  AdvMapH;   //
} CrState[MONNUM];
void SendCreatures(int *len,Byte **buf)
{
	STARTNA(__LINE__, 0)
	int i,j,k;
	for(i=j=0;i<MONNUM;i++){
		if(CrChanged[i].DayOfSet==0) continue;
		CrState[j].Index=i;
		CrState[j].Group=MonTable[i].Group;
		CrState[j].SubGroup=MonTable[i].SubGroup;
		CrState[j].Flags=MonTable[i].Flags;
		for(k=0;k<7;k++) CrState[j].CostRes[k]=MonTable[i].CostRes[k];
		CrState[j].Fight=MonTable[i].Fight;
		CrState[j].AIvalue=MonTable[i].AIvalue;
		CrState[j].Grow=MonTable[i].Grow;
		CrState[j].HGrow=MonTable[i].HGrow;
		CrState[j].HitPoints=MonTable[i].HitPoints;
		CrState[j].Speed=MonTable[i].Speed;
		CrState[j].Attack=MonTable[i].Attack;
		CrState[j].Defence=MonTable[i].Defence;
		CrState[j].DamageL=MonTable[i].DamageL;
		CrState[j].DamageH=MonTable[i].DamageH;
		CrState[j].NShots=MonTable[i].NShots;
		CrState[j].HasSpell=MonTable[i].HasSpell;
		CrState[j].AdvMapL=MonTable[i].AdvMapL;
		CrState[j].AdvMapH=MonTable[i].AdvMapH;
		++j;
	}
	*buf=(Byte *)CrState;
	*len=j*sizeof(_CrState_);
	RETURNV
}
void ReceiveCreatures(int len,Byte *buf)
{
	STARTNA(__LINE__, 0)
	int i,j,k,num;
	num=len/sizeof(_CrState_);
	if(len!=(int)(num*sizeof(_CrState_))){ MError("Wrong length of sent Modified Creatures"); RETURNV };
	if((num<0)||(num>MONNUM)){ MError("Incorrect number of Modified Creature"); RETURNV }
	for(i=0;i<len;i++) ((Byte *)CrState)[i]=buf[i];
	for(j=0;j<num;j++){
		i=CrState[j].Index;
		if((i<0)||(i>MONNUM)){ MError("Incorrect index of Modified Creature"); RETURNV }
		MonTable[i].Group=CrState[j].Group;
		MonTable[i].SubGroup=CrState[j].SubGroup;
		MonTable[i].Flags=CrState[j].Flags;
		for(k=0;k<7;k++) MonTable[i].CostRes[k]=CrState[j].CostRes[k];
		MonTable[i].Fight=CrState[j].Fight;
		MonTable[i].AIvalue=CrState[j].AIvalue;
		MonTable[i].Grow=CrState[j].Grow;
		MonTable[i].HGrow=CrState[j].HGrow;
		MonTable[i].HitPoints=CrState[j].HitPoints;
		MonTable[i].Speed=CrState[j].Speed;
		MonTable[i].Attack=CrState[j].Attack;
		MonTable[i].Defence=CrState[j].Defence;
		MonTable[i].DamageL=CrState[j].DamageL;
		MonTable[i].DamageH=CrState[j].DamageH;
		MonTable[i].NShots=CrState[j].NShots;
		MonTable[i].HasSpell=CrState[j].HasSpell;
		MonTable[i].AdvMapL=CrState[j].AdvMapL;
		MonTable[i].AdvMapH=CrState[j].AdvMapH;
	}
	RETURNV
}

/////////////////////////////
void ResetSSNames(void)
{
	STARTNA(__LINE__, 0)
	int i;
	if(SSNameBackStored==0){
		for(i=0;i<sizeof(SSNameBack);i++) ((Byte *)SSNameBack)[i]=((Byte *)SSNAME)[i];
		SSNameBackStored=1;
	}else{ // иначе восстановим
		for(i=0;i<sizeof(SSNameBack);i++) ((Byte *)SSNAME)[i]=((Byte *)SSNameBack)[i];
	}
	if(SSDescBackStored==0){
		for(i=0;i<sizeof(SSDescBack);i++) ((Byte *)SSDescBack)[i]=((Byte *)SSDESC)[i];
		SSDescBackStored=1;
	}else{ // иначе восстановим
		for(i=0;i<sizeof(SSDescBack);i++) ((Byte *)SSDESC)[i]=((Byte *)SSDescBack)[i];
	}
	if(SSNameDescBackStored==0){
		for(i=0;i<sizeof(SSNameDescBack);i++) ((Byte *)SSNameDescBack)[i]=((Byte *)SSNAMEDESC)[i];
		SSNameDescBackStored=1;
	}else{ // иначе восстановим
		for(i=0;i<sizeof(SSNameDescBack);i++) ((Byte *)SSNAMEDESC)[i]=((Byte *)SSNameDescBack)[i];
	}
	RETURNV
}
void ResetMonNames(void)
{
	STARTNA(__LINE__, 0)
	int i;
	if(MonNameBackStored==0){
		for(i=0;i<sizeof(MonTable2Back);i++) ((Byte *)MonTable2Back)[i]=((Byte *)MonTable2)[i];
		for(i=0;i<sizeof(MonTable3Back);i++) ((Byte *)MonTable3Back)[i]=((Byte *)MonTable3)[i];
		for(i=0;i<sizeof(MonTable4Back);i++) ((Byte *)MonTable4Back)[i]=((Byte *)MonTable4)[i];
		MonNameBackStored=1;
	}else{ // иначе восстановим
		for(i=0;i<sizeof(MonTable2Back);i++) ((Byte *)MonTable2)[i]=((Byte *)MonTable2Back)[i];
		for(i=0;i<sizeof(MonTable3Back);i++) ((Byte *)MonTable3)[i]=((Byte *)MonTable3Back)[i];
		for(i=0;i<sizeof(MonTable4Back);i++) ((Byte *)MonTable4)[i]=((Byte *)MonTable4Back)[i];
	}
	for(i=0;i<MONNUM_OLD;i++){ //Diakon
		MonTable[i].NameS=MonTable2[i].NameS;
		MonTable[i].NameP=MonTable3[i].NameP;
		MonTable[i].Spec=MonTable4[i].Spec;
	}
	RETURNV
}

void ResetHSpec(void)
{
	STARTNA(__LINE__, 0)
	int i;
	if(HSpecBackStored==0){
		for(i=0;i<sizeof(HSpecBack);i++) ((Byte *)HSpecBack)[i]=((Byte *)HSpecTable)[i];
		HSpecBackStored=1;
	}else{ // иначе восстановим
		for(i=0;i<sizeof(HSpecBack);i++) ((Byte *)HSpecTable)[i]=((Byte *)HSpecBack)[i];
	}
	RETURNV
}

int SaveERM(void)
{
	STARTNA(__LINE__, 0)
	int i;
////  _Scope_ *sp;

	for (i = 0; i<TextConstCount; i++)  if (TextConstVars[i])  TextConst[i] = TextConstBackup[i]; // Resore texts

	if(Saver("LERM",4)) RETURN(1)
	LuaCall("SaveGame");
	if(StrMan::Save()) RETURN(1)
	if(Saver(ERMVar,sizeof(ERMVar))) RETURN(1)
	if(Saver(ERMVarMacro,sizeof(ERMVarMacro))) RETURN(1)
	if(Saver(ERMVar2,sizeof(ERMVar2))) RETURN(1)
	if(Saver(ERMVar2Macro,sizeof(ERMVar2Macro))) RETURN(1)
	if(Saver(&ERMW,sizeof(ERMW))) RETURN(1)
	if(Saver(ERMVarH,sizeof(ERMVarH))) RETURN(1)
	if(Saver(ERMVarHMacro,sizeof(ERMVarHMacro))) RETURN(1)
	if(Saver(ERMFlags,sizeof(ERMFlags))) RETURN(1)
	if(Saver(ERMMacroName,sizeof(ERMMacroName))) RETURN(1)
	if(Saver(ERMMacroVal,sizeof(ERMMacroVal))) RETURN(1)
	if(Saver(ERM_Object,sizeof(ERM_Object))) RETURN(1)
	if(Saver(ERMString,sizeof(ERMString))) RETURN(1)
	if(Saver(ERMStringMacro,sizeof(ERMStringMacro))) RETURN(1)
	if(Saver(ERMTimer,sizeof(ERMTimer))) RETURN(1)
	if(Saver(&LastAutoTimer,sizeof(LastAutoTimer))) RETURN(1)
	if(Saver(ERM_Hero,sizeof(ERM_Hero))) RETURN(1)
	if(Saver(&NextWeekOf,sizeof(NextWeekOf))) RETURN(1)
	if(Saver(&NextWeekMess,sizeof(NextWeekMess))) RETURN(1)
	if(Saver(&NextMonthOf,sizeof(NextMonthOf))) RETURN(1)
	if(Saver(&NextMonthOfMonster,sizeof(NextMonthOfMonster))) RETURN(1)
	if(Saver(Square,sizeof(Square))) RETURN(1)
	if(Saver(HTable,sizeof(HTable))) RETURN(1)
	if(Saver(QuestLog,sizeof(QuestLog))) RETURN(1)
	_ArtSetUp_ *ap;
	for(i=0;i<ARTNUM;i++){
		ap=&GetArtBase()[i];
		if(Saver(&ArtNames[i],sizeof(_ArtNames_))) RETURN(1)
		if(Saver(&ap->Cost,sizeof(Dword))) RETURN(1)
		if(Saver(&ap->Position,sizeof(int))) RETURN(1)
		if(Saver(&ap->Type,sizeof(int))) RETURN(1)
		if(Saver(&ap->SuperN,sizeof(int))) RETURN(1)
		if(Saver(&ap->PartOfSuperN,sizeof(int))) RETURN(1)
		if(Saver(&ap->Disable,sizeof(Byte))) RETURN(1)
		if(Saver(&ap->NewSpell,sizeof(Byte))) RETURN(1)
	}
	if(Saver(MonsterUpgradeTable,sizeof(MonsterUpgradeTable))) RETURN(1)
	if(Saver(&PL_ExtDwellStd,sizeof(PL_ExtDwellStd))) RETURN(1)
	if(Saver(&EnableMithrill,sizeof(EnableMithrill))) RETURN(1)
	if(Saver(EnableChest,sizeof(EnableChest))) RETURN(1)
	if(Saver(MithrillVal,sizeof(MithrillVal))) RETURN(1)
	// записываем лишь первую половину опций
	if(Saver(&PL_WoGOptions,sizeof(PL_WoGOptions)/2)) RETURN(1)
	if(Saver(SSAllNames,sizeof(SSAllNames))) RETURN(1)
	if(Saver(MonNames,sizeof(MonNames))) RETURN(1)
	// сохраняем только спецификацию, но не их описание
	for(i=0;i<HERNUM;i++){
		if(Saver((Byte *)&HSpecTable[i],0x1C)) RETURN(1)
	}
	if(Saver(HSpecNames,sizeof(HSpecNames))) RETURN(1)
	if(Saver(Square2,sizeof(Square2))) RETURN(1)
// 3.58
	if(Saver(&AI_Delay,sizeof(AI_Delay))) RETURN(1)
	if(Saver(&AutoSaveFlag,sizeof(AutoSaveFlag))) RETURN(1)
// 3.59
	if(Saver(&TextConstVars,sizeof(TextConstVars))) RETURN(1)

////  v=CalculateScopes();
////  if(Saver(&v,sizeof(v))) return 1;
////  for(i=0;i<v;i++){
////    sp=GetScopeNum(i);
////    if(sp==0){ MError("Cannot find a scope (internal)."); return 1; }
////    if(Saver(&sp->Number,sizeof(sp->Number))) return 1;
////    if(Saver(&sp->Var[0],sizeof(sp->Var))) return 1;
////    if(Saver(&sp->String[0][0],sizeof(sp->String))) return 1;
////  }
	RETURN(0)
}

int LoadERM(int /*ver*/)
{
	STARTNA(__LINE__, 0)
	int i,j,v,ArtNum,MonNum,Size;
////  _Scope_ *sp;
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='L'||buf[1]!='E'||buf[2]!='R'||buf[3]!='M')
			{MError("LoadERM cannot start loading"); RETURN(1)}
	LuaCall("LoadGame");
	if(StrMan::Load()) RETURN(1)
	if(Loader(ERMVar,sizeof(ERMVar))) RETURN(1)
	if(Loader(ERMVarMacro,sizeof(ERMVarMacro))) RETURN(1)
	if(Loader(ERMVar2,sizeof(ERMVar2))) RETURN(1)
	if(Loader(ERMVar2Macro,sizeof(ERMVar2Macro))) RETURN(1)
	if(Loader(&ERMW,sizeof(ERMW))) RETURN(1)
	if(Loader(ERMVarH,sizeof(ERMVarH))) RETURN(1)
	if(Loader(ERMVarHMacro,sizeof(ERMVarHMacro))) RETURN(1)
	if(Loader(ERMFlags,sizeof(ERMFlags))) RETURN(1)
	if(Loader(ERMMacroName,sizeof(ERMMacroName))) RETURN(1)
	if(Loader(ERMMacroVal,sizeof(ERMMacroVal))) RETURN(1)
	if(Loader(ERM_Object,sizeof(ERM_Object))) RETURN(1)
	if(Loader(ERMString,sizeof(ERMString))) RETURN(1)
	if(Loader(ERMStringMacro,sizeof(ERMStringMacro))) RETURN(1)
	if(Loader(ERMTimer,sizeof(ERMTimer))) RETURN(1)
	if(Loader(&LastAutoTimer,sizeof(LastAutoTimer))) RETURN(1)
	if(Loader(ERM_Hero,sizeof(ERM_Hero))) RETURN(1)
	if(Loader(&NextWeekOf,sizeof(NextWeekOf))) RETURN(1)
	if(Loader(&NextWeekMess,sizeof(NextWeekMess))) RETURN(1)
	if(Loader(&NextMonthOf,sizeof(NextMonthOf))) RETURN(1)
	if(Loader(&NextMonthOfMonster,sizeof(NextMonthOfMonster))) RETURN(1)
	if(Loader(Square,sizeof(Square))) RETURN(1)
	_HeroInfo_ *hp;
	for(i=0;i<(HERNUM+8);i++){
		hp=&HTable[i];
		if(Loader(&hp->Enabled,sizeof(long))) RETURN(1)
		for(j=0;j<8;j++)
			if(Loader(&hp->_u1[j],sizeof(long))) RETURN(1)
		if(Loader(&hp->ArmyType[0],sizeof(long))) RETURN(1)
		if(Loader(&hp->ArmyType[1],sizeof(long))) RETURN(1)
		if(Loader(&hp->ArmyType[2],sizeof(long))) RETURN(1)
		if(Loader(&v,sizeof(long))) RETURN(1) //char  *HPSName;
		if(Loader(&v,sizeof(long))) RETURN(1) //char  *HPLName;
		for(j=0;j<8;j++)
			if(Loader(&hp->_u2[j],sizeof(Byte))) RETURN(1)
		if(Loader(&v,sizeof(long))) RETURN(1) //char  *Name;
		if(Loader(&hp->ArmyNum[0][0],sizeof(long))) RETURN(1)
		if(Loader(&hp->ArmyNum[0][1],sizeof(long))) RETURN(1)
		if(Loader(&hp->ArmyNum[1][0],sizeof(long))) RETURN(1)
		if(Loader(&hp->ArmyNum[1][1],sizeof(long))) RETURN(1)
		if(Loader(&hp->ArmyNum[2][0],sizeof(long))) RETURN(1)
		if(Loader(&hp->ArmyNum[2][1],sizeof(long))) RETURN(1)
//  if(Loader(HTable,sizeof(HTable))) return 1;
	}
	RefreshHeroPic();
	if(Loader(QuestLog,sizeof(QuestLog))) RETURN(1)
	_ArtSetUp_ *ap;
	ArtNum=ARTNUM;
	for(i=0;i<ArtNum;i++){
			ap=&GetArtBase()[i];
			if(Loader(&ArtNames[i],sizeof(_ArtNames_))) RETURN(1)
			if(Loader(&ap->Cost,sizeof(Dword))) RETURN(1)
			if(Loader(&ap->Position,sizeof(int))) RETURN(1)
			if(Loader(&ap->Type,sizeof(int))) RETURN(1)
			if(Loader(&ap->SuperN,sizeof(int))) RETURN(1)
			if(Loader(&ap->PartOfSuperN,sizeof(int))) RETURN(1)
			if(Loader(&ap->Disable,sizeof(Byte))) RETURN(1)
			if(Loader(&ap->NewSpell,sizeof(Byte))) RETURN(1)
			
			v=ArtNames[i].NameVar;
			if(v!=0){
				if((v<-StrMan::Count)){ MError("Artifact - wrong z var index for Name."); RETURN(1) }
				ap->Name = StrMan::GetStoredStr(v);
			}else{ ap->Name=ArtSetUpBack[i].Name; }
			
			v=ArtNames[i].DescVar;
			if(v!=0){
				if((v<-StrMan::Count)){ MError("Artifact - wrong z var index for Description."); RETURN(1) }
				ap->Description = StrMan::GetStoredStr(v);
			}else{ ap->Description=ArtSetUpBack[i].Description; }
			
			v=ArtNames[i].PickUp;
			if(v!=0){
				if((v<-StrMan::Count)){ MError("Artifact - wrong z var index for Pick Up Message."); RETURN(1) }
				ArtPickUp[i] = StrMan::GetStoredStr(v);
			}else{ ArtPickUp[i] = ArtPickUpBack[i]; }
		}
	Size=sizeof(MonsterUpgradeTable);
	if(Loader(MonsterUpgradeTable,Size)) RETURN(1)
	if(Loader(&PL_ExtDwellStd,sizeof(PL_ExtDwellStd))) RETURN(1)
	if(Loader(&EnableMithrill,sizeof(EnableMithrill))) RETURN(1)
	if(Loader(EnableChest,sizeof(EnableChest))) RETURN(1)
	if(Loader(MithrillVal,sizeof(MithrillVal))) RETURN(1)
	// загружаем лишь половину опций
	if(Loader(&PL_WoGOptions,sizeof(PL_WoGOptions)/2)) RETURN(1)
	/* второй набор не надо загружать !!!!! */
	// загрузили - нельзя редактировать
	//  EnableWoGDlgEdit=0;

	ResetSSNames();
	if(Loader(SSAllNames,sizeof(SSAllNames))) RETURN(1)
	for(i=0;i<SSNUM;i++){
			for(j=0;j<4;j++){
				v=SSAllNames[i].Var[j];
				if(v!=0){
					if((v<-StrMan::Count)){ MError("Secondary Skill - wrong z var index for Name."); RETURN(1) }
					char *str = StrMan::GetStoredStr(v);
					if(j==0){
						SSNAME[i].Name = str;
						SSNAMEDESC[i].Name[j] = str;
					}else{
						SSDESC[i].Name[j-1] = str;
						SSNAMEDESC[i].Name[j] = str;
					}
				}
			}
		}
	ResetMonNames();
//    if(ver<15) Size=(150+9+1+4+4+5+1+9*2)*sizeof(_MonNames_);
	Size=sizeof(MonNames);
	if(Loader(MonNames,Size)) RETURN(1)
//    if(ver<15) MonNum=(150+9+1+4+4+5+1+9*2);
	MonNum=MONNUM;
	for(i=0;i<MonNum;i++){
			for(j=0;j<5;j++){
				v=MonNames[i].Var[j];
				if(v!=0){
					if((v<-StrMan::Count)){ MError("Monster - wrong z var index for Name."); RETURN(1) }
					char *str = StrMan::GetStoredStr(v);
					switch(j){
						case 0: // single
							MonTable [i].NameS = str;
							MonTable2[i].NameS = str;
							break;
						case 1: // plural
							MonTable [i].NameP = str;
							MonTable3[i].NameP = str;
							break;
						case 2: // spec
							MonTable [i].Spec = str;
							MonTable4[i].Spec = str;
							break;
						case 3: // def
							MonTable [i].DefName = str;
							break;
						case 4: // sound prefix
							MonTable [i].SoundPrefix = str;
							break;
					}
				}
			}
		}
	ResetHSpec();
	// получаем только спецификацию, но не их описание
	for(i=0;i<HERNUM;i++){
		if(Loader((Byte *)&HSpecTable[i],0x1C)) RETURN(1)
	}
	if(Loader(HSpecNames,sizeof(HSpecNames))) RETURN(1)
	for(i=0;i<HERNUM;i++){
			for(j=0;j<3;j++){
				v=HSpecNames[i].Var[j];
				if(v!=0){
					if((v<-StrMan::Count)){ MError("HeroSpec - wrong z var index."); RETURN(1) }
					char *str = StrMan::GetStoredStr(v);
					switch(j){
						case 0: // short
							HSpecTable[i].SpShort = str;
							break;
						case 1: // full
							HSpecTable[i].SpFull = str;
							break;
						case 2: // descr
							HSpecTable[i].SpDescr = str;
							break;
					}
				}
			}
		}
	if(Loader(Square2,sizeof(Square2))) RETURN(1)
// 3.58
	if(Loader(&AI_Delay,sizeof(AI_Delay))) RETURN(1)
	SetDelay(AI_Delay);
	if(Loader(&AutoSaveFlag,sizeof(AutoSaveFlag))) RETURN(1)
	SetAutoSave(AutoSaveFlag);
// 3.59
	if(Loader(&TextConstVars,sizeof(TextConstVars))) RETURN(1)
	for (i = 0; i < TextConstCount; i++)
	{
		v = TextConstVars[i];
		if((v<-StrMan::Count)){ MError("TextConst - wrong z var index."); RETURN(1) }
		if (v!=0) TextConst[i] = StrMan::GetStoredStr(v, TextConst[i]);
	}

////  FreeAllScopes();
////  if(ver>9){
////    if(Loader(&v,sizeof(v))) return 1;
////    for(i=0;i<v;i++){
////      sp=(_Scope_ *)Alloc(sizeof(_Scope_));
////      if(sp==0){ MError("Cannot add a scope (no memory)."); return 1; }
////      if(Loader(&sp->Number,sizeof(sp->Number))) return 1;
////      if(Loader(&sp->Var[0],sizeof(sp->Var))) return 1;
////      if(Loader(&sp->String[0][0],sizeof(sp->String))) return 1;
////      sp->Next=FirstERMScope;
////      FirstERMScope=sp;
////    }
////  }
	RETURN(0)
}

void OnExitToMainMenu()
{
	//LuaCall("LoadContext", 1);  // can't do this - the ending save of campaign won't work well
	//ExitMapLua();
}

int MainMenuERMResetDone = 1;

void ResetERMMainMenu()
{
	if (MainMenuERMResetDone) return;
	ResetAll(0);
	MainMenuERMResetDone = 1;
}

void ResetERM(int game)
{
	STARTNA(__LINE__, 0)
	int i,j,k;
	// !!! Exit/Enter Lua context?
	MainMenuERMResetDone = 0;
	YVarInsideFunction=1; // disable wrong y var usage message
	StrMan::Reset();
	FillMem(ERMMacroName[0], sizeof(ERMMacroName), 0);
	FillMem((char*)ERMMacroVal, sizeof(ERMMacroVal), 0);
	for(i=0;i<20;i++)  { ERMVar[i]=0;   for(j=0;j<8;j++) ERMVarMacro[i][j]=0; }
	for(i=0;i<500;i++) { ERMVar2[i]=0;  for(j=0;j<8;j++) ERMVar2Macro[i][j]=0; }
	for(i=1000;i<VAR_COUNT_V;i++) { ERMVar2[i]=0;  for(j=0;j<16;j++) ERMVar2Macro[i][j]=0; } // !!! these things are bad
	for(i=0;i<500;i++) { ERMFlags[i]=0; }
	for(i=0;i<300;i++) { for(j=0;j<512;j++) ERMString[i][j]=0; for(j=0;j<8;j++) ERMStringMacro[i][j]=0; }
	for(i=500;i<1000;i++) { for(j=0;j<512;j++) ERMString[i][j]=0; for(j=0;j<8;j++) ERMStringMacro[i][j]=0; }
	for(i=0;i<10;i++)  { for(j=0;j<512;j++) ERMLString[i][j]=0; }
	for(j=0;j<50;j++){
		for(i=0;i<HERNUM;i++)  ERMVarH[i][j]=0;
		for(i=0;i<8;i++)  ERMVarHMacro[j][i]=0;
	}
	for(i=0;i<ERMOBJECTS;i++){
		ERM_Object[i].Used=0;
		ERM_Object[i].HintVar=0;
		for(j=0;j<8;j++){
			ERM_Object[i].Disabled[j]=0;
			ERM_Object[i].AMessage[j][0]=0;
			ERM_Object[i].AMessage[j][1]=0;
		}
		ERM_Object[i].PosMix=0;
	}
	for(i=0;i<HERNUM;i++){
		ERM_Hero[i].HintVar=0;
		for(j=0;j<8;j++){
			ERM_Hero[i].Disabled[j]=0;
		}
	}
	for(i=0;i<16;i++) { ERMVarX[i]=0; }
	for(i=0;i<100;i++){ ERMVarY[i]=0; }
	for(i=0;i<100;i++){ ERMVarYT[i]=0; }
	for(i=0;i<100;i++){ ERMVarF[i]=0; }
	for(i=0;i<100;i++){ ERMVarFT[i]=0; }
	for(i=0;i<100;i++){ ERMTimer[i].Owners=0; }
	LastAutoTimer = 99;
	for(i=0;i<144;i++){
		for(j=0;j<144;j++){
			for(k=0;k<2;k++){
				Square[i][j][k].Hero=255;
				Square[i][j][k].Owner=-1;
				Square[i][j][k].Number=0;
				Square[i][j][k].NumberT=0;
				Square[i][j][k].NumberS=0;
			}
		}
	}
	ClearQuestLog();
	NextWeekOf=0;
	NextWeekMess=0;
	NextMonthOf=0;
	NextMonthOfMonster=0;
	// если первый раз, то запомним структуру
	if(HTableBackStored==0){
		for(i=0;i<sizeof(HTable);i++) ((Byte *)HTableBack)[i]=((Byte *)HTable)[i];
		HTableBackStored=1;
	}else{ // иначе восстановим
		for(i=0;i<sizeof(HTable);i++) ((Byte *)HTable)[i]=((Byte *)HTableBack)[i];
	}
	for(i=0;i<8;i++){ // сбросим всех кеумастеров
		for(j=0;j<8;j++){
			SetKeyMaster(i,j,0);
		}
	}
	// настройка артифактов
	// если первый раз, то запомним структуру
	if(ArtSetUpBackStored==0){
		for(i=0; i<sizeof(ArtSetUpBack); i++) ((Byte *)ArtSetUpBack)[i]=((Byte *)GetArtBase())[i];
		for(i=0; i<ARTNUM; i++) ArtPickUpBack[i] = ArtPickUp[i];
		ArtSetUpBackStored=1;
	}else{ // иначе восстановим
		for(i=0;i<sizeof(ArtSetUpBack);i++) ((Byte *)GetArtBase())[i]=((Byte *)ArtSetUpBack)[i];
		for(i=0; i<ARTNUM; i++) ArtPickUp[i] = ArtPickUpBack[i];
	}
	for(i=0;i<ARTNUM;i++){
		ArtNames[i].NameVar=0; // номер z переменной (0-оригинальный)
		ArtNames[i].DescVar=0;
		ArtNames[i].PickUp=0;
	}
	for(i=0;i<MONNUM;i++) MonsterUpgradeTable[i]=-1;
//  PL_ExtDwellStd=0;
	EnableMithrill=0;
	for(i=0;i<20;i++) EnableChest[i]=0;
	for(i=0;i<8;i++) MithrillVal[i]=0;
// 3.58
	if (game)
		ResetWogify();
	// запустили - нельзя редактировать
//  EnableWoGDlgEdit=0;

	ResetSSNames();
	for(i=0;i<SSNUM;i++){
		for(j=0;j<4;j++){
			SSAllNames[i].Var[j]=0;
		}
	}
	ResetMonNames();
	for(i=0;i<MONNUM_OLD;i++){ // Diakon
		for(j=0;j<5;j++){
			MonNames[i].Var[j]=0;
		}
	}
	ResetHSpec();
	for(i=0;i<HERNUM;i++){
		HSpecNames[i].PicNum=0;
		for(j=0;j<3;j++){
			HSpecNames[i].Var[j]=0;
		}
	}
	char *p=(char *)Square2;
	for(i=0;i<sizeof(Square2);i++) p[i]=0;
	StringSet::Clear();

	if (TextConstBackupDone)
	{
		for (i = 0; i<TextConstCount; i++)
			if (TextConstVars[i])
			{
				TextConst[i] = TextConstBackup[i];
				TextConstVars[i] = 0;
			}
	}
	else
	{
		for (i = 0; i<TextConstCount; i++)
		{
			TextConstBackup[i] = TextConst[i];
		}
		TextConstBackupDone = 1;
		_TextFiles_SetValid();
	}

	AI_Delay=0x1000;
	SetDelay(AI_Delay);
	AutoSaveFlag=1;
	SetAutoSave(AutoSaveFlag);
////  CurrentScope=0;
////  ERMScope=0;
////  FreeAllScopes();
	RETURNV
}

/////////////////// Scope
////_Scope_ *FindScope(Word ScopeNumber)
////{
////  _Scope_ *sp;
////  if(ScopeNumber==0) return 0;
////  sp=FirstERMScope;
////  while(sp!=0){
////    if(sp->Number==ScopeNumber) break;
////    sp=sp->Next;
////  }
////  return sp;
////}
////int AddScope(Word ScopeNumber)
////{
////  int i,j;
////  _Scope_ *sp;
////  if(ScopeNumber==0) return 1;
////  sp=FindScope(ScopeNumber);
////  while(sp!=0) return 0;
////  sp=(_Scope_ *)Alloc(sizeof(_Scope_));
////  if(sp==0){ MError("Cannot add a scope (no memory)."); return 1; }
////  sp->Number=ScopeNumber;
////  sp->Next=FirstERMScope;
////  FirstERMScope=sp;
////  for(i=0;i<1000;i++) sp->Var[i]=0;
////  for(i=0;i<1000;i++){
////    for(j=0;j<512;j++) sp->String[i][j]=0;
////  }
////  return 0;
////}
////void FreeAllScopes(void)
////{
////  _Scope_ *sp,*sp1;
////  sp=FirstERMScope;
////  while(sp!=0){
////    sp1=sp;
////    sp=sp->Next;
////    Free(sp1);
////  }
////  FirstERMScope=0;
////}
////int CalculateScopes(void)
////{
////  int num=0;
////  _Scope_ *sp;
////  sp=FirstERMScope;
////  while(sp!=0){
////    ++num;
////    sp=sp->Next;
////  }
////  return num;
////}
////_Scope_ *GetScopeNum(int SNum)
////{
////  int num=0;
////  _Scope_ *sp;
////  sp=FirstERMScope;
////  while(sp!=0){
////    if(num==SNum) return sp;
////    ++num;
////    sp=sp->Next;
////  }
////  return 0;
////}

static char CE_Ret;
int CheatEnabled(void)
{
	__asm pusha
	__asm mov  eax,0x4CE950
	__asm call eax
	__asm mov  CE_Ret,al
	if(CE_Ret==0){ if(PL_CheatDis) CE_Ret=1; } // 903
	__asm popa
//  __asm mov  al,CE_Ret
	return CE_Ret;
}

static int PCMC_Ecx,PCMC_Ret;
int ParseCheatMenuCommand(void)
{
	__asm pusha
	__asm mov  PCMC_Ecx,ecx
	PCMC_Ret=0;
	if(PL_CheatDis){
		if(PCMC_Ecx==40140){
			__asm{
				mov eax, 0x699650
				mov eax,[eax]
				push 0                            // lParam
				push 0                            // wParam
				push 0x10                         // Msg
				push eax                          // hWnd
				mov  eax,0x63A288
				mov  eax,[eax]
				call eax
//      call ds:PostMessageA                    ; Indirect Call Near Procedure
			}
		}  
	}else{
		__asm{
			mov  ecx,PCMC_Ecx
			mov  eax,0x4F49E0
			call eax
			mov  PCMC_Ret,eax
		}
	}
	__asm popa
//  __asm mov  eax,PCMC_Ret
	return PCMC_Ret;
}
