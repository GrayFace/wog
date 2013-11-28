//#include "classes.h"
void __stdcall CallWoGSetup(_MouseStr_ *mp,int v2,int v1);
int GoToNewGameScreen(void);

////////////
extern int EnableWoGDlgEdit;
extern _DlgSetup DlgSetup;
void InitWoGSetup(void);
char *UseWogSetup(_DlgSetup *dlg);
void LoadERSfile(const char *TxtName, int ind);
int LoadSetupParam(void);
