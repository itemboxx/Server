//---------------------------------------------------------------------------
#pragma hdrstop

#include <math.h>
#include <stdlib.h>
#include "c:\home\util\usefullProc.h"

//---------------------------------------------------------------------------
void    SortArr(int cn,double *arr)
{
    for(int i=0;i<cn;i++)
    {
        int nb=i;
        double temp;
        for(int j=i+1;j<cn;j++)
            if(arr[nb]<arr[j])
                nb=j;
        temp=arr[i];
        arr[i]=arr[nb];
        arr[nb]=temp;
    }
}
//---------------------------------------------------------------------------
void    SortArr(int cn,int *arr)
{
    for(int i=0;i<cn;i++)
    {
        int nb=i,temp;
        for(int j=i+1;j<cn;j++)
            if(arr[nb]<arr[j])
                nb=j;
        temp=arr[i];
        arr[i]=arr[nb];
        arr[nb]=temp;
    }
}
//---------------------------------------------------------------------------
void    SortArr(int cn,int *arr,int *arrNb)
{
    for(int i=0;i<cn;i++)
        arrNb[i]=i;
    for(int i=0;i<cn;i++)
    {
        int nb=i,temp;
        for(int j=i+1;j<cn;j++)
            if(arr[nb]<arr[j])
                nb=j;
        temp=arr[i];
        arr[i]=arr[nb];
        arr[nb]=temp;
        temp=arrNb[i];
        arrNb[i]=arrNb[nb];
        arrNb[nb]=temp;
    }
}
//---------------------------------------------------------------------------
double  Average(int cn,double *arr)
{
    if(cn==0) return 0;
    double res=arr[0];
    for(int i=1;i<cn;i++)
        res+=arr[i];
    return res/cn;
}
//---------------------------------------------------------------------------
double  Average(vector <stResCount> &vect)
{
    int cn=vect.size(),cnAll=0;
    if(cn==0) return 0;
    double res=0;
    for(int i=0;i<cn;i++)
    {
        res+=vect[i]._res*vect[i]._cn;
        cnAll+=vect[i]._cn;
    }
    return res/cnAll;
}
//---------------------------------------------------------------------------
double  AverageAndDisp(int cn,double *arr,double &disp)
{
    if(cn==0) return 0;
    double aver=Average(cn,arr);
    disp=0;
    for(int i=0;i<cn;i++)
    {
        double x=arr[i]-aver;
        disp+=x*x;
    }
    disp=sqrt(disp/cn);
    return aver;
}
//---------------------------------------------------------------------------
double  AverageAndDisp(double &disp,vector <stResCount> &vect)
{
    int cn=vect.size(),cnAll=0;
    if(cn==0) return 0;
    double aver=Average(vect);
    disp=0;
    for(int i=0;i<cn;i++)
    {
        double x=vect[i]._res-aver;
        disp+=x*x*vect[i]._cn;
        cnAll+=vect[i]._cn;
    }
    if(cnAll==0) return 0;
    disp=sqrt(disp/cnAll);
    return aver;
}
//---------------------------------------------------------------------------
double  DistBetweenArr(int cn,double *arr1,double *arr2)
{
    double res=0;
    for(int i=0;i<cn;i++)
    {
        double d=arr1[i]-arr2[i];
        res+=d*d;
    }
    return sqrt(res);
}
//---------------------------------------------------------------------------
double  UsefullMax(int cn,double *arr)
{
    if(cn==0) return 0;
    double res=arr[0];
    for(int i=1;i<cn;i++)
        if(res<arr[i])
            res=arr[i];
    return res;
}
//---------------------------------------------------------------------------
double  UsefullMin(int cn,double *arr)
{
    if(cn==0) return 0;
    double res=arr[0];
    for(int i=1;i<cn;i++)
        if(res>arr[i])
            res=arr[i];
    return res;
}
//---------------------------------------------------------------------------
int  UsefullNbMax(int cn,double *arr)
{
    if(cn==0) return -1;
    int nb=0;
    for(int i=1;i<cn;i++)
        if(arr[nb]<arr[i])
            nb=i;
    return nb;
}
//---------------------------------------------------------------------------
int  UsefullNbMin(int cn,double *arr)
{
    if(cn==0) return -1;
    int nb=0;
    for(int i=1;i<cn;i++)
        if(arr[nb]>arr[i])
            nb=i;
    return nb;
}
//---------------------------------------------------------------------------
int     UsefullMax(int cn,int *arr)
{
    if(cn==0) return 0;
    int res=arr[0];
    for(int i=1;i<cn;i++)
        if(res<arr[i])
            res=arr[i];
    return res;
}
//---------------------------------------------------------------------------
int     UsefullMin(int cn,int *arr)
{
    if(cn==0) return 0;
    int res=arr[0];
    for(int i=1;i<cn;i++)
        if(res>arr[i])
            res=arr[i];
    return res;
}
//---------------------------------------------------------------------------
double  CalcDisp(double m,int cn)
{
    double disp=(1-m);
    disp=cn*m*disp*disp;
    disp+=cn*(1-m)*m*m;
    return sqrt(disp)/cn;
}
//---------------------------------------------------------------------------
void    RandNorm(double &v1,double &v2) // Две случайные величины распределенных по нормальному закону с матожиданием 0 и дисперсией 1
{
    double r1,r2;
    int d2=RAND_MAX/2+1;
    r1=rand();
    r1=(r1-d2)/d2;
    r2=rand();
    r2=(r2-d2)/d2;
    if(r1==0 && r2==0)
    {
        RandNorm(v1,v2);
        return;
    }
    double s=r1*r1+r2*r2;
    if(s>1)
    {
        RandNorm(v1,v2);
        return;
    }
    s=sqrt(-2*log(s)/s);
    v1=r1*s;
    v2=r2*s;
}
//---------------------------------------------------------------------------
double  RandNorm() // Случайная величины распределенная по нормальному закону с матожиданием 0 и дисперсией 1
{
    double r1,r2;
    RandNorm(r1,r2);
    return r1;
}
//---------------------------------------------------------------------------
double  CalcDispME(double m)
{
    double disp=(1-m);
    disp=m*disp*disp;
    disp+=(1-m)*m*m;
    return sqrt(disp);
}
//---------------------------------------------------------------------------
double  AmendCalc(double cnS,double w,double midlW)
{
    if(fabs(w-midlW)<1.E-10) return w;
    double val=midlW;
    if(midlW<0.05) val=0.05;
    double disp=CalcDispME(val)/sqrt(cnS);
    double m0=(w<0.5)? w : 1-w;
    double m1=(midlW<0.5)? midlW : 1-midlW;
    if(w<1.E-10 && midlW<1.E-10) return midlW;
    double k=disp/((m0>m1)? m0 : m1);
    if(k<0.08)
        return w;
    else if(k<0.25)
    {
        if(midlW>w)
        {
            w+=disp/2;
            return (w>midlW)? midlW : w;
        }
        else
        {
            w-=disp/2;
            return (w<midlW)? midlW : w;
        }
    }
    else
    {
        if(midlW>w)
        {
            w+=disp;
            return (w>midlW)? midlW : w;
        }
        else
        {
            w-=disp;
            return (w<midlW)? midlW : w;
        }
    }
}
//---------------------------------------------------------------------------
double  AverageAndDispOnRound(int cn,double *arr,double &disp)
{
    for(int i=0;i<cn;i++)
        arr[i]-=(int)arr[i];
    double *temp=new double[cn],aver;
    disp=-1;
    for(int i=0;i<37;i++)
    {
        double da=i/37.;
        double err=0;
        for(int j=0;j<cn;j++)
        {
            temp[j]=arr[j]-da;
            if(temp[j]<0)
                temp[j]=1-temp[j];
            if(temp[j]>0.5)
                temp[j]=1-temp[j];
            err+=temp[j]*temp[j];
        }
        if(disp<0 || disp>err)
        {
            disp=err;
            aver=da;
        }
    }
    delete temp;
    disp=sqrt(disp/cn);
    return aver;
}
//---------------------------------------------------------------------------
bool    Compare2Val(double val1,double val2,double dif) // если val1 отличается от val2 меньше чем в dif return true;
{
    double abs1=fabs(val1),abs2=fabs(val2);
    if(abs1>abs2)
    {
        if(abs1<1.E-10) return false;
        return (fabs(val1-val2)/abs1<dif);
    }
    else
    {
        if(abs2<1.E-10) return false;
        return (fabs(val1-val2)/abs2<dif);
    }
}
//---------------------------------------------------------------------------
int     Combination(int cnAll,int cn)  // сочетание cn из cnAll
{
    if(cn==0) return 0;
    if(cn>cnAll-cn) cn=cnAll-cn;
    __int64 res=cnAll;
    for(int i=1;i<cn;i++)
    {
        res*=(cnAll-i);
        res/=i+1;
    }
    return res;
}
//---------------------------------------------------------------------------
inline  double  ProbabilityExc(int cnAll,int cnRound,int cn,int cnVar)  // вероятность что прийде ровно cn карт из cnVar
{
    double res=Combination(cnRound,cn);
    if(res<0.5) res=1;
    for(int i=0;i<cnRound-cn;i++)
        res*=(double)(cnAll-cnVar-i)/(cnAll-i);
    cnAll-=cnRound-cn;
    for(int i=0;i<cn;i++)
        res*=(double)(cnVar-i)/(cnAll-i);
    return res;
}
//---------------------------------------------------------------------------
double  ProbabilityOver(int cnAll,int cnRound,int cn,int cnVar)  // например для флеша cn из cnVar
{
    if(cnVar==0 || cn>cnVar) return 0;
    double res=0;
    for(int i=0;i<cn;i++)
        res+=ProbabilityExc(cnAll,cnRound,i,cnVar);
    return 1-res;
}
//---------------------------------------------------------------------------
bool    CompareAnyBytes(int cn,BYTE *arr,BYTE *arr1)
{
    BYTE *ptr=(BYTE *)malloc(cn);
    memset(ptr,1,cn);
    bool res=true;
    for(int i=0;i<cn;i++)
    {
        int i1=0;
        for(;i1<cn;i1++)
            if(ptr[i1] && arr[i]==arr1[i])
            {
                ptr[i1]==0;
                break;
            }
        if(i1==cn)
        {
            res=false;
            break;
        }
    }
    free(ptr);
    return res;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
