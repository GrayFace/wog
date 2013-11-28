#include "dll.h"

STRUCTOFMEM TStructOfMem;
int MaxMonstr;

bool LoadingMonstrs()
{         

         MaxMonstr = GetMaxNumOfLoadingMonstrs();  
         AllocateMemmory(MaxMonstr, &TStructOfMem);   
         CopyOldMonstrsStruct(&TStructOfMem);
         PatchExe(MaxMonstr, &TStructOfMem);
         LoadNewMonstrs(&TStructOfMem);
         
     return TRUE;
}

bool CleanMemmory ()
{
        // очищаем выделенную память
        /*free(TStructOfMem.p_NewMonstrStruct);
        free(TStructOfMem.p_NewCrAnim);
        free(TStructOfMem.p_NewAtack);
        free(TStructOfMem.p_NewDefend);
        free(TStructOfMem.p_NewOption); */
        free(TStructOfMem.p_CharStruct);
        free(TStructOfMem.p_DevPalette);
        free(TStructOfMem.p_NewCadreAVW1);
        free(TStructOfMem.p_NewCadreAVW2); 
        free(TStructOfMem.p_NewCadreCprsm);  
        free(TStructOfMem.p_NewCadreTWC); 
        free(TStructOfMem.p_NewPaleate);  
        free(TStructOfMem.p_NewPaleateAVW1);  
        free(TStructOfMem.p_NewPaleateAVW2);  
        free(TStructOfMem.p_NewPaleateTWC);   
        free(TStructOfMem.p_NewShell);   
        free(TStructOfMem.p_NewSpriteAVW1);
        free(TStructOfMem.p_NewSpriteAVW2);
        free(TStructOfMem.p_NewSpriteCprsm);
        free(TStructOfMem.p_NewSpriteMem);
        free(TStructOfMem.p_NewSpriteMemAVW);
        free(TStructOfMem.p_NewSpriteMemTWC);
        free(TStructOfMem.p_NewSpriteTWC);
        free(TStructOfMem.p_ShellDefs);

		return TRUE;
}

