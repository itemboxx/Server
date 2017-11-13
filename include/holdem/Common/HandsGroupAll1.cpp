//---------------------------------------------------------------------------
//#include "stdafx.h"
#pragma hdrstop

#include <math.h>
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>

#include "C:\home\holdem\Common\HandsGroupAll.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat tpFloatArrAllHands::Distance(tpFloatArrAllHands &arr)
{
	double res = 0;
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		double d0 = _arr[i] - arr[i];
		res += d0*d0;
	}
	return sqrt(res);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void    clGroupHandsAll::ReadFileFloat(int handle)
{
	float val[ALL_CN_HAND];
	_read(handle, val, sizeof(val));
	for (int i = 0; i < ALL_CN_HAND; i++)
		_arr[i] = val[i];
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat	clGroupHandsAll::Norm(tpFloat val)
{
	double sum = 0;
	for (int i = 0; i < ALL_CN_HAND; i++)
		sum += _arr[i];
	if (sum > DOUBLE_0)
	{
		val /= sum;
		for (int i = 0; i < ALL_CN_HAND; i++)
			_arr[i] *= val;
	}
	return sum;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
double 	clGroupHandsAll::Weight()
{
	double sum = 0;
	for (int i = 0; i < ALL_CN_HAND; i++)
		sum += _arr[i];
	return sum;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clGroupHandsAll::SkipCard(tpCard c0)
{
	for (int i = 0; i < 52; i++)
		if (i != c0)
		{
			int nb = (i>c0) ? i*(i - 1) / 2 + c0 : c0*(c0 - 1) / 2 + i;
			_arr[nb] = 0;
		}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma package(smart_init)
