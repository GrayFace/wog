#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "b1.h"
#define __FILENUM__ 17

//////////////////////////
// Загрузка TXT
int LoadTXT(const char *name, TxtFile *tf)
{
	STARTNA(__LINE__, 0)
	_FileLoaded_ *FLpo;

	if(tf==0) RETURN(-1)
	__asm{
		mov   ecx,name
		mov   eax,0x55C2B0
		call  eax
		mov   FLpo,eax
	}
	if(FLpo==0){ 
		tf->fl=0; tf->sn=0;
		RETURN(-1)
	}
	tf->fl=FLpo;
	
	tf->sn=FLpo->Stop-FLpo->Start; // само разделит на 4
	RETURN(0)
}

void UnloadTXT(TxtFile *tf)
{
	STARTNA(__LINE__, 0)
	_FileLoaded_ *FLpo;

	FLpo=tf->fl;
	if(FLpo==0) RETURNV
	__asm{
		mov   ecx,FLpo
		mov   edx,[ecx]
		call  dword ptr [edx+4]
	}
	tf->fl=0; tf->sn=0;
	RETURNV
}

char *NoTxtString="{Сервис}: Запрошенная строка не найдена в файле текстового ресурса.";
char *ENoTxtString="{Service}: The line is not found in the text resource.";

char *ITxt(int StrNum,int ItemNum,TxtFile *tf)
{
	STARTNA(__LINE__, 0)
	_TxtString_ *strpo;
	int  ItemNumber;

	if(!tf || StrNum>=tf->sn) RETURN(WoGType ? NoTxtString : ENoTxtString)
	strpo=tf->fl->Start[StrNum];
	ItemNumber=strpo->Stop-strpo->Start;
	if(ItemNum>=ItemNumber) RETURN(WoGType ? NoTxtString : ENoTxtString)
	RETURN(strpo->Start[ItemNum])
}

void FileLoader(void);

void FileLoad(void)
{
	__asm pusha
	FileLoader();
	__asm popa
}


