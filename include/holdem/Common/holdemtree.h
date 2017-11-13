//---------------------------------------------------------------------------

#ifndef HoldemTreeH
#define HoldemTreeH
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "sitHoldem.h"
#include "..\..\Util\Matrix.h"
#include "..\..\util\clTree.h"
#include "..\Strategy\Source\HandsGroupEX.h"
#include "..\Strategy\Source\defaultType.h"

#define VIEW_PARAM
//#define	TEST_GROUPvsGROUP
#define	NB_NODE

#define	RES_SIT_FOLD		1
//#define	RES_SIT_SHOWDOWN2	2
#define	RES_SIT_SHOWDOWN	3
#define	RES_SIT_RIVER		4
#define	RES_SIT_TURN_ALLIN	5

class clCalcParam
{
public:
	clCalcParam() { _ptr = NULL; }
	clCalcParam(byte *val) { _ptr = val; }
	void	ClearData(int cnB);
	void	ClearEV(int cnB);
	void	MoveTestEV(int cnB);
	void	MultEV(int cnB, tpFloat k);
	void	ClearEVAndSetWeight(int cnB);
	static int		SizeParam(int cnB) { return sizeof(int) + 2 * sizeof(tpFloat) + 2 * cnB * sizeof(tpFloat); }
	static int		SizeParamForCalc(int cnB) { return sizeof(int) + 2 * sizeof(tpFloat) + 3 * cnB * sizeof(tpFloat); }
	void	GetWeight(int cnB, tpFloat *w);
	tpFloat *GetWeightForCalc(int cnB) { return ((tpFloat *)(_ptr + sizeof(int) + 2 * sizeof(tpFloat) + 2 * cnB * sizeof(tpFloat))); }
	bool	GetWeightStrat(int cnB, tpFloat *w);
	void	AddResEV(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp);
	void	AddResEVKOpp(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp);
	void	AddResEVAntiStrat(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp);
	void	AddResEVForCalc(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp);
	void	AddResEVForTest(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp);
	unsigned int		&CnCalc() { return *((unsigned int *)_ptr); }
	tpFloat	&WeightCalc() { return *((tpFloat *)(_ptr + sizeof(int))); }
	tpFloat	&AverRes() { return *((tpFloat *)(_ptr + sizeof(int) + sizeof(tpFloat))); }
	tpFloat *BeginEV() { return ((tpFloat *)(_ptr + sizeof(int) + 2 * sizeof(tpFloat))); }
	tpFloat *BeginPart(int cn) { return ((tpFloat *)(_ptr + sizeof(int) + 2 * sizeof(tpFloat) + cn * sizeof(tpFloat))); }
	tpFloat AbsEV(int nb) 
	{ 
		tpFloat w = WeightCalc(); 
		return (w > DOUBLE_0) ? (BeginEV()[nb] + AverRes()) / w : 0; 
	}
	void	ChangeStratWeight(int cnB, tpFloat m);
	void	SetParam(int cnB, tpFloat *w, int cn);
	double	SumStratWeight(int cnB);
	void	AddCalcParam(clCalcParam param, int cnB, double mult);		// нормируем StratWeight до mult и добавляем
	void	AddStratWeight(int cnB, int nb, tpFloat part) { BeginPart(cnB)[nb] += part; }

	byte *_ptr;
};

struct tpTreePlayerInfo
{
	int _nbCls[5][CN_PLAYER];
	int _handForce[CN_PLAYER];
	tpCard	_cardsTable[5];
};

class clAtrHoldemTree
{
public:
	clAtrHoldemTree() { _statCn = 0; } // _tabNabor = _tabCls = NULL;}
	void	ClearEV(int cnB);
	void	ClearData(int cnB);
	void	Clear() { _param.clear(); }
	int		CnClasters() { return (_sizeParam == 0) ? 0 : (int)(_param.size() / _sizeParam); }
	size_t	SizeInFile();
	void	WriteFile(int handle);
	void	ReadFile(int handle);
	void	operator = (clAtrHoldemTree &atr);
	clCalcParam		CalcParam(int nbCls) { return &_param[nbCls*_sizeParam]; }
	int  Result(int *nbCls, int ind);
	double  ResultPreFlop(tpTreePlayerInfo *nbCls, int ind);
	clDoubleCP  Result(int *force);
	void	CalcResSit(clSitHoldem &sit);
	void	GetAllWeight(int cnB, tpFloat *w);
	void	GetAllWeightNoNull(int cnB, tpFloat *w);
	void	GetAllWeightStrat(int cnB, tpFloat *w);
	size_t  CnCalc();
	size_t  CnCalcOneCls();
	void	AddDataBranch(int cnB, int nb, int nbSrc);
	void	DeleteBranch(int cnB, int nb);
	static int ConstSize() { return sizeof(clSitHoldem) + sizeof(int) + sizeof(double); }

	clSitHoldem _sitA;
	int _sizeParam;
	double _statCn;
	vector <byte> _param;
	//int	*_tabNabor, *_tabCls;
#ifdef NB_NODE
	int _nbNode;
#endif
private:
};

struct stNodeW
{
	void *_ptr;
	double _weight;
};

class clHoldemTree : public clTree < clAtrHoldemTree >
{
public:
	int AddBranch(clSitHoldem &sit);
	clHoldemTree *Branch(int nb) { return (clHoldemTree *)clTree < clAtrHoldemTree >::Branch(nb); }
	clHoldemTree *Parent() { return (clHoldemTree *)clTree < clAtrHoldemTree >::Parent(); }
	clHoldemTree *AddBranch()
	{
		clHoldemTree *node = (clHoldemTree *)clTree < clAtrHoldemTree >::AddBranch();
		return  node;
	}
	void	ReestablishSit(clSitHoldem sit);
	int		Indicator() { return AtrTree()->_sitA.Indicator(); }
	void	ClearData();
	void	ClearEV();
	void	ClearStatWeight();
	tpFloat		CalcNesh(int nbHero, tpTreePlayerInfo *info, tpFloat kH, tpFloat kOpp, tpFloat k0);
	tpFloat		CalcNeshKOpp(int nbHero, tpTreePlayerInfo *info, tpFloat kH, tpFloat kOpp, tpFloat k0);
	//	tpFloat		CalcNesh(clRootRiverTree *root, int nbHero, int nbClsHero, tpFloat kH, vector <float> &kOpp);
//	tpFloat		CalcNesh(clRootRiverTree *root, int nbHero, vector <float> &kH, vector <float> &kOpp, vector <float> &ev);
	clSitHoldem		&GetSit() { return AtrTree()->_sitA; }
	tpDis	GetDis() { return AtrTree()->_sitA.GetLastAct()->_dis; }
	clAnsiString	NameForBranch(int nb);
	clAnsiString	NameForNode() { return ""; }
	void		CalcWeightStat();
	void		CalcWeightStrat(vector <tpFloat> **gh);
	void		SetWeightStrat(tpTreePlayerInfo *info, double *wh);
	int			FindNbBranch(clHoldemTree *branch);
	int 	SetNbNode(int &nb);
	bool	EndedNode() { return CnBranch() == 0; }
	void	ChangeGH(int nb, clHandsGroupEx &ghH, vector <int> &nbHand);	// размерность ghH равна ALL_CN_HAND, а узла размерности nbHand
	void	ChangeGH(int nb, clHandsGroupEx &ghH);	// размерность узла равна ALL_CN_HAND
	void	ChangeGH(int nb, vector <tpFloat> &ghH);  // размерность узла совпадает с размерностью вектора
	size_t	SetStartParam(int *cnHand);
	size_t	SetStartParam(int **cnHand, bool setMem=true);
	void	SetResSit();
	clDoubleCP CalcStratEV(tpTreePlayerInfo *info);
	//	tpFloat CalcAntiStratEV(clRootRiverTree *root, int nbHero, int nbHand, clHandsGroupEx &gh);
	size_t  CnCalc() { return AtrTree()->CnCalc(); }
	size_t  CnCalcOneCls() { return AtrTree()->CnCalcOneCls(); }
	void	GetGHForNode(vector <tpFloat> *gh);		// подаем веса хендов по номерам кластеров, в них же возвращаются веса хендов для данного узла
//	int CnNode(enNameTrade trade);
	clHoldemTree *FindSitNode(int cnAct, clSitHoldem &sit, int curAct);	// точный узел соответствующий sit, иначе NULL
	clHoldemTree *FindSitNodePushFold(int cnAct, clSitHoldem &sit, int curAct);	
	void	FindSitNode(int cnAct, clSitHoldem &sit, int curAct, vector <stNodeW> &vect, double k = 1);
	void	FindBranchBetween(tpDis dis, stNbBtw &nb);
	clHoldemTree *GoBack(enAction act);
	bool	CheckAllNode();
	double	TreeVSTree(tpTreePlayerInfo *info, int nbHero, clHoldemTree *nodeOpp);
	void	AddWeightNode(tpTreePlayerInfo *info, double kW);
	bool	CutTree(enAction act);
//	void	GetCnCls(int *cnCls);
private:
//	tpFloat	NodeResult(clRootRiverTree *root, int nbHero, vector <float> &kH, vector <float> &kOpp, vector <float> &ev);
//	tpFloat	AddResEVToNode(clRootRiverTree *root, int nbHero, int cnB, vector <float> &resEV, tpFloat *w, vector <float> *evCur, vector <float> &kH, vector <float> &kOpp);
};

#ifdef VIEW_PARAM
struct tpCalcParam
{
	void	LoadParam(clCalcParam &par, int cnB)
	{
		_cnB = cnB;
		_cnCalc = par.CnCalc();
		_averRes = par.AverRes();
		_weightCalc = par.WeightCalc();
		tpFloat *begEv = par.BeginEV(), *begPart = par.BeginPart(cnB);
		for (int i = 0; i < cnB; i++)
		{
			_ev[i] = begEv[i];
			_part[i] = begPart[i];
			_absEv[i] = par.AbsEV(i);
		}
		par.GetWeightStrat(cnB, _weight);
	}

	int _cnCalc, _cnB;
	tpFloat _averRes, _weightCalc;
	tpFloat _ev[MAX_CN_BRANCH], _part[MAX_CN_BRANCH], _absEv[MAX_CN_BRANCH], _weight[MAX_CN_BRANCH];
};
#endif

int	GetDisLong(clSitHoldem &sit, tpDis *arrDis, tpDis *disH = NULL);
void	SetResSit3Pl(clSitHoldem &sit, int *arr);
int		*GetResSit3Pl(int *ptr, int f0, int f1, int f2);
void	CreateSit3(clSitHoldem &sit, clSitHoldem &sitIn);
void	BuildTreeNLAuto(clHoldemTree *ptrT, clSitHoldem sit, enNameTrade stF=TRADE_UNDEF);
void	BuildTreeNLFromHistory(clHoldemTree *ptrT, clSitHoldem &sit, clSitHoldem *sitH, int nbA);
void	BuildTreeNLPushFold(clHoldemTree *ptrT, clSitHoldem &sit, int nbA);
tpDis	GetDisPushFold(clSitHoldem &sit);
bool	CalcTreePushFold(clSitHoldem &sit, clHoldemTree *tree);
bool	ChangeTree(clHoldemTree *nodeIn, clHoldemTree *node, double kSt, double mult);
bool	FillTreeFromTree(clHoldemTree *nodeIn, clHoldemTree *node, double kSt, tpTableChangeNb *tab, double mult);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
