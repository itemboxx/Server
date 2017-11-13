#include <math.h>
#include <time.h>

//#include "C:\home\holdem\Strategy\Source\CalcResLeaf.h"
#include "c:\home\card\naborCards.h"
#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\StrategyStreet.h"
#include "c:\home\Util\TemplateFileRec.h"
#include "c:\Home\Util\Sort.h"

clRoot glHS;
clStrategyH glStrategy;

void	BuildTreeNLFromHistory(clHoldemTree *ptrT, clSitHoldem &sit, clSitHoldem *sitH, int nbA);
void	FillAtrTree(clHoldemTree *treeIn, clHoldemTree *tree, double k);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CheckTree(clHoldemTree *node)
{
	if (node->CnBranch() > 1)
	{
		for (int i = 1; i < node->CnBranch(); i++)
			if (node->Branch(i)->GetDis() == node->Branch(i - 1)->GetDis())
			{
				node->AtrTree()->AddDataBranch(node->CnBranch(), i - 1, i);
				node->DelBranch(i);
				CheckTree(node);
			}
	}
	for (int i = 0; i < node->CnBranch(); i++)
		CheckTree(node->Branch(i));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategyH::GetDis(clSitHoldem &sit, vector <tpDisNode> &vect, clRoot &hs, int mSec)
{
	vect.clear();
	int nbHero = sit.Indicator();
	if (nbHero == -1)
		return HistCreateDis(ACT_UNDEF);
	int t0 = GetTickCount();
	enNameTrade trade = sit.NameTrade();
	if (trade == hs._sit.NameTrade())
		return GetDis(hs, sit, vect, mSec);
	else if (trade == TRADE_FLOP)
	{
		ErrMessage("", "111");
		/*FindGHFlop(hs, sit);
		_flopTreeBank->GetTreeForRoot(hs,NULL);
		t0 = GetTickCount() - t0;
		return GetDis(sit, vect, hs, mSec - t0);*/
	}
	else if (trade == TRADE_TURN)
	{
		FindGHTurn(hs, sit);
		_turnTreeBank->GetTreeForRoot(hs,NULL);
		//CheckTree(&hs._tree);
		t0 = GetTickCount() - t0;
		return GetDis(sit, vect, hs, mSec - t0);
	}
	else if (trade == TRADE_RIVER)
	{
		FindGHRiver(hs, sit);
		if(!RecalcTreeRiver(hs, NULL))
			return HistCreateDis(ACT_UNDEF);
		t0 = GetTickCount() - t0;
		return GetDis(sit, vect, hs, mSec - t0);
	}
	else
		return HistCreateDis(ACT_UNDEF);

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	BuildTree(clRoot &hs, clSitHoldem &sit, int nbAct)
{
	hs._tree.ClearAllBranch();
	hs._tree.GetSit() = hs._sit;
	BuildTreeNLFromHistory(&hs._tree, hs._sit, &sit, nbAct);
	int arrCls[] = { hs.CnHand(0),hs.CnHand(1) };
	hs._tree.SetStartParam(arrCls);
	hs._tree.SetResSit();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis clStrategyH::GetDis(clRoot &hs, clSitHoldem &sit, vector <tpDisNode> &vect, int tSec)
{
	if (tSec < 500)
		return GetDisTree(&hs, sit, vect);

	int t0 = GetTickCount();
	enNameTrade trade = sit.NameTrade();
	int nbAct;
	if (trade == TRADE_TURN)
		nbAct = sit.NbFirstAct(ACT_DELTURN);
	else if (trade == TRADE_RIVER)
		nbAct = sit.NbFirstAct(ACT_DELRIVER);
	else if (trade == TRADE_FLOP)
		nbAct = sit.NbFirstAct(ACT_DELFLOP);
	else
	{
		return HistCreateDis(ACT_UNDEF);
	}
	nbAct++;
	if (hs._tree.FindSitNode(sit.CnAct() - nbAct, sit, nbAct) == NULL)
	{
		clHoldemTree treeN;
		treeN.MoveTree(&hs._tree);
		hs._sit.ExecHist(sit.PtrHistory(), nbAct);
		BuildTree(hs, sit, nbAct);

		FillAtrTree(&hs._tree, &treeN, 1);
	}
	t0 = GetTickCount() - t0;
	CalcTree(hs, tSec-t0);
	return GetDisTree(&hs, sit, vect);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis clStrategyH::GetDisTree(clRoot *hs, clSitHoldem &sit, vector <tpDisNode> &vect)
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
	{
		nbAct = sit.NbFirstTradeAct(2);
		if (nbAct == -1)
			nbAct = sit.CurNbAct();
		nbAct--;
	}
	else
	{
		return HistCreateDis(ACT_UNDEF);
	}
	nbAct++;
	vector <stNodeW> vn;
	hs->_tree.FindSitNode(sit.CurNbAct() - nbAct, sit, nbAct, vn, 1);
	int nbCls=-1;
	if(trade==TRADE_PREFLOP)
		nbCls= sit.PlayerHand(sit.Indicator()).NbHand();
	else
	{
		int nbHand = sit.PlayerHand(sit.Indicator()).NbHandAll();
		nbCls = hs->NbCls(sit.Indicator(), nbHand);
		if (nbCls == -1)
		{
			//ErrMessage("Ошибка ", "Номер кластера равен -1");
			int handle = _open("d:\\data\\saveSit.sit", O_BINARY | O_RDWR | O_CREAT);
			if (handle != -1)
			{
				_write(handle, &sit, sizeof(sit));
				_close(handle);
			}
			return HistCreateDis(ACT_UNDEF);
		}
	}
	for (int i = 0; i < vn.size(); i++)
	{
		clHoldemTree *node = (clHoldemTree *)vn[i]._ptr;
		clCalcParam cls = node->AtrTree()->CalcParam(nbCls);
		tpFloat w[MAX_CN_BRANCH];
		cls.GetWeightStrat(node->CnBranch(), w);
		for (int j = 0; j < node->CnBranch(); j++)
		{
			tpDisNode dn;
			dn._dis = sit.CreateDis(node->Branch(j)->GetDis());
			dn._weight = w[j] * vn[i]._weight;
			dn._ev = cls.AbsEV(j);
			vect.push_back(dn);
		}
	}

	int cn = CutAndNorm(vect, 0.01);
	if (cn == 0)
		return HistCreateDis(ACT_UNDEF);

	for (int i = 0; i < cn; i++)
		for (int j = i + 1; j < cn; j++)
			if (vect[i]._weight < vect[j]._weight)
			{
				tpDisNode temp = vect[i];
				vect[i] = vect[j];
				vect[j] = temp;
			}
// ???
	if (cn > 8)
	{
		vect.resize(8);
		cn = CutAndNorm(vect, 0.01);
	}
// ???
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
	return dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyH::FindGHRiver(clRoot &hs, clSitHoldem &sit)
{
	int nbAct = sit.NbFirstAct(ACT_DELRIVER);
	if (hs._tree.EndedNode())
	{
		hs._sit.ExecHist(sit.PtrHistory(), ++nbAct);
		/*		int nbH[2];
				nbH[0] = hs._sit.PlayerHand(0).NbHandAll();
				nbH[1] = hs._sit.PlayerHand(1).NbHandAll();
				clCard c0 = sit.CardsTable()[4];
				hs._gh[0].SkipCard(c0);
				hs._gh[1].SkipCard(c0);
				hs._gh[0].Norm();
				hs._gh[1].Norm();*/
		hs.CalcIndex();
	}
	else if (hs._sit.NameTrade() == TRADE_TURN)
	{
		vector <stNodeW> vn;
		int nbAT = sit.NbFirstAct(ACT_DELTURN);
		nbAT++;
		hs._tree.FindSitNode(nbAct - nbAT, sit, nbAT, vn, 1);

		clHandsGroupEx res[2];
		res[0].SetDim(ALL_CN_HAND);
		res[1].SetDim(ALL_CN_HAND);
		res[0].ClearDat();
		res[1].ClearDat();
		int dim0 = hs.CnHand(0);
		int dim1 = hs.CnHand(1);
		for (int i = 0; i < vn.size(); i++)
		{
			clHandsGroupEx gh0[2];
			double mult = vn[i]._weight;
			hs.RecalcGH((clHoldemTree *)vn[i]._ptr, gh0);
			for (int j = 0; j < dim0; j++)
			{
				int nb = hs.NbHand(0, j);
				res[0][nb] += gh0[0][nb] * mult;
			}
			for (int j = 0; j < dim1; j++)
			{
				int nb = hs.NbHand(1, j);
				res[1][nb] += gh0[1][nb] * mult;
			}
		}
		hs._gh[0] = res[0];
		hs._gh[1] = res[1];
		hs._sit.ExecHist(sit.PtrHistory(), ++nbAct);
		clCard c0 = sit.CardsTable()[4];
		hs._gh[0].SkipCard(c0);
		hs._gh[1].SkipCard(c0);
		hs._gh[0].Norm();
		hs._gh[1].Norm();
		hs.CalcIndex();
		//hs.CalcForce();
	}
	BuildTree(hs, sit, nbAct);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clStrategy::RecalcTreeTurn(clRootStreetTree &hs)
{
	clRootStreetTree root;
	root = hs;
	root.CalcIndex();
	_turnTreeBank.GetTreeForRoot(root, hs._tree);
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyH::FindGHTurn(clRoot &hs, clSitHoldem &sit)
{
	int nbAct = sit.NbFirstAct(ACT_DELTURN);
	nbAct++;
	if (hs.Empty())
	{
		hs._sit.ExecHist(sit.PtrHistory(), nbAct);
		int nbH[2];
		nbH[0] = hs._sit.PlayerHand(0).NbHandAll();
		nbH[1] = hs._sit.PlayerHand(1).NbHandAll();
		clCard c0 = sit.CardsTable()[3];
		hs._gh[0].SkipCard(c0);
		hs._gh[1].SkipCard(c0);
		hs._gh[0].Norm();
		hs._gh[1].Norm();
		hs.CalcIndex();
		hs.InitDeck();
		CompactGHTurn(hs, 0.5, 100, nbH);
		//hs.CalcForce();
		BuildTree(hs, sit, nbAct);
	}
	else
	{
		vector <stNodeW> vn;
		int nbAct = sit.NbFirstAct(ACT_DELFLOP);
		nbAct++;
		int nbA1 = sit.NbFirstAct(ACT_DELTURN);
		hs._tree.FindSitNode(nbA1 - nbAct, sit, nbAct, vn, 1);

		clHandsGroupEx res[2];
		res[0].SetDim(ALL_CN_HAND);
		res[1].SetDim(ALL_CN_HAND);
		res[0].ClearDat();
		res[1].ClearDat();
		int dim0 = hs.CnHand(0);
		int dim1 = hs.CnHand(1);
		for (int i = 0; i < vn.size(); i++)
		{
			clHandsGroupEx gh0[2];
			double mult = vn[i]._weight;
			hs.RecalcGH((clHoldemTree *)vn[i]._ptr, gh0);
			for (int j = 0; j < dim0; j++)
			{
				int nb = hs.NbHand(0, j);
				res[0][nb] += gh0[0][nb] * mult;
			}
			for (int j = 0; j < dim1; j++)
			{
				int nb = hs.NbHand(1, j);
				res[1][nb] += gh0[1][nb] * mult;
			}
		}
		hs._gh[0] = res[0];
		hs._gh[1] = res[1];
		hs._sit.ExecHist(sit.PtrHistory(), nbA1 + 1);
		clCard c0 = sit.CardsTable()[3];
		hs._gh[0].SkipCard(c0);
		hs._gh[1].SkipCard(c0);
		hs._gh[0].Norm();
		hs._gh[1].Norm();
		hs.CalcIndex();
		hs.InitDeck();
		/*int nbH[2];
		nbH[0] = hs._sit.PlayerHand(0).NbHandAll();
		nbH[1] = hs._sit.PlayerHand(1).NbHandAll();
		CompactGHTurn(hs, 0.5, 100, nbH);*/
		//hs.CalcForce();
		BuildTree(hs, sit, nbA1 + 1);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyH::FindGHFlop(clRoot &hs, clSitHoldem &sit)
{
	int nbAct = sit.NbFirstAct(ACT_DELFLOP);
	nbAct++;
	if (hs._tree.EndedNode())
	{
		hs._sit.ExecHist(sit.PtrHistory(), nbAct);
		int nbH[2];
		nbH[0] = hs._sit.PlayerHand(0).NbHandAll();
		nbH[1] = hs._sit.PlayerHand(1).NbHandAll();
		clCard *ptr = sit.CardsTable();
		hs._gh[0].SkipCard(ptr[0]);
		hs._gh[1].SkipCard(ptr[0]);
		hs._gh[0].SkipCard(ptr[1]);
		hs._gh[1].SkipCard(ptr[1]);
		hs._gh[0].SkipCard(ptr[2]);
		hs._gh[1].SkipCard(ptr[2]);
		hs._gh[0].Norm();
		hs._gh[1].Norm();
		hs.CalcIndex();
		hs.InitDeck();
		//hs.CompactGHQuickH(0.5, 100, nbH);
		//hs.CalcForce();
	}
	else
	{
		ErrMessage("Не реализованная функция", " Error");
		return;
	}
	BuildTree(hs, sit, nbAct);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*bool	clStrategyH::RecalcTreeRiver(clRoot &hs, tpUnfindRiverTree *ptr)
{
	clRootRiverTree root;
	root._sit = hs._sit;
	root._gh[0] = hs._gh[0];
	root._gh[1] = hs._gh[1];
	root.CalcIndex();
	root.CalcForce();
	return _bankRiver->GetTreeForRoot(root, hs._tree, ptr);
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyH::SaveFileUnf()
{
/*	if (_unfindTurn._file.IsOpen())
		_unfindTurn._file.SaveFile();
	if (_unfindRiver._file.IsOpen())
		_unfindRiver._file.SaveFile();*/
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
struct tpThreadCalcAtr
{
	int _cnCalc, _rand, _cn, _nb;
	bool _stop, _end;
	int *_force;
	clRootRiverTree *_root;
};

unsigned long _stdcall CalcThreadRiverTree(void *ptr)
{
	tpThreadCalcAtr *atr = (tpThreadCalcAtr *)ptr;
	srand(atr->_rand);
	tpTreePlayerInfo info;

	int cn0 = atr->_root->CnHand(0), cn1 = atr->_root->CnHand(1);
	atr->_cn = 0;
	while (!atr->_stop)
	{
		int r0 = random(cn0);
		int r1 = random(cn1);
		int nb0 = atr->_root->NbHand(0, r0);
		int nb1 = atr->_root->NbHand(1, r1);
		if (HandCrossHandNB(nb0, nb1))
			continue;

		info._nbCls[TRADE_RIVER][0] = r0;
		info._nbCls[TRADE_RIVER][1] = r1;
		info._handForce[0] = atr->_force[nb0];
		info._handForce[1] = atr->_force[nb1];
		tpFloat k0 = (atr->_cn % 1000) ? 0 : 0.1;
		atr->_root->_tree.CalcNesh(0, &info, atr->_root->_gh[0][nb0], atr->_root->_gh[1][nb1], k0);
		atr->_root->_tree.CalcNesh(1, &info, atr->_root->_gh[1][nb1], atr->_root->_gh[0][nb0], k0);
		atr->_cn++;
	}
	atr->_end = true;
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcRiverTreeMultiThread(clRootStreetTree &hs, int mSec)
{
	int force[ALL_CN_HAND];
	((clRootRiverTree *)(&hs))->CalcForce(force);
	tpThreadCalcAtr atr[CN_THREAD];
	int t0 = GetTickCount();
	for (int i = 0; i < CN_THREAD; i++)
	{
		atr[i]._cnCalc = 1000;
		atr[i]._nb = i;
		atr[i]._rand = (int)t0 + i;
		atr[i]._root = ((clRootRiverTree *)(&hs));
		atr[i]._force = force;
		atr[i]._stop = atr[i]._end = false;
		CreateThread(NULL, 0, &CalcThreadRiverTree, &atr[i], 0, NULL);
	}
	while (true)
	{
		Sleep(100);
		int cn = 0;
		for (int i = 0; i < CN_THREAD; i++)
			cn += atr[i]._cn;
		MSG tMessage;
		while (::PeekMessage(&tMessage, NULL, NULL, NULL, PM_REMOVE))
		{
			::TranslateMessage(&tMessage);
			::DispatchMessage(&tMessage);
		}
		int t1 = GetTickCount() - t0;
		if (t1>mSec || cn>10000000)
			for (int i = 0; i < CN_THREAD; i++)
				atr[i]._stop = true;
		int cnE = 0;
		for (; cnE < CN_THREAD; cnE++)
			if (!atr[cnE]._end)
				break;
		if (cnE == CN_THREAD)
			break;
	}
	glTime3 = 0;
	for (int i = 0; i < CN_THREAD; i++)
		glTime3 += atr[i]._cn;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyH::CalcTree(clRoot &hs, int mSec)
{
/*	hs._tree.ClearData();
	glTime3 = 0;
	int t0 = GetTickCount();
	if (hs._sit.NameTrade() == TRADE_RIVER)
	{
		CalcRiverTreeMultiThread(hs, mSec);
		return;
	}
	vector <tpFloat> gh[2];
	vector <float> ev[2];
	hs.CreateCompactGH(gh);
	hs.CalcAllIn();
	ev[0].resize(hs.CnHand(0));
	ev[1].resize(hs.CnHand(1));
	int nbN = 1;
	hs._tree.SetNbNode(nbN);
	for (; glTime3 < 100; glTime3++)
	{
		hs.CalcNeshMultiHandsTree(&hs._tree, 0, gh[0], gh[1], ev[0]);
		hs.CalcNeshMultiHandsTree(&hs._tree, 1, gh[1], gh[0], ev[1]);
		int t1 = GetTickCount() - t0;
		if ( t1 > mSec)
			break;
	}*/
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clStrategy::GetAllDis(clTreeBank &bank, tpRefHero ref, clSitHoldem &sit0, vector <tpDisNode> &val)
{
	clTreeBankUnit *unit = bank.LockBankUnit(ref._nbTree);
	int st = unit->_root._sit.PlMoney(0);
	clSitHoldem sit = sit0;
	sit.ChangeStacks(st);
	double mult = (double)sit0.PlMoney(0) / st;
	int nextAct = sit.NbFirstAct(ACT_DELTURN);
	if (nextAct == -1) return;
	vector <stNodeW> vn;
	unit->_root._tree.FindSitNode(sit.CnAct() - nextAct-1, sit, nextAct+1, vn, 1);
	for (int i = 0; i < vn.size(); i++)
	{
		clHoldemTree *node = (clHoldemTree *)vn[i]._ptr;
		int nbCls = (node->AtrTree()->CnClasters() == 1) ? 0 : ref._refNb._nb0;
		clCalcParam cls = node->AtrTree()->CalcParam(nbCls);
		tpFloat w[MAX_CN_BRANCH];
		cls.GetWeightStrat(node->CnBranch(), w);
		for (int j = 0; j < node->CnBranch(); j++)
		{
			tpDisNode dn;
			dn._dis = node->Branch(j)->GetDis();
			dn._weight = w[j] * vn[i]._weight*ref._weight*ref._refNb._w0;
			dn._ev = cls.AbsEV(j);
			dn.MultDis(mult);
			val.push_back(dn);
		}

		if (ref._refNb._w1 > DOUBLE_0)
		{
			nbCls = (node->AtrTree()->CnClasters() == 1) ? 0 : ref._refNb._nb1;
			cls = node->AtrTree()->CalcParam(nbCls);
			cls.GetWeightStrat(node->CnBranch(), w);
			for (int j = 0; j < node->CnBranch(); j++)
			{
				tpDisNode dn;
				dn._dis = node->Branch(j)->GetDis();
				dn._weight = w[j] * vn[i]._weight*ref._weight*ref._refNb._w0;
				dn._ev = cls.AbsEV(j);
				dn.MultDis(mult);
				val.push_back(dn);
			}
		}
	}
	bank.UnLockBankUnit(ref._nbTree);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::GetAllDis(clRiverBanks &bank, tpRefHero ref, clSitHoldem &sit0, vector <tpDisNode> &val)
{
	clRiverBankUnit *unit = bank.LockBankUnit(ref._nbTree);
	int st = unit->_root._sit.PlMoney(0);
	clSitHoldem sit = sit0;
	sit.ChangeStacks(st);
	double mult = (double)sit0.PlMoney(0) / st;
	int nextAct = sit.NbFirstAct(ACT_DELTURN);
	if (nextAct == -1) return;
	vector <stNodeW> vn;
	unit->_root._tree.FindSitNode(sit.CnAct() - nextAct, sit, nextAct, vn, 1);
	for (int i = 0; i < vn.size(); i++)
	{
		clHoldemTree *node = (clHoldemTree *)vn[i]._ptr;
		int nbCls = (node->AtrTree()->CnClasters() == 1) ? 0 : ref._refNb._nb0;
		clCalcParam cls = node->AtrTree()->CalcParam(nbCls);
		tpFloat w[MAX_CN_BRANCH];
		cls.GetWeightStrat(node->CnBranch(), w);
		for (int j = 0; j < node->CnBranch(); j++)
		{
			tpDisNode dn;
			dn._dis = node->Branch(j)->GetDis();
			dn._weight = w[j] * vn[i]._weight*ref._weight*ref._refNb._w0;
			dn._ev = cls.AbsEV(j);
			dn.MultDis(mult);
			val.push_back(dn);
		}

		if (ref._refNb._w1 > DOUBLE_0)
		{
			nbCls = (node->AtrTree()->CnClasters() == 1) ? 0 : ref._refNb._nb1;
			cls = node->AtrTree()->CalcParam(nbCls);
			cls.GetWeightStrat(node->CnBranch(), w);
			for (int j = 0; j < node->CnBranch(); j++)
			{
				tpDisNode dn;
				dn._dis = node->Branch(j)->GetDis();
				dn._weight = w[j] * vn[i]._weight*ref._weight*ref._refNb._w0;
				dn._ev = cls.AbsEV(j);
				dn.MultDis(mult);
				val.push_back(dn);
			}
		}
	}
	bank.UnLockBankUnit(ref._nbTree, unit);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategy::FindGH(clHistSave &hs, clSitHoldem &sit)
{
	enNameTrade trade = hs._sit.NameTrade();
	if (trade >= sit.NameTrade()) return;
	int nbHero = sit.Indicator();
	if (trade == TRADE_TURN)
	{
		clRootStreetTree root;
		root._sit = hs._sit;
		root._gh[0] = hs._gh[0];
		root._gh[1] = hs._gh[1];
		root.CalcIndex();
		vector <tpRefTree> vectRef;
		_turnTreeBank.GetAllTree(root, vectRef);
		if (vectRef.size() == 0)
			ErrMessage("clStrategy::FindGH", " Error");

		clHandsGroupEx ghIn[2];
		ghIn[0] = hs._gh[0];
		ghIn[1] = hs._gh[1];
		hs._gh[0].ClearDat();
		hs._gh[1].ClearDat();
		int dim0 = root.CnHand(0);
		int dim1 = root.CnHand(1);
		for (int i = 0; i < vectRef.size(); i++)
		{
			clHandsGroupEx gh0[2];
			gh0[0] = ghIn[0];
			gh0[1] = ghIn[1];
			double mult = vectRef[i]._weight;
			GetTreeGH(_turnTreeBank, vectRef[i], sit, gh0, root);
			for (int j = 0; j < dim0; j++)
			{
				int nb = root.NbHand(0, j);
				hs._gh[0][nb] += gh0[0][nb] * mult;
			}
			for (int j = 0; j < dim1; j++)
			{
				int nb = root.NbHand(1, j);
				hs._gh[1][nb] += gh0[1][nb] * mult;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	ChangeGHForSit(clRootStreetTree &root, clHoldemTree *node, tpRefTree &ref, clHandsGroupEx *gh)
{
	if (node->_parent != NULL)
	{
		clHoldemTree *parent = node->Parent();
		ChangeGHForSit(root, parent, ref, gh);
		int nb = parent->FindNbBranch(node);
		int ind = parent->Indicator();
		clHandsGroupEx &ghH = gh[ind];
		tpTableChangeNb &tabH = ref._tab[ind];
		for (int i = 0; i < ALL_CN_HAND; i++)
			if (ghH[i] > DOUBLE_0)
			{
				int nbCls = tabH._arr[i]._nb0;
				clCalcParam cls = parent->AtrTree()->CalcParam(nbCls);
				tpFloat w[MAX_CN_BRANCH];
				cls.GetWeightStrat(parent->CnBranch(), w);
				float res = ghH[i] * w[nb] * tabH._arr[i]._w0;
				if (tabH._arr[i]._w1 > DOUBLE_0)
				{
					nbCls = tabH._arr[i]._nb1;
					clCalcParam cls = parent->AtrTree()->CalcParam(nbCls);
					cls.GetWeightStrat(parent->CnBranch(), w);
					res += ghH[i] * w[nb] * tabH._arr[i]._w1;
				}
				ghH[i] = res;
			}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void clStrategy::GetTreeGH(clTreeBank &bank, tpRefTree ref, clSitHoldem &sit0, clHandsGroupEx *gh, clRootStreetTree &root)
{
	clTreeBankUnit *unit = bank.LockBankUnit(ref._nbTree);
	int nbRef[ALL_CN_HAND];
	int dim = unit->_root.CnHand(0);
	for (int i = 0; i < dim; i++)
		nbRef[unit->_root.NbHand(0, i)] = i;
	dim = root.CnHand(0);
	for (int i = 0; i < dim; i++)
	{
		int nb = root.NbHand(0, i);
		ref._tab[0]._arr[nb]._nb0 = nbRef[ref._tab[0]._arr[nb]._nb0];
		ref._tab[0]._arr[nb]._nb1 = nbRef[ref._tab[0]._arr[nb]._nb1];
	}
	dim = unit->_root.CnHand(1);
	for (int i = 0; i < dim; i++)
		nbRef[unit->_root.NbHand(1, i)] = i;
	dim = root.CnHand(1);
	for (int i = 0; i < dim; i++)
	{
		int nb = root.NbHand(1, i);
		ref._tab[1]._arr[nb]._nb0 = nbRef[ref._tab[1]._arr[nb]._nb0];
		ref._tab[1]._arr[nb]._nb1 = nbRef[ref._tab[1]._arr[nb]._nb1];
	}

	int st = unit->_root._sit.PlMoney(0);
	clSitHoldem sit = sit0;
	sit.ChangeStacks(st);
	int nextAct = sit.NbFirstAct(ACT_DELTURN);
	if (nextAct == -1) return;
	vector <stNodeW> vn;
	unit->_root._tree.FindSitNode(sit.CnAct() - nextAct, sit, nextAct, vn, 1);
	clHandsGroupEx ghCur[2], ghIn[2];
	ghIn[0] = gh[0];
	ghIn[1] = gh[1];
	gh[0].ClearDat();
	gh[1].ClearDat();
	for (int i = 0; i < vn.size(); i++)
	{
		ghCur[0] = ghIn[0];
		ghCur[1] = ghIn[1];
		ChangeGHForSit(unit->_root, (clHoldemTree *)vn[i]._ptr, ref, ghCur);
		double mult = vn[i]._weight;
		for (int j = 0; j < ALL_CN_HAND; j++)
		{
			gh[0][j] += ghCur[0][j] * mult;
			gh[1][j] += ghCur[1][j] * mult;
		}
	}
	bank.UnLockBankUnit(ref._nbTree);
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	FillAtrTree(clHoldemTree *nodeIn, clHoldemTree *node, double k)
{
	if (nodeIn->CnBranch() == 1 && node->CnBranch() == 1)
		FillAtrTree(nodeIn->Branch(0), node->Branch(0), k);
	if (nodeIn->CnBranch() >= 2 && node->CnBranch()>0)
	{
		int cnCls = nodeIn->AtrTree()->CnClasters();
		int cnBIn = nodeIn->CnBranch(), cnB = node->CnBranch();
		for (int i = 0; i < cnCls; i++)
		{
			clCalcParam cpIn = nodeIn->AtrTree()->CalcParam(i);
			clCalcParam cp = node->AtrTree()->CalcParam(i);
			cpIn.CnCalc() += cp.CnCalc()*k + 0.5;
			cpIn.WeightCalc() += cp.WeightCalc()*k;
			cpIn.AverRes() += cp.AverRes()*k;
		}

		for (int b = 0; b < cnBIn; b++)
		{
			stNbBtw nbb = { -1,-1,0,0 };
			node->FindBranchBetween(nodeIn->Branch(b)->GetDis(), nbb);
			if (nbb._nb0 == -1) continue;
			if (nbb._w1 < 0.1)
			{
				nbb._nb1 = -1;
				nbb._w0 = 1;
			}
			double k1 = k*nbb._w0;
			for (int i = 0; i < cnCls; i++)
			{
				clCalcParam cpIn = nodeIn->AtrTree()->CalcParam(i);
				clCalcParam cp = node->AtrTree()->CalcParam(i);
				cpIn.BeginEV()[b] += cp.BeginEV()[nbb._nb0] * k1;
				cpIn.BeginPart(cnBIn)[b] += cp.BeginPart(cnB)[nbb._nb0] * k1;
			}
			FillAtrTree(nodeIn->Branch(b), node->Branch(nbb._nb0), k1);
			if (nbb._nb1 != -1)
			{
				double k2 = k*nbb._w1;
				for (int i = 0; i < cnCls; i++)
				{
					clCalcParam cpIn = nodeIn->AtrTree()->CalcParam(i);
					clCalcParam cp = node->AtrTree()->CalcParam(i);
					cpIn.BeginEV()[b] += cp.BeginEV()[nbb._nb1] * k2;
					cpIn.BeginPart(cnBIn)[b] += cp.BeginPart(cnB)[nbb._nb1] * k2;
				}
				FillAtrTree(nodeIn->Branch(b), node->Branch(nbb._nb1), k2);
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
