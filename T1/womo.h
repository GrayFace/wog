#define MAXMAPSIZE 144
//extern short Way[MAXMAPSIZE+2][MAXMAPSIZE+2];
//extern short Stp[MAXMAPSIZE+2][MAXMAPSIZE+2];

class Point{
 public:
	short x,y,l;
	static short LineHasPoint;
	static Point Line[144];
	Point(){x=0;y=0;l=0;}
	Point(int xi,int yi,int li){x=(short)xi; y=(short)yi; l=(short)li;}
	Point(Point &p){x=p.x;y=p.y;l=p.l;}
	bool operator==(Point &p){if((x!=p.x)||(y!=p.y)||(l!=p.l)) return 0; else return 1; }
	void MakeLine(Point &p);
};
int FindWay(Point &s,Point &e,int where);
Dword MakeWay(Point &s,Point &e,int Steps);
/////////
void __stdcall RightClickAtMonster(_MapItem_ *mi,int sx,int sy);
/////////
int WoMoMove(int x0,int y0,int l0,int x1,int y1,int l1);
int WoMoPlace(int x,int y,int l,int subtype);
void DaylyWoMo(void);
int MakeWoMo(int x,int y,int l,int YESno);
int MakeWoMoPos(_MapItem_  *MIp,int YESno);
void CompleteWoMo(int player);
////////////
int ERM_WMon(char Cmd,int Num,_ToDo_*sp,Mes *Mp);
int FindStep(int x,int y,int l,int x1,int y1,int l1,Dword *MixPos,int Steps,int where);
////////////
int SaveWM(void);
void ResetWM(void);
int LoadWM(int ver);



