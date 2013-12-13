#pragma once

///////////////////////////////////////
// Загрузка TXT файлов

struct _TxtString_{
	int    Exist;        // 1=выделена память, 0=нет
	char **Start;
	char **Stop;
	char **End;
};

struct _FileLoaded_{   // размер 34
	Dword  Fun;          // +00 функция для освобождения памяти
	char   FileName[12]; // +04 
	Dword _u1[4];        // +10
	_TxtString_ **Start;  // +20 ук на первую строку
	_TxtString_ **Stop;   // +24 ук на строку за последней
	// +28 -> ук на выд память под строки
	// +2C -> полное содержимое файла
	// +30 = полный размер файла
};

struct TxtFile{
	_FileLoaded_ *fl;
	int sn;
};

/////////////////////////
int LoadTXT(const char *name, TxtFile *tf);
void UnloadTXT(TxtFile *tf);
char *ITxt(int StrNum,int ItemNum,TxtFile *tf);
