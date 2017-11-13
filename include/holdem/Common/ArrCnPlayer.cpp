//---------------------------------------------------------------------------
//#include "stdafx.h"
#pragma hdrstop

#include <math.h>

#include "ArrCnPlayer.h"

//---------------------------------------------------------------------------
void    clDoubleCP::FillArr(tpFloat0 val)
{
    for(int i=0;i<CN_PLAYER;i++)
        _arr[i]=val;
}
//---------------------------------------------------------------------------
clDoubleCP clDoubleCP::operator * (tpFloat0 val)
{
    clDoubleCP res;
    for(int i=0;i<CN_PLAYER;i++)
        res[i]=_arr[i]*val;
    return res;
}
//---------------------------------------------------------------------------
void    clDoubleCP::operator +=(clDoubleCP val)
{
    for(int i=0;i<CN_PLAYER;i++)
        _arr[i]+=val[i];
}
//---------------------------------------------------------------------------
void    clDoubleCP::operator =(tpFloat0 val)
{
    for(int i=0;i<CN_PLAYER;i++)
        _arr[i]=val;
}
//---------------------------------------------------------------------------
void    clDoubleCP::operator /=(tpFloat0 val)
{
    for(int i=0;i<CN_PLAYER;i++)
        _arr[i]/=val;
}
//---------------------------------------------------------------------------
void    clDoubleCP::Norm(tpFloat0 norm0)
{
	tpFloat0 sum = Sum() / norm0;
    if(sum>DOUBLE_0)
        for(int i=0;i<CN_PLAYER;i++)
            _arr[i]/=sum;
}
//---------------------------------------------------------------------------
tpFloat0  clDoubleCP::Sum()
{
	tpFloat0 sum = _arr[0];
    for(int i=1;i<CN_PLAYER;i++)
        sum+=_arr[i];
    return sum;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
clIntCP clIntCP::operator * (double val)
{
    clIntCP res;
    for(int i=0;i<CN_PLAYER;i++)
        res[i]=(int)(_arr[i]*val);
    return res;
}
//---------------------------------------------------------------------------
void    clIntCP::operator +=(clIntCP val)
{
    for(int i=0;i<CN_PLAYER;i++)
        _arr[i]+=val[i];
}
//---------------------------------------------------------------------------
void    clIntCP::operator =(int val)
{
    for(int i=0;i<CN_PLAYER;i++)
        _arr[i]=val;
}
//---------------------------------------------------------------------------
bool    clIntCP::operator ==(clIntCP val)
{
    for(int i=0;i<CN_PLAYER;i++)
        if(_arr[i]!=val[i])
            return false;
    return true;
}
//---------------------------------------------------------------------------
int     clIntCP::Sum()
{
    int sum=_arr[0];
    for(int i=1;i<CN_PLAYER;i++)
        sum+=_arr[i];
    return sum;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
double  ArrCnPlDist(clDoubleCP res0,clDoubleCP res)
{
    double dist=0;
    for(int i=0;i<CN_PLAYER;i++)
        dist+=(res0[i]-res[i])*(res0[i]-res[i]);
    return sqrt(dist);
}
//---------------------------------------------------------------------------

