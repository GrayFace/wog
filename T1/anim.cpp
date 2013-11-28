#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "b1.h"
#include "anim.h"
#define __FILENUM__ 9

#define OANIMNUM 1000

/////////////////////////
struct TAnim{
	short SAnim; // стиль анимации-один. для родственных анимаций 0-нет
	Word  OType;
	Word  OSType;
	int   dx,dy; // полный размер картинки
	int   vx,vy; // номер пустой клетки для заполнения
	char *parts; 
//  int   Turns; // время до след изменения
} TAnimation[]={
	{1, 207,1, 6,1,5,0,
#define PARTLEN 9
//# - номер фазы (первый #=0, начало анимации #=1)
//turns - время нахождения в фазе
//x - смещение до правого нижнего квадратика фазы
//y -"-
//dx - число клеток по x
//dy - число клеток по y
//InfSAnim - тип анимации для распространения
//InfTime - задержка распространения
//PathEn - разрешить проход (0-нет)
//-1 - последний
			"\x0\x9\x0\x0\x1\x1\x0\x0\x0" // 1-обычный
			"\x1\x1\x1\x0\x1\x1\x0\x0\x0" // 2-начало
			"\x2\x2\x2\x0\x1\x1\x1\x2\x0" // 3-максимум - поджигает
			"\x3\x1\x3\x0\x1\x1\x0\x0\x0" // 4-тухнет
			"\x4\x2\x4\x0\x1\x1\x0\x0\x1" // 5-затух - пепелище
			"\x5\x2\x0\x0\x1\x1\x0\x0\x0" // 6-вновь обычный (время охраняет от быстрого нового загорания)
			"\xFF"},
	{-1},
};
struct OAnim{
	char   Used;        // +00 1-занят 0-свободен
	char   Mode;        // +01 режим режима работы (см. выше)
	Word   LType;       // +02 тип, данный при загрузке (заполняется при иниц)
	short  x,y;         // +04 позиция
	char   l;           // +08 уровень
	char   dx,dy;       // +09 текущий видимымый участок
	char   CurState;    // +0B текущая фаза
	Byte   CurTime;     // +0C время нахождения в текущей фазе
	Byte   InfectTime;  // +0D зарожает соседей через такое время
	short  InfectSAnim; // +0E зарожает соседей номер стиля анимации
	char   NextState;   // +10 след. фаза
	Byte   NextTime;    // +11 через какое время включить след. фазу
	short  TAnimNum;    // +12 индекс описание анимации типа
//Byte _even[0x20-0x14];
}OAnimation[OANIMNUM];

/*
int CalcAllSAnim(Word SAnim)
{
	int num;
	TAnim *ta;
	for(num=0,ta=TAnimation;;ta++){
		if(ta->SAnim==-1) return num;
		if(ta->SAnim==SAnim) num++;
	}
}

short FindNextSAnim(short CurTAnim,Word SAnim)
{
	int    i;
	TAnim *ta;

	if(CurTAnim==-1){ i=0; ta=TAnimation; }
	else{ i=CurTAnim+1; ta=&TAnimation[i+1]; }
	for(;;ta++,i++){
		if(ta->SAnim==-1) return -1;
		if(ta->SAnim==SAnim) return (short)i;
	}
}
*/
/////////////////////////
static int Touch(int x,int y,int l,int dx,int dy,OAnim *oa)
{
	STARTNA(__LINE__, 0)
	if(l!=oa->l) RETURN(0)
	if(x>oa->x){ if(x>(oa->x+oa->dx)) RETURN(0) }
	else{ if(oa->x>(x+dx)) RETURN(0) }
	if(y>oa->y){ if(y>(oa->y+oa->dy)) RETURN(0) }
	else{ if(oa->y>(y+dy)) RETURN(0) }
	RETURN(1)
}

void Infection(OAnim *oa, char Mode,char State)
{
	STARTNA(__LINE__, 0)
	short  k;
	TAnim *ta,*tao;

	tao=&TAnimation[oa->TAnimNum];
	for(k=0,ta=TAnimation;;k++,ta++){
		if(ta->SAnim==-1) break; // дошли до конца - такой анимации не нашли
		if(ta->SAnim!=tao->SAnim) continue; // не тот тип
		// проверим на тип и подтип объекта
		if(ta->OType!=tao->OType) continue;
		if(ta->OSType!=tao->OSType) continue;
		oa->Mode=Mode;
		oa->NextState=State; 
		oa->CurTime=0;   // 
		oa->NextTime=0;  // немедленно
		oa->TAnimNum=k;  // сменим тип
	}  
	RETURNV
}

void InfectIt(OAnim *oas)
{
	STARTNA(__LINE__, 0)
	int    i;
	OAnim *oa;
	
	for(i=0;i<OANIMNUM;i++){
		oa=&OAnimation[i];
		if(oa->Used==0) continue;
		if(oa->Mode!=OAM_NOTHING) continue;
		if(Touch(oas->x,oas->y,oas->l,oas->dx,oas->dy,oa)){
			Infection(oa,oas->Mode,(char)oas->InfectSAnim);
		}
	}
	RETURNV
}

Word GetDCode(int x,int y){ return (Word)((y<<4)+x+((y+1)<<8)); }

void ChangeState(OAnim *oa,char State,int Auto)
{
	STARTNA(__LINE__, 0)
	int    i,j,k,l,x,y,dx,dy,_x,_y,tx,ty;
	char   c,path;
	short  is;
	Byte   it;
	ODraw *dr;
	_MapItem_ *MIp;
	TAnim *ta=&TAnimation[oa->TAnimNum];

	for(k=0;;k+=PARTLEN){
		c=ta->parts[k];
		if(c==-1){ // не нашли такого состояния
			oa->Mode=OAM_NOTHING; // все остановим
			RETURNV
		}
		if(c==State){
			x=ta->parts[k+2];  y=ta->parts[k+3];
			dx=ta->parts[k+4]; dy=ta->parts[k+5];
			is=ta->parts[k+6]; it=ta->parts[k+7];
			path=ta->parts[k+8]; // проходимость
			for(j=0;j<ta->dy;j++){
				for(i=0;i<ta->dx;i++){
					if((i<dx)&&(j<dy)){ _x=x+i; _y=j+y; }
					else{ _x=ta->vx; _y=ta->vy; }
					tx=oa->x-i; ty=oa->y-j;
					if((tx>=0)&&(ty>=0)){ // видимая часть
						MIp=GetMapItem(tx,ty,oa->l);
						for(dr=MIp->Draw;dr<MIp->DrawEnd;dr++){
							if(dr->LType!=oa->LType) continue;
							dr->Code=GetDCode(_x,_y);
							break;
						}
					}  
				}
			}  
			oa->dx=(char)dx; oa->dy=(char)dy;
			oa->CurState=State;
			oa->CurTime=0;
			oa->InfectSAnim=is; 
			oa->InfectTime=it;
			// устанавливаем доступность клетки
			MIp=GetMapItem(oa->x,oa->y,oa->l);
//      if(path) MIp->Bits&=0xFFFFFEFF;  // открываем
//      else     MIp->Bits|=0x00000100;  // закрываем
			if(path) MIp->Attrib&=0xFE;  // открываем
			else     MIp->Attrib|=0x01;  // закрываем
			if(Auto){
				for(l=0;;l+=PARTLEN){
					c=ta->parts[l];
					if(c==-1) break;
					if(c==(State+1)){
						oa->NextState=(char)(State+1);
						oa->NextTime=ta->parts[l+1];
						RETURNV
					}
				}  
				// не нашли такого состояния
				if(Auto==2){ // NONSTOP
					for(l=0;;l+=PARTLEN){
						c=ta->parts[l];
						if(c==-1) break;
						if(c==0){
							oa->NextState=0;
							oa->NextTime=ta->parts[l+1];
							RETURNV;
						}
					}
				}
				oa->Mode=OAM_NOTHING; // все остановим
				RETURNV
			}else{
				oa->Mode=OAM_NOTHING; // все сделали - остановим
			}  
			RETURNV
		}
	}
//  RETURNV
}

void AnimateAll(void)
{
	STARTNA(__LINE__, 0)
	int    i,Auto;
	OAnim *oa;
	
	for(i=0;i<OANIMNUM;i++){
		oa=&OAnimation[i];
		if(oa->Used==0) continue;
		switch(oa->Mode){
			case OAM_MANUAL:  Auto=0; break;
			case OAM_AUTO:    Auto=1; break;
			case OAM_NONSTOP: Auto=2; break;
			default: continue;
		}
		if(oa->CurTime>=oa->NextTime){
			ChangeState(oa,oa->NextState,Auto);
		}
		oa->CurTime++;
	}
	RETURNV
}

int GetFreeOAnim(void)
{
	STARTNA(__LINE__, 0)
	int    k;
	for(k=0;k<OANIMNUM;k++){
		if(OAnimation[k].Used==0) RETURN(k)
	}  
	RETURN(-1)
}

void InitAllAnimation(void)
{
	STARTNA(__LINE__, 0)
	int k;
	for(k=0;k<OANIMNUM;k++){
		OAnimation[k].Used=0;
	}  
	RETURNV
}

void StartAnimation(void)
{
	STARTNA(__LINE__, 0)
	int   oanum;
	Word  PlacedNum;
//  int   MapSize;
//  short i,j;
	short k;
//  char  l,ln;
	TAnim *ta;
	OAnim *oa;
	ODraw *dr;
	Dword Ot,OSt;
	_MapItem_ *MIp;
	_Position_ *PosS,*PosE,*p;
	_Types_    *TypS,*Typ;  
	InitAllAnimation();
	//if(WoG==0) RETURNV
	__asm{
		mov   ecx,BASE
		mov   ecx,[ecx]
		mov   eax,[ecx+0x1FB74]
		mov   TypS,eax
		mov   eax,[ecx+0x1FB84]
		mov   PosS,eax
		mov   eax,[ecx+0x1FB88]
		mov   PosE,eax
	}
	for(PlacedNum=0,p=PosS;p<PosE;p++,PlacedNum++){
		Typ=&TypS[p->num];
		Ot=Typ->type;
		OSt=Typ->subtype;
		for(k=0,ta=TAnimation;;k++,ta++){
			if(ta->SAnim==-1) break; // дошли до конца
			if((ta->OType==Ot)&&(ta->OSType==OSt)){ // есть такой тип
				oanum=GetFreeOAnim();
				if(oanum==-1){ 
					// не нашли пустое место. надо прорисовать
					// нулевую фазу и в дальнейшем не обрабатывать
					break;
				}
				oa=&OAnimation[oanum];
				oa->TAnimNum=k; // запомним первую подходящую
				oa->x=p->x; oa->y=p->y; oa->l=p->l;
				oa->Mode=OAM_MANUAL;
				// найдем родной LType
				MIp=GetMapItem(p->x,p->y,p->l);
				for(dr=MIp->Draw;dr<MIp->DrawEnd;dr++){
					// тут возможны проблемы, если два на одном месте :-(
					// ??????????????
					if(dr->Code==0x0100){       // нашли нулевой
						if(dr->LType==PlacedNum){ // нашли родной
//            oa->LType=dr->LType;
							oa->LType=PlacedNum;
							oa->NextState=0; // в 0-е состояние
							oa->CurTime=0;   // 
							oa->NextTime=0;  // немедленно
							oa->Used=1;      // все - добавили
							break;
						}  
					}  
				}
			}
		}
	}
				
/*
	MapSize=GetMapSize();
	ln=(char)(GetMapLevels()+1);
	for(l=0;l<ln;l++){
		for(j=0;j<MapSize;j++){
			for(i=0;i<MapSize;i++){
				MIp=GetMapItem(i,j,l);
				Ot=MIp->OType; OSt=MIp->OSType;
				for(k=0,ta=TAnimation;;k++,ta++){
					if(ta->SAnim==-1) break; // дошли до конца
					if((ta->OType==Ot)&&(ta->OSType==OSt)){ // есть такой тип
						oanum=GetFreeOAnim();
						if(oanum==-1){ 
							// не нашли пустое место. надо прорисовать
							// нулевую фазу и в дальнейшем не обрабатывать
							break;
						}
						oa=&OAnimation[oanum];
						oa->TAnimNum=k; // запомним первую подходящую
						oa->x=i; oa->y=j; oa->l=l;
						oa->Mode=OAM_MANUAL;
//oa->Mode=OAM_NONSTOP;
						// найдем родной LType
						for(dr=MIp->Draw;dr<MIp->DrawEnd;dr++){
							if(dr->Code==0x0100){ // нашли родной
								oa->LType=dr->LType;
								oa->NextState=0; // в 0-е состояние
								oa->CurTime=0;   // 
								oa->NextTime=0;  // немедленно
								oa->Used=1;      // все - добавили
								break;
							}  
						}
					}
				}
			}
		}
	}      
*/        
	AnimateAll();
	RETURNV
}

void GlbAnimation(void)
{
	STARTNA(__LINE__, 0)
	int    k;
	OAnim *oa;

	for(k=0;k<OANIMNUM;k++){
		oa=&OAnimation[k];
		if(oa->Used==0) continue;
		if(oa->InfectSAnim<=0) continue; // не надо инфицировать
		if(oa->InfectTime>oa->CurTime) continue; // не пора
		InfectIt(oa);
	}
	AnimateAll();
	RETURNV
}

void ProvokeAnimation(int x,int y, int l,int dx, int dy, short Type, char Mode)
{
	STARTNA(__LINE__, 0)
	int i,k;
	OAnim *oa;
	TAnim *ta,*tao;
	
	for(i=0;i<OANIMNUM;i++){
	 oa=&OAnimation[i];
	 if(oa->Used==0) continue;
	 if(oa->Mode!=OAM_NOTHING) continue;
	 if(Touch(x,y,l,dx,dy,oa)){
		 tao=&TAnimation[oa->TAnimNum];
		 for(k=0,ta=TAnimation;;k++,ta++){
			 if(ta->SAnim==-1) break; // дошли до конца - такой анимации не нашли
			 if(ta->SAnim!=Type) continue; // не тот тип
			 // проверим на тип и подтип объекта
			 if(ta->OType!=tao->OType) continue;
			 if(ta->OSType!=tao->OSType) continue;
			 oa->Mode=Mode;
			 oa->NextState=1; // начнем с первого состояния
			 oa->CurTime=0;   // 
			 oa->NextTime=0;  // немедленно
			 oa->TAnimNum=(char)k;  // сменим тип
			}  
		}
	}
	RETURNV
}

int SaveAnimations(void)
{
	STARTNA(__LINE__, 0)
	if(Saver("LANI",4)) RETURN(1)
	if(Saver(OAnimation,sizeof(OAnimation))) RETURN(1)
	RETURN(0)
}

int LoadAnimations(int /*ver*/)
{
	STARTNA(__LINE__, 0)
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='L'||buf[1]!='A'||buf[2]!='N'||buf[3]!='I')
			{MError("LoadAnimation cannot start loading"); RETURN(1)}
	if(Loader(OAnimation,sizeof(OAnimation))) RETURN(1)
	RETURN(0)
}

