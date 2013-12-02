#include "LogManager.h"		

//#ifdef DebugBuild
//void addlog(char *s,int flag){FILE* f=fopen("dxproxylog.txt", "at");if(flag==0)fprintf(f,"%s\n",s);else fprintf(f,"%s:%d\n",s,flag);fclose(f);}
//void clearlog(){DeleteFile("Logger.txt"); DeleteFile("Logger.txt");}
//#else
void clearlog(){DeleteFile("Logger.txt");}
//#endif
void adduserlog(const char *s){FILE* f=fopen("Logger.txt", "at"); fprintf(f,"%s\n",s);fclose(f);}

void CLogManager::AddEntry(unsigned char Tab,char* Text,DWORD TextColor,DWORD	BgColor)
{
	SEntry *sEntry=new SEntry;
	sEntry->BgColor=BgColor;
	sEntry->Dyn=false;
	sEntry->Text=Text;
	sEntry->TextColor=TextColor;
	//	return;
	LogEntry[Tab].insert(LogEntry[Tab].end(),sEntry); 
	//LogEntry[Tab].push_back(sEntry);
}

void CLogManager::AddDynEntry(unsigned char Tab,char* Text,DWORD TextColor,DWORD	BgColor)
{
	SEntry *sEntry=new SEntry;

	sEntry->BgColor=BgColor;
	sEntry->Dyn=true;
	sEntry->Text=new char[strlen(Text)+1];
	strcpy(sEntry->Text,Text);
	sEntry->TextColor=TextColor;
	LogEntry[Tab].insert(LogEntry[Tab].end(),sEntry); 
	//LogEntry[Tab].push_back(sEntry);
}

void CLogManager::InsertEntry(unsigned char Tab,int num,char* Text,DWORD TextColor,DWORD BgColor)
{
	if(num>=(int)LogEntry[Tab].size()||num<0)
		return;
	SEntry *sEntry=new SEntry;
	sEntry->BgColor=BgColor;
	sEntry->Dyn=false;
	sEntry->Text=Text;
	sEntry->TextColor=TextColor;
	int i;std::deque<SEntry*>::iterator it;
	for(it=LogEntry[Tab].begin(),i=0; i<num; it++,i++);
	LogEntry[Tab].insert(it,sEntry); 
}

void CLogManager::InsertDynEntry(unsigned char Tab,int num,char* Text,DWORD TextColor,DWORD BgColor)
{
	if(num>=(int)LogEntry[Tab].size()||num<-1)
		return;
	SEntry *sEntry=new SEntry;
	sEntry->BgColor=BgColor;
	sEntry->Dyn=true;
	sEntry->Text=new char[strlen(Text)+1];
	strcpy(sEntry->Text,Text);
	sEntry->TextColor=TextColor;
	int i;std::deque<SEntry*>::iterator it;
	for(it=LogEntry[Tab].begin(),i=0; i<num; it++,i++);
	LogEntry[Tab].insert(it,sEntry); 
}

SEntry *CLogManager::GetEntry(unsigned char Tab,int num)
{
	if(num>=(int)LogEntry[Tab].size()||num<0)
		return NULL;
	return LogEntry[Tab][num];
}

void CLogManager::DeleteEntry(unsigned char Tab,int num)
{
	if(num>=(int)LogEntry[Tab].size()||num<0)
		return;
	int i;std::deque<SEntry*>::iterator it;
	for(it=LogEntry[Tab].begin(),i=0; i!=num; it++,i++);
	if(LogEntry[Tab][num]->Dyn)
		delete LogEntry[Tab][num]->Text;
	delete LogEntry[Tab][num];
	LogEntry[Tab].erase(it);
}

void CLogManager::ClearTab(unsigned char Tab)
{
	if(LogEntry[Tab].empty())
		return;
	int i;
	for(i=0;i<(int)LogEntry[Tab].size();i++)
	{
		if(LogEntry[Tab][i]->Dyn)
			delete LogEntry[Tab][i]->Text;
		delete LogEntry[Tab][i];
	}
	LogEntry[Tab].clear(); 
}

int CLogManager::GetSize(unsigned char Tab)
{
	return LogEntry[Tab].size();
}