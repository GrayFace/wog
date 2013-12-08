#ifndef _LOGMANAGER_H_
#define _LOGMANAGER_H_

//#define DebugBuild

#include <windows.h>
#include <stdio.h>


#ifdef DebugBuild
	void __inline addlog(char *s,int flag = 0){FILE* f=fopen("dxproxylog.txt", "at");if(flag==0)fprintf(f,"%s\n",s);else fprintf(f,"%s:%d\n",s,flag);fclose(f);}
	void __inline clearlog(){DeleteFile("Logger.txt"); DeleteFile("Logger.txt");}
#else
	void __inline clearlog(){ DeleteFile("Logger.txt"); }
	void __inline addlog(char*,int = 0) {}
#endif

void __inline adduserlog(const char *s){
	FILE* f;
	if(!fopen_s(&f, "Logger.txt", "at"))
		fprintf(f,"%s\n",s);fclose(f);
}


#endif