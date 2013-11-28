#include <windows.h>

#include "MappingFile.h"
#include "Memmory.h"
#include "..\structs.h"

extern DEF TDef;

bool LoadingMonstrs();
bool CleanMemmory ();
int  GetMaxNumOfLoadingMonstrs();
int GetMaxGenerationMonstr ();
void InitilizeStructFromWoG (void* p_wMonstrStruct , void* p_wCrAnim, void* p_wAtack, void* p_wDefend, void* p_wOption,  void* p_wSingleName, void* p_wPularName, void* p_wSpec);
void _LoadPortrets (DEF* TDef, void* p_SpriteMem, void* p_SpriteStruct, void* p_pal, int n_s, int i_s, int index);
void _LoadSprite (void* p_cadre, void* p_palette, void* p_spr, SPRITE* p_sprite, void* p_bufpal, int numMonstr);
/*extern "C"{
 __stdcall void InsertShellAsmCode ();
 __stdcall void InsertCodeDef (); 
 __stdcall __declspec(dllexport) void LoadSnar (int NumMonstr, int TypeSnar, char* &p_NameSnar);
 __stdcall void LoadPortrets (DEF* TDef);
 __stdcall void LoadSnar (int NumMonstr, int TypeSnar, char* &p_NameSnar);
 __stdcall void Hook47BE90 ();
 __stdcall void Hook47C300 ();
 __stdcall void Hook47CE10 ();
 __stdcall void Hook47C9C0 ();
 __stdcall void Hook47D660 ();
 __stdcall void Hook47D250 ();
 __stdcall void _Hook (int* p_DevicePal, SPRITEINMEM* p_sprite); 
}*/

void Hook47BE90();
void Hook47D660();
void Hook47D250();
void Hook47CE10();
void Hook47C9C0 ();
void Hook47C300 ();
void InsertCodeDef ();
void InsertShellAsmCode ();
void Hook54110F ();
