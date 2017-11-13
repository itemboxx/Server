//---------------------------------------------------------------------------

#ifndef IntDiapH
#define IntDiapH
//---------------------------------------------------------------------------
#include "c:\Home\Util\WorkStr.h"

template <typename _T>
class clIntDiap
{
public :
    inline  clIntDiap()                 {Clear();}
    clIntDiap(_T min,_T max);
void    Clear()
{
    _min=1;
    _max=0;
}
    void    operator=(int v)			{SetDiap(v,v);}
    bool	operator==(int v)           {return (Single()&& _min==v);}
void    SetDiap(char *str)
{
    int len=strlen(str);
    if(WSFirstSmblIsDig(str,len))
    {
        int size;
//        _def=true;
        _min=WSReadInt(str,len,size);
        if(size==len)
            _max=_min;
        else
            _max=WSReadInt(str+size,len-size,size);
    }
    else
//        _def=false;
        Clear();
}
void    SetDiap(char *str,int len,char d=':')
{
    if(WSFirstSmblIsDig(str,len))
    {
        int size;
        _max=_min=WSReadInt(str,len,size);
        if(size<len)
        {
            str+=size;
            len-=size;
            if(str[0]==d)
                _max=WSReadInt(str,len,size);
        }
    }
    else
//        _def=false;
        Clear();
}
    inline  void    SetDiap(_T min,_T max)    {_min=min;_max=max;}
clAnsiString ToString()
{
    clAnsiString as;
    if(DefineDiap())
    {
        as=_min;
        if(_max != _min)
        {
            as+=" : ";
            as+=_max;
        }
    }
    else
        as="Не важно";
    return as;
}
    inline  void    ReadString(clAnsiString as)   {SetDiap(as.c_str());}
bool    Include(int val)
{
    if(DefineDiap()) if(val<_min || val >_max) return false;
    return true;
}
bool    Include(clIntDiap <_T> val)
{
    if(DefineDiap())
    {
        if(!val.DefineDiap()) return false;
        if(val._min<_min || val._max >_max) return false;
    }
    return true;
}
    void    IncludeOn(clIntDiap <_T> val);
    void    IncludeOn(int val);
    inline  bool    Single()    {return (_min==_max);}
    bool    Crossing(clIntDiap val);
    inline  bool    DefineDiap()        {return _min<=_max;}

    _T _min,_max;
};

clIntDiap<int>   IntDiap(int min,int max);
//---------------------------------------------------------------------------
#endif
