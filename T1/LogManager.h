#ifndef _LOGMANAGER_H_
#define _LOGMANAGER_H_

//#define DebugBuild

#include <windows.h>
#include <deque>

extern void clearlog();
extern void adduserlog(const char *s);
#define addlog adduserlog

//#ifdef DebugBuild
//extern void addlog(char *s,int flag=0);
//#else
//void __inline addlog(char*,int = 0) {}
//#endif

#define TAB_GENERAL		0
#define TAB_WOG			1
#define TAB_ERM			2
#define TAB_LUA			3
#define TAB_OTHER		4
#define TAB_CMD			5
#define TAB_MSG			6

struct SEntry
{
	DWORD			TextColor;
	DWORD			BgColor;
	char		*	Text;
	bool			Dyn;
};


class CLogManager
{
	public:
		void 		AddEntry(unsigned char Tab,char* Text,DWORD TextColor=0xFFFFFF,DWORD BgColor=0);
		void 		AddDynEntry(unsigned char Tab,char* Text,DWORD TextColor=0xFFFFFF,DWORD	BgColor=0);
		void 		InsertEntry(unsigned char Tab,int num,char* Text,DWORD TextColor=0xFFFFFF,DWORD	BgColor=0);
		void 		InsertDynEntry(unsigned char Tab,int num,char* Text,DWORD TextColor=0xFFFFFF,DWORD	BgColor=0);
		SEntry	*	GetEntry(unsigned char Tab,int num);
		void 		DeleteEntry(unsigned char Tab,int num);
		void 		ClearTab(unsigned char Tab);
		int 		GetSize(unsigned char Tab);
	private:
		std::deque<SEntry*>	LogEntry[10];
	protected:

	//friends:
};


#endif