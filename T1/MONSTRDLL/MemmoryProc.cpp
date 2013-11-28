#include <windows.h>
#include "dll.h"


CRANIM T_CrAnim;
CHARSTRUCT T_CharStr;

//void** p_MonstrStruct; //= (void**)0x6747B0;    /* адрес структуры монстров */
//void** p_CrAnim; //= (void**) 0x50CC04;         /* указатель на структуру cranim */
//void** p_Atack; //= (void**) 0x440237;          /* указатель на структуру свойств монстров при атаке */
//void** p_Defend; //= (void**) 0x440916;         /* указатель на структуру свойств при защите монстров */
//void** p_Option; //= (void**) 0x447475;         /* указатель на структуру ... свойств монстров */
  void** p_Shell  = (void**) 0x43DA9F;          // указатель на структуру снарядов

int i_ExistingMonstr = 197;                  /* кол-во стандартных монстров */
/*
//адреса где встречаются указатели на структуру монстров
void** p_AddressOfPatchStrMonstr [] = {(void**)0x7ADD1, (void**)0x2747B0,
(void**)0x2EB5D2, (void**)0x2EB8E0, (void**)0x2F1455, (void**)0x2F1473, (void**)0x2F1482,
(void**)0x2F1492, (void**)0x2F14A1, (void**)0x2FBF55, (void**)0x2FBF55, (void**)0x2FC3AE,
(void**)0x2FC3CA, (void**)0x2FC50C, (void**)0x2FC50C, (void**)0x2FD6E0, (void**)0x30705C,
(void**)0x308BED, (void**)0x325350, (void**)0x32535C, (void**)0x326FAC};

// адреса где встречаются указатели на структуру CrAnim
void** p_AddressOfPatchCrAnim [] = {(void**)0x10CC04, (void**)0x27FF74};

// адреса где встречаются указатели на структуру св-в при атаке
void** p_AddressOfPatchAtack [] = {(void**)0x40237};

// адреса где встречаются указатели на структуру св-в при защите
void** p_AddressOfPatchDefend [] = {(void**)0x40916};

// адреса где встречаются указатели на структуру св-в 
void** p_AddressOfPatchOption [] = {(void**)0x47475};
*/
// адрес таблицы снарядов
void** p_AddressOfShell [] = {(void**)0x3DA9F};

/*
void** p_AddressOfERMLimit [] = {(void**)0x2F1410, (void**)0x2EB4F8, (void**)0x2CDCA9, 
                                 (void**)0x2CE326, (void**)0x2CD4BD, (void**)0x2AFECA,
                                 (void**)0x2B9F50, (void**)0x2BF76B, (void**)0x2BFA89, (void**)0x2BFB15,
                                 (void**)0xDBA62, (void**)0x1F40CD, (void**)0xDBB75, (void**)0x1C84B1,
                                 (void**)0x1DDA4B, (void**)0x1213E5, (void**)0xDBAE2, (void**)0xDBB98,
                                 (void**)0x12141E, (void**)0xDBA9F, (void**)0x150826, (void**)0xC2B1,
                                 (void**)0x12FFBB, (void**)0xA6A7E};
                                 
void** p_AddressOfERM [] = {(void**)7693397, //0 
                            (void**)7693514, //4
                            (void**)7694533, //16
                            (void**)7550016, (void**)0x71F252,(void**)0x724E05,(void**)0x733507,(void**)0x750DA5, //20
                            (void**)7550069, (void**)0x71F26E, (void**)0x733533, (void**)0x750DBB, //24 
                            (void**)7550123, (void**)0x73355F, (void**)0x750DD1, //28
                            (void**)7693689, (void**)0x750639, (void**)0x750948, (void**)0x760FD2, (void**)0x761589, //32
                            (void**)0x760FFC, (void**)0x7615B3, //36
                            (void**)0x761025, (void**)0x7615DC, //40
                            (void**)0x76104F, (void**)0x761606, //44
                            (void**)0x761079, (void**)0x761630, //48
                            (void**)0x7610A2, (void**)0x761659, //52
                            (void**)0x7610CC, (void**)0x761683, //56
                            (void**)7693743, (void**)0x750654, (void**)0x750962, (void**)0x7610F6, (void**)0x7616AD, //60
                            (void**)7693797, (void**)0x75066C, (void**)0x75097A, (void**)0x76111F, (void**)0x7616D6, //64
                            (void**)7693851, (void**)0x750684, (void**)0x750992, (void**)0x761149, (void**)0x761700, //68
                            (void**)7693905, (void**)0x75069C, (void**)0x7509AA, (void**)0x761173, (void**)0x76172A, //72
                            (void**)7693959, (void**)0x7506B4, (void**)0x7509C2, (void**)0x76119C, (void**)0x761753, //76
                            (void**)7694013, (void**)0x7506CC, (void**)0x7509DA, (void**)0x7566DB, (void**)0x7566EA, (void**)0x7611C6, (void**)0x76177D, //80
                            (void**)7694099, (void**)0x7506E4, (void**)0x7509F2, (void**)0x7611F0, (void**)0x7617A7, //84
                            (void**)7694154, (void**)0x7506FC, (void**)0x750A0A, (void**)0x761219, (void**)0x7617D0,//88 
                            (void**)7694209, (void**)0x750714, (void**)0x750A22, (void**)0x761243, (void**)0x7617FA, //92
                            (void**)7694264, (void**)0x75072C, (void**)0x750A3A, (void**)0x76126D, (void**)0x761824,//96
                            (void**)7694319, (void**)0x750744, (void**)0x750A52, (void**)0x761296, (void**)0x76184D, //100
                            (void**)7694374, (void**)0x75075C, (void**)0x750A6A, (void**)0x7612C0, (void**)0x761877, //104
                            (void**)7694429, (void**)0x750774, (void**)0x750A82, (void**)0x7612EA, (void**)0x7618A1,//108
                            (void**)7694484, (void**)0x75078C, (void**)0x750A9A, (void**)0x761310, (void**)0x7618CA};//112
                            
int i_OffERM [] = {0, 4, 16, 20, 20, 20, 20, 20 ,24, 24, 24, 24 ,28, 28, 28, 32, 32, 32, 32, 32, 
                   36, 36, 40, 40, 44, 44, 48, 48, 52, 52, 56, 56, 60, 60, 60, 60, 60, 64, 64, 64, 64, 64,
                   68, 68, 68, 68, 68, 72, 72, 72, 72, 72, 76, 76, 76, 76, 76, 80, 80, 80, 80, 80, 80, 80,
                   84, 84, 84, 84, 84, 88, 88, 88, 88, 88, 92, 92, 92, 92, 92, 96, 96, 96, 96, 96,
                   100, 100, 100, 100, 100, 104, 104, 104, 104, 104, 108, 108, 108, 108, 108, 112, 112, 112, 112, 112};
*/
/*----------------------------------------------------------------------------*/
bool AllocateMemmory(int MaxMonstr, STRUCTOFMEM* P_StructOfMem)
{
 /*   // выделяем память под структуру новых монстров
    P_StructOfMem->p_NewMonstrStruct = calloc(sizeof(STRUCTOFMONSTR), MaxMonstr);
    
    // выделяем память под структуру анимации в бою
    P_StructOfMem->p_NewCrAnim = calloc(sizeof(CRANIM), MaxMonstr);
    
    // выделяем память под структуру св-в при атаке
    P_StructOfMem->p_NewAtack = calloc(1, MaxMonstr);
    
    //выделяем память под структуру св-в при защите
    P_StructOfMem->p_NewDefend = calloc(1, MaxMonstr);
    
    //выделяем память под структуру св-в
    P_StructOfMem->p_NewOption = calloc(1, MaxMonstr);
*/ 
    // выделяем память под строковые константы новых монстров
    P_StructOfMem->p_CharStruct = calloc(sizeof(CHARSTRUCT), MaxMonstr - i_ExistingMonstr);
   
    // выделяем память под новую структуру снарядов
    P_StructOfMem->p_NewShell = calloc(1, MaxMonstr);
    
    //выделяем память под имена новых дефов снарядов
    P_StructOfMem->p_ShellDefs = calloc(32, MaxMonstr - i_ExistingMonstr);
    
    // выделяем память под данные спрайтов cprsmall
    P_StructOfMem->p_NewCadreCprsm = calloc(0x3000, MaxMonstr - i_ExistingMonstr);    
    // выделяем память под структуру спрайтов cprsmall
    P_StructOfMem->p_NewSpriteCprsm = calloc(sizeof(SPRITEINMEM), MaxMonstr - i_ExistingMonstr);
    P_StructOfMem->p_NewSpriteMem = calloc(4, MaxMonstr + 2);    
    // выделяем память под палитру спрайтов
    P_StructOfMem->p_NewPaleate = calloc(0x1000, MaxMonstr - i_ExistingMonstr);
    
    // выделяем память под данные спрайтов twcrport
    P_StructOfMem->p_NewCadreTWC = calloc(0x3000, MaxMonstr - i_ExistingMonstr);    
    // выделяем память под структуру спрайтов twcrport
    P_StructOfMem->p_NewSpriteTWC = calloc(sizeof(SPRITEINMEM), MaxMonstr - i_ExistingMonstr);
    P_StructOfMem->p_NewSpriteMemTWC = calloc(4, MaxMonstr + 2);    
    // выделяем память под палитру спрайтов twcrport
    P_StructOfMem->p_NewPaleateTWC = calloc(0x1000, MaxMonstr - i_ExistingMonstr);
    
    P_StructOfMem->p_NewSpriteMemAVW = calloc(4, MaxMonstr*2);
    // выделяем память под данные спрайтов avwattack1
    P_StructOfMem->p_NewCadreAVW1 = calloc(0x3000, MaxMonstr - i_ExistingMonstr);    
    // выделяем память под структуру спрайтов avwattack1
    P_StructOfMem->p_NewSpriteAVW1 = calloc(sizeof(SPRITEINMEM), MaxMonstr - i_ExistingMonstr);
    //P_StructOfMem->p_NewSpriteMemAVW1 = calloc(4, MaxMonstr + 2);    
    // выделяем память под палитру спрайтов avwattack1
    P_StructOfMem->p_NewPaleateAVW1 = calloc(0x1000, MaxMonstr - i_ExistingMonstr);
    
    // выделяем память под данные спрайтов avwattack2
    P_StructOfMem->p_NewCadreAVW2 = calloc(0x3000, MaxMonstr - i_ExistingMonstr);    
    // выделяем память под структуру спрайтов avwattack2
    P_StructOfMem->p_NewSpriteAVW2 = calloc(sizeof(SPRITEINMEM), MaxMonstr - i_ExistingMonstr);
    //P_StructOfMem->p_NewSpriteMemAVW2 = calloc(4, MaxMonstr + 2);    
    // выделяем память под палитру спрайтов avwattack2
    P_StructOfMem->p_NewPaleateAVW2 = calloc(0x1000, MaxMonstr - i_ExistingMonstr);    
    
    // память для хранения подменяемой девайс палитры
    P_StructOfMem->p_DevPalette = calloc(0x1000, 1);   
    
    return TRUE;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void InitilizeStructFromWoG (void* p_wMonstrStruct , void* p_wCrAnim, void* p_wAtack, void* p_wDefend, void* p_wOption,  void* p_wSingleName, void* p_wPularName, void* p_wSpec)
{
    TStructOfMem.p_NewMonstrStruct = p_wMonstrStruct;
    TStructOfMem.p_NewCrAnim       = p_wCrAnim;
    TStructOfMem.p_NewAtack        = p_wAtack;
    TStructOfMem.p_NewDefend       = p_wDefend;
    //TStructOfMem.p_NewOption       = p_wOption;
    TStructOfMem.p_WogMonSName     = p_wSingleName;
    TStructOfMem.p_WogMonPName     = p_wPularName;
    TStructOfMem.p_WogMonSpec      = p_wSpec;   
    
    return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
bool CopyOldMonstrsStruct(STRUCTOFMEM* P_StructOfMem)
{
/*     
    // копируем старую структуру монстров
    memcpy(P_StructOfMem->p_NewMonstrStruct, *p_MonstrStruct, i_ExistingMonstr*sizeof(STRUCTOFMONSTR));
    //копируем старую структуру анимации
    memcpy(P_StructOfMem->p_NewCrAnim, *p_CrAnim, i_ExistingMonstr*sizeof(CRANIM));
    //копируем старуюструктуру св-в при атаке
    memcpy(P_StructOfMem->p_NewAtack, *p_Atack, i_ExistingMonstr - 0x16);
    //копируем старую структуру св-в при защите
    memcpy(P_StructOfMem->p_NewDefend, *p_Defend, i_ExistingMonstr - 0x3F);
    //копируем старую стр-ру св-в    
    memcpy(P_StructOfMem->p_NewOption, *p_Option, i_ExistingMonstr);
*/
    // копируем старую структуру снарядов
    memcpy(P_StructOfMem->p_NewShell, *p_Shell, i_ExistingMonstr);
    
    return TRUE;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void ModOffset (void*** p_Address)
{
    if ((int)*p_Address < 0x29C000)
        *p_Address = (void**)((int)*p_Address + 0x400000);
    else
        *p_Address = (void**)((int)*p_Address + 0x465000);        

}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

bool PatchExe (int MaxMonstr, STRUCTOFMEM* P_StructOfMem)
{
  /*  // патчим адреса со старой структурой монстра
    for(int i = 0; i< sizeof(p_AddressOfPatchStrMonstr)/sizeof(void**); i++)
    {        
            ModOffset(&p_AddressOfPatchStrMonstr[i]);
            *p_AddressOfPatchStrMonstr[i] = (void*)P_StructOfMem->p_NewMonstrStruct; 
    }
    
    // патчим адреса со старой CRAnim
    for(int i = 0; i< sizeof(p_AddressOfPatchCrAnim)/sizeof(void**); i++)
    {        
            ModOffset(&p_AddressOfPatchCrAnim[i]);
            *p_AddressOfPatchCrAnim[i] = (void*)P_StructOfMem->p_NewCrAnim; 
    }
    
    // патчим адреса со старой структурой св-в при атаке
    for(int i = 0; i< sizeof(p_AddressOfPatchAtack)/sizeof(void**); i++)
    {        
            ModOffset(&p_AddressOfPatchAtack[i]);
            *p_AddressOfPatchAtack[i] = (void*)P_StructOfMem->p_NewAtack; 
    }
    
    // патчим адреса со старой структурой св-в при защите
    for(int i = 0; i< sizeof(p_AddressOfPatchDefend)/sizeof(void**); i++)
    {        
            ModOffset(&p_AddressOfPatchDefend[i]);
            *p_AddressOfPatchDefend[i] = (void*)P_StructOfMem->p_NewDefend; 
    }
    
    // патчим адреса со старой структурой св-в
    for(int i = 0; i< sizeof(p_AddressOfPatchOption)/sizeof(void**); i++)
    {        
            ModOffset(&p_AddressOfPatchOption[i]);
            *p_AddressOfPatchOption[i] = (void*)P_StructOfMem->p_NewOption; 
    }
    
    // снятие ограничений с ЕРМ
    for(int i=0; i<sizeof(p_AddressOfERMLimit)/sizeof(void**); i++)
    {
        ModOffset(&p_AddressOfERMLimit[i]);
        *p_AddressOfERMLimit[i] = (void*)(MaxMonstr +1);
    }
    
    for(int i=0; i<sizeof(p_AddressOfERM)/sizeof(void**); i++)
    {
        *p_AddressOfERM[i] = (void*)((int)P_StructOfMem->p_NewMonstrStruct + i_OffERM[i]);
    }
    
    */
    
    // вставляем код вызова процедуры снарядов
    int* p_LimitShell = (int*) 0x43DA95;
    *p_LimitShell = MaxMonstr;
    for(int i = 0; i< sizeof(p_AddressOfShell)/sizeof(void**); i++)
    {        
            ModOffset(&p_AddressOfShell[i]);
            *p_AddressOfShell[i] = (void*)P_StructOfMem->p_NewShell; 
    }
    BYTE* p_Draco = (BYTE*)((int)P_StructOfMem->p_NewShell + 197);
    *p_Draco = 16;
    void* p_ProcForSnaryad = (void*)0x43DAA3;
    LOADDEF TLoadDef;
    TLoadDef.jmp = 0xE9;
    int offset = (int)&InsertShellAsmCode - 0x43DAA3 - 5;
    memcpy(&TLoadDef.Adr[0], &offset, sizeof(int));
    memcpy(p_ProcForSnaryad, &TLoadDef, sizeof(LOADDEF));
    
    // вставляем код вызова процедуры подгрузки портретов
    void* p_ProcForDef = (void*)0x55D102;
    TLoadDef.jmp = 0xE9;
    offset = (int)&InsertCodeDef - 0x55D102 - 5;
    memcpy(&TLoadDef.Adr[0], &offset, sizeof(int));
    memcpy(p_ProcForDef, &TLoadDef, sizeof(LOADDEF));
    
    // вставляем код перехвата Hook47BE90
    void* p_ProcForHook47BE90 = (void*)0x47BE90;
    TLoadDef.jmp = 0xE9;
    offset = (int)&Hook47BE90 - 0x47BE90 - 5;
    memcpy(&TLoadDef.Adr[0], &offset, sizeof(int));
    memcpy(p_ProcForHook47BE90, &TLoadDef, sizeof(LOADDEF));
    
    // вставляем код перехвата Hook47C300
    void* p_ProcForHook47C300 = (void*)0x47C300;
    TLoadDef.jmp = 0xE9;
    offset = (int)&Hook47C300 - 0x47C300 - 5;
    memcpy(&TLoadDef.Adr[0], &offset, sizeof(int));
    memcpy(p_ProcForHook47C300, &TLoadDef, sizeof(LOADDEF)); 
    
    // вставляем код перехвата Hook47C9C0
    void* p_ProcForHook47C9C0 = (void*)0x47C9C0;
    TLoadDef.jmp = 0xE9;
    offset = (int)&Hook47C9C0 - 0x47C9C0 - 5;
    memcpy(&TLoadDef.Adr[0], &offset, sizeof(int));
    memcpy(p_ProcForHook47C9C0, &TLoadDef, sizeof(LOADDEF));
    
    // вставляем код перехвата Hook47CE10
    void* p_ProcForHook47CE10 = (void*)0x47CE10;
    TLoadDef.jmp = 0xE9;
    offset = (int)&Hook47CE10 - 0x47CE10 - 5;
    memcpy(&TLoadDef.Adr[0], &offset, sizeof(int));
    memcpy(p_ProcForHook47CE10, &TLoadDef, sizeof(LOADDEF));
    
    // вставляем код перехвата Hook47D250
    void* p_ProcForHook47D250 = (void*)0x47D250;
    TLoadDef.jmp = 0xE9;
    offset = (int)&Hook47D250 - 0x47D250 - 5;
    memcpy(&TLoadDef.Adr[0], &offset, sizeof(int));
    memcpy(p_ProcForHook47D250, &TLoadDef, sizeof(LOADDEF));
    
    // вставляем код перехвата Hook47D660
    void* p_ProcForHook47D660 = (void*)0x47D660;
    TLoadDef.jmp = 0xE9;
    offset = (int)&Hook47D660 - 0x47D660 - 5;
    memcpy(&TLoadDef.Adr[0], &offset, sizeof(int));
    memcpy(p_ProcForHook47D660, &TLoadDef, sizeof(LOADDEF));

    return TRUE;
}
/*----------------------------------------------------------------------------*/ 
/*----------------------------------------------------------------------------*/
// динамическая подгрузка портретов
extern int i_ExistingMonstr;
DEF TDef;
extern int MaxMonstr;

void LoadPortrets (DEF* TDef)
{ 
     
    // дабавление портретов моснтров в деф cprsmall.def
    char* CprSmall = "cprsmall.def";  

    if (memcmp(&TDef->c_Name[0], CprSmall, sizeof(CprSmall))==0)         
    {
        _LoadPortrets (TDef, TStructOfMem.p_NewSpriteMem, TStructOfMem.p_NewSpriteCprsm, TStructOfMem.p_NewPaleate, 1, 0, 2);

    }
    
    CprSmall  = "twcrport.def";    

    if (memcmp(&TDef->c_Name[0], CprSmall, sizeof(CprSmall))==0)         
    {
        _LoadPortrets (TDef, TStructOfMem.p_NewSpriteMemTWC, TStructOfMem.p_NewSpriteTWC, TStructOfMem.p_NewPaleateTWC, 1, 0, 2);

    }
    
    CprSmall = "avwattak.def";    

    if (memcmp(&TDef->c_Name[0], &CprSmall[0], sizeof(CprSmall))==0)         
    {
        _LoadPortrets (TDef, TStructOfMem.p_NewSpriteMemAVW, TStructOfMem.p_NewSpriteAVW1, TStructOfMem.p_NewPaleateAVW1, 2, 0, 0);
        _LoadPortrets (TDef, TStructOfMem.p_NewSpriteMemAVW, TStructOfMem.p_NewSpriteAVW2, TStructOfMem.p_NewPaleateAVW2, 2, 1, 0);

    }

	return;

}
/*----------------------------------------------------------------------------*/
void _LoadPortrets (DEF* TDef, void* p_SpriteMem, void* p_SpriteStruct, void* p_pal, int n_s, int i_s, int index)
{
        WIN32_FIND_DATA find_data;
        HANDLE find_file;
        int num = 0;
        char buf [128];

            GROUPS* T_Group = *(TDef->T_Group);
            memcpy(p_SpriteMem, T_Group->T_Sprite, 4*(i_ExistingMonstr*n_s+index));
            T_Group->T_Sprite = p_SpriteMem;               
           
            if((find_file = FindFirstFile("DATA_NEW\\MONSTRS\\*.*", &find_data))!=INVALID_HANDLE_VALUE)
            {
            
                while(FindNextFile(find_file,&find_data)!=FALSE)
                {
                    if((num=atoi(&find_data.cFileName[0]))>197)
                    {
                        T_Group->i_Count = MaxMonstr*n_s +index;
                        T_Group->i_Count1 = MaxMonstr*n_s +index;
                        int* p_PortCprSmall = (int*)((int)p_SpriteMem + (num*n_s +index +i_s -1*n_s)*4);
                        void* p_NewPal = (void*)((int)p_pal + (num - i_ExistingMonstr -1)*768);
                        void* p_NewPortCprSmall = (void*)((int)p_SpriteStruct + (num - i_ExistingMonstr -1)*sizeof(SPRITEINMEM));
                        *p_PortCprSmall = (int)p_NewPortCprSmall; 
                    }
                }
            } 
            FindClose(find_file);  
    
} 

/*----------------------------------------------------------------------------*/
char* NameSnaryad [] = {"plcbowx.def", "cprzeax.def", "pelfx.def", "cprgre.def",
                        "pmagex.def", "cprgtix.def", "cprgogx.def", "PLICH.def",  
						"pmedusx.def","porchx.def", "PCYCLBX.def", "pplizax.def", 
						"PiceE.def",  "Phalf.def", "SMCatx.def", "SMBalx.def", "ZShot195.def"};


// динамическая подгрузка снарядов
 void LoadSnar (int NumMonstr, int TypeSnar, char* &p_NameSnar)
{
   
    if(TypeSnar == 0xFF)
    {
        p_NameSnar = (char*)((int)TStructOfMem.p_ShellDefs + (NumMonstr - i_ExistingMonstr +2)*32);
        return;
    }         
                      
    p_NameSnar = NameSnaryad[TypeSnar];
	return;
}
/*----------------------------------------------------------------------------*/  

/*----------------------------------------------------------------------------*/  
void* (__fastcall *GetDevPal)(void* p_DevPalette, int arg1, void* NewPalette, int arg11, int arg2, int arg3, int arg4, int arg5, int arg6) = (void*(__fastcall *)(void*, int, void*, int, int, int, int, int, int))(0x522BC0);

void _Hook (int* p_DevicePal, SPRITEINMEM* p_sprite)
{
     SPRITEINMEM* p_DefSprite = (SPRITEINMEM*)TStructOfMem.p_NewSpriteCprsm;
     SPRITEINMEM* p_DefSpriteTWC = (SPRITEINMEM*)TStructOfMem.p_NewSpriteTWC;
     SPRITEINMEM* p_DefSpriteAVW1 = (SPRITEINMEM*)TStructOfMem.p_NewSpriteAVW1;
     SPRITEINMEM* p_DefSpriteAVW2 = (SPRITEINMEM*)TStructOfMem.p_NewSpriteAVW2;
     
     int* arg1 = (int*)0x69E5DC;
	 int* arg11 = (int*)0x69E5DC;
     int* arg2 = (int*)0x69E5E4;
     int* arg3 = (int*)0x69E5D0;
     int* arg4 = (int*)0x69E5E0;
     int* arg5 = (int*)0x69E5F0;
     int* arg6 = (int*)0x69E5D8;                        

     for(int i=0; i<(MaxMonstr - i_ExistingMonstr); i++)
     {
           if(memcmp(&p_DefSprite->c_Name[0], &p_sprite->c_Name[0], sizeof(p_DefSprite->c_Name))==0)
           {
               void* p_PaleateData = (void*)((int)TStructOfMem.p_NewPaleate + 0x1000*i);
               
               GetDevPal(TStructOfMem.p_DevPalette, *arg1, p_PaleateData, *arg11, *arg2, *arg3, *arg4, *arg5, *arg6);
               *p_DevicePal = (int)TStructOfMem.p_DevPalette;
           }
           
           if(memcmp(&p_DefSpriteTWC->c_Name[0], &p_sprite->c_Name[0], sizeof(p_DefSpriteTWC->c_Name))==0)
           {
               void* p_PaleateData = (void*)((int)TStructOfMem.p_NewPaleateTWC + 0x1000*i);
               
               GetDevPal(TStructOfMem.p_DevPalette, *arg1, p_PaleateData, *arg11, *arg2, *arg3, *arg4, *arg5, *arg6);
               *p_DevicePal = (int)TStructOfMem.p_DevPalette;
           }
           
           if(memcmp(&p_DefSpriteAVW1->c_Name[0], &p_sprite->c_Name[0], sizeof(p_DefSpriteAVW1->c_Name))==0)
           {
               void* p_PaleateData = (void*)((int)TStructOfMem.p_NewPaleateAVW1 + 0x1000*i);
               
               GetDevPal(TStructOfMem.p_DevPalette, *arg1, p_PaleateData, *arg11, *arg2, *arg3, *arg4, *arg5, *arg6);
               *p_DevicePal = (int)TStructOfMem.p_DevPalette;
           }
           
           if(memcmp(&p_DefSpriteAVW2->c_Name[0], &p_sprite->c_Name[0], sizeof(p_DefSpriteAVW2->c_Name))==0)
           {
               void* p_PaleateData = (void*)((int)TStructOfMem.p_NewPaleateAVW2 + 0x1000*i);
               
               GetDevPal(TStructOfMem.p_DevPalette, *arg1, p_PaleateData, *arg11, *arg2, *arg3, *arg4, *arg5, *arg6);
               *p_DevicePal = (int)TStructOfMem.p_DevPalette;
           }
                      
           p_DefSprite += 1;
           p_DefSpriteTWC += 1;
           p_DefSpriteAVW1 += 1;
           p_DefSpriteAVW2 += 1;


     }        

}

void* p_NameSnar;
int jmpAdr = 0x43DB1D;

__declspec (naked) void InsertShellAsmCode (){
	__asm{
		pushad;
		pushfd;
		push offset p_NameSnar;
		push ecx;
		push eax;
		call LoadSnar;
		add esp, 0xC;
		popfd;
		popad;
		mov ecx, p_NameSnar;
		jmp jmpAdr;
	}
}

__declspec (naked) void InsertCodeDef (){
	__asm{
		pushad;
		pushfd;
		push eax;
		call LoadPortrets;
		add esp,4;
		popfd;
		popad;		
		retn;
	}
}

int jmpAdr1 = 0x47C307;

int palette;
int sprclass;

__declspec (naked) void Hook47C300 (){
	__asm{
		mov sprclass, ecx;
		mov palette, esp;
		add palette, 2Ch;
		pushad;
		pushfd;
		push sprclass;
		push palette;
		call _Hook;
		add esp,8;
		popfd;
		popad;
		push ebp;
		mov ebp, esp;
		push ecx;
		mov al, byte ptr [ebp+3Ch];
		jmp jmpAdr1;
	}
}

int jmpAdr3 = 0x47C9C6;

__declspec (naked) void Hook47C9C0 (){
	__asm{
		mov sprclass, ecx;
		mov palette, esp;
		add palette, 2Ch;
		pushad;
		pushfd;
		push sprclass;
		push palette;
		call _Hook;
		add esp,8;
		popfd;
		popad;
		push ebp;
		mov ebp, esp;
		push ecx;
		mov edx, ecx;
		jmp jmpAdr3;
	}
}

int jmpAdr4 = 0x47CE17;

__declspec (naked) void Hook47CE10(){
	__asm{
		mov sprclass, ecx;
		mov palette, esp;
		add palette, 2Ch;
		pushad;
		pushfd;
		push sprclass;
		push palette;
		call _Hook;
		add esp,8;
		popfd;
		popad;
		push ebp;
		mov ebp, esp;
		push ecx;
		mov dl, byte ptr [ebp+38];
		jmp jmpAdr4;
	}
}

int jmpAdr5 = 0x47D257;

__declspec (naked) void Hook47D250(){
	__asm{
		mov sprclass, ecx;
		mov palette, esp;
		add palette, 2Ch;
		pushad;
		pushfd;
		push sprclass;
		push palette;
		call _Hook;
		add esp,8;
		popfd;
		popad;
		push ebp;
		mov ebp, esp;
		push ecx;
		mov dl, byte ptr [ebp+34];
		jmp jmpAdr5;
	}
}

int jmpAdr6 = 0x47D666;

__declspec (naked) void Hook47D660(){
	__asm{
		mov sprclass, ecx;
		mov palette, esp;
		add palette, 2Ch;
		pushad;
		pushfd;
		push sprclass;
		push palette;
		call _Hook;
		add esp,8;
		popfd;
		popad;
		push ebp;
		mov ebp, esp;
		push ecx;
		mov edx, ecx;
		jmp jmpAdr6;
	}
}

int jmpAdr7 = 0x47BE97;

__declspec (naked) void Hook47BE90(){
	__asm{
		mov sprclass, ecx;
		mov palette, esp;
		add palette, 2Ch;
		pushad;
		pushfd;
		push sprclass;
		push palette;
		call _Hook;
		add esp,8;
		popfd;
		popad;
		push ebp;
		mov ebp, esp;
		push ecx;
		mov eax, dword ptr [ecx+24h];
		jmp jmpAdr7;
	}
}

// настройка генератора карт (генерируем новых и ВоГовских монстров)
//MGENERATOR mGenerator [MONNUM];

int gmon;
int nMon;
int NotCheck = 0x541137;
int Check =    0x541116;

__declspec (naked) void Hook54110F(){
	__asm pushad;
	__asm mov gmon, ebx;
	__asm mov eax, dword ptr [edi];
	__asm mov nMon, eax;

	if (nMon == -1){
		__asm popad;
		__asm jmp NotCheck;
	}

	__asm popad;
	__asm lea eax, dword ptr [esi + esi*4];
	__asm jmp Check;

}
/*----------------------------------------------------------------------------*/   
