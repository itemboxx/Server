//---------------------------------------------------------------------------

#ifndef FlopTreeH
#define FlopTreeH
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "..\..\Source\HandsGroupEX.h"
#include "..\..\..\Common\sitHoldem.h"
#include "..\..\..\Common\clRoot.h"
#include "..\..\..\..\NeuroNet\NeuroNet.h"
#include "..\..\AppTurnTree\Source\TurnTree.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
#define CN_FLOP_STANDART	10

void	CreateFlopDat(clRoot &root, clStreetDat &dat);

class clRootFlopTree : public clRoot
{
public:
	//size_t	SizeInFile();
	//int 	WriteFile(int handle);
	//int 	ReadFileAndPrepare(int handle);
	//void operator = (clRoot &root) { *((clRoot*)this) = *((clRoot*)&root); }
	//void operator = (clRootFlopTree &root);
	void	CreateTreeNL();
	void	CalcAll() { CalcIndex(); CalcAllInFlop(); }
	void	CalcAllInFlop();
	//void	CutTree();
	void	CalcMultiHandsStandartGH(size_t cnCalc, clLayersBPNet *net);
	void	CalcNeshMultiHands(size_t cnCalc, clLayersBPNet *net);
	void	CalcEvTree(vector <float> *ev, clLayersBPNet *net);
	//void	CompactGH(double minW, int cnMin, clStreetDat *dat = 0);
	//void	CompactGHQuick(double minW, int cnMin) { clStreetDat dat; InitDeck(); CreateStreetDatQuick(dat); CompactGH(minW, cnMin, &dat); }
	//void	CompactGHQuickH(double minW, int cnMin, int *nbH) { clStreetDat dat; CreateTurnDatQuick(dat); CompactGH_H(minW, cnMin, nbH, dat); }
	void	CreateStreetDat(clStreetDat &dat) { CreateFlopDat(*this, dat); }
	void	CreateStreetDatMin(clStreetDat &dat, clStreetDat *datIn = NULL);
	//int		&Force(int nbCard, int nbHand) { return _force[nbCard*ALL_CN_HAND + nbHand]; }
	//int		&Force(int kTurn, int kRiver, int nbHand) { return _force[nbHand*CN_CARD_COL*CN_CARD_COL + kTurn*CN_CARD_COL + kRiver]; }
	//int		&Force(vector <int> &force, int nbCard, int nbHand) { return force[nbCard*ALL_CN_HAND + nbHand]; }
	float	PrAllIn(int nbCls0, int nbCls1) { return _prAllIn.Element(nbCls0, nbCls1, 0); }
	//void	SetWeightStrat();
	//void	CalcNeshMultiHands(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev, tpUnfindTree *ut);
	void	CalcNeshMultiHands(clHoldemTree *node, int nbHero, vector <float> &kH, vector <float> &kOpp, vector <float> &ev, clCard *cards, clLayersBPNet *net);
	void	CalcNeshMultiHandsStandartGH(clHoldemTree *node, int nbHero, vector <float> &kH, vector <float> &kOpp, vector <float> &ev, clCard *cards, clLayersBPNet *net);
	void	ReCalcParam(clStreetDat *dat);
	void	FindMinParam(clStreetDat &dat);

	//vector <int> _force;
	clMatrixCub <float> _prAllIn;

private:
	//void	CalcForceFlop();
	//void	CalcAllInFlop();
	//void	CompactGH(double ur, int cnMin, clStreetDat dat);
	//void	CompactGH_H(double minW, int cnMin, int *nbH, clStreetDat dat);
	//void	CreateFlopDat(clStreetDat &dat);
	void	CalcEVMultiHands(clHoldemTree *node, int nbHero, vector <float> &kH, vector <float> &kOpp, vector <float> &ev, clLayersBPNet *net);
	void	ResultMultiHandsAll(clHoldemTree *node, int nbHero, vector <float> &kH, vector <float> &kOpp, vector <float> &ev, clLayersBPNet *net);
	void	ResultMultiHands(clHoldemTree *node, int nbHero, vector <float> &kH, vector <float> &kOpp, vector <float> &ev, clCard *cards, clLayersBPNet *net);
	void	AddResEVMultiHands(clHoldemTree *node, int nbHero, int cnB, vector <float> &ev, vector <float> *evCur, tpFloat *w, vector <float> &kH, vector <float> &kOpp);
	void	ResultMultiHandsStandartGH(clHoldemTree *node, int nbHero, vector <float> &kH, vector <float> &kOpp, vector <float> &ev, clCard *cards, clLayersBPNet *net);
	void	AddResEVMultiHandsStandartGH(clHoldemTree *node, int nbHero, int cnB, vector <float> &ev, vector <float> *evCur, tpFloat *w, vector <float> &kH, vector <float> &kOpp);
};

void	InitFlopStandart();
int *GetNbHandFlopStandart(int nb);
clCard	*GetBoardFlopStandart();
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
