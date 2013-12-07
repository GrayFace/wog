#include <string.h>
//#include <winbase.h>
#include <stdio.h>
//#include <stdlib.h>
#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "casdem.h"
#include "service.h"
#include "herospec.h"
#include "monsters.h"
#include "anim.h"
#include "womo.h"
#include "sound.h"
#include "erm.h"
#include "ai.h"
#include "npc.h"
#include "artifact.h"
//#include "wogsetup.h"
#include "CrExpo.h"
#include "timer.h"
#include "lod.h"
#include "spell.h"
#include "prot.h"
#include "zzzz.h"
#include "global.h"
#include "MONSTRDLL/dll.h"
#include "TOWNS/towns.h"
#ifdef _DEBUG_LUA
	#include "erm_lua.h"
#endif
#define __FILENUM__ 8

#ifdef DEBUG
	#define DSNUM 100
	#define DP(X)  (long)X,#X
	#define DS(X)  (long)X,#X,sizeof(X)
	#define DS0(X) (long)X,0,0
#else
	#define DP(X)  (long)X
	#define DS(X)  (long)X
	#define DS0(X) (long)X
#endif

#ifdef DEBUG
struct _DebugDP_{
	char   name[16]; // имя структуры
	Dword  dp[4];    // указатели
} DebugDP[DSNUM];
int DebugDPNum;
#endif
// Менеджеры Героев
char *ManName[]={
 "combatManager", // = 0
 "townManager",   // = 1
 0
};
///////////////////////////////////////////////
// переменные для подгрузки новых монстров
//HMODULE  hMonLib; // Diakon
//typedef void (__stdcall *MYPROC)(void* p_wMonstrStruct , void* p_wCrAnim, void* p_wAtack, void* p_wDefend, void* p_wOption, void* p_wSingleName, void* p_wPularName, void* p_wSpec); // Diakon
//MYPROC InitilizeStructFromWoG; // Diakon инициализация структур ВоГа в длл 
//typedef bool (__stdcall *LOADMONPROC)(); //Diakon
//LOADMONPROC LoadNewMonstrs; //Diakon подгрузка новых монстров
//typedef bool (__stdcall *PROCCLEANMEM)(); // Diakon
//PROCCLEANMEM CleanMemmory; //Diakon очистка выделенной памяти
//int JMP = 0xE9; //Diakon
//int PSUB = 0x324; //Diakon
//int MSUB = -804; // Diakon
//int GENMONNUM = 0xC5;
//int GENMON = GENMONNUM -1;
///////////////////////////////////////////////

///////////////////////////////////////////////
// тип игры 0-английская, 1-русская
int WoGType=0;
///////////////////////////////////////////////
//  описание видео файлов
#define VIDNUM_0 141
#define VIDNUM_A 10
_VidInfo_ VidArr[VIDNUM_0+VIDNUM_A];
_VidInfo_ VidArrAdd[VIDNUM_A]={
	{"AZVid000",0x691260,{0,0,0,0},{0,0}},
	{"AZVid001",0x691260,{0,0,0,0},{0,0}},
	{"AZVid002",0x691260,{0,0,0,0},{0,0}},
	{"AZVid003",0x691260,{0,0,0,0},{0,0}},
	{"AZVid004",0x691260,{0,0,0,0},{0,0}},
	{"AZVid005",0x691260,{0,0,0,0},{0,0}},
	{"AZVid006",0x691260,{0,0,0,0},{0,0}},
	{"AZVid007",0x691260,{0,0,0,0},{0,0}},
	{"AZVid008",0x691260,{0,0,0,0},{0,0}},
	{"AZVid009",0x691260,{0,0,0,0},{0,0}}
};
struct {
	int x,y;
} VidData[VIDNUM_A]={
	{0x50,0x40},
	{0x50,0x40},{0x50,0x40},{0x50,0x40},{0x70,0x40},
	{0x50,0x40},{0x50,0x40},{0x50,0x40},
	{0x80,0x40},{0x80,0x40}
};

///////////////////////////////////////////////
// типы монстров для генерации в начале месяца
// 0x63E678 [0x0C]
//Byte NewMonthMonTypes[12];
///////////////////////////////////////////////
// CrBank
// Описание добавить в CRTRAITS.TXT (150d+)
// Добавить строки для анимации в Cranim.txt (+1, т.к. нет башни)
//!!!!! обязательно добавлять TXT в H3BITMAP.LOD
//!!!!! добавить DEF с типом 16 и подтипом 11,12 ...
crbanks1   crbankt1[BANKNUM];
crbanks2   crbankt2[BANKNUM];
_CrBank_   CrBankTable[BANKNUM];
//////////////////////////////////////////////
// Artifact
_ArtSetUp_ ArtTable[ARTNUM];
char       ArtBonus[ARTNUM][4];
char       ArtBonusAdd[ARTNUM-ARTNUM_0][4]={// новые артифакты
	{0,0,0,0},
	{0,0,0,0},
};
char      *ArtPickUp[ARTNUM];
/*
_ArtSetUp_ ArtTableAdd[ARTNUM-ARTNUM_0]={
	{"Name",1234,5,'R',"Description",-1,-1,0,0,0,0}
};
*/
/////////////////////////////////////////////
// Monster готово (кроме редактора)
// Добавить в CPRSMALL.def и TwCrPort.def необходимые рожи монстров (сдвинутые на 1, т.к нет башни)
// Сделать DEF анимации.
_MonAnim_ MonATable [MONNUM];
_MonInfo_ MonTableBack [MONNUM];
_MonInfo_ MonTable  [MONNUM];
_MonInfo_ _MonAdd[MONNUM-MONNUM_0]={          // все не боятся страха
{ 0,6,"aagl","ZM150Z.def",0x00010113}, // 150 Castle - Архангел +2мор +Nресур
{ 1,6,"godr","ZM151Z.def",0x8000009B}, // 151 Rampart - Зол.Дракон +ослепл.
{ 2,6,"gtit","ZM152Z.def",0x00009614}, // 152 Tower - Титан +стр.2раза +молния +возд.щит
{ 3,6,"advl","ZM153Z.def",0x00010110}, // 153 Inferno - Архидьявол -2уд. +страх
{ 4,6,"ghdr","ZM154Z.def",0x80060483}, // 154 Necropolis - Др.Привидение -2мор
{ 5,6,"bkdr","ZM155Z.def",0x8000049B}, // 155 Dungeon - Черн.Дракон +страх
{ 6,6,"bmth","ZM156Z.def",0x00000093}, // 156 Stronghold - Ужасное чудище +2уд. +отв.2раза
{ 7,6,"chyd","ZM157Z.def",0x00090091}, // 157 Fortress - Гидра Хаоса +кислота
{ 8,6,"phoe","ZM158Z.def",0x0000409B}, // 158 Conflux - Феникс +огн.щит +убийца
{-1,2,"wrth","ZM159G.def",0x00024402}, // 159 Ghost
{-1,6,"aagl","ZM160G.def",0x00024400}, // 160 God1
{-1,6,"aagl","ZM161G.def",0x00024400}, // 161 God2
{-1,6,"aagl","ZM162G.def",0x00024400}, // 162 God3
{-1,6,"aagl","ZM163G.def",0x00024400}, // 163 God4
{-1,5,"sglm","ZM164GD.def",0x00024400}, // 164 защитники бога 1
{-1,5,"sglm","ZM165GD.def",0x00024400}, // 165 защитники бога 2
{-1,5,"sglm","ZM166GD.def",0x00024400}, // 166 защитники бога 3
{-1,5,"sglm","ZM167GD.def",0x00024400}, // 167 защитники бога 4
{-1,6,"bkdr","ZM168DG.def",0x800B040B}, // 168 гидродракон
{-1,4,"zelt","ZM169ZL.def",0x00001014}, // 169 красный монах
{-1,4,"hcrs","ZM170SW.def",0x00100014}, // 170 белый шутер
{-1,4,"hcrs","ZM171SR.def",0x00100014}, // 171 красный шутер
{-1,5,"bgor","ZM172N.def",0x00020409},  // 172 кошмар
{-1,2,"aagl","ZM173M.def",0x00000010},  // 173 Santa Gremlin
{ 0,6,"crus","ZM174NPC.def",0x00000006},// 174 NPC1 Left Castle
{ 1,6,"monk","ZM175NPC.def",0x00000006},// 175 NPC2 Left Rampart
{ 2,6,"lich","ZM176NPC.def",0x00000006},// 176 NPC3 Left Tower
{ 3,6,"sgrg","ZM177NPC.def",0x00000006},// 177 NPC4 Left Inferno
{ 4,6,"gnol","ZM178NPC.def",0x00060406},// 178 NPC5 Left Necro
{ 5,6,"pfoe","ZM179NPC.def",0x00000006},// 179 NPC6 Left Dungeon
{ 6,6,"trll","ZM180NPC.def",0x00000006},// 180 NPC7 Left Stronghold
{ 7,6,"amag","ZM181NPC.def",0x00000006},// 181 NPC8 Left Fortress
{ 8,6,"geni","ZM182NPC.def",0x00020406},// 182 NPC9 Left Conflux
{ 0,6,"crus","ZM174NPC.def",0x00000006},// 183 NPC1 Right Castle
{ 1,6,"monk","ZM175NPC.def",0x00000006},// 184 NPC2 Right Rampart
{ 2,6,"lich","ZM176NPC.def",0x00000006},// 185 NPC3 Right Tower
{ 3,6,"sgrg","ZM177NPC.def",0x00000006},// 186 NPC4 Right Inferno
{ 4,6,"gnol","ZM178NPC.def",0x00060406},// 187 NPC5 Right Necro
{ 5,6,"pfoe","ZM179NPC.def",0x00000006},// 188 NPC6 Right Dungeon
{ 6,6,"trll","ZM180NPC.def",0x00000006},// 189 NPC7 Right Stronghold
{ 7,6,"amag","ZM181NPC.def",0x00000006},// 190 NPC8 Right Fortress
{ 8,6,"geni","ZM182NPC.def",0x00020406},// 191 NPC9 Right Conflux
{ 1,2,"ecnt","ZM192Z.def",  0x00009015},// 192 Rampart Centaur-Elf
{-1,5,"monk","ZM193Z.def",  0x00000016},// 193 Sorceress
{-1,3,"monk","ZM194Z.def",  0x00000010},// 194 Werewolf
{-1,5,"bgor","ZM195Z.def",  0x00024019},// 195 Fire Horse
{ 4,6,"ghdr","ZM196Z.def",  0x8006148F},// 196 Dracolish
/*//<<<<<<< .mine
//=======
{-1,1,"pike","cpkman.def",  0x00000000},// 197
{-1,1,"pike","cpkman.def",  0x00000000},// 198
{-1,1,"pike","cpkman.def",  0x00000000},// 199
{-1,1,"pike","cpkman.def",  0x00000000},// 200
{-1,1,"pike","cpkman.def",  0x00000000},// 201
{-1,1,"pike","cpkman.def",  0x00000000},// 202
{-1,1,"pike","cpkman.def",  0x00000000},// 203
{-1,1,"pike","cpkman.def",  0x00000000},// 204
{-1,1,"pike","cpkman.def",  0x00000000},// 205
{-1,1,"pike","cpkman.def",  0x00000000},// 206
{-1,1,"pike","cpkman.def",  0x00000000},// 207
{-1,1,"pike","cpkman.def",  0x00000000},// 208
{-1,1,"pike","cpkman.def",  0x00000000},// 209
{-1,1,"pike","cpkman.def",  0x00000000},// 210
{-1,1,"pike","cpkman.def",  0x00000000},// 211
{-1,1,"pike","cpkman.def",  0x00000000},// 212
{-1,1,"pike","cpkman.def",  0x00000000},// 213
{-1,1,"pike","cpkman.def",  0x00000000},// 214
{-1,1,"pike","cpkman.def",  0x00000000},// 215
{-1,1,"pike","cpkman.def",  0x00000000},// 216
{-1,1,"pike","cpkman.def",  0x00000000},// 217
{-1,1,"pike","cpkman.def",  0x00000000},// 218
{-1,1,"pike","cpkman.def",  0x00000000},// 219
{-1,1,"pike","cpkman.def",  0x00000000},// 220
{-1,1,"pike","cpkman.def",  0x00000000},// 221
{-1,1,"pike","cpkman.def",  0x00000000},// 222
{-1,1,"pike","cpkman.def",  0x00000000},// 223
{-1,1,"pike","cpkman.def",  0x00000000},// 224
{-1,1,"pike","cpkman.def",  0x00000000},// 225
{-1,1,"pike","cpkman.def",  0x00000000},// 226
{-1,1,"pike","cpkman.def",  0x00000000},// 227
{-1,1,"pike","cpkman.def",  0x00000000},// 228
{-1,1,"pike","cpkman.def",  0x00000000},// 229
{-1,1,"pike","cpkman.def",  0x00000000},// 230
{-1,1,"pike","cpkman.def",  0x00000000},// 231
{-1,1,"pike","cpkman.def",  0x00000000},// 232
{-1,1,"pike","cpkman.def",  0x00000000},// 233
{-1,1,"pike","cpkman.def",  0x00000000},// 234
{-1,1,"pike","cpkman.def",  0x00000000},// 235
{-1,1,"pike","cpkman.def",  0x00000000},// 236
{-1,1,"pike","cpkman.def",  0x00000000},// 237
{-1,1,"pike","cpkman.def",  0x00000000},// 238
{-1,1,"pike","cpkman.def",  0x00000000},// 239
{-1,1,"pike","cpkman.def",  0x00000000},// 240
{-1,1,"pike","cpkman.def",  0x00000000},// 241
{-1,1,"pike","cpkman.def",  0x00000000},// 242
{-1,1,"pike","cpkman.def",  0x00000000},// 243
{-1,1,"pike","cpkman.def",  0x00000000},// 244
{-1,1,"pike","cpkman.def",  0x00000000},// 245
{-1,1,"pike","cpkman.def",  0x00000000},// 246
{-1,1,"pike","cpkman.def",  0x00000000},// 247
{-1,1,"pike","cpkman.def",  0x00000000},// 248
{-1,1,"pike","cpkman.def",  0x00000000},// 249
{-1,1,"pike","cpkman.def",  0x00000000},// 250
{-1,1,"pike","cpkman.def",  0x00000000},// 251
{-1,1,"pike","cpkman.def",  0x00000000},// 252
{-1,1,"pike","cpkman.def",  0x00000000},// 253
{-1,1,"pike","cpkman.def",  0x00000000}// 254
//>>>>>>> .r59*/
//  паладин
// настройка шутеров в твблице MoJumper5
};
_MNameS_  MonTable2[MONNUM];
_MNameP_  MonTable3[MONNUM];
_MNameSp_ MonTable4[MONNUM];
//int MonUpdate[MONNUM];
//int MonUpdateAdd[MONNUM-MONNUM0]
//////////////////////////////////////////////
// Heroes
_HeroBios_ HBiosTable[HERNUM+8]/*,HBiosTablePluss[HERNUM-HERNUM_0]={
	{"Born to kill"}
}*/;
_HeroSpec_ HSpecTable[HERNUM]/*,HSpecTablePluss[HERNUM-HERNUM_0]={
	{0,1}
}*/;
_HeroInfo_ HTable[HERNUM+8];
/*,HTablePluss[HERNUM-HERNUM_0]={
	{{0,7,0,6,1,1,1,0,-1},{0,2,4},"HPS000Kn.PCX","HPL000Kn.PCX",{1,1,0,0,0,0,0,0},
	"Ivan",{{1,1},{1,1},{1,1}}}
}*/
_HeroName_ HTable2[HERNUM]/*,HTable2Pluss[HERNUM-HERNUM_0]={
	{"Ivan"}
}*/;
_Hero_ HMTable[HERNUM];
//////////////////////////////////////////////
// Жилища монстров Dwellings готово (кроме редактора где только 2)
// 1. добавлять названия в конец CrGen1.txt
// 2. в файл objects.txt добавляются (тип 17, подтип начиная с 80):
// XXX.def 1... 0... 111111111 011111111 17 80 0 0
// 3. добавить DEF
int   DwMonster[DWNUM];
char *DwName[DWNUM];
///////////////////////////////////////////
// Замки
//void InitCastles(void);
///////////////////////////////////////////
// Magic готово
// 1.создать DEF
// 2.прибавить единичку к MAGICS в structs.h
// 3.добавить описание анимации в MagicAniAdd
_MagicAni_ MagicAni[MAGICS];
//#define MagicGhost 0x53
_MagicAni_ MagicAniAdd[4]={
	{"ZMagic1.def","MGhost",1},   // 0x53
	{"ZMGC02.def", "MGblock",1},  // 0x54 Complete Block Commander's 
	{"ZMGC01.def", "MGblock2",1}, // 0x55 Complete Block Exp 
	{"ZMGC03.def", "MGblock3",1}, // 0x56 Partial Block Exp
};
////////////////////////////////////////
_Spell_ Spells[SPELLNUM];
////////////////////////////////////////
_CArtSetup_ CArtSetup[32]={
	{0x81,{0x80000000,0x0000001F,0x00000000,0x00000000,0x00000000}}, // 0
	{0x82,{0x00000000,0x01C00000,0x00000000,0x00000000,0x00000000}}, // 1
	{0x83,{0x00000000,0x00000000,0xC0000000,0x00000001,0x00000000}}, // 2
	{0x84,{0x04104100,0x00000000,0x00000000,0x00000000,0x00000000}}, // 3
	{0x85,{0x00000000,0x00000000,0x00000000,0x07C00000,0x00000000}}, // 4
	{0x86,{0x00000000,0x00003FE0,0x00000000,0x00000000,0x00000000}}, // 5
	{0x87,{0x41041000,0x00000000,0x00000000,0x00000000,0x00000000}}, // 6
	{0x88,{0x00000000,0x00000000,0x00000080,0x08000000,0x00000000}}, // 7
	{0x89,{0x00000000,0x70000000,0x00000000,0x00000000,0x00000000}}, // 8
	{0x8A,{0x00000000,0x00000000,0x00000E00,0x00000000,0x00000000}}, // 9
	{0x8B,{0x00000000,0x00000000,0x00007000,0x00000000,0x00000000}}, // 10
	{0x8C,{0x00000000,0x00000000,0x00000000,0x0002E000,0x00000000}}, // 11
//  { 158,{0x10410400,0x00000000,0x00000000,0x00000000,0x00000000}}, // 12
//  {0x00,{0x00000000,0x00000000,0x00000000,0x00000000,0x00000000}}
};

////////////////////////////////////////
// New name of Objects.txt for Random map
char RMObjectsFile[]="ZAObjts.txt";
////////////////////////////////////////
//Dword      StartStructure;
 int        MapWoG=0;
 //int        WoGatLoad=0;
 TxtFile    Strings;
////////////////////////////////////////
// трансляция скелетон трансформером
int SkelTrans   [MONNUM];
int SkelTransAdd[MONNUM-MONNUM_0+5]={
 145, //  Catapult
 146, //  Ballista
 147, //  First Aid Tent
 148, //  Ammo Cart
 149, //  Arrow Towers
 56,  //150, //  Supreme Archangel
 68,  //151, //  Diamond Dragon
 56,  //152, //  Lord of Thunder
 56,  //153, //  Antichrist
 68,  //154, //  Blood Dragon
 68,  //155, //  Darkness Dragon
 56,  //156, //  Ghost Behemoth
 68,  //157, //  Hell Hydra
 56,  //158, //  Sacred Phoenix
 159, //  Ghost
 160, //  Emissary of War
 161, //  Emissary of Peace
 162, //  Emissary of Mana
 163, //  Emissary of Lore
 56,  //164, //  Fire Messenger
 56,  //165, //  Earth Messenger
 56,  //166, //  Air Messenger
 56,  //167, //  Water Messenger
 68,  //168, //  Gorynych
 56,  //169, //  War zealot
 56,  //170, //  Arctic Sharpshooter
 56,  //171, //  Lava Sharpshooter
 56,  //172, //  Nightmare
 56,  //173, //  Santa Gremlin
 56,  //174, //  Paladin
 56,  //175, //  Hierophant
 56,  //176, //  Temple Guardian
 56,  //177, //  Succubus
 56,  //178, //  Soul Eater
 56,  //179, //  Brute
 56,  //180, //  Ogre Leader
 56,  //181, //  Shaman
 56,  //182, //  Astral Spirit
 56,  //183, //  Paladin 2
 56,  //184, //  Hierophant 2
 56,  //185, //  Temple Guardian 2
 56,  //186, //  Succubus 2
 56,  //187, //  Soul Eater 2
 56,  //188, //  Brute 2
 56,  //189, //  Ogre Leader 2
 56,  //190, //  Shaman 2
 56,  //191, //  Astral Spirit 2
 56,  //192, //  Rampart Centaur-Elf
 56,  //193, //  Sorceress
 56,  //194, //  Werewolf
 56,  //195  //  Fire Horse
 68,  //196, //  Dracolish
 56,  //197
 56,  //198
 56,  //199
 56,  //200
 56,  //201
 56,  //202
 56,  //203
 56,  //204
 56,  //205
 56,  //206
 56,  //207
 56,  //208
 56,  //209
 56,  //210
 56,  //211
 56,  //212
 56,  //213
 56,  //214
 56,  //215
 56,  //216
 56,  //217
 56,  //218
 56,  //219
 56,  //220
 56,  //221
 56,  //222
 56,  //223
 56,  //224
 56,  //225
 56,  //226
 56,  //227
 56,  //228
 56,  //229
 56,  //230
 56,  //231
 56,  //232
 56,  //233
 56,  //234
 56,  //235
 56,  //236
 56,  //237
 56,  //238
 56,  //239
 56,  //240
 56,  //241
 56,  //242
 56,  //243
 56,  //244
 56,  //245
 56,  //246
 56,  //247
 56,  //248
 56,  //249
 56,  //250
 56,  //251
 56,  //252
 56,  //253
 56  //254
 //56  //255
};

void FindERMn(void);
void FindERMl(void);
void GlobalEvent(void);
//void EventERM(void);
void __stdcall EventERM(_Hero_ *Hp,_MapItem_ *MIp,Dword MixPos,Dword _n1);
// у паскалевой процедуры аргументы в обратном порядке
//void pascal HeroCheck(int Flag,int Radius,int Owner,int Level,int NewY,int NewX);
//int  CheckSOG(void);
void SaveGame(void);
void LoadGame(void);
void FileLoad(void);
//void FileTypeN(void);
//void FileTypeL(void);
int CheckFileTypeN(void);
//void CheckFileTypeL(void);
//void pascal SaveGamePrepare(Dword);
void __stdcall Creature2Hire(Dword);
//void LeaveCreature(void);
//void CalledDayly(void);
//void CalledBeforeTurn1(void);
//void CalledBeforeTurn(void);
void CalledBeforeTurn1New(void);
void CalledBeforeTurnNew(void);
void __stdcall ServiceManager(Dword);
void __stdcall NewStartLoading(Dword,Dword);
//void pascal SpecPicture(Dword po);
//void SpecText(char *buf,char *po);
//void pascal SpecPictureS(Dword d1,Dword d2,Dword d3,Dword d4,
//                         Dword spec,Dword d6,Dword d7,Dword d8,
//                         Dword d9,Dword d10,Dword d11);
//void SpecPictureC(void);
//void pascal SpecPictureC(Dword,char *,int,char *,char *,char *);
void GameChoosen(void);
Dword GameChoosen2(void);
void __stdcall GameChoosen3(int,int);
//void MonMapPicOn(void);
//void MonMapPicOff(void);
int ERM_MonAtr(char,int,_ToDo_ *sp,Mes *Mp);
void __stdcall MonsterKickBack(Dword,int,int,int);
//void InitArmore(void);
void __stdcall Enter2Object(_Hero_ *Hr,_MapItem_ *Mi,int,int GM_ai); //ecx=avd.Manager
void __stdcall Enter2Monster(_MapItem_ *Mi,_Hero_ *Hr,int,int GM_ai); //ecx=avd.Manager
void __stdcall Enter2Monster2(_MapItem_ *Mi,_Hero_ *Hr,int,int GM_ai); //ecx=avd.Manager
int  __stdcall ShowMagicBookMouse(Dword position);
void __stdcall SetUpBeforeBattle(Dword position);
int __stdcall ReduceDefence(Dword,Dword);
void SetUpNewRound(void);
void LoadProtected(void);
void NoRestart(void);
void __stdcall ShowIntro(int,int,int,int,int,int);
void __stdcall TowerShot(Dword MonNum);
void TutorialGame(void);
void GainLevel(void);
void ThroughWall(void);
void Check4Fairy(void);
void FairyPower(void);
void ShowVersion(void);
//void WeeklyDwellingGrow(int);
void DropGame1(void);
void DropGame2(void);
void DropGame3(void);
void DropGame4(void);
int SaveB1(void);
int LoadB1(int ver);
void ResetB1(void);
int __stdcall AIthinkAboutRes(Dword PlStr);
void MP3Start(void);
int __stdcall AIMapGoal(Dword,Dword,Dword,Dword,Dword);
void ArtMove1(void);
void ArtMove2(void);
void __stdcall ShowSmallSymbol(int,int,int,int);
int AddMithrillAuto(int,int,int,int Type,int Num);
Byte * __stdcall BuildAllDiff(int *difflen);
int __stdcall GZWrite(int len);
int __stdcall GZClose(void);
int __stdcall GZOpen(int par);
void CalledAfterTurn(void);
void __stdcall MouseMoveBattle(int,int,int);
Dword SoundManager(void);
int CheckCheat(Byte *,Dword);
void FixQuit98Crash(void);
void BuildUpNewComboArts(void);
// 3.59
void __fastcall LoadGameSetUpPlayers(Dword obj,Dword,Dword par);
void CastMessage(void);
Dword AcidBreathMessage(Dword buf,char *str,Dword p1,Dword p2);
Dword AlwaysLuckyMessage(Dword buf,char *str,Dword p1);

Dword OriginalCallPointer; // вспомогательная переменная
struct __Callers{
	long  where;
	long  forig;
	long  fnew;
#ifdef DEBUG
// DEBUG для отладки
	char *remember;
#endif  
} Callers[]={
// FindERMn = 0
	{0x4FDF8B,0x0,DP(FindERMn)},
// FindERMl = 1
	{0x4FE577,0x0,DP(FindERMl)},
// EventERM = 2
//  {0x49F6F6,0x0,DP(EventERM)},
	{0x481061,0x0,DP(EventERM)},
// GlobalEvent = 3 !!! привязан
	{0x4CD6AA,0x4CD3D0,DP(GlobalEvent)},
// HeroCheck   = 4 !!! привязан
	{0x48098B,0x49CDD0,DP(HeroCheck)},
// Save Game   = 5 !!! привязан
	{0x4BEB3B,0x4BDC40,DP(SaveGame)},
// Load Game   = 6 !!! привязан (см. также далее настройку игроков)
	{0x4BD908,0x4BDC40,DP(LoadGame)},
// 4 Creatures = 7 !!! привязан
	{0x5D432C,0x4B0770,DP(Creature2Hire)},
// Castle Service = 8 !!! привязан
	{0x4B0A74,0x4B0950,DP(ServiceManager)},
// Castle Service = 9 !!! привязан
	{0x5BF43F,0x4305A0,DP(CastleServiceRedraw)},
// New Game start loading = 10 !!! привязан
	{0x4FDCF0,0x4FDB40,DP(NewStartLoading)},
// AI Start Building = 11 !!! привязано
	{0x5BF7E4,0x5C1120,DP(AIBuild)},
// Картинка спец. Героя = 12 !!! привязано
// Есть сюда еще одна ссылка ниже
	{0x4E1F2A,0x5FF3A0,DP(SpecPicture)},
// Текст спец. Героя = 13 !!! привязано
	{0x4E1F4B,0x6179DE,DP(0)},
// Картинка Маленькая спец. Героя = 14 !!! привязано
	{0x51DCE7,0x4EA800,DP(SpecPictureS)},
// Картинка в настройках карты спец. Героя = 15 !!! привязано
	{0x5828C6,0x575E20,DP(SpecPictureC)},
// Переход на настройку спец. карты. Героя = 16 !!! привязано
	{0x587013,0x5813D0,DP(MapSetup)},
// Игра выбрана - переход на загрузку (кнопка Ok). = 17 !!! привязано
// Есть сюда еще одна ссылка ниже
	{0x5869D1,0x58BFB0,DP(GameChoosen)},
// Переход на обработку ответа монстра = 18 !!! привязано
	{0x441991,0x4408E0,DP(MonsterKickBack)},

// Игра выбрана - переход на загрузку (двойной клик). Настройка выше = 17 !!! привязано
	{0x587623,0,DP(GameChoosen)},
// Иконка и имя в списке героев при настройке - переход на загрузку.
	{0x58DF33,0,DP(NextHeroChoosen)},
// Leave Creature 
	//{0x415AEE,0x4F6C00,DP(LeaveCreature)}, // called from HintWindow
// Castle Service
	{0x5D38AD,0x0,DP(CastleService)},
	{0x5D6B90,0x0,DP(CastleService2)},
// FileLoader
	{0x47A6B3,0x0,DP(FileLoad)},
// Game New/Load
////  {0x4C413E,0x0,(long)FileTypeN},
////  {0x4C5323,0x0,(long)FileTypeL},
	{0x5849C7,0x0,DP(CheckFileTypeN)},
//  {0x5849C7,0x0,(long)CheckFileTypeL},
//  {0x4C8101,0x0,DP(CalledDayly)},
//  {0x4F0516,0x0,DP(CalledBeforeTurn1)},
	{0x4F04F6,0x0,DP(CalledBeforeTurn1New)},
//  {0x4C75E6,0x0,DP(CalledBeforeTurn)},
	{0x4CC4AF,0x0,DP(CalledBeforeTurnNew)},
	{0x4CC4F8,0x0,DP(CalledBeforeTurnNew)},
// after turn processing
	{0x409CDF,0x0,DP(CalledAfterTurn)},
	{0x4087A0,0x0,DP(CalledAfterTurn)},
	{0x408B84,0x0,DP(CalledAfterTurn)},
	{0x4CC404,0x0,DP(CalledAfterTurn)},
	{0x556B36,0x0,DP(CalledAfterTurn)},
// Addition Speciality Picture
	{0x4E11E0,0x0,DP(NewSpecPrepare)}, // Prepare 1
	{0x4E1F38,0x0,DP(NewSpecShow)},    // Show 1,2
	{0x4DED05,0x0,DP(NewSpecPrepare2)}, // Prepare 2
	{0x4DDA15,0x0,DP(NewSpecDescr)}, // Text 1,2
// Картинка спец. Героя при встрече двух привязано к 12
	{0x5AEC9F,0,DP(SpecPicture)},
// Картинка атаки монстра (AvWattak.def)
//  {0x4118BF,0x0,DP(MonMapPicOn)},
// Картинка убитого монстра (AvWattak.def)
//  {0x4110C7,0x0,DP(MonMapPicOff)},
// Иниц. оружия метания для монстров.
//  {0x43DA8E,0x0,DP(InitArmore)},
// Заход в любой объект Gamer
	{0x4AA766,0,DP(Enter2Object)},
// Заход в любой объект AI
	{0x4ACA04,0,DP(Enter2Object)},
// Атака на монстра с ходу
	{0x4813CA,0x4A7630,DP(Enter2Monster)},
// Атака на монстра с места
	{0x4A9475,0x4A73B0,DP(Enter2Monster2)},
// Выставляет героя на защиту замка (не проверял их пренадлежность)
	{0x4AAD53,0x4175E0,DP(MakeTownDefender)},
// Перемещение монстров по слотам
	{0x5D5297,0,DP(M2MStack)},
	{0x5D53B5,0,DP(M2MSmooth)},
	{0x5AF90A,0,DP(M2MSmooth)},
	{0x5AF841,0,DP(M2MSwap)},
	{0x4C6B98,0,DP(M2MDismiss)},
// Нападение героя на что-то другое
	{0x49F195,0,DP(Get2Battle)},{0x4A09DC,0,DP(Get2Battle)},{0x4A0BF6,0,DP(Get2Battle)},
	 {0x4A1763,0,DP(Get2Battle)},{0x4A25A3,0,DP(Get2Battle)},{0x4A37C2,0,DP(Get2Battle)},
	{0x4A44F8,0,DP(Get2Battle)},{0x4A57D0,0,DP(Get2Battle)},{0x4A8D55,0,DP(Get2Battle)},
	{0x4AAFA1,0,DP(Get2Battle)},{0x4AB028,0,DP(Get2Battle)},{0x4ABBCB,0,DP(Get2Battle)},
	{0x4AC5EA,0,DP(Get2Battle)},{0x4ACC37,0,DP(Get2Battle)},
// Действие конкретного монстра во время битвы
	{0x473F6B,0,DP(Monster2Battle)},
// показывает книгу с закл при навидении мыши
	{0x4761D8,0,DP(ShowMagicBookMouse)},
// настройка монстров на поле боя перед битвой
	{0x463BF6,0,DP(SetUpBeforeBattle)},
// снижение защиты чудищем
	{0x4430C8,0,DP(ReduceDefence)},
	{0x442234,0,DP(ReduceDefence)},
	{0x442387,0,DP(ReduceDefence)},
	{0x4427AF,0,DP(ReduceDefence)},
	{0x4438DD,0,DP(ReduceDefence)},
	{0x46D805,0,DP(ReduceDefence)},
	{0x5F37DE,0,DP(ReduceDefence)},
// предустановки монстров на каждом раунде
	{0x475A3D,0,DP(SetUpNewRound)},
// предустановки монстров на каждом раунде для всех (объемлет)
	{0x4795AE,0,DP(SetUpNewRoundOnce)},
// полет монстров
	{0x44571B,0,DP(DoNotFly)},
// не действует магия
	{0x5A6A5C,0,DP(NoMagic)},
// страх 1
	{0x464944,0,DP(Fear1)},
	{0x4649AA,0,DP(Fear2)},
// проверка на врайтсов
//  {0x4650C5,0,DP(CheckForWrite)},
// проверка на драк-прив
	{0x440245,0,DP(GDMagic)},
// проверка на троля (автовост)
	{0x446BD6,0,DP(IsTroll)},
// проверка на феникса (автовост)
	{0x46905A,0,DP(IsFireBird)},
// проверка на harpy (летит назад)
	{0x47832B,0,DP(IsHarpy)},
// Магия Медузы (превр. в камень)
	{0x4404A0,0,DP(MedusaMagic)},
// Магия Юникорна (ослепляет)
	{0x440337,0,DP(UnicornMagic)},
// прверка на разрешение вывода Message
	{0x4F6C0A,0,DP(Check4MessageEnable)},
// прверка на шутера - без потери мощности
	{0x46723E,0,DP(ShooterPower)},
// проверка на дьявола
	{0x44B00D,0,DP(IsDevil)},
// проверка на дьявола
	{0x43D954,0,DP(IsDevil1)},
	{0x4B3736,0,DP(IsDevil1)},
	{0x4B301C,0,DP(IsDevil1)},
// проверка на дьявола
	{0x44C049,0,DP(IsDevil2)},
// проверка на ангела
	{0x44ACF1,0,DP(IsAngel)},
	{0x44BA58,0,DP(IsAngel)},
// проверка на ангела
	{0x44BA6F,0,DP(IsAngel1)},
// проверка на защищенную карту
	{0x4C4143,0,DP(LoadProtected)},
// формирует подсказку
	{0x40B151,0,DP(HintControl0)}, // get mouse over map item
	{0x40D0EF,0,DP(HintControl)}, // change mouse over message
	{0x413912,0,DP(HintControl0)}, // get right click map item
	{0x415AEE,0,DP(HintWindow)}, // change right click message
// неделя кого-то
	{0x4C8466,0,DP(WeekOf)},
	{0x4C8C1E,0,DP(MonthOf)},
	{0x4C8C65,0,DP(MonthOf2)},
// запретим рестарт сценария
//  {0x41ABF4,0,DP(NoRestart)},
// вызывается для новой карты из компании
	{0x45B3E1,0,DP(GameChoosen2)},
// стрельба башни замка
	{0x43FEE3,0,DP(TowerShot)},
// introduction
	{0x45E3E6,0,DP(ShowIntro)},
	{0x471574,0,DP(ShowIntro)},
	{0x488B4E,0,DP(ShowIntro)},
	{0x4EE86A,0,DP(ShowIntro)},
	{0x4EE95F,0,DP(ShowIntro)},
	{0x4EEF02,0,DP(ShowIntro)},
	{0x4F013D,0,DP(ShowIntro)},
	{0x4F022B,0,DP(ShowIntro)},
	{0x4F0823,0,DP(ShowIntro)},
	{0x4F08CC,0,DP(ShowIntro)},
	{0x4F097A,0,DP(ShowIntro)},
	{0x4F09EA,0,DP(ShowIntro)},
	{0x4F488B,0,DP(ShowIntro)},
	{0x5D81AC,0,DP(ShowIntro)},
	{0x5D823C,0,DP(ShowIntro)},

// выбран туториал
	{0x4EF457,0,DP(TutorialGame)},
// рестарт сценария
	{0x4EF846,0,DP(GameChoosen3)},
// +1 уровень для героя
	{0x4DAB55,0,DP(GainLevel)},
	{0x4DABBD,0,DP(GainLevelSSkill)},
	{0x4DABD3,0,DP(GainLevelSSkill)},
	{0x4DAC00,0,DP(GainLevelSSkill)},
	{0x4DAC16,0,DP(GainLevelSSkill)},
//  {0x46713A,0,DP(ThroughWall)},
	{0x46711A,0,DP(ThroughWall)},
	{0x4DD82F,0,DP(MakeQuestLog)},
//  {0x4DB68C,0,DP(ArtifacsDescription)}, // зачем-то заменяло описание арта на его хинт. called when artifact description is requested in hero screen of IF:Q dialog
// пропуск фазы тактики
	{0x462916,0,DP(NoMoreTactic)},
	{0x474B8F,0,DP(NoMoreTactic1)},
	{0x4758C9,0,DP(NoMoreTactic1)},
// типа сказочный дракон
	{0x47602C,0,DP(Check4Fairy)},
// сила удара сказ дракона
	{0x44836D,0,DP(FairyPower)},
// еженедельное обновление жилищь
	{0x4C8795,0,DP(WeeklyDwellingGrow)},
// отладка скрипта
//	{0x4DE66D,0,DP(ERMDebug)},
// показ версии ВоГа
	{0x4FBFFE,0,DP(ShowVersion)},

// апгрейд монстров
	{0x4E810B,0,DP(FOH_UpgradeTo)},
	{0x469B66,0,DP(FOH_UpgradeTo)},
	{0x47AA1F,0,DP(FOH_UpgradeTo)},
	{0x4A729F,0,DP(FOH_UpgradeTo)},
	{0x4AC4A1,0,DP(FOH_UpgradeTo)},
	{0x4C6A13,0,DP(FOH_UpgradeTo)},
	{0x4C6A69,0,DP(FOH_UpgradeTo)},
	{0x4C6A9F,0,DP(FOH_UpgradeTo)},
	{0x4E4EA6,0,DP(FOH_UpgradeTo)},
	{0x4E64FA,0,DP(FOH_UpgradeTo)},
	{0x4E86B5,0,DP(FOH_UpgradeTo)},
	{0x5280E9,0,DP(FOH_UpgradeTo)},
	{0x529C1B,0,DP(FOH_UpgradeTo)},
	{0x4E807B,0,DP(FOH_CanBeUpgraded)},
	{0x47A9E3,0,DP(FOH_CanBeUpgraded)},
	{0x4AC445,0,DP(FOH_CanBeUpgraded)},
	{0x4E8300,0,DP(FOH_CanBeUpgraded)},
	{0x4E867A,0,DP(FOH_CanBeUpgraded)},
// бросить текущую игру
	{0x409389,0,DP(DropGame1)},
	{0x409314,0,DP(DropGame2)},
	{0x5B397E,0,DP(DropGame3)},
	{0x4F0568,0,DP(DropGame4)},
// подготовить поле битвы
	{0x4627BC,0,DP(PrepareBF)},
	{0x49389F,0,DP(NoCliff)},
	{0x46382E,0,DP(SetBFBack)},
// первая прорисовка поля битвы
	{0x462C19,0,DP(AppearWithoutTacktic)},
// клики мауса на карте
	{0x408955,0,DP(MouseClick)},
// клики мауса на в городе
	{0x5D3867,0,DP(MouseClickTown)},
// вход и выход в Town Hall 3.58 !?TH0 !?TH1
	{0x5D3968,0,DP(EnterTownHall)},
// AI думает брать ресурсы или нет
	{0x52969F,0,DP(AIthinkAboutRes)},
// MP3 файлы
	{0x59AC51,0,DP(MP3Start)}, // нельзя запускать ERM в другом потоке. Правильные хуки поставлены в global_hooks.h
// AI куда ходить
	{0x42E12E,0,DP(AIMapGoal)}, //
// Перерисовка артифактов при их переносе из слота в слот
	{0x50D0FA,0,DP(ArtMove1)}, //
	{0x50D103,0,DP(ArtMove2)}, //
// NPC
	{0x4DA9B6,0,DP(NPC_AddExpo)}, //
	{0x46359E,0,DP(PlaceNPC2)}, //
//  {0x4DD981,0,DP(NPC_ShowInfo)}, //
// Creature Magic Resistance
//  {0x423E35,0,DP(MagicResist)}, // * [esi+4]
//  {0x424A56,0,DP(MagicResist)}, // ** Chain Lighting [esi+edi+4]
//  {0x424B62,0,DP(MagicResist)}, // Chain Lighting [esi+edi+4]
//  {0x424C9C,0,DP(MagicResist)}, // **? [esi+edi+4]
//  {0x424DD6,0,DP(MagicResist)}, // * [esi+4]
//  {0x424EFA,0,DP(MagicResist)}, // **? [esi+4]
//  {0x42504E,0,DP(MagicResist)}, // ? [esi+4]
//  {0x4251B0,0,DP(MagicResist)}, // ? [esi+edi+4]
//  {0x4252E4,0,DP(MagicResist)}, // * [ebx+4]
//  {0x425496,0,DP(MagicResist)}, // * [esi+4]
// edi -> Mon только для этого места вызова!!!!!
	{0x5A7C3B,0,DP(MagicResist)}, // + Any (esi=edx) [edi+x34] ->Mon
// Функция парализации монстром
	{0x4405CA,0,DP(ParalizeChance)}, //
// Death Stare
//  {0x440C00,0,DP(Random4DeathStare)}, //
	{0x440BF6,0,DP(Random4DeathStare)}, //
	{0x440C40,0,DP(DeathStarePost)}, //
// Champion
	{0x44306D,0,DP(IsChampion)}, //
// Кастует магию героя существо
	{0x5A1C7B,0,DP(CastHeroOrCreature)}, //
// Палатка лечит
	{0x47851C,0,DP(FirstAidTentHealths)}, //
// Контроль над балистой
	{0x474589,0,DP(BalistaControl)}, //
// Клики мышкой в экране характеристик героя
	{0x4DD632,0,DP(ClickNPC)}, //
// Клик мышкой на поле битвы на защиту
	{0x47265C,0,DP(ChooseMonAttack)}, //
// Правильно показывает Мифрил на маленькой карте
	{0x5F8602,0,DP(ShowSmallSymbol)}, //
// Клик мышкой при встрече героев
	{0x5B02DD,0,DP(MouseClick2Hero)}, //
// Автоматическое добавление Мифрила
	{0x4E3879,0,DP(AddMithrillAuto)}, //
// Клик мышкой на поле битвы
	{0x4746E8,0,DP(MouseClickBattle)}, //
// Обработка Мифрила в LE
	{0x49F7BD,0,DP(EventPrepareMithril)}, //
	{0x49FDE4,0,DP(EventMakeUpMithril)}, //
// Одевает артифакт
	{0x4E2CC1,0,DP(ArtifactOn)}, //
// Сримает артифакт
	{0x4E2F60,0,DP(ArtifactOff)}, //
// Защита AI от проверки новых артифактов
	{0x433A1A,0,DP(ArtAIAnal)}, //
	{0x433A60,0,DP(ArtAIAnal)}, //
	{0x433DE1,0,DP(ArtAIAnal)}, //
	{0x525639,0,DP(ArtAIAnal)}, //
	{0x5261AD,0,DP(ArtAIAnal)}, //
	{0x52AE2A,0,DP(ArtAIAnal)}, //
// вызов диалога установки ВоГа
	//{0x588478,0,DP(CallWoGSetup)}, // new setup now
// вылет при нападении АИ на героя без армии
	{0x42DC72,0,DP(AIattackNPConly)}, //
// АИ захватывает город с героем с одним NPC без битвы
	{0x5BE41C,0,DP(NPCOnlyInCastle)}, //
	{0x4AADD1,0,DP(NPCOnlyInCastle2)}, //

// передача инфы перед битвой по сети
	{0x512FD2,0,DP(Send2Defender)}, // 1005
	{0x4ACB5A,0,DP(Receive4Attacker)}, //
{0x5575E3,0,DP(Receive4Attacker)}, //
// передача инфы после битвы по сети
	{0x4AE0C3,0,DP(Send4Defender)}, // 1011
//  {0x512FD2,0,DP(Send4Defender)}, // 3.58 MP fix
	{0x557046,0,DP(Receive4DefenderAfter)}, //
// передача инфы после битвы по сети если АИ атаковал человека
	{0x557514,0,DP(Receive4DefenderAfter2)}, //

// построение DIFF файла перед передачей WoG оптимизация
	{0x4CAE51,0,DP(BuildAllDiff)}, //
// запись файлов с 0 компрессией как образ
	{0x6067E2,0,DP(GZOpen)}, //
	{0x4D6FCC,0,DP(GZClose)}, //
	{0x4D700D,0,DP(GZWrite)}, //
// правый клик на монстре на карте - инфа о нем
	{0x40A781,0,DP(RightClickAtMonster)}, //
// После выигрыша защищающегося героя в МП корректировка показа скилов
	{0x5570C1,0,DP(FixHeroSecScillsShow)}, //

// Показ экспы для стэка (большой рисунок/анимация)
	{0x5F5545,0,DP(ShowExpo)}, //
// Показ экспы для стэка (средний рисунок в экране Героя)
	{0x5BC9B1,0,DP(ShowNumberMid)}, //

// Показ экспы при плавном перемещении юнитов в стэках
	{0x4493EB,0,DP(ShowHint)}, //
	{0x449FCC,0,DP(ShowHint)}, //
	{0x44A0EE,0,DP(ShowHint1)}, //
// 3.58
// Carry Over Hero Setup
	{0x4860F9,0,DP(CarryOverHero)}, //
// триггер перемещение мыши и вывод текста
	{0x492E4F,0,DP(MouseMoveBattle)}, //
// тригер движения мауса по полям в городе
	{0x5D4612,0,DP(MouseOverTown)}, //
// кастование магии на карте приключений
	{0x409EC4,0,DP(AdvMagicManager)}, //
// кастование магии на карте приключений (вспомогат)
	{0x41C532,0,DP(AdvMagicManagerSkill)}, //
// расчет сопр магии типа Dwarf
	{0x5A88F0,0,DP(MagicDwarfResistance3)}, //
//  {0x5A0605,0,DP(MagicDwarfResistance)}, //
//  {0x5A4D6F,0,DP(MagicDwarfResistance)}, // для заклов по площади
	// does not work properly :-(
	//  {0x5A658D,0,DP(MagicDwarfResistance)}, // для след целей в цепной молнии
//  {0x5A4F49,0,DP(MagicDwarfResistance2)}, // для армагедона
// оставленные монстры убегают, а не деруться.
	{0x4A6ADD,0,DP(E2MWoMoNoBattle)}, //
// левый клик мауса на карте
	{0x408994,0,DP(MouseClick2)}, //
	{0x4089AB,0,DP(MouseClick3)}, //
// Blocking Monster ability
	{0x43F95B,0,DP(ApplyDamage)}, //
	{0x43FA5E,0,DP(ApplyDamage)}, //
	{0x43FD3D,0,DP(ApplyDamage)}, //
	{0x4400DF,0,DP(ApplyDamage)}, //
	{0x440858,0,DP(ApplyDamage)}, //
	{0x440E70,0,DP(ApplyDamage)}, //
	{0x441048,0,DP(ApplyDamage)}, //
	{0x44124C,0,DP(ApplyDamage)}, //
	{0x441739,0,DP(ApplyDamage)}, //
	{0x44178A,0,DP(ApplyDamage)}, //
	{0x46595F,0,DP(ApplyDamage)}, //
	{0x469A93,0,DP(ApplyDamage)}, //
	{0x5A1065,0,DP(ApplyDamage)}, // quick battle
// Post instructions call
	{0x4C01A2,0x0,DP(PostInstrCall)},
// слияние стэков в городе
	{0x5D520D,0x0,DP(M2MCombine)},
// Add Town garrizon in a Hero's army
	{0x4B9CE0,0x0,DP(M2MHero2Town)},
// Показ опыта в диалоге инфы о монстре
// 5F3700 (вызов 4684F0) - битва
// 5F3EF0 (вызов 4C6B56) - обычно
// 5F45B0 (вызовов много) - в жтлищах - не надо
	{0x4C6B56 /*5F3EF0*/,0x0,DP(ShowExpoPrep)},
	{0x4684F0 /*5F3700*/,0x0,DP(ShowExpoPrepBattle)},
	{0x4E8892 /*5F45B0*/,0x0,DP(ShowExpoPrepUnknown)},
	{0x5513B3 /*5F45B0*/,0x0,DP(ShowExpoPrepUnknown)},
	{0x551460 /*5F45B0*/,0x0,DP(ShowExpoPrepUnknown)},
	{0x55150E /*5F45B0*/,0x0,DP(ShowExpoPrepUnknown)},
	{0x5515B4 /*5F45B0*/,0x0,DP(ShowExpoPrepUnknown)},
	{0x5655AE /*5F45B0*/,0x0,DP(ShowExpoPrepUnknown)},
	{0x566916 /*5F45B0*/,0x0,DP(ShowExpoPrepUnknown)},
//  {0x /*5F45B0*/,0x0,DP(ShowExpoPrepUnknown)},

// плавная передача монстров из одного места в другое
	{0x4DDDE2,0x0,DP(SmoothPassInternalFun)},
	{0x5AF90A,0x0,DP(SmoothPassInternalFun)},
	{0x5D53B5,0x0,DP(SmoothPassInternalFun)},
// перенос стэков внутри экрана героя
	{0x4DDD77,0x0,DP(MoveStackInHeroArmy)}, // *
	{0x4DDDC3,0x0,DP(MoveStackInHeroArmy2)}, // *
// показ диалога об опыте стэка
//  {0x5F4E84,0x0,DP(ExpDetailedInfo)}, // *
	{0x5F4C63,0x0,DP(ExpDetailedInfo)}, // *
//  {0x5F522D,0x0,DP(ExpDetailedInfo)}, // *
// апгрэйд экспы монстров на холме
	{0x4E86E3,0x0,DP(UpgrHill)}, // *
// апгрэйд экспы монстров в экране монстра
	{0x4C6BD6,0x0,DP(UpgrInd)}, // *
// снижение цены кастинга закла
	{0x4E5572,0x0,DP(CheckSpellCostReduction)}, // *
// модификация бонуса защиты при выборе защиты
	{0x4790C3,0x0,DP(DefenseBonus)}, // *
// модификация бонуса DeathBlow
	{0x4435A3,0x0,DP(DeathBlow)}, // *
// модификация Экстра дамэдж против конкретного монстра
	{0x443198,0x0,DP(PersonalHate)}, // *
// запрет читерства в синглплэйере
	{0x402334,0x0,DP(CheatEnabled)}, // *
// запрет работы чит меню
	{0x4F8710,0x0,DP(ParseCheatMenuCommand)}, // *
// SoD фикс. установка существа уровня для случ героя
	{0x4D8F1D,0x0,DP(SetUpRandomCreatures)}, // *
// WoG Cheats
	{0x4024AC,0x0,DP(CheckCheat)}, // *
	{0x4024F1,0x0,DP(CheckCheat)}, // *
	{0x402536,0x0,DP(CheckCheat)}, // *
	{0x4025D5,0x0,DP(CheckCheat)}, // *
	{0x40260B,0x0,DP(CheckCheat)}, // *
	{0x402636,0x0,DP(CheckCheat)}, // *
	{0x402675,0x0,DP(CheckCheat)}, // *
	{0x4026A0,0x0,DP(CheckCheat)}, // *
	{0x4026D1,0x0,DP(CheckCheat)}, // *
	{0x40272F,0x0,DP(CheckCheat)}, // *
	{0x4027AC,0x0,DP(CheckCheat)}, // *
	{0x402811,0x0,DP(CheckCheat)}, // *
	{0x402898,0x0,DP(CheckCheat)}, // *
	{0x4028BA,0x0,DP(CheckCheat)}, // *
	{0x402927,0x0,DP(CheckCheat)}, // *
	{0x402953,0x0,DP(CheckCheat)}, // *
	{0x471BA9,0x0,DP(CheckCheat)}, // *
	{0x471BD8,0x0,DP(CheckCheat)}, // *
	{0x471C09,0x0,DP(CheckCheat)}, // *
// кастинг спела после удара
//  {0x4412AE,0x0,DP(CastPostAttackSpell)}, // *
// минотавр - позитивная мораль всегда
	{0x43DDCE,0x0,DP(MinotourMoral)}, // *
// стреляет даже если противник рядом
	{0x442686,0x0,DP(ShootAnyTime)}, // *
	{0x44587A,0x0,DP(ShootAnyTime)}, // *
	{0x467231,0x0,DP(ShootAnyTime2)}, // *
// unicorn - магическая аура
	{0x43E800,0x0,DP(UnicornAura)}, // *
	{0x43E8AE,0x0,DP(UnicornAura2)}, // *
// поддержка режима редактирования опций ВоГификации
	//{0x4EF3A0,0x0,DP(GoToNewGameScreen)}, // * new setup now
// Сопротисление спец магии Dispel
	{0x5A8454+8,0x0,DP(DispellResist)}, // *
	{0x5A84DF+8,0x0,DP(DispellResist)}, // *
/*
	{0x5A1971,0x0,DP(SpecMagicResiatance)}, // *
//...
	{0x041FAA0+0x01D3,0x0,DP(SpecMagicResiatance)}, // *
	{0x0436A80+0x0065,0x0,DP(SpecMagicResiatance)}, // *
	{0x0437F10+0x010D,0x0,DP(SpecMagicResiatance)}, // *
	{0x0438660+0x0092,0x0,DP(SpecMagicResiatance)}, // *
	{0x0438A10+0x00ED,0x0,DP(SpecMagicResiatance)}, // *
	{0x0438BB0+0x00DF,0x0,DP(SpecMagicResiatance)}, // *
	{0x0438D50+0x014E,0x0,DP(SpecMagicResiatance)}, // *
	{0x0438EC0+0x0269,0x0,DP(SpecMagicResiatance)}, // *
	{0x0439330+0x01BF,0x0,DP(SpecMagicResiatance)}, // *
	{0x043AFC0+0x0167,0x0,DP(SpecMagicResiatance)}, // *
	{0x043B150+0x0158,0x0,DP(SpecMagicResiatance)}, // *
	{0x059FBB0+0x00C3,0x0,DP(SpecMagicResiatance)}, // *
	{0x059FE50+0x0088,0x0,DP(SpecMagicResiatance)}, // *
	{0x059FFE0+0x0065,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A0140+0x04BA,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A0140+0x0EB6,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A0140+0x10B3,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A0140+0x1A57,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A0140+0x1FA4,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A3CD0+0x0063,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A3F90+0x0019,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A3FD0+0x0087,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A3FD0+0x0142,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A4260+0x0085,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A4260+0x013E,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A4C80+0x00E4,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A4ED0+0x006E,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A6500+0x0082,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A6500+0x00A9,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A69E0+0x0060,0x0,DP(SpecMagicResiatance)}, // *
	{0x05A8950+0x001A,0x0,DP(SpecMagicResiatance)}, // *
*/
// Фикс не проверки АИ флага разрешения/запрещения тактики
	{0x462847,0x0,DP(HeroTacticFix)}, // *
// Фикс неправильной картинки артифактов после 127 (пропуск картинки Пандоры) SoD bug 
	{0x4C970F,0x0,DP(WrongRelictPictureFix)}, // *
// запрет на смену вида курсора
	{0x50CEAA,0x0,DP(DisableMouse)}, // *
// расчет силы магии архангела (можно сделать управляемым !!!!)
	{0x5A87AD,0x0,DP(IsAngel3)}, // *
// показ инфы о стэке на поле боя по правой кнопке с кнопкой Ок - далее
// возможность апгрейда существ в городе, если заменен тип - далее
// фикс падения при выходе при попытке освободить мою статическую память
	{0x619BB1,0x0,DP(MyStatMemCheck)}, // *
// снятие закла со стэка после атаки
	{0x441C79,0x0,DP(ResetSpell)}, // *
// типы физич атаки - первый удар
	{0x441AE5,0x0,DP(MonsterStrikeFirst)}, // *
// типы физич атаки - ответный удар
	{0x441B5D,0x0,DP(MonsterStrikeBack)}, // *
// типы физич атаки - второй удар (double strike)
	{0x441BFC,0x0,DP(MonsterStrikeSecond)}, // *
// fix Win98 quit crash
	{0x4F84F4,0x0,DP(FixQuit98Crash)}, // *
// new WoG 3.58 art - shield (retailate)
	{0x441AFF,0x0,DP(HasArt157)}, // *
// new WoG 3.58 art - axe (+1strike)
	{0x441C01,0x0,DP(HasArt158)}, // *
// fix No default Bio if a name set but bio not
//  {0x4DD97E,0x0,DP(FixBioBug)}, // *
// Dracolish 100% poison
//  {0x440559,0x0,DP(DracolishRandom)}, // *
// Dragon Heart Art - summon dragons
	{0x464FC1,0x0,DP(Check4DragonHeartArt)}, // *
// Cast Animate Dead without a Hero at level 0
//  {0x5A87D5,0x0,DP(NonHeroNPCAnimDeadCast)}, // *
// Dracolish (Power Lich shoot)
	{0x43FA1F,0x0,DP(DracolichShoot)}, // *
	{0x41ED5F,0x0,DP(DracolichShoot)}, // *
// Swap AI stacks before battle
//  {0x4ADFAD,0x0,DP(SwapAIStacks)}, // *
	{0x4ADF80,0x0,DP(DisableAIStackSwitching)}, // *
	{0x4ADFA8,0x0,DP(DisableAIStackSwitching)}, // *
// Shooting Animation for spec units (type of shoot in the table 0)
	{0x43DAAA,0x0,DP(LoadShootingDef)}, // *
// подстройка проходимости клетки
	{0x506130,0x0,DP(AdjustSquarePassability)}, // *
// отсылка инфы о двух героях по сети - встреча (3.58)
//  {0x4AAB83,0x0,DP(Send358_1)}, // *
// прием инфы о двух героях по сети - встреча (3.58)
//  {0x40624D,0x0,DP(Receive358_1)}, // *
	{0x5D4E9F,0x0,DP(SoDFix_SpaceInAlliedTown)}, // *
// вызов элементалей не работает без хозяина-героя
	{0x5A7579,0x0,DP(FixSummonElementalNoHero)}, // *
// вызов элементалей не работает без хозяина-героя
	{0x5A8D56,0x0,DP(FixDeathRippleNoHero)}, // *
// расчет уровня заклинания 
	{0x4E5362,0x0,DP(FixNoHero4SpellLevelCalc)}, // *
	{0x4E552B,0x0,DP(FixNoHero4SpellLevelCalc)}, // *
	{0x4E57EA,0x0,DP(FixNoHero4SpellLevelCalc)}, // *
	{0x4E58B9,0x0,DP(FixNoHero4SpellLevelCalc)}, // *
// расчет уровня заклинания 2
	{0x5A86AF,0x0,DP(FixNoHero4SpellPower)}, // *
// правильный показ типа двелинга в окне Kingdom Overview
	{0x520DB3,0x0,DP(FixWrongDwIndKingdomOverview)}, // *
// фикс расчета длительности заклинания, если нет героя
	{0x425C53,0x0,DP(FixNoHero4SpellDuration)}, // *
	{0x43C53C,0x0,DP(FixNoHero4SpellDuration)}, // *
	{0x5272AF,0x0,DP(FixNoHero4SpellDuration)}, // *
	{0x5A03CC,0x0,DP(FixNoHero4SpellDuration)}, // *
	{0x5A71A1,0x0,DP(FixNoHero4SpellDuration)}, // *
// показ инфы о стэке на поле боя по правой кнопке с кнопкой Ок
	{0x476445,0x0,DP(FixShowBFPopUpMonInfo)}, // *
// отладка сети
//  {0x51BD60,0x0,DP(NetworkMessageParse)}, // *
	{0x5A0FF6,0x0,DP(DeathRippleResist)}, // *
// фикс неправильной подсказки для артифактов в рюкзаке в ArtMerchant
	{0x5EEBC5,0x0,DP(SoDArtMenchHintFix)}, // *
// не стреляет второй раз, если выстрелов не осталось
	{0x43FF7E,0x0,DP(FixShootTwiceOneShot)}, // *
// фикс неверного опыта для существ при наеме в замке
	{0x5C71D5,0x0,DP(TownWrongExpoFix)}, // *
// запрет каста клона на командиров
	{0x5A875B,0x0,DP(NoClone4CommandersFix)}, // *
// прием команды по сети во время битвы
	{0x473E1C,0x0,DP(ReceiveNetBFCommand)}, // *
// прием команды по сети на карте прикл.
	{0x557E06,0x0,DP(ReceiveNetAMCommand)}, // *
// 3.58f снятие бонуса героя от GR при переходе на след. карту
	{0x486245,0x0,DP(CrossOverGodBonus)}, // *
// перевод героя в гарнизон города при атаке
	{0x4AAFCB,0x0,DP(M2MHero2Town2)}, // 3.58f Human autoaranging
	{0x525896,0x0,DP(M2MHero2Town2)}, // 3.58f AI ???
	{0x43DA74,0x0,DP(LoadShootingDef4Commanders)}, // 3.58f Fix "no Commander shooting animation loaded"
	{0x5D4643,0x0,DP(FixWrongTownListInfo)}, // 3.58f Fix "wrong town info in town list MP"

// 3.59
	{0x4EDCC5,0x0,DP(MPProcessTimeAndRemind)}, // show time on the screen every second
	// Fix Time does not count until new player start turn
	{0x419EA8,0x0,DP(MPStartTimerFix)},
	// Load Game - настройка игроков
	{0x58C6E9,0x588600,DP(LoadGameSetUpPlayers)},
	// show WoG creatures in animated waiting dialogs
	//{0x589190,0x0,DP(ChooseCrToAnimDlg)},
	//{0x589259,0x0,DP(ChooseCrToAnimDlg)},
	//{0x55543B,0x0,DP(ChooseCrToAnimDlg)},
	//{0x556FAF,0x0,DP(ChooseCrToAnimDlg)},
	//{0x557493,0x0,DP(ChooseCrToAnimDlg)},
	// new town support
	{0x4CE132,0x0,DP(AdjustNewTownStructures)}, // setup all town structures at EXE start

	{0x4472F8,0x0,DP(CastMessage)}, // fix previous WoG version
	{0x5A223D,0x0,DP(AcidBreathMessage)},
	{0x44C15C,0x0,DP(AlwaysLuckyMessage)},

	// Spell support
	{0x4EDEAF,0x0,DP(ParseSpTraitsTxt)}, // parse SpTraits

	//{0x601E07,0x0,DP(newPaintGetClientRect)},//Hook GetClientRect in WM_PAINT message
	//{0x601ACC,0x0,DP(px_DirectDrawCreate)}, //DX Proxy Run
	//{0x55479B,0x0,DP(HookCmdLineEnteredText)},	

	// врезка перехода на менеджер сопративления монстров
	//{0x44A4A7, 0x0, DP(ResistMagicManager)}, // Diakon

	// врезка Fix генератора
	//{0x54110F, 0x0, DP(Hook54110F)}, // Diakon

	// подгрузка snd (+глючное сообщение об ошибке, если нет towns.snd и не найден звук)
	//{0x55C704,0x0,DP(LoadNewSnd)}, // Diakon
	//{0x55C7E0,0x0,DP(LoadNewSnd)}, // Diakon

	{0,0,0}
};

struct __Accessers{
	long  where;
	long  what;
#ifdef DEBUG
// DEBUG для отладки
	char *remember;
	long  size;
#endif
	long  len;
} Accessers[]={
// разрешает запуск более чем одних героев
///////////////////////////////////////  {0x4F80FA,DS0(0xB8),1},
// новая таблица видео
	{0x44D5C3+2,DS0(VidArr),4},
	{0x598FF8+2,DS0(VidArr),4},
	{0x44D561+2,DS0(&VidArr[0].Po),4},
	{0x598EFD+2,DS0(&VidArr[0].Po),4},
	{0x5975EB+2,DS0(&VidArr[0].Atr[0]),4},
	{0x59787C+3,DS0(&VidArr[0].Atr[0]),4},
	{0x44D873+3,DS0(&VidArr[0].Atr[1]),4},
	{0x599288+3,DS0(&VidArr[0].Atr[1]),4},
	{0x44D828+3,DS0(&VidArr[0].Atr[2]),4},
	{0x44D937+3,DS0(&VidArr[0].Atr[2]),4},
	{0x44DBA9+3,DS0(&VidArr[0].Atr[2]),4},
	{0x5977E3+2,DS0(&VidArr[0].Atr[2]),4},
	{0x599249+3,DS0(&VidArr[0].Atr[2]),4},
	{0x59930B+3,DS0(&VidArr[0].Atr[2]),4},
	{0x44D5BD+2,DS0(&VidArr[0].Atr[3]),4},
	{0x598EC8+3,DS0(&VidArr[0].Atr[3]),4},
// показывает тип монстров в городе всегда
	{0x416E01,DS0(0x000001B8),4},
	{0x416E05,DS0(0x0016EB00),4},
// Устанавливает тип для фиксированных кампаний СоД на Вог
	{0x584A2C+3,DS0(0x00000003),4},
// ZObjcts.txt вместо Objects.txt
	{0x67FBD4,DS0(0x6A624F5A),4},
// новое имя Objects.txt для генератора случ. карт
	{0x5366FB+1,DS0(RMObjectsFile),4},
// ZCrBank.txt вместо CrBanks.txt
	{0x6703A8,DS0(0x4272435A),4},
	{0x6703AC,DS0(0x2E6B6E61),4},
// ZCrGn1.txt вместо CrGen1.txt
	{0x660278,DS0(0x4772435A),4},
// заменяет видео Credits.bik на Credits.smk
//  {0x683CAC,DS0(0x01000000),4},
// заменяет BIK видео на новую DLL 0x08000000 на 0x00800000
//  {0x44D3BC+2,DS0(0x00800000),4},
// генерация WoG карт вместо SoD в Рандоме
	{0x54A1D4+1,DS0(SOGMAPTYPE),4},
/*
// CalledDayly
	{0x4C8101,DS0(0xE9),1},
	{0x4C75E6,DS0(0xE9),1},
*/
// NewStartLoading
//  {0x4FDCF0,DS0(0xE9),1},
// Artifact
// подгрузка artraits
	{0x44CB32,DS(ArtTable),4},
	{0x44CD1E,DS(ArtTable),4},
	{0x44CD6C,DS(ArtTable),4},
	{0x660B68,DS(ArtTable),4},
	{0x44CCDF,DS(&ArtTable[0].Disable),4},
	{0x44CCFA,DS(&ArtTable[0].NewSpell),4},
	{0x44CCA8,DS((ARTNUM+2)*4),4},
	{0x44CACA,DS((ARTNUM+2)*4),4},
// подгрузка artevent
	{0x49DD9A,DS(&ArtPickUp[-1]),4},
	{0x49F2E7,DS(ArtPickUp),4},
	{0x49F51E,DS(ArtPickUp),4},
	{0x49F5DA,DS(ArtPickUp),4},
	{0x49DD90,DS((ARTNUM)*4),4},
// бонусы артифактов
	{0x4E2D26,DP(ArtBonus),4}, //
//  {0x4E2D3D,DP(&ArtBonus[ARTNUM]),4}, //
	{0x4E2D3D,DP(&ArtBonus[160]),4}, // 3.58 Combo Part fix
//  {0x4E2DD7,DP(&ArtBonus[ARTNUM]),4}, //
	{0x4E2DD7,DP(&ArtBonus[160]),4}, // 3.58 Combo Part fix
	{0x4E2DF1,DP(ArtBonus),4}, //
	{0x4E2E95,DP(ArtBonus),4}, //
//  {0x4E2EAC,DP(&ArtBonus[ARTNUM]),4}, //
	{0x4E2EAC,DP(&ArtBonus[160]),4}, // 3.58 Combo Part fix
//  {0x4E2F41,DP(&ArtBonus[ARTNUM]),4}, //
	{0x4E2F41,DP(&ArtBonus[160]),4}, // 3.58 Combo Part fix
	{0x4E2F7C,DP(ArtBonus),4}, //
//  {0x4CF680,DP(0x75),1}, //
//  {0x48E5B3,DP(0x75),1}, //
//  {0x4DC12D,DP(0x75),1}, //
//  {0x4DC246,DP(0x75),1}, //
//  {0x67F554  ,(long)3,4}, // делает вид, что установлен AB
// CrBank
	{0x47A4A8+3,DS (crbankt1),4},
	{0x47A4AF+3,DS (crbankt2),4},
	{0x47A4B6+3,DS0(&CrBankTable[0].Name),4},
	{0x47A68F+1,DS0(&CrBankTable[BANKNUM].Name),4},
	{0x47A3C1+1,DS (CrBankTable),4},
	{0x47A3EC+1,DS0(CrBankTable),4},
	{0x67029C  ,DS0(CrBankTable),4},
	{0x47A3BA+1,DS0(BANKNUM),1},
	{0x47A3E5+1,DS0(BANKNUM),1},
// Monster
	{0x50CBB5+3,DS0(MONNUM_OLD)-150+5,4},// Diakon
	{0x47AD75+3,DS0(MONNUM_OLD)-150+5,4},// Diakon
	{0x67FF74  ,DS (MonATable),4},
	{0x50CC01+3,DS0(MonATable),4},
	{0x47ADCE+3,DS (MonTable),4},
	{0x6747B0  ,DS0(MonTable),4},
	{0x47AE28+3,DS (MonTable2),4},
	{0x47ADF5+1,DS0(MonTable2),4},
	{0x47B12C+1,DS0(MonTable2),4},
	{0x47AEBC+3,DS (MonTable3),4},
	{0x47AE85+1,DS0(MonTable3),4},
	{0x47B10C+1,DS0(MonTable3),4},
	{0x47B091+3,DS (MonTable4),4},
	{0x47B057+1,DS0(MonTable4),4},
	{0x47B0EC+1,DS0(MonTable4),4},
	{0x4A1655+2,DS0(MONNUM),4},{0x4A189B+1,(long)MONNUM,4},  

	{0x47ADEC+1,(long)MONNUM,4},{0x47AE7C+1,(long)MONNUM,4},{0x47B04D+1,(long)MONNUM,4},
	{0x47B0E5+1,(long)MONNUM,4},{0x47B105+1,(long)MONNUM,4},{0x47B125+1,(long)MONNUM,4},

	{0x40AC91+1,(long)MONNUM,4},{0x40ACF1+2,(long)MONNUM,4},{0x40C2B0+1,(long)MONNUM,4},
	{0x416948+1,(long)MONNUM,4},{0x43F9B2+1,(long)MONNUM,4},{0x43FA88+1,(long)MONNUM,4},
	{0x43FDA8+1,(long)MONNUM,4},{0x43FE24+2,(long)MONNUM,4},
	{0x4401B5+1,(long)MONNUM,4},{0x4409D7+1,(long)MONNUM,4},{0x440A21+1,(long)MONNUM,4},
	{0x440A85+1,(long)MONNUM,4},{0x440C9A+1,(long)MONNUM,4},{0x440CD5+1,(long)MONNUM,4},
	{0x440D49+1,(long)MONNUM,4},{0x440D86+1,(long)MONNUM,4},{0x440F5B+1,(long)MONNUM,4},
	{0x4418D0+1,(long)MONNUM,4},{0x44192E+1,(long)MONNUM,4},{0x443265+1,(long)MONNUM,4},
	{0x44329A+1,(long)MONNUM,4},{0x443307+1,(long)MONNUM,4},{0x44333C+1,(long)MONNUM,4},
	{0x44372A+2,(long)MONNUM,4},{0x443796+2,(long)MONNUM,4},{0x446C86+1,(long)MONNUM,4},
	{0x446CF4+1,(long)MONNUM,4},{0x44730E+1,(long)MONNUM,4},{0x44BB13+2,(long)MONNUM,4},
	{0x44BC64+1,(long)MONNUM,4},{0x44C060+2,(long)MONNUM,4},{0x464670+1,(long)MONNUM,4},
	{0x46487A+1,(long)MONNUM,4},{0x464A91+1,(long)MONNUM,4},{0x465242+1,(long)MONNUM,4},
	{0x4652E4+1,(long)MONNUM,4},{0x478593+1,(long)MONNUM,4},{0x4785CB+1,(long)MONNUM,4},
	{0x4790D7+1,(long)MONNUM,4},{0x479142+1,(long)MONNUM,4},{0x47927F+1,(long)MONNUM,4},
	{0x47931B+1,(long)MONNUM,4},{0x4840B2+1,(long)MONNUM,4},
	{0x4863FC+2,(long)MONNUM,4},{0x491F65+1,(long)MONNUM,4},
	{0x491FB5+1,(long)MONNUM,4},{0x4920A9+2,(long)MONNUM,4},{0x4921A1+2,(long)MONNUM,4},
	{0x4921EC+2,(long)MONNUM,4},{0x4922B6+1,(long)MONNUM,4},{0x4922ED+1,(long)MONNUM,4},
	{0x49233E+1,(long)MONNUM,4},{0x492375+1,(long)MONNUM,4},{0x492521+1,(long)MONNUM,4},
	{0x4925BE+1,(long)MONNUM,4},{0x49262F+1,(long)MONNUM,4},{0x4926C9+1,(long)MONNUM,4},
	{0x49275D+1,(long)MONNUM,4},{0x4927E8+1,(long)MONNUM,4},{0x492959+1,(long)MONNUM,4},
	{0x492D0A+2,(long)MONNUM,4},{0x492DAD+1,(long)MONNUM,4},
	// 497903
	{0x49E705+2,(long)MONNUM,4},
	{0x49E728+2,(long)MONNUM,4},{0x49EE73+1,(long)MONNUM,4},{0x49EFC0+1,(long)MONNUM,4},
	{0x4A0435+2,(long)MONNUM,4},{0x4A04D7+2,(long)MONNUM,4},
	// 4A1655
	{0x4A17D9+1,(long)MONNUM,4},{0x4A1803+1,(long)MONNUM,4},{0x4A1828+1,(long)MONNUM,4},
	{0x4A184D+1,(long)MONNUM,4},
	// 4A189B
	{0x4A19FA+2,(long)MONNUM,4},
	{0x4A1A9F+2,(long)MONNUM,4},{0x4A42F1+1,(long)MONNUM,4},{0x4A6A7E+1,(long)MONNUM,4},
	{0x4A6C96+2,(long)MONNUM,4},{0x4A6F57+2,(long)MONNUM,4},{0x4A6FC6+1,(long)MONNUM,4},
	{0x4AB893+2,(long)MONNUM,4},{0x4ABC6C+1,(long)MONNUM,4},{0x4ABF68+1,(long)MONNUM,4},
	{0x4AE27F+1,(long)MONNUM,4},{0x4AE2CC+2,(long)MONNUM,4},{0x4CC86D+1,(long)MONNUM,4},
	{0x4CC8C9+1,(long)MONNUM,4},{0x4CC988+1,(long)MONNUM,4},{0x4DBA61+1,(long)MONNUM,4},
	{0x4DBA9D+2,(long)MONNUM,4},{0x4DBAE0+2,(long)MONNUM,4},{0x4DBB2D+1,(long)MONNUM,4},
	{0x4DBB74+1,(long)MONNUM,4},{0x4DBB96+2,(long)MONNUM,4},{0x4F1F88+1,(long)MONNUM,4},
	{0x4F2010+1,(long)MONNUM,4},{0x4F20D6+1,(long)MONNUM,4},{0x4F2138+1,(long)MONNUM,4},
	{0x4F479B+1,(long)MONNUM,4},{0x4F5BD0+2,(long)MONNUM,4},{0x4F5C4A+2,(long)MONNUM,4},
	{0x51402E+1,(long)MONNUM,4},{0x514078+1,(long)MONNUM,4},{0x5213E4+2,(long)MONNUM,4},
	{0x52141C+2,(long)MONNUM,4},{0x5217C7+1,(long)MONNUM,4},{0x521812+1,(long)MONNUM,4},
	{0x521835+2,(long)MONNUM,4},{0x52191C+1,(long)MONNUM,4},{0x52195C+1,(long)MONNUM,4},
	{0x5219BD+1,(long)MONNUM,4},{0x5219CD+2,(long)MONNUM,4},{0x5219F7+2,(long)MONNUM,4},
	{0x52FEF7+2,(long)MONNUM,4},{0x52FF5B+2,(long)MONNUM,4},{0x52FFB9+2,(long)MONNUM,4},
	{0x55036E+1,(long)MONNUM,4},{0x550825+1,(long)MONNUM,4},{0x55113E+1,(long)MONNUM,4},
	{0x551D8E+2,(long)MONNUM,4},{0x5632DA+2,(long)MONNUM,4},{0x565365+1,(long)MONNUM,4},
	{0x56637B+1,(long)MONNUM,4},{0x5664C8+1,(long)MONNUM,4},{0x56EBD3+1,(long)MONNUM,4},
	{0x56ECD4+1,(long)MONNUM,4},{0x56F115+1,(long)MONNUM,4},{0x5706A5+1,(long)MONNUM,4},
	{0x570A6E+1,(long)MONNUM,4},{0x570D4D+1,(long)MONNUM,4},{0x59F90A+2,(long)MONNUM,4},
	{0x5A21ED+1,(long)MONNUM,4},{0x5A22E6+1,(long)MONNUM,4},{0x5A2CC3+1,(long)MONNUM,4},
	{0x5A2DCD+2,(long)MONNUM,4},{0x5A2E27+2,(long)MONNUM,4},{0x5A753A+1,(long)MONNUM,4},
	{0x5A77C0+2,(long)MONNUM,4},{0x5A77FB+2,(long)MONNUM,4},{0x5A79B2+1,(long)MONNUM,4},
	{0x5A79ED+1,(long)MONNUM,4},{0x5A7D4B+2,(long)MONNUM,4},{0x5A7DBB+2,(long)MONNUM,4},
	{0x5A8A08+2,(long)MONNUM,4},{0x5A8A69+2,(long)MONNUM,4},{0x5A8B52+2,(long)MONNUM,4},
	{0x5A9378+1,(long)MONNUM,4},{0x5B0E09+2,(long)MONNUM,4},{0x5C08EA+2,(long)MONNUM,4},
	{0x5C605C+1,(long)MONNUM,4},{0x5C656A+1,(long)MONNUM,4},{0x5C789A+1,(long)MONNUM,4},
	{0x5C7946+1,(long)MONNUM,4},{0x5C79BC+1,(long)MONNUM,4},{0x5C7A1D+1,(long)MONNUM,4},
	{0x5C7AB0+1,(long)MONNUM,4},{0x5C7B25+2,(long)MONNUM,4},{0x5C7B48+1,(long)MONNUM,4},
	{0x5C7FD4+1,(long)MONNUM,4},{0x5C8046+1,(long)MONNUM,4},{0x5C84B0+1,(long)MONNUM,4},
	{0x5C9C3A+1,(long)MONNUM,4},
	{0x5D0C56+1,(long)MONNUM,4},{0x5D1059+2,(long)MONNUM,4},
	{0x5DD0A4+1,(long)MONNUM,4},{0x5DD976+1,(long)MONNUM,4},{0x5DDA4A+1,(long)MONNUM,4},
	{0x5EF08C+1,(long)MONNUM,4},{0x5F3939+1,(long)MONNUM,4},{0x5F40CC+1,(long)MONNUM,4},
/* это все элементы диалогов, а не монстры
	{0x5121EA+1,(long)MONNUM,4},{0x53A51F+1,(long)MONNUM,4},{0x53B1ED+1,(long)MONNUM,4},
	{0x5601C4+2,(long)MONNUM,4},{0x5758AC+1,(long)MONNUM,4},{0x575B6C+1,(long)MONNUM,4},
//  {0x5AC62E+1,(long)MONNUM,4}, // вызывает рисование книги закл. на голове
	{0x5C2565+1,(long)MONNUM,4},{0x5C3DFD+1,(long)MONNUM,4},
	{0x5CA9B3+1,(long)MONNUM,4},{0x5CAA24+1,(long)MONNUM,4},{0x5CAA94+1,(long)MONNUM,4},
	{0x5CABF9+1,(long)MONNUM,4},{0x5CAC6A+1,(long)MONNUM,4},{0x5CACDB+1,(long)MONNUM,4},
	{0x5CAE41+1,(long)MONNUM,4},{0x5CAEB2+1,(long)MONNUM,4},{0x5CAF23+1,(long)MONNUM,4},
*/
//  {0x+1,(long)MONNUM,4},{0x+1,(long)MONNUM,4},{0x+1,(long)MONNUM,4},

// Hero
// Новые специализации (по индексу картинки)
//  {0x4E1F21+2,DS0(0x34),1}, // картинка в диалоге о герое
//  {0x4E1F3B+2,DS0(0x34),1}, // текст о герое
//  HeroBios
	{0x5B9A0E+2,DS0((HERNUM+7)*4),4},
	{0x5B9A18+2,DS0(&HBiosTable[-1].HBios),4},
	{0x5641A2+2,DS0(&HBiosTable[-1].HBios),4},
	{0x4D92B7+3,DS (HBiosTable),4},
	{0x4DD975+3,DS0(HBiosTable),4},
//  HeroSpec
	{0x4D7449+1,DS0(&HSpecTable[0].SpFull),4},
	{0x4D745A+2,DS0(&HSpecTable[HERNUM].SpFull),4},
	{0x679C80  ,DS (HSpecTable),4},

	{0x4E68D7+1,DS0(HERNUM),4},
	{0x4E6897+2,DS (HTable2),4},
	{0x4E689F+3,DS0(HTable2),4},
	{0x4E68E0+1,DS0(HTable2),4},
	{0x67DCE8  ,DS (HTable),4},
	{0x4E68A6+1,DS0(&HTable[0].ArmyNum),4},
	{0x4E69A6+2,DS0(&HTable[HERNUM].ArmyNum),4},
/*
	{0x40247D+1,DS0(0x5534),2},{0x40247D+3,DS0(HMTable),4},{0x40315B+1,DS0(0x553C),2},{0x40315B+3,DS0(HMTable),4},
	{0x4035DF+1,DS0(0x551C),2},{0x4035DF+3,DS0(HMTable),4},{0x40625B+1,DS0(0x450C),2},{0x40625B+3,DS0(HMTable),4},
	{0x406284+1,DS0(0x450C),2},{0x406284+3,DS0(HMTable),4},{0x4062A6+1,DS0(0x450C),2},{0x4062A6+3,DS0(HMTable),4},
	{0x4062BB+1,DS0(0x450C),2},{0x4062BB+3,DS0(HMTable),4},{0x407B19+1,DS0(0x4534),2},{0x407B19+3,DS0(HMTable),4},
	{0x407FFA+1,DS0(0x4504),2},{0x407FFA+3,DS0(HMTable),4},{0x4080EC+1,DS0(0x4D0C),2},{0x4080EC+3,DS0(HMTable),4},
	{0x408BFA+1,DS0(0x453C),2},{0x408BFA+3,DS0(HMTable),4},{0x408E2B+1,DS0(0x453C),2},{0x408E2B+3,DS0(HMTable),4},
	{0x408EA2+1,DS0(0x4504),2},{0x408EA2+3,DS0(HMTable),4},{0x409097+1,DS0(0x550C),2},{0x409097+3,DS0(HMTable),4},
	{0x409261+1,DS0(0x4504),2},{0x409261+3,DS0(HMTable),4},{0x409A8A+1,DS0(0x4504),2},{0x409A8A+3,DS0(HMTable),4},
	{0x409B7C+1,DS0(0x4504),2},{0x409B7C+3,DS0(HMTable),4},{0x40A7F4+1,DS0(0x451C),2},{0x40A7F4+3,DS0(HMTable),4},
	{0x40A9C9+1,DS0(0x4504),2},{0x40A9C9+3,DS0(HMTable),4},{0x40AF7C+1,DS0(0x4D0C),2},{0x40AF7C+3,DS0(HMTable),4},
	{0x40AFF5+1,DS0(0x4D0C),2},{0x40AFF5+3,DS0(HMTable),4},{0x40B126+1,DS0(0x5534),2},{0x40B126+3,DS0(HMTable),4},
	{0x40BBD4+1,DS0(0x4534),2},{0x40BBD4+3,DS0(HMTable),4},{0x40DF86+1,DS0(0x4504),2},{0x40DF86+3,DS0(HMTable),4},
	{0x40E0F4+1,DS0(0x553C),2},{0x40E0F4+3,DS0(HMTable),4},{0x40E4AD+1,DS0(0x4504),2},{0x40E4AD+3,DS0(HMTable),4},
	{0x40E583+1,DS0(0x4514),2},{0x40E583+3,DS0(HMTable),4},{0x40E607+1,DS0(0x4D3C),2},{0x40E607+3,DS0(HMTable),4},
	{0x40E9A4+1,DS0(0x5504),2},{0x40E9A4+3,DS0(HMTable),4},{0x40EC31+1,DS0(0x453C),2},{0x40EC31+3,DS0(HMTable),4},
	{0x40F10B+1,DS0(0x4D0C),2},{0x40F10B+3,DS0(HMTable),4},{0x40FCA8+1,DS0(0x4D0C),2},{0x40FCA8+3,DS0(HMTable),4},
	{0x40FDBA+1,DS0(0x4D34),2},{0x40FDBA+3,DS0(HMTable),4},{0x41024A+1,DS0(0x4D34),2},{0x41024A+3,DS0(HMTable),4},
	{0x412CE0+1,DS0(0x4504),2},{0x412CE0+3,DS0(HMTable),4},{0x413784+1,DS0(0x4514),2},{0x413784+3,DS0(HMTable),4},
	{0x414555+1,DS0(0x4534),2},{0x414555+3,DS0(HMTable),4},{0x41652D+1,DS0(0x4534),2},{0x41652D+3,DS0(HMTable),4},
	{0x4165BA+1,DS0(0x551C),2},{0x4165BA+3,DS0(HMTable),4},{0x4167DF+1,DS0(0x551C),2},{0x4167DF+3,DS0(HMTable),4},
	{0x4168D0+1,DS0(0x4D04),2},{0x4168D0+3,DS0(HMTable),4},{0x416FAB+1,DS0(0x551C),2},{0x416FAB+3,DS0(HMTable),4},
	{0x41711D+1,DS0(0x4D34),2},{0x41711D+3,DS0(HMTable),4},{0x41763E+1,DS0(0x4D3C),2},{0x41763E+3,DS0(HMTable),4},
	{0x417B74+1,DS0(0x4D3C),2},{0x417B74+3,DS0(HMTable),4},{0x418DE1+1,DS0(0x451C),2},{0x418DE1+3,DS0(HMTable),4},
	{0x418E4C+1,DS0(0x4D04),2},{0x418E4C+3,DS0(HMTable),4},{0x419182+1,DS0(0x5504),2},{0x419182+3,DS0(HMTable),4},
	{0x4192D4+1,DS0(0x4D04),2},{0x4192D4+3,DS0(HMTable),4},{0x41943D+1,DS0(0x4D04),2},{0x41943D+3,DS0(HMTable),4},
	{0x4199F5+1,DS0(0x450C),2},{0x4199F5+3,DS0(HMTable),4},{0x41C4EE+1,DS0(0x5534),2},{0x41C4EE+3,DS0(HMTable),4},
	{0x41C63C+1,DS0(0x4D34),2},{0x41C63C+3,DS0(HMTable),4},{0x41C6B2+1,DS0(0x5534),2},{0x41C6B2+3,DS0(HMTable),4},
	{0x41C7A4+1,DS0(0x4D34),2},{0x41C7A4+3,DS0(HMTable),4},{0x41C862+1,DS0(0x5534),2},{0x41C862+3,DS0(HMTable),4},
	{0x41C943+1,DS0(0x4534),2},{0x41C943+3,DS0(HMTable),4},{0x41CA56+1,DS0(0x451C),2},{0x41CA56+3,DS0(HMTable),4},
	{0x41D034+1,DS0(0x4534),2},{0x41D034+3,DS0(HMTable),4},{0x41D259+1,DS0(0x5534),2},{0x41D259+3,DS0(HMTable),4},
	{0x41D544+1,DS0(0x5534),2},{0x41D544+3,DS0(HMTable),4},{0x428175+1,DS0(0x4D34),2},{0x428175+3,DS0(HMTable),4},
	{0x429807+1,DS0(0x5504),2},{0x429807+3,DS0(HMTable),4},{0x42986C+1,DS0(0x5504),2},{0x42986C+3,DS0(HMTable),4},
	{0x42B13F+1,DS0(0x4504),2},{0x42B13F+3,DS0(HMTable),4},{0x42BB00+1,DS0(0x451C),2},{0x42BB00+3,DS0(HMTable),4},
	{0x42DEE8+1,DS0(0x4534),2},{0x42DEE8+3,DS0(HMTable),4},{0x42EFB6+1,DS0(0x4504),2},{0x42EFB6+3,DS0(HMTable),4},
	{0x42F6F7+1,DS0(0x4534),2},{0x42F6F7+3,DS0(HMTable),4},{0x43145A+1,DS0(0x4504),2},{0x43145A+3,DS0(HMTable),4},
	{0x4315CA+1,DS0(0x5504),2},{0x4315CA+3,DS0(HMTable),4},{0x431633+1,DS0(0x550C),2},{0x431633+3,DS0(HMTable),4},
	{0x4316EA+1,DS0(0x450C),2},{0x4316EA+3,DS0(HMTable),4},{0x4317EB+1,DS0(0x4504),2},{0x4317EB+3,DS0(HMTable),4},
	{0x431EE9+1,DS0(0x4D04),2},{0x431EE9+3,DS0(HMTable),4},{0x432048+1,DS0(0x550C),2},{0x432048+3,DS0(HMTable),4},
	{0x4331A3+1,DS0(0x4504),2},{0x4331A3+3,DS0(HMTable),4},{0x433B02+1,DS0(0x4D0C),2},{0x433B02+3,DS0(HMTable),4},
	{0x4518D1+1,DS0(0x451C),2},{0x4518D1+3,DS0(HMTable),4},{0x453193+1,DS0(0x4D0C),2},{0x453193+3,DS0(HMTable),4},
	{0x4534D6+1,DS0(0x450C),2},{0x4534D6+3,DS0(HMTable),4},{0x453552+1,DS0(0x4D04),2},{0x453552+3,DS0(HMTable),4},
	{0x47F11D+1,DS0(0x4D0C),2},{0x47F11D+3,DS0(HMTable),4},{0x47F1DB+1,DS0(0x453C),2},{0x47F1DB+3,DS0(HMTable),4},
	{0x47F4B9+1,DS0(0x451C),2},{0x47F4B9+3,DS0(HMTable),4},{0x47F69A+1,DS0(0x553C),2},{0x47F69A+3,DS0(HMTable),4},
	{0x47FB87+1,DS0(0x4D0C),2},{0x47FB87+3,DS0(HMTable),4},{0x47FF58+1,DS0(0x4534),2},{0x47FF58+3,DS0(HMTable),4},
	{0x4801BA+1,DS0(0x4D0C),2},{0x4801BA+3,DS0(HMTable),4},{0x4803CA+1,DS0(0x4D0C),2},{0x4803CA+3,DS0(HMTable),4},
	{0x480644+1,DS0(0x4D0C),2},{0x480644+3,DS0(HMTable),4},{0x4806D2+1,DS0(0x4504),2},{0x4806D2+3,DS0(HMTable),4},
	{0x480B0A+1,DS0(0x4D0C),2},{0x480B0A+3,DS0(HMTable),4},{0x480FFA+1,DS0(0x450C),2},{0x480FFA+3,DS0(HMTable),4},
	{0x48124A+1,DS0(0x4D34),2},{0x48124A+3,DS0(HMTable),4},{0x4812E5+1,DS0(0x550C),2},{0x4812E5+3,DS0(HMTable),4},
	{0x4814CC+1,DS0(0x4D04),2},{0x4814CC+3,DS0(HMTable),4},{0x4817CA+1,DS0(0x5504),2},{0x4817CA+3,DS0(HMTable),4},
	{0x48181C+1,DS0(0x453C),2},{0x48181C+3,DS0(HMTable),4},{0x48190E+1,DS0(0x4D0C),2},{0x48190E+3,DS0(HMTable),4},
	{0x48197E+1,DS0(0x4504),2},{0x48197E+3,DS0(HMTable),4},{0x481AAC+1,DS0(0x551C),2},{0x481AAC+3,DS0(HMTable),4},
	{0x483C46+1,DS0(0x4D0C),2},{0x483C46+3,DS0(HMTable),4},{0x483C89+1,DS0(0x5504),2},{0x483C89+3,DS0(HMTable),4},
	{0x483CCE+1,DS0(0x5534),2},{0x483CCE+3,DS0(HMTable),4},{0x483F8B+1,DS0(0x4504),2},{0x483F8B+3,DS0(HMTable),4},
	{0x4860E8+1,DS0(0x451C),2},{0x4860E8+3,DS0(HMTable),4},{0x486BB0+1,DS0(0x5534),2},{0x486BB0+3,DS0(HMTable),4},
	{0x487652+1,DS0(0x550C),2},{0x487652+3,DS0(HMTable),4},{0x4896AC+1,DS0(0x4504),2},{0x4896AC+3,DS0(HMTable),4},
	{0x48971B+1,DS0(0x5504),2},{0x48971B+3,DS0(HMTable),4},{0x49A2EA+1,DS0(0x4D04),2},{0x49A2EA+3,DS0(HMTable),4},
	{0x49B0DD+1,DS0(0x4D04),2},{0x49B0DD+3,DS0(HMTable),4},{0x49D446+1,DS0(0x4D04),2},{0x49D446+3,DS0(HMTable),4},
	{0x49D4AC+1,DS0(0x5504),2},{0x49D4AC+3,DS0(HMTable),4},{0x49E03F+1,DS0(0x5504),2},{0x49E03F+3,DS0(HMTable),4},
	{0x49E088+1,DS0(0x550C),2},{0x49E088+3,DS0(HMTable),4},{0x4A249F+1,DS0(0x4D34),2},{0x4A249F+3,DS0(HMTable),4},
	{0x4A3CAC+1,DS0(0x5534),2},{0x4A3CAC+3,DS0(HMTable),4},{0x4AA735+1,DS0(0x4D3C),2},{0x4AA735+3,DS0(HMTable),4},
	{0x4AAD33+1,DS0(0x4D14),2},{0x4AAD33+3,DS0(HMTable),4},{0x4AAE0C+1,DS0(0x551C),2},{0x4AAE0C+3,DS0(HMTable),4},
	{0x4AAF6D+1,DS0(0x551C),2},{0x4AAF6D+3,DS0(HMTable),4},{0x4AAFFB+1,DS0(0x551C),2},{0x4AAFFB+3,DS0(HMTable),4},
	{0x4B262B+1,DS0(0x4504),2},{0x4B262B+3,DS0(HMTable),4},{0x4B9CCB+1,DS0(0x453C),2},{0x4B9CCB+3,DS0(HMTable),4},
	{0x4BA767+1,DS0(0x5534),2},{0x4BA767+3,DS0(HMTable),4},{0x4BA7BB+1,DS0(0x4D34),2},{0x4BA7BB+3,DS0(HMTable),4},
	{0x4BA8C6+1,DS0(0x4514),2},{0x4BA8C6+3,DS0(HMTable),4},{0x4BA939+1,DS0(0x5514),2},{0x4BA939+3,DS0(HMTable),4},
	{0x4BA99F+1,DS0(0x450C),2},{0x4BA99F+3,DS0(HMTable),4},{0x4BAA05+1,DS0(0x4D0C),2},{0x4BAA05+3,DS0(HMTable),4},
	{0x4BC922+1,DS0(0x451C),2},{0x4BC922+3,DS0(HMTable),4},{0x4BFDA7+1,DS0(0x4504),2},{0x4BFDA7+3,DS0(HMTable),4},
	{0x4BFF35+1,DS0(0x4D0C),2},{0x4BFF35+3,DS0(HMTable),4},{0x4C0044+1,DS0(0x4534),2},{0x4C0044+3,DS0(HMTable),4},
	{0x4C676F+1,DS0(0x550C),2},{0x4C676F+3,DS0(HMTable),4},{0x4C709C+1,DS0(0x4D3C),2},{0x4C709C+3,DS0(HMTable),4},
	{0x4C70EF+1,DS0(0x5534),2},{0x4C70EF+3,DS0(HMTable),4},{0x4C7160+1,DS0(0x4534),2},{0x4C7160+3,DS0(HMTable),4},
	{0x4C76D6+1,DS0(0x550C),2},{0x4C76D6+3,DS0(HMTable),4},{0x4C7767+1,DS0(0x4D0C),2},{0x4C7767+3,DS0(HMTable),4},
	{0x4C8036+1,DS0(0x4D34),2},{0x4C8036+3,DS0(HMTable),4},{0x4C809E+1,DS0(0x5534),2},{0x4C809E+3,DS0(HMTable),4},
	{0x4C8178+1,DS0(0x4504),2},{0x4C8178+3,DS0(HMTable),4},{0x4C81C2+1,DS0(0x4504),2},{0x4C81C2+3,DS0(HMTable),4},
	{0x4C820A+1,DS0(0x4504),2},{0x4C820A+3,DS0(HMTable),4},{0x4C8247+1,DS0(0x4504),2},{0x4C8247+3,DS0(HMTable),4},
	{0x4C82A5+1,DS0(0x553C),2},{0x4C82A5+3,DS0(HMTable),4},{0x4C839F+1,DS0(0x4D04),2},{0x4C839F+3,DS0(HMTable),4},
	{0x4C8830+1,DS0(0x551C),2},{0x4C8830+3,DS0(HMTable),4},{0x4C9EA5+1,DS0(0x550C),2},{0x4C9EA5+3,DS0(HMTable),4},
	{0x4CAB9E+1,DS0(0x4D34),2},{0x4CAB9E+3,DS0(HMTable),4},{0x4CAC54+1,DS0(0x450C),2},{0x4CAC54+3,DS0(HMTable),4},
	{0x4DE57C+1,DS0(0x4504),2},{0x4DE57C+3,DS0(HMTable),4},{0x4DE61A+1,DS0(0x550C),2},{0x4DE61A+3,DS0(HMTable),4},
	{0x4E18C5+1,DS0(0x4504),2},{0x4E18C5+3,DS0(HMTable),4},{0x4E1AD7+1,DS0(0x4514),2},{0x4E1AD7+3,DS0(HMTable),4},
	{0x4E7715+1,DS0(0x4D0C),2},{0x4E7715+3,DS0(HMTable),4},{0x4E7FB5+1,DS0(0x4D14),2},{0x4E7FB5+3,DS0(HMTable),4},
	{0x4E86D8+1,DS0(0x550C),2},{0x4E86D8+3,DS0(HMTable),4},{0x4E87BC+1,DS0(0x4D0C),2},{0x4E87BC+3,DS0(HMTable),4},
	{0x4E87D6+1,DS0(0x553C),2},{0x4E87D6+3,DS0(HMTable),4},{0x4E8ABF+1,DS0(0x4D0C),2},{0x4E8ABF+3,DS0(HMTable),4},
	{0x4E8AD9+1,DS0(0x553C),2},{0x4E8AD9+3,DS0(HMTable),4},{0x4F1A4B+1,DS0(0x450C),2},{0x4F1A4B+3,DS0(HMTable),4},
	{0x4F26A6+1,DS0(0x5504),2},{0x4F26A6+3,DS0(HMTable),4},{0x4F313A+1,DS0(0x5504),2},{0x4F313A+3,DS0(HMTable),4},
	{0x4F318F+1,DS0(0x4D0C),2},{0x4F318F+3,DS0(HMTable),4},{0x4F4A11+1,DS0(0x4D1C),2},{0x4F4A11+3,DS0(HMTable),4},
	{0x4F53B3+1,DS0(0x4D34),2},{0x4F53B3+3,DS0(HMTable),4},{0x4F540C+1,DS0(0x5534),2},{0x4F540C+3,DS0(HMTable),4},
	{0x4F5481+1,DS0(0x5534),2},{0x4F5481+3,DS0(HMTable),4},{0x4FD243+1,DS0(0x4504),2},{0x4FD243+3,DS0(HMTable),4},
	{0x4FD2A5+1,DS0(0x4504),2},{0x4FD2A5+3,DS0(HMTable),4},{0x4FD2F3+1,DS0(0x4504),2},{0x4FD2F3+3,DS0(HMTable),4},
	{0x4FD37E+1,DS0(0x4504),2},{0x4FD37E+3,DS0(HMTable),4},{0x4FD4A1+1,DS0(0x4504),2},{0x4FD4A1+3,DS0(HMTable),4},
	{0x50609B+1,DS0(0x4504),2},{0x50609B+3,DS0(HMTable),4},{0x5060EC+1,DS0(0x4D1C),2},{0x5060EC+3,DS0(HMTable),4},
	{0x513AC9+1,DS0(0x4504),2},{0x513AC9+3,DS0(HMTable),4},{0x513CE7+1,DS0(0x4504),2},{0x513CE7+3,DS0(HMTable),4},
	{0x51C90D+1,DS0(0x4D34),2},{0x51C90D+3,DS0(HMTable),4},{0x51CBB3+1,DS0(0x4534),2},{0x51CBB3+3,DS0(HMTable),4},
	{0x51D5B6+1,DS0(0x4514),2},{0x51D5B6+3,DS0(HMTable),4},{0x51F3C0+1,DS0(0x5534),2},{0x51F3C0+3,DS0(HMTable),4},
	{0x51F687+1,DS0(0x450C),2},{0x51F687+3,DS0(HMTable),4},{0x51FA10+1,DS0(0x450C),2},{0x51FA10+3,DS0(HMTable),4},
	{0x51FAAD+1,DS0(0x4504),2},{0x51FAAD+3,DS0(HMTable),4},{0x51FB29+1,DS0(0x5504),2},{0x51FB29+3,DS0(HMTable),4},
	{0x52137B+1,DS0(0x5534),2},{0x52137B+3,DS0(HMTable),4},{0x52163E+1,DS0(0x450C),2},{0x52163E+3,DS0(HMTable),4},
	{0x521741+1,DS0(0x5534),2},{0x521741+3,DS0(HMTable),4},{0x5217A9+1,DS0(0x4534),2},{0x5217A9+3,DS0(HMTable),4},
	{0x52189C+1,DS0(0x4504),2},{0x52189C+3,DS0(HMTable),4},{0x5218D4+1,DS0(0x4504),2},{0x5218D4+3,DS0(HMTable),4},
	{0x52210B+1,DS0(0x550C),2},{0x52210B+3,DS0(HMTable),4},{0x5222BD+1,DS0(0x450C),2},{0x5222BD+3,DS0(HMTable),4},
	{0x52297F+1,DS0(0x4D04),2},{0x52297F+3,DS0(HMTable),4},{0x522AD7+1,DS0(0x4D0C),2},{0x522AD7+3,DS0(HMTable),4},
	{0x525741+1,DS0(0x5504),2},{0x525741+3,DS0(HMTable),4},{0x5257FB+1,DS0(0x4534),2},{0x5257FB+3,DS0(HMTable),4},
	{0x525950+1,DS0(0x4D04),2},{0x525950+3,DS0(HMTable),4},{0x525E87+1,DS0(0x4D04),2},{0x525E87+3,DS0(HMTable),4},
	{0x526526+1,DS0(0x5514),2},{0x526526+3,DS0(HMTable),4},{0x5267F5+1,DS0(0x5504),2},{0x5267F5+3,DS0(HMTable),4},
	{0x526C00+1,DS0(0x4504),2},{0x526C00+3,DS0(HMTable),4},{0x526F42+1,DS0(0x4504),2},{0x526F42+3,DS0(HMTable),4},
	{0x526FC0+1,DS0(0x4534),2},{0x526FC0+3,DS0(HMTable),4},{0x5270E7+1,DS0(0x4534),2},{0x5270E7+3,DS0(HMTable),4},
	{0x527E84+1,DS0(0x450C),2},{0x527E84+3,DS0(HMTable),4},{0x52A1C9+1,DS0(0x4534),2},{0x52A1C9+3,DS0(HMTable),4},
	{0x52A8AD+1,DS0(0x4D04),2},{0x52A8AD+3,DS0(HMTable),4},{0x52B1A4+1,DS0(0x5514),2},{0x52B1A4+3,DS0(HMTable),4},
	{0x52B36F+1,DS0(0x551C),2},{0x52B36F+3,DS0(HMTable),4},{0x52B48F+1,DS0(0x4D1C),2},{0x52B48F+3,DS0(HMTable),4},
	{0x52B5D7+1,DS0(0x451C),2},{0x52B5D7+3,DS0(HMTable),4},{0x557096+1,DS0(0x5504),2},{0x557096+3,DS0(HMTable),4},
	{0x5683A6+1,DS0(0x4D1C),2},{0x5683A6+3,DS0(HMTable),4},{0x569DB0+1,DS0(0x4534),2},{0x569DB0+3,DS0(HMTable),4},
	{0x56A896+1,DS0(0x450C),2},{0x56A896+3,DS0(HMTable),4},{0x56E478+1,DS0(0x4504),2},{0x56E478+3,DS0(HMTable),4},
	{0x56E579+1,DS0(0x453C),2},{0x56E579+3,DS0(HMTable),4},{0x56E9A4+1,DS0(0x453C),2},{0x56E9A4+3,DS0(HMTable),4},
	{0x5721A8+1,DS0(0x4504),2},{0x5721A8+3,DS0(HMTable),4},{0x5722A9+1,DS0(0x453C),2},{0x5722A9+3,DS0(HMTable),4},
	{0x572471+1,DS0(0x4514),2},{0x572471+3,DS0(HMTable),4},{0x582654+1,DS0(0x551C),2},{0x582654+3,DS0(HMTable),4},
	{0x582866+1,DS0(0x451C),2},{0x582866+3,DS0(HMTable),4},{0x5AEEFB+1,DS0(0x550C),2},{0x5AEEFB+3,DS0(HMTable),4},
	{0x5AEF23+1,DS0(0x550C),2},{0x5AEF23+3,DS0(HMTable),4},{0x5BE40F+1,DS0(0x4504),2},{0x5BE40F+3,DS0(HMTable),4},
	{0x5BE4A8+1,DS0(0x550C),2},{0x5BE4A8+3,DS0(HMTable),4},{0x5BE689+1,DS0(0x4504),2},{0x5BE689+3,DS0(HMTable),4},
	{0x5BE7BE+1,DS0(0x4D0C),2},{0x5BE7BE+3,DS0(HMTable),4},{0x5BE7EF+1,DS0(0x550C),2},{0x5BE7EF+3,DS0(HMTable),4},
	{0x5BE87B+1,DS0(0x450C),2},{0x5BE87B+3,DS0(HMTable),4},{0x5BE89B+1,DS0(0x453C),2},{0x5BE89B+3,DS0(HMTable),4},
	{0x5BE9A6+1,DS0(0x4D0C),2},{0x5BE9A6+3,DS0(HMTable),4},{0x5BF5A7+1,DS0(0x4504),2},{0x5BF5A7+3,DS0(HMTable),4},
	{0x5BF5EE+1,DS0(0x5504),2},{0x5BF5EE+3,DS0(HMTable),4},{0x5BFD42+1,DS0(0x5504),2},{0x5BFD42+3,DS0(HMTable),4},
	{0x5BFD69+1,DS0(0x4D0C),2},{0x5BFD69+3,DS0(HMTable),4},{0x5BFE0F+1,DS0(0x450C),2},{0x5BFE0F+3,DS0(HMTable),4},
	{0x5C173D+1,DS0(0x4D0C),2},{0x5C173D+3,DS0(HMTable),4},{0x5C17D2+1,DS0(0x4514),2},{0x5C17D2+3,DS0(HMTable),4},
	{0x5C188B+1,DS0(0x4504),2},{0x5C188B+3,DS0(HMTable),4},{0x5C69E5+1,DS0(0x5504),2},{0x5C69E5+3,DS0(HMTable),4},
	{0x5C6A12+1,DS0(0x5504),2},{0x5C6A12+3,DS0(HMTable),4},{0x5C7253+1,DS0(0x4D3C),2},{0x5C7253+3,DS0(HMTable),4},
	{0x5C7291+1,DS0(0x5514),2},{0x5C7291+3,DS0(HMTable),4},{0x5C7373+1,DS0(0x553C),2},{0x5C7373+3,DS0(HMTable),4},
	{0x5C76A1+1,DS0(0x4504),2},{0x5C76A1+3,DS0(HMTable),4},{0x5C76C1+1,DS0(0x4504),2},{0x5C76C1+3,DS0(HMTable),4},
	{0x5C7E0A+1,DS0(0x4D04),2},{0x5C7E0A+3,DS0(HMTable),4},{0x5C7EED+1,DS0(0x4504),2},{0x5C7EED+3,DS0(HMTable),4},
	{0x5C9BED+1,DS0(0x4504),2},{0x5C9BED+3,DS0(HMTable),4},{0x5CE99D+1,DS0(0x453C),2},{0x5CE99D+3,DS0(HMTable),4},
	{0x5CE9C1+1,DS0(0x553C),2},{0x5CE9C1+3,DS0(HMTable),4},{0x5D1D62+1,DS0(0x4534),2},{0x5D1D62+3,DS0(HMTable),4},
	{0x5D2238+1,DS0(0x550C),2},{0x5D2238+3,DS0(HMTable),4},{0x5D31E6+1,DS0(0x4504),2},{0x5D31E6+3,DS0(HMTable),4},
	{0x5D3209+1,DS0(0x5504),2},{0x5D3209+3,DS0(HMTable),4},{0x5D3238+1,DS0(0x4504),2},{0x5D3238+3,DS0(HMTable),4},
	{0x5D33DD+1,DS0(0x4504),2},{0x5D33DD+3,DS0(HMTable),4},{0x5D34EF+1,DS0(0x4534),2},{0x5D34EF+3,DS0(HMTable),4},
	{0x5D3513+1,DS0(0x5534),2},{0x5D3513+3,DS0(HMTable),4},{0x5D4499+1,DS0(0x4D04),2},{0x5D4499+3,DS0(HMTable),4},
	{0x5D4516+1,DS0(0x4504),2},{0x5D4516+3,DS0(HMTable),4},{0x5D7DA9+1,DS0(0x4D04),2},{0x5D7DA9+3,DS0(HMTable),4},
	{0x5D7E84+1,DS0(0x5534),2},{0x5D7E84+3,DS0(HMTable),4},{0x5D80C3+1,DS0(0x450C),2},{0x5D80C3+3,DS0(HMTable),4},
	{0x5D843F+1,DS0(0x550C),2},{0x5D843F+3,DS0(HMTable),4},{0x5D8480+1,DS0(0x5504),2},{0x5D8480+3,DS0(HMTable),4},
	{0x5D84F3+1,DS0(0x4504),2},{0x5D84F3+3,DS0(HMTable),4},{0x5D853E+1,DS0(0x550C),2},{0x5D853E+3,DS0(HMTable),4},
	{0x5D8773+1,DS0(0x553C),2},{0x5D8773+3,DS0(HMTable),4},{0x5D8862+1,DS0(0x4504),2},{0x5D8862+3,DS0(HMTable),4},
	{0x5D89D6+1,DS0(0x4D0C),2},{0x5D89D6+3,DS0(HMTable),4},{0x5D8AAB+1,DS0(0x5504),2},{0x5D8AAB+3,DS0(HMTable),4},
	{0x5DE4AB+1,DS0(0x553C),2},{0x5DE4AB+3,DS0(HMTable),4},{0x5DE50D+1,DS0(0x4504),2},{0x5DE50D+3,DS0(HMTable),4},
	{0x5DEF1D+1,DS0(0x4504),2},{0x5DEF1D+3,DS0(HMTable),4},{0x5DF3DF+1,DS0(0x4534),2},{0x5DF3DF+3,DS0(HMTable),4},
	{0x5DF460+1,DS0(0x550C),2},{0x5DF460+3,DS0(HMTable),4},{0x5EA231+1,DS0(0x450C),2},{0x5EA231+3,DS0(HMTable),4},
	{0x5EA3AE+1,DS0(0x450C),2},{0x5EA3AE+3,DS0(HMTable),4},{0x5EA4A8+1,DS0(0x4D14),2},{0x5EA4A8+3,DS0(HMTable),4},
	{0x5F1B9D+1,DS0(0x4D3C),2},{0x5F1B9D+3,DS0(HMTable),4},{0x5F1D0B+1,DS0(0x4D0C),2},{0x5F1D0B+3,DS0(HMTable),4},
	{0x5F1DE1+1,DS0(0x453C),2},{0x5F1DE1+3,DS0(HMTable),4},{0x5F1F68+1,DS0(0x4504),2},{0x5F1F68+3,DS0(HMTable),4},
	{0x5F7835+1,DS0(0x5504),2},{0x5F7835+3,DS0(HMTable),4},{0x5F78DA+1,DS0(0x5534),2},{0x5F78DA+3,DS0(HMTable),4},
	{0x5F7CDA+1,DS0(0x5534),2},{0x5F7CDA+3,DS0(HMTable),4},

	{0x40E5FB+2,DS0(0x4D04),2},{0x40E5FB+4,DS0(&HMTable[0].y),4},
	{0x417B7B+2,DS0(0x4D0C),2},{0x417B7B+4,DS0(HMTable),4},
	{0x4199ED+2,DS0(0x4514),2},{0x4199ED+4,DS0(HMTable),4},
	{0x4814D3+1,DS0(0x4D04),2},{0x4814D3+3,DS0(&HMTable[0].RefData1),4},
	{0x483C3E+2,DS0(0x4D04),2},{0x483C3E+4,DS0(&HMTable[0].Owner),4},
//  {0x4B6638+2,DS0(0x45C3),2},{0x4B8638+4,DS0(&HMTable[0].Owner),4},
	{0x4B8AC4+1,DS0(0x3D),1},{0x4B8AC4+2,DS0(&HMTable[0].Owner),4},
	{0x4BB183+1,DS0(0x1D),1},{0x4BB183+2,DS0(&HMTable[0].Spec),4}, //BB183 Spec
	{0x4BB305+1,DS0(0x05),1},{0x4BB305+2,DS0(&HMTable[0].Spec),4}, //BB305 Spec
	{0x4BB49C+1,DS0(0x05),1},{0x4BB49C+2,DS0(&HMTable[0].Spec),4}, //BB49C Spec
	{0x4BD0AB+1,DS0(0x3D),1},{0x4BD0AB+2,DS0(HMTable),4},
	{0x4BE501+1,DS0(0x3D),1},{0x4BE501+2,DS0(HMTable),4},
	{0x4BEF6E+1,DS0(0x05),1},{0x4BEF6E+2,DS0(HMTable),4},
	{0x4BF5A6+2,DS0(0x05),1},{0x4BF5A6+3,DS0(&HMTable[0].y),4},
	{0x4BF860+1,DS0(0x05),1},{0x4BF860+2,DS0(&HMTable[0].y),4},
	{0x4BF946+2,DS0(0x4504),2},{0x4BF946+4,DS0(&HMTable[0].Owner),4},
	{0x4BFB47+1,DS0(0x35),1},{0x4BFB47+2,DS0(HMTable),4},
	{0x4C6D0F+1,DS0(0x4D04),2},{0x4C6D0F+3,DS0(&HMTable[0].RefData1),4},
	{0x4C6D16+1,DS0(0x4D0C),2},{0x4C6D16+3,DS0(&HMTable[0].RefData1),4},
	{0x4C7920+1,DS0(0x3D),1},{0x4C7920+2,DS0(&HMTable[0].Owner),4}, // ?
	{0x4C7D8F+1,DS0(0x05),1},{0x4C7D8F+2,DS0(&HMTable[0].RefData1),4}, 
	{0x4C7EF7+1,DS0(0x35),1},{0x4C7EF7+2,DS0(&HMTable[0].SpPoints),4},
	{0x4C83D4,DS0(0x90C003),3}, 
// не надо! {0x4C83DA+1,DS0(0x0D),1},
		{0x4C83DA+2,DS0(HMTable),4},
// не надо! {0x4C83E0,DS0(0xA0),1},
		{0x4C83E0+1,DS0(&HMTable[0].PSkill[3]),4},
// не надо! C8422
		{0x4C8422+3,DS0(&HMTable[0].SpPoints),4},
	{0x4C89DF+1,DS0(0x05),1},{0x4C89DF+2,DS0(&HMTable[0].RefData1),4},
	{0x4C9408+1,DS0(0x1C),1},
	{0x4C9415,DS0(0x90DB03),3}, 
// не надо! C941D
		{0x4C941D+2,DS0(&HMTable[0].Ct[0]),4},
// не надо! {0x4C94BD+1,DS0(0x0D),1},
		{0x4C94BD+2,DS0(HMTable),4},
// не надо! {0x4C94E3+1,DS0(0x0D),1},
		{0x4C94E3+2,DS0(HMTable),4},
	{0x4CD8E6+2,DS0(0x4504),2},{0x4CD8E6+4,DS0(&HMTable[0].Owner),4},
	{0x4CD9D2+2,DS0(0x4D0C),2},{0x4CD9D2+4,DS0(&HMTable[0].Owner),4},
	{0x4CDE34+1,DS0(0x05),1},{0x4CDE34+2,DS0(HMTable),4},
	{0x4CE4F9+1,DS0(0x15),1},{0x4CE4F9+2,DS0(HMTable),4},
	{0x4DA466+1,DS0(0x5D04),2},{0x4DA466+3,DS0(&HMTable[0].RefData2),4},
	{0x4F4ED8+1,DS0(0x4514),2},{0x4F4ED8+3,DS0(&HMTable[0].Ct[0]),4},
	{0x526FC7+1,DS0(0x4504),2},{0x526FC7+3,DS0(&HMTable[0].Movement),4},
	{0x58D579+1,DS0(0x5504),2},{0x58D579+3,DS0(&HMTable[0].Name[0]),4},
	{0x58D697+1,DS0(0x4504),2},{0x58D697+3,DS0(&HMTable[0].Name[0]),4},
	{0x5BE681+2,DS0(0x4514),2},{0x5BE681+4,DS0(&HMTable[0].Owner),4},
	{0x62C9C3,DS0(0xB8),1},{0x62C9C3+1,DS0(HMTable),4},
	{0x62CB5F,DS0(0xB8),1},{0x62CB5F+1,DS0(HMTable),4},
*/
/*
	{0x4BB1EC+2,DS0(HERNUM),4},{0x4BB252+2,DS0(HERNUM),4},{0x4BB36F+2,DS0(HERNUM),4},
	{0x4BB500+2,DS0(HERNUM),4},{0x4BD1CB+3,DS0(HERNUM),4},{0x4BE51D+1,DS0(HERNUM),4},
	{0x4BE597+3,DS0(HERNUM),4},{0x4BEF9A+2,DS0(HERNUM),4},{0x4BF61E+2,DS0(HERNUM),4},
	{0x4BF8D8+2,DS0(HERNUM),4},{0x4BFBA1+2,DS0(HERNUM),4}, // 4C2CCF 4CAADB инициализация из _THero_
	{0x4C81DE+2,DS0(HERNUM),4},{0x4C8222+2,DS0(HERNUM),4},{0x4C825F+2,DS0(HERNUM),4},
	{0x4C89E5+1,DS0(HERNUM),4},{0x4CDE53+3,DS0(HERNUM),4},
*/
// CastleManager
	{0x5BF444,DS0(0x0C985),2}, // перерисовка на карте внешнего вида
	{0x5BF448,DS0(0x90C032),3}, // перерисовка на карте внешнего вида
	{0x5BF45A,DS0(0x0C985),2}, // перерисовка на карте внешнего вида
// Magic
	{0x4963E7+2,DS0(MAGICS),1},
	{0x43F77B+3,DS (MagicAni),4},
	{0x43FB67+3,DS0(MagicAni),4},
	{0x4963F9+2,DS0(MagicAni),4},
	{0x4965CD+2,DS0(MagicAni),4},
	{0x5A5033+3,DS0(MagicAni),4},
	{0x5A6B11+3,DS0(MagicAni),4},
	{0x5A7A71+3,DS0(MagicAni),4},
	{0x5A9629+3,DS0(MagicAni),4},
	{0x496518+2,DS0(&MagicAni[0].Name),4},
	{0x4689C1+3,DS0(&MagicAni[0].Name),4},
	{0x4966CB+2,DS0(&MagicAni[0].Name),4},
	{0x5A6D2A+3,DS0(&MagicAni[0].Name),4},
	{0x5A7B03+3,DS0(&MagicAni[0].Name),4},
	{0x43E500+3,DS0(&MagicAni[0].Type),4},
// L/N G/C (icon)
////  {0x4579F5+2,0x0B,1}, // NG
////  {0x45C912+2,0x0B,1}, // LG
////  {0x5135C3+2,0x0B,1}, // NC
////  {0x578820+2,0x0B,1}, //
	{0x4C415E,DS0(0x73),1}, // переходы (>=0x1C, а не =0x1C)
	{0x513791,DS0(0x73),1}, //
	{0x5836DF,DS0(0x73),1}, //
	{0x585BD7,DS0(0x73),1}, //
	{0x58C168,DS0(0x73),1}, //
	{0x502BA3,DS0(0x930F),2}, // при загрузке монстров заставляет добавлять 1 байт

// Dwellings
	{0x418751+3,DS (DwMonster),4}, //
	{0x4B85A0+3,DS0(DwMonster),4}, // 
	{0x5032D6+2,DS0(DwMonster),4}, //
	{0x5032C7+1,DS0(&DwMonster[DWNUM-1]),4}, //
	{0x5032BB+1,DS0(DWNUM-1),4}, // 
	{0x521A84+3,DS0(DwMonster),4}, // 
	{0x521A7F+2,DS0(DWNUM),1}, //
	{0x534CE7+3,DS0(DwMonster),4}, // 
	{0x405CD8+2,DS0(&DwName[-1]),4}, // 
	{0x405CCE+2,DS0(DWNUM*4),4}, //
	{0x40B514+3,DS (DwName),4}, // 
	{0x40B51E+3,DS0(DwName),4}, // 
	{0x413E6A+3,DS0(DwName),4}, // 
	{0x413E75+3,DS0(DwName),4}, //
	{0x4A1550+3,DS0(DwName),4}, // 
//{(long)&DwMonster[DWNUM_0+1],DS0(133),4},
	{0x447473+2,DS0(MoJumper1),4}, // книги заклинаний на монстре показ
	{0x447479+3,DS0(MoJmpAdr1),4}, //Diakon адреса переходов
	{0x492A56+2,DS0(-2),1}, // Diakon
	{0x492A61+2,DS0(MagHint),4}, //Diakon
	{0x492A67+3,DS0(MagHintJmp),4}, //Diakon
	{0x447468+2,DS0(MONNUM-13),1}, // книги заклинаний на монстре показ
	{0x4476FC  ,DS0(0x044748E),4}, // книги заклинаний на монстре показ
	{0x44825D+2,DS0(MoJumper2),4}, // магия монстра
	{0x448263+3,DS0(MagJmp),4}, //Diakon
	{0x44824C+2,DS0(MONNUM-13),1}, // магия монстра
	{0x440914+2,DS0(MoJumper3),4}, // автомагия во время атаки монстра
	{0x440909+2,DS0(MONNUM-63),1}, // автомагия во время атаки монстра
	{0x440235+2,DS0(MoJumper4),4}, // магия монстра после атаки монстра
	{0x44023B+3,DS0(MoJumper4po),4}, // магия монстра после атаки монстра
	{0x44022E+2,DS0(MONNUM-22),1}, // магия монстра после атаки монстра
	{0x43DA9D+2,DS0(MoJumper5),4}, // подгрузка стреляющего оружия
	{0x43DA94+1,DS0(MONNUM-2),4}, // подгрузка стреляющего оружия
	{0x44A4B1+2,DS0(MoJumper6),4}, // возможность действия магии на монстра
	{0x44A4AA+2,DS0(MONNUM-16),1}, // возможность действия магии на монстра
	{0x421477+2,DS0(AIMagicJumper),4}, // AI смотрит может ли магией атаковать
	{0x42147D+3,DS0(AIJmpAdr),4},
	{0x421470+2,DS0(MONNUM-13),1}, //
	{0x4F63EB,DS0(0),1}, // корректный показ мифрила
//  {0x4B8130,DS0((256+21)*4),4}, // слухи в таверне 256 - станд.
// Обработка Мифрила в LE
	{0x49FF98+2,DS0(8),1}, // корректный счет мифрила
// 3.58 fix Arrow Tower + Armorer bug
	{0x465944,DS0(0x4D),1}, // fmul instead of fdiv
// Скелетон Трансформер
	{0x5664B5+3,DS (SkelTrans),4},
	{0x56689D+3,DS (SkelTrans),4},
	{0x566D79+3,DS (SkelTrans),4},
	{0x566F3C+3,DS (SkelTrans),4},
	{0x566FA4+3,DS (SkelTrans),4},
	{0x566FD1+3,DS (SkelTrans),4},
// fix с наймом АИ монстров в замке, если они >145
	{0x428885+1,DS (MONNUM*12),4},
	{0x4288C4+2,DS (MONNUM*12),4},
	{0x4288A3+3,DS (MONNUM),4},
	{0x4288E3+1,DS (MONNUM),4},
// показ инфы о стэке на поле боя по правой кнопке с кнопкой Ок
//  {0x476435+1,DS (0),1},
// возможность апгрейда существ в городе, если заменен тип
	{0x4C698F,DS (0xC03B),2},
// перенос таблицы настроек Combo артифактов
	{0x660B6C,DS (CArtSetup),4},
//    {0x4865C6+2,DS(160),4}, // ???? AI think about art
//    {0x486664+2,DS(160),4}, // ???? AI think about art
//    {0x4875EE+2,DS(160),4}, // ???? AI think about art
	{0x48E5AD+2,DS(160),4}, 
	{0x4CF67A+2,DS(160),4}, 
	{0x4C2277+2,DS(160),4}, 
	{0x4DC319+2,DS(160),4}, 
	{0x4E33DC+2,DS (32),1},
	{0x4E34B1+2,DS (32),1},
	{0x4E353D+2,DS (32),1},
	{0x4DC3C4+2,DS (32),1},
	{0x4DC418+2,DS (32),1},
	{0x4DC127+2,DS(160),4}, 
	{0x4DC49E+2,DS (32),1},
	{0x4DC1DA+2,DS(160),4}, 
	{0x4DC240+2,DS(160),4},
	{0x4DC26A+2,DS(160),4},
//  {0x44C549+2,DS(ARTNUM),4},
	{0x44C549+2,DS(160),4},
	{0x4DDFA4+2,DS(160),4}, // only first 160 arts may be a part of combo
	{0x44D1A8+2,DS(160),4}, // only first 160 arts may be a part of combo
	{0x4E2A5C+2,DS(160*32),4}, // only first 160 arts may be a part of combo
	{0x4E2967+2,DS(160*32),4}, // only first 160 arts may be a part of combo
	// расширение
{(long)&DwMonster[DWNUM_0+ 0],DS0(150),4},  // 80
{(long)&DwMonster[DWNUM_0+ 1],DS0(151),4},  // 81
{(long)&DwMonster[DWNUM_0+ 2],DS0(152),4},  // 82
{(long)&DwMonster[DWNUM_0+ 3],DS0(153),4},  // 83
{(long)&DwMonster[DWNUM_0+ 4],DS0(154),4},  // 84
{(long)&DwMonster[DWNUM_0+ 5],DS0(155),4},  // 85
{(long)&DwMonster[DWNUM_0+ 6],DS0(156),4},  // 86
{(long)&DwMonster[DWNUM_0+ 7],DS0(157),4},  // 87
{(long)&DwMonster[DWNUM_0+ 8],DS0(158),4},  // 88
{(long)&DwMonster[DWNUM_0+ 9],DS0(171),4},  // 89
{(long)&DwMonster[DWNUM_0+10],DS0(170),4},  // 90
{(long)&DwMonster[DWNUM_0+11],DS0(168),4},  // 91
{(long)&DwMonster[DWNUM_0+12],DS0(172),4},  // 92 nightmare
{(long)&DwMonster[DWNUM_0+13],DS0(164),4},  // 93 golems (!!! must be corrected later down)
{(long)&DwMonster[DWNUM_0+14],DS0(169),4},  // 94 красный монах
{(long)&DwMonster[DWNUM_0+15],DS0(173),4},  // 95 SG
// 3.58
{(long)&DwMonster[DWNUM_0+16],DS0(192),4},  // 96 Sylvan Centour
{(long)&DwMonster[DWNUM_0+17],DS0(193),4},  // 97 Sorceress
{(long)&DwMonster[DWNUM_0+18],DS0(194),4},  // 98 Werewolf
{(long)&DwMonster[DWNUM_0+19],DS0(195),4},  // 99 Fire Horse
{(long)&DwMonster[DWNUM_0+20],DS0(196),4},  // 100 Dracolich
// коррекция вывода картинок в экране Kingdom Overview
	{0x681860,DS('Z'),1}, // "ZlagPort.def"
	{0x520ECF+2,DS(DWNUM+3),1}, // Horn
	{0x520EFC+3,DS(DWNUM+3),4}, 
	{0x520F2C+2,DS(DWNUM+4),1}, // Horn Anti-Magic
	{0x520F55+3,DS(DWNUM+4),4}, 
	{0x520FF9+2,DS(DWNUM+7),1}, // Shypyard
	{0x52103A+3,DS(DWNUM+7),4}, 
	{0x520C76+2,DS(DWNUM+5),1}, // Abandoned Mine
	{0x520CA7+3,DS(DWNUM+5),4}, 
	{0x520CE9+2,DS(DWNUM+2),1}, // Lighthouse
	{0x520D1A+3,DS(DWNUM+2),4}, 
//  {0x520ECF+2,DS(DWNUM-5),1}, // Horn
//  {0x520EFC+3,DS(DWNUM-5),4}, 

// 3.59
	// new town support (!!! will be remade in Lua)
//	{0x5C0359+3,DS(AllEnBuildings),4}, 
//	{0x5C0360+3,DS(AllEnBuildings)+4,4}, 
//	{0x5C3836+3,DS(AllEnBuildings),4}, 
//	{0x5C383F+3,DS(AllEnBuildings)+4,4}, 
//	{0x5D3749+3,DS(AllEnBuildings),4}, 
//	{0x5D3753+3,DS(AllEnBuildings)+4,4}, 
//	{0x5D378A+3,DS(AllEnBuildings),4}, 
//	{0x5D3793+3,DS(AllEnBuildings)+4,4}, 
//	{0x5C0E9C+3,DS(AllEnBuildings),4}, 
//	{0x5C0EA3+3,DS(AllEnBuildings)+4,4}, 
//	{0x5C0FE8+3,DS(AllEnBuildings),4}, 
//	{0x5C0FF1+3,DS(AllEnBuildings)+4,4}, 
//	{0x5C0D72+3,DS(AllEnBuildings),4}, 
//	{0x5C0D81+3,DS(AllEnBuildings)+4,4}, 
//
//	{0x5C11D7+3,DS(BuildDepends),4},
//	{0x5C11DE+3,DS(BuildDepends)+4,4},
//	{0x42B281+3,DS(BuildDepends),4},
//	{0x42B290+3,DS(BuildDepends)+4,4},
//	{0x5C12E5+3,DS(BuildDepends),4},
//	{0x5C12EC+3,DS(BuildDepends)+4,4},
//	{0x5C1377+3,DS(BuildDepends),4},
//	{0x5C137E+3,DS(BuildDepends)+4,4},
//	{0x5D6015+3,DS(BuildDepends),4},
//	{0x5D601C+3,DS(BuildDepends)+4,4},
//
//	{0x5BED7F+3,DS(BuildExclusions),4},
//	{0x5BED88+3,DS(BuildExclusions)+4,4},
//	{0x5C0F07+3,DS(BuildExclusions),4},
//	{0x5C0F18+3,DS(BuildExclusions)+4,4},
//	{0x5C0D0E+3,DS(BuildExclusions),4},
//	{0x5C0D15+3,DS(BuildExclusions)+4,4},
//	{0x5BF2E3+3,DS(BuildExclusions),4},
//	{0x5BF2EA+3,DS(BuildExclusions)+4,4},
//	{0x5BF782+3,DS(BuildExclusions),4},
//	{0x5BF789+3,DS(BuildExclusions)+4,4},
//	{0x5C03A4+3,DS(BuildExclusions),4},
//	{0x5C03AB+3,DS(BuildExclusions)+4,4},
//	{0x5C0C64+3,DS(BuildExclusions),4},
//	{0x5C0C6B+3,DS(BuildExclusions)+4,4},
////  {0x5C1974+2,DS(BuildExclusions)+8,4}, // check for previous field
//	{0x5D606B+2,DS(BuildExclusions),4},
//
//	{0x460DE2+3,DS(TownStructOnScreen),4},
//
//	{0x460DD1+2,DS(TownStructOnScreenNum),4},
//	{0x461038+2,DS(TownStructOnScreenNum),4},
//	{0x46131D+2,DS(TownStructOnScreenNum),4},
//	{0x4613AE+2,DS(TownStructOnScreenNum),4},
//	{0x4613CC+2,DS(TownStructOnScreenNum),4},
//	{0x46140F+2,DS(TownStructOnScreenNum),4},
//	{0x46142B+2,DS(TownStructOnScreenNum),4},
//	{0x4614C2+2,DS(TownStructOnScreenNum),4},
//	{0x4614DE+2,DS(TownStructOnScreenNum),4},
//	{0x46162D+2,DS(TownStructOnScreenNum),4},
	// Lod support
	{0x559395+1,DS(&LodTable[0]),4},
	{0x5593A4+1,DS(&LodTable[1]),4},
	{0x5593B3+1,DS(&LodTable[2]),4},
	{0x5593C2+1,DS(&LodTable[3]),4},
	{0x5593D1+1,DS(&LodTable[4]),4},
	{0x5593E0+1,DS(&LodTable[5]),4},
	{0x5593EF+1,DS(&LodTable[6]),4},
	{0x5593FE+1,DS(&LodTable[7]),4},
	{0x55944C+1,DS(LodTable),4},
	{0x55A571+2,DS(LodTable),4},
	{0x55A577+2,DS(LodTable+4),4},
	{0x55A629+2,DS(LodTable),4},
	{0x55A62F+2,DS(LodTable+4),4},
	{0x55A726+2,DS(LodTable+4),4},
	{0x55A7D8+1,DS(LodTable+4),4},
	{0x55ABB8+2,DS(LodTable+4),4},
	{0x55ABE1+2,DS(LodTable+4),4},
	{0x55AC3D+2,DS(LodTable+4),4},
	{0x55AC6A+2,DS(LodTable+4),4},
	{0x55AFC6+2,DS(LodTable+4),4},
	{0x55AFEF+2,DS(LodTable+4),4},
	{0x55B04A+2,DS(LodTable+4),4},
	{0x55B077+2,DS(LodTable+4),4},
	{0x55B40D+2,DS(LodTable+4),4},
	{0x55B436+2,DS(LodTable+4),4},
	{0x55B492+2,DS(LodTable+4),4},
	{0x55B4BF+2,DS(LodTable+4),4},
	{0x55B7D0+2,DS(LodTable+4),4},
	{0x55B7F9+2,DS(LodTable+4),4},
	{0x55B852+2,DS(LodTable+4),4},
	{0x55B879+2,DS(LodTable+4),4},
	{0x55BBE3+2,DS(LodTable+4),4},
	{0x55BC0C+2,DS(LodTable+4),4},
	{0x55BC68+2,DS(LodTable+4),4},
	{0x55BC95+2,DS(LodTable+4),4},
	{0x55BEF8+2,DS(LodTable+4),4},
	{0x55BF21+2,DS(LodTable+4),4},
	{0x55C1C8+2,DS(LodTable+4),4},
	{0x55C1F1+2,DS(LodTable+4),4},
	{0x55CA54+2,DS(LodTable+4),4},
	{0x55CA7D+2,DS(LodTable+4),4},
	{0x55CAD7+2,DS(LodTable+4),4},
	{0x55CB00+2,DS(LodTable+4),4},
	{0x55D197+2,DS(LodTable+4),4},
	{0x55D1C0+2,DS(LodTable+4),4},
	{0x55D227+2,DS(LodTable+4),4},
	{0x55D250+2,DS(LodTable+4),4},
	{0x55D2A8+2,DS(LodTable+4),4},
	{0x55D2C7+2,DS(LodTable+4),4},
	
//  {0x559575+1,DS(&LodTypes::Table[0]),4},
//  {0x5595B0+1,DS(&LodTypes::Table[1]),4},
//  {0x5595DF+1,DS(&LodTypes::Table[2]),4},
//  {0x559606+1,DS(&LodTypes::Table[3]),4},
	{0x55A548+3,DS(LodTypes::Table),4},
	{0x55ABA3+3,DS(LodTypes::Table),4},
	{0x55AC28+3,DS(LodTypes::Table),4},
	{0x55AFB1+3,DS(LodTypes::Table),4},
	{0x55B035+3,DS(LodTypes::Table),4},
	{0x55B3F8+3,DS(LodTypes::Table),4},
	{0x55B47D+3,DS(LodTypes::Table),4},
	{0x55B7BB+3,DS(LodTypes::Table),4},
	{0x55B840+3,DS(LodTypes::Table),4},
	{0x55BBCE+3,DS(LodTypes::Table),4},
	{0x55BC53+3,DS(LodTypes::Table),4},
	{0x55BEE3+3,DS(LodTypes::Table),4},
	{0x55C1B3+3,DS(LodTypes::Table),4},
	{0x55C43F+3,DS(LodTypes::Table),4},
	{0x55CA38+3,DS(LodTypes::Table),4},
	{0x55CA3F+3,DS(LodTypes::Table),4},
	{0x55CABB+3,DS(LodTypes::Table),4},
	{0x55CAC2+3,DS(LodTypes::Table),4},
	{0x55D17B+3,DS(LodTypes::Table),4},
	{0x55D212+3,DS(LodTypes::Table),4},
	{0x55D296+3,DS(LodTypes::Table),4},
	{0x55CA31+3,DS(&LodTypes::Table[0][0].Inds),4},
	{0x55CAB4+3,DS(&LodTypes::Table[0][0].Inds),4},
	{0x55D174+3,DS(&LodTypes::Table[0][0].Inds),4},
	{0x55AB9C+3,DS(&LodTypes::Table[0][1]),4},
	{0x55AC21+3,DS(&LodTypes::Table[0][1]),4},
	{0x55AFAA+3,DS(&LodTypes::Table[0][1]),4},
	{0x55B02E+3,DS(&LodTypes::Table[0][1]),4},
	{0x55B3F1+3,DS(&LodTypes::Table[0][1]),4},
	{0x55B476+3,DS(&LodTypes::Table[0][1]),4},
	{0x55B7B4+3,DS(&LodTypes::Table[0][1]),4},
	{0x55B839+3,DS(&LodTypes::Table[0][1]),4},
	{0x55BBC7+3,DS(&LodTypes::Table[0][1]),4},
	{0x55BC4C+3,DS(&LodTypes::Table[0][1]),4},
	{0x55BEDC+3,DS(&LodTypes::Table[0][1]),4},
	{0x55C1AC+3,DS(&LodTypes::Table[0][1]),4},
	{0x55D20B+3,DS(&LodTypes::Table[0][1]),4},
	{0x55AB95+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55AC1A+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55AFA3+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55B027+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55B3EA+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55B46F+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55B7AD+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55B832+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55BBC0+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55BC45+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55BED5+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55C1A5+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55D204+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55D28F+3,DS(&LodTypes::Table[0][1].Inds),4},
	{0x55C438+3,DS(&LodTypes::Table[0][2]),4},

	// Spells support
	{0x59E467+3,DS(Spells),4},
	{0x687FA8,DS(Spells),4},

	// поддержка новых замков (+глюки с координатами в Rampart)
	//{0x5C6EE9+3,DS0(&CastInMem.DefNameBuild[0][0].BuldingName),4}, //Diakon
	//{0x5C3393+3,DS0(&CastInMem.DefBorderName[0][0].BorderName),4}, //Diakon
	//{0x5C32E6+3,DS0(&CastInMem.Coordinate[0][0].XCoordinate),4}, //Diakon	
	//{0x5C32F3+3,DS0(&CastInMem.Coordinate[0][0].YCoordinate),4}, //Diakon

	{0,0}
};

struct __Copiers{
	Byte *from;
	Byte *to;
	long  len;
} Copiers[]={
// FindERMn
	{(Byte *)"\xE8\x1\x1\x1\x1\x5E\x33\xC0\x5B\x5D\xC2\x10\x00",(Byte *)0x4FDF8B,13},
// FindERMl
	{(Byte *)"\xE8\x1\x1\x1\x1\x5E\x33\xC0\x5B\x8B\xE5\x5D\xC2\x10\x00",(Byte *)0x4FE577,15},
// EventERM
//  {(Byte *)"\xE8\x1\x1\x1\x1\x6A\xFF\xEB\x06\x90\x55\x8B\xEC\xEB\xF1",(Byte *)0x49F6F6,15},
// GlobalEvent
// Castle Service
	{(Byte *)"\x89\x7D\xF0\xE8\x1\x1\x1\x1\x83\xFF\x19\x90\x90\x90\x90\x90",(Byte *)0x5D38AA,16},
	{(Byte *)"\xE8\x1\x1\x1\x1\x33\xC0\x90\x3B\xDF",(Byte *)0x5D6B90,10},
// NextHeroChoosen
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90",(Byte *)0x58DF33,7},
// New Speciality
	{(Byte *)"\xE8\x1\x1\x1\x1\xEB\x20",(Byte *)0x4E11E0,7}, //Picture 1 Prepare
	{(Byte *)"\x8D\x4D\xD8\xE8\x1\x1\x1\x1\xEB\x14",(Byte *)0x4E1F35,10}, //Picture 1,2 Show
	{(Byte *)"\x6C",(Byte *)0x68C782,1}, //Original Picture 2 Disable
	{(Byte *)"\xE8\x1\x1\x1\x1\xEB\x1D",(Byte *)0x4DED05,7}, //Picture 2 Prepare
	{(Byte *)"\xE8\x1\x1\x1\x1\x8B\xF8\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",(Byte *)0x4DDA15,18}, //Text 1,2
// FileLoader
	{(Byte *)"\xE8\x1\x1\x1\x1\xC3",(Byte *)0x47A6B3,6},
// New/Load Map/Game
////  {(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x4C413E,5}, // N G (тип)
////  {(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x4C5323,5}, // L G (тип)
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90",(Byte *)0x5849C7,9}, // N G (тип)
////  {(Byte *)"\x8B\x4B\x0C\x89\x4F\x0C\x8B\x4B\x10\x89\x4F\x10\x8B\x53\x14\x89\x57\x14",(Byte *)0x58F7DB,18}, // copy (тип)
////  {(Byte *)"\x8B\x4B\x0C\x89\x4F\x0C\x8B\x4B\x10\x89\x4F\x10\x8B\x53\x14\x89\x57\x14",(Byte *)0x59083A,18}, // copy (тип)
//  {(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x4C413E,5},
//CrBank
	{(Byte *)0x67037C,(Byte *)crbankt1,sizeof(crbanks1)*BANKNUM_0},
	{(Byte *)0x6702A0,(Byte *)crbankt2,sizeof(crbanks2)*BANKNUM_0},
//Monsters
	{(Byte *)0x6703B8,(Byte *)MonTable,sizeof(_MonInfo_)*MONNUM_0},
// HeroSpec
	{(Byte *)0x678420,(Byte *)HSpecTable,sizeof(_HeroSpec_)*HERNUM_0},
// HeroInfo
	{(Byte *)0x679DD0,(Byte *)HTable,sizeof(_HeroInfo_)*(HERNUM_0+8)},
// Magic
	{(Byte *)0x641E18,(Byte *)MagicAni,sizeof(_MagicAni_)*0x53},
// Dwellings
	{(Byte *)0x63D570,(Byte *)DwMonster,sizeof(int)*DWNUM_0},
// Картинка атаки монстра (AvWattak.def)
//  {(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x4118BF,6},
// Картинка убитого монстра (AvWattak.def)
//  {(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x4110C7,6},
// Иниц. оружия у стреляющих монстров
//  {(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x43DA8E,6},
// полет монстров
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x44571B,5},
// страх 1
	{(Byte *)"\xE8\x1\x1\x1\x1\x0A\xC0",(Byte *)0x464944,7},
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",(Byte *)0x4649AA,21},
// проверка на врайтсов
//  {(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90",(Byte *)0x4650C5,8},
// проверка на драк-прив
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x440245,6},
// проверка на троля (автовост)
	{(Byte *)"\xE8\x1\x1\x1\x1\x3D\xFF\xFF\xFF\xFF\x90\x90\x90\x90\x90",(Byte *)0x446BD6,15},
	{(Byte *)"\x8B\xCA\x90\x90\x90\x3B\xC2\x90",(Byte *)0x446C2E,8},
// проверка на феникса (автовост)
	{(Byte *)"\xE8\x1\x1\x1\x1\x0A\xC0",(Byte *)0x46905A,7},
// проверка на harpy (летит назад)
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x0A\xC0",(Byte *)0x47832B,8},
// прверка на разрешение вывода Message
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x4F6C0A,6},
// прверка на шутера - без потери мощности
	{(Byte *)"\xE8\x1\x1\x1\x1\x0B\xC0\x90",(Byte *)0x46723E,8},
// проверка на дьявола
	{(Byte *)"\xE8\x1\x1\x1\x1\x0B\xD2\x74\x09\x90\x90\x90\x90\x90\x90\x90",(Byte *)0x44B00D,16},
// проверка на дьявола
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90",(Byte *)0x43D954,8},
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90",(Byte *)0x4B3736,8},
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90\x90\x90\x90",(Byte *)0x4B301C,12},
// проверка на дьявола
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90\x90\x0B\xC0\x74\x29",(Byte *)0x44C049,14},
// проверка на ангела
	{(Byte *)"\xE8\x1\x1\x1\x1\x0B\xC0\x74\x07\x90\x90\x90\x90\x90",(Byte *)0x44ACF1,14},
	{(Byte *)"\xE8\x1\x1\x1\x1\x0B\xC0\x74\x07\x90\x90\x90\x90\x90",(Byte *)0x44BA58,14},
// проверка на ангела
	{(Byte *)"\xE8\x1\x1\x1\x1\x0B\xC0\x74\x07\x90\x90\x90\x90\x90",(Byte *)0x44BA6F,14},
// проверка на защищенную карту
	{(Byte *)"\x75\x71\xE8\x1\x1\x1\x1\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",(Byte *)0x4C4141,24},
// запретим рестарт сценария
//  {(Byte *)"\xE8\x1\x1\x1\x1\xEB\x38",(Byte *)0x41ABF4,7},
// скопируем типы монстров нового месяца
//  {(Byte *)0x63E678,NewMonthMonTypes,12},
// вызывается для новой карты из компании
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x45B3E1,5},
// +1 уровень для героя
	{(Byte *)"\x0B",(Byte *)0x4DAB49,1},
	{(Byte *)"\xE8\x1\x1\x1\x1\x8B\xF0\x89\x75\xEC",(Byte *)0x4DAB55,10},
	{(Byte *)"\x90\x90\x90\x90\x90\x90\x90\x90\x90",(Byte *)0x4DAB65,9},
// Стрельба через стены без потерь
//  {(Byte *)"\xE8\x1\x1\x1\x1\x0B\xC0\x90\x90\x90\x90\x90\x90\x90\x90\x90\x0F\x85",(Byte *)0x46713A,18},
	{(Byte *)"\xE8\x1\x1\x1\x1\x0B\xC0\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x0F\x85",(Byte *)0x46711A,50},
// Перерисовка артифактов при их переносе из слота в слот
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x50D0FA,6},
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x50D103,6},
// таблица бонусов артифактов
	{(Byte *)0x63E758,(Byte *)ArtBonus,ARTNUM_0*4},
	{(Byte *)ArtBonusAdd,(Byte *)ArtBonus[ARTNUM_0],(ARTNUM-ARTNUM_0)*4},
// хинт для артифактов (подставлялся заместо описания)
//  {(Byte *)"\xE8\x1\x1\x1\x1\x8B\xF0",(Byte *)0x4DB68C,7},
// типа сказочный дракон
	{(Byte *)"\xE8\x1\x1\x1\x1\x0B\xC0",(Byte *)0x47602C,7},
// сила удара сказ дракона
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x44836D,6},
// показ версии ВоГа
	{(Byte *)"\xE8\x1\x1\x1\x1\xEB\x38",(Byte *)0x4FBFFE,7},
// бросить текущую игру
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90\x90",(Byte *)0x409389,10},
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90\x90",(Byte *)0x409314,10},
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x5B397E,6},
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x4F0568,5},
// настройка поля битвы
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x49389F,6},
// сделать видимым для AI весь радиус анализа
////  {(Byte *)"\xE9\x08\x04\x0\x0\x90",(Byte *)0x42EF3C,6},
////  {(Byte *)"\xEB",(Byte *)0x42F15B,1},
// NPC
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x4DA9B6,5},
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90",(Byte *)0x46359E,7},
//  {(Byte *)"\xE8\x1\x1\x1\x1\xE9\x2E\xFC\xFF\xFF",(Byte *)0x4DD981,10},
// Champion
	{(Byte *)"\xC7\x45\xF0\x0\x0\x0\x0\xE8\x1\x1\x1\x1",(Byte *)0x443066,12},
// Death Stare
//  {(Byte *)"\x90\x90\x90\x90\x90",(Byte *)0x440BFB,5},
	{(Byte *)"\xE8\x1\x1\x1\x1\x8B\xD9\x90\x90\x90\x90\x90\x90\x90\x90",(Byte *)0x440BF6,15},
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x440C40,5},
// Контроль над балистой
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x84\xC0",(Byte *)0x474589,8},
// Клики мышкой в экране характеристик героя
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90",(Byte *)0x4DD632,7},
// Клик мышкой на поле битвы на защиту
	{(Byte *)"\xE8\x1\x1\x1\x1\xEB\x1E\x90\x90\x90",(Byte *)0x47265C,10},
// клики мауса на в городе
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x5D3867,6},
// Клик мышкой при встрече героев
	{(Byte *)"\xE8\x1\x1\x1\x1\x8D\x70\xFF\x81\xFE\xD6\x0\x0\x0\x0F\x87\x11\x06\x0\x0",(Byte *)0x5B02DD,20},
// Автоматическое добавление Мифрила
	{(Byte *)"\xE8\x1\x1\x1\x1\x0B\xC0\x74\x54",(Byte *)0x4E3879,9},
// Клик мышкой на поле битвы
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x4746E8,6},
// Обработка LE c Мифрилом
	{(Byte *)"\x8B\x7D\x1C\xE8\x1\x1\x1\x1",(Byte *)0x49F7BA,8},
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x33\xFF",(Byte *)0x49FDE4,8},
// Сримает артифакт
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90",(Byte *)0x4E2CC1,7},
// Одевает артифакт
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90",(Byte *)0x4E2F60,7},
// фикс бага с АИ и Ангельским Альянсом (вылетает при найме нейтральных существ)
	{(Byte *)"\x33\xFF\x90\x90\x90\x90\x90",(Byte *)0x42C8B4,7},
	{(Byte *)"\x33\xF6\x90\x90\x90\x90\x90",(Byte *)0x42C77F,7},
// После выигрыша защищающегося героя в МП корректировка показа скилов
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x5570C1,6},
// Показ экспы для стэка (средний рисунок в экране Героя)
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90",(Byte *)0x5BC9B1,9},

// новые добавлять здесь
	{(Byte *)&MagicAniAdd[0],(Byte *)&MagicAni[0x53],sizeof(MagicAniAdd)},
	{(Byte *)&_MonAdd[0],(Byte *)&MonTable[MONNUM_0],sizeof(_MonAdd)},
// Видео файлы
	{(Byte *)0x683A10,(Byte *)VidArr,sizeof(_VidInfo_)*VIDNUM_0},
	{(Byte *)&VidArrAdd[0],(Byte *)&VidArr[VIDNUM_0],sizeof(VidArrAdd)},
// герои
//  {(Byte *)HBiosTablePluss,(Byte *)&HBiosTable[HERNUM_0+8],sizeof(HBiosTablePluss)},
//  {(Byte *)HSpecTablePluss,(Byte *)&HSpecTable[HERNUM_0],sizeof(HSpecTablePluss)},
//  {(Byte *)HTablePluss,(Byte *)&HTable[HERNUM_0+8],sizeof(HTablePluss)},
//  {(Byte *)HTable2Pluss,(Byte *)&HTable2[HERNUM_0],sizeof(HTable2Pluss)},
// MP3 файлы
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x59AC51,5}, //
// 3.58
// Carry Over Hero Setup
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x4860F9,6}, //
// Carry Over Hero New Art Setup
	{(Byte *)"\xEB\x19",(Byte *)0x486672,2}, //
	{(Byte *)"\xEB\x5C\x90",(Byte *)0x4865CE,3}, // ROE
	{(Byte *)"\x83",(Byte *)0x4875F5,1}, // 3.59 ROE carry over new arts crash fix
// фикс проблемы с вылетом по 0x5be8ae
// если сбой с героями случается (SoD)
	{(Byte *)"\x0F\x84\x65\x01\x00\x00",(Byte *)0x5BE88B,6}, //
//Skeleton Transformer
	{(Byte *)0x64130C,(Byte *)SkelTrans,sizeof(int)*(MONNUM_0-5)},
	{(Byte *)&SkelTransAdd[0],(Byte *)&SkelTrans[MONNUM_0-5],sizeof(SkelTransAdd)},
// Speed up AI thinking (0x00007D00 original)
// Now through ERM
//  {(Byte *)"\x00\x10\x00\x00",(Byte *)0x526C9E,4}, //
//  {(Byte *)"\x00\x10\x00\x00",(Byte *)0x526CEE,4}, //
// Post Instruction call 3.58
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90\x90",(Byte *)0x4C01A2,10}, //
// слияние стэков в городе
	{(Byte *)"\xE8\x1\x1\x1\x1\xE9\x69\xFF\xFF\xFF",(Byte *)0x5D520D,10}, //
// перенос стэков внутри экрана героя
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",(Byte *)0x4DDD77,20}, //
// фикс продажи артифактов дороже или дешевле - запрет стрелочек
	{(Byte *)"\x5A\xE4\x5E\x00\x5A\xE4\x5E\x00",(Byte *)0x5EE9E0,8}, // !!! TODO: proper fix
// фикс неправильной подсказки для артифактов в рюкзаке в ArtMerchant
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x5EEBC5,6}, //
// апгрэйд экспы монстров на холме
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90",(Byte *)0x4E86E3,7}, //
// апгрэйд экспы монстров в экране монстра
	{(Byte *)"\x8D\x45\x08\xE8\x1\x1\x1\x1\x90",(Byte *)0x4C6BD3,9}, //
// снижение цены кастинга закла
	{(Byte *)"\x90\x53\x56\xE8\x1\x1\x1\x1\x8B\xF0\xEB\x11\x90",(Byte *)0x4E556F,13}, //
// модификация бонуса защиты при выборе защиты
	{(Byte *)"\xE8\x1\x1\x1\x1\x8B\x43\x4C\x3B\xC7\x8B\x43\x34\x90",(Byte *)0x4790C3,14}, //
// модификация бонуса DeathBlow
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x4435A3,6}, //
// модификация Экстра дамэдж против конкретного монстра
	{(Byte *)"\x8B\x4D\x08\xE8\x1\x1\x1\x1\x0B\xC0\x0F\x84\x6B\x02\x00\x00\x8B\x4D\xF0\xEB\x67\x90\x90",(Byte *)0x443195,23}, //
// SoD фикс. установка существа уровня для случ героя
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x4D8F1D,6}, //
// кастинг спела после удара
//  {(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x5F\x5E\x5B",(Byte *)0x4412AE,10}, //
// минотавр - позитивная мораль всегда
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x43DDCE,5}, //
// unicorn - магическая аура
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x43E800,5}, //
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x43E8AE,5}, //
// Сопротисление спец магии Dispel
	{(Byte *)"\x8B\x4D\x10\x51\x8B\x4D\x0C\x51\xE8\x1\x1\x1\x1\x90\x90\x90",(Byte *)0x5A8454,16}, //
	{(Byte *)"\x8B\x4D\x10\x51\x8B\x4D\x0C\x51\xE8\x1\x1\x1\x1\x90\x90\x90",(Byte *)0x5A84DF,16}, //
// Фикс не проверки АИ флага разрешения/запрещения тактики
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x462847,5}, //
// запрет на смену вида курсора
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x50CEAA,6}, //
// расчет силы магии архангела (можно сделать управляемым !!!!)
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x5A87AD,6}, //
// фикс падения при выходе при попытке освободить мою статическую память
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x619BB1,6}, //
// fix Win98 quit crash
	{(Byte *)"\xE8\x1\x1\x1\x1\x85\xC9\x74\x49\x90\x90",(Byte *)0x4F84F4,11}, //
// new WoG 3.58 art - shield (retailate)
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x441AFF,6}, //
// new WoG 3.58 art - axe (+1strike)
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x441C01,6}, //
// fix No default Bio if a name set but bio not
	{(Byte *)"\x33\xC0\x90\x90\x90",(Byte *)0x4DD96C,5}, //
//  {0x48E5B3,DP(0x75),1}, // Combo arts fix for arts >160
	{(Byte *)"\xEB\x26\x90\x90\x90\x90\x90",(Byte *)0x48E5B5,7}, //
	{(Byte *)"\xEB\xD0\x90",(Byte *)0x4DC248,3}, //
// Dracolish (Power Lich shoot)
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90",(Byte *)0x43FA1F,9}, //
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x41ED5F,5}, //
// 3.58 Combo Part fix
	{(Byte *)"\xE9\x80\x0\x0\x0",(Byte *)0x4E2D43,5}, //
	{(Byte *)"\xE9\x7B\x0\x0\x0",(Byte *)0x4E2EB3,5}, //
	{(Byte *)"\x5F\x73\x24\x90\x90",(Byte *)0x4CF682,5}, //
	{(Byte *)"\xEB\x23\x90",(Byte *)0x4DC12F,3}, //
// Swap AI stacks before battle
//  {(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x4ADFAD,5}, //
// Shooting Animation for spec units (type of shoot in the table 0)
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x43DAAA,5}, //
// прием инфы о двух героях по сети - встреча (3.58)
//  {(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x40624D,5}, //
// вызов элементалей не работает без хозяина-героя
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90",(Byte *)0x5A7579,7}, //
	{(Byte *)"\x90\x90\x90",(Byte *)0x5A7586,3}, //
// вызов элементалей не работает без хозяина-героя
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90",(Byte *)0x5A8D56,7}, //
	{(Byte *)"\x90\x90\x90",(Byte *)0x5A8D63,3}, //
// правильный показ типа двелинга в окне Kingdom Overview
	{(Byte *)"\x90\x90\x90",(Byte *)0x520DA0,3}, //
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x520DB3,5}, //
// SoD потенциальный баг при просмотре ViewEarth
	{(Byte *)"\x90\x90",(Byte *)0x5FC087,2}, //
	{(Byte *)"\x90\x90",(Byte *)0x5FC0B5,2}, //
// не стреляет второй раз, если выстрелов не осталось
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x43FF7E,6}, //
// отладка сети
//  {(Byte *)"\xE8\x1\x1\x1\x1\xC3",(Byte *)0x51BD60,6}, //
// запрет каста клона на командиров
	{(Byte *)"\xE8\x1\x1\x1\x1\x3B\xD0",(Byte *)0x5A875B,7}, //
// 3.58f correct version "3 . 5 8"
//  {(Byte *)WOG_VERSION_WIDE,(Byte *)0x6B09EC,8}, //
// снятие бонуса героя от GR при переходе на след. карту
	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90",(Byte *)0x486245,8}, //
// 3.58f Fix "no Commander shooting animation loaded"
	{(Byte *)"\xE8\x1\x1\x1\x1\x90",(Byte *)0x43DA74,6}, //
// 3.58f Fix "wrong town info in town list MP"
	{(Byte *)"\xE8\x1\x1\x1\x1",(Byte *)0x5D4643,5}, //
// 3.59
	// fix to not show minimap to the first human player
	{(Byte *)"\x90\x90\x90\x90\x90",(Byte *)0x4C752C,5}, //
	// WoG Lod files
	{(Byte *)"h3wog.lod",(Byte *)0x682F08,10}, //
	{(Byte *)"h3custom.lod",(Byte *)0x682EF8,13}, //
	{(Byte *)"h3std.lod",(Byte *)0x682EE8,10}, //

	{(Byte *)"\xE8\x1\x1\x1\x1\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",(Byte *)0x4472F8,15}, //
	// Spells support
	{(Byte *)0x6854A0,(Byte *)Spells,sizeof(_Spell_)*SPELLNUM_0}, //

	// врезка перехода на менеджер сопративления монстров
	//{(Byte*)InsertResistFunc,(Byte*)0x44A4A7,6}, // Diakon

	// врезка FixGeneratora
	//{(Byte*)&JMP, (Byte*)0x54110F, 1}, //Diakon
	//{(Byte*)&PSUB, (Byte*)(0x541013+2), 4}, //Diakon
	//{(Byte*)&MSUB, (Byte*)(0x54106B+2), 4}, //Diakon
	//{(Byte*)&MSUB, (Byte*)(0x54109C+3), 4}, //Diakon
	//{(Byte*)&MSUB, (Byte*)(0x5410E1+3), 4}, //Diakon
	//{(Byte*)&MSUB, (Byte*)(0x541174+3), 4}, //Diakon
	//{(Byte*)&GENMONNUM, (Byte*)(0x541063+1), 4}, //Diakon
	//{(Byte*)&GENMONNUM, (Byte*)(0x5410B3+1), 4}, //Diakon
	//{(Byte*)&GENMON, (Byte*)(0x541159+1), 4}, //Diakon

	// новые замки
	// копирование названий дефов зданий
	//{(Byte*)(0x643074), (Byte*)&CastInMem.DefNameBuild[0][0].BuldingName,sizeof(CastInMem.DefNameBuild[0])*TOWNNUM_0},
	// оконтовки зданий
	//{(Byte*)(0x68A3DC), (Byte*)&CastInMem.DefBorderName[0][0].BorderName,sizeof(CastInMem.DefBorderName[0])*TOWNNUM_0},
	// копирование координат стандартных зданий
	//{(Byte*)(0x68AA0E), (Byte*)&CastInMem.Coordinate[0][0].XCoordinate,sizeof(CastInMem.Coordinate[0])},

	{0,0,0}
};

void Initialize(void);
Dword InitEnters(void);
void FixDLLEntries(void);
void InitCRT(void);

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if ((int)hInst != 0x7700000)  MessageBox(0, "WogDll.dll got relocated!", 0, 0);
		GEr.Clear();
		Initialize();
		GEr.canLog = true;
	}
	return true;
}

#ifdef _DEBUG_LUA

void RunDebugLua();

void main()
{
	InitCRT();
	RunDebugLua();
}

#else

__declspec( naked ) void main(void)
{
// переход на ориг обработчик
	__asm  pusha;
	FixDLLEntries();
	InitCRT();
	GEr.Clear();
//  try{	
    //GENMONNUM = GetMaxGenerationMonstr();

	//PSUB = GENMONNUM*4 + 0x10; //Diakon
	//GENMON = GENMONNUM -1;
 //   MSUB = -PSUB; // Diakon
	Initialize();
	GEr.canLog = true;

	//hMonLib = LoadLibrary("NewMonTown/MONSTRDLL.dll"); // Diakon
	//InitilizeStructFromWoG = (MYPROC)GetProcAddress(hMonLib, "InitilizeStructFromWoG@32"); //Diakon
	//InitilizeStructFromWoG((void*)&MonTable,(void*)&MonATable,(void*)&MoJumper4,(void*)&MoJumper3,(void*)&MoJumper1, (void*)&MonTable2[0], (void*)&MonTable3[0], (void*)&MonTable4[0] ); //Diakon
	//LoadNewMonstrs = (LOADMONPROC)GetProcAddress(hMonLib, "LoadingMonstrs@0"); //Diakon
	//CleanMemmory = (PROCCLEANMEM) GetProcAddress(hMonLib, "CleanMemmory@0"); //Diakon
	//LoadNewMonstrs (); // Diakon
	//LoadingMonstrs();


	__asm  popa;
#ifdef DEBUG
	__asm  mov  eax,offset DebugDP
	__asm{ 
		mov  eax,0x40007C; 
		mov  eax,[eax]; 
		call  eax 
	}
#else
	__asm{ 
		mov  eax,GEr.InterruptMe
		mov  eax,0x40007C; 
		mov  eax,[eax]; 
		jmp  eax 
//    call eax
	}
#endif
//  }catch(...){ GEr.Show(); }
}
#endif

static __declspec( naked ) void ForAK(void){
	__asm{ // для АК
//    db "ZVS_4_AK_START"
		_emit WOGCRC & 0xFF
		_emit (WOGCRC>>8) & 0xFF
		_emit (WOGCRC>>16) & 0xFF
		_emit (WOGCRC>>24) & 0xFF
		_emit 'Z'
		_emit 'V'
		_emit 'S'
		_emit '_'
		_emit '4'
		_emit '_'
		_emit 'A'
		_emit 'K'
		_emit '_'
		_emit 'S'
		_emit 'T'
		_emit 'A'
		_emit 'R'
		_emit 'T'
//    db 10000 dup (0)
#include "space1k.cpp"
#include "space1k.cpp"
#include "space1k.cpp"
#include "space1k.cpp"
#include "space1k.cpp"
#include "space1k.cpp"
#include "space1k.cpp"
#include "space1k.cpp"
#include "space1k.cpp"
#include "space1k.cpp"
//    db "ZVS_4_AK_STOP"
		_emit 'Z'
		_emit 'V'
		_emit 'S'
		_emit '_'
		_emit '4'
		_emit '_'
		_emit 'A'
		_emit 'K'
		_emit '_'
		_emit 'S'
		_emit 'T'
		_emit 'O'
		_emit 'P'
	}
}

/*
extern "C" int  _heap_init(int mtflag);
extern "C" int  _mtinit(void);
extern "C" void _ioinit(void);
extern "C" void _cinit(void);
extern "C" unsigned int _osver;
extern "C" unsigned int _winminor;
extern "C" unsigned int _winmajor;
extern "C" unsigned int _winver;
extern "C" unsigned int _osplatform;

	OSVERSIONINFO posvi;      
int MyGetVersionExA(OSVERSIONINFO *lpVersionInfo);
#define VER_PLATFORM_WIN32_NT 2

void InitCRT(void){
	// Get the full Win32 version
//  _osver = GetVersion();
//  _winminor = (_osver >> 8) & 0x00FF ;
//  _winmajor = _osver & 0x00FF ;
//  _winver = (_winmajor << 8) + _winminor;
//  _osver = (_osver >> 16) & 0x00FFFF ;
//         * Dynamically allocate the OSVERSIONINFOA buffer, so we avoid
//         * triggering the /GS buffer overrun detection.  That can't be
//         * used here, since the guard cookie isn't available until we
//         * initialize it from here!

//         * Get the full Win32 version
				posvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
				(void)MyGetVersionExA(&posvi);

				_osplatform = posvi.dwPlatformId;
				_winmajor = posvi.dwMajorVersion;
				_winminor = posvi.dwMinorVersion;

//         * The somewhat bizarre calculations of _osver and _winver are
//         * required for backward compatibility (used to use GetVersion)
				_osver = (posvi.dwBuildNumber) & 0x07fff;
				if ( _osplatform != VER_PLATFORM_WIN32_NT )
						_osver |= 0x08000;
				_winver = (_winmajor << 8) + _winminor;

//         * Determine if this is a managed application
//        managedapp = check_managed_app();

#ifdef _MT
	_heap_init(1);               // initialize heap 
#else  
	_heap_init(0);               // initialize heap 
#endif 
#ifdef _MT
	_mtinit();                   // initialize multi-thread 
#endif 
	_ioinit();                      // initialize lowio 

	_cinit();                       // do C data initialize 
	
}
*/
struct HImport{
	Dword  HintName;
	Dword  Time;
	Dword  Chain;
	Dword  Name;
	Dword  Thunk;
};
struct LinkIt{
	char  *DllName;
	char  *FunName;
	Dword *Place;
} GLinks[]={
	{0,0,0}
};
Dword GetFunEntryPoint(char *DllName,char *FunName){
	Dword hdll=Service_LoadLibrary(DllName); if(hdll==0) return 0;
	return(Service_GetProcAddress(hdll,FunName));
}
void FixDLLEntries(void){
	HImport *My=(HImport *)(*(Dword *)0x400600);
	Dword Shift=*(Dword *)0x400604;
	char *DllName;
	for(int i=0;;i++){
		if(My[i].HintName==0) break; // end of the table
		My[i].HintName+=Shift;
		My[i].Name+=Shift;
		My[i].Thunk+=Shift;
		DllName=(char *)My[i].Name;
		long *FunNames=(long *)My[i].HintName;
		Dword *Thunks=(Dword *)My[i].Thunk;
		for(int j=0;;j++){
			if(FunNames[j]==0) break; // all table is fixed
			if(FunNames[j]>0)
				FunNames[j]+=Shift;
			for(int k=0;;k++){// end of the table, still not found :-(
				if(GLinks[k].DllName==0){ 
					Thunks[j]=GetFunEntryPoint(DllName,(char *)(FunNames[j]>0 ? FunNames[j]+2 : (Word)FunNames[j]));
					if(Thunks[j]==0){ 
						return;
					}
					break; 
				}
				if(strcmp(DllName,GLinks[k].DllName)) continue; // wrong DLL name
				if(FunNames[j] > 0) {
					if(strcmp((char *)(FunNames[j]+2),GLinks[k].FunName)) continue; // wrong Fun name
				} else {
					if(FunNames[j] != (long)GLinks[k].FunName) continue;
				}
				Thunks[j]=*GLinks[k].Place;
				break;
			}
		}
	}
//  }catch(...){ GEr.Add("FixDLLEntries",0); throw __LINE__; } 
}

Dword Initialize_P=0;
void Initialize(void)
{
	int   i,j;
	long  del;
	Byte *s,*d;

	Initialize_P=(Dword)ForAK; // to prevent removing of ForAK
	StartDLLService();
	if(LoadZVSDialogs()) return /*Exit()*/;
	//  CommonDialog("Hi from a Heroes3 extension!");
//  START("Start Up Initialization");

//InitWoGSetup();
//UseWogSetup(&DlgSetup);

	ParseCommandLine();

	for(i=0;;i++){
		if(Accessers[i].where==0) break;
		s=(Byte *)&Accessers[i].what;
		d=(Byte *)Accessers[i].where;
		for(j=0;j<Accessers[i].len;j++) *d++=*s++;
//    *(long *)(Accessers[i].where)=Accessers[i].what;
#ifdef DEBUG
		if(DebugDPNum<DSNUM){
			if(Accessers[i].remember!=0){
				for(j=0;j<16;j++){
					char c=Accessers[i].remember[j];
					DebugDP[DebugDPNum].name[j]=c;
					if(c==0) break;
				}
				DebugDP[DebugDPNum].dp[0]=(Dword)Accessers[i].what;
				DebugDP[DebugDPNum].dp[1]=(Dword)Accessers[i].what+Accessers[i].size;
				++DebugDPNum;
			}
		}
#endif
	}
	for(i=0;;i++){
		if(Copiers[i].from==0) break;
		j=Copiers[i].len;
		do{ *Copiers[i].to++=*Copiers[i].from++; --j;  }while(j!=0);
	}
	for(i=0;;i++){
		if(Callers[i].where==0) break;
		if(Callers[i].fnew==0) continue; // если новый указатель=0, то пропустим
		del=Callers[i].fnew-Callers[i].where-5;
		*(long *)(Callers[i].where+1)=del;
#ifdef DEBUG
		if(DebugDPNum<DSNUM){
			if(Callers[i].remember!=0){
				for(j=0;j<16;j++){
					char c=Callers[i].remember[j];
					DebugDP[DebugDPNum].name[j]=c;
					if(c==0) break;
				}  
				DebugDP[DebugDPNum].dp[0]=(Dword)Callers[i].fnew;
				++DebugDPNum;
			}
		}
#endif   
	}
	for(i=0;i<BANKNUM;i++){ CrBankTable[i].SetUp=1; }
// CrBank initialize
//  crbankt1[11].BMonsterType=156;     // 0x0C - Angel
	crbankt2[11].DMonsterType[0]=22;   // Дендроид
	crbankt2[11].DMonsterType[1]=18;   // Эльф
	crbankt2[11].DMonsterType[2]=22;   // Дендроид
	crbankt2[11].DMonsterType[3]=18;   // Эльф
	crbankt2[11].DMonsterType[4]=-1;   // все
//  crbankt1[12].BMonsterType=133;     // Crystal Dragon
	crbankt2[12].DMonsterType[0]=33;   // Стальной Голем
	crbankt2[12].DMonsterType[1]=29;   // Мастер Гремлин
	crbankt2[12].DMonsterType[2]=33;   // Стальной Голем
	crbankt2[12].DMonsterType[3]=29;   // Мастер Гремлин
	crbankt2[12].DMonsterType[4]=-1;   // no more
//  crbankt2[13].BMonsterNum=1;        // всегда только 1
	crbankt1[13].BMonsterType=160;     // God1
	crbankt2[13].DMonsterType[0]=164;  // Алмазный голем
	crbankt2[13].DMonsterType[1]=164;  // голем
	crbankt2[13].DMonsterType[2]=164;  // голем
	crbankt2[13].DMonsterType[3]=164;  // голем
	crbankt2[13].DMonsterType[4]=-1;   // Алмазный голем
//  crbankt2[14].BMonsterNum=1;        // всегда только 1
	crbankt1[14].BMonsterType=161;     // God2
	crbankt2[14].DMonsterType[0]=165;  // Алмазный голем
	crbankt2[14].DMonsterType[1]=165;  // Алмазный голем
	crbankt2[14].DMonsterType[2]=165;  // Алмазный голем
	crbankt2[14].DMonsterType[3]=165;  // Алмазный голем
	crbankt2[14].DMonsterType[4]=-1;   // Алмазный голем
//  crbankt2[15].BMonsterNum=1;       // всегда только 1
	crbankt1[15].BMonsterType=162;     // God3
	crbankt2[15].DMonsterType[0]=166;  // Алмазный голем
	crbankt2[15].DMonsterType[1]=166;  // Алмазный голем
	crbankt2[15].DMonsterType[2]=166;  // Алмазный голем
	crbankt2[15].DMonsterType[3]=166;  // Алмазный голем
	crbankt2[15].DMonsterType[4]=-1;   // Алмазный голем
//  crbankt2[16].BMonsterNum=1;      // всегда только 1
	crbankt1[16].BMonsterType=163;     // God4
	crbankt2[16].DMonsterType[0]=167;  // Алмазный голем
	crbankt2[16].DMonsterType[1]=167;  // Алмазный голем
	crbankt2[16].DMonsterType[2]=167;  // Алмазный голем
	crbankt2[16].DMonsterType[3]=167;  // Алмазный голем
	crbankt2[16].DMonsterType[4]=-1;   // Алмазный голем
// vampire
	crbankt1[17].BMonsterType=63;      // Vampire
	crbankt2[17].DMonsterType[0]=62;   //
	crbankt2[17].DMonsterType[1]=-1;   // no more
// vampire 2
	crbankt1[18].BMonsterType=63;      // Vampire
	crbankt2[18].DMonsterType[0]=62;   //
	crbankt2[18].DMonsterType[1]=-1;   // no more
// джины
	crbankt1[19].BMonsterType=37;      // Мастер Джин
	crbankt2[19].DMonsterType[0]=37;   // Мастер Джин
	crbankt2[19].DMonsterType[1]=-1;   // no more
// камни в обычных горах
	crbankt2[20].DMonsterType[0]=73;   // Гарпия ведьма
	crbankt2[20].DMonsterType[1]=75;   // Дурной глаз
	crbankt2[20].DMonsterType[2]=73;   // Гарпия ведьма
	crbankt2[20].DMonsterType[3]=75;   // Дурной глаз
	crbankt2[20].DMonsterType[4]=-1;   // no more

	MonTable[123].SubGroup=2; //Ice Elementals
	MonTable[125].SubGroup=4; //Magma Elementals
	MonTable[129].SubGroup=3; //Energy Elementals

	LoadIniPath(DeveloperPath, "DeveloperPath", "");
	LoadIniPath(ModsPath, "ModsPath", (DeveloperPath[0] ? Format("%sMods\\", DeveloperPath) : "Mods\\"));
	
	if(!DeveloperPath[0] && DoesFileExist("h3std.lod",1))  LodTypes::Add2List(6);
	if(!DeveloperPath[0])  LodTypes::Add2List(4);
	if(DoesFileExist("h3custom.lod",1))  LodTypes::Add2List(5);

	newGlobalInitSub();
	InitEnters();

//  RETURNV;
}
//void __stdcall WinMain (int,int,int,int){}
// сброс всех структур
int ThisIsNewGame;
void ResetAll(int game = 1)
{
	STARTNA(__LINE__, 0)
	if(game){
		ThisIsNewGame=1; // новая, а не загруженная
		ResetCastles();
		ResetMapMon();
		ResetCurse();
		ResetB1();
		ResetWM();
	}
	ResetERM(game);
	ResetMP3();
	if(game){
		ResetNPC();
		ResetAI();
		CrExpoSet::Clear(); // 3.58 R
		CrExpMod::Clear(); // 3.58 R
		CrExpBon::Clear(); // 3.58 R
		ResetA160(); // 3.58
	}
	ResetLTimer(); // 3.59
	//ResetNewTownStruct(); // 3.59
	ResetSpells(); // 3.59
	ResetLODs(); // 3.59
	GLB_DisableMouse=0;
//asm{
//mov eax,0
//mov eax,[eax]
//}

	//...
	RETURNV
}
///////////////////////////////////////////////
// загружаем строки
int InitAllTXT(void)
{
	STARTNA(__LINE__, 0)
	if(LoadTXTCasDem()) Exit();
//...
	RETURN(0)
}

static char *FearMessagePo=0;
void TxtOutOfExe(void)
{
	STARTNA(__LINE__, 0)
	char *po;
/*
	po=ITxt(34,0,&Strings);
	__asm{ 
		mov ebx,0x447302+1
		mov eax,po
		mov [ebx],eax
	}
	po=ITxt(35,0,&Strings);
	__asm{ 
//    mov ebx,0x6616D4
		mov ebx,0x4472F8+1
		mov eax,po
		mov [ebx],eax
	}
	po=ITxt(36,0,&Strings);
	__asm{ 
		mov ebx,0x5A2237+1
		mov eax,po
		mov [ebx],eax
	}
	po=ITxt(37,0,&Strings);
	__asm{ 
		mov ebx,0x44C156+1
		mov eax,po
		mov [ebx],eax
	}
*/
	// 3.59
	po=ITxt(121,0,&Strings); // страх
	FearMessagePo=po;
	__asm{
		mov   ebx,0x464AC6
		mov   word PTR [ebx-2],0x9090
		mov   byte PTR [ebx],0xB8
		mov   eax,offset FearMessagePo
		mov   [ebx+1],eax
		mov   dword PTR [ebx+5],0x9090008B
		mov   dword PTR [ebx+9],0x90909090
		mov   dword PTR [ebx+9],0x90909090
		mov   byte PTR  [ebx+13],0x90
	}
/*
	__asm{
		mov ebx,0x464AC6
		mov al,0xB8 
		mov [ebx],al
		mov eax,po  
		mov [ebx+1],eax;
		mov al,0x90
		mov [ebx+5],al
		mov ebx,0x464ACE
		mov al,0xB8
		mov [ebx],al
		mov eax,po
		mov [ebx+1],eax
		mov al,0x90
		mov [ebx+5],al
	}
*/
	RETURNV
}

static Dword CM_tmp;
__declspec( naked ) void CastMessage(void){
	__asm{
		mov   eax,35  
		cmp   ecx, 1
		jz   _done
		mov   eax,34
	}
_done:
	__asm{ 
		mov  CM_tmp,eax
		push ecx 
	}
	CM_tmp=(Dword)ITxt(CM_tmp,0,&Strings);
	__asm{ 
		pop ecx 
		mov edx,CM_tmp
		ret
	}
}

Dword AcidBreathMessage(Dword buf,char * /*str*/,Dword p1,Dword p2){
	Dword (*fun)(Dword,char *,Dword,Dword); *(Dword *)&fun=0x50C7F0;
	return fun(buf,ITxt(36,0,&Strings),p1,p2);
}

Dword AlwaysLuckyMessage(Dword buf,char * /*str*/,Dword p1){
	Dword (*fun)(Dword,char *,Dword); *(Dword *)&fun=0x50C7F0;
	return fun(buf,ITxt(37,0,&Strings),p1);
}

//void BaseFileLoader()
//{
//	__asm pusha
//	STARTNA(__LINE__, 0)
//	STOP
//	__asm popa
//}

void FileLoader(void)
{
	__asm pusha
	STARTNA(__LINE__, 0)
	if(LoadTXT("ZMESS00.TXT",&Strings)) Exit(); // не может загрузить TXT
	// выносим внутренние сообщения в файл наружу
	TxtOutOfExe();
	// тип локализации
	switch(ITxt(0,0,&Strings)[0]){
		case 'R': WoGType=1; break;
		default : WoGType=0;
	}
	InitHeroLocal(); // локализация описания новых специальностей
	newFileLoader();
//...
	if(LoadGhostParam()) Exit(); // не проинициализировалось
	if(InitAllTXT()) Exit(); // не проинициализировалось
	if(LoadSphinxTXT()) Exit(); // не проинициализировалось
	if(LoadNPCParam()) Exit(); // не проинициализировалось
	//if(LoadSetupParam()) Exit(); // не проинициализировалось
	if(LoadExpTXT()) Exit(); // 3.58 R
	GLB_DisableMouse=0;
	STOP
	__asm popa
}

/*
int CheckSOG(void) // ==0 -WoG <>0 other
{
	asm{
		push  ebx
		mov   ebx,BASE
		mov   ebx,[ebx]   // -> база
		add   ebx,0x1F86C // -> type
		mov   eax,[ebx]
		sub   eax,SODMAPTYPE
	}
	if(_EAX==0){
		asm{
			add   ebx,IDSHIFT // -> WoG
			movzx eax,byte ptr [ebx]
			sub   eax,SOGID
		}
	}
	asm pop ebx
	return _EAX
}
*/
/////////////////////////////////////////
// LoadSave
Dword LoadSave_Pointer;

int Saver(const void *Po,int Len)
{
	STARTNA(__LINE__, 0)
	__asm{
		mov   ecx,LoadSave_Pointer
		mov   eax,Po
		mov   edx,Len
		push  edx
		push  eax
		mov   edx,[ecx]
		call  dword ptr [edx+8]
		sub   eax,Len
		mov   IDummy,eax
	}
	RETURN(IDummy)
}

int Loader(void *Po,int Len)
{
	STARTNA(__LINE__, 0)
	__asm{
		mov   edx,LoadSave_Pointer
		mov   eax,Po
		mov   ecx,Len
		push  ecx
		push  eax
		mov   ecx,edx
		mov   edx,[edx]
		call  dword ptr [edx+4]
		sub   eax,Len
		mov   IDummy,eax
	}
	RETURN(IDummy)
}
////////////
//static Byte  *AllNew,*AllOld;
//static int    AllNLen/*,AllOLen*/;

//static int *FinalDiffLen;
/*
static struct _DiffStr{
	Byte *Old,*New;
	int   OLen,NLen;
} *DiffStr,DiffStrWoG;
static Byte *SoDDiff,*WoGDiff;
static int   OrigOLen,OrigNLen;
static int   NLen,OLen,DLen,LastDiffSectionIndex;
void CheckDiff(Byte *Diff,int len)
{
	int   *dp;
	Byte  *bp=Diff;
	int    shift=4;
	NLen=0; OLen=0; DLen=*((int *)bp);
	while(shift<len){
		dp=(int *)&bp[shift];
		LastDiffSectionIndex=shift;
		NLen+=dp[0];
		dp[2]&=0x000000FF;
		if(dp[2]){ // есть данные
			OLen+=dp[1];
			shift+=(12+dp[0]);
		}else{ // нет данных
			OLen+=dp[0];
			shift+=12;
		}
	}
}
static int  SoDNLen,SoDOLen;
static char Signature[]="ZvsRealSignature1703SPb";
int FindSignature(char *New,int NLen,char *Old,int OLen)
{
	int i;
	SoDNLen=-1; SoDOLen=-1;
	for(i=0;i<(NLen-strlen(Signature));i++){
		if(New[i]!='Z') continue;
		if(StrNCmp((Byte *)&New[i],(Byte *)Signature,strlen(Signature))){ SoDNLen=i; break; }
	}
	if(SoDNLen==-1) return 1;
	for(i=0;i<(OLen-strlen(Signature));i++){
		if(Old[i]!='Z') continue;
		if(StrNCmp((Byte *)&Old[i],(Byte *)Signature,strlen(Signature))){ SoDOLen=i; break; }
	}
	if(SoDOLen==-1) return 1;
	return 0;
}

void BuildWoGDiff(Byte *Diff,Byte *New,Byte *Old,int Len)
{
	int  *dp;
	Byte *bp=Diff;
	int   i,j,shift=0;
	int   SeqStart=0,NoNum=0,YesNum=0;
	for(i=0;i<Len;i++){
		if(New[i]!=Old[i]){ // несовпадение
			if(!NoNum){ // до этого были только совпадения - запишем
				dp=(int *)&bp[shift];
				dp[0]=YesNum; dp[1]=0; dp[2]=0; shift+=12;
				SeqStart=i; YesNum=0; NoNum=1;
			}else{ // уже накапливаем
				++NoNum;
			}
		}else{ // совпадение
			if(NoNum){ // было несовпадение
				if(StrNCmp(&New[i+1],&Old[i+1],23)==1){ // длинная послед совпадений далее >23 - запишем несовпадения
					dp=(int *)&bp[shift];
					dp[0]=NoNum; dp[1]=NoNum; dp[2]=1; shift+=12;
					for(j=0;j<NoNum;j++) bp[shift+j]=New[SeqStart+j];
					shift+=NoNum;
					SeqStart=i; NoNum=0; YesNum=1;
				}else{ // считаем как несовпадение
					++NoNum;
				}
			}else{
				++YesNum;
			}
		}
	}
	dp=(int *)&bp[shift];
	if(NoNum){ // последнее было несовпадение
		dp[0]=NoNum; dp[1]=NoNum; dp[2]=1; shift+=12;
		for(j=0;j<NoNum;j++) bp[shift+j]=New[SeqStart+j];
		shift+=NoNum;
	}else{
		dp[0]=YesNum; dp[1]=0; dp[2]=0; shift+=12;
	}
	WoGDiffLen=shift;
}

int AddWoGDiff(Byte *Diff,int SoDDiffLen)
{
	int  *dp;
	//static int    NLen,OLen,DLen,LastDiffSectionIndex;
	CheckDiff(Diff,SoDDiffLen);
	if(OLen>SoDOLen){ Message("Diff build Error 1.",1); return -1; }
	if(OLen<SoDOLen){
		dp=(int *)&Diff[LastDiffSectionIndex];
		dp[1]=SoDOLen-OLen;
	}
	//static int WoGDiffLen;
	//static Byte *WoGDiff;
	BuildWoGDiff(&DiffStr->New[SoDNLen],&DiffStr->Old[SoDOLen],OrigNLen-SoDNLen,,OrigOLen-SoDOLen);

	dp=(int *)Diff;
	dp[0]=OrigNLen;
	return(SoDDiffLen+WoGDiffLen);
	//static Byte  *AllNew,*AllOld;
	//static int    AllNLen,AllOLen;
}

Byte * pascal BuildAllDiff(int *difflen)
{
	DiffStr=(_DiffStr *)_ECX;
	int stdflag=0,maxwoglen;
	if(FindSignature((char *)DiffStr->New,(int)DiffStr->NLen,(char *)DiffStr->Old,(int)DiffStr->OLen)){ Message("Diff build Error 2.",1); stdflag=1; }
	if((DiffStr->NLen-SoDNLen)!=(DiffStr->OLen-SoDOLen)){ Message("Diff build Error 3.",1); stdflag=1; }
	if(stdflag==0){
		maxwoglen=DiffStr->OLen; if(maxwoglen<DiffStr->NLen) maxwoglen=DiffStr->NLen; maxwoglen+=0x1400;
		OrigNLen=DiffStr->NLen; OrigOLen=DiffStr->OLen;
		DiffStr->NLen=SoDNLen; DiffStr->OLen=SoDOLen;
	}
	asm{
		mov    eax,difflen
		mov    ecx,DiffStr
		push   eax
		mov    eax,0x490CC0
		call   eax
		mov    SoDDiff,eax
	}
	if(stdflag==0){
		DiffStr->NLen=OrigNLen; DiffStr->OLen=OrigOLen;
		if(SoDDiff==0){ Message("Diff build Error 4.",1); return SoDDiff; }
		if(*difflen==0){ Message("Diff build Error 5.",1); return SoDDiff; }
		WoGDiff=(Byte *)Alloc(maxwoglen);
		if(WoGDiff==0){ Message("Diff build Error 4.",1); return SoDDiff; }
		for(int i=0;i<*difflen;i++) WoGDiff[i]=SoDDiff[i];
		stdflag=AddWoGDiff(WoGDiff,*difflen);
		if(stdflag==-1){ Message("Diff build Error 4.",1); Free(WoGDiff); return SoDDiff; }
		Free(SoDDiff);
		*difflen=stdflag;
		return WoGDiff;
	}
	return SoDDiff;
}
*/
//////////////
static void *NonZipped[100];
void *IsOurFile(int h)
{
	if((h<1)||(h>99)) return 0;
	return NonZipped[h];
}
int AddNonZipped(char *path,char *mode)
{
	void *hp;
	STARTNA(__LINE__, 0)
	__asm{
		mov    ebx,mode
		mov    eax,path
		push   ebx
		push   eax
		mov    eax,0x619691
		call   eax
		add    esp,8
	}
	_EAX(hp);
	if(hp==0) RETURN(0)
	for(int i=1;i<100;i++){
		if(NonZipped[i]==0){
			NonZipped[i]=hp;
			RETURN(i)
		}
	}
	RETURN(0)
}
void DelNonZipped(int h)
{
	STARTNA(__LINE__, 0)
	void *hp=NonZipped[h];
	__asm{
		mov    eax,hp
		push   eax
		mov    eax,0x618F5E
		call   eax
		add    esp,4
	}
	NonZipped[h]=0;
	RETURNV
}

int WriteNonZipped(int h,Byte *buf,int len)
{
	STARTNA(__LINE__, 0)
	Byte  c;
	void *hp=NonZipped[h];
	for(int i=0;i<len;i++){
		c=buf[i];
		__asm{
			xor    ebx,ebx
			mov    bl,c
			mov    eax,hp
			push   eax
			push   ebx
			mov    eax,0x618B53
			call   eax
			add    esp,8
		}
	}
	RETURN(len)
}
//////////////
static Dword GZOpen_Ecx,GZOpen_Edx;
int __stdcall GZOpen(int par)
{
	_ECX(GZOpen_Ecx); 
	_EDX(GZOpen_Edx);
	STARTNA(__LINE__, 0)
	char *mode=(char *)GZOpen_Edx;
	if((mode[0]=='w')&&(mode[2]=='0')){
		RETURN(AddNonZipped((char *)GZOpen_Ecx,(char *)GZOpen_Edx))
	}else{
		__asm{
			mov   eax,par
			mov   edx,GZOpen_Edx
			mov   ecx,GZOpen_Ecx
			push  eax
			mov   eax,0x6067F0
			call  eax
			mov   IDummy,eax
		}
		RETURN(IDummy)
	}
}
static Dword GZClose_Ecx;
int __stdcall GZClose(void)
{
	_ECX(GZClose_Ecx);
	STARTNA(__LINE__, 0)
	if(IsOurFile((int)GZClose_Ecx)){
		DelNonZipped((int)GZClose_Ecx);
		RETURN(0)
	}else{
		__asm{
			mov   ecx,GZClose_Ecx
			mov   eax,0x607080
			call  eax
			mov   IDummy,eax
		}
		RETURN(IDummy)
	}
}
static Dword GZWrite_Ecx,GZWrite_Edx;
int __stdcall GZWrite(int len)
{
	_ECX(GZWrite_Ecx); 
	_EDX(GZWrite_Edx);
	STARTNA(__LINE__, 0)
	if(IsOurFile((int)GZWrite_Ecx)){
		RETURN(WriteNonZipped((int)GZWrite_Ecx,(Byte *)GZWrite_Edx,len))
	}else{
		__asm{
			mov   eax,len
			mov   edx,GZWrite_Edx
			mov   ecx,GZWrite_Ecx
			push  eax
			mov   eax,0x606EF0
			call  eax
			mov   IDummy,eax
		}
		RETURN(IDummy)
	}
}
//////////////
struct _DiffStr{ Byte *Old,*New; int OLen,NLen; };
static int LastDiffSectionIndex;
int CheckDiff(Byte *Diff,int len)
{
	STARTNA(__LINE__, 0)
	int   *dp,olen=0;
	Byte  *bp=Diff;
	int    shift=4;
	while(shift<len){
		dp=(int *)&bp[shift];
		LastDiffSectionIndex=shift;
		dp[2]&=0x000000FF;
		if(dp[2]){ // есть данные
			olen+=dp[1];
			shift+=(12+dp[0]);
		}else{ // нет данных
			olen+=dp[0];
			shift+=12;
		}
	}
	RETURN(olen)
}
static _DiffStr DiffStrSoD;
static Byte    *SoDDiff;
static int      SoDDiffLen;
int BuildSoDDiff(Byte *pnew,Byte *pold,int nlen,int olen)
{
	STARTNA(__LINE__, 0)
	int  *dp,len;
	DiffStrSoD.New=pnew;
	DiffStrSoD.Old=pold;
	DiffStrSoD.NLen=nlen;
	DiffStrSoD.OLen=olen;
	__asm{
		lea    eax,SoDDiffLen
		lea    ecx,DiffStrSoD
		push   eax
		mov    eax,0x490CC0
		call   eax
		mov    SoDDiff,eax
	}
	if(SoDDiff==0){ Message("Diff build Error 4.",1); RETURN(1) }
	if(SoDDiffLen==0){ Message("Diff build Error 5.",1); RETURN(1) }
	len=CheckDiff(SoDDiff,SoDDiffLen);
	if(len>olen){ Message("Diff build Error 1.",1); RETURN(1) }
	if(len<olen){
		dp=(int *)&SoDDiff[LastDiffSectionIndex];
		dp[1]=olen-len;
	}
	RETURN(0)
}
//static _DiffStr DiffStrWoG;
static Byte    *WoGDiff;
static int      WoGDiffLen;
/*
int BuildWoGDiff(Byte *pnew,Byte *pold,int nlen,int olen)
{
	int  *dp,len;
	DiffStrWoG.New=pnew;
	DiffStrWoG.Old=pold;
	DiffStrWoG.NLen=nlen;
	DiffStrWoG.OLen=olen;
	asm{
		lea    eax,WoGDiffLen
		lea    ecx,DiffStrWoG
		push   eax
		mov    eax,0x490CC0
		call   eax
		mov    WoGDiff,eax
	}
	if(WoGDiff==0){ Message("Diff build Error 4.",1); return 1; }
	if(WoGDiffLen==0){ Message("Diff build Error 5.",1); return 1; }
	len=CheckDiff(WoGDiff,WoGDiffLen);
	if(len>olen){ Message("Diff build Error 1.",1); return 1; }
	if(len<olen){
		dp=(int *)&WoGDiff[LastDiffSectionIndex];
		dp[1]=olen-len;
	}
	return 0;
}
*/
//static int (*SaveSetupState_c)(char *Name,void *Buf,int Len);
//static int (*LoadSetupState_c)(char *Name,void *Buf,int Len);
/*
int UnZipBytes(Byte *in,Byte* out,int len)
{
	int i=0,j=0;
	Word n,Shift=11;
	Byte b;
	do{
		b=in[Shift];
		n=*(Word *)&in[Shift+1];
		for(i=0;i<n;i++,j++) out[j]=in[Shift+5+i];
		Shift+=n+5;
	}while((b&1)==0);
}
*/
//void BuildWoGDiff(Byte *Diff,Byte *New,Byte *Old,int Len)
int BuildWoGDiff(Byte *pnew,Byte *pold,int nlen,int olen)
{
	STARTNA(__LINE__, 0)
	int maxlen,*dp;
	Byte *bp;
	int   i,j,k,shift=0;
	int   iSeqStart=0,jSeqStart=0,NoNum=0,YesNum=0;
	maxlen=nlen; if(olen>maxlen) maxlen=olen; maxlen+=0x1400;
	WoGDiff=(Byte *)Alloc(maxlen); if(WoGDiff==0){ Message("Diff build Error 1.",1); RETURN(1) }
	bp=WoGDiff;
	for(i=j=0;i<nlen;i++,j++){
/*
		if(pnew[i]==0){ // может быть вставка 00,00,80,FF,7F
			if(*(Dword *)&pnew[i+1]==0x7FFF8000){ // она
				if(pold[j]==0){ // может и здесь она
					if(*(Dword *)&pold[j+1]==0x7FFF8000){ // она
						// ничего не делаем - удачное совпадение
						goto _Done;
					}
				}
				// проверим, код далее на совпадение
				if(StrNCmp(&pnew[i+5],&pold[j],4)==1){ // совпадает - считаем вставкой
					if(NoNum){ // уже было несовпадение
						NoNum+=5;
					}else{ // не было несовпадений - запишем и начнем
						dp=(int *)&bp[shift];
						dp[0]=YesNum; dp[1]=0; dp[2]=0; shift+=12;
						iSeqStart=i; jSeqStart=j;
						YesNum=0; NoNum=5;
					}
					i+=5;
				}else{ // не совпадает - не вставка (допустим)
				}
			}
		}
		if(pold[j]==0){
			if(*(Dword *)&pold[j+1]==0x7FFF8000){ // она
				// проверим, код далее на совпадение
				if(StrNCmp(&pnew[i],&pold[j+5],4)==1){ // совпадает - считаем вставкой
					if(NoNum){ // уже было несовпадение
					}else{ // не было несовпадений - запишем и начнем
						dp=(int *)&bp[shift];
						dp[0]=YesNum; dp[1]=0; dp[2]=0; shift+=12;
						iSeqStart=i++; jSeqStart=j++; // продвинем на 1 байт, иначе никак сейчас
						YesNum=0; NoNum=1;
					}
					j+=5;
				}else{ // не совпадает - не вставка (допустим)
				}
			}
		}
_Done:
*/
/*
		if(pnew[i]!=pold[j]){ // несовпадение
			if(StrNCmp(&pnew[i+5],&pold[j],16)==1){ // совпадает - считаем вставкой
				if(NoNum){ // уже было несовпадение
					NoNum+=5;
				}else{ // не было несовпадений - запишем и начнем
					dp=(int *)&bp[shift];
					dp[0]=YesNum; dp[1]=0; dp[2]=0; shift+=12;
					iSeqStart=i; jSeqStart=j;
					YesNum=0; NoNum=5;
				}
				i+=5;
			}else{ // не совпадает - не вставка (допустим)
			}
			if(StrNCmp(&pnew[i],&pold[j+5],16)==1){ // совпадает - считаем вставкой
				if(NoNum){ // уже было несовпадение
				}else{ // не было несовпадений - запишем и начнем
					dp=(int *)&bp[shift];
					dp[0]=YesNum; dp[1]=0; dp[2]=0; shift+=12;
					iSeqStart=i++; jSeqStart=j++; // продвинем на 1 байт, иначе никак сейчас
					YesNum=0; NoNum=1;
				}
				j+=5;
			}else{ // не совпадает - не вставка (допустим)
			}
		}
*/
		if(pnew[i]!=pold[j]){ // несовпадение
			if(!NoNum){ // до этого были только совпадения - запишем
				dp=(int *)&bp[shift];
				dp[0]=YesNum; dp[1]=0; dp[2]=0; shift+=12;
				iSeqStart=i; jSeqStart=j;
				YesNum=0; NoNum=1;
			}else{ // уже накапливаем
				++NoNum;
			}
		}else{ // совпадение
			if(NoNum){ // было несовпадение
				if(memcmp(&pnew[i+1],&pold[j+1],23)==0){ // длинная послед совпадений далее >23 - запишем несовпадения
					dp=(int *)&bp[shift];
					dp[0]=NoNum; dp[1]=j-jSeqStart; dp[2]=1; shift+=12;
					for(k=0;k<NoNum;k++) bp[shift+k]=pnew[iSeqStart+k];
					shift+=NoNum;
					iSeqStart=i; jSeqStart=j;
					NoNum=0; YesNum=1;
				}else{ // считаем как несовпадение
					++NoNum;
				}
			}else{
				++YesNum;
			}
		}
	}
	dp=(int *)&bp[shift];
	if(NoNum){ // последнее было несовпадение
		dp[0]=NoNum; dp[1]=NoNum; dp[2]=1; shift+=12;
		for(k=0;k<NoNum;k++) bp[shift+k]=pnew[iSeqStart+k];
		shift+=NoNum;
	}else{
		dp[0]=YesNum; dp[1]=0; dp[2]=0; shift+=12;
	}
	WoGDiffLen=shift;
	RETURN(0)
}
static int  SoDNLen,SoDOLen;
static char Signature[]="ZvsRealSignature1703SPb";
int FindSignature(char *New,int NLen,char *Old,int OLen)
{
	STARTNA(__LINE__, 0)
	int i;
	SoDNLen=-1; SoDOLen=-1;
	for(i=0;i<(NLen-(int)strlen(Signature));i++){
		if(New[i]!='Z') continue;
		if(memcmp((Byte *)&New[i],(Byte *)Signature,strlen(Signature))==0){ SoDNLen=i; break; }
	}
	if(SoDNLen==-1) RETURN(1)
	for(i=0;i<(OLen-(int)strlen(Signature));i++){
		if(Old[i]!='Z') continue;
		if(memcmp((Byte *)&Old[i],(Byte *)Signature,strlen(Signature))==0){ SoDOLen=i; break; }
	}
	if(SoDOLen==-1) RETURN(1)
	RETURN(0)
}
static Byte     *AllDiff;
static _DiffStr *DiffStr;
Byte * __stdcall BuildAllDiff(int *difflen)
{
	_ECX(DiffStr);
	STARTNA(__LINE__, 0)
	int i,stdflag=0;
	if(FindSignature((char *)DiffStr->New,(int)DiffStr->NLen,(char *)DiffStr->Old,(int)DiffStr->OLen)){ Message("Diff build Error 2.",1); stdflag=1; }
	if(stdflag==0){
		if(BuildSoDDiff(DiffStr->New,DiffStr->Old,SoDNLen,SoDOLen)){ Message("Diff build Error 2.",1); stdflag=1; }
	}
	if(stdflag==0){
		if(BuildWoGDiff(&DiffStr->New[SoDNLen],&DiffStr->Old[SoDOLen],DiffStr->NLen-SoDNLen,DiffStr->OLen-SoDOLen)){ Message("Diff build Error 2.",1); stdflag=1; }
	}
	if(stdflag==0){
		AllDiff=(Byte *)Alloc(SoDDiffLen+WoGDiffLen);
		if(AllDiff==0){ Message("Diff build Error 4.",1); stdflag=1; }
	}
	if(stdflag==0){
		for(i=0;i<SoDDiffLen;i++) AllDiff[i]=SoDDiff[i];
		for(i=0;i<WoGDiffLen;i++) AllDiff[i+SoDDiffLen]=WoGDiff[i];
		Free(SoDDiff);
		Free(WoGDiff);
		*((int *)AllDiff)=DiffStr->NLen;
		*difflen=SoDDiffLen+WoGDiffLen;
		RETURN(AllDiff)
	}
	RETURN(0)
}

////////////////
/*
int WriteAlignment()
{
	if(Saver(Signature,strlen(Signature))) return 1;
}
int SkipAlignment()
{
}
*/
char MapSavedWoG[1024];

int SaveSignature(void)
{
	STARTNA(__LINE__, 0)
	char *str=WOG_STRING_VERSION;
	int len=strlen(str);
	if(Saver(&len,sizeof(len))) RETURN(1)
	if(Saver(str,len)) RETURN(1)
	if(Saver(Signature,strlen(Signature))) RETURN(1)
	RETURN(0)
}
int LoadSignature(int)
{
	STARTNA(__LINE__, 0)
	int  len;
	char buf[64];
	if(Loader(&len,sizeof(len))) RETURN(1)
	if(Loader(MapSavedWoG,len)) RETURN(1)
	if(Loader(buf,strlen(Signature))) RETURN(1)
	buf[63] = 0;
	if(memcmp((Byte *)buf,(Byte *)Signature,strlen(Signature))!=0) RETURN(1)
	RETURN(0)
}

//int RealCurrentUser=0;
static int ActTurn=0,AutoTurn=1;
void SaveManager(void)
{
//  char *MapName;
	STARTNA(__LINE__, 0)
	int done=0;
	//if(WoG==0) RETURNV
	do{
/*
		asm{
			mov   eax,BASE
			mov   eax,[eax]
			add   eax,0x1F4D5
			mov   MapName,eax
		}
//    if((RealCurrentUser==-1)&&(IsThis(RealCurrentUser)==0)&&(StrNCmp((Byte *)MapName,(Byte *)"NEWGAME",8)==0)){
		if((RealCurrentUser==-1)||(IsThis(RealCurrentUser)==0)){
*/
		if((ActTurn==0)&&(AutoTurn==0)){
			Message(/*ITxt(28,0,&Strings)*/
				"{WARNING!}\n\nYou cannot write down the game at not your turn.\n"
				"This may cause a problem when you load it\n"
				"Game still will be saved, but you are warned!\n\n"
				"We recommend you now:\n"
				"- wait until your turn and save the game again;\n"
				"- ask an active player to save the game at his/her side (through the chat)."
				,1);
			if(Saver("ZVZS",4)) break;
		}else{
			if(Saver("ZVSS",4)) break;
		}
		if(SaveSignature()) break;
//    if(Saver(CastleState,CASTLESNUM*sizeof(_CastleState_))) break;
		if(SaveCustom()) break; // fixed size
		if(SaveCasDem()) break; // fixed size
		if(SaveMapMon()) break; // fixed size
		if(SaveAnimations()) break;  // fixed size
		if(SaveERM()) break; // fixed size
		if(SaveCurse()) break; // fixed size
		if(SaveB1()) break; // fixed size
		if(SaveWM()) break; // fixed size
		if(SaveMP3()) break; // fixed size
		if(SaveNPC()) break; // fixed size
		if(SaveAI()) break;
		if(CrExpoSet::Save()) break; // 3.58 R
		if(CrExpMod::Save()) break; // 3.58 R
		if(CrExpBon::Save()) break; // 3.58 R
		if(SaveA160()) break; // 3.58 R
		if(SaveLTimer()) break; // 3.59
		if(SaveNewTown()) break; // 3.59
		if(SaveSpells()) break; // 3.59
		if(SaveLODs()) break; // 3.59
//...
		if(Saver("ZVS",3)) break;
		done=1;
	}while(0);
	if(done==0){
		// не записалось что-то
//    Message("{Error}:\n\nGame was not saved properly or old version.",1);
		Message(ITxt(28,0,&Strings),1);
	}
	RETURNV
}

void LoadManager(void)
{
	STARTNA(__LINE__, 0)
	int done=0,ver;
	char buf[10];
	//if(WoGatLoad==0){ WoG=0; RETURNV }
	do{
		ActTurn=1;
		ThisIsNewGame=0; // не новая, а загруженная
		if(Loader(buf,4)) break;
		if(buf[0]!='Z') break;
		if(buf[1]!='V') break;
		if(buf[2]=='Z'){
			Message("{WARNING!}\n\nThis game was saved in multiplayer mode (not a hotseat)\n"
				"by a {non-active} player.\n"
				"The saved game will be loaded but there may be some problems.\n\n"
				"We recommend you to load another saved game or autosaved one."
				,1);
		}else{
			if(buf[2]!='S') break;
		}
		ver=buf[3]-'A';
		// 18...
		//version 18(S + 3.59)
		//version 17(R + Creature Expo Save+ ComboArts Save)
		//version 16(Q + AI thinking delay)
		//version 15(P + WoG date that map is saved)
		if(ver<SAVEWOG359) { Message("Games saved in WoG version below 3.59 are not supported",1); break; }
		if(LoadSignature(ver)) break;
		if(LoadCustom(ver)) break;
		if(LoadCasDem(ver)) break;
		if(LoadMapMon(ver)) break;
		if(LoadAnimations(ver)) break;
		if(LoadERM(ver)) break;
		if(LoadCurse(ver)) break;
		if(LoadB1(ver)) break;
		if(LoadWM(ver)) break;
		if(LoadMP3(ver)) break;
		if(LoadNPC(ver)) break;
		if(LoadAI(ver)) break;
		if(CrExpoSet::Load(ver)) break; // 3.58 R
		if(CrExpMod::Load(ver)) break; // 3.58 R
		if(CrExpBon::Load(ver)) break; // 3.58 R
		if(LoadA160(ver)) break; // 3.58 R
		if(LoadLTimer(ver)) break; // 3.59
		if(LoadNewTown(ver)) break; // 3.59
		if(LoadSpells(ver)) break; // 3.59
		if(LoadLODs(ver)) break; // 3.59
		
//...
		if(Loader(buf,3)) break; // последняя проверка
		if(buf[0]!='Z'||buf[1]!='V'||buf[2]!='S') break;
// загрузку ERM сделаем здесь после всех загрузок
		FindERM();
		done=1;
	}while(0);
	if(done==0){
		// не записалось что-то
//    Message("{Error}:\n\nGame was not loaded properly.",1);
		Message(ITxt(27,0,&Strings),1);
	}
	RETURNV
}
/*
void BetaVersionMessage(void)
{
	Message(
"{You are playing a beta version of 3.58!}\n\n"
"Please read file WOG358BETA.TXT for instructions about error reporting and following them carefully\n\n"
"Please do not give copies of this beta version to anyone nor make it available for download by others unless specifically "
"authorized by the WoG Team. Also, please restrict discussion of the 3.58 beta to the wakeofgods list or private email with "
"other beta testers and WoG Team members. We also ask that you do not post or leak information or screenshots from the beta."
		,1);
}
*/
void SaveGame(void)
{
	_ESI(LoadSave_Pointer);
	__asm pusha
	GameBeforeSave();
	SaveManager();
	OriginalCallPointer=Callers[5].forig;
	__asm popa;
	__asm call dword ptr [OriginalCallPointer]
}


// связано со след. функцией. При загрузке многоп. игры (Network/Hotseat)
// на сервере, настройка "игрока за этим PC" проходит позже.
void __fastcall LoadGameSetUpPlayers(Dword obj,Dword, Dword par){
	void (__fastcall *fn)(Dword,Dword,Dword); *(Dword *)&fn=0x588600;
	fn(obj,0,par);
	GameAfterLoad();
}
void LoadGame(void)
{
	_EBX(LoadSave_Pointer);
	__asm pusha

//BetaVersionMessage();

	LoadManager();
	OriginalCallPointer=Callers[6].forig;
	__asm popa;
	__asm call dword ptr [OriginalCallPointer]
	__asm pusha
		if(HasAnyAtThisPC()){
			GameAfterLoad();
		}
	__asm popa
}
/*
void CheckMapTypeN(void)
{
	asm  pusha
	if(_EAX==4) OriginalCallPointer=0x4C4159;
	else OriginalCallPointer=0x4C4143;
	asm  mov eax,[ebx]
	if(_EAX==SOGMAPTYPE){
		asm mov byte ptr [ebx],SODMAPTYPE
		asm mov al,SOGID
		asm mov byte ptr [ebx+IDSHIFT],al
	}else{
		asm mov al,SODID
		asm mov byte ptr [ebx+IDSHIFT],al
	}
	asm  popa
}

void FileTypeN(void)
{
	CheckMapTypeN();
	asm pop  eax
	asm push dword ptr [OriginalCallPointer]
}

void CheckMapTypeL(void)
{
	asm  pusha
	if(_EAX==4) OriginalCallPointer=0x4C533E;
	else OriginalCallPointer=0x4C5328;
	asm  mov eax,[esi]
	if(_EAX==SOGMAPTYPE){
		asm mov byte ptr [esi],SODMAPTYPE
		asm mov al,SOGID
		asm mov byte ptr [esi+IDSHIFT],al
	}else{
		asm mov al,SODID
		asm mov byte ptr [esi+IDSHIFT],al
	}
	asm  popa
}

void FileTypeL(void)
{
	CheckMapTypeL();
	asm pop  eax
	asm push dword ptr [OriginalCallPointer]
}
*/

__declspec( naked ) int CheckFileTypeN(void)
{
//  asm mov   al,[esi+IDSHIFT]
	__asm{ 
		sub   al,0x1C
		setne al
		movzx eax,al
		ret
	}
//asm pusha
//PrepareSpecWoG(_EAX);
//asm popa
}
/*
void CheckFileTypeL(void)
{
	asm mov   al,[esi+IDSHIFT]
	asm movzx eax,al
}

Dword  SGP_type;
Byte  *SGP_po;
void pascal SaveGamePrepare(Dword)
{
	asm pusha
	asm mov   al,[esi+IDSHIFT]
	asm movzx eax,al
	SGP_type=_EAX;
	SGP_po=(Byte *)_ESI;
	if(SGP_type==SOGID) *SGP_po=SOGMAPTYPE;
// уже не привязан!!! надо снова привязывать
	OriginalCallPointer=Callers[8].forig;
	asm popa
	asm push  esi
	asm call  dword ptr [OriginalCallPointer]
	asm pusha
	if(SGP_type==SOGID) *SGP_po=SODMAPTYPE;
	asm popa
}
*/
/////////////////////////////////
int FindManager(Dword po)
{
	STARTNA(__LINE__, 0)
	int   i;
	char *mn=(char *)(po+0x14);
	for(i=0;ManName[i]!=0;i++){
		if(StrCmp(ManName[i],mn)){ // found
			RETURN(i)
		}
	}
	RETURN(-1)
}

void __stdcall ServiceManager(Dword)
{
	Dword  Pointer;
	_EDI(Pointer);
	__asm pusha
//  edi - указатель на менеджер (+38 -> замок)
	switch(FindManager(Pointer)){ // не наш менеджер
		case 1: // CastleManager
			CastleExit(Pointer); break;
		default:;
	}
	OriginalCallPointer=Callers[8].forig;
	__asm popa
	__asm push edi
	__asm call dword ptr [OriginalCallPointer]
}

////////////////////////////////////
/*
void cbFunWater(int x,int y,int l)
{
	_MapItem_ *MIp;
	MIp=GetMapItem(x,y,l);
	MIp->Land=4; // болото
	PlaceObject(x,y,l,208,1);
}

void PathOverWater(int x1,int y1,int x2,int y2,int l)
{
	MakeLine(x1,y1,x2,y2,l,cbFunWater);
}
*/
////////////////////////////////////
// в начале каждого дня
/*
void CalledDayly(void)
{
	asm pusha
	GlbAnimation();
	CastleTowers();
	DaylyMonstr();
	asm popa
}
// перед передачей управления самому первому игроку
void CalledBeforeTurn1(void)
{
	int cu;
	asm pusha
	if(ThisIsNewGame){ // для загруженной игры уже было вызвано
		cu=CurrentUser();
		DaylyCastle(cu);
		DaylySG(cu);
		MakeDarkness(cu);
		DaylyGodsBonus(cu);
		DaylyCurse(cu);
		RunTimer(cu);
//...
	}
	asm popa
	asm{
		mov   eax,0x4B0BA0
		call  eax
	}
}
// перед передачей управления игроку
void CalledBeforeTurn(void)
{
	int cu;
	asm pusha
	cu=CurrentUser();
	DaylyCastle(cu);
	DaylySG(cu);
	MakeDarkness(cu);
	DaylyGodsBonus(cu);
	DaylyCurse(cu);
	RunTimer(cu);
//...
	asm popa
}
*/
int LastTurnedDay=-1; // день когда сработал последний раз
// перед передачей управления самому первому игроку
void CalledBeforeTurn1New(void)
{
	STARTNA(__LINE__, 0)
	__asm pusha
	int cu;
	if((ThisIsNewGame||          // для загруженной игры уже было вызвано
			(LastTurnedDay==-1))&&   // еще не записывали после начала - первый запуск
		 (IsThis(CurrentUser())) // первые, и он ходит
		){
		LastTurnedDay=GetCurDate();
			GlbAnimation();
			CastleTowers();
			DaylyMonstr();
			DaylyWoMo();
			ResetNPCExp(-1);
			DaylyCastleKey();
			CrExpoSet::DaylyAIExperience(-1);
		cu=CurrentUser();// RealCurrentUser=cu;
		ActTurn=1;
//    DaylyCastleKey(cu);
		DaylyCastle(cu);
		DaylySG(cu);
		MakeDarkness(cu);
		DaylyGodsBonus(cu);
		DaylyCurse(cu);
// WM Legion + Invisible bug
		CompleteWoMo(cu);
		DaylyNPC(cu);
		CrExpoSet::DaylyAIExperience(cu);
		RunTimer(cu);
//...
	}
	__asm popa
	STOP
	__asm{
		mov   eax,0x4CD5D0 // обработка GE
		call  eax
	}
}
// перед передачей управления игроку
void CalledBeforeTurnNew(void)
{
	STARTNA(__LINE__, 0)
	__asm pusha
	int cu;
	if(LastTurnedDay!=GetCurDate()){
		GlbAnimation();
		CastleTowers();
		DaylyMonstr();
		DaylyWoMo();
		DaylyMonChanged();
		DaylyCastleKey();
		LastTurnedDay=GetCurDate();
	}
	cu=CurrentUser(); // RealCurrentUser=cu;
	ActTurn=1;
//  DaylyCastleKey(cu);
	DaylyCastle(cu);
	DaylySG(cu);
	MakeDarkness(cu);
	DaylyGodsBonus(cu);
	DaylyCurse(cu);
	CompleteWoMo(cu);
	DaylyNPC(cu);
	CrExpoSet::DaylyAIExperience(cu);
	RunTimer(cu);
//...
	__asm popa
	STOP
	__asm{
		mov   eax,0x4CD5D0 // обработка GE
		call  eax
	}
}

//static int OutOfPC=-1;
// перед передачей управления ДРУГОМУ игроку
void CalledAfterTurn(void)
{
	__asm pusha
/////  RunPostTimer(CurrentUser());
//...
//  OutOfPC=RealCurrentUser;
//  START("Player's turn Finalization");
	ActTurn=0;
	AutoTurn=1;
	__asm popa
	__asm{
		mov   eax,0x4C6CA0
		call  eax
	}
	__asm pusha
//  if(OutOfPC==RealCurrentUser){ // игрок НЕ сменился - передали по сети
//    RealCurrentUser=-1; // нельзя записывать
//  }
	AutoTurn=0;
	__asm popa
}

void MPGameMessage(void)
{
	Message(
"{IMPORTANT notes for network based MP games!}\n\n"
"WoG 3.58 was successfully tested in a network based TCP/IP mode.\n\n"
"If you have a problem with an MP game, please read the file \"WoG & Network.txt\" to find a solution.\n"
"Also, not all WoGifying scripts will work correctly in Human vs Human MP battles (also some if the AI attacks a distant "
"player). These scripts will be automatically disabled in network Human vs Human MP battles, and we have prepared a text "
"file listing which combat scripts should work correctly and which are currently disabled.\n"
"This reference file is \"WoG MP Compatibility.txt\"."
		,1);
}
static int NSL_GameType;
void __stdcall NewStartLoading(Dword,Dword)
{
	__asm pusha
	__asm push  edx
	__asm push  edi
	OriginalCallPointer=Callers[10].forig;
	__asm call dword ptr [OriginalCallPointer]

//BetaVersionMessage();
	__asm{
		mov   eax,0x698A40
		mov   eax,[eax]
		mov   NSL_GameType,eax
	}
	if((NSL_GameType!=0)&&(NSL_GameType!=3)){ // not singl and not hot seat
		MPGameMessage();
	}
	ResetAll();
	__asm popa;
}

/////////////////////////////////
static int wog_gc;
void GC_PreLoadAll(int SINGLEcampaign)
{
//asm int 3
	__asm{
		mov  eax,BASE
		mov  eax,[eax]
		add  eax,0x1F86C
		mov  eax,[eax]
		mov  DDummy,eax
	}
	if(DDummy!=SOGMAPTYPE) wog_gc=0; else wog_gc=1;
	
	//if(PL_ApplyWoG>1) wog_gc=1;
	if(LoadCrTraits(1)==0) Exit();
	//if(LoadCrTraits(wog_gc)==0) Exit();
//  if(wog_gc){
		if(SINGLEcampaign){
			if(LoadCustomTxt(1,1)==0) Exit();
			PrepareSpecWoG(wog_gc);
		}
    //CleanMemmory(); //Diakon
    //LoadNewMonstrs(); // Diakon	
	ResetLoadDefaultResist(); //Diakon
	ResetLoadHarpyAtack(); //Diakon
	//LoadNewMonstrs(&TStructOfMem); //Diakon
//  }
}
// вызывается для новой и для загрузки
void GameChoosen(void)
{
	__asm pusha
	GC_PreLoadAll(1);
	OriginalCallPointer=Callers[17].forig;
	__asm popa;
	__asm call dword ptr [OriginalCallPointer]
}
// вызывается для новой карты из компании
Dword GameChoosen2(void)
{
	__asm pusha
	BackupNPC(); // 3.58
	GC_PreLoadAll(0);
	__asm popa
	__asm mov   eax,BASE
	__asm mov   eax,[eax]
	__asm mov   DDummy,eax
	return DDummy;
}
// вызывается для рестарта карты
void __stdcall GameChoosen3(int,int)
{
	__asm pusha
	GC_PreLoadAll(0);
	__asm popa
	__asm{
		mov    ecx,0x6992B0
		mov    ecx,[ecx]
		push   1
		push   1
		mov    eax,0x50CEA0
		call   eax
	}
}
void TutorialGame(void)
{
	__asm pusha
	GC_PreLoadAll(1);
	__asm popa
	__asm{
		mov   eax,0x4ED930
		call  eax
	}
}

static Dword LM_Len,LM_Ebx;
static Byte TmpBuf[500];
void LoadProtected(void)
{
	__asm{
		pusha
		mov    LM_Ebx,ebx
	}
	STARTNA(__LINE__, 0)
	__asm{
		mov    eax,[ebx]
		cmp    eax,0x34
		jb     l_std
		jmp    l_prot
	}
l_wrong:
__asm{
		mov    ebx,LM_Ebx
		mov    dword ptr [ebx],0x01
		jmp    l_std
	}
l_prot:
	__asm{
		mov    dword ptr [ebx],0x33
		xor    ecx,ecx
		mov    cl,al
		mov    ah,al
		and    al,0xF0
		shr    al,4
		and    ah,0x0F
		cmp    ah,al
		ja     l_al
		mov    al,ah
	}
l_al:
	__asm{
		xor    ah,ah
		mov    dl,cl
		and    dl,0xF0
		and    cl,0x0F
		shr    dl,4
		shl    cl,4
		or     cl,dl
		add    cx,ax
		dec    ecx
		mov    LM_Len,ecx
		push   ecx
		lea    ecx,TmpBuf
		push   ecx
		mov    ecx,edi
		mov    eax,[edi]
		call   [eax+4]
		mov    ecx,LM_Len
		sub    ecx,3
		mov    ax,word ptr [TmpBuf+ecx]
		or     ax,ax
		jne    l_wrong
		add    ecx,2
		mov    al,[TmpBuf+ecx]
		or     al,al
		jne    l_wrong
	}
l_std:
	STOP
	__asm{
		popa
	}
}

//////////////////////////////////////
/*
void InitArmore(void)
{
//!!! Здесь важно, что в стеке меняется адрес возврата
// в оригинале должно быть:
//    mov    eax,[ebx+0x34]
//    add    eax,0xFFFFFFFE
	asm{
		mov    eax,[ebx+0x34]
		add    eax,0xFFFFFFFE
		cmp    eax,152-2  // проверка на нового титана
		je    _found150
		cmp    eax,169-2  // проверка на красного монаха
		je    _found169
		cmp    eax,170-2  // проверка на белого шутера
		je    _found170
		cmp    eax,171-2  // проверка на красного шутера
		je    _found171
		jne   _notfound
	}
_found150:
	asm{
		pop    ebx
		pop    ecx
		mov    ecx,0x43DB13
		push   ecx
		push   ebx
	}
_found169:
_found170:
_found171:
	asm{
		pop    ebx
		pop    ecx
		mov    ecx,0x43DAD4
		push   ecx
		push   ebx
	}  
_notfound:  
}
void Horror(void)
{
	asm{
		
	}
}
*/
/*
char *Mess1[]={"Сообщение 1","Ответ 1","Ответ 2","Ответ 3",0};
char *Mess2[]={"Сообщение 2","Ответ 1","Ответ 2",0};

char **Messs[]={
	Mess1,Mess2,0
};

char ***Messages=Messs;

void fun111(void)
{
	// Формирование списка сообщений
	char **mes;
	char  *mestxt;
	for(int i=0;;i++){
		mes=Messages[i]; // char**
		if(mes==0) break;     // больше нет сообщений
		mestxt=mes[0]; // указатель на само сообщение i
		// здесь mestxt можно запихнуть в список для вывода
	}

	// Формирование списка ответов на i-е сообщение
//  char **mes;
	char  *repltxt;
	// i=номер поступившего сообщения
	mes=Messages[1]; // char**
	for(int j=0;;j++){
		repltxt=mes[j+1];   // указатель на ответ на сообщение i
		if(repltxt==0) break; // больше нет ответов на сообщение
		// здесь repltxt можно запихнуть в список для вывода
	}
}
*/
static int AMA_Ret,AMA_Type,AMA_Num;
static _Hero_ *AMA_Hp;
int AddMithrillAuto(int,int,int,int ZType,int Num)
{
	__asm mov    eax,ZType
	__asm mov    AMA_Type,eax;
	__asm mov    eax,Num
	__asm mov    AMA_Num,eax;
	__asm mov    AMA_Hp,esi;
	__asm pusha
	STARTNA(__LINE__, 0)
	if(ZType<0) AMA_Ret=0;
	else if(ZType>7) AMA_Ret=0;
	else if(ZType==7){ MithrillVal[AMA_Hp->Owner]+=Num; AMA_Ret=0; }
	else AMA_Ret=1;
	STOP
	__asm popa
//  __asm mov    eax,AMA_Ret
	return AMA_Ret;
}
int Enter2H4Obj(int GM_ai,_MapItem_ *Mi,_Hero_ *Hr,Dword /*avd.Manager*/,Dword* /*Object*/)
{
	int st;

//CustomReq(1);
	STARTNA(__LINE__, 0)
	if(Mi->OType!=63) RETURN(-1) // НЕ H4Obj
	st=Mi->OSType;
	if(st<1) RETURN(-1) // не H4Obj
	if((st>=10)&&(st<=13)){
		ApplySphinx(GM_ai,Hr,Mi);
	}/*else Message("Not realized yet.",1);*/
	if(GM_ai==0) RETURN(-1) // если AI пусть еще возьмет что надо
	RETURN(1) // нечего делать
}
void EnterOutOfH4Obj(int /*GM_ai*/,Dword /*Object*/)
{
	return;
}
void CorrectDwellings(_MapItem_ *Mi) // dwellings
{
	STARTNA(__LINE__, 0)
	int type;
	_Dwelling_ *dw;
	if(Mi->OSType!=93) RETURNV // не големы
	dw=FindDwellStr(Mi);
	if(dw==0) RETURNV // не нашли такого вообще - ошибка ?
	type=(GetCurDate()%4)+164;
	dw->Mon2Hire[0]=type;
	dw->GType[0]=type;
	dw->GNum[0]=10;
	RETURNV
}

///////////////////////
static _Sphinx1 QDMess={
 0,0,0,0,
 0,
 0,0,0,0,
 0,0,0,0,
 0,0,0,0,
 0,0,0,0,
 0
};

_Hero_ *HeroEntered2Object=0;
static Dword E2O_AM,E2O_Ebx,E2O_Esi,E2O_Edi,E2O_MixPos;
void __stdcall Enter2Object(_Hero_ *Hr,_MapItem_ *Mi,int Unk1,int GM_ai) //ecx=avd.Manager
{
	int   tp,ret,x,y,l,Adj,Adj_GM_ai,Adj_Res,val;
	int   o_t,o_st;
	Dword Object,Adj_cw;
	_ECX(E2O_AM);
	/* E2O_MixPos=_EDX;*/ 
	_EBX(E2O_Ebx); 
	_ESI(E2O_Esi); 
	_EDI(E2O_Edi);
	STARTNA(__LINE__, 0)
	HeroEntered2Object=Hr;
///////
// AI does not transfer it.
	Map2Coord(Mi,&x,&y,&l); E2O_MixPos=PosMixed(x,y,l);
///////
	Adj=0;
	tp=0;
	if(Mi->OType==17) CorrectDwellings(Mi); // dwellings
	o_t=Mi->OType; o_st=Mi->OSType;
	if(ERM2Object(0,GM_ai,E2O_MixPos,Mi,Hr,o_t,o_st)){ // запрещено к посещению
		STOP
		__asm{
			mov esi,E2O_Esi
			mov edi,E2O_Edi
			mov ecx,E2O_AM
			mov ebx,E2O_Ebx
			mov edx,E2O_MixPos
		}
		return;
	}
	// hide hero again (he may be shown if UN:L was called)
	void (__fastcall *fn)(_Hero_ *hp); *(Dword*)&fn = 0x4D7950;
	fn(Hr);
/*
__asm{
		pusha
		mov  eax,0x4F8970 // get time
		call eax 
		mov  ecx,0x69D680
		mov  [ecx],eax
		mov  [ecx+4],eax
		mov  [ecx+8],100000 // turn limit
		mov  [ecx+12],25000
		mov  [ecx+16],0
		mov  eax,0x557E90
		call eax
		popa
	}
*/
	do{
		ret=Enter2Castle(GM_ai,Mi,Hr,E2O_AM);
		if(ret>=0){ tp=1; break; }
		ret=Enter2Dwelling(GM_ai,Mi,Hr,E2O_AM,&Object);
		if(ret>=0){ tp=2; break; }
		ret=Enter2Bank(GM_ai,Mi,Hr,E2O_AM,&Object);
		if(ret>=0){ tp=3; break; }
		ret=Enter2H4Obj(GM_ai,Mi,Hr,E2O_AM,&Object);
		if(ret>=0){ tp=4; break; }
	}while(0);
	if(DoesHeroHasVisitCurse(Hr->Number,Mi->OType,Mi->OSType)>=0){ // проклятье посещения
		if(GM_ai){
			Message(ITxt(124,0,&Strings),1);
			goto l_exit;
		}
	}
	// Mithrill and new chests
	if(Mi->OType==101){ // Chests
		switch(Mi->OSType){
			case 0: break; // standard
/*
			case  7: // Commander boost bonuses
			case  8:
			case  9:
			case 10:
				break;
*/
			default:
//        if(EnableChest[Mi->OSType]||
//          ((Mi->OSType>=7)&&(Mi->OSType<=10))){
				if(EnableChest[Mi->OSType]){
					if((Mi->OSType>=7)&&(Mi->OSType<=10)){
						switch(Mi->OSType){
							case  7: QDMess.Pic1Path=".\\Data\\ZVS\\LIB1.RES\\_MP0.BMP"; break;
							case  8: QDMess.Pic1Path=".\\Data\\ZVS\\LIB1.RES\\_AT0.BMP"; break;
							case  9: QDMess.Pic1Path=".\\Data\\ZVS\\LIB1.RES\\_DF0.BMP"; break;
							default: QDMess.Pic1Path=".\\Data\\ZVS\\LIB1.RES\\_SP0.BMP"; break;
						}
						if(HeroHasCommander(Hr)){
							if(HeroHasAliveCommander(Hr)){ // есть и жив
								Adj=Mi->OSType+10; Adj_GM_ai=GM_ai; Adj_cw=Mi->SetUp;
								Adj_Res=GetRes(Hr->Owner,6);
								GM_ai=0; // AI
								// no art
								((_CChest_ *)&Mi->SetUp)->HasArt=0;
								// 500 gold or 0 exp
								((_CChest_ *)&Mi->SetUp)->Bonus=1;
							}else{ // есть и мертв
								if(GM_ai){
									QDMess.Pic1Hint=ITxt(205,0,&Strings);
									QDMess.Text1=ITxt(206+Mi->OSType-7,0,&Strings);
									QDMess.Text3=ITxt(210+Mi->OSType-7,0,&Strings);
									QDMess.ShowCancel=1;
									if(QuickDialog(&QDMess)==-1){ // Esc
										ret=1;
									}else{ // Ok
										Adj=Mi->OSType; Adj_GM_ai=GM_ai; Adj_cw=Mi->SetUp;
										Adj_Res=GetRes(Hr->Owner,6);
										GM_ai=0; // AI
										// no art
										((_CChest_ *)&Mi->SetUp)->HasArt=0;
										// 500 gold or 0 exp
										((_CChest_ *)&Mi->SetUp)->Bonus=1;
									}
								}
							}
						}else{ // нет
							if(PL_NoNPC!=0){ // запрещены
								// as maximum bomus as possible 7500g or 7000exp
								((_CChest_ *)&Mi->SetUp)->Bonus=(Word)Random(7,15);
							}else{ // просто у этого нет
								if(GM_ai){
									QDMess.Pic1Hint=ITxt(205,0,&Strings);
									QDMess.Text1=ITxt(214+Mi->OSType-7,0,&Strings);
									QDMess.Text3=ITxt(218+Mi->OSType-7,0,&Strings);
									QDMess.ShowCancel=1;
									if(QuickDialog(&QDMess)==-1){ // Esc
										ret=1;
									}else{ // Ok
										Adj=Mi->OSType; Adj_GM_ai=GM_ai; Adj_cw=Mi->SetUp;
										Adj_Res=GetRes(Hr->Owner,6);
										GM_ai=0; // AI
										// no art
										((_CChest_ *)&Mi->SetUp)->HasArt=0;
										// 500 gold or 0 exp
										((_CChest_ *)&Mi->SetUp)->Bonus=1;
									}
								}
							}
						}
					}else{ // другие сундуки
						Adj=Mi->OSType+10; Adj_GM_ai=GM_ai; Adj_cw=Mi->SetUp;
						Adj_Res=GetRes(Hr->Owner,6);
						GM_ai=0; // AI
						// no art
						((_CChest_ *)&Mi->SetUp)->HasArt=0;
						// 500 gold or 0 exp
						((_CChest_ *)&Mi->SetUp)->Bonus=1;
					}
				}
		}
	}
	if(Mi->OType==79){ // Resource
		switch(Mi->OSType){
			case 7: // Mithrill
				if(EnableMithrill){
					Adj=30; Adj_GM_ai=GM_ai; Adj_cw=Mi->SetUp;
					Adj_Res=((_CRes_ *)&Mi->SetUp)->Value;
					((_CRes_ *)&Mi->SetUp)->Value=0;
					Mi->OSType=1;
					GM_ai=0; // AI
				}else{
					val=Random(1,5); if(val==2) ++val;
					Mi->OSType=(Word)val;
				}
				break;
		}
	}
	__asm{
		mov esi,E2O_Esi
		mov edi,E2O_Edi
		mov ecx,E2O_AM
		mov ebx,E2O_Ebx
		mov edx,E2O_MixPos
	}
	if(ret>0) goto l_exit; // все сделано - незачем входить
	__asm{
		push   GM_ai
		push   Unk1
		push   Mi
		push   Hr
		mov    eax,0x4A8160
		call   eax
	}
	_ECX(E2O_AM); 
	_EBX(E2O_Ebx); 
	_ESI(E2O_Esi); 
	_EDI(E2O_Edi);
	// Mithrill and new chests
	if(Adj!=0){
		if((Adj>10)&&(Adj<30)){ // chest
			if(Mi->SetUp==0xFFFFFFFF){ // taken
				GM_ai=Adj_GM_ai;
				if(GetRes(Hr->Owner,6)!=Adj_Res) SetRes(Hr->Owner,6,Adj_Res);
				if((Adj>=17)&&(Adj<=20)){
					// Commander bonus
					AdjastCommanderStat(Hr,Adj-17,GM_ai);
				}
			}else{ // not
				GM_ai=Adj_GM_ai; Mi->SetUp=Adj_cw;
			}
		}
		if(Adj==30){ // mithrill
			if(Mi->SetUp==0xFFFFFFFF){ // taken
				GM_ai=Adj_GM_ai;
				MithrillVal[Hr->Owner]+=Adj_Res;
			}else{ // not
				GM_ai=Adj_GM_ai; Mi->SetUp=Adj_cw;
			}
		}
	}
	if(GM_ai==0) CommanderArtsAI(Hr);
	if(ret==0){ // было что-то найдено
		switch(tp){
			case 2:
				EnterOutOfDwelling(GM_ai,Object);
				break;
			case 3:
				EnterOutOfBank(GM_ai,Object);
				break;
			case 4:
				EnterOutOfH4Obj(GM_ai,Object);
				break;
		}
	}
l_exit:
	ERM2Object(1,GM_ai,E2O_MixPos,Mi,Hr,o_t,o_st); // пост тригер
	HeroEntered2Object=0;
	STOP
	__asm{
		mov esi,E2O_Esi
		mov edi,E2O_Edi
		mov ecx,E2O_AM
		mov ebx,E2O_Ebx
	}
	M_MDisabled=0;
	M_MDisabledNext=0;
}

static Dword SSS_Ecx,SSS_Edx;
static int SSS_Icon;
void _ShowSmallSymbol(void)
{
	if(SSS_Icon==0x13){ // Мифрил получается Золото+1
		SSS_Icon=0x12;
	}
}
void __stdcall ShowSmallSymbol(int Icon,int par2,int par3,int par4)
{
	__asm  mov   SSS_Ecx,ecx
	__asm  mov   SSS_Edx,edx
	__asm  mov   eax,Icon
	__asm  mov   SSS_Icon,eax
	__asm  pusha
	STARTNA(__LINE__, 0)
	_ShowSmallSymbol();
	STOP
	__asm  popa
	__asm  mov   eax,SSS_Icon
	__asm  mov   ebx,par2
	__asm  mov   ecx,par3
	__asm  mov   edx,par4
	__asm  push  edx
	__asm  push  ecx
	__asm  push  ebx
	__asm  push  eax
	__asm  mov   ecx,SSS_Ecx
	__asm  mov   edx,SSS_Edx
	__asm  mov   eax,0x5F7760
	__asm  call  eax
}

static int     AIn,AIx,AIy,AIl,SetAIValue;
static _Hero_ *AIAR_hp;
static _MapItem_ *AIAR_mi;
static Byte    SetAttrib;
static Word    SetType,SetSType;
static Dword   SetSetUp;
static _Hero_ *Hp;
static _MapItem_ *SetMp;
int __stdcall AIthinkAboutRes(Dword PlStr)
{
	_ECX(AIAR_hp);
	_EDX(AIAR_mi);

	STARTNA(__LINE__, 0)
	if((AIAR_mi->OSType==6)&&(AIAR_mi->SetUp==0)){
		RETURN(SetAIValue)
	}
//  MixedPos(&AIx,&AIy,&AIl,AIRun[AIn].MixPos);

	if(AIAR_mi->OSType==7){ // Mithrill
		AIAR_mi->OSType=6;
		__asm{
			mov    ecx,AIAR_hp  // -> HeroStr
			mov    edx,AIAR_mi  // -> MapItem
			mov    eax,PlStr
			push   eax          // -> структура игрока +020AD0+i*168h
			mov    eax,0x52ACA0
			call   eax
		}
		AIAR_mi->OSType=7;
	}else{
		__asm{
			mov    ecx,AIAR_hp  // -> HeroStr
			mov    edx,AIAR_mi  // -> MapItem
			mov    eax,PlStr
			push   eax          // -> структура игрока +020AD0+i*168h
			mov    eax,0x52ACA0
			call   eax
		}
	}
	__asm mov  IDummy,eax
	RETURN(IDummy)
}

static Dword   SetRet,SetMp2;
static int     SetWay,SetDist,SetX,SetY,SetL;
static _MapItem_ *SetMIp;
int __stdcall AIMapGoal(Dword p5,Dword p4,Dword p3,Dword p2,Dword p1)
{
	__asm  mov    Hp,ecx
	STARTNA(__LINE__, 0)
	__asm  pusha
		AIn=AIRGetAny((short)Hp->Number,(short)Hp->Owner,-1,4); // получить активный
		if(AIn>=0){
			MixedPos(&AIx,&AIy,&AIl,AIRun[AIn].MixPos);
			if(AIx==Hp->x)
				if(AIy==Hp->y)
					if(AIl==Hp->l){ // уже здесь
						AIn=AIRGoNextActive((short)AIn);
//            if(AIn>=0) AIRun[AIn].CurMixPos=-1;
					}
			if(AIn>=0){
				MixedPos(&AIx,&AIy,&AIl,AIRun[AIn].MixPos);
				SetMIp=GetNearestWay(AIx,AIy,AIl,Hp->x,Hp->y,Hp->l,&SetDist,&SetWay);
				if(SetMIp!=0){ Map2Coord(SetMIp,&SetX,&SetY,&SetL);
				}else{ SetX=Hp->x; SetY=Hp->y; SetL=Hp->l;}
				if(FindStep(SetX,SetY,SetL,AIx,AIy,AIl,&SetMp2,5,0/*land*/)==1){
					SetMp=GetMapItem2(SetMp2);
					SetAttrib=SetMp->Attrib; SetMp->Attrib=0x10;
					SetType=SetMp->OType; SetMp->OType=79;
					SetSType=SetMp->OSType; SetMp->OSType=6;
					SetSetUp=SetMp->SetUp; SetMp->SetUp=0;
					SetAIValue=AIRun[AIn].AIValue;
				}
			}
		}
	__asm  popa
	__asm{
			 push   p1
			 push   p2
			 push   p3
			 push   p4
			 push   p5
			 mov    eax, 0x42EDD0
// здесь долго думает
			 call   eax
			 mov    SetRet,eax
	}
	__asm  pusha
		if(AIn>=0){
			SetMp->Attrib=SetAttrib;
			SetMp->OType=SetType;
			SetMp->OSType=SetSType;
			SetMp->SetUp=SetSetUp;
		}
	__asm  popa
	STOP
	return SetRet;
// call 42edd0
//asm int 3
/*
	asm   mov   eax,p5
	asm   mov   SetPo,eax
	asm   pusha
	if(SetPo[1]!=0) Free((void *)SetPo[1]);
	SetPo[1]=(Dword)Alloc(16);
	SetPo[2]=SetPo[3]=SetPo[1]+16;
	SetPo=(Dword *)SetPo[1];
	SetPo[0]=0x00050005;
	SetPo[1]=500000;
	SetPo[2]=500;
	SetPo[3]=0;
	asm   popa
	return 0xC4653600;
*/
}

// запретим рестарт сценария
void NoRestart(void)
{
	Message(ITxt(123,0,&Strings),1);
}

//:004EEEE1 33D2           xor    edx,edx      смещение по x
//:004EEEE3 B921000000     mov    ecx,00000021 номер файла SMK
//:004EEEE8 6A01           push   00000001     ?
//:004EEEEA 6A00           push   00000000     ?
//:004EEEEC 6A01           push   00000001     ?
//:004EEEEE 6858020000     push   00000258     dx
//:004EEEF3 6820030000     push   00000320     dy
//:004EEEF8 6872010000     push   00000172     смещение по y
//:004EEEFD A368956900     mov    [00699568],eax
//:004EEF02 E869890A00     call   H3.00597870
void __stdcall ShowIntro(int y_,int dy_,int dx_,int p1_,int p2_,int p3_)
{
	int smk_; _ECX(smk_);
	int x_; _EDX(x_);
	int ind;
	STARTNA(__LINE__, 0)
//y_-=20; if(y_<0) y_=0;// для того, чтобы большой фонт не мелькал
	switch(smk_){
		case 0x21: //intro
// здесь установить координаты и размер SMK
//#ifdef RUS
		if(WoGType){ x_=115;  y_=142; }
		else{ x_=8;  y_=104; }
		break;
//    if((dx_==0x258)&&(dy_==0x320))
//    dx_/=2; dy_/=2;
		case 0x6C: // Новое начало a
		case 0x6D: // Новое начало b
		case 0x6E: // Новое начало c
		case 0x6F: // Новое начало d
		case 0x70: // Новое начало end
		case 0x71: // Элексир Жизни a
		case 0x72: // Элексир Жизни b
		case 0x73: // Элексир Жизни c
		case 0x74: // Элексир Жизни d
		case 0x75: // Элексир Жизни end
			__asm{
				mov  eax,BASE
				mov  eax,[eax]
				add  eax,0x1F86C
				mov  eax,[eax]
				mov  DDummy,eax
			}
			if(DDummy==SOGMAPTYPE){
				ind=smk_-0x6C;
				smk_+=(VIDNUM_0-0x6C);
				x_=VidData[ind].x;
				y_=VidData[ind].y;
			}
			break;
	}
	__asm{
		 push   p3_
		 push   p2_
		 push   p1_
		 push   dx_
		 push   dy_
		 push   y_
		 mov    edx,x_
		 mov    ecx,smk_
		 mov    eax,0x597870
		 call   eax
	}
	RETURNV
}

void ShowVersion(void)
{
	__asm pusha
	STARTNA(__LINE__, 0)
#ifdef TE
	if(WoGType){ Message("{Текущая версия}\n\nHoMM III\nTournament Edition : "TE_STRING_VERSION"\nERM : "ERM_STRING_VERSION,4); }
	else{ Message("{Current version}\n\nHoMM III\nTournament Edition : "TE_STRING_VERSION"\nERM : "ERM_STRING_VERSION,4); }
#else
	if(WoGType){ Message("{Текущая версия}\n\nWoG : "WOG_STRING_VERSION"\nERM : "ERM_STRING_VERSION,4); }
	else{ Message("{Current version}\n\nWoG : "WOG_STRING_VERSION"\nERM : "ERM_STRING_VERSION,4); }
#endif
	STOP
	__asm popa
}

void DropGame1(void) // L
{
	__asm pusha
	STARTNA(__LINE__, 0)
	PutBackHeroInfo();
	STOP
	__asm popa
	__asm{
		mov   eax,0x697728
		mov   dword ptr [eax],0x0000066
		mov   eax,1
	}
}

void DropGame2(void) // N
{
	__asm pusha
	STARTNA(__LINE__, 0)
	PutBackHeroInfo();
	STOP
	__asm popa
	__asm{
		mov   eax,0x697728
		mov   dword ptr [eax],0x0000065
		mov   eax,1
	}
}

void DropGame3(void) // Options->Load, Options->Menu
{
	__asm mov    dword ptr [edi],0x0000200
	__asm cmp    edx,0x66
	__asm jne    lnono
	__asm cmp    edx,0x6C
	__asm jne    lnono
	__asm cmp    edx,0x69
	__asm jne    lnono
	__asm pusha
	STARTNA(__LINE__, 0)
	PutBackHeroInfo();
	STOP
	__asm popa
lnono:;
}

void DropGame4(void) // Restart
{
	__asm pusha
	STARTNA(__LINE__, 0)
	PutBackHeroInfo();
	STOP
	__asm popa
	__asm mov    eax,0x69D85C
	__asm mov    al,[eax]
}

////////////////////////////////////
/*
void __cdecl RaiseException(
//extern "PASCAL" RaiseException(
	Dword dwExceptionCode,  // exception code
	Dword dwExceptionFlags,  // continuable exception flag
	Dword nNumberOfArguments,  // number of arguments in array
	const Dword *lpArguments   // address of array of arguments
);*/
//#include <windows.h>
int SaveB1(void)
{
	STARTNA(__LINE__, 0)
	if(Saver("LB1 ",4)) RETURN(1)
	if(Saver(&LastTurnedDay,sizeof(LastTurnedDay))) RETURN(1)
	if(Saver(CArtSetup,sizeof(CArtSetup))) RETURN(1)
	RETURN(0)
//__asm int 3
//  GEr.Add("TestBug",0); 
//  RaiseException(__LINE__,0,0,NULL);
//    int v=3; v=v/0;
//  }catch(...){ GEr.Add("SaveB1",0); RaiseException(__LINE__,0,0,NULL); }
}

int LoadB1(int/* ver*/)
{
	STARTNA(__LINE__, 0)
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='L'||buf[1]!='B'||buf[2]!='1'||buf[3]!=' ')
			{MError("LoadB1 cannot start loading"); RETURN(1)}
	if(Loader(&LastTurnedDay,sizeof(LastTurnedDay))) RETURN(1) 
	if(Loader(CArtSetup,sizeof(CArtSetup))) RETURN(1)
	RETURN(0)
}
void ResetB1(void)
{
	STARTNA(__LINE__, 0)
	LastTurnedDay=-1; // день когда сработал последний раз
	BuildUpNewComboArts();
	RETURNV
}

static struct Cheats{
	Byte  B;
	Word  W;
	Dword D;
	int   Len;
	Dword Sig;
} ChAr[19]={
	{0x8A,0x8E7B,0x348E0D18,0x0D,0x63A4DC-0x5160F7+0},  // nwcmorpheus -> wogfellowship
	{0x97,0x1803,0x3A3EBCF6,0x0B,0x63A52C-0x5160F7+1},  // nwcbluepill -> wogdarklord
	{0x7C,0xF6DE,0x6236EE53,0x0F,0x63A4A8-0x5160F7+2},  // nwcneo -> woggandalfwhite
	{0x85,0x16FE,0xB0170CF6,0x0B,0x63A4E8-0x5160F7+3},  // nwcoracle -> wogpalantir
	{0xF5,0x805C,0x01D20B48,0x0C,0x63A498-0x5160F7+4},  // nwclotsofguns -> wogoliphaunt
	{0x11,0x09FF,0x1CFEBCF6,0x0A,0x63A544-0x5160F7+5},  // nwcthereisnospoon -> wogsaruman
	{0x9E,0xA497,0xF922BCC6,0x09,0x63A508-0x5160F7+6},  // nwcignoranceisbliss -> wogmordor
	{0xDA,0x4D44,0x991478DB,0x10,0x63A48C-0x5160F7+7},  // nwcagents -> wogpathofthedead
	{0x79,0x833F,0x9984EC08,0x16,0x63A560-0x5160F7+8},  // nwcphisherprice -> wogsarumanofmanycolors
	{0x0C,0x21E2,0x00F2F0C2,0x0A,0x63A538-0x5160F7+9},  // nwcredpill -> wogonering
	{0x2E,0xB164,0x82535CEF,0x0C,0x63A480-0x5160F7+10}, // nwctrinity -> woggaladriel
	{0x91,0x24F0,0x34E2F0D6,0x0B,0x63A51C-0x5160F7+11}, // nwctheconstruct -> wogisengard
	{0x00,0x786F,0xD64B2224,0x0C,0x63A4C8-0x5160F7+12}, // nwcnebuchadnezzar -> wogshadowfax
	{0x53,0xADA9,0xD81B0D16,0x08,0x63A4B0-0x5160F7+13}, // nwcfollowthewhiterabbit -> wogbilbo
	{0xF2,0x06BD,0x5911FD19,0x0E,0x63A558-0x5160F7+14}, // nwczeon -> wogminasterith
	{0xC2,0xEFA4,0x1FF118EC,0x0E,0x63A4F4-0x5160F7+15},  // nwcwhatisthematrix -> wogeyeofsauron

	{0x31,0x9A9A,0xE422B0D2,0x08,0x63D49C-0x5160F7+16},  // nwcredpill -> wogfrodo
	{0x83,0xFF14,0xF226AD06,0x0B,0x63D490-0x5160F7+17},  // nwcbluepill -> wogdenethor
	{0x35,0x1A13,0x1722ACF6,0x0A,0x63D4A8-0x5160F7+18}   // nwcthereisnospoon -> woggandalf
};

static int GetInd(Dword Sig)
{
	STARTNA(__LINE__, 0)
	for(int i=0;i<19;i++){
		if(ChAr[i].Sig==Sig-0x5160F7+i) RETURN(i)
	}
	RETURN(-1)
}
int CheckCheat(Byte *str,Dword Sig)
{
	STARTNA(__LINE__, 0)
	int Ind=GetInd(Sig);
	if(Ind<0 || Ind>18) RETURN(1)
	Byte  chkb=0;
	Word  chkw=0;
	Dword chkd=0;
	int i;
	int n=strlen((char *)str);
	for(i=0;i<n;i++)   chkb=(Byte)(chkb+(str[i]+(Byte)(i-7)));
	for(i=0;i<n/2;i++) chkw=(Word)(chkw+(Word)(*(Word*)&str[i*2]-(Word)(i+5)));
	for(i=0;i<n/4;i++) chkd=chkd+(Dword)(*(Dword*)&str[i*4]*(Dword)(i+3));
	if(ChAr[Ind].B!=chkb) RETURN(1)
	if(ChAr[Ind].W!=chkw) RETURN(1)
	if(ChAr[Ind].D!=chkd) RETURN(1)
	if(ChAr[Ind].Len!=n)  RETURN(1)
	RETURN(0)
}

Dword  _ArtList_::Ar[100];
Dword *_ArtList_::ArL[4];
 
__declspec( naked ) void FixQuit98Crash(void)
{
	__asm{
		push  ecx
		mov   eax,0x59A6F0
		call  eax
		pop   ecx
		ret
	}
}

int NoMoreChecksS=0;

void BuildUpNewComboArts(void)
{
	STARTNA(__LINE__, 0)
	FillMem(CArtSetup,sizeof(CArtSetup),0);
	_CArtSetup_ *combo=(_CArtSetup_ *)0x693938;
	for(int i=0;i<12;i++) CArtSetup[i]=combo[i];
	int Arts[10]={10,16,22,28};
	BuildUpCombo(158,12,4,Arts);
	ArtBonus[157][1]=2; // +2 defence to a new shield
//  ArtTable[158].SuperN=12;
//  ArtTable[10].PartOfSuperN=12;
//  ArtTable[16].PartOfSuperN=12;
//  ArtTable[22].PartOfSuperN=12;
//  ArtTable[28].PartOfSuperN=12;

//  __asm int 3
//  __asm nop 
	SOD_CRC(387656) 
		if(__v!=(SODCRC+387656)) EXITERS(774653,eax,__v) // RETURNV
//  SOD_CRC(132) if(__v!=0x1234+132) RETURNV
//  WOG_CRC(0) if(__v!=0x1234+45678) RETURNV
//  WOG_CRC(45678) if(__v!=0x1234+45678) RETURNV

	STOP
}

//--------------------DIAKON---------------------------------//
int* OffsetSndName = (int*)(0x5987C0+1);
int OffsetOldName  = 0x684ADC;
char* NewSndName;
int LoadSndProc = 0x5987A0;
int LoadSound = 0x55C340;
int ClearSndMemory = 0x5989E0;
int SndResult = 0;
int ReturnAdr;
int Arg1, i_eax, i_ecx, i_edx, i_ebx, i_esi, i_edi;
__declspec (naked) void LoadNewSnd(){
	__asm{
		pushad;
		call ClearSndMemory; 
		popad;
		pop ReturnAdr;
		pop Arg1;	
		push ReturnAdr;
		mov i_eax, eax;
		mov i_ecx, ecx;
		mov i_edx, edx;
		mov i_ebx, ebx;
		mov i_esi, esi;
		mov i_edi, edi;
	}


	*OffsetSndName = OffsetOldName;
	__asm call LoadSndProc;
	__asm push Arg1;
	__asm mov eax, i_eax;
	__asm mov ecx, i_ecx;
    __asm mov edx, i_edx;
	__asm mov ebx, i_ebx;
	__asm mov esi, i_esi;
	__asm mov edi, i_edi;
	__asm call LoadSound;
	__asm mov SndResult, eax;
	if (SndResult) {__asm ret};

    NewSndName = "data\\Towns.snd";
	*OffsetSndName = (int)NewSndName;
	__asm call LoadSndProc;
	__asm push Arg1;
	__asm mov eax, i_eax;
	__asm mov ecx, i_ecx;
    __asm mov edx, i_edx;
	__asm mov ebx, i_ebx;
	__asm mov esi, i_esi;
	__asm mov edi, i_edi;
	__asm call LoadSound;
	__asm mov SndResult, eax;
	if (SndResult) {__asm ret};

	__asm ret;

}