#include <math.h>
#include <time.h>

#include "c:\home\card\naborCards.h"
#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\TurnTree.h"
#include "c:\home\Util\TemplateFileRec.h"
#include "c:\Home\Util\Sort.h"
#include "C:\home\Holdem\Common\HoldemTree.h"
#include "C:\home\holdem\Common\FlopAllIn.h"
#include "c:\home\util\Clasters.h"
#include "C:\home\Holdem\Strategy\AppRiverTrees\Source\clRiverBanks.h"
#include "c:\Home\Util\Matrix.h"
#include "c:\Home\Util\AnyType.h"
#include "c:\Home\Util\AnyTemplate.h"

//bool glTurnDatNew = true;

class clTurnDatDat
{
public:
	void	Init10();
	void	Init16();
	void	Init20();
	int GetNbHand(int pos, int i) { return _nbHand[pos][i]; }
	int &Force(int nbPl, int k, int nbH) { return _force[nbPl][nbH*CN_CARD_COL + k]; }
	clMatrixCub <tp2Int> _matrix;

	int _nbHand[2][CN_STREETDAT_UNIT];
	clCard _board[5];
	tpBooleanDeck _deck;
	int _force[2][CN_CARD_COL*CN_STREETDAT_UNIT];
} glTDD;

//---------------------------------------------------------------------------------------------------------------------------------------------------
void	InitTurnDatDat()
{
	if (CN_STREETDAT_UNIT == 10)
		glTDD.Init10();
	else if (CN_STREETDAT_UNIT == 16)
		glTDD.Init16();
	else if (CN_STREETDAT_UNIT == 20)
		glTDD.Init20();
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
int *GetNbHandTurnDatDat(int nb) { return glTDD._nbHand[nb]; }
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clTurnDatDat::Init10()
{
	_board[0] = Card(C_5, S_C);
	_board[1] = Card(C_6, S_C);
	_board[2] = Card(C_8, S_S);
	_board[3] = Card(C_T, S_S);
	_deck.SetDeck(true);
	_deck.Skip(_board[0]);
	_deck.Skip(_board[1]);
	_deck.Skip(_board[2]);
	_deck.Skip(_board[3]);
	clHand hand[CN_STREETDAT_UNIT];
	hand[0].SetHand(Card(C_9, S_D), Card(C_7, S_D));
	hand[1].SetHand(Card(C_T, S_D), Card(C_5, S_D));
	hand[2].SetHand(Card(C_T, S_D), Card(C_7, S_D));
	hand[3].SetHand(Card(C_T, S_D), Card(C_A, S_D));
	hand[4].SetHand(Card(C_K, S_D), Card(C_8, S_D));
	hand[5].SetHand(Card(C_J, S_D), Card(C_9, S_D));
	hand[6].SetHand(Card(C_K, S_D), Card(C_4, S_D));
	hand[7].SetHand(Card(C_Q, S_D), Card(C_Q, S_C));
	hand[8].SetHand(Card(C_3, S_D), Card(C_3, S_C));
	hand[9].SetHand(Card(C_K, S_D), Card(C_2, S_D));
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		_nbHand[0][i] = hand[i].NbHandAll();
		hand[i][0].suit = S_H;
		hand[i][1].suit = (hand[i][0].face == hand[i][1].face) ? S_S : S_H;
		_nbHand[1][i] = hand[i].NbHandAll();
	}

	ZeroMemory(_force, sizeof(_force));

	tpBooleanDeck deck = _deck;
	tpCard cards[5] = { _board[0], _board[1], _board[2], _board[3] };
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		int nbH = GetNbHand(0, i);
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				cards[4] = k;
				int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
				Force(0, k, i) = glVectRiverHandGame[nbR].ToInt();
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		int nbH = GetNbHand(1, i);
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				cards[4] = k;
				int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
				Force(1, k, i) = glVectRiverHandGame[nbR].ToInt();
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
	_matrix.SetDim(CN_STREETDAT_UNIT, CN_STREETDAT_UNIT, CN_CARD_COL);
	_matrix.ZeroData();
	for (int i0 = 0; i0 < CN_STREETDAT_UNIT; i0++)
	{
		int nb0 = GetNbHand(0, i0);
		tpHand hand0(nb0);
		deck[hand0[0]] = false;
		deck[hand0[1]] = false;
		for (int i1 = 0; i1 < CN_STREETDAT_UNIT; i1++)
		{
			int nb1 = GetNbHand(1, i1);
			tpHand hand1(nb1);
			deck[hand1[0]] = false;
			deck[hand1[1]] = false;
			tp2Int *v0 = _matrix.Array(i0, i1);
			for (int k = 0; k < CN_CARD_COL; k++)
				if (deck[k])
				{
					v0[k]._val2 = 1;
					int f0 = Force(0, k, i0), f1 = Force(1, k, i1);
					if (f0 > f1)
						v0[k]._val1 = 1;
					else if (f1 > f0)
						v0[k]._val1 = -1;
					else
						v0[k]._val1 = 0;
				}
			deck[hand1[0]] = true;
			deck[hand1[1]] = true;
		}
		deck[hand0[0]] = true;
		deck[hand0[1]] = true;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clTurnDatDat::Init16()
{
	//glTrasser.WriteMessage((clAnsiString)"A");
	_board[0] = Card(C_5, S_C);
	_board[1] = Card(C_6, S_C);
	_board[2] = Card(C_8, S_S);
	_board[3] = Card(C_T, S_S);
	_deck.SetDeck(true);
	_deck.Skip(_board[0]);
	_deck.Skip(_board[1]);
	_deck.Skip(_board[2]);
	_deck.Skip(_board[3]);
	clHand hand[CN_STREETDAT_UNIT];
	hand[0].SetHand(Card(C_9, S_D), Card(C_7, S_D));
	hand[1].SetHand(Card(C_4, S_D), Card(C_7, S_D));
	hand[2].SetHand(Card(C_T, S_D), Card(C_8, S_D));
	hand[3].SetHand(Card(C_T, S_D), Card(C_7, S_D));
	hand[4].SetHand(Card(C_5, S_D), Card(C_7, S_D));
	hand[5].SetHand(Card(C_T, S_D), Card(C_A, S_D));
	hand[6].SetHand(Card(C_K, S_D), Card(C_8, S_D));
	hand[7].SetHand(Card(C_Q, S_D), Card(C_6, S_D));
	hand[8].SetHand(Card(C_J, S_D), Card(C_9, S_D));
	hand[9].SetHand(Card(C_J, S_D), Card(C_7, S_D));
	hand[10].SetHand(Card(C_Q, S_D), Card(C_3, S_D));
	hand[11].SetHand(Card(C_K, S_D), Card(C_4, S_D));
	hand[12].SetHand(Card(C_Q, S_D), Card(C_Q, S_C));
	hand[13].SetHand(Card(C_3, S_D), Card(C_3, S_C));
	hand[14].SetHand(Card(C_A, S_D), Card(C_Q, S_D));
	hand[15].SetHand(Card(C_K, S_D), Card(C_2, S_D));
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		_nbHand[0][i] = hand[i].NbHandAll();
		hand[i][0].suit = S_H;
		hand[i][1].suit = (hand[i][0].face == hand[i][1].face) ? S_S : S_H;
		_nbHand[1][i] = hand[i].NbHandAll();
	}

	ZeroMemory(_force, sizeof(_force));

	tpBooleanDeck deck = _deck;
	tpCard cards[5] = { _board[0], _board[1], _board[2], _board[3] };
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		int nbH = GetNbHand(0, i);
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				cards[4] = k;
				int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
				Force(0, k, i) = glVectRiverHandGame[nbR].ToInt();
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		int nbH = GetNbHand(1, i);
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				cards[4] = k;
				int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
				Force(1, k, i) = glVectRiverHandGame[nbR].ToInt();
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clTurnDatDat::Init20()
{
	//glTrasser.WriteMessage((clAnsiString)"A");
	_board[0] = Card(C_5, S_C);
	_board[1] = Card(C_7, S_C);
	_board[2] = Card(C_7, S_S);
	_board[3] = Card(C_9, S_S);
	_deck.SetDeck(true);
	_deck.Skip(_board[0]);
	_deck.Skip(_board[1]);
	_deck.Skip(_board[2]);
	_deck.Skip(_board[3]);
	clHand hand[CN_STREETDAT_UNIT];
	hand[0].SetHand(Card(C_9, S_D), Card(C_7, S_D));
	hand[1].SetHand(Card(C_8, S_D), Card(C_6, S_D));
	hand[2].SetHand(Card(C_A, S_D), Card(C_7, S_D));
	hand[3].SetHand(Card(C_8, S_D), Card(C_7, S_D));
	hand[4].SetHand(Card(C_2, S_D), Card(C_7, S_D));
	hand[5].SetHand(Card(C_K, S_D), Card(C_9, S_D));
	hand[6].SetHand(Card(C_9, S_D), Card(C_3, S_D));
	hand[7].SetHand(Card(C_Q, S_D), Card(C_5, S_D));
	hand[8].SetHand(Card(C_5, S_D), Card(C_4, S_D));
	hand[9].SetHand(Card(C_K, S_D), Card(C_K, S_C));
	hand[10].SetHand(Card(C_4, S_D), Card(C_4, S_C));
	hand[11].SetHand(Card(C_T, S_D), Card(C_8, S_D));
	hand[12].SetHand(Card(C_6, S_D), Card(C_4, S_D));
	hand[13].SetHand(Card(C_A, S_D), Card(C_8, S_D));
	hand[14].SetHand(Card(C_K, S_D), Card(C_6, S_D));
	hand[15].SetHand(Card(C_J, S_D), Card(C_T, S_D));
	hand[16].SetHand(Card(C_K, S_D), Card(C_A, S_D));
	hand[17].SetHand(Card(C_A, S_D), Card(C_T, S_D));
	hand[18].SetHand(Card(C_Q, S_D), Card(C_4, S_D));
	hand[19].SetHand(Card(C_3, S_D), Card(C_2, S_D));
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		_nbHand[0][i] = hand[i].NbHandAll();
		hand[i][0].suit = S_H;
		hand[i][1].suit = (hand[i][0].face == hand[i][1].face) ? S_S : S_H;
		_nbHand[1][i] = hand[i].NbHandAll();
	}

	ZeroMemory(_force, sizeof(_force));

	tpBooleanDeck deck = _deck;
	tpCard cards[5] = { _board[0], _board[1], _board[2], _board[3] };
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		int nbH = GetNbHand(0, i);
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				cards[4] = k;
				int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
				Force(0, k, i) = glVectRiverHandGame[nbR].ToInt();
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		int nbH = GetNbHand(1, i);
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				cards[4] = k;
				int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
				Force(1, k, i) = glVectRiverHandGame[nbR].ToInt();
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	RecalcStreetDat(clStreetDat &datQ, clStreetDat &dat100)
{
	clStreetDat dat;
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		vector <tpStreetDatGH> &vectQ = datQ._vectUnit[nbPl];
		vector <tpStreetDatGH> &vect100 = dat100._vectUnit[nbPl];
		vector <tpStreetDatGH> &vect = dat._vectUnit[nbPl];
		int dim = vectQ.size();
		int dim100 = vect100.size();
		vect.resize(dim);
		int nbR[ALL_CN_HAND];
		memset(nbR, -1, sizeof(nbR));
		for (int i = 0; i < dim100; i++)
			nbR[vect100[i]._nb] = i;
		for (int i = 0; i < dim; i++)
		{
			int nb = vectQ[i]._nb;
			if (nbR[nb] != -1)
				vect[i] = vect100[nbR[nb]];
			else
				vect[i]._nb = -1;
		}
		int nbF = 0;
		for (; nbF < dim; nbF++)
			if (vect[nbF]._nb != -1)
				break;
		for (int i = 0; i < nbF; i++)
		{
			vect[i]._force = vect[nbF]._force;
			vect[i]._force50 = vect[nbF]._force50;
			vect[i]._nb = vectQ[i]._nb;
			vect[i]._weight = vectQ[i]._weight;
		}
		int nbL = dim-1;
		for (; nbF >= 0; nbF--)
			if (vect[nbF]._nb != -1)
				break;
		for (int i = dim-1; i > nbL; i--)
		{
			vect[i]._force = vect[nbL]._force;
			vect[i]._force50 = vect[nbL]._force50;
			vect[i]._nb = vectQ[i]._nb;
			vect[i]._weight = vectQ[i]._weight;
		}
		int j = nbF+1;
		while (j < nbL)
		{
			for (; j <= nbL; j++)
				if (vect[j]._nb != -1)
					break;
			for (int i = nbF + 1; i < j; i++)
			{
				float dF = vectQ[nbF].Distance(vectQ[i]);
				float dL = vectQ[j].Distance(vectQ[i]);
				float sum = dF + dL;
				if (sum > DOUBLE_0)
				{
					float kF = dL / sum;
					float kL = dF / sum;
					vect[i]._force = vect[nbF]._force*kF + vect[j]._force*kL;
					vect[i]._force50 = vect[nbF]._force50*kF + vect[j]._force50*kL;
					vect[i]._nb = vectQ[i]._nb;
					vect[i]._weight = vectQ[i]._weight;
				}
			}
			nbF = j++;
		}
	}
	dat.Copy50AndSort();
	datQ = dat;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
bool	clRootStreetTree::CreateTurnDatQuick(clStreetDat &dat)
{
	int dim0 = CnHand(0), dim1 = CnHand(1);
	dat._vectUnit[0].resize(dim0);
	dat._vectUnit[1].resize(dim1);
	for (int i = 0; i < dim0; i++)
	{
		glTrasser.WriteMessage(i);
		int nb = NbHand(0, i);
		tpHand hand(nb);
		int nbF = GetTurnHandIndex(_cards, hand);
		dat._vectUnit[0][i] = { nb,0,glVectTurnEV100[nbF],glVectTurnEV50[nbF] };
	}
	for (int i = 0; i < dim1; i++)
	{
		int nb = NbHand(1, i);
		tpHand hand(nb);
		int nbF = GetTurnHandIndex(_cards, hand);
		dat._vectUnit[1][i] = { nb,0,glVectTurnEV100[nbF],glVectTurnEV50[nbF] };
	}
	float w = 0;
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
			{
				float ww = _gh[0][nb0] * _gh[1][nb1];
				dat._vectUnit[0][i0]._weight += ww;
				dat._vectUnit[1][i1]._weight += ww;
				w += ww;
			}
		}
	}
	for (int i = 0; i < dim0; i++)
		dat._vectUnit[0][i]._weight /= w;
	for (int i = 0; i < dim1; i++)
		dat._vectUnit[1][i]._weight /= w;

	dat.Copy50AndSort();

	clRootStreetTree rr;
	rr._sit = _sit;
	rr._gh[0] = _gh[0];
	rr._gh[1] = _gh[1];
	rr.CompactGH(0.01, 100, dat);
	clStreetDat dd;
	if (!rr.CreateStreetDat(dd))
		return false;
	RecalcStreetDat(dat, dd);

	dat._k = 1;
	dat._stack[0] = _sit.PlayerMoney(0);
	dat._stack[1] = _sit.PlayerMoney(1);
	dat._pot = _sit.TotalPot();
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef TURN_FULLTREE
#else
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int 	clRootStreetTree::ReadFileAndPrepare(int handle)
{
	clRoot::ReadFile(handle);
	CalcAll();
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void clRootStreetTree::operator = (clRootStreetTree &root)
{
	*((clRoot *)this) = *((clRoot*)&root);
	_force = root._force;
	_prAllIn = root._prAllIn;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CreateTreeNL()
{
	_tree.ClearAllBranch();
	_tree.GetSit() = _sit;
	BuildTreeNLAuto(&_tree, _sit, _sit.NameTrade());

	CalcAll();
	int arrCls[] = { CnHand(0),CnHand(1) };
	_tree.SetStartParam(arrCls);
	_tree.SetResSit();
#ifdef NB_NODE
	int nb = 0;
	int cnNode = _tree.SetNbNode(nb);
#endif
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CutTreeS(clHoldemTree *node)
{
	if (node->Indicator() == -1 && node->CnBranch() == 1)
		node->ClearAllBranch();
	else
		for (int i = 0; i < node->CnBranch(); i++)
			CutTreeS(node->Branch(i));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CutTree()
{
	CutTreeS(&_tree);
	_tree.SetResSit();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clRootStreetTree::CalcForceFlop()
{
	_force.resize(ALL_CN_HAND*CN_CARD_COL*CN_CARD_COL);
	ZeroMemory(&_force[0], ALL_CN_HAND*CN_CARD_COL*CN_CARD_COL * sizeof(int));
	tpBooleanDeck deck;
	tpCard cards[5];
	for (int i = 0; i < 3; i++)
	{
		cards[i] = _sit.CardsTable()[i];
		deck.Skip(cards[i]);
	}

	bool calcH[ALL_CN_HAND];
	ZeroMemory(calcH, sizeof(calcH));
	int dim = CnHand(0);
	for (int i = 0; i < dim; i++)
	{
		int nbH = NbHand(0, i);
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				cards[3] = k;
				for (int k1 = k + 1; k1 < CN_CARD_COL; k1++)
					if (deck[k1])
					{
						cards[4] = k1;
						int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
						int f = glVectRiverHandGame[nbR].ToInt();
						Force(k, k1, nbH) = f;
						Force(k1, k, nbH) = f;
					}
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
		calcH[nbH] = true;
	}
	dim = CnHand(1);
	for (int i = 0; i < dim; i++)
	{
		int nbH = NbHand(1, i);
		if (calcH[nbH]) continue;
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				cards[3] = k;
				for (int k1 = k + 1; k1 < CN_CARD_COL; k1++)
					if (deck[k1])
					{
						cards[4] = k1;
						int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
						int f = glVectRiverHandGame[nbR].ToInt();
						Force(k, k1, nbH) = f;
						Force(k1, k, nbH) = f;
					}
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CalcAllIn()
{
	if (_sit.NameTrade() == TRADE_TURN)
		CalcAllInTurn();
	//else
		//CalcAllInFlop();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CalcAllInTurn()
{
	if (_force.size() == 0)
		CalcForceTurn(_force);
	tpBooleanDeck deck;
	tpCard cards[5];
	int cnRiver = 44;
	for (int i = 0; i < 4; i++)
	{
		cards[i] = _sit.CardsTable()[i];
		deck.Skip(cards[i]);
	}
	int dim0 = CnHand(0), dim1 = CnHand(1);
	_prAllIn.SetDim(dim0, dim1);
	_prAllIn.ZeroData();
	for (int k0 = 0; k0 < dim0; k0++)
	{
		int nb0 = NbHand(0, k0);
		tpHand hand0(nb0);
		deck[hand0[0]] = false;
		deck[hand0[1]] = false;
		for (int k1 = 0; k1 < dim1; k1++)
		{
			int nb1 = NbHand(1, k1);
			if (HandCrossHandNB(nb0, nb1)) continue;
			tpHand hand1(nb1);
			deck[hand1[0]] = false;
			deck[hand1[1]] = false;
			//tpFloat val = _gh[0][nb0] * _gh[1][nb1];
			tpFloat ev = 0;
			for (int k = 0; k < CN_CARD_COL; k++)
				if (deck[k])
				{
					if (Force(k, nb0) > Force(k, nb1))
						ev += 1;
					else if (Force(k, nb0) == Force(k, nb1))
						ev += 0.5;

				}
			_prAllIn.Element(k0, k1, 0) = ev / cnRiver;
			deck[hand1[0]] = true;
			deck[hand1[1]] = true;
		}
		deck[hand0[0]] = true;
		deck[hand0[1]] = true;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clRootStreetTree::CalcAllInFlop()
{
	if (_force.size() == 0)
		CalcForceFlop();
	InitDeck();
	int dim0 = CnHand(0), dim1 = CnHand(1);
	_prAllIn.SetDim(dim0, dim1);
	_prAllIn.ZeroData();
	for (int k0 = 0; k0 < dim0; k0++)
	{
		int nb0 = NbHand(0, k0);
		tpHand hand0(nb0);
		for (int k1 = 0; k1 < dim1; k1++)
		{
			int nb1 = NbHand(1, k1);
			if (HandCrossHandNB(nb0, nb1)) continue;
			tpHand hand1(nb1);
			_prAllIn.Element(k0, k1, 0) = FlopAllInResult(nb0, nb1, _cards);
		}
	}

}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CalcMultiHandsTree(size_t cnCalc)
{
	vector <tpFloat> gh[2];
	vector <float> ev[2];
	CreateCompactGH(gh);
	ev[0].resize(CnHand(0));
	ev[1].resize(CnHand(1));

	for (int i = 0; i < cnCalc; i++)
	{
		//glTrasser.WriteMessage(i);
		CalcNeshMultiHandsTree(&_tree, 0, gh[0], gh[1], ev[0]);
		CalcNeshMultiHandsTree(&_tree, 1, gh[1], gh[0], ev[1]);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CalcEVMultiHands(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev)
{
	int ind = node->Indicator();
	if (node->EndedNode())
		return ResultMultiHandsTree(node, nbHero, kH, kOpp, ev);

	int dim = node->AtrTree()->CnClasters();
	int cnB = node->CnBranch();
	tpFloat *w = new tpFloat[cnB*dim];
	ZeroMemory(w, cnB*dim * sizeof(tpFloat));
	ZeroMemory(&ev[0], ev.size() * sizeof(float));
	if (ind == nbHero)
	{
		node->AtrTree()->GetAllWeightStrat(cnB, w);
		vector <float> evCur[MAX_CN_BRANCH];
		vector <tpFloat> kCur;
		for (int k = 0; k < cnB; k++)
		{
			kCur = kH;
			evCur[k].resize(dim);
			for (int i = 0; i < dim; i++)
				kCur[i] *= w[i*cnB + k];
			CalcEVMultiHands(node->Branch(k), nbHero, kCur, kOpp, evCur[k]);
			for (int i = 0; i < dim; i++)
				ev[i] += evCur[k][i] * w[i*cnB + k];
		}
	}
	else
	{
		int dimEV = ev.size();
		node->AtrTree()->GetAllWeightStrat(cnB, w);
		vector <float> evCur(dimEV);
		vector <tpFloat> kCur;
		for (int k = 0; k < cnB; k++)
		{
			kCur = kOpp;
			for (int i = 0; i < dim; i++)
				kCur[i] *= w[i*cnB + k];
			CalcEVMultiHands(node->Branch(k), nbHero, kH, kCur, evCur);
			for (int i = 0; i < dimEV; i++)
				ev[i] += evCur[i];
		}
	}
	delete w;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CalcEvTree(vector <float> *evIn, tpUnfindTree *ut)
{
	evIn[0].resize(ALL_CN_HAND);
	evIn[1].resize(ALL_CN_HAND);
	ZeroMemory(&evIn[0][0], ALL_CN_HAND * sizeof(float));
	ZeroMemory(&evIn[1][0], ALL_CN_HAND * sizeof(float));

	vector <tpFloat> gh[2];
	vector <float> ev[2];
	CreateCompactGH(gh);
	int dim0 = CnHand(0), dim1 = CnHand(1);
	ev[0].resize(dim0);
	ev[1].resize(dim1);

	CalcEVMultiHands(&_tree, 0, gh[0], gh[1], ev[0]);
	CalcEVMultiHands(&_tree, 1, gh[1], gh[0], ev[1]);

	for (int i = 0; i < dim0; i++)
	{
		int nbH = NbHand(0, i);
		tpFloat k = 0;
		for (int i1 = 0; i1 < dim1; i1++)
			if (!HandCrossHandNB(nbH, NbHand(1, i1)))
				k += gh[1][i1];
		if (k > DOUBLE_0)
			evIn[0][nbH] = ev[0][i] / k;
	}
	for (int i = 0; i < dim1; i++)
	{
		int nbH = NbHand(1, i);
		tpFloat k = 0;
		for (int i0 = 0; i0 < dim0; i0++)
			if (!HandCrossHandNB(nbH, NbHand(0, i0)))
				k += gh[0][i0];
		if (k > DOUBLE_0)
			evIn[1][nbH] = ev[1][i] / k;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clRootStreetTree::ResultMultiHands(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev, tpUnfindTree *ut)
{
	int *ptr = (int *)&node->AtrTree()->_param[0];
	int opp = (nbHero == 0) ? 1 : 0;
	int dimH = CnHand(nbHero);
	int dimOpp = CnHand(opp);
	ZeroMemory(&ev[0], dimH * sizeof(float));
	if (node->GetSit().CNPlayerIsCard() == 1)		//Fold
	{
		for (int i0 = 0; i0 < dimH; i0++)
		{
			int nbH = NbHand(nbHero, i0);
			for (int i1 = 0; i1 < dimOpp; i1++)
				if (!HandCrossHandNB(nbH, NbHand(opp, i1)))
					ev[i0] += ptr[nbHero] * kOpp[i1];
		}
	}
	else if (node->GetSit().CNPlayerInTrade() < 2)	//AllIn
	{
		if (nbHero == 0)
			for (int i0 = 0; i0 < dimH; i0++)
			{
				int nbH = NbHand(nbHero, i0);
				for (int i1 = 0; i1 < dimOpp; i1++)
					if (!HandCrossHandNB(nbH, NbHand(opp, i1)))
					{
						float win = PrAllIn(i0, i1);
						float res0 = ptr[0] * win + ptr[4] * (1 - win);
						ev[i0] += res0 * kOpp[i1];
					}
			}
		else
			for (int i0 = 0; i0 < dimH; i0++)
			{
				int nbH = NbHand(nbHero, i0);
				for (int i1 = 0; i1 < dimOpp; i1++)
					if (!HandCrossHandNB(nbH, NbHand(opp, i1)))
					{
						float win = PrAllIn(i1, i0);
						tpFloat resH = ptr[5] * (1 - win) + ptr[1] * win;
						ev[i0] += resH * kOpp[i1];
					}
			}
	}
	else
	{
		clNodeBank *bank;
		if (_sit.NameTrade() == TRADE_TURN)
		{
			bank = &glTurnNodeBank;
		}
		else
		{
			bank = &glFlopNodeBank;
		}
		clRootStreetTree root;
		root._sit = node->GetSit();
		root._gh[nbHero].SetDim(ALL_CN_HAND);
		root._gh[nbHero].ClearDat();
		for (int i = 0; i < dimH; i++)
			root._gh[nbHero][NbHand(nbHero, i)] = kH[i];
		root._gh[opp].SetDim(ALL_CN_HAND);
		root._gh[opp].ClearDat();
		for (int i = 0; i < dimOpp; i++)
			root._gh[opp][NbHand(opp, i)] = kOpp[i];
		root.CalcIndex();
		if (root.CnHand(0) == 0 || root.CnHand(1) == 0)
			return;
		root.CalcForce();
		vector <float> evCur(ALL_CN_HAND);
		int stB = bank->FillEV(&root, nbHero, evCur, ut);
		double mult = ((double)root._sit.PlMoney(0)) / stB;
		for (int i = 0; i < dimH; i++)
			ev[i] = evCur[NbHand(nbHero, i)] * mult;

		for (int i = 0; i < dimH; i++)
		{
			int nbH = NbHand(nbHero, i);
			double sum = 0;
			for (int iOpp = 0; iOpp < dimOpp; iOpp++)
			{
				int nbOpp = NbHand(opp, iOpp);
				if (HandCrossHandNB(nbH, nbOpp)) continue;
				sum += kOpp[iOpp];
			}
			ev[i] *= sum;
		}
	}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
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
void	clRootStreetTree::ResultMultiHandsTree(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev)
{
	int *ptr = (int *)&node->AtrTree()->_param[0];
	int opp = (nbHero == 0) ? 1 : 0;
	int dimH = CnHand(nbHero);
	int dimOpp = CnHand(opp);
	ZeroMemory(&ev[0], dimH * sizeof(float));
	if (node->GetSit().CNPlayerIsCard() == 1)		//Fold
	{
		for (int i0 = 0; i0 < dimH; i0++)
		{
			int nbH = NbHand(nbHero, i0);
			for (int i1 = 0; i1 < dimOpp; i1++)
				if (!HandCrossHandNB(nbH, NbHand(opp, i1)))
					ev[i0] += ptr[nbHero] * kOpp[i1];
		}
	}
	else if (node->GetSit().CNPlayerInTrade() < 2)	//AllIn
	{
		if (nbHero == 0)
			for (int i0 = 0; i0 < dimH; i0++)
			{
				int nbH = NbHand(nbHero, i0);
				for (int i1 = 0; i1 < dimOpp; i1++)
					if (!HandCrossHandNB(nbH, NbHand(opp, i1)))
					{
						float win = PrAllIn(i0, i1);
						float res0 = ptr[0] * win + ptr[4] * (1 - win);
						ev[i0] += res0 * kOpp[i1];
					}
			}
		else
			for (int i0 = 0; i0 < dimH; i0++)
			{
				int nbH = NbHand(nbHero, i0);
				for (int i1 = 0; i1 < dimOpp; i1++)
					if (!HandCrossHandNB(nbH, NbHand(opp, i1)))
					{
						float win = PrAllIn(i1, i0);
						tpFloat resH = ptr[5] * (1 - win) + ptr[1] * win;
						ev[i0] += resH * kOpp[i1];
					}
			}
	}
	else
	{
		clRootRiverTree rr;
		rr._sit = node->GetSit();
		rr._sit.ExecAct(HistAction(NB_DILER, ACT_DELRIVER, 0));
		vector <float> evCur(ALL_CN_HAND);
		vector <float> kHand[2] = { vector <float>(ALL_CN_HAND),vector <float>(ALL_CN_HAND) };
		ev.resize(ALL_CN_HAND);
		ZeroMemory(&ev[0], ALL_CN_HAND * sizeof(float));

		float w0[ALL_CN_HAND];
		ZeroMemory(w0, sizeof(w0));

		tpBooleanDeck deck;
		for (int i = 0; i < _sit.CnCardTable(); i++)
			deck.Skip(_sit.CardsTable()[i].Number());
		clHandsGroupEx ghIn[2];
		ghIn[0]._vect.resize(ALL_CN_HAND);
		ghIn[0].ClearDat();
		ghIn[1]._vect.resize(ALL_CN_HAND);
		ghIn[1].ClearDat();
		for (int i = 0; i < dimH; i++)
			ghIn[nbHero][NbHand(nbHero, i)] = kH[i];
		for (int i = 0; i < dimOpp; i++)
			ghIn[opp][NbHand(opp, i)] = kOpp[i];

		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				//glTrasser.WriteMessage(k);
				rr._gh[0] = ghIn[0];
				rr._gh[1] = ghIn[1];
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

				int st0 = glBankRiver.FillRiverEV(&rr, nbHero, evCur);
				tpFloat f0 = (tpFloat)_sit.PlMoney(0) / st0;
				for (int i = 0; i < rr.CnHand(nbHero); i++)
				{
					int nb = rr.NbHand(nbHero, i);
					ev[nb] += evCur[nb] * kHand[nbHero][nb] * f0;
					w0[nb] += kHand[nbHero][nb];
				}
			}

		for (int i = 0; i < dimH; i++)
		{
			int nbH = NbHand(nbHero, i);
			if (w0[nbH] > DOUBLE_0)
			{
				double sum = 0;
				for (int iOpp = 0; iOpp < dimOpp; iOpp++)
				{
					int nbOpp = NbHand(opp, iOpp);
					if (HandCrossHandNB(nbH, nbOpp)) continue;
					sum += kOpp[iOpp];
				}
				ev[i] *= sum / w0[nbH];
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::AddResEVMultiHands(clHoldemTree *node, int nbHero, int cnB, vector <float> &resEV, vector <float> *evCur, tpFloat *w, vector <tpFloat> &kH, vector <tpFloat> &kOpp)
{
	int opp = (nbHero == 0) ? 1 : 0;
	int dim = CnHand(nbHero), dimOpp = CnHand(opp);
	for (int i = 0; i < dim; i++)
	{
		int nbH = NbHand(nbHero, i);
		tpFloat ev[MAX_CN_BRANCH];
		for (int k = 0; k < cnB; k++)
			ev[k] = evCur[k][i];
		tpFloat kOpp1 = 0;
		for (int i1 = 0; i1 < dimOpp; i1++)
			if (!HandCrossHandNB(nbH, NbHand(opp, i1)))
				kOpp1 += kOpp[i1];

		clCalcParam cls = node->AtrTree()->CalcParam(i);
		cls.AddResEV(cnB, resEV[i], w + i*cnB, ev, kH[i], kOpp1);
#ifdef VIEW_PARAM
		tpCalcParam par0;
		par0.LoadParam(cls, cnB);
		par0;
#endif
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clRootStreetTree::CalcNeshMultiHands(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev, tpUnfindTree *ut)
{
	//glTrasser.WriteMessage(node->_val->_nbNode);
	int ind = node->Indicator();
	if (node->EndedNode())
		return ResultMultiHands(node, nbHero, kH, kOpp, ev, ut);

	int dim = node->AtrTree()->CnClasters();
	int cnB = node->CnBranch();
	tpFloat *w = new tpFloat[cnB*dim];
	ZeroMemory(w, cnB*dim * sizeof(tpFloat));
	ZeroMemory(&ev[0], ev.size() * sizeof(float));
	if (ind == nbHero)
	{
		node->AtrTree()->GetAllWeightNoNull(cnB, w);
		vector <float> evCur[MAX_CN_BRANCH];
		vector <tpFloat> kCur;
		for (int k = 0; k < cnB; k++)
		{
			kCur = kH;
			evCur[k].resize(dim);
			for (int i = 0; i < dim; i++)
				kCur[i] *= w[i*cnB + k];
			CalcNeshMultiHands(node->Branch(k), nbHero, kCur, kOpp, evCur[k], ut);

			//vector <float> vvv; vvv = evCur[k];

			for (int i = 0; i < dim; i++)
				ev[i] += evCur[k][i] * w[i*cnB + k];
		}
		AddResEVMultiHands(node, nbHero, cnB, ev, evCur, w, kH, kOpp);
	}
	else
	{
		int dimEV = ev.size();
		node->AtrTree()->GetAllWeight(cnB, w);
		vector <float> evCur(dimEV);
		vector <tpFloat> kCur;
		for (int k = 0; k < cnB; k++)
		{
			kCur = kOpp;
			for (int i = 0; i < dim; i++)
				kCur[i] *= w[i*cnB + k];
			CalcNeshMultiHands(node->Branch(k), nbHero, kH, kCur, evCur, ut);
			for (int i = 0; i < dimEV; i++)
				ev[i] += evCur[i];
		}
	}
	delete w;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CalcNeshMultiHandsTree(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <float> &ev)
{
	//glTrasser.WriteMessage(node->_val->_nbNode);
	int ind = node->Indicator();
	if (node->EndedNode())
		return ResultMultiHandsTree(node, nbHero, kH, kOpp, ev);

	int dim = node->AtrTree()->CnClasters();
	int cnB = node->CnBranch();
	tpFloat *w = new tpFloat[cnB*dim];
	ZeroMemory(w, cnB*dim * sizeof(tpFloat));
	ZeroMemory(&ev[0], ev.size() * sizeof(float));
	if (ind == nbHero)
	{
		node->AtrTree()->GetAllWeightNoNull(cnB, w);
		vector <float> evCur[MAX_CN_BRANCH];
		vector <tpFloat> kCur;
		for (int k = 0; k < cnB; k++)
		{
			kCur = kH;
			evCur[k].resize(dim);
			for (int i = 0; i < dim; i++)
				kCur[i] *= w[i*cnB + k];
			CalcNeshMultiHandsTree(node->Branch(k), nbHero, kCur, kOpp, evCur[k]);

			vector <float> vvv; vvv = evCur[k];

			for (int i = 0; i < dim; i++)
				ev[i] += evCur[k][i] * w[i*cnB + k];
		}
		AddResEVMultiHands(node, nbHero, cnB, ev, evCur, w, kH, kOpp);
	}
	else
	{
		int dimEV = ev.size();
		node->AtrTree()->GetAllWeight(cnB, w);
		vector <float> evCur(dimEV);
		vector <tpFloat> kCur;
		for (int k = 0; k < cnB; k++)
		{
			kCur = kOpp;
			for (int i = 0; i < dim; i++)
				kCur[i] *= w[i*cnB + k];
			CalcNeshMultiHandsTree(node->Branch(k), nbHero, kH, kCur, evCur);
			for (int i = 0; i < dimEV; i++)
				ev[i] += evCur[i];
		}
	}
	delete w;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::SetWeightStrat()
{
	_tree.ClearStatWeight();
	int cn0 = CnHand(0), cn1 = CnHand(1);
	enNameTrade trade = _sit.NameTrade();
	for (int i0 = 0; i0 < cn0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < cn1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (HandCrossHandNB(nb0, nb1)) continue;
			tpTreePlayerInfo info;
			info._nbCls[trade][0] = i0;
			info._nbCls[trade][1] = i1;
			double w0[] = { _gh[0][nb0],_gh[1][nb1] };
			_tree.SetWeightStrat(&info, w0);
		}
	}
	_tree.CalcWeightStat();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clRootStreetTree::CreateStreetDat(clStreetDat &dat)
{
	if (_sit.NameTrade() == TRADE_TURN)
		return CreateTurnDat(dat);
	else
	{
		/*		if (CnHand(0) > CN_HANDS_FORBD || CnHand(1) > CN_HANDS_FORBD)
				{
					clStreetDat datC, datQ;
					clRootStreetTree root = *this;
					CreateFlopDatQuick(dat);
					root.CompactGH(1, CN_HANDS_FORBD, &dat);
					root.CreateFlopDatQuick(datQ); +
					root.CreateFlopDat(datC);
					ReCalcFlopDat(dat, datQ, datC);
				}
				else
		CreateFlopDat(dat);*/
		//CreateFlopDatQuick(dat);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clRootStreetTree::CreateStreetDatQuick(clStreetDat &dat)
{
	if (_sit.NameTrade() == TRADE_TURN)
		CreateTurnDatQuick(dat);
	else
		CreateFlopDatQuick(dat);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clRootStreetTree::CnMultiHandsCalc()
{
	int cn= CnHand(_tree.Indicator());
	if (cn == 0)
	{
		CalcIndex();
		cn = CnHand(_tree.Indicator());
	}
	return _tree.CnCalc() / cn;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
struct tp3Float
{
	float _force, _force50, _weight;
};
bool	Compare3Float(tp3Float val1, tp3Float val2) { return (val1._force50 < val2._force50); }

/*void	FillDatFlop(clRootStreetTree *root, int nbHero, vector <tpStreetDatGH>  &vect, tp2Float *val, tpFloat wSum)
{
	tpBooleanDeck deck;
	for (int i = 0; i < root->_sit.CnCardTable(); i++)
		deck.Skip(root->_sit.CardsTable()[i].Number());
	int dim = root->CnHand(nbHero);
	tp3Float *arr = new tp3Float[dim*CN_CARD_COL];
	ZeroMemory(arr, dim*CN_CARD_COL * sizeof(tp3Float));
	vect.resize(dim);
	for (int i = 0; i < dim; i++)
	{
		int nb = root->NbHand(nbHero, i);
		tpHand hand(nb);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int kT = 0; kT < CN_CARD_COL; kT++)
			if (deck[kT])
				for (int kR = kT + 1; kR < CN_CARD_COL; kR++)
					if (deck[kR])
					{
						int ind = i*CN_CARD_COL*CN_CARD_COL + kT*CN_CARD_COL + kR;
						int indR = i*CN_CARD_COL*CN_CARD_COL + kR*CN_CARD_COL + kT;
						if (val[ind]._weight > DOUBLE_0)
							val[ind]._force /= val[ind]._weight;
						else
							val[ind]._force = 0;
						val[indR] = val[ind];  // заполняем реверсные поля
					}

		vect[i]._nb = nb;
		tp2Float *ptrT = &val[i*CN_CARD_COL*CN_CARD_COL];
		for (int kT = 0; kT < CN_CARD_COL; kT++)
			if (deck[kT])
			{
				tp2Float *ptr = ptrT + kT*CN_CARD_COL;
				SortShell(CN_CARD_COL, ptr, Compare2Float);
				float sum = 0, wk = 0;
				int k = 0;
				for (; k < CN_CARD_COL / 2; k++)
				{
					sum += ptr[k]._force*ptr[k]._weight;
					wk += ptr[k]._weight;
				}
				float sum1 = 0, wk1 = 0;
				for (; k < CN_CARD_COL; k++)
				{
					sum1 += ptr[k]._force*ptr[k]._weight;
					wk1 += ptr[k]._weight;
				}
				int ind = i*CN_CARD_COL + kT;
				arr[ind]._weight += wk + wk1;
				arr[ind]._force = (sum + sum1) / (wk + wk1);
				arr[ind]._force50 = sum / wk;
			}
		tp3Float *ptr = arr + i*CN_CARD_COL;
		SortShell(CN_CARD_COL, ptr, Compare3Float);
		int k = 0;
		float sum = 0, wk = 0, sum50 = 0;
		for (; k < CN_CARD_COL / 2; k++)
		{
			sum += ptr[k]._force*ptr[k]._weight;
			sum50 += ptr[k]._force50*ptr[k]._weight;
			wk += ptr[k]._weight;
		}
		float wk1 = 0;
		for (; k < CN_CARD_COL; k++)
		{
			sum += ptr[k]._force*ptr[k]._weight;
			wk1 += ptr[k]._weight;
		}
		vect[i]._weight = (wk + wk1) / wSum;
		vect[i]._force = sum / (wk + wk1);
		vect[i]._force50 = sum50 / wk;

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
	delete arr;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clRootStreetTree::CreateFlopDat(clStreetDat &dat)
{
	dat._k = K_FOR_FLOPDAT;
	InitDeck();

	int dim0 = CnHand(0);
	int dim1 = CnHand(1);
	dat._vectUnit[0].resize(dim0);
	dat._vectUnit[1].resize(dim1);
	vector <tpStreetDatGH> &vect0 = dat._vectUnit[0];
	vector <tpStreetDatGH> &vect1 = dat._vectUnit[1];
	ZeroMemory(&vect0[0], dim0 * sizeof(tpStreetDatGH));
	ZeroMemory(&vect1[0], dim1 * sizeof(tpStreetDatGH));
	double w = 0;
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (HandCrossHandNB(nb0, nb1))
				continue;
			double ww = _gh[0][nb0] * _gh[1][nb1];
			float evF = FlopAllInResult(nb0, nb1, _cards);
			vect0[i0]._force += evF*ww;
			vect0[i0]._weight += ww;
			vect1[i1]._force += (1-evF)*ww;
			vect1[i1]._weight += ww;
			w += ww;
		}
	}

	for (int i0 = 0; i0 < dim0; i0++)
	{
		tpStreetDatGH &datE = vect0[i0];
		datE._nb = NbHand(0,i0);
		if (datE._weight > DOUBLE_0)
			datE._force /= datE._weight;
		datE._weight /= w;
		tpHand hand(datE._nb);
		int nbF = GetFlopHandIndex(_cards, hand);
		datE._force50 = glVectFlopEV50[nbF];
	}
	for (int i1 = 0; i1 < dim1; i1++)
	{
		tpStreetDatGH &datE = vect1[i1];
		datE._nb = NbHand(1, i1);
		if (datE._weight > DOUBLE_0)
			datE._force /= datE._weight;
		datE._weight /= w;
		tpHand hand(datE._nb);
		int nbF = GetFlopHandIndex(_cards, hand);
		datE._force50 = glVectFlopEV50[nbF];
	}

	dat.Copy50AndSort();
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------


/*void	clRootStreetTree::CreateFlopDat(clStreetDat &dat)
{
	if (_force.size() == 0)
		CalcForceFlop();
	tpBooleanDeck deck;
	for (int i = 0; i < _sit.CnCardTable(); i++)
		deck.Skip(_sit.CardsTable()[i].Number());
	int dim0 = CnHand(0), dim1 = CnHand(1);
	tp2Float *val0 = new tp2Float[dim0*CN_CARD_COL*CN_CARD_COL];
	tp2Float *val1 = new tp2Float[dim1*CN_CARD_COL*CN_CARD_COL];
	ZeroMemory(val0, dim0*CN_CARD_COL*CN_CARD_COL * sizeof(tp2Float));
	ZeroMemory(val1, dim1*CN_CARD_COL*CN_CARD_COL * sizeof(tp2Float));
	float w = 0;
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		tpHand hand0(nb0);
		deck[hand0[0]] = false;
		deck[hand0[1]] = false;
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
			{
				tpHand hand1(nb1);
				deck[hand1[0]] = false;
				deck[hand1[1]] = false;
				for (int kT = 0; kT < CN_CARD_COL; kT++)
					if (deck[kT])
						for (int kR = kT + 1; kR < CN_CARD_COL; kR++)
							if (deck[kR])
							{
								float ww = _gh[0][nb0] * _gh[1][nb1];
								int ind0 = i0*CN_CARD_COL*CN_CARD_COL + kT*CN_CARD_COL + kR, ind1 = i1*CN_CARD_COL*CN_CARD_COL + kT*CN_CARD_COL + kR;
								val0[ind0]._weight += ww;
								val1[ind1]._weight += ww;
								w += ww;
								int f0 = Force(kT, kR, nb0), f1 = Force(kT, kR, nb1);
								if (f0 > f1)
									val0[ind0]._force += ww;
								else if (f1 > f0)
									val1[ind1]._force += ww;
								else
								{
									val0[ind0]._force += ww / 2;
									val1[ind1]._force += ww / 2;
								}
							}
				deck[hand1[0]] = true;
				deck[hand1[1]] = true;
			}
		}
		deck[hand0[0]] = true;
		deck[hand0[1]] = true;
	}

	FillDatFlop(this, 0, dat._vectUnit[0], val0, w);
	FillDatFlop(this, 1, dat._vectUnit[1], val1, w);

	delete val0;
	delete val1;
	dat.Copy50AndSort();
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clRootStreetTree::CreateFlopDatQuick(clStreetDat &dat)
{
	dat._k = K_FOR_FLOPDAT;
	InitDeck();
	int dim0 = CnHand(0), dim1 = CnHand(1);
	dat._vectUnit[0].resize(dim0);
	dat._vectUnit[1].resize(dim1);
	for (int i = 0; i < dim0; i++)
	{
		int nb = NbHand(0, i);
		tpHand hand(nb);
		int nbF = GetFlopHandIndex(_cards, hand);
		dat._vectUnit[0][i] = { nb,0,glVectFlopEV100[nbF],glVectFlopEV50[nbF] };
	}
	for (int i = 0; i < dim1; i++)
	{
		int nb = NbHand(1, i);
		tpHand hand(nb);
		int nbF = GetFlopHandIndex(_cards, hand);
		dat._vectUnit[1][i] = { nb,0,glVectFlopEV100[nbF],glVectFlopEV50[nbF] };
	}
	float w = 0;
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
			{
				float ww = _gh[0][nb0] * _gh[1][nb1];
				dat._vectUnit[0][i0]._weight += ww;
				dat._vectUnit[1][i1]._weight += ww;
				w += ww;
			}
		}
	}
	for (int i = 0; i < dim0; i++)
		dat._vectUnit[0][i]._weight /= w;
	for (int i = 0; i < dim1; i++)
		dat._vectUnit[1][i]._weight /= w;

	dat.Copy50AndSort();
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CreateSitStreet(clSitHoldem &sit, int stack, int pot, enNameTrade trade)
{
	clStacks st; st.Init(100, 0, stack, stack);
	sit.Init(st);

	sit._arrAct.Clear();
	sit.AddAct(POS_SB, ACT_POSTSB, sit.BigBlind() / 2);
	sit.AddAct(POS_BB, ACT_POSTBB, sit.BigBlind());
	sit.AddAct(NB_DILER, ACT_DELCARD, 0);
	if (pot > 2 * sit.BigBlind())
	{
		sit.AddAct(POS_SB, ACT_RAISE, pot / 2 - 0.5*sit.BigBlind());
		sit.AddAct(POS_BB, ACT_CALL, pot / 2 - sit.BigBlind());
	}
	else
	{
		sit.AddAct(POS_SB, ACT_CALL, 0.5*sit.BigBlind());
		sit.AddAct(POS_BB, ACT_CHECK, 0);
	}
	sit.AddAct(NB_DILER, ACT_DELFLOP, 0);
	if (trade == TRADE_TURN)
	{
		sit.AddAct(POS_BB, ACT_CHECK, 0);
		sit.AddAct(POS_SB, ACT_CHECK, 0);
		sit.AddAct(NB_DILER, ACT_DELTURN, 0);
	}

	sit.ExecHist(sit.PtrHistory());
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
struct tp2Float
{
	float _force, _weight;
};

bool	CompareForce(tpStreetDatGH val1, tpStreetDatGH val2) { return (val1._force < val2._force); }
bool	CompareForce50(tpStreetDatGH val1, tpStreetDatGH val2) { return (val1._force50 < val2._force50); }
bool	clRootStreetTree::CreateTurnDat(clStreetDat &dat)
{
	dat._k = 1;
	int stPl = (_sit.PlayerMoney(0) > _sit.PlayerMoney(1)) ? _sit.PlayerMoney(1) : _sit.PlayerMoney(0);
	dat._stack[0] = stPl;
	dat._stack[1] = stPl;
	dat._pot = _sit.TotalPot();
#ifdef TURN_FULLTREE
	vector <int> forceFull;
	CalcForceTurn(forceFull);
#else
	if (_force.size() == 0)
		CalcForceTurn(_force);
#endif
	tpBooleanDeck deck;
	for (int i = 0; i < _sit.CnCardTable(); i++)
		deck.Skip(_sit.CardsTable()[i].Number());
	int dim0 = CnHand(0), dim1 = CnHand(1);
	tp2Float *val0 = new tp2Float[dim0*CN_CARD_COL];
	tp2Float *val1 = new tp2Float[dim1*CN_CARD_COL];
	ZeroMemory(val0, dim0*CN_CARD_COL * sizeof(tp2Float));
	ZeroMemory(val1, dim1*CN_CARD_COL * sizeof(tp2Float));
	float w = 0;
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		tpHand hand0(nb0);
		deck[hand0[0]] = false;
		deck[hand0[1]] = false;
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
			{
				tpHand hand1(nb1);
				deck[hand1[0]] = false;
				deck[hand1[1]] = false;
				float ww = _gh[0][nb0] * _gh[1][nb1];
				for (int k = 0; k < CN_CARD_COL; k++)
					if (deck[k])
					{
						int ind0 = i0*CN_CARD_COL + k, ind1 = i1*CN_CARD_COL + k;
						val0[ind0]._weight += ww;
						val1[ind1]._weight += ww;
						w += ww;
#ifdef TURN_FULLTREE
						int f0 = Force(forceFull, k, nb0), f1 = Force(forceFull, k, nb1);
#else
						int f0 = Force(k, nb0), f1 = Force(k, nb1);
#endif
						if (f0 > f1)
							val0[ind0]._force += ww;
						else if (f1 > f0)
							val1[ind1]._force += ww;
						else
						{
							val0[ind0]._force += ww / 2;
							val1[ind1]._force += ww / 2;
						}
					}
				deck[hand1[0]] = true;
				deck[hand1[1]] = true;
			}
		}
		deck[hand0[0]] = true;
		deck[hand0[1]] = true;
	}
	if (w < DOUBLE_0)
		return false;
	dat._vectUnit[0].resize(dim0);
	for (int i = 0; i < dim0; i++)
	{
		dat._vectUnit[0][i]._nb = NbHand(0, i);
		tp2Float *ptr = &val0[i*CN_CARD_COL];
		int k = 0;
		float sum = 0, wk = 0, sum2 = 0;
		for (; k < CN_CARD_COL; k++)
			if (ptr[k]._weight > DOUBLE_0)
			{
				sum += ptr[k]._force;
				wk += ptr[k]._weight;
				sum2 += ptr[k]._force*ptr[k]._force / ptr[k]._weight;
			}
		if (wk > DOUBLE_0)
		{
			dat._vectUnit[0][i]._weight = wk / w;
			dat._vectUnit[0][i]._force = sum / wk;
			dat._vectUnit[0][i]._force50 = sqrt(sum2 / wk);
		}
		else
		{
			dat._vectUnit[0][i]._weight = 0;
			dat._vectUnit[0][i]._force = 0;
			dat._vectUnit[0][i]._force50 = 0;
		}
	}
	dat._vectUnit[1].resize(dim1);
	for (int i = 0; i < dim1; i++)
	{
		dat._vectUnit[1][i]._nb = NbHand(1, i);
		tp2Float *ptr = &val1[i*CN_CARD_COL];
		int k = 0;
		float sum = 0, wk = 0, sum2 = 0;
		for (; k < CN_CARD_COL; k++)
			if (ptr[k]._weight > DOUBLE_0)
			{
				sum += ptr[k]._force;
				wk += ptr[k]._weight;
				sum2 += ptr[k]._force*ptr[k]._force / ptr[k]._weight;
			}
		if (wk > DOUBLE_0)
		{
			dat._vectUnit[1][i]._weight = wk / w;
			dat._vectUnit[1][i]._force = sum / wk;
			dat._vectUnit[1][i]._force50 = sqrt(sum2 / wk);
		}
		else
		{
			dat._vectUnit[1][i]._weight = 0;
			dat._vectUnit[1][i]._force = 0;
			dat._vectUnit[1][i]._force50 = 0;
		}
	}
	delete val0;
	delete val1;
	dat.Copy50AndSort();
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	CompactRight(vector <tpStreetDatGH> &vect, int nb, double min)
{
	int dim = vect.size();
	int nb1 = FindUnit(vect, nb, dim, min);
	if (nb1 == -1)
	{
		tpStreetDatGH v;
		for (int i = nb + 1; i < dim; i++)
		{
			nb1 = CreateUnitVirtual(vect, i, dim, min, v);
			if(nb1 != -1)
		}

	}
	CompactBetween(vect, nb, nb1, min);
	CompactRight(vect, nb1, min);
}

int FindUnit(vector <tpStreetDatGH> &vect, int nb0, int nb1, double min)
{
	for (int i = nb0; i < nb1; i++)
		if (vect[i]._weight > min)
			return i;
	return -1;
}

int	CreateUnit(vector <tpStreetDatGH> &vect, int nb1, int nb2, double min)
{
	int i = nb1;
	for (; i < nb2; i++)
	{
		if (vect[i]._weight > min) break;
		vect[i + 1] += vect[i];
		vect[i]._weight = 0;
	}
	return i;
}

int	CompactLeft(vector <tpStreetDatGH> &vect, int nb1, int nb2, double min)
{
	int nb = nb1;
	tpStreetDatGH v = vect[nb1];
	if (v._weight < min)
		for (nb++; nb < nb2; nb++)
		{
			v += vect[nb];
			vect[nb]._weight = 0;
			if (v._weight > min)
				break;
		}
	if (nb < nb2 && vect[nb].Distance(v) < vect[nb].Distance(vect[nb2]))
	{
		CompactBetween(vect, nb, nb2, min);
	}
	else
	{
		vect[nb2] += v;
		for (int i = nb + 1; i < nb2; i++)
		{
			vect[nb2] += vect[i];
			vect[i]._weight = 0;
		}
	}
	return nb2;
}

void	CompactVectDat(vector <tpStreetDatGH> &vect, double min)
{
	int dim = vect.size(), nb = 0;
	if (dim <= CN_STREETDAT_UNIT)
		return;
	if (vect[0]._weight < min)
	{
		nb = FindUnit(vect, 1, dim, min);
		if (nb == -1)
			nb = CreateUnit(vect, 0, dim, min);
		else
			nb = CompactLeft(vect, 0, nb);
	}
	CompactRight(vect, nb);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CompactStreetDat(clStreetDat  &dat, double min)
{
	CompactVectDat(dat._vectUnit[0], min);
	CompactVectDat(dat._vectUnit[1], min);
	CompactVectDat(dat._vectUnit50[0], min);
	CompactVectDat(dat._vectUnit50[1], min);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CreateStreetDatMin(clStreetDat &dat, clStreetDat *datIn0)
{
	clStreetDat  dd;
	if (datIn0 == NULL)
		CreateStreetDat(dd);
	else
		dd = *datIn0;
	CompactStreetDat(dd, 0.01);

	ReCalcParam(&dd);
	CalcIndex();
	//root.CalcForce();
	FindMinParam(dd);
	CreateStreetDat(dat);
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CreateStreetDatMin(clStreetDat &dat, clStreetDat *datIn0)
{
	clStreetDat *datIn = datIn0, dd;
	if (datIn0 == NULL)
	{
		CreateStreetDat(dd);
		datIn = &dd;
	}

	ReCalcParam(datIn);
	CalcIndex();
	//root.CalcForce();
	FindMinParam(*datIn);
	CreateStreetDat(dat);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CompactGH_H(double minW, int cnMin, int *nbIn, clStreetDat dat)
{
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		vector <tpStreetDatGH> &vect = dat._vectUnit[nbPl];
		int nbH = nbIn[nbPl];
		while (true)
		{
			int dim = vect.size();
			//glTrasser.WriteMessage(dim);
			if (dim <= cnMin)
				break;
			int nb = (vect[0]._nb != nbH) ? 0 : 1;
			for (int i = 1; i < dim; i++)
				if (vect[i]._nb != nbH)
					if (vect[i]._weight < vect[nb]._weight)
						nb = i;
			if (vect[nb]._weight > minW)
				break;
			int nb1 = (nb == 0) ? 1 : 0;
			float dist = vect[nb].Distance(vect[nb1]);
			for (int i = nb1 + 1; i < dim; i++)
			{
				float val = vect[nb].Distance(vect[i]);
				if (val < dist)
				{
					dist = val;
					nb1 = i;
				}
			}
			vect[nb1]._weight += vect[nb]._weight;
			vect.erase(vect.begin() + nb);
		}
		_gh[nbPl].ClearDat();
		int dim = vect.size();
		for (int i = 0; i < dim; i++)
			_gh[nbPl][vect[i]._nb] = vect[i]._weight;
		_gh[nbPl].Norm();
	}
	CalcIndex();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStreetDat::Copy50AndSort()
{
	_vectUnit50[0] = _vectUnit[0];
	_vectUnit50[1] = _vectUnit[1];
	int dim0 = _vectUnit[0].size(), dim1 = _vectUnit[1].size();
	SortShell(dim0, &_vectUnit[0][0], CompareForce);
	SortShell(dim1, &_vectUnit[1][0], CompareForce);
	SortShell(dim0, &_vectUnit50[0][0], CompareForce50);
	SortShell(dim1, &_vectUnit50[1][0], CompareForce50);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat clStreetDat::Distance(clStreetDat &dat)
{
	float res = 0;
	for (int k = 0; k < 2; k++)
	{
		int i = 0, j = 0;
		vector <tpStreetDatGH> &vectI = dat._vectUnit[k], &vectJ = _vectUnit[k];
		int dimI = vectI.size(), dimJ = vectJ.size();
		float ost = vectJ[j]._weight, weight = vectI[i]._weight;
		while (i < dimI)
		{
			if (weight < ost)
			{
				float val = vectJ[j]._force - vectI[i]._force;
				res += val*val*weight;
				ost -= weight;
				i++;
				if (i == dimI)
					break;
				weight = vectI[i]._weight;
				if (weight < DOUBLE_0)
					break;
			}
			else
			{
				float val = vectJ[j]._force - vectI[i]._force;
				res += val*val*ost;
				weight -= ost;
				j++;
				if (j == dimJ)
					break;
				ost = vectJ[j]._weight;
			}
		}
	}
	float res50 = 0;
	for (int k = 0; k < 2; k++)
	{
		int i = 0, j = 0;
		vector <tpStreetDatGH> &vectI = dat._vectUnit50[k], &vectJ = _vectUnit50[k];
		int dimI = vectI.size(), dimJ = vectJ.size();
		float ost = vectJ[j]._weight, weight = vectI[i]._weight;
		while (i < dimI)
		{
			if (weight < ost)
			{
				float val = vectJ[j]._force50 - vectI[i]._force50;
				res50 += val*val*weight;
				ost -= weight;
				i++;
				if (i == dimI)
					break;
				weight = vectI[i]._weight;
				if (weight < DOUBLE_0)
					break;
			}
			else
			{
				float val = vectJ[j]._force50 - vectI[i]._force50;
				res50 += val*val*ost;
				weight -= ost;
				j++;
				if (j == dimJ)
					break;
				ost = vectJ[j]._weight;
			}
		}
	}
	res = sqrt((res + _k*res50) / (1 + _k));
	float st = float(_stack[0]) / _pot, datSt = float(dat._stack[0]) / dat._pot;
	float ds = (datSt > st) ? (datSt - st) / datSt : (st - datSt) / st;
	return (res + ds);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CalcForceTurn(vector <int> &force)
{
	force.resize(ALL_CN_HAND*CN_CARD_COL);
	ZeroMemory(&force[0], ALL_CN_HAND*CN_CARD_COL * sizeof(int));
	tpBooleanDeck deck;
	tpCard cards[5];
	for (int i = 0; i < 4; i++)
	{
		cards[i] = _sit.CardsTable()[i];
		deck.Skip(cards[i]);
	}

	int dim = CnHand(0);
	for (int i = 0; i < dim; i++)
	{
		int nbH = NbHand(0, i);
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{
				cards[4] = k;
				int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
				Force(force, k, nbH) = glVectRiverHandGame[nbR].ToInt();
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
	dim = CnHand(1);
	for (int i = 0; i < dim; i++)
	{
		int nbH = NbHand(1, i);
		tpHand hand(nbH);
		deck[hand[0]] = false;
		deck[hand[1]] = false;
		for (int k = 0; k < CN_CARD_COL; k++)
			if (deck[k])
			{ 
				if (Force(force, k, nbH) != 0)
					break;
				cards[4] = k;
				int nbT, nbR = GetTurnAndRiverHandIndex(cards, hand, nbT);
				Force(force, k, nbH) = glVectRiverHandGame[nbR].ToInt();
			}

		deck[hand[0]] = true;
		deck[hand[1]] = true;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::ReCalcParam(clStreetDat *datIn)
{
	clStreetDat datCur, *dat;
	if (datIn == NULL)
	{
		dat = &datCur;
		CreateStreetDat(*dat);
	}
	else
		dat = datIn;
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		clClasters <tpStreetDatGH> cls;
		vector <tpStreetDatGH> &vect = dat->_vectUnit[nbPl];
		int dim = vect.size();
		cls.Resize(CN_STREETDAT_UNIT);
		if (CN_STREETDAT_UNIT > dim)
		{
			for (int i = 0; i < CN_STREETDAT_UNIT; i++)
			{
				cls[i].resize(1);
				if (i < dim)
					cls[i][0] = vect[i];
				else
					cls[i][0] = { i,0,0,0 };
			}
		}
		else
		{
			/*int cur = 0;
			for (int i = 0; i < CN_STREETDAT_UNIT; i++)
			{
				int cn = (dim - cur) / (CN_STREETDAT_UNIT - i);
				cls[i].resize(cn);
				memcpy(&cls[i][0], &vect[cur], cn * sizeof(tpStreetDatGH));
				cur += cn;
			}*/
			int k = 0, cur = 0;
			float step = 1. / CN_STREETDAT_UNIT, sum = 0, val = 0;
			for (int i = 0; i < CN_STREETDAT_UNIT - 1; i++)
			{
				val += step;
				for (; k < dim; k++)
				{
					sum += vect[k]._weight;
					if (sum >= val)
						break;
				}
				if (dim - k < CN_STREETDAT_UNIT - i)
					k = dim - CN_STREETDAT_UNIT + i;
				k++;
				int cn = k - cur;
				cls[i].resize(cn);
				memcpy(&cls[i][0], &vect[cur], cn * sizeof(tpStreetDatGH));
				cur = k;
			}
			int cn = dim - cur;
			cls[CN_STREETDAT_UNIT - 1].resize(cn);
			memcpy(&cls[CN_STREETDAT_UNIT - 1][0], &vect[cur], cn * sizeof(tpStreetDatGH));
			for (int i = 0; i < 5; i++)
				if (!cls.ReStraightClasters())
					break;
		}
		dim = cls.CnClasters();
		double w0[200];
		for (int i = 0; i < dim; i++)
		{
			int dim0 = cls[i].size();
			for (int k = 1; k < dim0; k++)
				cls[i][0]._weight += cls[i][k]._weight;
			w0[i] = cls[i][0]._weight;
			if (w0[i] < DOUBLE_0)
				w0[i] = 10 * DOUBLE_0;
		}
		_gh[nbPl].ClearDat();
		for (int i = 0; i < dim; i++)
			_gh[nbPl][glTDD.GetNbHand(nbPl, i)] = w0[i];
		_gh[nbPl].Norm();
	}

	memcpy(_sit.CardsTable(), glTDD._board, sizeof(glTDD._board));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	NormDatTurnEx(vector <float> &gh, int nbH, double w0)
{
	double sum = 0;
	int dim = gh.size();
	for (int i = 0; i < dim; i++)
		sum += gh[i];
	for (int i = 0; i < dim; i++)
		gh[i] /= sum;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
float	CreateStreetDatForMinNew(clStreetDat &dat, vector <float> *gh)
{
	ErrMessage("CreateStreetDatForMinNew ", "Не реализованная функция");
	tp2Float *val0 = new tp2Float[CN_STREETDAT_UNIT*CN_CARD_COL];
	tp2Float *val1 = new tp2Float[CN_STREETDAT_UNIT*CN_CARD_COL];
	ZeroMemory(val0, CN_STREETDAT_UNIT*CN_CARD_COL * sizeof(tp2Float));
	ZeroMemory(val1, CN_STREETDAT_UNIT*CN_CARD_COL * sizeof(tp2Float));
	float w = 0;
	for (int i0 = 0; i0 < CN_STREETDAT_UNIT; i0++)
	{
		int nb0 = glTDD.GetNbHand(0, i0);
		for (int i1 = 0; i1 < CN_STREETDAT_UNIT; i1++)
		{
			int nb1 = glTDD.GetNbHand(1, i1);
			float ww = gh[0][i0] * gh[1][i1];
			tp2Float *v0 = val0 + i0*CN_CARD_COL;
			tp2Float *v1 = val1 + i1*CN_CARD_COL;
			for (int k = 0; k < CN_CARD_COL; k++)
				if (glTDD._matrix.Element(i0, i1, k)._val2)
				{
					v0[k]._weight += ww;
					v1[k]._weight += ww;
					w += ww;
					int f0 = glTDD._matrix.Element(i0, i1, k)._val1;
					if (f0 == 1)
						v0[k]._force += ww;
					else if (f0 == -1)
						v1[k]._force += ww;
					else
					{
						v0[k]._force += ww / 2;
						v1[k]._force += ww / 2;
					}
				}
		}
	}
	dat._vectUnit[0].resize(CN_STREETDAT_UNIT);
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		dat._vectUnit[0][i]._nb = glTDD.GetNbHand(0, i);
		tp2Float *ptr = &val0[i*CN_CARD_COL];
		int k = 0;
		float sum = 0, wk = 0, sum2 = 0;
		for (; k < CN_CARD_COL; k++)
			if (ptr[k]._weight > DOUBLE_0)
			{
				sum += ptr[k]._force;
				wk += ptr[k]._weight;
				sum2 += ptr[k]._force*ptr[k]._force / ptr[k]._weight;
			}
		dat._vectUnit[0][i]._weight = wk / w;
		dat._vectUnit[0][i]._force = sum / wk;
		dat._vectUnit[0][i]._force50 = sqrt(sum2 / wk);
	}
	dat._vectUnit[1].resize(CN_STREETDAT_UNIT);
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		//glTrasser.WriteMessage(i);
		dat._vectUnit[1][i]._nb = glTDD.GetNbHand(1, i);
		tp2Float *ptr = &val1[i*CN_CARD_COL];
		int k = 0;
		float sum = 0, wk = 0, sum2 = 0;
		for (; k < CN_CARD_COL; k++)
			if (ptr[k]._weight > DOUBLE_0)
			{
				sum += ptr[k]._force;
				wk += ptr[k]._weight;
				sum2 += ptr[k]._force*ptr[k]._force / ptr[k]._weight;
			}
		dat._vectUnit[1][i]._weight = wk / w;
		dat._vectUnit[1][i]._force = sum / wk;
		dat._vectUnit[1][i]._force50 = sqrt(sum2 / wk);
	}
	delete val0;
	delete val1;
	dat.Copy50AndSort();
	return w;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CreateStreetDatForMin(clStreetDat &dat, vector <float> *gh)
{
	tpBooleanDeck &deck = glTDD._deck;

	tp2Float *val0 = new tp2Float[CN_STREETDAT_UNIT*CN_CARD_COL];
	tp2Float *val1 = new tp2Float[CN_STREETDAT_UNIT*CN_CARD_COL];
	ZeroMemory(val0, CN_STREETDAT_UNIT*CN_CARD_COL * sizeof(tp2Float));
	ZeroMemory(val1, CN_STREETDAT_UNIT*CN_CARD_COL * sizeof(tp2Float));
	float w = 0;
	for (int i0 = 0; i0 < CN_STREETDAT_UNIT; i0++)
	{
		int nb0 = glTDD.GetNbHand(0, i0);
		tpHand hand0(nb0);
		deck[hand0[0]] = false;
		deck[hand0[1]] = false;
		for (int i1 = 0; i1 < CN_STREETDAT_UNIT; i1++)
		{
			int nb1 = glTDD.GetNbHand(1, i1);
			tpHand hand1(nb1);
			deck[hand1[0]] = false;
			deck[hand1[1]] = false;
			float ww = gh[0][i0] * gh[1][i1];
			tp2Float *v0 = val0 + i0*CN_CARD_COL;
			tp2Float *v1 = val1 + i1*CN_CARD_COL;
			for (int k = 0; k < CN_CARD_COL; k++)
				if (deck[k])
				{
					v0[k]._weight += ww;
					v1[k]._weight += ww;
					w += ww;
					int f0 = glTDD.Force(0, k, i0), f1 = glTDD.Force(1, k, i1);
					if (f0 > f1)
						v0[k]._force += ww;
					else if (f1 > f0)
						v1[k]._force += ww;
					else
					{
						v0[k]._force += ww / 2;
						v1[k]._force += ww / 2;
					}
				}
			deck[hand1[0]] = true;
			deck[hand1[1]] = true;
		}
		deck[hand0[0]] = true;
		deck[hand0[1]] = true;
	}
	dat._vectUnit[0].resize(CN_STREETDAT_UNIT);
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		dat._vectUnit[0][i]._nb = glTDD.GetNbHand(0, i);
		tp2Float *ptr = &val0[i*CN_CARD_COL];
		int k = 0;
		float sum = 0, wk = 0, sum2 = 0;
		for (; k < CN_CARD_COL; k++)
			if (ptr[k]._weight > DOUBLE_0)
			{
				sum += ptr[k]._force;
				wk += ptr[k]._weight;
				sum2 += ptr[k]._force*ptr[k]._force / ptr[k]._weight;
			}
		dat._vectUnit[0][i]._weight = wk / w;
		dat._vectUnit[0][i]._force = sum / wk;
		dat._vectUnit[0][i]._force50 = sqrt(sum2 / wk);
	}
	dat._vectUnit[1].resize(CN_STREETDAT_UNIT);
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		//glTrasser.WriteMessage(i);
		dat._vectUnit[1][i]._nb = glTDD.GetNbHand(1, i);
		tp2Float *ptr = &val1[i*CN_CARD_COL];
		int k = 0;
		float sum = 0, wk = 0, sum2 = 0;
		for (; k < CN_CARD_COL; k++)
			if (ptr[k]._weight > DOUBLE_0)
			{
				sum += ptr[k]._force;
				wk += ptr[k]._weight;
				sum2 += ptr[k]._force*ptr[k]._force / ptr[k]._weight;
			}
		dat._vectUnit[1][i]._weight = wk / w;
		dat._vectUnit[1][i]._force = sum / wk;
		dat._vectUnit[1][i]._force50 = sqrt(sum2 / wk);
	}
	delete val0;
	delete val1;
	dat.Copy50AndSort();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::FindMinParam(clStreetDat &datIn)
{
	clStreetDat dat1;
	vector <float> ghEx[2], g0, g1;
	int dim00 = CnHand(0), dim11 = CnHand(1);
	if (dim00 != dim11)
	{
		ErrMessage("", " FindMinParam");
	}
	ghEx[0].resize(CN_STREETDAT_UNIT);
	ghEx[1].resize(CN_STREETDAT_UNIT);
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		ghEx[0][i] = _gh[0][glTDD.GetNbHand(0, i)];
		ghEx[1][i] = _gh[1][glTDD.GetNbHand(1, i)];
	}
	memcpy(dat1._stack, datIn._stack, sizeof(dat1._stack));
	dat1._pot = datIn._pot;
	CreateStreetDatForMin(dat1, ghEx);
	double min = dat1.Distance(datIn);
	double w0 = 0.1;
	int nb0 = 0, cn = 0, nb, nbPl;
	while (w0 > 0.001)
	{
		if (nb0 < CN_STREETDAT_UNIT)
		{
			nbPl = 0;
			nb = nb0;
		}
		else
		{
			nbPl = 1;
			nb = nb0 - CN_STREETDAT_UNIT;
		}
		vector <float> ghCur = ghEx[nbPl];
		ghEx[nbPl][nb] += w0;
		//NormDatTurnEx(ghEx[nbPl], nb, w0);
		TempNormVect(CN_STREETDAT_UNIT, &ghEx[nbPl][0], 1);
		CreateStreetDatForMin(dat1, ghEx);
		double d0 = dat1.Distance(datIn);
		if (d0 < min)
		{
			min = d0;
			cn = -1;
		}
		else
		{
			ghEx[nbPl] = ghCur;
			if (ghEx[nbPl][nb] >= w0)
			{
				ghEx[nbPl][nb] -= w0;
				//NormDatTurnEx(ghEx[nbPl], nb, -w0);
				TempNormVect(CN_STREETDAT_UNIT, &ghEx[nbPl][0], 1);
				CreateStreetDatForMin(dat1, ghEx);
				double d0 = dat1.Distance(datIn);
				if (d0 < min)
				{
					min = d0;
					cn = -1;
				}
				else
					ghEx[nbPl] = ghCur;
			}
		}
		nb0++;
		cn++;
		if (nb0 >= CN_STREETDAT_UNIT + CN_STREETDAT_UNIT)
			nb0 = 0;
		if (cn == CN_STREETDAT_UNIT + CN_STREETDAT_UNIT)
		{
			cn = 0;
			w0 /= 2;
		}
		g0 = ghEx[0];
		g1 = ghEx[1];
	}
	//CreateRiverDatForMin(dat1, ghEx);
	//double d0 = dat1.Distance(datIn);
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		_gh[0][glTDD.GetNbHand(0, i)] = ghEx[0][i];
		_gh[1][glTDD.GetNbHand(1, i)] = ghEx[1][i];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpStreetDatGH::operator +=(tpStreetDatGH &val)
{
	if (val._weight > DOUBLE_0)
	{
		float w0 = _weight + val._weight;
		_force = (_force*_weight + val._force*val._weight) / w0;
		_force50 = (_force50*_weight + val._force50*val._weight) / w0;
		_weight = w0;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int tpStreetDatGH::FindNbCenterElement(vector <tpStreetDatGH> &vect)
{
	int dim = vect.size();
	tpStreetDatGH res = { 0,0,0,0 };
	for (int i = 0; i < dim; i++)
	{
		res._force += vect[i]._weight*vect[i]._force;
		res._force50 += vect[i]._weight*vect[i]._force50;
		res._weight += vect[i]._weight;
	}
	if (res._weight < DOUBLE_0)
		return 0;
	res._force /= res._weight;
	res._force50 /= res._weight;
	int nb = 0;
	double dist = vect[0].DistanceSqr(res);
	for (int i = 1; i < dim; i++)
	{
		double d = vect[i].DistanceSqr(res);
		if (d < dist)
		{
			dist = d;
			nb = i;
		}
	}
	return nb;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CompactGH(double minW, int cnMin, clStreetDat *datIn)
{
	if (datIn == NULL)
	{
		clStreetDat dat;
		CreateStreetDat(dat);
		CompactGH(minW, cnMin, dat);
	}
	else
		CompactGH(minW, cnMin, *datIn);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootStreetTree::CompactGH(clInComeDataRoot *id, double minW, int cnMin, clStreetDat dat)
{
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		vector <tpStreetDatGH> &vect = dat._vectUnit[nbPl];
		while (true)
		{
			int dim = vect.size();
			//glTrasser.WriteMessage(dim);
			if (dim <= cnMin)
				break;
			int nb = 0;
			for (int i = 1; i < dim; i++)
				if (vect[i]._weight < vect[nb]._weight)
					nb = i;
			if (vect[nb]._weight > minW)
				break;
			int nb1 = (nb == 0) ? 1 : 0;
			float dist = vect[nb].Distance(vect[nb1]);
			for (int i = nb1 + 1; i < dim; i++)
			{
				float val = vect[nb].Distance(vect[i]);
				if (val < dist)
				{
					val = dist;
					nb1 = i;
				}
			}
			vect[nb1]._weight += vect[nb]._weight;
			vect.erase(vect.begin() + nb);
		}
		id->_gh[nbPl].ClearDat();
		int dim = vect.size();
		for (int i = 0; i < dim; i++)
			id->_gh[nbPl][vect[i]._nb] = vect[i]._weight;
		id->_gh[nbPl].Norm();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	CreateStreetDatRoot(clRoot &root, clStreetDat &dat)
{
	clRootStreetTree rr;
	rr._sit = root._sit;
	rr._gh[0] = root._gh[0];
	rr._gh[1] = root._gh[1];
	rr.CalcIndex();
	return rr.CreateStreetDat(dat);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	CompactGHTurn(clRoot &root, double val, int cnHand, int *nbH)
{
	clRootStreetTree rr;
	rr._sit = root._sit;
	rr._gh[0] = root._gh[0];
	rr._gh[1] = root._gh[1];
	rr.CalcIndex();
	rr.CompactGHQuickH(val, cnHand, nbH);

	root._gh[0] = rr._gh[0];
	root._gh[1] = rr._gh[1];
	root.CalcIndex();
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
