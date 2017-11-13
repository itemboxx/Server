//---------------------------------------------------------------------------

#ifndef StrategyPlayEXH
#define StrategyPlayEXH
//---------------------------------------------------------------------------
#include "StrategyPlay.h"
#include "..\..\Strategy\AppTurnTree\Source\StrategyStreet.h"

class clStrategyPlayEX
{
public:

	bool	Init() { return _stratNew.Init(); }
	bool	Load();
	tpDis	DecideRemote(clSitHoldem &sit, tpArrResBranch8 &arr, int msec);
	tpDis	GetDis(clSitHoldem &sit, vector <tpDisNode> &vect, clRoot &hs, int mSec = 0);
	bool	FindGHFlopEx(clRoot *hs, clSitHoldem &sit);
	//void	FindGHRiverEx(clRoot *hs, clSitHoldem &sit);
	bool	CreateHistCalc(clRoot &hs, clSitHoldem &sit);
	bool	ClearOldHistCalc(int tSec);

	clStrategyPlay _stratOld;
	clStrategyH _stratNew;
	HANDLE  _lockEX;
	clFileRecT <clRoot> _fileR;

	struct tpHistCalcAtr
	{
		__int64 _nbHand;
		int _curAct;
		time_t _time;
		clRoot _histCalc;
	};
	vector <tpHistCalcAtr *> _calcAtr;

	bool	PushBackCalcAtr(tpHistCalcAtr *atr)
	{
		DWORD val = WaitForSingleObject(_lockEX, INFINITE);
		if (val == WAIT_OBJECT_0)
		{
			_calcAtr.push_back(atr);
			ReleaseSemaphore(_lockEX, 1, NULL);
			return true;
		}
		return false;
	}
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
