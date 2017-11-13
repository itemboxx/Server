//---------------------------------------------------------------------------
#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <io.h>
#include <math.h>

#include "c:\Home\UTIL\FuncDPoint.h"

using namespace std;
//---------------------------------------------------------------------------
double  clDPoint::Distance(clDPoint p0)
{
	tpFloat dx = p0.x - x, dy = p0.y - y;
	return sqrt(dx*dx+dy*dy);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
tpFloat	PerpendicularDistance(clDPoint p, clDPoint a1, clDPoint a2)
{
	if (a1.x == a2.x)
		return fabs(p.x - a1.x);
	else if (a1.y == a2.y)
		return fabs(p.y - a1.y);
	tpFloat A = 1 / (a2.x - a1.x);
	tpFloat B = 1 / (a1.y - a2.y);
	tpFloat C = a1.x / (a1.x - a2.x) + a1.y / (a2.y - a1.y);
	return fabs(A*p.x + B*p.y + C) / sqrt(A*A + B*B);
}
//---------------------------------------------------------------------------
size_t	DouglasPeucker(size_t dim, clDPoint *arr, tpFloat epsilon)
{
	//Ќаходим точку с максимальным рассто€нием от пр€мой между первой и последней точками набора
	tpFloat dmax = 0;
	size_t index = 0;
	for (size_t i = 1; i < dim - 1; i++)
	{
		tpFloat d = PerpendicularDistance(arr[i], arr[0], arr[dim - 1]);
		if (d > dmax)
		{
			index = i;
			dmax = d;
		}
	}
				//≈сли максимальна€ дистанци€ больше, чем epsilon, то рекурсивно вызываем еЄ на участках
	if (dmax >= epsilon)
	{
		size_t d1 = DouglasPeucker(index + 1, arr, epsilon);
		size_t d2 = DouglasPeucker(dim - index, arr + index, epsilon);
		// —троим итоговый набор точек
		memcpy(arr + d1, arr + index + 1, (d2 - 1)*sizeof(clDPoint));
		return d1 + d2 - 1;
	}
	else
	{
		arr[1] = arr[dim - 1];
		return 2;
	}
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
