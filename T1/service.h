#include "classes.h"
#include "txtfile.h"
#include "windows.h"
////////////////////////
/*
typedef struct _OSVERSIONINFO
{ 
	Dword dwOSVersionInfoSize; 
	Dword dwMajorVersion; 
	Dword dwMinorVersion; 
	Dword dwBuildNumber; 
	Dword dwPlatformId; 
	char  szCSDVersion[ 128 ]; 
} OSVERSIONINFO;
*/
////////////////////////
extern int     TerminateFlag;
extern TxtFile TXTSphinx;
extern char    DeveloperPath[MAX_PATH];
extern char    ModsPath[MAX_PATH];
////////////////////////
int LoadSphinxTXT(void);
int StartDLLService(void);
int StopDLLService(void);
///////////////////////
Dword Service_LoadLibrary(char *DLLName);
Dword Service_GetProcAddress(Dword Handle,char *FunName);
int   Service_FreeLibrary(Dword Handle);
int Service_GetForegroundWindow(void);
Dword Service_timeGetTime(void);
int Service_GetFileAttributes(char *FileName);
///////////////////////
int LoadZVSDialogs(void);
int CommonDialog(char *Mess);
int ChooseCastle(void);
int CustomReq(int Num,int startup,char **answ);
int SphinxReq(int Num);
void CustomPic(int Ind,int startup);
void CustomHPic(int Ind,int Hn,int startup);
int ShowCurse(_CurseShow *CurseShow);
int ShowNPC(_DlgNPC *DlgNPC);
int AddExtCMDRequest(int Num
		,char *txt1,char *txt2,char *txt3
		,char *pic1,char *pic2,char *pic3,char *pic4
		,char *pch1,char *pch2,char *pch3,char *pch4
		,char *but1,char *but2,char *but3,char *but4
		,char *bth1=0,char *bth2=0,char *bth3=0,char *bth4=0
		,char HasEscape=1);
int MultiCheckReq( char*TxtH,int state,char *txt[12],int chRAD);
void MultiCheckReqInt(_CheckBoxes *StartUpSetUp);
int AddExtCMDPic(int Num,char *pic,int Repiter);
char *HasText(char *str);
int ShowDiplom(int);
int ShowRadio(_PopUpRadioButtons *PopUpRadioButtons);
void ShowWoGSetup(_DlgSetup *DlgSetup);
int SaveSetupState(char *Name,void *Buf,int Len);
int LoadSetupState(char *Name,void *Buf,int Len);
// 3.58
int ChooseFileDlg(_ChooseFile* ChooseFile, char *Default);
int QuickDialog(_Sphinx1 *Sphinx);
int ChooseIconDlg(_IconList * IconList);
int WoGMessage(char *,char * =0,char * =0,char * =0,char * =0);
int ReadIntINI(int DefValue,char *Parameter,char *Section="Common",char *File=".\\WoG.ini");
int ReadStrINI(char *Value,int Len,char *DefValue,char *Parameter,char *Section="Common",char *File=".\\WoG.ini");
int WriteIntINI(int Value,char *Parameter,char *Section="Common",char *File=".\\WoG.ini");
int WriteStrINI(char *Value,char *Parameter,char *Section="Common",char *File=".\\WoG.ini");
int ShowUpgradeDlg(_MonsterInfo *MonInfo);
void PicDlg(char *Msg,char *Pic);
void PicDlg_Int(char *Msg,char *Pic);
char *GetFolder(int Mod);
int DoesFileExist(char *FName,int Mod);
PROC DllImport(const char * dll, const char * proc, bool halt);

void __inline LoadIniPath(char (&buf)[MAX_PATH], char *name, char *def, char *Section="Common", char *File=".\\WoG.ini")
{
	ReadStrINI(buf, MAX_PATH, def, name, Section, File);
	int i = strlen(buf);
	if (i > 0 && i < MAX_PATH-1 && buf[i-1] != '\\' && buf[i-1] != '/')
	{
		buf[i] = '\\';
		buf[i+1] = 0;
	}
}

///////////////////////
//int LoadCustomTxt(void);
int LoadCustomTxt(int NEWload,int Apply);
int SaveCustom(void);
int LoadCustom(int ver);

void InitCRT(void);

void TestDlg(void);


