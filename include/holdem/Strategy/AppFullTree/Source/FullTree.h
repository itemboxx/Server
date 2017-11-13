//---------------------------------------------------------------------------

#ifndef FullTreeH
#define FullTreeH
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "..\..\Source\HandsGroupEX.h"
#include "..\..\..\..\util\clTree.h"
#include "..\..\..\Common\HoldemTree.h"
#include "..\..\..\Common\clRoot.h"

/*struct tpDisNode
{
	tpDis _dis;
	double _ev, _weight;
	int _cnCalc;
};
struct tpInfoStreet
{
	int _nbCls, _force;
	float _weight;
};

class clFlopTree : public clHoldemTree
{
	friend class clRootFlopTree;
public:
	clFlopTree *Branch(int nb) { return (clFlopTree *)clTree < clAtrHoldemTree >::Branch(nb); }
private:
	void	CalcWeightStrat(clHandsGroupEx **gh);
	clFlopTree *FindSitNodeA(clSitHoldem &sit);
};*/

class clFullTreeRoot : public clRoot
{
public:
	void	ClearTab();
	size_t	SizeInFile();
	int 	WriteFile(int handle);
	int 	ReadFile(int handle);
	void operator = (clFullTreeRoot &root);
	void	CalcTree(size_t cnCalc);
	size_t	CreateTreeNL(int sizeMb, bool &setMem);
	//void	CalcWeightStrat();
	void	CalcEvTreeTurn(vector <float> *ev);
	//double	CalcAntistrategy();
	//void	SimpleCompactGH(int cn);
	//void	CreateFlopDat(clStreetDat &dat);
	//int		NbTurn(int nbHand, int nbCard) { return nbHand*CN_CARD_COL + nbCard; }
	//int		NbRiver(int nbHand, int nbTurn, int nbRiver) { return nbHand*CN_CARD_COL*CN_CARD_COL + nbTurn*CN_CARD_COL + nbRiver; }
	size_t	SetStartParam(bool setMem);
	tpDis	GetDis(clSitHoldem &sit, vector <tpDisNode> &val);

	vector <int> _tabFlop[3], _tabTurn[3], _tabRiver[3];
	int _cnFlop[3], _cnTurn[3], _cnRiver[3];
private:
	clCalcParam		GetCalcParam(clSitHoldem &sit, clHoldemTree *node);
	size_t	CreatePreFlopTreeNL(int sizeMb, bool &setMem);
	size_t	CreateFlopTreeNL(int sizeMb, bool &setMem);
	void	CreateTabFlop();
	void	CreateTabTurn();
	void	CreateTabRiver();
	void	ReCalcTurnInd(int nbTurnClsFile);
	void	ReCalcRiverInd(int nbRiverClsFile);
};

/*int		CutAndNorm(vector <tpDisNode> &arr);
int		CompareSit(clSitHoldem &sit, clSitHoldem &sit0);
void	CreateSitPreFlop(clSitHoldem &sit, int stack, int pot);
void	CreateFlopDatA(clStreetDat &dat, clSitHoldem &sit, clHandsGroupEx *gh);*/
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
