#ifndef ___STRUCTS
#define ___STRUCTS

#pragma warning(disable: 4514 4127)
#pragma warning(disable: 4244 4725 4239)
//#define DEBUG // !!! разрешаем отладку
//#define BETAVERSION " BETAVERSION (NOT FOR PUBLIC USE!!!) " 

// 3.58 std definition
#define  MAX_PATH       260

#define WOG_VERSION 359
#define WOG_STRING_VERSION "3 . 59 Alpha 8\nBuilt on " __DATE__/*""BETAVERSION""*/
//#define TE_STRING_VERSION "1 . 03\nBuilt on "__DATE__/*""BETAVERSION""*/
//#define WOG_VERSION_WIDE "\x33\x0\x2E\x0\x35\x0\x38\x0\x66\x0"
//#define WOG_VERSION_WIDE "\x33\x0\x2E\x0\x35\x0\x39\x0\x0\x0"

// !release! ERM_VERSION = 300 + major build version. When scripts API changes a new major version must start.
#define ERM_VERSION 307
#define ERM_STRING_VERSION "307"/*BETAVERSION""*/

#define SAVEWOGLETTER 'S'
#define SAVEWOG359    (SAVEWOGLETTER-'A')
#define SAVEWOG358F   (SAVEWOGLETTER-'A'-1)

#define HEAPSIZE 30000000L // 30Mb

#define SLCH '/'
#define STRCH '^'
#define BASE         0x699538
#define BaseStruct   (*(Byte**)0x699538)
#define combatManager   (*(Byte**)0x699420)
#define MainWindow   (*(HWND*)0x699650)
//#define ARTTABLEADR  0x660B68
#define MEMALLOC     0x617492

#define SOGID 1
#define SODID 0
//#define IDSHIFT 0x15
#define SOGMAPTYPE 0x33
#define SODMAPTYPE 0x1C

#define BANKNUM_0 11   /*11*/
#define BANKNUM  (11+2+4+4)

#define ARTNUM_0  0x91
//#define ARTNUM   (0x91+10+1) // 3.57f
//#define ARTNUM   (0x91+10+1+1+2+2+10) //3.58
#define ARTNUM   255 //3.59

#define SSNUM_0   28
#define SSNUM     28

#define TOWNNUM_0 9
#define TOWNNUM   10

#define RESNUM_0       6    // станд. кол-во ресурсов (6)
#define RESNUM         7    // кол. рес. с Мифрилом
#define MONNUM_0       150  // Do not change!!!
#define MONNUM         500  //(150+9+1+4+4+5+1+9*2+5) Diakon
#define MONNUM_OLD     197  // Diakon
#define GHOSTTYPE     (150+9)   // привидение
#define GHOSTNUM       5        // количество их генерации
#define GHOSTDAYS      6        // день их появления
#define GODMONTSTRT   (150+9+1) // после привидения
#define GODMONTNUM     4        // количество богов

#define HERNUM_0 0x9C // Do not change!!!
#define HERNUM   0x9C //0x9D
//#define HERSETUPNUM   0x9C //0x9D
#define MAGICS   0x57 // 0x53

#define SPELLNUM_0 81 
#define SPELLNUM   200

#define DWNUM_0  0x50 // Do not change!!!
//#define DWNUM    0x60 // 3.57f
#define DWNUM    0x65 // 3.58

#define HSUPERSPEC     8 //5  // Расширенные специальности героев
#define HSPEC_DARKNESS 1 //10 // Ходячая коптилка
#define HSPEC_ANYTOWN  2 // Восстанавливает любой тип города

// проклятия
#define CURSE_BLIND    1 // карта не открывается
#define CURSE_SLOCK    2 // замочек
#define CURSE_NDIPL    3 // нет дипломатии
#define CURSE_NMONY    4 // вычет денег
#define CURSE_PR1345   5 // ресурсы
#define CURSE_NMANA    6 // потеря очков заклинаний 
#define CURSE_PMANA    7 // доп. очки заклинаний 
#define CURSE_NSCUT    8 // уменьшеный радиус видимости
#define CURSE_PEXP     9 // доп. опыт каждый день
#define CURSE_SLOW    10 // замедление
#define CURSE_MAIR    11 // возд. магия
#define CURSE_MEARTH  12 // земл. магия
#define CURSE_MFIRE   13 // огн. магия
#define CURSE_MWATER  14 // вод. магия
#define CURSE_PR0     15 // дерево
#define CURSE_PR2     16 // руда
#define CURSE_PR5     17 // Самоцветы
#define CURSE_PR1     18 // Ртуть
#define CURSE_PR3     19 // Сера
#define CURSE_PR4     20 // Кристаллы
#define CURSE_PR6     21 // Золото

#define CURSE_PR02    64 // ресурсы
#define CURSE_SPEED   65 // ускорение

//#pragma pack(1)
typedef unsigned char  Byte;
typedef unsigned short Word;
typedef unsigned long  Dword;

// VC related
#define pusha pushad
#define popa  popad

#define _EAX(X) __asm mov X,eax
#define _EBX(X) __asm mov X,ebx
#define _ECX(X) __asm mov X,ecx
#define _EDX(X) __asm mov X,edx
#define _ESI(X) __asm mov X,esi
#define _EDI(X) __asm mov X,edi
#define _EBP(X) __asm mov X,ebp

// стандартный формат строки по указателю
struct _Mes_{
	char *s;
	long  l;  
};

// стандартный формат выделенной строки по указателю
struct _AMes_{
	_Mes_ m;
	long  a;
};

struct _FMes_{
	int   Rf;
	_Mes_ Ms;
	int   Sz;
};

enum _ERM_VarTyp_
{
	vtSimple = 0,
	vtFlag = 1,
	vtQuickVar = 2,
	vtV = 3,
	vtW = 4,
	vtX = 5,
	vtY = 6,
	vtZ = 7,
	vtE = 8,
};

/////////////////////////////////
struct VarNum{
	int  Num; // номер флага, переменной  или число
	Byte Type; // тип переменной
// (в случае yv1 это - 6, т.е. y)
// 0=число, 1=флаг, 2=f...t, 3=v1...1000, 4=w1...100, 5=x1...100, 6=y1...100
// 7=z-20...1000+,8=e1...e100
// ******* Not used now: 8=Scope v1...1000, 9=Scope z1...z500
	Byte IType; // тип индиксирующей переменной
// indexed (в случае yv1 это - 3, т.е. v)
// 0=нет, 1=флаг, 2=f...t, 3=v1...1000, 4=w1...100, 5=x1...100, 6=y1...100
	Byte Check; // тип проверки
// 0=nothing, 1?, 2=, 3<>, 4>, 5<, 6>=, 7<=
};

///////////////////////////
struct _ToDo_{
	Word   Type;            // тип (id) ресивера
	Byte   Disabled;        // запрещен приемник
	Byte   DisabledPrev;    // пред состояние запрета
	VarNum Efl[2][16][2];      // до 16 флагов зависимости & и |
	Dword  Pointer;         // указатель на структуру
	VarNum Par[16];         // указатель на структуру свойств этого объекта
	int    ParSet;          // сколько Pointers установлено
	_Mes_  Self;            // HE#:
	_Mes_  Com;             // X#/#/#/#/#
};

struct _Cmd_{
	struct _Cmd_ *Next;  // ук на след стр. (0-конец)
	struct _Cmd_ *ListNext;  // ук на след стр. с подобным id (0-конец)
	Dword  Event;        // id события
	Word   Type;         // номер скопа
	Word   Num;          // кол-во элементов в след структуре
	Word   Scope;        // 3.59 номер скопа/скрипта
	Byte   Disabled;     // запрещен генератор
	Byte   DisabledPrev; // пред состояние запрета
	VarNum Efl[2][16][2];      // до 16 флагов зависимости & и |
	struct _ToDo_ ToDo[1];
};

struct Mes{
	long  i;    // первым для быстрого доступа
	_Mes_ m;
//  VarNum Efl [16][2];
//  VarNum Ofl [16][2];      // до 16 флагов зависимости |
	VarNum Efl[2][16][2];      // до 16 флагов зависимости & и |
	VarNum VarI[16]; // сами параметры
	char  c[16]; // вроде, используется только при парсинге
	char  f[16]; // 1, если использован d синтаксис
	int   n[16];
	// строки ^^ обрабатываются не так как все. Они всегда стоят в конце и берутся по m.c[i] и далее
};

struct _ZPrintf_{
	int  _u1;
	char *Str;
	int   Len;
	int   Len2;
};

struct _VidInfo_{
	char *Name;
	Dword Po;
	Byte  Atr[4];
	Dword Add[2];
};
/////////////////////////////
// scope
struct _Scope_{
	int     Number; // номер скопа
 _Scope_ *Next; // ук на след
	int     Var[1000];
	char    String[500][512];
};

// вспомагательная структура
struct _MonArr_{
	int    Ct[7];    //   +0   dd*7  = тип существ (-1 - нет)
	int    Cn[7];    //   +1C  dd*7  = количество
};
//
class _ArtList_{
	Dword  _u1;
	Dword  *Start;
	Dword  *Stop;
	Dword  *All;
	static Dword  Ar[100];
	static Dword *ArL[4];
	int  Down(void){ int i; Dword *po; for(i=0,po=Start;po<Stop;po++,i++){ Ar[i]=*po; } return i; }
	void Up(int Num){ 
		ArL[0]=(Dword *)1; ArL[1]=Ar; ArL[2]=ArL[3]=&Ar[Num]; Dword localword=(Dword)ArL;
		__asm{
			mov   eax,localword
			mov   ecx,this
			push  eax
			mov   eax,0x4D44B0
			call  eax
		}
	}
public:
	int  GetNum(void){  return(Stop-Start); }
	int  Get(int Ind){ if(Ind<0) return -1; if(Ind>=GetNum()) return -1; return Start[Ind]; }
	void Set(int Ind,int Art){ if(Ind<0) return; if(Ind>=GetNum()) return; Start[Ind]=Art; }
	int  Add(int Art){ int n=Down(); Ar[n]=Art; Up(n+1); return n; }
	void Del(int Ind){ if(Ind<0) return; int n=Down(); for(int i=Ind+1;i<n;i++) Ar[i-1]=Ar[i]; Up(n-1); }
};

/////////////////////////////
// ресурсы, герои и все остальное у игрока
//   +020AD0  8 структур по игрокам, размер 168h   l 0x4BD898
struct _PlayerSetup_{
	char  Owner;     //  +0 db = номер цвера игрока
	char  HasHeroes; //  +1 db = количество героев у игрока
	char _u1[2];
	int   CurHero;   //  +4 dd = текущий активный герой (-1 - нет)
	int   Heroes[8]; //  +8 dd*8 = номера игроков у героя (по количеству, но не >8)
	int   THeroL;    //  +28 = герой для найма в таверне
	int   THeroR;    //  +2C = герой для найма в таверне
	char _u2[0x0D];  //  +30
	char  DaysLeft;  //  +3D db = сколько дней до убивания героя (может быть >7)
									 //      если не FF, то всегда выдает сообщение
	char  HasTowns;  //  +3E db = количество городов у игрока
	char  CurTown;   //  +3F db = текущий активный город игрока (номерб -1 - нет)
	char  Towns[48]; //  +40 db*48 = номера всех городов у игрока
	char _u3[4];     //  +70
	int   TopHeroInd; // (+64???) +74 = индекс среди 8 героев, который верхний
	char _u4[0xB0];  // +78
	// +9C = ресурсы
	// +B8 dd = посещенные магические сады
	// +BC dd = посещение магического ручья
	// +C8 dd = номер в игре сетевого игрока ??? (вышел)
	// +E1 db = 1-GM / 0-AI (тот, что сидит за экраном)
	// +E2 db = 1-GM / 0-AI (главный)
	// +E4 dw = MP на этом PC ? (номер игрока как игрока на карте???)
	// +E8 dd = 32 бита - один на Комбо Артифакт, что этот игрок имеет
	double ResImportance[7]; // +128 double*7 = важность ресурса для AI


// перерисовка героев в окне справа
//00403267 6A01           push   00000001
//00403269 6A01           push   00000001
//0040326B 6AFF           push   FFFFFFFF
//0040326D 8BCE           mov    ecx,esi ->  _PlayerSetup_ *
//0040326F E86C000000     call   H3WOG.004032E0
//  int   TopTownInd; // +68 = индекс среди городов, который верхний
// перерисовка городов в окне справа
//004032C7 6A01           push   00000001
//004032C9 6A01           push   00000001
//004032CB 6AFF           push   FFFFFFFF
//004032CD 8BCE           mov    ecx,esi ->  _PlayerSetup_ *
//004032CF E84C010000     call   H3WOG.00403420

//       +84 dd = ?
//       +B8 dd = посещенные магические сады
//       +BC dd = посещение магического ручья
//       +СС
//       +E1 db = 1-GM / 0-AI (?)
//       +E2 db = 1-GM / 0-AI (главный)
//       120h*8 - описания всех игроков (8)
//   +020B6C dd - дерево
//   +020B70 dd - сера       
//   +020B74 dd - камни
//   +020B78 dd - ртуть       
//   +020B7C dd - драг. камни       
//   +020B80 dd - кристаллы       
//   +020B84 dd - золото
//   +020BB2 db = GAMER/ai (?)
};

struct _BlackMarketInfo_{
	Dword art[7];
}; // size = 0x1C


/////////////////////////////
//    [P1+4] - начальный адрес типов объектов карты (длина 44h)
//    [P1+8] - конечный адрес типов объектов карты
// Формат:
//     +4  [dd] -> char * имя DEF файла
//     +8  [dd] = длина имени DEF файла
//     +10 [db] = ?{2} dx
//     +11 [db] = ?{1} dy
//     +14 [dd*2] = ? 1 вит на элемент, всего 30h элементов
//     +1C [dd*2] = ? 1 вит на элемент, всего 30h элементов (6 байт после DEF в карте(1))
//     +24 [dd*2] = ? 1 вит на элемент, всего 30h элементов
//     +2C [dd*2] = ? 1 вит на элемент, всего 30h элементов (след. 6 байт в карте(2))
//     +38 [dd] = # типа объекта {1A-event} (4 байта из карты(4))
//     +3C [dd] = ID объекта (4 байта из карты(5)) - подтип (тип замка,№ артифакта ...)
//     +40 [db] = ?{00} (1 байта из карты(6))
// структура в карте:
// +0 dd-длина DEF,DEF имя,6б(1),6б(2),2б(-),2б(-),4б(4),4б(5),1б(-),1б(6),10hб(-)
// *[65F458]+8+(4)*16 -> [+38] # типа объекта (коррекция старых)
struct _Types_{
	Dword  _u1;      //+0
	char   *defname; //+4
	long    deflen;  //+8
	Dword  _u2;      //+C
	Byte    Width,Height,_u5,_u6; //+10
	Byte    MaskObject[8]; //+14
	Byte    MaskShadow[8]; //+1C
	Byte    MaskEmpty[8]; //+24
	Byte    EnterMask[8]; //+2C
	Dword  _u7;
	long    type;
	long    subtype; 
	Byte    Flat,_u10,_u11,_u12;
};

//    [P1+24] - начальный адрес указатели на имя DEF файла для типа (длина 4h)
//    [P1+28] - конечный адрес
// Формат:

//    [P1+14] - начальный адрес положений объектов карты (длина 0Ch)
//    [P1+18] - конечный адрес положений объектов карты
// Формат:
//    +0  [db] - n порядковый номер среди таких же объектов FF-порядок нормирован 
//    +1  [db] - бу1
//    +2  [db] - бу2
//    +3  [db] - бу3
//        +0 dd - номер подтипа героя (для героев)
//        +0 dd - номер сира (для сиров)
//    +4  [db] - x
//    +5  [db] - y
//    +6  [db] - level
//    +7  [db] - ?
//    +8  [dw] - номер типа по порядку загрузки в таблицу типов
//    +A  [db] - ?
// структура в карте:
// 1(x),1(y),1(l),2(t),2(-),5(-)
// 504D04 - вычисляет +A [db]
struct _Position_{
//  Byte  ser,c1,c2,_u2;
	Dword SetUp;
	Byte  x,y,l,_v1;
	Word  num; // номер дефа
	Byte  t,_v2;
};
// первые 4 байта для монстров
struct _CMonster_{
	unsigned  Number    :12; // +00 число
	unsigned  Agression : 5; // +0C 1C-присоединяется, 0A-кровожадное
//  unsigned  NoMore    : 1; // +11 не растет
//  unsigned  NoRun     : 1; // +12 не убегает
	unsigned  NoRun     : 1; // +12 не убегает 3.58 fix
	unsigned  NoMore    : 1; // +11 не растет
	unsigned  SetUpInd  : 8; // +13 номер в настройке объектов (P1+44)
	unsigned _u1        : 4; // +1B
	unsigned  HasSetUp  : 1; // +1F имеет настройку структуры монстра
}; 
// первые 4 байта для учителя
struct _CSchoolar_{ // type 0x51
	unsigned  Type  : 3; // чему учит (0-перв.ум.,1-втор.ум.,2-заклин)
	unsigned  PSkill: 3;
	unsigned  SSkill: 7;
	unsigned  Spell :10;
	unsigned _u1    : 9;
};
// первые 4 байта для LE
struct _EventPos_{
	unsigned  Number   : 10; // номер по порядку загрузки
	unsigned  Enabled4 :  8; // кому можно активировать
	unsigned  AIEnable :  1; // можно ли AI
	unsigned  OneVisit :  1; // отменить после первого посещения
	unsigned _u1       : 12; // ?
};
// первые 4 байта для сундука
struct _CChest_{  // type 101 (дес)
	unsigned  ArtNum : 10;  // номер артифакта
	unsigned  HasArt :  1;  // артифакт (1) или бонус (0)
	unsigned  Bonus  :  4;  // бонус золота * 500 (опыт * 500 - 500)
	unsigned _u1     : 17;
};
// первые 4 байта для ресурсов
struct _CRes_{
	unsigned Value    : 19;  // количество ресурся
	unsigned SetUpNum : 12;  // номер в структуре _ArtRes_
	unsigned HasSetUp :  1;  // есть настройка
};
// первые 4 байта для могилы воина
struct _CWTomb_{ // type 0x6C
	unsigned  HasArt :  1;  // арт. еще есть
	unsigned _u1     :  4;
	unsigned  Whom   :  8;  // кто уже посетил
	unsigned  ArtNum : 10;  // номер арт
	unsigned _u2     :  9;
};
// первые 4 байта для дерева знаний
struct _CKTree_{ // type 0x66
	unsigned  Number :  5;  // номер, всего 32
	unsigned  Whom   :  8;  // кто уже посетил
	unsigned  Type   :  2;  // 0,1,2
	unsigned _u1     : 17;
};
// первые 4 байта для костра
struct _CFire_{ // type 0x0C
	unsigned  ResType :  4;
	unsigned  ResVal  : 28;
};
// первые 4 байта для норы
struct _CLean_{ // type 0x27
	unsigned  Number  :  5; // =0 - пуст
	unsigned _u1      :  1;
	unsigned  ResVal  :  4;
	unsigned  ResType :  4;
	unsigned _u2      : 18;
};
// первые 4 байта для хижины ведьмы
struct _CWHat_{ // type 0x71
	unsigned _u1      :  5;
	signed    Whom    :  8; // кто посетил
	signed    SSkill  :  7; // номер втор. умения
	unsigned _u2      : 12;
};
// первые 4 байта для обучающего камня
struct _CLStone_{ // type 0x64
	int Number;     // номер камня
};
// первые 4 байта для сада откровения
struct _CPluss_{ // type 0x20 ...
};
// NO первые 4 байта для конюшни type 0x5E 
// NO первые 4 байта для фонтана молодости type 0x1F - настройки нет 4C1E2F
// NO первые 4 байта для домика фей type 0x1C 
// NO первые 4 байта для фонтана удачи type 0x1E
// NO первые 4 байта для оазиса type 0x38
// первые 4 байта для телеги
struct _CWagon_{ // type 0x69
	unsigned  ResVal  :  5; // кол-во ресурса
	signed    Whom    :  8; // кто посетил
	unsigned  HasBon  :  1; // есть бонус
	unsigned  HasArt  :  1; // есть арт.(1) или рес. (0)
	unsigned  ArtNum  : 10; // номер арт
	unsigned  ResType :  4; // +19h
	unsigned _u3      :  3; 
};
// первые 4 байта для скелета
struct _CSkelet_{ // type 0x16
	unsigned  Number :  5; // номер
	unsigned _u1     :  1;
	unsigned  ArtNum : 10; // номер арт.
	unsigned  HasArt :  1; // есть арт.
	unsigned _u2     : 15; 
};
// первые 4 байта для магического ручья 
// НАСТРАИВАЮТСЯ 2 клетки
struct _CMSpring_{ // type 0x30
	unsigned  Number :  5; // номер
	unsigned _u1     :  1;
	unsigned  Power  :  1; // не посещен - заряжен
	unsigned _u2     : 25;
};
// первые 4 байта для водяной мельницы
struct _CWMill_{ // type 0x6D
	unsigned  Bonus   :  5; // кол-во золота (будет *500)
	unsigned  Whom    :  8;
	unsigned _u1      : 19;
};
// первые 4 байта для CrBank
struct _CCrBank_{ // type 16
	unsigned _u1      :  5; //
	unsigned  Whom    :  8; // кто из игроков хоть когда заходил
	unsigned  Ind     : 12; // индекс по порядку структур
	unsigned  Taken   :  1; // взят (1) или еще нет (0)
	unsigned _u2      :  6;
};
// первые 4 байта для пирамиды 
struct _CPyram_{ // type 0x3F (тип 0)
	unsigned  Visited :  1; // 1-не посещено
	unsigned  Number  :  4; // ???
	unsigned  Whom    :  8; // кем посещено
	unsigned  Spell   :  8; // номер заклинания
	unsigned _u1      : 11;
};

// первые 4 байта для лебединого озера
struct _CSSwan_{ // type 0xE
	unsigned  Number  :  5;
	unsigned  Whom    :  8; //
	unsigned  BonLuck :  4; // 1...3
	unsigned _u3      : 15; 
};
// первые 4 байта для монолита
struct _CMonol_{ // type 0x39 
	int  Number;   // номер по-порядку
};
// первые 4 байта для склепа сложно - настройка в 4C0F16
//struct _CCrypt_{ // type 0x54 
//  Word _u1      :  5;
//  Word _u2      :  8;
//};
// первые 4 байта для магического сада - настройка в 4C
struct _CMGarden_{ // type 0x37 
	unsigned  Number  :  5;
	unsigned _u1      :  1;
	unsigned  ResType :  4; // всегда = 5
	unsigned  HasRes  :  1; // =1 - есть ресурс
	unsigned _u2      : 21;
};
// первые 4 байта для ветряной мельницы
struct _CMill_{ // type 0x70 ...
	unsigned  ResType :  4;
	unsigned _u1      :  9;
	unsigned  ResVal  :  4; // =0 - посещено и изъято
	unsigned _u2      : 15; 
};
// первые 4 байта для шахт
struct _CMine_{
	int   Num;      // номер по порядку
};  
// первые 4 байта верфи
struct _CShipyard_{
	signed    Owner   :  8; // хозяин   
	unsigned  xs      :  8; //
	unsigned  ys      :  8; //
	unsigned _u3      :  8; //
};  
// первые 4 байта для алтарей мысли (учить закл)
struct _CShrine_{
	unsigned _u1      : 13;
	unsigned  Spell   : 10; // номер заклинания
	unsigned _u2      :  9;
};  
// первые 4 байта для университета (учить закл)
struct _CUniver_{
	unsigned _u1      :  5;
	unsigned  Whom    :  8; // кем посещено?
	unsigned  Num     : 12; // номер универа
	unsigned _u2      :  7;
};  

// Описатели знаков и бутылок
// [BASE+4E37C] -> размер 0x14, номер = настройка в _Position_
struct _Sign_{
	char  HasMess;
	Byte _u1[3];
	Dword _u2;
	_AMes_  Mes;
};

// Описатели шахт
// [BASE+4E38C] -> размер 0x40, номер = настройка в _Position_
struct _Mine_{
	char  Owner;    // хозяин -1 = ничья 
	char  ResNum;   // номер ресурса (0...6) или 100 для маяка (!)
	Byte _u1[2];
	int   GType[7]; // тип монстров охраны
	int   GNumb[7]; // число монстров охраны
	Byte  xe;       // координаты входа
	Byte  ye;
	Byte  le;
	Byte _u2;
};

// Гарнизон
struct _Horn_{
	int  Owner;
//  Byte _u1[3];
	int   GType[7]; // тип монстров охраны
	int   GNumb[7]; // число монстров охраны
	char  NotRem;   // неудаляемый
	Byte  ex;
	Byte  ey;
	Byte  el; // ????
};

// Университет
struct _Univer_{
	int  SSkill[4]; // новера вторых скилов
};

// Event, Pandora Box:
//    [P1+54] - начальный адрес настройки объектов карты (длина E4h)
//    [P1+58] - конечный адрес 
// Формат:
//    +0  [dd] - ? 
//    +4  [dd] -> сообщение 
//    +8  [dd] = длина сообщения
//    +C  [dd] = ? {1F}
//    +10 [dd] = флаг охрана есть или нет
//    +14 [dd*7] = тип охраны
//    +30 [dd*7] = количество
//    +50 [dd] = опыт
//    +54 [dd] = баллы закл.
//    +58 [db] = мораль
//    +59 [db] = удача
//    +5C [dd*7] = ресурсы
//    +78 [db*4] = первичные навыки
//    +80 [dd] -> начало таблицы втор. умений (dd номер + dd развитость)
//    +84 [dd] -> конец  таблицы втор. умений
//    +90 [dd] -> начало таблицы артефактов (dd номер)
//    +94 [dd] -> конец  таблицы артефиктов
//    +A0 [dd] -> начало таблицы заклинаний (dd номер)
//    +A4 [dd] -> конец  таблицы заклинаний
//    +AC [dd*7] = тип существ (-1 - нет)
//    +C8 [dd*7] = количество существ
//    +E4 конец
// структура в карте:
struct _Event_{
	Dword _u1;
	_AMes_  Mes;
	int    fl_G;
//  int    Gt[7];
//  int    Gn[7];
	_MonArr_ Guard;
	Dword  fl_O;
	Dword  Exp;
	long   SpPow;
	char   Moral;
	char   Luck;
	Word  _n1;
	long   Res[7];
	Byte   First[4];
	Dword _u4;
	Dword (*Second)[2];
	Dword (*SecondE)[2];
	Dword _u5;
//  Dword _u6;
//  Dword *Artif;
//  Dword *ArtifE;
//  Dword _u7;
	_ArtList_ Arts;
	Dword _u8;
	Dword *Spell;
	Dword *SpellE;
	Dword _u9;
//  int    Ct[7];
//  int    Cn[7];
	_MonArr_ Mon;
};

// Artefacts & Resources:
//    [P1+34] - начальный адрес настройки объектов карты (длина 4Ch)
//    [P1+38] - конечный адрес
// Формат:
//    +0  [dd] - ?
//    +4  [dd] -> сообщение
//    +8  [dd] = длина сообщения
//    +C  [dd] = ? {1F}
//    +10 [dd] = есть охрана/нет
//    +14 [dd*7] = тип охраны
//    +30 [dd*7] = количество
struct _ArtRes_{
	Dword _u1;
	_AMes_  Mes;
	char   fl_G;
	Byte  _u2[3];
	int    Gt[7];
	int    Gn[7];
};

// Monsters:
//    [P1+44] - начальный адрес настройки объектов карты (длина 30h)
//    [P1+48] - конечный адрес 
// Формат:
//    +0  [dd] - есть настройка/нет
//    +4  [dd] -> сообщение 
//    +8  [dd] = длина сообщения
//    +C  [dd] = ? {1F}
//    +10 [dd*7] = ресурсы
//    +2C [dd] = артифакт (-1 - нет)
struct _Monster_{
	int    Set;
	_AMes_ Mes;
	int    Res[7];
	int    Artefact;
};

// P2=[P1+84]; P3=[P2+34{размер события}*N{номер события}
// P4=[P1+88]; конечный адрес для  P3; N=(P4-P2)/34
// P3: +4 [dd] -> сообщение
//     +8 [dd] =  длина сообщения
//     +C [dd] =  ??
//     +10[dd*7] = кол-во ресурсов дать/взять
//     +2C[db] = флаги, кто может активировать событие (8 бит по цветам)
//     +2d[db] = применять ли к человеческим игрокам (<>0)
//     +2E[db] = применять ли к комп. игрокам (<>0)
//     +30[dw] = первый день появления +1 (считая с нуля)
//     +32[dw] = повтор через столько дней (0-не повторять)
// хранятся в произвольном порядке для разных дней,
// но последовательно для одного дня.
struct _GlbEvent_{
	Dword _u1;
	_AMes_  Mes;
	long   Res[7];
	Byte   Colors;
	Byte   HEnable;
	Byte   AIEnable;
	Byte  _u3;
	Word   FirstDate;
	Word   Repeater;
};

// Events of Castles:
//    [P1+94] - начальный адрес настройки объектов карты (длина 50h)
//    [P1+98] - конечный адрес 
// Формат:
//    +0  [dd] - ? 
//    +4  [dd] -> текст события
//    +8  [dd] =  длина имени
//    +C  [dd] = ? {1F}
//    +10 [dd*7] = дать/взять ресурсы
//    +2C[db] = флаги, кто может активировать событие (8 бит по цветам)
//    +2d[db] = применять ли к человеческим игрокам (<>0)
//    +2E[db] = применять ли к комп. игрокам (<>0)
//    +30[dw] = первый день появления +1 (считая с нуля)
//    +32[dw] = повтор через столько дней (0-не повторять)
//    +34[dd] = # Castla при загрузке типов
//    +38[db*6] = бит - здание (построить)
//    +40[dw*7] = дать существ соотв. уровня
//    +4E[dw] = ?
struct _CastleEvent_{
	Dword _u1;
	_AMes_  Mes;
	long   Res[7];
	Byte   Colors;
	Byte   HEnable;
	Byte   AIEnable;
	Byte  _u3;
	Word   FirstDate;
	Word   Repeater;
	Dword  CastleNum;
	Byte   BuildIt[6],_u5[2];
	Word   Cr[7],_u6;
};
// Seer:
//    [P1+64/(B4)] - начальный адрес настройки объектов карты (длина 13h/(4))
//    [P1+68/(B8)] - конечный адрес 
// 574780 грузит часть что искать
//    +0   dd ->структура чего принести
//    +5   db(d) = тип чего дадим
//    +9   db(d) = ?
//    +D   dw = количество опыта (тип A)
// структура чего ищем
//    +0   dd
//    +4   dd
//    +C   dd -> сообщение1
//    +10  dd =  длина сообщ1
//    +14  dd = ? (1F)
//    +18  dd
//    +1C  dd -> сообщение2
//    +20  dd =  длина сообщ2
//    +24  dd = ? (1F)
//    +28  dd
//    +2C  dd -> сообщение3
//    +30  dd =  длина сообщ3
//    +34  dd = ? (1F)
//    +40  dd = Id кого ищем

//   ADR0+000A4 db*334*N - описание всех типов героев
//   +0   db   = хозяин (цвет) FF - ничей
//   +4   dd   = номер подтипа (конкретный герой)
//   +8   dd   = Id 
//   +C   db   = 1-есть имя
//   +D   db*D = имя,0
//   +1A  db   = 1-есть опыт
//   +1C  dd   = опыт
//   +20  db   = 1-есть картинка
//   +21  db   = номер картинки
//   +22  db   = 1-есть 2-е скилы
//   +24  dd   = кол. вторых скилов
//   +28  dd*8 = номера вторых скилов
//   +30  db*8 = уровни вторых скилов
//   +38  db   = 1-есть существа
//   +3C  dd*7 = типы существ
//   +58  dw*7 = кол-во существ
//   +66  db   = группа/разброс
//   +67  db   = 1-есть артифакты
//   +68  dd*2*13 = артифакты dd-номер,dd-(FF) +E8 -книга(3,FF)
//   +100 dd*2*40 = арт в рюкзаке dd-номер, dd-(FF)
//   +300 db   = число артифактов в рюкзаке
//   +301 dw*2 = начальная позиция на карте
//   +305 db   = радиус обегания
//   +306 db   = 1-есть биография
//   +308 dd   = 1-выделена память под биографию
//   +30C dd   -> ук. на биографию
//   +310 dd   = длина биографии
//   +314 dd   = ?(1F)
//   +318 dd   = 0-м,1-ж,FF-умолчание
//   +31C db   = 1-есть заклинания
//   +320 db*A = заклинания
//   +32C db   = 1-есть первичные умения
//   +32D db*4 = 4-ре первичных умения
struct _THero_{
	char    Owner;   //! = хозяин (цвет) FF - ничей
	Byte   _u1[3];
	Dword   Number;  // = номер подтипа (конкретный герой)
	Dword   Id;      // = Id
	Byte    fl_N;    // = 1-есть имя
	char    Name[13];//! = имя,0
	Byte    fl_E;    // = 1-есть опыт
	Byte   _u2;
	int     Exp;     //   +1C  dd   = опыт
	Byte    fl_P;    //   +20  db   = 1-есть картинка
	Byte    Pic;     //   +21  db   = номер картинки
	Byte    fl_SS;   //   +22  db   = 1-есть 2-е скилы
	Byte   _u3;
	Dword   SSNum;   //!   +24  dd   = кол. вторых скилов
	Byte    SSkill[8];//  +28  dd*8 = номера вторых скилов
	Byte    SSLev[8];//   +30  db*8 = уровни вторых скилов
	Byte    fl_C;    //   +38  db   = 1-есть существа
	Byte   _u4[3];
	Dword   CType[7];//   +3C  dd*7 = типы существ (-1)
	Word    CNum[7]; //   +58  dw*7 = кол-во существ
	Byte    Group;   //   +66  db   = группа/разброс
	Byte    fl_A;    //   +67  db   = 1-есть артифакты
	Dword   IArt[19][2];//+68  dd*2*13 = артифакты dd-номер,dd-(FF) +E8 -книга(3,FF)
	Dword   OArt[64][2];//+100 dd*2*40 = арт в рюкзаке dd-номер, dd-(FF)
	Byte    OANum;   //   +300 db   = число артифактов в рюкзаке
	Word    x0,y0;   //   +301 2*dw = нач. позиция на карте
	Byte    Run;     //!   +305 db   = радиус обегания
	Byte    fl_B;    //   +306 db   = 1-есть биография
	Byte   _u6;
	Dword  _u7;      //   +308 dd   = 1-выделена память под биографию
	char   *Bibl;    //   +30C dd   -> ук. на биографию
	long    BiblLen; //   +310 dd   = длина биографии
	Dword  _u8;      //   +314 dd   = ?(1F)
	Byte    Sex;     //   +318 dd   = 0-м,1-ж,FF-умолчание
	Byte   _u9[3];
	Byte    fl_SP;   //   +31C db   = 1-есть заклинания
	Byte   _u10[3];
	Byte    Spell[10];//  +320 db*A = заклинания
	Word   _u11;
	Byte    fl_PS;   //   +32C db   = 1-есть первичные умения
	char    PSkill[4];//  +32D db*4 = 4-ре первичных умения
	Byte   _u12[3];
};

//   ADR0+21620 db*492h*N - описания конкретных героев (формируется 4CACAE)
//   +00  dw    = x position
//   +02  dw    = y position
//   +04  dw    = ? старшая часть y (y<<2>>C)
//   +18  dw    = ? баллы заклинаний
//   +1A  dd    = номер подтипа 0...9B - задает специализацию (может меняться)
//   +1E  dd    = Id
//   +22  db    = нозяин (цвет)
//   +23  db*D  = имя,0
//   +30  dd    = str[8] str=(*[67CD08])[номер подтипа *5C]
//   +34  db    = номер картинки
//   +44  db    = базовый x для обегания (FF-не ограничен)
//   +45  db    = базовый y для обегания (FF-не ограничен)
//   +46  db    = радиус обегания (FF-не ограничен)
//   +49  dd    = опыт до след уровня
//   +4D  dd    = опыт до след уровня
//   +51  dd    = модеф. опыт
//   +55  dd    = ? сила закл=уровень
//   +91  dd*7  = тип существ (-1 - нет)
//   +AD  dd*7  = количество
//   +C9  db*1C = уровень 2-х скилов (один байт - уровень этого номера скила 1,2,3) 0-нет
//   +E5  db*1C = порядок отображения 2-х скилов в окне героя (1,2,3,4,5,6)
//   +101 dd    = количество 2-х скилов
//   +12D dd*2*13 = артифакты dd-номер,dd-(FF)  -книга(3,FF)
//   +1D4 dd*2*40 = арт в рюкзаке dd-номер, dd-(FF)
//   +3D5 dd    = пол
//   +3D9 db    = есть биография
//   +3DA dd    -> биография
//   +3EA db*46 = заклинание (есть/нет)
//   +430 db*46 = уровень заклинания (>=1)
//   +476 db*4  = первичные навыки
struct _Hero_{
	Word   x;        //   +00  dw    = x position
	Word   y;        //   +02  dw    = y position
	Word   l;        //   +04  dw    = ? уровень старшая часть y (y<<2>>C)
	Byte   Visible;  //   +06  db    = 1 - есть на карте (внутри города или не активен)
//  Byte  _u1[17];
// +07 db - x
// +08 db - (?) мусор (к x)
// +09 db - y
// +0A db - l(?) мусор (к y)
		Dword  PlMapItem; // MixedPos
// +0B db - (?) l
		Byte  _u1;
// +0C dd - тип объекта на котором герой стоял
		long   PlOType; // dd +1E с карты
// +10 db - бит занятости во флагах поверхности 00001000
// это бит означающий, что здесь есть/была точка входа (желтая клетка)
		Dword  Pl0Cflag;
// +14 dd - SetUp с карты
		Dword  PlSetUp; // dd +0 с карты
	Word   SpPoints; //   +18  dw    = баллы заклинаний
	int    Number;   //   +1A  dd    = номер подтипа (конкретный герой)
	Dword  _unk_Id;       //   +1E  dd    = Id (при чём тут Id???)
	char   Owner;    //   +22  db    = нозяин (цвет)
	char   Name[13]; //*   +23  db*D  = имя,0
	long   Spec;     //   +30  dd    = str[8] str=(*[67CD08])[номер подтипа *5C]
	Byte   Pic;      //*   +34  db    = номер картинки
// dw +35 ???
// dw +41 ???
// db +43 0
	Byte  _u2[15];
		// +3E db -??? 4E3BB5 - used in luck calculation
	Byte   x0;       //   +44  db    = базовый x для обегания (FF-не ограничен)
	Byte   y0;       //   +45  db    = базовый y для обегания (FF-не ограничен)
	Byte   Run;      //   +46  db    = радиус обегания (FF-не ограничен)
	Byte  _u3;       //   +47  db    = ??? 
	Byte   Flags;    //   +48  8*bb  (463253)
		// 01 - тип группировки юнитов
		// 02 - разрешена тактика для героя
	int    Movement0;//    +49  dd    = полное перемещение начальное
	int    Movement; //*   +4D  dd    = оставшиеся перемещения
	Dword  Exp;      //*   +51  dd    = опыт
	Word   ExpLevel; //*   +55  dw    = уровень
//  Dword  VStones;  // +57 32 камня (1-посещен,2-нет)
//  Dword  VMTower;  // +5B Башня Мардетто
//  Dword  VGarden;  // +5F сад откровения
//  Dword  VMCamp;   // +63 лагерь наемников
//  Dword  VSAxis;   // +67 звездное колесо
//  Dword  VKTree;   // +6B дерево знаний
//  Dword  VUniver;  // +6F университет
//  Dword  VArena;   // +73 арена
//  Dword  VSMagic;  // +77 школа магов
//  Dword  VSWar;    // +7B школа войны
	Dword  Visited[10];
	Byte  _u4[18];   // +7F
	int    Ct[7];    //   +91  dd*7  = тип существ (-1 - нет)
	int    Cn[7];    //   +AD  dd*7  = количество
	Byte   SSkill[28];//   +C9  db*1C = уровень 2-х скилов (один байт - уровень этого номера скила 1,2,3) 0-нет
		// C9=Pathfinding CA=Archery CB=Logistics CC=Scouting CD=Diplomacy CE=Navigation CF=Leadership 
		// D0=Wisdom D1=Mysticism D2=Luck D3=Ballistics D4=Eagle Eye D5=Necromancy D6=Estates D7=Fire Magic 
		// D8=Air Magic D9=Water Magic DA=Earth Magic DB=Scholar DC=Tactics DD=Artillery DE=Learning DF=Offence 
		// E0=Armorer E1=Intelligence E2=Sorcery E3=Resistance E4=First Aid 
	Byte   SShow[28]; //   +E5  db*1C = порядок отображения 2-х скилов в окне героя (1,2,3,4,5,6)
	Dword  SSNum;    //   +101 dd    = количество 2-х скилов
//  Word  RefData1;  //   +105  4814D3+...
//  Word  RefData2;  //   +107  4DA466
	Dword  TempMod;    // +105 временные модификаторы
// нач. иниц. при найме 0xFFF9FFFF
//  00000002 = конющня 3.59 ERM
//  00000008 = лебединое озеро swan pond 3.59 ERM
//  00000020 = (???) фонтан удачи fountain of fortune 3.59 ERM
//  00000080 = оазис
//  00002000 = домик фей
//  00040000 = в лодке на воде
//  00200000 = -3morale в вариор томб
//  00400000 = Give Maximum Luck
//  00800000 = Give Maximum Moral
//  38000000 = конкретный тип фонтана удачи
	Byte  _u6[9];   // +109
	Dword _u7;      // +112 
	char   DMorale;  // +116 модификаторы морали (накапливаются)
	Byte  _u60[3];
	char   DMorale1; // +11A модиф морали (оазис)
	char   DLuck;    // +11B модиф удачи до след битвы
	Byte  _u6a[17];
	int   IArt[19][2];  //+12D dd*2*13h = артифакты dd-номер,dd-номер закла(или -1) (книга 3,FF)
	Byte  FreeAddSlots;   //+1C5 количество пустых доп. слотов слева
	char  LockedSlot[14]; //+1C6
	int   OArt[64][2];  //+1D4 dd*2*40 = арт в рюкзаке dd-номер, dd-номер закла(или -1)
	Byte    OANum;   //   +3D4 db   = число артифактов в рюкзаке
	Dword  Sex;      //   +3D5 dd    = пол
	Byte   fl_B;     //   +3D9 db    = есть биография
//  char  *Bibl;     //   +3DA dd    -> биография
//  Byte  _u7[12];    //  +3DE
		Dword  _5b;     // +3DA
		_AMes_ Bibl;    // +3DE
	Byte   Spell[70]; //   +3EA db*46 = заклинание (есть/нет)
	Byte   LSpell[70];//   +430 db*46 = уровень заклинания (>=1)
	char   PSkill[4]; //   +476 db*4  = первичные навыки
	Byte  _u8[24];
};

struct _Boat_{ // 0x28 - looks like a hero
		Word   x;        //   +00  dw    = x position
		Word   y;        //   +02  dw    = y position
		Word   l;        //   +04  db+db (выравнивание)  = ? уровень
		Byte   Visible;  //   +06  db    = 1 - есть на карте (внутри города или не активен)
//  Byte  _u1[17];
// +07 db - x
// +08 db - (?) мусор (к x)
// +09 db - y
// +0A db - (?) мусор (к y)
		Dword  PlMapItem;
// +0B db - (?) l
	Byte  _u1;
// +0C dd - тип объекта на котором герой стоял
		Dword   PlOType; // dd +1E с карты
// +10 db - бит занятости во флагах поверхности 00001000
// это бит означающий, что здесь есть/была точка входа (желтая клетка)
		Byte   Pl0Cflag;
	Byte  _u2[3];
// +14 dd - SetUp с карты
		Dword  PlSetUp; // dd +0 с карты

		Byte    par3;   //   +18  db    = (1)
		Byte    par6;   //   +19  db    = (0)
		Byte    BoatPicture;   //   +1A  db
		Byte    par2;   //   +1B  db    = (2) 0...6
		Byte    Owner;    //   +1C  db    = player 0...7,-1
		Byte   _u3;
		Word   _u5;     //   +1E  dd    = Id (0x606E)
		int     HeroInd;  //   +20  dd  
		char    HasHero;  //   +24  db
		Byte   _u6[3];
};
// ADR0=15B6920 неаходится в [r698560][e69A538]
//  +1F63E dw - текущий день недели >=1
//  +1F640 dw - текущая неделя >=1
//  +1F642 dw - текущий месяц >=1
struct _Date_{
	Word  Day;
	Word  Week;
	Word  Month;
};


// Специализация героев 677440 24h*N
//  +0   dd = тип специализации (0,1,2,3,4,5,6,7)
//  +4   dd = подтип
//  +8   dd
//  +C   dd
//  +10  dd
//  +14  dd
//  +18  dd

// ADR0:
// +4E224 db*90 = структура с кучей единичек (17 штук в конце) [2-я по счету]
// +4E658 db*1C = след байты
//       4BCE09 подгр след 136h байт
// +1F680 dd=1,dd->start,dd->stop,dd->stop
// +4E2B4 db*90h (до +4E344)= 0-артифакт разрешен (c 0), 1-нет (каждый байт - подтип)

// 1.Тексты сообщений можно копировать с дополнением {}{}{}{} - они убираются из потока
// 2.Самый базовый адрес ADR0=15B6920 неаходится в [r698560][e69A538]
//       +00098 -> структуры замков (88h размер)
//          +54 db = ? (0)
//          +68 dd = 8 тип города (0...8)
//       +000A4 db*334h*N - описание всех типов героев
//       +1FB70 структура main
//       +1F63E dw - текущий день недели >=1
//       +1F640 dw - текущая неделя >=1
//       +1F642 dw - текущий месяц >=1
//       +1F86C dd - тип карты (0E,15,1C)
//       +1F879 db*8 - номер команды игрока по числу команд {0,1,2,3,4,5,6,7}
//       +1FC44 dd - размер карты (по x?)
//       +1FC40 dd - ? адрес всего на поверхности
//       +21614 -> структуры замков (168h размер)
//          +0  db = номер замка
//          +5  db = x
//          +6  db = y
//          +7  db = level
//       +21620 db*492h*N - описания конкретных героев (формируется 4CACAE)
//       +4E39C dd -> таблица внешних генераторов монстров
// 3.Структура загрузки 9477D0[4] ->[0]*buf [8]len already loaded
//   New:9477D0, Load:947AEC
// 4. Load: 4FDF55 грузит ландшафт из сейва
//    New : 4FDA1E     
// 5. Load: 4FE153 грузит типы объектов (+4,+14,+24)
//    New : 4FDC4C
// 6. Load: 4FE16F грузит локальные евенты (+54)
// 7. Load: 4FE261 грузит глобальные евенты (+84)
//    New : 4FDC66 
// 8. Блок памяти выделяется (dd-выделено/нет,dd-начало,dd-конец,dd-полный объем)
//   Если память уже выделена, то проверяется размер.
//   Если размер меньше, то освоб. и выделяется новая.
//   Если больше или равен, то устанавливается корректно конец
// 50322F переход на загрузку по типу артифакта (новая игра)
//
// 5048FC подгрузка элемента таблицы типов объектов
// 504B6B - читает кол-во объектов в [ebp-14]
// 503227 - переход на загрузку по шаблону типа объекта
// 4FDC66 edx(ebp+8)=&STR1 ; ecx(esi)=P1
//   на выходе esi=P1
// STR1: 9477D0[4] ->[0]*buf [8]len already loaded
// загрузка событий из карты.


// [P1+D0] -> MAP поверхность карты
// [P1+D4] = dx=dy - размер карты

// Load r подгружает это в 4FEC90
// Поверхность ADR=MAP+(x+dy*y)*26h
//  +0   dd   = 4 байта настройки = _CMonster_ (для монстров)
//  +4   dd   = +0 4 байта из записи (0,1A,0,0) (поверхность,тип,речка,тип)
//  +8   dw   = +4 2 байта из записи (0,0) (дорога,тип)
//  +A   dw   = 0
//  +C   dd   = ? 0 используется при передвижении героев
//    00 01 00 00 - непроходимо (красная клетка)
//    00 10 00 00 - есть что-то, что надо настраивать (желтая клетка)
//    00 02 00 00 - граница к воде ???
//    00001000 - стоит герой
//  +10  dw   = +6 2 байта из записи (10,C0) (поверхность - зеркало,тип?)
//  +12  dd   -> 8,по 4 байта из единиц и нулей (прорисовка обЪектов)
//  +16  dd   -> на конец стр
//  +1A  dd   -> -"- (выделенная)
//  +1E  dw   = +8 2 байта из записи (36,0) (тип)
//  +20  dw   = 0
//  +22  dw   = +A 2 байта из записи (8A,0) (подтип)
//  +24  dw   = +C 2 номер пол. объекта на карте [P1+14]...[P1+18]
struct ODraw{ Word LType; Word Code; };
// LType = индекс в _Position_
// Code = z << 8 + y << 4 + x  
//        x, y - от правого нижнего угла def'а в обратную сторону,
//        z - высота клетки в дефе: 0 - часть почвы, 1 - 1й этаж, 2 - 2й этаж, ...)
struct _MapItem_{
	Dword   SetUp;     // +0
	Byte    Land;      // +4
	Byte    LType;     // +5
	Byte    River;     // +6
	Byte    RType;     // +7
	Byte    Road;      // +8
	Byte    RdType;    // +9
	Word   _u1;        // +0A
//  Dword   Bits;
	Byte    Mirror;    // +0C тип отражения
		
	Byte    Attrib;    // +0D ж или к клетки
	Word   _Bits;      // +0E
	Word   _u2;        // +10
	ODraw   *Draw;     // +12
	ODraw   *DrawEnd;  // +16
	ODraw   *DrawEnd2; // +1A
	int     OType;     // +1E
	Word    OSType;    // +22
	Word    DrawNum;   // +24
};
// Лагерь беженцев.
// +0   dd = число монстров для найма
// +22h dw = тип монстров для найма

///////////////////////////////
// Таблица Артефактов
struct _ArtSetUp_{
	char  *Name;        // +00
	Dword  Cost;        // +04
	int    Position;    // +08
	int    Type;        // +0C
	char  *Description; // +10 строка описания идет сразу за строкой именем
	int    SuperN;      // +14
	int    PartOfSuperN;// +18
	Byte   Disable;     // +1C
	Byte   NewSpell;    // +1D
	Byte  _u1;          // +1E
	Byte  _u2;          // +1F
};
// Combo art
struct _CArtSetup_{
	int   Index;
	Dword ArtNums[5];
};

/////////  CRBANKS.TXT (11d по 4)
struct _CrBankMap_{
	long  DMonsterType[7]; // -1 - no
	long  DMonsterNum[7];
	long  Res[7];
	long  BMonsterType; // -1 - no
	char  BMonsterNum;
	Byte _u1[3];
	_ArtList_ Arts;
/*
	Dword HasArtDescr;
	void *ArtStart; // set up (copying) at 004D258F
	void *ArtStop;
	void *ArtAll;
*/
};
//!!!!! обязательно добавлять TXT в H3BITMAP.LOD
//!!!!! добавить DEF с типом 16 и подтипом 11,12 ...
struct crbanks1{ // 0x67037C (11d)
	long BMonsterType; // -1 - no
};
struct crbanks2{ // 0x6702A0 (11d)
	long DMonsterType[5]; // -1 - no
};

struct _BankItem_{
	long  DMonsterType[7]; // -1 - no
	long  DMonsterNum[7];
	long  Res[7];
	long  BMonsterType; // -1 - no
	Byte  BMonsterNum;
	Byte  Chance;   // 30,30,30,10
	Byte  Upgrade;  // 50,50,50,50,
	Byte  Treasure; // 0/1
	Byte  ArMinor;  // 0/1
	Byte  ArMajor;  // 0/1
	Byte  ArRelic;  // 0/1
	Byte _n1;
};

struct _CrBank_{ // 695088 (11d)
	long   SetUp; // =1
	_AMes_  Name;
	_BankItem_ Bank[4];
};
/////////  CRTRAITS.TXT (150d)
// Добавить строки для анимации в Cranim.txt (+1, т.к. нет башни)
// Добавить в CPRSMALL.def и TwCrPort.def необходимые рожи монстров (сдвинутые на 1)
// Сделать DEF анимации.
struct _MNameS_{
	char *NameS;
};
struct _MNameP_{
	char *NameP;
};
struct _MNameSp_{
	char *Spec;
};
struct _MonAnim_{ // Cranim.txt
	Word  w5;
	Word  w6;
	Word  w7;
	Word  w8;
	Word  w9;
	Word _u1;
	Dword f10;  
	Dword f11;  
	Dword f12;  
	Dword f13;  
	Dword f14;  
	Dword f15;
	Dword f16;  
	Dword f17;  
	Dword f18;  
	Dword f19;  
	Dword f20;  
	Dword f21;  
	Dword d22;  
	Dword d23;  
	Dword d1;  
	Dword d2;  
	Dword d3;  
	Dword d4;  
};
struct _MonInfo_{
	long  Group;     // +0  0...8,-1 - neutral
	long  SubGroup;  // +4  0...6,-1 - not available (423D87, 42A0FC)
	char *SoundPrefix; // +8  (4242B7)
	char *DefName;   // +C  подгр и настр поля в 43DA45 
	Dword Flags;     // +10 (424354, 42C6C0)
	char *NameS;     // +14
	char *NameP;     // +18
	char *Spec;      // +1C
	long  CostRes[7];// +20 (42B73A)
	long  Fight;     // +3C
	long  AIvalue;   // +40
	long  Grow;      // +44 начальное количество монстров для найма
	long  HGrow;     // +48
	long  HitPoints; // +4C
	long  Speed;     // +50
	long  Attack;    // +54
	long  Defence;   // +58
	long  DamageL;   // +5C
	long  DamageH;   // +60
	long  NShots;    // +64
	long  HasSpell;  // +68 - сколько раз колдует
	long  AdvMapL;   // +6C
	long  AdvMapH;   // +70
};

////////////////////////////////////////
///////// Sec Skills
struct _SSName_{ // 0x698BC4
	char *Name;
};
#define SSNAME ((_SSName_ *)0x698BC4)

struct _SSDesc_{ // 0x698C34
	char *Name[3]; // баз,продв,эксп
};
#define SSDESC ((_SSDesc_ *)0x698C34)

struct _SSNameDesc_{ // 0x698D88
	char *Name[4]; // имя,баз,продв,эксп
};
#define SSNAMEDESC ((_SSNameDesc_ *)0x698D88)
////////////////////////////////////////
///////// HEROES
struct _HeroName_{
	char *Name;
};
// таблица настройки героев 0x679DD0
struct _HeroInfo_{ // size=5Ch
	long   Enabled;       // разрешен 1 или нет 0
	long  _u1[8];         // +4
	long   ArmyType[3];   // +24h
	char  *HPSName;       // +30h 
	char  *HPLName;       // +34h
	Byte  _u2[8];         // +38h
	char  *Name; //       // +40h
	long   ArmyNum[3][2]; // +44h
};
// загрузка все лиц 57C7B8
//  формирует c ebx+
//   +C8 до +354, +354 < hpsrand.def
//   +7C < ScnrStar.def

struct _HeroType_{ // HcTraits.txt 4E6A0E -> 698B78
// универсальные установки для 8-ми типов героев   
};
// 4E6A10+1 ->

struct _HeroBios_{ // HeroBios.txt 5B9A06 -> 6A6740 (6A673C) (4*N) N=A3h(N+7)
	char *HBios;
};
// 5B9A0E+2(4) = N*4
// 5B9A18+2(4) ->

struct _HeroSpec_{ // HeroSpec.txt 4D744E -> 678420 (28h*4)
	Dword  Spec;     // +0
	Dword  Spec1;    // +4h
	Dword  Spec2;    // +8h
	Dword  Spec3;    // +Ch
	Dword  Spec4;    // +10h
	Dword  Spec5;    // +14h
	Dword  Spec6;    // +18h
	char  *SpShort;  // +1Ch
	char  *SpFull;   // +20h
	char  *SpDescr;  // +24h
};
// 4D7449+1(4) -> (+20h)
// 4D745A+2(4) -> (28h*N+20h)
// 679C80  (4) -> 
//////////////////////////////////
/////////  Magic
struct _MagicAni_{
	char *DefName;
	char *Name;
	int   Type;
};


// Замки
// 6971F0 (18h*9[число типов городов])
// 6A60B0 ()
// 
// + 24h dw * 8  = количество доступных для найма монстров
// +150h dd + dd = постройки
// 0 01 -  0 Гильдия магов ур 1
//   02 -  1 2
//   04 -  2 3
//   08 -  3 4
//   10 -  4 5,?(6,7)
//   20 -  5 таверна
//   40 -  6 верфь(0,4,7,8),?(1,2,3,5,6) 
//   80 -  7 форт (+ слева картинка)
// 1 01 -  8 цитадель (+)
//   02 -  9 замок (+)
//   04 - 10 управа (+)
//   08 - 11 префектура (+)
//   10 - 12 муниципалитет (+)
//   20 - 13 капитолий (+)
//   40 - 14 рынок
//   80 - 15 хранилище ресурсов
// 2 01 - 16 кузница
//   02 - 17 маяк(0),таинств.пруд(1),торг.арт.(2,5,8),?(3),вуаль тьмы(4),
//           чер.ход(6),кл.бог.войны(7)
//   04 - 18 баст.гриф.(0-3),гильд горн.(1-2),крылья воятеля(2-2),инкуб(3-1),
//           взрыт.мог.(4-1),гриб.кольца(5-1),стол(6-1),кв.капитана(7-1),сад жизни(8-1)
//   08 - 19 баст.гриф.+(0-3+),гильд горн.(1-2+),крылья воятеля(2-2+),инкуб(3-1+),
//           взрыт.мог.(4-1+),гриб.кольца(5-1+),стол(6-1+),кв.капитана(7-1+),сад жизни(8-1+)
//   10 - 20 ?(0,1,2,3,4,5,6,7,8)
//   20 - 21 конюшни(0),фонтан уд(1),см.башня(2),сер.тучи(3),ус.ч.магии(4),
//           водов.маны(5),гил.н.раб(6),знаки страха(7),ун.магии(8)
//   40 - 22 бр.меча(0),сокровищ(1),библ(2),врата замка(3),преобр.ск(4),
//           портал выз(5),дв.балист(6),обелиск крови(7),?(8)
//   80 - 23 ?(0,1,4,7,8),стена зн.(2),орден огня(3),ак.б.иск(5),храм волх(6)
// 3 01 - 24 ?(0,2,4,5,6,7,8),мол.дендр(1-5),клетки(3-3)
//   02 - 25 ?(0,2,4,5,6,7,8),мол.дендр(1-5+),клетки(3-3+)
//   04 - 26 грааль
//   08 - 27 ?(x)
//   10 - 28 ?(x)
//   20 - 29 ?(x)
//   40 - 30 жил 1  (x-1)
//   80 - 31 жил 2  (x-2)
// 4 01 - 32 жил 3  (x-3)
//   02 - 33 жил 4  (x-4)
//   04 - 34 жил 5  (x-5)
//   08 - 35 жил 6  (x-6)
//   10 - 36 жил 7  (x-7)
//   20 - 37 жил 1+ (x-1+)
//   40 - 38 жил 2+ (x-2+)
//   80 - 39 жил 3+ (x-3+)
// 5 01 - 40 жил 4+ (x-4+)
//   02 - 41 жил 5+ (x-5+)
//   04 - 42 жил 6+ (x-6+)
//   08 - 43 жил 7+ (x-7+)
// +158h dd + dd = модификаторы и производители существ
// теже жилища
// 0 80 - форт слева картинка 
// 2 08 - "Инкубатор"
// 3 40 - "Бесы" карт
//   80 - "Гоги"
// 4 01 - "Адские Гончие"
//   02 - "Демоны"
//   04 - "Порождение Зла"
//   08 - "Эфриты"
//   10 - "Дьяволы"
//   20 - + "Черти" ("Бесы")
//   40 - + "Магоги" ("Гоги")
//   80 - + "Церберы" ("Адские Гончие")
// 5 01 - + "Рогатые Демоны" ("Демоны")
//   02 - + "Адские Отродья" ("Порождение Зла")
//   04 - + "Султаны Эфритов" ("Эфриты")
//   08 - + "Архидьяволы" ("Дьяволы")

struct _CastleSetup_{
	Byte   Number;        //* +0 0,1,2,...
	char   Owner;         //*O +1 0,...
	char   BuiltThisTurn; // +2 - уже строили в этот турн (0-нет, 1-да, 2-не наш город)
	Byte  _u2;            //* +3 0
	Byte   Type;          //*T +4 0,1...,8
	Byte   x;             //* +5
	Byte   y;             //* +6
	Byte   l;             //* +7
	Byte   Pos2PlaceBoatX;//* +8 помещать лодку при покупки в Shipyard
	Byte   Pos2PlaceBoatY;//* +9
	Byte  _uAa[2];        // +0A
	int    IHero;         //* +0Ch = номер героя внутри города (-1 - никого нет)
	int    VHero;         //* +10h = номер героя снаружи города (-1 - никого нет)
	char   MagLevel;      //*G +14h = уровень магической гильдии в городе (исп. AI для постройки)
	Byte  _u15;
	Word   Monsters[2][7];//*M- +16h ко-лво простых и апгрейднутых
	char  _u32;           //*- +32 = ?
	char  _u33;           //* +33 = 1
	char  _u34;           //* +34 = 0
	Byte  _u35a[3];
	int   _u38;           //* +38 = -1
	int    MonSummonT;    //* +3C = тип монстров в портале призвания
	short  MonSummonN;    //* +40 = кол-во монстров в портале призвания
	Word  _u42;           //  +42
	int    Spels[5][6];   //*G- +44 сами заклинания
	char   MagicHild[5];  //*G- +BCh = колво заклинаний в уровне гильдии
	Byte  _uC1[3];
	char  _uC4;           //* +C4 = 0
	Byte  _uC5[3];
	_AMes_ Name;          //*N +C8 -> Имя города
	int   _u8[3];         //* +D4 = 0
	Dword  GuardsT[7];    //*M +E0 = охрана замка
	Dword  GuardsN[7];    //*M +FC = кол-во охраны
	Dword  GuardsT0[7];   //*M- +118 = охрана замка
	Dword  GuardsN0[7];   //*M- +134 = кол-во охраны
	Byte   Built[8];      //*B +150h = уже построенные здания для показа (0400)
	Byte   Bonus[8];      //*B +158h = бонус на существ, ресурсы и т.п., вызванный строениями (все построенные здания, включая негейды)
	Dword  BMask[2];      //*B- +160h = маска доступных для строения строений
};

// BASE+98
//5B 64 02 D7 FF 00 XX XX XX XX XX XX XX XX XX XX
//XX XX XX XX XX XX XX XX 01 00 XX XX FF FF FF FF
//FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
//FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00
//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//00 00 00 00 00 XX XX XX 00 XX XX XX 00 00 00 00
//00 00 00 00 00 00 00 00 01 00 00 00 00 XX XX XX
//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//00 00 00 00 00 00 00 00
//005027468B0D38956900   mov    ecx,[00699538]
//0050274C 8B919C000000   mov    edx,[ecx+0000009C]
//00502752 81C194000000   add    ecx,00000094
//00502758 8D8540FFFFFF   lea    eax,[ebp-000000C0] -> структурка
//0050275E 50             push   eax
//0050275F 6A01           push   00000001
//00502761 52             push   edx
//00502762 E819690000     call   H3.00509080
struct _CastlePreSetup_{
	Dword  Id;            // + 0 = (можно 0)идентификатор из карты (уникальный)
	char   Owner;         // + 4 = -1 (ничей)
	Byte  _u9;            // + 5 = 0
	Byte  _u11[18];       // + 6 = XXX
	Byte  _u10;           // +18 = 1 (есть форт)
	Byte  _u7;            // +19 = 0
	Byte  _u12[2];        // +1A = XXX
	int    GuardsT[7];    // +1C = охрана замка
	int    GuardsN[7];    // +38 = кол-во охраны
	Byte  _u6;            // +54 = 0
	Byte  _u13[3];        // +55 = XXX
	Byte  _u1;            // +58 = 0
	Byte  _u14[3];        // +59 = XXX
	Byte  _u2[12];        // +5C = 0
	int    SType;         // +68 = 1 тип города
	Byte  _u8;            // +6C = 0
	Byte  _u15[3];        // +6D = XXX
	Byte  _u3[12];        // +70 = 0 (что запрещено строить)
	Byte  _u4[12];        // +7C = 0 (что построено)
};
////////////////////////////
struct _Main_{
	Dword   _u1a; struct  _Types_       *Tp0,*Tp1,*_u2a;   // +00
	Dword   _u1b; struct  _Position_    *Pp0,*Pp1,*_u2b;   // +10
	Dword   _u1c[4]; // DEFы                               // +20
	Dword   _u1d; struct  _ArtRes_      *Ap0,*Ap1,*_u2d;   // +30
	Dword   _u1e; struct  _Monster_     *Mp0,*Mp1,*_u2e;   // +40
	Dword   _u1f; struct  _Event_       *Ep0,*Ep1,*_u2f;   // +50
	Dword   _u1g[4];                                       // +60
	Dword   _u1h[4];                                       // +70
	Dword   _u1i; struct  _GlbEvent_    *GEp0,*GEp1,*_u2i; // +80
	Dword   _u1j; struct  _CastleEvent_ *CEp0,*CEp1,*_u2j; // +90
	Dword   _u1k[4];                                       // +A0
	Dword   _u1l[4];                                       // +B0
	Dword   _u1m[4];                                       // +C0
	struct _MapItem_ *MIp0; Dword MapSize; Dword _u1n[2];  // +D0
};

// def формат
//  +0   dd
//  +4   dd
//  +8   dd
//  +C   dd     = S кол-во сериалов
//  +10  db*300 = палитра
//  +310 - описания сериалов *S
//  +XXX - образы pcx-ов
//
// Описания сериалов
//  +0   dd  = номер сериала (0...S-1)
//  +4   dd  = M количество серий
//  +8   dd
//  +C   dd
//  +10  - описания серий *M
//
// Описания серий
//  +0   (db*D)*M - имя pcx-ов
//  +XX   dd*M    - смещение pcx-а
//
// pcx
//  +0   dd  = N размер в байтах образа pcx-а
//  +4   dd
//  +8   dd
//  +C   dd
//  +10  dd
//  +14  dd  = W количество строк
//  +18  dd
//  +1C  dd
//  +20  db*N = образ pcx-а
//
// Образ pcx-а
//  +0   dw(dd)*W  = смещение на начало строки от начала образа
//  +XXX db*.. =?


// db = тип поверхности
// db = тип рисунка
// db = тип речки
// db = конф. речки
// db = тип дороги
// db = конф. дороги
// db = тип отражения
// db = занятость клетки
//   &  1 = занято (красная клетка)
//   &  2 = граница поверхности (след тип к воде 3,0, наоборот - 0,0)
//   &  4 = 
//   &  8 = 
//   & 10 = занято (желтая клетка)
//   & 20 = 
//   & 40 = 
//   & 80 = 

////////////////////////////////////////
// найм монстров герою
/*
//   mov    dx, число монстров
//   push   ebx
//   mov    ebx,[ebp+08]
//   push   edi
//   movsx  edi,word ptr [esi+22]
//   mov    ecx,ebx
//   mov    [ebp+10],edx
//   call   HEROES3.004D92C0
//   test   al,al
//   je     HEROES3.004A43D0 (004A43D0)
	 push   00000000
	 push   FFFFFFFF
	 push   00000000
	 push   FFFFFFFF
	 push   00000000
	 push   FFFFFFFF
	 lea    eax,[ebp+10] = число монстров (dw)
	 push   eax
	 push   00000000
	 push   edi = тип монстров
	 add    ebx,00000091 -> слоты для армий у героя (ebx->Герой)
	 push   ebx
	 lea    ecx,[ebp-000000BC] -> буфер на ~20h байт
	 call   0x0551750
	 lea    ecx,[ebp-000000BC] -> буфер на ~20h байт
	 push   ecx
	 mov    ecx,[00699550] ???
	 call   HEROES3.004B0770
	 movsx  eax,word ptr [ebp+10] = оставшееся число монстров для найма
*/
/* // нанимает в городе
asm{
	mov    eax,[006AAABC]
	mov    ecx,[00699550]
	push   eax
	call   HEROES3.004B0770
}
*/
/* // добавить герою опыт
	mov    ecx, ук. на стр. героя
	push   00000001
	push   00000000
	push   количество опыта (золотой эквивалент)
	call   HEROES3.004E3620
*/ 
// [69CCF4] = текущий активный игрок
/*
 mov    eax,[0x69CCF4] = цвет игрока
 mov    edi,[0x699538] = BASE
 lea    edx,[eax+4*eax]
 lea    edx,[edx+8*edx]
 lea    edx,[esi+2*edx]  esi = номер ресурса
 lea    edx,[edi+4*edx+00020B6C]
 cmp    dword ptr [edx],00000000
*/

// ффункция 47F9B0 вызывается на каждом ходу !

// видимость поверхности - бескомандный режим
//004F8043 A1CC836700     mov    eax,[006783CC] // dy (48h)
//004F8048 0FAF4508       imul   eax,[ebp+8]    // level (ebx)
//004F804C 03C2           add    eax,edx        // y (edx)
//004F804E 0FAF05C8836700 imul   eax,[006783C8] // dx
//004F8055 03C1           add    eax,ecx        // x (ecx)
//004F8057 8B0D488A6900   mov    ecx,[00698A48] // -> shadow's map
// битовые маски 1 бит - один игрок 1=красный , 2=синий и т.п.
//004F805D 668B0441       mov    ax,[ecx+2*eax] // =0 - invisible, 1=visible
//
// видимость поверхности - бескомандный режим
//004F8073 A1CC836700     mov    eax,[006783CC] // dy (48h)
//004F8078 0FAF4508       imul   eax,[ebp+08]   // level
//004F807C 03C2           add    eax,edx
//004F807E 0FAF05C8836700 imul   eax,[006783C8]
//004F8085 03C1           add    eax,ecx
//004F8087 8B0D488A6900   mov    ecx,[00698A48]
//004F808D 8D0441         lea    eax,[ecx+2*eax]
//
// затемнить часть поверхности
//004A11B2 6A14           push   00000014     // радиус затемнения
//004A11B4 50             push   eax          // номер хозяина(цвета)
//004A11B5 8B450E         mov    eax,[ebp+0E] //
//004A11B8 C1E606         shl    esi,06       //
//004A11BB 8D0C8500000000 lea    ecx,[4*eax]  //
//004A11C2 C1E006         shl    eax,06       //
//004A11C5 0FBFD1         movsx  edx,cx       //
//004A11C8 0FBFC0         movsx  eax,ax       //
//004A11CB 0FBFCE         movsx  ecx,si       //
//004A11CE C1FA0C         sar    edx,0C       //
//004A11D1 C1F806         sar    eax,06       //
//004A11D4 52             push   edx          // level
//004A11D5 50             push   eax          // y
//004A11D6 C1F906         sar    ecx,06       //
//004A11D9 51             push   ecx          // x
//004A11DA 8B0D38956900   mov    ecx,[00699538] // -> BASE
//004A11E0 E85BBEFFFF     call   HEROES3.0049D040

// перемещение героя в новое место
// esi -> Hero
// eax = радиус видимости
//00480967 0FBF4E04       movsx  ecx,word ptr [esi+04] // Level
//0048096B 0FBF5602       movsx  edx,word ptr [esi+02] // exY
//0048096F 50             push   eax                   // radius of visability
//00480970 0FBE4622       movsx  eax,byte ptr [esi+22] // Owner (color)
//00480974 50             push   eax
//00480975 51             push   ecx
//00480976 8B4DE8         mov    ecx,[ebp-18]
//00480979 0FBF06         movsx  eax,word ptr [esi]
//0048097C 03D1           add    edx,ecx
//0048097E 8B0D38956900   mov    ecx,[00699538]
//00480984 52             push   edx
//00480985 8B55DC         mov    edx,[ebp-24]
//00480988 03C2           add    eax,edx
//0048098A 50             push   eax
//0048098B E840C40100     call   HEROES3.0049CDD0
// [GF] надо попробовать захучить - смотреть, есть ли проклятие на радиус обхора, и уменьшать его. (4bugfix)

//////////////
// Магия
// 6854A0 структура описания магий (хранится в [687FA8])
// спец действия для мостра после атаки или ответа
// 4408E0
// картинка действия магии
//00440B88 B9400A6600     mov    ecx,00660A40   // -> имя wav
//00440B8D E8DE9B1500     call   HEROES3.0059A770 // -> eax открытый wav
//00440B92 8B0D20946900   mov    ecx,[00699420] // указатель на батл менеджер
//00440B98 6A00           push   00000000
//00440B9A 6A64           push   00000064       // задержка (1/скорость анимации)
//00440B9C 56             push   esi            // -> структура монстра во время битвы
//00440B9D 6A4A           push   0000004A       // номер магии
//00440B9F 8BF8           mov    edi,eax        // -> открытый wav
//00440BA1 8BDA           mov    ebx,edx        // =0 ?
//00440BA3E818580500     call   HEROES3.004963C0

// наем существ
//BB0 80 0C 64 00 00 00 00 00 00 00 00 00 FF FF FF FF А         ____
//BC0 FF FF FF FF 55 6E 6B 6E 6F 77 6E 00 00 00 00 00 ____Unknown
//BD0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//BE0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//BF0 00 00 00 00 00 00 00 00 62 00 00 00 00 00 00 00 
//C00 77 00 00 00 34 C8 7D 01 00 00 00 00 77 00 00 00 
//C10 76 00 00 00 FF FF FF FF FF FF FF FF 34 C8 7D 01 
//C20 34 C8 7D 01 00 00 00 00 00 00 00 00 00 00 00 00 
//C30 00 00 00 00 1E 00 00 00 FF FF FF FF 00 00 00 00 
//C40 01 00 00 00 00 00 00 00 F0 C8 7D 01 01 00 00 00 
//C50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//C60 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//C70 00 00 00 00 00 00 00 00 D1 00 00 00 51 00 00 00 

/*
// Heroes Managers
struct _Manager_{
	Dword    fun;
	Dword   _u1[4];
	char     Mane[16]; // имя менеждера
//...  
};
*/

// 5258B0 - думает что делать AI с замком (главная)
// 42BEB0 - думает что делать AI с замком (что строить из Магических Гильдий)
// 40F8A0 call 40F350 - вызывается на каждом шагу Героя.
// 4B0C3A - где-то вокруг этой точки крутится основной цикл на карте
struct _Dwelling_{     // 0x5C размер
	Byte   Type;        // +00 db = 0x11 = 17 - Жилище (0x14 = жилище с 4 монстрами)
	Byte   SType;       // +01 db = тип строения (подномер)
	Byte  _u1;          // +02 db = 0xEC
	Byte  _u2;          // +03 db = 1
	long   Mon2Hire[4]; // +04 dd*7 = тип монстра для найма (-1=нет)
	Word   Num2Hire[4]; // +14 dw*4 = количество монстров для найма (0=нет)
	long   GType[7];    // +1C dd*7 - тип охранников
	long   GNum[7];     // +38 dd*7 - кол-во охранников
	Byte   x;           // +54 db (3)
	Byte   y;           // +55 db
	Byte   l;           // +56 db
	char   Owner;       // +57 db Хозяин -1=не чей
	Byte _u4;           // +58 db (0xFF)
	Byte _u5;           // +59 db (0x75)
	Byte _u6;           // +5A db (0xA0)
	Byte _u7;           // +5B db (0x00)
};

struct _CombatMon_{  // размер 0x548
										 // +00 db (1) когда отаковал уже(?)
										 // +01 db
										 // +02 db (0C) 44188C
										 // +03 db
										 // +08 dd = полное число стеков у игрока
										 // +10 dd =-1 после атаки и/или ответа(????)
										 // +1С dd = позиция на поле боя (куда бежать/стрелять)
										 // +20 db = огненный щит
										 // +28 dd = -1 (номер стэка клона этого)  
										 // +30 db 43E33C
	int   Type;        // +34 dd = тип монстра
	int   Pos;         // +38 dd = позиция монстра на поле боя (куда атакует)
										 // +3C dd = заклинания 
										 // +40 dd 43E236
										 // +44 dd (=1) сдвиг в сторону второй занятой клетки для монстра с двумя клетками
	int   MonNum;      // +4C - число монстров
										 // +50 - число монстров до удара по ним в тек. атаку
	int   LostLastHP;  // +58 - потери здоровья последнего монстра
										 // +5C - номер слота героя (0...6), -1 - будет удален после битвы
	int   MonNum0;     // +60 - число монстров в начале битвы
										 // +6C - полное здоровье (исп. как база для лечения)
										 // +70 dd 44150F 441744
										 // +74 dd -1 для продвинутых элементалей
										 // +78 dd = уровень существа (0...6)
	Dword Flags;       // +84
		// 00000001 - 0x00 DOUBLE_WIDE - занимает 2 клетки
		// 00000002 - 0x01 летает
		// 00000004 - 0x02 стреляет
		// 00000008 - 0x03 расширенный радиус атаки (на две клетки)
		// 00000010 - 0x04 живое существо (можно восстанавливаться вампиру)
		// 00000020 - 0x05 CATAPULT - может разрушать стены
		// 00000040 - 0x06 SIEGE_WEAPON - осадное оружие - не двигается (5508CB)
		// 00000080 - 0x07 KING_1
		// 00000100 - 0x08 KING_2
		// 00000200 - 0x09 KING_3
		// 00000400 - 0x0A ??? 00020000 + 40,41,83 - не чуствителен к псих атаке
		// 00000800 - 0x0B нет описания (35,74,75)
		// 00001000 - 0x0C в ближнем бою бьет как в дальнем
		// 00002000 - 0x0D ----
		// 00004000 - 0x0E ??? IMMUNE_TO_FIRE_SPELLS
		// 00008000 - 0x0F стреляет дважды
		// 00010000 - 0x10 атака без ответа
		// 00020000 - 0x11 ... не подвержен низкой морали (?)
		// 32,33,56-69,112-117,120,121,123,,125,127,129,141,145-149 
		// 00040000 - 0x12 нечисть
		// 00080000 - 0x13 бьет всех врагов рядом
		// 00100000 - 0x14 расширенный радиус стреляющих юнитов
		// 00200000 - 0x15 стэк убит? 41E617 чародей,firebird - может еще кастовать?
		// 00400000 - 0x16 421BDC,421FC4 (что-то с вызовом)
		// 00800000 - 0x17 копия стэку - умирает сразу
		// 01000000 - 0x18 гарпии-ведьмы
		// 02000000 - 0x19 остался(уже) ждать СБРОСИТЬ - МОЖЕТ ЖДАТЬ СНОВА
		// 04000000 - 0x1A уст. после атаки СБРОСИТЬ - ВНОВЬ МОЖЕТ ОТАКОВАТЬ
		// 08000000 - 0x1B - выбрал защиту
		// 10000000 - 0x1C - не может быть ресуректен ???
		// 20000000 - 0x1D + 43DFAF
		// 40000000 - 0x1E + 43E06F
		// 80000000 - 0x1F дракон
//  int   FullHP;      // +B0 dd = ???
	int   HitPoints;   // +C0 dd = здоровье монстра
	int   Speed;       // +С4 dd = скорость монстра
	int   Attack;      // +C8 dd = атака с бонусами
	int   Defence;     // +CC dd = защита с бонусами
										 // +D0 dd = мин. дамэдж
										 // +D4 dd = макс. дамэдж
										 // +D8 dd = количество выстрелов
										 // +DC dd = количество заклинаний 0=нет ДЛЯ ВОСКРЕШЕНИЯ УВЕЛИЧИТЬ
										 // +E8 db =
										 // +E9 db = 1, если умирал хоть один
										 // +EA db = 1, если был убит весь стэк
										 // +EC dd = номер заклинания существа в тек раунде 0x50 Acid breath
										 // +F0 db=1 перед атакой на него 441434
										 // +F4 dd (0,1) индекс игрока 0 или 1 4414AF 443D69
										 // +F8 dd = номер стэка у стороны на поле боя
										 // +FC dd = ? что-то с магией
										 // +100 dd 43DEA4
										 // +104 dd 43DEAD
										 // +108 dd
										 // +158 dd =?
										 // +164 dd -> ctroll.def (троль)
	Byte *ShootDef;    // +168 - загруж.деф. для стреляющих (иниц в 43DA8E)
										 // +16C
										 // +194 dd = количество уже наложенных заклинаний
										 // +198 dd*? (есть заклинание (длительность) или нет по номерам)
										 //  +198 dd = Summon Boat
										 //  +19C dd = Scuttle Boat
										 //  +1A0 dd = Visions
										 //  +1A4 dd = View Earth
										 //  +1A8 dd = Disguise
										 //  +1AC dd = View Air
										 //  +1B0 dd = Fly
										 //  +1B4 dd = Water Walk
										 //  +1B8 dd = Dimension Door
										 //  +1BC dd = Town Portal

										 //  +1C0 dd = Quicksand
										 //  +1C4 dd = Land Mine
										 //  +1C8 dd = Force Field
										 //  +1CC dd = Fire Wall
										 //  +1D0 dd = Earthquake
										 //  +1D4 dd = Magic Arrow
										 //  +1D8 dd = Ice Bolt
										 //  +1DC dd = Lightning Bolt
										 //  +1E0 dd = Implosion
										 //  +1E4 dd = Chain Lightning
										 //  +1E8 dd = Frost Ring
										 //  +1EC dd = Fireball
										 //  +1F0 dd = Inferno
										 //  +1F4 dd = Meteor Shower
										 //  +1F8 dd = Death Ripple
										 //  +1FC dd = Destroy Undead
										 //  +200 dd = Armageddon
										 //  +204 dd = Shield
										 //  +208 dd = Air Shield
										 //  +20C dd = fire Shield
										 //  +210 dd = Protection from Air
										 //  +214 dd = Protection from Fire
										 //  +218 dd = Protection from Water
										 //  +21C dd = Protection from Earth
										 //  +220 dd = Anti-Magic
										 //  +224 dd = Dispel
										 //  +228 dd = Magic Mirror
										 //  +22C dd = Cure
										 //  +230 dd = Resurrection
										 //  +234 dd = Animate Dead
										 //  +238 dd = Sacrifice
										 //  +23C dd = Bless
										 //  +240 dd = Curse
										 //  +244 dd = Bloodlust
										 //  +248 dd = Precision
										 //  +24C dd = Weakness
										 //  +250 dd = Stone Skin
										 //  +254 dd = Disrupting Ray
										 //  +258 dd = Prayer
										 //  +25C dd = Mirth
										 //  +260 dd = Sorrow
										 //  +264 dd = Fortune
										 //  +268 dd = Misfortune
										 //  +26C dd = Haste
										 //  +270 dd = Slow
										 //  +274 dd = Slayer
										 //  +278 dd = Frenzy
										 //  +27C dd = Titan's Lightning Bolt
										 //  +280 dd = Counterstrike
										 //  +284 dd = Berserk
										 //  +288 dd = Hypnotize
										 //  +28C dd = Forgetfulness
										 //  +290 dd = Blind
										 //  +294 dd = Teleport
										 //  +298 dd = Remove Obstacle
										 //  +29C dd = Clone
										 //  +2A0 dd = Fire Elemental
										 //  +2A4 dd = Earth Elemental
										 //  +2A8 dd = Water Elemental
										 //  +2AC dd = Air Elemental

										 //  +2B0 dd = Stone   443D3A 43E0EB 441B0D (!= не отв. на атаку)
										 //  +2B4 dd = Poison
										 //  +2B8 dd = Bind
										 //  +2BC dd = Desease
										 //  +2C0 dd = Paralyze
										 //  +2C4 dd = Aging
										 //  +2C8 dd = Death Cloud
										 //  +2CC dd = Thunderbolt
										 //  +2D0 dd = Dispel
										 //  +2D4 dd = Death Stare
										 //  +2D8 dd = Acid Breath
									// +2DC dd*? (сила действия заклинания)
										 //  +2DC dd = Summon Boat
										 //  +2E0 dd = Scuttle Boat
										 //  +2E4 dd = Visions
										 //  +2E8 dd = View Earth
										 //  +2EC dd = Disguise
										 //  +2F0 dd = View Air
										 //  +2F4 dd = Fly
										 //  +2F8 dd = Water Walk
										 //  +2FC dd = Dimension Door
										 //  +300 dd = Town Portal

										 //  +304 dd = Quicksand
										 //  +308 dd = Land Mine
										 //  +30C dd = Force Field
										 //  +310 dd = Fire Wall
										 //  +314 dd = Earthquake
										 //  +318 dd = Magic Arrow
										 //  +31C dd = Ice Bolt
										 //  +320 dd = Lightning Bolt
										 //  +324 dd = Implosion
										 //  +328 dd = Chain Lightning
										 //  +32C dd = Frost Ring
										 //  +330 dd = Fireball
										 //  +334 dd = Inferno
										 //  +338 dd = Meteor Shower
										 //  +33C dd = Death Ripple
										 //  +340 dd = Destroy Undead
										 //  +344 dd = Armageddon
										 //  +348 dd = Shield
										 //  +34C dd = Air Shield
										 //  +350 dd = fire Shield
										 //  +354 dd = Protection from Air
										 //  +358 dd = Protection from Fire
										 //  +35C dd = Protection from Water
										 //  +360 dd = Protection from Earth
										 //  +364 dd = Anti-Magic
										 //  +368 dd = Dispel
										 //  +36C dd = Magic Mirror
										 //  +370 dd = Cure
										 //  +374 dd = Resurrection
										 //  +378 dd = Animate Dead
										 //  +37C dd = Sacrifice
										 //  +380 dd = Bless
										 //  +384 dd = Curse
										 //  +388 dd = Bloodlust
										 //  +38C dd = Precision
										 //  +390 dd = Weakness
										 //  +394 dd = Stone Skin
										 //  +398 dd = Disrupting Ray
										 //  +39C dd = Prayer
										 //  +3A0 dd = Mirth
										 //  +3A4 dd = Sorrow
										 //  +3A8 dd = Fortune
										 //  +3AC dd = Misfortune
										 //  +3B0 dd = Haste
										 //  +3B4 dd = Slow
										 //  +3B8 dd = Slayer
										 //  +3BC dd = Frenzy
										 //  +3C0 dd = Titan's Lightning Bolt
										 //  +3C4 dd = Counterstrike
										 //  +3C8 dd = Berserk
										 //  +3CC dd = Hypnotize
										 //  +3D0 dd = Forgetfulness
										 //  +3D4 dd = Blind
										 //  +3D8 dd = Teleport
										 //  +3DC dd = Remove Obstacle
										 //  +3E0 dd = Clone
										 //  +3E4 dd = Fire Elemental
										 //  +3E8 dd = Earth Elemental
										 //  +3EC dd = Water Elemental
										 //  +3F0 dd = Air Elemental

										 //  +3F4 dd = Stone   443D3A 43E0EB 441B0D (!= не отв. на атаку)
										 //  +3F8 dd = Poison
										 //  +3FC dd = Bind
										 //  +400 dd = Desease
										 //  +404 dd = Paralyze
										 //  +408 dd = Aging
										 //  +40C dd = Death Cloud
										 //  +410 dd = Thunderbolt
										 //  +414 dd = Dispel
										 //  +418 dd = Death Stare
										 //  +41C dd = Acid Breath
													 
										 // +444 dd -> 
										 // +448 dd (2) 
										 // +44C dd
										 // +454 dd 441B17 (кол-во ответов на атаку 0= не отв. на атаку)
// настройка для грифонов 46D6A0
// КОЛИЧЕСТВО ОТВЕТОВ НВ АТАКУ
										 // +458 dd Bless добавка к Max. Damage
										 // +45C dd Curse убавка к Min. Damage
										 // +464 dd Bloodlast добавка к Атаке с бонусами
										 // +468 dd Precision добавка к Атаке с бонусами
										 // +48C dd KING_123 тип (1=KING_1,2=KING_2,3=KING_3)
										 //   исп для расчета Slayer. Бонус 8 к Атаке: 0x4421D2     
										 // +490 dd номер атакера по порядку??? уже атаковал??? (сбрасывается после первого удара)
										 // +494 dd кол. доп. ответов на атаку, добавленных Counerstrike заклом
										 // +4A4 dd 
										 // +4C0 db Blinded - снизить защиту (сбросить после?) при атаке на него (уст. перед ударом)
										 // +4C1 db Paralized - снизить защиту (сбросить после?) при атаке на него (уст. перед ударом)
										 // +4C2 dd Forgetfulness - уровень (>2 - не может стрелять)
										 // +4DC dd = величина бонуса при выборе защиты
										 // +4E0 dd заклинание для сказ дракона
										 // +4EC dd 44152A
										 // +4F1 db 43DF88

										 // +514 dd
										 // +518 dd -> dd first \ adjusted stacks pointers
										 // +51C dd -> dd last  /

										 // +524 dd
										 // +528 dd -> dd first \ adjusted to wich stacks pointers
										 // +52C dd -> dd last  /
};

// сменить хозяина шахты
//:004A380A 8B4508         mov    eax,[ebp+08] -> ук. на клетку входа на карте
//:004A380D8B15F4CC6900   mov    edx,[0069CCF4] = новый номер хозяина
//:004A3813 6A01           push   00000001
//:004A3815 52             push   edx  = новый номер хозяина
//:004A3816 8B08           mov    ecx,[eax] = номер шахты в списке по-порядку
//:004A3818 51             push   ecx  = номер
//:004A3819 8B0D38956900   mov    ecx,[00699538] = [BASE]
//:004A381F E87C2B0200     call   H3.004C63A0

// вход в любой тип объекта: 4A8160
// игрок
//:004AA75C 8B550C         mov    edx,[ebp+0C]
//:004AA75F 6A01           push   00000001 = GM/AI
//:004AA761 52             push   edx      = 0x08A002 ?
//:004AA762 53             push   ebx      -> ук на клетку входа в объект
//:004AA763 57             push   edi      -> ук на героя
//:004AA764 8BCE           mov    ecx,esi  -> adv. Manager
//:004AA766 E8F5D9FFFF     call   H3.004A8160
// AI    4AC990
//:004AC9F4 8D4510         lea    eax,[ebp+10]
//:004AC9F7 8B00           mov    eax,[eax]
//:004AC9F9 6A00           push   00000000
//:004AC9FB 52             push   edx
//:004AC9FC 8B5508         mov    edx,[ebp+08]
//:004AC9FF 52             push   edx
//:004ACA00 56             push   esi
//:004ACA01 89464D         mov    [esi+4D],eax
//:004ACA04 E857B7FFFF     call   H3.004A8160

// 42FC50 - вызывается на каждом ходу AI для дальнейшего анализа

// перерисовка во время смены игрока
//:004C73DA 8B0D30956900   mov    ecx,[00699530]
//:004C73E0 E85B520200     call   H3.004EC640

// 506170 - настраивает вид объектов на карте

// Атака
//   точка входа всегда 4AD160
//   главная функция 4786B0 - по очереди каждый монстр.

// Гарпии летят назад:
//   проверка 47832B
//   полет назад 478360

// Колдовство чародея перед атакой
//   вход в функцию 447D00
//   проверка 447ED9
//   колдовство 447F5A
//   показ магии 447F70
//   магия "Жертва" 5A85C4
//   остальные магии, переход по таблице 5A8655, 4477D8

// Самая первая функция перед атакой.
// Привидения (отъем маны)
//   проверка 4650D0
// Сказочный дракон (какой-то бонус)
//   проверка 4650D5
// Чародей
//   проверка 4650DA

// тип атаки задается в 478952
// 1 - колдует герой
// 2 - монстр идет
// 3 - стоит на месте
// 6 - доходит и бьет
// 7 - стреляет (чародеи)
// 8 - ждать
// A - магический удар (сказ. драконы)
// B - лечить палаткой

// проверяет на наличие импов для переноса маны 5A24CF


// проверка на AI игрока
// mov    eax,[0069CCFC]
// mov    cl,[eax+000000E2]
// test   cl,cl
// je     H3.004CC52F (004CC52F)
// mov    cl,[eax+000000E1]
// test   cl,cl
// je     H3.004CC52F (004CC52F)

// установка 7 дней до смерти после потери города
//004F356B C60307         mov    byte ptr [ebx],07
// 4cc490 - процедура анализа

// добавляет ресурс игроку
//  mov    ecx,[esi] = количество
//  push   ecx
//  mov    ecx,[ebp+08] -> структ. героя (от него нужен только хозяин)
//  push   edi = номер ресурса
//  call   H3.004E3870

// перерисовка состояния героя после изменений на карте?
//  push   00000000
//  push   00000000
//  mov    ecx,ebx -> advManager
//  call   H3.0040F1D0

// перерисовывает экран advanture
//:004A34F8 8B45FC         mov    eax,[ebp-04] -> advManager
//:004A34FB 6A01           push   00000001
//:004A34FD 6A01           push   00000001
//:004A34FF 6AFF           push   FFFFFFFF
//:004A3501 8B4844         mov    ecx,[eax+44]
//:004A3504E8D7FDF5FF     call   H3.004032E0
// перерисовывает правое нижнее окно карты
//:004A3509 8B4DFC         mov    ecx,[ebp-04] -> advManager
//:004A350C 6A01           push   00000001
//:004A350E 6A01           push   00000001
//:004A3510 6A01           push   00000001
//:004A3512 E82928F7FF     call   H3.00415D40

// 4AA75C - набор функций по перерисовке ?

// перерис. ресурсы
//    mov    ecx, -> advManager
//004AA7C1 8B4E44         mov    ecx,[esi+44]
//004AA7C4 6A01           push   00000001
//004AA7C6 6A01           push   00000001
//004AA7C8 E83397F5FF     call   H3.00403F00

// проверка на умирание егрока
// ecx -> BASE
// esi = номер игрока
//004F341E8A843136F60100 mov    al,[ecx+esi+0001F636]
//004F3425 84C0           test   al,al
//004F3427 0F85F5010000   jne    H3.004F3622
//004F342D 8A840FD10A0200 mov    al,[edi+ecx+00020AD1]
//004F3434 84C0           test   al,al
//004F3436 8A840F0E0B0200 mov    al,[edi+ecx+00020B0E]
//004F343D 0F85B0000000   jne    H3.004F34F3
//004F3443 84C0           test   al,al
//004F3445 0F85C9010000   jne    H3.004F3614
//004F344B 8BCE           mov    ecx,esi
//004F344D E8DEE3FFFF     call   H3.004F1830
//004F3452 8B0D38956900   mov    ecx,[00699538]
//004F3458 E883B2FDFF     call   H3.004CE6E0
//004F345D 3BF0           cmp    esi,eax
//004F345F 7538           jne    H3.004F3499 (004F3499)
//004F3461 A1C45D6A00     mov    eax,[006A5DC4]

// генерация артефакта уровня в eax (2,4,8,0x10)
//:004C1C11 8B0D38956900   mov    ecx,[00699538]
//:004C1C17 50             push   eax
//:004C1C18 E873750000     call   H3.004C9190

// проверка на возможность обучения в универе
//004A2F55 83F80A         cmp    eax,0000000A

// считает сколько надо очков до след. уровня в eax
//:004A6425 0FBF4E55       movsx  ecx,word ptr [esi+55] = опыт
//:004A6429 E862420300     call   H3.004DA690

// добавление монолита в нвчале
//:004C1667 8B0D38956900   mov    ecx,[00699538]
//:004C166D 8A81E8E30400   mov    al,[ecx+0004E3E8]
//:004C1673 3C30           cmp    al,30        всего
//:004C1675 0F8DB4070000   jnl    H3.004C1E2F
//:004C167B 0FBED0         movsx  edx,al
//:004C167E 8916           mov    [esi],edx    настройка на карте - номер
//:004C1680 A138956900     mov    eax,[00699538]
//:004C1685 FE80E8E30400   inc    byte ptr [eax+0004E3E8] увеличим общее колво на 1

// отъем перемещения при посещении леб. озера
//:004A88CBC7474D00000000 mov    dword ptr [edi+4D],00000000

// +8 перемещения в оазисе
//:004A39E8 8B5649         mov    edx,[esi+49]
//:004A39EB 8B4E4D         mov    ecx,[esi+4D]
//:004A39F4 B820030000     mov    eax,00000320
//:004A39F9 03D0           add    edx,eax
//:004A39FB 03C8           add    ecx,eax
//:004A3A0B 895649         mov    [esi+49],edx
//:004A3A0E 894E4D         mov    [esi+4D],ecx

// добавление опыта герою
////:004A603F 6A01           push   00000001
////:004A6041 6A00           push   00000000
////:004A6043 56             push   esi        = очки опыта
////:004A6044 8BCF           mov    ecx,edi    -> стр. героя
////:004A6046E8D5D50300     call   H3.004E3620
//... к [ecx+51] добавить опыт
//:004A6098 8BCF           mov    ecx,edi    -> стр. героя
//:004A609A E8F1480300     call   H3.004DA990

// номера ф-й перехода по (типам-5)
//  5: 00 01 19 02 19 19 19 19 19 19 19 19 03 19 19 03
// 21: 19 19 19 19 19 04 19 19 19 19 19 19 05 06 19 07
// 37: 19 19 19 19 19 08 19 19 19 19 19 19 19 19 19 19
// 53: 09 0A 19 19 19 19 0B 19 19 06 19 19 00 00 00 00
// 69: 00 06 0A 0A 0A 0A 0A 0C 0D 19 0C 19 0E 19 0F 19
// 85: 19 19 10 11 11 11 0B 19 12 19 19 19 19 0D 19 19
//101: 19 19 19 19 19 19 19 19 19 19 19 19 13 19 19 19
//117: 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19
//133: 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19
//149: 19 19 19 19 19 19 19 19 19 19 19 19 19 0A 0A 0A
//165: 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19
//181: 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19
//197: 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19
//213: 19 14 15 16 17 18
// сами функции перехода (0x50353F):
// 0 46 35 50 00 +5=арт на карте 65,66,67,68,69=случ.арт
// 1 75 37 50 00 -6=ящик пондоры
// 2 78 35 50 00 -8=лодка
// 3 B6 38 50 00 +17=жил монстра,20=жм новые
// 4 F0 35 50 00 -26=LE
// 5 CD 38 50 00 +33=H гарнизон, [219=V гарнизон]
// 6 5D 35 50 00  34=герой 62=тюрьма 70=случ герой
// 7 03 37 50 00 +36=грааль
// 8 9F 38 50 00 +42=маяк
// 9 80 38 50 00 +53=шахта
// A D9 35 50 00 +54=монстр 71,72,73,74,75=случ монстр 162,163,164=спец.случ.мон
// B 69 38 50 00 +59=бут на воде 91=знак
// C EC 36 50 00 +76=случ.рес 79=конкр.рес
// D BE 35 50 00 -77=случ.город 98=конкр.город
// E 90 37 50 00 *81=ученый
// F A7 37 50 00 83=Seer
//10 97 36 50 00 +87=верфь
//11 1F 38 50 00 *88,89,90=Sgrine беседка с обучением закл.
//12 80 36 50 00 +93=свиток с закл.
//13 FF 3A 50 00 +113=хиж.ведьмы
//14 0B 36 50 00 -214=AHplace.def (Переходящий Герой)
//15 A4 3A 50 00 215=хранитель вопроса
//16 E8 38 50 00 -216=случ.жил. любое
//17 83 39 50 00 -217=с.ж.по уровню
//18 0F 3A 50 00 -218=с.ж.по городам
//19 33 3B 50 00 ???=все остальные

//   4: 00 01 02 03 37 37
//  10: 37 37 04 37 37 37 05 06 07 08
//  20: 09 37 0A 0B 0C 0D 37 37 37 0E
//  30: 0F 37 10 11 37 37 37 37 37 12
//  40: 37 13 14 37 15 16 37 17 18 37
//  50: 37 19 37 1A 1B 1C 37 1D 37 37
//  60: 37 1E 37 1F 37 37 37 37 37 37
//  70: 37 37 37 37 37 37 37 37 20 21
//  80: 37 22 23 37 24 25 26 27 28 29
//  90: 2A 37 37 37 37 37 37 37 37 37
// 100: 2B 2C 2D 2E 2F 30 37 31 32 33
// 110: 37 34 35 36
// сами функции перехода 2 (0x4С0A6D):
//  0: 74 0A 4C 00   4 Arena
//  1: 88 0A 4C 00   5 Artifact
//  2: AD 0A 4C 00   6 Pandora's Box
//  3: F1 0A 4C 00   7 Black Market
//  4: A5 0B 4C 00  12 Campfire
//  5: CC 0B 4C 00  16 Creature Bank
//  6: 77 0C 4C 00  17 Creature Generator 1
//  7: ED 0C 4C 00  18 Creature Generator 2
//  8: 26 0D 4C 00  19 Creature Generator 3
//  9: 5F 0D 4C 00  20 Creature Generator 4
//  A: F1 0D 4C 00  22 Corpse
//  B: 58 0E 4C 00  23 Marletto Tower
//  C: 66 0E 4C 00  24 Derelict Ship
//  D: 76 10 4C 00  25 Dragon Utopia
//  E: 26 11 4C 00  29 Flotsam
//  F: 39 11 4C 00  30 Fountain of Fortune
// 10: 7C 11 4C 00  32 Garden of Revelation
// 11: 8A 11 4C 00  33 Garrison
// 12: 3A 12 4C 00  39 Lean To
// 13: 8A 12 4C 00  41 Library of Enlightenment
// 14: 98 12 4C 00  42 Lighthouse
// 15: 13 13 4C 00  44 Monolith One Way Exit
// 16: 0C 14 4C 00  45 Monolith Two Way
// 17: 7B 14 4C 00  47 School of Magic
// 18: 89 14 4C 00  48 Magic Spring
// 19: 2A 15 4C 00  51 Mercenary Camp
// 1A: 38 15 4C 00  53 Mine
// 1B: E3 15 4C 00  54 Monster
// 1C: 25 16 4C 00  55 Mystical Garden
// 1D: 67 16 4C 00  57 Obelisk
// 1E: 90 16 4C 00  61 Star Axis
// 1F: 9E 16 4C 00  63 Pyramid
// 20: 62 17 4C 00  78 Refugee Camp
// 21: 91 17 4C 00  79 Resource
// 22: E9 17 4C 00  81 Scholar
// 23: 54 18 4C 00  82 Sea Chest
// 24: 16 0F 4C 00  84 Crypt
// 25: C6 0F 4C 00  85 Shipwreck
// 26: B2 18 4C 00  86 Shipwreck Survivor
// 27: 0D 19 4C 00  87 Shipyard
// 28: 68 19 4C 00  88 Shrine of Magic Incantation
// 29: B5 19 4C 00  89 Shrine of Magic Gesture
// 2A: 19 1A 4C 00  90 Shrine of Magic Thought
// 2B: 84 1A 4C 00 100 Learning Stone
// 2C: 98 1A 4C 00 101 Treasure Chest
// 2D: 1D 1B 4C 00 102 Tree of Knowledge
// 2E: 81 13 4C 00 103 Subterranean Gate
// 2F: 55 1B 4C 00 104 University
// 30: 62 1B 4C 00 105 Wagon
// 31: C8 1B 4C 00 107 School of War
// 32: DC 1B 4C 00 108 Warrior's Tomb
// 33: 38 1C 4C 00 109 Water Wheel
// 34: 48 1C 4C 00 111 Whirlpool
// 35: 00 1D 4C 00 112 Windmill
// 36: 3D 1D 4C 00 113 Witch Hut
// 37: 2F 1E 4C 00 все остальные

// 0 46 35 50 00 +5=арт на карте 65,66,67,68,69=случ.арт
// 1 75 37 50 00 -6=ящик пондоры
// 2 78 35 50 00 -8=лодка
// 3 B6 38 50 00 +17=жил монстра,20=жм новые
// 4 F0 35 50 00 -26=LE
// 5 CD 38 50 00 +33=H гарнизон, [219=V гарнизон]
// 6 5D 35 50 00  34=герой 62=тюрьма 70=случ герой
// 7 03 37 50 00 +36=грааль
// 8 9F 38 50 00 +42=маяк
// 9 80 38 50 00 +53=шахта
// A D9 35 50 00 +54=монстр 71,72,73,74,75=случ монстр 162,163,164=спец.случ.мон
// B 69 38 50 00 +59=бут на воде 91=знак
// C EC 36 50 00 +76=случ.рес 79=конкр.рес
// D BE 35 50 00 -77=случ.город 98=конкр.город
// E 90 37 50 00 *81=ученый
// F A7 37 50 00 83=Seer
//10 97 36 50 00 +87=верфь
//11 1F 38 50 00 *88,89,90=Sgrine беседка с обучением закл.
//12 80 36 50 00 +93=свиток с закл.
//13 FF 3A 50 00 +113=хиж.ведьмы
//14 0B 36 50 00 -214=AHplace.def
//15 A4 3A 50 00 215=хранитель вопроса
//16 E8 38 50 00 -216=случ.жил. любое
//17 83 39 50 00 -217=с.ж.по уровню
//18 0F 3A 50 00 -218=с.ж.по городам
//19 33 3B 50 00 ???=все остальные

// соответствие монстров (шест.) к индексам двелингов
// 6A 60 4A 42 44 0A 0E 70 0C 5E
// 36 68 10 71 34 12 72 1E 24 56
// 62 54 2C 66 1A 04 48 2E 6E 2A
// 64 22 50 4C 4E 08 26 30 5A 58
// 32 52 5C 1C 28 16 46 73 3C 6C
// 14 18 40 3E 38 3A 00 02 06 76
// 78 82 84 85 86 87 88 89 18 70
// 71 72 73 8A 8B 8C 8D 8E 8F 90

// 4C8C00 - генерация монстров в конце месяца
// [697798] -тип монстра для генерации

// 4C0980 - функция настройки объектов на карте после загрузки

// 4C413B - загрузка типа карты для новой
// 5849С7 - показ соотв. иконки

// 448263 переход по конкретной магии монстра
// 4482DE воскрешение монстров
// 4482D3 вычисляет количество хитпоинтов для восстановления как
//        N*25*k (k=4)
// 4482D9 C1.E0.02 shl eax,2 (2 меняем на то, что надо)

// делает черным экран adventure
//:004626EF 8B0DD0926900   mov    ecx,[006992D0]
//:004626F5 6A01           push   00000001
//:004626F7 6A04           push   00000004
//:004626F9 6A01           push   00000001
//:004626FB 8945E8         mov    [ebp-18],eax
//:004626FE 8955EC         mov    [ebp-14],edx
//:00462701 E80A0B1A00     call   H3.00603210

// 462600 - строит все структуры перед битвой
// проверка на пузырек с кровью драконов перед атакой
//:004E6400 83387F cmp    dword ptr [eax],0000007F

// настройка монстра если есть герой хозяин (с артиф)
//:0043D4D0 56             push   esi
//:0043D4D1 50             push   eax
//:0043D4D2 E8B98E0A00     call   H3.004E6390

// рисует хар-ки монстра в маленьком окошечке на поле боя
//:00475039 E8128AFFFF     call   H3.0046DA50

// процедура рисования книги закл на монстре во время битвы
//:004470F0 55             push   ebp
// вызов
//:004761D2 8B4508         mov    eax,[ebp+08] // инд. поз на поле
//:004761D5 8BCF           mov    ecx,edi      // структ монстра
//:004761D7 50             push   eax
//:004761D8E8130FFDFF     call   H3.004470F0
//:004761DD 84C0           test   al,al        // не ноль, если книга
//:004761DF 745B           je     H3.0047623C (0047623C)

// еастройка каждого монстра перед битвой
// ecx->структ
//:00463301E8CAA2FDFF     call   H3.0043D5D0
//:00463306 8B4DF4         mov    ecx,[ebp-0C] ->структ монстра
//:00463309 E802A4FDFF     call   H3.0043D710
//:0046330E 8B45F4         mov    eax,[ebp-0C]

// книга, если сказ. дракон
//:0047602C817F3486000000 cmp    dword ptr [edi+34],00000086

// переход по типу магии для монстра
//:00447462 8B4634         mov    eax,[esi+34]
//:00447465 8D48F3         lea    ecx,[eax-0D]
//:00447468 83F979         cmp    ecx,00000079
//:0044746B0F8781020000   ja     H3.004476F2
//:00447471 33D2           xor    edx,edx
//:00447473 8A9120774400   mov    dl,[ecx+00447720]
//:00447479 FF2495FC764400 jmp    [4*edx+004476FC]
//:00447720 00 08 08 08 08 08 08 08 08 08 08 08 08 08 08 08
//:00447730 08 08 08 08 08 08 08 08 01 08 08 08 08 08 08 08
//:00447740 08 08 08 08 08 08 00 08 08 08 08 08 08 08 08 08
//:00447750 08 08 08 08 08 08 08 08 08 08 08 08 08 08 08 08
//:00447760 08 08 08 08 08 08 08 08 08 08 08 08 08 08 02 08
//:00447770 08 08 08 08 08 08 08 08 08 08 08 08 08 08 08 08
//:00447780 08 08 08 08 08 08 08 08 08 08 08 08 08 08 03 08
//:00447790 04 08 05 08 06 08 08 08 08 07

// переход по конкретной магии монстра
//048248: 83C0F3                       add    eax,-00D
//04824B: 4A                           dec    edx
//04824C: 83F879                       cmp    eax,079  ;"y"
//04824F: 8996DC000000                 mov    [esi][0000000DC],edx
//048255: 0F8782010000                 ja     0000483DD   ---------
//04825B: 33C9                         xor    ecx,ecx
//04825D: 8A8888844400                 mov    cl,[eax][000448488]
//048263: FF248D60844400               jmp    d,[000448460][ecx]*4
//5E 83 44 00-DD 83 44 00-00 09 09 09-09 09 09 09
//09 09 09 09-09 09 09 09-09 09 09 09-09 09 09 09
//01 09 09 09-09 09 09 09-09 09 09 09-09 09 02 09
//09 09 09 09-09 09 09 09-09 09 09 09-09 09 09 09
//09 09 09 09-09 09 09 09-09 09 09 09-09 09 09 09
//09 09 09 09-09 09 03 09-09 09 09 09-09 09 09 09
//09 09 09 09-09 09 09 09-09 09 09 09-09 09 09 09
//09 09 09 09-09 09 04 09-05 09 06 09-07 09 09 09
//09 08 90 90-90 90 90 90-90 90 90 90-90 90 90 90

// начальная настройка монстров
//:00463BED 8A86C5530000   mov    al,[esi+000053C5]
//:00463BF3 8BCE           mov    ecx,esi
//:00463BF5 50             push   eax
//:00463BF6E8E5F5FFFF     call   H3.004631E0

// тип монстра, который восстанавливается на месте убитого
//:005A776A 6A0D           push   0000000D

// коррекция защиты для чудища
//:004422E0 8B4034         mov    eax,[eax+34]
//:004422E3 83F860         cmp    eax,00000060
//:004422E6 7511           jne    H3.004422F9 (004422F9)
//:004422E8 DB450C         fild   dword ptr[ebp+0C]
//:004422EB D95D0C         fstp   dword ptr[ebp+0C]
//:004422EE D9450C         fld    dword ptr[ebp+0C]
//:004422F1 D80DB0B86300   fmul   dword ptr[H3.0063B8B0]
//:004422F7 EB14           jmp    H3.0044230D (0044230D)
//:004422F9 83F861         cmp    eax,00000061
//:004422FC 7519           jne    H3.00442317 (00442317)
//:004422FE DB450C         fild   dword ptr[ebp+0C]
//:00442301 D95D0C         fstp   dword ptr[ebp+0C]
//:00442304 D9450C         fld    dword ptr[ebp+0C]
//:00442307 D80DACB86300   fmul   dword ptr[H3.0063B8AC]
//:0044230D D86D0C         fsubr  dword ptr[ebp+0C]
//:00442310 E87F5C1D00     call   H3.00617F94
//:00442315 8BD8           mov    ebx,eax

// покадровое воспроизведение дефа магии на поле
//00496523 8B5D10         mov    ebx,[ebp+10]
//00496526 8B86E8320100   mov    eax,[esi+000132E8]
//0049652C 8B4828         mov    ecx,[eax+28]
//0049652F 85C9           test   ecx,ecx
//004965317E11           jle    H3.00496544 (00496544)
//00496533 8B482C         mov    ecx,[eax+2C]
//00496536 833900         cmp    dword ptr [ecx],00000000
//00496539 7409           je     H3.00496544 (00496544)
//0049653B 8B501C         mov    edx,[eax+1C]
//0049653E 8B02           mov    eax,[edx]
//00496540 8B00           mov    eax,[eax]
//00496542 EB02           jmp    H3.00496546 (00496546)
//00496544 33C0           xor    eax,eax
//00496546 3BF8           cmp    edi,eax
//00496548 7D1B           jnl    H3.00496565 (00496565)
//0049654A 6A01           push   00000001
//0049654C 6A01           push   00000001
//0049654E 53             push   ebx
//0049654F 6A00           push   00000000
//00496551 6A00           push   00000000
//00496553 6A01           push   00000001
//00496555 8BCE           mov    ecx,esi
//00496557 89BEF0320100   mov    [esi+000132F0],edi
//0049655D E85EDAFFFF     call   H3.00493FC0
//00496562 47             inc    edi
//00496563 EBC1           jmp    H3.00496526 (00496526)

// переход по заклинаниям на монстров (спецмагия, огн. щит и т.п.)
//:004446FB 8A9978514400   mov    bl,[ecx+00445178]
//:00444701FF249DF4504400 jmp    [4*ebx+004450F4] 

// переход по проверке на возможность заклинания
//:005A86668A881C895A00   mov    cl,[eax+005A891C]
//:005A866C FF248D00895A00 jmp    [4*ecx+005A8900]

// след. переход по проверке на возможность заклинания
//:0044A2628A88FCA54400   mov    cl,[eax+0044A5FC]
//:0044A268 FF248DB8A54400 jmp    [4*ecx+0044A5B8]

// след. переход по проверке на возможность заклинания
//:0044A4B18A8858A64400   mov    cl,[eax+0044A658]
//:0044A4B7 FF248D34A64400 jmp    [4*ecx+0044A634]

// 4EDE3D - тыкание в артифакты на теле
// lea    esi,[eax-02]   // esi = номер ячейки на теле
// mov    eax,[00698B70] // -> герой хозяин
// or     edi,FFFFFFFF
// mov    [ebp-10],esi
// mov    ecx,[eax+8*esi+0000012D] // ноиер арт
// mov    [ebp-1C],ecx
// mov    edx,[eax+8*esi+00000131] // ??? = -1 в слоте
// mov    [ebp-18],edx
// mov    edx,[00698AD8] // = -1
// cmp    edx,edi
// jne    H32.004DE1BB // если не равны, то выход
// cmp    ecx,edi
// je     H32.004DD5B9 // нет артифакта ?
// test   bl,bl        // 0-левая мышь, 1-правая
// je     H32.004DE0C9
// mov    edx,[00699538] // = BASE
// cmp    dword ptr [edx+0001F698],00000002 //
// jl     H32.004DE0AF // переход, если до СоД (сост.арт.)
// mov    edi,[00660B68] // -> табл. артиф
// mov    edx,ecx
// shl    edx,05
// add    edx,edi
// mov    edi,[edx+14] // номер составного арт
// mov    ebx,[edx+18] // сам является частью сост. арт
// cmp    edi,FFFFFFFF
// je     H32.004DDFE9
// lea    eax,[ebp-2C] // -> ук. на буфер
// mov    [ebp-14],ecx // номер арт
// or     ebx,FFFFFFFF // ? не часть составного
// push   eax
// lea    ecx,[ebp-14] // -> dd=номер арт, dd=-1
// mov    [ebp-10],ebx 
// ...
// разбираем арт
//:004DDF4F8B0D708B6900   mov    ecx,[00698B70]
// маски битов арт. : [0x660B6C]
// dd = номер арт для этого сост арт.
// dd*5 = один бит = один артифакт.
// мод. перв скилов артиф.: 63E758 проверка конца = 63E998
// db*4 мод скилов

// добавление заклинания из свитка в книгу
//:004D987A 8B10           mov    edx,[eax]
//:004D987C 8B4004         mov    eax,[eax+04]
//:004D987F 83FAFF         cmp    edx,FFFFFFFF
//:004D98820F84AD010000   je     H32.004D9A35
//:004D9888 83FA01         cmp    edx,00000001
//:004D988B 750C           jne    H32.004D9899 (004D9899)
//:004D988D 88940330040000 mov    [ebx+eax+00000430],dl
//:004D9894 E99C010000     jmp    H32.004D9A35

// переход по типу заклинания при добавлении артифакта
//:004D95DB 8D46AA         lea    eax,[esi-56]
//:004D95DE 83F831         cmp    eax,00000031
//:004D95E1 0F87B6010000   ja     H32.004D979D
//:004D95E733C9           xor    ecx,ecx
//:004D95E9 8A8800984D00   mov    cl,[eax+004D9800]
//:004D95EF FF248DDC974D00 jmp    [4*ecx+004D97DC]

// переход по загрузке оружия монстра
//0003DA8E: 8B4334                       mov    eax,[ebx][00034]
//0003DA91: 83C0FE                       add    eax,-002
//0003DA94: 3D90000000                   cmp    eax,000000090
//0003DA99: 777F                         ja     00003DB1A   ---------
//0003DA9B: 33C9                         xor    ecx,ecx
//0003DA9D: 8A88A4DB4300                 mov    cl,[eax][00043DBA4]
//0003DAA3: FF248D60DB4300               jmp    d,[00043DB60][ecx]*4

// переход по определению действует магия или нет на монстра
//0044A4A4 8B55F8         mov    edx,[ebp-08] // номер монстра
//0044A4A7 8D42F0         lea    eax,[edx-10]
//0044A4AA83F875         cmp    eax,00000075
//0044A4AD 7716           ja     H3.0044A4C5 (0044A4C5)
//0044A4AF 33C9           xor    ecx,ecx
//0044A4B1 8A8858A64400   mov    cl,[eax+0044A658]
//0044A4B7 FF248D34A64400 jmp    [4*ecx+0044A634]

// тип существа, которого боятся (лазурный дракон)
//:00464AC0 837DFC01       cmp    dword ptr [ebp-04],00000001
//:00464AC4 7508           jne    H3.00464ACE (00464ACE)
//:00464AC6 8B82E43B0000   mov    eax,[edx+00003BE4]
//:00464ACC EB06           jmp    H3.00464AD4 (00464AD4)
//:00464ACE 8B82E83B0000   mov    eax,[edx+00003BE8]

// добавление университета
//:004C1B5556             push   esi
//:004C1B56 8BCB           mov    ecx,ebx
//:004C1B58 E853E8FFFF     call   H3.004C03B0
//:004C1B5D E9CD020000     jmp    H3.004C1E2F

// переход для показа подсказки по объекту
//:0040B1688A8874D24000   mov    cl,[eax+0040D274]
//:0040B16E FF248D88D14000 jmp    [4*ecx+0040D188]

// проверка на сравнение строк при поиске уже подгруженного файла
//:0055EE2853             push   ebx
//:0055EE29 50             push   eax
//:0055EE2A E891A90B00     call   H3.006197C0

// выбор сообщения в начале новой недели
//:004CC9E9 8B0D4C846900   mov    ecx,[0069844C] номер существа, чья неделя
//:004CC9EFA124786A00     mov    eax,[006A7824] само сообщение "... %s ..."
//:004CC9F4 8B148D78776A00 mov    edx,[4*ecx+006A7778] название существа
//:004CC9FB 52             push   edx
//:004CC9FC 50             push   eax
//:004CC9FD 6828746900     push   00697428 -> буфер
//:004CCA02 E8D7AF1400     call   H3.006179DE
//:004CCA07 83C40C         add    esp,0000000C

// выбор типа недели
//:004C846B BA04000000     mov    edx,00000004 если 4-я неделя, то новый месяц
//:004C8470 A34C846900     mov    [0069844C],eax от 0 до 0E номер не сущ. существа
//:004C8475 66399740F60100 cmp    [edi+0001F640],dx // конец месяца?
//:004C847C C745F405000000 mov    dword ptr [ebp-0C],00000005
//:004C84830F8407010000   je     H3.004C8590 // новый месяц
//:004C8489 B901000000     mov    ecx,00000001
//:004C848E E82D430400     call   H3.0050C7C0
//:004C8493 83F801         cmp    eax,00000001
//:004C8496 0F85F4000000   jne    H3.004C8590  0-обычная неделя, 1-неделя существа
//... установка типа монстра для недели типа 1
//:004C8588A34C846900     mov    [0069844C],eax

// обновление всех генераторов существ в начале недели
//:004C878B 8B8B9CE30400   mov    ecx,[ebx+0004E39C]
//:004C8791 6A00           push   00000000
//:004C8793 03CF           add    ecx,edi
//:004C8795 E8C6FFFEFF     call   H3.004B8760
//:004C879A 46             inc    esi
//:004C879B 83C75C         add    edi,0000005C

// обновление объектов на карте в начале недели
//:004C883E 8B461E         mov    eax,[esi+1E]
//:004C884183C0E2         add    eax,FFFFFFE2
//:004C8844 83F852         cmp    eax,00000052
//:004C8847 0F873D010000   ja     H3.004C898A
//:004C884D 33C9           xor    ecx,ecx
//:004C884F 8A88A48B4C00   mov    cl,[eax+004C8BA4]
//:004C8855 FF248D848B4C00 jmp    [4*ecx+004C8B84]

// сбрасывает посещение конюшни в конце недели для героя
//:004C89DF 8D8325170200   lea    eax,[ebx+00021725]
//:004C89E5 BA9C000000     mov    edx,0000009C
//:004C89EA 8B08           mov    ecx,[eax]
//:004C89EC F6C102         test   cl,02
//:004C89EF 7405           je     H3.004C89F6 (004C89F6)
//:004C89F1 83C1FE         add    ecx,FFFFFFFE
//:004C89F4 8908           mov    [eax],ecx
//:004C89F60592040000     add    eax,00000492
//:004C89FB 4A             dec    edx
//:004C89FC 75EC           jne    H3.004C89EA (004C89EA)

// схватка в пирамиде
//:004A3F99 8B5510         mov    edx,[ebp+10] MixedPos
//:004A3F9C 6A00           push   00000000
//:004A3F9E 6A00           push   00000000
//:004A3FA0 6AFF           push   FFFFFFFF
//:004A3FA2 6A02           push   00000002
//:004A3FA4 6A14           push   00000014
//:004A3FA6 6A75           push   00000075
//:004A3FA8 52             push   edx
//:004A3FA9 8D4508         lea    eax,[ebp+08] ->[->Hero]
//:004A3FAC 53             push   ebx ->MapItem
//:004A3FAD 50             push   eax
//:004A3FAE6A74           push   00000074
//:004A3FB0 57             push   edi ->Hero
//:004A3FB1 8BCE           mov    ecx,esi ->AdvManager
//:004A3FB3 C7450828000000 mov    dword ptr [ebp+08],00000028
//:004A3FBA E8B1820000     call   H3.004AC270
//:004A3FBF 85C0           test   eax,eax
//:004A3FC1 0F855A010000   jne    H3.004A4121

// показ начального SMK
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
// менять надо 597870

// утака стресковой башней
//0043FEE2 52             push   edx  // номер монстра на кого ударяет в ряду с 0
//0043FEE3 E8C8570200     call   H3.004656B0

// показ самого первого сообщения после загрузки карты
//:004F04F0 8B0D38956900   mov    ecx,[00699538]
//:004F04F6 E8D5D0FDFF     call   H3WOG.004CD5D0
// вызов главного цикла - вся игра там
//:004F0510 8B0D50956900   mov    ecx,[00699550]
//:004F0516 E88506FCFF     call   H3WOG.004B0BA0
// выбор в главном меню
//:004EF331 E8EA67FEFF     call   H3WOG.004D5B20
// выбор в картовом меню
//:004F0B6A E851430900     call   H3WOG.00584EC0
// переход по типу загрузки игры (все)
//:004EF287 0F87FE0B0000   ja     H3WOG.004EFE8B
//:004EF28DFF248524064F00 jmp    [4*eax+004F0624] 
// переход по типу объекта для показа конкретной картинки по правой мыше
//:0040A6F6FF2495E0AA4000 jmp    [4*edx+0040AAE0] 
// показывает картинку замка в опред. позиции
//:0040A762E899BF0000     call   H3WOG.00416700
// определяет насколько полна инфа о городе по правой мыши
//:00416DF0E8FB5C0B00     call   H3WOG.004CCAF0
//:00416DF5 83F802         cmp    eax,00000002
//:00416DF8 7C07           jl     H3WOG.00416E01
//:00416DFA B802000000     mov    eax,00000002
//:00416DFF EB1D           jmp    H3WOG.00416E1E
//:00416E01 8B0D38956900   mov    ecx,[00699538]
//:00416E07 57             push   edi
//:00416E08 E8E35C0B00     call   H3WOG.004CCAF0
//:00416E0D 33C9           xor    ecx,ecx
//:00416E0F 83F801         cmp    eax,00000001
//:00416E12 0F9DC1         setnl  cl
//:00416E15 8BC1           mov    eax,ecx
//:00416E17 EB05           jmp    H3WOG.00416E1E
//:00416E19 B803000000     mov    eax,00000003
//:00416E1E 50             push   eax

// устанавливает тип для фиксированных компаний СоД
//:00584A2CC745FC02000000 mov    dword ptr [ebp-04],00000002

// настройка LE перед помещением на карту
//00B674C0 02 00 00 80 00 00 00 00 00 00 00 00 00 00 00 00
//00B674D0 00 00 00 00 FF FF FF FF FF FF FF FF FF FF FF FF
//00B674E0 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
//00B674F0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//00B67500 00 00 00 00 00 00 00 00 00 00 00 00 00 77 B6 00
//00B67510 00 00 00 00 00 00 00 00 00 00 81 20 00 00 00 00
//00B67520 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//00B67530 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//00B67540 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF FF
//00B67550 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00
//00B67560 00 00 00 00 00 00 00 00 00 00 00 00 FF FF FF FF
//00B67570 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
//00B67580 FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00
//00B67590 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//00B675A0 00 00 00 00

// get name of artefact
//  mov    eax,0x660B68
//  mov    eax,[eax]
//  mov    edx,???   = art. number
//  shl    edx,5
//  mov    esi,[edx+eax] -> name of artefact

// get name of spell
// mov    edx,0x687FA8
// mov    edx,[edx]
// mov    ecx,eax   = spell number
// shl    ecx,04
// add    ecx,eax
// mov    esi,[edx+8*ecx+10] -> name of spell

//  имя строения в определенном городе
//  mov    ecx,???  = номер типа города + 24 (0x16)
//  sub    ecx,00000016
//  mov    edx,???  = подтип строения
//  mov    eax,0x460CC0
//  call   eax  = eax -> имя структуры в городе

// название монстра
//  mov    ecx,??? = тип монстра
//  lea    edx,[8*ecx] 
//  sub    edx,ecx
//  lea    ecx,[ecx+4*edx]
//  mov    edx,0x6747B0
//  mov    edx,[edx]
//  mov    ecx,[edx+4*ecx+14] -> название монстра ед число
//  mov    ecx,[edx+4*ecx+18] -> название монстра мн число

// вторичные скилы
//004F5D85 8B0DF0DC6700   mov    ecx,[0067DCF0]
//004F5D8B 8BC2           mov    eax,edx = скил
//004F5D8D C1E81F         shr    eax,1F
//004F5D90 03D0           add    edx,eax
//004F5D92 33C0           xor    eax,eax
//004F5D94 C1E204         shl    edx,04
//004F5D97 8B540AF0       mov    edx,[edx+ecx-10]

//005A775B 8B83F4000000   mov    eax,[ebx+000000F4] ebx -> генерирующий монстр
//005A77646A01           push   1 , 0      , 1  = перерисовка
//005A7766 6A00           push   0 , 800000 , 400000 = биты для установки новому стэку
//005A7768 52             push   edx  = координата клетки поля
//005A7769 51             push   ecx  = количество
//005A776A 6A30           push   00000030 = номер монстра
//005A776C 50             push   eax = номер игрока (0,1)
//005A776D 8BCF           mov    ecx,edi -> комбат менеждер
//005A776F E8BC22EDFF     call   H3WOG.00479A30
//005A7774 8BF0           mov    esi,eax  -> ук. на нового монстра
//005A7776 8B450C         mov    eax,[ebp+0C] = номер слота в армии героя
//005A7779 8BCF           mov    ecx,edi
//005A777B 89465C         mov    [esi+5C],eax 
//005A777E E80DBBEEFF     call   H3WOG.00493290

//  AI просчитывает магию монстра
//0042146A 8B4834         mov    ecx,[eax+34]
//0042146D 83C1F3         add    ecx,FFFFFFF3
//0042147083F979         cmp    ecx,00000079
//00421473 7760           ja     H3WOG.004214D5 (004214D5)
//00421475 33D2           xor    edx,edx
//00421477 8A91EC144200   mov    dl,[ecx+004214EC]
//0042147D FF2495DC144200 jmp    [4*edx+004214DC]

//00 03 03 03 03 03 03 03 03 03 03 03 03 03 03 03
//03 03 03 03 03 03 03 03 01 03 03 03 03 03 03 03
//03 03 03 03 03 03 00 03 03 03 03 03 03 03 03 03
//03 03 03 03 03 03 03 03 03 03 03 03 03 03 03 03
//03 03 03 03 03 03 03 03 03 03 03 03 03 03 01 03
//03 03 03 03 03 03 03 03 03 03 03 03 03 03 03 03
//03 03 03 03 03 03 03 03 03 03 03 03 03 03 03 03
//03 03 03 03 03 03 03 03 03 02 90 90 90 90 90 90

//84 14 42 00 9F 14 42 00 BA 14 42 00 D5 14 42 00

// Fort on Hill
//:004A8DF3 8B0DB0926900   mov    ecx,[006992B0]
//:004A8DF9 6A01           push   00000001
//:004A8DFB 6A00           push   00000000
//:004A8DFD E89E400600     call   H3WOG.0050CEA0
//:004A8E02 8B0DB0926900   mov    ecx,[006992B0]
//:004A8E08 6A01           push   00000001
//:004A8E0A E8A1490600     call   H3WOG.0050D7B0
//:004A8E0F 8D8D2CFCFFFF   lea    ecx,[ebp-000003D4]
//:004A8E15 E896E80300     call   H3WOG.004E76B0
//:004A8E1A 6AFF           push   FFFFFFFF
//:004A8E1C 6AFF           push   FFFFFFFF
//:004A8E1E 8D8D2CFCFFFF   lea    ecx,[ebp-000003D4]
//:004A8E24 C745FC01000000 mov    dword ptr [ebp-04],00000001
//:004A8E2B E8D0691500     call   H3WOG.005FF800
//:004A8E30 8D8D2CFCFFFF   lea    ecx,[ebp-000003D4]
//:004A8E36 E815F10300     call   H3WOG.004E7F50
//:004A8E3B8D8D2CFCFFFF   lea    ecx,[ebp-000003D4]
//:004A8E41 C745FCFFFFFFFF mov    dword ptr [ebp-04],FFFFFFFF
//:004A8E48 E883F00300     call   H3WOG.004E7ED0

//Заполнение буфера для найма войск в городе
//005D42F6 8B5338         mov    edx,[ebx+38]
//005D42F9 83C7E2         add    edi,FFFFFFE2
//005D42FC 6A01           push   00000001
//005D42FE 57             push   edi       -> номер строения (двелинга с 0)
//005D42FF 52             push   edx       -> Город
//005D4300 8BC8           mov    ecx,eax   -> вуфер на 0xBC байт
//005D4302E859D6F7FF     call   H3WOG.00551960

//  mov    edi,[esi] контрольное слово
//  and    edi,FFFFE01F
//  mov    [esi],edi

//  mov    eax,[ebx+0004E3DC]
//  mov    ecx,edi
//  lea    edi,[ebx+0004E3D8]
//  test   eax,eax
//  je     no_one
//  mov    edx,[edi+08]
//  sub    edx,eax
//  mov    eax,4BDA12F7
//  imul   edx
//  sar    edx,05
//  mov    eax,edx
//  shr    eax,1F
//  add    edx,eax
//  mov    eax,edx число уже добавленных
//no_one:
//  and    eax,00000FFF
//  and    ecx,FC001FFF
//  shl    eax,0D
//  or     eax,ecx
//  mov    [esi],eax контр. слово
//
//  lea    ecx,[ebp-00000170]
//  call   H3WOG.0044A750 иниц структ
//  mov    cl,[ebp-11] ???
//  xor    eax,eax
//  mov    [ebp-00000114],cl
//  mov    [ebp-00000110],eax
//  mov    [ebp-0000010C],eax
//  mov    [ebp-00000108],eax
//  movsx  edx,word ptr [esi+22] тип CrBank (подтип)
//  lea    ecx,[ebp-00000170]
//  mov    [ebp-04],eax
//  call   H3WOG.0047A6C0 настройка конкр. типа.
//  mov    eax,[edi+08] конец CrBank
//  lea    edx,[ebp-00000170]
//  push   edx
//  push   00000001
//  push   eax
//  mov    ecx,edi
//  call   H3WOG.004D22B0 добавляет
//  lea    ecx,[ebp-00000114]
//  mov    dword ptr [ebp-04],FFFFFFFF ????
//  call   H3WOG.005BC5E0 ???

// 41D210 применение магии DD на поверхности

// перенести в позицию
// esi->AdvManager
// mov    ax,[esi+000000E4]
// lea    ecx,[ebx-09] ecx=x-9
// xor    ecx,eax
// lea    edx,[edi-08] edx=y-8
// and    ecx,000003FF
// push   00000000
// xor    ecx,eax
// mov    ax,[esi+000000E6]
// xor    edx,eax
// mov    [esi+000000E4],cx
// and    edx,000003FF
// mov    ecx,esi
// xor    edx,eax
// mov    al,[ebp+08] al=level
// and    eax,0000000F
// and    dh,C3
// shl    eax,0A
// or     edx,eax
// mov    [esi+000000E6],dx
// call   H3WOG.0040F7D0
// push   00000000
// push   00000000
// push   00000000
// push   00000001
// push   00000001
// mov    ecx,esi
// call   H3WOG.004136F0
// push   00000000
// push   00000000
// mov    ecx,esi
// call   H3WOG.0040F1D0

// задержка
// call   H3WOG.004F8970 получить время (системное)
// mov    ecx,eax
// add    ecx,00007D0 задержка около 2-х секунд
// call   H3WOG.004F8980 подождать до этого момента

// mov    eax,[ebp+14] // тип (0x36 - монстр)
// push   ebx
// push   esi
// push   edi
// cmp    eax,00000047 // случ монстр
// mov    edi,ecx // ecx->BASE
// jne    H3WOG.004C956A (004C956A) //переход+2
// mov    dword ptr [ebp+14],00000036
// mov    cl,[ebp+10] // l
// mov    edx,[ebp+1C] // control word
// mov    ebx,[ebp+08] // x
// mov    al,[ebp+0C]  // y
// mov    [ebp-06],cl // =l
// mov    [ebp-0C],edx //=control word
// mov    edx,000000FF
// xor    ecx,ecx
// mov    [ebp-08],bl //=x
// mov    [ebp-07],al //=y
// mov    word ptr [ebp-04],0000
// call   H3WOG.0050C7C0 // случ число в AL от 00 до FF
// mov    [ebp-02],al//=случ.число ?
// mov    eax,[ebp+14] // type
// cmp    eax,0000007C
// lea    esi,[edi+0001FB70]
//...
// mov    eax,[ebp+18] //subtype
// mov    ecx,[ebp+14] //type
// push   FFFFFFFF
// push   eax
// push   ecx
// lea    edx,[ebp-0C]//->control word
// mov    ecx,esi
// push   edx
// call   H3WOG.00506570 //???
// mov    ecx,[edi+0001FB88] ->Positions
// add    edi,0001FB80 // их начало
// lea    eax,[ebp-0C]
// push   eax
// push   00000001
// push   ecx
// mov    ecx,edi
// call   H3WOG.0054D4E0 //добавляет position
// mov    eax,[edi+04]   // ук. на начало
// test   eax,eax        // есть что-нибудь?
// jne    H3WOG.004C9624 (004C9624)
// ...
// mov    edi,[edi+08] // конец таблицы
// push   00000001
// sub    edi,eax
// mov    eax,2AAAAAAB
// imul   edi
// sar    edx,1
// mov    eax,edx
// mov    ecx,esi
// shr    eax,1F
// add    edx,eax
// dec    edx // последний номер в Position (только что добавленный)
// push   edx
// call   H3WOG.00506170

// !!!!!!!!!!!!!!!!
// если не хватает чего-нибудь для компиляции, можно настроить это в
// G:\BC5\BIN\TASM.CFG

// заполняет все поле припядствиями. (игнорировать - чистое)
//004627BA 8BCB           mov    ecx,ebx //->CombatManager
//004627BC E8AF360000     call   H3WOG.00465E70

// edi = 63C7C8+4*5*номер(0...F)
//00465D9D 8B4F10         mov    ecx,[edi+10]   // -> имя def с преп.
//00465DA0 E81B6C0F00     call   H3WOG.0055C9C0 // загрузка
//00465DA5 8B8E603D0100   mov    ecx,[esi+00013D60] // esi-> CombatManager
//00465DAB 8945C8         mov    [ebp-38],eax // ->pfuh вуа
//00465DAE 8B5DEC         mov    ebx,[ebp-14] // = позиция
//00465DB1 83C8FF         or     eax,FFFFFFFF
//00465DB4 8845D1         mov    [ebp-2F],al
//00465DB7 8945DC         mov    [ebp-24],eax
//00465DBA 81C6583D0100   add    esi,00013D58
//00465DC0 8D45C8         lea    eax,[ebp-38]
//00465DC3 50             push   eax
//00465DC4 897DCC         mov    [ebp-34],edi
//00465DC7 6A01           push   00000001
//00465DC9 33FF           xor    edi,edi
//00465DCB 51             push   ecx
//00465DCC 8BCE           mov    ecx,esi
//00465DCE 885DD0         mov    [ebp-30],bl
//00465DD1 C645D201       mov    byte ptr [ebp-2E],01
//00465DD5 897DD4         mov    [ebp-2C],edi
//00465DD8 897DD8         mov    [ebp-28],edi
//00465DDB E8804C0000     call   H3WOG.0046AA60
//00465DE0 8B4604         mov    eax,[esi+04]
//00465DE3 3BC7           cmp    eax,edi
//00465DE5 7504           jne    H3WOG.00465DEB
//00465DE7 33D2           xor    edx,edx
//00465DE9 EB16           jmp    H3WOG.00465E01
//00465DEB 8B7608         mov    esi,[esi+08]
//00465DEE 2BF0           sub    esi,eax
//00465DF0 B8ABAAAA2A     mov    eax,2AAAAAAB
//00465DF5 F7EE           imul   esi
//00465DF7 C1FA02         sar    edx,02
//00465DFA 8BC2           mov    eax,edx
//00465DFC C1E81F         shr    eax,1F
//00465DFF 03D0           add    edx,eax
//00465E01 6A02           push   00000002
//00465E03 4A             dec    edx
//00465E04 53             push   ebx
//00465E05 8D4DC8         lea    ecx,[ebp-38]
//00465E08 52             push   edx
//00465E09 51             push   ecx
//00465E0A 8B4DF0         mov    ecx,[ebp-10]
//00465E0D E87E070000     call   H3WOG.00466590
//00465E12 8B55BC         mov    edx,[ebp-44]
//00465E15 52             push   edx
//00465E16 E8D5521A00     call   H3WOG.0060B0F0
//00465E1B 8B4DF4         mov    ecx,[ebp-0C]
//00465E1E 83C404         add    esp,00000004

// перемещение монстра esi -> CombatManager
// ebx -> Monster Structure
//00478A2E 8B4E44         mov    ecx,[esi+44]
//00478A31 6A01           push   00000001
//00478A33 51             push   ecx
//00478A34 8BCB           mov    ecx,ebx
//00478A36 E8F5CFFCFF     call   H3WOG.00445A30

// получение имени def для заднего плана битвы
//0046382E E87D0A0000     call   H3WOG.004642B0
//00463833 898664340100   mov    [esi+00013464],eax

// выбор мелодии для битвы
//004626C5 BA08000000     mov    edx,00000008
//004626CA B901000000     mov    ecx,00000001
//004626CF E8ECA00A00     call   H3WOG.0050C7C0 // случ. число
//004626D4 48             dec    eax
//004626D5 8D4DD0         lea    ecx,[ebp-30] // буфер для имени wav
//004626D8 50             push   eax
//004626D9 68E8FE6600     push   0066FEE8
//004626DE 51             push   ecx
//004626DF E8FA521B00     call   H3WOG.006179DE // находим имя
//004626E7 8D4DD0         lea    ecx,[ebp-30]
//004626EA E881801300     call   H3WOG.0059A770 // открываем файл звуковой

// определяет картинку героя на поле боя
//00463077 8B8840BD6300   mov    ecx,[eax+0063BD40]
//0046307DE83E990F00     call   H3WOG.0055C9C0

// перерисовка экрана битвы в след состояние
//00478B94 8BCE           mov    ecx,esi  // esi ->CombatManager
//00478B96 C6863040010001 mov    byte ptr [esi+00014030],01
//00478B9D E86E0B0000     call   H3WOG.00479710

// анимация стрельбы монстра
// esi -> CombatManager, edx=позиция, куда стрелять, ecx=ук на стреляющего монстра
//00478BA2 8B5644         mov    edx,[esi+44]
//00478BA5 6A01           push   00000001
//00478BA7 52             push   edx
//00478BA8 8BCB           mov    ecx,ebx
//00478BAAE8D1CBFCFF     call   H3WOG.00445780

// монстр идет в позицию
//00478A2E 8B4E44         mov    ecx,[esi+44]
//00478A31 6A01           push   00000001
//00478A33 51             push   ecx
//00478A34 8BCB           mov    ecx,ebx
//00478A36 E8F5CFFCFF     call   H3WOG.00445A30

// колдует герой (esi->CombatManager)
//00478974 8B5648         mov    edx,[esi+48] =? (-1)
//00478977 8B4644         mov    eax,[esi+44] -> позиция для атаки
//0047897A 8B4E40         mov    ecx,[esi+40] -> номер заклинания
//0047897D 6A03           push   00000003     =? сила
//0047897F 57             push   edi          = уровень для монстра
//00478980 52             push   edx
//00478981 57             push   edi          =0-герой,1-монстр (2=?)
//00478982 50             push   eax
//00478983 51             push   ecx
//00478984 8BCE           mov    ecx,esi
//00478986 E8B5771200     call   H3WOG.005A0140

// атака монстра в опред. позицию
//00478D70 8B4D08         mov    ecx,[ebp+08] //? буфер
//00478D73 C6863040010001 mov    byte ptr [esi+00014030],01
//00478D7A 51             push   ecx
//00478D7B 8BCE           mov    ecx,esi
//00478D7D E8AEF4FFFF     call   H3WOG.00478230

// рисует стрельбу монстра esi->стреляющий монстр, edi->монстр в кого стреляют
//0043F6F6 8B7D08         mov    edi,[ebp+08]
//0043F6F9 8BCE           mov    ecx,esi
//0043F6FB 57             push   edi
//0043F6FC E8DFF8FFFF     call   H3WOG.0043EFE0

// рисует отклонение монстра при атаке на него (но жив)
//0043FA6F 8B5D08      //////////   mov    ebx,[ebp+08] = 0(стэк жив) 1(убит)
//0043FA72 8B0D20946900   mov    ecx,[00699420] -> CombatManager
//0043FA78 6A00           push   00000000
//0043FA7A 6AFF           push   FFFFFFFF
//0043FA7C E8EF8A0200     call   H3WOG.00468570

// убирает/перерисовывает цифирки у монстров
//00468F5B 8B4DF8         mov    ecx,[ebp-08]
//00468F5E 6A00           push   00000000
//00468F60 6A01           push   00000001
//00468F62 6A00           push   00000000
//00468F64 6A00           push   00000000
//00468F66 6A00           push   00000000
//00468F68 6A01           push   00000001
//00468F6A E851B00200     call   H3WOG.00493FC0

//чтобы убить монстра надо:
// число = 0 [+4C] dd
// число до удара=1 [+50] dd
// флаг атаковали и убили одного=1 [+E9] db
// флаг убит стэк=1 [+EA] db
// или вызвать функцию:
//0043FA5B 8BCF           mov    ecx,edi // ->монстр под ударом
//0043FA5D 53             push   ebx     // =количество очков дамэджа
//0043FA5E E84D430000     call   H3WOG.00443DB0

// прорисовка поля боя после черного экрана
//00462C13 6A00           push   00000000
//00462C15 6A04           push   00000004
//00462C17 6A00           push   00000000
//00462C19 E8F2051A00     call   H3WOG.00603210

// по ук. на клетку находит ук на стэк монстра (в EAX)
//005A4011 8BCE           mov    ecx,esi
//005A4013 E81832F4FF     call   H3WOG.004E7230

// количество квиксендов на поле от мастерства
//:005A06648B3CB524226400 mov    edi,[4*esi+00642224]
// генерировать позицию для квик сэнда
//:005A06ADE87EC2F6FF     call   H3WOG.0050C930
// сколько минных полей кидать на поле.
//:005A084B8B14B534226400 mov    edx,[4*esi+00642234]
// генерировать позицию для минного поля
//:005A08C5E866C0F6FF     call   H3WOG.0050C930

struct _MouseStr_{
	int Type;   // +0 
	int SType;  // +4
	int Item;   // +8
	int Flags;  // +C
	int Xabs;   // +10
	int Yabs;   // +14
	 int Param;  // +18 3.59
	 int Flags2; // +1C 3.59
};
//40A077A1CC836700     mov    eax,[006783CC]
//40A07C 83F824         cmp    eax,00000024
//40A07F 7425           je     H3WOG.0040A0A6 (0040A0A6)
//40A081 83F848         cmp    eax,00000048
//40A084 7417           je     H3WOG.0040A09D (0040A09D)
//40A086 83F86C         cmp    eax,0000006C
//40A089 7409           je     H3WOG.0040A094 (0040A094)
// XL
//40A08B C745F80000803F mov    dword ptr [ebp-08],3F800000
//40A092 EB19           jmp    H3WOG.0040A0AD (0040A0AD)
// 0x6C: L
//40A094 C745F893A9AA3F mov    dword ptr [ebp-08],3FAAA993
//40A09B EB10           jmp    H3WOG.0040A0AD (0040A0AD)
// 0x48: M
//40A09D C745F800000040 mov    dword ptr [ebp-08],40000000
//40A0A4 EB07           jmp    H3WOG.0040A0AD (0040A0AD)
// 0x26: S
//40A0A6 C745F800008040 mov    dword ptr [ebp-08],40800000
//40A0AD 8B5344         mov    edx,[ebx+44] // ebx->advManager
//40A0B0 8B4F14         mov    ecx,[edi+14] // edi->mouse structure
//40A0B3 56             push   esi // save
//40A0B4 8B724C         mov    esi,[edx+4C]
//40A0B7 0FBF461A       movsx  eax,word ptr [esi+1A] // left border of map
//40A0BB 2BC8           sub    ecx,eax
//40A0BD 894DFC         mov    [ebp-04],ecx
//40A0C0 DB45FC         fild   dword ptr[ebp-04]
//40A0C3 D95DFC         fstp   dword ptr[ebp-04]
//40A0C6 D945FC         fld    dword ptr[ebp-04]
//40A0C9 D875F8         fdiv   dword ptr[ebp-08]
//40A0CC E8C3DE2000     call   H3WOG.00617F94
//40A0D1 0FBF5618       movsx  edx,word ptr [esi+18]
//40A0D5 8945FC         mov    [ebp-04],eax // y
//40A0D8 8B4508         mov    eax,[ebp+08] // ->mouse structure
//40A0DB 668BBBE4000000 mov    di,[ebx+000000E4] // ebx->advManager
//40A0E2 8B4810         mov    ecx,[eax+10] // x or
//40A0E5 2BCA           sub    ecx,edx
//40A0E7 894D08         mov    [ebp+08],ecx
//40A0EA DB4508         fild   dword ptr[ebp+08]
//40A0ED D95D08         fstp   dword ptr[ebp+08]
//40A0F0 D94508         fld    dword ptr[ebp+08]
//40A0F3 D875F8         fdiv   dword ptr[ebp-08]
//40A0F6 E899DE2000     call   H3WOG.00617F94
//40A0FB 8B55FC         mov    edx,[ebp-04]

// AI определяет ценность ресурса для подбора
//0052969A 8BCB           mov    ecx,ebx  // -> HeroStr
//0052969C 52             push   edx      // -> структура игрока +020AD0+i*168h
//0052969D 8BD6           mov    edx,esi  // -> MapItem
//0052969F E8FC150000     call   H3WOG.0052ACA0

// проверка на корректность координат позиции
//0040A563 8D4DF4         lea    ecx,[ebp-0C]
//0040A566 8945F4         mov    [ebp-0C],eax // = MixedPos
//0040A569 E8226B0A00     call   H3WOG.004B1090
//0040A56E84C0           test   al,al        // = 1 - Ok
//0040A570 0F8460050000   je     H3WOG.0040AAD6

// проверяет видна ли клетка для AI
// можно использовать
//0042F150 E8EB8E0C00     call   H3.004F8040
//0042F155 840510CD6900   test   [0069CD10],al
//0042F15B 7521           jne    H3.0042F17E (0042F17E) переход = видна

// переход по типу объекта для вычисления значимости для AI
//00528546 F6460D10       test   byte ptr [esi+0D],10
//0052854A 0F8475140000   je     H3.005299C5
//00528550 8B461E         mov    eax,[esi+1E]
//00528553 83C0FC         add    eax,FFFFFFFC
//00528556 83F86D         cmp    eax,0000006D
//00528559 0F8766140000   ja     H3.005299C5
//0052855F33C9           xor    ecx,ecx
//00528561 8A88E09A5200   mov    cl,[eax+00529AE0]
//00528567 FF248DD0995200 jmp    [4*ecx+005299D0]
//0052856E 56             push   esi

/*
005A29E0 B808A66300     mov    eax,0063A608
005A29E5 8B8BFC320100   mov    ecx,[ebx+000132FC]
005A29EB 6A00           push   00000000
005A29ED 6A01           push   00000001
005A29EF 50             push   eax
005A29F0 E8DBFFECFF     call   H3WOG.004729D0
005A29F5 B980846800     mov    ecx,00688480
005A29FA E8717DFFFF     call   H3WOG.0059A770
005A29FF 8BFA           mov    edi,edx
005A2A01 8B93C0320100   mov    edx,[ebx+000132C0]
005A2A07 F7DA           neg    edx
005A2A09 1BD2           sbb    edx,edx
005A2A0B 6A00           push   00000000
005A2A0D 83E210         and    edx,00000010
005A2A10 6A64           push   00000064
005A2A12 52             push   edx
005A2A13 6A4C           push   0000004C
005A2A15 8BCB           mov    ecx,ebx
005A2A17 8BF0           mov    esi,eax
005A2A19 E8723BEFFF     call   H3WOG.00496590
005A2A1E B970846800     mov    ecx,00688470
005A2A23 E8487DFFFF     call   H3WOG.0059A770
005A2A28 8945C8         mov    [ebp-38],eax
005A2A2B 8B83C0320100   mov    eax,[ebx+000132C0] // ebx->CombatManager
005A2A31 F7D8           neg    eax                // атакующая сторона
005A2A33 1BC0           sbb    eax,eax
005A2A35 6A00           push   00000000
005A2A37 24F0           and    al,F0
005A2A39 6A64           push   00000064
005A2A3B 83C010         add    eax,00000010
005A2A3E 8BCB           mov    ecx,ebx
005A2A40 50             push   eax
005A2A41 6A4B           push   0000004B
005A2A43 8955CC         mov    [ebp-34],edx
005A2A46 E8453BEFFF     call   H3WOG.00496590
005A2A4B57             push   edi
005A2A4C 83C9FF         or     ecx,FFFFFFFF
005A2A4F 56             push   esi
005A2A50 E86B7DFFFF     call   H3WOG.0059A7C0
005A2A55 8B55CC         mov    edx,[ebp-34]
005A2A58 8B45C8         mov    eax,[ebp-38]
005A2A5B 52             push   edx
005A2A5C 83C9FF         or     ecx,FFFFFFFF
005A2A5F 50             push   eax
005A2A60 E85B7DFFFF     call   H3WOG.0059A7C0
*/

// переход по типу картинки для показа сообщений
//004F55958A8FE4634F00   mov    cl,[edi+004F63E4]
//004F559B FF248D9C634F00 jmp    [4*ecx+004F639C]
//004F55A2 8B14BD7CF56700 mov    edx,[4*edi+0067F57C]

// монстр кастит магию на стэк
//00478CAC 8B4644         mov    eax,[esi+44] // позиция для каста в комбат мэнэджере
//00478CAF 8BCB           mov    ecx,ebx      // ук. на структуру монстра-кастера
//00478CB1 50             push   eax
//00478CB2E8C9F2FCFF     call   H3WOG.00447F80

// рисует обычную стрелку вместо артифакта, когда его кинули куда-то.
//004DE23A 8B0DB0926900   mov    ecx,[006992B0]
//004DE240 6A00           push   00000000
//004DE242 6A00           push   00000000
//004DE244 E857EC0200     call   H3WOG.0050CEA0

// проверка на посещение монолита
// mov    edi,[00699538]
// mov    al,[0069CD10]
// lea    ecx,[edx+edi+0004E3E9] edx=номер монолита
// mov    dl,[ecx]
// test   dl,al
// jne    H3WOG.004A95B8

// добавить монстров герою
//  mov    edx,[ebp+08] -> Hero
//  push   FFFFFFFF
//  push   esi = number
//  push   edi = type
//  lea    ecx,[edx+00000091] -> Heros Army Slots
//  call   H3WOG.0044A9B0 try to add
//  test   eax,eax        added
//  jne    H3WOG.004A6D92  yes
//  mov    al,[ebp+18] HUMAN/ai
//  mov    ecx,[ebp+08] -> Hero
//  test   al,al  HUMAN?
//  push   esi = number
//  mov    edx,edi = type
//  je     H3WOG.004A6D8D  AI
//  call   H3WOG.005D15D0  Ask human to find room
//  jmp    H3WOG.004A6D92
//  call   H3WOG.0052C140  AI Auto add

// проверка на наличие артифакта
//  push   0000007D         // номер
//  call   H3WOG.004D9460
//  test   al,al            // !=0 - есть

// переход по команде при приеме данных по сети
//00557072 8A88BC725500   mov    cl,[eax+005572BC]
//00557078 FF248DA4725500 jmp    [4*ecx+005572A4]

// проверка цвета на АИ
//004AD1E9 8B0D38956900   mov    ecx,[00699538] // [BASE]
//004AD1EF 56             push   esi            // color (player)
//004AD1F0 E80B140200     call   H3WOG.004CE600 // check
//004AD1F5?84C0           test   al,al          // al=1 if a human

// открытие, запись и закрытие файла hiscores.dat 
//004EA4E0 6880000000     push   00000080
//004EA4E5 8D8DA0FEFFFF   lea    ecx,[ebp-00000160]
//004EA4EB 6801830000     push   00008301
//004EA4F0 51             push   ecx
//004EA4F1 E857FC1200     call   H3WOG.0061A14D
//004EA4F6 8BF0           mov    esi,eax
//004EA4F8 83C41C         add    esp,0000001C
//004EA4FB 83FEFF         cmp    esi,FFFFFFFF
//004EA4FE 750D           jne    H3WOG.004EA50D (004EA50D)
//004EA500 8B0DF0F16700   mov    ecx,[0067F1F0]
//004EA506 E8E59B0000     call   H3WOG.004F40F0
//004EA50B EB1E           jmp    H3WOG.004EA52B (004EA52B)
//004EA50D 8B151C946900   mov    edx,[0069941C]
//004EA513 6898080000     push   00000898
//004EA518 83C238         add    edx,00000038
//004EA51B 52             push   edx
//004EA51C 56             push   esi
//004EA51D E81D621300     call   H3WOG.0062073F
//004EA522 56             push   esi
//004EA523 E845FB1200     call   H3WOG.0061A06D
//004EA528?83C410         add    esp,00000010

// разбор посылки по сети
// 405DC0

// урон атакера расчет (AI только):
// 443C60 - hand/dist
// 5A7BF0 - spell
// урон защитнику:
// 443DB0 - hand/dist/spell

// Анимация стрельбы для лича/повер лича:
// 43FA1F
// расчет дамэджа для Dread Knight:
// 4435A3

// стрельба 43FA54
// 43FA54 - расчет дамэджа
// 43FA5E - нанесение урона (ebx=урон)
// 43FA7C - анимация удара для обоих + наложенные заклинания
// 43FAD4 - вывод строки об уроне

// вывод строки на поле боя (временный): 492E3B

// переход по типу действия по левому клику в правую часть карты прикл.
//00409A0E 8A88F49F4000   mov    cl,[eax+00409FF4]
//00409A14FF248DB89F4000 jmp    [4*ecx+00409FB8] 

// дествие заклинания на карте
//0041C5D4 8B15D0926900   mov    edx,[006992D0] -> heroWindowManager
//0041C5DA 8B4238         mov    eax,[edx+38] = номер спела
//0041C5DD 50             push   eax
//0041C5DE8BCF           mov    ecx,edi -> advManager
//0041C5E0 E82B000000     call   H3WOG.0041C610
//0041C5E5 6A01           push   00000001
//0041C5E7 6A01           push   00000001
//0041C5E9 6A01           push   00000001
//0041C5EB 8BCF           mov    ecx,edi
//0041C5ED E84E97FFFF     call   H3WOG.00415D40

// полная реализация магии на карте (с книгой и выбором)
//00409EC2 8BCE           mov    ecx,esi
//00409EC4E8A7250100     call   H3WOG.0041C470

// показ книжки и выбор заклинания
//0041C520 E8FB341E00     call   H3WOG.005FFA20

// AI думает
// 526A51 -> 526D58 -> 42EE3E -> 42F6B5

// трансляция скелетон трансформером
//struct _SkelTrans_ {
//  int Mon2;
//};

// 444230 - снять закл с монстра
// пар = номер закла, ecx -> стр. монстра

// перерисовка встречи двух героев
//005B0935 8B0DD0926900   mov    ecx,[006992D0]
//005B093B 6858020000     push   00000258
//005B0940 6820030000     push   00000320
//005B0945 6A00           push   00000000
//005B0947 6A00           push   00000000
//005B0949 E842280500     call   H3WOG.00603190

//0044A830 убивает стэк в массиве _MonArr_

// From IDA
//.text:004025E5                     movsx ecx, [esi+_Hero_.ExpLevel]        ; Hero's Level Up
//.text:004025E9                     call NeedExpoToNextLevel                ; Call Procedure
//.text:004025EE                     mov ecx, esi                            ; MainStructure
//.text:004025F0                     push 1                                  ; int
//.text:004025F2                     push 1                                  ; ShowLevelUp
//.text:004025F4                     push eax                                ; Expo
//.text:004025F5                     call GiveExperience                     ; Call Procedure

//.text:0040261B                     or  [esi+_Hero_.TempMod], 400000h       ; Give Maximum Luck

//.text:00402646                     mov eax, [esi+_Hero_.TempMod]           ; Give endless movement
//.text:0040264C                     mov ecx, esi                            ; Hero
//.text:0040264E                     or  eax, 1000000h                       ; Logical Inclusive OR
//.text:00402653                     mov [esi+_Hero_.TempMod], eax
//.text:00402659                     call CalcHeroMovementsPoints2           ; Call Procedure
//.text:0040265E                     mov [esi+_Hero_.Movement], eax
//.text:00402661                     mov [esi+_Hero_.Movement0], eax

//.text:00402685                     or  [esi+_Hero_.TempMod], 800000h       ; Give Maximum Moral

//.text:004026AC                     mov eax, PlayerStructPo                 ; Open map for Oracle quest
//.text:004026B1                     mov byte ptr [eax+38h], 30h
//.text:004026B5                     mov ecx, UnknStruct_s3B8                ; MainStructure
//.text:004026BB                     call sub_0041A750                       ; Call Procedure

//.text:0040281D                     mov ecx, 2                              ; Lose
//.text:00402822
//.text:00402822 loc_00402822:                                               ; CODE XREF: ProcessCheats+459j
//.text:00402822                     call sub_004F3370                       ; Call Procedure
//.text:00402827
//.text:00402827 loc_00402827:                                               ; CODE XREF: ProcessCheats+151j
//.text:00402827                                                             ; ProcessCheats+174j ...
//.text:00402827                     mov edx, ZPrPo                          ; int
//.text:0040282D                     or  ecx, 0FFFFFFFFh                     ; Logical Inclusive OR
//.text:00402830                     mov eax, [edx+20h]
//.text:00402833                     mov esi, [eax+414h]
//.text:00402839                     xor eax, eax                            ; Logical Exclusive OR
//.text:0040283B                     mov edi, esi
//.text:0040283D                     repne scasb                             ; Compare String
//.text:0040283F                     not ecx                                 ; One's Complement Negation
//.text:00402841                     dec ecx                                 ; Decrement by 1
//.text:00402842                     mov ebx, ecx
//.text:00402844                     cmp ebx, 0FFFFFFFDh                     ; Compare Two Operands
//.text:00402847                     jbe short loc_0040284E                  ; Jump if Below or Equal (CF=1 | ZF=1)
//.text:00402849                     call sub_0060B0FB                       ; Call Procedure
//.text:0040284E
//.text:0040284E loc_0040284E:                                               ; CODE XREF: ProcessCheats+3F7j
//.text:0040284E                     mov edi, [ebp+var_C]
//.text:00402851                     xor edx, edx                            ; int
//.text:00402853                     mov ecx, [edi+4]
//.text:00402856                     cmp ecx, edx                            ; Compare Two Operands
//.text:00402858                     jz  loc_004029AD                        ; Jump if Zero (ZF=1)
//.text:0040285E                     mov al, [ecx-1]
//.text:00402861                     test al, al                             ; Logical Compare
//.text:00402863                     jz  loc_004029AD                        ; Jump if Zero (ZF=1)
//.text:00402869                     cmp al, 0FFh                            ; Compare Two Operands
//.text:0040286B                     jz  loc_004029AD                        ; Jump if Zero (ZF=1)
//.text:00402871                     cmp ebx, edx                            ; Compare Two Operands
//.text:00402873                     jnz loc_004029D1                        ; Jump if Not Zero (ZF=0)
//.text:00402879                     dec al                                  ; Decrement by 1
//.text:0040287B                     mov [ecx-1], al
//.text:0040287E                     mov [edi+4], edx
//.text:00402881                     mov [edi+8], edx
//.text:00402884                     mov [edi+0Ch], edx

// to Win - the same, but ecx=1 

// Анимация и действие облака смерти Личей.
//.text:0043FB27                     mov ecx, SpellTable_Offset
// ИИ продумывает существ, что стреляют по площади
//.text:0041ED5A                     cmp eax, 2Dh                            ; Compare Two Operands


// кастинг чего-либо фтакующего при ближней атаке
// esi -> AMon, edi ->DMon
//                     mov eax, [edi+4Ch]     // number of creatures
//                     test eax, eax
//                     jle loc_004412AB       // no one left
//                     mov eax, [esi+288h]    // cast Hypnotyze?
//                     test eax, eax
//                     jz  short loc_0044110B // NO
//                     mov ecx, [esi+0F4h]    // side of owner (0,1)
//                     mov eax, 1
//                     sub eax, ecx           // switch to oponent side
//                     jmp short loc_00441111
// loc_0044110B:                                               ; CODE XREF: sub_004408E0+81Aj
//                     mov eax, [esi+0F4h]    // side of owner (0,1)
// loc_00441111:                                               ; CODE XREF: sub_004408E0+829j
//                     mov ecx, pCombatManager
//                     push 1
//                     push 1
//                     push edi               // -> DMon
//                     push eax               // Side
//                     push 78                // Spell Index
//                     call sub_005A8950
//                     test al, al
//                     jz  short loc_00441141
//                     mov ecx, [edi+38h]     // Dest position
//                     push 3
//                     push 0
//                     push -1
//                     push 1
//                     push ecx
//                     mov ecx, pCombatManager
//                     push 78                // Spell Index
//                     call sub_005A0140

// Использование иконки маг. книги у дракона:
// 1. подменить код проверки для рисования книги: 5F3D50
// при нажатии на нее:
// 2. вызывается функция 5F53E0
// 3. проверка действия 5F0E..
// 4. сам спел определяется в 468523 - берется из +4E0 в стр. монстра.

// Магическая атака
//.text:005A7BF0 ; int __fastcall ApplyMagicDamage(int CombMan,int,int Par,int DMonPo,_Hero_ *AHp,_Hero_ *DHp,int Spell,int Damage)
//.text:005A7BF0 ApplyMagicDamage    proc near                               ; CODE XREF: sub_00421670+58p
//.text:005A7BF0                                                             ; sub_004225C0+8Bp ...

// возможность апгрейда в городе - проверка, что группа существа равна типу замка
//:004C698F 3BC1           cmp    eax,ecx

// сброс берсерка (чего угодно????) сразу после после атаки
//00441C75 6A3B           push   0000003B
//00441C77 8BCF           mov    ecx,edi ->Mon
//00441C79?E8B2250000     call   H3WOG.00444230

// позволяет дважды стрелюющим юнитам дважды атаковать в ближнем бою
// 441BAF - проверка на шутера

// позволяет отвечать на атаку, даже если у атакера no retaliation бит
// 441B0B - проверка на no retaliation

// описание типов героев по типам городов 

// обработка заклов в +EC структуре монстра в 468CBE

// стреляет второй раз
//0043FF79 E8A2F6FFFF     call   H3WOG.0043F620 
//0043FF7E 8B8684000000   mov    eax,[esi+00000084]
//0043FF84?C1E80F         shr    eax,0F
//0043FF87 A801           test   al,01
//0043FF89 740F           je     H3WOG.0043FF9A (0043FF9A)
//0043FF8B 8B434C         mov    eax,[ebx+4C]
//0043FF8E 85C0           test   eax,eax
//0043FF90 7E08           jle    H3WOG.0043FF9A (0043FF9A)
//0043FF92 53             push   ebx
//0043FF93 8BCE           mov    ecx,esi
//0043FF95 E886F6FFFF     call   H3WOG.0043F620

// 602C94 - закрывает диалоги (в частности при сетевой необходимости)
// 1000 - ??? (24h) r:405E16
// 1001 - передача данных карты
// 1002 - ??? (18h)
// 1003 - ??? (28h)
// 1004 - передача сообщения по сети (H,"Message") r:405E72 rb:473E09
//*1005 - передача инфы перед битвой (H,A lot of Info) r:405E81
// 1006 - передача хода игроку (28h) (H,dd Новый Игрок) 405EA6
// 1006 - в битве передача действия (28h) (H,action(cm+3C),param(cm+40),position(cm+44),par2(cm+48),rndseed) sb:47883B rb:473D5E
// 1008 - ??? что-то с передачей хода (14h) rb: 473E4A
// 1009 - передвет по сети +E4 поле для структуры игрока (18h) (H,dd Поле) [возможно номер игрока по подключению] r:406213
//*1011 - передача прокачки героя после выигрыша на активный PC (H, dd Hero,dd*7 SecSc,dd PrSkill,dd SeCkillNum)
// 1012 - ??? подтверждение в получении (14h)
// 1013 - ??? Waiting for other players.... (14h)
// 1014 - игрок бросает игру - замена на AI (H,dd Игрок) r:405F43 rb:473E9C
// 1015 - поменять хост r:557D15
// 1016 - перейти к след человеку (18h) (H,dd Игрок) r:405EA6
// 1017 - ??? что-то с передачей хода (H,dd Игрок) r:405F7B
// 1018 - ??? добавляет сетевого игрока в игру (18h) (H,dd Игрок) r:405F9C
// 1019 - ??? кто-то выиграл в игре (другой игрок) (64h) (H,dd ???Игрок, dd*13h rкопир в +01F89C) r:405FAB
// 1020 - ??? время игры вышло (3Ch) (H,dd ???[db +9D с карты], 9*dd ??? [dd +7C с карты или= +01F8E8]) r:405FCB
// 1021 - открыть поверхность (20h) (H,dd MixedPos,dd Owner,dd Radius) r:405FEB
// 1022 - ??? закрыть поверхность (20h) r:4060FF
// 1028 - ???
// 1039 - ??? (длина 7D4h)
// 1045 - ??? короткое (14h)
// 1049 - ???
// 1050 - ??? (1Ch) r:481953
// 1051 - перемещение героя (1Ch) (H,dd HeroIndex,dd MixedPos???) r:481962
// 1052 - ??? что-то с Шахтой/Маяком (1Ch) r:4819A0
// 1053 - ??? что-то с Городом (1Ch) r:4819D1
// 1054 - передать контроль над Dwelling другому игроку (1Ch) (H,dd DwellStrInd, dd NewOwner) r:481BC5
// 1055 - ??? что-то с Гарнизоном (1C) r:481BDF
// 1056 - ??? Open Area (1Ch) r:481BF9
// 1057 - ??? что-то с лодкой (1Ch) r:481A04
// 1058 - ??? удалить объект на карте (монстр?) (18h) (H,dd PosMixed) r:481A5D
// 1059 - ??? убить героя (1Ch) r:481A91
// 1060 - ??? (20h) (H,dd HeroIndex, dd ???,dd CurentPlayer) r:481AF7
// 1061 - ??? (18h) r:481B6C
// 1062 - спрятать героя с карты (вошел в город) (H,dd HeroNumber) r:481C13
// 1063 - ???
// 1064 - встреча двух героев передача структур двух героев (H[l=938h],_Hero_ h1,_Hero_ h2) 406244 r:406230
// 1065 - ??? (14h)
// 1066 - ??? (938h)
// 1070 - ??? (14h)
// 1071 - ??? напоминание, что игрок получил ход (14h) (H) r:4062D3
// 1072 - ??? что-то с ping проверка связи (18h)
// 1073 - ??? что-то с ping проверка связи (18h) 
// 1074 - ??? передача ресурсов игроку (20h) r:40630A
// 1075 - ??? просьба ресурса (1Ch) (H,dd Player,dd Resource???) r:406314
// 1076 - "Sorry, your connection was lost." (H) r:40631E r:557CDD
// 1078 - выиграл игрок номер (18h)(H,dd Player) r:406332
// 1079 - ??? какой-то игрок бросил игру (18h)

// если в описание арта есть скобки [], то дуда добавляется название спела.
// функция 0x4DB650

struct _Spell_{ // size:88h
	int   FRposNOzerENneg; // +0 -1=enemy, 0=square/global/area, 1=friend
	char *SoundFileName;   // +4 
	int   DefIndex;        // +8 used to get DefName
	int   Flags;            // +C
//0x00000001 - BF spell
//0x00000002 - MAP spell
//0x00000004 - Has a time scale (3 rounds or so)
//0x00000008 - Creature Spell
//0x00000010 - target - single stack
//0x00000020 - target - single shooting stack (???)
//0x00000040 - has a mass version at expert level
//0x00000080 - target - any location
//0x00000100 - 
//0x00000200 - 
//0x00000400 - Mind spell
//0x00000800 - friendly and has mass version
//0x00001000 - cannot be cast on SIEGE_WEAPON
//0x00002000 - Spell from Artifact
//0x00004000 -
//0x00008000 - AI 
//0x00010000 - AI area effect
//0x00020000 - AI
//0x00040000 - AI
//0x00080000 - AI number/ownership of stacks may be changed 
//0x00100000 - AI
	char  *Name;            // +10h
	char  *AbbrName;        // +14h
	int    Level;           // +18h
	int    SchoolBits;      // +1Ch Air=1,Fire=2,Water=4,Earth=8
	int    Cost[4];         // +20h cost mana per skill level
	int    Eff_Power;       // +30h
	int    Effect[4];       // +34h effect per skill level
	int    Chance2GetVar[9];// +44h chance per class
	int    AIValue[4];      // +68h 
	char  *Descript[4];     // +78h
};

void *h3new(size_t sz);
void h3delete(void *po);

// структура атаки с возвратом
struct HARPYATACK { // Diakon
	bool IsHave;
};

struct SPELLSTR{  //Diakon
	int ResistEq;  // какой % сопративления к данному заклинанию
};//SPELLSTR

struct SUBRESSTR{ //Diakon
	int FullResistEq; // какой % сопративления ко всем заклинаниям
}; //SUBRESSTR

struct LEVELRESSTR{ //Diakon
	int LevelResistEq; // какой % сопративление к данному и более нижнему уровню заклинаний
};//LEVELRESSTR

struct TYPEMAGSTR{
	int TypeResistEq; // % сопративления к типу магии
};	

struct RESISTSTRUCT {    // Diakon
	SUBRESSTR AllResist;      // сопративление всей магии
	LEVELRESSTR SpelLevelResist[5];      // сопративление уравню магии
	TYPEMAGSTR TypeResist [9]; // сопративление типу магии
	SPELLSTR Magic [SPELLNUM];// сопративление заклинанию
	bool MyMagic;
};// RESISTSTRUCT

extern RESISTSTRUCT MonResStr[MONNUM];
extern Byte MoJumper1[MONNUM-13]; //Diakon
extern int MoJmpAdr1[]; //Diakon
extern Byte MagHint[MONNUM-13];//Diakon
extern int MagHintJmp[];//Diakon
extern Byte MoJumper2[MONNUM-13];//Diakon
extern int MagJmp[]; //Diakon
extern Byte AIMagicJumper[MONNUM-13];

struct CASTMAGIC{ // Diakon
	int NumOfMag;
	bool SpellCast[SPELLNUM];
};

extern CASTMAGIC StrCastMag [MONNUM];

void MagicBookManager();

#endif
