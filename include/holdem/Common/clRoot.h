//---------------------------------------------------------------------------

#ifndef ROOT_H
#define ROOT_H
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "HoldemTree.h"
#include "..\Strategy\Source\defaultType.h"

class clInComeDataRoot
{
public:
	size_t	SizeInFile();
	int 	WriteFile(int handle);
	int 	ReadFile(int handle);
	void	operator = (clInComeDataRoot &gset);

	clSitHoldem _sit;
	clHandsGroupEx _gh[3];
};

class clRoot : public clInComeDataRoot
{
public:
	clRoot() {}
	clRoot(clRoot &root) { *this = root; }
	void	operator = (clRoot &root);
	bool	Empty() { return _tree.CnBranch() == 0; }
	size_t	SizeInFile();
	int		WriteFile(int handle);
	int		ReadFile(int handle);
	void	CreateTreeNLCompact();
	void	CreateTreeNL1326();
	void	InitDeck();
	void	CalcIndex(double min=DOUBLE_0);
	int		CnHand(int nbPl) { return (int)_vectHand[nbPl].size(); }
	int		NbHand(int nbPl, int nb) { return _vectHand[nbPl][nb]; }
	int		NbCls(int nbHero, int nbHand);
	void	CompactGHSimple(int cn);
	void	CreateCompactGH(vector <tpFloat> *ghF);
	void	CreateCompactGH(vector <float> *ghF);
	void	CreateCompactGH(clHandsGroupEx *ghF);
	void	CalcWeightBranchStrat();
	void	RecalcGH(clHoldemTree *node, clHandsGroupEx *gh);
	tpDis	GetDis(clSitHoldem &sit, vector <tpDisNode> &vect);
	void	CalcAbsWeightGH(vector <float> *weight);

	clHoldemTree _tree;

	tpCard _cards[5];
	tpBooleanDeck _deck;
	vector <int> _vectHand[3];

private:
	void	CalcWeightStrat2Pl(clHoldemTree *node, clHandsGroupEx **ghF);
	void	RecalcGH2Pl(clHoldemTree *node, vector <tpFloat> *gh);
};

int		CutAndNorm(vector <tpDisNode> &arr, double minW);
void	ReFormatData(tpIncomeFloatData *dat, clInComeDataRoot *root);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
