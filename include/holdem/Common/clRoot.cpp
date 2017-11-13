//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <math.h>
#include <time.h>

#include "C:\home\Holdem\Common\clRoot.h"
#include "c:\Home\Util\Sort.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clInComeDataRoot::SizeInFile()
{
	size_t size = sizeof(clSitHoldem);
	size += _gh[0].SizeInFile();
	size += _gh[1].SizeInFile();
	size += _gh[2].SizeInFile();
	return size;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	clInComeDataRoot::WriteFile(int handle)
{
	_write(handle, &_sit, sizeof(clSitHoldem));
	_gh[0].WriteFile(handle);
	_gh[1].WriteFile(handle);
	_gh[2].WriteFile(handle);
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int clInComeDataRoot::ReadFile(int handle)
{
	_read(handle, &_sit, sizeof(clSitHoldem));
	_gh[0].ReadFile(handle);
	_gh[1].ReadFile(handle);
	_gh[2].ReadFile(handle);
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clInComeDataRoot::operator = (clInComeDataRoot &gset)
{
	_sit = gset._sit;
	_gh[0] = gset._gh[0];
	_gh[1] = gset._gh[1];
	_gh[2] = gset._gh[2];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::operator = (clRoot &root)
{
	_tree = root._tree;
	_sit = root._sit;
	for (int i = 0; i < 3; i++)
	{
		_gh[i] = root._gh[i];
		_vectHand[i] = root._vectHand[i];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clRoot::SizeInFile()
{
	size_t size = clInComeDataRoot::SizeInFile();
	return size + _tree.SizeInFile();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int 	clRoot::WriteFile(int handle)
{
	clInComeDataRoot::WriteFile(handle);
	_tree.WriteFile(handle);
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int 	clRoot::ReadFile(int handle)
{
	clInComeDataRoot::ReadFile(handle);
	_tree.ReadFile(handle);
	_tree.ReestablishSit(_sit);
#ifdef NB_NODE
	int nb = 0;
	int cnNode = _tree.SetNbNode(nb);
#endif
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CreateTreeNLCompact()
{
	_tree.ClearAllBranch();
	_tree.AtrTree()->_sitA = _sit;
	CalcIndex();
	BuildTreeNLAuto(&_tree, _sit, _sit.NameTrade());
	int cnHand[] = { CnHand(0),CnHand(1),CnHand(2) };
	_tree.SetStartParam(cnHand);
	_tree.SetResSit();
#ifdef NB_NODE
	int nbN = 0;
	_tree.SetNbNode(nbN);
#endif
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CreateTreeNL1326()
{
	_tree.ClearAllBranch();
	_tree.AtrTree()->_sitA = _sit;
	CalcIndex();
	BuildTreeNLAuto(&_tree, _sit, _sit.NameTrade());
	int cnHand[] = { ALL_CN_HAND,ALL_CN_HAND,ALL_CN_HAND };
	_tree.SetStartParam(cnHand);
	_tree.SetResSit();
#ifdef NB_NODE
	int nbN = 0;
	_tree.SetNbNode(nbN);
#endif
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::InitDeck()
{
	for (int i = 0; i < _sit.CnCardTable(); i++)
	{
		_cards[i] = _sit.CardsTable()[i];
		_deck.Skip(_cards[i]);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CalcIndex(double min)
{
	if (_sit.CnPlayer() == 2)
	{
		_vectHand[0].clear();
		_vectHand[1].clear();
		_vectHand[0].reserve(ALL_CN_HAND);
		_vectHand[1].reserve(ALL_CN_HAND);
		for (int i = 0; i < ALL_CN_HAND; i++)
		{
			if (_gh[0][i] > min)
				_vectHand[0].push_back(i);
			if (_gh[1][i] > min)
				_vectHand[1].push_back(i);
		}
	}
	else
	{
		_vectHand[0].clear();
		_vectHand[1].clear();
		_vectHand[2].clear();
		_vectHand[0].reserve(ALL_CN_HAND);
		_vectHand[1].reserve(ALL_CN_HAND);
		_vectHand[2].reserve(ALL_CN_HAND);
		for (int i = 0; i < ALL_CN_HAND; i++)
		{
			if (_gh[0][i] > min)
				_vectHand[0].push_back(i);
			if (_gh[1][i] > min)
				_vectHand[1].push_back(i);
			if (_gh[2][i] > min)
				_vectHand[2].push_back(i);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		clRoot::NbCls(int nbHero, int nbHand)
{
	int dim = CnHand(nbHero);
	for (int i = 0; i < dim; i++)
		if (nbHand == NbHand(nbHero, i))
			return i;
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CompactGHSimple(int cnMin)
{
	int cnPl = _sit.CnPlayer();
	int arr[ALL_CN_HAND];
	for (int nbPl = 0; nbPl < cnPl; nbPl++)
	{
		clHandsGroupEx gh = _gh[nbPl];
		SortShellIndDef(ALL_CN_HAND, &gh[0], arr);
		_gh[nbPl].ClearDat();
		for (int i = 0; i < ALL_CN_HAND; i++)
		{
			int nb = ALL_CN_HAND - 1 - i;
			if (i<cnMin)
				_gh[nbPl][arr[nb]] = gh[nb];
		}
		_gh[nbPl].Norm();
	}
	CalcIndex();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CreateCompactGH(vector <tpFloat> *ghF)
{
	int dim = CnHand(0);
	ghF[0].resize(dim);
	for (int i = 0; i < dim; i++)
		ghF[0][i] = _gh[0][NbHand(0, i)];
	dim = CnHand(1);
	ghF[1].resize(dim);
	for (int i = 0; i < dim; i++)
		ghF[1][i] = _gh[1][NbHand(1, i)];
	if (_sit.CnPlayer() > 2)
	{
		dim = CnHand(2);
		ghF[2].resize(dim);
		for (int i = 0; i < dim; i++)
			ghF[2][i] = _gh[2][NbHand(2, i)];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CreateCompactGH(vector <float> *ghF)
{
	int dim = CnHand(0);
	ghF[0].resize(dim);
	for (int i = 0; i < dim; i++)
		ghF[0][i] = _gh[0][NbHand(0, i)];
	dim = CnHand(1);
	ghF[1].resize(dim);
	for (int i = 0; i < dim; i++)
		ghF[1][i] = _gh[1][NbHand(1, i)];
	if (_sit.CnPlayer() > 2)
	{
		dim = CnHand(2);
		ghF[2].resize(dim);
		for (int i = 0; i < dim; i++)
			ghF[2][i] = _gh[2][NbHand(2, i)];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CreateCompactGH(clHandsGroupEx *ghF)
{
	int dim = CnHand(0);
	ghF[0].SetDim(dim);
	for (int i = 0; i < dim; i++)
		ghF[0][i] = _gh[0][NbHand(0, i)];
	dim = CnHand(1);
	ghF[1].SetDim(dim);
	for (int i = 0; i < dim; i++)
		ghF[1][i] = _gh[1][NbHand(1, i)];
	if (_sit.CnPlayer() > 2)
	{
		dim = CnHand(2);
		ghF[1].SetDim(dim);
		for (int i = 0; i < dim; i++)
			ghF[2][i] = _gh[2][NbHand(2, i)];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CalcWeightBranchStrat()
{
	if (_sit.CnPlayer() == 2)
	{
		clHandsGroupEx ghF[3], *gh0[3];
		CreateCompactGH(ghF);
		gh0[0] = &ghF[0];
		gh0[1] = &ghF[1];
		//gh0[2] = &ghF[2];
		CalcWeightStrat2Pl(&_tree, gh0);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CalcWeightStrat2Pl(clHoldemTree *node, clHandsGroupEx **gh)
{
	if (node->CnBranch() == 0)
		return;
	else if (node->CnBranch() == 1)
	{
		node->Branch(0)->_weight = 1;
		CalcWeightStrat2Pl(node->Branch(0), gh);
	}
	else
	{
		int ind = node->Indicator(), opp = (ind == 0) ? 1 : 0;
		int dim = CnHand(ind), dimOpp = CnHand(opp);
		tpFloat *w = new tpFloat[node->CnBranch()*dim];
		ZeroMemory(w, node->CnBranch()*dim * sizeof(tpFloat));
		node->AtrTree()->GetAllWeightStrat(node->CnBranch(), w);
		clHandsGroupEx ghCur(dim), *gh0[2];
		gh0[ind] = &ghCur;
		gh0[opp] = gh[opp];
		tpFloat sum = 0, w0[MAX_CN_BRANCH];
		for (int k = 0; k < node->CnBranch(); k++)
		{
			for (int i = 0; i < dim; i++)
				ghCur[i] = (*(gh[ind]))[i] * w[i*node->CnBranch() + k];
			CalcWeightStrat2Pl(node->Branch(k), gh0);
			w0[k] = 0;
			for (int iH = 0; iH < dim; iH++)
				for (int i1 = 0; i1 < dimOpp; i1++)
					if (!HandCrossHandNB(NbHand(ind, iH), NbHand(opp, i1)))
						w0[k] += ghCur[iH] * gh[opp]->_vect[i1];
			sum += w0[k];
		}
		if (sum > DOUBLE_0)
			for (int i = 0; i < node->CnBranch(); i++)
				node->Branch(i)->_weight = w0[i] / sum;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::RecalcGH(clHoldemTree *node, clHandsGroupEx *gh)
{
	if (_sit.CnPlayer() == 2)
	{
		vector <tpFloat> gh0[2];
		CreateCompactGH(gh0);
		RecalcGH2Pl(node, gh0);
		int dim0 = CnHand(0), dim1 = CnHand(1);
		gh[0].SetDim(ALL_CN_HAND);
		gh[0].ClearDat();
		for (int i = 0; i < dim0; i++)
			gh[0][NbHand(0, i)] = gh0[0][i];
		gh[1].SetDim(ALL_CN_HAND);
		gh[1].ClearDat();
		for (int i = 0; i < dim1; i++)
			gh[1][NbHand(1, i)] = gh0[1][i];
		gh[0].Norm();
		gh[1].Norm();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::RecalcGH2Pl(clHoldemTree *node, vector <tpFloat> *gh)
{
	if (node->_parent != NULL)
	{
		clHoldemTree *parent = node->Parent();
		RecalcGH2Pl(parent, gh);
		int nb = parent->FindNbBranch(node);
		parent->ChangeGH(nb, gh[parent->Indicator()]);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		CutAndNorm(vector <tpDisNode> &arr, double minW)
{
	int k = 0;
	int cn = arr.size();
	for (int i = 0; i < cn; i++)
	{
		int j = 0;
		if (arr[i]._dis._act == ACT_FOLD || arr[i]._dis._act == ACT_CALL || arr[i]._dis._act == ACT_ALL_IN || arr[i]._dis._act == ACT_CHECK)
		{
			for (; j < k; j++)
				if (arr[i]._dis._act == arr[j]._dis._act)
					break;
		}
		else
			for (; j < k; j++)
				if (arr[i]._dis == arr[j]._dis)
					break;
		if (j == k)
			arr[k++] = arr[i];
		else
			arr[j]._weight += arr[i]._weight;
	}

	double sum = 0;
	cn = k; k = 0;
	for (int i = 0; i < cn; i++)
		if (arr[i]._weight > minW)
		{
			sum += arr[i]._weight;
			arr[k++] = arr[i];
		}
	if (sum > DOUBLE_0)
		for (int i = 0; i < k; i++)
			arr[i]._weight /= sum;
	arr.resize(k);
	return k;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis clRoot::GetDis(clSitHoldem &sit, vector <tpDisNode> &vect)
{
	enNameTrade trade = sit.NameTrade();
	int nbAct;
	if (trade == TRADE_TURN)
		nbAct = sit.NbFirstAct(ACT_DELTURN);
	else if (trade == TRADE_RIVER)
		nbAct = sit.NbFirstAct(ACT_DELRIVER);
	else if (trade == TRADE_FLOP)
		nbAct = sit.NbFirstAct(ACT_DELFLOP);
	else if (trade == TRADE_PREFLOP)
		nbAct = sit.NbFirstAct(ACT_DELCARD);
	nbAct++;
	vector <stNodeW> vn;
	_tree.FindSitNode(sit.CnAct() - nbAct, sit, nbAct, vn, 1);
	int nbHand = sit.PlayerHand(sit.Indicator()).NbHandAll();
	int nbCls = NbCls(sit.Indicator(), nbHand);
	for (int i = 0; i < vn.size(); i++)
	{
		clHoldemTree *node = (clHoldemTree *)vn[i]._ptr;
		clCalcParam cls = node->AtrTree()->CalcParam(nbCls);
		tpFloat w[MAX_CN_BRANCH];
		cls.GetWeightStrat(node->CnBranch(), w);
		for (int j = 0; j < node->CnBranch(); j++)
		{
			tpDisNode dn;
			dn._dis = node->Branch(j)->GetDis();
			dn._weight = w[j] * vn[i]._weight;
			dn._ev = cls.AbsEV(j);
			vect.push_back(dn);
		}
	}

	int cn = CutAndNorm(vect, 0.03);
	int nbB = cn - 1;
	tpFloat rVal = (tpFloat)rand() / RAND_MAX;
	for (int i = 0; i < cn; i++)
		if (vect[i]._weight > rVal)
		{
			nbB = i;
			break;
		}
		else
			rVal -= vect[i]._weight;
	tpDis dis = sit.CreateDis(vect[nbB]._dis);
	for (int i = 0; i < cn; i++)
		for (int j = i + 1; j < cn; j++)
			if (vect[i]._weight < vect[j]._weight)
			{
				tpDisNode temp = vect[i];
				vect[i] = vect[j];
				vect[j] = temp;
			}
	return dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRoot::CalcAbsWeightGH(vector <float> *weight)
{
	int dim0 = CnHand(0), dim1 = CnHand(1);
	weight[0].resize(ALL_CN_HAND);
	ZeroMemory(&weight[0][0], ALL_CN_HAND * sizeof(float));
	weight[1] = weight[0];
	float sum = 0;
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
			{
				double ww = _gh[0][nb0] * _gh[1][nb1];
				weight[0][nb0] += ww;
				weight[1][nb1] += ww;
				sum += ww;
			}
		}
	}
	if (sum > DOUBLE_0)
	{
		for (int i0 = 0; i0 < dim0; i0++)
		{
			int nb0 = NbHand(0, i0);
			weight[0][nb0] /= sum;
		}
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			weight[1][nb1] /= sum;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	ReFormatData(tpIncomeFloatData *dat, clInComeDataRoot *root)
{
	dat->_sit = root->_sit;
	int dim = root->_gh[0].Dimension();
	memcpy(dat->_gh[0], &root->_gh[0][0], dim * sizeof(float));
	dim = root->_gh[1].Dimension();
	memcpy(dat->_gh[1], &root->_gh[1][0], dim * sizeof(float));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
