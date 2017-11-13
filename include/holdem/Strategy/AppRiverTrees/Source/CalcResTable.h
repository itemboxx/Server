//---------------------------------------------------------------------------

#ifndef CalcResTableH
#define CalcResTableH
//---------------------------------------------------------------------------
#include "..\..\..\Common\sitHoldem.h"

class clCalcResTable
{
public:
	void    operator = (clSitHoldem &sit);
	void    StartTable();
	void    EndGame();
	void    WinPlayer(int nbPl);
	void    Win2Player(int nbPl1, int nbPl2);
	void    Win3Player(int nbPl1, int nbPl2, int nbPl3);
	clDoubleCP CalcMoney(clCalcResTable *StartTable);
	int CnPlayersIsCard();
	clStacks    GetStacks();
	int     PlMoney(int nb)     { return _money[nb] + _bet[nb]; }

	bool _isCard[CN_PLAYER];
	int  _bb, _cnPl, _money[CN_PLAYER], _bet[CN_PLAYER], _nbHAll[CN_PLAYER];
};
//---------------------------------------------------------------------------
#endif
