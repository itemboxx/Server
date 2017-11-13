//---------------------------------------------------------------------------
#pragma hdrstop

#include "c:\Home\Util\WorkStr.h"
#include "c:\Home\Util\IntDiap.h"

//---------------------------------------------------------------------------
template <typename _T> clIntDiap<_T>::clIntDiap(_T min,_T max)
{
    _min=min;
    _max=max;
}
//---------------------------------------------------------------------------
template <typename _T> void    clIntDiap<_T>::IncludeOn(clIntDiap<_T> val)
{
    if(!val.DefineDiap())
        Clear();
    if(DefineDiap())
    {
        if(val._min<_min)
            _min=val._min;
        if(val._max >_max)
            _max=val._max;
    }
}
//---------------------------------------------------------------------------
template <typename _T> void    clIntDiap<_T>::IncludeOn(int val)
{
    if(!DefineDiap())
        _min=_max=val;
    else
    {
        if(val<_min)
            _min=val;
        if(val >_max)
            _max=val;
    }
}
//---------------------------------------------------------------------------
template <typename _T> bool    clIntDiap<_T>::Crossing(clIntDiap val)
{
    if(!DefineDiap() || !val.DefineDiap()) return true;
    if(_min>=val._min && _min<val._max) return true;
    if(_max>val._min && _max<=val._max) return true;
    if(_min<=val._min && _max>=val._max) return true;
    return false;
}
//---------------------------------------------------------------------------
clIntDiap<int>   IntDiap(int min,int max)
{
    clIntDiap <int> res;
        res._max=max;
        res._min=min;
    return res;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
