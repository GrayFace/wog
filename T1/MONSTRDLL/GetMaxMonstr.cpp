#include "dll.h"
#include "..\structs.h"

extern HARPYATACK HarpyStr [MONNUM];

/*---------------------------------------------------------------------------------------------------*/
// проверяем файлы в папке DATA_NEW\MONSTRS и находим максимальный номер монстра
// если меньше 197 - возвращаем 197, чтобы не затереть старых монстров
int GetMaxNumOfLoadingMonstrs()
{
    WIN32_FIND_DATA find_data;
    HANDLE find_file;
    int max = 0;
    char buf [128];
            
            if((find_file = FindFirstFile("DATA_NEW\\MONSTRS\\*.*", &find_data))!=INVALID_HANDLE_VALUE)
            {
            
                while(FindNextFile(find_file,&find_data)!=FALSE)
                {
                     if((max < atoi(&find_data.cFileName[0]))&&(atoi(&find_data.cFileName[0])>197))
                     max = atoi(&find_data.cFileName[0]);
                }
            }
            
            FindClose(find_file);
            
            if (max<197)
               return 197;
                
return max;
}
        
/*-------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------*/
extern int i_ExistingMonstr;
void* (__fastcall *ModDefPal)(void* Palette, void* DefPalette, void* DefPalCopy) =(void* (__fastcall*)(void*, void*, void*))(0x523370);

bool LoadNewMonstrs(STRUCTOFMEM* P_StructOfMem)
{
    WIN32_FIND_DATA find_data;
    HANDLE find_file;
    int numMonstr = 0;
    //char buf [128];
    MEMVIEWOFFILE T_ViewFile;
    // опознование файла
    char SIGNATURE [] = {'M','N','T','R'};

            if((find_file = FindFirstFile("DATA_NEW\\MONSTRS\\*.*", &find_data))!=INVALID_HANDLE_VALUE)
            {            
                while(FindNextFile(find_file,&find_data)!=FALSE)                
                {

                     if((numMonstr = atoi(&find_data.cFileName[0]))>197)
                     {
                            char buf[128] = {'D','A','T','A','_','N','E','W','/','M','O','N','S','T','R','S','/', 0x0};
                            char* FileMonstr = strcat(&buf[0], &find_data.cFileName[0]);      
                            GetMemViewOfFile(FileMonstr, &T_ViewFile);
                            if (memcmp(SIGNATURE, T_ViewFile.MapView, sizeof(SIGNATURE))==0)
                            {
                                STRUCTOFMONSTRINFILE* p_StructMntrInFile = (STRUCTOFMONSTRINFILE*)((int)T_ViewFile.MapView + sizeof(SIGNATURE));
                                CRANIMINFILE* p_CrAnimInFile = (CRANIMINFILE*)((int)p_StructMntrInFile + sizeof(STRUCTOFMONSTRINFILE));
                                FUTURES* p_Futures = (FUTURES*)((int)p_CrAnimInFile + sizeof(CRANIMINFILE));
                                SHELL* p_Shell = (SHELL*)((int)p_Futures + sizeof(FUTURES));
                                SPRITE* p_TWCRPORT = (SPRITE*) ((int)p_Shell + sizeof(SHELL));
                                SPRITE* p_CPRSMALL = p_TWCRPORT + 1;
                                SPRITE* p_AVWATAK1 = p_CPRSMALL + 1;
                                SPRITE* p_AVWATAK2 = p_AVWATAK1 + 1;                              

                                // заполняем строковые константы
                                CHARSTRUCT* p_ChStruct = (CHARSTRUCT*)((int)P_StructOfMem->p_CharStruct + (numMonstr - i_ExistingMonstr-1)*sizeof(CHARSTRUCT));
                                memcpy(&p_ChStruct->c_SoundName[0], &p_StructMntrInFile->c_SoundName[0], sizeof(p_StructMntrInFile->c_SoundName));
                                memcpy(&p_ChStruct->c_DefName[0], &p_StructMntrInFile->c_DefName[0], sizeof(p_StructMntrInFile->c_DefName));
                                memcpy(&p_ChStruct->c_Name[0], &p_StructMntrInFile->c_Name[0], sizeof(p_StructMntrInFile->c_Name));
                                memcpy(&p_ChStruct->c_PluralName[0], &p_StructMntrInFile->c_PluralName[0], sizeof(p_StructMntrInFile->c_PluralName));
                                memcpy(&p_ChStruct->c_Features[0], &p_StructMntrInFile->c_Features[0], sizeof(p_StructMntrInFile->c_Features));
                                STRUCTOFMONSTR* p_StrMonstr = (STRUCTOFMONSTR*)((int)P_StructOfMem->p_NewMonstrStruct + (numMonstr-1)*sizeof(STRUCTOFMONSTR));                                

                                // заполняем в памяте структуру нового монстра                               
                                p_StrMonstr->i_Town = p_StructMntrInFile->i_Town;
                                p_StrMonstr->i_Level = p_StructMntrInFile->i_Level;
                                p_StrMonstr->pc_SoundName = &p_ChStruct->c_SoundName[0];
                                p_StrMonstr->pc_DefName = &p_ChStruct->c_DefName[0];
                                p_StrMonstr->i_Flags = p_StructMntrInFile->i_Flags;                           
                              
                                p_StrMonstr->pc_Name = &p_ChStruct->c_Name[0];
                                memcpy((void*)((int)P_StructOfMem->p_WogMonSName + numMonstr*4), &p_StrMonstr->pc_Name, sizeof(p_StrMonstr->pc_Name)); 
                                p_StrMonstr->pc_PluralName = &p_ChStruct->c_PluralName[0];
                                memcpy((void*)((int)P_StructOfMem->p_WogMonPName + numMonstr*4), &p_StrMonstr->pc_PluralName, sizeof(p_StrMonstr->pc_PluralName));
                                p_StrMonstr->pc_Features = &p_ChStruct->c_Features[0];     
                                memcpy((void*)((int)P_StructOfMem->p_WogMonSpec + numMonstr*4), &p_StrMonstr->pc_Features, sizeof(p_StrMonstr->pc_Features));                           
                                
                                p_StrMonstr->i_CostWood = p_StructMntrInFile->i_CostWood;
                                p_StrMonstr->i_CostMercury = p_StructMntrInFile->i_CostMercury;
                                p_StrMonstr->i_CostOre = p_StructMntrInFile->i_CostOre;
                                p_StrMonstr->i_CostSulfor = p_StructMntrInFile->i_CostSulfor;
                                p_StrMonstr->i_CostCrystal = p_StructMntrInFile->i_CostCrystal;
                                p_StrMonstr->i_CostGems = p_StructMntrInFile->i_CostGems;
                                p_StrMonstr->i_CostGold = p_StructMntrInFile->i_CostGold;
                                p_StrMonstr->i_FightValue = p_StructMntrInFile->i_FightValue;                                
                                p_StrMonstr->i_AiValue = p_StructMntrInFile->i_AiValue;
                                p_StrMonstr->i_Growth = p_StructMntrInFile->i_Growth;
                                p_StrMonstr->i_HordeGrowth = p_StructMntrInFile->i_HordeGrowth;
                                p_StrMonstr->i_HitPoints = p_StructMntrInFile->i_HitPoints;
                                p_StrMonstr->i_Speed = p_StructMntrInFile->i_Speed;
                                p_StrMonstr->i_Attack = p_StructMntrInFile->i_Attack;
                                p_StrMonstr->i_Defence = p_StructMntrInFile->i_Defence;
                                p_StrMonstr->i_DamageLow = p_StructMntrInFile->i_DamageLow;
                                p_StrMonstr->i_DamageHigh = p_StructMntrInFile->i_DamageHigh;
                                p_StrMonstr->i_Shots = p_StructMntrInFile->i_Shots;
                                p_StrMonstr->i_Spells = p_StructMntrInFile->i_Spells;
                                p_StrMonstr->i_AdvLow = p_StructMntrInFile->i_AdvLow;
                                p_StrMonstr->i_AdvHigh = p_StructMntrInFile->i_AdvHigh;
                                
                                // заполняем структуру CrAnim
                                CRANIM* p_CrAnim = (CRANIM*)((int)P_StructOfMem->p_NewCrAnim + (numMonstr-1)*sizeof(CRANIM));
                                memcpy(p_CrAnim, p_CrAnimInFile, sizeof(CRANIM)); 
                                
                                // заполняем структуры св-в монстров                             
                                BYTE* p_Attack = (BYTE*)((int)P_StructOfMem->p_NewAtack + numMonstr - 1 -0x16);
                                *p_Attack = p_Futures->b_atack_fut;
                                
                                BYTE* p_Defend = (BYTE*) ((int)P_StructOfMem->p_NewDefend + numMonstr - 1 -0x3F);
                                *p_Defend = p_Futures->b_defend_fut;
                                
                                //BYTE* p_Option = (BYTE*) ((int)P_StructOfMem->p_NewOption + numMonstr - 1);
                                //*p_Option = p_Futures->b_option_fut;
								
								// атака с возвратом
								HarpyStr[numMonstr-1].IsHave = p_Futures->garpy;

								// сопротивление магии
								memcpy(&MonResStr[numMonstr-1], &p_Futures->StrResist, sizeof(RESISTSTRUCT));

								// магия
								memcpy(&StrCastMag[numMonstr-1], &p_Futures->StrMagicCast, sizeof(CASTMAGIC));

								if (p_StructMntrInFile->i_Spells > 0)
								{
									 MoJumper1[numMonstr-1-13] = 0x09;
									 MagHint[numMonstr-1-13] = 0x0A;
									 MoJumper2[numMonstr-1-13] = 0x0A;
									 AIMagicJumper[numMonstr-1-13] = 0x04;
								}
								else
								{
									 MoJumper1[numMonstr-1-13] = 0x08;
									 MagHint[numMonstr-1-13] = 0x09;
									 MoJumper2[numMonstr-1-13] = 0x09;
									 AIMagicJumper[numMonstr-1-13] = 0x03;
								}
                                
                                // запоняем струтуры снарядов
                                BYTE* p_ShellInMem = (BYTE*) ((int)P_StructOfMem->p_NewShell + numMonstr - 1);
                                *p_ShellInMem = p_Shell->b_type_of_shell;
                                
                                void* p_DefShell = (void*)((int)P_StructOfMem->p_ShellDefs + (numMonstr - i_ExistingMonstr-1)*32);
                                memcpy(p_DefShell, &p_Shell->c_name_def_of_shell[0], sizeof(p_Shell->c_name_def_of_shell));
                                
                                // зполняем структуру данных спрайтов cprport 
                                void* NewPalette = calloc(0x1000, 1); 
                                // подгружаем спрайт cprsmall                               
                                _LoadSprite (P_StructOfMem->p_NewCadreCprsm, P_StructOfMem->p_NewPaleate, P_StructOfMem->p_NewSpriteCprsm, p_CPRSMALL, NewPalette, numMonstr);
                                // подгружаем спрайт twcrport
                                _LoadSprite (P_StructOfMem->p_NewCadreTWC, P_StructOfMem->p_NewPaleateTWC, P_StructOfMem->p_NewSpriteTWC, p_TWCRPORT, NewPalette, numMonstr);
                                // подгружаем спрайт avwatak1 
                                _LoadSprite (P_StructOfMem->p_NewCadreAVW1, P_StructOfMem->p_NewPaleateAVW1, P_StructOfMem->p_NewSpriteAVW1, p_AVWATAK1, NewPalette, numMonstr);
                                // подгружаем спрайт avwatak2
                                _LoadSprite (P_StructOfMem->p_NewCadreAVW2, P_StructOfMem->p_NewPaleateAVW2, P_StructOfMem->p_NewSpriteAVW2, p_AVWATAK2, NewPalette, numMonstr);
                                free(NewPalette);                                                          

                            }   
							CleanMemView(&T_ViewFile);
                     } 
					 
                }
            }
            FindClose(find_file);

			return TRUE;
}
//-------------------------------------------------------------------------------------------------------------
void _LoadSprite (void* p_cadre, void* p_palette, void* p_spr, SPRITE* p_sprite, void* p_bufpal, int numMonstr)
{
     void* p_CadreDataS = (void*)((int)p_cadre + (numMonstr - i_ExistingMonstr -1)*sizeof(p_sprite->b_CadreData));
     memcpy(p_CadreDataS, p_sprite->b_CadreData, sizeof(p_sprite->b_CadreData));                                
     void* p_PaleateData = (void*)((int)p_palette  + (numMonstr - i_ExistingMonstr -1)*0x1000);
     memcpy(p_PaleateData, p_sprite->b_Paleate, sizeof(p_sprite->b_Paleate));
     ModDefPal(p_bufpal, p_PaleateData, p_PaleateData);
     memcpy(p_PaleateData, p_bufpal, 0x1000);
     SPRITEINMEM* p_SpriteMem = (SPRITEINMEM*)p_spr + (numMonstr - i_ExistingMonstr-1);
     p_SpriteMem->p_Class = (void*)0x63D6BC;                             
     memcpy(&p_SpriteMem->c_Name[0], &p_sprite->c_Name[0], sizeof(p_SpriteMem->c_Name));
     p_SpriteMem->i_CadreSize = p_sprite->i_CadreSize;
     p_SpriteMem->i_CompressionType = p_sprite->i_CompressionType;
     p_SpriteMem->i_DataSize = p_sprite->i_DataSize; 
     p_SpriteMem->i_FrameHeight = p_sprite->i_FrameHeight;
     p_SpriteMem->i_FrameLeft = p_sprite->i_FrameLeft;
     p_SpriteMem->i_FrameTop = p_sprite->i_FrameTop;
     p_SpriteMem->i_FrameWidth = p_sprite->i_FrameWidth;
     p_SpriteMem->i_Height = p_sprite->i_Height;
     p_SpriteMem->i_RefCount = p_sprite->i_RefCount;
     p_SpriteMem->i_Unk = p_sprite->i_Unk;
     p_SpriteMem->i_Unk1 = p_sprite->i_Unk1;
     p_SpriteMem->i_Unk2 = p_sprite->i_Unk2;
     p_SpriteMem->i_Width = p_sprite->i_Width;                                
     p_SpriteMem->pv_CadreData = p_CadreDataS;
}

int GetMaxGenerationMonstr (){

	WIN32_FIND_DATA find_data;
    HANDLE find_file;
    int max = 0;
	int numMonstr;
    char buf [128];
        MEMVIEWOFFILE T_ViewFile;
    // опознование файла
    char SIGNATURE [] = {'M','N','T','R'};

            if((find_file = FindFirstFile("DATA_NEW\\MONSTRS\\*.*", &find_data))!=INVALID_HANDLE_VALUE)
            {            
                while(FindNextFile(find_file,&find_data)!=FALSE)                
                {

                     if((numMonstr = atoi(&find_data.cFileName[0]))>197)
                     {
                            char buf[128] = {'D','A','T','A','_','N','E','W','/','M','O','N','S','T','R','S','/', 0x0};
                            char* FileMonstr = strcat(&buf[0], &find_data.cFileName[0]);      
                            GetMemViewOfFile(FileMonstr, &T_ViewFile);
                            if (memcmp(SIGNATURE, T_ViewFile.MapView, sizeof(SIGNATURE))==0)
                            {
                                STRUCTOFMONSTRINFILE* p_StructMntrInFile = (STRUCTOFMONSTRINFILE*)((int)T_ViewFile.MapView + sizeof(SIGNATURE));
                                CRANIMINFILE* p_CrAnimInFile = (CRANIMINFILE*)((int)p_StructMntrInFile + sizeof(STRUCTOFMONSTRINFILE));
                                FUTURES* p_Futures = (FUTURES*)((int)p_CrAnimInFile + sizeof(CRANIMINFILE));

								if ((p_Futures->random == TRUE)&&(max < numMonstr))
									max = numMonstr;								
									
							}
							CleanMemView(&T_ViewFile);
					 }
				}
			}

            FindClose(find_file);
            
            if (max<197)
               return 197;
                
return max;

}

