#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <windows.h>
#include "LogManager.h"
#include "dxproxy.h"


extern void newWriteInMemory(int queue=0);
void newGlobalInitSub();
void newFileLoader();
void ParseCommandLine();

PCHAR* CommandLineToArgvA(PCHAR CmdLine,int* _argc);


extern char	AppPath[MAX_PATH];

extern char HookHintWindow; // set to 1 inside HintWindow
extern bool PlayingBM_V;
extern int InGame;
extern int EnableMovingMonsters;

#endif
