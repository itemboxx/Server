//---------------------------------------------------------------------------
#pragma hdrstop

#include "TemplateFileRec.h"

//---------------------------------------------------------------------------
size_t ReadBigBlock(int handle, void *ptr, size_t sizeA)
{
	size_t step = 0x40000000, res = 0;
	byte *buf = (byte *)ptr;
	while (sizeA > 0)
	{
		size_t size = (sizeA > step) ? step : sizeA;
		int val = _read(handle, buf, size);
		res += val;
		if (val != size) break;
		sizeA -= size;
		buf += size;
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t WriteBigBlock(int handle, void *ptr, size_t sizeA)
{
	size_t step = 0x40000000, res = 0;
	byte *buf = (byte *)ptr;
	while (sizeA > 0)
	{
		size_t size = (sizeA > step) ? step : sizeA;
		int val = _write(handle, buf, size);
		res += val;
		if (val != size) break;
		sizeA -= size;
		buf += size;
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma package(smart_init)
 