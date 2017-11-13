#include <math.h>
#include <time.h>

//#include "C:\home\holdem\Strategy\Source\ClastersBoard.h"
//#include "C:\home\holdem\Strategy\Source\CalcSitEV.h"
//#include "c:\home\card\naborCards.h"
#include "TreeOnFile.h"
#include "..\..\..\Util\TemplateFileRec.h"

void	SaveTreeToFileA(clFileRecT <clAtrStratHoldem> &file, clTreeOnFile *treeF, clTreeStrat *tree);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		SaveTreeToFile(char *str, clStrategy &strat)
{
	clTreeStrat &tree = strat._tree;
	clTreeOnFile treeF;
	int cnR = tree.CnNode();
//	treeF = tree;
	clFileRecT <clAtrStratHoldem> file;
	file.Create(str, 0, cnR);
	SaveTreeToFileA(file, &treeF, &tree);
	CString ss = str;
	ss += ".bin";
	int handle=_open(ss.GetBuffer(), O_RDWR | O_BINARY | O_CREAT | O_TRUNC);
	if (handle == -1)
	{
		ErrMessage("Не открывается файл","");
		return -1;
	}
	strat._nameFileFlop.WriteFile(handle);
	strat._nameFileTurn.WriteFile(handle);
	strat._nameFileRiver.WriteFile(handle);
	cnR = treeF.WriteFile(handle);
	_close(handle);
	return cnR;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	SaveTreeToFileA(clFileRecT <clAtrStratHoldem> &file, clTreeOnFile *treeF, clTreeStrat *tree)
{
	clAtrStratHoldem atr;
	atr._cnB = tree->CnBranch();
	atr = *tree->AtrTree();
	treeF->AtrTree()->_nbRec = file.AddRecord(&atr);
	treeF->AtrTree()->_dis = tree->GetSit().GetLastAct()->_dis;
	for (int i = 0; i < tree->CnBranch(); i++)
	{
		clTreeOnFile *branch = (clTreeOnFile *)treeF->AddBranch();
		SaveTreeToFileA(file, branch, tree->Branch(i));
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void clAtrStratHoldem::operator = (clAtrTreeStrat &atr)
{
	//_sitA = atr._sitA;
	_vectInd = atr._vectInd;
	_statCn = atr._statCn;
	size_t dimA = atr.CnClasters();
	_param.resize(dimA*_cnB);
	for (int i = 0; i < dimA; i++)
	{
		clCalcParam0 cp = atr.CalcParam(i);
		cp.GetWeightStrat(_cnB, &_param[i*_cnB]);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		clAtrStratHoldem::WriteFile(int handle)
{
	//_write(handle, &_sitA, sizeof(clSitHoldem));
	_write(handle, &_cnB, sizeof(_cnB));
	WriteVectFile(handle, _vectInd);
	_write(handle, &_statCn, sizeof(_statCn));
	WriteVectFile(handle, _param);
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		clAtrStratHoldem::ReadFile(int handle)
{
	//_read(handle, &_sitA, sizeof(clSitHoldem));
	_read(handle, &_cnB, sizeof(_cnB));
	ReadVectFile(handle, _vectInd);
	_read(handle, &_statCn, sizeof(_statCn));
	ReadVectFile(handle, _param);
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpFloat0 *clAtrStratHoldem::GetStratWeight(int cnB,clSitHoldem &sit)
{
	int ind = sit.Indicator();
	if (ind == -1)
	{
		ErrMessage("Error", "");
		return NULL;
	}
	enNameTrade trade = sit.NameTrade();
	int nbH = 0;
	if (trade == TRADE_PREFLOP)
		nbH = GetHandIndex(sit.PlayerHand(ind));
	else if (trade == TRADE_FLOP)
		nbH = GetFlopHandIndex(sit.CardsTable(),sit.PlayerHand(ind));
	else if (trade == TRADE_TURN)
		nbH = GetTurnHandIndex(sit.CardsTable(), sit.PlayerHand(ind));
	else if (trade == TRADE_RIVER)
		nbH = GetRiverHandIndex(sit.CardsTable(), sit.PlayerHand(ind));
	if (_ptrIndex != NULL)
		nbH = _ptrIndex[0][nbH];
	if (_param.size() <= nbH*cnB)
	{
		ErrMessage("Error", " _param");
		return NULL;
	}
	return &_param[nbH*cnB];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeOnFile *clTreeOnFile::FindSitNode(clSitHoldem *sit0)
{
	clSitHoldem sit;
	sit = *sit0;
	int ind = sit.Indicator();
	int nbA = sit.CurNbAct();
	sit.FirstSit();
	return FindSitNode(sit, ind, nbA);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeOnFile *clTreeOnFile::FindSitNode(clSitHoldem &sit, int ind, int nbA)
{
	if (sit.Indicator() == ind && sit.CurNbAct() == nbA)
		return this;
	tpDis dis = sit.GetActIndicator()->_dis;
	clTreeOnFile *ptr = FindBranch(dis,sit.BigBlind());
	if (ptr != NULL)
	{
		sit.ExecDis(dis);
		return ptr->FindSitNode(sit, ind, nbA);
	}
	return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void		clTreeOnFile::FindSitNode(clSitHoldem *sit0, vector<stNodeW0> &vect)
{
	clSitHoldem sit;
	sit = *sit0;
	int ind = sit.Indicator();
	int nbA = sit.CurNbAct();
	sit.FirstSit();
	FindSitNode(sit, ind, nbA, vect,1);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool		clTreeOnFile::FindSitNode(clSitHoldem &sit, int ind, int nbA, vector<stNodeW0> &vect,double k)
{
	if (sit.Indicator() == ind && sit.CurNbAct() == nbA)
	{
		stNodeW0 nw;
		nw._ptr = this;
		nw._weight = k;
		vect.push_back(nw);
		return true;
	}
	tpDis dis = sit.GetActIndicator()->_dis;
	int nb1 = -1, nb2 = -1;
	double w1, w2;
	FindBranchBetween(dis, nb1, nb2, w1, w2);
	if (nb1 == -1)
	{
//		ErrMessage("Error", " FindBranchBetween");
		return false;
	}
	if (nb2 != -1 && w2>0.2)
	{
		clSitHoldem sit2 = sit;
		sit2.ExecDis(Branch(nb2)->GetDis());
		Branch(nb2)->FindSitNode(sit2, ind, nbA, vect, k*w2);
	}
	else w1 = 1;
	sit.ExecDis(Branch(nb1)->GetDis());
	Branch(nb1)->FindSitNode(sit, ind, nbA, vect, k*w1);
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clTreeOnFile::FindBranchBetween(tpDis dis, int &nb1, int &nb2, double &w1, double &w2)
{
	if (CnBranch() == 1)
	{
		nb1 = 0;
		nb2 = -1;
		w1 = 1;
		return;
	}
	if (dis._act == ACT_FOLD || dis._act == ACT_CHECK || dis._act == ACT_CALL || dis._act == ACT_ALL_IN)
	{
		for (int i = 0; i < CnBranch(); i++)
			if (Branch(i)->GetDis()._act == dis._act)
			{
				nb1 = i;
				nb2 = -1;
				w1 = 1;
				return;
			}
		nb1 = nb2 = -1;
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
					nb1 = i;
					nb2 = -1;
					w1 = 1;
					return;
				}
				if (disT._money > dis._money)
				{
					if (nbB == -1)
						nbB = i;
					else if (disT._money < Branch(nbB)->GetDis()._money)
						nbB = i;
				}
				else if (disT._money<dis._money)
					if (nbL == -1)
						nbL = i;
					else if (disT._money > Branch(nbL)->GetDis()._money)
						nbL = i;
			}
		}
		if (nbB == -1)
		{
			nb1 = nbL;
			w1 = 1;
			nb2 = -1;
			return;
		}
		if (nbL == -1)
		{
			nb1 = nbB;
			w1 = 1;
			nb2 = -1;
			return;
		}
		int dB = Branch(nbB)->GetDis()._money - dis._money;
		int dL = dis._money - Branch(nbL)->GetDis()._money;
		if (dB < dL)
		{
			nb1 = nbB;
			nb2 = nbL;
			w1 = (double)dL / (dB + dL);
			w2 = (double)dB / (dB + dL);
		}
		else
		{
			nb1 = nbL;
			nb2 = nbB;
			w1 = (double)dB / (dB + dL);
			w2 = (double)dL / (dB + dL);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_DIST		100

double DistBetweenDis(tpDis disT, tpDis disP, int bb)
{
	if (disP._act <= ACT_CALL)
		return(disT._act == disP._act) ? 0 : MAX_DIST;
	if (disT._act <= ACT_CHECK)
		return MAX_DIST;
	return (fabs((double)(disP._money - disT._money) / bb));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clTreeOnFile *clTreeOnFile::FindBranch(tpDis dis, int bb)
{
	for (int i = 0; i<_cnB; i++)
		if (Branch(i)->AtrTree()->_dis == dis)
			return Branch(i);
	double dist = MAX_DIST;
	int nbMin = -1;
	for (int i = 0; i < _cnB; i++)
	{
		double val = DistBetweenDis(Branch(i)->AtrTree()->_dis, dis,bb);
		if (dist > val)
		{
			dist = val;
			nbMin = i;
		}
	}
	return (nbMin==-1)? NULL : Branch(nbMin);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
