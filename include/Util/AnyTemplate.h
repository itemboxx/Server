//---------------------------------------------------------------------------

#ifndef AnyTemplateH
#define AnyTemplateH
//#include "c:\Home\Util\AnyTemplate.h"
//---------------------------------------------------------------------------
template <typename T> double  DistForVect(int size,T *vect1,T *vect2)
{
    double res=0;
    for(int i=0;i<size;i++)
    {
        double d=vect1[i]-vect2[i];
        res+=d*d;
    }
    return sqrt(res);
}
//---------------------------------------------------------------------------
template <typename T> void  TempChange(T &val1,T &val2)
{
    T temp;
    temp=val1;
    val1=val2;
    val2=temp;
}
//---------------------------------------------------------------------------
template <typename T> void  TempNormVect(int size, T *vect, double sum)
{
	double res = 0;
	for (int i = 0; i<size; i++)
		res += vect[i];
	if (res > DOUBLE_0)
	{
		res = sum / res;
		for (int i = 0; i < size; i++)
			vect[i] *= res;
	}
}
//---------------------------------------------------------------------------
#endif
