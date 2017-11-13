#include <math.h>
#include <time.h>

#include "ClastersBoard.h"
#include "Strategy.h"
//#include "C:\home\holdem\Strategy\Source\CalcSitEV.h"
#include "..\..\..\card\naborCards.h"

//#define _CRT_SECURE_NO_WARNINGS
#define	MIN_DIST_CLASTERS				0.1
#define	MAX_CN_CLASTERS					16
#define	CN_STAT_TREE					1000000

//double	DistClasters(clClasterS &cls0, clClasterS &cls1, int pot);
size_t glCn0 = 0, glCn1 = 0;

clDoubleCP	CalcEVForCalc(clTreeStrat *node, int nbH, tpCardsGame &cards, tpFloat0 kH, tpFloat0 kOpp);
int	FuncLong(clSitHoldem &sit, tpDis *arrDis, tpDis *disH);
int	FuncDeckMiddle(clSitHoldem &sit, tpDis *arrDis, tpDis *disH);
int	FuncDeckShort(clSitHoldem &sit, tpDis *arrDis, tpDis *disH);
int	FuncMiddle(clSitHoldem &sit, tpDis *arrDis, tpDis *disH);
int	RecalcDis(int cn, tpDis *arrDis, tpDis disH, tpDis disA, tpDis disC);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double	DistHand169(int nbHand, int nbHand2)
{
	double res = glListHand.UrDHand(nbHand) - glListHand.UrDHand(nbHand2);
	return res*res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrTreeStrat::ClearEV(int cnB)
{
	int dim = _param.size() / _sizeParam;
	for (int i = 0; i < dim; i++)
		CalcParam(i).ClearEV(cnB);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clAtrTreeStrat::SizeInFile()
{
	size_t res = sizeof(clSitHoldem) + sizeof(byte);
	res += SizeVectFile(_param);
	res += SizeVectFile(_vectInd);
	res += sizeof(short);
	if (_sitRes != NULL)
	{
		res += sizeof(short);
		if (_sitRes->_cnPl>0)
			res += _sitRes->_cnPl*sizeof(short);
		res += sizeof(short) + _sitRes->_res.size()*sizeof(clDoubleCP);
	}
	res+=sizeof(_statCn);
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrTreeStrat::WriteFile(int handle)
{
	_write(handle, &_sitA, sizeof(clSitHoldem));
	_write(handle, &_sizeParam, sizeof(byte));
//	_write(handle, &_verTree, sizeof(byte));
	WriteVectFile<byte>(handle, _param);
	WriteVectFile<short>(handle, _vectInd);
	short val = 0;
	if (_sitRes != NULL)
	{
		val = 1;
		_write(handle, &val, sizeof(short));
		_write(handle, &_sitRes->_cnPl, sizeof(short));
		if (_sitRes->_cnPl>0)
			_write(handle, _sitRes->_nbPl, _sitRes->_cnPl*sizeof(short));
		val = _sitRes->_res.size();
		_write(handle, &val, sizeof(short));
		if (val>0)
			_write(handle, &_sitRes->_res[0], val*sizeof(clDoubleCP));
	}
	else
		_write(handle, &val, sizeof(short));
	_write(handle, &_statCn, sizeof(_statCn));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrTreeStrat::ReadFile(int handle)
{
	_read(handle, &_sitA, sizeof(clSitHoldem));
	_read(handle, &_sizeParam, sizeof(byte));
//	_read(handle, &_verTree, sizeof(byte));
//	_verTree = TREE_FOR_CALC;
	ReadVectFile<byte>(handle, _param);
	ReadVectFile<short>(handle, _vectInd);
	short val;
	_read(handle, &val, sizeof(short));
	if (val)
	{
		if (_sitRes == NULL)
			_sitRes = new tpSitRes;
		_read(handle, &_sitRes->_cnPl, sizeof(short));
		if (_sitRes->_cnPl > 0)
			_read(handle, _sitRes->_nbPl, _sitRes->_cnPl*sizeof(short));
		_read(handle, &val, sizeof(short));
		_sitRes->_res.resize(val);
		if (val > 0)
			_read(handle, &_sitRes->_res[0], val*sizeof(clDoubleCP));
	}
	else if (_sitRes != NULL)
		delete _sitRes;
	_read(handle, &_statCn, sizeof(_statCn));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrTreeStrat::operator = (clAtrTreeStrat &atr)
{
	_sitA=atr._sitA;
	_sizeParam=atr._sizeParam;
	_param=atr._param;
	_vectInd=atr._vectInd;
	_statCn=atr._statCn;
	if (atr._sitRes == NULL)
	{
		if (_sitRes != NULL)
		{
			delete _sitRes;
			_sitRes = NULL;
		}
	}
	else
	{
		if (_sitRes == NULL)
			_sitRes = new tpSitRes;
		_sitRes = atr._sitRes;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		clAtrTreeStrat::GetNbForCls(int nb)
{
/*	enNameTrade trade = _sitA.NameTrade();
	if (trade == TRADE_PREFLOP) return nb;
	else if (trade == TRADE_FLOP)
	{
		size_t dim = _vectInd.size();
		if (dim != 0)
			for (size_t i = 0; i < dim;i++)
				if (_vectInd[i] == nb)
				{
					nb = i;
					break;
				}
		if (_ptrIndex != NULL)
		{
			dim = _ptrIndex->size();
			if (dim != 0)
				for (size_t i = 0; i < dim; i++)
					if ((*_ptrIndex)[i] == nb)
					{
						nb = i;
						break;
					}
		}
	}
	return nb;*/
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam0::ClearEV(int cnB)
{
	CnCalc() = 0;
	WeightCalc() = 0;
	AverRes() = 0;
	tpFloat0 *ev = BeginEV();
	cnB *= 2;
	for (int i = 0; i < cnB; i++)
		ev[i] = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam0::ClearEVAndSetWeight(int cnB)
{
	CnCalc() = 0;
	WeightCalc() = 0;
	AverRes() = 0;
	if (cnB == 0) return;
	tpFloat0 *ev = BeginEV();
	for (int i = 0; i < 2*cnB; i++)
		ev[i] = 0;
	tpFloat0 val = 1.f / cnB;
	ev += 2 * cnB;
	for (int i = 0; i < cnB; i++)
		ev[i] = val;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clCalcParam::GetWeight(int cnB, tpFloat *w)
{
	tpFloat *ev = BeginEV();
	tpFloat sum = 0;

	for (int i = 0; i < cnB; i++)
		if (ev[i]>(float)DOUBLE_0)
			sum += ev[i];
	if (sum > (float)DOUBLE_0)
	{
		//		glCn0++;
		for (int i = 0; i < cnB; i++)
			w[i] = (ev[i] >(float)DOUBLE_0) ? ev[i] / sum : 0;
	}
	else
	{
		//		glCn1++;
		for (int i = 0; i < cnB; i++)
			w[i] = 1. / cnB;
	}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam0::GetWeight(int cnB, tpFloat0 *w)
{
	tpFloat0 *ev = BeginEV();
	tpFloat0 sum = 0;

	if (cnB == 1)
	{
		w[0] = 1;
	}
	else if (cnB == 2)
	{
		if (ev[0] > (float)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (float)DOUBLE_0)
			sum += ev[1];
		if (sum > (float)DOUBLE_0)
		{
			w[0] = (ev[0] > (float)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (float)DOUBLE_0) ? ev[1] / sum : 0;
		}
		else
			w[0] = w[1] = 0.5f;
	}
	else if (cnB == 3)
	{
		if (ev[0] > (float)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (float)DOUBLE_0)
			sum += ev[1];
		if (ev[2] > (float)DOUBLE_0)
			sum += ev[2];
		if (sum > (float)DOUBLE_0)
		{
			w[0] = (ev[0] > (float)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (float)DOUBLE_0) ? ev[1] / sum : 0;
			w[2] = (ev[2] > (float)DOUBLE_0) ? ev[2] / sum : 0;
		}
		else
			w[0] = w[1] = w[2] = 1.f / 3;
	}
	else if (cnB == 4)
	{
		if (ev[0] > (float)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (float)DOUBLE_0)
			sum += ev[1];
		if (ev[2] > (float)DOUBLE_0)
			sum += ev[2];
		if (ev[3] > (float)DOUBLE_0)
			sum += ev[3];
		if (sum > (float)DOUBLE_0)
		{
			w[0] = (ev[0] > (float)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (float)DOUBLE_0) ? ev[1] / sum : 0;
			w[2] = (ev[2] > (float)DOUBLE_0) ? ev[2] / sum : 0;
			w[3] = (ev[3] > (float)DOUBLE_0) ? ev[3] / sum : 0;
		}
		else
			w[0] = w[1] = w[2] = w[3] = 0.25f;
	}
	else if (cnB == 5)
	{
		if (ev[0] > (float)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (float)DOUBLE_0)
			sum += ev[1];
		if (ev[2] > (float)DOUBLE_0)
			sum += ev[2];
		if (ev[3] > (float)DOUBLE_0)
			sum += ev[3];
		if (ev[4] > (float)DOUBLE_0)
			sum += ev[4];
		if (sum > (float)DOUBLE_0)
		{
			w[0] = (ev[0] > (float)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (float)DOUBLE_0) ? ev[1] / sum : 0;
			w[2] = (ev[2] > (float)DOUBLE_0) ? ev[2] / sum : 0;
			w[3] = (ev[3] > (float)DOUBLE_0) ? ev[3] / sum : 0;
			w[4] = (ev[4] > (float)DOUBLE_0) ? ev[4] / sum : 0;
		}
		else
			w[0] = w[1] = w[2] = w[3] = w[4] = 0.2f;
	}
	else if (cnB == 6)
	{
		if (ev[0] > (float)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (float)DOUBLE_0)
			sum += ev[1];
		if (ev[2] > (float)DOUBLE_0)
			sum += ev[2];
		if (ev[3] > (float)DOUBLE_0)
			sum += ev[3];
		if (ev[4] > (float)DOUBLE_0)
			sum += ev[4];
		if (ev[5] > (float)DOUBLE_0)
			sum += ev[5];
		if (sum > (float)DOUBLE_0)
		{
			w[0] = (ev[0] > (float)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (float)DOUBLE_0) ? ev[1] / sum : 0;
			w[2] = (ev[2] > (float)DOUBLE_0) ? ev[2] / sum : 0;
			w[3] = (ev[3] > (float)DOUBLE_0) ? ev[3] / sum : 0;
			w[4] = (ev[4] > (float)DOUBLE_0) ? ev[4] / sum : 0;
			w[5] = (ev[5] > (float)DOUBLE_0) ? ev[5] / sum : 0;
		}
		else
			w[0] = w[1] = w[2] = w[3] = w[4] = w[5] = 1.f/6;
	}
	else
	{
		for (int i = 0; i < cnB; i++)
			if (ev[i]>(float)DOUBLE_0)
				sum += ev[i];
		if (sum > (float)DOUBLE_0)
		{
			//		glCn0++;
			for (int i = 0; i < cnB; i++)
				w[i] = (ev[i] >(float)DOUBLE_0) ? ev[i] / sum : 0;
		}
		else
		{
			//		glCn1++;
			for (int i = 0; i < cnB; i++)
				w[i] = 1. / cnB;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam0::GetWeightStrat(int cnB, tpFloat0 *w)
{
	tpFloat0 *part = BeginPart(cnB);
	tpFloat0 sum = 0;
	for (int i = 0; i < cnB; i++)
		sum += part[i];
	if (sum > DOUBLE_0)
		for (int i = 0; i < cnB; i++)
			w[i] = part[i] / sum;
	else
		for (int i = 0; i < cnB; i++)
			w[i] = 1. / cnB;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam0::AddResEV(int cnB, tpFloat0 res, tpFloat0 *w, tpFloat0 *ev, tpFloat0 k, tpFloat0 kOpp)
{
	tpFloat0 *evW = BeginEV();
	tpFloat0 *part = BeginPart(cnB);
	CnCalc()++;
	WeightCalc() += kOpp;
	AverRes() += res;
	for (int i = 0; i < cnB; i++)
	{
		evW[i] += ev[i] - res;
		part[i] += k * w[i];
	}
	unsigned val = CnCalc() << 10;
	if (val == 0)
	{						// уменьшаем накапливаемые величины
		WeightCalc() /= 2;
		AverRes() /= 2;
		for (int i = 0; i < cnB; i++)
		{
			evW[i] /= 2;
			part[i] /= 2;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam0::AddResEVForCalc(int cnB, tpFloat0 res, tpFloat0 *w, tpFloat0 *ev, tpFloat0 k, tpFloat0 kOpp)
{
	tpFloat0 *evW = BeginEV();
	tpFloat0 *part = BeginPart(cnB);
	CnCalc()++;
	WeightCalc() += kOpp;
	AverRes() += res;
	for (int i = 0; i < cnB; i++)
	{
		evW[i] += ev[i] - res;
		part[i] += k * w[i];
	}

	tpFloat0 sum = 0;
	tpFloat0 *ptrW = GetWeightForCalc(cnB);
	for (int i = 0; i < cnB; i++)
		if (evW[i]>DOUBLE_0)
			sum += evW[i];
	if (sum > (float)DOUBLE_0)
		for (int i = 0; i < cnB; i++)
			ptrW[i] = (evW[i] >DOUBLE_0) ? evW[i] / sum : 0;
	else
		for (int i = 0; i < cnB; i++)
			ptrW[i] = 1. / cnB;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam0::ChangeStratWeight(int cnB, tpFloat0 m)
{
	tpFloat0 *part = BeginPart(cnB);
	for (int i = 0; i < cnB; i++)
		part[i] *= m;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam0::SetParam(int cnB,tpFloat0 *w, int cn)
{
	CnCalc()=cn;
	WeightCalc() = cn;
	AverRes() =0;
	tpFloat0 *evW = BeginEV();
	tpFloat0 *part = BeginPart(cnB);
	for (int i = 0; i < cnB; i++)
	{
		evW[i] = 0;
		part[i] = w[i]*cn;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clCalcParam::GetWeight(int cnB, tpFloat *w)
{
	tpFloat *ev = BeginEV();
	tpFloat sum = 0, res = 0, arr[MAX_CN_BRANCH];
	for (int i = 0; i < cnB; i++)
		res += ev[i];
	res /= cnB;

	for (int i = 0; i < cnB; i++)
	{
		arr[i] = ev[i] - res;
		if (arr[i]>DOUBLE_0)
			sum += arr[i];
	}
	if (sum > DOUBLE_0)
		for (int i = 0; i < cnB; i++)
			w[i] = (arr[i] > DOUBLE_0) ? arr[i] / sum : 0;
	else
		for (int i = 0; i < cnB; i++)
			w[i] = 1. / cnB;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::GetWeightStrat(int cnB, tpFloat *w)
{
	tpFloat *part = BeginPart(cnB);
	tpFloat sum = 0;
	for (int i = 0; i < cnB; i++)
		sum += part[i];
	if (sum > DOUBLE_0)
		for (int i = 0; i < cnB; i++)
			w[i] = part[i] / sum;
	else
		for (int i = 0; i < cnB; i++)
			w[i] = 1. / cnB;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::AddResEV(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp)
{
	tpFloat *evW = BeginEV();
	tpFloat *part = BeginPart(cnB);
	CnCalc()++;
	WeightCalc() += kOpp;
	for (int i = 0; i < cnB; i++)
	{
		evW[i] += ev[i];
		part[i] += k * w[i];
	}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int clTreeStrat::AddBranch(clSitHoldem &sit)
{
	clTreeStrat *node = AddBranch();
	node->AtrTree()->_sitA = sit;
	node->_weight = 0;
	return CnBranch() - 1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CreateRandom(clSitHoldem &sit)
{
	static clCol deck;
	if (deck.CnCards() == 0)
		deck.New();
	deck.ShufleOldCol();
	sit.DealCards(deck);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*tpFloat	CreateRandom(clSitHoldem &sit, clClasterS *cls, clHandsGroupEx *arrInHand)
{
	static clCol deck;
	if (deck.CnCards() == 0)
		deck.New();
	deck.ShufleOldCol();
	sit.DealCards(deck);
	return 1;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		clTreeStrat::GetNbCls(tpCardsGame &cards,int ind)
{
	if (CnClasters() == 1) return 0;
	enNameTrade trade = GetSit().NameTrade();
	int nbH = 0;
	if (trade == TRADE_PREFLOP)
		nbH=GetHandIndex(cards._handsPl[ind]);
	else if (trade == TRADE_FLOP)
		nbH = cards._indFlop[ind];
	else if (trade == TRADE_TURN)
		nbH = cards._indTurn[ind];
	else if (trade == TRADE_RIVER)
		nbH = cards._indRiver[ind];
	if (AtrTree()->_ptrIndex != NULL)
		nbH = AtrTree()->_ptrIndex[nbH];
	if (AtrTree()->_vectInd.size() == 0)
		return nbH;
	else return AtrTree()->_vectInd[nbH];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		clTreeStrat::GetNbCls(tpHand &hand, tpCard *board)
{
	if (CnClasters() == 1) return 0;
	enNameTrade trade = GetSit().NameTrade();
	int nbH = 0;
	if (trade == TRADE_PREFLOP)
		nbH = GetHandIndex(hand);
	else if (trade == TRADE_FLOP)
		nbH = GetFlopHandIndex(board,hand);
	else if (trade == TRADE_TURN)
		nbH = GetTurnHandIndex(board, hand);
	else if (trade == TRADE_RIVER)
		nbH = GetTurnAndRiverHandIndex(board, hand,nbH);
//	if (nbH == -1) return -1;
	if (AtrTree()->_ptrIndex != NULL)
		nbH = AtrTree()->_ptrIndex[nbH];
	if (AtrTree()->_vectInd.size() == 0)
		return nbH;
	else return AtrTree()->_vectInd[nbH];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*clDoubleCP	clTreeStrat::CalcEV(tpCardsGame &cards, tpFloat k0)
{
	AtrTree()->_statCn += k0;
	if (EndedNode())
		return Result(cards);
	int ind = Indicator();
	if (ind == -1 || CnBranch() == 1) return Branch(0)->CalcEV(cards, k0);
	int nbCls = GetNbCls(cards, ind);
	clCalcParam cls = AtrTree()->CalcParam(nbCls);
	tpFloat ev[MAX_CN_BRANCH];
	tpFloat w[MAX_CN_BRANCH];
	cls.GetWeight(CnBranch(), w);
	clDoubleCP res; res = 0;
	for (int k = 0; k < CnBranch(); k++)
	{
		clDoubleCP f0 = 0;
		if(k0>DOUBLE_0 || w[k]>DOUBLE_0) f0=Branch(k)->CalcEV(cards, k0*w[k]);
		ev[k] = f0[ind];
		res += f0*w[k];
	}
	if (k0 > DOUBLE_0)
		cls.AddResEV(CnBranch(), res[ind], w, ev, k0);
	return res;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//size_t cnRecOpp = 0, cnAddRes = 0, cnRecH = 0, cnGetWeight = 0, cnCalcParam=0;
clDoubleCP	clTreeStrat::CalcEV(int nbH, tpCardsGame &cards, tpFloat0 kH, tpFloat0 kOpp)
{
	AtrTree()->_statCn += kH*kOpp;
	if (EndedNode())
		return Result(cards)*kOpp;
	int ind = Indicator();
	if (ind == -1 || CnBranch() == 1) return Branch(0)->CalcEV(nbH, cards, kH, kOpp);
	int nbCls = GetNbCls(cards, ind);
	//cnCalcParam++;
	clCalcParam0 cls = AtrTree()->CalcParam(nbCls);
	tpFloat0 w[MAX_CN_BRANCH];

	/*	struct stGGG{
	int _cnCalc;
	tpFloat _weightCalc, _averRes;
	tpFloat _ev[8], _part[8]; 
	}  val0 = *((stGGG *)cls._ptr);
	if (val0._weightCalc > DOUBLE_0)
	for (int k = 0; k < CnBranch(); k++)
	{
	val0._ev[k] = cls.AbsEV(k);
	val0._part[k]=cls.BeginPart(CnBranch())[k];
	}
	*/
	cls.GetWeight(CnBranch(), w);
	//cnGetWeight++;
	clDoubleCP res; res = 0;
	if (ind == nbH)
	{
		tpFloat0 ev[MAX_CN_BRANCH];// = { 0, 0, 0, 0, 0, 0, 0, 0 };
		for (int k = 0; k < CnBranch(); k++)
		{
			clDoubleCP f0 = Branch(k)->CalcEV(nbH, cards, kH*w[k], kOpp);
			//cnRecH++;
			ev[k] = f0[ind];
			res += f0*w[k];
		}
		cls.AddResEV(CnBranch(), res[ind], w, ev, kH, kOpp);
		//cnAddRes++;
	}
	else
	{
		for (int k = 0; k < CnBranch(); k++)
			if (w[k]>DOUBLE_0)
			{
				clDoubleCP f0 = Branch(k)->CalcEV(nbH, cards, kH, kOpp*w[k]);
				//cnRecOpp++;
				res += f0;
			}
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
vector <clDoubleCP> clTreeStrat::CalcEV(int nbH, int dim, tpCardsGame *cards, vector <tpFloat0> &kH, tpFloat0 kOpp)
{
	vector <clDoubleCP> res;res.resize(dim);
	for (int i = 0; i < dim; i++)
		AtrTree()->_statCn += kH[i]*kOpp;
	if (EndedNode())
	{
		for (int i = 0; i < dim; i++)
			res[i] = Result(cards[i])*kOpp;
		return res;
	}
	int ind = Indicator();
	if (ind == -1 || CnBranch() == 1) return Branch(0)->CalcEV(nbH, dim, cards, kH, kOpp);

	ZeroMemory(&res[0], dim*sizeof(tpFloat0));
	if (ind == nbH)
	{
		vector <tpFloat0> w; w.resize(dim*MAX_CN_BRANCH);
		vector <clCalcParam0> cls; cls.resize(dim);
		for (int i = 0; i < dim; i++)
		{
			int nbCls = GetNbCls(cards[i], ind);
			cls[i] = AtrTree()->CalcParam(nbCls);
			cls[i].GetWeight(CnBranch(), &w[i*MAX_CN_BRANCH]);
		}

		vector <tpFloat0> ev; ev.resize(dim*MAX_CN_BRANCH);
		for (int k = 0; k < CnBranch(); k++)
		{
			vector <tpFloat0> kHNew; kHNew = kH;
			for (int i = 0; i < dim; i++)
				kHNew[i] *= w[i*MAX_CN_BRANCH + k];
			vector <clDoubleCP> f0 = Branch(k)->CalcEV(nbH, dim, cards, kHNew, kOpp);
			for (int i = 0; i < dim; i++)
			{
				ev[i*MAX_CN_BRANCH+k] = f0[i][ind];
				res[i] += f0[i] * w[i*MAX_CN_BRANCH + k];
			}
		}
		for (int i = 0; i < dim; i++)
			cls[i].AddResEV(CnBranch(), res[i][ind], &w[i*MAX_CN_BRANCH], &ev[i*MAX_CN_BRANCH], kH[i], kOpp);
	}
	else
	{
		int nbCls = GetNbCls(cards[0], ind);
		clCalcParam0 cls = AtrTree()->CalcParam(nbCls);
		tpFloat0 w[MAX_CN_BRANCH];
		cls.GetWeight(CnBranch(), w);
		for (int k = 0; k < CnBranch(); k++)
			if (w[k]>DOUBLE_0)
			{
				vector <clDoubleCP> f0 = Branch(k)->CalcEV(nbH, dim, cards, kH, kOpp*w[k]);
				for (int i = 0; i < dim; i++)
					res[i] += f0[i];
			}
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP	clTreeStrat::CalcEVForCalc(int nbH, tpCardsGame &cards, tpFloat0 kH, tpFloat0 kOpp)
{
	AtrTree()->_statCn += kH*kOpp;
	if (EndedNode())
		return Result(cards)*kOpp;
	int ind = Indicator();
	if (ind == -1 || CnBranch() == 1) return Branch(0)->CalcEVForCalc(nbH, cards, kH, kOpp);

	int nbCls=GetNbCls(cards, ind);
	clCalcParam0 cls = AtrTree()->CalcParam(nbCls);
	tpFloat0 *w = cls.GetWeightForCalc(CnBranch());
	clDoubleCP res; res = 0;
	if (ind == nbH)
	{
		tpFloat0 ev[MAX_CN_BRANCH];// = { 0, 0, 0, 0, 0, 0, 0, 0 };
		for (int k = 0; k < CnBranch(); k++)
		{
			clDoubleCP f0 = Branch(k)->CalcEVForCalc(nbH, cards, kH*w[k], kOpp);
			ev[k] = f0[ind];
			res += f0*w[k];
		}

//		cls.AddResEVForCalc(CnBranch(), res[ind], w, ev, kH, kOpp);
		{
			int cnB = CnBranch();
			tpFloat0 *evW = cls.BeginEV();
			tpFloat0 *part = cls.BeginPart(cnB);
			cls.CnCalc()++;
			cls.WeightCalc() += kOpp;
			cls.AverRes() += res[ind];
			for (int i = 0; i < cnB; i++)
			{
				evW[i] += ev[i] - res[ind];
				part[i] += kH * w[i];
			}

			tpFloat0 sum = 0;
			tpFloat0 *ptrW = cls.GetWeightForCalc(cnB);
			for (int i = 0; i < cnB; i++)
				if (evW[i]>(float)DOUBLE_0)
					sum += evW[i];
			if (sum > (float)DOUBLE_0)
				for (int i = 0; i < cnB; i++)
					ptrW[i] = (evW[i] >(float)DOUBLE_0) ? evW[i] / sum : 0;
			else
				for (int i = 0; i < cnB; i++)
					ptrW[i] = 1. / cnB;
		}

	}
	else
	{
		for (int k = 0; k < CnBranch(); k++)
			if (w[k]>DOUBLE_0)
			{
				clDoubleCP f0 = Branch(k)->CalcEVForCalc(nbH, cards, kH, kOpp*w[k]);
				res += f0;
			}
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat0 clTreeStrat::EVNabor(tpCardsGame &cards, int nb)
{
	if (CnBranch() < 2) return 0;
	int ind = Indicator();
	if (ind == -1) return -1000000;
	int nbCls = GetNbCls(cards, ind);
	clCalcParam0 cls = AtrTree()->CalcParam(nbCls);
	return cls.AbsEV(nb);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeStrat *clTreeStrat::GetStratBranch(tpCardsGame &cards, clHandsGroupEx *ghH)
{
	if (EndedNode())
		return this;
	int ind = Indicator();
	if (ind == -1 || CnBranch() == 1)
		return Branch(0)->GetStratBranch(cards,ghH);
	int nbCls = GetNbCls(cards, ind);
	clCalcParam0 cls = AtrTree()->CalcParam(nbCls);

	/*struct stGGG{
		int _cnCalc;
		tpFloat _weightCalc,_averRes;
		tpFloat _ev[4], _part[4];
	}  val0 = *((stGGG *)cls._ptr);
	if (val0._weightCalc > DOUBLE_0)
		for (int k = 0; k < 4; k++)
		{
			val0._ev[k]=cls.AbsEV(k);
			val0._part[k] /= val0._weightCalc;
		}*/

	tpFloat0 w[MAX_CN_BRANCH];
	cls.GetWeightStrat(CnBranch(), w);
	tpFloat0 val = (tpFloat0)rand() / RAND_MAX;
	int nbB = CnBranch() - 1;
	for (int i = 0; i < CnBranch(); i++)
		if (w[i] > val)
		{
			nbB = i;
			break;
		}
		else
			val -= w[i];
	if (ghH != NULL)
		ChangeGHAll(nbB, cards._cardTable,ghH[ind]);
	return Branch(nbB);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeStrat *clTreeStrat::GetStratEndedNode(tpCardsGame &cards)
{
	if (EndedNode())
		return this;
	int ind = Indicator();
	if (ind == -1 || CnBranch() == 1) return Branch(0)->GetStratEndedNode(cards);
	int nbCls = GetNbCls(cards, ind);
	clCalcParam0 cls = AtrTree()->CalcParam(nbCls);

/*	struct stGGG{
		int _cnCalc;
		tpFloat _weightCalc, _averRes;
		tpFloat _ev[4], _part[4];
	}  val0 = *((stGGG *)cls._ptr);
	if (val0._weightCalc > DOUBLE_0)
		for (int k = 0; k < 4; k++)
		{
			val0._ev[k] = cls.AbsEV(k);
			val0._part[k] /= val0._weightCalc;
		}*/
	
	tpFloat0 w[MAX_CN_BRANCH];
	cls.GetWeightStrat(CnBranch(), w);
	tpFloat0 val = (tpFloat0)rand() / RAND_MAX;
	for (int i = 0; i < CnBranch(); i++)
		if (w[i] > val)
			return Branch(i)->GetStratEndedNode(cards);
		else
			val -= w[i];
	return Branch(CnBranch() - 1)->GetStratEndedNode(cards);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clCalcParam0 clTreeStrat::FindClaster(clSitHoldem &sit)
{
	if (CnClasters() == 1) return AtrTree()->CalcParam(0);
	enNameTrade trade = sit.NameTrade();
	int nbH = 0;
	if (trade == TRADE_PREFLOP)
		nbH = ((clHand *)sit.IndicatorCards())->NbHand();
	else if (trade == TRADE_FLOP)
		nbH = GetFlopHandIndex(sit.CardsTable(), sit.PlayerHand(sit.Indicator()));
	else if (trade == TRADE_TURN)
		nbH = GetTurnHandIndex(sit.CardsTable(), sit.PlayerHand(sit.Indicator()));
	else if (trade == TRADE_RIVER)
		nbH = GetRiverHandIndex(sit.CardsTable(), sit.PlayerHand(sit.Indicator()));
	if (AtrTree()->_ptrIndex != NULL)
		nbH = AtrTree()->_ptrIndex[nbH];
	if (AtrTree()->_vectInd.size() != 0)
		nbH= AtrTree()->_vectInd[nbH];

	return AtrTree()->CalcParam(nbH);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clAnsiString	clTreeStrat::NameForNode()
{
	clSitHoldem &sit = AtrTree()->_sitA;
	if (sit.Indicator() == -1)
	{
		if (sit.CNPlayerIsCard() == 1)
		{
			for (int i = 0; i < sit.CnPlayer(); i++)
				if (sit.PlayerIsCard(i))
					return (clAnsiString)"Win " + HistPosToString(i);
		}
		if (sit.CNPlayerInTrade()==0)
			return (clAnsiString)"All-In";
		if (sit.CnCardTable() == 0)
			return (clAnsiString)"Flop";
		if (sit.CnCardTable() == 3)
			return (clAnsiString)"Turn";
		if (sit.CnCardTable() == 4)
			return (clAnsiString)"River";
		if (sit.CnCardTable() == 4)
			return (clAnsiString)"ShowDown";
	}
	else
	{
		return HistPosToString(sit.Indicator());
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clTreeStrat::SetStartParam(int cnClsFlop, int cnClsTurn, int cnClsRiver, int *indFlop, int *indTurn, int *indRiver, bool mem)
{
	AtrTree()->_strat = NULL;
	clSitHoldem &sit = AtrTree()->_sitA;
	AtrTree()->_statCn = 0;
	if (EndedNode())
	{
		if (AtrTree()->_sitRes == NULL)
			AtrTree()->_sitRes = new tpSitRes;
		CalcSitRes(AtrTree()->_sitA, AtrTree()->_sitRes);
		AtrTree()->_ptrIndex = NULL;
		return AtrTree()->SizeInFile();
	} 
//	clClasterS cls;
//	cls._cn = CnBranch();
//	cls.ClearEV();
	size_t dim = 0;
	if (sit.NameTrade() == TRADE_PREFLOP)
	{
		dim = CN_HAND;
		AtrTree()->_ptrIndex = NULL;
	}
	else if (sit.NameTrade() == TRADE_FLOP)
	{
		dim = cnClsFlop;
		AtrTree()->_ptrIndex = indFlop;
		if (indFlop == NULL)
			dim = CN_NABOR_HAND_FLOP;
	}
	else if (sit.NameTrade() == TRADE_TURN)
	{
		dim = cnClsTurn;
		AtrTree()->_ptrIndex = indTurn;
	}
	else
	{
		dim = cnClsRiver;
		AtrTree()->_ptrIndex = indRiver;
	}
	if (sit.Indicator() == -1 || CnBranch()<2 || dim==0)
	{
		dim = 0;
		AtrTree()->_ptrIndex = NULL;
	}
	if (AtrTree()->_verTree == TREE_FOR_CALC)
	{
		int size = clCalcParam0::SizeParamForCalc(CnBranch());
		AtrTree()->_sizeParam = size;
		if (dim > 0)
		{
			AtrTree()->_param.resize(dim*size);
			byte *ptr = &AtrTree()->_param[0];
			for (size_t i = 0; i < dim; i++, ptr += size)
				((clCalcParam0)(ptr)).ClearEVAndSetWeight(CnBranch());
		}
	}
	else
	{
		int size = clCalcParam0::SizeParam(CnBranch());//   2 * (sizeof(int) + CnBranch() * sizeof(tpFloat));
		AtrTree()->_sizeParam = size;
		if (dim > 0)
		{
			AtrTree()->_param.resize(dim*size);
			byte *ptr = &AtrTree()->_param[0];
			for (size_t i = 0; i < dim; i++, ptr += size)
				((clCalcParam0)(ptr)).ClearEV(CnBranch());
		}
	}
	size_t res = AtrTree()->SizeInFile();
	tpFloat0 val = 1. / CnBranch();
	if (!mem)
	{
		AtrTree()->_param.clear();
		//std::vector<byte>().swap(AtrTree()->_param);
		AtrTree()->_param.shrink_to_fit();
	}
	for (int i = 0; i < CnBranch(); i++)
	{
		res+=Branch(i)->SetStartParam(cnClsFlop, cnClsTurn, cnClsRiver, indFlop, indTurn, indRiver,mem);
		Branch(i)->_weight = val;
	}
	_name = NameForNode();
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::SetIndexForNabor(int *indFlop, int *indTurn, int *indRiver)
{
	clSitHoldem &sit = AtrTree()->_sitA;
	if (EndedNode())
	{
		AtrTree()->_ptrIndex = NULL;
		return;
	} 
	if (sit.NameTrade() == TRADE_PREFLOP)
		AtrTree()->_ptrIndex = NULL;
	else if (sit.NameTrade() == TRADE_FLOP)
	{
		AtrTree()->_ptrIndex = indFlop;
	}
	else if (sit.NameTrade() == TRADE_TURN)
		AtrTree()->_ptrIndex = indTurn;
	else
		AtrTree()->_ptrIndex = indRiver;
	if (sit.Indicator() == -1 || CnBranch()<2)
		AtrTree()->_ptrIndex = NULL;
	for (int i = 0; i < CnBranch(); i++)
		Branch(i)->SetIndexForNabor(indFlop, indTurn, indRiver);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::ReCalcWeight(clHandsGroupEx **arrH, bool stratB)
{
	int ind = AtrTree()->_sitA.Indicator();
	if (ind == -1) return;
	if (GetSit().NameTrade() != TRADE_PREFLOP) return;
/*	if (CnBranch()==1)
	{
		Branch(0)->_weight = 1;
		if (GetSit().NameTrade() == TRADE_PREFLOP && Branch(0)->GetSit().NameTrade() == TRADE_FLOP)
		{
			clHandsGroupEx gh[CN_PLAYER], *ptrGH[CN_PLAYER];
			for (int i = 0; i < GetSit().CnPlayer(); i++)
			{
				ptrGH[i] = &gh[i];
				gh[i]._trade = TRADE_FLOP;
				gh[i]._vect.resize(CN_NABOR_HAND_FLOP);
				ZeroMemory(&gh[i]._vect[0], CN_NABOR_HAND_FLOP*sizeof(tpFloat));
				for (int j = 0; j < CN_HAND; j++)
				{
					int dimV = glHandToFlopK[j].size();
					tpFloat w0 = arrH[i]->_vect[j];
					for (int k = 0; k < dimV; k++)
						gh[i][glHandToFlopK[j][k]._index] += w0*glHandToFlopK[j][k]._cn;
				}
			}
			Branch(0)->ReCalcWeight(ptrGH, stratB);
			return;
		}
		Branch(0)->ReCalcWeight(arrH, stratB);
		return;
	}*/
	clHandsGroupEx gh, *ptrGH;
	ptrGH = arrH[ind];
	for (int i = 0; i < CnBranch(); i++)
	{
		Branch(i)->_weight = ChangeGH(i, &gh, ptrGH, stratB);
		arrH[ind] = &gh;
		Branch(i)->ReCalcWeight(arrH, stratB);
	}
	arrH[ind] = ptrGH;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::GetGH(clSitHoldem &sit0, clHandsGroupEx *ghH)
{
	clSitHoldem sit;
	sit = sit0;
	int ind = sit.Indicator();
	int nbA = sit.CurNbAct();
	sit.FirstSit();
	GetGHSit(sit, ind, nbA,ghH);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::GetGHSit(clSitHoldem &sit, int ind, int nbA, clHandsGroupEx *ghH)
{
	if (GetSit().Indicator() == ind && sit.CurNbAct() == nbA) 
		return;
	tpDis dis = sit.GetActIndicator()->_dis;
	clTreeStrat *ptr = FindBranch(dis);
	if (ptr == NULL)
	{
		if (dis._act == ACT_RAISE) dis._act = ACT_ALL_IN;
		ptr = FindBranch(dis);
	}
	if (ptr != NULL)
	{
		int nbH = GetSit().Indicator();
		tpCard cards[5];
		for (int k = 0; k < ptr->GetSit().CnCardTable(); k++)
			cards[k] = sit.CardsTable()[k];
		if (nbH == -1)
		{
			for (int i1 = 0; i1 < sit.CnPlayer(); i1++)
				if (ghH[i1]._vect.size() == ALL_CN_HAND)
					switch (ptr->GetSit().CnCardTable())
				{
						case 3:
							ghH[i1].SkipPreFlopAllHands(cards[0]);
							ghH[i1].SkipPreFlopAllHands(cards[1]);
							ghH[i1].SkipPreFlopAllHands(cards[2]);
							break;
						case 4: ghH[i1].SkipPreFlopAllHands(cards[3]); break;
						case 5: ghH[i1].SkipPreFlopAllHands(cards[4]); break;
				}
		}
		else
			if (ghH[nbH]._vect.size() == ALL_CN_HAND)
				for (int i = 0; i < CnBranch(); i++)
					if (ptr == Branch(i))
						ChangeGHAll(i, cards, ghH[nbH]);
		sit.ExecDis(dis);
		ptr->GetGHSit(sit, ind, nbA,ghH);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::ChangeGHAll(int nb, tpCard *board, clHandsGroupEx &ghH)
{
	int ind = Indicator();
	if (ind == -1 || CnBranch() == 1) return;
	for (int i = 0; i < CN_CARD_COL; i++)
		for (int j = i + 1; j < CN_CARD_COL; j++)
		{
			int nbH = j*(j - 1) / 2 + i;
			if (ghH[nbH] > DOUBLE_0)
			{
				tpHand hand = { j, i };
				int nbCls = GetNbCls(hand, board);
				clCalcParam0 cls = AtrTree()->CalcParam(nbCls);
				tpFloat0 w[MAX_CN_BRANCH];
				cls.GetWeightStrat(CnBranch(),w);
				ghH[nbH] *= w[nb];
			}
		}
	ghH.Norm();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double	clTreeStrat::ChangeGH(int nb, clHandsGroupEx *gh, clHandsGroupEx *ghIn,bool stratB)
{
	size_t dim = AtrTree()->CnClasters();
	if (dim == 0) return 1;
	int cnB = CnBranch();
	tpFloat0 w[MAX_CN_BRANCH];
	if (dim == 1)
	{
		*gh = *ghIn;
		clCalcParam0 cls = AtrTree()->CalcParam(0);
		if (stratB)
			cls.GetWeightStrat(cnB, w);
		else
			cls.GetWeight(cnB, w);
		return w[nb];
	}
	int dimIn = ghIn->_vect.size();
	//gh->_trade = GetSit().NameTrade();
	gh->_vect.resize(dimIn);
	if (AtrTree()->_vectInd.size() == 0)
	{
		if (stratB)
			for (int i = 0; i < dimIn; i++)
			{
				AtrTree()->CalcParam(i).GetWeightStrat(cnB,w);
				(*gh)[i] = (*ghIn)[i] * w[nb];
			}
		else
			for (int i = 0; i < dimIn; i++)
			{
				AtrTree()->CalcParam(i).GetWeight(cnB,w);
				(*gh)[i] = (*ghIn)[i] * w[nb];
			}
	}
	else
		if (stratB)
			for (int i = 0; i < dimIn; i++)
			{
				int nbCls = AtrTree()->_vectInd[i];
				AtrTree()->CalcParam(nbCls).GetWeightStrat(cnB,w);
				(*gh)[i] = (*ghIn)[i] * w[nb];
			}
		else
			for (int i = 0; i < dimIn; i++)
			{
				int nbCls = AtrTree()->_vectInd[i];
				AtrTree()->CalcParam(nbCls).GetWeight(cnB,w);
				(*gh)[i] = (*ghIn)[i] * w[nb];
			}
	return gh->Norm();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::ClearEV()
{
	AtrTree()->ClearEV(CnBranch());
	for (int i = 0; i < CnBranch(); i++)
		Branch(i)->ClearEV();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::ClearStatWeight()
{
	AtrTree()->_statCn = 0;
	for (int i = 0; i < CnBranch(); i++)
		Branch(i)->ClearStatWeight();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::CalcWeightStat()
{
	tpFloat0 sum=0;
	for (int i = 0; i < CnBranch(); i++)
	{
		sum += Branch(i)->AtrTree()->_statCn;
		Branch(i)->CalcWeightStat();
	}
	if (sum > DOUBLE_0)
		for (int i = 0; i < CnBranch(); i++)
			Branch(i)->_weight = Branch(i)->AtrTree()->_statCn / sum;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeStrat *clTreeStrat::FindSitNode(clSitHoldem &sit)
{
	int nbA = GetSit().CurNbAct();
	if (sit.CurNbAct()==nbA)
		return this;
	tpDis dis = sit.GetAct(nbA)->_dis;
	clTreeStrat *ptr = FindBranch(dis);
/*	if (ptr == NULL)
	{
		if (dis._act == ACT_RAISE) dis._act = ACT_ALL_IN;
		ptr = FindBranch(dis);
	}*/
	if (ptr != NULL)
		return ptr->FindSitNode(sit);
	return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeStrat *clTreeStrat::FindBeforeEqualDis(clSitHoldem &sit)
{
	int nbA = GetSit().CurNbAct();
	if (sit.CurNbAct() == nbA)
		return this;
	tpDis dis = sit.GetAct(nbA)->_dis;
	clTreeStrat *ptr = FindEqBranch(dis);
	if (ptr != NULL)
		return ptr->FindBeforeEqualDis(sit);
	return this;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeStrat *clTreeStrat::FindBranch(tpDis dis)
{
	for (int i = 0; i<_cnB; i++)
	{
		tpDis disT = Branch(i)->GetDis();
		if (disT == dis)
			return Branch(i);
	}
	return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeStrat *clTreeStrat::FindEqBranch(tpDis dis)
{
	for (int i = 0; i<_cnB; i++)
	{
		tpDis disT = Branch(i)->GetDis();
		if (disT == dis)
			return Branch(i);
	}
	if (dis._act > ACT_CALL && dis._act<ACT_ALL_IN)
	{
		int nbMin = -1, min = 0;
		for (int i = 0; i<_cnB; i++)
		{
			tpDis disT = Branch(i)->GetDis();
			if (disT._act > ACT_CALL && disT._act < ACT_ALL_IN)
			{
				int res = abs(disT._money - dis._money);
				if (nbMin == -1)
				{
					min = res;
					nbMin = i;
				}
				else if (res < min)
				{
					min = res;
					nbMin = i;
				}
			}
		}
		if (nbMin != -1)
			if (min < GetSit().BigBlind())
				return Branch(nbMin);
	}
	return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeStrat *clTreeStrat::FindBranch(enNameTrade trade)
{
	if (GetSit().NameTrade() == trade) return this;
	if (GetSit().Indicator() == -1)
		return Branch(0)->FindBranch(trade);
	clTreeStrat *branch = FindBranch(HistCreateDis(ACT_CHECK));
	if (branch==NULL)
		branch = FindBranch(HistCreateDis(ACT_FOLD));
	if (branch == NULL) return NULL;
	return branch->FindBranch(trade);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	clTreeStrat::SetNbNode(int nb0)
{
	int nb = nb0;
	SetNbNodeA(nb);
	return nb;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::SetNbNodeA(int &nb)
{
	_val->_statCn = nb++;
	for (int i = 0; i < CnBranch(); i++)
		Branch(i)->SetNbNodeA(nb);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::ChangeStratWeight(tpFloat0 m)
{
	int dim = AtrTree()->CnClasters();
	for (int i = 0; i < dim; i++)
	{
		clCalcParam0 p = AtrTree()->CalcParam(i);
		p.ChangeStratWeight(CnBranch(), m);
	}
	for (int i = 0; i < CnBranch(); i++)
		Branch(i)->ChangeStratWeight(m);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeStrat::ChangeStatWeight(tpFloat0 m)
{
	AtrTree()->_statCn*=m;
	for (int i = 0; i < CnBranch(); i++)
		Branch(i)->ChangeStatWeight(m);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	BuildTreeAuto(clTreeStrat *ptrT, clSitHoldem &sit, int cnBet)
{
	clSitHoldem sit0;
	if (sit.EndedSit())
	{
		ptrT->AtrTree()->_sitA = sit;
		return;
	}
	int ind = sit.Indicator();
	if (ind == -1)
	{
		ptrT->AtrTree()->_sitA = sit;
		if (sit.CNPlayerIsCard() == 1)
			return;
		if (sit.NameTrade() == TRADE_PREFLOP && sit.CNPlayerInTrade()>1)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELFLOP, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeAuto(ptrT->Branch(nb), sit,cnBet);
		}
		else if (sit.NameTrade() == TRADE_FLOP)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELTURN, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeAuto(ptrT->Branch(nb), sit, cnBet);
		}
		else if (sit.NameTrade() == TRADE_TURN)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELRIVER, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeAuto(ptrT->Branch(nb), sit, cnBet);
		}
		return;
	}
	/*	if (ind == -1)
		switch (sit.CnCardTable())
		{
			case 0: sit.ExecDis(HistCreateDis(ACT_DELFLOP)); break;
			case 3: sit.ExecDis(HistCreateDis(ACT_DELTURN)); break;
			case 4: sit.ExecDis(HistCreateDis(ACT_DELRIVER)); break;
			default: return;
		}*/
	int betP = sit.BigBlind()*((sit.CnCardTable() > 3) ? 2 : 1);
	tpDis disC = sit.CreateDisCallCheck();
	int bet = sit.MaxBet();
	if (bet < cnBet*betP)
	{
		tpDis dis = HistCreateDis(ACT_RAISE_TO, sit.MaxBet()+betP);
		dis = sit.CreateDis(dis);
		if (!(dis == disC))
		{
			sit0 = sit;
			sit0.ExecDis(dis);
			int nb = ptrT->AddBranch(sit0);
			clTreeStrat *branch = ptrT->Branch(nb);
			BuildTreeAuto(branch, sit0, cnBet);
		}
	}

	sit0 = sit;
	sit0.ExecDis(disC);
	int nb = ptrT->AddBranch(sit0);
	clTreeStrat *branch = ptrT->Branch(nb);
	BuildTreeAuto(branch, sit0, cnBet);

	tpDis dis = sit.CreateDis(HistCreateDis(ACT_FOLD));
	if (!(dis == disC))
	{
		sit0 = sit;
		sit0.ExecDis(dis);
		nb = ptrT->AddBranch(sit0);
		clTreeStrat *branch = ptrT->Branch(nb);
		BuildTreeAuto(branch, sit0, cnBet);
	}
}
//---------------------------------------------------------------------------
int FindStreet(clAtrTreeStrat *atr)
{
	return (atr->_sitA.NameTrade() == TRADE_RIVER);
}
//---------------------------------------------------------------------------
void	clStrategy::CreateLimit(int lev)
{
	_tree.ClearAllBranch();
	clSitHoldem sit;
	clStacks stack;
	stack._bb = 100; stack._ante = 0; stack[0] = 10000; stack[1] = 10000; stack[2] = 0;
	sit.Init(stack);
	_tree.AtrTree()->_sitA = sit;
//	tree->AtrTree()->_sitA._act._position = POS_BB;
	BuildTreeAuto(&_tree, sit, lev);
	int cnRoot = _tree.CnNode();
	_nameFileFlop = "Flop1344.bin";
	_nameFileTurn = "Turn2872.bin";
	_nameFileRiver = "River3662.bin";
	_nameFileFlop = "0";
	_nameFileTurn = "Turn88636.bin"; 
	_nameFileRiver = "River17372.bin";
	SetStartParam(true);
}
//---------------------------------------------------------------------------
void	clStrategy::WriteFile(int handle)
{
	_nameFileFlop.WriteFile(handle);
	_nameFileTurn.WriteFile(handle);
	_nameFileRiver.WriteFile(handle);
	byte cnPl = _tree.CnPlayer();
	_write(handle, &cnPl, 1);
	for (int i = 0; i < cnPl; i++)
		WriteVectFile(handle, _handsIn[i]._vect);
	_tree.WriteFile(handle);
}
//---------------------------------------------------------------------------
void	clStrategy::ReadFile(int handle)
{
	_nameFileFlop.ReadFile(handle);
	_nameFileTurn.ReadFile(handle);
	_nameFileRiver.ReadFile(handle);
	byte cnPl;
	_read(handle, &cnPl, 1);
	for (int i = 0; i < cnPl; i++)
		ReadVectFile(handle, _handsIn[i]._vect);
	_tree.ReadFile(handle);
	InitMemory();
	if (_nameFileFlop.Length()>0)
		SetIndexForNabor();
	else
	{
		SetStartParam(_tree.GetSit(), false);
		_tree.SetIndexForNabor(_indFlop, _indTurn, _indRiver);
	}
}
//---------------------------------------------------------------------------
void	BuildTreeAFAuto(clTreeStrat *ptrT, clSitHoldem &sit)
{
	clSitHoldem sit0;
	int ind = sit.Indicator();
	if (ind == -1)
	{
		ptrT->AtrTree()->_sitA = sit;
		return;
	}
	tpDis dis = HistCreateDis(ACT_ALL_IN, sit.PlayerMoney(ind));
	sit0 = sit;
	sit0.ExecDis(dis);
	int nb = ptrT->AddBranch(sit0);
	clTreeStrat *branch = ptrT->Branch(nb);
	BuildTreeAFAuto(branch, sit0);

	dis = sit.CreateDis(HistCreateDis(ACT_FOLD));
	sit0 = sit;
	sit0.ExecDis(dis);
	nb = ptrT->AddBranch(sit0);
	branch = ptrT->Branch(nb);
	BuildTreeAFAuto(branch, sit0);
}
//---------------------------------------------------------------------------
void	BuildTreeNLAuto(clTreeStrat *ptrT, clSitHoldem &sit, tpBuildTreeNLInfo &inf,int start,int maxLevel)
{
	if (maxLevel == 0) return;
	int ind = sit.Indicator();
	if (ind == -1)
	{
		ptrT->AtrTree()->_sitA = sit;
		if (sit.CNPlayerIsCard() == 1)
			return;
		if (sit.NameTrade() == TRADE_PREFLOP && sit.CNPlayerInTrade()>1)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELFLOP, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeNLAuto(ptrT->Branch(nb), sit, inf, start-1,maxLevel-1);
		}
		else if (sit.NameTrade() == TRADE_FLOP)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELTURN, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeNLAuto(ptrT->Branch(nb), sit, inf, start - 1, maxLevel - 1);
		}
		else if (sit.NameTrade() == TRADE_TURN)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELRIVER, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeNLAuto(ptrT->Branch(nb), sit, inf, start - 1, maxLevel - 1);
		}
		return;
	}

	clSitHoldem sit0;
	enNameTrade trade = sit.NameTrade();
	clBuildTreeNLInfoStreet &infT = (start>0) ? inf[0] : inf[((int)trade)];
	tpDis arrDis[MAX_CN_BRANCH];
	int cnDis = infT.GetInfo(sit, arrDis);
	for (int i = 0; i < cnDis;i++)
	{
		sit0 = sit;
		sit0.ExecDis(arrDis[i]);
		int nb = ptrT->AddBranch(sit0);

		if (trade == TRADE_PREFLOP && ind>POS_SB && sit.RoundTrade() == 1)
			if (arrDis[i]._act == ACT_FOLD)
			{
				ptrT = ptrT->Branch(nb);
				while (!sit0.EndedSit())
				{
					sit0.ExecDis(HistCreateDis(ACT_FOLD));
					ptrT->AddBranch(sit0);
					ptrT = ptrT->Branch(0);
				}
				continue;
			}

		BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, start - 1, maxLevel - 1);
	}

	/*

	tpDis disC = sit.CreateDisCallCheck();
	tpDis dis = sit.CreateDis(ACT_ALL_IN);
	if (!(dis == disC && infT._isCall) && infT._isAllIn)
	{
		sit0 = sit;
		sit0.ExecDis(dis);
		int nb = ptrT->AddBranch(sit0);
		BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, maxLevel - 1);
	}

	int maxBet = sit.MaxBet(), round = sit.RoundTrade();
	vector <double> *ptrV=NULL;
	if (round == 1)
	{
		bool fw = true;
		if (trade == TRADE_PREFLOP && maxBet > sit.BigBlind()) fw = false;
		if (trade > TRADE_PREFLOP && maxBet > 0) fw = false;
		ptrV = (fw) ? &infT._firstWord : &infT._firstRound;
	}
	else
		ptrV = (round==2) ? &infT._secondRound : &infT._replay;
	vector <double> &infoV = *ptrV;

	for (int i = 0; i < infoV.size(); i++)
	{
		int bet = 0;
		if (infoV[i]>0)
			bet = infoV[i]*sit.BigBlind();
		else
			bet = -(sit.PotLimitBank() + sit.MaxBet())*infoV[i];
		int val = sit.PlayerStack(ind) - sit.EffStackSitForPlayer(ind);
		//if (sit.IndicatorMoney() - val > infT._kBank * bet)
		if (sit.IndicatorMoney() > infT._kBank * bet)
		{
			dis = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, bet));
			if (!(dis == disC))
			{
				sit0 = sit;
				sit0.ExecDis(dis);
				int nb = ptrT->AddBranch(sit0);
				BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, maxLevel - 1);
			}
		}
		else if (!infT._isAllIn)
		{
			infT._isAllIn = true;
			dis = sit.CreateDis(ACT_ALL_IN);
			if (!(dis == disC))
			{
				sit0 = sit;
				sit0.ExecDis(dis);
				int nb = ptrT->AddBranch(sit0);
				BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, maxLevel - 1);
			}
		}
	}
	sit0 = sit;
	sit0.ExecDis(disC);
	int nb = ptrT->AddBranch(sit0);
	BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, maxLevel - 1);

	dis = sit.CreateDis(HistCreateDis(ACT_FOLD));
	if (!(dis == disC))
	{
		sit0 = sit;
		sit0.ExecDis(dis);
		int nb = ptrT->AddBranch(sit0);
		BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, maxLevel - 1);
	}*/
}
//---------------------------------------------------------------------------
void	clStrategy::CreateAF(clStacks stack)
{
	stack._bb = 100; stack._ante = 0;
	stack = 0;
	stack[0] = stack[1] = 300;
	_tree.ClearAllBranch();
	clSitHoldem sit;
	sit.Init(stack);
	_tree.AtrTree()->_sitA = sit;
	BuildTreeAFAuto(&_tree, sit);
	int cnRoot = _tree.CnNode();
	_nameFileFlop = "Flop3221.bin";
	_nameFileTurn = "Turn3K.bin";
	_nameFileRiver = "River3600.bin";
	_nameFileFlop = "0";
	_nameFileRiver = "0";
	SetStartParam(true);
}
//---------------------------------------------------------------------------
size_t	clStrategy::CreateNL(clStacks stack, bool mem)
{
	stack._bb = 100; stack._ante = 0;
	stack = 0;
	stack[0] = 2500;
	stack[1] = 2500;
	stack[2] = 600; 
	_tree.ClearAllBranch();
	//_tree.AtrTree()->_verTree = TREE_FOR_CALC;
	clSitHoldem sit;
	sit.Init(stack);
	_tree.AtrTree()->_sitA = sit;
	tpBuildTreeNLInfo inf;
	inf[0]._func = FuncLong;
	inf[TRADE_PREFLOP]._func = FuncDeckMiddle;
	inf[TRADE_FLOP]._func = FuncDeckMiddle;
	inf[TRADE_TURN]._func = FuncDeckShort;
	inf[TRADE_RIVER]._func = FuncDeckShort;

	int cnPl = stack.CnPlayer();
	BuildTreeNLAuto(&_tree, sit, inf, cnPl, 10000);
	int cnRoot = _tree.CnNode();
	_nameFileFlop = "0";
	//_nameFileFlop = "Flop3221.bin";
	_nameFileFlop = "Flop21K.bin";
	_nameFileFlop = "Flop100K.bin";
	//_nameFileFlop = "Flop415K.bin";

	_nameFileTurn = "Turn3K.bin";
	_nameFileTurn = "Turn10K.bin";
	//_nameFileTurn = "Turn30K.bin";

	_nameFileRiver = "River3600.bin";
	//_nameFileRiver = "0";
	//_nameFileRiver = "River17K.bin";
	
	return SetStartParam(mem);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::CalcEV(int cnCalc)
{
	for (int i = 0; i < _tree.CnPlayer(); i++)
		if (_handsIn[i]._vect.size() == ALL_CN_HAND)
		{
			CalcNesh(cnCalc);
			//CalcNeshVectHero(cnCalc,100);
			return;
		}
//	CalcEVStart(cnCalc);
	CalcEVStartVectHero(cnCalc,100);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::CalcEVStart(int cnCalc)
{
	bool bS = (_tree.AtrTree()->_sizeParam == clCalcParam0::SizeParam(_tree.CnBranch())); //bS = false;
	tpCardsGame cards; cards._cnPl = _tree.CnPlayer();
	clDeck deck;
	deck.New();
	for (int i = 0; i < cnCalc; i++)
	{
		deck.ShufleOldCol();
		cards.DealCards(deck);
		cards.CalcAllIndex();

		if (bS)
			for (int j = 0; j < cards._cnPl; j++)
				_tree.CalcEV(j, cards, 1, 1);
		else
			for (int j = 0; j < cards._cnPl; j++)
				_tree.CalcEVForCalc(j, cards, 1, 1);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::CalcEVStartVectHero(int cnCalc, int dim0)
{
	tpCardsGame cards; cards._cnPl = _tree.CnPlayer();

	vector <tpCardsGame> vectCards;
	vector <tpFloat0> vectkH;
	vectCards.resize(dim0);
	vectkH.resize(dim0);
	for (int i0 = 0; i0 < cnCalc; i0 += dim0)
	{
		for (int j = 0; j < cards._cnPl; j++)
		{
			tpBooleanDeck deck;
			int cnC = CN_CARD_COL;
			int opp = (j == 0) ? 1 : 0;
			cards._handsPl[opp][0] = deck.GiveRandomCard(cnC--);
			cards._handsPl[opp][1] = deck.GiveRandomCard(cnC--);

			for (int i = 0; i < 5; i++)
				cards._cardTable[i] = deck.GiveRandomCard(cnC--);
			cards.CalcAllIndex(opp);
			for (int k = 0; k < dim0; k++)
			{
				tpBooleanDeck deck0 = deck;
				cards._handsPl[j][0] = deck0.GiveRandomCard(cnC);
				cards._handsPl[j][1] = deck0.GiveRandomCard(cnC-1);
				{
					cards.CalcAllIndex(j);
					_tree.CalcEV(j, cards, 1, 1);
				}
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::ReCalcWeight()
{
	clHandsGroupEx *arrH[CN_PLAYER];
	for (int i = 0; i < _tree.GetSit().CnPlayer(); i++)
	{
		arrH[i] = new clHandsGroupEx;
		arrH[i]->FullPreFlop();
	}
	_tree.ReCalcWeight(arrH,false);
	for (int i = 0; i < _tree.GetSit().CnPlayer(); i++)
		delete arrH[i];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::CalcWeightStrat()
{
	clHandsGroupEx *arrH[CN_PLAYER];
	for (int i = 0; i < _tree.GetSit().CnPlayer(); i++)
	{
		arrH[i] = new clHandsGroupEx;
		arrH[i]->FullPreFlop();
	}
	_tree.ReCalcWeight(arrH,true);
	for (int i = 0; i < _tree.GetSit().CnPlayer(); i++)
		delete arrH[i];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#define	MIN_DIFF_PART		0.01
/*double	DistClasters(clClasterS &cls0, clClasterS &cls1,int pot)
{
	double res = 0;
	for (int i = 0; i < cls0._cn; i++)
	{
		double d0 = (cls0.NormEV(i) - cls1.NormEV(i)) / pot;
		res += d0*d0;
	}
	return sqrt(res);
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategy::GetDis(clSitHoldem &sit, tpArrResBranch8 *arr)
{
	clTreeStrat *node = _tree.FindSitNode(sit);
	if (node == NULL)
	{
		//node = _tree.FindSitNode(sit);
		arr->_cn = 0;
		return HistCreateDis(ACT_UNDEF);
	}
	if (node->CnBranch() == 1)
		return node->Branch(0)->GetDis();
	clCalcParam0 cls = node->FindClaster(sit);
	tpFloat0 w[MAX_CN_BRANCH], val = (tpFloat0)rand() / RAND_MAX;
	cls.GetWeightStrat(node->CnBranch(),w);
	if (arr != NULL)
	{
		arr->_cn = node->CnBranch();
		arr->_cnCalc = cls.CnCalc();
		for (int i = 0; i < node->CnBranch(); i++)
		{
			arr->_dis[i] = node->Branch(i)->GetDis();
			arr->_weight[i] = w[i];
		}
	}
	for (int i = 0; i < node->CnBranch(); i++)
		if (w[i] > val)
			return node->Branch(i)->GetDis();
		else
			val -= w[i];
	return node->Branch(node->CnBranch() - 1)->GetDis();
//	return HistCreateDis(ACT_FOLD);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	ReadVectFileToPtr(int handle, int *&ptr)
{
	unsigned dim;
	_read(handle, &dim, sizeof(unsigned));
	ptr=(int *)realloc(ptr,dim*sizeof(int));
	ReadBigBlock(handle, ptr, dim*sizeof(int));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		FindCnCls(size_t dim, int *vect)
{
	if (vect == NULL) return 0;
	int max = -1;
	for (int i = 0; i < dim; i++)
		if (vect[i]>max)
			max = vect[i];
	return max + 1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategy::ReadIndex()
{
	clAnsiString as;
	int handle;
	if (_nameFileFlop.Length() == 1)
		ClearIntPtr(_indFlop);
	else
	{
		as = (clAnsiString)PATH_CLASTERS + _nameFileFlop;
		handle = _open(as.c_str(), O_RDONLY | O_BINARY);
		if (handle == -1)
		{
			ErrMessage("Не открывается файл ", as);
			return false;
		}
		ReadVectFileToPtr(handle, _indFlop);
		_close(handle);
	}

	as = (clAnsiString)PATH_CLASTERS + _nameFileTurn;
	handle = _open(as.c_str(), O_RDONLY | O_BINARY);
	if (handle == -1)
	{
		ErrMessage("Не открывается файл ", as);
		return false;
	}
	ReadVectFileToPtr(handle, _indTurn);
	_close(handle);

	if (_nameFileRiver.Length() == 1)
		ClearIntPtr(_indRiver);
	else
	{
		as = (clAnsiString)PATH_CLASTERS + _nameFileRiver;
		handle = _open(as.c_str(), O_RDONLY | O_BINARY);
		if (handle == -1)
		{
			ErrMessage("Не открывается файл ", as);
			return false;
		}
		ReadVectFileToPtr(handle, _indRiver);
		_close(handle);
	}
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clStrategy::SetStartParam(bool mem)
{
	ReadIndex();

	int cnClsFlop = FindCnCls(CN_NABOR_HAND_FLOP,_indFlop);
	int cnClsTurn = FindCnCls(CN_NABOR_HAND_TURN,_indTurn);
	int cnClsRiver = FindCnCls(CN_NABOR_HAND_RIVER,_indRiver);
	
	return _tree.SetStartParam(cnClsFlop, cnClsTurn, cnClsRiver, _indFlop, _indTurn, _indRiver,mem);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategy::SetIndexForNabor()
{
	if (!ReadIndex()) return false;

	_tree.SetIndexForNabor(_indFlop, _indTurn, _indRiver);
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::CreateTreeNL(clSitHoldem &sit)
{
	enNameTrade trade = sit.NameTrade();
	if (trade < TRADE_TURN) return;
	int bank = sit.TotalBetAndBankAllPl();
	int stack = sit.EffStackAfterCall();
	int bb = sit.BigBlind();
	double k = (double)stack / bank;
	tpBuildTreeNLInfo inf;
	if (trade == TRADE_TURN)
	{
		inf[0]._func = FuncLong;
		inf[2]._func = FuncDeckMiddle;
		inf[3]._func = FuncDeckShort;
	}
	if (trade == TRADE_RIVER)
	{
		inf[0]._func = FuncLong;
		inf[3]._func = FuncDeckMiddle;
	}

	_tree.ClearAllBranch();
	_tree.GetSit() = sit;
	BuildTreeNLAuto(&_tree, sit, inf, 0, 10000);
	SetStartParam(sit, true);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::CreateTreeNL(clSitHoldem &sit0, clSitHoldem &sit1)
{
	enNameTrade trade = sit0.NameTrade();
	if (trade < TRADE_TURN) return;
	/*int bank = sit0.TotalBetAndBankAllPl();
	int stack = sit0.EffStackAfterCall();
	int bb = sit0.BigBlind();
	double k = (double)stack / bank;*/
	tpBuildTreeNLInfo inf;

	if (trade == TRADE_TURN)
	{
		inf[0]._func = FuncLong;
		inf[3]._func = FuncDeckMiddle;
		inf[4]._func = FuncDeckShort;
	}
	if (trade == TRADE_RIVER)
	{
		inf[0]._func = FuncLong;
		inf[4]._func = FuncDeckMiddle;
	}

	_tree.ClearAllBranch();
	_tree.GetSit() = sit0;
	BuildTreeNLAuto(&_tree, sit0, inf, &sit1, sit0.CurNbAct(),1);
	SetStartParam(sit0, true);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
struct tpThread1Atr
{
	int _nbH;
	int _cnCalc;
	bool _stop;
	clTreeStrat *_tree;
	vector <tpCardsGame> *_cards;
};

#define RAND_MAX_A		(RAND_MAX+1)
void	clStrategy::CalcNesh(int cnCalc)
{
	//size_t t0 = GetTickCount();
	tpCardsGame cards; cards._cnPl = _tree.CnPlayer();

	tpBooleanDeck deck;
	clSitHoldem &sit = _tree.GetSit();
	for (int i = 0; i < sit.CnCardTable(); i++)
	{
		cards._cardTable[i] = sit.CardsTable()[i];
		deck.Skip(cards._cardTable[i]);
	}

	tpHand *ptrH[CN_PLAYER],hand;
	for (int i = 0; i < sit.CnPlayer(); i++)
	{
		ptrH[i] = new tpHand[RAND_MAX_A];
		tpFloat0 val = 0;
		int nb = 0;
		for (int j = 0; j < ALL_CN_HAND; j++)
			if (_handsIn[i][j]>DOUBLE_0)
			{ 
				val += _handsIn[i][j];
				int nbM = val*(RAND_MAX_A)+0.5;
				hand.SetHandAll(j);
				for (; nb < nbM; nb++)
					ptrH[i][nb] = hand;
			}
		int a = 0;
		for (; nb < RAND_MAX_A; nb++)
			ptrH[i][nb] = hand;
	}
	//glCn0 += GetTickCount() - t0;

/*	vector <tpCardsGame> vectC[2];
	vectC[0].reserve(cnCalc);
	vectC[1].reserve(cnCalc);*/

	bool bS = (_tree.AtrTree()->_sizeParam == clCalcParam0::SizeParam(_tree.CnBranch())); //bS = false;
	for (int i0 = 0; i0 < cnCalc; )
	{
		int r = rand();
		cards._handsPl[0] = ptrH[0][r];
		r = rand();
		cards._handsPl[1] = ptrH[1][r];
		if (cards._handsPl[0].IncludeCardsHand(cards._handsPl[1]))
			continue;

		i0++;
		tpBooleanDeck deck0=deck;
		for (int i = 0; i < sit.CnPlayer(); i++)
		{
			deck0.Skip(cards._handsPl[i][0]);
			deck0.Skip(cards._handsPl[i][1]);
		}
		int cnC = CN_CARD_COL - 2 * sit.CnPlayer() - sit.CnCardTable();
		for (int i = sit.CnCardTable(); i < 5; i++)
			cards._cardTable[i] = deck0.GiveRandomCard(cnC--);
		cards.CalcAllIndex();

		if (bS)
			for (int j = 0; j < cards._cnPl; j++)
				_tree.CalcEV(j, cards, 1, 1);
		else
			for (int j = 0; j < cards._cnPl; j++)
			{
				_tree.CalcEVForCalc(j, cards, 1, 1);
				//vectC[j].push_back(cards);
			}
		//CalcEVForCalc(&_tree, j, cards, 1, 1);
	}
	for (int i = 0; i < sit.CnPlayer(); i++)
		delete ptrH[i];

/*	int handle = _open("c:\\home\\data\\cards1.dat", O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteVectFile(handle, vectC[0]);
		WriteVectFile(handle, vectC[1]);
		_close(handle);
	}*/
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::CalcNeshVectHero(int cnCalc,int dim0)
{
	tpCardsGame cards; cards._cnPl = _tree.CnPlayer();

	tpBooleanDeck deck;
	clSitHoldem &sit = _tree.GetSit();
	for (int i = 0; i < sit.CnCardTable(); i++)
	{
		cards._cardTable[i] = sit.CardsTable()[i];
		deck.Skip(cards._cardTable[i]);
	}
	tpHand *ptrH[CN_PLAYER], hand;
	for (int i = 0; i < sit.CnPlayer(); i++)
	{
		ptrH[i] = new tpHand[RAND_MAX_A];
		tpFloat0 val = 0;
		int nb = 0;
		for (int j = 0; j < ALL_CN_HAND; j++)
			if (_handsIn[i][j] > DOUBLE_0)
			{
				val += _handsIn[i][j];
				int nbM = val*(RAND_MAX_A);
				hand.SetHandAll(j);
				for (; nb < nbM; nb++)
					ptrH[i][nb] = hand;
			}
		for (; nb < RAND_MAX_A; nb++)
			ptrH[i][nb] = hand;
	}

/*	vector <tpCardsGame> vectC[2];
	vectC[0].reserve(cnCalc+dim0);
	vectC[1].reserve(cnCalc+dim0);*/

	int dim = 0;
	vector <tpCardsGame> vectCards;
	vector <tpFloat0> vectkH;
	vectCards.resize(dim0);
	vectkH.resize(dim0);
	for (int i0 = 0; i0 < cnCalc; i0 += dim)
	{
		for (int j = 0; j < cards._cnPl; j++)
		{
			int opp = (j == 0) ? 1 : 0;
			int r = rand(); 
			cards._handsPl[opp] = ptrH[opp][r];
			tpBooleanDeck deck0 = deck;
			deck0.Skip(cards._handsPl[opp][0]);
			deck0.Skip(cards._handsPl[opp][1]);
			int cnC = CN_CARD_COL - 2 - sit.CnCardTable();
			for (int i = sit.CnCardTable(); i < 5; i++)
				cards._cardTable[i] = deck0.GiveRandomCard(cnC--);
			cards.CalcAllIndex(opp);
			dim = 0;
			for (int k = 0; k < dim0; k++)
			{
				r = rand(); 
				cards._handsPl[j] = ptrH[j][r];
				if (deck0[cards._handsPl[j][0]] && deck0[cards._handsPl[j][1]])
				{
					cards.CalcAllIndex(j);
					vectCards[dim]=cards;
					_tree.CalcEV(j, cards, 1, 1);
					vectkH[dim++] = 1;
					//vectC[j].push_back(cards);
				}
			}
			//_tree.CalcEV(j, dim,&vectCards[0], vectkH, 1);
		}
	}
	for (int i = 0; i < sit.CnPlayer(); i++)
		delete ptrH[i];

/*	int handle = _open("c:\\home\\data\\cardsVect.dat", O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteVectFile(handle, vectC[0]);
		WriteVectFile(handle, vectC[1]);
		_close(handle);
	}*/
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*#define CN_THREAD		8

unsigned long _stdcall CalcThreadEV(void *ptr)
{
tpThread1Atr *atr = (tpThread1Atr *)ptr;
vector <tpCardsGame> &cards=*atr->_cards;
clDeck deck;
deck.New();
int st = 0, en = atr->_cnCalc / 2;
if (atr->_nbH == 1)
{
st = atr->_cnCalc / 2;
en = atr->_cnCalc;
}
srand(time(NULL) + atr->_nbH);
for (int i = st; i < en; i++)
{
deck.ShufleOldCol();
cards[i]._cnPl = 2;
cards[i].DealCards(deck);
cards[i].CalcAllIndex();
}
for (int i = 0; i < atr->_cnCalc; i++)
atr->_tree->CalcEV(atr->_nbH, cards[i], 1, 1);
atr->_stop = true;
return 0;
}

void	clStrategy::CalcEVThread(int cnCalc)
{
tpThread1Atr atr[CN_THREAD];
int cnT = 2;
vector <tpCardsGame> cards;
cards.resize(cnCalc);
for (int i = 0; i < cnT; i++)
{
atr[i]._cards = &cards;
atr[i]._cnCalc = cnCalc;
atr[i]._nbH = i;
atr[i]._stop = false;
atr[i]._tree=&_tree;
CreateThread(NULL, 0, &CalcThreadEV, &atr[i], 0, NULL);
}
while (true)
{
int j = 0;
for (; j<cnT; j++)
if (!atr[j]._stop)
break;
if (j == cnT)
break;
Sleep(100);
}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::SetStartParam(clSitHoldem &sit, bool setPar)
{
	enNameTrade trade = sit.NameTrade();
	if (trade < TRADE_TURN) return;
	clDeck deck;deck.New();
	tpCard cards[5];
	for (int i = 0; i < sit.CnCardTable(); i++)
	{
		cards[i] = sit.CardsTable()[i];
		deck.Skip(cards[i]);
	}
	int cnClsTurn = 0, cnClsRiver = 0; 
	if (trade == TRADE_TURN)
	{
		memset(_indTurn, -1,CN_NABOR_HAND_TURN*sizeof(int));
		for (int i = 0; i < deck.CnCards(); i++)
			for (int j = i+1; j < deck.CnCards(); j++)
			{
				tpHand hand; hand.SetHand(deck[j], deck[i]);
				int nbT = GetTurnHandIndex(cards, hand);
				if (_indTurn[nbT] == -1)
					_indTurn[nbT] = cnClsTurn++;
			}

		memset(_indRiver, -1, CN_NABOR_HAND_RIVER*sizeof(int));
		for (int k = 0; k < deck.CnCards(); k++)
		{
			cards[4] = deck[k];
			for (int i = 0; i < deck.CnCards(); i++)
				if (i != k)
					for (int j = i + 1; j < deck.CnCards(); j++)
						if (j != k)
						{
							int indTurn;
							tpHand hand; hand.SetHand(deck[j], deck[i]);
							int nbT = GetTurnAndRiverHandIndex(cards, hand, indTurn);
							if (_indRiver[nbT] == -1)
								_indRiver[nbT] = cnClsRiver++;
						}
		}
	}
	else if (trade == TRADE_RIVER)
	{
		memset(_indRiver, -1, CN_NABOR_HAND_RIVER*sizeof(int));
		for (int i = 0; i < deck.CnCards(); i++)
			for (int j = i + 1; j < deck.CnCards(); j++)
			{
				int indTurn;
				tpHand hand; hand.SetHand(deck[j], deck[i]);
				int nbT = GetTurnAndRiverHandIndex(cards, hand, indTurn);
				if (_indRiver[nbT] == -1)
					_indRiver[nbT] = cnClsRiver++;
			}
	}

	if (setPar)
		_tree.SetStartParam(1, cnClsTurn, cnClsRiver, _indFlop, _indTurn, _indRiver,true);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::InitMemory()
{
	_indFlop = (int *)realloc(_indFlop, CN_NABOR_HAND_FLOP*sizeof(int));
	_indTurn = (int *)realloc(_indTurn, CN_NABOR_HAND_TURN*sizeof(int));
	_indRiver = (int *)realloc(_indRiver, CN_NABOR_HAND_RIVER*sizeof(int));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	BuildTreeNLAuto(clTreeStrat *ptrT, clSitHoldem &sit, tpBuildTreeNLInfo &inf, clSitHoldem *sitH, int nbA, int start)
{
	if (sitH != NULL && sitH->CurNbAct() <= nbA) sitH = NULL;
	int ind = sit.Indicator();
	enNameTrade trade = sit.NameTrade();
	if (ind == -1)
	{
		ptrT->AtrTree()->_sitA = sit;
		if (sit.CNPlayerIsCard() == 1)
			return;
		if (trade == TRADE_PREFLOP && sit.CNPlayerInTrade()>1)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELFLOP, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeNLAuto(ptrT->Branch(nb), sit, inf, sitH,nbA+1,0);
		}
		else if (trade == TRADE_FLOP)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELTURN, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeNLAuto(ptrT->Branch(nb), sit, inf, sitH, nbA + 1,0);
		}
		else if (trade == TRADE_TURN)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELRIVER, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeNLAuto(ptrT->Branch(nb), sit, inf, sitH, nbA + 1,0);
		}
		return;
	}
	clSitHoldem sit0;
	clBuildTreeNLInfoStreet &infT = (start>0) ? inf[0] : inf[((int)trade)];
	tpDis arrDis[MAX_CN_BRANCH];
	tpDis disH = (sitH == NULL) ? HistCreateDis(ACT_UNDEF) : sitH->GetAct(nbA)->_dis;
	int cnDis = (sitH == NULL) ? infT.GetInfo(sit, arrDis) : infT.GetInfo(sit, arrDis, disH);

	for (int i = 0; i < cnDis; i++)
	{
		sit0 = sit;
		sit0.ExecDis(arrDis[i]);
		int nb = ptrT->AddBranch(sit0);
		if (disH == arrDis[i])
		{
			disH._act = ACT_UNDEF;
			BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, sitH, nbA + 1,start-1);
		}
		else
			BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf,false,1000);
	}


	/*
	tpDis disH = (sitH == NULL) ? HistCreateDis(ACT_UNDEF) : sitH->GetAct(nbA)->_dis;
	tpDis disC = sit.CreateDisCallCheck();
	tpDis dis = sit.CreateDis(ACT_ALL_IN);
	if (!(dis == disC) && (infT._isAllIn || disH._act == ACT_ALL_IN))
	{
		sit0 = sit;
		sit0.ExecDis(dis);
		int nb = ptrT->AddBranch(sit0);
		if (disH._act == ACT_ALL_IN)
		{
			disH._act = ACT_UNDEF;
			BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, sitH,nbA+1);
		}
		else
			BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, NULL,0);
	}

	int maxBet = sit.MaxBet(), round = sit.RoundTrade();
	vector <double> *ptrV = NULL;
	if (round == 1)
	{
		bool fw = true;
		if (trade == TRADE_PREFLOP && maxBet > sit.BigBlind()) fw = false;
		if (trade > TRADE_PREFLOP && maxBet > 0) fw = false;
		ptrV = (fw) ? &infT._firstWord : &infT._firstRound;
	}
	else
		ptrV = (round == 2) ? &infT._secondRound : &infT._replay;
	vector <double> &infoV = *ptrV;

	if (disH._act != ACT_UNDEF && disH._act > ACT_CALL)
	{
		tpDis disRaise[100];
		double min = 1.2;
		int nbMin = -1,cnD=0;
		for (int i = 0; i < infoV.size(); i++)
		{
			int bet = 0;
			if (infoV[i]>0)
				bet = infoV[i] * sit.BigBlind();
			else
				bet = sit.MaxBet() - sit.PotLimitBank()*infoV[i];
			if (sit.IndicatorMoney() > infT._kBank * bet)
			{
				dis = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, bet));
				if (!(dis == disC))
				{
					double d0 = fabs((dis._money - disH._money)*1. / sit.BigBlind());
					if (d0 < min)
					{
						min = d0;
						nbMin = cnD;
					}
					disRaise[cnD++] = dis;
				}
			}
		}
		if (nbMin == -1)
		{
			nbMin = cnD;
			disRaise[cnD++] = disH;
		}
		else
			disRaise[nbMin] = disH;
		for (int i = 0; i < cnD; i++)
		{
			sit0 = sit;
			sit0.ExecDis(disRaise[i]);
			int nb = ptrT->AddBranch(sit0);
			if (i == nbMin)
			{
				if (sitH->CurAct()>nbA+1)
					BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, sitH, nbA+1);
				else
					BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, NULL, 0);
			}
			else
				BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, NULL, 0);
		}
	}
	else
		for (int i = 0; i < infoV.size(); i++)
		{
			int bet = 0;
			if (infoV[i]>0)
				bet = infoV[i] * sit.BigBlind();
			else
				bet = sit.MaxBet() - sit.PotLimitBank()*infoV[i];
			if (sit.IndicatorMoney() > infT._kBank * bet)
			{
				dis = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, bet));
				if (!(dis == disC))
				{
					sit0 = sit;
					sit0.ExecDis(dis);
					int nb = ptrT->AddBranch(sit0);
					BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, NULL,0);
				}
			}
		}
	sit0 = sit;
	sit0.ExecDis(disC);
	int nb = ptrT->AddBranch(sit0);
	if (disC==disH)
		BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, sitH,nbA+1);
	else
		BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, NULL,0);

	dis = sit.CreateDis(HistCreateDis(ACT_FOLD));
	if (!(dis == disC))
	{
		sit0 = sit;
		sit0.ExecDis(dis);
		int nb = ptrT->AddBranch(sit0);
		if (disC== disH)
			BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, sitH, nbA + 1);
		else
			BuildTreeNLAuto(ptrT->Branch(nb), sit0, inf, NULL, 0);
	}*/
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP	CalcEVForCalc(clTreeStrat *node0, int nbH, tpCardsGame &cards, tpFloat0 kH0, tpFloat0 kOpp0)
{
	clTreeStrat *node=node0,*nodeUr[100];
	tpFloat0 kH=kH0, kOpp=kOpp0,kHUr[100], kOppUr[100];
	clDoubleCP result,resUr[100];
	int level = 0, cll[100];
	int kC = 0,kUr[100];
	int indUr[100];
	//	clCalcParam cls,clsUr[100];
	clCalcParam0 cls0;
	tpFloat0 *w = NULL, *wUr[100];
	tpFloat0 *ev,evUr[100][MAX_CN_BRANCH];

	LabelStart:
	node->AtrTree()->_statCn += kH*kOpp;
	if (node->EndedNode())
	{
		result = node->Result(cards)*kOpp;

		if (level == 0) return result; // begin return 
		level--;
		node = nodeUr[level];
		kH = kHUr[level];
		kOpp = kOppUr[level];
		kC = kUr[level];
		if (cll[level] == 1)
			goto Label1;
		else if (cll[level] == 2)
		{
			w = wUr[level];
			ev = evUr[level];
			goto Label2;
		}
		else if (cll[level] == 3)
		{
			w = wUr[level];
			goto Label3;
		}							// end return

	}
	indUr[level] = node->Indicator();
	if (indUr[level] == -1 || node->CnBranch() == 1)
	{
		nodeUr[level] = node;
		kHUr[level] = kH;
		kOppUr[level] = kOpp;
		cll[level] = 1;
		level++;
		node = node->Branch(0);
		goto LabelStart;
		//		return Branch(0)->CalcEVForCalc(nbH, cards, kH, kOpp);
	Label1:

		if (level == 0) return result; // begin return 
		level--;
		node = nodeUr[level];
		kH = kHUr[level];
		kOpp = kOppUr[level];
		kC = kUr[level];
		if (cll[level] == 1)
			goto Label1;
		else if (cll[level] == 2)
		{
			w = wUr[level];
			ev = evUr[level];
			goto Label2;
		}
		else if (cll[level] == 3)
		{
			w = wUr[level];
			goto Label3;
		}							// end return
	}

	int nbCls0 = node->GetNbCls(cards, indUr[level]);
	cls0 = node->AtrTree()->CalcParam(nbCls0);
	w = cls0.GetWeightForCalc(node->CnBranch());
	resUr[level] = 0;
	if (indUr[level] == nbH)
	{
		kC = 0;

		nodeUr[level] = node;
		kHUr[level] = kH;
		kOppUr[level] = kOpp;
		wUr[level] = w;
		cll[level] = 2;
	LabelCicle1:
		kUr[level] = kC;
		level++;
		node = node->Branch(kC);
		kH *= w[kC];
		//		clDoubleCP f0 = Branch(k)->CalcEVForCalc(nbH, cards, kH*w[k], kOpp);
		goto LabelStart;
	Label2:
		ev[kC] = result[indUr[level]];
		resUr[level] += result*w[kC];
		if (++kC < node->CnBranch())
			goto LabelCicle1;

		//		cls.AddResEVForCalc(CnBranch(), res[ind], w, ev, kH, kOpp);
		{
			int nbCls = node->GetNbCls(cards, indUr[level]);
			clCalcParam0 cls = node->AtrTree()->CalcParam(nbCls);
			clDoubleCP res = resUr[level];
			int ind = indUr[level];
			int cnB = node->CnBranch();
			tpFloat0 *evW = cls.BeginEV();
			tpFloat0 *part = cls.BeginPart(cnB);
			cls.CnCalc()++;
			cls.WeightCalc() += kOpp;
			cls.AverRes() += res[ind];
			for (int i = 0; i < cnB; i++)
			{
				evW[i] += ev[i] - res[ind];
				part[i] += kH * w[i];
			}

			tpFloat0 sum = 0;
			tpFloat0 *ptrW = cls.GetWeightForCalc(cnB);
			for (int i = 0; i < cnB; i++)
				if (evW[i]>(float)DOUBLE_0)
					sum += evW[i];
			if (sum > (float)DOUBLE_0)
				for (int i = 0; i < cnB; i++)
					ptrW[i] = (evW[i] >(float)DOUBLE_0) ? evW[i] / sum : 0;
			else
				for (int i = 0; i < cnB; i++)
					ptrW[i] = 1. / cnB;
		}

	}
	else
	{
		kC = 0;

		nodeUr[level] = node;
		kHUr[level] = kH;
		kOppUr[level] = kOpp;
		wUr[level] = w;
		cll[level] = 3;
	LabelCicle2:
		if(w[kC]>DOUBLE_0)
		{
			node = node->Branch(kC);
			kUr[level] = kC;
			level++;
			kOpp *= w[kC];
			//clDoubleCP f0 = Branch(k)->CalcEVForCalc(nbH, cards, kH, kOpp*w[k]);
			goto LabelStart;
		Label3:
			resUr[level] += result;
		}
		if (++kC < node->CnBranch())
			goto LabelCicle2;
	}

	result = resUr[level];
	if (level == 0) return result; // begin return 
	level--;
	node = nodeUr[level];
	kH = kHUr[level];
	kOpp = kOppUr[level];
	kC = kUr[level];
	if (cll[level] == 1)
		goto Label1;
	else if (cll[level] == 2)
	{
		w = wUr[level];
		ev = evUr[level];
		goto Label2;
	}
	else if (cll[level] == 3)
	{
		w = wUr[level];
		goto Label3;
	}							// end return
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	clBuildTreeNLInfoStreet::GetInfo(clSitHoldem &sit, tpDis *arrDis)
{
	if (_func == NULL)
	{
		int res = 0,ind=sit.Indicator();
		enNameTrade trade = sit.NameTrade();
		tpDis disC = sit.CreateDisCallCheck();
		tpDis dis = sit.CreateDis(ACT_ALL_IN);
		if (!(dis == disC && _isCall) && _isAllIn)
			arrDis[res++] = dis;

		int maxBet = sit.MaxBet(), round = sit.RoundTrade();
		vector <double> *ptrV = NULL;
		if (round == 1)
		{
			bool fw = true;
			if (trade == TRADE_PREFLOP && maxBet > sit.BigBlind()) fw = false;
			if (trade > TRADE_PREFLOP && maxBet > 0) fw = false;
			ptrV = (fw) ? &_firstWord : &_firstRound;
		}
		else
			ptrV = (round == 2) ? &_secondRound : &_replay;
		vector <double> &infoV = *ptrV;

		bool isA = _isAllIn;
		for (int i = 0; i < infoV.size(); i++)
		{
			int bet = 0;
			if (infoV[i]>0)
				bet = infoV[i] * sit.BigBlind();
			else
				bet = -(sit.PotLimitBank() + sit.MaxBet())*infoV[i];
			int val = sit.PlayerStack(ind) - sit.EffStackSitForPlayer(ind);
			//if (sit.IndicatorMoney() - val > infT._kBank * bet)
			if (sit.IndicatorMoney() > _kBank * bet)
			{
				dis = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, bet));
				if (!(dis == disC))
					arrDis[res++] = dis;
			}
			else if (!isA)
			{
				isA = true;
				dis = sit.CreateDis(ACT_ALL_IN);
				if (!(dis == disC))
					arrDis[res++] = dis;
			}
		}

		arrDis[res++] = disC;
		dis = sit.CreateDis(HistCreateDis(ACT_FOLD));
		if (!(dis == disC))
			arrDis[res++] = disC;
		return res;
	}
	else
		return _func(sit, arrDis, NULL);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	clBuildTreeNLInfoStreet::GetInfo(clSitHoldem &sit, tpDis *arrDis, tpDis disH)
{
	if (_func == NULL)
	{
		int res = 0, ind = sit.Indicator();
		enNameTrade trade = sit.NameTrade();
		tpDis disC = sit.CreateDisCallCheck();
		tpDis dis = sit.CreateDis(ACT_ALL_IN);
		if (!(dis == disC && _isCall) && (_isAllIn || disH._act == ACT_ALL_IN))
		{
			arrDis[res++] = dis;
			if (disH._act == ACT_ALL_IN)
				disH._act = ACT_UNDEF;
		}

		int maxBet = sit.MaxBet(), round = sit.RoundTrade();
		vector <double> *ptrV = NULL;
		if (round == 1)
		{
			bool fw = true;
			if (trade == TRADE_PREFLOP && maxBet > sit.BigBlind()) fw = false;
			if (trade > TRADE_PREFLOP && maxBet > 0) fw = false;
			ptrV = (fw) ? &_firstWord : &_firstRound;
		}
		else
			ptrV = (round == 2) ? &_secondRound : &_replay;
		vector <double> &infoV = *ptrV;

		bool isA = _isAllIn;
		for (int i = 0; i < infoV.size(); i++)
		{
			int bet = 0;
			if (infoV[i]>0)
				bet = infoV[i] * sit.BigBlind();
			else
				bet = -(sit.PotLimitBank() + sit.MaxBet())*infoV[i];
			int val = sit.PlayerStack(ind) - sit.EffStackSitForPlayer(ind);
			//if (sit.IndicatorMoney() - val > infT._kBank * bet)
			if (sit.IndicatorMoney() > _kBank * bet)
			{
				dis = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, bet));
				if (!(dis == disC))
					arrDis[res++] = dis;
			}
			else if (!isA)
			{
				isA = true;
				dis = sit.CreateDis(ACT_ALL_IN);
				if (!(dis == disC))
					arrDis[res++] = dis;
			}
		}
		if (disH._act != ACT_UNDEF && disH._act > ACT_CALL)
		{
			double min = 1.2;
			int nbMin = -1;
			for (int i = 0; i < res; i++)
			{
				double d0 = fabs((arrDis[i]._money - disH._money)*1. / sit.BigBlind());
				if (d0 < min)
				{
					d0 = min;
					nbMin = i;
				}
			}
			if (min < 0.2)
				arrDis[nbMin] = disH;
			else
				arrDis[res++] = disH;
		}

		arrDis[res++] = disC;
		dis = sit.CreateDis(HistCreateDis(ACT_FOLD));
		if (!(dis == disC))
			arrDis[res++] = disC;
		return res;
	}
	else
		return _func(sit, arrDis, &disH);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	FuncLong(clSitHoldem &sit, tpDis *arrDis, tpDis *disH)
{
	int cn = 0;
	enNameTrade trade = sit.NameTrade();
	int bb = sit.BigBlind();
	int maxBet = sit.MaxBet();
	if (trade == TRADE_PREFLOP && maxBet == bb)
	{
		tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
		tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
		int stack = sit.EffStackSitForPlayer(sit.Indicator()) / bb;
		if (stack > 20)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 5 * bb));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 3 * bb));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2.5 * bb));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2 * bb));
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else if (stack > 10)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 3 * bb));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2.5 * bb));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2 * bb));
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else if (stack > 6)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2.5 * bb));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2 * bb));
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else if (stack > 3)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2 * bb));
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else
		{
			tpDis disA = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			arrDis[cn++] = disA;
			if (!(disA == disC))
				arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
	}
	else
	{
		int stack = sit.EffStackAfterCall(sit.Indicator());
		int bank = sit.PotLimitBank();
		tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
		tpDis minR = sit.MinRaise();
		int minBet = minR._money - disC._money,betC=disC._money;
		double kBet = (minBet == 0) ? 1000. : (double)stack / minBet;
		if (stack == 0) kBet = 0;
		if (kBet<3)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			if (!(arrDis[0] == disC))
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_CALL));
			tpDis dis = sit.CreateDis(HistCreateDis(ACT_FOLD));
			if (!(dis == disC))
				arrDis[cn++] = dis;
		}
		else if (kBet<6)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			if (!(arrDis[0] == minR))
				arrDis[cn++] = minR;
			arrDis[cn++] = disC;
			tpDis dis = sit.CreateDis(HistCreateDis(ACT_FOLD));
			if (!(dis == disC))
				arrDis[cn++] = dis;
		}
		else
		{
			double k = (double)stack / bank;
			if (k<0.8)
			{
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
				tpDis disR = sit.CreateDis(HistCreateDis(ACT_RAISE, (arrDis[0]._money + minR._money) / 2));
				if (!(arrDis[0]==disR))
					arrDis[cn++] = disR;
				if (!(minR == disR))
					arrDis[cn++] = minR;
				arrDis[cn++] = disC;
				tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
				if (!(disF == disC))
					arrDis[cn++] = disF;
			}
			else if (k<1.5)
			{
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
				tpDis disR = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank*0.5));
				if (!(arrDis[0] == disR))
					arrDis[cn++] = disR;
				if (!(minR == disR))
					arrDis[cn++] = minR;
				arrDis[cn++] = disC;
				tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
				if (!(disF == disC))
					arrDis[cn++] = disF;
			}
			else if (k<2.25)
			{
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank));
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank*0.5));
				if (!(minR == arrDis[cn-1]))
					arrDis[cn++] = minR;
				arrDis[cn++] = disC;
				tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
				if (!(disF == disC))
					arrDis[cn++] = disF;
			}
			else
			{
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank*1.5));
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank));
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank*0.5));
				if (!(minR == arrDis[cn - 1]))
					arrDis[cn++] = minR;
				arrDis[cn++] = disC;
				tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
				if (!(disF == disC))
					arrDis[cn++] = disF;
			}
		}
	}
	if (disH != NULL)
		if (disH->_act > ACT_CALL)
		{
			tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
			tpDis disA = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			cn = RecalcDis(cn, arrDis, *disH, disA, disC);
		}
	return cn;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	FuncDeckMiddle(clSitHoldem &sit, tpDis *arrDis, tpDis *disH)
{
	int cn=0,stack = sit.EffStackAfterCall(sit.Indicator());
	int bank = sit.PotLimitBank();
	tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
	tpDis minR = sit.MinRaise();
	int minBet = minR._money - disC._money, betC = disC._money;
	double kBet = (minBet == 0) ? 1000. : (double)stack / minBet;
	if (kBet<6)
	{
		arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
		if (!(arrDis[0] == disC))
			arrDis[cn++] = disC;
		tpDis dis = sit.CreateDis(HistCreateDis(ACT_FOLD));
		if (!(dis == disC))
			arrDis[cn++] = dis;
	}
	else
	{
		tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
		double k = (double)stack / bank;
		if (k<0.8)
		{
			tpDis disA = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			if (disC._money*1.3<disA._money)
				arrDis[cn++] = disA;
			tpDis disR = sit.CreateDis(HistCreateDis(ACT_RAISE, (arrDis[0]._money + disC._money) / 2));
			if (disC._money*1.3<disR._money)
				arrDis[cn++] = disR;
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else if (k<1.6)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			tpDis disR = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank*0.5));
			arrDis[cn++] = disR;
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else if (k<3.2)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank*0.5));
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank*1.5));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank*0.5));
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
	}
	if (disH != NULL)
		if (disH->_act > ACT_CALL)
		{
			tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
			tpDis disA = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			cn = RecalcDis(cn, arrDis, *disH, disA, disC);
		}
	return cn;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	RecalcDis(int cn, tpDis *arrDis, tpDis disH, tpDis disA, tpDis disC)
{
	int add = -1;
	if (disH._act == ACT_ALL_IN)
	{
		if (arrDis[0]._act != ACT_ALL_IN && !(disC == disA))
			add = 0;
	}
	else
	{
		int nbMin = -1,min=0;
		for (int i = 0; i < cn; i++)
			if (arrDis[i]._act > ACT_CALL)
			{
				int val = abs(arrDis[i]._money - disH._money);
				if (nbMin == -1)
				{
					nbMin = i;
					min = val;
				}
				else if (val < min)
				{
					nbMin = i;
					min = val;
				}
			}
		add = 1;
		if (nbMin != -1)
			if ((double)min / disH._money < 0.2)
			{
				arrDis[nbMin] = disH;
				add = -1;
			}
	}
	if (add != -1)
	{
		for (int i = cn; i > add; i--)
			arrDis[i] = arrDis[i - 1];
		arrDis[add] = disH;
		cn++;
	}
	return cn;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	FuncDeckShort(clSitHoldem &sit, tpDis *arrDis, tpDis *disH)
{
	int cn = 0;
	tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
	tpDis disA = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
	int bank = sit.PotLimitBank();
	int maxBet = sit.MaxBet() - sit.PlayerBank(sit.Indicator());
	tpDis disR = sit.CreateDis(HistCreateDis(ACT_RAISE, maxBet + bank));
	if (disR._money*1.8 < disA._money)
		arrDis[cn++] = disR;
	else if (!(disC==disA))
		arrDis[cn++] = disA;
	arrDis[cn++] = disC;
	tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
	if (!(disF == disC))
		arrDis[cn++] = disF;
	if (disH != NULL)
		if (disH->_act > ACT_CALL)
			cn = RecalcDis(cn, arrDis, *disH, disA, disC);
	return cn;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	FuncMiddle(clSitHoldem &sit, tpDis *arrDis, tpDis *disH)
{
	int cn = 0;
	enNameTrade trade = sit.NameTrade();
	int bb = sit.BigBlind();
	int maxBet = sit.MaxBet();
	if (trade == TRADE_PREFLOP && maxBet == bb)
	{
		tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
		tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
		int stack = sit.EffStackSitForPlayer(sit.Indicator()) / bb;
		if (stack > 20)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 5 * bb));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 3 * bb));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2 * bb));
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else if (stack > 10)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 3 * bb));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2 * bb));
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else if (stack > 6)
		{
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2 * bb));
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else
		{
			tpDis disA = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			arrDis[cn++] = disA;
			if (!(disA==disC))
				arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		if (disH != NULL)
			if (disH->_act > ACT_CALL)
			{
				tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
				tpDis disA = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
				cn = RecalcDis(cn, arrDis, *disH, disA, disC);
			}
		return cn;
	}
	else return FuncDeckMiddle(sit, arrDis,disH);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
