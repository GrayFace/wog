#ifndef _TOWN_H_
#define _TOWN_H_

#include "structs.h"

//extern TxtFile TOWNTYPE;
extern char *TOWNTYPES[TOWNNUM],*BLKSMTH[TOWNNUM];
extern char *TownsBackGroundPrefix[];
extern char *TownsBuildingsDefs[];
extern char *TownsBuildingsIcons[];
extern char *TownsBuildingsSelection[];
extern char  TownsBuildingsTree[];
extern short TownsBuildingsCoord[];
extern long  TownsBuildingsDep[];
extern long  TownsBuildingsCost0[];
extern long  TownsBuildingsCost1[];
extern unsigned char  TownsBuildingsDrawQueue[];
extern long  TownsBuildingsInd[];
extern char  TownsBuildingsSpecNum[];
extern char *TownsHallDefs[];
extern char *TownsHalls[];
extern char *Towns1[];
extern long  TownsHallLoading[];
extern long  TownsHallBuilding[];
extern char *TownsMapObject0[];
extern char *TownsMapObject1[];
extern char *TownsMapObject2[];
extern char *Towns2[];
extern char *TownsBackGroundCreatures0[];
extern char *TownsBackGroundCreatures1[];
extern long  TownsCreaturesTree[];
extern char *TownsBackGroundMage[];
extern char *TownsBackGroundBattle[];
extern char *TownsWalls[];// 0066D84A                             L0066D84A:
extern char *TownsPuzzle[];
long TownsBlackSmithCreatures[];
long TownsBlackSmithArtifacts[];





int LoadTownsNames();
int LoadBlackSmithDesc();

void CaseBastionLoading();
void FormTownBuildDepends();


#endif




