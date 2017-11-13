//---------------------------------------------------------------------------

#ifndef WorkStrH
#define WorkStrH

#include <string.h>
#include <atlstr.h>
//#include "..\cards\Card.h"

typedef unsigned int uint;
class clAnsiString
{
public:
	clAnsiString();
	~clAnsiString();
	clAnsiString(const clAnsiString &as);
	clAnsiString(char *);
	clAnsiString(int);
	clAnsiString(size_t);
	clAnsiString(__int64);
	clAnsiString(double);
	clAnsiString(CString);
	int ReadFile(int handle);
	int WriteFile(int handle);
	int SizeInFile()			{ return sizeof(int) + _size; }

	char *c_str()		{ return _ptr; }

	void operator = (const clAnsiString &str);
	bool operator == (char *str);
	bool operator == (const clAnsiString &str);
	char &operator [] (int nb)					{ return _ptr[nb]; }

	clAnsiString operator + (const clAnsiString &as);
	clAnsiString operator + (char *str);

	clAnsiString &operator += (const clAnsiString &as);
	clAnsiString &operator += (char *x);
	clAnsiString &operator += (int x);
	clAnsiString &operator += (double x);

	void	SetLength(int val);
	int		Length()			{ return _size; }

private:
	char *_ptr;
	unsigned _size;
};
//---------------------------------------------------------------------------
bool    WSIsSymbol(char ch);
bool    WSStrFind(char *str,char * sample,int len,int &size);
inline  bool    WSStrFind(char *str,char * sample,int &size)    {return WSStrFind(str,sample,(int)strlen(str)-(int)strlen(sample),size);}
inline  bool    WSStrFindRusklinok(char *str,char * sample,int len)   {int size; return WSStrFind(str,sample,len,size);}
int     WSFindChar(char *str,int sizeBuf,char ch);
int     WSFindCharPrev(char *str,int sizeBuf,char ch);
int     WSFindNoChar(char *str,int sizeBuf,char ch);
int     WSFindCharFromEndStr(char *str,int len,char ch);
int     WSReadInt(char *str,int len,int &size);
int     WSReadRimInt(char *str,int len,int &size);
long long     WSReadInt64(char *str,int len,int &size);
inline  int     WSReadInt(char *str)        {int size;return WSReadInt(str,(int)strlen(str),size);}
double  WSReadDouble(char *str, size_t len, int &size);
inline  double  WSReadDouble(char *str) {int size; return WSReadDouble(str,strlen(str),size);}
int     WSReadWord(char *str,int len,char *word);
//clCard  WSReadCard(char *word);
int     WSReadMoney(char *str,int len,int &size,char t,char c);// параметры :
//str-строка,len-длина строки,size-длина строки содержащая money от начала str,
//t-символ разделитель тысяч долларов,c-символ разделитель центов
int     WSReadChips(char *str,int len,int &size0);
int     WSShiftFirstSmbl(char *str,int len);
bool    WSFirstSmblIsDig(char *str,int len);
inline bool WSSmblIsDig(char ch)  {return (ch>='0' && ch <='9');}
clAnsiString	WSDoubleToAS(double val,int r);
clAnsiString	WSDoubleToProc(double val, int r = 0);
clAnsiString	WSReadFileName(char *str);
int     FindDig(char *str,int len,char *dig,int &size);
//---------------------------------------------------------------------------
#endif
