//---------------------------------------------------------------------------

#ifndef STRATEGY_STREET_H
#define STRATEGY_STREET_H
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "C:\home\holdem\Strategy\Source\HandsGroupEX.h"
#include "C:\home\holdem\Common\sitHoldem.h"
#include "c:\home\util\clTree.h"
#include "C:\home\Holdem\Common\HoldemTree.h"
#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\TurnTree.h"
#include "C:\home\Holdem\Strategy\AppRiverTrees\Source\clRiverBanks.h"
//#include "C:\home\Holdem\Strategy\AppTurnTree\Source\NodeBanks.h"
#include "C:\home\Holdem\Strategy\AppTurnTree\Source\TreeBanks.h"
#include "C:\Home\Holdem\Strategy\AppFlopStrategy\Source\FlopBanks.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
extern clRoot glHS;

class clStrategyH
{
	friend class clStrategyPF;
public:
	clStrategyH() { _bankRiver = NULL; }
	void	Load()
	{
		if (!glBankRiver.IsOpen())
			glBankRiver.OpenA(PATH_BANK_RIVER, true);
		if (!glBankRiver.IsOpen())
			ErrMessage("Не открывается файл ", PATH_BANK_RIVER);
		_bankRiver = &glBankRiver;
		if (!glBankTurn.IsOpen())
			glBankTurn.OpenA(FILE_TURN_BANK, true);
		if (!glBankTurn.IsOpen())
			ErrMessage("Не открывается файл ", FILE_TURN_BANK);
		_turnTreeBank = &glBankTurn;
		if (!glBankFlop.IsOpen())
			glBankFlop.OpenA(FILE_FLOP_BANK, true);
		if (!glBankFlop.IsOpen())
			ErrMessage("Не открывается файл ", FILE_FLOP_BANK);
		_flopTreeBank = &glBankFlop;

		/*_unfindFlop._dist = 0.1;
		_unfindFlop._file.Create("d:\\data\\UnfindFlopGame.unf", 0, 10000);
		_unfindTurn._dist = 0.1;
		_unfindTurn._file.Create("d:\\data\\UnfindTurnGame.unf", 0, 10000);
		_unfindRiver._dist = 0.04;
		_unfindRiver._file.Create("d:\\data\\UnfindRiverGame.unf", 0, 10000);*/
	}
	bool	Init() { return _bankRiver != NULL; }
	tpDis	GetDis(clSitHoldem &sit, vector <tpDisNode> &vect, clRoot &hs, int mSec = 0);
	//tpDis	GetDis(clSitHoldem &sit, vector <tpDisNode> &vect) { return GetDis(sit, vect, glHS); }
	//bool	RecalcTreeRiver(clRoot &hs, tpUnfindRiverTree *ptr = NULL);
	void	SaveFileUnf();

	clRiverBanks *_bankRiver;
	clTreeBank *_turnTreeBank;
	clFlopBank *_flopTreeBank;
	//tpUnfindTree _unfindFlop, _unfindTurn;
	//tpUnfindRiverTree _unfindRiver;
private:
	static tpDis GetDisTree(clRoot *hs, clSitHoldem &sit, vector <tpDisNode> &vect);
	tpDis GetDis(clRoot &hs, clSitHoldem &sit, vector <tpDisNode> &vect, int mSec);
	void	FindGHFlop(clRoot &hs, clSitHoldem &sit);
	void	FindGHTurn(clRoot &hs, clSitHoldem &sit);
	//void	RecalcTreeTurn(clRootStreetTree &hs);
	void	FindGHRiver(clRoot &hs, clSitHoldem &sit);
	void	CalcTree(clRoot &hs, int mSec);
};

void	BuildTree(clRoot &hs, clSitHoldem &sit, int nbAct);

extern clStrategyH glStrategy;
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
