extern int ExpoFromBattle;

// functions for executable
void NPC_AddExpo(void);
void NPC_ShowInfo(void);
void ClickNPC(void);
// functions to call
int NPC_Resist(int,int,Byte *);
void DaylyNPC(int owner);
int GetNPCMagicPower(Byte *);
void PlaceNPCAtBattleStart(Byte *bm);
int PlaceNPCAtBattleStart2(Byte *bm,int Side,int Placed);
void CheckForAliveNPCAfterBattle(Byte *bm);
void AddMagic2NPC(Byte *mon,Byte *BatMan);
int CanNPCRegenerate(int,Byte *);
int NPC_Paralize(void);
//int HasNPCFear(void);
int HasNPCFear(Byte *Mon);
int NPCReduceDefence(Byte *mon,int Defence);
int NPCDeathStare(Byte *,Byte *);
int NPCChampion(Byte *);
void NPC2Castle(int GM_ai,_Hero_ *Hr,_CastleSetup_ *Cstr);
int NPCFirstAid(Byte *mon);
char NPCBalistaControl(_Hero_ *);
void NPCAttack0(_MonArr_ *G2B_MArrA,_MonArr_ *G2B_MArrD,_Hero_ *G2B_HrA,_Hero_ *G2B_HrD);
void NPCAttack(_MonArr_ *G2B_MArrA,_MonArr_ *G2B_MArrD,_Hero_ *G2B_HrA,_Hero_ *G2B_HrD);
int NPCNoShootPenalty(int Type,int YesRet,Byte *);
void CommanderArtsAI(_Hero_ *hp);
int GetSizeOfSendData(void);
void SendNPC(int *len,Byte **buf,int HeroNum);
void ReceiveNPC(int len,Byte *Buf);

int ERM_NPC(char Cmd,int /*Num*/,_ToDo_*sp,Mes *Mp);

void EnableNPC(int HNum,int HasAtStart);
void DisableNPC(int HNum);

int IsNPCDead(int HNum);
void ResetNPCExp(int HNum);
int  SaveNPC(void);
int  LoadNPC(int);
void ResetNPC(void);

int LoadNPCParam(void);

// 3.58
void CrossOverNPC(int ind);
void BackupNPC(void);
int  CommanderBlock(Byte *Mon,int Type,int /*Dam*/);
int  HeroHasCommander(_Hero_ *hp);
int  HeroHasAliveCommander(_Hero_ *hp);
void AdjastCommanderStat(_Hero_ *hp,int bonus,int GM_ai);
int  ShowBFNPCInfo(int Type,Byte *);
void ApplyCmdMonChanges(Byte *Mon);
int NonHeroNPCAnimDeadCast(void);
int IsNPCNotCreature(Byte *Mon);

