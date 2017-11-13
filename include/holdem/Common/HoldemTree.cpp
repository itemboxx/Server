//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <math.h>
#include <time.h>

#include "HoldemTree.h"
#include "..\..\Util\TemplateFileRec.h"
#include "..\..\Util\Sort.h"
#include "clHandsCompare.h"

enNameTrade glTradeNodeCalc = TRADE_UNDEF;

void	CalcNeshPreFlopPushFoldMultiThread(clHoldemTree *tree, int cnCalc);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::ClearData(int cnB)
{
	CnCalc() = 0;
	WeightCalc() = 0;
	AverRes() = 0;
	tpFloat *ev = BeginEV();
	cnB *= 2;
	for (int i = 0; i < cnB; i++)
		ev[i] = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::ClearEV(int cnB)
{
	CnCalc() = 0;
	WeightCalc() = 0;
	AverRes() = 0;
	tpFloat *ev = BeginEV();
	for (int i = 0; i < cnB; i++)
		ev[i] = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::MoveTestEV(int cnB)
{
	tpFloat *ev = BeginEV();
	tpFloat *evT = GetWeightForCalc(cnB);
	for (int i = 0; i < cnB; i++)
		ev[i] = evT[i];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::MultEV(int cnB, tpFloat k)
{
	CnCalc() *= k;
	WeightCalc() *= k;
	AverRes() *= k;
	tpFloat *ev = BeginEV();
	for (int i = 0; i < cnB; i++)
		ev[i] *= k;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::AddCalcParam(clCalcParam param, int cnB, double mult)
{
	tpFloat *partP = param.BeginPart(cnB), sum = 0;
	for (int i = 0; i < cnB; i++)
		sum += partP[i];
	if (sum > DOUBLE_0)
		mult /= sum;
	else return;
	CnCalc() += param.CnCalc()*mult;
	WeightCalc() += param.WeightCalc()*mult;
	AverRes() += param.AverRes()*mult;
	tpFloat *ev = BeginEV();
	tpFloat *evP = param.BeginEV();
	tpFloat *part = BeginPart(cnB);
	for (int i = 0; i < cnB; i++)
	{
		ev[i] += evP[i] * mult;
		part[i] += partP[i] * mult;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::ClearEVAndSetWeight(int cnB)
{
	CnCalc() = 0;
	WeightCalc() = 0;
	AverRes() = 0;
	if (cnB == 0) return;
	tpFloat *ev = BeginEV();
	for (int i = 0; i < 2 * cnB; i++)
		ev[i] = 0;
	tpFloat val = 1.f / cnB;
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
void	clCalcParam::GetWeight(int cnB, tpFloat *w)
{
#ifdef TEST_GROUPvsGROUP
	GetWeightStrat(cnB, w);
	return;
#endif
	tpFloat *ev = BeginEV();
	tpFloat sum = 0;

	if (cnB == 1)
	{
		w[0] = 1;
	}
	else if (cnB == 2)
	{
		if (ev[0] > (tpFloat)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (tpFloat)DOUBLE_0)
			sum += ev[1];
		if (sum > (tpFloat)DOUBLE_0)
		{
			w[0] = (ev[0] > (tpFloat)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (tpFloat)DOUBLE_0) ? ev[1] / sum : 0;
		}
		else
			w[0] = w[1] = 0.5f;
	}
	else if (cnB == 3)
	{
		if (ev[0] > (tpFloat)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (tpFloat)DOUBLE_0)
			sum += ev[1];
		if (ev[2] > (tpFloat)DOUBLE_0)
			sum += ev[2];
		if (sum > (tpFloat)DOUBLE_0)
		{
			w[0] = (ev[0] > (tpFloat)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (tpFloat)DOUBLE_0) ? ev[1] / sum : 0;
			w[2] = (ev[2] > (tpFloat)DOUBLE_0) ? ev[2] / sum : 0;
		}
		else
			w[0] = w[1] = w[2] = 1.f / 3;
	}
	else if (cnB == 4)
	{
		if (ev[0] > (tpFloat)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (tpFloat)DOUBLE_0)
			sum += ev[1];
		if (ev[2] > (tpFloat)DOUBLE_0)
			sum += ev[2];
		if (ev[3] > (tpFloat)DOUBLE_0)
			sum += ev[3];
		if (sum > (tpFloat)DOUBLE_0)
		{
			w[0] = (ev[0] > (tpFloat)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (tpFloat)DOUBLE_0) ? ev[1] / sum : 0;
			w[2] = (ev[2] > (tpFloat)DOUBLE_0) ? ev[2] / sum : 0;
			w[3] = (ev[3] > (tpFloat)DOUBLE_0) ? ev[3] / sum : 0;
		}
		else
			w[0] = w[1] = w[2] = w[3] = 0.25f;
	}
	else if (cnB == 5)
	{
		if (ev[0] > (tpFloat)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (tpFloat)DOUBLE_0)
			sum += ev[1];
		if (ev[2] > (tpFloat)DOUBLE_0)
			sum += ev[2];
		if (ev[3] > (tpFloat)DOUBLE_0)
			sum += ev[3];
		if (ev[4] > (tpFloat)DOUBLE_0)
			sum += ev[4];
		if (sum > (tpFloat)DOUBLE_0)
		{
			w[0] = (ev[0] > (tpFloat)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (tpFloat)DOUBLE_0) ? ev[1] / sum : 0;
			w[2] = (ev[2] > (tpFloat)DOUBLE_0) ? ev[2] / sum : 0;
			w[3] = (ev[3] > (tpFloat)DOUBLE_0) ? ev[3] / sum : 0;
			w[4] = (ev[4] > (tpFloat)DOUBLE_0) ? ev[4] / sum : 0;
		}
		else
			w[0] = w[1] = w[2] = w[3] = w[4] = 0.2f;
	}
	else if (cnB == 6)
	{
		if (ev[0] > (tpFloat)DOUBLE_0)
			sum = ev[0];
		if (ev[1] > (tpFloat)DOUBLE_0)
			sum += ev[1];
		if (ev[2] > (tpFloat)DOUBLE_0)
			sum += ev[2];
		if (ev[3] > (tpFloat)DOUBLE_0)
			sum += ev[3];
		if (ev[4] > (tpFloat)DOUBLE_0)
			sum += ev[4];
		if (ev[5] > (tpFloat)DOUBLE_0)
			sum += ev[5];
		if (sum > (tpFloat)DOUBLE_0)
		{
			w[0] = (ev[0] > (tpFloat)DOUBLE_0) ? ev[0] / sum : 0;
			w[1] = (ev[1] > (tpFloat)DOUBLE_0) ? ev[1] / sum : 0;
			w[2] = (ev[2] > (tpFloat)DOUBLE_0) ? ev[2] / sum : 0;
			w[3] = (ev[3] > (tpFloat)DOUBLE_0) ? ev[3] / sum : 0;
			w[4] = (ev[4] > (tpFloat)DOUBLE_0) ? ev[4] / sum : 0;
			w[5] = (ev[5] > (tpFloat)DOUBLE_0) ? ev[5] / sum : 0;
		}
		else
			w[0] = w[1] = w[2] = w[3] = w[4] = w[5] = 1.f / 6;
	}
	else
	{
		for (int i = 0; i < cnB; i++)
			if (ev[i] > (tpFloat)DOUBLE_0)
				sum += ev[i];
		if (sum > (tpFloat)DOUBLE_0)
		{
			//		glCn0++;
			for (int i = 0; i < cnB; i++)
				w[i] = (ev[i] > (tpFloat)DOUBLE_0) ? ev[i] / sum : 0;
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
bool	clCalcParam::GetWeightStrat(int cnB, tpFloat *w)
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
	return (sum > DOUBLE_0);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define USE_SECTION
void	clCalcParam::AddResEV(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp)
{
#ifdef USE_SECTION
	static CRITICAL_SECTION csArray;
	static int n = 0;
	if (n == 0)
	{
		InitializeCriticalSection(&csArray);
		n = 1;
	}
	EnterCriticalSection(&csArray);
#endif

	tpFloat *evW = BeginEV();
	tpFloat *part = BeginPart(cnB);
	CnCalc()++;
	WeightCalc() += kOpp;
	AverRes() += res;
	for (int i = 0; i < cnB; i++)
	{
		evW[i] += ev[i] - res;
#ifndef TEST_GROUPvsGROUP
		part[i] += k * w[i];
#endif
	}
	/*
	unsigned val = CnCalc() << 16;
	if (val == 0)
	{						// уменьшаем накапливаемые величины
	WeightCalc() /= 2;
	AverRes() /= 2;
	for (int i = 0; i < cnB; i++)
	{
	evW[i] /= 2;
	part[i] /= 2;
	}
	}*/
#ifdef USE_SECTION
	LeaveCriticalSection(&csArray);
#endif
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::AddResEVKOpp(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp)
{
	tpFloat *evW = BeginEV();
	tpFloat *part = BeginPart(cnB);
	CnCalc()++;
	WeightCalc() += kOpp;
	AverRes() += res*kOpp;
	for (int i = 0; i < cnB; i++)
	{
		evW[i] += (ev[i] - res)*kOpp;
		part[i] += k * w[i];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::AddResEVAntiStrat(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp)
{
#ifdef USE_SECTION
	static CRITICAL_SECTION csArray;
	static int n = 0;
	if (n == 0)
	{
		InitializeCriticalSection(&csArray);
		n = 1;
	}
	EnterCriticalSection(&csArray);
#endif

	tpFloat *evW = BeginEV();
	CnCalc()++;
	WeightCalc() += kOpp;
	AverRes() += res;
	for (int i = 0; i < cnB; i++)
	{
		evW[i] += ev[i] - res;
	}

#ifdef USE_SECTION
	LeaveCriticalSection(&csArray);
#endif
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::AddResEVForCalc(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp)
{
	tpFloat *evW = BeginEV();
	tpFloat *part = BeginPart(cnB);
	CnCalc()++;
	WeightCalc() += kOpp;
	AverRes() += res;
	for (int i = 0; i < cnB; i++)
	{
		evW[i] += ev[i] - res;
		part[i] += k * w[i];
	}

	tpFloat sum = 0;
	tpFloat *ptrW = GetWeightForCalc(cnB);
	for (int i = 0; i < cnB; i++)
		if (evW[i] > DOUBLE_0)
			sum += evW[i];
	if (sum > (float)DOUBLE_0)
		for (int i = 0; i < cnB; i++)
			ptrW[i] = (evW[i] > DOUBLE_0) ? evW[i] / sum : 0;
	else
		for (int i = 0; i < cnB; i++)
			ptrW[i] = 1. / cnB;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::AddResEVForTest(int cnB, tpFloat res, tpFloat *w, tpFloat *ev, tpFloat k, tpFloat kOpp)
{
	tpFloat *evW = GetWeightForCalc(cnB);
	tpFloat *part = BeginPart(cnB);
	CnCalc()++;
	WeightCalc() += kOpp;
	AverRes() += res;
	for (int i = 0; i < cnB; i++)
	{
		evW[i] += ev[i] - res;
		part[i] += k * w[i];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::ChangeStratWeight(int cnB, tpFloat m)
{
	tpFloat *part = BeginPart(cnB);
	for (int i = 0; i < cnB; i++)
		part[i] *= m;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clCalcParam::SetParam(int cnB, tpFloat *w, int cn)
{
	CnCalc() = cn;
	WeightCalc() = cn;
	AverRes() = 0;
	tpFloat *evW = BeginEV();
	tpFloat *part = BeginPart(cnB);
	for (int i = 0; i < cnB; i++)
	{
		evW[i] = 0;
		part[i] = w[i] * cn;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double	clCalcParam::SumStratWeight(int cnB)
{
	tpFloat *part = BeginPart(cnB);
	tpFloat sum = 0;
	for (int i = 0; i < cnB; i++)
		sum += part[i];
	return sum;
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
/*int	GetDisLong(clSitHoldem &sit, tpDis *arrDis, tpDis *disH)
{
	int cn = 0;
	enNameTrade trade = sit.NameTrade();
	int bb = sit.BigBlind();
	int maxBet = sit.MaxBet();

	int stack = sit.EffStackAfterCall(sit.Indicator());
	int bank = sit.PotLimitBank();
	tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
	tpDis minR = sit.MinRaise();
	tpDis disA = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
	tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
	int minBet = minR._money - disC._money, betC = disC._money;
	double kBet = (minBet == 0) ? 1000. : (double)stack / minBet;
	int round = sit.RoundTrade();
	if (stack == 0) kBet = 0;
	if (kBet < 3 || round == 3)
	{
		arrDis[cn++] = disA;
		if (!(disA == disC))
			arrDis[cn++] = disC;
		if (!(disF == disC))
			arrDis[cn++] = disF;
	}
	else if (kBet < 5)
	{
		arrDis[cn++] = disA;
		if (!(disA == minR))
			arrDis[cn++] = minR;
		arrDis[cn++] = disC;
		if (!(disF == disC))
			arrDis[cn++] = disF;
	}
	else
	{
		double k = (double)stack / bank;
		minR._money += minBet;
		if (k < 0.8)
		{
			arrDis[cn++] = disA;
			if (kBet > 8)
			{
				int bet = betC + minBet*((kBet > 15) ? 6 : 4);
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bet));
			}
			arrDis[cn++] = minR;
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else if (k < 1.5)
		{
			arrDis[cn++] = disA;
			int bet = int(betC + bank*0.5);
			if (2 * bet < disA._money)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bet));
			else
			{
				if (kBet > 8)
				{
					bet = betC + minBet*((kBet > 15) ? 6 : 4);
					arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bet));
				}
			}
			if (arrDis[cn - 1]._money > 2 * minR._money)
				arrDis[cn++] = minR;
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else if (k < 2.5)
		{
			arrDis[cn++] = disA;
			int bet = betC + bank;
			if (2 * bet < disA._money)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bet));
			bet = int(betC + bank*0.5);
			if (2 * bet < disA._money)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bet));
			if (cn == 1 && kBet > 8)
			{
				int bet = betC + minBet*((kBet > 15) ? 6 : 4);
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bet));
			}
			if (arrDis[cn - 1]._money > 2 * minR._money)
				arrDis[cn++] = minR;
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
		else
		{
			arrDis[cn++] = disA;
			int bet = int(betC + bank*1.5);
			if (bet * 2 < stack)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bet));
			//else
			//arrDis[cn++] = disA;
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank));
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, int(betC + bank*0.5)));
			if (arrDis[cn - 1]._money > 2 * minR._money)
				arrDis[cn++] = minR;
			arrDis[cn++] = disC;
			if (!(disF == disC))
				arrDis[cn++] = disF;
		}
	}
	return cn;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	GetDisLong(clSitHoldem &sit, tpDis *arrDis, tpDis *disH)
{
	int cn = 0;
	int bb = sit.BigBlind();
	int maxBet = sit.MaxBet();
	int stack = sit.EffStackAfterCall(sit.Indicator());
	int bank = sit.PotLimitBank();
	double k = (double)stack / bank;
	tpDis disC = sit.CreateDis(HistCreateDis(ACT_CALL));
	if (sit.NameTrade() == TRADE_PREFLOP && maxBet == bb)
	{
		if (stack >= 20 * bb)
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 5 * bb));
		else
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
		if (stack >= 6 * bb)
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 3 * bb));
		if (stack >= 4 * bb)
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE_TO, 2 * bb));
		if (!(arrDis[cn - 1] == disC))
			arrDis[cn++] = disC;
		tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
		if (!(disC == disF))
			arrDis[cn++] = disF;

		//arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
		//arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_FOLD));
		return cn;
	}
	if (maxBet == 0)
	{
		if (k <= 10)
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
		if (k > 12)
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bank * 3));
		if (k > 4.5)
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bank * 1.5));
		if (k > 3)
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bank));
		if (k > 2)
			arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bank * 0.6));
		if (k > 0.5)
		{
			tpDis dis = sit.CreateDis(HistCreateDis(ACT_RAISE, bank * 0.3));
			if (!(dis == disC))
				if (cn > 0 && dis._money * 2 <= arrDis[cn - 1]._money)
					arrDis[cn++] = dis;
			if (cn == 2 && arrDis[1]._money > 5 * bb)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, 2 * bb));
		}
		if (cn == 1)
		{
			double kBet = (double)stack / bb;
			if (kBet > 14.9)
			{
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, stack / 3));
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, 2 * bb));
			}
			else if (kBet > 9.9)
			{
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, stack / 2));
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, 2 * bb));
			}
			else if (kBet > 6)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, 2 * bb));
			else if (kBet > 3.5)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, bb));
		}
		arrDis[cn++] = disC;
	}
	else
	{
		int round = sit.RoundTrade();
		if (round > 1 && sit.Indicator() == 0)
		{
			int nbA = sit.NbFirstAct(ACT_DELRIVER);
			tpAction act = *sit.GetAct(nbA + 1);
			if (act._pos == 0 && act._dis._act == ACT_CHECK)
				round--;
		}
		tpDis disA = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
		int betC = disC._money;
		if (round == 1)
		{
			if (k < 3)
				arrDis[cn++] = disA;
			else
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank * 1.5));
			if (k > 2)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank));
			if (k > 1.5)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank * 0.5));
			if (k > 0.8)
			{
				tpDis dis = sit.MinRaise();
				if (dis._money * 2 <= arrDis[cn - 1]._money)
					arrDis[cn++] = dis;
			}
		}
		else if (round == 2)
		{
			if (k < 3)
				arrDis[cn++] = disA;
			if (k > 2)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank));
			if (k > 1.5)
				arrDis[cn++] = sit.CreateDis(HistCreateDis(ACT_RAISE, betC + bank * 0.5));
		}
		else
			arrDis[cn++] = disA;
		if (!(disC == disA))
			arrDis[cn++] = disC;
		tpDis disF = sit.CreateDis(HistCreateDis(ACT_FOLD));
		if (!(disC == disF))
			arrDis[cn++] = disF;
	}
	return cn;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	SetResSit3Pl(clSitHoldem &sit, int *ptr)
{
	clGamePoker game[6] = { POKER_MIN_GAME, POKER_MIN_GAME, POKER_MIN_GAME, POKER_MIN_GAME, POKER_MIN_GAME, POKER_MIN_GAME };
	// 123
	clSitHoldem sit0 = sit;
	int nb = 0;
	game[0].game = GM_4;
	game[1].game = GM_3;
	game[2].game = GM_2;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 132
	sit0 = sit; nb = 3;
	game[0].game = GM_4;
	game[1].game = GM_2;
	game[2].game = GM_3;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 122
	sit0 = sit; nb = 6;
	game[0].game = GM_4;
	game[1].game = GM_2;
	game[2].game = GM_2;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 213
	sit0 = sit; nb = 9;
	game[0].game = GM_3;
	game[1].game = GM_4;
	game[2].game = GM_2;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 312
	sit0 = sit; nb = 12;
	game[0].game = GM_2;
	game[1].game = GM_4;
	game[2].game = GM_3;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 212
	sit0 = sit; nb = 15;
	game[0].game = GM_2;
	game[1].game = GM_4;
	game[2].game = GM_2;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 231
	sit0 = sit; nb = 18;
	game[0].game = GM_3;
	game[1].game = GM_2;
	game[2].game = GM_4;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 321
	sit0 = sit; nb = 21;
	game[0].game = GM_2;
	game[1].game = GM_3;
	game[2].game = GM_4;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 221
	sit0 = sit; nb = 24;
	game[0].game = GM_2;
	game[1].game = GM_2;
	game[2].game = GM_4;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 112
	sit0 = sit; nb = 27;
	game[0].game = GM_4;
	game[1].game = GM_4;
	game[2].game = GM_2;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 121
	sit0 = sit; nb = 30;
	game[0].game = GM_4;
	game[1].game = GM_2;
	game[2].game = GM_4;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 211
	sit0 = sit; nb = 33;
	game[0].game = GM_2;
	game[1].game = GM_4;
	game[2].game = GM_4;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
	// 111
	sit0 = sit; nb = 36;
	game[0].game = GM_4;
	game[1].game = GM_4;
	game[2].game = GM_4;
	sit0.DistribPot(game);
	ptr[nb] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
	ptr[nb + 1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
	ptr[nb + 2] = sit0.PlayerMoney(2) - sit0.PlMoney(2);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		*GetResSit3Pl(int *ptr, int f0, int f1, int f2)
{
	if (f0 > f1)
	{
		if (f1 > f2)					//123
			return ptr;
		else if (f1 == f2)				//122
			return ptr + 6;
		else if (f0 > f2)				//132
			return ptr + 3;
		else if (f0 < f2)				//231
			return ptr + 18;
		else								//121
			return ptr + 30;
	}
	else if (f0 < f1)
	{
		if (f1 < f2)					//321
			return ptr + 21;
		else if (f1 == f2)				//211
			return ptr + 33;
		else if (f0 > f2)				//213
			return ptr + 9;
		else if (f0 < f2)				//312
			return ptr + 12;
		else								//212
			return ptr + 15;
	}
	else
	{
		if (f1 < f2)					//221
			return ptr + 24;
		else if (f1 == f2)				//111
			return ptr + 36;
		else								//112
			return ptr + 27;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CreateSit3(clSitHoldem &sit, clSitHoldem &sitIn)
{
	if (sit.CnPlayer() == 2)
	{
		clStacks st = sit.GetStacks();
		st[2] = st[0];
		sitIn.Init(st);
		sitIn.ExecAct(HistAction(2, ACT_FOLD, 0));
		for (int i = 3; i < sit.CnAct(); i++)
			sitIn.ExecDis(sit.GetAct(i)->_dis);
		memcpy(sitIn._atrHH._cardTable, sit._atrHH._cardTable, sizeof(sit._atrHH._cardTable));
	}
	else if (sit.CnPlayer() == 3)
		sitIn = sit;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clAtrHoldemTree::SizeInFile()
{
	size_t res = sizeof(tpDis) + sizeof(_statCn) + sizeof(_sizeParam);
	res += SizeVectFile(_param);
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::WriteFile(int handle)
{
	_write(handle, &_sitA.GetLastAct()->_dis, sizeof(tpDis));
	_write(handle, &_statCn, sizeof(_statCn));
	_write(handle, &_sizeParam, sizeof(_sizeParam));
	WriteVectFile(handle, _param);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::ReadFile(int handle)
{
	_read(handle, &_sitA._arrAct._act[0]._dis, sizeof(tpDis));
	_read(handle, &_statCn, sizeof(_statCn));
	_read(handle, &_sizeParam, sizeof(_sizeParam));
	ReadVectFile(handle, _param);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::operator = (clAtrHoldemTree &atr)
{
	_sitA = atr._sitA;
	_statCn = atr._statCn;
	_sizeParam = atr._sizeParam;
	_param = atr._param;
#ifdef NB_NODE
	_nbNode = atr._nbNode;
#endif
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::ClearEV(int cnB)
{
	if (cnB > 1)
	{
		int dim = CnClasters();
		for (int i = 0; i < dim; i++)
			CalcParam(i).ClearEV(cnB);
	}
	_statCn = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::ClearData(int cnB)
{
	if (_param.size() > 0)
		ZeroMemory(&_param[0], _param.size());
	_statCn = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int  clAtrHoldemTree::Result(int *force, int ind)
{
	int *ptr = (int *)&_param[0];
	if (_sitA.CnPlayer() == 2)
	{
		if (force[0] == force[1])
			ptr += 2;
		else if (force[0] < force[1])
			ptr += 4;
		return ptr[ind];
	}
	else
	{
		if (_sitA.CNPlayerIsCard() == 1)
			return ptr[ind];
		else
		{
			int nb = 0;
			if (force[0] > force[1])
			{
				if (force[1] > force[2])
					nb = 0; // 123
				else if (force[1] < force[2] && force[2] < force[0])
					nb = 3;	// 132
				else if (force[1] == force[2])
					nb = 6;	// 122
				else if (force[0] < force[2])
					nb = 18;	// 231
				else if (force[0] == force[2])
					nb = 30;	// 121
			}
			else if (force[0] < force[1])
			{
				if (force[0] > force[2])
					nb = 9; // 213
				else if (force[1] > force[2] && force[2] > force[0])
					nb = 12; // 312
				else if (force[2] == force[0])
					nb = 15; // 212
				else if (force[2] > force[1])
					nb = 21; // 321
				else if (force[2] == force[1])
					nb = 33; // 211
			}
			else
			{
				if (force[0] < force[2])
					nb = 24; // 221
				else if (force[0] > force[2])
					nb = 27; // 112
				else
					nb = 36; //111
			}
			return ptr[nb + ind];
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP  clAtrHoldemTree::Result(int *force)
{
	int *ptr = (int *)&_param[0];
	if (force[0] == force[1])
		ptr += 2;
	else if (force[0] < force[1])
		ptr += 4;
	clDoubleCP res;
	res[0] = ptr[0];
	res[1] = ptr[1];
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double  clAtrHoldemTree::ResultPreFlop(tpTreePlayerInfo *info, int ind)
{
	int *ptr = (int *)&_param[0];
	if (_sitA.CNPlayerInTrade() < 2)	//AllIn
	{
		double win = HCompWinHand169(info->_nbCls[TRADE_PREFLOP][0], info->_nbCls[TRADE_PREFLOP][1]).WinEq();
		if (ind == 0)
			return ptr[0] * win + ptr[4] * (1 - win);
		else
			return ptr[5] * (1 - win) + ptr[1] * win;

	}
	else
		return ptr[ind];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::CalcResSit(clSitHoldem &sit)
{
	if (sit.CnPlayer() == 2)
	{
		_param.resize(6 * sizeof(int));
		int *ptr = (int *)&_param[0];
		if (sit.CNPlayerIsCard() == 1)
		{
			sit.CloseBank();
			for (int i = 0; i < sit.CnPlayer(); i++)
			{
				if (sit.PlayerIsCard(i))
					sit.PlayerAtr(i)->_money += sit.TotalPot();
				ptr[i] = sit.ResultGame(i);
			}
			ptr[2] = ptr[0];
			ptr[3] = ptr[1];
			ptr[4] = ptr[0];
			ptr[5] = ptr[1];
		}
		else if (sit.CNPlayerInTrade() < 2 || sit.NameTrade() == TRADE_RIVER)
		{
			clSitHoldem sit0 = sit;
			clGamePoker game[6] = { POKER_MIN_GAME, POKER_MIN_GAME, POKER_MIN_GAME, POKER_MIN_GAME, POKER_MIN_GAME, POKER_MIN_GAME };
			game[0].game = GM_3;
			game[1].game = GM_2;
			sit0.DistribPot(game);
			ptr[0] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
			ptr[1] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
			sit0 = sit;
			game[0].game = GM_2;
			game[1].game = GM_2;
			sit0.DistribPot(game);
			ptr[2] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
			ptr[3] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
			sit0 = sit;
			game[0].game = GM_2;
			game[1].game = GM_3;
			sit0.DistribPot(game);
			ptr[4] = sit0.PlayerMoney(0) - sit0.PlMoney(0);
			ptr[5] = sit0.PlayerMoney(1) - sit0.PlMoney(1);
		}
	}
	else
	{
		if (sit.CNPlayerIsCard() == 1)
		{
			_param.resize(3 * sizeof(int));
			int *ptr = (int *)&_param[0];
			sit.CloseBank();
			for (int i = 0; i < sit.CnPlayer(); i++)
			{
				if (sit.PlayerIsCard(i))
					sit.PlayerAtr(i)->_money += sit.TotalPot();
				ptr[i] = sit.ResultGame(i);
			}
		}
		else if (sit.CNPlayerInTrade() < 2 || sit.NameTrade() == TRADE_RIVER)
		{
			_param.resize(13 * 3 * sizeof(int));
			int *ptr = (int *)&_param[0];
			SetResSit3Pl(sit, ptr);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::GetAllWeight(int cnB, tpFloat *w)
{
	int dim = CnClasters();
	for (int j = 0; j < dim; j++)
	{
		clCalcParam cls = CalcParam(j);
		cls.GetWeight(cnB, w + j*cnB);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::GetAllWeightNoNull(int cnB, tpFloat *w)
{
	int dim = CnClasters();
	for (int j = 0; j < dim; j++)
	{
		clCalcParam cls = CalcParam(j);
		tpFloat *ptr = w + j*cnB;
		cls.GetWeight(cnB, ptr);

		bool b0 = false;
		tpFloat sum = 1;
		for (int i = 0; i < cnB; i++)
			if (ptr[i] < DOUBLE_0)
			{
				//				if (!b0)
				//				{
				//					cls.GetWeightStrat(cnB, wS);
				//					b0 = true;
				//				}
				ptr[i] = 0.001;
				sum += 0.001;
				b0 = true;
			}
		if (b0)
			for (int i = 0; i < cnB; i++)
				ptr[i] /= sum;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::GetAllWeightStrat(int cnB, tpFloat *w)
{
	int dim = CnClasters();
	for (int j = 0; j < dim; j++)
	{
		clCalcParam cls = CalcParam(j);
		cls.GetWeightStrat(cnB, w + j*cnB);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t  clAtrHoldemTree::CnCalc()
{
	size_t res = 0, dim = CnClasters();
	for (int i = 0; i < (int)dim; i++)
		res += CalcParam(i).CnCalc();
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t  clAtrHoldemTree::CnCalcOneCls()
{
	size_t res = 0, dim = CnClasters();
	for (int i = 0; i < (int)dim; i++)
		res += CalcParam(i).CnCalc();
	return (dim > 0) ? res / dim : 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::AddDataBranch(int cnB, int nb, int nbSrc)
{
	int cnCls = CnClasters();
	for (int i = 0; i < cnCls; i++)
	{
		clCalcParam par = CalcParam(i);
		tpFloat *ev = par.BeginEV();
		tpFloat *part = par.BeginPart(cnB);
		ev[nb] += ev[nbSrc];
		part[nb] += part[nbSrc];
	}
	DeleteBranch(cnB, nbSrc);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clAtrHoldemTree::DeleteBranch(int cnB, int nb)
{
	int newSize = clCalcParam::SizeParam(cnB - 1);
	int cnCls = CnClasters();
	for (int i = 0; i < cnCls; i++)
	{
		clCalcParam par = CalcParam(i);
		tpFloat *ev = par.BeginEV();
		tpFloat *part = par.BeginPart(cnB);
		for (int j = nb + 1; j < cnB; j++)
		{
			ev[j - 1] = ev[j];
			part[j - i] = part[j];
		}
		byte *ptrNew = &_param[i*newSize];
		memcpy(ptrNew, par._ptr, newSize);
	}
	_sizeParam = newSize;
	_param.resize(cnCls*newSize);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int clHoldemTree::AddBranch(clSitHoldem &sit)
{
	clHoldemTree *node = AddBranch();
	node->AtrTree()->_sitA = sit;
	node->_weight = 0;
	return CnBranch() - 1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::ClearEV()
{
	if (CnBranch() > 1)
		AtrTree()->ClearEV(CnBranch());
	for (int k = 0; k < CnBranch(); k++)
		Branch(k)->ClearEV();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::ClearData()
{
	if (CnBranch() > 1)
		AtrTree()->ClearData(CnBranch());
	for (int k = 0; k < CnBranch(); k++)
		Branch(k)->ClearData();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::ReestablishSit(clSitHoldem sit)
{
	if (Parent() != NULL)
		sit.ExecDis(AtrTree()->_sitA._arrAct._act[0]._dis);
	AtrTree()->_sitA = sit;
	for (int i = 0; i < CnBranch(); i++)
		Branch(i)->ReestablishSit(sit);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::ClearStatWeight()
{
	AtrTree()->_statCn = 0;
	for (int k = 0; k < CnBranch(); k++)
		Branch(k)->ClearStatWeight();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat	clHoldemTree::CalcNesh(int nbHero, tpTreePlayerInfo *info, tpFloat kH, tpFloat kOpp, tpFloat k0)
{
	AtrTree()->_statCn += kH*kOpp;
	int ind = Indicator();
	if (EndedNode())
	{
		if (GetSit().NameTrade() == TRADE_PREFLOP)
			return AtrTree()->ResultPreFlop(info, nbHero)*kOpp;
		return AtrTree()->Result(info->_handForce, nbHero)*kOpp;
	}
	if (ind == -1)
		if (CnBranch() == 1)
			return Branch(0)->CalcNesh(nbHero, info, kH, kOpp, k0);
		else
		{
			ErrMessage("Error", "");
			return 0;
		}
	enNameTrade trade = GetSit().NameTrade();
	clCalcParam cls = AtrTree()->CalcParam(info->_nbCls[trade][ind]);
	tpFloat w[MAX_CN_BRANCH];

	cls.GetWeight(CnBranch(), w);
	tpFloat res = 0;
	if (ind == nbHero)
	{
		tpFloat ev[MAX_CN_BRANCH];// = { 0, 0, 0, 0, 0, 0, 0, 0 };
		for (int k = 0; k < CnBranch(); k++)
		{
			ev[k] = Branch(k)->CalcNesh(nbHero, info, kH*w[k], kOpp, k0);
			res += ev[k] * w[k];
		}
		cls.AddResEV(CnBranch(), res, w, ev, kH, kOpp);

	}
	else
	{
		if (k0 > DOUBLE_0)
		{
			tpFloat sum = 1;
			for (int k = 0; k < CnBranch(); k++)
				if (w[k] < DOUBLE_0)
				{
					w[k] += k0;
					sum += k0;
				}
			for (int k = 0; k < CnBranch(); k++)
				w[k] /= sum;
		}
		for (int k = 0; k < CnBranch(); k++)
			if (w[k] > DOUBLE_0)
			{
				tpFloat f0 = Branch(k)->CalcNesh(nbHero, info, kH, kOpp*w[k], k0);
				res += f0;
			}
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat	clHoldemTree::CalcNeshKOpp(int nbHero, tpTreePlayerInfo *info, tpFloat kH, tpFloat kOpp, tpFloat k0)
{
	AtrTree()->_statCn += kH*kOpp;
	int ind = Indicator();
	if (EndedNode())
		return AtrTree()->Result(info->_handForce, nbHero);
	if (ind == -1)
		if (CnBranch() == 1)
			return Branch(0)->CalcNeshKOpp(nbHero, info, kH, kOpp, k0);
		else
		{
			ErrMessage("Error", "");
			return 0;
		}
	enNameTrade trade = GetSit().NameTrade();
	clCalcParam cls = AtrTree()->CalcParam(info->_nbCls[trade][ind]);
	tpFloat w[MAX_CN_BRANCH];

	cls.GetWeight(CnBranch(), w);
	tpFloat res = 0;
	if (ind == nbHero)
	{
		tpFloat ev[MAX_CN_BRANCH];// = { 0, 0, 0, 0, 0, 0, 0, 0 };
		for (int k = 0; k < CnBranch(); k++)
		{
			ev[k] = Branch(k)->CalcNeshKOpp(nbHero, info, kH*w[k], kOpp, k0);
			res += ev[k] * w[k];
		}
		cls.AddResEVKOpp(CnBranch(), res, w, ev, kH, kOpp);
	}
	else
	{
		if (k0 > DOUBLE_0)
		{
			tpFloat sum = 1;
			for (int k = 0; k < CnBranch(); k++)
				if (w[k] < DOUBLE_0)
				{
					w[k] += k0;
					sum += k0;
				}
			for (int k = 0; k < CnBranch(); k++)
				w[k] /= sum;
		}
		for (int k = 0; k < CnBranch(); k++)
			if (w[k] > DOUBLE_0)
			{
				tpFloat f0 = Branch(k)->CalcNeshKOpp(nbHero, info, kH, kOpp*w[k], k0)*w[k];
				res += f0;
			}
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clHoldemTree::SetStartParam(int *cnHand)
{
	if (CnBranch() > 0)
	{
		int ind = Indicator();
		if (ind == -1)
		{
			AtrTree()->_sizeParam = 1;
			AtrTree()->_param.clear();
		}
		else
		{
			int cnCls = cnHand[Indicator()];
			int size = clCalcParam::SizeParam(CnBranch());
			AtrTree()->_sizeParam = size;
			AtrTree()->_param.resize(cnCls*size);
			byte *ptr = &AtrTree()->_param[0];
			std::ZeroMemory(ptr, cnCls*size);
		}
	}
	else return 0;
	size_t res = AtrTree()->SizeInFile();
	tpFloat val = 1. / CnBranch();
	for (int i = 0; i < CnBranch(); i++)
	{
		res += Branch(i)->SetStartParam(cnHand);
		Branch(i)->_weight = val;
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clHoldemTree::SetStartParam(int **cnHand, bool setMem)
{
	if (CnBranch() > 0)
	{
		int ind = Indicator();
		if (ind == -1)
		{
			AtrTree()->_sizeParam = 1;
			AtrTree()->_param.clear();
		}
		else
		{
			enNameTrade trade = GetSit().NameTrade();
			int cnCls = cnHand[trade][Indicator()];
			int size = clCalcParam::SizeParam(CnBranch());
			AtrTree()->_sizeParam = size;
			AtrTree()->_param.resize(cnCls*size);
			byte *ptr = &AtrTree()->_param[0];
			std::ZeroMemory(ptr, cnCls*size);
		}
	}
	else return 0;
	size_t res = AtrTree()->SizeInFile();
	if (!setMem)
	{
		AtrTree()->_param.clear();
		AtrTree()->_param.shrink_to_fit();
	}
	tpFloat val = 1. / CnBranch();
	for (int i = 0; i < CnBranch(); i++)
	{
		res += Branch(i)->SetStartParam(cnHand, setMem);
		Branch(i)->_weight = val;
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::SetResSit()
{
	if (EndedNode())
		AtrTree()->CalcResSit(GetSit());
	for (int i = 0; i < CnBranch(); i++)
		Branch(i)->SetResSit();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void		clHoldemTree::CalcWeightStat()
{
	double sum = 0;
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
void		clHoldemTree::SetWeightStrat(tpTreePlayerInfo *info, double *wh)
{
	AtrTree()->_statCn += wh[0] * wh[1];
	int ind = Indicator();
	if (ind == -1 || CnBranch() == 0) return;
	if (CnBranch() == 1)
	{
		Branch(0)->SetWeightStrat(info, wh);
		return;
	}
	double curW[] = { wh[0],wh[1] };
	int trade = GetSit().NameTrade();
	int nbCls = info->_nbCls[trade][ind];
	clCalcParam par = AtrTree()->CalcParam(nbCls);
	tpFloat w[MAX_CN_BRANCH];
	par.GetWeightStrat(CnBranch(), w);
	for (int i = 0; i < CnBranch(); i++)
	{
		curW[ind] = wh[ind] * w[i];
		Branch(i)->SetWeightStrat(info, curW);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void		clHoldemTree::CalcWeightStrat(vector <tpFloat> **gh)
{
	int ind = Indicator();
	vector <tpFloat> ghCur,*temp=gh[ind];
	gh[ind] = &ghCur;
	int dim = gh[ind]->size();
	ErrCheck("Не совпадают размерности", dim, AtrTree()->CnClasters());
	tpFloat *w=new tpFloat[CnBranch()*dim];
	AtrTree()->GetAllWeight(CnBranch(), w);
	double ws[MAX_CN_BRANCH];
	for (int k = 0; k < CnBranch(); k++)
	{
		ghCur = *temp;
		for (int i = 0; i<dim; i++)
			ghCur[i] *= w[i*CnBranch() + k];
		Branch(k)->CalcWeightStrat(gh);
//		необходимо учесть пересекаемость хэндов
	}
	delete w;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clAnsiString	clHoldemTree::NameForBranch(int nb)
{
	tpAction act = { Branch(nb)->GetDis(),Indicator() };
	return HistActToString(act);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		clHoldemTree::FindNbBranch(clHoldemTree *branch)
{
	for (int i = 0; i < CnBranch(); i++)
		if (_arr[i] == branch)
			return i;
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::ChangeGH(int nb, clHandsGroupEx &ghH, vector <int> &nbHand)	// размерность ghH равна ALL_CN_HAND, а узла размерности nbHand
{
	int dim = nbHand.size();
	for (int i = 0; i < dim; i++)
		if (ghH[nbHand[i]] > DOUBLE_0)
		{
			clCalcParam cls = AtrTree()->CalcParam(i);
			tpFloat w[MAX_CN_BRANCH];
			cls.GetWeightStrat(CnBranch(), w);
			ghH[nbHand[i]] *= w[nb];
		}
	//	ghH.Norm();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::ChangeGH(int nb, clHandsGroupEx &ghH)
{
	int dim = ghH.Dimension();
	for (int i = 0; i < dim; i++)
		if (ghH[i] > DOUBLE_0)
		{
			clCalcParam cls = AtrTree()->CalcParam(i);
			tpFloat w[MAX_CN_BRANCH];
			cls.GetWeightStrat(CnBranch(), w);
			ghH[i] *= w[nb];
		}
	//	ghH.Norm();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::ChangeGH(int nb, vector <tpFloat> &ghH)
{
	int dim = (int)ghH.size();
	for (int i = 0; i < dim; i++)
	{
		clCalcParam cls = AtrTree()->CalcParam(i);
		tpFloat w[MAX_CN_BRANCH];
		cls.GetWeightStrat(CnBranch(), w);
		ghH[i] *= w[nb];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int 	clHoldemTree::SetNbNode(int &nb)
{
#ifdef NB_NODE
	AtrTree()->_nbNode = nb++;
	for (int i = 0; i < CnBranch(); i++)
		Branch(i)->SetNbNode(nb);
#endif
	return nb;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::GetGHForNode(vector <tpFloat> *gh)
{
	if (_parent == NULL)
	{

	}
	else
	{
		Parent()->GetGHForNode(gh);
		int nb = Parent()->FindNbBranch(this);
		int ind = Parent()->Indicator();
		Parent()->ChangeGH(nb, gh[ind]);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*int clHoldemTree::CnNode(enNameTrade trade)
{
	int res = (_val->_sitA.NameTrade() == trade) ? 1 : 0;
	for (int i = 0; i < CnBranch(); i++)
		res += Branch(i)->CnNode(trade);
	return res;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP clHoldemTree::CalcStratEV(tpTreePlayerInfo *info)
{
	int ind = Indicator();
	if (EndedNode())
		return AtrTree()->Result(info->_handForce);
	if (ind == -1)
	{
		if (CnBranch() == 1)
			return Branch(0)->CalcStratEV(info);
		glTrasser.WriteMessage((clAnsiString)"End Tree");
		return 0;
	}
	enNameTrade trade = GetSit().NameTrade();
	int nbCls = info->_nbCls[trade][ind];
	clCalcParam cls = AtrTree()->CalcParam(nbCls);
	tpFloat w[MAX_CN_BRANCH];
	cls.GetWeightStrat(CnBranch(), w);
	clDoubleCP res = 0;
	for (int k = 0; k < CnBranch(); k++)
	{
		clDoubleCP val = Branch(k)->CalcStratEV(info);
		res += val* w[k];
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clHoldemTree	*clHoldemTree::FindSitNode(int cnAct, clSitHoldem &sit, int nextAct)
{
	if (cnAct == 0)
		return this;
	tpDis dis = sit.GetAct(nextAct)->_dis;
	int i = 0;
	for (; i < CnBranch(); i++)
		if (dis == Branch(i)->GetDis())
			break;

	return (i == CnBranch()) ? NULL : Branch(i)->FindSitNode(cnAct - 1, sit, nextAct + 1);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clHoldemTree	*clHoldemTree::FindSitNodePushFold(int cnAct, clSitHoldem &sit, int nextAct)
{
	if (cnAct == 0)
		return this;
	enAction act = sit.GetAct(nextAct)->_dis._act;
	if (act != ACT_FOLD) act = ACT_ALL_IN;
	int i = 0;
	for (; i < CnBranch(); i++)
		if (act == Branch(i)->GetDis()._act)
			break;

	return (i == CnBranch()) ? NULL : Branch(i)->FindSitNodePushFold(cnAct - 1, sit, nextAct + 1);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::FindSitNode(int cnAct, clSitHoldem &sit, int nextAct, vector <stNodeW> &vect, double k)
{
	if (cnAct == 0)
	{
		//if (GetSit().Indicator() == sit.Indicator())
		{
			stNodeW nw;
			nw._ptr = this;
			nw._weight = k;
			vect.push_back(nw);
		}
		return;
	}
	tpDis dis = sit.GetAct(nextAct)->_dis;
	stNbBtw nb = { -1,-1,0,0 };
	FindBranchBetween(dis, nb);
	if (nb._nb0 == -1)
	{
		if (dis._act == ACT_ALL_IN)
		{
			dis._act = ACT_RAISE;
			FindBranchBetween(dis, nb);
			if (nb._nb0 == -1)
				return;
		}
		else
			return;
	}
	if (nb._nb1 != -1 && nb._w1 > 0.2)
		Branch(nb._nb1)->FindSitNode(cnAct - 1, sit, nextAct + 1, vect, k*nb._w1);
	else nb._w0 = 1;
	Branch(nb._nb0)->FindSitNode(cnAct - 1, sit, nextAct + 1, vect, k*nb._w0);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::FindBranchBetween(tpDis dis, stNbBtw &nb)
{
	if (CnBranch() == 1)
	{
		nb._nb0 = 0;
		nb._nb1 = -1;
		nb._w0 = 1;
		return;
	}
	if (dis._act == ACT_FOLD || dis._act == ACT_CHECK || dis._act == ACT_CALL || dis._act == ACT_ALL_IN)
	{
		for (int i = 0; i < CnBranch(); i++)
			if (Branch(i)->GetDis()._act == dis._act)
			{
				nb._nb0 = i;
				nb._nb1 = -1;
				nb._w0 = 1;
				return;
			}
		nb._nb0 = nb._nb1 = -1;
	}
	else if (dis._act == ACT_BET || dis._act == ACT_RAISE)
	{
		int nbB = -1, nbL = -1;
		for (int i = 0; i < CnBranch(); i++)
		{
			tpDis &disT = Branch(i)->GetDis();
			if (disT._act >= ACT_BET)
			{
				if (disT._money == dis._money)
				{
					nb._nb0 = i;
					nb._nb1 = -1;
					nb._w0 = 1;
					return;
				}
				if (disT._money > dis._money)
				{
					if (nbB == -1)
						nbB = i;
					else if (disT._money < Branch(nbB)->GetDis()._money)
						nbB = i;
				}
				else if (disT._money < dis._money)
					if (nbL == -1)
						nbL = i;
					else if (disT._money > Branch(nbL)->GetDis()._money)
						nbL = i;
			}
		}
		if (nbB == -1)
		{
			nb._nb0 = nbL;
			nb._w0 = 1;
			nb._nb1 = -1;
			return;
		}
		if (nbL == -1)
		{
			nb._nb0 = nbB;
			nb._w0 = 1;
			nb._nb1 = -1;
			return;
		}
		int dB = Branch(nbB)->GetDis()._money - dis._money;
		int dL = dis._money - Branch(nbL)->GetDis()._money;
		if (dB < dL)
		{
			nb._nb0 = nbB;
			nb._nb1 = nbL;
			nb._w0 = (double)dL / (dB + dL);
			nb._w1 = (double)dB / (dB + dL);
		}
		else
		{
			nb._nb0 = nbL;
			nb._nb1 = nbB;
			nb._w0 = (double)dB / (dB + dL);
			nb._w1 = (double)dL / (dB + dL);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clHoldemTree *clHoldemTree::GoBack(enAction act)
{
	if (GetDis()._act == act) return this;
	clHoldemTree *parent = Parent();
	if (parent == NULL) return NULL;
	return parent->GoBack(act);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clHoldemTree::CheckAllNode()
{
	if (EndedNode())
	{
		if (!GetSit().EndedSit())
			return false;
	}
	for (int i = 0; i < CnBranch(); i++)
		if (!Branch(i)->CheckAllNode())
			return false;
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double	clHoldemTree::TreeVSTree(tpTreePlayerInfo *info, int nbHero, clHoldemTree *node)
{
	int ind = Indicator();
	if (EndedNode())
		return AtrTree()->Result(info->_handForce, nbHero);
	if (ind == -1)
		if (CnBranch() == 1)
			return Branch(0)->TreeVSTree(info, nbHero, node->Branch(0));
		else
		{
			ErrMessage("Error", "");
			return 0;
		}
	enNameTrade trade = GetSit().NameTrade();
	double res = 0;
	tpFloat w[MAX_CN_BRANCH];
	clCalcParam cls;
	if (ind == nbHero)
		cls = AtrTree()->CalcParam(info->_nbCls[trade][ind]);
	else
		cls = node->AtrTree()->CalcParam(info->_nbCls[trade][ind]);
	cls.GetWeightStrat(CnBranch(), w);

	for (int k = 0; k < CnBranch(); k++)
		if (w[k] > DOUBLE_0)
		{
			double resT = Branch(k)->TreeVSTree(info, nbHero, node->Branch(k));
			res += resT * w[k];
		}

	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHoldemTree::AddWeightNode(tpTreePlayerInfo *info, double kW)
{
	AtrTree()->_statCn += kW;
	if (CnBranch() > 1)
	{
		int ind = Indicator();
		enNameTrade trade = GetSit().NameTrade();
		tpFloat w[MAX_CN_BRANCH];
		clCalcParam cls = AtrTree()->CalcParam(info->_nbCls[trade][ind]);
		cls.GetWeightStrat(CnBranch(), w);
		for (int k = 0; k < CnBranch(); k++)
			if (w[k] > DOUBLE_0)
				Branch(k)->AddWeightNode(info, kW*w[k]);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clHoldemTree::CutTree(enAction act)
{
	bool res = false;
	int nb = AtrTree()->_nbNode;
	tpDis dis = GetDis();
	for (int i = CnBranch() - 1; i >= 0; i--)
	{
		tpDis disB = Branch(i)->GetDis();
		if (Branch(i)->GetDis()._act == act)
		{
			DelBranch(i);
			res = true;
		}
		else
		{
			bool res0 = Branch(i)->CutTree(act);
			res = res || res0;
		}
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	BuildTreeNLAuto(clHoldemTree *ptrT, clSitHoldem sit, enNameTrade stF)
{
	int ind = sit.Indicator();
	enNameTrade trade = sit.NameTrade();
	if (ind == -1)
	{
		//ptrT->GetDis() = sit.GetLastAct()->_dis;
		if (sit.CNPlayerIsCard() == 1)
			return;
		if (sit.CNPlayerInTrade() < 2)
			return;
		if (trade == stF)
			return;
		if (trade == TRADE_PREFLOP && sit.CNPlayerInTrade() > 1)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELFLOP, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeNLAuto(ptrT->Branch(nb), sit, stF);
		}
		else if (trade == TRADE_FLOP)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELTURN, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeNLAuto(ptrT->Branch(nb), sit, stF);
		}
		else if (trade == TRADE_TURN)
		{
			sit.ExecAct(HistAction(NB_DILER, ACT_DELRIVER, 0));
			int nb = ptrT->AddBranch(sit);
			BuildTreeNLAuto(ptrT->Branch(nb), sit, stF);
		}
		return;
	}

	clSitHoldem sit0;
	tpDis arrDis[MAX_CN_BRANCH];
	int cnDis = GetDisLong(sit, arrDis);

	/*if (trade == TRADE_FLOP)
	{
		arrDis[0] = sit.CreateDisCallCheck();
		cnDis = 1;
	}

	/*if (arrDis[0]._act != ACT_ALL_IN)
	{
		tpDis dis = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
		for (int i = cnDis; i > 0; i--)
			arrDis[i] = arrDis[i - 1];
		cnDis++;
		arrDis[0] = dis;
	}*/

	for (int i = 0; i < cnDis; i++)
	{
		sit0 = sit;
		sit0.ExecDis(arrDis[i]);
		int nb = ptrT->AddBranch(sit0);
		BuildTreeNLAuto(ptrT->Branch(nb), sit0, stF);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	BuildTreeNLFromHistory(clHoldemTree *ptrT, clSitHoldem &sit, clSitHoldem *sitH, int nbA)
{
	if (sitH != NULL && sitH->CurNbAct() <= nbA) sitH = NULL;
	int ind = sit.Indicator();

	if (ind == -1)
		return;

	clSitHoldem sit0;
	tpDis arrDis[MAX_CN_BRANCH];
	int cnDis = GetDisLong(sit, arrDis);
	int nbHist = -1;
	if (sitH != NULL)
	{
		int min = sit.EffStackSit();
		tpDis disH = sitH->GetAct(nbA)->_dis;
		for (int i = 0; i < cnDis; i++)
			if (disH == arrDis[i])
			{
				nbHist = i;
				min = 0;
				break;
			}
			else if (disH._act >= ACT_BET && arrDis[i]._act >= ACT_BET)
			{
				int dist = abs(disH._money - arrDis[i]._money);
				if (dist < min)
				{
					min = dist;
					nbHist = i;
				}
			}
		if (nbHist != -1)
		{
			double d = (arrDis[nbHist]._money == 0) ? 0 : (double)min / arrDis[nbHist]._money;
			if (d < 0.3)
				arrDis[nbHist] = disH;  // заменяем решение
			else
				nbHist = -1;
		}
		if (nbHist == -1) // добавляем решение
		{
			int i = cnDis; cnDis++;
			for (; i > 0; i--)
				if (disH._money < arrDis[i - 1]._money)
				{
					arrDis[i] = disH;
					nbHist = i;
					break;
				}
				else
					arrDis[i] = arrDis[i - 1];
			if (i == 0)
			{
				arrDis[0] = disH;
				nbHist = 0;
			}
		}
	}
	for (int i = 0; i < cnDis; i++)
	{
		sit0 = sit;
		sit0.ExecDis(arrDis[i]);
		int nb = ptrT->AddBranch(sit0);
		BuildTreeNLFromHistory(ptrT->Branch(nb), sit0, (i == nbHist) ? sitH : NULL, nbA + 1);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	BuildTreeNLPushFold(clHoldemTree *ptrT, clSitHoldem &sit, int nbA)
{
	int ind = sit.Indicator();
	if (ind == -1)
		return;
	tpDis arrDis[MAX_CN_BRANCH];
	int cnDis = 2;
	arrDis[0] = sit.CreateDis(HistCreateDis(ACT_ALL_IN));
	arrDis[1] = sit.CreateDis(HistCreateDis(ACT_FOLD));
	for (int i = 0; i < cnDis; i++)
	{
		clSitHoldem sit0 = sit;
		sit0.ExecDis(arrDis[i]);
		int nb = ptrT->AddBranch(sit0);
		BuildTreeNLPushFold(ptrT->Branch(nb), sit0, nbA + 1);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	HoldemTreeGetDisPushFold(clHoldemTree &tree, clSitHoldem &sit)
{
	int curAct = sit.NbFirstTradeAct(0);
	clHoldemTree *node = &tree;
	if (curAct != -1)
		node = tree.FindSitNodePushFold(sit.CnExecAct() - curAct, sit, curAct);
	if (node == NULL) return HistCreateDis(ACT_UNDEF);
	int nbCls = sit.PlayerHand(sit.Indicator()).NbHand();
	clCalcParam par = node->AtrTree()->CalcParam(nbCls);
	double w[MAX_CN_BRANCH];
	int cnB = node->CnBranch();
	par.GetWeightStrat(cnB, w);
	double r = randomFloat();
	for (int i = 0; i < cnB; i++)
		if (r < w[i])
			return node->Branch(i)->GetDis();
		else
			r -= w[i];
	return node->Branch(cnB - 1)->GetDis();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	CalcTreePushFold(clSitHoldem &sit, clHoldemTree *tree)
{
	if (sit.NameTrade() != TRADE_PREFLOP)
		return  false;
	int ind = sit.Indicator();
	if (sit.NbTradeActPl(ind, 0) != -1)
		return  false;
	clSitHoldem sit1 = sit;
	sit1.FirstSit();
	tree->ClearAllBranch();
	tree->GetSit() = sit1;
	int nb = 0;
	tree->SetNbNode(nb);
	BuildTreeNLPushFold(tree, sit1, sit.CnExecAct());
	int cnHand[] = { CN_HAND, CN_HAND };
	tree->SetStartParam(cnHand);
	tree->SetResSit();
	CalcNeshPreFlopPushFoldMultiThread(tree, 1000000);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	GetDisPushFold(clSitHoldem &sit)
{
	if (sit.NameTrade() != TRADE_PREFLOP)
		return  HistCreateDis(ACT_UNDEF);
	int ind = sit.Indicator();
	if (sit.NbTradeActPl(ind, 0) != -1)
		return  HistCreateDis(ACT_UNDEF);
	clHoldemTree tree;
	clSitHoldem sit1 = sit;
	sit1.FirstSit();
	tree.GetSit() = sit1;
	BuildTreeNLPushFold(&tree, sit1, sit.CnExecAct());
	int cnHand[] = { CN_HAND, CN_HAND };
	tree.SetStartParam(cnHand);
	tree.SetResSit();
	CalcNeshPreFlopPushFoldMultiThread(&tree, 1000000);
	return HoldemTreeGetDisPushFold(tree, sit);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
struct tpThreadNeshAtr
{
	int _cnCalc, _rand;
	clHoldemTree *_tree;
	bool	_stop, _end;
};
unsigned long _stdcall HTCalcNeshThread(void *ptr)
{
	tpThreadNeshAtr *atr = (tpThreadNeshAtr *)ptr;
	srand(atr->_rand);
	tpTreePlayerInfo info;
	tpBooleanDeck deck;
	for (int i0 = 0; i0 < atr->_cnCalc;)
	{
		if (atr->_stop)
			break;
		int r0 = random(ALL_CN_HAND);
		int r1 = random(ALL_CN_HAND);
		if (HandCrossHandNB(r0, r1))
			continue;
		info._nbCls[TRADE_PREFLOP][0] = HandAllGetNb169(r0);
		info._nbCls[TRADE_PREFLOP][1] = HandAllGetNb169(r1);
		tpFloat k0 = (i0 % 1000) ? 0 : 0.01;
		atr->_tree->CalcNesh(0, &info, 1, 1, k0);
		atr->_tree->CalcNesh(1, &info, 1, 1, k0);
		i0++;
	}
	atr->_end = true;
	return 0;
}

void	CalcNeshPreFlopPushFoldMultiThread(clHoldemTree *tree, int cnCalc)
{
	tpThreadNeshAtr atr[CN_THREAD];
	atr[0]._rand = time(NULL);
	for (int i = 0; i < CN_THREAD; i++)
	{
		atr[i]._cnCalc = cnCalc / CN_THREAD;
		atr[i]._tree = tree;
		atr[i]._stop = atr[i]._end = false;
		atr[i]._rand = atr[0]._rand + i;
		CreateThread(NULL, 0, &HTCalcNeshThread, &atr[i], 0, NULL);
	}
	int t0 = 0;
	while (true)
	{
		Sleep(100);
		int cnE = 0;
		for (; cnE < CN_THREAD; cnE++)
			if (!atr[cnE]._end)
				break;
		if (cnE == CN_THREAD)
			break;
		if (++t0 > 20)
			for (int i = 0; i < CN_THREAD; i++)
				atr[i]._stop = true;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	ChangeTree(clHoldemTree *nodeIn, clHoldemTree *node, double kSt, double mult)
{
	if (nodeIn->CnBranch() == 1)
		if (node->CnBranch() == 1)
			return ChangeTree(nodeIn->Branch(0), node->Branch(0), kSt, mult);
		else
			return false;
	if (nodeIn->CnBranch() >= 2 && node->CnBranch() > 0)
	{
		int cnCls = node->AtrTree()->CnClasters(); // число кластеров входного дерева должно быть не меньше чем у образца
		int cnBIn = nodeIn->CnBranch(), cnB = node->CnBranch();
		tpFloat *wAll = new tpFloat[cnCls*cnB];
		node->AtrTree()->GetAllWeightStrat(cnB, wAll);
		for (int b = 0; b < cnBIn; b++)
		{
			stNbBtw nbb = { -1,-1,0,0 };
			tpDis dis = nodeIn->Branch(b)->GetDis();
			dis._money /= kSt;
			node->FindBranchBetween(dis, nbb);
			if (nbb._nb0 == -1)
			{
				//clAnsiString as = (clAnsiString)"Не нашли ветви для " + HistDisToString(dis);
				//glTrasser.WriteMessage(as);
				continue;
				//return false;
			}
			if (nbb._nb1 == -1)
				for (int i = 0; i < cnCls; i++)
				{
					clCalcParam cpIn = nodeIn->AtrTree()->CalcParam(i);
					tpFloat part = wAll[i*cnB + nbb._nb0];
					cpIn.AddStratWeight(cnBIn, b, part*mult);
				}
			else
				for (int i = 0; i < cnCls; i++)
				{
					clCalcParam cpIn = nodeIn->AtrTree()->CalcParam(i);
					tpFloat part = wAll[i*cnB + nbb._nb0] * nbb._w0 + wAll[i*cnB + nbb._nb1] * nbb._w1;
					cpIn.AddStratWeight(cnBIn, b, part*mult);
				}
			bool res = ChangeTree(nodeIn->Branch(b), node->Branch(nbb._nb0), kSt, mult * nbb._w0);
			if (res == false) return false;
			if (nbb._nb1 != -1)
				res = ChangeTree(nodeIn->Branch(b), node->Branch(nbb._nb1), kSt, mult * nbb._w1);
			if (res == false) return false;
		}
		delete wAll;
	}
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	ChangeTreeGH(clHoldemTree *nodeIn, clHoldemTree *node, tpTableChangeNb *tab0) // деревья одинаковые;
{
	if (nodeIn->CnBranch() == 1)
		return ChangeTreeGH(nodeIn->Branch(0), node->Branch(0), tab0);
	if (nodeIn->CnBranch() == 0) return true;

	int ind = nodeIn->Indicator();
	tpTableChangeNb &tab = tab0[ind];

	int cnClsIn = nodeIn->AtrTree()->CnClasters(), cnCls = node->AtrTree()->CnClasters();
	int cnB = nodeIn->CnBranch();
	tpFloat *wAll = new tpFloat[cnCls*cnB];
	node->AtrTree()->GetAllWeightStrat(cnB, wAll);
	for (int b = 0; b < cnB; b++)
	{
		for (int i = 0; i < cnClsIn; i++)
		{
			stNbBtw chNb = tab._arr[i];
			clCalcParam cpIn = nodeIn->AtrTree()->CalcParam(i);
			tpFloat part = (chNb._nb1 == -1) ? wAll[chNb._nb0*cnB + b] : wAll[chNb._nb0*cnB + b] * chNb._w0 + wAll[chNb._nb1*cnB + b] * chNb._w1;
			cpIn.AddStratWeight(cnB, b, part);
		}

		bool res = ChangeTreeGH(nodeIn->Branch(b), node->Branch(b), tab0);
		if (res == false) return false;
	}
	delete wAll;
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	FillTreeFromTree(clHoldemTree *nodeIn, clHoldemTree *node, double kSt, tpTableChangeNb *tab, double mult)
{
	//if (glTime1 == 0)
	{
		clHoldemTree tree = *nodeIn;
		tree.ClearData();
		if (ChangeTree(&tree, node, kSt, mult))
			return ChangeTreeGH(nodeIn, &tree, tab);
		return false;
	}
#ifdef VIEW_PARAM
	tpCalcParam parIn, par0, par1;
#endif
	if (nodeIn->CnBranch() == 1 && node->CnBranch() == 1)
		return FillTreeFromTree(nodeIn->Branch(0), node->Branch(0), kSt, tab, mult);
	//return;
	if (nodeIn->CnBranch() >= 2 && node->CnBranch() > 0)
	{
		int ind = nodeIn->Indicator();
		int cnClsIn = nodeIn->AtrTree()->CnClasters();
		double *kP = new double[cnClsIn];		// выравнивающий коэффициент part
		double *kE = new double[cnClsIn];		// выравнивающий коэффициент ev
												//int cnCls0 = node->AtrTree()->CnClasters();
		int cnBIn = nodeIn->CnBranch(), cnB = node->CnBranch();
		double multEv = mult / kSt;
		for (int i = 0; i < cnClsIn; i++)
		{
			//glTrasser.WriteMessage(i);
			stNbBtw chNb = tab[ind]._arr[i];
			clCalcParam cpIn = nodeIn->AtrTree()->CalcParam(i);
			clCalcParam cp0 = node->AtrTree()->CalcParam(chNb._nb0);
#ifdef VIEW_PARAM
			parIn.LoadParam(cpIn, cnBIn);
			par0.LoadParam(cp0, cnB);
#endif
			cpIn.CnCalc() += cp0.CnCalc()*mult*chNb._w0 + 0.5;
			cpIn.WeightCalc() += cp0.WeightCalc()*mult*chNb._w0;
			cpIn.AverRes() += cp0.AverRes()*multEv*chNb._w0;
#ifdef VIEW_PARAM
			parIn.LoadParam(cpIn, cnBIn);
#endif
			if (chNb._nb1 != -1)
			{
				clCalcParam cp1 = node->AtrTree()->CalcParam(chNb._nb1);
				double sum = cp1.SumStratWeight(cnB);
				if (sum > DOUBLE_0)
					kP[i] = cp0.SumStratWeight(cnB) / sum;
				else
					kP[i] = 0;
				sum = cp1.WeightCalc();
				if (sum > DOUBLE_0)
					kE[i] = cp0.WeightCalc() / sum;
				else
					kE[i] = 0;
#ifdef VIEW_PARAM
				par1.LoadParam(cp1, cnB);
#endif
				cpIn.CnCalc() += cp1.CnCalc()*mult*chNb._w1 + 0.5;
				cpIn.WeightCalc() += cp1.WeightCalc()*mult*chNb._w1*kE[i];
				cpIn.AverRes() += cp1.AverRes()*multEv*chNb._w1*kE[i];
#ifdef VIEW_PARAM
				parIn.LoadParam(cpIn, cnBIn);
#endif
			}
			else
			{
				kP[i] = 0;
				kE[i] = 0;
			}
		}

		for (int b = 0; b < cnBIn; b++)
		{
			stNbBtw nbb = { -1,-1,0,0 };
			tpDis dis = nodeIn->Branch(b)->GetDis();
			dis._money /= kSt;
			node->FindBranchBetween(dis, nbb);
			if (nbb._nb0 == -1) return false;
			if (nbb._w1 < 0.1)
			{
				nbb._nb1 = -1;
				nbb._w0 = 1;
			}
			double kEv = multEv*nbb._w0, kPart = mult*nbb._w0;
			for (int i = 0; i < cnClsIn; i++)
			{
				stNbBtw chNb = tab[ind]._arr[i];
				clCalcParam cpIn = nodeIn->AtrTree()->CalcParam(i);
				clCalcParam cp = node->AtrTree()->CalcParam(chNb._nb0);
#ifdef VIEW_PARAM
				parIn.LoadParam(cpIn, cnBIn);
				par0.LoadParam(cp, cnB);
#endif
				cpIn.BeginEV()[b] += cp.BeginEV()[nbb._nb0] * kEv*chNb._w0;
				cpIn.BeginPart(cnBIn)[b] += cp.BeginPart(cnB)[nbb._nb0] * kPart*chNb._w0;
#ifdef VIEW_PARAM
				parIn.LoadParam(cpIn, cnBIn);
#endif
				if (chNb._nb1 != -1)
				{
					clCalcParam cp = node->AtrTree()->CalcParam(chNb._nb1);
#ifdef VIEW_PARAM
					par1.LoadParam(cp, cnB);
#endif
					cpIn.BeginEV()[b] += cp.BeginEV()[nbb._nb0] * kEv*chNb._w1*kE[i];
					cpIn.BeginPart(cnBIn)[b] += cp.BeginPart(cnB)[nbb._nb0] * kPart*chNb._w1*kP[i];
#ifdef VIEW_PARAM
					parIn.LoadParam(cpIn, cnBIn);
#endif
				}
			}
			FillTreeFromTree(nodeIn->Branch(b), node->Branch(nbb._nb0), kSt, tab, mult*nbb._w0);
			if (nbb._nb1 != -1)
			{
				double kEv = multEv*nbb._w1, kPart = mult*nbb._w1;
				for (int i = 0; i < cnClsIn; i++)
				{
					stNbBtw chNb = tab[ind]._arr[i];
					clCalcParam cpIn = nodeIn->AtrTree()->CalcParam(i);
					clCalcParam cp = node->AtrTree()->CalcParam(chNb._nb0);
#ifdef VIEW_PARAM
					parIn.LoadParam(cpIn, cnBIn);
					par0.LoadParam(cp, cnB);
#endif
					cpIn.BeginEV()[b] += cp.BeginEV()[nbb._nb1] * kEv*chNb._w0;
					cpIn.BeginPart(cnBIn)[b] += cp.BeginPart(cnB)[nbb._nb1] * kPart*chNb._w0;
#ifdef VIEW_PARAM
					parIn.LoadParam(cpIn, cnBIn);
#endif
					if (chNb._nb1 != -1)
					{
						clCalcParam cp = node->AtrTree()->CalcParam(chNb._nb1);
#ifdef VIEW_PARAM
						par1.LoadParam(cp, cnB);
#endif
						cpIn.BeginEV()[b] += cp.BeginEV()[nbb._nb1] * kEv*chNb._w1*kE[i];
						cpIn.BeginPart(cnBIn)[b] += cp.BeginPart(cnB)[nbb._nb1] * kPart*chNb._w1*kP[i];
#ifdef VIEW_PARAM
						parIn.LoadParam(cpIn, cnBIn);
#endif
					}
				}
				FillTreeFromTree(nodeIn->Branch(b), node->Branch(nbb._nb1), kSt, tab, mult*nbb._w1);
			}
		}
		delete kE;
		delete kP;
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
