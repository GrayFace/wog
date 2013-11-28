#include "structs.h"

///////////////////////////
class Dlg8Item{
	int    PicType;
	int    PicSubType;
	_FMes_ PicFileName;
	_FMes_ Mes;
	int    DefPicInd;
	int    field_2C;
	int    field_30;
	int    field_34;
	int    field_38;
	int    field_3C;
	int    field_40;
	int    field_44;
	int    field_48;
};

class Dlg8{
	_FMes_   Mes;
	int      X;
	int      Y;
	int      DX;
	int      DY;
	int      BdrDX;
	int      BdrSY;
	int      field_28;
	int      field_2C;
	int      field_30;
	Dlg8Item Items[8];
	int      ActionType;
	int      Param;
	int      Time2Show;
};
//////////////////////////////
typedef Byte DlgItem;

class HDlg{
public:
		struct _Body_{
			Byte Obj[0x60];
			struct _ADef_{
				int Cadre;
				DlgItem *Item;
			} ADef[10];
			int VInd,VX,VY;
			int HintItemId;
			struct _Hint_{
				int   ItemId; 
				char *Text;
			} *Hints;
			int NumOfItems;
			int Id;
		} *Body;
	static Dword VTable[15];
	HDlg *Next;
//  TxtFile *TF;
	static HDlg *Last; 
	static Dword __fastcall iActivate(_Body_ *body,int,int par1,int par2);
	static Dword __fastcall iDisactivate(_Body_ *body,int,int par1);
	static Dword __fastcall iFunction1(_Body_ *body,int,int par1);
	static Dword __fastcall iFunction2(_Body_ *,int,_MouseStr_ *);

	static int __inline IsDynamicText(const char * text)
	{
		return ((Dword)text < (Dword)&ERMString[0] || (Dword)text >= (Dword)&ERMString[0] + sizeof(ERMString))
			&& (!StringSet::Belongs(text));
	}
	static void __inline DeleteText(char * &text)
	{
		if (IsDynamicText(text))
			free((void*)text);
		text = 0;
	}
//  static int   VideoX;
//  static int   VideoY;
//  static int   VideoInd;
	HDlg(HDlg&){}
	void operator=(HDlg&){}
	public:
//  Byte *Body;
//  int   backVideoX;
//  int   backVideoY;
//  int   backVideoInd;
		int   Flags;

		static HDlg *Find(int ID);
		HDlg(int /*tp*/){ 
//      Body=(Byte *)h3new(0x70+3*4); 
			Body=(_Body_ *)h3new(sizeof(_Body_));
			Body->Hints=NULL;
			if(Body==NULL){ Flags=0; return; }// not initialized
			if(Last==NULL){ // first
				this->Next=NULL;
			}else{ // not first
				this->Next=Last;
			}
			Last=this; 
			Body->Id=0;
			Flags=1; // built up
//      TF=NULL;
		}
		~HDlg(){
			HDlg *cur=Last,*prev=NULL;
			while(cur!=NULL){
				if(cur==this){
					if(prev){
						prev->Next=Next;
					}else{
						Last=Next;
					}
					break;
				}
				prev=cur;
				cur=cur->Next;
			}
//      if(TF){ delete TF; TF=NULL; }
			if(Body->Hints)
			{
				for(int i = 0; i < Body->NumOfItems; i++)
					DeleteText(Body->Hints[i].Text);
				delete Body->Hints;
				Body->Hints=NULL;
			}
			Kill(1);
		}

		_Body_ *Initialize(int x,int y,int dx,int dy,int numofitems);
		void  AdjustPlayerColor(DlgItem *,int pl);
//    Byte *Demolish(int FreeMem);
		DlgItem *BuildPcx  (int x,int y,int dx,int dy,int itemind,char *picfile,int flags);
		int      BuildDefaultBackPcx(int dx,int dy,int withhint);
		DlgItem *BuildDef  (int x,int y,int dx,int dy,int itemind,char *picfile,int defpicind,int p2,int p3,int p4,int flags);
		int      SetADef(DlgItem *);
		// butons ???
		// 0x7800 - default Cansel code
		// 0x7801 - default No/Cansel code
		// 0x7802 - default Ok code
		// 0x7805
		DlgItem *BuildDef2 (int x,int y,int dx,int dy,int itemind,char *picfile,int p1,int p2,int p3,int p4,int flags);
		DlgItem *BuildTxt  (int x,int y,int dx,int dy,char *text,char *font,int color,int itemind,int align,int bkcolor,int flags);
//    DlgItem *BuildTxt  (int x,int y,int dx,int dy,char *text,char *font,int color,int align,int bkcolor,int flags);
		DlgItem *BuildTxtSB(int x,int y,int dx,int dy,char *text,char *font,int txtcolor,int sbcolor);
		DlgItem *BuildTxtED(int x,int y,int dx,int dy,int p1,char *text,char *font,int p2,int p3,int p4,int p5,int itemind,int p6,int p7,int p8,int p9);
		DlgItem *BuildScrollBar(int x,int y,int dx,int dy,int item,int flags);
		// подгрузка видео в VTable+4 0x5D823C
		// остановка видео в VTable+8 0x5D8276
		// item index may be -1
		// BuildEditedNumber 44983B
		// BuildScrollBar 4498FD
		DlgItem *AddItem(DlgItem *itempo);
		void LoadAllItems(void);
		void SetVideo(int ind,int x,int y);

		DlgItem *FindItemByID(int id);
		void EnableItem(DlgItem *itempo,int ENdis);
		void SendCmd2Item(int cmd,int item,Dword par);
		void SendCmd2AllItems(int cmd,int item,Dword par);

		void Refresh(int item);
		void Show(int par);
		void Kill(int freemem);

		int LoadFromText(int id, char *textfile);
		int AddHint(int id, char *text);
		static char *GetHint(_Body_ *,int id);
};

extern Dword ShowMapSpellDlg_VTable[15];

int ERM_Dlg(char Cmd,int Num,_ToDo_* /*sp*/,Mes *Mp);

void ShowMapSpellDlg(int dlgnum);
