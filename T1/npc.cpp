#include <memory.h>
#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "herospec.h"
#include "service.h"
#include "monsters.h"
#include "erm.h"
#include "b1.h"
#include "artifact.h"
//#include "wogsetup.h"
#include "CrExpo.h"
#define __FILENUM__ 12

static TxtFile    NPCTexts;
static TxtFile    NPCBios;

#define CALC_HP(Lvl) (40+Lvl*20)
#define CALC_DM(Lvl) (12+Lvl*4)
#define CALC_MinDM(dm) (dm/2)
#define AT Primary[0]
#define DF Primary[1]
#define HP Primary[2]
#define DM Primary[3]
#define MP Primary[4]
#define SP Primary[5]
#define MR Primary[6]
#define ATS Skills[0]
#define DFS Skills[1]
#define HPS Skills[2]
#define DMS Skills[3]
#define MPS Skills[4]
#define SPS Skills[5]
#define MRS Skills[6]
/*
#define ATP(i) Arts[i][0]
#define AAT(i) Arts[i][1]
#define ADF(i) Arts[i][2]
#define AHP(i) Arts[i][3]
#define ADM(i) Arts[i][4]
#define AMP(i) Arts[i][5]
#define ASP(i) Arts[i][6]
#define AMR(i) Arts[i][7]
*/
#define AT_DF  0x00000001
#define AT_HP  0x00000002
#define AT_DM  0x00000004
#define AT_MP  0x00000008
#define AT_SP  0x00000010
#define DF_HP  0x00000020
#define DF_DM  0x00000040
#define DF_MP  0x00000080
#define DF_SP  0x00000100
#define HP_DM  0x00000200
#define HP_MP  0x00000400
#define HP_SP  0x00000800
#define DM_MP  0x00001000
#define DM_SP  0x00002000
#define MP_SP  0x00004000
#define NPCARTBASE 146
#define ARTNUMINDEX 0
#define ARTBATTLES 1
class NPC{
 public:
	int  Used;
	int  Dead;
	int  Number;
	int  Type,HType;
	int  LastExpoInBattle;
	struct{
		unsigned  CustomPrimary : 1; // Пользователь сам контролирует параметры
//    unsigned  NoHeroOwner   : 1; // нет хозяина героя (3.58)
		unsigned _reserved      :31;
	} Fl;
	int  Primary[7]/*At,Df,Hp,Dm,Mp,Sp,Mr*/;
//  struct{
//    Byte  :1;
//  } Flags;
	int  Skills[7];
	short Arts[10][8];
	char Name[32];
	int  OldHeroExp;
	int  Exp;   // тек. опыт
	int  Level; // тек. уровень
	Dword  SpecBon[2];
	int  GetAvailableSpecBon(int);
	static int   Spec2Ind[15][2];
	static int   Bonus[7][5];
	static int   SpecBonus[7][5];
	static char *BonusBmp[6][6];
	static char *BonusArtBmp[6];
	static char *BonusBmpA[6][6];
	static char *SpecBonusBmp[6][6][4];
	static char *SpecBonusText[6][6][2];
	static char *SpecBonusPopUpText[6][6];
	int         FindSpecBonusBmp(int i,int j);
	static char *NextBonusBmp[6][2];
	static char *BonusText[6][6];
	static char *BonusPopUpText[6][6];
	static char *SpecBmp[9][2];
	static char *SpecText[9][2];
	static char *SpecHint[9][2];
	int CalcSkill(int);
	int CalculateSkills(int);
	int MayNextSkill(int);
//  static char *SkNames[7];
	static char *Descr[9];
	static char *Magics[9];
	static _ZPrintf_ Hint[2];
	static _ZPrintf_ Buffer,Buffer1;
	static int Levels[100];
	static int AISkillsChance[18][6];
	void SetOldHeroExp(int Exp){ OldHeroExp=Exp; }
	int  GetExp(void){ return Exp; }
	void AddExp(int,int); // добавить опыта
	int  GetNextLevel(void); // получить след ожид. уровень
	NPC();
	void Init(void);
	void ToText(int,int=0,int=0);
	char *ToHint(int);
	static char *ArtPicNames[10];
	static char  ArtTextBuffer[10][512];
	static int  ArtIsPossible(int ArtNum);
	int   ArtMayHave(int ArtNum);
	int   ArtGetFreeSlot(int ArtNum);
	int   ArtDoesHave(int ArtNum);
	int   ArtCalcSkill(int Slot,int Skill);
	int   ArtInSlot(int ArtNum);
	char *PrepareArtText(int Slot);
} NPCs[HERNUM],NPCsBack[HERNUM],NPCsa[2];

_ZPrintf_ NPC::Hint[2];
_ZPrintf_ NPC::Buffer;
_ZPrintf_ NPC::Buffer1;
int   NPC::Bonus[7][5]={
	{2,5,9,15,25},     // +AT
	{4,10,18,30,50},   // +DF
	{10,25,45,70,100}, // +HP%
	{10,25,45,70,100}, // +DM%
	{1,3,6,14,29},     // +MP
	{1,2,3,4,6},       // +SP
	{5,15,35,60,90}    // +MR%
};
int   NPC::SpecBonus[7][5]={
	{4,10,18,30,50},    // +AT
	{8,20,36,60,100},   // +DF
	{20,50,90,140,200}, // +HP%
	{20,50,90,140,200}, // +DM%
	{2,5,10,20,40},     //  MP
	{2,3,5,7,9},        // +SP
	{10,20,40,65,95}    // +MR%
};
//char *NPC::SkNames[7]={"(0) Attack","(0) Defense","(0) Hit Points","(0) Damage","(0) Magic Power","(0) Speed","(0) Magic Resistance"};
char *NPC::Descr[9];//={
//  "Description of NPC Type 1",
//  "Description of NPC Type 2",
//  "Description of NPC Type 3",
//  "Description of NPC Type 4",
//  "Description of NPC Type 5",
//  "Description of NPC Type 6",
//  "Description of NPC Type 7",
//  "Description of NPC Type 8",
//  "Description of NPC Type 9"
//};
char *NPC::Magics[9];//={
//"Cure","Shield","Precision","Fire Shield","Animate Dead",
//"Bloodlust","Stone Skin","Haste","Counterstrike"
//};

int NPC::Levels[100]={
				 0,      1000,      2000,      3200,      4600, // 0
			6200,      8000,     10000,     12200,     14700, // 5
		 17500,     20600,     24320,     28784,     34140, // 10
		 40567,     48279,     57533,     68637,     81961, // 15
		 97949,    117134,    140156,    167782,    200933, // 20
		240714,    288451,    345735,    414475,    496963, // 25
		595948,    714730,    857268,   1028313,   1233567, // 30
	 1479871,   1775435,   2130111,   2555722,   3066455, // 35
	 3679334,   4414788,   5297332,   6356384,   7627246, // 40
	 9152280,  10982320,  13178368,  15813625,  18975933, // 45
	22770702,  27324424,  32788890,  39346249,  47215079, // 50
	56657675,  67988790,  81586128,  97902933, 117483099, // 55
 140979298, 169174736, 203009261, 243610691, 292332407, // 60
 350798469, 420957739, 505148863, 606178211, 727413428, // 65
 872895688,1047474816,1256969772,1508363724,1810036464  // 70
// 74 is a max
/* H4
				 0,     1000,     2000,     3200,     4600, // 0
			6200,     8100,    10300,    12900,    16000, // 5
		 19700,    24100,    29300,    35500,    42900, // 10
		 51700,    62200,    74800,    89900,   108000, // 15
		129700,   155700,   186900,   224300,   269100, // 20
		322800,   387200,   464400,   557000,   668100, // 25
		801400,   961300,  1153100,  1383200,  1659300, // 30
	 1990600,  2388100,  2865100,  3437500,  4124300, // 35
	 4948400,  5937300,  7123900,  8548800, 10256400, // 40
	12306700, 14767000, 17719300, 21262000, 25513200, // 45
	30614600, 36736200, 44082100, 52897100, 63475100, // 50
	76168700, 91401000,109679700,131614100,157935300, // 55
 189520700,227423100,272905900,327485200,392980300, // 60
 471574400,565887300,679062700,814873100,977845500, // 65
1000000000, // 70
*/
};
int NPC::AISkillsChance[18][6]={
{5,15,20,5,35,20}, //Knight.......MP(35%); HP(20%); SP(20%); DF(15%); DM(5%) ; AT(5%)
{13,35,5,20,7,20}, //Cleric.......DF(35%); SP(20%); DM(20%); AT(13%); MP(7%) ; HP(5%)
{13,5,20,20,35,7}, //Ranger.......MP(35%); HP(20%); DM(20%); AT(13%); SP(7%) ; DF(5%)
{35,13,7,20,5,20}, //Druid........AT(35%); DM(20%); SP(20%); DF(13%); HP(7%) ; MP(5%)
{10,5,20,20,15,30},//Alchemist....SP(30%); DM(20%); HP(20%); MP(15%); AT(10%); DF(5%)
{30,15,35,10,5,5}, //Wizard.......HP(35%); AT(30%); DF(15%); DM(10%); SP(5%) ; MP(5%)
//------EVIL classes:
{20,13,5,35,7,20}, //Demoniac.....DM(35%); AT(20%); SP(20%); DF(13%); MP(7%) ; HP(5%)
{10,15,30,20,5,20},//Heretic......HP(30%); DM(20%); SP(20%); DF(15%); AT(10%); MP(5%)
{15,10,10,5,20,40},//Death Knight.SP(40%); MP(20%); AT(15%); DF(10%); HP(10%); DM(5%)
{5,10,15,20,10,40},//Necromancer..SP(40%); DM(20%); HP(15%); DF(10%); MP(10%); AT(5%)
{5,9,30,18,25,13}, //Overlord.....HP(30%); MP(25%); DM(18%); SP(13%); DF(9%) ; AT(5%)
{20,5,20,30,10,15},//Warlock......DM(30%); HP(20%); AT(20%); SP(15%); MP(10%); DF(5%)
//------NEUTRAL classes:
{5,25,5,5,25,35},  //Barbarian....SP(35%); DF(25%); MP(25%); AT(5%) ; HP(5%) ; DM(5%)
{20,15,10,5,30,20},//Battle Mage..MP(30%); SP(20%); AT(20%); DF(15%); HP(10%); DM(5%)
{35,5,15,5,10,30}, //Beastmaster..AT(35%); SP(30%); HP(15%); MP(10%); DM(5%) ; DF(5%)
{35,30,5,5,10,15}, //Witch........AT(35%); DF(30%); SP(15%); MP(10%); HP(5%) ; DM(5%)
{15,10,20,30,5,20},//Planeswalker.DM(30%); HP(20%); SP(20%); AT(15%); DF(10%); MP(5%)
{30,20,20,15,5,10} //Elementalist.AT(30%); DF(20%); HP(20%); DM(15%); SP(10%); MP(5%)
};

char *NPC::SpecBmp[9][2]={
	{".\\DATA\\ZVS\\LIB1.RES\\_B1_2.BMP",".\\DATA\\ZVS\\LIB1.RES\\_B1_1.BMP"},
	{".\\DATA\\ZVS\\LIB1.RES\\_B2_2.BMP",".\\DATA\\ZVS\\LIB1.RES\\_B2_1.BMP"},
	{".\\DATA\\ZVS\\LIB1.RES\\_B3_2.BMP",".\\DATA\\ZVS\\LIB1.RES\\_B3_1.BMP"},
	{".\\DATA\\ZVS\\LIB1.RES\\_B6_2.BMP",".\\DATA\\ZVS\\LIB1.RES\\_B4_1.BMP"},
	{".\\DATA\\ZVS\\LIB1.RES\\_B5_2.BMP",".\\DATA\\ZVS\\LIB1.RES\\_B5_1.BMP"},
	{".\\DATA\\ZVS\\LIB1.RES\\_B4_2.BMP",".\\DATA\\ZVS\\LIB1.RES\\_B6_1.BMP"},
	{".\\DATA\\ZVS\\LIB1.RES\\_B7_2.BMP",".\\DATA\\ZVS\\LIB1.RES\\_B7_1.BMP"},
	{".\\DATA\\ZVS\\LIB1.RES\\_B8_2.BMP",".\\DATA\\ZVS\\LIB1.RES\\_B8_1.BMP"},
	{".\\DATA\\ZVS\\LIB1.RES\\_B9_2.BMP",".\\DATA\\ZVS\\LIB1.RES\\_B9_1.BMP"},
};
char *NPC::SpecText[9][2];//={
//  {"Wise","Cure"},
//  {"First Aid Master","Shield"},
//  {"Mana Magician","Precision"},
//  {"Soul Reformer","Fire Shield"},
//  {"Undead","Animate Dead"},
//  {"Charming","Bloodlust"},
//  {"Ballistas Master","Stone Skin"},
//  {"Hero's truly","Haste"},
//  {"Pacifist","Counterstrike"}
//};
char *NPC::SpecHint[9][2];//={
//  {"Get 150% of Heor's experience","May cast Cure"},
//  {"Cooks First Aid Tent stack, Number = Commander Level","May cast Shield [Length = Magic Power]"},
//  {"Restore (Commander Level) mana points for Hero every battle turn","May cast Precision [Length = Magic Power]"},
//  {"Gives 25% of a battle experience in gold","May cast Fire Shild [Length = Magic Power]"},
//  {"Undead","May cast Unimate Dead [HP Number = (Magic Power/2)*50+60 ]"},
//  {"Steals 5% of neutral stacks before combat","Bloodlust [Length = Magic Power]"},
//  {"Cooks Ballistas Stack #=NPCLevel/4+1 (plus control)","Stone Skin [Length = Magic Power]"},
//  {"150% of Hero Attack and Defense","Haste (Speed + 5) [Length = Magic Power]"},
//  {"is Elemental + 5% of Creature in enemy Hero's army run away","Counterstrike [Length = Magic Power]"}
//};
char *NPC::BonusBmp[6][6]={
 { ".\\DATA\\ZVS\\LIB1.RES\\_ATNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT0.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_AT1.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT2.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_AT3.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT4.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_DFNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF0.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_DF1.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF2.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_DF3.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF4.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_HPNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP0.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_HP1.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP2.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_HP3.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP4.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_DMNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM0.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_DM1.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM2.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_DM3.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM4.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_MPNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_MP0.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_MP1.BMP",".\\DATA\\ZVS\\LIB1.RES\\_MP2.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_MP3.BMP",".\\DATA\\ZVS\\LIB1.RES\\_MP4.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_SPNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_SP0.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_SP1.BMP",".\\DATA\\ZVS\\LIB1.RES\\_SP2.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_SP3.BMP",".\\DATA\\ZVS\\LIB1.RES\\_SP4.BMP"}
};
char *NPC::BonusArtBmp[6]={
 ".\\DATA\\ZVS\\LIB1.RES\\A_AT.BMP",
 ".\\DATA\\ZVS\\LIB1.RES\\A_DF.BMP",
 ".\\DATA\\ZVS\\LIB1.RES\\A_HP.BMP",
 ".\\DATA\\ZVS\\LIB1.RES\\A_DM.BMP",
 ".\\DATA\\ZVS\\LIB1.RES\\A_MP.BMP",
 ".\\DATA\\ZVS\\LIB1.RES\\A_SP.BMP"
};
char *NPC::BonusBmpA[6][6]={
 { ".\\DATA\\ZVS\\LIB1.RES\\_ATNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT0=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_AT1=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT2=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_AT3=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT4=.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_DFNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF0=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_DF1=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF2=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_DF3=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF4=.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_HPNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP0=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_HP1=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP2=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_HP3=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP4=.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_DMNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM0=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_DM1=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM2=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_DM3=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM4=.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_MPNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_MP0=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_MP1=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_MP2=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_MP3=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_MP4=.BMP"},
 { ".\\DATA\\ZVS\\LIB1.RES\\_SPNO.BMP",".\\DATA\\ZVS\\LIB1.RES\\_SP0=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_SP1=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_SP2=.BMP",
	 ".\\DATA\\ZVS\\LIB1.RES\\_SP3=.BMP",".\\DATA\\ZVS\\LIB1.RES\\_SP4=.BMP"}
};
char *NPC::SpecBonusBmp[6][6][4]={
{{"NONE","NONE","NONE","NONE"},
 {".\\DATA\\ZVS\\LIB1.RES\\_AT_DF.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_DF_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_DF_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_DF_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_DM_HP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM_HP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM_HP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM_HP_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_AT_DM.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_DM_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_DM_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_DM_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_AT_MP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_MP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_MP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_MP_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_AT_SP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_SP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_SP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_SP_1.BMP"}},
{{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},
 {".\\DATA\\ZVS\\LIB1.RES\\_DF_HP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_HP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_HP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_HP_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_DF_DM.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_DM_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_DM_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_DM_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_HP_MP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP_MP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP_MP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP_MP_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_DF_SP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_SP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_SP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_SP_1.BMP"}},
{{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},
 {".\\DATA\\ZVS\\LIB1.RES\\_AT_HP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_HP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_HP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_AT_HP_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_DF_MP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_MP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_MP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DF_MP_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_HP_SP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP_SP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP_SP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_HP_SP_1.BMP"}},
{{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},
 {".\\DATA\\ZVS\\LIB1.RES\\_DM_MP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM_MP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM_MP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM_MP_1.BMP"},
 {".\\DATA\\ZVS\\LIB1.RES\\_DM_SP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM_SP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM_SP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_DM_SP_1.BMP"}},
{{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},{"NONE","NONE","NONE","NONE"},
 {".\\DATA\\ZVS\\LIB1.RES\\_MP_SP.BMP",".\\DATA\\ZVS\\LIB1.RES\\_MP_SP_P.BMP",".\\DATA\\ZVS\\LIB1.RES\\_MP_SP_0.BMP",".\\DATA\\ZVS\\LIB1.RES\\_MP_SP_1.BMP"}},
};
char *NPC::SpecBonusText[6][6][2];/*={
// AT,DF,HP,DM,MP,SP
{{"",""},
 {"Reduce Enemy Defense by 50% (Attack + Defense)","Commander can get : Reduce Enemy Defense by 50% (Attack + Defense)"},
 {"Attack twice (Attack + HitPoints)","Commander can get : Attack twice (Attack + HitPoints)"},
 {"Maximum damage always (Attack + Damage)","Commander can get : Maximum damage always (Attack + Damage)"},
 {"No Enemy Retaliation (Attack + Magic Power)","Commander can get : No Enemy Retaliation (Attack + Magic Power)"},
 {"Can Shoot (Attack + Speed)","Commander can get : Can Shoot (Attack + Speed)"}},
{{"",""},{"",""},
 {"Endless Retaliation (Defense + Hit Points)","Commander can get : Endless Retaliation (Defense + Hit Points)"},
 {"Strike all Enemies around (Defense + Damage)","Commander can get : Strike all Enemies around (Defense + Damage)"},
 {"Melee Attack 50% chance to Paralyse (Defense + Magic Power)","Commander can get : Melee Attacks 50% chance to Paralyse (Defense + Magic Power)"},
 {"Ignore Obstacles [fly] (Defense + Speed)","Commander can get : Ignore Obstacles [fly] (Defense + Speed)"}},
{{"",""},{"",""},{"",""},
 {"Fearsome (Hit Points + Damage)","Commander can get : Fearsome (Hit Points + Damage)"},
 {"Permanent Fireshield (Hit Points + Magic Power)","Commander can get : Permanent Fireshield (Hit Points + Magic Power)"},
 {"Regeneration 50 HP every turn (Hit Points + Speed)","Commander can get : Regeneration 50 HP every turn (Hit Points + Speed)"}},
{{"",""},{"",""},{"",""},{"",""},
 {"DeathStare (Damage + Magic Power)","Commander can get : DeathStare (Damage + Magic Power)"},
 {"Champion Distance Bonus (Damage + Speed)","Commander can get : Champion Distance Bonus (Damage + Speed)"}},
{{"",""},{"",""},{"",""},{"",""},{"",""},
 {"Summon Creature Stack (Magic Power + Speed)","Commander can get : Summon Creature Stack (Magic Power + Speed)"}},
};*/
char *NPC::SpecBonusPopUpText[6][6];/*={
// AT,DF,HP,DM,MP,SP
{"",
 "\nReduce Enemy Defense by 50%\n\nThe Commander gains this extra skill if he or she has\nAttack and Defense Grandmaster skills\n\nIt is marked with [D] in Commander info during a battle.\n",
 "\nAttack twice\n\nThe Commander gains this extra skill if he or she has\nAttack and Hit Points Grandmaster skills\n\nIt is marked with [2] in Commander info during a battle.\n",
 "\nMaximum damage always\n\nThe Commander gains this extra skill if he or she has\nAttack and Damage Grandmaster skills\n\nIt is marked with [M] in Commander info during a battle.\n",
 "\nNo Enemy Retaliation\n\nThe Commander gains this extra skill if he or she has\nAttack and Magic Power Grandmaster skills\n\nIt is marked with [N] in Commander info during a battle.\n",
 "\nCan Shoot\n\nThe Commander gains this extra skill if he or she has\nAttack and Speed Grandmaster skills\n\nIt is marked with [S] in Commander info during a battle.\n"},
{"","",
 "\nEndless Retaliation\n\nThe Commander gains this extra skill if he or she has\nDefense and Hit Points Grandmaster skills\n\nIt is marked with [E] in Commander info during a battle.\n\nStrike back every attack\n",
 "\nStrike all Enemies around\n\nThe Commander gains this extra skill if he or she has\nDefense and Damage Grandmaster skills\n\nIt is marked with [A] in Commander info during a battle.\n",
 "\nMelee Attack 50% chance to Paralyse\n\nThe Commander gains this extra skill if he or she has\nDefense and Magic Power Grandmaster skills\n\nIt is marked with [P] in Commander info during a battle.\n\nEvery attack has 50% chance to Paralyse an enemy for 3 rounds\n",
 "\nIgnore Obstacles [fly]\n\nThe Commander gains this extra skill if he or she has\nDefense and Speed Grandmaster skills\n\nIt is marked with [F] in Commander info during a battle.\n"},
{"","","",
 "\nFearsome\n\nThe Commander gains this extra skill if he or she has\nHit Points and Damage Grandmaster skills\n\nIt is marked with [O] in Commander info during a battle.\n",
 "\nPermanent Fireshield\n\nThe Commander gains this extra skill if he or she has\nHit Points and Magic Power Grandmaster skills\n\nIt is marked with [I] in Commander info during a battle.\n\nStarts Every round with Fire Shield\n",
 "\nRegeneration 50 HP every turn\n\nThe Commander gains this extra skill if he or she has\nHit Points and Speed Grandmaster skills\n\nIt is marked with [R] in Commander info during a battle.\n\nEvery turn (in the beginning) regenerates 50 Hit Points\n"},
{"","","","",
 "\nDeathStare\n\nThe Commander gains this extra skill if he or she has\nDamage and Magic Power Grandmaster skills\n\nIt is marked with [G] in Commander info during a battle.\n\nEvery attack has the Death Stare\nKills (Commander Level)/(Creature Level)\ncreatures in the stack\nwhere Creature Level is from 1 to 7\n",
 "\nChampion Distance Bonus\n\nThe Commander gains this extra skill if he or she has\nDamage and Speed Grandmaster skills\n\nIt is marked with [C] in Commander info during a battle.\n\nDamage grows for 5% per square\n"},
{"","","","","",
 "\nSummon Creature Stack\n\nThe Commander gains this extra skill if he or she has\nMagic Power and Speed Grandmaster skills\n\nIt is marked with [U] in Commander info during a battle.\n\nAt the battle beginning summons a creature stack\nMonster Type is (Commander Level - 15) / 2\nMonster Type here means an index in\nthis class monsters line\n[level1(0), Level1Upg(1), Level2(2),\nLevel2Upg(3),...Level7Upg(13)]\nNumber is (Commander Level)*10/(Type*Type+1)\n"}
};*/

char *NPC::ArtPicNames[10]={
	"[LODDEF]H3Sprite.lod;Artifact.def;147;0;0",
	"[LODDEF]H3Sprite.lod;Artifact.def;148;0;0",
	"[LODDEF]H3Sprite.lod;Artifact.def;149;0;0",
	"[LODDEF]H3Sprite.lod;Artifact.def;150;0;0",
	"[LODDEF]H3Sprite.lod;Artifact.def;151;0;0",
	"[LODDEF]H3Sprite.lod;Artifact.def;152;0;0",
	"[LODDEF]H3Sprite.lod;Artifact.def;153;0;0",
	"[LODDEF]H3Sprite.lod;Artifact.def;154;0;0",
	"[LODDEF]H3Sprite.lod;Artifact.def;155;0;0",
	"[LODDEF]H3Sprite.lod;Artifact.def;156;0;0"
};
//short NPC::ArtBonusVal[10][7]={
// AT,DF,HP,DM,MP,SP,MR
// {5,0,0,5,0,0,0},  // +5At +5%Dm
// {0,5,5,0,0,0,0},  // +5Df +5%Hp
// {1,0,0,10,0,0,0}, // +1At +10%Dm
// {0,1,10,0,0,0,0}, // +1Df +10%Hp
// {0,0,0,0,0,0,0},  //
// {0,0,0,0,0,0,0},  //
// {0,0,0,0,0,0,0},  //
// {0,0,0,0,0,0,0},  //
// {0,0,0,0,0,0,0},  //
// {0,0,0,0,0,0,0}   //
//};
int NPC::FindSpecBonusBmp(int i,int j){
	STARTNA(__LINE__, 0)
	int k;
	Dword mask;
//  if(i==j) return 0;
	if(i>=j)  RETURN(-1) //{ k=j; j=i; i=k; }
//////  if((Skills[i]>=4)&&(Skills[j]>=4)) return 0;
	switch(i){
		case 0:  k=-1+j; break;
		case 1:  k=3+j; break;
		case 2:  k=6+j; break;
		case 3:  k=8+j; break;
		case 4:  k=9+j; break;
		default: RETURN(1)
	}
	mask=1<<k;
	if(SpecBon[0]&mask) RETURN(0) // есть уже
	if(SpecBon[1]&mask) RETURN(-1) // нельзя
	// стандартная реакция
	if((Skills[i]==0)||(Skills[j]==0)) RETURN(-1)
	RETURN(1)
}
char *NPC::NextBonusBmp[6][2];

char *NPC::BonusText[6][6];/*={
	{"No Attack Skill [Right mouse button for more]",
	 "Basic Attack Skill [Right mouse button for more]",
	 "Advanced Attack Skill [Right mouse button for more]",
	 "Expert Attack Skill [Right mouse button for more]",
	 "Master Attack Skill [Right mouse button for more]",
	 "Grandmaster Attack Skill [Right mouse button for more]"},
	{"No Defense Skill [Right mouse button for more]",
	 "Basic Defense Skill [Right mouse button for more]",
	 "Advanced Defense Skill [Right mouse button for more]",
	 "Expert Defense Skill [Right mouse button for more]",
	 "Master Defense Skill [Right mouse button for more]",
	 "Grandmaster Defense Skill [Right mouse button for more]"},
	{"No Hit Points Skill [Right mouse button for more]",
	 "Basic Hit Points Skill [Right mouse button for more]",
	 "Advanced Hit Points Skill [Right mouse button for more]",
	 "Expert Hit Points Skill [Right mouse button for more]",
	 "Master Hit Points Skill [Right mouse button for more]",
	 "Grandmaster Hit Points Skill [Right mouse button for more]"},
	{"No Damage Skill [Right mouse button for more]",
	 "Basic Damage Skill [Right mouse button for more]",
	 "Advanced Damage Skill [Right mouse button for more]",
	 "Expert Damage Skill [Right mouse button for more]",
	 "Master Damage Skill [Right mouse button for more]",
	 "Grandmaster Damage Skill [Right mouse button for more]"},
	{"No Magic Power Skill [Right mouse button for more]",
	 "Basic Magic Power Skill [Right mouse button for more]",
	 "Advanced Magic Power Skill [Right mouse button for more]",
	 "Expert Magic Power Skill [Right mouse button for more]",
	 "Master Magic Power Skill [Right mouse button for more]",
	 "Grandmaster Magic Power Skill [Right mouse button for more]"},
	{"No Speed Skill [Right mouse button for more]",
	 "Basic Speed Skill [Right mouse button for more]",
	 "Advanced Speed Skill [Right mouse button for more]",
	 "Expert Speed Skill [Right mouse button for more]",
	 "Master Speed Skill [Right mouse button for more]",
	 "Grandmaster Speed Skill [Right mouse button for more]"}
};*/
char *NPC::BonusPopUpText[6][6];/*={
//  {2,5,9,15,25},     // +AT
	{"\nNo Attack Skill\n\nYour Commander has only Basic 5 Attack\n\nThe next levels of Attack Skill can give\nBasic: 7 Attack\nAdvanced: 10 Attack\nExpert: 14 Attack\nMaster: 20 Attack\nGrandmaster: 30 Attack\n\nWith Grandmaster Attack, your Commander can get Extra skills\nwith Grandmaster Defence skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Attack twice\nwith Grandmaster Damage skill: Maximum damage always\nwith Grandmaster Magic Power skill: No Enemy Retaliation\nwith Grandmaster Speed skill: Can Shoot\n",
	 "\nBasic Attack Skill\n\nYour Commander has 7 Attack\n\nThe next levels of Attack Skill can give\nAdvanced: 10 Attack\nExpert: 14 Attack\nMaster: 20 Attack\nGrandmaster: 30 Attack\n\nWith Grandmaster Attack, your Commander can get Extra skills\nwith Grandmaster Defence skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Attack twice\nwith Grandmaster Damage skill: Maximum damage always\nwith Grandmaster Magic Power skill: No Enemy Retaliation\nwith Grandmaster Speed skill: Can Shoot\n",
	 "\nAdvanced Attack Skill\n\nYour Commander has 10 Attack\n\nThe next levels of Attack Skill can give\nExpert: 14 Attack\nMaster: 20 Attack\nGrandmaster: 30 Attack\n\nWith Grandmaster Attack, your Commander can get Extra skills\nwith Grandmaster Defence skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Attack twice\nwith Grandmaster Damage skill: Maximum damage always\nwith Grandmaster Magic Power skill: No Enemy Retaliation\nwith Grandmaster Speed skill: Can Shoot\n",
	 "\nExpert Attack Skill\n\nYour Commander has 14 Attack\n\nThe next levels of Attack Skill can give\nMaster: 20 Attack\nGrandmaster: 30 Attack\n\nWith Grandmaster Attack, your Commander can get Extra skills\nwith Grandmaster Defence skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Attack twice\nwith Grandmaster Damage skill: Maximum damage always\nwith Grandmaster Magic Power skill: No Enemy Retaliation\nwith Grandmaster Speed skill: Can Shoot\n",
	 "\nMaster Attack Skill\n\nYour Commander has 20 Attack\n\nThe next level of Attack Skill can give\nGrandmaster: 30 Attack\n\nWith Grandmaster Attack, your Commander can get Extra skills\nwith Grandmaster Defence skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Attack twice\nwith Grandmaster Damage skill: Maximum damage always\nwith Grandmaster Magic Power skill: No Enemy Retaliation\nwith Grandmaster Speed skill: Can Shoot\n",
	 "\nGrandmaster Attack Skill\n\nYour Commander has 30 Attack\n\nThis is the last level of Attack Skill\n\nWith Grandmaster Attack, your Commander can get Extra skills\nwith Grandmaster Defence skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Attack twice\nwith Grandmaster Damage skill: Maximum damage always\nwith Grandmaster Magic Power skill: No Enemy Retaliation\nwith Grandmaster Speed skill: Can Shoot\n"},
//  {4,10,18,30,50},   // +DF
	{"\nNo Defense Skill\n\nYour Commander has only Basic 5 Defense\n\nThe next levels of Defense Skill can give\nBasic: 9 Defense\nAdvanced: 15 Defense\nExpert: 23 Defense\nMaster: 35 Defense\nGrandmaster: 55 Defense\n\nWith Grandmaster Defense, your Commander can get Extra skills\nwith Grandmaster Attack skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Endless Retaliation\nwith Grandmaster Damage skill: Strike all Enemies around\nwith Grandmaster Magic Power skill: Melee Attack may Paralyse\nwith Grandmaster Speed skill: Ignore Obstacles [fly]\n",
	 "\nBasic Defense Skill\n\nYour Commander has 9 Defense\n\nThe next levels of Defense Skill can give\nAdvanced: 15 Defense\nExpert: 23 Defense\nMaster: 35 Defense\nGrandmaster: 55 Defense\n\nWith Grandmaster Defense, your Commander can get Extra skills\nwith Grandmaster Attack skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Endless Retaliation\nwith Grandmaster Damage skill: Strike all Enemies around\nwith Grandmaster Magic Power skill: Melee Attack may Paralyse\nwith Grandmaster Speed skill: Ignore Obstacles [fly]\n",
	 "\nAdvanced Defense Skill\n\nYour Commander has 15 Defense\n\nThe next levels of Defense Skill can give\nExpert: 23 Defense\nMaster: 35 Defense\nGrandmaster: 55 Defense\n\nWith Grandmaster Defense, your Commander can get Extra skills\nwith Grandmaster Attack skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Endless Retaliation\nwith Grandmaster Damage skill: Strike all Enemies around\nwith Grandmaster Magic Power skill: Melee Attack may Paralyse\nwith Grandmaster Speed skill: Ignore Obstacles [fly]\n",
	 "\nExpert Defense Skill\n\nYour Commander has 23 Defense\n\nThe next levels of Defense Skill can give\nMaster: 35 Defense\nGrandmaster: 55 Defense\n\nWith Grandmaster Defense, your Commander can get Extra skills\nwith Grandmaster Attack skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Endless Retaliation\nwith Grandmaster Damage skill: Strike all Enemies around\nwith Grandmaster Magic Power skill: Melee Attack may Paralyse\nwith Grandmaster Speed skill: Ignore Obstacles [fly]\n",
	 "\nMaster Defense Skill\n\nYour Commander has 35 Defense\n\nThe next level of Defense Skill can give\nGrandmaster: 55 Defense\n\nWith Grandmaster Defense, your Commander can get Extra skills\nwith Grandmaster Attack skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Endless Retaliation\nwith Grandmaster Damage skill: Strike all Enemies around\nwith Grandmaster Magic Power skill: Melee Attack may Paralyse\nwith Grandmaster Speed skill: Ignore Obstacles [fly]\n",
	 "\nGrandmaster Defense Skill\n\nYour Commander has 55 Defense\n\nThis is the last level of Defense Skill\n\nWith Grandmaster Defense, your Commander can get Extra skills\nwith Grandmaster Attack skill: Reduce Enemy Defense by 50%\nwith Grandmaster HitPoints skill: Endless Retaliation\nwith Grandmaster Damage skill: Strike all Enemies around\nwith Grandmaster Magic Power skill: Melee Attack may Paralyse\nwith Grandmaster Speed skill: Ignore Obstacles [fly]\n"},
//  {10,25,45,70,100}, // +HP%
	{"\nNo Hit Points Skill\n\nYour Commander has only\nBasic 20HP+Level*20\n\nThe next levels of Hit Points Skill can give\nBasic: plus 10% to HP\nAdvanced: plus 25% to HP\nExpert: plus 45% to HP\nMaster: plus 70% to HP\nGrandmaster: plus 100% to HP\n\nWith Grandmaster Hit Points, your Commander can get Extra skills\nwith Grandmaster Attack skill: Attack twice\nwith Grandmaster Defense skill: Endless Retaliation\nwith Grandmaster Damage skill: Fearsome\nwith Grandmaster Magic Power skill: Permanent Fireshield\nwith Grandmaster Speed skill: Regeneration 50 HP every turn\n",
	 "\nBasic Hit Points Skill\n\nYour Commander has\nBasic 20HP+Level*20 plus 10%\n\nThe next levels of Hit Points Skill can give\nAdvanced: plus 25% to HP\nExpert: plus 45% to HP\nMaster: plus 70% to HP\nGrandmaster: plus 100% to HP\n\nWith Grandmaster Hit Points, your Commander can get Extra skills\nwith Grandmaster Attack skill: Attack twice\nwith Grandmaster Defense skill: Endless Retaliation\nwith Grandmaster Damage skill: Fearsome\nwith Grandmaster Magic Power skill: Permanent Fireshield\nwith Grandmaster Speed skill: Regeneration 50 HP every turn\n",
	 "\nAdvanced Hit Points Skill\n\nYour Commander has\nBasic 20HP+Level*20 plus 25%\n\nThe next levels of Hit Points Skill can give\nExpert: plus 45% to HP\nMaster: plus 70% to HP\nGrandmaster: plus 100% to HP\n\nWith Grandmaster Hit Points, your Commander can get Extra skills\nwith Grandmaster Attack skill: Attack twice\nwith Grandmaster Defense skill: Endless Retaliation\nwith Grandmaster Damage skill: Fearsome\nwith Grandmaster Magic Power skill: Permanent Fireshield\nwith Grandmaster Speed skill: Regeneration 50 HP every turn\n",
	 "\nExpert Hit Points Skill\n\nYour Commander has\nBasic 20HP+Level*20 plus 45%\n\nThe next levels of Hit Points Skill can give\nMaster: plus 70% to HP\nGrandmaster: plus 100% to HP\n\nWith Grandmaster Hit Points, your Commander can get Extra skills\nwith Grandmaster Attack skill: Attack twice\nwith Grandmaster Defense skill: Endless Retaliation\nwith Grandmaster Damage skill: Fearsome\nwith Grandmaster Magic Power skill: Permanent Fireshield\nwith Grandmaster Speed skill: Regeneration 50 HP every turn\n",
	 "\nMaster Hit Points Skill\n\nYour Commander has\nBasic 20HP+Level*20 plus 70%\n\nThe next levels of Hit Points Skill can give\nGrandmaster: plus 100% to HP\n\nWith Grandmaster Hit Points, your Commander can get Extra skills\nwith Grandmaster Attack skill: Attack twice\nwith Grandmaster Defense skill: Endless Retaliation\nwith Grandmaster Damage skill: Fearsome\nwith Grandmaster Magic Power skill: Permanent Fireshield\nwith Grandmaster Speed skill: Regeneration 50 HP every turn\n",
	 "\nGrandmaster Hit Points Skill\n\nYour Commander has\nBasic 20HP+Level*20 plus 100%\n\nThis is the last level of Hit Points Skill\n\nWith Grandmaster Hit Points, your Commander can get Extra skills\nwith Grandmaster Attack skill: Attack twice\nwith Grandmaster Defense skill: Endless Retaliation\nwith Grandmaster Damage skill: Fearsome\nwith Grandmaster Magic Power skill: Permanent Fireshield\nwith Grandmaster Speed skill: Regeneration 50 HP every turn\n"},
//  {10,25,45,70,100}, // +DM%
	{"\nNo Damage Skill\n\nYour Commander has\nBasic 8+Level*4 Damage\n\nThe next levels of Damage Skill can give\nBasic: plus 10% to Damage\nAdvanced: plus 25% to Damage\nExpert: plus 45% to Damage\nMaster: plus 70% to Damage\nGrandmaster: plus 100% to Damage\n\nWith Grandmaster Damage, your Commander can get Extra skills\nwith Grandmaster Attack skill: Maximum damage always\nwith Grandmaster Defense skill: Strike all Enemies around\nwith Grandmaster Hit Points skill: Fearsome\nwith Grandmaster Magic Power skill: DeathStare\nwith Grandmaster Speed skill: Champion Distance Bonus\n",
	"\nBasic Damage Skill\n\nYour Commander has\nBasic 8+Level*4 Damage + 10%\n\nThe next levels of Damage Skill can give\nAdvanced: plus 25% to Damage\nExpert: plus 45% to Damage\nMaster: plus 70% to Damage\nGrandmaster: plus 100% to Damage\n\nWith Grandmaster Damage, your Commander can get Extra skills\nwith Grandmaster Attack skill: Maximum damage always\nwith Grandmaster Defense skill: Strike all Enemies around\nwith Grandmaster Hit Points skill: Fearsome\nwith Grandmaster Magic Power skill: DeathStare\nwith Grandmaster Speed skill: Champion Distance Bonus\n",
	"\nAdvanced Damage Skill\n\nYour Commander has\nBasic 8+Level*4 Damage + 25%\n\nThe next levels of Damage Skill can give\nExpert: plus 45% to Damage\nMaster: plus 70% to Damage\nGrandmaster: plus 100% to Damage\n\nWith Grandmaster Damage, your Commander can get Extra skills\nwith Grandmaster Attack skill: Maximum damage always\nwith Grandmaster Defense skill: Strike all Enemies around\nwith Grandmaster Hit Points skill: Fearsome\nwith Grandmaster Magic Power skill: DeathStare\nwith Grandmaster Speed skill: Champion Distance Bonus\n",
	"\nExpert Damage Skill\n\nYour Commander has\nBasic 8+Level*4 Damage + 45%\n\nThe next levels of Damage Skill can give\nMaster: plus 70% to Damage\nGrandmaster: plus 100% to Damage\n\nWith Grandmaster Damage, your Commander can get Extra skills\nwith Grandmaster Attack skill: Maximum damage always\nwith Grandmaster Defense skill: Strike all Enemies around\nwith Grandmaster Hit Points skill: Fearsome\nwith Grandmaster Magic Power skill: DeathStare\nwith Grandmaster Speed skill: Champion Distance Bonus\n",
	"\nMaster Damage Skill\n\nYour Commander has\nBasic 8+Level*4 Damage + 70%\n\nThe next levels of Damage Skill can give\nGrandmaster: plus 100% to Damage\n\nWith Grandmaster Damage, your Commander can get Extra skills\nwith Grandmaster Attack skill: Maximum damage always\nwith Grandmaster Defense skill: Strike all Enemies around\nwith Grandmaster Hit Points skill: Fearsome\nwith Grandmaster Magic Power skill: DeathStare\nwith Grandmaster Speed skill: Champion Distance Bonus\n",
	"\nGrandmaster Damage Skill\n\nYour Commander has\nBasic 8+Level*4 Damage + 100%\n\nThis is the last level of Damage Skill\n\nWith Grandmaster Damage, your Commander can get Extra skills\nwith Grandmaster Attack skill: Maximum damage always\nwith Grandmaster Defense skill: Strike all Enemies around\nwith Grandmaster Hit Points skill: Fearsome\nwith Grandmaster Magic Power skill: DeathStare\nwith Grandmaster Speed skill: Champion Distance Bonus\n"},
//  {1,3,6,14,29},     // +MP
//  {5,15,35,60,90}    // +MR%
	{"\nNo Magic Power Skill\n\nYour Commander has only\nBasic 1 Magic Power\nMay cast 1 time\nBasic 5% Magic Resistance\n\nThe next levels of Magic Power Skill can give\nBasic: 2 Magic Power, casts 2 times, 10% Magic Resistance\nAdvanced: 4 Magic Power, casts 3 times, 20% Magic Resistance\nExpert: 7 Magic Power, casts 4 times, 40% Magic Resistance\nMaster: 15 Magic Power, casts 5 times, 65% Magic Resistance\nGrandmaster: 30 Magic Power, casts 6 times, 95% Magic Resistance\n\nWith Grandmaster Magic Power, your Commander can get Extra skills\nwith Grandmaster Attack skill: No Enemy Retaliation\nwith Grandmaster Defense skill: Melee Attack may Paralyse\nwith Grandmaster Hit Points skill: Permanent Fireshield\nwith Grandmaster Damage skill: DeathStare\nwith Grandmaster Speed skill: Summon Creature Stack\n",
	 "\nBasic Magic Power Skill\n\nYour Commander has\n2 Magic Power\nMay cast 2 times\n10% Magic Resistance\n\nThe next levels of Magic Power Skill can give\nAdvanced: 4 Magic Power, casts 3 times, 20% Magic Resistance\nExpert: 7 Magic Power, casts 4 times, 40% Magic Resistance\nMaster: 15 Magic Power, casts 5 times, 65% Magic Resistance\nGrandmaster: 30 Magic Power, casts 6 times, 95% Magic Resistance\n\nWith Grandmaster Magic Power, your Commander can get Extra skills\nwith Grandmaster Attack skill: No Enemy Retaliation\nwith Grandmaster Defense skill: Melee Attack may Paralyse\nwith Grandmaster Hit Points skill: Permanent Fireshield\nwith Grandmaster Damage skill: DeathStare\nwith Grandmaster Speed skill: Summon Creature Stack\n",
	 "\nAdvanced Magic Power Skill\n\nYour Commander has\n4 Magic Power\nMay cast 3 times\n20% Magic Resistance\n\nThe next levels of Magic Power Skill can give\nExpert: 7 Magic Power, casts 4 times, 40% Magic Resistance\nMaster: 15 Magic Power, casts 5 times, 65% Magic Resistance\nGrandmaster: 30 Magic Power, casts 6 times, 95% Magic Resistance\n\nWith Grandmaster Magic Power, your Commander can get Extra skills\nwith Grandmaster Attack skill: No Enemy Retaliation\nwith Grandmaster Defense skill: Melee Attack may Paralyse\nwith Grandmaster Hit Points skill: Permanent Fireshield\nwith Grandmaster Damage skill: DeathStare\nwith Grandmaster Speed skill: Summon Creature Stack\n",
	 "\nExpert Magic Power Skill\n\nYour Commander has\n7 Magic Power\nMay cast 4 times\n40% Magic Resistance\n\nThe next levels of Magic Power Skill can give\nMaster: 15 Magic Power, casts 5 times, 65% Magic Resistance\nGrandmaster: 30 Magic Power, casts 6 times, 95% Magic Resistance\n\nWith Grandmaster Magic Power, your Commander can get Extra skills\nwith Grandmaster Attack skill: No Enemy Retaliation\nwith Grandmaster Defense skill: Melee Attack may Paralyse\nwith Grandmaster Hit Points skill: Permanent Fireshield\nwith Grandmaster Damage skill: DeathStare\nwith Grandmaster Speed skill: Summon Creature Stack\n",
	 "\nMaster Magic Power Skill\n\nYour Commander has\n15 Magic Power\nMay cast 5 times\n65% Magic Resistance\n\nThe next levels of Magic Power Skill can give\nGrandmaster: 30 Magic Power, casts 6 times, 95% Magic Resistance\n\nWith Grandmaster Magic Power, your Commander can get Extra skills\nwith Grandmaster Attack skill: No Enemy Retaliation\nwith Grandmaster Defense skill: Melee Attack may Paralyse\nwith Grandmaster Hit Points skill: Permanent Fireshield\nwith Grandmaster Damage skill: DeathStare\nwith Grandmaster Speed skill: Summon Creature Stack\n",
	 "\nGrandmaster Magic Power Skill\n\nYour Commander has\n30 Magic Power\nMay cast 6 times\n95% Magic Resistance\n\nThis is the last level of Magic Power Skill\n\nWith Grandmaster Magic Power, your Commander can get Extra skills\nwith Grandmaster Attack skill: No Enemy Retaliation\nwith Grandmaster Defense skill: Melee Attack may Paralyse\nwith Grandmaster Hit Points skill: Permanent Fireshield\nwith Grandmaster Damage skill: DeathStare\nwith Grandmaster Speed skill: Summon Creature Stack\n"},
//  {1,2,3,4,6},       // +SP
	{"\nNo Speed Skill\n\nYour Commander has only Basic 4 Speed\n\nThe next levels of Speed Skill can give\nBasic: 5 Speed\nAdvanced: 6 Speed\nExpert: 7 Speed\nMaster: 8 Speed\nGrandmaster: 10 Speed\n\nWith Grandmaster Speed, your Commander can get Extra skills\nwith Grandmaster Attack skill: Can Shoot\nwith Grandmaster Defense skill: Ignore Obstacles [fly]\nwith Grandmaster HitPoints skill: Regeneration 50 HP every turn\nwith Grandmaster Damage skill: Champion Distance Bonus\nwith Grandmaster Magic Power skill: Summon Creature Stack\n",
	 "\nBasic Speed Skill\n\nYour Commander has 5 Speed\n\nThe next levels of Speed Skill can give\nAdvanced: 6 Speed\nExpert: 7 Speed\nMaster: 8 Speed\nGrandmaster: 10 Speed\n\nWith Grandmaster Speed, your Commander can get Extra skills\nwith Grandmaster Attack skill: Can Shoot\nwith Grandmaster Defense skill: Ignore Obstacles [fly]\nwith Grandmaster HitPoints skill: Regeneration 50 HP every turn\nwith Grandmaster Damage skill: Champion Distance Bonus\nwith Grandmaster Magic Power skill: Summon Creature Stack\n",
	 "\nAdvanced Speed Skill\n\nYour Commander has 6 Speed\n\nThe next levels of Speed Skill can give\nExpert: 7 Speed\nMaster: 8 Speed\nGrandmaster: 10 Speed\n\nWith Grandmaster Speed, your Commander can get Extra skills\nwith Grandmaster Attack skill: Can Shoot\nwith Grandmaster Defense skill: Ignore Obstacles [fly]\nwith Grandmaster HitPoints skill: Regeneration 50 HP every turn\nwith Grandmaster Damage skill: Champion Distance Bonus\nwith Grandmaster Magic Power skill: Summon Creature Stack\n",
	 "\nExpert Speed Skill\n\nYour Commander has 7 Speed\n\nThe next levels of Speed Skill can give\nMaster: 8 Speed\nGrandmaster: 10 Speed\n\nWith Grandmaster Speed, your Commander can get Extra skills\nwith Grandmaster Attack skill: Can Shoot\nwith Grandmaster Defense skill: Ignore Obstacles [fly]\nwith Grandmaster HitPoints skill: Regeneration 50 HP every turn\nwith Grandmaster Damage skill: Champion Distance Bonus\nwith Grandmaster Magic Power skill: Summon Creature Stack\n",
	 "\nMaster Speed Skill\n\nYour Commander has 8 Speed\n\nThe next levels of Speed Skill can give\nGrandmaster: 10 Speed\n\nWith Grandmaster Speed, your Commander can get Extra skills\nwith Grandmaster Attack skill: Can Shoot\nwith Grandmaster Defense skill: Ignore Obstacles [fly]\nwith Grandmaster HitPoints skill: Regeneration 50 HP every turn\nwith Grandmaster Damage skill: Champion Distance Bonus\nwith Grandmaster Magic Power skill: Summon Creature Stack\n",
	 "\nGrandmaster Speed Skill\n\nYour Commander has 10 Speed\n\nThis is the last level of Speed Skill\n\nWith Grandmaster Speed, your Commander can get Extra skills\nwith Grandmaster Attack skill: Can Shoot\nwith Grandmaster Defense skill: Ignore Obstacles [fly]\nwith Grandmaster HitPoints skill: Regeneration 50 HP every turn\nwith Grandmaster Damage skill: Champion Distance Bonus\nwith Grandmaster Magic Power skill: Summon Creature Stack\n"},
};*/

// AT,DF,HP,DM,MP,SP
int NPC::Spec2Ind[15][2]={
	{0,1},{0,2},{0,3},{0,4},{0,5},
	{1,2},{1,3},{1,4},{1,5},
	{2,3},{2,4},{2,5},
	{3,4},{3,5},
	{4,5}
};
int NPC::GetAvailableSpecBon(int step){
	STARTNA(__LINE__, 0)
	Dword mask;
	int   i,SBLevel[15];
	for(i=0;i<15;i++) SBLevel[i]=0;
	if(ATS>=4){
		if(DFS>=4) SBLevel[0]=ATS+DFS;
		if(HPS>=4) SBLevel[1]=ATS+HPS;
		if(DMS>=4) SBLevel[2]=ATS+DMS;
		if(MPS>=4) SBLevel[3]=ATS+MPS;
		if(SPS>=4) SBLevel[4]=ATS+SPS;
	}
	if(DFS>=4){
		if(HPS>=4) SBLevel[5]=DFS+HPS;
		if(DMS>=4) SBLevel[6]=DFS+DMS;
		if(MPS>=4) SBLevel[7]=DFS+MPS;
		if(SPS>=4) SBLevel[8]=DFS+SPS;
	}
	if(HPS>=4){
		if(DMS>=4) SBLevel[9]=HPS+DMS;
		if(MPS>=4) SBLevel[10]=HPS+MPS;
		if(SPS>=4) SBLevel[11]=HPS+SPS;
	}
	if(DMS>=4){
		if(MPS>=4) SBLevel[12]=DMS+MPS;
		if(SPS>=4) SBLevel[13]=DMS+SPS;
	}
	if(MPS>=4){
		if(SPS>=4) SBLevel[14]=MPS+SPS;
	}
/*
	for(val=0,ind=-1,i=0;i<15;i++){
		mask=1<<i;
		if(SBLevel[i]>val){
			if(SpecBon[0]&mask) continue; // уже есть
			if(SpecBon[1]&mask) continue; // нет, но нельзя этот
			val=SBLevel[i]; ind=i;
		}
	}
	return ind;
*/
	for(i=0;i<15;i++){
		mask=1<<i;
		if(SBLevel[i]>0){
			if(SpecBon[0]&mask) continue; // уже есть
			if(SpecBon[1]&mask) continue; // нет, но нельзя этот
			if(step==0){ RETURN(i) }
			else step--;
		}
	}
	RETURN(-1)
}

int NPC::GetNextLevel(void){
	STARTNA(__LINE__, 0)
//  int i;
//  float mul;
	if(Level>73){ RETURN(0)
//    for(mul=1.0,i=70;i<Level;i++) mul*=1.2;
//    RETURN(Levels[70]*mul)
	}else RETURN(Levels[Level+1])
}
int NPC::CalculateSkills(int level)
{
	STARTNA(__LINE__, 0)
	int i,n=0;
	for(i=0;i<6;i++){
		if(Skills[i]>=level) n++;
	}
	RETURN(n)
}
int NPC::MayNextSkill(int ind)
{
	STARTNA(__LINE__, 0)
	switch(Skills[ind]){
		case 0: // -
			if(CalculateSkills(1)>=4) RETURN(0) // нельзя - уже 4 есть
			RETURN(1)
		case 1: // Basic
			if(CalculateSkills(1)<2) RETURN(0)
			RETURN(1)
		case 2: // Advanced
			if(CalculateSkills(1)<3) RETURN(0)
//      if(CalculateSkills(1)<4) return 0;
//      if(CalculateSkills(2)<3) return 0;
			RETURN(1)
		case 3: // Expert
			if(CalculateSkills(1)<4) RETURN(0)
//      if(CalculateSkills(2)<4) return 0;
//      if(CalculateSkills(3)<3) return 0;
			RETURN(1)
		case 4: // Master
			if(CalculateSkills(1)<4) RETURN(0)
			if(CalculateSkills(2)<4) RETURN(0)
//      if(CalculateSkills(3)<4) return 0;
//      if(CalculateSkills(4)<3) return 0;
			RETURN(1)
		case 5: // GrandMaster
			RETURN(0) // нельзя - все уже
	}
	RETURN(0)
}
int NPC::CalcSkill(int ind)
{
	STARTNA(__LINE__, 0)
	int i,fl,val,spec=0;
	val=Primary[ind];
	if(Skills[ind]>0){
//    if((ind==0)&&(Type==4)) spec=1; // AT & Necro
//    if((ind==1)&&(Type==8)) spec=1; // DF & Conflux
//    if((ind==3)&&(Type==5)) spec=1; // DM & Dungeon
//    if((ind==5)&&(Type==6)) spec=1; // SP & Stronhold
		if((ind==2)||(ind==3)){
			if(spec){
				val+=val*SpecBonus[ind][Skills[ind]-1]/100;
			}else{
				val+=val*Bonus[ind][Skills[ind]-1]/100;
			}
		}else{
			if(spec){
				val+=SpecBonus[ind][Skills[ind]-1];
			}else{
				val+=Bonus[ind][Skills[ind]-1];
			}
		}
	}
	for(i=fl=0;i<10;i++){
		if(Arts[i][ARTNUMINDEX]==(NPCARTBASE+9)){ // суперкольцо
			if(fl==1) continue;
			if(Skills[ind]<=2){
				if((ind==2)||(ind==3)) val+=val*Bonus[ind][1]/100;
				else  val+=Bonus[ind][1];
			}
			fl=1;
			continue;
		}
		if((ind==2)||(ind==3)){
			val+=val*ArtCalcSkill(i,ind)/100;
		}else{
			val+=ArtCalcSkill(i,ind);
		}
	}
	RETURN(val)
}
/*
int  NPC::AIPromote(void)
{
	int val=Random(1,100);
	HType

}
*/
static int SkillsUp[6];
_DlgNPC DlgNPC;
void NPC::AddExp(int NewExp,int Auto){
	STARTNA(__LINE__, 0)
	int i,j/*,ThisExp*/;
	Dword mask;
	int DelExp=NewExp-OldHeroExp;
	if(Type==0) DelExp=DelExp*150/100;
	int NExp=Exp+DelExp;
	int owner;
//  if(Fl.NoHeroOwner) owner=-1;
	owner=GetHeroStr(Number)->Owner;
	while(NExp>=GetNextLevel()){
//    ThisExp=GetNextLevel();
		if(Level==74) break; // the maximum
		Level++;
		if(Fl.CustomPrimary==0){ // если пользователь не сам контролирует
			HP=CALC_HP(Level);
			DM=CALC_DM(Level);
		}
		if((IsThis(owner)==0)||(Auto==1)||(IsAI(owner))||(owner==-1)||(IsStartUp())){
			int sum,val,chance[6];
			sum=0; // флаг проверки
			val=GetAvailableSpecBon(0);
			if(val!=-1){
				 mask=1<<val;
				 if((SpecBon[1]&mask)==0){ // можно этот бонус
					 SpecBon[0]|=mask;
					 sum=1; // уже что-то добавили
				 }
			}
			if(sum==0){ // не добавлено?
				for(sum=0,i=0;i<6;i++){
					if(MayNextSkill(i)) chance[i]=AISkillsChance[HType][i];
					else chance[i]=0;
					sum+=chance[i];
				}
				if(sum!=0){ // есть что улучшать
					val=Random(1,sum);
					for(i=0;i<6;i++){
						if(val<=chance[i]){ Skills[i]++; if(i==4) Skills[6]++; break; }
						val-=chance[i];
					}
				}
			}
		}else{
			//!!! тут запрос что добавлять
//      Zsprintf2(&Buffer,"%s has reached level %i",(Dword)Name,(Dword)Level+1);
//      Message(Buffer.Str,1);
//      if((Level%3)!=0){ // каждый третий пропускаем
/*
			int   mayimprove=0;
			char *txt[12]={0,0,0,0,0,0,0,0,0,0,0,0};
			for(i=0;i<6;i++){
				if(MayNextSkill(i)){
					mayimprove=1;
					SkNames[i][1]=char('0'+Skills[i]);
					txt[i]=SkNames[i];
				}
			}
			if(mayimprove){
				int ret=MultiCheckReq("Choose the skill to improve",1,txt,1);
				if(ret&1) Skills[0]++;
				else if(ret&2) Skills[1]++;
				else if(ret&4) Skills[2]++;
				else if(ret&8) Skills[3]++;
				else if(ret&16){ Skills[4]++; Skills[6]++; }
				else if(ret&32) Skills[5]++;
			}
*/
			Exp=NExp/*ThisExp*/;
			ToText(1);
			i=ShowNPC(&DlgNPC);
			for(j=0;j<6;j++){
				if(DlgNPC.ArtOutput[j]==1){ // хочет передать артифакт обратно герою - нельзя
					ITxt(213,1,&NPCTexts);
					break;
				}
			}
			switch(i){
				case -1: Used=0; RETURNV
				case  0: break;
				default:
					if(SkillsUp[i-1]<0){ // спец бонус
						mask=1<<(-SkillsUp[i-1]-1);
						if(SpecBon[1]&mask) break; // нельзя этот бонус
						SpecBon[0]|=mask;
					}else{ // скил
						Skills[SkillsUp[i-1]]++;
						if(SkillsUp[i-1]==4) Skills[6]++;
					}
			}

		}
	}
	Exp=NExp;
	OldHeroExp=NewExp;
	if((Type==5)&&(LastExpoInBattle==1)/*&&(Fl.NoHeroOwner==0)*/){ // Inferno
		LastExpoInBattle=0;
		int owner=GetHeroStr(Number)->Owner;
		if(owner==-1) goto _Ok;
		int gold=GetRes(owner,6);
		int delt=DelExp*50/100;
		if(IsThis(owner) && (delt!=0)){
			RequestPic(ITxt(9*3+6*6*4+4,1,&NPCTexts),6,delt,1);
		}
		gold+=delt;
		SetRes(owner,6,gold);
	}
_Ok:;
	RETURNV
}

void NPC::Init(void){
	STARTNA(__LINE__, 0)
	int i,j;
//  Copy((Byte *)this,0,sizeof(NPC));
	for(i=0;i<7;i++) Skills[i]=0;
	AT=5; DF=5; SP=4; DM=12; MP=1; HP=40; MR=5;
//  for(i=0;i<32;i++) Name[i]=0;
//  Name[0]='N'; Name[1]='P'; Name[2]='C';
//  for(i=0;i<HERNUM;i++){
		char *s=ITxt(Number,0,&NPCBios);
		for(j=0;j<31;j++){
			Name[j]=s[j]; if(s[j]==0) break;
		}
		Name[31]=0;
//  }
	for(i=0;i<10;i++)
		for(j=0;j<8;j++)
			Arts[i][j]=0;
	Exp=0; Level=0; OldHeroExp=0;
	LastExpoInBattle=0;
	Fl.CustomPrimary=0;
	//!!!
	SpecBon[0]=SpecBon[1]=0;
	HType=GetHeroStr(Number)->Spec;
	Type=HType/2;
	Used=0; // запрещаем всем сразу
	Dead=0;
	RETURNV
}
NPC::NPC(){ /*Init();*/ }

int NPC::ArtCalcSkill(int Slot,int Skill)
{
	STARTNA(__LINE__, 0)
	int ArtNum,val=0;
	ArtNum=Arts[Slot][ARTNUMINDEX];
	switch(ArtNum){
		case NPCARTBASE: // +5 Attack + 1 for 6 battles
			if(Skill!=0) break;
			val=5+Arts[Slot][ARTBATTLES]/6;
			break;
		case NPCARTBASE+1: // +12% HP + 1% for 1 battles
			if(Skill!=2) break;
			val=12+Arts[Slot][ARTBATTLES];
			break;
		case NPCARTBASE+2: // +12% DM + 1% for 1 battles
			if(Skill!=3) break;
			val=12+Arts[Slot][ARTBATTLES];
			break;
		case NPCARTBASE+3: break; // nothing yet
		case NPCARTBASE+4: // MP
			if(Skill!=4) break;
			val=1+Arts[Slot][ARTBATTLES]/10;
			break;
		case NPCARTBASE+5: // SP
			if(Skill!=5) break;
			val=1+Arts[Slot][ARTBATTLES]/10;
			break;
		case NPCARTBASE+6: break; // nothing yet
		case NPCARTBASE+7: break; // nothing yet
		case NPCARTBASE+8: // +5 Defense + 1 for 6 battles
			if(Skill!=1) break;
			val=5+Arts[Slot][ARTBATTLES]/6;
			break;
		case NPCARTBASE+9: break; // nothing yet
	}
	RETURN(val)
}

char  NPC::ArtTextBuffer[10][512];
char *NPC::PrepareArtText(int Slot)
{
	STARTNA(__LINE__, 0)
	int i,val,days,ind;
	int ArtNum=Arts[Slot][ARTNUMINDEX];
	ind=ArtNum-NPCARTBASE;
	char *templ=ITxt(226+ind,1,&NPCTexts);
	strcpy(ArtTextBuffer[Slot], templ);
	ArtTextBuffer[Slot][i]=0;
	switch(ArtNum){
		case NPCARTBASE: // +5 Attack + 1 for 6 battles
			val=ArtCalcSkill(Slot,0);
			days=6-Arts[Slot][ARTBATTLES]%6;
			Zsprintf3(&Buffer,ArtTextBuffer[Slot],(Dword)val,(Dword)val+1,(Dword)days);
			StrCopy(ArtTextBuffer[Slot],511,Buffer.Str);
			break;
		case NPCARTBASE+1: // +12% HP + 1% for 1 battles
			val=ArtCalcSkill(Slot,2);
			days=1-Arts[Slot][ARTBATTLES]%1;
			Zsprintf3(&Buffer,ArtTextBuffer[Slot],(Dword)val,(Dword)val+1,(Dword)days);
			StrCopy(ArtTextBuffer[Slot],511,Buffer.Str);
			break;
		case NPCARTBASE+2: // +12% DM + 1% for 1 battles
			val=ArtCalcSkill(Slot,3);
			days=1-Arts[Slot][ARTBATTLES]%1;
			Zsprintf3(&Buffer,ArtTextBuffer[Slot],(Dword)val,(Dword)val+1,(Dword)days);
			StrCopy(ArtTextBuffer[Slot],511,Buffer.Str);
			break;
		case NPCARTBASE+3: break; // nothing yet
		case NPCARTBASE+4: // 1 MP + 1 for 10 battles
			val=ArtCalcSkill(Slot,4);
			days=10-Arts[Slot][ARTBATTLES]%10;
			Zsprintf3(&Buffer,ArtTextBuffer[Slot],(Dword)val,(Dword)val+1,(Dword)days);
			StrCopy(ArtTextBuffer[Slot],511,Buffer.Str);
			break;
		case NPCARTBASE+5: // 1 SP + 1 for 10 battles
			val=ArtCalcSkill(Slot,5);
			days=10-Arts[Slot][ARTBATTLES]%10;
			Zsprintf3(&Buffer,ArtTextBuffer[Slot],(Dword)val,(Dword)val+1,(Dword)days);
			StrCopy(ArtTextBuffer[Slot],511,Buffer.Str);
			break;
		case NPCARTBASE+6:  // nothing yet
			days=Arts[Slot][ARTBATTLES];
			if(SpecBon[0]&AT_SP){ // может стрелять
				if(days<25){ // +- 244 %
					StrCopy(ArtTextBuffer[Slot],511,ITxt(244,1,&NPCTexts));
					Zsprintf2(&Buffer,ArtTextBuffer[Slot],(Dword)25-days,0);
					StrCopy(ArtTextBuffer[Slot],511,Buffer.Str);
				}else{ // ++ 245
					StrCopy(ArtTextBuffer[Slot],511,ITxt(245,1,&NPCTexts));
				}
			}else{ // не может стрелять
				if(days<5){ // --- 246 %%%
					StrCopy(ArtTextBuffer[Slot],511,ITxt(246,1,&NPCTexts));
					Zsprintf3(&Buffer,ArtTextBuffer[Slot],(Dword)5-days,(Dword)25-days,(Dword)50-days);
				}else if(days<25){ // +-- 247 %%
					StrCopy(ArtTextBuffer[Slot],511,ITxt(247,1,&NPCTexts));
					Zsprintf2(&Buffer,ArtTextBuffer[Slot],(Dword)25-days,(Dword)50-days);
				}else if(days<50){ // ++- 248 %
					StrCopy(ArtTextBuffer[Slot],511,ITxt(248,1,&NPCTexts));
					Zsprintf2(&Buffer,ArtTextBuffer[Slot],(Dword)50-days,0);
				}else{ // +++ 249
					StrCopy(ArtTextBuffer[Slot],511,ITxt(249,1,&NPCTexts));
					break;
				}
				StrCopy(ArtTextBuffer[Slot],511,Buffer.Str);
				break;
			}
		case NPCARTBASE+7: break; // nothing yet
		case NPCARTBASE+8: // +5 Defense + 1 for 6 battles
			val=ArtCalcSkill(Slot,1);
			days=6-Arts[Slot][ARTBATTLES]%6;
			Zsprintf3(&Buffer,ArtTextBuffer[Slot],(Dword)val,(Dword)val+1,(Dword)days);
			StrCopy(ArtTextBuffer[Slot],511,Buffer.Str);
			break;
		case NPCARTBASE+9: break; // nothing yet
	}
	RETURN(ArtTextBuffer[Slot])
}

void NPC::ToText(int LevelUp,int ChangeDisabled,int Flag){
	STARTNA(__LINE__, 0)
	int i,j,k,l;
/*
	if(Used==0){ return "No NPC"; }
	Zsprintf2(&Buffer,"NPC Statistics\n\nName : %s\nDescr : %s",
									(Dword)Name,
									(Dword)Descr[Type]);
	Zsprintf3(&Buffer1,"%s\n\nAt=%i, Df=%i,",(Dword)Buffer.Str,CalcSkill(0),CalcSkill(1));
	Zsprintf3(&Buffer ,"%s Hp=%i, Sp=%i,",(Dword)Buffer1.Str,CalcSkill(2),CalcSkill(5));
	Zsprintf3(&Buffer1,"%s Dm=%i, Mp=%i,",(Dword)Buffer.Str,CalcSkill(3),CalcSkill(4));
	Zsprintf2(&Buffer ,"%s Mr=%i",(Dword)Buffer1.Str,CalcSkill(6));
	Zsprintf3(&Buffer1,"%s\n\nSk1=%i, Sk2=%i,",(Dword)Buffer.Str,Skills[0],Skills[1]);
	Zsprintf3(&Buffer ,"%s Sk3=%i, Sk4=%i,",(Dword)Buffer1.Str,Skills[2],Skills[3]);
	Zsprintf3(&Buffer1,"%s Sk5=%i, Sk6=%i,",(Dword)Buffer.Str,Skills[4],Skills[5]);
	Zsprintf3(&Buffer,"%s\n\nExp=%i, Level=%i,",(Dword)Buffer1.Str,GetExp(),Level+1);
	if(Dead){
		Zsprintf2(&Buffer1,"%s NextLevelExp=%i\n\nDEAD...DEAD...DEAD...DEAD",(Dword)Buffer.Str,GetNextLevel());
	}else{
		Zsprintf2(&Buffer1,"%s NextLevelExp=%i",(Dword)Buffer.Str,GetNextLevel());
	}
*/
////////////////////
//ShowWoGSetup();
 if(Type<0 || Type>8) Type=HType/2; // fix 3.58f if the type is incorrect set to Hero's type
 if(Type<0) Type=0; // fix 3.58f 
 if(Type>8) Type=8; // fix 3.58f 

 if(Used<=0) RETURNV
 _Hero_ *hp; if(Flag) hp=0; else hp=GetHeroStr(Number);
 DlgNPC.DlgLeft=-1000;
 if(LevelUp) DlgNPC.DlgTop=-1000; else DlgNPC.DlgTop=20;
 if(LevelUp){
	 Zsprintf2(&Buffer,ITxt(195,1,&NPCTexts)/*"%s has reached level %i"*/,(Dword)Name,(Dword)(Level+1));
	 DlgNPC.Name=Buffer.Str;
 }else{
	 DlgNPC.Name=Name;
 }
 DlgNPC.Portrait=ITxt(201+Type,1,&NPCTexts); // [LODDEF]H3sprite.lod;ZM174NPC.def;1;-1;-1
 DlgNPC.PortraitHint=ITxt(215,1,&NPCTexts);
 DlgNPC.Type=ITxt(216+Type,1,&NPCTexts);
 DlgNPC.TypeHint=ITxt(242,1,&NPCTexts);
 DlgNPC.TypePopUpText=Descr[Type];//"DlgNPC.TypePopUpText";
 DlgNPC.Description=ITxt(Number,1,&NPCBios);//Descr[Type];
 if(hp) DlgNPC.HeroName=hp->Name; else DlgNPC.HeroName="";
 DlgNPC.Level=Level+1;
 DlgNPC.pAT=CalcSkill(0);
 DlgNPC.pDF=CalcSkill(1);
 if(Flag==0){
	 if(Type==7){
		 DlgNPC.hAT=DlgNPC.pAT+hp->PSkill[0]*3/2;
		 DlgNPC.hDF=DlgNPC.pDF+hp->PSkill[1]*3/2;
	 }else{
		 DlgNPC.hAT=DlgNPC.pAT+hp->PSkill[0];
		 DlgNPC.hDF=DlgNPC.pDF+hp->PSkill[1];
	 }
 }else{
	 DlgNPC.hAT=DlgNPC.pAT;
	 DlgNPC.hDF=DlgNPC.pDF;
 }
 DlgNPC.pHP=CalcSkill(2);
 DlgNPC.pSP=CalcSkill(5);
 DlgNPC.pDMH=CalcSkill(3);
 DlgNPC.pDML=CALC_MinDM(CalcSkill(3));//CalcSkill(3)*3/4;
 DlgNPC.pMP=CalcSkill(4);
 DlgNPC.pMR=CalcSkill(6);
// if((ATS==5)&&(SPS==5)){
	if(SpecBon[0]&AT_SP){
	 DlgNPC.pShots=24;
 }else{
	 DlgNPC.pShots=0;
 }
 DlgNPC.CurExp=GetExp();
 DlgNPC.NextExp=GetNextLevel();
 DlgNPC.Bonus[0]=BonusBmp[0][Skills[0]];
 DlgNPC.Bonus[1]=BonusBmp[1][Skills[1]];
 DlgNPC.Bonus[2]=BonusBmp[2][Skills[2]];
 DlgNPC.Bonus[3]=BonusBmp[3][Skills[3]];
 DlgNPC.Bonus[4]=BonusBmp[4][Skills[4]];
 DlgNPC.Bonus[5]=BonusBmp[5][Skills[5]];
 DlgNPC.BonusHints[0]=BonusText[0][Skills[0]];
 DlgNPC.BonusHints[1]=BonusText[1][Skills[1]];
 DlgNPC.BonusHints[2]=BonusText[2][Skills[2]];
 DlgNPC.BonusHints[3]=BonusText[3][Skills[3]];
 DlgNPC.BonusHints[4]=BonusText[4][Skills[4]];
 DlgNPC.BonusHints[5]=BonusText[5][Skills[5]];
 DlgNPC.BonusPopUpText[0]=BonusPopUpText[0][Skills[0]];
 DlgNPC.BonusPopUpText[1]=BonusPopUpText[1][Skills[1]];
 DlgNPC.BonusPopUpText[2]=BonusPopUpText[2][Skills[2]];
 DlgNPC.BonusPopUpText[3]=BonusPopUpText[3][Skills[3]];
 DlgNPC.BonusPopUpText[4]=BonusPopUpText[4][Skills[4]];
 DlgNPC.BonusPopUpText[5]=BonusPopUpText[5][Skills[5]];
 int   p;
 k=0;
 for(i=0;i<6;i++){
	 DlgNPC.SpecBonus[i]="NONE";
	 DlgNPC.SpecBonusHints[i]="";
	 DlgNPC.SpecBonusPopUpText[i]="";
 }
 for(i=0;i<6;i++){
	 for(j=0;j<6;j++){
		 p=FindSpecBonusBmp(i,j);
		 if(p==-1) continue;
		 if(k>5) break; // НЕ ДОЛЖНО БЫТЬ
		 DlgNPC.SpecBonus[k]=SpecBonusBmp[i][j][p];
		 DlgNPC.SpecBonusHints[k]=SpecBonusText[i][j][p];
		 DlgNPC.SpecBonusPopUpText[k]=SpecBonusPopUpText[i][j];
		 k++;
	 }
 }
//__asm int 3
 DlgNPC.SpecIcon1=SpecBmp[Type][0];
// DlgNPC.SpecText1=SpecText[Type][0];
 DlgNPC.SpecPopUpText1=SpecText[Type][0];
 DlgNPC.SpecHint1=SpecHint[Type][0];
 DlgNPC.SpecIcon2=SpecBmp[Type][1];
// DlgNPC.SpecText2=SpecText[Type][1];
 DlgNPC.SpecPopUpText2=SpecText[Type][1];
 DlgNPC.SpecHint2=SpecHint[Type][1];
/*
 if(LevelUp){
	 DlgNPC.Flags=0x00000002;
 }else{
	 DlgNPC.Flags=0;//0x00000002;
 }
*/
 DlgNPC.Flags=0;
 if(Dead==0) DlgNPC.Flags|=1;
 if(Flag==0){
	 if((IsThis(hp->Owner)!=0)&&(hp->Owner==CurrentUser())&&(ChangeDisabled==0)){ // активный игрок
		 if(LevelUp==0) DlgNPC.Flags|=0x00000002; // можно имя задать
		 DlgNPC.Flags|=0x00000004; // можно передавать артифакты
		 DlgNPC.Flags|=0x00000008; // можно уволить
	 }else{ // пассивный
	 }
 }
 for(i=0;i<6;i++){
	 DlgNPC.ArtOutput[i]=0;
	 DlgNPC.ArtIcons[i]="NONE";
	 DlgNPC.ArtHints[i]=0;
	 DlgNPC.ArtPopUpTexts[i]=0;
	 if(Arts[i][ARTNUMINDEX]==0) continue;
	 if(Arts[i][ARTNUMINDEX]==0xFF){ // fix a possible bug
		 Arts[i][ARTNUMINDEX]=0;
		 continue;
	 }
	 k=Arts[i][ARTNUMINDEX]-NPCARTBASE;
	 DlgNPC.ArtIcons[i]=ArtPicNames[k];
	 DlgNPC.ArtHints[i]=ITxt(225,1,&NPCTexts); // хинт для иконки артифакта
	 DlgNPC.ArtPopUpTexts[i]=PrepareArtText(i); // текст для попап окна
 }
 for(i=0;i<6;i++){
	 if(Arts[i][ARTNUMINDEX]!=(NPCARTBASE+9)) continue;
	 for(j=0;j<6;j++){
		 if(Skills[j]>=2) continue;
		 DlgNPC.Bonus[j]=BonusArtBmp[j];
		 DlgNPC.BonusHints[j]=BonusText[j][2];
		 DlgNPC.BonusPopUpText[j]=ITxt(236+j,1,&NPCTexts);
	 }
 }
 if(LevelUp){
	 for(i=0;i<6;i++){
		 SkillsUp[i]=-1;
		 DlgNPC.Next[i]="NONE";
		 DlgNPC.NextActive[i]="NONE";
	 }
	 for(k=0,i=0;i<6;i++){
		 if(MayNextSkill(i)){
			 SkillsUp[k]=i;
			 DlgNPC.Next[k]=BonusBmp[i][Skills[i]+1];   // 6 путей к нижним 6 картинкам 70 на 70 из которых
			 DlgNPC.NextActive[k]=BonusBmpA[i][Skills[i]+1];
			 DlgNPC.NextHints[k]=BonusText[i][Skills[i]+1]; // 6 хинтов к нижним 6 картинкам
			 DlgNPC.NextPopUpTexts[k]=BonusPopUpText[i][Skills[i]+1]; // 6 текстов к нижним 6 картинкам
			 k++;
		 }
	 }
/*
	 if(k<6){
		 l=GetAvailableSpecBon();
		 if(l!=-1){
			 SkillsUp[k]=-1-l; // спец. умение
			 i=Spec2Ind[l][0]; j=Spec2Ind[l][1];
			 DlgNPC.Next[k]=SpecBonusBmp[i][j][2];   // 6 путей к нижним 6 картинкам 70 на 70 из которых
			 DlgNPC.NextActive[k]=SpecBonusBmp[i][j][3];
			 DlgNPC.NextHints[k]=SpecBonusText[i][j][0]; // 6 хинтов к нижним 6 картинкам
			 DlgNPC.NextPopUpTexts[k]=SpecBonusPopUpText[i][j]; // 6 текстов к нижним 6 картинкам
			 k++;
		 }
	 }
*/
	 for(int k0=k;k<6;k++){
		 l=GetAvailableSpecBon(k-k0);
		 if(l==-1) break;
		 SkillsUp[k]=-1-l; // спец. умение
		 i=Spec2Ind[l][0]; j=Spec2Ind[l][1];
		 DlgNPC.Next[k]=SpecBonusBmp[i][j][2];   // 6 путей к нижним 6 картинкам 70 на 70 из которых
		 DlgNPC.NextActive[k]=SpecBonusBmp[i][j][3];
		 DlgNPC.NextHints[k]=SpecBonusText[i][j][0]; // 6 хинтов к нижним 6 картинкам
		 DlgNPC.NextPopUpTexts[k]=SpecBonusPopUpText[i][j]; // 6 текстов к нижним 6 картинкам
	 }
	 if(k!=0){
		 DlgNPC.Request=1;
	 }else{
		 DlgNPC.Request=0;
	 }
 }else{
	 DlgNPC.Request=0;
 }
///////////////////
//  return Buffer1.Str;
	RETURNV
}

char *NPC::ToHint(int ind){
	STARTNA(__LINE__, 0)
	int i;
	if(Used<=0){ RETURN("No Commander???") }
	if(Dead==1){ RETURN("is Dead???") }
	Zsprintf2(&Buffer,ITxt(196,1,&NPCTexts)/*"L:%i,MP:%i"*/,(Dword)Level+1,(Dword)CalcSkill(4));
	char t[17];
//  for(i=0;i<16;i++) t[i]=' ';
	i=0;
//  if((ATS==5)&&(DMS==5))
	if(SpecBon[0]&AT_DM) t[i++]=ITxt(199,1,&NPCTexts)[ 0];//'M'; // AT+DM Maximum damage always
//  if((HPS==5)&&(MPS==5))
	if(SpecBon[0]&HP_MP) t[i++]=ITxt(199,1,&NPCTexts)[ 1];//'P'; // HP+MP Melee Attacks 50% chance to Paralyse
//  if((HPS==5)&&(SPS==5))
	if(SpecBon[0]&HP_SP) t[i++]=ITxt(199,1,&NPCTexts)[ 2];//'R'; // HP+SP Regeneration 50 HP points per turn
//  if((DFS==5)&&(HPS==5))
	if(SpecBon[0]&DF_HP) t[i++]=ITxt(199,1,&NPCTexts)[ 3];//'E'; // DF+HP Endless Retaliation
//  if((DMS==5)&&(SPS==5))
	if(SpecBon[0]&DM_SP) t[i++]=ITxt(199,1,&NPCTexts)[ 4];//'C'; // DM+SP Champion Distance
//  if((DMS==5)&&(HPS==5))
	if(SpecBon[0]&HP_DM) t[i++]=ITxt(199,1,&NPCTexts)[ 5];//'2'; // DM+HP Attack twice
//  if((ATS==5)&&(DFS==5))
	if(SpecBon[0]&AT_DF) t[i++]=ITxt(199,1,&NPCTexts)[ 6];//'D'; // AT+DF Reduce Enemy Defense by 80%
//  if((ATS==5)&&(MPS==5))
	if(SpecBon[0]&AT_MP) t[i++]=ITxt(199,1,&NPCTexts)[ 7];//'N'; // AT+MP No Enemy Retaliation
//  if((ATS==5)&&(HPS==5))
	if(SpecBon[0]&AT_HP) t[i++]=ITxt(199,1,&NPCTexts)[ 8];//'O'; // AT+HP Fearsome
//  if((ATS==5)&&(SPS==5))
	if(SpecBon[0]&AT_SP) t[i++]=ITxt(199,1,&NPCTexts)[ 9];//'S'; // AT+SP Can Shoot
//  if((DFS==5)&&(DMS==5))
	if(SpecBon[0]&DF_DM) t[i++]=ITxt(199,1,&NPCTexts)[10];//'A'; // DF+DM Strike all Enemy around
//  if((DFS==5)&&(MPS==5))
	if(SpecBon[0]&DF_MP) t[i++]=ITxt(199,1,&NPCTexts)[11];//'I'; // DF+MP Permanent Fireshield
//  if((DFS==5)&&(SPS==5))
	if(SpecBon[0]&DF_SP) t[i++]=ITxt(199,1,&NPCTexts)[12];//'B'; // DF+SP Block
//  if((DMS==5)&&(MPS==5))
	if(SpecBon[0]&DM_MP) t[i++]=ITxt(199,1,&NPCTexts)[13];//'G'; // DM+MP #=NPCLevel/10
//  if((MPS==5)&&(SPS==5))
	if(SpecBon[0]&MP_SP) t[i++]=ITxt(199,1,&NPCTexts)[14];//'F'; // MP+SP Ignore Obstacles (fly)
	t[i]=0;
	Zsprintf3(&Buffer1,ITxt(197,1,&NPCTexts)/*"%s,MR:%i%%[%s]"*/,(Dword)Buffer.Str,(Dword)CalcSkill(6),(Dword)t);
	Zsprintf3(&Hint[ind],ITxt(198,1,&NPCTexts)/*"%s\nCasts %s (%i)"*/,(Dword)Buffer1.Str,(Dword)Magics[Type],(Dword)MPS+1);
	RETURN(Hint[ind].Str)
}

void CheckForAliveNPCAfterBattle(Byte * /*bm*/);

static int NPC_NewHeroExpo;
static _Hero_ *NPC_CurHero;
void _NPC_AddExpo(void)
{
	STARTNA(__LINE__, 0)
	if(NPC_CurHero==0) RETURNV
	NPC &npc=NPCs[NPC_CurHero->Number];
	CheckForAliveNPCAfterBattle(0);
	if(npc.Used<=0) RETURNV
	if(npc.Dead==1) RETURNV
	npc.AddExp(NPC_NewHeroExpo,0);
//  npc.HP=(40+npc.Level*20);
//  npc.DM=(12+npc.Level*4);
	if(npc.Fl.CustomPrimary==0){ // если пользователь не сам контролирует
		npc.HP=CALC_HP(npc.Level);
		npc.DM=CALC_DM(npc.Level);
	}
	RETURNV
}

int ExpoFromBattle=0;
void NPC_AddExpo(void)
{
	__asm{
		mov  NPC_NewHeroExpo,edi
		mov  NPC_CurHero,ebx
		pusha
	}
// may be called twice per battle  
	STARTNA(__LINE__, 0)
	if(ExpoFromBattle){
		CrExpoSet::AddExpo(NPC_CurHero,NPC_NewHeroExpo,NPCs[NPC_CurHero->Number].OldHeroExp);
//    ExpoFromBattle=0;
	}
	_NPC_AddExpo();
	// 3.58 избежать доп. опыта для стэков, если нет командира
	if(NPC_CurHero!=0){
		NPCs[NPC_CurHero->Number].OldHeroExp=NPC_NewHeroExpo;
	}
	STOP
	__asm{
		popa
		mov  ecx,1
	}
}
/*
static int NPC_CurHeroNum;
static char *NPC_Bio;
//static _ZPrintf_ NPC_Buf;

void _NPC_ShowInfo(void)
{
	NPC &npc=NPCs[NPC_CurHeroNum];
	if(npc.Used<=0){
//    Zsprintf2(&NPC_Buf,"%s\n\n%s",(Dword)NPC_Bio,(Dword)npc.ToText(0));
//    NPC_Bio=NPC_Buf.Str;
	}else{
//    NPC_Bio=npc.ToText(0);
//////////////////////
		npc.ToText(0);
		if(ShowNPC(&DlgNPC)==-1){
			npc.Used=0;
		}
		for(int i=0;i<6;i++){
			if(DlgNPC.ArtOutput[i]==1){ // передаем артифакт обратно герою
				int Type=
			}
		}
/////////////////////
	}
}
void NPC_ShowInfo(void)
{
	// ecx -> сообщение
	// edx = номер героя
	asm{
		mov    NPC_CurHeroNum,edx
		mov    NPC_Bio,ecx
		pusha
	}
//  _NPC_ShowInfo();
	asm{
		popa
		mov    ecx,NPC_Bio
		push   0
		push   -1
		neg    bl
		push   0
		push   -1
		sbb    ebx,ebx
		push   0
		push   -1
		and    ebx,3
		push   0
		push   -1
		inc    ebx
		push   -1
		push   -1
		mov    edx,ebx
		mov    eax,0x4F6C00
		call   eax
	}
}
*/
void ResetNPCExp(int HNum){
	STARTNA(__LINE__, 0)
	int i;
	if(HNum==-1){ // for all
		for(i=0;i<HERNUM;i++){
//      NPCs[i].Used=1;
			NPCs[i].SetOldHeroExp(GetHeroStr(i)->Exp);
		}
	}else{ // for HNum
//    NPCs[HNum].Used=1;
		NPCs[HNum].SetOldHeroExp(GetHeroStr(HNum)->Exp);
	}
	RETURNV
}

void EnableNPC(int HNum,int HasAtStart){
	STARTNA(__LINE__, 0)
	int i;
	if(HNum==-1){ // for all
		for(i=0;i<HERNUM;i++){ NPCs[i].Used=HasAtStart; NPCs[i].Dead=0; }
	}else{ // for HNum
		NPCs[HNum].Used=HasAtStart; NPCs[HNum].Dead=0;
	}
	RETURNV
}
void DisableNPC(int HNum){
	STARTNA(__LINE__, 0)
	int i;
	if(HNum==-1){ // for all
		for(i=0;i<HERNUM;i++){ NPCs[i].Used=-1; NPCs[i].Dead=0; }
	}else{ // for HNum
		NPCs[HNum].Used=-1; NPCs[HNum].Dead=0;
	}
	RETURNV
}
int IsNPCDead(int HNum){
	STARTNA(__LINE__, 0)
	if((HNum<0)||(HNum>=HERNUM)) RETURN(0)
	if(NPCs[HNum].Used!=1) RETURN(0)
	if(NPCs[HNum].Dead==0) RETURN(1)
	RETURN(0)
}
////////////////////////
/*
Byte *PlaceNPC(int Type, int Side,NPC *npc)
{
	int   pos,num,tp,tp2,tpn;
	Byte *mstr, *mstr2;
//Byte *SammonCreature(int type,int num,int pos,int side,int slot,int redraw)
	if(Side==0){ // left
		pos=88; mstr=SammonCreature(Type,1,pos,Side,-1,0);
		if(mstr==0){ pos=71; mstr=SammonCreature(Type,1,pos,Side,-1,0); }
		if(mstr==0){ pos=105; mstr=SammonCreature(Type,1,pos,Side,-1,0); }
	}else{
		pos=98; mstr=SammonCreature(Type,1,pos,Side,-1,0);
		if(mstr==0){ pos=81; mstr=SammonCreature(Type,1,pos,Side,-1,0); }
		if(mstr==0){ pos=115; mstr=SammonCreature(Type,1,pos,Side,-1,0); }
	}
	if(mstr!=0){
		if(npc->Type==1){
			num=npc->Level+1;
			if(Side==0){ // Left
				pos=17; mstr2=SammonCreature(147,num,pos,Side,-1,0);
				if(mstr2==0){ pos=0x33; mstr2=SammonCreature(147,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=0x77; SammonCreature(147,num,pos,Side,-1,0); }
			}else{
				pos=33; mstr2=SammonCreature(147,num,pos,Side,-1,0);
				if(mstr2==0){ pos=0x43; mstr2=SammonCreature(147,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=0x87; SammonCreature(147,num,pos,Side,-1,0); }
			}
		}
		if(npc->Type==6){
			num=(npc->Level+1)/4+1;
			if(Side==0){ // Left
				pos=18; mstr2=SammonCreature(146,num,pos,Side,-1,0);
				if(mstr2==0){ pos=19; mstr2=SammonCreature(146,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=154; mstr2=SammonCreature(146,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=155; SammonCreature(146,num,pos,Side,-1,0); }
			}else{
				pos=32; mstr2=SammonCreature(146,num,pos,Side,-1,0);
				if(mstr2==0){ pos=31; mstr2=SammonCreature(146,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=168; mstr2=SammonCreature(146,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=167; SammonCreature(146,num,pos,Side,-1,0); }
			}
		}
//    if((npc->MPS==5)&&(npc->SPS==5))
		if(npc->SpecBon[0]&MP_SP){ // MP+SP
			tp=((npc->Level+1)-15)/2; if(tp<0) tp=0; if(tp>13) tp=13;
			tpn=tp/2+1; // 1...7
			tp2=(tp%2)*7+tp/2;
			tp2=MonInTownBase(npc->Type)[tp2];
//      num=npc->CalcSkill(2)/MonTable[tp].HitPoints; // HP
			num=((npc->Level+1)*10)/(tpn*tpn+1);
			if(Side==0){ // Left
				pos=18; mstr2=SammonCreature(tp2,num,pos,Side,-1,0);
				if(mstr2==0){ pos=19; mstr2=SammonCreature(tp2,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=20; mstr2=SammonCreature(tp2,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=154; mstr2=SammonCreature(tp2,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=155; mstr2=SammonCreature(tp2,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=156; SammonCreature(tp2,num,pos,Side,-1,0); }
			}else{
				pos=32; mstr2=SammonCreature(tp2,num,pos,Side,-1,0);
				if(mstr2==0){ pos=31; mstr2=SammonCreature(tp2,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=30; mstr2=SammonCreature(tp2,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=168; mstr2=SammonCreature(tp2,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=167; mstr2=SammonCreature(tp2,num,pos,Side,-1,0); }
				if(mstr2==0){ pos=166; SammonCreature(tp2,num,pos,Side,-1,0); }
			}
		}
	}
	return mstr;
}
*/

Byte *GetMPos(Byte *Bm,int Side,int Pos)
{
	STARTNA(__LINE__, 0)
	__asm{
		mov    ebx,Side
		lea    eax,[8*ebx]
		sub    eax,ebx
		mov    ebx,Pos
		lea    ecx,[ebx+2*eax]
		add    eax,ecx
		lea    ecx,[8*eax]
		sub    ecx,eax
		lea    edx,[ecx+2*ecx]
		lea    eax,[eax+8*edx]
		mov    ebx,Bm
		lea    eax,[ebx+8*eax+0x54CC]
		mov    BPDummy,eax
	}
	RETURN(BPDummy)
}

static int PS_Type;
static int PS_Num;
static int PS_Pos;
static int PS_Side;
static _Hero_ *PS_hp;
static int PS_Placed;
static int PS_Stack;
static Byte *Mstr;
Byte *PutStack(Byte *Bm,int Type,int Num,int Pos,int Side,_Hero_ *hp,int Placed,int Stack)
{
	STARTNA(__LINE__, 0)
	PS_Type=Type; PS_Num=Num; PS_Pos=Pos; PS_Side=Side; PS_hp=hp; PS_Placed=Placed; PS_Stack=Stack;
	Mstr=GetMPos(Bm,Side,Placed);
	__asm{
		push   PS_Stack;
		push   PS_Pos;
		push   PS_Placed;
		push   PS_Side;
		push   PS_hp;
		push   PS_Num;
		push   PS_Type;
		mov    ecx,Mstr
		mov    eax,0x43D5D0
		call   eax
		mov    ecx,Mstr
		mov    eax,0x43D710
		call   eax
	}
	RETURN(Mstr)
}

Byte *PlaceNPC2(Byte *Bm,int Type, int Side,NPC *npc,_Hero_ *Hp,int *Placed)
{
	STARTNA(__LINE__, 0)
	int   i,pos,num,tp,/*tp2,tpn,*/placed;
	Byte *mstr,*mon;
	if(Side==0) pos=88; else pos=98;
	mstr=PutStack(Bm,Type,1,pos,Side,Hp,*Placed,-1); ++*Placed;
	if(npc->Type==1){ // палатка ставится в 0x46344A
		num=npc->Level+1;
//    if(Side==0) pos=17; else pos=33;
		placed=0;
		for(i=0;i<21;i++){
			mon=&Bm[0x54CC+0x548*(i+Side*21)]; tp=*(int *)&mon[0x34];
			if(tp==147){ *(int *)&mon[0x60]+=num; *(int *)&mon[0x4C]+=num; placed=1; break; }
		}
		if(placed==0){
			if(Side==0) pos=153; else pos=169; // 3.57m2
			PutStack(Bm,147,num,pos,Side,Hp,*Placed,-1); ++*Placed;
		}
	}
	if(npc->Type==6){ // балиста ставится в 0x4633E8
		num=(npc->Level+1)/4+1;
//    if(Side==0) pos=17; else pos=33;
		placed=0;
		for(i=0;i<21;i++){
			mon=&Bm[0x54CC+0x548*(i+Side*21)]; tp=*(int *)&mon[0x34];
			if(tp==146){ *(int *)&mon[0x60]+=num; *(int *)&mon[0x4C]+=num; placed=1; break; }
		}
		if(placed==0){
			if(Side==0) pos=51; else pos=67; // 3.57m2
			PutStack(Bm,146,num,pos,Side,Hp,*Placed,-1); ++*Placed;
		}
	}
/*
	if(npc->SpecBon[0]&MP_SP){ // MP+SP Summoning - disabled
		tp=((npc->Level+1)-15)/2; if(tp<0) tp=0; if(tp>13) tp=13;
		tpn=tp/2+1; // 1...7
		tp2=(tp%2)*7+tp/2;
		tp2=MonInTownBase(npc->Type)[tp2];
		num=((npc->Level+1)*10)/(tpn*tpn+1);
//    if(Side==0) pos=19; else pos=31;
		if(Side==0) pos=121; else pos=133; // 3.57m2
		PutStack(Bm,tp2,num,pos,Side,Hp,*Placed,-1); ++*Placed;
	}
*/
	RETURN(mstr)
}

// инициализация 1 Ком С героем 
static int   NPCinBattleInd[2];
static Byte *NPCinBattleStr[2];
static void SetMonInitPars(NPC *npc,_MonInfo_ *mi)
{
	STARTNA(__LINE__, 0)
	int Slot;
	mi->Attack=npc->CalcSkill(0);
	mi->Defence=npc->CalcSkill(1);
	mi->HitPoints=npc->CalcSkill(2);
	mi->DamageH=npc->CalcSkill(3);
//  if((npc->ATS==5)&&(npc->DMS==5))
	if(npc->SpecBon[0]&AT_DM) mi->DamageL=mi->DamageH;
	else mi->DamageL=CALC_MinDM(mi->DamageH);//mi->DamageH*3/4;
	mi->HasSpell=npc->MPS+1;//CalcSkill(4); // число колдований
	mi->Speed=npc->CalcSkill(5);
//  if((npc->ATS==5)&&(npc->SPS==5))
	if(npc->SpecBon[0]&AT_SP) mi->Flags|=0x00001004;
	else mi->Flags&=0xFFFFEFFB;
	Slot=npc->ArtInSlot(152);
	if(Slot!=-1){
		if(npc->Arts[Slot][ARTBATTLES]>=5) mi->Flags|=0x00001004;
	}
//  if((npc->DMS==5)&&(npc->SPS==5)) mi->Flags|=0x00000008;
//  else mi->Flags&=0xFFFFFFF7;
//  if((npc->ATS==5)&&(npc->MPS==5))
	if(npc->SpecBon[0]&AT_MP) mi->Flags|=0x00010000;
	else mi->Flags&=0xFFFEFFFF;
//  if((npc->DMS==5)&&(npc->HPS==5))
	if(npc->SpecBon[0]&HP_DM) mi->Flags|=0x00008000;
	else mi->Flags&=0xFFFF7FFF;
//  if((npc->DFS==5)&&(npc->DMS==5))
	if(npc->SpecBon[0]&DF_DM) mi->Flags|=0x00080000;
	else mi->Flags&=0xFFF7FFFF;
//  if((npc->DFS==5)&&(npc->SPS==5))
	if(npc->SpecBon[0]&MP_SP) mi->Flags|=0x00000002;
	else mi->Flags&=0xFFFFFFFD;
//  if((npc->DMS==5)&&(npc->SPS==5)) mi->Flags|=0x00000020;
//  else mi->Flags&=0xFFFFFFDF;
	if(npc->ArtDoesHave(153)) mi->Flags|=0x80000008;
	else mi->Flags&=0x7FFFFFF7;
	RETURNV
}
// инициализация 1 Ком С героем 
static void SetMonInitAfter(NPC *npc,Byte *mon)
{
	STARTNA(__LINE__, 0)
	int v;
	switch(npc->Type){
		case 0: *(int *)&mon[0x4E0]=37; break; // Cure
		case 1: *(int *)&mon[0x4E0]=27; break; // Shield
		case 2: *(int *)&mon[0x4E0]=44; break; // Precission
		case 3: *(int *)&mon[0x4E0]=29; break; // Fire Shield
		case 4: *(int *)&mon[0x4E0]=39; break; // Animate Dead
		case 5: *(int *)&mon[0x4E0]=43; break; // Bloodlust
		case 6: *(int *)&mon[0x4E0]=46; break; // Stone Skin
		case 7:
			v=*(int *)&mon[0xC8]; v+=(v-npc->CalcSkill(0))*50/100; *(int *)&mon[0xC8]=v;
			v=*(int *)&mon[0xCC]; v+=(v-npc->CalcSkill(1))*50/100; *(int *)&mon[0xCC]=v;
			*(int *)&mon[0x4E0]=53; break; // Haste
		case 8: *(int *)&mon[0x4E0]=58; break; // Counterstrike
	}
	RETURNV
}
// проверка можно ли бонус стэка существа применить к коммандиру как к существу
int IsNPCNotCreature(Byte *Mon)
{
	STARTNA(__LINE__, 0)
	int mt=*(int *)&Mon[0x34];
	if((mt<174)||(mt>191)) RETURN(0); // not NPC type
	int Ind=0; if(mt>182) Ind=1;
	if(Mon==NPCinBattleStr[Ind]) RETURN(1) // standard commander
	NPC *npc=&NPCsa[Ind];
	if(npc->Used==0) RETURN(0) // not set for this battle - creature
	RETURN(1)
}

// инициализация Ком без героя или доп.
void ApplyCmdMonChanges(Byte *Mon)
{
	STARTNA(__LINE__, 0)
//__asm int 3
	int v;
	int mt=*(int *)&Mon[0x34];
	if((mt<174)||(mt>191)) RETURNV; // wrong mon type
	int Ind=0; if(mt>182) Ind=1;
	NPC *npc=&NPCsa[Ind];
	if(npc->Used==0) RETURNV // not set for this battle - creature
	if(Mon==NPCinBattleStr[Ind]) RETURNV // standard commander
	// set up pars
	*(int *)&Mon[0xC8]=npc->CalcSkill(0); // attack
	*(int *)&Mon[0xCC]=npc->CalcSkill(1); // defence
	*(int *)&Mon[0xC0]=npc->CalcSkill(2); // HP
	*(int *)&Mon[0x6C]=npc->CalcSkill(2); // HP 0
	*(int *)&Mon[0xD4]=npc->CalcSkill(3); // Max Damage
	if(npc->SpecBon[0]&AT_DM) *(int *)&Mon[0xD0]=*(int *)&Mon[0xD4]; // Low Damage
	else *(int *)&Mon[0xD0]=CALC_MinDM(*(int *)&Mon[0xD4]);//mi->DamageH*3/4;
	*(int *)&Mon[0xDC]=npc->MPS+1;//CalcSkill(4); // число колдований
	*(int *)&Mon[0xC4]=npc->CalcSkill(5); // speed
	Dword fl=*(Dword *)&Mon[0x84];
	if(npc->SpecBon[0]&AT_SP) fl|=0x00001004;
	else fl&=0xFFFFEFFB;
	int Slot=npc->ArtInSlot(152);
	if(Slot!=-1){
		if(npc->Arts[Slot][ARTBATTLES]>=5) fl|=0x00001004;
	}
	if(npc->SpecBon[0]&AT_MP) fl|=0x00010000;
	else fl&=0xFFFEFFFF;
	if(npc->SpecBon[0]&HP_DM) fl|=0x00008000;
	else fl&=0xFFFF7FFF;
	if(npc->SpecBon[0]&DF_DM) fl|=0x00080000;
	else fl&=0xFFF7FFFF;
	if(npc->SpecBon[0]&MP_SP) fl|=0x00000002;
	else fl&=0xFFFFFFFD;
	if(npc->ArtDoesHave(153)) fl|=0x80000008;
	else fl&=0x7FFFFFF7;
	*(Dword *)&Mon[0x84]=fl;
	switch(npc->Type){
		case 0: *(int *)&Mon[0x4E0]=37; break; // Cure
		case 1: *(int *)&Mon[0x4E0]=27; break; // Shield
		case 2: *(int *)&Mon[0x4E0]=44; break; // Precission
		case 3: *(int *)&Mon[0x4E0]=29; break; // Fire Shield
//    case 4: *(int *)&Mon[0x4E0]=39; break; // Animate Dead
		case 4: *(int *)&Mon[0x4E0]=53; break; // Haste
		case 5: *(int *)&Mon[0x4E0]=43; break; // Bloodlust
		case 6: *(int *)&Mon[0x4E0]=46; break; // Stone Skin
		case 7:
			v=*(int *)&Mon[0xC8]; v+=(v-npc->CalcSkill(0))*50/100; *(int *)&Mon[0xC8]=v;
			v=*(int *)&Mon[0xCC]; v+=(v-npc->CalcSkill(1))*50/100; *(int *)&Mon[0xCC]=v;
			*(int *)&Mon[0x4E0]=53; break; // Haste
		case 8: *(int *)&Mon[0x4E0]=58; break; // Counterstrike
	}
	RETURNV
}

int GetNPCMagicPower(Byte *mon)
{
	STARTNA(__LINE__, 0)
	int mt=*(int *)&mon[0x34];
	int val,ni,side=0;
	if(mt>182) side=1;
	if(mon!=NPCinBattleStr[side]){ // additional commander
		val=NPCsa[side].CalcSkill(4);
	}else{ // standard commander
		ni=NPCinBattleInd[side];
		val=NPCs[ni].CalcSkill(4);
//    if((mt==178)||(mt==187)){ val/=2; if(val<1) val=1; } // 3.57 original
		if((mt==178)||(mt==187)){ val/=4; if(val<1) val=1; } // 3.58 reduction
	}
	RETURN(val)
}

static int LastHeroMana[2];
/*
void PlaceNPCAtBattleStart(Byte *bm)
{
	_Hero_    *hp1=(_Hero_ *)*(Dword *)&bm[0x53CC];
	_Hero_    *hp2=(_Hero_ *)*(Dword *)&bm[0x53D0];
	NPC       *npc;
	Byte      *mstr;
	_MonInfo_ *mi;
//  int       *val;
	NPCinBattleInd[0]=NPCinBattleInd[1]=-1;
	NPCinBattleStr[0]=NPCinBattleStr[1]=0;
	LastHeroMana[0]=LastHeroMana[1]=-1;
	if(hp1==0) goto No_NPC; // нет первого героя
	npc=&NPCs[hp1->Number];
	if(npc->Used<=0) goto No_NPC;
	if(npc->Dead==1) goto No_NPC;
	mi=&MonTable[npc->Type+174+9*0];
	SetMonInitPars(npc,mi); // настройка параметров
	mi->Spec=npc->ToHint(0);
	mstr=PlaceNPC(npc->Type+174+9*0,0,npc);
	if(mstr==0) goto No_NPC; // не удалось поставить
	SetMonInitAfter(npc,mstr); // настройка параметров
	///////////
	NPCinBattleInd[0]=hp1->Number;
	NPCinBattleStr[0]=mstr;
	LastHeroMana[0]=hp1->SpPoints;
No_NPC:;
	if(hp2==0) goto No_NPC2; // нет первого героя
	npc=&NPCs[hp2->Number];
	if(npc->Used<=0) goto No_NPC2;
	if(npc->Dead==1) goto No_NPC2;
	mi=&MonTable[npc->Type+174+9*1];
	SetMonInitPars(npc,mi); // настройка параметров
	mi->Spec=npc->ToHint(1);
	mstr=PlaceNPC(npc->Type+174+9*1,1,npc);
	if(mstr==0) goto No_NPC2; // не удалось поставить
	SetMonInitAfter(npc,mstr); // настройка параметров
	//////////
	NPCinBattleInd[1]=hp2->Number;
	NPCinBattleStr[1]=mstr;
	LastHeroMana[1]=hp2->SpPoints;
No_NPC2:;
}
*/
int PlaceNPCAtBattleStart2(Byte *bm,int Side,int Placed)
{
	STARTNA(__LINE__, 0)
	_Hero_    *hp=(_Hero_ *)*(Dword *)&bm[0x53CC+Side*4];
	NPC       *npc;
	Byte      *mstr;
	_MonInfo_ *mi;

	// stack experience
//  ExpoFromBattle=1;

	NPCinBattleInd[Side]=-1; //NPCsa[Side].Init();
	NPCinBattleStr[Side]=0;
	LastHeroMana[Side]=-1;
//  NPCsa[Side].Used=1; 
//  NPCsa[Side].Dead=0;
	if(hp==0) goto No_NPC; // нет героя
	npc=&NPCs[hp->Number];
	if(npc->Used<=0) goto No_NPC;
	if(npc->Dead==1) goto No_NPC;
	mi=&MonTable[npc->Type+174+9*Side];
	SetMonInitPars(npc,mi); // настройка параметров
	mi->Spec=npc->ToHint(Side);
	mstr=PlaceNPC2(bm,npc->Type+174+9*Side,Side,npc,hp,&Placed);
	if(mstr==0) goto No_NPC; // не удалось поставить
	SetMonInitAfter(npc,mstr); // настройка параметров
	///////////
	NPCinBattleInd[Side]=hp->Number;
	NPCinBattleStr[Side]=mstr;
	LastHeroMana[Side]=hp->SpPoints;
No_NPC:;
	RETURN(Placed)
}

char *FormMonList(char *Header,int *Mt,int *Mn)
{
	STARTNA(__LINE__, 0)
	Zsprintf2(&NPC::Buffer,"%s\n",(Dword)Header,(Dword)0);
	for(int i=0;i<7;i++){
		if(i&1){
			if((Mt[i]!=-1)&&(Mn[i]!=0)){
				Zsprintf3(&NPC::Buffer ,ITxt(200,1,&NPCTexts)/*"%s\n%i of the %s"*/,(Dword)NPC::Buffer1.Str,(Dword)Mn[i],(Dword)MonTable3[Mt[i]].NameP);
			}else{
				Zsprintf2(&NPC::Buffer ,"%s",(Dword)NPC::Buffer1.Str,(Dword)0);
			}
		}else{
			if((Mt[i]!=-1)&&(Mn[i]!=0)){
				Zsprintf3(&NPC::Buffer1,ITxt(200,1,&NPCTexts)/*"%s\n%i of the %s"*/,(Dword)NPC::Buffer.Str ,(Dword)Mn[i],(Dword)MonTable3[Mt[i]].NameP);
			}else{
				Zsprintf2(&NPC::Buffer1,"%s",(Dword)NPC::Buffer.Str,(Dword)0);
			}
		}
	}
	RETURN(NPC::Buffer1.Str)
}


////////////////////////

////////////////////////
static int LFiredSet[2];
static _MonArr_ LFiredMon[2];
//static _MonArr_ FiredMon[2];
static void StoreFired(int Cmd,int HInd,int *Type,int *Num)
{
	STARTNA(__LINE__, 0)
	switch(Cmd){
		case -1: // clear
			memset(LFiredMon,0,sizeof(LFiredMon));
			LFiredSet[0]=LFiredSet[1]=-1;
			break;
		case  0: // Attacker
		case  1: // Defender
			for(int i=0;i<7;i++){
				LFiredMon[Cmd].Ct[i]=Type[i];
				LFiredMon[Cmd].Cn[i]=Num[i];
			}
			LFiredSet[Cmd]=HInd;
			break;
	}
	RETURNV
}
static int RemoveCreatureDistant(int Ind,int *Type,int *Num)
{
	STARTNA(__LINE__, 0)
	int ret=0;
	if(LFiredSet[Ind]==-1) RETURN(0)
	for(int i=0;i<7;i++){
		if(LFiredMon[Ind].Ct[i]!=Type[i]){ MError("MP: Mon type is not equal (AstrSprt)"); RETURN(0) }
		Num[i]=LFiredMon[Ind].Cn[i];
		ret=1;
	}
	RETURN(ret)
}
static int AddCreatureBack(int Ind,int *HInd,int *Type,int *Num)
{
	STARTNA(__LINE__, 0)
	int ret=0;
	if(LFiredSet[Ind]==-1) RETURN(0)
	*HInd=LFiredSet[Ind];
	for(int i=0;i<7;i++){
//    if(LFiredMon[Ind].Ct[i]!=Type[i]){ MError("MP: Mon type is not equal (AstrSprt)"); return 0; }
		Type[i]=LFiredMon[Ind].Ct[i];
		Num[i]=LFiredMon[Ind].Cn[i];
		if(Num[i]==0) continue;
		ret=1;
	}
	RETURN(ret)
}

void CheckForAliveNPCAfterBattle(Byte * /*bm*/)
{
	STARTNA(__LINE__, 0)
	int i,HInd,Type[7],Num[7];
	for(i=0;i<2;i++){
		if(AddCreatureBack(i,&HInd,Type,Num)==0) continue;
		_Hero_ *hp=GetHeroStr(HInd);
		if(hp->Owner==-1) continue; // lost the battle
		if(i!=WinerIs()) continue; // not this side won
		if(IsThis(hp->Owner)!=0){
			Message(FormMonList(ITxt(257,1,&NPCTexts),Type,Num),1);
		}
		for(int j=0;j<7;j++){ hp->Cn[j]+=Num[j]; }
	}
	StoreFired(-1,0,0,0);  // clear the table
	if(NPCinBattleInd[0]==-1) goto No_NPC;
	NPCs[NPCinBattleInd[0]].LastExpoInBattle=0;
	if(NPCinBattleStr[0]==0) goto No_NPC; // и не было
	if(*(int *)&NPCinBattleStr[0][0x4C]!=0){ // остался жив
		NPCs[NPCinBattleInd[0]].LastExpoInBattle=1;
		for(i=0;i<10;i++) NPCs[NPCinBattleInd[0]].Arts[i][ARTBATTLES]++;
		goto No_NPC;
	}
	// был и мертв - запретим
	if(NPCs[NPCinBattleInd[0]].ArtDoesHave(149)==0){
		NPCs[NPCinBattleInd[0]].Dead=1;
		for(i=0;i<10;i++) NPCs[NPCinBattleInd[0]].Arts[i][ARTBATTLES]=0;
	}
No_NPC:;
	if(NPCinBattleInd[1]==-1) goto No_NPC2;
	NPCs[NPCinBattleInd[1]].LastExpoInBattle=0;
	if(NPCinBattleStr[1]==0) goto No_NPC2; // и не было
	if(*(int *)&NPCinBattleStr[1][0x4C]!=0){ // остался жив
		NPCs[NPCinBattleInd[1]].LastExpoInBattle=1;
		for(i=0;i<10;i++) NPCs[NPCinBattleInd[1]].Arts[i][ARTBATTLES]++;
		goto No_NPC2;
	}
	// был и мертв - запретим
	if(NPCs[NPCinBattleInd[1]].ArtDoesHave(149)==0){
		NPCs[NPCinBattleInd[1]].Dead=1;
		for(i=0;i<10;i++) NPCs[NPCinBattleInd[1]].Arts[i][ARTBATTLES]=0;
	}
No_NPC2:;
	NPCinBattleInd[0]=NPCinBattleInd[1]=-1;
	NPCinBattleStr[0]=NPCinBattleStr[1]=0;
	// restore all commanders for additional
	for(i=174;i<2*9;i++) MonTable[i].Flags|=0x00000006;
	RETURNV
}

void NPCAttack0(_MonArr_ *G2B_MArrA,_MonArr_ *G2B_MArrD,_Hero_ *G2B_HrA,_Hero_ *G2B_HrD)
{
	STARTNA(__LINE__, 0)
	int  i,yes,pers,Mt[7],MnS[7],MnD[7];
	NPC *npc;
//  if(IsThis(G2B_HrA->Owner)!=0) StoreFired(-1,0,0,0);
	NPCsa[0].Init();
	NPCsa[1].Init();
	do{
		if(G2B_HrA==0) break;
		npc=&NPCs[G2B_HrA->Number];
		if(npc->Type!=3) break; // Inferno
		if(npc->Used<=0) break;
		if(npc->Dead==1) break;
		if(G2B_HrD!=0) break; // не нейтральный стек
		pers=npc->Level/2+5; if(pers>20) pers=20;
		for(i=0;i<7;i++){ Mt[i]=G2B_MArrD->Ct[i]; }
		yes=RemoveCreature(G2B_MArrD->Ct,G2B_MArrD->Cn,MnS,MnD,pers); // pers домножен на 100
		if(yes==0) break;
		if(IsThis(G2B_HrA->Owner)!=0){
			Message(ITxt(9*3+6*6*4+1,1,&NPCTexts),1);
			CrChangeDialog(G2B_HrA,Mt[0],MnD[0],Mt[1],MnD[1],Mt[2],MnD[2],
								Mt[3],MnD[3],Mt[4],MnD[4],Mt[5],MnD[5],Mt[6],MnD[6]);
		}else{
			AddCreature(G2B_MArrA->Ct,G2B_MArrA->Cn,Mt,MnD);
		}
		for(i=0;i<7;i++){ G2B_MArrD->Cn[i]=MnS[i]; if(MnS[i]<=0) G2B_MArrD->Ct[i]=-1; }
	}while(0);
	do{
		if(G2B_HrD==0) break;
		npc=&NPCs[G2B_HrD->Number];
		if(npc->Type!=3) break; // Inferno
		if(npc->Used<=0) break;
		if(npc->Dead==1) break;
		if(G2B_HrA!=0) break; // не нейтральный стек
		pers=npc->Level/2+5; if(pers>20) pers=20;
		for(i=0;i<7;i++){ Mt[i]=G2B_MArrA->Ct[i]; }
		yes=RemoveCreature(G2B_MArrA->Ct,G2B_MArrA->Cn,MnS,MnD,pers); // pers домножен на 100
		if(yes==0) break;
		if(IsThis(G2B_HrD->Owner)!=0){
			Message(ITxt(9*3+6*6*4+1,1,&NPCTexts),1);
			CrChangeDialog(G2B_HrD,Mt[0],MnD[0],Mt[1],MnD[1],Mt[2],MnD[2],
								Mt[3],MnD[3],Mt[4],MnD[4],Mt[5],MnD[5],Mt[6],MnD[6]);
		}else{
			AddCreature(G2B_MArrD->Ct,G2B_MArrD->Cn,Mt,MnD);
		}
		for(i=0;i<7;i++){ G2B_MArrA->Cn[i]=MnS[i]; if(MnS[i]<=0) G2B_MArrA->Ct[i]=-1; }
	}while(0);
	RETURNV
}

void NPCAttack(_MonArr_ *G2B_MArrA,_MonArr_ *G2B_MArrD,_Hero_ *G2B_HrA,_Hero_ *G2B_HrD)
{
	STARTNA(__LINE__, 0)
	int  i,yes,pers,Mt[7],MnS[7],MnD[7];
	NPC *npc;
	if(G2B_HrA!=0 && IsThis(G2B_HrA->Owner)!=0) StoreFired(-1,0,0,0);
	do{ // Astral Spirit Attacker
		if(G2B_HrA==0) break;
		npc=&NPCs[G2B_HrA->Number];
		if(npc->Type!=8) break;
		if(npc->Used<=0) break;
		if(npc->Dead==1) break;
		if(G2B_HrD==0) break; // нет героя в армии
		pers=npc->Level/2+5; if(pers>20) pers=20;
		for(i=0;i<7;i++){ Mt[i]=G2B_MArrD->Ct[i]; }
		// 3.58 fired monsters transfered
		if(IsThis(G2B_HrA->Owner)!=0){
			yes=RemoveCreature(G2B_MArrD->Ct,G2B_MArrD->Cn,MnS,MnD,pers); // pers домножен на 100
		}else{
			yes=RemoveCreatureDistant(1,G2B_MArrD->Ct,MnD);
		}
		if(yes==0) break; // no one
		if(IsThis(G2B_HrA->Owner)!=0){ StoreFired(1,G2B_HrD->Number,Mt,MnD);  }
		if(IsThis(G2B_HrA->Owner)!=0){
//      Message(ITxt(9*3+6*6*4+2,1,&NPCTexts),1);
//      CrChangeDialog(G2B_HrD,Mt[0],MnD[0],Mt[1],MnD[1],Mt[2],MnD[2],
//                Mt[3],MnD[3],Mt[4],MnD[4],Mt[5],MnD[5],Mt[6],MnD[6]);
			Message(FormMonList(ITxt(9*3+6*6*4+2,1,&NPCTexts),Mt,MnD),1);
		}
		else if(IsThis(G2B_HrD->Owner)!=0){
			Message(FormMonList(ITxt(9*3+6*6*4+3,1,&NPCTexts),Mt,MnD),1);
		}
		if((IsThis(G2B_HrA->Owner)!=0)&&(IsAI(G2B_HrA->Owner)==0)){ // 3.57 MP support
			// Атакер здесь и не АИ
			for(i=0;i<7;i++){ G2B_MArrD->Cn[i]=MnS[i]; if(MnS[i]<=0) G2B_MArrD->Ct[i]=-1; }
		}else if((IsThis(G2B_HrD->Owner)!=0)&&(IsAI(G2B_HrA->Owner)!=0)){ // 3.57 MP support
			// Деф. здесь и Атакер АИ
			for(i=0;i<7;i++){ G2B_MArrD->Cn[i]=MnS[i]; if(MnS[i]<=0) G2B_MArrD->Ct[i]=-1; }
		}
	}while(0);
	do{ // Astral Spirit Defender
		if(G2B_HrD==0) break;
		npc=&NPCs[G2B_HrD->Number];
		if(npc->Type!=8) break;
		if(npc->Used<=0) break;
		if(npc->Dead==1) break;
		if(G2B_HrA==0) break; // нет героя в армии
		pers=npc->Level/2+5; if(pers>20) pers=20;
		for(i=0;i<7;i++){ Mt[i]=G2B_MArrA->Ct[i]; }
		// 3.58 fired monsters transfered
		if(IsThis(G2B_HrA->Owner)!=0){
			yes=RemoveCreature(G2B_MArrA->Ct,G2B_MArrA->Cn,MnS,MnD,pers); // pers домножен на 100
		}else{
			yes=RemoveCreatureDistant(0,G2B_MArrA->Ct,MnD);
		}
		if(yes==0) break;
		if(IsThis(G2B_HrA->Owner)!=0){ StoreFired(0,G2B_HrA->Number,Mt,MnD); }
		if(IsThis(G2B_HrD->Owner)!=0){
			Message(FormMonList(ITxt(9*3+6*6*4+2,1,&NPCTexts),Mt,MnD),1);
		}
		else if(IsThis(G2B_HrA->Owner)!=0){
			Message(FormMonList(ITxt(9*3+6*6*4+3,1,&NPCTexts),Mt,MnD),1);
		}
		if((IsThis(G2B_HrA->Owner)!=0)&&(IsAI(G2B_HrA->Owner)==0)){ // 3.57 MP support
			// Атакер здесь и не АИ
			for(i=0;i<7;i++){ G2B_MArrA->Cn[i]=MnS[i]; if(MnS[i]<=0) G2B_MArrA->Ct[i]=-1; }
		}else if((IsThis(G2B_HrD->Owner)!=0)&&(IsAI(G2B_HrA->Owner)!=0)){ // 3.57 MP support
			// Деф. здесь и Атакер АИ
			for(i=0;i<7;i++){ G2B_MArrA->Cn[i]=MnS[i]; if(MnS[i]<=0) G2B_MArrA->Ct[i]=-1; }
		}
	}while(0);
	RETURNV
}

////////////////////////
//static int DNPC_ResType[4]={1,3,4,5};
void DaylyNPC(int /*owner*/)
{
/*
	int     i,j;
	_Hero_ *hp;
	NPC    *npc;
	for(i=0;i<HERNUM;i++){
		hp=GetHeroStr(i);
		if(hp->Owner!=owner) continue; // не наш
		npc=&NPCs[i];
		if(npc->Used==0) continue; // нет NPC
		if(npc->Dead==1) continue; // нет NPC
		if(npc->Type==1){ // Rampart - вызов шарпшутеров
			if((GetCurDate()%3)!=0) continue; // каждый 3-й день
			for(j=0;j<7;j++){
				if(hp->Ct[j]==19) goto _Found;
			}
			for(j=0;j<7;j++){
				if(hp->Ct[j]==-1){ hp->Cn[j]=0; goto _Found; }
			}
			continue;
_Found:
			hp->Ct[j]=19; hp->Cn[j]+=1+npc->Level/5;
		}
		if(npc->Type==2){ // Tower - ресурсы по кругу
			if(owner==-1) goto _Ok;
			int res=DNPC_ResType[GetCurDate()%4];
			int val=GetRes(owner,res);
			val+=1;
			SetRes(owner,res,val);
		}
_Ok:;
	}
*/
}
int NPC_Resist(int Type,int Damage,Byte *Mon)
{
	STARTNA(__LINE__, 0)
	int val,side=0; if(Type>=183) side=1;
//  int val=NPCs[NPCinBattleInd[side]].Mr;
	if(Mon!=NPCinBattleStr[side]){ // additional commander
		val=NPCsa[side].CalcSkill(6);
	}else{ // standard commander
		val=NPCs[NPCinBattleInd[side]].CalcSkill(6);
	}
/*
	switch(NPCs[NPCinBattleInd[side]].Skills[4]){ // MP
		case 0: val=5; break;
		case 1: val=10; break;
		case 2: val=20; break;
		case 3: val=40; break;
		case 4: val=65; break;
		case 5: val=95; break;
	}
*/
	RETURN(Damage*(100-val)/100)
}
////////////////////////
void AddMagic2NPC(Byte *mon,Byte *BatMan)
{
//HP+MP Permanent Fireshield
	STARTNA(__LINE__, 0)
	_Hero_ *hp;
	int tp=*(int *)&mon[0x34];
	int side=0,bside=1; if(tp>=183){ side=1; bside=0; }
	NPC *npc;
	if(mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
	}
//  if((npc->DFS==5)&&(npc->MPS==5))
	if(npc->SpecBon[0]&DF_MP){
		SetMonMagic(mon,0x1D,1000,3,0); // огненный щит
	}
//  if((npc->DFS==5)&&(npc->HPS==5))
	if(npc->SpecBon[0]&DF_HP){
		*(int *)&mon[0x454]=1000;
	}
	do{
		if((tp!=176)&&(tp!=185)) break;
//   +13D68 db = 1-фаза тактики, 0-фаза битвы
		if(BatMan[0x13D68]!=0) break;
		if(*(int *)&mon[0x4C]!=1) break;
		if(NPCinBattleInd[side]==-1) break;
		if(mon!=NPCinBattleStr[side]) break; // additional commander - no bonus
//    _Hero_ *hp=GetHeroStr(NPCinBattleInd[side]);
// fix for 3.57f
		if(side) hp=G2B_HrD; else hp=G2B_HrA;
		int cmana=hp->SpPoints;
		if(LastHeroMana[side]<=cmana){ // маны стало больше или не изменилась
			LastHeroMana[side]=cmana;
			break;
		}
		int mmana=hp->PSkill[3]*10;
		switch(hp->SSkill[24]){ // Intelligence
			case 3: mmana= mmana*2; break;
			case 2: mmana=(mmana*150)/100; break;
			case 1: mmana=(mmana*125)/100; break;
		}
		if(cmana<mmana){
//      int dmana=(npc->Level+1);
			int dmana=(LastHeroMana[side]-cmana)*(20+(npc->Level+1)*5)/100;
			if(dmana>(LastHeroMana[side]-cmana)*90/100){
				dmana=(LastHeroMana[side]-cmana)*90/100;
			}
			if(dmana<1) dmana=1;
			cmana+=dmana;
			if(cmana>mmana) cmana=mmana;
			hp->SpPoints=(Word)cmana;
			ManaAdd2Hero(bside);
			LastHeroMana[side]=cmana;
		}
	}while(0);
	RETURNV
}
// вернуть -1, если надо регенерировать
int CanNPCRegenerate(int tp,Byte *Mon)
{
	STARTNA(__LINE__, 0)
	int ret=0;
	if((tp<174)||(tp>191)) RETURN(0)
	int side=0; if(tp>=183) side=1;
	NPC *npc;
	if(Mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
	}
//  if((npc->HPS==5)&&(npc->SPS==5))
	if(npc->SpecBon[0]&HP_SP){
		/*if(Random(1,100)>50)*/ ret=-1;
	}
	RETURN(ret)
}
// вернуть <20 если нужно парализовать
static Byte *DidParalize;
int NPC_Paralize(void)
{
	STARTNA(__LINE__, 0)
	Byte *mon;
	_ESI(mon);
	int tp=*(int *)&mon[0x34];
	int side=0; if(tp>=183) side=1;
	NPC *npc;
	if(mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
	}
//  if((npc->HPS==5)&&(npc->MPS==5))
	if(npc->SpecBon[0]&HP_MP){
		if(Random(1,100)>50){
			DidParalize=mon;
			RETURN(1)
		}
	}
	RETURN(100)
}
// возвр не 0, если да
int HasNPCFear(Byte *Mon)
{
	STARTNA(__LINE__, 0)
//  Byte *mn=(Byte *)_ECX;
//  int tp=*(int *)&mn[-0x18];
//  int tp=*(int *)&mn[-0x18];
	int tp=*(int *)&Mon[0x34];
	int side=0; if(tp>=183) side=1;
	NPC *npc;
	if(Mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
	}
//  if((npc->ATS==5)&&(npc->HPS==5))
	if(npc->SpecBon[0]&AT_HP){
		RETURN(1)
	}
	RETURN(0)
}
int NPCReduceDefence(Byte *mon,int Defence)
{
/*RD_Ret*20/100*/
	STARTNA(__LINE__, 0)
	int tp=*(int *)&mon[0x34];
	int side=0; if(tp>=183) side=1;
	NPC *npc;
	if(mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
	}
//  if((npc->ATS==5)&&(npc->DFS==5))
	if(npc->SpecBon[0]&AT_DF){
		RETURN(Defence*50/100)
	}
	RETURN(Defence)
}
int NPCDeathStare(Byte *mon,Byte *Dmon)
{
	STARTNA(__LINE__, 0)
	int tp=*(int *)&mon[0x34];
	if((tp<174)||(tp>191)) RETURN(-1)
	int side=0; if(tp>=183) side=1;
	NPC *npc;
	if(mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
	}
//  if((npc->DMS==5)&&(npc->MPS==5))
	if(npc->SpecBon[0]&DM_MP){
		if(DidParalize==mon){
			DidParalize=0;
			RETURN(-1) // уже парализовал
		}
		DidParalize=0;
		tp=*(int *)&Dmon[0x34];
		_MonInfo_ *mi=&MonTable[tp];
		tp=mi->SubGroup+1; if(tp<1) tp=1;
		/*if(Random(1,100)>50)*/ RETURN((npc->Level+1)/tp)
	}
	RETURN(0)
}
int NPCChampion(Byte *mon)
{
	STARTNA(__LINE__, 0)
	int tp=*(int *)&mon[0x34];
	if((tp<174)||(tp>191)) RETURN(tp)
	int side=0; if(tp>=183) side=1;
	NPC *npc;
	if(mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
	}
	if(*(Dword *)&mon[0x84]&0x4) RETURN(tp) // стреляет
//  if((npc->DMS==5)&&(npc->SPS==5))
	if(npc->SpecBon[0]&DM_SP){
		RETURN(11)
	}
	RETURN(tp)
}
int NPCFirstAid(Byte *mon)
{
	return *(int *)&mon[0x4C];
//  return 5;
}
char NPCBalistaControl(_Hero_ *hp)
{
	STARTNA(__LINE__, 0)
//  if((hp->Spec/2)==6){
	NPC *npc=&NPCs[hp->Number];
	if(npc->Type==6){
		if((npc->Used>0)&&(npc->Dead==0)) RETURN(1)
	}
	__asm    mov   eax,hp
	__asm    mov   al,[eax+0xDD]
	__asm    mov   CDummy,al
	RETURN(CDummy)
}
/*
Castle:     0 Get 150% experience
Rampart:    1 Cooks First Aid Tent stack #=NPCLevel
Tower:      2 Restore ??? mana points for Hero every battle turn
Inferno:    3 Gives 25% of exp in a battle in gold
Necro:      4 is Undead
Dungeon:    5 Steals 5% of neutral stacks before combat (uncompleted)
Stronghold: 6 Cooks Ballistas Stack #=NPCLevel/4+1 (plus control)
Fortress:   7 150% of Hero Attack and Defense
Conflux:    8 is Elemental (no mind, no moral) + Creature run away (uncompleted)

 AT,DF,DM,HP,MP,SP
+AT+DM Maximum damage always  [---Behemoth---]
+HP+MP Permanent Fireshield   [---Efretti---]
+HP+SP Regeneration 50 HP points [---Trolls---]
+DF+HP Endless Retaliation    [---Griffin---]
+DM+SP Champion Distance Bonus   [---Champions---]
+DM+HP Fearsome               [---Nightmares---]
+AT+DF Reduce Enemy Defense by 50%
+AT+MP No Enemy Retaliation   [---Cerberi---]
+AT+HP Attack twice           [---Wolves---]
+AT+SP Can Shoot  [---Ballista---]
+DF+DM Strike all Enemy around     [---Hydras---]
+DF+MP Melee Attacks 50% chance to Paralyse  [---Medusas---]
+DF+SP Ignore Obstacles (fly)     [---AirElemental---]
+DM+MP DeathStare #=NPCLevel/5
+MP+SP Summon Stack
!!! Gorgons Attack или Удар Грома
??? Breath Attack
	- can shoot without distance penalty
	- can shoot without obstacle penalty
	- can view troops in enemy towns and armies
	- +50% movement
	- +100% most powerful creature in all player's towns
	- 10% enemy creatures before the battle
*/
void NPC2Castle(int /*GM_ai*/,_Hero_ *Hr,_CastleSetup_ *Cstr)
{
	STARTNA(__LINE__, 0)
	int This,AI,gold,goldor,answer;
	if(Hr==0) RETURNV
	int num=Hr->Number;
	int owner=Hr->Owner; if(owner==-1) RETURNV
	if(owner!=Cstr->Owner) RETURNV // если не наш город
	CloseCastleGate(Hr,Cstr);
	NPC *npc=&NPCs[num];
	if(npc->Used<0) RETURNV // запрещен
	This=IsThis(owner);
	AI=IsAI(owner);
	if(npc->Used==0){ // нет или уволен
		gold=1000;
		goldor=GetRes(owner,6);
		if(goldor<gold){
			if(This&& !AI) RequestPic(ITxt(9*3+6*6*4+5,1,&NPCTexts),6,gold,1);
			RETURNV
		}
		if(This&& !AI) answer=RequestPic(ITxt(9*3+6*6*4+6,1,&NPCTexts),6,gold,2);
		else answer=1;
		if(answer){
			goldor-=gold;
			SetRes(owner,6,goldor);
			npc->Init();
			ResetNPCExp(num);
			EnableNPC(num,1);
			COCall(2,Hr);
		}
	}else{ // есть
		if(npc->Dead==1){ // мертв
			if((npc->Level>=30)&&(Cstr->MagLevel<3)){ if(!AI) RequestPic(ITxt(185,1,&NPCTexts),22+Cstr->Type,2,1); RETURNV }
			if((npc->Level>=20)&&(Cstr->MagLevel<2)){ if(!AI) RequestPic(ITxt(184,1,&NPCTexts),22+Cstr->Type,1,1); RETURNV }
			if((npc->Level>=10)&&(Cstr->MagLevel<1)){ if(!AI) RequestPic(ITxt(183,1,&NPCTexts),22+Cstr->Type,0,1); RETURNV }
//      gold=(npc->Level+1)*100;
			gold=(npc->Level*npc->Level+npc->Level%2)*50;
			goldor=GetRes(owner,6);
			if(goldor<gold){
				if(This&& !AI) RequestPic(ITxt(9*3+6*6*4+7,1,&NPCTexts),6,gold,1);
				RETURNV
			}
			if(gold<1){ // 3.58 не нужно золота - не надо иконки
				if(This && !AI) answer=RequestPic(ITxt(9*3+6*6*4+8,1,&NPCTexts),21,npc->Type+174,2);
				else answer=1;
			}else{
				if(This && !AI) answer=Request2Pic(ITxt(9*3+6*6*4+8,1,&NPCTexts),21,npc->Type+174,6,gold,2);
				else answer=1;
			}
			if(answer){
				goldor-=gold;
				SetRes(owner,6,goldor);
				ResetNPCExp(num);
				EnableNPC(num,1);
				COCall(3,Hr);
			}
		}
	}
	RETURNV
}
////////////////////////
// Может ли командер использовать артифакт в принципе?
int NPC::ArtIsPossible(int ArtNum){
	if(ArtNum<NPCARTBASE || ArtNum>=NPCARTBASE+10) return 0;
	return 1;
}
int NPC::ArtMayHave(int /*ArtNum*/){
	return 1;
}
int NPC::ArtGetFreeSlot(int /*ArtNum*/){
	for(int i=0;i<6;i++) if(Arts[i][ARTNUMINDEX]<=0) return i;
	return -1;
}
int NPC::ArtDoesHave(int ArtNum){
	for(int i=0;i<6;i++) if(Arts[i][ARTNUMINDEX]==ArtNum) return 1;
	return 0;
}
int NPC::ArtInSlot(int ArtNum){
	for(int i=0;i<6;i++) if(Arts[i][ARTNUMINDEX]==ArtNum) return i;
	return -1;
}

int ProcessArt(NPC *npc,int ArtNum)
{
	STARTNA(__LINE__, 0)
	int Slot;
	if(npc->ArtIsPossible(ArtNum)==0){
		Message(ITxt(210,1,&NPCTexts),1);
		RETURN(0)
	}
	if(npc->ArtMayHave(ArtNum)==0){
		Message(ITxt(211,1,&NPCTexts),1);
		RETURN(0)
	}
	if(npc->ArtDoesHave(ArtNum)){
		Message(ITxt(243,1,&NPCTexts),1);
		RETURN(0)
	}
	Slot=npc->ArtGetFreeSlot(ArtNum);
	if(Slot==-1){
		Message(ITxt(212,1,&NPCTexts),1);
		RETURN(0)
	}
	npc->Arts[Slot][ARTNUMINDEX]=(short)ArtNum;
	npc->Arts[Slot][ARTBATTLES]=0;
//  for(i=0;i<7;i++){
//    npc->Arts[Slot][i]=npc->ArtBonusVal[ArtNum-NPCARTBASE][i];
//  }
	//*****************
	RETURN(1)
}
int ProcessArtAI(NPC *npc,int ArtNum)
{
	STARTNA(__LINE__, 0)
	int Slot;
	if(npc->ArtIsPossible(ArtNum)==0) RETURN(0)
	if(npc->ArtMayHave(ArtNum)==0) RETURN(0)
	if(npc->ArtDoesHave(ArtNum)) RETURN(0)
	Slot=npc->ArtGetFreeSlot(ArtNum);
	if(Slot==-1) RETURN(0)
	npc->Arts[Slot][ARTNUMINDEX]=(short)ArtNum;
	npc->Arts[Slot][ARTBATTLES]=0;
	RETURN(1)
}
void CommanderArtsAI(_Hero_ *hp)
{
	STARTNA(__LINE__, 0)
	NPC *npc=&NPCs[hp->Number];
	if(hp->OANum==0) RETURNV
	for(int i=0;i<64;i++){
		if(hp->OArt[i][0]==-1) continue;
		if(ProcessArtAI(npc,hp->OArt[i][0])){
			hp->OArt[i][0]=-1;
			hp->OArt[i][1]=-1;
			hp->OANum=(Byte)(hp->OANum-1);
		}
	}
	RETURNV
}
int NPCNoShootPenalty(int Type,int YesRet,Byte *mon)
{
	STARTNA(__LINE__, 0)
	int Slot,Yes=0,No=1; if(YesRet){ Yes=1; No=0; }
	if((Type<174)||(Type>191)) RETURN(No)
	int side=0; if(Type>=183) side=1;
	NPC *npc;
	if(mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
	}
	if(npc->ArtDoesHave(152)){
		Slot=npc->ArtInSlot(152); if(Slot==-1) RETURN(No)
		if(npc->SpecBon[0]&AT_SP){ // может стрелять
			if(Yes==0){ // нет дистанции (стены)
				if(npc->Arts[Slot][ARTBATTLES]>=25) RETURN(Yes)
			}else{ // нет препятствий
				if(npc->Arts[Slot][ARTBATTLES]>=0) RETURN(Yes)
			}
		}else{ // не может стрелять
			if(Yes==0){ // нет дистанции (стены)
				if(npc->Arts[Slot][ARTBATTLES]>=50) RETURN(Yes)
			}else{ // нет препятствий
				if(npc->Arts[Slot][ARTBATTLES]>=25) RETURN(Yes)
			}
		}
	}
	RETURN(No)
}
// Blocking ability
int CommanderBlock(Byte *Mon,int Type,int /*Dam*/)
{
	STARTNA(__LINE__, 0)
	if((Type<174)||(Type>191)) RETURN(0)
	int side=0; if(Type>=183) side=1;
	NPC *npc;
	if(Mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
	}
	if(npc->SpecBon[0]&DF_SP){ // блок
		if(Random(1,100)<=30){
//      int fl=0;
//      if((*((Dword *)&Mon[0x84]))&0x08000000) fl=1;
			// флаг защиты поставим
			*((Dword *)&Mon[0x84])|=0x08000000;
			__asm{ // анимация блока
				mov    ecx,0x699420
				mov    ecx,[ecx] // bat man
				mov    eax,Mon
				mov    edx,0x54
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
//      if(fl==0) *((Dword *)&Mon[0x84])&=0xF7FFFFFF;
			char *po=ITxt(250,1,&NPCTexts);
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
	}
	RETURN(0)
}

////////////////////////
// возвр 1, если кидали артифакт и он взят
int RealClickNPC(_MouseStr_ *ms,_Hero_ *hp,int DropArt)
{
	STARTNA(__LINE__, 0)
	int moved=0;
	NPC *npc=&NPCs[hp->Number];
	switch(ms->SType){
		case 0x0C: // левый нажали
			switch(npc->Used){
				case -1: // запрещен
					Message(ITxt(182,1,&NPCTexts),1);
					break;
				case 0: // разрешен, но нет
					Message(ITxt(181,1,&NPCTexts),1);
					break;
				case 1: // есть
					if(DropArt!=-1 && DropArt!=0xFF){
						// проверяем можно ли добавить артифакт и, если да, то добавляем
						if(ProcessArt(npc,DropArt)==0) RETURN(0) // не смогли добавить
					}else{
//asm int 3
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
					if(moved) RedrawHeroScreen(hp);
					if(DropArt!=-1 && DropArt!=0xFF) RETURN(1)
					break;
			}
			break;
		case 0x0D: // левый отпустили
			break;
		case 0x0E: // правый нажали
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
			break;
	}
	RETURN(0)
}

int ShowBFNPCInfo(int Type,Byte *mon)
{
	STARTNA(__LINE__, 0)
	if((Type<174)||(Type>191)) RETURN(0)
	int side=0; if(Type>=183) side=1;
	int flag;
	NPC *npc;
	if(mon!=NPCinBattleStr[side]){ // additional commander
		npc=&NPCsa[side];
		if(npc->Used==0) RETURN(0) // not set for this battle - creature
		flag=1;
	}else{ // standard commander
		npc=&NPCs[NPCinBattleInd[side]];
		flag=0;
	}
	npc->ToText(0,1,flag);
	ShowNPC(&DlgNPC);
	RETURN(1)
}

//static int CNPC_Ret;
static _MouseStr_ *CNPC_ms;
static _Hero_  *CNPC_hp;
static int CNPC_DropArt;
void _ClickNPC(void)
{
	STARTNA(__LINE__, 0)
	__asm mov   eax,0x698B70
	__asm mov   eax,[eax]
	__asm mov   CNPC_hp,eax
	if(MouseClickHero(CNPC_ms,CNPC_hp)){
		CNPC_ms->Item=0x6E; // клик в поле без функций - выйдем
		RETURNV
	}
	if(CNPC_ms->Type!=0x200) RETURNV
	if(CNPC_ms->SType == 12 || CNPC_ms->SType == 14){
		BlessesDescr(CNPC_ms, CNPC_hp);
	}
	if(CNPC_ms->Item>=0x44 && CNPC_ms->Item<=0x4A){ // ckick to a stack
		// кидали ли артифакт
		__asm    mov    eax,0x698AD8
		__asm    mov    eax,[eax]
		__asm    mov    CNPC_DropArt,eax
		if(CNPC_DropArt!=-1 && CNPC_DropArt!=0xFF){ // есть артифакт
			if(CrExpoSet::DropArt2Stack(CNPC_ms,CNPC_hp,CNPC_DropArt)){
//        CNPC_ms->Item=0x6E;
				__asm{
					mov    eax,0x698AD8
					mov    dword ptr [eax],-1
					mov    dword ptr [eax+4],-1
					mov    ecx,0x6992B0
					mov    ecx,[ecx]
					push   0
					push   0
					mov    eax,0x50CEA0
					call   eax
				}
			}
			SetActiveStack(-1);
			CNPC_ms->Item=0x6E;
		}
	}
	if(CNPC_ms->Item!=150) RETURNV
// кидали ли артифакт на NPC?
	__asm    mov    eax,0x698AD8
	__asm    mov    eax,[eax]
	__asm    mov    CNPC_DropArt,eax
	if(RealClickNPC(CNPC_ms,CNPC_hp,CNPC_DropArt)){
//  if(CNPC_DropArt!=-1){
		__asm{
			mov    eax,0x698AD8
			mov    dword ptr [eax],-1
			mov    dword ptr [eax+4],-1
			mov    ecx,0x6992B0
			mov    ecx,[ecx]
			push   0
			push   0
			mov    eax,0x50CEA0
			call   eax
		}
	}
/*
	Copy((Byte *)MC_msp,(Byte *)&MC_MouseStr,sizeof(_MouseStr_));
	MC_changed=0;
	if(MapSmPos(&MC_MouseStr,&MC_MixPos)==1){
		if(MC_MouseStr.Item==0){ MC_MouseStr.Item=37; MC_changed=1; }
	}else{
		if(MC_MouseStr.Item==37){ MC_MouseStr.Item=0; MC_changed=1; }
	}
	MixedPos(&MC_x,&MC_y,&MC_l,MC_MixPos);
	MC_Std=1;
	pointer=30310; //
	ProcessERM();
	if(MC_changed==1){
		if(MC_MouseStr.Item==0) MC_MouseStr.Item=37;
		else{ if(MC_MouseStr.Item==37) MC_MouseStr.Item=0; }
	}
	Copy((Byte *)&MC_MouseStr,(Byte *)MC_msp,sizeof(_MouseStr_));
*/
	RETURNV
}
void ClickNPC(void)
{
	__asm pusha
	__asm mov    eax,-1
	__asm mov    [edx+0x3C],eax
//  asm mov    CNPC_Ret,eax
	__asm mov    CNPC_ms,esi
	_ClickNPC();
	__asm popa
//  asm mov    eax,CNPC_Ret
//  return _EAX;
}
////////////////////////
/*
struct {
	int HeroNumber;
	struct{
		Dword   HasCatap   : 1;
		Dword   HasBalista : 1;
		Dword   HasFAT     : 1;
		Dword   HasAmmo    : 1;
		Dword   HasSpBook  : 1;
	} Fl;
	NPC HeroNPC;
	_MonArr_ FiredMon[2];
} SendData;
*/
////////////////////////
struct ClassSendData{
	int HeroNumber;
	struct{
		unsigned   HasCatap   : 1;
		unsigned   HasBalista : 1;
		unsigned   HasFAT     : 1;
		unsigned   HasAmmo    : 1;
		unsigned   HasSpBook  : 1;
	} Fl;
	NPC HeroNPC;
	int FiredSet[2];
	_MonArr_ FiredMon[2];
} SendData;
////////////////////////

int GetSizeOfSendData(void)
{
	return sizeof(SendData);
}
/*
void SendNPC(Byte *Buf)
{
	int i,first,last;
	_Hero_ *hp;
	for(i=0;i<sizeof(SendData);i++) ((char *)&SendData)[i]=0;
	int hn=GetCurHero();
	SendData.HeroNumber=hn;
	if(hn==-1){ // такого быть не должно вообще-то
	}else{
		hp=GetHeroStr(SendData.HeroNumber);
		if(hp->IArt[13][0]==4) SendData.Fl.HasCatap=1;
		if(hp->IArt[17][0]==0) SendData.Fl.HasBalista=1;
		if(hp->IArt[15][0]==6) SendData.Fl.HasFAT=1;
		if(hp->IArt[14][0]==5) SendData.Fl.HasAmmo=1;
		if(hp->IArt[16][0]==3) SendData.Fl.HasSpBook=1;
		SendData.HeroNPC=NPCs[hn];
	}
	first=*(int *)&Buf[0x0C];
	for(i=0;i<sizeof(SendData);i++) Buf[first+i]=((char *)&SendData)[i];
	last=first+sizeof(SendData);
	*((int *)&Buf[last])=first; last+=4;
	Buf[last]='Z'; Buf[last+1]='V'; Buf[last+2]='S'; last+=3;
	*(int *)&Buf[0x0C]=last;
}
*/
void SendNPC(int *len,Byte **buf,int HeroNum)
{
	STARTNA(__LINE__, 0)
	int i;
	_Hero_ *hp;
//  for(i=0;i<sizeof(SendData);i++) ((char *)&SendData)[i]=0;
	memset(&SendData,0,sizeof(SendData));
	int hn;
	if(HeroNum==-1)hn=GetCurHero(); else hn=HeroNum;
	SendData.HeroNumber=hn;
	if(hn==-1){ // такого быть не должно вообще-то
	}else{
		hp=GetHeroStr(SendData.HeroNumber);
		if(hp->IArt[13][0]==4) SendData.Fl.HasCatap=1;
		if(hp->IArt[17][0]==0) SendData.Fl.HasBalista=1;
		if(hp->IArt[15][0]==6) SendData.Fl.HasFAT=1;
		if(hp->IArt[14][0]==5) SendData.Fl.HasAmmo=1;
		if(hp->IArt[16][0]==3) SendData.Fl.HasSpBook=1;
		SendData.HeroNPC=NPCs[hn];
	}
	// 3.58
	for(i=0;i<2;i++){
		SendData.FiredSet[i]=LFiredSet[i];
		memcpy(&SendData.FiredMon[i],&LFiredMon[i],sizeof(_MonArr_));
	}
	*len=sizeof(SendData);
	*buf=(Byte *)&SendData;
	RETURNV
}
/*
void ReceiveNPC(Byte *Buf)
{
	int i,first,last;
	_Hero_ *hp;
	last=*(int *)&Buf[0x0C];
	if((Buf[last-3]!='Z')||(Buf[last-2]!='V')||(Buf[last-1]!='S')){ Message("WoG: Data was not sent",1); return; }// не передавали
	first=*(int *)&Buf[last-7];
	for(i=0;i<sizeof(SendData);i++) ((char *)&SendData)[i]=0;
	if((last-first-7)!=sizeof(SendData)){ Message("WoG: Wrong Transferred data size",1); return; }
	for(i=0;i<sizeof(SendData);i++) ((Byte *)&SendData)[i]=Buf[first+i];

	if(SendData.HeroNumber==-1){ // не должно быть
	}else{
		hp=GetHeroStr(SendData.HeroNumber);
		hp->IArt[13][0]=hp->IArt[17][0]=hp->IArt[15][0]=hp->IArt[14][0]=hp->IArt[16][0]=-1;
		if(SendData.Fl.HasCatap) hp->IArt[13][0]=4;
		if(SendData.Fl.HasBalista) hp->IArt[17][0]=0;
		if(SendData.Fl.HasFAT) hp->IArt[15][0]=6;
		if(SendData.Fl.HasAmmo) hp->IArt[14][0]=5;
		if(SendData.Fl.HasSpBook) hp->IArt[16][0]=3;
		NPCs[SendData.HeroNumber]=SendData.HeroNPC;
	}
}
*/
void ReceiveNPC(int len,Byte *Buf)
{
	STARTNA(__LINE__, 0)
	int i/*,MnD[7]*/;
	_Hero_ *hp;
	for(i=0;i<sizeof(SendData);i++) ((char *)&SendData)[i]=0;
	if(len!=sizeof(SendData)){ TError("Wrong length of sent NPC Data"); RETURNV };
	for(i=0;i<sizeof(SendData);i++) ((Byte *)&SendData)[i]=Buf[i];
	if(SendData.HeroNumber==-1){ // не должно быть
	}else{
		hp=GetHeroStr(SendData.HeroNumber);
		hp->IArt[13][0]=hp->IArt[17][0]=hp->IArt[15][0]=hp->IArt[14][0]=hp->IArt[16][0]=-1;
		if(SendData.Fl.HasCatap) hp->IArt[13][0]=4;
		if(SendData.Fl.HasBalista) hp->IArt[17][0]=0;
		if(SendData.Fl.HasFAT) hp->IArt[15][0]=6;
		if(SendData.Fl.HasAmmo) hp->IArt[14][0]=5;
		if(SendData.Fl.HasSpBook) hp->IArt[16][0]=3;
		NPCs[SendData.HeroNumber]=SendData.HeroNPC;
	}
/*
	for(i=0;i<2;i++){
		if(SendData.FiredSet[i]==0) continue;
		_Hero_  *hp=(_Hero_ *)*(Dword *)&bm[0x53CC+i*4];
		if(hp==0) continue;
		yes=RemoveCreatureDistant(0,hp->Ct,MnD);
		if(IsThis(hp->Owner)!=0){
			Message(FormMonList(ITxt(9*3+6*6*4+2,1,&NPCTexts),Mt,MnD),1);
		}
		else if(IsThis(G2B_HrA->Owner)!=0){
			Message(FormMonList(ITxt(9*3+6*6*4+3,1,&NPCTexts),Mt,MnD),1);
		}

	}
*/
//asm int 3
//  int a=SendData.FiredSet[0];
	for(i=0;i<2;i++){
		LFiredSet[i]=SendData.FiredSet[i];
		memcpy(&LFiredMon[i],&SendData.FiredMon[i],sizeof(_MonArr_));
	}
	RETURNV
}
////////////////////////

int ERM_NPC(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
//  STARTC("Commander: Process CO command",&Mp->m.s[Mp->i])
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  i,ind,v,v2,v3,sind;
	Dword andmask,ormask;
	NPC *npc;
	ind=GetVarVal(&sp->Par[0]);
	if((ind<-4)||(ind>=HERNUM)){ MError("\"!!CO\"-Commander index is out of range."); RETURN(0) }
	if(ind==-1){ // current
		if(ERM_HeroStr==0){ MError("\"!!CO\"-Cannot get the current Commander (no current hero)"); RETURN(0) }
		ind=ERM_HeroStr->Number;
		if((ind<0)||(ind>=HERNUM)){ MError("\"!!CO\"-Commander index is out of range."); RETURN(0) }
	}
	if(ind>=0) npc=&NPCs[ind]; else npc=&NPCs[0]; // на всякий случай
	if(ind==-3) npc=&NPCsa[0]; // additional npcs attacker
	if(ind==-4) npc=&NPCsa[1]; // additional npcs defender
	switch(Cmd){
		case 'E': // E - разрешить/запретить
			if(ind==-2){ // для всех
				if(Apply(&v,4,Mp,0)){ MError("\"!!CO:E\"-cannot be got or checked for all Commanders."); RETURN(0) }
				for(i=0;i<HERNUM;i++) NPCs[i].Used=v;
				break;
			}
			v=npc->Used;
			if(Apply(&v,4,Mp,0)) break;
			npc->Used=v;
			break;
		case 'D': // D - живой/мертвый
			if(ind==-2){ // для всех
				if(Apply(&v,4,Mp,0)){ MError("\"!!CO:D\"-cannot be got or checked for all Commanders."); RETURN(0) }
				for(i=0;i<HERNUM;i++) NPCs[i].Dead=v;
				break;
			}
			v=npc->Dead; if(Apply(&v,4,Mp,0)) break; npc->Dead=v;
			break;
//    case 'C': // участвует без героя
//      if(ind==-2){ MError("\"!!CO:С\"-cannot be applied to all Commanders."); RETURN(0) }
//      v=npc->Fl.NoHeroOwner; if(Apply(&v,4,Mp,0)) break; npc->Fl.NoHeroOwner=v;
//      break;
		case 'T': // T - тип Командера
			if(ind==-2){ MError("\"!!CO:T\"-cannot be applied to all Commanders."); RETURN(0) }
			v=npc->Type; if(Apply(&v,4,Mp,0)) break; 
			if(v<0) v=0; if(v>8) v=8;
			npc->Type=v;
			break;
		case 'H': // H - тип Хозяина Командера
			if(ind==-2){ MError("\"!!CO:H\"-cannot be applied to all Commanders."); RETURN(0) }
			v=npc->HType; if(Apply(&v,4,Mp,0)) break; 
			if(v<0) v=0; if(v>17) v=17;
			npc->HType=v;
			break;
		case 'P': // P - навыки
//  int  Primary[7]/*At,Df,Hp,Dm,Mp,Sp,Mr*/;
			if(Num==1){
				if(ind==-2){
					if(Apply(&v,4,Mp,0)){ MError("\"!!CO:P\"-cannot be got or checked for all Commanders."); RETURN(0) }
					for(i=0;i<HERNUM;i++) NPCs[i].Fl.CustomPrimary=v;
					break;
				}
				if(npc->Fl.CustomPrimary) v=1; else v=0;
				if(Apply(&v,4,Mp,0)) break;
				if(v) npc->Fl.CustomPrimary=1; else npc->Fl.CustomPrimary=0;
				break;
			}
			CHECK_ParamsMin(2);
			if(Apply(&v,4,Mp,0)){ MError("\"!!CO:P\"-cannot get or check the type of skill."); RETURN(0) }
			if((v<0)||(v>6)){ MError("\"!!CO:P\"-index of skill out of range (0...6)."); RETURN(0) }
			if(ind==-2){ // для всех
				if(Apply(&v2,4,Mp,1)){ MError("\"!!CO:P\"-cannot be got or checked for all Commanders."); RETURN(0) }
				for(i=0;i<HERNUM;i++) NPCs[i].Primary[v]=v2;
				break;
			}
			v2=npc->Primary[v]; if(Apply(&v2,4,Mp,1)) break; npc->Primary[v]=v2;
			break;
		case 'S': // S - скилы
//  int  Skills[7];
			CHECK_ParamsMin(2);
			if(Apply(&v,4,Mp,0)){ MError("\"!!CO:S\"-cannot get or check the type of skill level."); RETURN(0) }
			if((v<0)||(v>6)){ MError("\"!!CO:S\"-index of skill level out of range (0...6)."); RETURN(0) }
			if(ind==-2){ // для всех
				if(Apply(&v2,4,Mp,1)){ MError("\"!!CO:S\"-cannot be got or checked for all Commanders."); RETURN(0) }
				for(i=0;i<HERNUM;i++) NPCs[i].Skills[v]=v2;
				break;
			}
			v2=npc->Skills[v]; if(Apply(&v2,4,Mp,1)) break; npc->Skills[v]=v2;
			break;
		case 'A': // A - артифакты   //  short Arts[10][8];
			if(ind==-2){ MError("\"!!CO:A\"-cannot be applied to all Commanders."); RETURN(0) }
			if(Apply(&v,4,Mp,0)){ MError("\"!!CO:A\"-cannot get or check the command type."); RETURN(0) }
			ERMVar2[0]=0;
			switch(v){
				case 1: // add art type A1/#arttype/#battlewon -> v[0]
					if(Num<3){ MError("\"!!CO:A1\"-wrong syntax"); RETURN(0) }
					if(Apply(&v2,4,Mp,1)){ MError("\"!!CO:A1\"-cannot get or check art type."); RETURN(0) }
					if(Apply(&v3,4,Mp,2)){ MError("\"!!CO:A1\"-cannot get or check number of battles."); RETURN(0) }
					int Slot;
					if(npc->ArtIsPossible(v2)==0){ ERMVar2[0]=1; break; }
					if(npc->ArtMayHave(v2)==0){ ERMVar2[0]=2; break; }
					if(npc->ArtDoesHave(v2)){ ERMVar2[0]=3; break; }
					Slot=npc->ArtGetFreeSlot(v2);
					if(Slot==-1){ ERMVar2[0]=4; break; }
					npc->Arts[Slot][ARTNUMINDEX]=(short)v2;
					npc->Arts[Slot][ARTBATTLES]=v3;
					break;
				case 2: // take art type A2/#arttype -> v[0]
					if(Num<2){ MError("\"!!CO:A2\"-wrong syntax"); RETURN(0) }
					if(Apply(&v2,4,Mp,1)){ MError("\"!!CO:A2\"-cannot get or check art type."); RETURN(0) }
					if((v3=npc->ArtInSlot(v2))==-1){ ERMVar2[0]=1; break; }
					for(i=0;i<8;i++) npc->Arts[v3][i]=0;
					break;
				case 3: // art at pos A3/#pos/$arttype/$battlewon
					if(Num<4){ MError("\"!!CO:A3\"-wrong syntax"); RETURN(0) }
					if(Apply(&v,4,Mp,1)){ MError("\"!!CO:A3\"-cannot get or check art slot."); RETURN(0) }
					if((v<0) || (v>5)){ MError("\"!!CO:A3\"-incorrect art slot (0...5)."); RETURN(0) }
					v2=npc->Arts[v][ARTNUMINDEX];
					v3=npc->Arts[v][ARTBATTLES];
					if(Apply(&v2,4,Mp,2)==0) npc->Arts[v][ARTNUMINDEX]=v2;
					if(Apply(&v3,4,Mp,3)==0) npc->Arts[v][ARTBATTLES]=v3;
					break;
				case 4: // set/get all A4/$a1/$b1/$a2/$b2/$a3/$b3/$a4/$b4/$a5/$b5/$a6/$b6
					if(Num<13){ MError("\"!!CO:A4\"-wrong syntax"); RETURN(0) }
					for(i=0;i<6;i++){
						v2=npc->Arts[i][ARTNUMINDEX];
						v3=npc->Arts[i][ARTBATTLES];
						if(Apply(&v2,4,Mp,1+i*2)==0) npc->Arts[i][ARTNUMINDEX]=v2;
						if(Apply(&v3,4,Mp,2+i*2)==0) npc->Arts[i][ARTBATTLES]=v3;
					}
					break;
			}
		case 'N': // N - имя
//  char Name[32];
			if(ind==-2){ MError("\"!!CO:S\"-cannot be got or checked for all Commanders."); RETURN(0) }
			if(Mp->VarI[0].Check==1){ // ?
				sind=GetVarVal(&Mp->VarI[0]);
				if(BAD_INDEX_LZ(sind)||(sind>1000)){ MError("\"CO:N\"-wrong z var index (-10...-1,1...1000)."); RETURN(0) }
				if(sind>0){ StrCopy(ERMString[sind-1],31,npc->Name); ERMString[sind-1][31]=0; }
				else      { StrCopy(ERMLString[-sind-1],31,npc->Name); ERMLString[-sind-1][31]=0; }
			}else{
				sind=Mp->n[0];
				if(BAD_INDEX_LZ(sind)||(sind>1000)){ MError("\"CO:N\"-wrong z var index (-10...-1,1...1000)."); RETURN(0) }
				if(sind>0) StrCopy(npc->Name,31,ERMString[sind-1]);
				else       StrCopy(npc->Name,31,ERMLString[-sind-1]);
			}
			break;
		case 'X': // X - опыт
//  int  OldHeroExp;
//  int  Exp;   // тек. опыт
//  int  Level; // тек. уровень
			CHECK_ParamsMin(2);
			if(Apply(&v,4,Mp,0)){ MError("\"!!CO:X\"-cannot get or check the type of exp"); RETURN(0) }
			if((v<0)||(v>2)){ MError("\"!!CO:X\"-index of exp type out of range (0...2)"); RETURN(0) }
			if(ind==-2){ // для всех
				if(Apply(&v2,4,Mp,1)){ MError("\"!!CO:X\"-cannot be got or checked for all Commanders."); RETURN(0) }
				switch(v){
					case 0: for(i=0;i<HERNUM;i++) NPCs[i].OldHeroExp=v2; break;
					case 1: for(i=0;i<HERNUM;i++) NPCs[i].Exp=v2;        break;
					case 2: for(i=0;i<HERNUM;i++) NPCs[i].Level=v2-1;      break;
				}
				break;
			}
			switch(v){
				case 0: v2=npc->OldHeroExp; if(Apply(&v2,4,Mp,1)) break; npc->OldHeroExp=v2; break;
				case 1: v2=npc->Exp; if(Apply(&v2,4,Mp,1)) break; npc->Exp=v2; break;
				case 2: v2=npc->Level+1; if(Apply(&v2,4,Mp,1)) break; npc->Level=v2-1; break;
			}
			break;
		case 'B': // B - спец способности
//  Dword  SpecBon[2];
			CHECK_ParamsMin(2);
			if(Apply(&v,4,Mp,0)){ MError("\"!!CO:B\"-cannot get or check the type of spec bonus"); RETURN(0) }
			if((v<0)||(v>3)){ MError("\"!!CO:B\"-index of spec bonus type out of range (0...3)"); RETURN(0) }
			if(ind==-2){ // для всех
				if(Apply(&v2,4,Mp,1)){ MError("\"!!CO:B\"-cannot be got or checked for all Commanders."); RETURN(0) }
				switch(v){
					case 0: // set/get all
						ormask=*((Dword *)&v2);
						for(i=0;i<HERNUM;i++) NPCs[i].SpecBon[0]=ormask; break;
					case 1: // set/get one
						if(Num<3){ MError("\"!!CO:P\"-wrong syntsx"); RETURN(0) }
						if((v2<0)||(v2>14)){ MError("\"!!CO:B\"-index of spec bonus out of range (0...14)"); RETURN(0) }
						if(Apply(&v3,4,Mp,2)){ MError("\"!!CO:B\"-cannot be got or checked for all Commanders."); RETURN(0) }
						andmask=~(((Dword)1)<<v2);
						if(v3!=0) ormask=(((Dword)1)<<v2);
						for(i=0;i<HERNUM;i++){
							NPCs[i].SpecBon[0]&=andmask;
							NPCs[i].SpecBon[0]|=ormask;
						}
						break;
					case 2: // set/get en all
						ormask=*((Dword *)&v2);
						for(i=0;i<HERNUM;i++) NPCs[i].SpecBon[1]=ormask; break;
					case 3: // set/get en one
						CHECK_ParamsMin(3);
						if((v2<0)||(v2>14)){ MError("\"!!CO:B\"-index of spec bonus out of range (0...14)"); RETURN(0) }
						if(Apply(&v3,4,Mp,2)){ MError("\"!!CO:B\"-cannot be got or checked for all Commanders."); RETURN(0) }
						andmask=~(((Dword)1)<<v2);
						if(v3!=0) ormask=(((Dword)1)<<v2);
						for(i=0;i<HERNUM;i++){
							NPCs[i].SpecBon[1]&=andmask;
							NPCs[i].SpecBon[1]|=ormask;
						}
						break;
				}
				break;
			}
			switch(v){
				case 0: // set/get all
					ormask=npc->SpecBon[0]; if(Apply(&ormask,4,Mp,1)) break; npc->SpecBon[0]=ormask; break;
				case 1: // set/get one
					CHECK_ParamsMin(3);
					if(Apply(&v2,4,Mp,1)){ MError("\"!!CO:B\"-cannot get or check spec bonus index."); RETURN(0) }
					if((v2<0)||(v2>14)){ MError("\"!!CO:B\"-index of spec bonus out of range (0...14)"); RETURN(0) }
					ormask=(((Dword)1)<<v2);
					if(npc->SpecBon[0]&ormask) v3=1; else v3=0;
					if(Apply(&v3,4,Mp,2)) break;
					andmask=~(((Dword)1)<<v2);
					ormask=0; if(v3!=0) ormask=(((Dword)1)<<v2);
					npc->SpecBon[0]&=andmask; npc->SpecBon[0]|=ormask;
					break;
				case 2: // set/get en all
					ormask=npc->SpecBon[1]; if(Apply(&ormask,4,Mp,1)) break; npc->SpecBon[1]=ormask; break;
				case 3: // set/get en one
					CHECK_ParamsMin(3);
					if(Apply(&v2,4,Mp,1)){ MError("\"!!CO:B\"-cannot get or check spec bonus index."); RETURN(0) }
					if((v2<0)||(v2>14)){ MError("\"!!CO:B\"-index of spec bonus out of range (0...14)"); RETURN(0) }
					ormask=(((Dword)1)<<v2);
					if(npc->SpecBon[1]&ormask) v3=1; else v3=0;
					if(Apply(&v3,4,Mp,2)) break;
					andmask=~(((Dword)1)<<v2);
					ormask=0; if(v3!=0) ormask=(((Dword)1)<<v2);
					npc->SpecBon[1]&=andmask; npc->SpecBon[0]|=ormask;
					break;
			}
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}
// 3.58 переход героя с NPC на др. карту
void CrossOverNPC(int ind)
{
	STARTNA(__LINE__, 0)
	if((ind<0)||(ind>=HERNUM)) RETURNV
	Copy((Byte *)&NPCsBack[ind],(Byte *)&NPCs[ind],sizeof(NPC));
	RETURNV
}

int HeroHasCommander(_Hero_ *hp){
	STARTNA(__LINE__, 0)
	if(NPCs[hp->Number].Used==1) RETURN (1)
	RETURN(0)
}

int HeroHasAliveCommander(_Hero_ *hp){
	STARTNA(__LINE__, 0)
	if(HeroHasCommander(hp)==0) RETURN(0)
	if(NPCs[hp->Number].Dead==1) RETURN (0)
	RETURN(1)
}


static _Sphinx1 QDMess={
 0,0,0,0,
 0,
 0,0,0,0,
 0,0,0,0,
 0,0,0,0,
 0,0,0,0,
 0
};

void AdjastCommanderStat(_Hero_ *hp,int bonus,int GM_ai)
{
	STARTNA(__LINE__, 0)
	NPC *npc=&NPCs[hp->Number];
	QDMess.Pic1Hint=ITxt(255,1,&NPCTexts);
	QDMess.Pic2Hint=ITxt(256,1,&NPCTexts);
	QDMess.Pic1Path=".\\Data\\ZVS\\LIB1.RES\\NPC#.GIF";
	QDMess.Pic1Path[23]=(char)(npc->Type+'0');
	switch(bonus){
		case 0: // MP
			QDMess.Text1=ITxt(251,1,&NPCTexts);
			QDMess.Pic2Path=".\\Data\\ZVS\\LIB1.RES\\_MP0.BMP";
			npc->MP+=1; break;
		case 1: // AT
			QDMess.Text1=ITxt(252,1,&NPCTexts);
			QDMess.Pic2Path=".\\Data\\ZVS\\LIB1.RES\\_AT0.BMP";
			npc->AT+=2; break;
		case 2: // DF
			QDMess.Text1=ITxt(253,1,&NPCTexts);
			QDMess.Pic2Path=".\\Data\\ZVS\\LIB1.RES\\_DF0.BMP";
			npc->DF+=2; break;
		case 3: // SP
			QDMess.Text1=ITxt(254,1,&NPCTexts);
			QDMess.Pic2Path=".\\Data\\ZVS\\LIB1.RES\\_SP0.BMP";
			npc->SP+=1; break;
		default: RETURNV
	}
	if(GM_ai){
		QuickDialog(&QDMess);
	}
	RETURNV
}

////////////////////////
int SaveNPC(void)
{
	STARTNA(__LINE__, 0)
	if(Saver("NPCS",4)) RETURN(1)
	if(Saver(NPCs,sizeof(NPCs))) RETURN(1)
	RETURN(0)
}

// 3.58
void BackupNPC(void){
	STARTNA(__LINE__, 0)
	Copy((Byte *)NPCs,(Byte *)NPCsBack,sizeof(NPCs));
	BackupGodBonus();
	RETURNV
}

void ResetNPC(void){
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<HERNUM;i++){
		NPCs[i].Number=i;
		NPCs[i].Init();
		NPCs[i].AddExp(GetHeroStr(i)->Exp,0);
	}
	if(/*(WoG==0)||*/(PL_NoNPC!=0)){
		DisableNPC(-1);
		for(i=7;i<11;i++){ EnableChest[i]=0; }
	}else{
		EnableNPC(-1,!PL_NPC2Hire);
		for(i=7;i<11;i++){ EnableChest[i]=1; }
	}
	RETURNV
}

int LoadNPC(int /*ver*/)
{
	STARTNA(__LINE__, 0)
//  ResetNPC();
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='N'||buf[1]!='P'||buf[2]!='C'||buf[3]!='S')
		{MError("LoadNPC cannot start loading"); RETURN(1)}
	if(Loader(NPCs,sizeof(NPCs))) RETURN(1)
	BackupNPC(); // 3.58
	RETURN(0)
}
//////////////////
int LoadNPCParam(void)
{
	STARTNA(__LINE__, 0)
	int i,j;
	if(LoadTXT("ZNPC00.TXT",&NPCTexts)) RETURN(1) // не может загрузить TXT
	if(LoadTXT("ZNPC01.TXT",&NPCBios)) RETURN(1) // не может загрузить TXT
	for(i=0;i<9;i++) NPC::Magics[i]=ITxt(i+1,1,&NPCTexts);
	for(i=0;i<9;i++){
		NPC::SpecHint[i][0]=ITxt(i+9+1,1,&NPCTexts);
		NPC::SpecHint[i][1]=ITxt(i+9+1,2,&NPCTexts);
	}
	for(i=0;i<9;i++){
		NPC::SpecText[i][0]=ITxt(i+9*2+1,1,&NPCTexts);
		NPC::SpecText[i][1]=ITxt(i+9*2+1,2,&NPCTexts);
	}
	for(i=0;i<6;i++){
		for(j=0;j<6;j++){
			NPC::SpecBonusText[i][j][0]=ITxt(i*6+j+9*3+1,1,&NPCTexts);
			NPC::SpecBonusText[i][j][1]=ITxt(i*6+j+9*3+1,2,&NPCTexts);
		}
	}
	for(i=0;i<6;i++){
		for(j=0;j<6;j++){
			NPC::SpecBonusPopUpText[i][j]=ITxt(i*6+j+9*3+6*6+1,1,&NPCTexts);
		}
	}
	for(i=0;i<6;i++){
		for(j=0;j<6;j++){
			NPC::BonusText[i][j]=ITxt(i*6+j+9*3+6*6*2+1,1,&NPCTexts);
		}
	}
	for(i=0;i<6;i++){
		for(j=0;j<6;j++){
			NPC::BonusPopUpText[i][j]=ITxt(i*6+j+9*3+6*6*3+1,1,&NPCTexts);
		}
	}
//9*3+6*6*4+3 - последний занятый
	for(i=0;i<9;i++){
		NPC::Descr[i]=ITxt(186+i,1,&NPCTexts);
	}
	RETURN(0)
}
/*
__declspec( naked ) int NonHeroNPCAnimDeadCast(void)
{
	__asm{
		or    ecx,ecx
		jne  _HasHero
		xor   eax,eax
		ret   8
	}
_HasHero:
	__asm{
		push  [esp+8] 
		push  [esp+8]
		mov   eax,0x04E52F0
		call  eax
		ret   8
	}
}
*/