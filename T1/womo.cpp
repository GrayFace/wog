#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "erm.h"
#include "b1.h"
#include "womo.h"
#include "prot.h"
#define __FILENUM__ 7

//#define MS 144
short Way[MAXMAPSIZE+2][MAXMAPSIZE+2];
short Stp[MAXMAPSIZE+2][MAXMAPSIZE+2];
//#define L 1
//#define R 2
//#define T 4
//#define B 8
/* class Point{
 public:
	short x,y,l;
	static short LineHasPoint;
	static Point Line[144];
	Point(){x=0;y=0;l=0;}
	Point(int xi,int yi,int li){x=(short)xi; y=(short)yi; l=(short)li;}
	Point(Point &p){x=p.x;y=p.y;l=p.l;}
	operator ==(Point &p){if((x!=p.x)||(y!=p.y)||(l!=p.l)) return 0; else return 1; }
	void MakeLine(Point &p);
};*/
short Point::LineHasPoint=0;
Point Point::Line[144];

void Point::MakeLine(Point &p)
{
	STARTNA(__LINE__, 0)
	int i,xt,yt,e,de,dde,dx=x-p.x,dy=y-p.y,ddx,ddy;
	if((dx==0)&&(dy==0)){ LineHasPoint=0; RETURNV }
	if(dx<0) ddx=-dx; else ddx=dx; if(dy<0) ddy=-dy; else ddy=dy;
	if(ddx>=ddy){ // 1,4,5,8
		if(dx>0){ // 1,8
			if(dy>=0){ // 1
				e=ddx; de=ddy+ddy; dde=ddx+ddx;
				for(i=0,xt=p.x,yt=p.y;xt<=x;xt++,i++){ Line[i]=Point(xt,yt,l); e-=de; if(e<0){ e+=dde; yt++; }}
				LineHasPoint=(short)(ddx+1);
			}else{ // 8
				e=ddx; de=ddy+ddy; dde=ddx+ddx;
				for(i=0,xt=p.x,yt=p.y;xt<=x;xt++,i++){ Line[i]=Point(xt,yt,l); e-=de; if(e<0){ e+=dde; yt--; }}
				LineHasPoint=(short)(ddx+1);
			}
		}else{ // 4,5
			if(dy>=0){ // 4
				e=ddx; de=ddy+ddy; dde=ddx+ddx;
				for(i=0,xt=p.x,yt=p.y;xt>=x;xt--,i++){ Line[i]=Point(xt,yt,l); e-=de; if(e<0){ e+=dde; yt++; }}
				LineHasPoint=(short)(ddx+1);
			}else{ // 5
				e=ddx; de=ddy+ddy; dde=ddx+ddx;
				for(i=0,xt=p.x,yt=p.y;xt>=x;xt--,i++){ Line[i]=Point(xt,yt,l); e-=de; if(e<0){ e+=dde; yt--; }}
				LineHasPoint=(short)(ddx+1);
			}
		}
	}else{ // 2,3,6,7
		if(dy>0){ // 2,3
			if(dx>=0){ // 2
				e=ddy; de=ddx+ddx; dde=ddy+ddy;
				for(i=0,yt=p.y,xt=p.x;yt<=y;yt++,i++){ Line[i]=Point(xt,yt,l); e-=de; if(e<0){ e+=dde; xt++; }}
				LineHasPoint=(short)(ddy+1);
			}else{ // 3
				e=ddy; de=ddx+ddx; dde=ddy+ddy;
				for(i=0,yt=p.y,xt=p.x;yt<=y;yt++,i++){ Line[i]=Point(xt,yt,l); e-=de; if(e<0){ e+=dde; xt--; }}
				LineHasPoint=(short)(ddy+1);
			}
		}else{ // 6,7
			if(dx>=0){ // 7
				e=ddy; de=ddx+ddx; dde=ddy+ddy;
				for(i=0,yt=p.y,xt=p.x;yt>=y;yt--,i++){ Line[i]=Point(xt,yt,l); e-=de; if(e<0){ e+=dde; xt++; }}
				LineHasPoint=(short)(ddy+1);
			}else{ // 6
				e=ddy; de=ddx+ddx; dde=ddy+ddy;
				for(i=0,yt=p.y,xt=p.x;yt>=y;yt--,i++){ Line[i]=Point(xt,yt,l); e-=de; if(e<0){ e+=dde; xt--; }}
				LineHasPoint=(short)(ddy+1);
			}
		}
	}
	RETURNV
}
int Map(int x,int y,int l,int where)
{
	STARTNA(__LINE__, 0)
	_MapItem_ *MIp;
	MIp=GetMapItem(x,y,l);
	switch(where){
		case 0: // land
			if(EmptyPlace(MIp)==0) RETURN(1) else RETURN(0)
		case 1: // water
			if(EmptyPlaceWater(MIp)==0) RETURN(1) else RETURN(0)
		case 2: // any
		default:
			if(EmptyPlaceAny(MIp)==0) RETURN(1) else RETURN(0)
	}
}

int FindNearestHero(int x,int y,int l,int d,int owner)
{
	STARTNA(__LINE__, 0)
	int i,hn,hdx=0,hdy=0,dx,dy;
	_Hero_ *hp;
	for(hn=-1,i=0;i<HERNUM;i++){
		hp=GetHeroStr(i);
		if(hp==0) RETURN(-1)
		if(hp->Owner==-1) continue; // ничей
		if(hp->Visible==0) continue;  // не на карте
		if(owner!=-1){ if(hp->Owner!=owner) continue; }// нитот
		if(hp->l!=l) continue;
		dx=hp->x-x; if(dx<0) dx=-dx; dy=hp->y-y; if(dy<0) dy=-dy;
		if((dx>d)||(dy>d)) continue;
		if(hn==-1){ hdx=dx; hdy=dy; hn=i; }
		else{ if((dx-hdx+dy-hdy)<0){ hdx=dx; hdy=dy; hn=i; } }
	}
	RETURN(hn)
}

int FindNearestTown(int x,int y,int l,int d,int owner)
{
	STARTNA(__LINE__, 0)
	_CastleSetup_ *Castle;
	int i,Num,hn,hdx=0,hdy=0,dx,dy;
	Castle=GetTownBase();
	if(Castle==0) RETURN(-1)
	Num=GetTownNum();
	for(hn=-1,i=0;i<Num;i++,Castle++){
		if(owner!=-1){ if(Castle->Owner!=owner) continue; }// нитот
		if(Castle->l!=l) continue;
		dx=Castle->x-x; if(dx<0) dx=-dx; dy=Castle->y-y; if(dy<0) dy=-dy;
		if((dx>d)||(dy>d)) continue;
		if(hn==-1){ hdx=dx; hdy=dy; hn=i; }
		else{ if((dx-hdx+dy-hdy)<0){ hdx=dx; hdy=dy; hn=i; } }
	}
	RETURN(hn)
}

int FindWay(Point &s,Point &e,int where)
{
	STARTNA(__LINE__, 0)
	short i,j,k,l,d,v,change,sz;

	for(i=0;i<MAXMAPSIZE;i++){
		for(j=0;j<MAXMAPSIZE;j++){
			Way[i+1][j+1]=-1;
		}
	}
	sz=(short)GetMapSize();
	for(i=0;i<sz;i++)
		for(j=0;j<sz;j++)
			if(Map(i,j,s.l,where)) Way[i+1][j+1]=-1; else Way[i+1][j+1]=0;
	for(i=0;i<MAXMAPSIZE+2;i++){
		Way[0][i]=-1; Way[MAXMAPSIZE+1][i]=-1; Way[i][0]=-1; Way[i][MAXMAPSIZE+1]=-1;
	}
	Way[s.x+1][s.y+1]=0; //начальная точка
	Way[e.x+1][e.y+1]=1; //конечная точка
	if(s==e) RETURN(1)
	do{
		change=0;
		for(i=1;i<MAXMAPSIZE+1;i++)
			for(j=1;j<MAXMAPSIZE+1;j++){
				if(Way[i][j]>0){ // есть что делать
					for(k=-1;k<2;k++)
						for(l=-1;l<2;l++){
							if((k==0)&&(l==0)) continue;
							if(Way[i+k][j+l]==-1) continue;
							if((k!=0)&&(l!=0)) d=3; // диагональ
							else d=2; // гор или вер
							v=(short)(Way[i][j]+d);
							if((v<Way[i+k][j+l])||(Way[i+k][j+l]==0)){
								Way[i+k][j+l]=v;
								change=1;
							}
						}
				}
			}
//    if(Way[s.x+1][s.y+1]!=0) break;
	}while(change!=0); // пока что-то менялось
//  }while(Way[s.x][s.y]==0); // пока в начальную точку не запишем что-то
	RETURN(0)
}

void MakeWay(Point &s,Point &e)
{
	STARTNA(__LINE__, 0)
	short i,j,k,l,v,m,n,step;

	for(i=0;i<MAXMAPSIZE+2;i++)
		for(j=0;j<MAXMAPSIZE+2;j++)
			Stp[i][j]=0;
	step=1;
	i=(short)(s.x+1); j=short(s.y+1);
	m=i; n=j;
	do{
		v=Way[i][j];
		for(k=-1;k<2;k++){
			for(l=-1;l<2;l++){
				if((k==0)&&(l==0)) continue;
				if(Way[i+k][j+l]<=0) continue;
				if(Way[i+k][j+l]<v){
					v=Way[i+k][j+l]; m=short(i+k); n=short(j+l);
				}
			}
		}
//    if(v==-1) return;
		Stp[m][n]=step++; i=m; j=n;
	}while(((e.x+1)!=m)||((e.y+1)!=n)); // пока не дошли
	RETURNV
}

Dword MakeWay(Point &s,Point &e,int Steps)
{
	STARTNA(__LINE__, 0)
	short i,j,k,l,v,m,n,step;

	for(i=0;i<MAXMAPSIZE+2;i++)
		for(j=0;j<MAXMAPSIZE+2;j++)
			Stp[i][j]=0;
	step=1;
	i=(short)(s.x+1); j=short(s.y+1);
	m=i; n=j;
	do{
		v=Way[i][j];
		for(k=-1;k<2;k++){
			for(l=-1;l<2;l++){
				if((k==0)&&(l==0)) continue;
				if(Way[i+k][j+l]<=0) continue;
				if(Way[i+k][j+l]<v){
					v=Way[i+k][j+l]; m=short(i+k); n=short(j+l);
				}
			}
		}
//    if(v==-1) return;
		Stp[m][n]=step++; i=m; j=n;
		if(step==Steps){ // достали - выходим
			break;
		}
	}while(((e.x+1)!=m)||((e.y+1)!=n)); // пока не дошли
	RETURN(PosMixed(m-1,n-1,s.l))
}

int FindStep(Point &s,Point &e,int where)
{
	STARTNA(__LINE__, 0)
/*
	if(s.l!=e.l) return 0;
	if(FindWay(s,e)==1) return 3; // уже на месте
	if(Way[s.x+1][s.y+1]<=0) return 0; // нет пути
	MakeWay(s,e);
	if(Stp[e.x+1][e.y+1]==1) return 2; // где-то на соседней позиции
	return 1; // есть путь
*/
	int i;
	if(s.l==e.l){
		if(FindWay(s,e,where)==1) RETURN(3) // уже на месте
	}
	if((s.l!=e.l)||(Way[s.x+1][s.y+1]<=0)){ // нет пути
		s.MakeLine(e);
//asm int 3
		for(i=0;i<s.LineHasPoint;i++){
			if(FindWay(s,s.Line[i],where)==1) RETURN(6) // не дойти, но дальше никак
			if(Way[s.x+1][s.y+1]<=0) continue; // нет пути
			MakeWay(s,s.Line[i]);
			if(Stp[s.Line[i].x+1][s.Line[i].y+1]==1) RETURN(5) // где-то на соседней позиции
			RETURN(4) // есть путь но не до конца
		}
		RETURN(0) // нет пути
	}
	MakeWay(s,e);
	if(Stp[e.x+1][e.y+1]==1) RETURN(2) // где-то на соседней позиции
	RETURN(1) // есть путь
}

int FindStep(int x,int y,int l,int x1,int y1,int l1,Dword *MixPos,int Steps,int where){
	STARTNA(__LINE__, 0)
	int i;
	Point s(x,y,l),e(x1,y1,l1),t;
	t=e;
	if(l==l1){
		if(FindWay(s,e,where)==1) RETURN(2) // уже на месте
	}
	if((l!=l1)||(Way[x+1][y+1]<=0)){ // нет пути
		s.MakeLine(e);
		for(i=0;i<Point::LineHasPoint;i++){
			if(FindWay(s,Point::Line[i],where)==1) RETURN(2)
			if(Way[s.x+1][s.y+1]<=0) continue; // нет пути
			t=Point::Line[i]; goto _Ok;
		}
		RETURN(0) // нет пути
	}
_Ok:
	*MixPos=MakeWay(s,t,Steps);
	RETURN(1)
}
#define WOMONUM 1000
static struct _WoMo_{
	char  ToDo,Player;
	Byte  Radius;
	struct _Flags_{
		unsigned Call4Dest   : 1;
		unsigned Call4Killed : 1;
		unsigned GoToDest    : 1;
		unsigned Search4Hero : 1;
		unsigned StandAfter  : 1;
		unsigned Search4Owner: 1;
		signed   Where       : 3; // 0-land,1-water,2-any
		unsigned Unused      :23;
	} Flags;
	Byte  x,y,l;
	Byte  new_x,new_y,new_l;
	Byte  des_x,des_y,des_l;
} WoMo[WOMONUM];
#define WMO_NOTUSED   0
#define WMO_STAND     1
#define WMO_MOVE      2
#define WMO_ATTACK    3
//#define WMO_GOTO      4

static void ClearWM(int ind)
{
	STARTNA(__LINE__, 0)
	_WoMo_ *wmp=&WoMo[ind];
	wmp->ToDo=WMO_NOTUSED;
	wmp->Player=0; wmp->Radius=0;
	*(Dword *)&wmp->Flags=0;
	wmp->x=wmp->y=wmp->l=0;
	wmp->new_x=wmp->new_y=wmp->new_l=0;
	wmp->des_x=wmp->des_y=wmp->des_l=0;
	RETURNV
}

static int FindWM(int x,int y, int l)
{
	STARTNA(__LINE__, 0)
	int i; _WoMo_ *wmp;
	for(i=0;i<WOMONUM;i++){
		wmp=&WoMo[i];
		if(wmp->ToDo==WMO_NOTUSED) continue;
		if((wmp->x==(Byte)x)&&(wmp->y==(Byte)y)&&(wmp->l==(Byte)l)) RETURN(i)
	}
	RETURN(-1)
}

static int AddWM(int x,int y, int l)
{
	STARTNA(__LINE__, 0)
	int i; _WoMo_ *wmp;
	if((i=FindWM(x,y,l))!=-1) RETURN(i) // уже есть на этом месте
	for(i=0;i<WOMONUM;i++){
		wmp=&WoMo[i];
		if(wmp->ToDo!=WMO_NOTUSED) continue;
		ClearWM(i);
		wmp->x=(Byte)x; wmp->y=(Byte)y; wmp->l=(Byte)l;
		wmp->new_x=(Byte)x; wmp->new_y=(Byte)y; wmp->new_l=(Byte)l;
		wmp->des_x=(Byte)x; wmp->des_y=(Byte)y; wmp->des_l=(Byte)l;
		wmp->Radius=20; wmp->Player=-1; wmp->ToDo=WMO_STAND;
		RETURN(i)
	}
	RETURN(-1)
}

static int DelWM(int x, int y,int l)
{
	STARTNA(__LINE__, 0)
	int i; _WoMo_ *wmp;
	for(i=0;i<WOMONUM;i++){
		wmp=&WoMo[i];
		if(wmp->ToDo==WMO_NOTUSED) continue;
		if(wmp->x!=(Byte)x) continue;
		if(wmp->y!=(Byte)y) continue;
		if(wmp->l!=(Byte)l) continue;
//    wmp->ToDo=WMO_NOTUSED;
		ClearWM(i);
		RETURN(i)
	}
	RETURN(-1)
}

static _CMonster_ WO_mo;
int WoMoPlace(int x,int y,int l,int subtype)
{
	STARTNA(__LINE__, 0)
	int ret;
	Dword stpword;
	WO_mo.Number=1;
	WO_mo.Agression=0x0A;
	WO_mo.NoMore=0;
	WO_mo.NoRun=1;
	WO_mo.SetUpInd=0;
	WO_mo.HasSetUp=0;
	stpword=*(Dword *)&WO_mo;
	ret=AddWM(x,y,l);
	if(ret==-1){ MError("WoMoPlace: Cannot add more Wandering Monsters"); RETURN(-1) }
	_PlaceObject(x,y,l,54,subtype,-1,stpword);
	CoorectMonsterRadius();
	RETURN(ret)
}
/*
int FindWoMoPosition(int x,int y,int l)
{
	int i,n;
	_Position_ *po=GetPositionBase();
	if(po==0){ MError("FindWoMoPosition: No Positions at all."); return -1; }
	n=GetPositionNum();
	for(i=n-1;i>=0;i--){
		if(po[i].x!=x) continue;
		if(po[i].y!=y) continue;
		if(po[i].l!=l) continue;
		return i;
	}
	MError("FindWoMoPosition: Cannot Find Corresponding Position.");
	return -1;
}
*/
int PlaceWoMoInternal(int subtype,int PoInd,_Position_ *po)
{
	STARTNA(__LINE__, 0)
	__asm{
		mov    ecx,BASE
		mov    ecx,[ecx]
		mov    edi,ecx // ecx->BASE
//mov    edx,0xFF
//xor    ecx,ecx
//mov    eax,0x50C7C0
//call   eax // случ число в AL от 00 до FF
//mov    edx,po
//mov    [edx+10],al
// mov    [ebp-02],al//=случ.число ?
		lea    esi,[edi+0x1FB70] // -> типы
		mov    eax,subtype
		mov    ecx,0x36
		mov    edx,po
		push   -1
		push   eax
		push   ecx
		mov    ecx,esi
		push   edx
		mov    eax,0x506570
		call   eax // поместить на карту
		mov    edx,PoInd
		push   1
		push   edx
		mov    ecx,esi
		mov    eax,0x506170
		call   eax
	}
	RETURN(0)
}

void RemDraw(int x,int y,int l,int Lind)
{
	STARTNA(__LINE__, 0)
	int fl;
	ODraw *dr;
	_MapItem_ *MIp=GetMapItem(x,y,l);
	do{
		fl=0;
		for(dr=MIp->Draw;dr<MIp->DrawEnd;dr++){
			if(dr->LType!=Lind) continue;
			fl=1; break;
		}
		if(fl){
			for(;dr<MIp->DrawEnd-1;dr++){
				dr[0].LType=dr[1].LType;
				dr[0].Code=dr[1].Code;
//        break;
			}
			--MIp->DrawEnd;
		}
	}while(fl!=0);
	RETURNV
}

int WoMoMove(int ind)
{
	STARTNA(__LINE__, 0)
	int x0,y0,l0,x1,y1,l1;
	int Lind;
	int subtype;
	Dword stpword;
//  _CMonster_ *SMp;
	_MapItem_ *MIp;
	_WoMo_ *wmp=&WoMo[ind];
	if(wmp->ToDo==WMO_NOTUSED){ MError("WoMoMove: Try to move not a Wandering Monster"); RETURN(-2) }
	x0=wmp->x; y0=wmp->y; l0=wmp->l; x1=wmp->new_x; y1=wmp->new_y; l1=wmp->new_l;
	MIp=GetMapItem(x0,y0,l0);
	subtype=MIp->OSType;
	stpword=MIp->SetUp;
//asm int 3
	if(MIp->OType!=54){ MError("WoMoMove: Not a monster."); RETURN(-2) }
//  int poind=FindWoMoPosition(x0,y0,l0); if(poind==-1) return -1;
	int poind=MIp->DrawNum; //if(poind==-1) return -1;
//  if(EmptyPlace(GetMapItem(x1,y1,l1))==0) return -1;
// 3.58 fix for WoMo on the Water
	if(Map(x1,y1,l1,wmp->Flags.Where)) RETURN(-1)
	DelObject(x0,y0,l0);
	_Position_ *po=&(GetPositionBase()[poind]);
	po->x=(Byte)x1; po->y=(Byte)y1; po->l=(Byte)l1;
	Lind=poind;//->num;
//asm int 3
	if(x0>=1){
//    MIp=GetMapItem(x0-1,y0,l0); MIp->Mirror&=0x7F;
		RemDraw(x0-1,y0,l0,Lind);
		if(y0>=1){
//      MIp=GetMapItem(x0-1,y0-1,l0); MIp->Mirror&=0x7F;
			RemDraw(x0-1,y0-1,l0,Lind);
		}
	}
	if(y0>=1){
//    MIp=GetMapItem(x0,y0-1,l0); MIp->Mirror&=0x7F;
		RemDraw(x0,y0-1,l0,Lind);
	}
//  MIp=GetMapItem(x0,y0,l0); MIp->Mirror&=0x7F;
	RemDraw(x0,y0,l0,Lind);
//  _PlaceObject(x1,y1,l1,54,subtype,-1,stpword);
		// 3.57f Legion WM bug fix 29.01.03
		MIp->OType=0; MIp->SetUp=0xFFFFFFFF; MIp->Attrib&=0xEF; MIp->DrawNum=0xFFFF;
	PlaceWoMoInternal(subtype,Lind,po);
	MIp=GetMapItem(x1,y1,l1);
//  if(MIp->SetUp==0xFFFFFFFF) return -1;
	if(MIp->OType!=54){ MError("WoMoMove: Monster was not placed at the new position."); RETURN(-2) }
	MIp->SetUp=stpword;
// 3.58 leave agression as is
//  SMp=(_CMonster_ *)MIp;
//  SMp->Agression=0x0B;
	CoorectMonsterRadius();
	RETURN(0)
}

int MakeWoMoPos(_MapItem_  *MIp,int YESno)
{
	STARTNA(__LINE__, 0)
	int x,y,l,ret;
	if(MIp==0) RETURN(-1)
	if(MIp->OType!=54) RETURN(-1)
	Map2Coord(MIp,&x,&y,&l);
	if(YESno){
		ret=AddWM(x,y,l);
		if(ret==-1){ MError("MakeWoMoPos: Cannot add more Wandering Monsters"); RETURN(-1) }
	}else{
		ret=DelWM(x,y,l);
	}
	RETURN(ret)
}

int MakeWoMo(int x,int y,int l,int YESno)
{
	STARTNA(__LINE__, 0)
	_MapItem_  *MIp;
	MIp=GetMapItem(x,y,l);
	RETURN(MakeWoMoPos(MIp,YESno))
}

int WhereTo(int ind)
{
	STARTNA(__LINE__, 0)
	_WoMo_ *wmp;
	int x,y,l,new_x,new_y;
	wmp=&WoMo[ind];
	x=wmp->x; y=wmp->y; l=wmp->l;
	new_x=x; new_y=y;
	if(Stp[x+1+1][y+1]!=0){ new_x=x+1; }
	else if(Stp[x+1+1][y+1-1]!=0){ new_x=x+1; new_y=y-1; }
	else if(Stp[x+1+1][y+1+1]!=0){ new_x=x+1; new_y=y+1; }
	else if(Stp[x+1][y+1-1]!=0){ new_y=y-1; }
	else if(Stp[x+1][y+1+1]!=0){ new_y=y+1; }
	else if(Stp[x+1-1][y+1]!=0){ new_x=x-1; }
	else if(Stp[x+1-1][y+1-1]!=0){ new_x=x-1; new_y=y-1; }
	else if(Stp[x+1-1][y+1+1]!=0){ new_x=x-1; new_y=y+1; }
	wmp->new_x=(Byte)new_x; wmp->new_y=(Byte)new_y; wmp->new_l=(Byte)l;
	if((new_x!=x)||(new_y!=y)) RETURN(1)
	else RETURN(0)
}
/*
int AddWoMo2Do(int ind,int todo)
{
	_WoMo_ *wmp;
	int x,y,l,new_x,new_y;

	wmp=&WoMo[ind];
	x=wmp->x; y=wmp->y; l=wmp->l;
	new_x=x; new_y=y;
	if((todo==WMO_MOVE))||(todo==WMO_ATTACK)){
		if(Stp[x+1+1][y+1]!=0){ new_x=x+1; }
		else if(Stp[x+1+1][y+1-1]!=0){ new_x=x+1; new_y=y-1; }
		else if(Stp[x+1+1][y+1+1]!=0){ new_x=x+1; new_y=y+1; }
		else if(Stp[x+1][y+1-1]!=0){ new_y=y-1; }
		else if(Stp[x+1][y+1+1]!=0){ new_y=y+1; }
		else if(Stp[x+1-1][y+1]!=0){ new_x=x-1; }
		else if(Stp[x+1-1][y+1-1]!=0){ new_x=x-1; new_y=y-1; }
		else if(Stp[x+1-1][y+1+1]!=0){ new_x=x-1; new_y=y+1; }
	}
	wmp->new_x=(Byte)new_x; wmp->new_y=(Byte)new_y; wmp->new_l=(Byte)l;
	if(hn>0) wmp->Player=GetHeroStr(hn)->Owner;
	else wmp->Player=-1;
//  wmp->hn=(short)hn;
	++WoMoAll;
	return 0;
}
*/
void CallWMKilled(int moind)
{
	STARTNA(__LINE__, 0)
	ERM_GM_ai=-1;
	ERMVar2[996]=moind+1;
	pointer=30306;
	ProcessERM();
	RETURNV
}
void CallWMDest(int moind)
{
	STARTNA(__LINE__, 0)
	ERM_GM_ai=-1;
	ERMVar2[996]=moind+1;
	pointer=30305;
	ProcessERM();
	RETURNV
}

void AutoWoMo(void)
{
	STARTNA(__LINE__, 0)
	int i,x,y,l,n,Owner;
	_MapItem_  *MIp;
//  _CMonster_ *MSp;
	_Hero_ *hp;
	_CastleSetup_ *cs;
	_WoMo_ *wmp;
	for(i=0;i<WOMONUM;i++){
		wmp=&WoMo[i];
		if(wmp->ToDo==WMO_NOTUSED) continue;
		wmp->ToDo=WMO_STAND;
		x=wmp->x; y=wmp->y;l=wmp->l;
		MIp=GetMapItem(x,y,l);
		if(MIp->OType!=54){ // убили монстра
			if(wmp->Flags.Call4Killed) CallWMKilled(i);
			ClearWM(i); continue;
		}
		if(wmp->Flags.GoToDest){
			if((wmp->x==wmp->des_x)&&(wmp->y==wmp->des_y)&&(wmp->l==wmp->des_l)){ // уже пришли
				wmp->Flags.GoToDest=0;
				if(wmp->Flags.Call4Dest) CallWMDest(i);
				continue; // уже пришли
			}
			switch(FindStep(Point(x,y,l),Point(wmp->des_x,wmp->des_y,wmp->des_l),wmp->Flags.Where)){
				case 2: // на соседней клетке
				case 5: // на соседней клетке (но не до конца)
				case 1: // есть путь
				case 4: // есть путь (но не до конца)
					if(WhereTo(i)){ wmp->ToDo=WMO_MOVE; wmp->Player=(Byte)CurrentUser(); }
					break;
				case 3: // уже пришел - стоим
				case 6: // уже пришел - стоим (но не до конца)
				default:;
			}
			continue;
		}
		if(wmp->Flags.Search4Hero){
			hp=GetHeroStr(wmp->des_x);
			if(hp->Owner==-1){ // ничейный
				wmp->Flags.Search4Hero=0;
			}else{
				switch(FindStep(Point(x,y,l),Point(hp->x,hp->y,hp->l),wmp->Flags.Where)){
					case 1: // есть путь
					case 4: // есть путь (но не до конца)
						if(WhereTo(i)){ wmp->ToDo=WMO_MOVE; wmp->Player=hp->Owner; }
						break;
					case 2: // атака
					case 5: // атака (но не до конца)
					case 3: // уже пришел (не должно быть)
					case 6: // уже пришел (но не до конца)
						if(WhereTo(i)){ wmp->ToDo=WMO_ATTACK; wmp->Player=hp->Owner; }
					default:;
				}
				continue;
			}
		}
		Owner=-1;
		if(wmp->Flags.Search4Owner){
			Owner=wmp->des_x;
		}else{
			if(wmp->Flags.StandAfter) continue;
		}
		n=FindNearestHero(x,y,l,wmp->Radius,Owner);
		if(n!=-1){
			hp=GetHeroStr(n);
			switch(FindStep(Point(x,y,l),Point(hp->x,hp->y,hp->l),wmp->Flags.Where)){
				case 1: // есть путь
				case 4: // есть путь (но не до конца)
				case 5: // атака (но не до конца)
					if(WhereTo(i)){ wmp->ToDo=WMO_MOVE; wmp->Player=hp->Owner; }
					break;
				case 2: // атака
				case 3: // уже пришел (не должно быть)
					if(WhereTo(i)){ wmp->ToDo=WMO_ATTACK; wmp->Player=hp->Owner; }
					break;
				case 6: // уже пришел (но не до конца)
				default:;
			}
		}
		if(wmp->ToDo!=WMO_STAND) continue; // уже нашли что делать
		n=FindNearestTown(x,y,l,wmp->Radius,Owner);
		if(n!=-1){
			cs=&(GetTownBase()[n]);
			switch(FindStep(Point(x,y,l),Point(cs->x,cs->y+1,cs->l),wmp->Flags.Where)){
				case 1: // есть путь
				case 4: // есть путь (но не до конца)
				case 2: // на соседней клетке
				case 5: // на соседней клетке (но не до конца)
					if(WhereTo(i)){ wmp->ToDo=WMO_MOVE; wmp->Player=cs->Owner; }
					break;
				case 3: // уже пришел - стоим
				case 6: // уже пришел - стоим (но не до конца)
				default:;
			}
		}
//    if(wmp->ToDo!=WMO_STAND) continue; // уже нашли что делать
	}
	RETURNV
}
/*
void AutoWoMo(void)
{
	int i,x,y,l,sz,ln,n;
	_MapItem_  *MIp;
	_CMonster_ *MSp;
	_Hero_ *hp;
	_CastleSetup_ *cs;
	sz=GetMapSize();
	ln=GetMapLevels()+1;
	WoMoAll=0;
	for(l=0;l<ln;l++){
		for(y=0;y<sz;y++){
			for(x=0;x<sz;x++){
				MIp=GetMapItem(x,y,l);
				if(MIp->OType!=54) continue;
				MSp=(_CMonster_ *)MIp;
				if(MSp->Agression!=0x0B) continue;
				n=FindNearestHero(x,y,l,10);
				if(n!=-1){
					hp=GetHeroStr(n);
					switch(FindStep(Point(x,y,l),Point(hp->x,hp->y,hp->l))){
						case 1: AddWoMo2Do(x,y,l,1,0); break;
						case 2:
						case 3: AddWoMo2Do(x,y,l,2,n); break;
						default:;
					}
					continue;
				}
				n=FindNearestTown(x,y,l,10);
				if(n!=-1){
					cs=&(GetTownBase()[n]);
					switch(FindStep(Point(x,y,l),Point(cs->x,cs->y+1,cs->l))){
						case 1:
						case 2: AddWoMo2Do(x,y,l,1,0); break;
						case 3: AddWoMo2Do(x,y,l,3,0); break;
						default:;
					}
					continue;
				}
			}
		}
	}
}
*/
void CompleteWoMo(int player)
{
	STARTNA(__LINE__, 0)
	int i,x,y,l,new_x,new_y,new_l;
	int Mn,Mt,mn,ret;
	Dword PosMix;
	_MapItem_  *MIp,*MIp2;
	_CMonster_ *SMp;
	_Monster_  *MEp;
	_Hero_ *Hp;
	_WoMo_ *wmp;
	for(i=0;i<WOMONUM;i++){
		wmp=&WoMo[i];
		if(wmp->ToDo==WMO_NOTUSED) continue;
		if(wmp->Player!=player) continue;
		x=wmp->x; y=wmp->y;l=wmp->l;
		MIp=GetMapItem(x,y,l);
		if(MIp->OType!=54){ // убили монстра
			if(wmp->Flags.Call4Killed) CallWMKilled(i);
			ClearWM(i); continue;
		}
		new_x=wmp->new_x; new_y=wmp->new_y; new_l=wmp->new_l;
		switch(wmp->ToDo){
			case WMO_MOVE:
				ret=WoMoMove(i);
				if(ret==-2){ // внутренняя ошибка - удалим монстра совсем
					DelObject(x,y,l);
					ClearWM(i);
				}else if(ret==-1){ // позиция занята - нельзя сдвинуть
					 wmp->ToDo=WMO_STAND;
				}else{ // удачно сдвинули
					 wmp->x=(Byte)new_x; wmp->y=(Byte)new_y; wmp->l=(Byte)new_l;
					 wmp->ToDo=WMO_STAND;
				}
				break;
			case WMO_ATTACK:
				MIp2=GetMapItem(new_x,new_y,new_l);
				if(MIp2->OType==0x22){ // героя еще есть
					Hp=GetHeroStr(MIp2->SetUp);
					if(Hp->Owner==player){
						SMp=(_CMonster_ *)MIp;
						Mn=SMp->Number;
						if(SMp->HasSetUp){
							mn=SMp->SetUpInd;
							if(mn>GetMonsterNum()){ MError("AutoWoMo: cannot find monster by num (internal)."); continue; }
							MEp=GetMonsterBase(); if(MEp==0){ MError("AutoWoMo: cannot find monster base(internal)."); continue; }
							MEp=&MEp[mn];
						}else MEp=0;
						Mt=MIp->OSType;
						PosMix=PosMixed(x,y,l);
						if(wmp->Flags.Search4Hero){
							if(wmp->Flags.Call4Dest) CallWMDest(i);
						}
						if(DoBattle(&Mn,Hp,Mt,PosMix)==0){
							// убили монстра
							DelObject(x,y,l);
							ClearWM(i);
							if(MEp!=0){ // есть настройка
/*
struct _Monster_{
	long   Set;
	_AMes_  Mes;
	long   Res[7];
	Dword  Artefact;
};
*/
							}
						}else{
							// монстр остался жив
							SMp->Number=(Word)Mn;
							if(MEp!=0){ // есть настройка
								//...
							}
						}
					}
				}
				wmp->ToDo=WMO_STAND;
				break;
			default:;
		}
	}
	RETURNV
}

void DaylyWoMo(void)
{
	STARTNA(__LINE__, 0)
	WOG_CRC(124356) 
		if(__v!=124356) EXITERW(6756,edi,__v) // RETURNV
// инициализация всего что надо сбросить для новой игры
	AutoWoMo();
//  for(int i=0;i<8;i++) CompleteWoMo(i);
	RETURNV
}

static Dword RCM_Ecx;
void __stdcall RightClickAtMonster(_MapItem_ *mi,int sx,int sy)
{
	int   x,y,l,wm,act,stype;
	char *mes;
	_CMonster_ *ctrl;
	__asm{ mov RCM_Ecx,ecx}
	do{
		if(mi==0) break;
		if(mi->OType!=0x36) break; // не монстр
		Map2Coord(mi,&x,&y,&l);
		wm=FindWM(x,y,l); // может WM ?
		// проверим на количество
		ctrl=(_CMonster_ *)mi;
		stype=mi->OSType;
		if(ctrl->Number==4095){ // слишком много
			act=IsThis(CurrentUser());
			if(act){
				if(wm==-1){
					mes="Looks like this Stack is too big (=4095)\nDo you want to remove it from the map?";
					if(RequestPic(mes,21,stype,2)){
						DelObject(x,y,l);
						mi->OType=0; mi->SetUp=0xFFFFFFFF; mi->Attrib&=0xEF; mi->DrawNum=0xFFFF;
						CoorectMonsterRadius();
					}
				}else{
					mes="Looks like this Wandering Stack is too big (=4095)\nDo you want to remove it from the map?";
					if(RequestPic(mes,21,stype,2)){
						DelObject(x,y,l);
						ClearWM(wm);
						mi->OType=0; mi->SetUp=0xFFFFFFFF; mi->Attrib&=0xEF; mi->DrawNum=0xFFFF;
						CoorectMonsterRadius();
					}
				}
			}else{
				if(wm==-1){
					mes="Looks like this Stack is too big (=4095)\nYou can remove this stack at your turn.";
					RequestPic(mes,21,stype,4);
				}else{
					mes="Looks like this Wandering Stack is too big (=4095)\nYou can remove this stack at your turn.";
					RequestPic(mes,21,stype,4);
				}
			}
			return;
		}
	}while(0);
	__asm{
		mov   eax,sx
		mov   ecx,mi
		push  sy
		push  eax
		push  ecx
		mov   ecx,RCM_Ecx
		mov   eax,0x4170B0
		call  eax
	}
/*
	if(wm!=-1){
		Message("Wandering",1);
	}
*/
}

///////////////////
int ERM_WMon(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
	STARTNA(__LINE__,&Mp->m.s[Mp->i])
	int  v,x,y,l,t,wm,i;
	_WoMo_ *wmp;
	VarNum *vnp=&sp->Par[0];
	switch(Cmd){
		case 'P': // Px/y/l/subtype/?var  поставить монстра
			CHECK_ParamsMin(5);
			if(Apply(&x,4,Mp,0)) RETURN(0)
			if(Apply(&y,4,Mp,1)) RETURN(0)
			if(Apply(&l,4,Mp,2)) RETURN(0)
			if(Apply(&t,4,Mp,3)) RETURN(0)
			v=WoMoPlace(x,y,l,t)+1;
			Apply(&v,4,Mp,4);
			break;
		case 'C': // Cx/y/l/?num  если монстр бродячий, то вернуть его номер
			CHECK_ParamsMin(4);
			if(Apply(&x,4,Mp,0)) RETURN(0)
			if(Apply(&y,4,Mp,1)) RETURN(0)
			if(Apply(&l,4,Mp,2)) RETURN(0)
			v=0;
			for(i=0;i<WOMONUM;i++){
				wmp=&WoMo[i];
				if(wmp->ToDo==WMO_NOTUSED) continue;
				if(wmp->x!=x) continue;
				if(wmp->y!=y) continue;
				if(wmp->l!=l) continue;
				v=i+1;
				break;
			}
			Apply(&v,4,Mp,3);
			break;
		case 'M': // Mx/y/l/?var  преобразовать монстра в WM
			CHECK_ParamsMin(4);
			if(Apply(&x,4,Mp,0)) RETURN(0)
			if(Apply(&y,4,Mp,1)) RETURN(0)
			if(Apply(&l,4,Mp,2)) RETURN(0)
			v=MakeWoMo(x,y,l,1)+1;
			Apply(&v,4,Mp,3);
			break;
		case 'E': // E?exist проверка на существование
			wm=GetVarVal(vnp);
			if((wm<1)||(wm>WOMONUM)){ MError("\"!!MW:A\"-WM out of range (1...1000)."); RETURN(0) }
			wmp=&WoMo[wm-1];
			if(wmp->ToDo==WMO_NOTUSED) v=0; else v=1;
			Apply(&v,4,Mp,0);
			break;
		case 'A': // A#/par атрибуты
			CHECK_ParamsMin(2);
			wm=GetVarVal(vnp);
			if((wm<1)||(wm>WOMONUM)){ MError("\"!!MW:A\"-WM out of range (1...1000)."); RETURN(0) }
			wmp=&WoMo[wm-1];
			if(wmp->ToDo==WMO_NOTUSED){ MError("\"!!MW:A\"-WM is not exist."); RETURN(0) }
			t=0; Apply(&t,4,Mp,0);
			switch(t){
				case 1: // позиция x/y/l
					CHECK_ParamsMin(4);
					x=wmp->x; y=wmp->y; l=wmp->l;
					Apply(&x,4,Mp,1); Apply(&y,4,Mp,2); Apply(&l,4,Mp,3);
					if((x!=wmp->x)||(y!=wmp->y)||(l!=wmp->l)){
					wmp->new_x=(Byte)x; wmp->new_y=(Byte)y; wmp->new_l=(Byte)l;
						if(WoMoMove(wm-1)==-1){ // неудачно сдвинули
							wmp->ToDo=WMO_STAND;
						}else{ // удачно
							wmp->x=(Byte)x; wmp->y=(Byte)y; wmp->l=(Byte)l;
							wmp->ToDo=WMO_STAND;
						}
					}
//          if(Apply(&x,4,Mp,1)==0) wmp->x=(Byte)x;
//          if(Apply(&y,4,Mp,2)==0) wmp->y=(Byte)y;
//          if(Apply(&l,4,Mp,3)==0) wmp->l=(Byte)l;
					break;
//        case 2: // тип 1-обычный, 2-в опред.точку
//          v=wmp->ToDo; if(Apply(&v,4,Mp,1)==0) wmp->ToDo=(Byte)v;
//          break;
				case 2: // назначение x/y/l
					CHECK_ParamsMin(4);
					x=wmp->des_x; y=wmp->des_y; l=wmp->des_l;
					if(Apply(&x,4,Mp,1)==0) wmp->des_x=(Byte)x;
					if(Apply(&y,4,Mp,2)==0) wmp->des_y=(Byte)y;
					if(Apply(&l,4,Mp,3)==0) wmp->des_l=(Byte)l;
					break;
				case 3: // флаги
					CHECK_ParamsMin(6);
					v=wmp->Flags.Call4Dest;   if(Apply(&v,4,Mp,1)==0) wmp->Flags.Call4Dest=(Word)v;
					v=wmp->Flags.Call4Killed; if(Apply(&v,4,Mp,2)==0) wmp->Flags.Call4Killed=(Word)v;
					v=wmp->Flags.GoToDest;    if(Apply(&v,4,Mp,3)==0) wmp->Flags.GoToDest=(Word)v;
					v=wmp->Flags.Search4Hero; if(Apply(&v,4,Mp,4)==0) wmp->Flags.Search4Hero=(Word)v;
					v=wmp->Flags.StandAfter;  if(Apply(&v,4,Mp,5)==0) wmp->Flags.StandAfter=(Word)v;
					if(Num>6){ v=wmp->Flags.Search4Owner; if(Apply(&v,4,Mp,6)==0) wmp->Flags.Search4Owner=(Word)v; }
					if(Num>7){ v=wmp->Flags.Where; if(Apply(&v,4,Mp,7)==0) wmp->Flags.Where=v; }
					break;
				case 4: // радиус
					v=wmp->Radius; if(Apply(&v,4,Mp,1)==0) wmp->Radius=(Byte)v;
					break;
				default:
					MError("\"!!MW:A\"-Anknown command type."); RETURN(0)
			}
			break;
		default:
			EWrongCommand();
			RETURN(0)
	}
	RETURN(1)
}
///////////////////
int SaveWM(void)
{
	STARTNA(__LINE__, 0)
	if(Saver("LWMN",4)) RETURN(1)
	SOD_CRC(6478) 
		if(__v!=(SODCRC+6478)) EXITERS(5436,ecx,__v) // RETURNV
	if(Saver(WoMo,sizeof(WoMo))) RETURN(1)
	RETURN(0)
}

void ResetWM(void)
{
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<WOMONUM;i++) ClearWM(i);
	RETURNV
}

int LoadWM(int /*ver*/)
{
	STARTNA(__LINE__, 0)
	char buf[4]; if(Loader(buf,4)) RETURN(1)
	if(buf[0]!='L'||buf[1]!='W'||buf[2]!='M'||buf[3]!='N')
			{MError("LoadWM cannot start loading"); RETURN(1) }
	if(Loader(WoMo,sizeof(WoMo))) RETURN(1)
	RETURN(0)
}


