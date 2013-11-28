/* структура в памяти строковых констант из структуры монстра */
struct CHARSTRUCT
{
       char  c_SoundName [5];     /* первые четыре буквы озвучки */
       char  c_DefName [512];     /* деф монстра */
       char  c_Name [128];        /* имя монстра */
       char  c_PluralName [128];  /* множественное имя монстра */
       char  c_Features [512];   /* описание способностей */    
};

struct STRUCTOFMEM
{  
    void* p_NewCrAnim;       /* указатель на новую структуру CrAnim */
    void* p_NewAtack;        /* указатель на новую структуру свойств моснтров при атаке */
    void* p_NewDefend;       /* указатель на новую структуру свойств монстров при защите */
    void* p_NewMonstrStruct; /* указатель на новую структуру монстров */
    void* p_NewOption;       /* указатель на новую структуру ... свойств монстров */
    void* p_CharStruct;      /* указатель на память содержащую строки из структуры монстров */
    void* p_NewShell;        // указатель на новую структуру снарядов
    void* p_ShellDefs;       // указатель на названия дефов снарядов новых монстров
    
    void* p_WogMonSName;     // указатель на имя одного в ВоГе
    void* p_WogMonPName;     // указатель на имя многих в ВоГе
    void* p_WogMonSpec;      // указаетль на описание св-в монстра
    
    void* p_NewSpriteCprsm;  // указатель на структуру кадра
    void* p_NewSpriteMem;    // указатель в коде
    void* p_NewCadreCprsm;   // указатель на структуру спрайта
    void* p_NewPaleate;      // указатель на палитру спрайтов 
    
    void* p_NewSpriteTWC;  // указатель на структуру кадра
    void* p_NewSpriteMemTWC;    // указатель в коде
    void* p_NewCadreTWC;   // указатель на структуру спрайта
    void* p_NewPaleateTWC;      // указатель на палитру спрайтов 
    
    void* p_NewSpriteMemAVW; 
    
    void* p_NewSpriteAVW1;  // указатель на структуру кадра
    void* p_NewCadreAVW1;   // указатель на структуру спрайта
    void* p_NewPaleateAVW1;      // указатель на палитру спрайтов 
    
    void* p_NewSpriteAVW2;  // указатель на структуру кадра
    void* p_NewCadreAVW2;   // указатель на структуру спрайта
    void* p_NewPaleateAVW2;      // указатель на палитру спрайтов
    
    void* p_DevPalette;      // указатель на подменяемую девайс палитру
    
};

extern STRUCTOFMEM TStructOfMem;

struct STRUCTOFMONSTR
{
       int   i_Town;        /* номер города к которому принадлежит монстр*/
       int   i_Level;       /* уровень монстра в замке */
       char* pc_SoundName;   /* первые четыре буквы озвучки */
       char* pc_DefName;     /* указатель на деф монстра */
       int   i_Flags;       /* флаги монстра */
       char* pc_Name;        /* указатель на имя монстра */
       char* pc_PluralName;  /* указатель на множественное имя монстра */
       char* pc_Features;    /* указатель на описание способностей */
       int   i_CostWood;    /* стоимость монстра - дерево */ 
       int   i_CostMercury; /* ... ртуть */
       int   i_CostOre;     /* ... камень */ 
       int   i_CostSulfor;  /* ... сера */
       int   i_CostCrystal; /* ... кристалы */
       int   i_CostGems;    /* ... джемы */
       int   i_CostGold;    /* ... золото */
       int   i_FightValue;  
       int   i_AiValue;
       int   i_Growth;      /* прирост */
       int   i_HordeGrowth;
       int   i_HitPoints;   /* энергия */
       int   i_Speed;       /* скорость */
       int   i_Attack;      /* атака */
       int   i_Defence;     /* защита */
       int   i_DamageLow;   /* урон минимальный */
       int   i_DamageHigh;  /* урон максимальный */
       int   i_Shots;       /* кол-во выстрелов */
       int   i_Spells;      /* кол-во манны */
       int   i_AdvLow;      
       int   i_AdvHigh;
}; 

/* структура cranim в файле */
struct CRANIM
{
    WORD w_anim1;
    WORD w_anim2;
    WORD w_anim3;
    WORD w_anim4;
    WORD w_anim5;
    WORD w_anim6;
    float f_anim7;
    float f_anim8;
    float f_anim9;
    float f_anim10;
    float f_anim11;
    float f_anim12;
    float f_anim13;
    float f_anim14;
    float f_anim15;
    float f_anim16;
    float f_anim17;
    float f_anim18;
    int i_anim19;
    int i_anim20;
    int i_anim21;
    int i_anim22;
    int i_anim23;
    int i_anim24;
    
};

struct LOADDEF
{
    BYTE jmp;
    char Adr [4];    
};

struct SPRITEINMEM
{
    void* p_Class;
    char c_Name[12];
    int i_Unk;
    int i_Unk1;
    int i_RefCount;
    int i_DataSize;
    int i_CadreSize;
    int i_CompressionType;
    int i_Width;
    int i_Height;
    int i_FrameWidth;
    int i_FrameHeight;
    int i_FrameLeft;
    int i_FrameTop;
    int i_Unk2;
    void* pv_CadreData;
};

struct GROUPS
{
    int i_Count;
    int i_Count1;
    void* T_Sprite;
};

struct DEF
{
    void* p_Class;
    char c_Name [16];
    int i_Type;
    int i_RefCount;
    GROUPS** T_Group;
    void* p_PaleteDev;
    void* p_Palette;
    int i_GroupsCount;
    void* p_ActiveGroup;
    int i_Width;
    int i_Height;    
};

bool AllocateMemmory(int MaxMonstr, STRUCTOFMEM* p_StructOfMem);
bool CopyOldMonstrsStruct(STRUCTOFMEM* P_StructOfMem);
bool PatchExe (int MaxMonstr, STRUCTOFMEM* P_StructOfMem);
bool LoadNewMonstrs(STRUCTOFMEM* P_StructOfMem);
void LoadSnar (int NumMonstr, int TypeSnar, char* &p_NameSnar);
void LoadPortrets (DEF* TDef);
void _Hook (int* p_DevicePal, SPRITEINMEM* p_sprite);