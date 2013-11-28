#include <string.h>
//#define EXPOMUL 100
#define EXPOMUL1 1
#define EXPOMAX2 20000
//#define HEREXPMUL 50
#define HEREXPMUL 1
#define BFNUM 42
#define CURMON 1000

#define EXPARTFIRST 156
//#define STREXPARTFIRST "7"
#define EXPARTLAST  156

#define MAXICONNUM 7

typedef void * CRLOC;
class CrExpo{
	CrExpo(CrExpo &);
public:
	int   Expo; // тек. опыт на 1 существо
	int   Num;  // число существ
	struct {
		unsigned   Act    :  1;  // есть/нет
		#define CE_HERO  1
		#define CE_MAP   2
		#define CE_TOWN  3
		#define CE_MINE  4
		#define CE_HORN  5
		unsigned   Type   :  4;  // тип - где хранится
		unsigned   MType  :  8;  // тип существ
		unsigned   mHasArt :  1;  // has art
		unsigned   mArt    :  2;  // up to 4 arts
		unsigned   mCopyArt:  2;  // up to 4 copy
		unsigned   mSubArt :  4;  // up to 16 sub art
		unsigned  _un     : 10;  // reserve
//    unsigned  _un    : 19;
	} Fl;
	union _Dt{
		CRLOC UniData; // универсальный для присвоения только
		#define MAKEMP(x,y,l) (void *)((x)+(y)<<16+(l)*0x04000000)
		Dword MixPos; // позиция на карте
		#define MAKEHS(i,s) (void *)((i)+(s)*0x10000)
		struct _Hero{
			short Ind;  // номер героя
			short Slot; // слот в армии героя
		} Hero;
		#define MAKESS(x,y,l) (void *)((x)+(y)*0x100+(l)*0x10000)
		struct _Map{
			unsigned  X    :8;
			unsigned  Y    :8;
			unsigned  L    :1;
			unsigned _un   :15; // резерв
		} Map;
		#define MAKETS(x,y,l,s) (void *)((x)+(y)*0x100+(l)*0x10000+(s)*0x20000)
		struct _Town{
			unsigned  X    :8;
			unsigned  Y    :8;
			unsigned  L    :1;
			signed    Slot :15; // слот в армии героя
		} Town;
		#define MAKEMS(x,y,l,s) (void *)((x)+(y)*0x100+(l)*0x10000+(s)*0x20000)
		struct _Mine{
			unsigned  X    :8;
			unsigned  Y    :8;
			unsigned  L    :1;
			signed    Slot :15; // слот в армии шахты
		} Mine;
		#define MAKEZS(x,y,l,s) (void *)((x)+(y)*0x100+(l)*0x10000+(s)*0x20000)
		struct _Horn{
			unsigned  X    :8;
			unsigned  Y    :8;
			unsigned  L    :1;
			signed    Slot :15; // слот в армии шахты
		} Horn;
		#define MAKESS(x,y,l) (void *)((x)+(y)*0x100+(l)*0x10000)
	} Dt;

	CrExpo();
	~CrExpo();
	CrExpo &operator =(CrExpo &);

	void SetN(int Type,CRLOC Data,int mtype,int num,int expo=0,int arts=0);
	void Clear();
	void Check4Max();
	int  RealType(int type=-2);
	int  RealNum(int num=-2);
	void RecalcExp2RealNum(int num,int type);
	int  Validate(void);
	void AddExpRel(int e){ if(Fl.Act==0) return; Expo+=e; }
	// artifact
	int  HasArt(void){ return Fl.mHasArt; }
	int  GetArt(void){ return Fl.mArt+EXPARTFIRST; }
	int  GetSubArt(void){ return Fl.mSubArt; }
	void SetSubArt(int s){ Fl.mSubArt=s; }
	int  SetArt(int a,int s){ Fl.mArt=a-EXPARTFIRST; Fl.mSubArt=s; Fl.mHasArt=1; Fl.mCopyArt=0; return 1; }
	void SetArtAll(int h,int a,int s,int c){ Fl.mHasArt=h; Fl.mArt=a-EXPARTFIRST; Fl.mSubArt=s; Fl.mCopyArt=c; }
	void DelArt(void){ Fl.mArt=0; Fl.mSubArt=0; Fl.mHasArt=0; Fl.mCopyArt=0; return; }
	int  ArtCopy(void){ return Fl.mCopyArt; }
	void ArtCopy(int cpy){ if(cpy>3) cpy=3; if(cpy<0) cpy=0; Fl.mCopyArt=cpy; }
	int  GetArtNums(void){ if(Fl.mHasArt==0) return 0; return(ArtCopy()+1); }
	int  TakeArt(void){ 
		if(Fl.mHasArt==0) return 0; 
		if(ArtCopy()>0){ ArtCopy(ArtCopy()-1); return 1; }
		Fl.mArt=0; Fl.mSubArt=0; Fl.mHasArt=0; Fl.mCopyArt=0;
		return 1; 
	}
	void CopyArt(CrExpo &s){ Fl.mArt=s.Fl.mArt; Fl.mSubArt=s.Fl.mSubArt; Fl.mHasArt=s.Fl.mHasArt; Fl.mCopyArt=s.Fl.mCopyArt; }
	int  AddArt(CrExpo &s){
		if(s.HasArt()==0) return 0; // нечего добавлять
		if(HasArt()){ // уже есть арт
			ArtCopy(ArtCopy()+s.ArtCopy()+1); // количество копий плюс сам один артифакт
		}else{ // нет арта
			CopyArt(s);
		}
		return 1;
	}
	int AddArt(int nums){
		if(nums<1) return 0;
		if(HasArt()){ // уже есть арт
			ArtCopy(ArtCopy()+nums); // количество копий - количество добавляемы плюс копии старые
		}else{ // нет арта
			SetArt(EXPARTFIRST,0); ArtCopy(nums-1);
		}
		return 1;
	}
};

#define MAXCRINFONUM 10000L
class CrExpoSet{
	static CrExpo  Body[MAXCRINFONUM];
	static int LastUser; // последний игрок
	public:
		static int PlayerMult;
		static int AIMult[5];
		static int AIBase[5];
		static int AITMult[5];
	private:
	static Dword HerExpo[256]; // опыт всех героев
//  static Byte  HerArts[256]; // артифакт стэков у АИ героев
	
	CrExpoSet(CrExpoSet &);
	CrExpoSet();
 ~CrExpoSet();
	CrExpoSet &operator=(CrExpoSet &);
	// найти такой
	static int FindIt(int Type,CRLOC Data);
	public:
		static CrExpo BFBody[BFNUM]/*,Cur*/;

		static void Copy(CrExpo *Dst,CrExpo *Src){ memcpy(Dst,Src,sizeof(CrExpo)); }
//    static CrExpo *GetCur(void){ return &Cur; }
		static CrExpo *GetBF(int ind){ return &BFBody[ind]; }
		static CrExpo *Get(int ind){ return &Body[ind]; }
		static void StartAllBF(_MonArr_ *AMAr,_MonArr_ *DMAr);
		static void StopAllBF(void);
		static void AdjustStackTypes(int ATded,_Hero_ *Hp); // подправить типы стэков если АИ их переставлял
		// найти такой публично
		static CrExpo *Find(int Type,CRLOC Data);
		static void Clear();
		static int  Load(int Ver);
		static int  Save();
		// такой должен быть и ему меняем
		static int SetN(int Type,CRLOC Data,int mtype,int num,int expo);
		static int Set(CrExpo &cr);
		// удалить
		static void Del(int Type,CRLOC Data);
		// потщем пустое место
		static CrExpo *FindEmpty(void);
		// получить опыт
		static int GetExp(int Type,CRLOC Data);
		// get experience or expo level
		static int GetExpM(int Type,CRLOC Data,int M=0);
		// подправить статистику стэку (если есть что) на поле боя
//    static int ApplyExp(int Type,CRLOC Data,Byte *Mon);
		// добавить всем живым этого героя опыт
		static void AddExpo(_Hero_ *Hp,int NewHExp,int OldHExp);
		// перенести стэк из одного слота в другой
		static int HMove(int Type,int TypeN,CRLOC Data,CRLOC DataN,int type,int typeN,int num,int numN);
		// добавить первый стэк ко второму
		static int HComb(int Type,int TypeN,CRLOC Data,CRLOC DataN,int type,int typeN,int num,int numN);
		// добавить первый стэк ко второму реально (не только опыт)
		static int HCombReal(int Type,int TypeN,CRLOC Data,CRLOC DataN,int type,int typeN,int num,int numN);
		// ищем - кто хозяин армии
		static int FindType(_MonArr_ *MArr,int Stack,int *Type,CRLOC *Crloc);
		// ищем - кто хозяин по позиции объекта
		static int FindType(int x,int y,int l,int Stack,int *Type,CRLOC *Crloc,int *MType=0,int *MNum=0);
		// ищем - кто хозяин армии по индексу
		static int FindType(int Ind,int *Type,CRLOC *Crloc,int *Slot);

		static int Modify(int Tp,int Type,CRLOC Crloc,int E,int M=0,int T=-1,int ON=0,int N=0,_MonArr_ *Ma=0);

		static void DaylyAIExperience(int User); // AI новый пользователь - применить опыт
//    static void BuildBFExpo(int *ExpoArr); // составить список опыта
		static int DropArt2Stack(_MouseStr_ *ms,_Hero_ *hp,int DropArt);
};

int LoadExpTXT(void);

char *Exp2String(int exp,int Limit,int HasArt);
char *Exp2String2(int exp[2],int Limit,int HasArt[2]);
///////////////////////////////////////
#define MAXCREXPAB 256
class CrExpMod{
	static struct CrExpModStr{
		float ExpMul;
		float UpgrMul;
		int   Limit;
		int   Lvl11Exp;
		int   Cap; // in percettages
	} Body[MAXCREXPAB],Dummy;
	static int Ranks[11]; // fullrange scale , exp per level

	friend void CrExpoSet::StartAllBF(_MonArr_ *AMAr,_MonArr_ *DMAr);
	friend void CrExpoSet::AdjustStackTypes(int atDEF,_Hero_ *Hp);
	friend void CrExpoSet::StopAllBF(void);

	CrExpMod(CrExpMod &);
	CrExpMod();
 ~CrExpMod();
	CrExpMod &operator=(CrExpMod &);

	public:
	static struct CrExpModStr BFBody[BFNUM],Cur;
		
		static void Copy(CrExpModStr *Dst,CrExpModStr *Src){ memcpy(Dst,Src,sizeof(CrExpModStr)); }
		static CrExpModStr *GetCur(void){ return &Cur; }
		static CrExpModStr *GetBF(int cr){ 
			if((cr<0) || (cr>=BFNUM)) return &Dummy;
			return &BFBody[cr]; 
		}
		static CrExpModStr *Get(int cr){
			if(cr==CURMON) return &Cur;
			if((cr<-BFNUM)||(cr>=MAXCREXPAB)) return &Dummy;
			if(cr<0) return &BFBody[-cr-1];
			else return &Body[cr];
		}
		static int  Load(int Ver);
		static int  Save();
		static int  Clear(); // из TXT файла

		static float ExpMul(int cr){ return Get(cr)->ExpMul; }
		static void  SExpMul(int cr,float Val){ Get(cr)->ExpMul=Val; }
		static float UpgrMul(int cr){ return Get(cr)->UpgrMul; }
		static void  SUpgrMul(int cr,float Val){ Get(cr)->UpgrMul=Val; }
		static int   Limit(int cr){ return Get(cr)->Limit; }
		static void  SLimit(int cr,int Val){ Get(cr)->Limit=Val; }
		static int   CrLimit(CrExpo *cr){
			if(cr==0) return EXPOMAX2;
			int tp=cr->Fl.MType;
			return Get(tp)->Limit;
		}
		static int   Lvl11(int cr){ return Get(cr)->Lvl11Exp; }
		static void  SLvl11(int cr,int Val){ Get(cr)->Lvl11Exp=Val; }
		static int   MaxExpo(int cr){ return Get(cr)->Limit+Get(cr)->Lvl11Exp; }
		static int   CrMaxExpo(CrExpo *cr){
			if(cr==0) return 1;
			int tp=cr->Fl.MType;
			return Get(tp)->Limit+Get(tp)->Lvl11Exp;
		}
		static int   Cap(int cr){ return Get(cr)->Cap; }
		static void  SCap(int cr,int Val){ Get(cr)->Cap=Val; }
		static int   CrCap(CrExpo *cr){
			if(cr==0) return 100;
			int tp=cr->Fl.MType;
			return Get(tp)->Cap;
		}
		static int   CapIt(CrExpo *cr,int Exp){
			int v=CrLimit(cr)*CrCap(cr)/100; if (v<1) v=1;
			if(Exp>v) Exp=v;
			return Exp;
		}
		static int   Exp4Level(int cr,int lvl,int exp0=0){
			if((cr<0)||(cr>=MAXCREXPAB)) return 0;
			if(lvl<0) return 0;
			if(lvl>10) return MaxExpo(cr);
			int lvl0=GetRank(cr,exp0);
			int dexp=exp0-GetRankExp(cr,lvl0);
			lvl0+=lvl; if(lvl0<0) lvl0=0; if(lvl0>11) lvl0=11;
			int Exp=GetRankExp(cr,lvl0)+dexp;
			return Exp;
		}
		static int  GetRank(int cr,int exp){
			float scale=(float)Get(cr)->Limit/Ranks[0];
			if(scale<0.000001) return 0;
			float expnorm=(float)((exp+0.00001)/scale);
			for(int i=1;i<11;i++){
				expnorm-=Ranks[i];
				if(expnorm<0) return i-1;
			}
			return 10;
		}
		static int  GetRank(CrExpo *cr){
			float scale=(float)Get(cr->Fl.MType)->Limit/Ranks[0];
			if(scale<0.000001) return 0;
			float expnorm=(float)((cr->Expo+0.00001)/scale);
			for(int i=1;i<11;i++){
				expnorm-=Ranks[i];
				if(expnorm<0) return i-1;
			}
			return 10;
		}
		static int  GetLimRank(int exp,int lim){
			float scale=(float)lim/Ranks[0];
			if(scale<0.000001) return 0;
			float expnorm=(float)((exp+0.00001)/scale);
			for(int i=1;i<11;i++){
				expnorm-=Ranks[i];
				if(expnorm<0) return i-1;
			}
			return 10;
		}
		static int  GetRankExp(int cr,int rank,int rank0=0){
			int exp=0;
			float scale=(float)Get(cr)->Limit/Ranks[0];
			if(scale<0.000001) return 0;
			if(rank<0) rank=0;
			if(rank>11) rank=11;
			if(rank0<0) rank0=0;
			if(rank0>11) rank0=11;
			if(rank<=rank0) return 0;
			for(int i=rank0;i<rank;i++){
				if(i!=10){
					exp+=Ranks[i+1];
				}else{
					exp+=Get(cr)->Lvl11Exp;
				}
			}
			return (int)((exp+0.01)*scale);
		}
};
////////////////////////////////////////
class CrExpBon{
	static struct CrExpBonStr{
		struct {
			unsigned  Act :  1;  // есть/нет
			unsigned _un  :  31;
		} Fl;
		char  Type; // тип спецспособности
		char  Mod; // модификатор типа
//    char *Descr; // указатель на описание
		Byte  Lvls[11];  // бонус на уровнях
	}Body[MAXCREXPAB][20],Dummy[20];
	static struct CrExpBonBFStartStat{
		char fA,fD,fH,fm,fM,fS,fO,fP,fR;
		int  aA,aD,aH,am,aM,aS,aO,aP,aR;
	} BFStat[BFNUM];

	CrExpBon(CrExpBon &);
	CrExpBon();
 ~CrExpBon();
	CrExpBon &operator=(CrExpBon &);

	friend void CrExpoSet::StartAllBF(_MonArr_ *AMAr,_MonArr_ *DMAr);
	friend void CrExpoSet::AdjustStackTypes(int atDEF,_Hero_ *Hp);
	friend void CrExpoSet::StopAllBF(void);

	static int  FindBon(CrExpBonStr *str,char Type,int Mod, int silent=0);
	static void CrExpBon::IntCastMassSpell(Byte *Mon,Byte *DMon,char Ch);
	public:
	static int IsBattle;
	static struct CrExpBonStr BFBody[BFNUM][20],Cur[20];
	static char BFBodyAct[BFNUM];

		static void ClearCrExpBonBFStartStat(int Index){
			if(Index<0 || Index>=BFNUM) return;
			SetMem(&BFStat[Index],sizeof(CrExpBonBFStartStat),0);
		}
		static int  B2AIndex(int AIndex){
			for(int i=0;i<BFNUM;i++) if(BFBodyAct[i]==AIndex) return i;
			return -1;
		}
		static void SetB2AIndex(int BIndex,int AIndex){
			if(BIndex<0 || BIndex>=BFNUM) return;
			if(AIndex!=-1) BFBodyAct[BIndex]=(char)AIndex; // не бал в армии - установим
			if(BFBodyAct[BIndex]>0) return; // был в армии - пропустим
			BFBodyAct[BIndex]=(char)AIndex;
		}
		static void Copy(CrExpBonStr *Dst,CrExpBonStr *Src){ memcpy(Dst,Src,sizeof(CrExpBonStr)*20); }
		static CrExpBonStr *Get(int cr,int mtype=0){
			if(cr==CURMON) return &Cur[0];
			if((cr<-BFNUM)||(cr>=MAXCREXPAB)) return &Dummy[0];
			if(cr<0){ 
				if(BFBodyAct[-cr-1]==0){ // not installed
					return &BFBody[mtype][0];
				}else{
					return &BFBody[-cr-1][0];
				}
			}
			else return &Body[cr][0];
		}
		static void MakeCur(int Pos,int MType){
			if(Pos==-1){ // not a BF
				memcpy(&Cur[0],&Body[MType][0],sizeof(CrExpBonStr)*20);
				CrExpMod::Copy(CrExpMod::GetCur(),CrExpMod::Get(MType));
			//  CrExpoSet::Copy(CrExpoSet::GetCur(),CrExpoSet::Get(MType));
			}else{ // BF
			//  CrExpoSet::Copy(CrExpoSet::GetCur(),CrExpoSet::GetBF(Pos));
				if(BFBodyAct[Pos]==0){ // not installed
					memcpy(&Cur[0],&Body[MType][0],sizeof(CrExpBonStr)*20);
					CrExpMod::Copy(CrExpMod::GetCur(),CrExpMod::Get(MType));
				}else{
					memcpy(&Cur[0],&BFBody[Pos][0],sizeof(CrExpBonStr)*20);
					CrExpMod::Copy(CrExpMod::GetCur(),CrExpMod::GetBF(Pos));
				}
			}
		}
		static int  ApplyBFBonus(int TypeS,int TypeD, int Mode);

		static int  Load(int Ver);
		static int  Save();
		static int  Clear(); // из TXT файла
		static int  SetBonLine(int MType,int Ind,int Flags, int Type, int Mod, int *lvls);
		static int  GetBonLine(int MType,int Ind,int *Flags, int *Type, int *Mod, int *lvls);
		static int  FindThisBonLineInd(int MType,char Bon,char Mod);

		static int  GetStackExpo(Byte *Mon,int *VExp);
		static int  PrepareBFExpStructure(Byte *BatMan);
//    static int  GetHeroStackExpo(_Hero_ *Hp,int Stack);
		static int  Apply(Byte *Mon); // настройка бонусов на поле боя 1 раз в самом начале
		static int  Apply2(Byte *Mon); // настройка бонусов на поле боя каждый новый раунд

		static int  StackBlock(Byte *Mon,int DefPerc); // применить блок?
		static int  StackBlockPartial(Byte *Mon,int Dam); // частичный блок
		static int  Fear(Byte *Mon); // страх?
		static int  Fearless(Byte *Mon); // бесстрашный
		static int  DwarfResist(Byte *Mon,int Res,int Spell); // шанс игнорир магию (dwarf type)
		static int  DwarfResistFriendly(Byte *Mon,int Res,int Spell); // шанс игнорир друж магию (dwarf type)
		static int  NoDistPenalty(Byte *Mon); // нет пенальти от дистанции
		static int  NoObstPenalty(Byte *Mon); // нет пенальти от припятствий
		static int  StackSubSpCost(_Hero_ *Hp,int Stack,int val); // снизить цену заклинания
		static int  DefenceBonus(Byte *Mon,int Def); // бонус к защите при выборе защиты
		//440422 - Dead Knight Curse???
		//4435A3 - Dead Knight Deathblow
		static int  DeathBlow(Byte *Mon,int Chance); // бонус к защите при выборе защиты
		static int  PersonalHate(Byte *MonA,int DType,int mult); // первональная ненависть
		static int  CastSpell(Byte *Mon,Byte *DMon,_Hero_ *hpa,_Hero_ *hpd); // может скастовать спел при атаке
		static void CastMassSpell(Byte *Mon,Byte *DMon); // может скастовать массовый спел при атаке
		static void CastMassSpell2(Byte *Mon,Byte *DMon); // может скастовать массовый спел при атаке ОСЛЕ УДАРА
		static void ApplySpell(Byte *mon,Byte *BatMan); // наложить заклинание в начале раунда
		static int  IsHarpy(Byte *Mon); 
		static int  ReduceDefence(Byte *AMon,Byte *DMon,int Def);
		static int  Champion(Byte *Mon); // бонус чемпиона
		static int  GolemResist(Byte *Mon,int Dam,int OrDam,int Spell); // сопротивление Голема
		static int  Regenerate(Byte *Mon,int HPloss); // регенерация
		static int  MinotourMoral(Byte *Mon); // позитивная мораль в битве
		static int  UnicornAura(Byte *Mon); // магич. аура
		static int  ShootAnyTime(Byte *Mon); // стреляет даже если противник рядом
		static int  DeathStare(Byte *Mon); // DeathStare
		static int  Rebirth(Byte *Mon); // возраждение стэке (FireBird) число заклов - число кастов
		static int  SGBonus(Byte *Mon,int *Type,int *Num); // бонус существ для SG
//    static float DispellResistAndOther(Byte *Mon,int Spell,int ASide,float Res); // иммунитет к Dispell и др.
		static int  DispellResist(int Spell,Byte *Mon,int ASide); // иммунитет к Dispell
//    static void ApplyMassSpell(Byte *mon); // наложить заклинание в начале раунда на эксперте

		static void PrepareInfo(int Mon,int Num,int Expo,CrExpo *Cr,int Changing);
		static void ShowInfo(int Mon,int Num,int Expo,CrExpo *Cr);
		static void ShowInfo(Byte *Mon);

		static int  ExperienceSpellCastingPower(int Spell);
};

int ERM_AICrExp(char Cmd,int Num,_ToDo_*sp,Mes *Mp);
//extern CrExpo ToSendCrExpo[2][7];
//void SwapAIStacks(void);


