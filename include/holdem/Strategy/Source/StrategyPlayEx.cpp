#include <math.h>
#include <time.h>

//#include "C:\home\holdem\Strategy\Source\ClastersBoard.h"
//#include "C:\home\holdem\Strategy\Source\Strategy.h"
#include "StrategyPlayEx.h"
//#include "c:\home\Util\FileRec.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlayEX::Load()
{
#ifdef COMP2
	if (!_stratOld.Load("d:\\home\\Save\\forPlay\\NL2_4.aaa")) return false;
#else
	if (!_stratOld.Load("c:\\home\\bin\\forPlay\\NL2_4.aaa")) return false;
#endif
	_stratNew.Load();
	//_trade = TRADE_UNDEF;
	_lockEX = CreateSemaphore(NULL, 1, 1, "");
	_fileR.Create("d:\\data\\root.bin", 0, 1000);
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlayEX::ClearOldHistCalc(int tSec)
{
	DWORD val = WaitForSingleObject(_lockEX, INFINITE);
	if (val == WAIT_OBJECT_0)
	{
		time_t t0 = time(NULL) - tSec;
		for (int i = _calcAtr.size() - 1; i >= 0; i--)
			if (_calcAtr[i]->_time < t0)
			{
				delete _calcAtr[i];
				_calcAtr.erase(_calcAtr.begin() + i);
			}
		ReleaseSemaphore(_lockEX, 1, NULL);
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategyPlayEX::DecideRemote(clSitHoldem &sit0, tpArrResBranch8 &arr, int msec)
{
	enNameTrade trade = sit0.NameTrade(), tr0 = TRADE_TURN;
	if (trade < tr0)
		return _stratOld.DecideTree(sit0, arr);

	clSitHoldem sit = sit0;
	double k = 1 / sit.ChangeBB(100);
	tpDis dis = HistCreateDis(ACT_UNDEF);
	int i = 0;
	for (; i < _calcAtr.size(); i++)
		if (sit.NbGame() == _calcAtr[i]->_nbHand)
			break;
	vector <tpDisNode> vect;
	if (i >= _calcAtr.size()) // не нашли
	{
		tpHistCalcAtr *atr = new tpHistCalcAtr;
		atr->_nbHand = sit.NbGame();
		atr->_curAct = sit.CurAct();
		if (!CreateHistCalc(atr->_histCalc, sit))
		{
			arr._cn = 0;
			delete atr;
			return HistCreateDis(ACT_UNDEF);
		}

		/*if (trade == TRADE_TURN && _fileR.IsOpen())
		{
			if (_fileR.CnRecord() < 100)
				_fileR.AddRecord(&atr->_histCalc);
			else
				_fileR.CloseFile();
		}*/

		dis = _stratNew.GetDis(sit, vect, atr->_histCalc, msec);
		atr->_time = time(NULL);
		PushBackCalcAtr(atr);
	}
	else
	{
		tpHistCalcAtr *ptr = _calcAtr[i];
		dis = _stratNew.GetDis(sit, vect, ptr->_histCalc, msec);
		ptr->_time = time(NULL);
	}
	ClearOldHistCalc(600);
	arr._cn = vect.size();
	for (int i = 0; i < arr._cn; i++)
	{
		arr._cnCalc = vect[0]._cnCalc;
		arr._dis[i] = vect[i]._dis;
		arr._ev[i] = vect[i]._ev;
		arr._weight[i] = vect[i]._weight;
	}
	dis._money = dis._money*k + 0.5;
	arr.MultMoney(k);
	return dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlayEX::CreateHistCalc(clRoot &hs, clSitHoldem &sit)
{
	enNameTrade trade = sit.NameTrade();
	if (trade == TRADE_FLOP)
	{
		//ErrMessage("", "111");
		if (FindGHFlopEx(&hs, sit))
		{
			_stratNew._flopTreeBank->GetTreeForRoot(hs, NULL);
			return true;
		}
	}
	else if (trade == TRADE_TURN)
	{
		if (FindGHFlopEx(&hs, sit))
		{
			_stratNew._turnTreeBank->GetTreeForRoot(hs, NULL);
			return true;
		}
	}
	else if (trade == TRADE_RIVER)
	{
		//FindGHRiverEx(&hs, sit);
		//_stratNew.RecalcTreeRiver(*((clRootStreetTree *)&hs), NULL);
		return false;
	}
	return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategyPlayEX::GetDis(clSitHoldem &sit, vector <tpDisNode> &vect, clRoot &hs, int mSec)
{
	int t0 = GetTickCount();
	enNameTrade trade = sit.NameTrade(), tr0 = TRADE_FLOP;
	if (trade < tr0)
	{
		//_trade = trade;
		tpArrResBranch8 arr;
		tpDis dis = _stratOld.DecideTree(sit, arr);
		vect.resize(arr._cn);
		for (int i = 0; i < arr._cn; i++)
		{
			vect[i]._dis = arr._dis[i];
			vect[i]._cnCalc = arr._cnCalc;
			vect[i]._ev = arr._ev[i];
			vect[i]._weight = arr._weight[i];
		}
		return dis;
	}
	else if (hs._sit.NameTrade() == TRADE_UNDEF)
	{
		//_trade = trade;
		if (trade == TRADE_FLOP)
		{
			//ErrMessage("", "111");
			FindGHFlopEx(&hs, sit);
			_stratNew._flopTreeBank->GetTreeForRoot(*((clRootStreetTree *)&hs), NULL);
		}
		else if (trade == TRADE_TURN)
		{
			FindGHFlopEx(&hs, sit);
			_stratNew._turnTreeBank->GetTreeForRoot(*((clRootStreetTree *)&hs), NULL);
		}
		else if (trade == TRADE_RIVER)
		{
			//FindGHRiverEx(&hs, sit);
			//_stratNew.RecalcTreeRiver(*((clRootStreetTree *)&hs), NULL);
		}
		t0 = GetTickCount() - t0;
		return _stratNew.GetDis(sit, vect, hs, mSec - t0);
	}
	else
		return _stratNew.GetDis(sit, vect, hs, mSec);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlayEX::FindGHFlopEx(clRoot *hs, clSitHoldem &sit)
{
	enNameTrade trade = sit.NameTrade();
	int nbAct = 0;
	if (trade == TRADE_FLOP)
		nbAct = sit.NbFirstAct(ACT_DELFLOP);
	else if (trade == TRADE_TURN)
		nbAct = sit.NbFirstAct(ACT_DELTURN);
	else if (trade == TRADE_RIVER)
		nbAct = sit.NbFirstAct(ACT_DELRIVER);
	nbAct++;

	clSitHoldem sit1;
	sit1.ExecHist(sit.PtrHistory(), nbAct);
	clHistCalcS histC;
	if (!_stratOld.GetGroupHandsIn(histC, sit1))
	{
		histC._handsIn[0]._vect.clear();
		return false;
	}
	hs->_gh[0] = histC._handsIn[0];
	hs->_gh[1] = histC._handsIn[1];
	hs->_sit = sit1;

	hs->CalcIndex();
#ifndef TURN_FULLTREE
	int nbH[2];
	nbH[0] = sit.PlayerHand(0).NbHandAll();
	nbH[1] = sit.PlayerHand(1).NbHandAll();
	hs->InitDeck();
	hs->CompactGHQuickH(1, 100, nbH);

	hs->CalcIndex();
	hs->CalcForce();
#endif
	BuildTree(*hs, sit, nbAct + 1);
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------void	clStrategyPlayEX::FindGHFlopEx(clRootStreetTree *hs, clSitHoldem &sit)
/*void	clStrategyPlayEX::FindGHRiverEx(clRootStreetTree *hs, clSitHoldem &sit)
{
	int nbAct = sit.NbFirstAct(ACT_DELRIVER);
	nbAct++;

	clSitHoldem sit1;
	sit1.ExecHist(sit.PtrHistory(), nbAct);
	clHistCalc histC;
	if (!_stratOld.GetGroupHandsIn(histC, sit1))
	{
		histC._handsIn[0]._vect.clear();
		return;
	}
	hs->_gh[0] = histC._handsIn[0];
	hs->_gh[1] = histC._handsIn[1];
	hs->_sit = sit1;

	int nbH[2] = {-1,-1}, ind=sit.Indicator();
	nbH[0] = sit.PlayerHand(0).NbHandAll();
	nbH[1] = sit.PlayerHand(1).NbHandAll();
	//nbH[ind] = sit.PlayerHand(ind).NbHandAll();

	//hs->CalcIndex();
	//hs->InitDeck();
	//hs->CompactGHQuickH(1, 100, nbH);

	hs->CalcIndex();
	hs->CalcForce();  ///????
	BuildTree(*hs, sit, nbAct + 1);
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
