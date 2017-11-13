#include <math.h>
#include <time.h>

#include "c:\home\card\naborCards.h"
#include "c:\home\Util\TemplateFileRec.h"
#include "c:\Home\Util\Sort.h"
#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\RiverTreeNew.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::operator =  (clRootRiverTree &root)
{
	*((clGentlemanSet*)this) = *((clGentlemanSet*)&root);
	_tree = root._tree;
	CalcIndexHands();
	CalcForce();
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clRootRiverTree::SizeInFile()
{
	size_t size = clGentlemanSet::SizeInFile();
	return size + _tree.SizeInFile();
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
int 	clRootRiverTree::WriteFile(int handle)
{
	clGentlemanSet::WriteFile(handle);
	_tree.WriteFile(handle);
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
int 	clRootRiverTree::ReadFile(int handle)
{
	clGentlemanSet::ReadFile(handle);
	_tree.ReadFile(handle);
	_tree.ReestablishSit(_sit);
	CalcIndexHands();
	CalcForce();
#ifdef NB_NODE
	int nb = 1;
	_tree.SetNbNode(nb);
#endif
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcTree(size_t cnCalc)
{
	tpTreePlayerInfo info;
	int cn0 = CnHand(0), cn1 = CnHand(1);
	for (int i0 = 0; i0 < cnCalc;)
	{
		int r0 = random(cn0);
		int r1 = random(cn1);
		int nb0 = NbHand(0, r0);
		int nb1 = NbHand(1, r1);
		if (HandCrossHandNB(nb0, nb1))
			continue;

		info._nbCls[TRADE_RIVER][0] = nb0;
		info._nbCls[TRADE_RIVER][1] = nb1;
		info._handForce[0] = _force[nb0];
		info._handForce[1] = _force[nb1];
		tpFloat k0 = (i0 % 1000) ? 0 : 0.1;
		_tree.CalcNesh(0, &info, _gh[0][nb0], _gh[1][nb1], k0);
		_tree.CalcNesh(1, &info, _gh[1][nb1], _gh[0][nb0], k0);
		i0++;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CreateTreeNL()
{
	_tree.ClearAllBranch();
	_tree.AtrTree()->_sitA = _sit;
	CalcIndexHands();
	CalcForce();
	BuildTreeNLAuto(&_tree, _sit);
	int cnHand[] = { ALL_CN_HAND,ALL_CN_HAND };
	_tree.SetStartParam(cnHand);
	_tree.SetResSit();
#ifdef NB_NODE
	int nbN = 0;
	_tree.SetNbNode(nbN);
#endif
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcIndexHands()
{
	_vectHand[0].clear();
	_vectHand[1].clear();
	_vectHand[0].reserve(ALL_CN_HAND);
	_vectHand[1].reserve(ALL_CN_HAND);
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		if (_gh[0][i] > DOUBLE_0)
			_vectHand[0].push_back(i);
		if (_gh[1][i] > DOUBLE_0)
			_vectHand[1].push_back(i);
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcForce()
{
	ZeroMemory(_force, sizeof(_force));
	tpBooleanDeck deck;
	tpCard card[5];
	for (int i = 0; i < 5; i++)
	{
		card[i] = _sit.CardsTable()[i];
		deck.Skip(card[i]);
	}
	if (glVectRiverHandGame.size() == 0)
	{
		clGamePoker game;
		for (int i = 0; i < CN_CARD_COL; i++)
			if (deck[i])
				for (int j = i + 1; j < CN_CARD_COL; j++)
					if (deck[j])
					{
						clHand hand; hand.SetHand(j, i);
						int nb = hand.NbHandAll();
						game.GameHoldemQ(hand, _sit.CardsTable());
						tpGame24 g0(game);
						_force[nb] = g0.ToInt();
					}
	}
	else
	{
		for (int i = 0; i < CN_CARD_COL; i++)
			if (deck[i])
				for (int j = i + 1; j < CN_CARD_COL; j++)
					if (deck[j])
					{
						tpHand hand; hand.SetHand(j, i);
						int nb = hand.NbHandAll();
						int nbT, nbR = GetTurnAndRiverHandIndex(card, hand, nbT);
						_force[nb] = glVectRiverHandGame[nbR].ToInt();
					}
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
bool FuncSortArr(tpRiverGHUnit val1, tpRiverGHUnit val2) { return val1._wf < val2._wf; }
void	clRootRiverTree::CreateRiverDat(clRiverDat &dat)
{
	int dim0 = CnHand(0), dim1 = CnHand(1);
	dat._vectUnit[0].resize(dim0);
	dat._vectUnit[1].resize(dim1);
	for (int i = 0; i < dim0; i++)
		dat._vectUnit[0][i] = { NbHand(0,i),0,0 };
	for (int i = 0; i < dim1; i++)
		dat._vectUnit[1][i] = { NbHand(1,i),0,0 };

	tpFloat w = 0;
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
			{
				tpFloat ww = _gh[0][nb0] * _gh[1][nb1];
				dat._vectUnit[0][i0]._weight += ww;
				dat._vectUnit[1][i1]._weight += ww;
				if (_force[nb0] > _force[nb1])
					dat._vectUnit[0][i0]._wf += ww;
				else  if (_force[nb0] == _force[nb1])
				{
					dat._vectUnit[0][i0]._wf += ww / 2;
					dat._vectUnit[1][i1]._wf += ww / 2;
				}
				else
					dat._vectUnit[1][i1]._wf += ww;
				w += ww;
			}
		}
	}
	for (int i = 0; i < dim0; i++)
		if (dat._vectUnit[0][i]._weight > DOUBLE_0)
		{
			dat._vectUnit[0][i]._wf /= dat._vectUnit[0][i]._weight;
#ifdef DISTANCE_SQRT_EV
			dat._vectUnit[0][i]._wf *= dat._vectUnit[0][i]._wf;
#endif
			dat._vectUnit[0][i]._weight /= w;
		}
	for (int i = 0; i < dim1; i++)
		if (dat._vectUnit[1][i]._weight > DOUBLE_0)
		{
			dat._vectUnit[1][i]._wf /= dat._vectUnit[1][i]._weight;
#ifdef DISTANCE_SQRT_EV
			dat._vectUnit[1][i]._wf*dat._vectUnit[1][i]._wf;
#endif
			dat._vectUnit[1][i]._weight /= w;
		}

	SortShell(dim0, &dat._vectUnit[0][0], FuncSortArr);

	SortShell(dim1, &dat._vectUnit[1][0], FuncSortArr);

}
//---------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------
float	clRiverDat::DistanceGH(clRiverDat &dat)
{
	float res = 0;
	for (int k = 0; k < 2; k++)
	{
		int i = 0, j = 0;
		int dimI = dat._vectUnit[k].size(), dimJ = _vectUnit[k].size();
		float ost = this->_vectUnit[k][j]._weight, weight = dat._vectUnit[k][i]._weight;
		//float sum = weight;
		while (i < dimI)
		{
			if (weight < ost)
			{
				float val = _vectUnit[k][j]._wf - dat._vectUnit[k][i]._wf;
				res += val*val*weight;
				ost -= weight;
				i++;
				if (i == dimI)
					break;
				weight = dat._vectUnit[k][i]._weight;
				if (weight < DOUBLE_0)
					break;
			}
			else
			{
				float val = _vectUnit[k][j]._wf - dat._vectUnit[k][i]._wf;
				res += val*val*ost;
				weight -= ost;
				j++;
				if (j == dimJ)
					break;
				ost = _vectUnit[k][j]._weight;
			}
		}
	}
	return sqrt(res / 2);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
