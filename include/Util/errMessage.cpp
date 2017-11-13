//---------------------------------------------------------------------------

#pragma hdrstop


#define _CRT_SECURE_NO_WARNINGS

#include <fcntl.h>
//#include <alloc.h>
#include <io.h>
#include <sys\stat.h>
#include <stdlib.h>
#include <afxwin.h>
#include "errMessage.h"

#define FILE_FOR_TRASSER "C:\\WORK\\UTIL\\TRASSER\\trass"
//#define FILE_FOR_TRASSER "\\\\192.168.0.33\\share\\trasser\\trass"

clErrTrasser glTrasser;
//AnsiString glTrasserAs;

size_t glTime1=0, glTime2=0, glTime3=0;
unsigned iran = 0;

inline	unsigned randQ()
{
	return iran = 1664525 * iran + 1013904223;
}

inline	unsigned randomQ(int a)
{
	iran = 1664525 * iran + 1013904223;
	return iran%a;
}

double randomFloat()				{ return rand() / ((double)(RAND_MAX+1)); }
inline	double	randomDouble()	{ return ((double)rand()*(RAND_MAX+1)+rand()) / (RAND_MAX+1)/ (RAND_MAX+1); }
//---------------------------------------------------------------------------
unsigned random32(unsigned a)
{
	unsigned res = rand();
	res = res << 16;
	res += rand();
	return res%a;
}
//---------------------------------------------------------------------------
bool    ErrCheck(int val, int vMin, int vMax)
{
    return ErrCheck("Error",val,vMin,vMax);
}
//---------------------------------------------------------------------------
bool    ErrCheck(int val,int val1)
{
    return ErrCheck("Error",val,val1);
}
//---------------------------------------------------------------------------
#ifdef  ERR_MESSAGE
void    ErrMessage(clAnsiString a1,clAnsiString a2)
{
	a1 += a2;
	AfxMessageBox(_T(a1.c_str()));
}
//---------------------------------------------------------------------------
bool    ErrCheck(char *mes,int val,int vMin,int vMax)
{
    if(val<vMin || val>vMax)
    {
        char str[128];
        strcpy(str,"val=");
        _itoa(val,str+strlen(str),10);
        strcat(str,"; vMin=");
        _itoa(vMin,str+strlen(str),10);
        strcat(str,"; vMax=");
        _itoa(vMax,str+strlen(str),10);
        strcat(str,";");
        ErrMessage(str,mes);
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool    ErrCheck(char *mes,int val,int src)
{
    if(val!=src)
    {
        char str[128];
        strcpy(str,"val=");
        _itoa(val,str+strlen(str),10);
        strcat(str,"; src=");
        _itoa(src,str+strlen(str),10);
        strcat(str,";");
        ErrMessage(str,mes);
        return false;
    }
    return true;
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
clErrTrasser::clErrTrasser()
{
    _handle=-1;
#ifdef  ERR_TRASSER
    int nb=0;
	
    while(true)
    {
		
        clAnsiString as=FILE_FOR_TRASSER;
		as += (clAnsiString)nb + (clAnsiString)".txt";
        if((_handle=_open(as.c_str(),O_TEXT | O_RDWR))==-1)
        {
            _handle=_open(as.c_str(),O_TEXT | O_RDWR | O_CREAT);
            _chmod(as.c_str(),S_IREAD | S_IWRITE);
            break;
        }
        else
            _close(_handle);
        nb++;
    }
	
#endif
}
//---------------------------------------------------------------------------
clErrTrasser::~clErrTrasser()
{
    if(_handle != -1)
        _close(_handle);
}
//---------------------------------------------------------------------------
void    clErrTrasser::WriteMessage(clAnsiString as)
{
#ifdef  ERR_TRASSER
	char c = '\n';
	_write(_handle, &c, sizeof(char));
	char *str = as.c_str();
	_write(_handle, str, strlen(str));
#endif
}
//---------------------------------------------------------------------------
void    clErrTrasser::WriteMessage(CString str)
{
#ifdef  ERR_TRASSER
	char c = '\n';
	_write(_handle, &c, sizeof(char));
	_write(_handle, &str, strlen(str));
#endif
}
//---------------------------------------------------------------------------
#ifdef  ERR_MESSAGE
void    *ErrMalloc(unsigned size)
{
    void *ptr=malloc(size);
    if(ptr==NULL)
        ErrMessage("Нет памяти","ErrMalloc");
    return ptr;
}
//---------------------------------------------------------------------------
void    *ErrRealloc(void *ptr0,unsigned size)
{
    void *ptr=realloc(ptr0,size);
    if(ptr==NULL)
        ErrMessage("Нет памяти","ErrRealloc");
    return ptr;
}
#endif
//---------------------------------------------------------------------------

