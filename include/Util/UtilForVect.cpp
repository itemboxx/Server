//---------------------------------------------------------------------------
#pragma hdrstop

#include <io.h>
#include <math.h>

#include "usefullProc.h"
#include "UtilForVect.h"
#include "errMessage.h"

//---------------------------------------------------------------------------
void    VectDoubleWriteFile(vector<double> &vect,int handle)
{
    int cn=(int)vect.size();
    _write(handle,&cn,sizeof(cn));
    _write(handle,&vect.begin()[0],cn*sizeof(double));
}
//---------------------------------------------------------------------------
void    VectDoubleReadFile(vector<double> &vect,int handle)
{
    int cn;
    _read(handle,&cn,sizeof(cn));
    vect.resize(cn);
    _read(handle,&vect.begin()[0],cn*sizeof(double));
}
//---------------------------------------------------------------------------
double    VectDoubleAverDistanceAndDisp(vector<vector <double> > &val1,vector<vector <double> > &val2,double &disp)
{
	int size = (int)val1.size();
	if (size != (int)val2.size())
        return -1;
    double *arrD=(double *)malloc(size*sizeof(double));
    if(arrD==NULL) return -1;
    for(int i=0;i<size;i++)
        arrD[i]=VectDoubleDistance(val1[i],val2[i]);
    double res=AverageAndDisp(size,arrD,disp);
    free(arrD);
    return res;
}
//---------------------------------------------------------------------------
double  VectDoubleDistance(vector<double> &vect1,vector<double> &vect2)
{
    if(vect1.size() != vect2.size()) return -1;
    double res=0;
    for(int i=0;i<vect1.size();i++)
    {
        double d=vect1[i]-vect2[i];
        res+=d*d;
    }
    return sqrt(res);
}
//---------------------------------------------------------------------------
void    VectDoubleNorm(vector<double> &vect,double sum)
{
    double sum0=0;
    for(int i=0;i<vect.size();i++)
        sum0+=vect[i];
    if(sum0<DOUBLE_0)
    {
		int cn = (int)vect.size();
        return;
    }
    double mult=(double)sum/sum0;
    for(int i=0;i<vect.size();i++)
        vect[i]*=mult;
}
//---------------------------------------------------------------------------
void    VectIntNorm(vector<int> &vect,int sum)
{
    int sum0=0;
    for(int i=0;i<vect.size();i++)
        sum0+=vect[i];
    double mult=(double)sum/sum0;
    for(int i=0;i<vect.size();i++)
    {
		vect[i] = (int)(vect[i] * mult);
        if(vect[i]==0) vect[i]=1;
    }
    sum0=0;
    for(int i=0;i<vect.size();i++)
        sum0+=vect[i];
    sum0-=sum;
    if(sum0>0)
    {
        for(int i=0;i<vect.size();i++)
            if(vect[i]>sum0+1)
            {
                vect[i]-=sum0;
                break;
            }
    }
    else if(sum0<0)
        for(int i=0;i<-sum0;i++)
            vect[i]++;
}
//---------------------------------------------------------------------------

