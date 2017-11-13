//---------------------------------------------------------------------------

#ifndef TurnNodeH
#define TurnNodeH
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "C:\home\holdem\Strategy\Source\HandsGroupEX.h"
#include "C:\home\holdem\Common\sitHoldem.h"
#include "C:\home\Holdem\Common\HoldemTree.h"
#include "C:\home\Holdem\Common\clRoot.h"
//#include "C:\home\holdem\Strategy\Source\defaultType.h"
//#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\RiverTreeNew.h"
#include "C:\home\holdem\Strategy\AppRiverTrees\Source\RiverTree.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
class clStreetNode  : public clInComeDataRoot
{
public:
	size_t	SizeInFile();
	int 	WriteFile(int handle);
	int 	ReadFile(int handle);
	void CalcEV(void * ptrUt);
	int		GetCnCalc() { return _sit.ParamInt(); }
	void	SetCnCalc(int cn) { _sit.ParamInt() = cn; }

	vector <float> _ev[3];
private:
	void CalcEVTurn(void * ptrUt);
	void CalcEVFlop(void * ptrUt);
};

void	CalcKHand(clRoot *root, vector <float> *kHand);
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
