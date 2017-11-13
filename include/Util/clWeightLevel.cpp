//---------------------------------------------------------------------------
#pragma hdrstop

#include "c:\Home\Util\clWeightLevel.h"

//---------------------------------------------------------------------------
bool	clWeightLevel::Load(int dim, float *arr)
{
	resize(dim);
	float level = 0;
	for (int i = 0; i < dim; i++)
	{
		(*this)[i]._weight = arr[i];
		(*this)[i]._level = level;
		level += arr[i];
	}
	return (fabs(level-1)< 0.001f);
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
 