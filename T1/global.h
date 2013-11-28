#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <windows.h>
#include "LogManager.h"
#include "dxproxy.h"




extern	const	DWORD		oldMainProcAddress;
//extern	const char(__stdcall * oldAnalizeEnteredTextProc)(char *,int);
//extern	const	DWORD		oldAnalizeEnteredTextAddress;

extern	const	bool		newExtendedDebugMod;
extern  LPPX_DIRECTDRAW		px_DxObj;
extern	CLogManager			LogManager;


extern void				newWriteInMemory(int queue=0);
void					newGlobalInitSub();
void					newFileLoader();
extern void				ParseCommandLine();
extern LRESULT CALLBACK newMainProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
extern HRESULT WINAPI	px_DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
extern BOOL				newPaintGetClientRect(HWND hWnd,LPRECT lpRect);
///
extern const char __stdcall HookCmdLineEnteredText(char *str, int var);

extern PCHAR*			CommandLineToArgvA(PCHAR CmdLine,int* _argc);


extern char	*			ApplicationPath;
extern char	*			ApplicationDir;

extern char HookHintWindow; // set to 1 inside HintWindow
extern bool PlayingBM_V;
extern int InGame;
extern int EnableMovingMonsters;

#endif
