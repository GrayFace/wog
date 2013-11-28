class String{
	int   Ind;
	char *Text;
	String(String &);
	public:
		String();
	 ~String();
		void Set(int ind,char *txt);
		void Clear();
		int GetInd();
		char *GetText();
};
#define MAXSTRINGNUM 50000L
class StringSet{
	StringSet(StringSet &);
	StringSet();
 ~StringSet();
	StringSet &operator=(StringSet &);
	public:
		static void Clear();
		static char *GetText(int ind);
		static bool Belongs(const char *str);
};

class StrMan{
	static int NextStr;
public:
	static int Count;
	static _AMes_ ERMDynString[16384];

private:
	StrMan(StrMan &);
	StrMan();
 ~StrMan();
	StrMan &operator=(StrMan &);
public:
	static int Push(char * str, int len = -1); // allocates a copy of str and returns its index in ERMDynString array
	static void Remove(int Index);
	static void Reset();
	static int Save();
	static int Load();
	static char *GetStoredStr(int ind, char *backup = 0);
	static int Apply(char* &str, char* backup, int &var, Mes* Mp, int ind); // returns 1 if the string was changed
	static int Apply(char* str, Mes* Mp, int ind, int bufsize = 512); // returns 1 if the string was changed
};
