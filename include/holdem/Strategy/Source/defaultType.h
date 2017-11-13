//---------------------------------------------------------------------------

#ifndef DefaultType_H
#define DefaultType_H
//---------------------------------------------------------------------------
using namespace std;

#include "..\..\Common\sitholdem.h"
#include "HandsGroupEX.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
extern double MAX_NEAR_DIST;

struct tpDisNode
{
	void MultDis(double k)
	{
		_dis._money = int(_dis._money*k + 0.5);
		_ev *= k;
	}

	tpDis _dis;
	double _ev, _weight;
	int _cnCalc;
};

struct stNbBtw
{
	short _nb0, _nb1;
	float _w0, _w1;
};

struct tpTableChangeNb
{
	void	ZeroData() { ZeroMemory(_arr, sizeof(_arr)); }
	void	CreateSimple()
	{
		for (int i = 0; i < ALL_CN_HAND; i++)
		{
			stNbBtw val = { i,0,1,0 };
			_arr[i] = val;
		}
	}

	stNbBtw _arr[ALL_CN_HAND];
};

struct tpRefHero
{
	stNbBtw _refNb;
	int _nbTree, _stack;
	double _weight;
};

struct tpRefTree
{
	tpTableChangeNb _tab[2];
	int _nbTree, _stack;
	double _weight;
};

struct tpIncomeFloatData
{
	float _gh[2][ALL_CN_HAND];
	clSitHoldem _sit;
};

struct tpIncomeFloatData3
{
	float _gh[3][ALL_CN_HAND];
	clSitHoldem _sit;
};
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
