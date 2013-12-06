#ifndef __LOD__
#define __LOD__

#include "structs.h"
//void *h3new(size_t sz);

class Lod{
//    Byte LodInternal[0x190];
		int Ind;

		int LoadIt(char *name);
		void ReloadItems();
	public:
		int Kind;
		int Location;
		char Name[32];
		Lod *Next;
		static Lod *First;

		Lod(int location, char *name, int kind);
		~Lod();
		static int  LoadCustomLOD(int location, char *name, int kind);
		static void UnloadCustomLOD(int ind);

		static const int STORED = 0;
		static const int TEMP = 1;
		static const int PERM = 2;
};

#define LODNUM 1000
extern Byte LodTable[LODNUM][0x190];

class LodTypes{
	public :
		static struct _Table{ int Num,*Inds; } Table[4][3];
		static int Seq[4][3][LODNUM];
		static int Add2List(int ind);
		static int Del4List(int ind);
		static void ReloadItem(char *name);
		static int __inline Load(int ind, char *filename)
		{
			return (((int (__fastcall *)(Byte*,int,char *,int))0x4FAF30)(&LodTable[ind][4],0,filename,1/*read only*/));
		}
};

int ERM_LODs(char Cmd,int Num,_ToDo_*sp,Mes *Mp);
int LoadLODs(int /*ver*/);
int SaveLODs(void);
int ResetLODs(void);
int LoadCustomLOD(int location,char *name);
void LoadNewSnd(); // Diakon

#endif