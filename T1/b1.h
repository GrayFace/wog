#include "txtfile.h"

extern int        MapWoG;
//extern int        WoGatLoad;
extern int        WoGType;
extern TxtFile    Strings;
extern Dword      OriginalCallPointer; // вспомогательная переменная
extern _Hero_    *HeroEntered2Object;
struct __Callers{
	long  where;
	long  forig;
	long  fnew;
#ifdef DEBUG
// DEBUG для отладки
	char *remember;
#endif  
};
extern __Callers Callers[];
extern _VidInfo_ VidArr[];
extern _HeroSpec_ HSpecTable[];
extern _HeroBios_ HBiosTable[];
extern _MonInfo_ MonTableBack [MONNUM];
extern _MonInfo_  MonTable[MONNUM];
//extern _HeroInfo_ HTable[];
extern _HeroInfo_ HTable[HERNUM+8];
//extern Byte NewMonthMonTypes[];
extern _MagicAni_ MagicAni[MAGICS];
extern _ArtSetUp_ ArtTable[ARTNUM];
extern char * ArtPickUp[ARTNUM];
extern _MNameS_  MonTable2[MONNUM];
extern _MNameP_  MonTable3[MONNUM];
extern _MNameSp_ MonTable4[MONNUM];
extern _CArtSetup_ CArtSetup[32];
extern _Spell_ Spells[SPELLNUM];

extern char MapSavedWoG[1024];

extern int NoMoreChecksS;
extern int NoMoreChecksW;

int Saver(const void *Po,int Len);
int Loader(void *Po,int Len);
int FindManager(Dword po);
void ResetAll(int game = 1);
