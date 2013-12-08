#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "casdem.h"
#include "service.h"
#include "monsters.h"
#include "herospec.h"
#include "erm.h"
#include "npc.h"
//#include "classes.h"
#include "b1.h"
#include "CrExpo.h"
#define  __FILENUM__ 2

struct _HeroSpecWoG_{
	int    Ind; // +1
	int    SpecPicInd[2];
//  char  *HPSName[2];
//  char  *HPLName[2];
	char  *Name[2];
	char  *HBios[2];
	Byte  *HPSLoaded[2];
	Byte  *HPLLoaded[2];
	_HeroSpec_ Spec[2];
} HeroSpecWoG[]={
// Нагаш
{79,{156},/*{"HPSZ0001.PCX","HPS079Nc.PCX"},{"HPLZ0001.PCX","HPL079Nc.PCX"},*/
		{"1"},{"2"},{0},{0},{{HSUPERSPEC,HSPEC_DARKNESS,0,0,0,0,0,"3","4","5"},{0}}},
// Жеддит
{91,{157},/*{"HPL091WL.PCX","HPS091WL.PCX"},{"HPL091WL.PCX","HPL091WL.PCX"},*/
	 {"6"},{"7"},{0},{0},{{HSUPERSPEC,HSPEC_ANYTOWN,0,0,0,0,0,"8","9","10"},{0}}},
{-1}
};

void InitHeroLocal(void)
{
	STARTNA(__LINE__, 0)
	if(WoGType){
		HeroSpecWoG[0].Name[0]="Коптилка";
		HeroSpecWoG[0].HBios[0]="В детстве Нагаш любил часами сидеть под черной тенью Вуали Тьмы. Рожденный некромантом, он не любил общества и всегда пытался скрыться с глаз. Поэтому, Вуаль Тьмы притягивала его как магнит.  И, конечно, ему было интересно узнать, как она работает. Яблок на голову Нагаша не падало, но терпение и труд сделали свое дело. Долгие годы тренировок позволили ему в совершенстве овладеть такой полезной специальностью, как Вуаль Тьмы. И теперь он не упускает шанса воспользоваться своим навыком.";
		HeroSpecWoG[0].Spec[0].SpShort="Затемнение";
		HeroSpecWoG[0].Spec[0].SpFull="Ходячая Вуаль Тьмы";
		HeroSpecWoG[0].Spec[0].SpDescr="{Передвижная вуаль тьмы}\n\n По мере передвижения Героя по карте, закрывает прилегающую часть поверхности для всех игроков, которые не являются союзниками данного."
			"\n\nРадиус закрываемой поверхности определяется по следующей формуле:"
			"\n1. Выбирается меньшее из полного текущего количества очков заклинаний и уровнем знаний*10."
			"\n2. Делим полученное число на 10 и прибавляем 0.5"
			"\n3. Минимальный радиус 1.5";
		HeroSpecWoG[1].Name[0]="Строитель";
		HeroSpecWoG[1].HBios[0]="В юности Джедит объездил весь мир,  и везде его поражала красота незнакомых городов. Будучи очень любопытным и наблюдательным, он всегда расспрашивал местных каменщиков о том, как же такие города строятся. Теперь, разобравшись в этом, он смог бы, пожалуй, построить любой город.";
		HeroSpecWoG[1].Spec[0].SpShort="Постройка";
		HeroSpecWoG[1].Spec[0].SpFull="Постройка Города";
		HeroSpecWoG[1].Spec[0].SpDescr="Позволяет выбирать любой тип города при восстановлении полностью разрушенного города.";
	}else{
		HeroSpecWoG[0].Name[0]="Pitch";
		HeroSpecWoG[0].HBios[0]="In childhood, Nagash liked to sit for hours under the black shadow of the Veil of Darkness. Because he was a necromancer, he did not like society and always tried to disappear from sight. And thus, the Veil of Darkness drew him like a magnet. He was intrigued by how it functioned, and after much study he finally understood. In essence, it was extremely simple and only required a source of mana to be channeled. Long years of training have allowed Nagash to perfect this useful ability and now he wastes no time in taking advantage of the skill.";
		HeroSpecWoG[0].Spec[0].SpShort="Darkness";
		HeroSpecWoG[0].Spec[0].SpFull="Pitch darkness";
		HeroSpecWoG[0].Spec[0].SpDescr="{Mobile Veil of Darkness}\n\n When the hero moves, the shroud is replaced in a circle around him for all players who are not allies."
			"\n\nRadius of the shroud circle is determined by the following formula:"
			"\n1. Take the lesser of current spell points or (knowledge x 10)"
			"\n2. This number is divided by 10 and then 0.5 is added"
			"\n3. Minimum radius is 1.5";
		HeroSpecWoG[1].Name[0]="Builder";
		HeroSpecWoG[1].HBios[0]="In youth, Jeddite traveled all over the world, and everywhere he went he was amazed at the beauty of unfamiliar cities. Being very curious and observant, he always asked local masons how such cities were constructed. Now, having this knowledge, he is able to direct the construction of  any type of city that is being rebuilt.";
		HeroSpecWoG[1].Spec[0].SpShort="Rebulding";
		HeroSpecWoG[1].Spec[0].SpFull="Town Rebuilding";
		HeroSpecWoG[1].Spec[0].SpDescr="Allows Hero to choose any type of city when rebuilding one that has been completely destroyed.";
	}
	RETURNV
}

struct _HeroSpecCus_ HeroSpecCus[HERNUM];
int HeroInfoBackStored=0;
_HeroInfo_    HeroInfoBack[HERNUM+8]; //?????????
struct _PicsBack_{
	Byte *HPSLoaded;
	Byte *HPLLoaded;
} PicsBack[HERNUM];

void PutBackHeroInfo(void)
{
	STARTNA(__LINE__, 0)
	if(HeroInfoBackStored==0) RETURNV
	_HeroInfo_ *hp=GetHIBase();
	Copy((Byte *)HeroInfoBack,(Byte *)hp,sizeof(HeroInfoBack));
	RETURNV
}

void SetHeroPic(int hn,char *Lpic,char *Spic, char * /*path*/)
{
	STARTNA(__LINE__,Lpic)
	_HeroSpecCus_ *p;

	if((hn<0)||(hn>=HERNUM)){ EWrongParam(); RETURNV }
	p=&HeroSpecCus[hn];
	p->Used=1;
	StrCopy(p->HPLName,13,Lpic);
	StrCopy(p->HPSName,13,Spic);
	RETURNV
}

void ChangeHeroPic(int hn,char *Lpic,char *Spic)
{
	STARTNA(__LINE__,Lpic)
	_HeroSpecCus_ *p;
	_HeroInfo_    *hp=GetHIBase();

	if((hn<0)||(hn>=HERNUM)){ EWrongParam(); RETURNV }
	p=&HeroSpecCus[hn];
	if((Lpic==0)&&(Spic==0)){ // восстановить оригинал
		p->Used=0;
		hp[hn].HPLName=HeroInfoBack[hn].HPLName;
//asm int 3
		hp[hn].HPSName=HeroInfoBack[hn].HPSName;
		p->HPLName[0] = 0;
		p->HPSName[0] = 0;
//    RedrawMap();
		RETURNV
	}
	p->Used=1;
	if(Lpic!=0){
		StrCopy(p->HPLName,28,Lpic);
//asm int 3
		hp[hn].HPLName=p->HPLName;
//    p->HPLLoaded=LoadPCX(p->HPLName);
	}
	if(Spic!=0){
		StrCopy(p->HPSName,28,Spic);
//asm int 3
		hp[hn].HPSName=p->HPSName;
//    p->HPSLoaded=LoadPCX(p->HPSName);
	}
//  RedrawMap();
	RETURNV
}

void ChangeHeroPicN(int hn,int hn1)
{
	STARTNA(__LINE__, 0)
	_HeroSpecCus_ *p;
	_HeroInfo_    *hp=GetHIBase();

	if((hn<0)||(hn>=HERNUM)){ EWrongParam(); RETURNV }
	if((hn1<0)||(hn1>=HERNUM)){ EWrongParam(); RETURNV }
	p=&HeroSpecCus[hn];
	if(hn==hn1) p->Used=0; else p->Used=1;
	hp[hn].HPLName=HeroInfoBack[hn1].HPLName;
	StrCopy(p->HPLName,13,hp[hn].HPLName);
//asm int 3
	hp[hn].HPSName=HeroInfoBack[hn1].HPSName;
	StrCopy(p->HPSName,13,hp[hn].HPSName);
//  RedrawMap();
	RETURNV
}

void RefreshHeroPic(void)
{
	STARTNA(__LINE__, 0)
	int i;
	_HeroSpecCus_ *cp;
	_HeroInfo_ *hp=GetHIBase();

	for(i=0;i<HERNUM;i++){
		if(HeroSpecCus[i].Used==0) continue;
		cp=&HeroSpecCus[i];
		if(cp->HPLName[0]!=0){
			hp[i].HPLName=cp->HPLName;
			cp->HPLLoaded=LoadPCX(cp->HPLName);
		}
		if(cp->HPSName[0]!=0){
//asm int 3
			hp[i].HPSName=cp->HPSName;
			cp->HPSLoaded=LoadPCX(cp->HPSName);
		}
	}
	RETURNV
}

void PrepareSpecWoG(int WoG)
{
	STARTNA(__LINE__, 0)
	int   i,j,ind;
//  char *tp,*tp1;
	_HeroSpec_ *sp;
	_HeroSpecWoG_ *wp;
//  _HeroSpecCus_ *cp;
//  char fpath[128];

	_HeroInfo_ *hp=GetHIBase();
	if(HeroInfoBackStored==0){
		Copy((Byte *)hp,(Byte *)HeroInfoBack,sizeof(HeroInfoBack));
		for(i=0;i<HERNUM;i++){
//asm int 3
			PicsBack[i].HPSLoaded=LoadPCX(hp[i].HPSName);
			PicsBack[i].HPLLoaded=LoadPCX(hp[i].HPLName);
		}
		HeroInfoBackStored=1;
	}else{
		Copy((Byte *)HeroInfoBack,(Byte *)hp,sizeof(HeroInfoBack));
	}
	sp=(_HeroSpec_ *)0x678420;
	for(i=0;i<HERNUM_0;i++){
		HSpecTable[i].Spec=sp[i].Spec;
		HSpecTable[i].Spec1=sp[i].Spec1;
		HSpecTable[i].Spec2=sp[i].Spec2;
		HSpecTable[i].Spec3=sp[i].Spec3;
		HSpecTable[i].Spec4=sp[i].Spec4;
		HSpecTable[i].Spec5=sp[i].Spec5;
		HSpecTable[i].Spec6=sp[i].Spec6;
	}
/*
	for(i=0;i<HERNUM;i++){
		HeroInfoBack[i].HPSName
	}
*/
	for(i=0;;i++){
		ind=HeroSpecWoG[i].Ind;
		if(ind==-1) break;
		wp=&HeroSpecWoG[i];
		wp->SpecPicInd[1]=ind;
//      HeroSpecWoG[i].HPSName[1]=hp[ind].HPSName;
//      HeroSpecWoG[i].HPLName[1]=hp[ind].HPLName;
// Имя пока закоментировано
//      HeroSpecWoG[i].Name[1]=hp[ind].Name;
//      HeroSpecWoG[i].HBios[1]=HBiosTable[i].HBios;
////    wp->HPSLoaded[0]=LoadPCX(wp->HPSName[0]);
////    wp->HPSLoaded[1]=LoadPCX(wp->HPSName[1]);
////    wp->HPLLoaded[0]=LoadPCX(wp->HPLName[0]);
////    wp->HPLLoaded[1]=LoadPCX(wp->HPLName[1]);
		Copy((Byte *)&HSpecTable[ind],(Byte *)&wp->Spec[1],sizeof(_HeroSpec_));
		if(WoG)j=0; else j=1;
////    if((StrCmp(hp[ind].HPSName,wp->HPSName[0]))||
////       (StrCmp(hp[ind].HPSName,wp->HPSName[1]))){
////         tp=wp->HPSName[j]; tp1=tp; hp[ind].HPSName=tp1;
////    }
////    if((StrCmp(hp[ind].HPLName,wp->HPLName[0]))||
////       (StrCmp(hp[ind].HPLName,wp->HPLName[1]))){
////         tp=wp->HPLName[j]; hp[ind].HPLName=tp;
////    }
// Имя пока закоментировано
//    hp[ind].Name=HeroSpecWoG[i].Name[j];
//    HBiosTable[ind].HBios=HeroSpecWoG[i].HBios[j];
		Copy((Byte *)&wp->Spec[j],(Byte *)&HSpecTable[ind],sizeof(_HeroSpec_));
	}
	if(WoG){
/*
		for(i=0;i<HERNUM;i++){
			if(HeroSpecCus[i].Used==0) continue;
			cp=&HeroSpecCus[i];
			if(cp->HPLName!=0){
				hp[i].HPLName=cp->HPLName;
				cp->HPLLoaded=LoadPCX(cp->HPLName);
			}
			if(cp->HPSName!=0){
				hp[i].HPSName=cp->HPSName;
				cp->HPSLoaded=LoadPCX(cp->HPSName);
			}
		}
*/
		RefreshHeroPic();
	}
	RETURNV
}

void MapSetup(void)
{
	int wog;
	__asm pusha
	__asm{
		mov  eax,BASE
		mov  eax,[eax]
		add  eax,0x1F86C
		mov  eax,[eax]
		mov  DDummy,eax
	}
	STARTNA(__LINE__, 0)
	if(DDummy == SOGMAPTYPE || PL_ApplyWoG>1) wog = PL_NewHero; else wog=0;
	if(LoadCustomTxt(1,0)==0) Exit();
	PrepareSpecWoG(wog);
	STOP
	OriginalCallPointer=Callers[16].forig;
	__asm popa;
	__asm call dword ptr [OriginalCallPointer]
}

/////////////////////////////////
/*
void test1(void)
{
	_HeroInfo_ *hp;
	_HeroSpec_ *sp;
	asm{ mov eax,0x67DCE8; mov eax,[eax] } hp=(_HeroInfo_ *)_EAX;
	hp[0].HPSName=hp[3].HPSName;
	hp[0].HPLName=hp[3].HPLName;
	hp[0].Name="Ivan";
//  asm{ mov eax,0x678420 } sp=(_HeroSpec_ *)_EAX;
	sp=HSpecTable; // UN44.DEF, UN32.DEF
	sp[0].Spec=sp[15].Spec;
	sp[0].Spec1=sp[15].Spec1;
	sp[0].Spec2=sp[15].Spec2;
	sp[0].Spec3=sp[15].Spec3;
	sp[0].Spec4=sp[15].Spec4;
	sp[0]._u1=sp[15]._u1;
	sp[0]._u2=sp[15]._u2;
	sp[0].SpShort=sp[15].SpShort;
	sp[0].SpFull=sp[15].SpFull;
	sp[0].SpDescr=sp[15].SpDescr;
}
*/
////////////////////////////
// специализации героя
_Hero_ *hp;
Dword  ebp_;
int    hn;

void _SpecPicture(Dword po)
{
	STARTNA(__LINE__, 0)
	int i,pi,ind,n;
	//if(WoG==0) RETURNV
// edx (hp) -> герой, которому рисуют специализацию
// hp->Number = номер настоящей спец.
// [ebp-10] =
// проверяем на место вызова. EDX = ук на стр. героя или его номер
	if(((Dword)hp)<HERNUM) n=(int)hp; else n=hp->Number;
	for(i=0;;i++){
		ind=HeroSpecWoG[i].Ind;
		if(ind==-1) break;
		if(n==ind){
			pi=HeroSpecWoG[i].SpecPicInd[0];
			__asm{
				mov  eax,pi;
				mov  ebx,po
				mov  [ebx+0x18],eax // номер специализации
			}
		}
	}
	ind=Check4NewSpecPic(n);
	if(ind!=0){
		--ind;
		__asm{
			mov  eax,ind;
			mov  ebx,po
			mov  [ebx+0x18],eax // номер картинки специализации
		}
	}
	RETURNV
}

void __stdcall SpecPicture(Dword po)
{
	__asm pusha
	_EDX(hp);
	_SpecPicture(po);
	OriginalCallPointer=Callers[12].forig;
	__asm popa
	__asm push po
	__asm call dword ptr [OriginalCallPointer]
}

Dword _SpecPictureS(Dword spec)
{
	STARTNA(__LINE__, 0)
	int i,ind;
	//if(WoG==0) RETURN(spec)
// spec = номер настоящей спец.
	ind=Check4NewSpecPic(spec);
	if(ind!=0){
		RETURN(ind-1)// номер картинки специализации
	}
	for(i=0;;i++){
		ind=HeroSpecWoG[i].Ind;
		if(ind==-1) break;
		if((int)spec==ind){
			RETURN(HeroSpecWoG[i].SpecPicInd[0])
		}
	}
	RETURN(spec)
}

void __stdcall SpecPictureS(Dword d11,Dword d10,Dword d9,Dword d8,
												 Dword d7,Dword d6,Dword spec,Dword d4,
												 Dword d3,Dword d2,Dword d1)
{
	__asm pusha
//  hp=(_Hero_ *)_EDX;
	spec=_SpecPictureS(spec);
	OriginalCallPointer=Callers[14].forig;
	__asm popa
	__asm{ 
		push d1
		push d2
		push d3
		push d4
		push spec
		push d6
		push d7
		push d8
		push d9
		push d10
		push d11
		call dword ptr [OriginalCallPointer]
	}
}
/*
void _SpecPictureC(Dword *po)
{
//  if(WoG==0) return;
	int i;
	int Num=po[10];
	asm{
		mov  eax,BASE
		mov  eax,[eax]
		add  eax,0x1F86C
		mov  eax,[eax]
	}
	if(_EAX!=SOGMAPTYPE) return;
	for(i=0;i<Num;i++){
//    po[11+i] = тип героя
		po[11+i]=0;
	}
}

void SpecPictureC(void)
{
	Dword *po=(Dword *)_EAX;
	OriginalCallPointer=Callers[15].forig;
	asm call dword ptr [OriginalCallPointer]
	asm pusha
	_SpecPictureC(po);
	asm popa
}
*/
void _SpecPictureC(char **pname,int *pnum,char ** /*hname*/,Byte **spl)
{
	STARTNA(__LINE__, 0)
	int i,ind,wog,cn;
	// проверка на WoG
	__asm{
		mov  eax,BASE
		mov  eax,[eax]
		add  eax,0x1F86C
		mov  eax,[eax]
		mov  DDummy,eax
	}
	if(DDummy!=SOGMAPTYPE) wog=0; else wog=1;
	cn=*pnum;
	*spl=PicsBack[cn].HPSLoaded;
	for(i=0;;i++){
		ind=HeroSpecWoG[i].Ind;
		if(ind==-1) break;
		if(*pnum==ind){
//      if(HeroSpecWoG[i].HPSLoaded[1]==0){
//        HeroSpecWoG[i].HPSLoaded[1]=*spl;
//      }
			if(wog){
				*pnum=HeroSpecWoG[i].SpecPicInd[0];
				*pname=HeroSpecWoG[i].Spec[0].SpShort;
// Имя пока закоментировано
//        *hname=HeroSpecWoG[i].Name[0];
////        if((StrCmp((char *)&((*spl)[4]),HeroSpecWoG[i].HPSName[0]))||
////           (StrCmp((char *)&((*spl)[4]),HeroSpecWoG[i].HPSName[1])))
////           *spl=HeroSpecWoG[i].HPSLoaded[0];
			}else{
				*pnum=HeroSpecWoG[i].SpecPicInd[1];
				*pname=HeroSpecWoG[i].Spec[1].SpShort;
// Имя пока закоментировано
//        *hname=HeroSpecWoG[i].Name[1];
////        if((StrCmp((char *)&((*spl)[4]),HeroSpecWoG[i].HPSName[0]))||
////           (StrCmp((char *)&((*spl)[4]),HeroSpecWoG[i].HPSName[1])))
////           *spl=HeroSpecWoG[i].HPSLoaded[1];
			}
			break;
		}
	}
	if(wog){
		if((HeroSpecCus[cn].Used)&&(HeroSpecCus[cn].HPSName!=0)){
			 *spl=HeroSpecCus[cn].HPSLoaded;
			 RETURNV
		}
	}
	RETURNV
}

void __stdcall SpecPictureC(Byte *SPicLoaded,char *Name,Dword d3,int SpNum,char *SpName,Dword d0)
{
	__asm pusha
	_SpecPictureC(&SpName,&SpNum,&Name,&SPicLoaded);
	OriginalCallPointer=Callers[15].forig;
	__asm popa
	__asm{
		push   d0
		push   SpName
		push   SpNum
		push   d3
		push   Name
		push   SPicLoaded
		call   dword ptr [OriginalCallPointer]
	}
}

char *_SpecText(char *po)
{
	STARTNA(__LINE__, 0)
	int i,ind;
	//if(WoG==0) RETURN(po)
// edx (hp) -> герой, которому рисуют специализацию
// hp->Number = номер настоящей спец.
// po-> на короткое описание специализации
	for(i=0;;i++){
		ind=HeroSpecWoG[i].Ind;
		if(ind==-1) break;
		if(hp->Number==ind){
			RETURN(HeroSpecWoG[i].Spec[0].SpShort)
		}
	}
	RETURN(po)
}

//void SpecText(char *buf,char *po)
//{
//	__asm pusha
//	_EDX(hp);
//	po=_SpecText(po);
//	OriginalCallPointer=Callers[13].forig;
//	__asm popa
//	__asm push po
//	__asm push buf
//	__asm call dword ptr [OriginalCallPointer]
//	__asm add  esp,+8
//}

static char *nhname;
static Byte *nhspic;
//static int  _nothing[100];
void _NextHeroChoosen(void)
{
	STARTNA(__LINE__, 0)
	int i,ind,wog;
	// проверка на WoG
	__asm{
		mov  eax,BASE
		mov  eax,[eax]
		add  eax,0x1F86C
		mov  eax,[eax]
		mov  DDummy,eax
	}
	if(DDummy!=SOGMAPTYPE) wog=0; else wog=1;
//!!!!!! hn - не номер героя, а номер картинки!!!!!!!!
	if(hn>=HERNUM) RETURNV
//  cn=hn;
// непонятно, почему это?
//  nhspic=PicsBack[cn].HPSLoaded;
	for(i=0;;i++){
		ind=HeroSpecWoG[i].Ind;
		if(ind==-1) break;
		if(hn==ind){
////      if(wog){
// Имя пока закоментировано
//        nhname=HeroSpecWoG[i].Name[0];
////        if((StrCmp((char *)&nhspic[4],HeroSpecWoG[i].HPSName[0]))||
////           (StrCmp((char *)&nhspic[4],HeroSpecWoG[i].HPSName[1])))
////           nhspic=HeroSpecWoG[i].HPSLoaded[0];
////      }else{
// Имя пока закоментировано
//        nhname=HeroSpecWoG[i].Name[1];
////        if((StrCmp((char *)&nhspic[4],HeroSpecWoG[i].HPSName[0]))||
////           (StrCmp((char *)&nhspic[4],HeroSpecWoG[i].HPSName[1])))
////           nhspic=HeroSpecWoG[i].HPSLoaded[1];
////      }
			break;
		}
	}
	if(wog){
		if((HeroSpecCus[hn].Used)&&(HeroSpecCus[hn].HPSName[0])){
			 nhspic=HeroSpecCus[hn].HPSLoaded;
			 RETURNV
		}
	}
	RETURNV
}

void NextHeroChoosen(void)
{
/*
	asm mov  nhname,eax
	asm mov  eax,[eax-9]
	asm mov  hn,eax
	asm mov  eax,[esi+0xC8]
	asm mov  nhspic,eax
	asm pusha
	_NextHeroChoosen();
	asm popa
	asm mov  eax,nhspic
	asm mov  ecx,nhname
*/
	__asm mov  hn,eax
	__asm mov  ecx,[esi+4*eax+0xC8]
	__asm mov  nhspic,ecx
	__asm mov  ecx,[ebp+0x10]
	__asm mov  nhname,ecx
	__asm pusha
	_NextHeroChoosen();
	__asm popa
	__asm mov  ecx,nhname
	__asm mov  [ebp+0x10],ecx
	__asm mov  ecx,nhspic
}

/////////////////////////////////////////
int CalcHideArea(_Hero_ *Hp,int Correct)
{
	STARTNA(__LINE__, 0)
	int l,s,t,r;
	s=Hp->SpPoints;
	l=Hp->PSkill[3]; if(l<2) l=2;
	if(s>(l*10)) t=l; else t=s/10;
	r=0;
	while(t>1){
		t/=2; ++r;
	}
	if(Correct){
		if(s>(l*10)) Hp->SpPoints-=(Word)r;
	}
	if(r<1) r=1;
	RETURN(r)
}

void __stdcall HeroCheck(int NewX,int NewY,int Level,int Owner,int Radius,int Flag)
// у паскалевой процедуры аргументы в обратном порядке
{
	int     hn;
	Dword   basepo,po;
	_Hero_ *Hp;
//  void pascal (*OrFun)(int,int,int,int,int,int);

	_ECX(basepo);
	_ESI(Hp);
//  OrFun=(void pascal (*)(int,int,int,int,int,int))Callers[4].forig;
//  OrFun(NewX,NewY,Level,Owner,Radius,Flag);
// для "ослепленного" героя выход здесь.
	STARTNA(__LINE__, 0)
	//if(WoG){
		if(DoesHeroHas(Hp->Number,CURSE_BLIND)!=-1) RETURNV
		if(DoesHeroHas(Hp->Number,CURSE_NSCUT)!=-1) Radius=2;
	//}
	po=Callers[4].forig;
	__asm{
		push   Flag
		push   Radius
		push   Owner
		push   Level
		push   NewY
		push   NewX
		mov    esi,Hp
		mov    ecx,basepo
		mov    eax,po
		call   eax
	}
	//if(WoG==0) RETURNV
//if(Hp->Number!=0) return;
	hn=Hp->Number;
	// герой двигается - поищем ERM
	HeroMove(Hp,NewX,NewY,Level);
	if(CheckForCreature(Hp,155)==0){ // Darkness Dragon
		if(HSpecTable[hn].Spec!=HSUPERSPEC) RETURNV
		if(HSpecTable[hn].Spec1!=HSPEC_DARKNESS) RETURNV
	}
	HideArea(NewX,NewY,Level,Owner,CalcHideArea(Hp,0));
	RETURNV
}
/*
static int CheckForCreature(_Hero_ *hp,int type)
{
	for(int i=0;i<7;i++){
		if(hp->Ct[i]==type) return 1; // черный дракон
	}
	return 0;
}
*/
void MakeDarkness(int Owner)
{
	STARTNA(__LINE__, 0)
	int    i;
	_Hero_ *hp,*chp;
	//if(WoG==0) RETURNV
	if(Owner==-1) RETURNV
	__asm{
		mov  eax,BASE
		mov  eax,[eax]
		add  eax,0x21620
		mov  hp,eax
	}
	for(i=0;i<HERNUM;i++){
		chp=&hp[i];
		if(chp->Owner!=Owner) continue;
		if(CheckForCreature(chp,155)==0){ // Darkness Dragon
			if(HSpecTable[i].Spec!=HSUPERSPEC) continue;
			if(HSpecTable[i].Spec1!=HSPEC_DARKNESS) continue;
		}  
		HideArea(chp->x,chp->y,chp->l,Owner,CalcHideArea(chp,1));
	}  
	RETURNV
}

static char *StdSpecDef="UN44.DEF";
static char *AddSpec1Def="ZGodBon.DEF";
static char *AddSpec2Def="ZGodBon.DEF";
int NewSpecPrepare(void)
{
	int r;
	STARTNA(__LINE__, 0)
	if(PL_NewHero==0){
		__asm{
			mov    ecx,eax
			mov    eax,StdSpecDef
			push   0x10
			push   0
			push   0
			push   0
			push   0
			push   eax
			push   0x8B // номер
			push   0x2C // dy
			push   0x2C //5A // dx
			push   0xB4 // y
			push   0x41 // x
			mov    eax,0x4EA800
			call   eax
			mov    r, eax
		}
	}else{
		__asm{
			mov    ecx,eax
			mov    eax,AddSpec1Def
			push   0x10
			push   0
			push   0
			push   0
			push   0
			push   eax
			push   0x8B // номер
			push   0x2C//0x2C // dy
			push   0x2C//0x2C //5A // dx
			push   0xB4//0xB4 // y
			push   0x3F//0x41+1//0x41 // x
			mov    eax,0x4EA800
			call   eax
			mov    r, eax
		}
	}  
	RETURN(r)
}

int NewSpecPrepare2(void)
{
	int r;
	STARTNA(__LINE__, 0)
	if(PL_NewHero==0){
		__asm{
			mov    ecx,eax
			mov    eax,StdSpecDef
			push   0x10
			push   0
			push   0
			push   0
			push   0
			push   eax
			push   0x6B // номер
			push   0x2C // dy
			push   0x2C //5A // dx
			push   0xB4 // y
			push   0x70 //0x70 // x
			mov    eax,0x4EA800
			call   eax
			mov    r, eax
		}  
	}else{
		__asm{
			mov    ecx,eax
			mov    eax,AddSpec2Def
			push   0x10
			push   0
			push   0
			push   0
			push   0
			push   eax
			push   0x6B // номер
			push   0x2C // dy
			push   0x2C //5A // dx
			push   0xB4 // y
			push   0x71 // x
			mov    eax,0x4EA800
			call   eax
			mov    r, eax
		}  
	}
	RETURN(r)
}

static _Hero_ *hp_ns;
static Dword  *bf_ns;
static Dword   ebx_ns;
static void _NewSpecShow(int god,int curse)
{
	STARTNA(__LINE__, 0)
	if(PL_NewHero==0){
		__asm{
			mov  ecx,bf_ns
			mov  dword ptr [ecx+4],4
			mov  dword ptr [ecx+8],0x8B
			mov  eax,hp_ns
			mov  eax,[eax+0x1A]
			mov  [ecx+0x18],eax
			mov  ebx,ebx_ns
			push ecx
			mov  ecx,ebx
			mov  eax,0x5FF3A0
			call eax
		}
		__asm{
			mov  ecx,bf_ns
			mov  dword ptr [ecx+4],4
			mov  dword ptr [ecx+8],0x6B
			mov  eax,hp_ns
			mov  eax,[eax+0x1A]
			mov  [ecx+0x18],eax
			mov  ebx,ebx_ns
			push ecx
			mov  ecx,ebx
			mov  eax,0x5FF3A0
			call eax
		}
	}else{ // WoG
		__asm{
			mov  ecx,bf_ns
			mov  dword ptr [ecx+4],4
			mov  dword ptr [ecx+8],0x8B
			mov  eax,god
			mov  [ecx+0x18],eax
			mov  ebx,ebx_ns
			push ecx
			mov  ecx,ebx
			mov  eax,0x5FF3A0
			call eax
		}
		__asm{
			mov  ecx,bf_ns
			mov  dword ptr [ecx+4],4
			mov  dword ptr [ecx+8],0x6B
			mov  eax,curse
			mov  [ecx+0x18],eax
			mov  ebx,ebx_ns
			push ecx
			mov  ecx,ebx
			mov  eax,0x5FF3A0
			call eax
		}
	}  
	RETURNV
}

//static int DoesHeroHas(_Hero_ *hr,int type);
static int FillCurseStruct(_Hero_ *hr);
static char *_GC_Pics[]={ // шаблоны
".\\DATA\\ZVS\\LIB1.RES\\No1.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse1.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse2.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse3.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse4.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse5.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse6.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse7.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse8.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse9.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse10.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse11.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse12.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse13.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse14.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse15.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse16.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse17.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse18.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse19.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse20.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse21.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse22.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse23.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse24.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse25.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse26.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse27.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse28.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse29.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse30.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse31.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse32.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse33.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse34.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse35.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse36.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse37.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse38.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse39.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse40.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse41.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse42.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse43.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse44.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse45.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse46.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse47.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse48.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse49.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse50.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse51.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse52.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse53.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse54.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse55.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse56.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse57.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse58.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse59.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse60.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse61.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse62.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse63.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse64.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse65.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse66.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse67.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse68.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse69.bmp",
".\\DATA\\ZVS\\LIB1.RES\\Curse70.bmp",
};
static char _GC_Length[100][50];
static char *GC_Pics[100];
static char *GC_Descr[100];
static char *GC_Length[100];
// 71 "Will last for another %s turns."
// 72 "Will last forever :-)"
static _CurseShow CurseShow={GC_Pics,GC_Descr,GC_Length};

// 3.58 artifact left
static char _Art_Pics[64][60]; // шаблоны
static char _Art_Ind[64];
static _IconList IconList;
_IconListItem IconListItem[64]; // число арт в рюкзаке макс.

static _ZPrintf_ FHA_tmp;
int ChooseArt(_Hero_ *hp,int Remove)
{
	STARTNA(__LINE__, 0)
	int i,j,art;
	IconList.Items=IconListItem;
	IconList.Caption=ITxt(222,0,&Strings);
	IconList.ItemCount=hp->OANum;
	if(IconList.ItemCount<0) IconList.ItemCount=0;
	if(IconList.ItemCount>63) IconList.ItemCount=63;
	IconList.OnlyOne=1;
	IconList.ShowCancel=1;
	if(hp->OANum==0){ // нет артифактов в рюкзаке
		Message(ITxt(223,0,&Strings),1);
		RETURN(-1)
	}
	for(i=0,j=0;(i<64)&&(j<IconList.ItemCount);i++){
		if(hp->OArt[i][0]==-1) continue; // нет в слоте артика
		_Art_Ind[j]=(char)i;
		if(hp->OArt[i][0]==1){ // scroll
			IconListItem[j].Description=GetSpellName(hp->OArt[i][1]);
			Zsprintf2(&FHA_tmp,"[LODDEF]H3sprite.lod;spellint.def;%i;0;0",(Dword)hp->OArt[i][1]+2,0);
		}else{
			IconListItem[j].Description=ArtTable[hp->OArt[i][0]].Name;
			Zsprintf2(&FHA_tmp,"[LODDEF]H3sprite.lod;Artifact.def;%i;0;0",(Dword)hp->OArt[i][0]+1,0);
		}
		IconListItem[j].Checked=0;
		StrCopy(_Art_Pics[j],59,FHA_tmp.Str);
		IconListItem[j].Path=_Art_Pics[j];
		j++;
	}
//  IconListItem[0].Checked=1;
	ChooseIconDlg(&IconList);
	art=-1;
	for(i=0;i<IconList.ItemCount;i++){
		if(IconListItem[i].Checked){
			j=_Art_Ind[i];
			art=hp->OArt[j][0];
			if(art==1){ // scroll
				art=hp->OArt[j][1]+1000;
			}
			if(Remove){
/*
				for(;i<IconList.ItemCount-1;i++){
					hp->OArt[i][0]=hp->OArt[i+1][0];
					hp->OArt[i][1]=hp->OArt[i+1][1];
				}
*/
				hp->OANum--;
				hp->OArt[j][0]=-1;
				hp->OArt[j][1]=-1;
			}
			break;
		}
	}
	RETURN(art)
}
/////////
void NewSpecShow(void)
{
	_EDX(hp_ns);
	_ECX(bf_ns);
	_EBX(ebx_ns);
	__asm pusha
	STARTNA(__LINE__, 0)
	int gt=DoesHeroGot(hp_ns);
//  int cr=DoesHeroHas(hp_ns->Number,0);
//  if(gt!=0) gt-=GODMONTSTRT; else gt=4;
//  if(cr==-1) cr=6; else cr=5;
	if(gt!=0) gt-=GODMONTSTRT; else gt=6;
//  if(cr==-1) cr=6; else cr=5;
//  cr=5;
	_NewSpecShow(gt,/*cr*/5);
	STOP
	__asm popa
}

//char *NSdescr[2]={"This is a first additional field","This is a second additional field"};
static Dword fn_ns;  // номер поля
static Dword ret_ns;
char *_NewSpecDescr(char *string1,char *string2)
{
	STARTNA(__LINE__, 0)
	char *zret;
	if(PL_NewHero==0) fn_ns=0x76;
	switch(fn_ns){
		case 0x8B: zret=string1;
			break;
		case 0x6B: zret=string2;
			break;
		default:
			__asm{
				mov  ecx,hp_ns 
				mov  eax,[ecx+0x1A]
				lea  edx,[eax+4*eax]
				mov  eax,0x679C80
				mov  eax,[eax]
				mov  eax,[eax+8*edx+0x24]
			}  
			_EAX(zret);
	}
	RETURN(zret)
}

_ZPrintf_ Descr1;
void NewSpecDescr(void)
{
	_EAX(fn_ns);
	_ECX(hp_ns);
	__asm pusha
	STARTNA(__LINE__, 0)
	int gt=DoesHeroGot(hp_ns);
	char *string1;
	if(gt==0){ 
		string1=ITxt(53,0,&Strings);
	}else{
		Zsprintf2(&Descr1,ITxt(54,0,&Strings),(Dword)ITxt(55+gt-GODMONTSTRT,0,&Strings),
							 GetGodBonus(hp_ns->Number,0));
		string1=Descr1.Str;
	}  
	int cr=DoesHeroHas(hp_ns->Number,0);
	char *string2;
	if(cr==-1){ 
		string2=ITxt(70,0,&Strings);
	}else{
/*
		Zsprintf2(&Descr1,ITxt(54,0,&Strings),(Dword)ITxt(55+-GODMONTSTRT,0,&Strings),
							 GetGodBonus(hp_ns->Number,0));
		string2=Descr1.Str;
*/    
//    string2=ITxt(53,0,&Strings);
		string2=" ";
		if(fn_ns==0x6B){
			FillCurseStruct(hp_ns);
			ShowCurse(&CurseShow);
			M_MDisabled=1;
			M_MDisabledNext=0;
		}
	}  
	ret_ns=(Dword)_NewSpecDescr(string1,string2);
	STOP
	__asm popa
	__asm mov  ecx,-1
	__asm mov  eax,ret_ns
}

// Рисует картинку - название специализации
//004E1F2F 8B15708B6900   mov    edx,[00698B70] -> текущий герой
//004E1F35 8B0D809C6700   mov    ecx,[00679C80] -> HeroSpec.txt
//004E1F3B 8B421A         mov    eax,[edx+1A]   = номер (спец) героя
//004E1F3E 8D0480         lea    eax,[eax+4*eax]
//004E1F41 8B54C11C       mov    edx,[ecx+8*eax+1C] -> текст спец. героя
//004E1F45 52             push   edx
//004E1F46 6828746900     push   00697428      -> буфер
//004E1F4B E89C262200     call   007045EC      -> скопировать в буфер
//004E1F50 83C408         add    esp,00000008
//004E1F53 8D45D8         lea    eax,[ebp-28]
//004E1F56 BE03000000     mov    esi,00000003
//004E1F5B8BCB           mov    ecx,ebx
//004E1F5D 50             push   eax
//004E1F5E 8975DC         mov    [ebp-24],esi
//004E1F61 C745E08B000000 mov    dword ptr [ebp-20],0000008B
//004E1F68 C745F028746900 mov    dword ptr [ebp-10],00697428
//004E1F6F E82CD41100     call   HEROES3.005FF3A0

////////////////////////////////////
// Проклятия
#define CURSENUM 1000
//#define GODMONTSTRT   (150+9+1) // после привидения
//#define GODMONTNUM     4        // количество богов
static struct _GodCurse_{
	int  Type;     // тип проклятия, 0=свободно
	int  HeroInd;  // герой-хозяин
	int  StartDay; // день, когда появился у героя
	int  Length;   // длительность действия
	int  CurseVal; // Значение параметра наказания
	int  Backup[2];
} CurseInfo[CURSENUM];

static _ZPrintf_ FCS_tmp;
static int FillCurseStruct(_Hero_ *hr)
{
	STARTNA(__LINE__, 0)
	int i,j,cnum;
	if(hr==0) RETURN(-1)
	for(i=0,j=0;i<CURSENUM;i++){
		if(CurseInfo[i].Type==0) continue;
		if(CurseInfo[i].HeroInd!=hr->Number) continue;
		cnum=CurseInfo[i].Type;
		if(j>=99){ Error(); RETURN(0) }
		GC_Pics[j]=_GC_Pics[cnum];
		if(cnum>40){
			GC_Descr[j]=ITxt(90+cnum,0,&Strings);
		}else{
			GC_Descr[j]=ITxt(80+cnum,0,&Strings);
		}
		if(CurseInfo[i].Length==9999){ // вечно
			StrCopy(_GC_Length[j],50,ITxt(72,0,&Strings));
			GC_Length[j]=_GC_Length[j];
		}else{
			Zsprintf2(&FCS_tmp,ITxt(71,0,&Strings),
				(Dword) CurseInfo[i].CurseVal,
				(Dword)(CurseInfo[i].StartDay+CurseInfo[i].Length-GetCurDate()));
			StrCopy(_GC_Length[j],50,FCS_tmp.Str);
			GC_Length[j]=_GC_Length[j];
		}
		++j;
	}
	GC_Pics[j]=0;
	RETURN(0)
}

// ищет
int DoesHeroHas(int hn,int type)
{
	STARTNA(__LINE__, 0)
	for(int i=0;i<CURSENUM;i++){
		if(CurseInfo[i].Type==0) continue;
		if(type!=0){
			if(CurseInfo[i].Type!=type) continue;
		}
		if(CurseInfo[i].HeroInd==hn) RETURN(i)
	}
	RETURN(-1)
}

int FindFreeCurse()
{
	STARTNA(__LINE__, 0)
	for(int i=0;i<CURSENUM;i++){
		if(CurseInfo[i].Type==0) RETURN(i)
	}
	RETURN(-1)
}

static int DHVC_Table[][3]={
{22,109,-1}, // Water Wheel
{23,17,-1},  // Dwelling
{23,20,-1},  // Dwelling
{23,216,-1}, // Dwelling
{23,217,-1}, // Dwelling
{23,218,-1}, // Dwelling
{24,4,-1},   // Arena
{25,61,-1},  // Axis
{26,32,-1},  // Magic Garden
{27,100,-1}, // Learning Stone
{28,41,-1},  // Libr
{29,23,-1},  // Marletto
{30,51,-1},  // Merc camp A+1
{31,47,-1},  // 
{32,104,-1}, // Univer
{33,107,-1}, //
{34,113,-1}, // Witch Hut
{35,103,-1}, // Cave
{36,101,-1}, // Chest
{37,55,-1},  // Mystic Garden
{38,79,-1},  // All Resources
{39,97,-1},  // 
{40,31,-1},  // 
{41,84,-1},  // Crypt
{42,35,-1},  // Fort on Hill
{43,88,-1},  // Shrine
{43,89,-1},  // Shrine
{43,90,-1},  // Shrine
{44,42,-1},  // Lighthous
{45,13,1},   // Maps
{46,13,2},   // Maps
{47,13,0},   // Maps
{48,7,-1},   // Market
{49,43,-1},  // Teleporter
{49,44,-1},  // Teleporter
{50,57,-1},  // Obelisk
{51,60,-1},  // 
{52,99,-1},  // Post
{53,63,0},   // Piramid
{54,62,-1},  // Prison
{55,58,-1},  // 
{56,80,-1},  // Charch
{57,81,-1},  // Scholar
{58,94,-1},  // Stable
{59,102,-1}, // L.Tree
{60,95,-1},  // Tavern
{61,49,-1},  // Well
{62,111,-1}, // WP
//...
{0,0,0}
};
int DoesHeroHasVisitCurse(int hn, int type,int stype)
{
	STARTNA(__LINE__, 0)
	int cn;
	if((type==63)&&(stype>0)) RETURN(-1) // ERM object cannot be prohibited
	for(int i=0;;i++){
		cn=DHVC_Table[i][0];
		if(cn==0) break;
		if(DHVC_Table[i][1]==type){
			if((DHVC_Table[i][2]==stype)||(DHVC_Table[i][2]==-1)) goto found;
		}
	}
	RETURN(-1) // не нашли такого типа
found:  
	RETURN(DoesHeroHas(hn,cn))
}

char LockGroupSize[14];

void NeedLockGroupSize()
{
	FillMemory(LockGroupSize, sizeof(LockGroupSize), 0);
	for (int i = 0; i < 19; i++)
		LockGroupSize[LockGroups(i)]++;
}

static char* _AddCurse(int hn,int cr,int *val,int ind)
{
	STARTNA(__LINE__, 0)
	int i,j,n;
	_Hero_ *hp=GetHeroStr(hn);
	_GodCurse_ *cu = &CurseInfo[ind];
	NeedLockGroupSize();
	switch(cr){
		case CURSE_BLIND:
			break;
		case CURSE_SLOCK:
			i = *val;
			if (i == -1)
			{
				for (n = -1, j = 0; j <= 8; j++)
					if (hp->LockedSlot[j] < LockGroupSize[j])
						n++;
				if (n < 0)  RETURN("all slots are already locked.")
				n = Random(0,n);
				for (j = 0; j <= 8; j++)
					if (hp->LockedSlot[j] < LockGroupSize[j] && --n < 0)
						break;
				*val = i = j;
			}
			else
			{
				if (hp->LockedSlot[i] >= LockGroupSize[i])  RETURN("the slot is already locked.")
			}
			// now lock it
			n = ++hp->LockedSlot[i];
			for (j = 18; j >= 0; j--)
				if ((LockGroups(j) == i) && (hp->IArt[j][0] == -1) && (--n <= 0))
					break;

			if (j < 0) // need to remove an art
			{
				j = 0;
				while (LockGroups(j) != i || hp->IArt[j][0] == -1)  j++;
				cu->Backup[0] = hp->IArt[j][0];
				cu->Backup[1] = hp->IArt[j][1];
				hp->IArt[j][0] = hp->IArt[j][1] = -1;
			}
			else
			{
				cu->Backup[0] = cu->Backup[1] = -1;
			}
			break;
	}
	RETURN(0)
}

static int _DelCurse(int hn,int cr,int ind)
{
	STARTNA(__LINE__, 0)
	_Hero_     *hp = GetHeroStr(hn);
	_GodCurse_ *cu = &CurseInfo[ind];
	int  val = cu->CurseVal;
	int  i,j;

	cu->Type=0;
	cu->HeroInd=0;
	cu->StartDay=0;
	cu->Length=0;
	cu->CurseVal=0;

	switch(cr){
		case CURSE_BLIND:
			ShowArea(hp->x,hp->y,hp->l,hp->Owner,2);
			RedrawMap();
			break;
		case CURSE_SLOCK:
			i = val;
			--hp->LockedSlot[i];
			if (cu->Backup[0] != -1) // return art
			{
				j = 0;
				while (LockGroups(j) != i || hp->IArt[j][0] != -1)
					if (++j > 18){ MError("Curse error - unable to find place for artifact"); RETURN(0) }
				hp->IArt[j][0] = cu->Backup[0];
				hp->IArt[j][1] = cu->Backup[1];
			}
			break;
	}
	RETURN(0)
}

int AddCurse(int cr,int val,int len,int flag,int hi)
{
	STARTNA(__LINE__, 0)
	int i;
	if(hi<0 || hi>=HERNUM) RETURN(0)
	if(flag==3){ // удалить все
		for(i=0;i<CURSENUM;i++){
			cr=CurseInfo[i].Type;
			if(cr==0) continue;
			if(CurseInfo[i].HeroInd!=hi) continue;
			if(_DelCurse(hi,cr,i)){ Error(); RETURN(-1) }// почему-то не удалилось
		}
		RETURN(0)
	}
	if (flag != 2) { MError("assertion failed in AddCurse"); RETURN(-1) }
	for(i=0;i<CURSENUM;i++){
		if(CurseInfo[i].Type!=cr) continue;
		if(CurseInfo[i].HeroInd!=hi) continue;
		CurseInfo[i].Length+=len;
		CurseInfo[i].CurseVal = max(val, CurseInfo[i].CurseVal);
		RETURN(0)
	}  
	for(i=0;i<CURSENUM;i++){
		if(CurseInfo[i].Type!=0) continue;
		if(_AddCurse(hi,cr,&val,i)) RETURN(0) // почему-то не добавилось
		CurseInfo[i].Type=cr;
		CurseInfo[i].HeroInd=hi;
		CurseInfo[i].StartDay=GetCurDate();
		CurseInfo[i].Length=len;
		CurseInfo[i].CurseVal=val;
		RETURN(0)
	}
	RETURN(-1)
}

int ERM_Curse(Mes &M, int Num, int hn)
{
	STARTNA(__LINE__, 0)
	if(Num<3){ EWrongParamsNum(); RETURN(1) }
	if(Num<4)  M.n[3] = 1;
	if(M.n[3]<0 || M.n[3]>3){ MError2("wrong action kind (0...3)."); RETURN(1) }
	if(M.n[3]==3 && !M.VarI[1].Check && !M.VarI[2].Check)
		RETURN(AddCurse(0, 0, 0, 3, hn));

	int cr = M.n[0];
	int i = DoesHeroHas(hn, cr);
	if (i < 0)
	{
		i = FindFreeCurse();
		if (i < 0) { MError2("too many curses."); RETURN(1) }
	}
	
	int val = CurseInfo[i].CurseVal, len = max(CurseInfo[i].StartDay + CurseInfo[i].Length - GetCurDate(), 0);
	int baseLen = len;
	if (M.n[3] == 2)  // for backward compatibility with possible strange scripts
	{
		if(M.f[1]) val = 0;
		if(M.f[2]) len = 0;
	}
	if (Apply(&val, 4, &M, 1) | Apply(&len, 4, &M, 2))  RETURN(0)  // get syntax
	
	if (M.n[3] == 2)  // add/sub
	{
		len += baseLen;
		val += CurseInfo[i].CurseVal;
	}
	if (M.n[3] == 0 || len <= 0)  // delete
	{
		if ((CurseInfo[i].Length != 0) && _DelCurse(hn, cr, i)) { Error(); RETURN(1) }
		RETURN(0)
	}
	if (cr == CURSE_SLOCK && (val < -1 || val > 13)){ MError2("wrong slot number (-1...13)"); RETURN(1) }
	if (cr == CURSE_SLOCK && val != CurseInfo[i].CurseVal && CurseInfo[i].Length != 0)
		if (_DelCurse(hn, cr, i))
			{	Error(); RETURN(1) }

	if (CurseInfo[i].Length == 0)
	{
		char* err = _AddCurse(hn, cr, &val, i);
		if (err){ MError2(err); RETURN(1) }
	}
	CurseInfo[i].Type = cr;
	CurseInfo[i].HeroInd = hn;
	CurseInfo[i].CurseVal = val;
	CurseInfo[i].Length = len;
	CurseInfo[i].StartDay = GetCurDate();
	RETURN(0)
}

void DaylyCurse(int Owner)
{
	STARTNA(__LINE__, 0)
	int     i,day,cr,val,hn,v;
	_Hero_ *hr;
	day=GetCurDate();
	for(i=0;i<CURSENUM;i++){
		cr=CurseInfo[i].Type;
		if(cr==0) continue;
		val=CurseInfo[i].CurseVal;
		hn=CurseInfo[i].HeroInd;
		hr=GetHeroStr(hn);
		if(hr->Owner!=Owner) continue;
		if((CurseInfo[i].StartDay+CurseInfo[i].Length)<day){ // закончилось
			if(_DelCurse(hn,cr,i)){ Error(); RETURNV }// почему-то не удалилось
			continue;
		}
		switch(cr){
			case CURSE_NMONY : AddRes(Owner,6,-val); break;
			case CURSE_NMANA : v=(int)hr->SpPoints-val; if(v<0) v=0; hr->SpPoints=(Word)v; break;
			case CURSE_PMANA : v=(int)hr->SpPoints+val; if(v>900) v=900; hr->SpPoints=(Word)v; break;
			case CURSE_PEXP  : hr->Exp+=val; AddExp(hr); break;
			case CURSE_SLOW  : hr->Movement-=val; break;
			case CURSE_SPEED : hr->Movement+=val; break;
			case CURSE_PR1345: AddRes(Owner,1,val); AddRes(Owner,3,val); AddRes(Owner,4,val); AddRes(Owner,5,val); break;
			case CURSE_PR02  : AddRes(Owner,0,val); AddRes(Owner,2,val); break;
			case CURSE_PR0   : AddRes(Owner,0,val); break; // дерево
			case CURSE_PR2   : AddRes(Owner,2,val); break; // руда
			case CURSE_PR5   : AddRes(Owner,5,val); break; // Самоцветы
			case CURSE_PR1   : AddRes(Owner,1,val); break; // Ртуть
			case CURSE_PR3   : AddRes(Owner,3,val); break; // Сера
			case CURSE_PR4   : AddRes(Owner,4,val); break; // Кристаллы
			case CURSE_PR6   : AddRes(Owner,6,val); break; // Золото
		}
	}
	// бонусы существ
	for(i=0;i<HERNUM;i++){
		hr=GetHeroStr(i);
		if(hr->Owner!=Owner) continue;
		if(CheckForCreature(hr,151)==1){ // алмазный дракон
			AddRes(Owner,5,1);
		}
		MagicWonder(hr);
	}
	RETURNV
}

////////////////////
int SaveCurse(void)
{
	STARTNA(__LINE__, 0)
	int i;
	if(Saver("LCRS",4)) RETURN(1)
	if(Saver(CurseInfo,sizeof(CurseInfo))) RETURN(1)
/// 24.11.01 3.52
	for(i=0;i<HERNUM;i++){
		HeroSpecCus[i].HPSLoaded=0;
		HeroSpecCus[i].HPLLoaded=0;
	}
///
	if(Saver(HeroSpecCus,sizeof(HeroSpecCus))) RETURN(1)
	RETURN(0)
}

int LoadCurse(int /*ver*/)
{
	STARTNA(__LINE__, 0)
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='L'||buf[1]!='C'||buf[2]!='R'||buf[3]!='S')
			{MError("LoadCurse cannot start loading"); RETURN(1) }
	if(Loader(CurseInfo,sizeof(CurseInfo))) RETURN(1)
	if(Loader(HeroSpecCus,sizeof(HeroSpecCus))) RETURN(1)
	RefreshHeroPic();
	RETURN(0)
}

void ResetCurse(void)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<CURSENUM;i++){
		CurseInfo[i].Type=0;
		CurseInfo[i].HeroInd=0;
		CurseInfo[i].StartDay=0;
		CurseInfo[i].CurseVal=0;
		CurseInfo[i].Length=0;
	}
/*
	_HeroInfo_    *hp=GetHIBase();
	for(i=0;i<HERNUM;i++){
		hp[i].HPLName=HeroInfoBack[i].HPLName;
		hp[i].HPSName=HeroInfoBack[i].HPSName;
	}
*/
// нельзя здесь иниц.
/*
	for(i=0;i<HERNUM;i++){
		HeroSpecCus[i].Used=0;
		for(j=0;j<24;j++){
			HeroSpecCus[i].HPSName[j]=0;
			HeroSpecCus[i].HPLName[j]=0;
		}
		HeroSpecCus[i].HPSLoaded=0;
		HeroSpecCus[i].HPLLoaded=0;
	}
*/
	RETURNV
}

static int COH_Num;
void _CarryOverHero(void)
{
	STARTNA(__LINE__, 0)
	CrossOverNPC(COH_Num);
//  CrossOverGodBonus(COH_Num);
	RETURNV
}
Dword CarryOverHero(void)
{
	__asm pusha
	__asm mov  COH_Num,ecx
	_CarryOverHero();
	__asm popa
	__asm lea eax,[esi+0x1F458];
	__asm mov DDummy,eax
	return DDummy;
}
/*
char *_FixBioBug(int HeroNum)
{
	char *ret=GetHeroStr(HeroNum)->Bibl.m.s;
	if(ret==0){// no bio set
		ret=HBiosTable[HeroNum].HBios;
	}
	return ret;
}

__declspec( naked ) void FixBioBug(void)
{
	__asm{
		pusha
		push    edx
		call    _FixBioBug
		pop     edx
		mov     DDummy,eax
		popa
		mov     ecx,DDummy
		ret
	}
}
*/