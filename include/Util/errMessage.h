//---------------------------------------------------------------------------
#ifndef errMessageH
#define errMessageH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <memory.h>
//#include <vcl>
#include "c:\home\Util\WorkStr.h"
#include <atlstr.h>

#define _CRTDBG_MAP_ALLOC
#define DOUBLE_0    (1.E-12)
#define BYTE_UNDEF  255
#define UNDEF_NB    -1
#define INT_UNDEF           -1

#define ERR_MESSAGE
#define ERR_TRASSER
#define ERR_TEMP			12

#ifdef  ERR_MESSAGE
void    ErrMessage(clAnsiString text,clAnsiString caption);   //Application->MessageBox(a1,a2, MB_OKCANCEL)
bool    ErrCheck(char *mes,int val,int vMin,int vMax);
bool    ErrCheck(char *mes,int val,int src);
void    *ErrMalloc(unsigned size);
void    *ErrRealloc(void *ptr,unsigned size);
#else
inline  bool    ErrCheck(char *mes,int val,int vMin,int vMax)   {return true;}
inline  bool    ErrCheck(char *mes,int val,int src)             {return true;}
inline  void    ErrMessage(char *a1,char *a2)   {}
inline  void    *ErrMalloc(unsigned size)               {return malloc(size);}
inline  void    *ErrRealloc(void *ptr,unsigned size)    {return realloc(ptr,size);}
#endif

bool    ErrCheck(int val,int vMin,int vMax);
bool    ErrCheck(int val,int src);
void    ErrTrasser(char *str);

class clErrTrasser
{
public:
    clErrTrasser();
    ~clErrTrasser();
	void    WriteMessage(clAnsiString str);
	void    WriteMessage(CString str);
private:
    int _handle;
};

typedef double tpFloat;
typedef float tpFloat0;
extern clErrTrasser glTrasser;
unsigned random32(unsigned a);
double randomFloat();
double	randomDouble();

inline	int random(int a)		{	return rand() % a; }

extern size_t glTime1, glTime2, glTime3;

//---------------------------------------------------------------------------
#endif
