#define NUMBULDINGS 44
#define NUMCASTLE 20

struct BULDINGSCOORD{
	unsigned short XCoordinate;
	unsigned short YCoordinate;
};

struct BULDINGSCOST{
	int wood;
	int mercury;
	int ore;
	int sulfor;
	int crystal;
	int gems;
	int gold;
};

struct CASTLEBULDINGS{
	char* BuldingName;        // название строения
	char* BuldingFutures;     // описание строения
	char* BuldingDefName;     // деф строения
	char* BorderDefName;      // деф оконтовки
	BULDINGSCOORD Coordinats; // координаты здания
	BULDINGSCOST  CostBuld;   // стоимость здания 
	bool IsBuld;              // доступность для строения
	int TypeMonstr;           // для двелингов тип монстра
	int TypeResource;         // для хранилища тип ресурса
	int WarMashine;           // для кузницы тип машины
	int BuldForDecor;         // зависимость декора
};

struct CASTELS{
	char SIGNATURE [4];
	CASTLEBULDINGS Buldings[NUMBULDINGS];
};

extern CASTELS Castels [NUMCASTLE];

// структуры для замены в памяти///////////////////////////

struct BUILDNAME{
	char* BuldingName;
};

struct BORDNAME{
	char* BorderName;
};

struct CASTELSINMEMORY{
	BUILDNAME     DefNameBuild[NUMCASTLE][NUMBULDINGS];   // массив для названий дефов зданий
    BORDNAME      DefBorderName[NUMCASTLE][NUMBULDINGS];  // массив для оконтовок зданий
	BULDINGSCOORD Coordinate[NUMCASTLE][NUMBULDINGS*6/4];     // координаты зданий в экране города

};

extern CASTELSINMEMORY CastInMem;


