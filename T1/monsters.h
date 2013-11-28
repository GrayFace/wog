extern Byte MoJumper1[MONNUM-13]; //Diakon
extern int MoJmpAdr1[]; //Diakon
extern Byte MagHint[MONNUM-13];//Diakon
extern int MagHintJmp[];//Diakon
extern Byte MoJumper2[MONNUM-13];//Diakon
extern int MagJmp[]; //Diakon
extern Byte MoJumper3[];
extern Byte MoJumper4[];
extern Dword MoJumper4po[];
extern Byte MoJumper5[];
extern Byte MoJumper6[];
extern Byte AIMagicJumper[MONNUM-13];
int AIJmpAdr []; //Diakon
extern int  G2B_CompleteAI; // полностью AI битва (1) или нет (0)
extern _Hero_ *G2B_HrD,*G2B_HrA;
extern int BADistFlag;
extern int MonInfoDlgPopUpSetting;

int __fastcall MagicResist(int dmg, int spell, int Type);
int Random4DeathStare(void);
void DeathStarePost(void);
int ParalizeChance(void);
int IsChampion(void);
int __stdcall CastHeroOrCreature(int p3,int p2,int p1);
float FirstAidTentHealths(void);
char BalistaControl(void);
void PlaceNPC2(void);
int __stdcall Send2Defender(int p2,int p3);
int __stdcall Receive4Attacker(int p1);
int __stdcall Send4Defender(int p2,int p3);
int __stdcall Receive4DefenderAfter(int p2,int p1);
int __stdcall AIattackNPConly(int p2,int p1);
int __stdcall Receive4DefenderAfter2(int p2,int p1);
int NPCOnlyInCastle(void);
int NPCOnlyInCastle2(void);
void FixHeroSecScillsShow(void);
int __stdcall ShowExpo(int x,int y,int dx,int dy,char *Buf,char *Font,int Color,int par4,int par3,int BColor,int par1);
void __stdcall ShowNumberMid(void);
Dword ShowHint(Dword p1,Dword p2,Dword p3);
void __stdcall ShowHint1(Dword,Dword,Dword,char *Text);
// 3.58
//int MagicDwarfResistance(void);
void __stdcall E2MWoMoNoBattle(int,int,int,int,int,int,int,int,int,int);
int __stdcall ApplyDamage(int);
void M2MCombine(void);
char __stdcall M2MHero2Town(_MonArr_ *mas);
void __stdcall M2MHero2Town2(_MonArr_ *mas); //3.58f
int __stdcall ShowExpoPrep(_MonArr_ *MArr,int Stack,_Hero_ *Hp,Dword p7,Dword p6,Dword p5,Dword p4,Dword p3,Dword p2,Dword p1);
void __stdcall SmoothPassInternalFun(/*_MonArr_ *MonArrS,int,*/int SrcStack,_MonArr_ *MonArrD,int DstStack,int P1,int P2);
void MoveStackInHeroArmy(void);
void __stdcall MoveStackInHeroArmy2(/*_MonArr_ *MonArrS,int,*/int StackD,_MonArr_ *MonArrD,int StackS);
//void pascal ExpDetailedInfo(int,int,int,int,int,int,int,int,int,int);
int __stdcall ExpDetailedInfo(int Field);
int __stdcall ShowExpoPrepBattle(Dword p4,Dword p3,Dword p2,Dword p1);
void UpgrInd(void);
void UpgrHill(void);
int __stdcall CheckSpellCostReduction(int Cost,_Hero_ *Hp);
void DefenseBonus(void);
int DeathBlow(void);
int PersonalHate(void);
//int MagicDwarfResistance2(void);
float __fastcall MagicDwarfResistance3(int spell, int, int p2,int p1);
Byte SetUpRandomCreatures(void);
void CastPostAttackSpell(void);
int MinotourMoral(void);
char __stdcall ShootAnyTime(int Art);
char __stdcall ShootAnyTime2(int Art);
int UnicornAura(void);
int UnicornAura2(void);
//float pascal SpecMagicResiatance(int p1,int p2,int p3,Byte *Mon,int Side,int Spell);
char __stdcall DispellResist(int Side,_Hero_ *DHero);
int HeroTacticFix(void);
void WrongRelictPictureFix(int Buffer,int FormatString,int ArtNum);
void IsAngel3(void);
int __stdcall ShowExpoPrepUnknown(int,Dword,Dword,Dword);
void __stdcall ResetSpell(int Spell);
void __stdcall MonsterStrikeFirst(Dword Par1,Dword Par2);
void __stdcall MonsterStrikeBack(Dword Par1,Dword Par2);
void __stdcall MonsterStrikeSecond(Dword Par1,Dword Par2);
int DracolishRandom(void);
void DisableAIStackSwitching(int);
void LoadShootingDef(void);
//int __stdcall Send358_1(int p2,int p3);
//int __stdcall Receive358_1(void); // 40624D
void FixSummonElementalNoHero(void);
void FixDeathRippleNoHero(void);
int __stdcall FixNoHero4SpellLevelCalc(int Scool,int Spell);
int __stdcall FixNoHero4SpellPower(int Par3,int Par2,int Spell);
void FixWrongDwIndKingdomOverview(void);
int FixNoHero4SpellDuration(void);
void __stdcall FixShowBFPopUpMonInfo(Byte *Mon,int);
float __stdcall DeathRippleResist(int Spell,int ASide,Byte* DMon,int Mod1,int Mod2,int heroMONSTR);
int FixShootTwiceOneShot(void);
void TownWrongExpoFix(void);
void NoClone4CommandersFix(void);
// 3.58f
void CrossOverGodBonus(void);

///////////////////
void ManaAdd2Hero(int side);
int ERM_MonAtr(char Cmd,int Num,_ToDo_*,Mes *Mp);
int ERM_Battle(char Cmd,int Num,_ToDo_*,Mes *Mp);
int ERM_BRound(char Cmd,int Num,_ToDo_*,Mes *Mp);
int LoadGhostParam(void);
void __stdcall MonsterKickBack(Dword mon,int HPdel,int Killed,int HPorig);
void __stdcall Enter2Monster(_MapItem_ *Mi,_Hero_ *Hr,int MixedPos,int GM_ai);
void __stdcall Enter2Monster2(_MapItem_ *Mi,_Hero_ *Hr,int MixedPos,int GM_ai);
void __stdcall Creature2Hire(Dword);
int PlaceCreature(void);
void MonMapPicOn(void);
void MonMapPicOff(void);
int SaveMapMon(void);
int LoadMapMon(int ver);
void ResetMapMon(void);
int Enter2Dwelling(int GM_ai,_MapItem_ *Mi,_Hero_ *Hr,Dword /*avd.Manager*/,Dword *Object);
void EnterOutOfDwelling(int GM_ai,Dword Object);
void __stdcall M2MSmooth(int SSlot,Dword DStr,int DSlot,Dword P2v,Dword P1v);
void __stdcall M2MStack(int SSlot,Dword DStr,int DSlot);
void M2MSwap(void);
void __stdcall M2MDismiss(int SSlot);
int Enter2Bank(int,_MapItem_*,_Hero_*,Dword,Dword*);
void EnterOutOfBank(int,Dword);
int __stdcall Get2Battle(Dword,_Hero_*,_MonArr_*,int,
												 Dword,_Hero_*,_MonArr_*,int,
												 Dword,Dword); // ECX=Adv.Manager
void DaylyGodsBonus(int);
int GetGodBonus(int hi,int gt);
int DoesHeroGot(_Hero_ *hr);
void __stdcall Monster2Battle(Dword Pv2,Dword Pv1);
void DoNotFly(void);
void NoMagic(void);
void Fear1(void);
void Fear2(void);
void CheckForWrite(void);
void GDMagic(void);
void IsTroll(void);
void IsFireBird(void);
void IsHarpy(void);
void MedusaMagic(void);
void UnicornMagic(void);
void ShooterPower(void);
void IsDevil(void);
void IsDevil1(void);
void IsDevil2(void);
void IsAngel(void);
void IsAngel1(void);
void ThroughWall(void);
void SetUpNewRoundOnce(void);
extern int MonsterOfWeek[];
void DaylyMonstr(void);
void NoMoreTactic(void);
void NoMoreTactic1(void);
void DaylySG(int);
void __stdcall WeeklyDwellingGrow(int);
void __stdcall AppearWithoutTacktic(int,int,int);
void PrepareBF(void);
Dword NoCliff(void);
Dword SetBFBack(void);
int ERM_BattleField(char Cmd,int Num,_ToDo_*,Mes *Mp);
int ERM_BMonster(char Cmd,int Num,_ToDo_*,Mes *Mp);
int ERM_MAction(char Cmd,int /*Num*/,_ToDo_* /*sp*/,Mes *Mp);
int ERM_BHero(char Cmd,int Num,_ToDo_*sp,Mes *Mp);
int ERM_BUniversal(char Cmd,int Num,_ToDo_*,Mes *Mp);
void ChooseMonAttack(void);
void SendIt(Byte *Source,int Len);
void ReceiveIt(Byte *Dest,int Len);
int M2B_FindPos(int Code,int *x,int *y);
int M2B_GetPosCode(int x,int y);
int M2B_CheckPosCode(int x,int y);

// 3.58
int ERM_MonRes(char Cmd,int Num,_ToDo_*sp,Mes *Mp); //!!MR
int ERM_MonFeature(char Cmd,int/*Num*/,_ToDo_* /*sp*/,Mes *Mp); //!!MF
//int SCastSpell(int SNum,Byte *Mp,_Hero_ *hpa,_Hero_ *hpd,int Fixed=-1);
int SCastSpell(int SNum,Byte *Mp,_Hero_ *hpa,_Hero_ *hpd,int Fixed=-1,int NoAnim=0);
void WereWolf(Byte *MonA,Byte *MonD);
void Dracolich(Byte *MonA,Byte *MonD);
char Check4DragonHeartArt(int Art);
void DracolichShoot(void);
void NetworkMessageParse(Dword,char *buffer, const char *format,int p1,int p2,int p3,int p4,int p5,int p6);
// 3.58f
void BackupGodBonus(void);
void LoadShootingDef4Commanders(void);

int ERM_NetworkService(char Cmd,int Num,_ToDo_*sp,Mes *Mp);
void ReceiveNetBFCommand(void);
void ReceiveNetAMCommand(void);
void SendCustomRequest(int ind,int *var,int num);
#define MYNETMES 2000

#define NM_VARS     1 /*send ERM v vars from to*/
#define NM_WVARS    2 /*send ERM w vars from to*/
#define NM_FLAGS    3 /*send ERM flags*/
#define NM_RGSEED  10 /*set RG seed*/
#define NM_CUSTOM  11 /*custom request*/
class NetData{
public:
	int    Flags;
	Dword *DataBuf;
	NetData(int cmd,int whom,void *data,int num){
		DataBuf=new Dword[num+7]; Flags=1;
		DataBuf[0]=0xFFFFFFFF;
		DataBuf[1]=0;
		DataBuf[2]=MYNETMES;
		DataBuf[3]=(num+7)*4;
		DataBuf[4]=0;
		DataBuf[5]=cmd;
		DataBuf[6]=whom;
		if(data)
			for(int i=0;i<num;i++)
				DataBuf[i+7]=((Dword *)data)[i];
	}
	NetData(Dword *data){
		Flags=0;
		DataBuf=data;
	}
	NetData(){ Flags=0; DataBuf=0; }
	~NetData(void){ if(Flags&1) delete DataBuf; DataBuf=0; Flags=0;}

	NetData(NetData &src){
		DataBuf=0; Flags=0;
		if(src.DataBuf!=0){
			int len=src.DataBuf[3]/4;
			DataBuf=new Dword[len]; Flags=1;
			for(int i=0;i<len;i++) DataBuf[i]=src.DataBuf[i];
		}
	}
	NetData &operator=(NetData &src){
		if(Flags&1) delete DataBuf;
		DataBuf=0; Flags=0;
		if(src.DataBuf!=0){
			int len=src.DataBuf[3]/4;
			DataBuf=new Dword[len]; Flags=1;
			for(int i=0;i<len;i++) DataBuf[i]=src.DataBuf[i];
		}
		return *this;
	}
	
	int Command(void){ return DataBuf[5]; }
	int Whom(void){ return DataBuf[6]; }
	void *Data(void){ return &DataBuf[7]; }

	int GetBatleOppositePlayer(void){
		__asm{
			mov   eax,0x699420
			mov   eax,[eax]
			mov   eax,[eax+132C0h]
			mov   edx,0x697790
			shl   eax, 2
			sub   edx, eax
			mov   eax,[edx]
			mov   IDummy,eax
		}
		return IDummy;
	}
	int SendTo(int whom=-1){
		if(whom==-1) whom=0x7F;//whom=GetBatleOppositePlayer();
		Dword *_bp=DataBuf;
		__asm{
			mov    ecx,_bp
			mov    edx,whom
			push   1
			push   0
			mov    eax,0x5549E0
			call   eax
			mov    IDummy,eax
		}
		return IDummy;
	}
};

////////////////////////////////////////////////////////
// структуры и функции по управлению сопративляемостью к магии моснтров
// функция управления сопративлением магии
void ResistMagicManager (); //Diakon
// функция для патча exe
void InsertResistFunc(); // Diakon
// функция сброса/загрузки настроек сопративляемости монстров
void ResetLoadDefaultResist(); //Diakon
// функция сброса\загрузки монстров с атакой гарпии
void ResetLoadHarpyAtack(); //Diakon
// управление хинта при касте магии
void ManageHintCast(); //Diakon
void MagicCastManager(); //Diakon
void RandMagic(); //Diakon
void RandMagicAI(); //Diakon
void AICastMagic(); //Diakon


////////////////////////////////////////////////////////



