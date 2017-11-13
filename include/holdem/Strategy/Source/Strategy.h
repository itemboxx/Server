//---------------------------------------------------------------------------

#ifndef StrategyH
#define StrategyH
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "..\..\Common\sitHoldem.h"
#include "..\..\..\util\clTree.h"
#include "HandsGroupEX.h"
#include "CalcSitEV.h"
#include "..\..\..\util\Clasters.h"

#define	TREE_FOR_CALC		1

class clCalcParam0
{
public:
	clCalcParam0()		{ _ptr = NULL; }
	clCalcParam0(byte *val)		{ _ptr = val; }
	void	ClearEV(int cnB);
	void	ClearEVAndSetWeight(int cnB);
	static int		SizeParam(int cnB)					{ return 3 * sizeof(int) + 2 * cnB * sizeof(tpFloat0); }
	static int		SizeParamForCalc(int cnB)			{ return 3 * sizeof(int) + 3 * cnB * sizeof(tpFloat0); }
	void	GetWeight(int cnB, tpFloat0 *w);
	tpFloat0 *GetWeightForCalc(int cnB)					{ return ((tpFloat0 *)(_ptr + 12 + 2*cnB*sizeof(tpFloat0))); }
	void	GetWeightStrat(int cnB, tpFloat0 *w);
	void	AddResEV(int cnB, tpFloat0 res, tpFloat0 *w, tpFloat0 *ev, tpFloat0 k, tpFloat0 kOpp);
	void	AddResEVForCalc(int cnB, tpFloat0 res, tpFloat0 *w, tpFloat0 *ev, tpFloat0 k, tpFloat0 kOpp);
	unsigned int		&CnCalc()			{ return *((unsigned int *)_ptr); }
	tpFloat0	&WeightCalc()		{ return *((tpFloat0 *)(_ptr + 4)); }
	tpFloat0	&AverRes()			{ return *((tpFloat0 *)(_ptr + 8)); }
	tpFloat0 *BeginEV()			{ return ((tpFloat0 *)(_ptr + 12)); }
	tpFloat0 *BeginPart(int cn)			{ return ((tpFloat0 *)(_ptr + 12+cn*sizeof(tpFloat0))); }
	tpFloat0 AbsEV(int nb)		{ tpFloat0 w = WeightCalc(); return (w > DOUBLE_0) ? (BeginEV()[nb]+AverRes()) / w : 0; }
	void	ChangeStratWeight(int cnB, tpFloat0 m);
	void	SetParam(int cnB, tpFloat0 *w, int cn);

	byte *_ptr;
};

class clAtrTreeStrat
{
public:
	clAtrTreeStrat()				{ Clear(); _sitRes = NULL; _verTree = 0; _statCn = 0; }
	void	ClearEV(int cnB);
	void	Clear()					{ _sitA.Clear(); }
	size_t		CnClasters()		{ return (_sizeParam==0)? 0 : _param.size() / _sizeParam; }
	size_t	SizeInFile();
	void	WriteFile(int handle);
	void	ReadFile(int handle);
	void	operator = (clAtrTreeStrat &atr);
	clCalcParam0		CalcParam(int nbCls)		{ return &_param[nbCls*_sizeParam]; }
	int		GetNbForCls(int nb);

	clSitHoldem _sitA;
	
	byte _sizeParam,_verTree;
	vector <byte> _param;
	vector <short> _vectInd;
	tpSitRes *_sitRes;
	void *_strat;
	int *_ptrIndex;
	double _statCn;
};

class clTreeStrat : public clTree < clAtrTreeStrat >
{
	friend class clStrategy;
public:
	int AddBranch(clSitHoldem &sit);
	clTreeStrat *Branch(int nb)				{ return (clTreeStrat *)clTree < clAtrTreeStrat >::Branch(nb); }
	clTreeStrat *AddBranch()				
	{ 
		clTreeStrat *node = (clTreeStrat *)clTree < clAtrTreeStrat >::AddBranch(); 	
		node->AtrTree()->_verTree = AtrTree()->_verTree; 
		return  node; 
	}
	int		Indicator()						{ return AtrTree()->_sitA.Indicator(); }
	void	ClearEV();
	void	ClearStatWeight();
	inline	int		GetNbCls(tpCardsGame &cards, int ind);
	int		GetNbCls(tpHand &hand,tpCard *board);
	clDoubleCP  Result(tpCardsGame &cards)	{ return CalcSitEV(AtrTree()->_sitA, &cards, AtrTree()->_sitRes); }
	clDoubleCP	CalcEV(int nbH, tpCardsGame &cards, tpFloat0 kH, tpFloat0 kOpp);
	vector <clDoubleCP>	CalcEV(int nbH, int dim, tpCardsGame *cards, vector <tpFloat0> &kH, tpFloat0 kOpp);
	clDoubleCP	CalcEVForCalc(int nbH, tpCardsGame &cards, tpFloat0 kH, tpFloat0 kOpp);
	tpDis		GetDis()						{ return AtrTree()->_sitA.GetLastAct()->_dis; }
	tpDis		GetStratDis(clSitHoldem &sit);
	clAnsiString	NameForNode();
	clSitHoldem &GetSit()					{ return AtrTree()->_sitA; }
	void		CalcWeightStat();
	clTreeStrat *FindBranch(tpDis dis);
	clTreeStrat *FindEqBranch(tpDis dis);
	clTreeStrat *FindBranch(enNameTrade trade);
	clTreeStrat *FindSitNode(clSitHoldem &sit);				// точная последовательность решений
	clTreeStrat *FindBeforeEqualDis(clSitHoldem &sit);		// возвращаем последний узел, где еще примерно соответствует sit
	void		GetGH(clSitHoldem &sit, clHandsGroupEx *ghH);
	clTreeStrat *GetStratEndedNode(tpCardsGame &cards);
	clTreeStrat *GetStratBranch(tpCardsGame &cards, clHandsGroupEx *ghH);
	int 		SetNbNode(int nb);
	bool	EndedNode()						{ return CnBranch() == 0; }
	tpFloat0 EVNabor(tpCardsGame &cards, int nb);
	void	ChangeStratWeight(tpFloat0 m);
	void	ChangeStatWeight(tpFloat0 m);
	void	ChangeGHAll(int nb, tpCard *board, clHandsGroupEx &ghH);
	clDoubleCP	CalcTreeVect(int nbH, vector <tpCardsGame> &vectCards, vector <tpFloat0> &vectkH, tpFloat0 kOpp);

private:

//	clTreeStrat *FindSitNode(clSitHoldem &sit, int ind, int nbA);
	size_t	CnClasters()					{ return AtrTree()->CnClasters(); }
//	clClasterS	&Claster(int nb)			{ return AtrTree()->_vectCls[nb]; }
	clCalcParam0 FindClaster(clSitHoldem &sit);
//	clClasterS *FindClaster(int nbHand)		{ return &AtrTree()->_vectCls[nbHand]; }
	int		CnPlayer()						{ return AtrTree()->_sitA.CnPlayer(); }
	enNameTrade	PokerStreet()				{ return AtrTree()->_sitA.NameTrade(); }
	int		BetPlayer()						{ int ind = Indicator(); return AtrTree()->_sitA.BetPl(ind); }
	int		BetPlayerOrBB()					{ int bet = BetPlayer(); return (bet < AtrTree()->_sitA.BigBlind()) ? AtrTree()->_sitA.BigBlind() : bet; }
	void	ReCalcWeight(clHandsGroupEx **arrH,bool stratB);
//	void	CalcWeightStrat(clHandsGroupEx **arrH);
	double	ChangeGH(int nb, clHandsGroupEx *gh, clHandsGroupEx *ghIn, bool stratB);
//	double	ChangeStratGH(int nb, clHandsGroupEx *gh, clHandsGroupEx *ghIn);
	void	GetGHSit(clSitHoldem &sit, int ind, int nbA, clHandsGroupEx *ghH);
	void	SetNbNodeA(int &nb);
	size_t	SetStartParam(int cnClsFlop, int cnClsTurn, int cnClsRiver, int *indFlop, int *indTurn, int *indRiver, bool mem);
	void	SetIndexForNabor(int *indFlop, int *indTurn, int *indRiver);

//	int _nbNode;
};

struct tpArrResBranch8
{
	tpArrResBranch8()		{ _cn = 0; }
	void	MultMoney(double k)
	{
		for (int i = 0; i < _cn; i++)
			_dis[i]._money = _dis[i]._money*k + 0.5;
	}

	tpDis _dis[MAX_CN_BRANCH];
	double _ev[MAX_CN_BRANCH], _weight[MAX_CN_BRANCH];
	int _cn,_cnCalc;
};

class clStrategy
{
	friend class clStrategyPlay;
	friend class clHistCalc;
public:
	clStrategy()		{ _indFlop = _indTurn = _indRiver = NULL; }
	~clStrategy()
	{
		if (_indFlop != NULL) free(_indFlop);
		if (_indTurn != NULL) free(_indTurn);
		if (_indRiver != NULL) free(_indRiver);
	}
	void	CreateLimit(int lev);
	void	CreateAF(clStacks stack);
	size_t	CreateNL(clStacks stack,bool mem);
	void	WriteFile(int handle);
	void	ReadFile(int handle);
	void	CalcEV(int cnCalc);
	void	CalcEVStart(int cnCalc);
	void	CalcEVStartVectHero(int cnCalc, int dim);
	//void	CalcEVThread(int cnCalc);
	void	InitMemory();
	void	ReCalcWeight();
	void	CalcWeightStrat();
	tpDis	GetDis(clSitHoldem &sit, tpArrResBranch8 *arr=NULL);	
	void	CreateTreeNL(clSitHoldem &sit);
	void	CreateTreeNL(clSitHoldem &sit0, clSitHoldem &sit);
	void	CalcNesh(int cnCalc);
	void	CalcNeshVectHero(int cnCalc, int dim);
	void	SetStartParam(clSitHoldem &sit, bool setPar);
	bool	IsHandsIn()										{ return _handsIn[0]._vect.size() > 0; }

	clTreeStrat		_tree;
	clAnsiString _nameFileFlop, _nameFileTurn, _nameFileRiver;
	clHandsGroupEx _handsIn[CN_PLAYER];
private:
	bool	ReadIndex();
	size_t	SetStartParam(bool mem);
	bool	SetIndexForNabor();
	static 		void	ClearIntPtr(int *&ptr) { if (ptr != NULL){ free(ptr); ptr = NULL; } }

	int *_indFlop, *_indTurn, *_indRiver;
};

typedef  int(*tpFuncBuildTree)(clSitHoldem &, tpDis *, tpDis *);

class clBuildTreeNLInfoStreet
{
public:
	clBuildTreeNLInfoStreet()			{ _isFold = _isCall = _isAllIn = true; _func = NULL; }
	
	int	GetInfo(clSitHoldem &sit, tpDis *arrDis);
	int	GetInfo(clSitHoldem &sit, tpDis *arrDis, tpDis disH);

	bool _isFold, _isCall, _isAllIn;
	double _kBank;
	vector <double> _firstWord, _firstRound, _secondRound, _replay;	// положительные в ВВ отрицательные в pot
	tpFuncBuildTree _func;
};

struct tpBuildTreeNLInfo
{
	clBuildTreeNLInfoStreet &operator [](int nb)	{ return _infS[nb]; }

	clBuildTreeNLInfoStreet _infS[5];
};

//void	CreateInf(clBuildTreeNLInfoStreet &inf, double k, int bankBB);
void	BuildTreeNLAuto(clTreeStrat *ptrT, clSitHoldem &sit, tpBuildTreeNLInfo &inf, clSitHoldem *sitH, int nbA,int start);
extern size_t glCn0, glCn1;
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
