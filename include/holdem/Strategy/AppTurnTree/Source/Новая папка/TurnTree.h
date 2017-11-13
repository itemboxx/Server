//---------------------------------------------------------------------------

#ifndef TurnTreeH
#define TurnTreeH
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "C:\home\holdem\Strategy\Source\HandsGroupEX.h"
#include "C:\home\holdem\Common\sitHoldem.h"
#include "c:\home\util\clTree.h"
#include "C:\home\Holdem\Common\clRoot.h"
//#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\TurnNode.h"

#define	NB_NODE
#define	TURN_FULLTREE
#define CN_VAR_XMM 8
#ifdef TURN_FULLTREE
#include "C:\Home\Holdem\Strategy\AppFullTree\Source\FullTree.h"
#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------
struct tpStreetDatGH
{
	float Distance(tpStreetDatGH val) { float f = _force - val._force; f *= f; float f50 = _force50 - val._force50; f50 *= f50; return sqrt((f + f50) / 2); }
	float DistanceSqr(tpStreetDatGH val) { float f = _force - val._force; f *= f; float f50 = _force50 - val._force50; f50 *= f50; return (f + f50); }
	bool operator > (tpStreetDatGH &val) { return _force + _force50 > val._force + val._force50; }
	float Force() { return _force + _force50; }
	static int FindNbCenterElement(vector <tpStreetDatGH> &vect);

	int _nb;
	float _weight, _force, _force50;
#ifdef BOARD_VALUE
	clHandValue _hv;
	clHand _hand;
#endif
};

#define K_FOR_FLOPDAT		1
#define CN_STREETDAT_UNIT	10

class clStreetDat
{
public:
	clStreetDat() { _k = 1; }
	size_t	SizeInFile()
	{
		return sizeof(_k) + sizeof(_stack) + sizeof(_pot) + SizeVectFile(_vectUnit[0]) + SizeVectFile(_vectUnit[1]) + SizeVectFile(_vectUnit[2]) + SizeVectFile(_vectUnit50[0]) + SizeVectFile(_vectUnit50[1]) + SizeVectFile(_vectUnit50[2]);
	}
	void	WriteFile(int handle)
	{
		_write(handle, &_k, sizeof(_k));
		_write(handle, &_stack, sizeof(_stack));
		_write(handle, &_pot, sizeof(_pot));
		WriteVectFile(handle, _vectUnit[0]);
		WriteVectFile(handle, _vectUnit[1]);
		WriteVectFile(handle, _vectUnit[2]);
		WriteVectFile(handle, _vectUnit50[0]);
		WriteVectFile(handle, _vectUnit50[1]);
		WriteVectFile(handle, _vectUnit50[2]);
	}
	void	ReadFile(int handle)
	{
		_read(handle, &_k, sizeof(_k));
		_read(handle, &_stack, sizeof(_stack));
		_read(handle, &_pot, sizeof(_pot));
		ReadVectFile(handle, _vectUnit[0]);
		ReadVectFile(handle, _vectUnit[1]);
		ReadVectFile(handle, _vectUnit[2]);
		ReadVectFile(handle, _vectUnit50[0]);
		ReadVectFile(handle, _vectUnit50[1]);
		ReadVectFile(handle, _vectUnit50[2]);
	}
	tpFloat Distance(clStreetDat &dat);
	void	Copy50AndSort();

	float _k;
	int _stack[3], _pot;
	vector <tpStreetDatGH> _vectUnit[3], _vectUnit50[3];
private:
	//tpFloat DistanceGH_1(clStreetDat &dat);
	//tpFloat DistanceGH_Sq(clStreetDat &dat);
};

struct tpPrepareBd
{
	int  WriteFile(int handle) { _gset.WriteFile(handle); _dat.WriteFile(handle); return 0; }
	int  ReadFile(int handle) { _gset.ReadFile(handle); _dat.ReadFile(handle); return 0; }
	size_t SizeInFile() { return _gset.SizeInFile() + _dat.SizeInFile(); }

	clInComeDataRoot _gset;
	clStreetDat _dat;
};

struct tpUnfindTree;

#ifdef TURN_FULLTREE
class clRootStreetTree : public clFullTreeRoot
{
public:
	int 	ReadFileAndPrepare(int handle) { return clFullTreeRoot::ReadFile(handle); }
	void	CreateTreeNL() { bool b0 = true; 	clFullTreeRoot::CreateTreeNL(1000,b0);}
	bool	CreateStreetDat(clStreetDat &dat) { return CreateTurnDat(dat); }
	void	ReCalcParam(clStreetDat *dat);
	void	FindMinParam(clStreetDat &dat);
	int		&Force(vector <int> &force, int nbCard, int nbHand) { return force[nbCard*ALL_CN_HAND + nbHand]; }
	void	CompactGH(double minW, int cnMin, clStreetDat *dat = 0);
	void	CompactGHQuick(double minW, int cnMin) {  }
	void	CompactGHQuickH(double minW, int cnMin, int *nbH) {  }
	void	CalcAll() { CalcIndex(); CalcForce(); InitDeck(); }
	void	CalcForce()			{}

	void	CalcForceTurn(vector <int> &force);
	bool	CreateTurnDat(clStreetDat &dat);
	//void	CreateTurnDatQuick(clStreetDat &dat);
	void	CompactGH(double ur, int cnMin, clStreetDat dat);
};
#else
class clRootStreetTree : public clRoot
{
public:
	//size_t	SizeInFile();
	//int 	WriteFile(int handle);
	int 	ReadFileAndPrepare(int handle);
	//void operator = (clRoot &root) { *((clRoot*)this) = *((clRoot*)&root); }
	void operator = (clRootStreetTree &root);
	//void operator = (clStreetNode &node) { *((clInComeDataRoot*)this) = *((clInComeDataRoot*)&node); }
	void	CreateTreeNL();
	void	CutTree();
	void	CalcForce() { CalcForceTurn(_force); }
	void	CalcAllIn();
	void	CalcAll()	{ CalcIndex(); CalcForce(); CalcAllIn();  InitDeck(); }
	//void	CalcMultiHandsNode(size_t cnCalc, tpUnfindTree *ut=NULL);
	void	CalcMultiHandsTree(size_t cnCalc);
	void	CalcEvTree(vector <float> *ev, tpUnfindTree *ut);
	void	CompactGH(double minW, int cnMin, clStreetDat *dat = 0);
	//void	CompactGHQuick(double minW, int cnMin) { clStreetDat dat; InitDeck(); CreateStreetDatQuick(dat); CompactGH(minW, cnMin, &dat); }
	void	CompactGHQuickH(double minW, int cnMin, int *nbH) { clStreetDat dat; CreateTurnDatQuick(dat); CompactGH_H(minW, cnMin, nbH, dat); }
	bool	CreateStreetDat(clStreetDat &dat);
	//void	CreateStreetDatQuick(clStreetDat &dat);
	int		&Force(int nbCard, int nbHand) { return _force[nbCard*ALL_CN_HAND + nbHand]; }
	int		&Force(int kTurn, int kRiver, int nbHand) { return _force[nbHand*CN_CARD_COL*CN_CARD_COL + kTurn*CN_CARD_COL + kRiver]; }
	int		&Force(vector <int> &force, int nbCard, int nbHand) { return force[nbCard*ALL_CN_HAND + nbHand]; }
	float	PrAllIn(int nbCls0, int nbCls1) { return _prAllIn.Element(nbCls0, nbCls1, 0); }
	void	SetWeightStrat();
	//void	CalcNeshMultiHands(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev, tpUnfindTree *ut);
	void	CalcNeshMultiHandsTree(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev);
	void	ReCalcParam(clStreetDat *dat);
	void	FindMinParam(clStreetDat &dat);

	//clHoldemTree _tree;
	vector <int> _force;
	clMatrixCub <float> _prAllIn;

private:
	void	CalcForceTurn(vector <int> &force);
	//void	CalcForceTurn();
	//void	CalcForceFlop();
	void	CalcAllInTurn();
	//void	CalcAllInFlop();
	void	CompactGH(double ur, int cnMin, clStreetDat dat);
	void	CompactGH_H(double minW, int cnMin, int *nbH, clStreetDat dat);
	bool	CreateTurnDat(clStreetDat &dat);
	//void	CreateFlopDat(clStreetDat &dat);
	void	CalcEVMultiHands(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev);
	//void	ResultMultiHands(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev, tpUnfindTree *ut);
	void	ResultMultiHandsTree(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev);
	void	AddResEVMultiHands(clHoldemTree *node, int nbHero, int cnB, vector <float> &ev, vector <float> *evCur, tpFloat *w, vector <tpFloat> &kH, vector <tpFloat> &kOpp);
	void	CreateTurnDatQuick(clStreetDat &dat);
	//void	CreateFlopDatQuick(clStreetDat &dat);
};
#endif

//void	CreateSitStreet(clSitHoldem &sit, int stack, int pot, enNameTrade trade);
void	InitTurnDatDat();
int *GetNbHandTurnDatDat(int nb);
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
