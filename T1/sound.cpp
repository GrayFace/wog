#include "structs.h"
#include "common.h"
#include "txtfile.h"
//#include "classes.h"
#include "herospec.h"
#include "service.h"
#include "b1.h"
#include "erm.h"
#include "sound.h"
#include "string.h"
#define __FILENUM__ 10

/////////////////////////////////////
// MP3
#define MP3NUM 200
static struct _MP3{
	char *Old;
	char  New[256];
} MP3[MP3NUM]={
// города 0+
{"cstletown"},{"rampart"},{"towertown"},{"infernotown"},{"necrotown"},
{"dungeon"},{"stronghold"},{"fortresstown"},{"elemtown"},
// поверхности 9+
{"dirt"},{"sand"},{"grass"},{"snow"},{"swamp"},{"rough"},{"underground"},{"lava"},{"water"},
// 18+
{"mainmenuwog"},{"mainmenu"},
// 20+
{"aitheme0"},{"aitheme1"},{"aitheme2"},
// 23+
{"bladeabcampaign"},{"bladedbcampaign"},{"bladedscampaign"},
{"bladeflcampaign"},{"bladefwcampaign"},{"bladepfcampaign"},
// 29+
{"campainmusic01"},{"campainmusic02"},{"campainmusic03"},
{"campainmusic04"},{"campainmusic05"},{"campainmusic06"},
{"campainmusic07"},{"campainmusic08"},{"campainmusic09"},
{"campainmusic10"},{"campainmusic11"},
// 40+
{"combat01"},{"combat02"},{"combat03"},{"combat04"},
// 44+
{"eviltheme"},{"goodtheme"},{"neutraltheme"},{"secrettheme"},
// 48
{"looplepr"},
// 49+
{"losecastle"},{"defend castle"},
// 51+
{"losecombat"},{"retreat battle"},{"surrender battle"},{"win battle"},
// 55+
{"lose campain"},{"ultimatelose"},{"win scenario"}
//58 нет
// {"GRASS","..\\maps\\grass1"}
};
static char *MP3Name=0;

void ResetMP3(void)
{
	STARTNA(__LINE__, 0)
	for(int i=0;i<MP3NUM;i++){
		for(int j=0;j<256;j++){
			MP3[i].New[j]=0;
		}
	}
	RETURNV
}
int SaveMP3(void)
{
	STARTNA(__LINE__, 0)
	if(Saver("LMP3",4)) RETURN(1)
	for(int i=0;i<MP3NUM;i++){
		if(Saver(MP3[i].New,256)) RETURN(1)
	}
	RETURN(0)
}
int LoadMP3(int /*ver*/)
{
	STARTNA(__LINE__, 0)
//  if(ver<10){ // не было этого вообще
		ResetMP3();
//    return 0;
//  }
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='L'||buf[1]!='M'||buf[2]!='P'||buf[3]!='3')
		{MError("LoadMP3 cannot start loading"); RETURN(1)}
	for(int i=0;i<MP3NUM;i++){
		if(Loader(MP3[i].New,256)) RETURN(1)
	}
	RETURN(0)
}
/*
static char* GetStd(int ind)
{
	if((ind<0)||(ind>58)) ind=0;
	return MP3Names[ind];
}
*/
static int FindThis(char *name, bool old)
{
	STARTNA(__LINE__,name) //  STARTC("MP3: Find This",name)
	int   i;
	_MP3 *mp;
	char  buf[256];
	StrCopy(buf,256,name);
	ToLower(buf);
	for(i=0;i<MP3NUM;i++){
		mp=&MP3[i];
		if(StrCmp(buf, ((old || mp->New[0] == 0) ? mp->Old : mp->New))){ // нашли
			RETURN(i)
		}
	}
	RETURN(-1)
}

void MP3Process(void)
{
	STARTNA(__LINE__, 0)
	int i;
	MP3Name = (char*)0x6A33F4; // 4 thread
	i=FindThis(MP3Name, true);
	if(i>=0 && MP3[i].New[0]!=0) // нашелся и он изменен
		StrCopy(MP3Name,256,MP3[i].New);
	MP3Call(); // пробежим по тригерам
	MP3Name = (char*)0x6A32F0; // 4 passing as parameter
	RETURNV
}

extern volatile long WaitForWav;
extern volatile long WaitForWavTimeout;

void Mp3WaitForWav()
{
	if (InterlockedExchange(&WaitForWav, 0) == 0) return;
	int timeout = WaitForWavTimeout;
	int sndMan = *(int*)0x699414;
	LPCRITICAL_SECTION cs = (LPCRITICAL_SECTION)(sndMan + 144);
	int (__stdcall *AIL_sample_status)(int wav);
	*(int*)&AIL_sample_status = *(int*)0x63A424;
	while (timeout > (int)getTime())
	{
		if (*(int*)0x6992E0)  return;
		if (*(int*)(sndMan + 60) == 0)  return;
		if (*(int*)(sndMan + 124))
		{
			EnterCriticalSection(cs);
			int i = *(int*)(sndMan + 124) - 1;
			for (; i >= 0; i--)
				if (AIL_sample_status(*(int*)(sndMan + 68 + i*4)) == 4)  break; // still playing
			LeaveCriticalSection(cs);
			if (i < 0)  return; // none playing
		}
		Sleep(50);
	}
}

__declspec( naked ) void MP3Start(void)
{
	_asm
	{
		pusha
		call Mp3WaitForWav
		popa
		mov    edi,0x6A33F4
		ret
	}
}

char* PlayMP3Name = (char*)0x6A32F0;
int* PlayNoMP3 = (int*)0x699658;

void PlayMP3()
{
	_asm
	{
		mov ecx, 0x699414
		mov ecx, [ecx] // soundManager
		mov eax, 0x59AF00 // 0x59AFB0
		call eax
	}
}

int ERM_MP3(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i]) //  STARTC("Process !!MP command",0)
	int ind,v;
	switch(Cmd){
		case 'C': // C$  играть этот/какой играем
			CHECK_ParamsNum(1);
			v = FindThis(MP3Name, false) + 1;
			if(Apply(&v,4,Mp,0)) break; // check or get
			if((v<1)||(v>MP3NUM)){ MError2("wrong MP3 table index (1...200)."); RETURN(0) }
			if(MP3[v-1].New[0]) // was set
				StrCopy(MP3Name,256,MP3[v-1].New);
			else
				StrCopy(MP3Name,256,MP3[v-1].Old);
			break;
		case 'P': // P^text^ , P1/$ играем сейчас
			CHECK_ParamsMax(2);
			if(Num==2){ // с переменной (устарело)
				if(Mp->n[0]==1){
					if(BAD_INDEX_LZ(Mp->n[1])){ MError("\"MP:P\"-wrong z var index (-10...-1, 1...1000+)."); RETURN(0) }
					const char * str = GetErmString(Mp->n[1]);
					if (str) StrCopy(MP3Name,256,str);
				}else{ MError("\"MP:P\"-wrong syntax (P1/$)."); RETURN(0) }
			}else{
				StrMan::Apply(MP3Name, Mp, 0, 256);
			}
			break;
		case 'S':    // S#/$ установить/получить файл для #
			           // S#/^text^ установить файл для #
			CHECK_ParamsMax(2);
			ind=0;
			if(Apply(&ind,4,Mp,0)){ MError2("cannot get or check num."); RETURN(0) }
			if((ind<1)||(ind>MP3NUM)){ MError2("wrong MP3 table index (1...200)."); RETURN(0) }
			if(Num==1){ // S# восстановить оригинальный
				StrCopy(MP3[ind-1].New,256,"");
			}else if(Num==2){
				StrMan::Apply(MP3[ind-1].New, Mp, 1, 256);
			}else{ EWrongParamsNum(); RETURN(0) }
			break;
		case 'N':
		{
			CHECK_ParamsMax(1);
			if (Mp->VarI[0].Type == vtZ || Mp->VarI[0].Type == 0 && Mp->VarI[0].Num == 0)
			{
				StrMan::Apply(PlayMP3Name, Mp, 0, 256);
			}
			else
			{
				if (Apply(&v, 4, Mp, 0)) break;
				if((v<1)||(v>MP3NUM)){ MError2("wrong MP3 table index (1...200)."); RETURN(0) }
				if (MP3[v-1].New[0])
					StrCopy(PlayMP3Name, 256, MP3[v-1].New);
				else
					StrCopy(PlayMP3Name, 256, MP3[v-1].Old);
			}
			PlayMP3();
			break;
		}
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}
// играет MP3 по номеру
//:0059A4E3 8B4508         mov    eax,[ebp+08]
//:0059A4E6 83F802         cmp    eax,00000002
//:0059A4E9 7C16           jl     HEROES3.0059A501
//:0059A4EB 83F80A         cmp    eax,0000000A
//:0059A4EE 7F11           jg     HEROES3.0059A501
//:0059A4F0 8B0485304B6800 mov    eax,[4*eax+00684B30]
//:0059A4F7 6A00           push   00000000
//:0059A4F9 6A00           push   00000000
//:0059A4FB 50             push   eax
//:0059A4FC E8AF0A0000     call   HEROES3.0059AFB0

// вызов проц выше
//:004080A4 33D2           xor    edx,edx
//:004080A6 8A9130836700   mov    dl,[ecx+00678330]
//:004080AC 8B0D14946900   mov    ecx,[00699414]
//:004080B2 52             push   edx
//:004080B3 E828241900     call   HEROES3.0059A4E0

// еще один вызов
//:005C70E2 8B4B38         mov    ecx,[ebx+38]
//:005C70E5 6A01           push   00000001
//:005C70E7 56             push   esi
//:005C70E8 0FBE5104       movsx  edx,byte ptr [ecx+04]
//:005C70EC 8B0D14946900   mov    ecx,[00699414]
//:005C70F2 8B0495CC366400 mov    eax,[4*edx+006436CC]
//:005C70F9 50             push   eax
//:005C70FA E8B13EFDFF     call   HEROES3.0059AFB0
///////////////////////
//#define LOOPSNDNUM 0x46
//static struct _LSND{
//  char  New[256];
//} LSND[LOOPSNDNUM]={
//};

extern int InGame;

char SP_Current[256];
char* __fastcall SoundProcess(char *name)
{
	//if (InGame <= 0) return name;
	
	STARTNA(__LINE__, 0)
	StrCopy(SP_Current, 256, name);
	SoundCall(); // пробежим по тригерам
	RETURN(SP_Current);
}

int ERM_Sound(char Cmd,int Num,_ToDo_*,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i]) //  STARTC("Process !!SN command",0)
	char *sound=0;
	switch(Cmd){
		case 'S':    // S# получить/установить тек файл в z#
			CHECK_ParamsNum(1);
			StrMan::Apply(SP_Current, Mp, 0, 256);
			break;
		case 'P':    // P# проиграть файл в z#
			CHECK_ParamsNum(1);
			sound = GetErmText(Mp, 0);
			if (sound == 0) RETURN(0)
			__asm{
				mov    ecx,sound
				mov    edx,-1
				push   3
				mov    eax,0x59A890
				call   eax
			}
			break;
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

