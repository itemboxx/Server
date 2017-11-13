#include <math.h>
#include <time.h>

#include "c:\home\card\naborCards.h"
#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\TurnNode.h"
#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\TurnTree.h"
#include "C:\home\Holdem\Strategy\AppRiverTrees\Source\clRiverBanks.h"
#include "C:\home\Holdem\Strategy\AppTurnTree\Source\TreeBanks.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clStreetNode::SizeInFile()
{
	size_t size = clInComeDataRoot::SizeInFile();
	size += SizeVectFile(_ev[0]);
	size += SizeVectFile(_ev[1]);
	size += SizeVectFile(_ev[2]);
	return size;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
int 	clStreetNode::WriteFile(int handle)
{
	clInComeDataRoot::WriteFile(handle);
	WriteVectFile(handle, _ev[0]);
	WriteVectFile(handle, _ev[1]);
	WriteVectFile(handle, _ev[2]);
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
int 	clStreetNode::ReadFile(int handle)
{
	clInComeDataRoot::ReadFile(handle);
	ReadVectFile(handle, _ev[0]);
	ReadVectFile(handle, _ev[1]);
	ReadVectFile(handle, _ev[2]);
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcKHand(clRoot *root, vector <float> *kHand)
{
	ZeroMemory(&kHand[0][0], kHand[0].size() * sizeof(float));
	ZeroMemory(&kHand[1][0], kHand[1].size() * sizeof(float));
	int dim0 = root->CnHand(0), dim1 = root->CnHand(1);
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = root->NbHand(0, i0);
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = root->NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
			{
				kHand[0][nb0] += root->_gh[1][nb1];
				kHand[1][nb1] += root->_gh[0][nb0];
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStreetNode::CalcEV(void * ptrUt)
{
	if (_sit.NameTrade() == TRADE_TURN)
		CalcEVTurn(ptrUt);
	else
		CalcEVFlop(ptrUt);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStreetNode::CalcEVTurn(void * ptrUt)
{
	clRootRiverTree rr;
	rr._sit = _sit;
	rr._sit.ExecAct(HistAction(NB_DILER, ACT_DELRIVER, 0));
	vector <float> evCur[2] = { vector <float>(ALL_CN_HAND),vector <float>(ALL_CN_HAND) };
	vector <float> kHand[2] = { vector <float>(ALL_CN_HAND),vector <float>(ALL_CN_HAND) };
	_ev[0].resize(ALL_CN_HAND);
	_ev[1].resize(ALL_CN_HAND);
	ZeroMemory(&_ev[0][0], ALL_CN_HAND * sizeof(float));
	ZeroMemory(&_ev[1][0], ALL_CN_HAND * sizeof(float));

	float w0[2][ALL_CN_HAND];
	ZeroMemory(w0, sizeof(w0));

	int cnCard = 0;
	tpBooleanDeck deck;
	for (int i = 0; i < _sit.CnCardTable(); i++)
		deck.Skip(_sit.CardsTable()[i].Number());
	for (int k = 0; k < CN_CARD_COL; k++)
		if (deck[k])
		{
			//glTrasser.WriteMessage(k);
			//cnCard++;
			rr._gh[0] = _gh[0];
			rr._gh[1] = _gh[1];
			rr._gh[0].SkipPreFlopAllHands(k);
			rr._gh[1].SkipPreFlopAllHands(k);

			rr.CalcIndex();
			CalcKHand(&rr, kHand);

			rr._gh[0].Norm();
			rr._gh[1].Norm();
			rr._sit.CardsTable()[4] = CardNB(k);
			if (rr.CnHand(0) == 0 || rr.CnHand(1) == 0)
				continue;
			rr.CalcForce();

			glTime2 = k;

			int st0 = glBankRiver.FillRiverEV(&rr, evCur, (tpUnfindRiverTree *)ptrUt);
			tpFloat f0 = (tpFloat)_sit.PlMoney(0) / st0;
			for (int i = 0; i < rr.CnHand(0); i++)
			{
				int nb = rr.NbHand(0, i);
				_ev[0][nb] += evCur[0][nb] * kHand[0][nb] * f0;
				w0[0][nb] += kHand[0][nb];
			}
			for (int i = 0; i < rr.CnHand(1); i++)
			{
				int nb = rr.NbHand(1, i);
				_ev[1][nb] += evCur[1][nb] * kHand[1][nb] * f0;
				w0[1][nb] += kHand[1][nb];
			}
		}
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		if (w0[0][i] > DOUBLE_0)
			_ev[0][i] /= w0[0][i];
		if (w0[1][i] > DOUBLE_0)
			_ev[1][i] /= w0[1][i];
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clStreetNode::CalcEVFlop(void *ptrUt)
{
	clRootStreetTree rr;
	rr._sit = _sit;
	rr._sit.ExecAct(HistAction(NB_DILER, ACT_DELTURN, 0));
	vector <float> evCur[2] = { vector <float>(ALL_CN_HAND),vector <float>(ALL_CN_HAND) };
	vector <float> kHand[2] = { vector <float>(ALL_CN_HAND),vector <float>(ALL_CN_HAND) };
	_ev[0].resize(ALL_CN_HAND);
	_ev[1].resize(ALL_CN_HAND);
	ZeroMemory(&_ev[0][0], ALL_CN_HAND * sizeof(float));
	ZeroMemory(&_ev[1][0], ALL_CN_HAND * sizeof(float));

	float w0[2][ALL_CN_HAND];
	ZeroMemory(w0, sizeof(w0));

	tpBooleanDeck deck;
	for (int i = 0; i < _sit.CnCardTable(); i++)
		deck.Skip(_sit.CardsTable()[i].Number());
	for (int k = 0; k < CN_CARD_COL; k++)
		if (deck[k])
		{
			//glTrasser.WriteMessage(k);
			rr._gh[0] = _gh[0];
			rr._gh[1] = _gh[1];
			rr._gh[0].SkipPreFlopAllHands(k);
			rr._gh[1].SkipPreFlopAllHands(k);

			rr.CalcIndex();
			CalcKHand(&rr, kHand);

			rr._gh[0].Norm();
			rr._gh[1].Norm();
			rr._sit.CardsTable()[3] = CardNB(k);
			if (rr.CnHand(0) == 0 || rr.CnHand(1) == 0)
				continue;
			rr.CalcForce();

			glTime2 = k;

			int st0 = glBankTurn.FillEV(&rr, evCur, (tpUnfindTree *)ptrUt);
			tpFloat f0 = (tpFloat)_sit.PlMoney(0) / st0;
			for (int i = 0; i < rr.CnHand(0); i++)
			{
				int nb = rr.NbHand(0, i);
				_ev[0][nb] += evCur[0][nb] * kHand[0][nb] * f0;
				w0[0][nb] += kHand[0][nb];
			}
			for (int i = 0; i < rr.CnHand(1); i++)
			{
				int nb = rr.NbHand(1, i);
				_ev[1][nb] += evCur[1][nb] * kHand[1][nb] * f0;
				w0[1][nb] += kHand[1][nb];
			}
		}
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		if (w0[0][i] > DOUBLE_0)
			_ev[0][i] /= w0[0][i];
		if (w0[1][i] > DOUBLE_0)
			_ev[1][i] /= w0[1][i];
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
