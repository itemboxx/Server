//---------------------------------------------------------------------------
//#include "stdafx.h"
#pragma hdrstop

#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include <share.h>
#include <afxwin.h>

#include "GlobalsForCalc.h"
#include "clHandsCompare.h"
#include "SeveralHandCmp.h"
#include "..\Strategy\Source\HandsGroupEX.h"
#include "FlopAllIn.h"
#include "..\..\Util\errMessage.h"

//#define	FILE_FLOP_ALLIN		"c:\\data\\flopAllin.bin"

void	TestFunc();
//---------------------------------------------------------------------------
bool    GlobalsForCalcLoad(int reg)
{
	//TestFunc();
	int handle; _sopen_s(&handle, "c:\\Home\\bin\\comp0.tmp", O_RDWR | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (handle == -1)
	{
		AfxMessageBox(_T("Не открывается файл c:\\Home\\bin\\comp0.tmp"));
		return false;
	}
	glHandCmp.ReadFileAll(handle);
	_close(handle);
	SHCMemSet();
	if (!SHCLoadFromFile("c:\\Home\\bin\\SeveralHandCmp.bin"))
	{
		AfxMessageBox(_T("Не открывается файл c:\\Home\\bin\\SeveralHandCmp.bin"));
		return false;
	}
	glListHand.LoadFromFile("C:\\Home\\bin\\ListHandAllIn_24.bin");
	if (reg)
		if (!LoadDataHandsBoard(reg))
		{
			AfxMessageBox(_T("Не открывается файл c HandsBoard"));
			return false;
		}
	/*
	if (reg & REG_FLOP_ALLIN)
	{
		glVectFlopAllin.resize(CN_NABOR_HAND_RIVER);
		int handle = _open("c:\\Home\\bin\\flopAllin.bin", O_CREAT | O_RDWR | O_BINARY);
		if (handle == -1)
		{
			AfxMessageBox(_T("Не открывается файл c:\\Home\\bin\\flopAllin.bin"));
			return false;
		}
		ReadBigBlock(handle, &glVectFlopAllin[0], CN_NABOR_HAND_RIVER * sizeof(float));
		_close(handle);
	}
	*/
	return true;
}
//---------------------------------------------------------------------------
void    CloseGlobalsForCalc()
{
	SHCMemFree();
}
//---------------------------------------------------------------------------
template <class T> void MyMemset8(T *buf, T val0, size_t cn)
{
	size_t size = cn * sizeof(val0);
	struct st1
	{
		byte _val[8];
	}val, *ptr = (st1 *)buf;
	memcpy(&val, &val0, sizeof(val0));
	for (short i = sizeof(val0); i < 8; i *= 2)
		memcpy(val._val + i, &val, i);
	cn = cn * sizeof(val0) / 8;
	for (size_t i = 0; i < cn; i++)
		ptr[i] = val;
}
void	TestFunc()
{
	srand((unsigned)time(NULL));
	int n1 = random(3);
	int n2 = random(3);

	typedef int typeX;
	typeX val = -1;
	size_t cn = CN_NABOR_HAND_TURN, size = cn * sizeof(val), cnC = 10;
	typeX *buf = (typeX *)malloc(size);

	//ZeroMemory(_indTurn, CN_NABOR_HAND_TURN*sizeof(int));

	size_t t0 = GetTickCount();
	for (size_t k = 0; k < cnC; k++)
		for (size_t i = 0; i < cn; i++)
			buf[i] = val;
	size_t t1 = GetTickCount() - t0;
	t0 = GetTickCount();
	for (size_t k = 0; k < cnC; k++)
		memset(buf, val, size);
	//	ZeroMemory(buf, size);
	size_t t2 = GetTickCount() - t0;
	t0 = GetTickCount();
	//cnC *= 1000;
	cn /= 8 / sizeof(val);
	for (size_t k = 0; k < cnC; k++)
	{
		/*		struct st1
				{
					byte _val[8];
				}val8, *ptr = (st1 *)buf;
				memcpy(&val8, &val, sizeof(val));
				for (short i = sizeof(val); i < 8; i *= 2)
					memcpy(val8._val + i, &val8, i);
				cn = cn*sizeof(val) / 8;
				for (size_t i = 0; i < cn; i++)
					ptr[i] = val8;*/
		__int64 v64 = val, *ptr = (__int64 *)buf;
		v64 += v64 << 4;
		for (size_t i = 0; i < cn; i++)
			ptr[i] = v64;
	}
	//MyMemset8(buf, val, cn);
	size_t t3 = GetTickCount() - t0;
	t0 = GetTickCount();
}
//---------------------------------------------------------------------------

//#pragma package(smart_init)
