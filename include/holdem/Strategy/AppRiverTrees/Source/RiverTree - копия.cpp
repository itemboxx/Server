#include <math.h>
#include <time.h>

#include "c:\home\card\naborCards.h"
#include "C:\home\holdem\Strategy\AppRiverTrees\Source\CalcResTable.h"
#include "C:\home\holdem\Strategy\AppRiverTrees\Source\RiverTree.h"
#include "c:\home\Util\TemplateFileRec.h"
#include "c:\Home\Util\Sort.h"
#include "c:\home\util\Clasters.h"
#include "c:\home\util\AnyType.h"

#define	CN_RIVERDAT_UNIT	78
class clRiverDatDat
{
public:
	void	Init();
	int GetNbHand(int pos, int i) { return _nbHand[pos][i]; }

	int _nbHand[2][CN_RIVERDAT_UNIT];
	clCard _board[5];
} glRDD;

void	BuildTreeNLAutoA(clHoldemTree *tree, clSitHoldem &sit);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	InitRiverDatDat() { glRDD.Init(); }
struct tpSChe
{
	int _nbH;
	clGamePoker _game;
};
int *GetNbRiverDatDat(int nb) { return glRDD._nbHand[nb]; }

bool SortHands(tpSChe v1, tpSChe v2) { return v1._game < v2._game; }
void	clRiverDatDat::Init()
{
	_board[0] = Card(C_2, S_C);
	_board[1] = Card(C_3, S_C);
	_board[2] = Card(C_5, S_C);
	_board[3] = Card(C_7, S_S);
	_board[4] = Card(C_8, S_S);
	tpSChe s1[ALL_CN_HAND / 2], s2[ALL_CN_HAND / 2];
	int cn = 0;
	for (int c1 = C_3; c1 <= C_A; c1++)
		for (int c2 = C_2; c2 < c1; c2++)
		{
			clHand hand;
			hand.SetHand(Card((enFace)c1, S_H), Card((enFace)c2, S_H));
			s1[cn]._nbH = hand.NbHandAll();
			s1[cn]._game.GameHoldemQ(hand, _board);
			hand.SetHand(Card((enFace)c1, S_D), Card((enFace)c2, S_D));
			s2[cn]._nbH = hand.NbHandAll();
			s2[cn]._game = s1[cn]._game;
			cn++;
		}
	SortShell(cn, s1, SortHands);
	SortShell(cn, s2, SortHands);
	for (int i = 0; i < cn; i++)
	{
		if (i)
			if (s1[i - 1]._game == s1[i]._game)
				ErrMessage("", "Error");
		_nbHand[0][i] = s1[i]._nbH;
		_nbHand[1][i] = s2[i]._nbH;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat	clRootRiverTree::NodeResult(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <tpFloat> &ev)
{
	tpFloat res = 0;
	ZeroMemory(&ev[0], ev.size() * sizeof(tpFloat));
	int opp = (nbHero == 0) ? 1 : 0;
	int dim = CnHand(nbHero), dimOpp = CnHand(opp);
	if (nbHero == 0)
		for (int iH = 0; iH < dim; iH++)
		{
			int nbH = NbHand(nbHero, iH);
			for (int iOpp = 0; iOpp < dimOpp; iOpp++)
			{
				int nbOpp = NbHand(opp, iOpp);
				if (HandCrossHandNB(nbH, nbOpp))
					continue;
				int *ptr = (int *)&node->AtrTree()->_param[0];
				int f0 = _force[nbH], f1 = _force[nbOpp];
				if (f0 == f1)
					ptr += 2;
				else if (f0 < f1)
					ptr += 4;
				ev[iH] += ptr[0] * kOpp[iOpp];
			}
		}
	else
		for (int iH = 0; iH < dim; iH++)
		{
			int nbH = NbHand(nbHero, iH);
			for (int iOpp = 0; iOpp < dimOpp; iOpp++)
			{
				int nbOpp = NbHand(opp, iOpp);
				if (HandCrossHandNB(nbH, nbOpp))
					continue;
				int *ptr = (int *)&node->AtrTree()->_param[0];
				int f1 = _force[nbH], f0 = _force[nbOpp];
				if (f0 == f1)
					ptr += 2;
				else if (f0 < f1)
					ptr += 4;
				ev[iH] += ptr[1] * kOpp[iOpp];
			}
		}
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat	clRootRiverTree::AddResEVToNode(clHoldemTree *node, int nbHero, int cnB, vector <tpFloat> &resEV, tpFloat *w, vector <tpFloat> *evCur, vector <tpFloat> &kH, vector <tpFloat> &kOpp)
{
	tpFloat res0 = 0;
	int opp = (nbHero == 0) ? 1 : 0;
	int dim = (int)kH.size(), dimOpp = (int)kOpp.size();
	for (int i = 0; i < dim; i++)
	{
		int nbH = NbHand(nbHero, i);
		tpFloat ev[MAX_CN_BRANCH];
		for (int k = 0; k < cnB; k++)
			ev[k] = evCur[k][i];
		tpFloat kOpp1 = 0;
		for (int i1 = 0; i1 < dimOpp; i1++)
		{
			int nbOpp = NbHand(opp, i1);
			if (!HandCrossHandNB(nbH, nbOpp))
				kOpp1 += kOpp[i1];
		}

		clCalcParam cls = node->AtrTree()->CalcParam(i);
		cls.AddResEV(cnB, resEV[i], w + i*cnB, ev, kH[i], kOpp1);
	}
	return res0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat	clRootRiverTree::CalcNeshA(clHoldemTree *node, int nbHero, int nbCls, tpFloat kH, vector <tpFloat> &kOpp)
{
	int dim = (int)kOpp.size();
	float sumOpp = 0;
	for (int i = 0; i < dim; i++)
		sumOpp += kOpp[i];
	node->AtrTree()->_statCn += kH*sumOpp;

	int ind = node->Indicator();
	if (node->EndedNode())
	{
		tpFloat res = 0;
		int f3[3], fH = _force[NbHand(nbHero, nbCls)];
		if (nbHero == 0)
		{
			f3[0] = fH;
			for (int i = 0; i < dim; i++)
				if (kOpp[i] > DOUBLE_0)
				{
					f3[1] = _force[NbHand(1, i)];
					res += node->AtrTree()->Result(f3, nbHero)*kOpp[i];
				}
		}
		else
		{
			f3[1] = fH;
			for (int i = 0; i < dim; i++)
				if (kOpp[i] > DOUBLE_0)
				{
					f3[0] = _force[NbHand(0, i)];
					res += node->AtrTree()->Result(f3, nbHero)*kOpp[i];
				}
		}
		return res;
	}

	tpFloat res = 0;
	if (ind == nbHero)
	{
		tpFloat w[MAX_CN_BRANCH];
		clCalcParam cls = node->AtrTree()->CalcParam(nbCls);
		cls.GetWeight(node->CnBranch(), w);
		tpFloat ev[MAX_CN_BRANCH];
		for (int k = 0; k < node->CnBranch(); k++)
		{
			ev[k] = CalcNeshA(node->Branch(k), nbHero, nbCls, kH*w[k], kOpp);
			res += ev[k] * w[k];
		}
		cls.AddResEV(node->CnBranch(), res, w, ev, kH, sumOpp);
	}
	else
	{
		tpFloat *w = new tpFloat[node->CnBranch()*ALL_CN_HAND];
		ZeroMemory(w, node->CnBranch()*ALL_CN_HAND * sizeof(tpFloat));
		node->AtrTree()->GetAllWeight(node->CnBranch(), w);

		for (int k = 0; k < node->CnBranch(); k++)
		{
			vector <tpFloat> kk = kOpp;
			for (int i = 0; i < dim; i++)
				kk[i] *= w[i*node->CnBranch() + k];
			tpFloat f0 = CalcNeshA(node->Branch(k), nbHero, nbCls, kH, kk);
			res += f0;
		}
		delete w;
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcNeshATest(clHoldemTree *node, int nbHero, int nbCls, tpFloat kH, vector <tpFloat> &kOpp, vector <tpFloat> &ev)
{
	int dim = (int)kOpp.size();
	float sumOpp = 0;
	for (int i = 0; i < dim; i++)
		sumOpp += kOpp[i];
	node->AtrTree()->_statCn += kH*sumOpp;

	int ind = node->Indicator();
	if (node->EndedNode())
	{
		int f3[3], fH = _force[NbHand(nbHero, nbCls)];
		if (nbHero == 0)
		{
			f3[0] = fH;
			for (int i = 0; i < dim; i++)
				if (kOpp[i] > DOUBLE_0)
				{
					f3[1] = _force[NbHand(1, i)];
					ev[i] += node->AtrTree()->Result(f3, nbHero)*kOpp[i];
				}
		}
		else
		{
			f3[1] = fH;
			for (int i = 0; i < dim; i++)
				if (kOpp[i] > DOUBLE_0)
				{
					f3[0] = _force[NbHand(0, i)];
					ev[i] += node->AtrTree()->Result(f3, nbHero)*kOpp[i];
				}
		}
		return;
	}

	ZeroMemory(&ev[0], dim * sizeof(tpFloat));
	if (ind == nbHero)
	{
		tpFloat w[MAX_CN_BRANCH], ev0[MAX_CN_BRANCH];
		clCalcParam cls = node->AtrTree()->CalcParam(nbCls);
		cls.GetWeight(node->CnBranch(), w);
		vector <tpFloat> evCur[MAX_CN_BRANCH];
		for (int k = 0; k < node->CnBranch(); k++)
		{
			evCur[k].resize(dim);
			CalcNeshATest(node->Branch(k), nbHero, nbCls, kH*w[k], kOpp, evCur[k]);
			for (int i = 0; i < dim; i++)
				ev[i] += evCur[k][i] * w[k];
		}
		for (int i = 0; i < dim; i++)
		{
			for (int k = 0; k < node->CnBranch(); k++)
				ev0[k] = evCur[k][i];
			cls.AddResEV(node->CnBranch(), ev[i], w, ev0, kH, kOpp[i]);
		}
	}
	else
	{
		tpFloat *w = new tpFloat[node->CnBranch()*ALL_CN_HAND];
		ZeroMemory(w, node->CnBranch()*ALL_CN_HAND * sizeof(tpFloat));
		node->AtrTree()->GetAllWeight(node->CnBranch(), w);

		for (int k = 0; k < node->CnBranch(); k++)
		{
			vector <tpFloat> kk = kOpp, evCur;
			for (int i = 0; i < dim; i++)
				kk[i] *= w[i*node->CnBranch() + k];
			CalcNeshATest(node->Branch(k), nbHero, nbCls, kH, kk, evCur);
			for (int i = 0; i < dim; i++)
				ev[i] += evCur[i];
		}
		delete w;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat	clRootRiverTree::CalcNeshA(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <tpFloat> &ev)
{
	if (node->EndedNode())
		return NodeResult(node, nbHero, kH, kOpp, ev);

	int dim = node->AtrTree()->CnClasters();
	int dimH = CnHand(nbHero);
	int ind = node->Indicator();
	tpFloat res = 0;
	tpFloat *w = new tpFloat[node->CnBranch()*ALL_CN_HAND];
	ZeroMemory(w, node->CnBranch()*ALL_CN_HAND * sizeof(tpFloat));
	ZeroMemory(&ev[0], ev.size() * sizeof(tpFloat));
	node->AtrTree()->GetAllWeight(node->CnBranch(), w);
	if (ind == nbHero)
	{
		vector <tpFloat> evCur[MAX_CN_BRANCH];
		for (int k = 0; k < node->CnBranch(); k++)
		{
			vector <tpFloat> kk = kH;
			evCur[k].resize(dimH);
			for (int i = 0; i < dim; i++)
				kk[i] *= w[i*node->CnBranch() + k];
			CalcNeshA(node->Branch(k), nbHero, kk, kOpp, evCur[k]);

			//vector <tpFloat> ev0 = evCur[k];

			for (int i = 0; i < dimH; i++)
				ev[i] += evCur[k][i] * w[i*node->CnBranch() + k];
		}
		AddResEVToNode(node, nbHero, node->CnBranch(), ev, w, evCur, kH, kOpp);
	}
	else
	{
		vector <tpFloat> evCur(dimH);
		for (int k = 0; k < node->CnBranch(); k++)
		{
			vector <tpFloat> kk = kOpp;
			for (int i = 0; i < dim; i++)
				kk[i] *= w[i*node->CnBranch() + k];
			CalcNeshA(node->Branch(k), nbHero, kH, kk, evCur);
			for (int i = 0; i < dimH; i++)
				ev[i] += evCur[i];
			int a = 0;
		}
	}
	delete w;
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat clRootRiverTree::CalcAntiStratEV(clHoldemTree *node, int nbHero, int nbCls, clHandsGroupEx &gh)
{
	if (node->EndedNode())
	{
		tpFloat res = 0;
		int opp = (nbHero == 0) ? 1 : 0, nb = NbHand(nbHero, nbCls);
		size_t dim = CnHand(opp);
		for (int i = 0; i < dim; i++)
		{
			int nbOpp = NbHand(opp, i);
			int f[3] = { _force[nb], _force[nbOpp] };
			if (nbHero == 1) { f[0] = _force[nbOpp]; f[1] = _force[nb]; }
			res += node->AtrTree()->Result(f, nbHero)*gh[nbOpp];
		}
		return  res;
	}
	int ind = node->Indicator();
	if (ind == -1)
	{
		if (node->CnBranch() == 1)
			return CalcAntiStratEV(node->Branch(0), nbHero, nbCls, gh);
		glTrasser.WriteMessage((clAnsiString)"End Tree");
		return 0;
	}
	tpFloat w[MAX_CN_BRANCH];

	if (nbHero == ind)
	{
		for (int k = 0; k < node->CnBranch(); k++)
			w[k] = CalcAntiStratEV(node->Branch(k), nbHero, nbCls, gh);
		int nbMax = 0;
		for (int k = 1; k < node->CnBranch(); k++)
			if (w[k] > w[nbMax])
				nbMax = k;
		return w[nbMax];
	}
	else
	{
		clHandsGroupEx gh0[MAX_CN_BRANCH];
		for (int j = 0; j < node->CnBranch(); j++)
		{
			gh0[j]._vect.resize(ALL_CN_HAND);
			gh0[j].ClearDat();
		}
		size_t dim = CnHand(ind);
		for (int i = 0; i < dim; i++)
		{
			int nb = NbHand(ind, i);
			clCalcParam cls = node->AtrTree()->CalcParam(i);
			cls.GetWeightStrat(node->CnBranch(), w);
			for (int j = 0; j < node->CnBranch(); j++)
				gh0[j][nb] = float(gh[nb] * w[j]);
		}
		tpFloat res = 0;
		for (int k = 0; k < node->CnBranch(); k++)
			res += CalcAntiStratEV(node->Branch(k), nbHero, nbCls, gh0[k]);
		return res;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*tpFloat clRiverTree::CalcQAntiStratEV(clRootRiverTree *root, int nbHero, int nbCls, clHandsGroupEx &gh)
{
	if (EndedNode())
	{
		glTrasser.WriteMessage((clAnsiString)"End Tree");
		return 0;
	}
	int ind = Indicator();
	if (ind == -1)
	{
		glTrasser.WriteMessage((clAnsiString)"End Tree");
		return 0;
	}
	tpFloat w[MAX_CN_BRANCH];

	if (nbHero == ind)
	{
		clCalcParam cls = AtrTree()->CalcParam(nbCls);
		for (int k = 0; k < CnBranch(); k++)
			w[k] = cls.AbsEV(k);
		int nbMax = 0;
		for (int k = 1; k < CnBranch(); k++)
			if (w[k] > w[nbMax])
				nbMax = k;
		return w[nbMax];
	}
	else
	{
		tpFloat w0 = 0, ev0 = 0;
		int dim = root->CnHand(ind);
		for (int i = 0; i < dim; i++)
		{
			int nbH = root->NbHand(ind, i);
			clCalcParam cls = AtrTree()->CalcParam(i);
			cls.GetWeightStrat(CnBranch(), w);
			tpFloat ev = 0;
			for (int k = 0; k < CnBranch(); k++)
				ev += Branch(k)->CalcQAntiStratEV(root, nbHero, nbCls, gh)*w[k];
			ev0 += ev*gh[nbH];
			w0 += gh[nbH];
		}
		return (w0 > DOUBLE_0) ? ev0 / w0 : 0;
	}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::operator =  (clRootRiverTree &root)
{
	_tree = root._tree;
	_sit = root._sit;
	_gh[0] = root._gh[0];
	_gh[1] = root._gh[1];
	_gh[2] = root._gh[2];
	_vectHand[0] = root._vectHand[0];
	_vectHand[1] = root._vectHand[1];
	_vectHand[2] = root._vectHand[2];
	memcpy(_force, root._force, ALL_CN_HAND * sizeof(int));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clRootRiverTree::SizeInFile()
{
	size_t size = _tree.SizeInFile();
	size += sizeof(clSitHoldem);
	size += SizeVectFile(_gh[0]._vect) + SizeVectFile(_gh[1]._vect) + SizeVectFile(_gh[2]._vect);
	return size;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int 	clRootRiverTree::WriteFile(int handle)
{
	_tree.WriteFile(handle);
	_write(handle, &_sit, sizeof(clSitHoldem));
	WriteVectFile(handle, _gh[0]._vect);
	WriteVectFile(handle, _gh[1]._vect);
	WriteVectFile(handle, _gh[2]._vect);
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int 	clRootRiverTree::ReadFile(int handle)
{
	_tree.ReadFile(handle);
	_read(handle, &_sit, sizeof(clSitHoldem));
	if (_sit.CurNbAct() == 0)
		return -1;
	//_sit.ExecHist(_sit.CurNbAct());
	_tree.ReestablishSit(_sit);
	ReadVectFile(handle, _gh[0]._vect);
	ReadVectFile(handle, _gh[1]._vect);
	ReadVectFile(handle, _gh[2]._vect);
	CalcIndex();
	CalcForce();
#ifdef NB_NODE
	int nb = 1;
	_tree.SetNbNode(nb);
#endif
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int 	clRootRiverTree::ReadFileRoot(int handle)
{
	clRoot::ReadFile(handle);
	CalcIndex();
	CalcForce();
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*int		CnNoZeroUnits(clRiverGHUnit *arr)
{
	for (int cn = 0; cn < ALL_CN_HAND; cn++)
		if (arr[cn]._weight < DOUBLE_0)
			return cn;
	return ALL_CN_HAND;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void		clRootRiverTree::CalcIndexHands()
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
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FuncSortArr(tpRiverGHUnit val1, tpRiverGHUnit val2) { return val1._wf < val2._wf; }
void	clRootRiverTree::CreateRiverDat(clRiverDat &dat)
{
	_sit.CloseBank(); 
	int st = (_sit.PlayerMoney(0) > _sit.PlayerMoney(1)) ? _sit.PlayerMoney(1) : _sit.PlayerMoney(0);
	dat._stack = (float)st / _sit.TotalPot();
	//dat._stack = CurStackToPot(_sit);
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
#ifdef DIST_DAT_SQR
			dat._vectUnit[0][i]._wf *= dat._vectUnit[0][i]._wf;
#endif
			dat._vectUnit[0][i]._weight /= w;
		}
	for (int i = 0; i < dim1; i++)
		if (dat._vectUnit[1][i]._weight > DOUBLE_0)
		{
			dat._vectUnit[1][i]._wf /= dat._vectUnit[1][i]._weight;
#ifdef DIST_DAT_SQR
			dat._vectUnit[1][i]._wf *= dat._vectUnit[1][i]._wf;
#endif
			dat._vectUnit[1][i]._weight /= w;
		}

	SortShell(dim0, &dat._vectUnit[0][0], FuncSortArr);

	SortShell(dim1, &dat._vectUnit[1][0], FuncSortArr);

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CreateRiverDatRoot(clRoot &root, clRiverDat &dat)
{
	clRootRiverTree rr;
	rr._sit = root._sit;
	rr._gh[0] = root._gh[0];
	rr._gh[1] = root._gh[1];
	rr.CalcIndex();
	rr.CalcForce();
	rr.CreateRiverDat(dat);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CreateRiverDatMin(clRiverDat &dat, clRiverDat *datIn0)
{
	clRiverDat *datIn=datIn0, dd;
	if (datIn0 == NULL)
	{
		CreateRiverDat(dd);
		datIn = &dd;
	}

	ReCalcParam(RIVERBANK_CN_HANDS, datIn);
	CalcIndex();
	CalcForce();
	FindMinParam(*datIn);

	CalcIndex();
	CreateRiverDat(dat);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clRootRiverTree::CreateRiverDatEx(clRiverDat &dat)
{
	CreateRiverDat(dat);
	double delta = 1. / (CN_RIVERDAT_UNIT - 1);
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		double wh[CN_RIVERDAT_UNIT];
		ZeroMemory(wh, sizeof(wh));
		vector <tpRiverGHUnit> &vect = dat._vectUnit[nbPl];
		int dim = vect.size(), nb = 0;
		double val = 1 - 0.5*delta;
		for (int k = 0; k < dim; )
			if (vect[k]._wf > val)
				wh[nb] += vect[k++]._weight;
			else
			{
				val -= delta;
				nb++;
			}
		vect.resize(CN_RIVERDAT_UNIT);
		for (int i = 0; i < CN_RIVERDAT_UNIT; i++)
		{
			tpRiverGHUnit u;
			u._nbH = glRDD.GetNbHandHero(i);
			u._weight = wh[i];
			u._wf = glRDD.GetForceHero(i);
		}
	}
	_sit.CardsTable()[0] = Card(C_2, S_C);
	_sit.CardsTable()[1] = Card(C_2, S_D);
	_sit.CardsTable()[2] = Card(C_2, S_H);
	_sit.CardsTable()[3] = Card(C_3, S_C);
	_sit.CardsTable()[4] = Card(C_4, S_S);
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CreateRiverDatForMin(clRiverDat &dat, vector <float> *gh)
{
	int dim = gh[0].size();
	vector <tpRiverGHUnit> &vect0 = dat._vectUnit[0];
	vect0.resize(dim);
	for (int i = 0; i < dim; i++)
	{
		vect0[i]._nbH = i;
		vect0[i]._weight = gh[0][i];
		vect0[i]._wf = gh[1][i] / 2;
		for (int k = i + 1; k < dim; k++)
			vect0[i]._wf += gh[1][k];
#ifdef DIST_DAT_SQR
		vect0[i]._wf *= vect0[i]._wf;
#endif
	}
	vector <tpRiverGHUnit> &vect1 = dat._vectUnit[1];
	vect1.resize(dim);
	for (int i = 0; i < dim; i++)
	{
		vect1[i]._nbH = i;
		vect1[i]._weight = gh[1][i];
		vect1[i]._wf = gh[0][i] / 2;
		for (int k = i + 1; k < dim; k++)
			vect1[i]._wf += gh[0][k];
#ifdef DIST_DAT_SQR
		vect1[i]._wf *= vect1[i]._wf;
#endif
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	NormDatEx(vector <float> &gh, int nbH, double w0)
{
	double sum = 0;
	int dim = gh.size();
	for (int i = 0; i < dim; i++)
		sum += gh[i];
	for (int i = 0; i < dim; i++)
		gh[i] /= sum;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::FindMinParam(clRiverDat &datIn)
{
	clRiverDat dat0, dat1;
	CreateRiverDat(dat0);
	vector <float> ghEx[2],g0,g1;
	int dim0 = CnHand(0), dim1 = CnHand(1);
	if (dim0 != dim1)
	{
		ErrMessage("", " FindMinParam");
	}
	ghEx[0].resize(dim0);
	ghEx[1].resize(dim0);
	for (int i = 0; i < dim0; i++)
	{
		ghEx[0][i] = _gh[0][dat0._vectUnit[0][i]._nbH];
		ghEx[1][i] = _gh[1][dat0._vectUnit[1][i]._nbH];
	}
	dat1._stack = dat0._stack;
	CreateRiverDatForMin(dat1, ghEx);
	double min = dat1.Distance(datIn);
	double w0 = 0.1;
	int nb0 = 0, cn = 0, nb, nbPl;
	while (w0 > 0.001)
	{
		if (nb0 < dim0)
		{
			nbPl = 0;
			nb = nb0;
		}
		else
		{
			nbPl = 1;
			nb = nb0 - dim0;
		}
		vector <float> ghCur = ghEx[nbPl];
		ghEx[nbPl][nb] += w0;
		NormDatEx(ghEx[nbPl], nb, w0);
		CreateRiverDatForMin(dat1, ghEx);
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
				NormDatEx(ghEx[nbPl], nb, -w0);
				CreateRiverDatForMin(dat1, ghEx);
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
		if (nb0 >= dim0 + dim1)
			nb0 = 0;
		if (cn == dim0 + dim1)
		{
			cn = 0;
			w0 /= 2;
		}
		g0 = ghEx[0];
		g1 = ghEx[1];
	}
	//CreateRiverDatForMin(dat1, ghEx);
	//double d0 = dat1.Distance(datIn);
	for (int i = 0; i < dim0; i++)
	{
		_gh[0][glRDD.GetNbHand(0,i)] = (ghEx[0][i] > DOUBLE_0) ? ghEx[0][i] : 10 * DOUBLE_0;
		_gh[1][glRDD.GetNbHand(1,i)] = (ghEx[1][i] > DOUBLE_0) ? ghEx[1][i] : 10 * DOUBLE_0;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double clRootRiverTree::TreeVsTree(clHoldemTree &tree)
{
	tpTreePlayerInfo info;
	int cn0 = CnHand(0), cn1 = CnHand(1);
	double res0 = 0, res1 = 0;
	double res, sum = 0;
	for (int i0 = 0; i0 < cn0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < cn1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (HandCrossHandNB(nb0, nb1))
				continue;
			double ww = _gh[0][nb0] * _gh[1][nb1];
			info._nbCls[TRADE_RIVER][0] = i0;
			info._nbCls[TRADE_RIVER][1] = i1;
			info._handForce[0] = _force[nb0];
			info._handForce[1] = _force[nb1];
			double d0 = _tree.TreeVSTree(&info, 0, &tree);
			double d1 = _tree.TreeVSTree(&info, 1, &tree);
			res0 += d0*ww;
			res1 += d1*ww;
			sum += ww;
		}
	}
	return (sum > DOUBLE_0) ? (res0 + res1) / sum / 2 : 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::SetWeightNodeStrat()
{
	_tree.ClearWeight();
	CalcIndex();
	tpTreePlayerInfo info;
	int cn0 = CnHand(0), cn1 = CnHand(1);
	clDoubleCP res0 = 0, res1 = 0;
	double res, sum = 0;
	for (int i0 = 0; i0 < cn0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < cn1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (HandCrossHandNB(nb0, nb1))
				continue;
			info._nbCls[TRADE_RIVER][0] = i0;
			info._nbCls[TRADE_RIVER][1] = i1;
			info._handForce[0] = _force[nb0];
			info._handForce[1] = _force[nb1];
			_tree.AddWeightNode(&info, _gh[0][nb0] * _gh[1][nb1]);
		}
	}
	_tree.CalcWeightStat();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void		clRootRiverTree::CalcForce(int *force)
{
	ZeroMemory(force, sizeof(_force));
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
						force[nb] = g0.ToInt();
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
						force[nb] = glVectRiverHandGame[nbR].ToInt();
					}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcTree(size_t cnCalc)
{
	if (_sit.CnPlayer() == 2)
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

			info._nbCls[TRADE_RIVER][0] = r0;
			info._nbCls[TRADE_RIVER][1] = r1;
			info._handForce[0] = _force[nb0];
			info._handForce[1] = _force[nb1];
			tpFloat k0 = (i0 % 1000) ? 0 : 0.1;
			_tree.CalcNesh(0, &info, _gh[0][nb0], _gh[1][nb1], k0);
			_tree.CalcNesh(1, &info, _gh[1][nb1], _gh[0][nb0], k0);
			i0++;
		}
	}
	else
	{
		tpTreePlayerInfo info;
		int cn0 = CnHand(0), cn1 = CnHand(1), cn2 = CnHand(2);
		for (int i0 = 0; i0 < cnCalc;)
		{
			int r0 = random(cn0);
			int r1 = random(cn1);
			int nb0 = NbHand(0, r0);
			int nb1 = NbHand(1, r1);
			if (HandCrossHandNB(nb0, nb1))
				continue;
			int r2 = random(cn2);
			int nb2 = NbHand(2, r2);
			if (HandCrossHandNB(nb0, nb2))
				continue;
			if (HandCrossHandNB(nb1, nb2))
				continue;

			info._nbCls[TRADE_RIVER][0] = r0;
			info._nbCls[TRADE_RIVER][1] = r1;
			info._nbCls[TRADE_RIVER][2] = r2;
			info._handForce[0] = _force[nb0];
			info._handForce[1] = _force[nb1];
			info._handForce[2] = _force[nb2];
			tpFloat k0 = (i0 % 1000) ? 0 : 0.1;
			_tree.CalcNesh(0, &info, _gh[0][nb0], _gh[1][nb1] * _gh[2][nb2], k0);
			_tree.CalcNesh(1, &info, _gh[1][nb1], _gh[0][nb0] * _gh[2][nb2], k0);
			_tree.CalcNesh(2, &info, _gh[2][nb2], _gh[0][nb0] * _gh[1][nb1], k0);
			i0++;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcTreeKOpp(size_t cnCalc)
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

		info._nbCls[TRADE_RIVER][0] = r0;
		info._nbCls[TRADE_RIVER][1] = r1;
		info._handForce[0] = _force[nb0];
		info._handForce[1] = _force[nb1];
		tpFloat k0 = (i0 % 1000) ? 0 : 0.1;
		_tree.CalcNeshKOpp(0, &info, _gh[0][nb0], _gh[1][nb1], k0);
		_tree.CalcNeshKOpp(1, &info, _gh[1][nb1], _gh[0][nb0], k0);
		i0++;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcTreeOnevsGroup(size_t cnCalc)
{
	vector <tpFloat> kOpp0[2], kOpp;
	int dim0 = CnHand(0), dim1 = CnHand(1);
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		int dim = CnHand(nbPl);
		kOpp0[nbPl].resize(dim);
		for (int i = 0; i < dim; i++)
			kOpp0[nbPl][i] = _gh[nbPl][NbHand(nbPl, i)];
	}

	for (int i = 0; i < cnCalc; i++)
	{
		for (int r0 = 0; r0 < dim0; r0++)
		{
			int nb0 = NbHand(0, r0), nb1 = 0;
			kOpp = kOpp0[1];
			for (int i1 = 0; i1 < dim1; i1++)
			{
				nb1 = NbHand(1, i1);
				if (HandCrossHandNB(nb0, nb1))
					kOpp[i1] = 0;
			}
			CalcNeshA(&_tree, 0, r0, _gh[0][nb0], kOpp);
		}
		for (int r1 = 0; r1 < dim1; r1++)
		{
			int nb1 = NbHand(1, r1);
			kOpp = kOpp0[0];
			for (int i0 = 0; i0 < dim0; i0++)
			{
				int nb0 = NbHand(0, i0);
				if (HandCrossHandNB(nb0, nb1))
					kOpp[i0] = 0;
			}
			CalcNeshA(&_tree, 1, r1, _gh[1][nb1], kOpp);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcTreeOnevsGroupTest(size_t cnCalc)
{
	vector <tpFloat> kOpp0[2], kOpp, ev;
	int dim0 = CnHand(0), dim1 = CnHand(1);
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		int dim = CnHand(nbPl);
		kOpp0[nbPl].resize(dim);
		for (int i = 0; i < dim; i++)
			kOpp0[nbPl][i] = _gh[nbPl][NbHand(nbPl, i)];
	}

	for (int i = 0; i < cnCalc; i++)
	{
		for (int r0 = 0; r0 < dim0; r0++)
		{
			int nb0 = NbHand(0, r0), nb1 = 0;
			kOpp = kOpp0[1];
			for (int i1 = 0; i1 < dim1; i1++)
			{
				nb1 = NbHand(1, i1);
				if (HandCrossHandNB(nb0, nb1))
					kOpp[i1] = 0;
			}
			CalcNeshATest(&_tree, 0, r0, _gh[0][nb0], kOpp, ev);
		}
		for (int r1 = 0; r1 < dim1; r1++)
		{
			int nb1 = NbHand(1, r1);
			kOpp = kOpp0[0];
			for (int i0 = 0; i0 < dim0; i0++)
			{
				int nb0 = NbHand(0, i0);
				if (HandCrossHandNB(nb0, nb1))
					kOpp[i0] = 0;
			}
			CalcNeshATest(&_tree, 1, r1, _gh[1][nb1], kOpp, ev);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcTreeGroupvsGroup(size_t cnCalc)
{
	vector <tpFloat> k[2], ev;
	int dim0 = CnHand(0), dim1 = CnHand(1);
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		int dim = CnHand(nbPl);
		k[nbPl].resize(dim);
		for (int i = 0; i < dim; i++)
			k[nbPl][i] = _gh[nbPl][NbHand(nbPl, i)];
	}
	ev.resize((dim0 > dim1) ? dim0 : dim1);
	for (int i = 0; i < cnCalc; i++)
	{
		CalcNeshA(&_tree, 0, k[0], k[1], ev);
		CalcNeshA(&_tree, 1, k[1], k[0], ev);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CreateTreeNL()
{
	_tree.ClearAllBranch();
	_tree.AtrTree()->_sitA = _sit;
	CalcIndex();
	CalcForce();
	BuildTreeNLAutoA(&_tree, _sit);
	int cnHand[] = { CnHand(0),CnHand(1),CnHand(2) };
	_tree.SetStartParam(cnHand);
#ifdef NB_NODE
	int nbN = 0;
	_tree.SetNbNode(nbN);
#endif
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcAbsWeight(vector <float> *weight)
{
	int cn0 = CnHand(0), cn1 = CnHand(1);
	weight[0].resize(cn0);
	weight[1].resize(cn1);
	ZeroMemory(&weight[0][0], weight[0].size() * sizeof(float));
	ZeroMemory(&weight[1][0], weight[1].size() * sizeof(float));
	float w = 0;
	for (int i0 = 0; i0 < cn0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < cn1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (HandCrossHandNB(nb0, nb1))
				continue;
			float ww = _gh[0][nb0] * _gh[1][nb1];
			w += ww;
			weight[0][i0] += ww;
			weight[1][i1] += ww;
		}
	}
	if (w > DOUBLE_0)
	{
		for (int i = 0; i < cn0; i++)
			weight[0][i] /= w;
		for (int i = 0; i < cn1; i++)
			weight[1][i] /= w;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat clRootRiverTree::AbsAntistrategy()
{
	tpFloat ev0 = 0, ev1 = 0;
	int dim0 = CnHand(0);
	int dim1 = CnHand(1);
	vector <float> weight[2];
	CalcAbsWeight(weight);
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		clHandsGroupEx gh(ALL_CN_HAND);
		gh.ClearDat();
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
				gh[nb1] = _gh[1][nb1];
		}
		gh.Norm();
		tpFloat ev = CalcAntiStratEV(&_tree, 0, i0, gh);

		tpFloat a = ev*weight[0][i0];

		ev0 += ev*weight[0][i0];
	}
	for (int k1 = 0; k1 < dim1; k1++)
	{
		int i1 = NbHand(1, k1);
		clHandsGroupEx gh(ALL_CN_HAND);
		gh.ClearDat();
		for (int k0 = 0; k0 < dim0; k0++)
		{
			int i0 = NbHand(0, k0);
			if (!HandCrossHandNB(i0, i1))
				gh[i0] = _gh[0][i0];
		}
		gh.Norm();
		tpFloat ev = CalcAntiStratEV(&_tree, 1, k1, gh);

		tpFloat a = ev*weight[1][k1];

		ev1 += ev*weight[1][k1];
	}
	return (ev0 + ev1) / 2;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*tpFloat clRootRiverTree::QAntistrategy()
{
	tpFloat ev0 = 0, ev1 = 0;
	int dim0 = CnHand(0);
	int dim1 = CnHand(1);
	vector <float> weight[2];
	CalcAbsWeight(weight);
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		clHandsGroupEx gh(ALL_CN_HAND);
		gh.ClearDat();
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
				gh[nb1] = _gh[1][nb1];
		}
		gh.Norm();
		tpFloat ev = _tree.CalcQAntiStratEV(this, 0, i0, gh);
		ev0 += ev*weight[0][i0];
	}
	for (int k1 = 0; k1 < dim1; k1++)
	{
		int i1 = NbHand(1, k1);
		clHandsGroupEx gh(ALL_CN_HAND);
		gh.ClearDat();
		for (int k0 = 0; k0 < dim0; k0++)
		{
			int i0 = NbHand(0, k0);
			if (!HandCrossHandNB(i0, i1))
				gh[i0] = _gh[0][i0];
		}
		gh.Norm();
		tpFloat ev = _tree.CalcQAntiStratEV(this, 1, k1, gh);
		ev1 += ev*weight[1][k1];
	}
	return (ev0 + ev1) / 2;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CalcEV(vector <float> *ev)
{
	int dim0 = CnHand(0), dim1 = CnHand(1);
	ev[0].resize(ALL_CN_HAND);
	ev[1].resize(ALL_CN_HAND);
	ZeroMemory(&ev[0][0], ALL_CN_HAND * sizeof(float));
	ZeroMemory(&ev[1][0], ALL_CN_HAND * sizeof(float));

	tpTreePlayerInfo info;
	tpFloat w0[ALL_CN_HAND], w1[ALL_CN_HAND];
	ZeroMemory(w0, ALL_CN_HAND * sizeof(tpFloat));
	ZeroMemory(w1, ALL_CN_HAND * sizeof(tpFloat));
	for (int i0 = 0; i0 < dim0; i0++)
	{
		int nb0 = NbHand(0, i0);
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			if (!HandCrossHandNB(nb0, nb1))
			{
				float ww = _gh[0][nb0] * _gh[1][nb1];
				info._nbCls[TRADE_RIVER][0] = i0;
				info._nbCls[TRADE_RIVER][1] = i1;
				info._handForce[0] = _force[nb0];
				info._handForce[1] = _force[nb1];
				clDoubleCP res0 = _tree.CalcStratEV(&info);
				ev[0][nb0] += res0[0] * ww;
				ev[1][nb1] += res0[1] * ww;
				w0[i0] += ww;
				w1[i1] += ww;
			}
		}
	}
	for (int i = 0; i < dim0; i++)
		if (w0[i] > DOUBLE_0)
			ev[0][NbHand(0, i)] /= w0[i];
	for (int i = 0; i < dim1; i++)
		if (w1[i] > DOUBLE_0)
			ev[1][NbHand(1, i)] /= w1[i];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::CompactGH(tpFloat minW, int cnMin, clRiverDat *datIn)
{
	clRiverDat datCur, *dat;
	if (datIn == NULL)
	{
		dat = &datCur;
		CreateRiverDat(*dat);
	}
	else
		dat = datIn;
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		vector <tpRiverGHUnit> &vect = dat->_vectUnit[nbPl];
		while (true)
		{
			int dim = vect.size();
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
		_gh[nbPl].ClearDat();
		int dim = vect.size();
		for (int i = 0; i < dim; i++)
			_gh[nbPl][vect[i]._nbH] = vect[i]._weight;
		_gh[nbPl].Norm();
	}
	//CreateTreeNL();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clRootRiverTree::ReCalcParam(int cnHand, clRiverDat *datIn)
{
	clRiverDat datCur, *dat;
	if (datIn == NULL)
	{
		dat = &datCur;
		CreateRiverDat(*dat);
	}
	else
		dat = datIn;
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		clClasters <tpRiverGHUnit> cls;
		vector <tpRiverGHUnit> &vect = dat->_vectUnit[nbPl];
		int dim = vect.size();
		cls.Resize(cnHand);
		if (cnHand > dim)
		{
			for (int i = 0; i < cnHand; i++)
			{
				cls[i].resize(1);
				if (i < dim)
					cls[i][0] = vect[i];
				else
					cls[i][0] = { i,0,0 };
			}
		}
		else
		{
			int cur = 0;
			cls[0].push_back(vect[0]);
			for (int i = 0; i < cnHand; i++)
			{
				int cn = (dim - cur) / (cnHand - i);
				cls[i].resize(cn);
				memcpy(&cls[i][0], &vect[cur], cn * sizeof(tpRiverGHUnit));
				cur += cn;
			}
			for (int i = 0; i < 4; i++)
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
			_gh[nbPl][glRDD.GetNbHand(nbPl, i)] = w0[i];
		_gh[nbPl].Norm();
	}

	memcpy(_sit.CardsTable(), glRDD._board, sizeof(glRDD._board));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	BuildTreeNLAutoA(clHoldemTree *ptrT, clSitHoldem &sit)
{
	int ind = sit.Indicator();
	if (ind == -1)
	{
		ptrT->AtrTree()->CalcResSit(sit);
		return;
	}

	clSitHoldem sit0;
	tpDis arrDis[MAX_CN_BRANCH];
	int cnDis = GetDisLong(sit, arrDis);
	for (int i = 0; i < cnDis; i++)
	{
		sit0 = sit;
		sit0.ExecDis(arrDis[i]);
		int nb = ptrT->AddBranch(sit0);
		BuildTreeNLAutoA(ptrT->Branch(nb), sit0);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
float	clRiverDat::Distance(clRiverDat &dat)
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
				res += val*val*weight;// *FuncK(sum);
				ost -= weight;
				i++;
				if (i == dimI)
					break;
				weight = dat._vectUnit[k][i]._weight;
				//sum += weight;
				if (weight < DOUBLE_0)
					break;
			}
			else
			{
				float val = _vectUnit[k][j]._wf - dat._vectUnit[k][i]._wf;
				res += val*val*ost;// *FuncK(sum);
				weight -= ost;
				j++;
				if (j == dimJ)
					break;
				ost = _vectUnit[k][j]._weight;
			}
		}
	}
	res = sqrt(res / 2);
	float ds = (dat._stack > _stack) ? (dat._stack - _stack) / dat._stack : (_stack - dat._stack) / _stack;
	return (res + ds) / 2;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CreateSitRiver(clSitHoldem &sit, int stack, int pot)
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
	sit.AddAct(POS_BB, ACT_CHECK, 0);
	sit.AddAct(POS_SB, ACT_CHECK, 0);

	sit.AddAct(NB_DILER, ACT_DELTURN, 0);
	sit.AddAct(POS_BB, ACT_CHECK, 0);
	sit.AddAct(POS_SB, ACT_CHECK, 0);

	sit.AddAct(NB_DILER, ACT_DELRIVER, 0);

	sit.ExecHist(sit.PtrHistory());
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
int tpRiverGHUnit::FindNbCenterElement(vector <tpRiverGHUnit> &vect)
{
	int dim = vect.size();
	double res = 0, w0 = 0;
	for (int i = 0; i < dim; i++)
	{
		res += vect[i]._weight*vect[i]._wf;
		w0 += vect[i]._weight;
	}
	if (w0 < DOUBLE_0)
		return 0;
	res /= w0;
	int nb = 0;
	w0 = fabs(vect[0]._wf - res);
	for (int i = 1; i < dim; i++)
	{
		double d = fabs(vect[i]._wf - res);
		if (d < w0)
		{
			w0 = d;
			nb = i;
		}
	}
	return nb;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
