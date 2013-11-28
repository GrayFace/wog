//#include "classes.h"
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include "txtfile.h"
#include "service.h"
#include "common.h"
#include "erm.h"
#include "erm_lua.h"
#include "b1.h"
//#include "b1.h"
#include "wogsetup.h"
#include "prot.h"
#define __FILENUM__ 13

static TxtFile    WoGTexts;
static TxtFile    WoGCommands;
static char       ScriptCommandLoaded[1100];
static TxtFile    ScriptCommand[1100];
static char       WoGFilePath[MAX_PATH];
static char       WoGFileName[MAX_PATH];
int EnableWoGDlgEdit;
_DlgSetup DlgSetup;
DlgSetup_Page DlgPages[8];
DlgSetup_ItemList DlgItems[8][4];
int   DlgButState[10]={1,1,1,-1/*MP*/,1,1,-1,-1,1,-1};
															// #0 - кнопка Ok,  (-1 не действует)
															// #1 - Save,
															// #2 - Restore,
															// #3 - Multiplayer,
															// #4 - UnSelectAll,
															// #5 - SelectAll,
															// #6 - Next,
															// #7 - Back,
															// #8 - Load,
															// #9 - Restore Multiplayer
int   DlgButState2[10]={1,1,-1,-1,-1,-1,-1,-1,-1,-1};
int   _PDisabled[8][4][20];
int   PStates[8][4][20];
int   DStates[8][4][20];
int   MStates[8][4][20];
char *PNames[8][4][20];
char *PHints[8][4][20];
char *PPopUps[8][4][20];

//int PL_WoGOptions[200]; // все опции
int   InternalVars[8][4][20];

void Prepare2Show(void);
void Prepare2Close(void);

static DlgSetup_ItemList *GetList(DlgSetup_Page *Page,int Ind)
{
	STARTNA(__LINE__, 0)
	switch(Ind){
		case 0: RETURN(Page->ItemListLT)
		case 1: RETURN(Page->ItemListLB)
		case 2: RETURN(Page->ItemListRT)
		case 3: RETURN(Page->ItemListRB)
		default:
			TError("Setup dialog: Incorrect ItemList index.");
			RETURN(Page->ItemListLT)
	}
}
// !!!!!!!!! Зависимость кнопок
void CheckDepend(int Flag)
{
	STARTNA(__LINE__, 0)
// 3.58 Dependance
	if(PStates[0][2][2]==0) PStates[0][2][3]=2; else PStates[0][2][3]&=1;
//  if(PStates[0][3][2]==0) PStates[0][3][3]=2; else PStates[0][3][3]&=1;
	if(PStates[0][2][4]==0){ 
		PStates[0][2][5]=2; 
		PStates[2][0][3]=2; 
		PStates[2][0][4]=2; 
		PStates[3][2][0]=2; 
		PStates[3][2][1]=2; 
		PStates[3][3][9]=2; 
	}else{ 
		PStates[0][2][5]&=1; 
		PStates[2][0][3]&=1; 
		PStates[2][0][4]&=1; 
		PStates[3][2][0]&=1; 
		PStates[3][2][1]&=1; 
		PStates[3][3][9]&=1; 
	}
//  if(PStates[0][3][4]==0) PStates[0][3][5]=2; else PStates[0][3][5]&=1;
	if(PStates[0][2][10]==0){
		PStates[0][3][0]=3; // set as default
		PStates[0][3][1]=2;
		PStates[0][3][2]=2;
		PStates[0][3][3]=2;
		PStates[0][3][4]=2;
		PStates[0][3][5]=2;
		PStates[0][3][6]=2;
		PStates[0][3][7]=2;
	}else{
		PStates[0][3][0]&=1;
		PStates[0][3][1]&=1;
		PStates[0][3][2]&=1;
		PStates[0][3][3]&=1;
		PStates[0][3][4]&=1;
		PStates[0][3][5]&=1;
		PStates[0][3][6]&=1;
		PStates[0][3][7]&=1;
	}
	if(PStates[1][0][0]==0) PStates[1][0][1]=2; else PStates[1][0][1]&=1;
	if(PStates[1][1][0]==0){
		PStates[1][1][1]=2;
		PStates[1][1][2]=2;
		PStates[1][1][3]=2;
		PStates[1][1][4]=2;
	}else{
		PStates[1][1][1]&=1;
		PStates[1][1][2]&=1;
		PStates[1][1][3]&=1;
		PStates[1][1][4]&=1;
		if(Flag==2000+100+3){
			if(PStates[1][1][3]==1) PStates[1][1][2]=0;
			if(PStates[1][1][2]==1) PStates[1][1][3]=0;
		}else{
			if(PStates[1][1][2]==1) PStates[1][1][3]=0;
			if(PStates[1][1][3]==1) PStates[1][1][2]=0;
		}
	}
	if(Flag==2000+100+5){ // Mithril Enhancements
		if(PStates[1][1][5]==1) PStates[1][1][6]=1;
//    if(PStates[1][1][5]==0) PStates[1][1][6]=0;
	}
	if(Flag==2000+100+6){ // Mithril Display
//    if(PStates[1][1][6]==1) PStates[1][1][5]=1;
		if(PStates[1][1][6]==0) PStates[1][1][5]=0;
	}
	if(Flag==3000+0+6){ // War Machines I
		if(PStates[2][0][6]==1){ PStates[2][0][7]=0; PStates[2][0][8]=0;}
		if(PStates[2][0][6]==0){ PStates[2][0][7]&=1; PStates[2][0][8]&=1;}
	}
	if(Flag==3000+0+7){ // War Machines II
		if(PStates[2][0][7]==1){ PStates[2][0][6]=0; PStates[2][0][8]=0;}
		if(PStates[2][0][7]==0){ PStates[2][0][6]&=1; PStates[2][0][8]&=1;}
	}
	if(Flag==3000+0+8){ // War Machines III
		if(PStates[2][0][8]==1){ PStates[2][0][6]=0; PStates[2][0][7]=0;}
		if(PStates[2][0][8]==0){ PStates[2][0][6]&=1; PStates[2][0][7]&=1;}
	}
	if(PStates[2][0][6]==1){ PStates[2][0][7]=0; PStates[2][0][8]=0;}
	if(PStates[2][0][7]==1){ PStates[2][0][6]=0; PStates[2][0][8]=0;}
	if(PStates[2][0][8]==1){ PStates[2][0][6]=0; PStates[2][0][7]=0;}
/* 3.57
	if(PStates[0][1][1]==0){
		PStates[0][1][2]=2;
		PStates[0][1][3]=2;
		PStates[0][1][4]=2;
	}else{
		PStates[0][1][2]&=1;
		PStates[0][1][3]&=1;
		PStates[0][1][4]&=1;
		if(Flag==4){
			if(PStates[0][1][4]==1) PStates[0][1][3]=0;
			if(PStates[0][1][3]==1) PStates[0][1][4]=0;
		}else{
			if(PStates[0][1][3]==1) PStates[0][1][4]=0;
			if(PStates[0][1][4]==1) PStates[0][1][3]=0;
		}
	}
	if(PStates[0][2][4]==0) PStates[0][2][5]=2;
	else PStates[0][2][5]&=1;
	if(PStates[0][2][6]==0) PStates[0][2][7]=2;
	else PStates[0][2][7]&=1;
*/
	RETURNV;
}

static int LastChoosenPage=-1;
static char CFPath[MAX_PATH];
static _ChooseFile ChooseFile={
	"128,129",
	"130,131",
	WoGFilePath,
	"132",
	CFPath,MAX_PATH-1,0x3F,1,1,0
};
int MyListener(int Type,int Page, int ItemList,int Item)
{
	STARTNA(__LINE__, 0)
	int i,j,k,l;
	DlgSetup.Flags&=~0x1;
	WOG_CRC(75474) 
		if(__v!=75474) EXITERW(9735245,eax,__v) // RETURNV
// инициализация всего что надо сбросить для новой игры
	if(Type==2){ LastChoosenPage=Page-1; }
	if(Type==0){
		if(Item==0){ // Ok
			RETURN(2)
		}
		if(Item==2){ // Default
			for(i=0;i<8;i++){
				for(j=0;j<4;j++){
					for(k=0;k<20;k++){
						if(_PDisabled[i][j][k]) PStates[i][j][k]=2; // 3.58
						else PStates[i][j][k]=DStates[i][j][k];
					}
				}
			}
			CheckDepend(-1);
			DlgSetup.ButtonsStates[1]=1; // Save
			DlgSetup.ButtonsStates[8]=1; // Restore
			RETURN(1)
		}
		if((Item==4)||(Item==5)){ // Sel, Unsel
			i=LastChoosenPage;
			if(Item==4) l=1; else l=0;
			if((i<0)||(i>7)) RETURN(0)
			for(j=0;j<4;j++){
				if(DlgItems[i][j].Type!=1) continue;
				for(k=0;k<DlgItems[i][j].ItemCount;k++){
					if(_PDisabled[i][j][k]) PStates[i][j][k]=2; // 3.58
					else PStates[i][j][k]=l;
				}
			}
			CheckDepend(-1);
			DlgSetup.ButtonsStates[1]=1; // Save
			DlgSetup.ButtonsStates[8]=1; // Restore
			RETURN(1)
		}
		if(Item==1){ // save
			Prepare2Close();
			ChooseFile.FileEditEnabled=1;
			ChooseFile.Caption=ITxt(128,1,&WoGTexts);
			ChooseFile.Description=ITxt(130,1,&WoGTexts);
			ChooseFile.Mask=ITxt(132,1,&WoGTexts);
			StrCopy(CFPath,MAX_PATH-1,WoGFileName);
			if(ChooseFileDlg(&ChooseFile,"WoGSetupEx.dat")) RETURN(0)
			// записываем лишь первый набор опций
			if(SaveSetupState(ChooseFile.Buf,PL_WoGOptions,sizeof(PL_WoGOptions)/2)){
//      if(SaveSetupState("WOGSETUP.DAT",PL_WoGOptions,sizeof(PL_WoGOptions))){
//        Message(ITxt(125,1,&WoGTexts),1);
//        CommonDialog(ITxt(125,1,&WoGTexts));
				WoGMessage(ITxt(125,1,&WoGTexts),"\n",ChooseFile.Buf);
				RETURN(0)
			}
//3.58      DlgSetup.ButtonsStates[1]=0; // Save
//3.58      DlgSetup.ButtonsStates[8]=0; // Restore
//      StrCopy(WoGFilePath,255,".\\");
//      StrCopy(WoGFileName,1023,ChooseFile.Buf);
//asm int 3
			SplitPath(ChooseFile.Buf,WoGFilePath,WoGFileName);
			WriteStrINI(WoGFilePath,"Options_File_Path","WoGification");
			WriteStrINI(WoGFileName,"Options_File_Name","WoGification");
			RETURN(1)
		}
		if(Item==8){ // load
			ChooseFile.FileEditEnabled=0;
			ChooseFile.Caption=ITxt(129,1,&WoGTexts);
			ChooseFile.Description=ITxt(131,1,&WoGTexts);
			ChooseFile.Mask=ITxt(132,1,&WoGTexts);
//      CFPath[0]=0;
//      MakePath(CFPath,WoGFilePath,WoGFileName);
			StrCopy(CFPath,MAX_PATH-1,WoGFileName);
			if(ChooseFileDlg(&ChooseFile,"WoGSetupEx.dat")) RETURN(0)
//      if(SaveSetupState(ChooseFile.Buf,PL_WoGOptions,sizeof(PL_WoGOptions))){
			// загружаем первую половину опций и копируем во второй набор
			k=LoadSetupState(ChooseFile.Buf,PL_WoGOptions,sizeof(PL_WoGOptions)/2);
			for(i=0;i<PL_WONUM;i++) PL_WoGOptions[1][i]=PL_WoGOptions[0][i];
//      k=LoadSetupState("WOGSETUP.DAT",PL_WoGOptions,sizeof(PL_WoGOptions));
//3.58      DlgSetup.ButtonsStates[8]=0; // Restore
			if(k<=0){ // к равно длине прочитанных файлов
//        Message(ITxt(126,1,&WoGTexts),1);
//        CommonDialog(ITxt(126,1,&WoGTexts));
				WoGMessage(ITxt(126,1,&WoGTexts),"\n",ChooseFile.Buf);
				RETURN(0)
			}
//      StrCopy(WoGFilePath,255,".\\");
//      StrCopy(WoGFileName,1023,ChooseFile.Buf);
			SplitPath(ChooseFile.Buf,WoGFilePath,WoGFileName);
			WriteStrINI(WoGFilePath,"Options_File_Path","WoGification");
			WriteStrINI(WoGFileName,"Options_File_Name","WoGification");
			Prepare2Show();
//3.58      DlgSetup.ButtonsStates[1]=0; // Save
			if(k < sizeof(PL_WoGOptions)/2){
//        Message(ITxt(127,1,&WoGTexts),1);
				RETURN(1)
			}
			RETURN(1)
		}
		if(Item==3){ // Multiplayer
			for(i=0;i<8;i++){
				for(j=0;j<4;j++){
					for(k=0;k<20;k++){
						PStates[i][j][k]=MStates[i][j][k];
					}
				}
			}
			CheckDepend(-1);
			DlgSetup.ButtonsStates[1]=1; // Save
			DlgSetup.ButtonsStates[8]=1; // Restore
			RETURN(1)
		}
	}
	if(Type==1){
		// !!!!!!!!!!!!!!!!!!!
		// 3.58 dependance
		if(
			 ((Page==1)&&(ItemList==2)&&(Item==2))|| // Left monster
//       ((Page==1)&&(ItemList==3)&&(Item==2))|| // Left monster
			 ((Page==1)&&(ItemList==2)&&(Item==4))|| // Hire Commander
//       ((Page==1)&&(ItemList==3)&&(Item==4))|| // Hire Commander
			 ((Page==1)&&(ItemList==2)&&(Item==10))|| // Experience Enabling
			 ((Page==2)&&(ItemList==0)&&(Item==0))|| // No Fort
			 ((Page==2)&&(ItemList==1)&&((Item==0)||(Item==2)||(Item==3)))|| // WM and week of M
			 ((Page==2)&&(ItemList==1)&&((Item==5)||(Item==6)))|| // Mithril Enchnsment or MithrilDisplay
			 ((Page==3)&&(ItemList==0)&&((Item==6)||(Item==7)||(Item==8))) // War Machines I/II/III
			){
			CheckDepend(Page*1000+ItemList*100+Item);
			DlgSetup.ButtonsStates[8]=1; // Restore
			DlgSetup.ButtonsStates[1]=1; // Save
			RETURN(1)
		}
/* 3.57
		if(((Page==1)&&(ItemList==1)&&((Item==1)||(Item==3)||(Item==4)))||
			 ((Page==1)&&(ItemList==2)&&(Item==4))||
			 ((Page==1)&&(ItemList==2)&&(Item==6))){
			CheckDepend(Item);
			DlgSetup.ButtonsStates[8]=1; // Restore
			DlgSetup.ButtonsStates[1]=1; // Save
			RETURN(1)
		}
*/
		if((DlgSetup.ButtonsStates[8]!=1)||(DlgSetup.ButtonsStates[1]!=1)){
			DlgSetup.ButtonsStates[8]=1; // Restore
			DlgSetup.ButtonsStates[1]=1; // Save
			RETURN(1)
		}
	}
	RETURN(0)
}

static void BuildAll(void)
{
	STARTNA(__LINE__, 0)
	for(int i=0;i<8;i++){
		DlgSetup.Pages[i]=&DlgPages[i];
		DlgPages[i].ItemListLT=&DlgItems[i][0];
		DlgPages[i].ItemListLB=&DlgItems[i][1];
		DlgPages[i].ItemListRT=&DlgItems[i][2];
		DlgPages[i].ItemListRB=&DlgItems[i][3];
		DlgPages[i].ItemListLT->ItemState=&PStates[i][0][0];
		DlgPages[i].ItemListLB->ItemState=&PStates[i][1][0];
		DlgPages[i].ItemListRT->ItemState=&PStates[i][2][0];
		DlgPages[i].ItemListRB->ItemState=&PStates[i][3][0];
		DlgPages[i].ItemListLT->ItemName=&PNames[i][0][0];
		DlgPages[i].ItemListLB->ItemName=&PNames[i][1][0];
		DlgPages[i].ItemListRT->ItemName=&PNames[i][2][0];
		DlgPages[i].ItemListRB->ItemName=&PNames[i][3][0];
		DlgPages[i].ItemListLT->ItemHint=&PHints[i][0][0];
		DlgPages[i].ItemListLB->ItemHint=&PHints[i][1][0];
		DlgPages[i].ItemListRT->ItemHint=&PHints[i][2][0];
		DlgPages[i].ItemListRB->ItemHint=&PHints[i][3][0];
		DlgPages[i].ItemListLT->ItemPopUp=&PPopUps[i][0][0];
		DlgPages[i].ItemListLB->ItemPopUp=&PPopUps[i][1][0];
		DlgPages[i].ItemListRT->ItemPopUp=&PPopUps[i][2][0];
		DlgPages[i].ItemListRB->ItemPopUp=&PPopUps[i][3][0];
	}
	DlgSetup.ButtonsStates=DlgButState;
	DlgSetup.Listener=MyListener;
	DlgSetup.NextButtonHint="DlgSetup.NextButtonHint";
	DlgSetup.BackButtonHint="DlgSetup.BackButtonHint";
	RETURNV;
}

int AddItem(int Script,int Page,int Group,int Item,int Default,int Multip,int Internal, char *Text,char *Hint,char *PopUp)
{
	STARTNA(__LINE__, 0)
	int   st;
	DlgSetup_ItemList *po;
	char  TxtName[1024];

	if(Text[0]==0) RETURN(0) // проверка на пустой
	// check for Script
	st=0;
	// !!!
	//if(Script!=-1){
	//	if((Script<0)||(Script>=SCRIPTSNUM)) RETURN(0)
	//	char *name=GetScriptName(Script+2);
	//	sprintf(TxtName,"%s%s.erm","..\\DATA\\S\\",name);
	//	if(Service_GetFileAttributes(&TxtName[3])==-1){ // return 0; // нет такого файла
	//		st=1;
	//	}
	//}
	// check for page, group and item
	if((Page<0)||(Page>7)) RETURN(0)
	if((Group<0)||(Group>3)) RETURN(0)
	po=GetList(DlgSetup.Pages[Page],Group);
	if(Item==-1){ Item=po->ItemCount; po->ItemCount++; }
	if(Item>=po->ItemCount) po->ItemCount=Item+1;
	if(Item>=20) RETURN(0)
	if(st){ po->ItemState[Item]=2; _PDisabled[Page][Group][Item]=1; }
	else{ po->ItemState[Item]=Default; _PDisabled[Page][Group][Item]=0; }
	po->ItemName[Item]=Text;
	po->ItemHint[Item]=Hint;
	po->ItemPopUp[Item]=PopUp;
	InternalVars[Page][Group][Item]=Internal;
	DStates[Page][Group][Item]=Default;
	MStates[Page][Group][Item]=Multip;
	RETURN(1)
}

// инициализация - заполнение структуры
void InitWoGSetup(void)
{
	STARTNA(__LINE__, 0)
	int i,j,k;
	DlgSetup_Page *p;
	DlgSetup_ItemList *l;

	BuildAll();
	for(i=0;i<8;i++){
		for(j=0;j<4;j++){
			DlgItems[i][j].ItemCount=0;
			for(k=0;k<20;k++){
				InternalVars[i][j][k]=-1;
				MStates[i][j][k]=0;
				_PDisabled[i][j][k]=0;
			}
		}
	}
	DlgSetup.Flags|=0x1;
//  for(i=0;i<10;i++) DlgSetup.ButtonsStates[i]=0;
	DlgSetup.ButtonsStates[0]=1; // Ok
	DlgSetup.ButtonsStates[1]=1; // Save 3.58
	DlgSetup.ButtonsStates[8]=1; // Restore 3.58
//  DlgSetup.ButtonsStates[3]=1; // Multiplayer
	for(i=0;i<8;i++){
		p=DlgSetup.Pages[i];
		p->Enabled=1;
		for(j=0;j<4;j++){
			l=GetList(DlgSetup.Pages[i],j);
			l->Type=1;
		}
	}
	for(i=2;i<WoGCommands.sn;i++){
		if(AddItem(
			 a2i(ITxt(i,1,&WoGCommands)), //int Script,
			 a2i(ITxt(i,2,&WoGCommands)), //int Page,
			 a2i(ITxt(i,3,&WoGCommands)), //int Group,
			 a2i(ITxt(i,4,&WoGCommands)), //int Item,
			 a2i(ITxt(i,5,&WoGCommands)), //int Default,
			 a2i(ITxt(i,6,&WoGCommands)), //int Multiplayer,
			 a2i(ITxt(i,7,&WoGCommands)), //int Internal,
			 ITxt(i,8,&WoGCommands), //char *Text,
			 ITxt(i,9,&WoGCommands), //char *Hint,
			 ITxt(i,10,&WoGCommands) //char *PopUp
		)==0) continue;
	}
	for(i=0;i<1100;i++){
		if(ScriptCommandLoaded[i]==0) continue;
		for(j=0;j<ScriptCommand[i].sn;j++){
			if(AddItem(
				a2i(ITxt(j,1,&ScriptCommand[i])), //int Script,
				a2i(ITxt(j,2,&ScriptCommand[i])), //int Page,
				a2i(ITxt(j,3,&ScriptCommand[i])), //int Group,
				a2i(ITxt(j,4,&ScriptCommand[i])), //int Item,
				a2i(ITxt(j,5,&ScriptCommand[i])), //int Default,
				a2i(ITxt(j,6,&ScriptCommand[i])), //int Multiplayer,
				a2i(ITxt(j,7,&ScriptCommand[i])), //int Internal,
				ITxt(j,8,&ScriptCommand[i]), //char *Text,
				ITxt(j,9,&ScriptCommand[i]), //char *Hint,
				ITxt(j,10,&ScriptCommand[i]) //char *PopUp
			)==0) continue;
		}
	}
	for(i=0;i<8;i++){
		k=0;
		for(j=0;j<4;j++){
			if(DlgItems[i][j].ItemCount==0) DlgItems[i][j].Type=0;
			else{
				DlgItems[i][j].Type=1; k=1;
			}
		}
		if(k==0){
			DlgSetup.Pages[i]->Enabled=0;
		}
		// !!!!!!!!! Радио Бутоны
		// 3.58 Dependance
		DlgItems[0][0].Type=2;
		DlgItems[0][3].Type=2;
		DlgItems[4][0].Type=2;

/* 3.57
		DlgItems[0][0].Type=2;
		DlgItems[1][0].Type=2;
*/
	}
	MakePath(CFPath,WoGFilePath,WoGFileName);
	// загружаем первую половину опций и копируем во второй набор (потом)
	k=LoadSetupState(CFPath,PL_WoGOptions,sizeof(PL_WoGOptions)/2);
	if(k<=0){ // не открыть - откр дефаултный
		StrCopy(WoGFilePath,MAX_PATH-1,".\\");
		StrCopy(WoGFileName,MAX_PATH-1,"WoGSetupEx.dat");
		WriteStrINI(WoGFilePath,"Options_File_Path","WoGification");
		WriteStrINI(WoGFileName,"Options_File_Name","WoGification");
		// загружаем первую половину опций и копируем во второй набор (потом)
		k=LoadSetupState("WoGSetupEx.dat",PL_WoGOptions,sizeof(PL_WoGOptions)/2);
	}
//  for(i=0;i<PL_WONUM;i++) PL_WoGOptions[1][i]=PL_WoGOptions[0][i];
	DlgSetup.ButtonsStates[8]=1; // Restore 3.58
	if(k>0){
		Prepare2Show();
		DlgSetup.ButtonsStates[1]=1; // Save 3.58
	}
	Prepare2Close();
	RETURNV;
}
void ProcessAll(void){
	STARTNA(__LINE__, 0)
	int i,j;
	DlgSetup_Page *p;
	DlgSetup_ItemList *l;

	BuildAll();
	DlgSetup.Name=ITxt(1,1,&WoGTexts);
	DlgSetup.Hint=ITxt(2,1,&WoGTexts);
	DlgSetup.PopUp=ITxt(3,1,&WoGTexts);
	DlgSetup.Intro=ITxt(4,1,&WoGTexts);
	for(i=0;i<8;i++){
		p=DlgSetup.Pages[i];
		p->Name=ITxt(5+i*3,1,&WoGTexts);
		p->Hint=ITxt(6+i*3,1,&WoGTexts);
		p->PopUp=ITxt(7+i*3,1,&WoGTexts);
		for(j=0;j<4;j++){
			l=GetList(DlgSetup.Pages[i],j);
			l->Name=ITxt(29+i*12+j*3,1,&WoGTexts);
			l->Hint=ITxt(30+i*12+j*3,1,&WoGTexts);
			l->PopUp=ITxt(31+i*12+j*3,1,&WoGTexts);
		}
	}
	RETURNV;
}

void Prepare2Show(void)
{
	STARTNA(__LINE__, 0)
	int i,j,k,ind;

	for(i=0;i<8;i++){
		for(j=0;j<4;j++){
			switch(DlgItems[i][j].Type){
				case 0: break; // нет группы
				case 1:
					for(k=0;k<20;k++){
						if(InternalVars[i][j][k]==-1) break;
						ind=InternalVars[i][j][k];
						if((ind<0)||(ind>=PL_WONUM)) break;
						if(_PDisabled[i][j][k]) PStates[i][j][k]=2;
						else{
							if((ind>0)&&(ind<5)) PStates[i][j][k]=!PL_WoGOptions[0][ind];
							else PStates[i][j][k]=PL_WoGOptions[0][ind];
						}
					}
					break;
				case 2:
					for(k=0;k<20;k++) PStates[i][j][k]=0;
					ind=InternalVars[i][j][0];
					if((ind<0)||(ind>=PL_WONUM)) break;
					ind=PL_WoGOptions[0][ind];
					if((ind<0)||(ind>=DlgItems[i][j].ItemCount)) break;
					PStates[i][j][ind]=1;
					break;
			}
		}
	}
	CheckDepend(-1);
	RETURNV;
}

void Prepare2Close(void)
{
	STARTNA(__LINE__, 0)
	int i,j,k,ind;

	for(i=0;i<8;i++){
		for(j=0;j<4;j++){
			switch(DlgItems[i][j].Type){
				case 0: break; // нет группы
				case 1:
					for(k=0;k<20;k++){
						if(InternalVars[i][j][k]==-1) continue;
						ind=InternalVars[i][j][k];
						if((ind<0)||(ind>=PL_WONUM)) continue;
						if(_PDisabled[i][j][k]) PL_WoGOptions[0][ind]=0;
						else{
							if((ind>0)&&(ind<5)) PL_WoGOptions[0][ind]=!PStates[i][j][k];
							else PL_WoGOptions[0][ind]=PStates[i][j][k];
						}
					}
					break;
				case 2:
					ind=InternalVars[i][j][0];
					if((ind<0)||(ind>=PL_WONUM)) break;
					for(k=0;k<20;k++) if(PStates[i][j][k]==1) break;
					if(k>=20) break;
					PL_WoGOptions[0][ind]=k;
					break;
			}
		}
	}
	RETURNV;
}
/////////////////
static Dword CWS_Ecx,CWS_Edx;
_MouseStr_ *CWS_Mp;
void _CallWoGSetup(void)
{
	STARTNA(__LINE__, 0)
	int i;
//  if(CWS_Mp->Xabs<0x297) return;
//  if(CWS_Mp->Xabs>0x2EA) return;
//  if(CWS_Mp->Yabs<0x4E) return;
//  if(CWS_Mp->Yabs>0x7D) return;
	if(CWS_Mp->Xabs<0x270) RETURNV
	if(CWS_Mp->Xabs>0x2EC) RETURNV
	if(CWS_Mp->Yabs<0x6A) RETURNV
	if(CWS_Mp->Yabs>0x79) RETURNV
	DlgSetup.Flags|=0x1;
	LastChoosenPage=-1;
	if(EnableWoGDlgEdit){
		for(i=0;i<PL_WONUM;i++) PL_WoGOptions[0][i]=PL_WoGOptions[1][i];
	}
	if(EnableWoGDlgEdit){
		DlgSetup.ButtonsStates=DlgButState;
		DlgSetup.ButtonsStates[1]=1; // Save
		DlgSetup.ButtonsStates[8]=1; // Restore
	}else{
		DlgSetup.ButtonsStates=DlgButState2;
	}
	Prepare2Show();
	// 3.58
	if(EnableWoGDlgEdit) DlgSetup.Flags&=~0x2; else DlgSetup.Flags|=0x2;
	ShowWoGSetup(&DlgSetup);
	Prepare2Close();
	if(EnableWoGDlgEdit){
		for(i=0;i<PL_WONUM;i++) PL_WoGOptions[1][i]=PL_WoGOptions[0][i];
	}
	RETURNV
}
int GoToNewGameScreen(void)
{
	__asm{
		mov   eax,1
		mov   EnableWoGDlgEdit,eax
		mov   eax,0x4F0B20
		call  eax
		push  eax
		xor   eax,eax
		mov   EnableWoGDlgEdit,eax
		pop   eax
		mov   IDummy,eax
	}
	return IDummy;
}

void __stdcall CallWoGSetup(_MouseStr_ *mp,int v2,int v1)
{
	__asm mov   CWS_Ecx,ecx
	__asm mov   CWS_Edx,edx
	__asm mov   eax,mp
	__asm mov   CWS_Mp,eax
	__asm pusha
	_CallWoGSetup();
	__asm popa
	__asm push  v1
	__asm push  v2
	__asm push  mp
	__asm mov   edx,CWS_Edx
	__asm mov   ecx,CWS_Ecx
	__asm mov   eax,0x586880
	__asm call  eax
}

///////////////////////////
static char Load2Path[MAX_PATH]; // 3.59 alternate path for loading scripts
void LoadERSfile(const char *TxtName, int ind){
	STARTNA(__LINE__, 0)
	if(LoadTXT(TxtName,&ScriptCommand[ind])) ScriptCommandLoaded[ind]=0;
	else ScriptCommandLoaded[ind]=1;
	RETURNV
}
int LoadSetupParam(void)
{
	STARTNA(__LINE__, 0)
	int i;

	if(LoadTXT("ZSETUP00.TXT",&WoGTexts)) RETURN(1) // не может загрузить TXT
	if(LoadTXT("ZSETUP01.TXT",&WoGCommands)) RETURN(1) // не может загрузить TXT
	ReadStrINI(WoGFilePath,MAX_PATH,".\\","Options_File_Path","WoGification");
	ReadStrINI(WoGFileName,MAX_PATH,"WoGSetupEx.dat","Options_File_Name","WoGification");
	ReadStrINI(Load2Path,MAX_PATH,"","Alternate_Script_Location","WoGification");
	LuaCall("LoadAllERS");
	ProcessAll();
	InitWoGSetup();
	for(i=0;i<PL_WONUM;i++) PL_WoGOptions[1][i]=PL_WoGOptions[0][i];
	RETURN(0)
}

