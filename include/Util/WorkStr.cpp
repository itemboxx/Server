//---------------------------------------------------------------------------
//#pragma hdrstop

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <io.h>

#include "c:\home\Util\WorkStr.h"

#define MAX_LEN_WORD    31

//bool    IsSymbol(char ch);
//---------------------------------------------------------------------------
clAnsiString::clAnsiString()
{
	_ptr = NULL;
	_size = 0;
}
//---------------------------------------------------------------------------
clAnsiString::clAnsiString(CString str)
{
	_size = strlen(str);
	if (_size > 0)
	{
		_ptr = (char *)malloc(_size + 1);
		memcpy(_ptr, str, _size + 1);
	}
	else
		_ptr = NULL;
}
//---------------------------------------------------------------------------
clAnsiString::clAnsiString(char *str)
{
	_size = strlen(str);
	if (_size > 0)
	{
		_ptr = (char *)malloc(_size + 1);
		memcpy(_ptr, str, _size + 1);
	}
	else
		_ptr = NULL;
}
//---------------------------------------------------------------------------
clAnsiString::~clAnsiString()
{
	if (_ptr != NULL)
		free(_ptr);
}
//---------------------------------------------------------------------------
clAnsiString::clAnsiString(const clAnsiString &as)	
{ 
	if (as._size == 0)
	{
		_size = 0;
		_ptr = NULL;
	}
	else
	{
		_size = as._size;
		_ptr = (char *)malloc(_size + 1);
		memcpy(_ptr, as._ptr, _size + 1);
	}
}
//---------------------------------------------------------------------------
void clAnsiString::operator = (const clAnsiString &str)
{
	_size = str._size;
	if (_size > 0)
	{
		_ptr = (char *)realloc(_ptr, _size + 1);
		memcpy(_ptr, str._ptr, _size + 1);
	}
}
//---------------------------------------------------------------------------
clAnsiString::clAnsiString(int x)
{
	_ptr = (char *)malloc(64);
	_i64toa(x, _ptr, 10);
	_size = strlen(_ptr);
}
//---------------------------------------------------------------------------
clAnsiString::clAnsiString(size_t x)
{
	_ptr = (char *)malloc(64);
	_i64toa(x, _ptr, 10);
	_size = strlen(_ptr);
}
//---------------------------------------------------------------------------
clAnsiString::clAnsiString(__int64 x)
{
	_ptr = (char *)malloc(64);
	_itoa(x, _ptr, 10);
	_size = strlen(_ptr);
}
//---------------------------------------------------------------------------
clAnsiString::clAnsiString(double val)
{
	_ptr = (char *)malloc(64);
	_gcvt(val, 20, _ptr);
	_size = strlen(_ptr);
}
//---------------------------------------------------------------------------
int clAnsiString::ReadFile(int handle)
{
	int len;
	_read(handle, &len, sizeof(int));
	if (len > 0)
	{
		SetLength(len);
		_read(handle, _ptr, _size);
		_ptr[_size] = '\0';
	}
	return 0;
}
//---------------------------------------------------------------------------
int clAnsiString::WriteFile(int handle)
{
	_write(handle, &_size, sizeof(_size));
	if (_size>0)
		_write(handle, _ptr,_size);
	return 0;
}
//---------------------------------------------------------------------------
bool clAnsiString::operator == (char *str)
{
	return !strcmp(_ptr, str);
}
//---------------------------------------------------------------------------
bool clAnsiString::operator == (const clAnsiString &str)
{
	return !strcmp(_ptr, str._ptr);
}
//---------------------------------------------------------------------------
clAnsiString clAnsiString::operator + (const clAnsiString &as)
{
	clAnsiString res;
	res._size = _size + as._size;
	res._ptr = (char *)realloc(res._ptr,res._size + 1);
	memcpy(res._ptr, _ptr, _size);
	memcpy(res._ptr + _size, as._ptr, as._size + 1);
	return res;
}
//---------------------------------------------------------------------------
clAnsiString clAnsiString::operator + (char *str)
{
	clAnsiString res;
	res._size = _size + strlen(str);
	res._ptr = (char *)realloc(res._ptr,res._size + 1);
	memcpy(res._ptr, _ptr, _size);
	memcpy(res._ptr + _size, str, res._size-_size + 1);
	return res;
}
//---------------------------------------------------------------------------
clAnsiString &clAnsiString::operator += (const clAnsiString &as)
{
	if (as._ptr != NULL)
	{
		_ptr = (char *)realloc(_ptr, _size + as._size + 1);
		memcpy(_ptr + _size, as._ptr, as._size + 1);
		_size += as._size;
	}
	return *this;
}
//---------------------------------------------------------------------------
clAnsiString &clAnsiString::operator += (char *str)
{
	int len = strlen(str);
	if (len > 0)
	{
		_ptr = (char *)realloc(_ptr, _size + len + 1);
		memcpy(_ptr + _size, str, len);
		_size += len;
		_ptr[_size] = '\0';
	}
	return *this;
}
//---------------------------------------------------------------------------
clAnsiString &clAnsiString::operator += (int x)
{
	char str[64];
	_itoa(x, str, 10);
	return *this += str;
}
//---------------------------------------------------------------------------
void	clAnsiString::SetLength(int val)
{
	_ptr = (char *)realloc(_ptr, val+1);
	_size = val;
	_ptr[val] = '\0';
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool    WSStrFind(char *str, char * sample, int len, int &s0) // len-количество букв в строке среди которых ищется первая буква из  sample
{
    int size=strlen(sample);
    for(int i=0;i<=len;i++)
    {
        int j;
        for(j=0;j<size;j++)
            if(str[i+j]!=sample[j])
                break;
        if(j==size)
        {
            s0=i+size;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
int     WSReadWord(char *str,int size0,char *word)
{
    int i,pos=0;
    for(i=0;i<size0;i++)
        if(WSIsSymbol(str[i]))
            break;
    if(i==size0)
    {
        word[0]='\0';
        return -1;
    }
    if(size0>i+MAX_LEN_WORD)
        size0=i+MAX_LEN_WORD;
    for(;i<size0;i++)
        if(WSIsSymbol(str[i]))
            word[pos++]=str[i];
        else
        {
            word[pos]='\0';
            return i;
        }
    word[MAX_LEN_WORD]='\0';
    return -1;
}
//---------------------------------------------------------------------------
int     WSReadInt(char *str,int len,int &size)
{
    int res;
    char dig[16];
    int beg=FindDig(str,len,dig,size);
    if(strlen(dig)==0)
        return -1;
    res=atoi(dig);
    if(str[beg-1]=='-') res*=-1;
    return res;
}
//---------------------------------------------------------------------------
char glRimChar[]={'I','V','X','L','C'};
int glRimVal[]={1,5,10,50,100};
const int glCnRimDig=sizeof(glRimChar);

int IndexRimChar(char c)
{
    for(int i=0;i<glCnRimDig;i++)
        if(c == glRimChar[i])
            return i;
    return -1;
}

int WSRimInt(char *str,int cn)
{
    int res=0,v0=0,ur=glCnRimDig;
    for(int i=0;i<cn;i++)
    {
        int ur0=IndexRimChar(str[i]);
        if(ur0>ur)
        {
            res+=glRimVal[ur0]-v0;
            v0=0;
        }
        else if(ur0<ur)
        {
            res+=v0;
            v0=glRimVal[ur0];
        }
        else
            v0+=glRimVal[ur0];
        ur=ur0;
    }
    return res+v0;
}

int     WSReadRimInt(char *str,int len,int &size)
{
    int i=0;
    char dig[16];
    for(size=0;size<len;size++)
        if(IndexRimChar(str[size]) != -1)
            break;
    for(;size<len;size++)
        if(IndexRimChar(str[size]) != -1)
            dig[i++]=str[size];
        else break;
    return WSRimInt(dig,i);
}
//---------------------------------------------------------------------------
long long     WSReadInt64(char *str,int len,int &size)
{
    long long val=0;
    char dig[16];
    FindDig(str,len,dig,size);
    for(unsigned int i=0;i<strlen(dig);i++)
        val=val*10+dig[i]-'0';
    return val;
}
//---------------------------------------------------------------------------
int     WSFindChar(char *str,int sizeBuf,char ch)
{
    for(int i=0;i<sizeBuf;i++)
        if(str[i]==ch)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
int     WSFindCharPrev(char *str,int sizeBuf,char ch)
{
    for(int i=0;i<sizeBuf;i++)
        if(str[-i]==ch)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
int     WSFindNoChar(char *str,int sizeBuf,char ch)
{
    for(int i=0;i<sizeBuf;i++)
        if(str[i]!=ch)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
int     WSFindCharFromEndStr(char *str,int len,char ch)
{
    for(int i=len-1;i>=0;i--)
        if(str[i]==ch)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
bool    WSIsSymbol(char ch)
{
    if(ch=='_') return true;
    if(ch>='0' && ch<='9') return true;
    if(ch>='A' && ch<='Z') return true;
    if(ch>='a' && ch<='z') return true;
    if(ch>='А' && ch<='я') return true;
    if(ch=='ё') return true;
    return false;
}
//---------------------------------------------------------------------------
int     FindDig(char *str,int len,char *dig,int &size)
{
    int pos=0,posd=0,beg=-1;
    for(;pos<len;pos++)
    {
        char ch=str[pos];
        if(WSSmblIsDig(ch))
        {
            if(posd==0) beg=pos;
            dig[posd++]=str[pos];
        }
        else if(posd>0) break;
    }
    dig[posd]='\0';
    size=pos;
    return beg;
}
//---------------------------------------------------------------------------
/*clCard  WSReadCard(char *word)
{
    clCard card;
    switch(word[0])
    {
        case '2' : card.face=C_2;break;
        case '3' : card.face=C_3;break;
        case '4' : card.face=C_4;break;
        case '5' : card.face=C_5;break;
        case '6' : card.face=C_6;break;
        case '7' : card.face=C_7;break;
        case '8' : card.face=C_8;break;
        case '9' : card.face=C_9;break;
        case 'T' : card.face=C_T;break;
        case 'J' : card.face=C_J;break;
        case 'Q' : card.face=C_Q;break;
        case 'K' : card.face=C_K;break;
        case 'A' : card.face=C_A;break;
        default  : card.face=C_UNDEF;return card;
    }
    switch(word[1])
    {
        case 'c' : card.suit=S_C;break;
        case 'd' : card.suit=S_D;break;
        case 'h' : card.suit=S_H;break;
        case 's' : card.suit=S_S;break;
        default  : card.face=C_UNDEF;break;
    }
    return card;
}*/
//---------------------------------------------------------------------------
double  WSReadDouble(char *str,int len,int &size)
{
    double x1=WSReadInt(str,len,size);
    int m=1;
    if(x1==0)
    {
        for(int i=0;i<len;i++)
            if(WSSmblIsDig(str[i]))
            {
                if(i>0 && str[i-1]=='-')
                    m=-1;
                break;
            }
    }

    if(str[size]=='.' || str[size]==',')
        if(WSSmblIsDig(str[size+1]))
        {
            int size1;
            int len0=len-size;
//            if(len0>9) len0=9;
            double x2=WSReadInt64(str+size,len0,size1);
            for(int i=0;i<size1-1;i++)
                x2/=10;
            x1+=x2;
            size+=size1;
        }


    return(m*x1);

}
//---------------------------------------------------------------------------
int     WSReadMoney(char *str,int len,int &size,char t,char c)
{
    int size0;
    int x1=WSReadInt(str,len,size);
    if(str[size]==t)
    {
        if(WSSmblIsDig(str[size+1]))
        {
            int mon=WSReadInt(str+size,len-size,size0);
            x1=x1*1000+mon;
            size+=size0;
            if(str[size]==t)
            {
                if(WSSmblIsDig(str[size+1]))
                {
                    mon=WSReadInt(str+size,len-size,size0);
                    x1=x1*1000+mon;
                    size+=size0;
                }
            }
        }
    }
    x1*=100;
    if(str[size]==c)
        if(WSSmblIsDig(str[size+1]))
        {
            int size1;
            int x2=WSReadInt(str+size,len-size,size1);
            if(size1==1)
                x2*=10;
            x1+=x2;
            size+=size1;
        }
    return x1;
}
//---------------------------------------------------------------------------
int     WSReadChips(char *str,int len,int &size)
{
    int size0;
    int x1=WSReadInt(str,len,size);
    if(str[size]==',' || str[size]=='\'')
    {
        if(!WSSmblIsDig(str[size+1])) return(x1);
        int mon=WSReadInt(str+size+1,len-size,size0);
        if(size0 != 3)
        {
            return x1;
        }
        x1=x1*1000+mon;
        size+=size0;
        if(str[size]==',' || str[size]=='\'')
        {
            if(!WSSmblIsDig(str[size+1])) return(x1);
            mon=WSReadInt(str+size,len-size,size0);
            x1=x1*1000+mon;
            size+=size0;
        }
    }
    return (x1<0)? 0 : x1;
}
//---------------------------------------------------------------------------
int     WSShiftFirstSmbl(char *str,int len)
{
    for(int i=0;i<len;i++)
        if(WSIsSymbol(str[i]))
            return  i;
    return -1;
}
//---------------------------------------------------------------------------
bool    WSFirstSmblIsDig(char *str,int len)
{
    for(int i=0;i<len;i++)
        if(WSIsSymbol(str[i]))
            return  WSSmblIsDig(str[i]);
    return false;
}
//---------------------------------------------------------------------------
clAnsiString WSDoubleToAS(double val,int r)
{
	char str[128];
	double v0 = val;
	int len = 0;
	if (v0 < 0)
	{
		str[len++] = '-';
		v0 = -v0;
	}
	int a = v0;
	int b0 = 0;
	if (a > 0)
	{
		_itoa(a, str+len, 10);
		len = strlen(str);
		str[len++]='.';
		v0 -= a;
		b0 = (len<r)? 2 : 0;
	}
	else
	{
		str[len] = '0';
		str[len+1]='.';
		len += 2;
	}
	while (len<=r)
	{
		v0 *= 10;
		a = v0;
		v0 -= a;
		str[len++] ='0'+a;
		if (a > 0) b0++;
	}
	if (b0<2)
	{
		char str[128];
		_gcvt(val,r,str);
		clAnsiString as=str;
		return as;
	}
	str[len] = '\0';
	clAnsiString as=str;
	return as;
}
//---------------------------------------------------------------------------
clAnsiString WSReadFileName(char *str)
{
    clAnsiString res;
    char word[256];
    int len=strlen(str);
    int size=WSReadWord(str,len,word);
    res=word;
    if(str[size]=='.')
    {
        WSReadWord(str+size,len-size,word);
		res += (clAnsiString)'.' + (clAnsiString)word;
    }
    return res;
}
//---------------------------------------------------------------------------
clAnsiString WSDoubleToProc(double val,int r)
{
    return (clAnsiString)(((int)(val*1000))/10.)+(clAnsiString)"%";
}
//---------------------------------------------------------------------------

#pragma package(smart_init)