//include only one and only for global.cpp

//queue=-2 reserved to town mode
//queue=-3 reserved to 32 bit mode
//queue=-200 reserved to map mode

__newCallers newCallers[]=
{
	{0x4F8171,long(newMainProc),H_ADRO|H_RADR},//Replace MainProc
	{0x601ACC,long(px_DirectDrawCreate),H_CALL,-3}, //DX Proxy Run
	//{0x55479B,long(HookCmdLineEnteredText),H_CALL5},  // bad hook code, causes crash in multiplayer
	//{0x602181, (long)&OnChangeMode, H_CALL5},
	//{0x601AA0, (long)&OnChangeMode, H_JMP5},

/*
	//Towns
	{0x5C6E43,long(&TownsBackGroundPrefix),H_ADRO|H_RADR,0},//TownsBackGroundPrefix string array
	{0x5C6EE9+3,long(&TownsBuildingsDefs),H_ADRO|H_RADR,0},//TownsBuildingsDefs string array
	{0x484162,long(&TownsBuildingsIcons),H_ADRO|H_RADR,0},//TownsBuildingsIcons string array
	{0x5C3393+3,long(&TownsBuildingsSelection),H_ADRO|H_RADR,0},//TownsBuildingsSelection string array
	{0x460DE2+3,long(&TownsBuildingsTree),H_ADRO|H_RADR,0},//TownsBuildingsTree char array
	{0x5C32E6+3,long(&TownsBuildingsCoord)+1*sizeof(short),H_ADRO|H_RADR,0},//TownsBuildingsCoord+1*sizeof(short) short array
	{0x5C32F3+3,long(&TownsBuildingsCoord)+2*sizeof(short),H_ADRO|H_RADR,0},//TownsBuildingsCoord+2*sizeof(short) short array
	{0x0042B281+3,long(&TownsBuildingsDep),H_ADRO|H_RADR,0},//TownsBuildingsDep long array
	{0x004EB84F+3,long(&TownsBuildingsDep),H_ADRO|H_RADR,0},//TownsBuildingsDep long array
	{0x004EB882+3,long(&TownsBuildingsDep),H_ADRO|H_RADR,0},//TownsBuildingsDep long array
	{0x005C11D7+3,long(&TownsBuildingsDep),H_ADRO|H_RADR,0},//TownsBuildingsDep long array
	{0x005C12E5+3,long(&TownsBuildingsDep),H_ADRO|H_RADR,0},//TownsBuildingsDep long array
	{0x005C1377+3,long(&TownsBuildingsDep),H_ADRO|H_RADR,0},//TownsBuildingsDep long array
	{0x005D6015+3,long(&TownsBuildingsDep),H_ADRO|H_RADR,0},//TownsBuildingsDep long array
	{0x0042B290+3,long(&TownsBuildingsDep)+1*sizeof(long),H_ADRO|H_RADR,0},//TownsBuildingsDep+1*sizeof(long) long array
	{0x004EB88B+3,long(&TownsBuildingsDep)+1*sizeof(long),H_ADRO|H_RADR,0},//TownsBuildingsDep+1*sizeof(long) long array
	{0x005C11DE+3,long(&TownsBuildingsDep)+1*sizeof(long),H_ADRO|H_RADR,0},//TownsBuildingsDep+1*sizeof(long) long array
	{0x005C12EC+3,long(&TownsBuildingsDep)+1*sizeof(long),H_ADRO|H_RADR,0},//TownsBuildingsDep+1*sizeof(long) long array
	{0x005C137E+3,long(&TownsBuildingsDep)+1*sizeof(long),H_ADRO|H_RADR,0},//TownsBuildingsDep+1*sizeof(long) long array
	{0x005D601C+3,long(&TownsBuildingsDep)+1*sizeof(long),H_ADRO|H_RADR,0},//TownsBuildingsDep+1*sizeof(long) long array
	{0x005C6EB0+4,long(&TownsBuildingsDrawQueue),H_ADRO|H_RADR,0},//TownsBuildingsDrawQueue char array
	{0x005C6FE4+4,long(&TownsBuildingsDrawQueue),H_ADRO|H_RADR,0},//TownsBuildingsDrawQueue char array
	{0x005C6FE4+4,long(&TownsBuildingsDrawQueue),H_ADRO|H_RADR,0},//TownsBuildingsDrawQueue char array
	{0x00484324+3,long(&TownsBuildingsInd),H_ADRO|H_RADR,0},//TownsBuildingsInd long array
	{0x005C0320+3,long(&TownsBuildingsInd),H_ADRO|H_RADR,0},//TownsBuildingsInd long array
	{0x005C0E20+3,long(&TownsBuildingsInd),H_ADRO|H_RADR,0},//TownsBuildingsInd long array
	{0x005C0EFA+3,long(&TownsBuildingsInd),H_ADRO|H_RADR,0},//TownsBuildingsInd long array
	{0x00460DD1+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array
	{0x00461038+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array
	{0x0046131D+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array
	{0x004613AE+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array
	{0x004613CC+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array
	{0x0046140F+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array
	{0x0046142B+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array
	{0x004614C2+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array
	{0x004614DE+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array
	{0x0046162D+2,long(&TownsBuildingsSpecNum),H_ADRO|H_RADR,0},//TownsBuildingsSpecNum char array

		//// 0046104D  83F908                                                    		cmp	ecx,00000008h




	{0x005BF845+3,long(&TownsBuildingsCost0),H_ADRO|H_RADR,0},//TownsBuildingsCost0 long array
	{0x005C14D9+3,long(&TownsBuildingsCost0),H_ADRO|H_RADR,0},//TownsBuildingsCost0 long array
	{0x005C1566+3,long(&TownsBuildingsCost0),H_ADRO|H_RADR,0},//TownsBuildingsCost0 long array
	{0x005C15DA+3,long(&TownsBuildingsCost0),H_ADRO|H_RADR,0},//TownsBuildingsCost0 long array

 //005BF845  8D3C9580986A00                                            		lea	edi,[L006A9880+edx*4]
 //005C14D9  8D048D80986A00                                            		lea	eax,[L006A9880+ecx*4]
 //005C1566  8D348D80986A00                                            		lea	esi,[L006A9880+ecx*4]
 //005C15DA  8D348D80986A00                                            		lea	esi,[L006A9880+ecx*4]

 	{0x005BF826+3,long(&TownsBuildingsCost1),H_ADRO|H_RADR,0},//TownsBuildingsCost1 long array
	{0x005C14B8+3,long(&TownsBuildingsCost1),H_ADRO|H_RADR,0},//TownsBuildingsCost1 long array
	{0x005C1539+3,long(&TownsBuildingsCost1),H_ADRO|H_RADR,0},//TownsBuildingsCost1 long array
	{0x005C15BB+3,long(&TownsBuildingsCost1),H_ADRO|H_RADR,0},//TownsBuildingsCost1 long array
	//{0x005C19FF+2,long(&TownsBuildingsCost1),H_ADRO|H_RADR,0},//TownsBuildingsCost1 long array
// 005BF826  8D3C9544836A00                                            		lea	edi,[L006A8344+edx*4]
// 005C14B8  8D048D44836A00                                            		lea	eax,[L006A8344+ecx*4]
// 005C1539  8D348D44836A00                                            		lea	esi,[L006A8344+ecx*4]
// 005C15BB  8D348D44836A00                                            		lea	esi,[L006A8344+ecx*4]
// 005C19FF  81FE44836A00                                              		cmp	esi,L006A8344



//extern long TownsBuildingsCost0[];
//extern long TownsBuildingsCost1[];

	{0x5C70F5,long(&Towns1),H_ADRO|H_RADR,0},//Towns1 string array
	{0x5D5B21,long(&TownsHallDefs),H_ADRO|H_RADR,0},//TownsHallDefs string array
	{0x5D9A6D,long(&TownsHallDefs),H_ADRO|H_RADR,0},//TownsHallDefs string array
	{0x407126,long(&TownsHalls),H_ADRO|H_RADR,0},//TownsHalls string array
	{0x40716A,long(&TownsHalls),H_ADRO|H_RADR,0},//TownsHalls string array
	{0x407193,long(&TownsHalls),H_ADRO|H_RADR,0},//TownsHalls string array
	{0x5CA90B+3,long(&TownsHallLoading),H_ADRO|H_RADR,0},//TownsHallLoading long array
	{0x461056+3,long(&TownsHallBuilding),H_ADRO|H_RADR,0},//TownsHallBilding long array


	{0x4C9820+3,long(&TownsMapObject0),H_ADRO|H_RADR,0},//TownsMapObject0 string array
	{0x4C980D+3,long(&TownsMapObject1),H_ADRO|H_RADR,0},//TownsMapObject1 string array
	{0x4C97C1+3,long(&TownsMapObject2),H_ADRO|H_RADR,0},//TownsMapObject2 string array
	{0x54A4AA+2,long(&Towns2),H_ADRO|H_RADR,0},//Towns2 string array
	{0x44964D+3,long(&TownsBackGroundCreatures0),H_ADRO|H_RADR,0},//TownsBackGroundCreatures0 string array
	{0x55000F+3,long(&TownsBackGroundCreatures0),H_ADRO|H_RADR,0},//TownsBackGroundCreatures0 string array
	{0x551E35+3,long(&TownsBackGroundCreatures0),H_ADRO|H_RADR,0},//TownsBackGroundCreatures0 string array
	{0x5F5450+3,long(&TownsBackGroundCreatures0),H_ADRO|H_RADR,0},//TownsBackGroundCreatures0 string array
	{0x5D90A4+3,long(&TownsBackGroundCreatures1),H_ADRO|H_RADR,0},//TownsBackGroundCreatures1 string array
	{0x5D9356+3,long(&TownsBackGroundCreatures1),H_ADRO|H_RADR,0},//TownsBackGroundCreatures1 string array
	{0x428602+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x428964+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x429BB1+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x429DEC+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x429F32+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x42A026+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x42B538+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x42B5D9+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x42B5F3+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x42B724+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x42BE42+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x42CF07+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x42D241+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x432E94+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x432F5F+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x43363B+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x47AA7F+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x47AA90+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x47AB00+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x47AB11+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x47AB80+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x47AB91+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x4BF308+2,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x4C8D2D+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x503290+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x51CFD8+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x525AAD+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x52A31B+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5519A7+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x551B68+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x576455+2,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5BE383+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5BE3AB+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5BEF9E+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5BFC66+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5BFFDF+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C0098+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C0203+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C0264+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C057E+4,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C0B34+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C0BEC+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C6023+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C7196+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C7CE5+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5C7D1E+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5D9DE4+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5D9E5D+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5D9ED3+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5D9F4C+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5D9FC5+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5DA03E+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5DA0C2+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5DA1BA+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5DD099+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5DD96B+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5DDAD6+3,long(&TownsCreaturesTree),H_ADRO|H_RADR,0},//TownsCreaturesTree string array
	{0x5765AA+2,long(&TownsCreaturesTree)+3*sizeof(long),H_ADRO|H_RADR,0},//TownsCreaturesTree+3*sizeof(long) long array
	{0x47AB33+3,long(&TownsCreaturesTree)+7*sizeof(long),H_ADRO|H_RADR,0},//TownsCreaturesTree+7*sizeof(long) long array
	{0x4BF302+2,long(&TownsCreaturesTree)+7*sizeof(long),H_ADRO|H_RADR,0},//TownsCreaturesTree+7*sizeof(long) long array
	{0x525A8B+3,long(&TownsCreaturesTree)+7*sizeof(long),H_ADRO|H_RADR,0},//TownsCreaturesTree+7*sizeof(long) long array
	{0x5C0527+4,long(&TownsCreaturesTree)+7*sizeof(long),H_ADRO|H_RADR,0},//TownsCreaturesTree+7*sizeof(long) long array

	{0x5CCED7+3,long(&TownsBackGroundMage),H_ADRO|H_RADR,0},//TownsBackGroundMage string array
	{0x4642C8+3,long(&TownsBackGroundBattle),H_ADRO|H_RADR,0},//TownsBackGroundBattle string array
	{0x52C9CA+3,long(&TownsPuzzle),H_ADRO|H_RADR,0},//TownsPuzzle string array
	{0x52CF59+3,long(&TownsPuzzle),H_ADRO|H_RADR,0},//TownsPuzzle string array

	{0x5D17AF+3,long(&TownsBlackSmithCreatures),H_ADRO|H_RADR,0},//TownsBlackSmithCreatures long array
	{0x5D18C1+3,long(&TownsBlackSmithCreatures),H_ADRO|H_RADR,0},//TownsBlackSmithCreatures long array
	{0x5D1B02+3,long(&TownsBlackSmithCreatures),H_ADRO|H_RADR,0},//TownsBlackSmithCreatures long array
	{0x5D1FA1+3,long(&TownsBlackSmithCreatures),H_ADRO|H_RADR,0},//TownsBlackSmithCreatures long array
	{0x5D1FD3+3,long(&TownsBlackSmithCreatures),H_ADRO|H_RADR,0},//TownsBlackSmithCreatures long array
	{0x5D224C+3,long(&TownsBlackSmithCreatures),H_ADRO|H_RADR,0},//TownsBlackSmithCreatures long array
	{0x5D2289+3,long(&TownsBlackSmithCreatures),H_ADRO|H_RADR,0},//TownsBlackSmithCreatures long array
	{0x525DE0+3,long(&TownsBlackSmithArtifacts),H_ADRO|H_RADR,0},//TownsBlackSmithArtifacts long array
	{0x5D1D89+3,long(&TownsBlackSmithArtifacts),H_ADRO|H_RADR,0},//TownsBlackSmithArtifacts long array
	{0x5D1EBE+3,long(&TownsBlackSmithArtifacts),H_ADRO|H_RADR,0},//TownsBlackSmithArtifacts long array
	{0x5D223F+3,long(&TownsBlackSmithArtifacts),H_ADRO|H_RADR,0},//TownsBlackSmithArtifacts long array
	{0x5D1ECB+3,long(&TownsBlackSmithArtifacts)+1*sizeof(long),H_ADRO|H_RADR,0},//TownsBlackSmithArtifacts+1*sizeof(long) long array


	{0x40C9A8+3,long(TOWNTYPES),H_ADRO|H_RADR,-2},//TownsTypesTxt string array
	{0x5697F7+3,long(TOWNTYPES),H_ADRO|H_RADR,-2},//TownsTypesTxt string array
	{0x57635A+3,long(TOWNTYPES),H_ADRO|H_RADR,-2},//TownsTypesTxt string array
	{0x58D95A+3,long(TOWNTYPES),H_ADRO|H_RADR,-2},//TownsTypesTxt string array
	{0x58DD24+3,long(TOWNTYPES),H_ADRO|H_RADR,-2},//TownsTypesTxt string array
	{0x5C1854+3,long(TOWNTYPES),H_ADRO|H_RADR,-2},//TownsTypesTxt string array
	{0x5D2E64+3,long(BLKSMTH),H_ADRO|H_RADR,-2},//TownsBlackSmithDesc string array


//{0x4CE132,long(FormTownBuildDepends),H_CALL,0},//FormTownBuildDepends sub
//004CE132  E8D9D60100                                                		call	SUB_L004EB810


// 00461056  FF248D68164600                     jmp	[CASE_PROCTABLE_00461668+ecx*4]//TownsHallBuilding long array
// 00460DE2  8A944198CF6600                     mov	dl,[ecx+eax*2+L0066CF98]//TownsBuildingsTree
// 005C32E6  0FBF880EAA6800                     movsx	ecx,[eax+L0068AA0E]//TownsBuildingsCoord+1*sizeof(short) short array
// 005C32F3  0FBF9010AA6800                     movsx	edx,[eax+L0068AA10]//TownsBuildingsCoord+2*sizeof(short) short array
// 0042B281  8B04D5E8776900                     mov	eax,[L006977E8+edx*8]//TownsBuildingsDep long array
// 004EB84F  8D04C5E8776900                     lea	eax,[L006977E8+eax*8]//TownsBuildingsDep long array
// 004EB882  8B34CDE8776900                     mov	esi,[L006977E8+ecx*8]//TownsBuildingsDep long array
// 005C11D7  8B34D5E8776900                     mov	esi,[L006977E8+edx*8]//TownsBuildingsDep long array
// 005C12E5  8B14CDE8776900                     mov	edx,[L006977E8+ecx*8]//TownsBuildingsDep long array
// 005C1377  8B3CC5E8776900                     mov	edi,[L006977E8+eax*8]//TownsBuildingsDep long array
// 005D6015  8B3CCDE8776900                     mov	edi,[L006977E8+ecx*8]//TownsBuildingsDep long array
// 0042B290  8B14D5EC776900                     mov	edx,[L006977EC+edx*8]//TownsBuildingsDep+1*sizeof(long) long array
// 004EB88B  8B0CCDEC776900                     mov	ecx,[L006977EC+ecx*8]//TownsBuildingsDep+1*sizeof(long) long array
// 005C11DE  8B04D5EC776900                     mov	eax,[L006977EC+edx*8]//TownsBuildingsDep+1*sizeof(long) long array
// 005C12EC  8B3CCDEC776900                     mov	edi,[L006977EC+ecx*8]//TownsBuildingsDep+1*sizeof(long) long array
// 005C137E  8B04C5EC776900                     mov	eax,[L006977EC+eax*8]//TownsBuildingsDep+1*sizeof(long) long array
// 005D601C  8B0CCDEC776900                     mov	ecx,[L006977EC+ecx*8]//TownsBuildingsDep+1*sizeof(long) long array
// 005C6EB0  0FBEB497C42E6400                   movsx	esi,[edi+edx*4+L00642EC4]//Towns3 char array
// 005C6FE4  0FBEB487C42E6400                   movsx	esi,[edi+eax*4+L00642EC4]//Towns3 char array

// 005C3393  8B1C9DDCA36800              		mov	ebx,[L0068A3DC+ebx*4]
// 005C6EE9  8B0C8D74306400              		mov	ecx,[L00643074+ecx*4]
// 005C70F2  8B0495CC366400              		mov	eax,[L006436CC+edx*4]
// 0048415F  8B0495B8556700              		mov	eax,[L006755B8+edx*4]
// 005D5B1E  8B148DD4F56700              		mov	edx,[L0067F5D4+ecx*4]
// 005D9A6A  8B3C85D4F56700              		mov	edi,[L0067F5D4+eax*4]
// 00407123  8B3C85C4F46500                     mov	edi,[L0065F4C4+eax*4]
// 00407167  8B0CB5C4F46500                     mov	ecx,[L0065F4C4+esi*4]
// 00407190  8B0C85C4F46500                     mov	ecx,[L0065F4C4+eax*4]

// 005CA90B  FF2485ACCC5C00              		jmp	[CASE_PROCTABLE_005CCCAC+eax*4]
// 004C9820  8B3C8D0C7A6700              		mov	edi,[L00677A0C+ecx*4]//TownsMapObject0
// 004C980D  8B3C8D307A6700              		mov	edi,[L00677A30+ecx*4]//TownsMapObject1
// 004C97C1  8B3C85547A6700              		mov	edi,[L00677A54+eax*4]//TownsMapObject2
// 0054A4AA  8BB87C276800                		mov	edi,[eax+L0068277C]//Towns2
// 0044964D  8B3C8560296800              		mov	edi,[L00682960+eax*4]//TownsBackGroundCreatures0
// 0055000F  8B0C8D60296800              		mov	ecx,[L00682960+ecx*4]//TownsBackGroundCreatures0
// 00551E35  8B0C8D60296800              		mov	ecx,[L00682960+ecx*4]//TownsBackGroundCreatures0
// 005F5450  8B148D60296800              		mov	edx,[L00682960+ecx*4]//TownsBackGroundCreatures0
// 005D90A4  8B3C8548A36800              		mov	edi,[L0068A348+eax*4]//TownsBackGroundCreatures1
// 005D9356  8B3C8548A36800              		mov	edi,[L0068A348+eax*4]//TownsBackGroundCreatures1
// 00484324  8B148510896800                     mov	edx,[L00688910+eax*4]//TownsBuildingsInd long array
// 005C0320  8B049510896800                     mov	eax,[L00688910+edx*4]//TownsBuildingsInd long array
// 005C0E20  8B0C8510896800                     mov	ecx,[L00688910+eax*4]//TownsBuildingsInd long array
// 005C0EFA  8B0C8D10896800                     mov	ecx,[L00688910+ecx*4]//TownsBuildingsInd long array
// 00460DD1  8A903CD06600                       mov	dl,[eax+L0066D03C]//TownsBuildingsSpecNum char array
// 00461038  8A903CD06600                       mov	dl,[eax+L0066D03C]//TownsBuildingsSpecNum char array
// 0046131D  8A823CD06600                       mov	al,[edx+L0066D03C]//TownsBuildingsSpecNum char array
// 004613AE  8A823CD06600                       mov	al,[edx+L0066D03C]//TownsBuildingsSpecNum char array
// 004613CC  8A823CD06600                       mov	al,[edx+L0066D03C]//TownsBuildingsSpecNum char array
// 0046140F  8A913CD06600                       mov	dl,[ecx+L0066D03C]//TownsBuildingsSpecNum char array
// 0046142B  8A913CD06600                       mov	dl,[ecx+L0066D03C]//TownsBuildingsSpecNum char array
// 004614C2  8A8A3CD06600                       mov	cl,[edx+L0066D03C]//TownsBuildingsSpecNum char array
// 004614DE  8A823CD06600                       mov	al,[edx+L0066D03C]//TownsBuildingsSpecNum char array
// 0046162D  8A9A3CD06600                       mov	bl,[edx+L0066D03C]//TownsBuildingsSpecNum char array
/*
 00428602  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 00428964  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 00429BB1  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 00429DEC  8B0C85B4476700              		mov	ecx,[L006747B4+eax*4]
 00429F32  8B0485B4476700              		mov	eax,[L006747B4+eax*4]
 0042A026  8B0485B4476700              		mov	eax,[L006747B4+eax*4]
 0042B538  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 0042B5D9  8B3C95B4476700              		mov	edi,[L006747B4+edx*4]
 0042B5F3  8B34B5B4476700              		mov	esi,[L006747B4+esi*4]
 0042B724  8B048DB4476700              		mov	eax,[L006747B4+ecx*4]
 0042BE42  8B1485B4476700              		mov	edx,[L006747B4+eax*4]
 0042CF07  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 0042D241  8B0485B4476700              		mov	eax,[L006747B4+eax*4]
 00432E94  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 00432F5F  8B148DB4476700              		mov	edx,[L006747B4+ecx*4]
 0043363B  8B0485B4476700              		mov	eax,[L006747B4+eax*4]
 0047AA7F  8B3CB5B4476700              		mov	edi,[L006747B4+esi*4]
 0047AA90  3B0C85B4476700              		cmp	ecx,[L006747B4+eax*4]
 0047AB00  8B1CB5B4476700              		mov	ebx,[L006747B4+esi*4]
 0047AB11  3B0C85B4476700              		cmp	ecx,[L006747B4+eax*4]
 0047AB80  8B1CB5B4476700              		mov	ebx,[L006747B4+esi*4]
 0047AB91  3B0C85B4476700              		cmp	ecx,[L006747B4+eax*4]
 004BF308  8BB0B4476700                		mov	esi,[eax+L006747B4]
 004C8D2D  8B148DB4476700              		mov	edx,[L006747B4+ecx*4]
 00503290  8B1485B4476700              		mov	edx,[L006747B4+eax*4]
 0051CFD8  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 00525AAD  3B048DB4476700              		cmp	eax,[L006747B4+ecx*4]
 0052A31B  8B1C95B4476700              		mov	ebx,[L006747B4+edx*4]
 005519A7  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 00551B68  8B0C85B4476700              		mov	ecx,[L006747B4+eax*4]
 00576455  8D80B4476700                		lea	eax,[eax+L006747B4]
 005BE383  3B149DB4476700              		cmp	edx,[L006747B4+ebx*4]
 005BE3AB  8B1495B4476700              		mov	edx,[L006747B4+edx*4]
 005BEF9E  8B0485B4476700              		mov	eax,[L006747B4+eax*4]
 005BFC66  8B048DB4476700              		mov	eax,[L006747B4+ecx*4]
 005BFFDF  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 005C0098  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 005C0203  8B0C8DB4476700              		mov	ecx,[L006747B4+ecx*4]
 005C0264  8D14F5B4476700              		lea	edx,[L006747B4+esi*8]
 005C057E  668B0C85B4476700            		mov	cx,[L006747B4+eax*4]
 005C0B34  8B1C85B4476700              		mov	ebx,[L006747B4+eax*4]
 005C0BEC  8B1C95B4476700              		mov	ebx,[L006747B4+edx*4]
 005C6023  8B0485B4476700              		mov	eax,[L006747B4+eax*4]
 005C7196  8B048DB4476700              		mov	eax,[L006747B4+ecx*4]
 005C7CE5  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 005C7D1E  8B0485B4476700              		mov	eax,[L006747B4+eax*4]
 005D9DE4  8B1495B4476700              		mov	edx,[L006747B4+edx*4]
 005D9E5D  8B148DB4476700              		mov	edx,[L006747B4+ecx*4]
 005D9ED3  8B148DB4476700              		mov	edx,[L006747B4+ecx*4]
 005D9F4C  8B148DB4476700              		mov	edx,[L006747B4+ecx*4]
 005D9FC5  8B148DB4476700              		mov	edx,[L006747B4+ecx*4]
 005DA03E  8B148DB4476700              		mov	edx,[L006747B4+ecx*4]
 005DA0C2  8B148DB4476700              		mov	edx,[L006747B4+ecx*4]
 005DA1BA  8B148DB4476700              		mov	edx,[L006747B4+ecx*4]
 005DD099  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 005DD96B  8B0495B4476700              		mov	eax,[L006747B4+edx*4]
 005DDAD6  8B048DB4476700              		mov	eax,[L006747B4+ecx*4]
*/
// 005765AA  8D82C0476700                         lea	eax,[edx+L006747C0]//TownsCreaturesTree+3*sizeof(long) long array
// 0047AB33  8B0495D0476700                       mov	eax,[L006747D0+edx*4]//TownsCreaturesTree+7*sizeof(long) long array
// 004BF302  8BB8D0476700                         mov	edi,[eax+L006747D0]//TownsCreaturesTree+7*sizeof(long) long array
// 00525A8B  8B1485D0476700                       mov	edx,[L006747D0+eax*4]//TownsCreaturesTree+7*sizeof(long) long array
// 005C0527  668B148DD0476700                     mov	dx,[L006747D0+ecx*4]//TownsCreaturesTree+7*sizeof(long) long array


// 005CCED7  8B0C956CA36800              		mov	ecx,[L0068A36C+edx*4]//TownsBackGroundMage
// 004642C8  8B048DA0D26300              		mov	eax,[L0063D2A0+ecx*4]//TownsBackGroundBattle
// 0052C9CA  8B1485D0186800              		mov	edx,[L006818D0+eax*4]//TownsPuzzle string array
// 0052CF59  8B14B5D0186800              		mov	edx,[L006818D0+esi*4]//TownsPuzzle string array


// 0040C9A8  8B0C8D5C756A00                     mov	ecx,[L006A755C+ecx*4]//TownsTypesTxt string array
// 005697F7  8B0C855C756A00                     mov	ecx,[L006A755C+eax*4]//TownsTypesTxt string array
// 0057635A  8B0CBD5C756A00                     mov	ecx,[L006A755C+edi*4]//TownsTypesTxt string array
// 0058D95A  8B0C855C756A00                     mov	ecx,[L006A755C+eax*4]//TownsTypesTxt string array
// 0058DD24  8B0C855C756A00                     mov	ecx,[L006A755C+eax*4]//TownsTypesTxt string array
// 005C1854  8B04855C756A00                     mov	eax,[L006A755C+eax*4]//TownsTypesTxt string array
// 005D2E64  8B3C9DD87E6A00                     mov	edi,[L006A7ED8+ebx*4]//TownsBlackSmithDesc string array
// 005D17AF  8B048DA02E6400                     mov	eax,[L00642EA0+ecx*4]//TownsBlackSmithCreatures long array
// 005D18C1  8B0485A02E6400                     mov	eax,[L00642EA0+eax*4]//TownsBlackSmithCreatures long array
// 005D1B02  8B148DA02E6400                     mov	edx,[L00642EA0+ecx*4]//TownsBlackSmithCreatures long array
// 005D1FA1  8B048DA02E6400                     mov	eax,[L00642EA0+ecx*4]//TownsBlackSmithCreatures long array
// 005D1FD3  8B0485A02E6400                     mov	eax,[L00642EA0+eax*4]//TownsBlackSmithCreatures long array
// 005D224C  8B04BDA02E6400                     mov	eax,[L00642EA0+edi*4]//TownsBlackSmithCreatures long array
// 005D2289  8B04BDA02E6400                     mov	eax,[L00642EA0+edi*4]//TownsBlackSmithCreatures long array
// 00525DE0  8B04D560AA6A00                     mov	eax,[L006AAA60+edx*8]//TownsBlackSmithArtifacts long array
// 005D1D89  8B14CD60AA6A00                     mov	edx,[L006AAA60+ecx*8]//TownsBlackSmithArtifacts long array
// 005D1EBE  8B0CC560AA6A00                     mov	ecx,[L006AAA60+eax*8]//TownsBlackSmithArtifacts long array
// 005D223F  8D14FD60AA6A00                     lea	edx,[L006AAA60+edi*8]//TownsBlackSmithArtifacts long array
// 005D1ECB  8B14C564AA6A00                     mov	edx,[L006AAA64+eax*8]//TownsBlackSmithArtifacts+1*sizeof(long) long array



	{0x4FD57F, (long)GetTerrainOverlayEnd, H_JMP5}, // GetTerrainOverlayTypeOfMapItem
	{0x4FD532, (long)GetTerrainOverlaySwitch, H_CALL5},
	//{0x4F6C05, (long)DisplayDialogHook, H_CALL5},
	//{0x47FF00, (long)BeforeHeroMove, H_CALL5},
	{0x52FDD3, (long)MonDlgBorderColorBug, H_CALL5}, // monster right click dialog borders bug
	{0x530D2E, (long)GarrisonDlgBorderColorBug, H_JMP5}, // garrison right click dialog borders bug
	// make monsters play the 'standing' animation:
	{0x43DEA4, (long)MovingMonsters, H_CALL5}, // make monsters play the 'standing' animation
	{0x493FC0, (long)MovingMonsters2, H_CALL5}, // handle AnimationStep
	{0x4468D3, (long)MovingMonsters3, H_CALL5}, // prevent drawing shadow twice while monster is moving
	{0x446103, (long)MovingMonsters3, H_CALL5}, // prevent drawing shadow twice after Catapult shot
	{0x4B4AC0, (long)MovingMonsters4, H_CALL5}, // show animation while flying
	{0x43F4AA, (long)MovingMonsters5, H_CALL5}, // show animation while shooting
	{0x46784B, (long)MovingMonsters5, H_CALL5}, // show animation while Catapult shooting
	{0x46818D, (long)MovingMonsters5, H_CALL5}, // show animation while Arrow Tower shooting
	{0x467CBE, (long)MovingMonsters5, H_CALL5}, // show animation while magic missils move
	{0x59F470, (long)MovingMonsters6, H_LONG}, // teleport select monster
	{0x59F48F, (long)MovingMonsters7, H_LONG}, // teleport select place
	{0x59F63B, (long)MovingMonsters8, H_LONG}, // sacrifice select whom to revieve
	{0x59F65A, (long)MovingMonsters9, H_LONG}, // sacrifice select whom to kill
	//{0x603190, (long)onRedrawPartOfScreen, H_CALL5},
	{0x59A7C0, (long)NoWavWait, H_JMP5}, // WaitForWavToPlay
	{0x59A1C0, (long)NoWavWait, H_JMP5}, // WaitForWav2
	{0x59A770, (long)NoWavWait2, H_JMP5}, // Play almost all sounds asynchronousely (PlayWAVFile -> LoadWAVplayAsync)
	{0x445EB9, (long)NoWavWait3a, H_CALL5}, // Play Ballista shot explosion sound asynchronously
	{0x445FE8, (long)NoWavWait3b, H_CALL5}, // Play Ballista shot explosion sound asynchronously
	{0x5A067E, (long)NoWavWait3a, H_CALL5}, // Play Quicksand sound asynchronously
	{0x5A071A, (long)NoWavWait3b, H_CALL5}, // Play Quicksand sound asynchronously
	{0x5A088C, (long)NoWavWait3a, H_CALL5}, // Play Landmine sound asynchronously
	{0x5A0934, (long)NoWavWait3b, H_CALL5}, // Play Landmine sound asynchronously
	{0x59B110, (long)CompleteCombatStartSound, H_CALL5}, // прерывает все звуки. Ожидания звуков больше нет, поэтому это не нужно
	{0x59B2AD, (long)CompleteCombatStartSound, H_CALL5}, // прерывает все звуки
	{0x462C30, (long)CombatStartDelay, H_CALL5}, // still need a little delay on combat start
	//{0x5A6127, (long)ShortCombatDelays, H_CALL5}, // !!! possible option: faster combat  (ProsessMessages while shooter's arrow moves)
	{0x5998AF, (long)OnLoadMP3, H_CALL5}, // !?MP
	{0x59AF90, (long)OnLoadMP3, H_CALL5}, // !?MP
	{0x59B202, (long)OnLoadMP3, H_CALL5}, // !?MP
	//{0x4B0BA0, (long)MessagesLoop, H_CALL5}, // called when a map starts
	{0x55C930, (long)OnLoadWav, H_CALL5}, // !?SN
	//{0x602AFB, (long)OnModalDialog, H_CALL5}, // ProcessDialog (bad way to hook)
	//{0x41B120, (long)_OnDialogAction, H_JMP5}, // Dlg_ProcessAction (bad way to hook)

	// Hook dialogs
	//{0x6029C7, (long)_OnShowDialog, H_JMP5},
	{0x6029B6, (long)"6A00 90", H_COPY}, // push 0
	{0x6029BE, (long)"90", H_COPY},
	{0x602A09, (long)_OnShowDialog1, H_JMP5},
	{0x602A30, (long)_OnShowDialog1, H_JMP5},
	{0x602A4A, (long)_OnShowDialog2, H_JMP5},
	{0x602A65, (long)_OnHideDialog, H_CALL5},
	{0x602C56, (long)_OnDialogCallback, H_JMP5},

	//{0x50CEA5, (long)_OnChangeCursor, H_CALL5}, // SetMouseCursor hook (for researches)
	//{0x4017F9, (long)tmpQuestLog, H_CALL5},
	{0x462DF0, (long)OnBattleStart, H_CALL5},
	{0x47CA09, (long)_DrawDef1ToMap1, H_CALL5}, // fix SOD bug: compression type 1 couldn't be used on map
	{0x47D6A4, (long)_DrawDef1ToMap2, H_CALL5}, // fix SOD bug: compression type 1 couldn't be used on map
	{0x4EC800, (long)_TranslateKey, H_JMP5}, // locale support by edits
	{0x4EC7C6, (long)_TranslateKeyPart, H_JMP5}, // allow '`' and '~' characters in edits
	{0x5BFA00, (long)_GetTownIncome, H_JMP5}, // !?CI0
	{0x5BFF60, (long)_GetTownGrowth, H_JMP5}, // !?CI1
	{0x4F5584, (long)_MessagePictureInit, H_JMP5}, // MessageEx initialization
	{0x4F11E7, (long)_MessagePictureHint, H_JMP5}, // MessageEx hint
	{0x558DF0, (long)_BuildResBar, H_JMP5}, // build ResBar panel
	{0x40FC80, (long)_GetFlagColor, H_JMP5}, // get flag color of an object
	{0x410EA6, (long)_GetFlagColor1, H_JMP|H_INSR6}, // allow flag color for all objects
	{0x4125A9, (long)_GetFlagColor2, H_JMP|H_INSR6}, // allow flag color for all objects
	{0x5F7847, (long)"9090 9090", H_COPY}, // allow flag color for all objects
	{0x5F8906, (long)"9090 909090909090", H_COPY}, // allow flag color for all objects
	{0x40EEDF, (long)_DigGrail, H_JMP5}, // last check for valid Grail place
	{0x40F04B, (long)_DigGrailFail, H_JMP5}, // dig where no Grail exists
	{0x4DAF06, (long)_PostGainLevel, H_JMP5}, // gain level post-trigger
	{0x4DAF01, (long)_PostGainLevelChosen, H_CALL5}, // gain level post-trigger, SSkill chosen
	// SoD campaigns
	{0x66B7BC, (long)"c", H_STR},
	{0x66CB2C, (long)"7365637265742E683363000066696E616C2E68336300000073616E64726F2E683363000067656C752E6833630000000067656D2E68336300796F672E68336300637261672E68336300000000", H_COPY},
	{0x66CC18, (long)"63616D707370312E7063780063616D707561312E7063780063616D70726E312E7063780063616D70656C312E7063780063616D706E62312E7063780063616D706262312E7063780063616D706873312E70637800", H_COPY},
	{0x66CD84, (long)"campbkx2.pcx", H_STR0},
	{0x675D7C, (long)"4833783255416D004833783255416C004833783255416B004833783255416A004833783255416900483378325541680048337832554167004833783255416600483378325541650048337832554164004833783255416300483378325541620048337832554161004833783253506500483378325350640048337832535063004833783253506200483378325350610048337832524E650048337832524E640048337832524E630048337832524E620048337832524E6100483378324E426500483378324E426400483378324E426300483378324E426200483378324E4261004833783248536500483378324853640048337832485363004833783248536200483378324853610048337832454C650048337832454C640048337832454C630048337832454C620048337832454C61004833783242426600483378324242650048337832424264004833783242426300483378324242620048337832424261004142766F504634004142766F504633004142766F504632004142766F504631004142766F465735004142766F465734004142766F465733004142766F465732004142766F465731004142766F464C35004142766F464C34004142766F464C33004142766F464C32004142766F464C31004142766F445335004142766F445334004142766F445333004142766F445332004142766F445331004142766F444235004142766F444234004142766F444233004142766F444232004142766F444231004142766F414239004142766F414238004142766F414237004142766F414236004142766F414235004142766F414234004142766F414233004142766F414232004142766F41423100733163007331620073316100673363006733620067336100653264006532630065326200653241650000000065326100673264006732630067326200673261006E31635F640000006E3162006E316100653163006531620065316100673163006731620067316100", H_COPY}, // Campaign voice
	{0x684998, (long)"756E686F6C7900007269736500000000656C6978697200006E65770062697274680000006861636B00000000", H_COPY},
	// done SoD campaigns
	{0x4F80DC, (long)_OnStart, H_JMP5}, // start of the game, after initialization of CRT, in check for multiple copies
	{0x528520, (long)_GetAIMapPosValue, H_JMP5}, // 
	{0x49258D, (long)"909090909090 9090 909090909090", H_COPY}, // show damage no metter creature info enabled or not
	{0x4926B9, 0x49272B, H_JMP5}, // show damage no metter creature info enabled or not
	{0x5776EF, 0x57772F, H_JMP5}, // remove buggy disk space check
	{0x4FBDCF, 0xEB, H_BYTE}, // remove buggy disk space check
	//{0x4FDF2C, (long)_OnEarlyNewGame, H_CALL5}, // New Game before loading placed objects
	{0x4EDD65, (long)_OnLodsLoaded, H_CALL5}, // Load h3wog.lod from DeveloperPath, run Global scripts
	{0x55C9C0, (long)_OnLoadDef, H_CALL5}, // Don't use global cache for def frames
	{0x55CDE2, (long)_OnLoadDefFindCadre, H_JMP5}, // Don't use global cache for def frames
	{0x55CEFA, (long)_OnLoadDefAddCadre, H_CALL5}, // Don't use global cache for def frames
	{0x63D6C0, (long)_OnDefCadreDeref, H_DWORD}, // Don't use global cache for def frames
	{0x4EC685, (long)OnNoNewMessage, H_CALL5}, // Insert Sleep(1) into message loop
	{0x4CCC40+3, 0x696DE8, H_DWORD}, // Fix rare crash on map start related to tavern rumors
	{0x4F0DD0, (long)_FixParseCmdLine, H_CALL5|H_INSR6}, // For paths with "/"; " ?" -> "/?"
	{0x4EF444, 0x4EF450, H_NOP}, // Bugfix: on start of Tutorial 2 combat options were reset to default values
	{0x4EEF4C, (long)_OnFastMapLoad, H_CALL5, -200}, // Command line options: /mapname, /mapdir
	{0x4EEE29, 0xEB, H_BYTE, -200}, // Command line options: /mapname, /mapdir
	//{0x4ED190+6, (long)&HeroesGameType, H_DWORD}, // Always allow Conflux
	//{0x456CAF, (long)&HeroesGameTypeStdPo, H_DWORD}, // Always allow Conflux, but not AB campaign
	//{0x457228, (long)&HeroesGameTypeStdPo, H_DWORD}, // Always allow Conflux, but not AB campaign
	{0x4ED2AF+3, 0x63FE20, H_DWORD}, // Don't require h3blade.exe for Conflux and AB campaign
	{0x619BB1, (long)_MyStatMemCheck, H_CALL5|H_INSR6}, // фикс падения при выходе при попытке освободить нашу статическую память
	{0x4B5654, (long)_FixTextLinesCount, H_CALL5|H_INSR6}, // GetLinesCountInText hanging if Width is too small
	{0x40770E, (long)_FixLoadActiveHero, H_CALL5}, // Don't reset current hero when loading a game
	{0x600530, 0x600532, H_NOP}, // Strange hanging bug
	{0x4C5D09, 0x4C5D0F, H_NOP}, // Don't limit events text length to 65535 characters
	{0x684AF8, 0xAC44, H_DWORD}, // Use 44khz
	{0x684B00, 2, H_DWORD}, // Use stereo
	{0x660DFC, (long)"C", H_STR}, // Classic SoD campaign button
	{0x50C674, 0x50C67F, H_NOP}, // SoD bug: 3 seconds startup delay

	{0}
};

__newCopiers newCopiers[]=
{
	//Bit per pixel ignored
	{0x600A55,"\xB3\x01\x90",3,0,-3},
	{0x601A69,"\xB0\x01\x90",3,0,-3},
	{0x602117,"\xB0\x01\x90",3,0,-3},

/*
	//Towns
	{0x5CA902+2,"\x09",1,0},//Count towns attached to TownsHallLoading
	{0x46104D+2,"\x09",1,0},//Count towns attached to TownsHallBuilding 

//Don't use this
{0x5309D4,"\x90\x90",2,-2},//005309D4  7EE8		jle	L005309BE//Problem







	 //005CA902  83F808                      		cmp	eax,00000008h//Count towns attached to TownsHallLoading
	 //0046104D  83F908								cmp	ecx,00000008h//Count towns attached to TownsHallBuilding 

/*


	{0x4868F5+2,"\x09",1,0},//Count towns attached to TownsHallLoading
	{0x486D45+2,"\x09",1,0},//Count towns attached to TownsHallLoading
	{0x4B2ADF+2,"\x09",1,0},//Count towns attached to TownsHallLoading
	{0x4CA99E+1,"\x09",1,0},//Count towns attached to TownsHallLoading
	{0x4D908E+6,"\x09",1,0},//Count towns attached to TownsHallLoading
	{0x49CDEA+2,"\x09",1,0},//Count towns attached to TownsHallLoading
	{0x49CE13+2,"\x09",1,0},//Count towns attached to TownsHallLoading

 004868F5  83FE08                                                    		cmp	esi,00000008h
 00486D45  83FF08                                                    		cmp	edi,00000008h
 004B2ADF  83F808                                                    		cmp	eax,00000008h
 004CA99E  BA08000000                                                		mov	edx,00000008h
 004D908E  83B85CF4010008                                            		cmp	dword ptr [eax+0001F45Ch],00000008h
 0049CDEA  83F808                                                    		cmp	eax,00000008h
 0049CE13  83F908                                                    		cmp	ecx,00000008h

*/

	{0}
};