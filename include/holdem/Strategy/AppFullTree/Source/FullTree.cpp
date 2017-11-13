#include <math.h>
#include <time.h>

#include "..\..\..\..\card\naborCards.h"
#include "FullTree.h"
#include "..\..\..\..\Util\TemplateFileRec.h"
#include "..\..\..\..\Util\Sort.h"
#include "..\..\..\Common\clHandsCompare.h"
//#include "C:\home\holdem\Strategy\AppRiverTrees\Source\RiverTree.h"
//#include "C:\home\Holdem\Strategy\AppTurnTree\Source\clTurnBanks.h"
#include "..\..\Source\ClastersBoard.h"

char *glNameTurnClasters[] = { "Turn1600K.bin" ,"Turn309K.bin" ,"Turn122K.bin" ,"Turn30K.bin" ,"Turn10K.bin" ,"Turn3K.bin","Turn1500.bin" };
int	 glSizeTurnClasters[] = { 1600000, 309000 ,122000 ,30000 ,10000 ,3000, 1500 };
char *glNameRiverClasters[] = { "River416K.bin" ,"River85K.bin" ,"River17K.bin" ,"River3600.bin" };
int	 glSizeRiverClasters[] = { 6500, 3000 ,1500 ,600 };
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void		clFlopTree::CalcWeightStrat(clHandsGroupEx **gh)
{
	if (CnBranch() == 0)
		return;
	else if (CnBranch() == 1)
	{
		Branch(0)->_weight = 1;
		Branch(0)->CalcWeightStrat(gh);
	}
	else
	{
		int ind = Indicator(), opp = (ind == 0) ? 1 : 0;
		int dim = AtrTree()->CnClasters();
		tpFloat *w = new tpFloat[CnBranch()*dim];
		ZeroMemory(w, CnBranch()*dim * sizeof(tpFloat));
		AtrTree()->GetAllWeightStrat(CnBranch(), w);
		clHandsGroupEx ghCur(dim), *gh0[2];
		gh0[ind] = &ghCur;
		gh0[opp] = gh[opp];
		tpFloat sum = 0, w0[MAX_CN_BRANCH];
		for (int k = 0; k < CnBranch(); k++)
		{
			for (int i = 0; i < CN_HAND; i++)
				ghCur[i] = gh[ind]->_vect[i] * w[i*CnBranch() + k];
			Branch(k)->CalcWeightStrat(gh0);
			w0[k] = 0;
			for (int i0 = 0; i0 < CN_HAND; i0++)
				for (int i1 = 0; i1 < CN_HAND; i1++)
					w0[k] += gh0[0]->_vect[i0] * gh0[1]->_vect[i1] * HandMultCrossHand169(i0, i1);
			sum += w0[k];
		}
		if (sum > DOUBLE_0)
			for (int i = 0; i < CnBranch(); i++)
				Branch(i)->_weight = w0[i] / sum;
		delete w;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	clFlopTree::FindSitNode(clSitHoldem &sit, int nextAct, vector <stNodeW> &vect, double k)
{
	int nbA = GetSit().CurNbAct();
	if (sit.CurNbAct() == nbA && GetSit().Indicator() == sit.Indicator())
	{
		stNodeW nw;
		nw._ptr = this;
		nw._weight = k;
		vect.push_back(nw);
		return;
	}
	tpDis dis = sit.GetAct(nextAct)->_dis;
	int nb1 = -1, nb2 = -1;
	double w1, w2;
	FindBranchBetween(dis, nb1, nb2, w1, w2);
	if (nb1 == -1)
		return;
	if (nb2 != -1 && w2 > 0.2)
		Branch(nb2)->FindSitNode(sit, nextAct + 1, vect, k*w2);
	else w1 = 1;
	Branch(nb1)->FindSitNode(sit, nextAct + 1, vect, k*w1);
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clFullTreeRoot::SizeInFile()
{
	size_t size = clRoot::SizeInFile();

	size += SizeVectFile(_tabFlop[0]);
	size += SizeVectFile(_tabFlop[1]);
	size += SizeVectFile(_tabFlop[2]);
	size += SizeVectFile(_tabTurn[0]);
	size += SizeVectFile(_tabTurn[1]);
	size += SizeVectFile(_tabTurn[2]);
	size += SizeVectFile(_tabRiver[0]);
	size += SizeVectFile(_tabRiver[1]);
	size += SizeVectFile(_tabRiver[2]);
	return size + 9 * sizeof(int);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	clFullTreeRoot::WriteFile(int handle)
{
	clRoot::WriteFile(handle);
	WriteVectFile(handle, _tabFlop[0]);
	WriteVectFile(handle, _tabFlop[1]);
	WriteVectFile(handle, _tabFlop[2]);
	WriteVectFile(handle, _tabTurn[0]);
	WriteVectFile(handle, _tabTurn[1]);
	WriteVectFile(handle, _tabTurn[2]);
	WriteVectFile(handle, _tabRiver[0]);
	WriteVectFile(handle, _tabRiver[1]);
	WriteVectFile(handle, _tabRiver[2]);
	_write(handle, _cnFlop, sizeof(_cnFlop));
	_write(handle, _cnTurn, sizeof(_cnTurn));
	_write(handle, _cnRiver, sizeof(_cnRiver));
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	clFullTreeRoot::ReadFile(int handle)
{
	clRoot::ReadFile(handle);
	ReadVectFile(handle, _tabFlop[0]);
	ReadVectFile(handle, _tabFlop[1]);
	ReadVectFile(handle, _tabFlop[2]);
	ReadVectFile(handle, _tabTurn[0]);
	ReadVectFile(handle, _tabTurn[1]);
	ReadVectFile(handle, _tabTurn[2]);
	ReadVectFile(handle, _tabRiver[0]);
	ReadVectFile(handle, _tabRiver[1]);
	ReadVectFile(handle, _tabRiver[2]);
	_read(handle, _cnFlop, sizeof(_cnFlop));
	_read(handle, _cnTurn, sizeof(_cnTurn));
	_read(handle, _cnRiver, sizeof(_cnRiver));

	InitDeck();
	CalcIndex();
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clFullTreeRoot::CalcTree(size_t cnCalc)
{
	tpTreePlayerInfo info;
	tpBooleanDeck deck;
	tpCard card[5];
	memcpy(card, _cards, sizeof(card));

	int cn0 = CnHand(0), cn1 = CnHand(1);
	enNameTrade trade = _sit.NameTrade();
	for (int i0 = 0; i0 < cnCalc;)
	{
		int r0 = random(cn0);
		int r1 = random(cn1);
		int nb0 = NbHand(0, r0);
		int nb1 = NbHand(1, r1);
		if (HandCrossHandNB(nb0, nb1))
			continue;
		//glTrasser.WriteMessage(i0);
		tpHand h0(nb0), h1(nb1);
		deck = _deck;
		deck[h0[0]] = false;
		deck[h0[1]] = false;
		deck[h1[0]] = false;
		deck[h1[1]] = false;
		int nbT0,nbR0=-1, nbT1, nbR1;
		if (trade == TRADE_PREFLOP)
		{
			card[0] = deck.GiveRandomCard(48);
			card[1] = deck.GiveRandomCard(47);
			card[2] = deck.GiveRandomCard(46);
			card[3] = deck.GiveRandomCard(45);
			card[4] = deck.GiveRandomCard(44);
			nbR0 = GetTurnAndRiverHandIndex(card, h0, nbT0);
			nbR1 = GetTurnAndRiverHandIndex(card, h1, nbT1);
			info._nbCls[TRADE_PREFLOP][0] = HandAllGetNb169(nb0);
			info._nbCls[TRADE_PREFLOP][1] = HandAllGetNb169(nb1);
			info._nbCls[TRADE_FLOP][0] = _tabFlop[0][GetFlopHandIndex(card, h0)];
			info._nbCls[TRADE_FLOP][1] = _tabFlop[1][GetFlopHandIndex(card, h1)];
			info._nbCls[TRADE_TURN][0] = _tabTurn[0][nbT0];
			info._nbCls[TRADE_TURN][1] = _tabTurn[1][nbT1];
			info._nbCls[TRADE_RIVER][0] = _tabRiver[0][nbR0];
			info._nbCls[TRADE_RIVER][1] = _tabRiver[1][nbR1];
		}
		else if (trade == TRADE_FLOP)
		{
			card[3] = deck.GiveRandomCard(45);
			card[4] = deck.GiveRandomCard(44);
			info._nbCls[TRADE_FLOP][0] = r0;
			info._nbCls[TRADE_FLOP][1] = r1;
			nbT0 = r0*CN_CARD_COL + card[3]._card;
			nbT1 = r1*CN_CARD_COL + card[3]._card;
			info._nbCls[TRADE_TURN][0] = _tabTurn[0][nbT0];
			info._nbCls[TRADE_TURN][1] = _tabTurn[1][nbT1];
			info._nbCls[TRADE_RIVER][0] = _tabRiver[0][nbT0*CN_CARD_COL + card[4]._card];
			info._nbCls[TRADE_RIVER][1] = _tabRiver[1][nbT1*CN_CARD_COL + card[4]._card];

			if (info._nbCls[TRADE_TURN][0] == -1 || info._nbCls[TRADE_TURN][1] == -1 || info._nbCls[TRADE_RIVER][0] == -1 || info._nbCls[TRADE_RIVER][1] == -1)
				ErrMessage("-1 ", "Error");
		}
		else if (trade == TRADE_TURN)
		{
			card[4] = deck.GiveRandomCard(44);
			info._nbCls[TRADE_TURN][0] = r0;
			info._nbCls[TRADE_TURN][1] = r1;
			info._nbCls[TRADE_RIVER][0] = r0*CN_CARD_COL + card[4]._card;
			info._nbCls[TRADE_RIVER][1] = r1*CN_CARD_COL + card[4]._card;
		}
		else
		{
			info._nbCls[TRADE_RIVER][0] = r0;
			info._nbCls[TRADE_RIVER][1] = r1;
		}
		if (nbR0 == -1)
		{
			nbR0 = GetTurnAndRiverHandIndex(card, h0, nbT0);
			nbR1 = GetTurnAndRiverHandIndex(card, h1, nbT1);
		}
		info._handForce[0] = glVectRiverHandGame[nbR0].ToInt();
		info._handForce[1] = glVectRiverHandGame[nbR1].ToInt();
		tpFloat k0 = (i0 % 1000) ? 0 : 0.01;
		_tree.CalcNesh(0, &info, _gh[0][nb0], _gh[1][nb1], k0);
		_tree.CalcNesh(1, &info, _gh[1][nb1], _gh[0][nb0], k0);
		i0++;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*double clFullTreeRoot::CalcAntistrategy()
{
	vector <float> weight[2];
	CalcAbsWeightGH(weight);
	return 0;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clFullTreeRoot::CalcEvTreeTurn(vector <float> *ev)
{
	InitDeck();
	tpCard card[5];
	memcpy(card, _cards, sizeof(card));

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
		tpHand h0(nb0);
		for (int i1 = 0; i1 < dim1; i1++)
		{
			int nb1 = NbHand(1, i1);
			tpHand h1(nb1);
			if (!HandCrossHandNB(nb0, nb1))
			{
				float ww = _gh[0][nb0] * _gh[1][nb1];

				for(int k=0;k<CN_CARD_COL;k++)
					if (_deck[k])
					{
						card[4] = k;
						info._nbCls[TRADE_TURN][0] = i0;
						info._nbCls[TRADE_TURN][1] = i1;
						info._nbCls[TRADE_RIVER][0] = i0*CN_CARD_COL + k;
						info._nbCls[TRADE_RIVER][1] = i1*CN_CARD_COL + k;

						int nbT, nbR0 = GetTurnAndRiverHandIndex(card, h0, nbT);
						int nbR1 = GetTurnAndRiverHandIndex(card, h1, nbT);
						info._handForce[0] = glVectRiverHandGame[nbR0].ToInt();
						info._handForce[1] = glVectRiverHandGame[nbR1].ToInt();
						clDoubleCP res0 = _tree.CalcStratEV(&info);
						ev[0][nb0] += res0[0] * ww;
						ev[1][nb1] += res0[1] * ww;
						w0[i0] += ww;
						w1[i1] += ww;
					}
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
void	clFullTreeRoot::CreateTabFlop()
{
	_tabFlop[0].resize(ALL_CN_HAND, 0);
	_tabFlop[1].resize(ALL_CN_HAND, 0);
	_cnFlop[0] = CnHand(0);
	_cnFlop[1] = CnHand(1);
	for (int i = 0; i < _cnFlop[0]; i++)
		_tabFlop[0][NbHand(0, i)] = i;
	for (int i = 0; i < _cnFlop[1]; i++)
		_tabFlop[1][NbHand(1, i)] = i;

	_tabTurn[0].resize(_cnFlop[0] * CN_CARD_COL, -1);
	_tabTurn[1].resize(_cnFlop[1] * CN_CARD_COL, -1);


	_tabRiver[0].resize(_cnFlop[0] * CN_CARD_COL*CN_CARD_COL, -1);
	_tabRiver[1].resize(_cnFlop[1] * CN_CARD_COL*CN_CARD_COL, -1);

	InitDeck();

	int *indTurn = (int *)malloc(CN_NABOR_HAND_TURN * sizeof(int));
	int *indRiver = (int *)malloc(CN_NABOR_HAND_RIVER * sizeof(int));
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		_cnTurn[nbPl] = 0;
		_cnRiver[nbPl] = 0;
		memset(indTurn, 255, CN_NABOR_HAND_TURN * sizeof(int));
		memset(indRiver, 255, CN_NABOR_HAND_RIVER * sizeof(int));
		for (int j = 0; j < _cnFlop[nbPl]; j++)
		{
			int nb = NbHand(nbPl, j);
			{
				tpHand hand(nb);
				_deck[hand[0]] = false;
				_deck[hand[1]] = false;
				for (int k = 0; k < CN_CARD_COL; k++)
					if (_deck[k])
					{
						_cards[3] = k;
						_deck[k] = false;
						int nbT = GetTurnHandIndex(_cards, hand);
						if (indTurn[nbT] == -1)
							indTurn[nbT] = _cnTurn[nbPl]++;
						_tabTurn[nbPl][j*CN_CARD_COL + k] = indTurn[nbT];
						for (int k1 = 0; k1 < CN_CARD_COL; k1++)
							if (_deck[k1])
							{
								_cards[4] = k1;
								int nbR = GetTurnAndRiverHandIndex(_cards, hand, nbT);
								if (indRiver[nbR] == -1)
									indRiver[nbR] = _cnRiver[nbPl]++;
								_tabRiver[nbPl][j*CN_CARD_COL*CN_CARD_COL + k*CN_CARD_COL + k1] = indRiver[nbR];
							}
						_deck[k] = true;
					}
				_deck[hand[0]] = true;
				_deck[hand[1]] = true;
			}
		}
	}
	free(indTurn);
	free(indRiver);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clFullTreeRoot::CreateTabTurn()
{
	_cnFlop[0] = _cnFlop[1] = 0;
	_tabTurn[0].resize(ALL_CN_HAND, 0);
	_tabTurn[1].resize(ALL_CN_HAND, 0);
	_cnTurn[0] = CnHand(0);
	_cnTurn[1] = CnHand(1);
	for (int i = 0; i < _cnTurn[0]; i++)
		_tabTurn[0][NbHand(0, i)] = i;
	for (int i = 0; i < _cnTurn[1]; i++)
		_tabTurn[1][NbHand(1, i)] = i;
	_cnRiver[0] = _cnTurn[0] * CN_CARD_COL;
	_cnRiver[1] = _cnTurn[1] * CN_CARD_COL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clFullTreeRoot::CreateTabRiver()
{
	_cnFlop[0] = _cnFlop[1] = 0;
	_cnTurn[0] = _cnTurn[1] = 0;
	_tabRiver[0].resize(ALL_CN_HAND, 0);
	_tabRiver[1].resize(ALL_CN_HAND, 0);
	_cnRiver[0] = CnHand(0);
	_cnRiver[1] = CnHand(1);
	for (int i = 0; i < _cnRiver[0]; i++)
		_tabRiver[0][NbHand(0, i)] = i;
	for (int i = 0; i < _cnRiver[1]; i++)
		_tabRiver[1][NbHand(1, i)] = i;

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clFullTreeRoot::SetStartParam(bool setMem)
{
	int arrPF[3], arrFlop[3], arrTurn[3], arrRiver[3];
	int *arrCls[] = { arrPF,arrFlop,arrTurn,arrRiver };
	arrPF[0] = CN_HAND;
	arrPF[1] = CN_HAND;
	arrFlop[0] = _cnFlop[0];
	arrFlop[1] = _cnFlop[1];
	arrTurn[0] = _cnTurn[0];
	arrTurn[1] = _cnTurn[1];
	arrRiver[0] = _cnRiver[0];
	arrRiver[1] = _cnRiver[1];
	size_t size = _tree.SetStartParam(arrCls, setMem);
	if(setMem)
		_tree.SetResSit();
	return size;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*int		CutAndNorm(vector <tpDisNode> &arr)
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
		if (arr[i]._weight > 0.000003)
		{
			sum += arr[i]._weight;
			arr[k++] = arr[i];
		}
	if (sum > DOUBLE_0)
		for (int i = 0; i < k; i++)
			arr[i]._weight /= sum;
	arr.resize(k);
	return k;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		CompareSit(clSitHoldem &sit, clSitHoldem &sit0)
{
	if (sit.CurAct() < sit0.CurAct())
		return -1;
	if (sit.CnCardTable() < sit0.CnCardTable())
		return -1;
	for (int i = 0; i < sit0.CnCardTable(); i++)
		if (sit.CardsTable()[i] != sit0.CardsTable()[i])
			return -1;
	clGameHistory hist1 = *sit.PtrHistory(), hist2 = *sit0.PtrHistory();
	hist1._arrAct._cnAct = sit0.CurAct();
	hist2._arrAct._cnAct = sit0.CurAct();
	if (hist1 == hist2)
		return sit0.CurAct();
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clCalcParam		clFullTreeRoot::GetCalcParam(clSitHoldem &sit, clHoldemTree *node)
{
	int ind = node->Indicator();
	int nb=sit.PlayerHand(ind).NbHandAll();
	enNameTrade trade = _sit.NameTrade();
	if (trade == TRADE_PREFLOP)
	{
		tpHand hand(nb);
		switch (node->GetSit().NameTrade())
		{
			case TRADE_PREFLOP	: nb = HandAllGetNb169(nb); break;
			case TRADE_FLOP:
				nb = GetFlopHandIndex(_cards, hand);
				nb = _tabFlop[ind][nb];
				break;
			case TRADE_TURN:
				nb = GetTurnHandIndex(_cards, hand);
				nb = _tabTurn[ind][nb];
				break;
			case TRADE_RIVER:
			{
				int nbT;
				nb = GetTurnAndRiverHandIndex(_cards, hand, nbT);
				nb = _tabRiver[ind][nb];
				break;
			}
		}
	}
	else if (trade == TRADE_FLOP)
	{
		nb = _tabFlop[ind][nb];
		trade = node->GetSit().NameTrade();
		if (trade == TRADE_TURN)
			nb = _tabTurn[ind][nb*CN_CARD_COL + sit.CardsTable()[3].Number()];
		else if (trade == TRADE_RIVER)
		{
			nb = nb*CN_CARD_COL + sit.CardsTable()[3].Number();
			nb = _tabRiver[ind][nb*CN_CARD_COL + sit.CardsTable()[4].Number()];
		}
	}
	else if (trade == TRADE_TURN)
	{
		nb = _tabTurn[ind][nb];
		trade = node->GetSit().NameTrade();
		if (trade == TRADE_RIVER)
			nb = nb*CN_CARD_COL + sit.CardsTable()[4].Number();
	}
	else 
	{
		nb = _tabRiver[ind][nb];
	}
	return node->AtrTree()->CalcParam(nb);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clFullTreeRoot::GetDis(clSitHoldem &sit, vector <tpDisNode> &val)
{
	int nbA = CompareSit(sit, _sit);
	if (nbA == -1)
		return HistCreateDis(ACT_UNDEF);
	vector <stNodeW> vect;
	_tree.FindSitNode(sit.CurNbAct() - nbA, sit, nbA, vect);
	if (vect.size() == 0)
		return HistCreateDis(ACT_UNDEF);

	enNameTrade trade = sit.NameTrade();
	for (int i = 0; i < vect.size(); i++)
	{
		clHoldemTree *node = (clHoldemTree *)vect[i]._ptr;
		clCalcParam cls = GetCalcParam(sit, node);
		tpFloat w[MAX_CN_BRANCH];
		cls.GetWeightStrat(node->CnBranch(), w);
		for (int j = 0; j < node->CnBranch(); j++)
		{
			tpDisNode dn;
			dn._dis = node->Branch(j)->GetDis();
			dn._weight = w[j] * vect[i]._weight;
			dn._ev = cls.AbsEV(j);
			val.push_back(dn);
		}
	}
	int cn = CutAndNorm(val,0.01);
	int nbB = cn - 1;
	tpFloat rVal = (tpFloat)rand() / RAND_MAX;
	for (int i = 0; i < cn; i++)
		if (val[i]._weight > rVal)
		{
			nbB = i;
			break;
		}
		else
			rVal -= val[i]._weight;
	tpDis dis = sit.CreateDis(val[nbB]._dis);
	for (int i = 0; i < cn; i++)
		for (int j = i + 1; j < cn; j++)
			if (val[i]._weight < val[j]._weight)
			{
				tpDisNode temp = val[i];
				val[i] = val[j];
				val[j] = temp;
			}
	return dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*clCalcParam		clRootFlopTree::GetCalcParam(clSitHoldem &sit, clFlopTree *node)
{
	if (node->AtrTree()->CnClasters() == 1) return node->AtrTree()->CalcParam(0);
	enNameTrade trade = sit.NameTrade();
	int nbH = ((clHand *)sit.IndicatorCards())->NbHandAll();
	if (trade == TRADE_TURN)
		nbH = _tabTurn[NbTurn(nbH, sit.CardsTable()[3].Number())];
	else if (trade == TRADE_RIVER)
		nbH = _tabRiver[NbRiver(nbH, sit.CardsTable()[3].Number(), sit.CardsTable()[4].Number())];

	return node->AtrTree()->CalcParam(nbH);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
struct tpAtrZeroTree
{
	int SizeInFile() { SizeVectFile(_vect); }
	void ReadFile(int handle) { ReadVectFile(handle, _vect); }
	void WriteFile(int handle) { WriteVectFile(handle, _vect); }
	void	Clear() {}

	vector <byte> _vect;
};

class clTreeZero : public clTree <tpAtrZeroTree>
{
public:
	/*	void  CopyTree(clTreeStrat *tree)
		{
			ClearAllBranch();
			for (int i = 0; i < tree->CnBranch(); i++)
			{
				_arr[_cnB] = new clTree();
				_arr[_cnB]->_parent = this;
				((clTreeZero *)_arr[_cnB])->CopyTree(tree->Branch(i));
				_cnB++;
			}
			_weight = tree->_weight;
			_name = tree->_name;
			AtrTree()->_vect.resize(sizeof(tpDis));
			tpDis dis = tree->GetDis();
			memcpy(&AtrTree()->_vect[0], &dis, sizeof(tpDis));
		}*/
		/*	clTreeZero *Branch(int nb) { return (clTreeZero *)clTree < tpAtrZeroTree >::Branch(nb); }
			void  CopyTreeTo(clFlopTree *ptrT, clSitHoldem &sit)
			{
				ptrT->GetSit() = sit;
				ptrT->_name = _name;
				ptrT->ClearAllBranch();
				for (int i = 0; i < CnBranch(); i++)
				{
					clSitHoldem sit0 = sit;
					tpDis dis;
					memcpy(&dis, &Branch(i)->AtrTree()->_vect[0], sizeof(tpDis));
					sit0.ExecDis(dis);
					int nb = ptrT->AddBranch(sit0);
					Branch(i)->CopyTreeTo(ptrT->Branch(nb), sit0);
				}
			}
		};*/

int FuncNodePreCalc(clHoldemTree *node, enNameTrade trade)
{
	int res = (node->GetSit().NameTrade() == trade && node->GetSit().Indicator() != -1) ? clCalcParam::SizeParam(node->CnBranch()) : 0;
	for (int i = 0; i < node->CnBranch(); i++)
		res += FuncNodePreCalc(node->Branch(i), trade);
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clFullTreeRoot::ClearTab()
{
	_tabFlop[0].clear();
	_tabFlop[1].clear();
	_tabFlop[2].clear();
	_tabTurn[0].clear();
	_tabTurn[1].clear();
	_tabTurn[2].clear();
	_tabRiver[0].clear();
	_tabRiver[1].clear();
	_tabRiver[2].clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clFullTreeRoot::CreateTreeNL(int sizeMb, bool &setMem)
{
	ClearTab();
	InitDeck();
	CalcIndex();
	_tree.ClearAllBranch();
	_tree.AtrTree()->_sitA = _sit;
	BuildTreeNLAuto(&_tree, _sit);
#ifdef NB_NODE
	int nb = 0;
	int cnNode = _tree.SetNbNode(nb);
#endif
	enNameTrade trade = _sit.NameTrade();
	if (trade == TRADE_PREFLOP)
		return CreatePreFlopTreeNL(sizeMb, setMem);
	else if (trade == TRADE_FLOP)
		return CreateFlopTreeNL(sizeMb, setMem);
	else
	{
		if (trade == TRADE_TURN)
			CreateTabTurn();
		else
			CreateTabRiver();
		setMem = true;
		return SetStartParam(setMem);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clFullTreeRoot::CreateFlopTreeNL(int sizeMb, bool &setMem)
{
	size_t sizeIn = (size_t)sizeMb * 1024 * 1024;
	CreateTabFlop();
	if (setMem)
		return SetStartParam(setMem);

	size_t size = SetStartParam(setMem);
	if (size < sizeIn)
	{
		setMem = true;
		return SetStartParam(setMem);
	}
	int iR = sizeof(glSizeRiverClasters) / sizeof(int);
	int iT = sizeof(glSizeTurnClasters) / sizeof(int);

	size_t dR = 1E13, dT = 1E12;
	while (iR > 0 || iT > 0)
	{
		if ((dR > dT && iR > 0) || iT == 0)
		{
			ReCalcRiverInd(--iR);
			size_t size1 = SetStartParam(setMem);
			if (size1 < sizeIn)
			{
				setMem = true;
				return SetStartParam(setMem);
			}
			dR = size - size1;
			size = size1;
		}
		else
		{
			ReCalcTurnInd(--iT);
			size_t size1 = SetStartParam(setMem);
			if (size1 < sizeIn)
			{
				setMem = true;
				return SetStartParam(setMem);
			}
			dT = size - size1;
			size = size1;
		}
	}
	return size;
}
////--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t	clFullTreeRoot::CreatePreFlopTreeNL(int sizeMb, bool &setMem)
{

	ErrMessage("Не реализованная функция ", "CreatePreFlopTreeNL");
	return 0;
}
////--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clFullTreeRoot::ReCalcTurnInd(int nbFile)
{
	clAnsiString as = (clAnsiString)PATH_CLASTERS + glNameTurnClasters[nbFile];
	int handle = _open(as.c_str(), O_BINARY | O_RDONLY);
	if (handle == -1)
	{
		ErrMessage("Не открывается файл ", as);
		return;
	}
	unsigned dim;
	_read(handle, &dim, sizeof(unsigned));
	int *ind = (int *)malloc(CN_NABOR_HAND_TURN * sizeof(int));
	ReadBigBlock(handle, ind, dim * sizeof(int));
	_close(handle);

	int cnCls = glSizeTurnClasters[nbFile] * 2;
	int *indTurn = (int *)malloc(cnCls * sizeof(int));

	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		memset(indTurn, 255, cnCls * sizeof(int));
		_cnTurn[nbPl] = 0;
		for (int j = 0; j < CnHand(nbPl); j++)
		{
			int nb = NbHand(nbPl, j);
			tpHand hand(nb);
			_deck[hand[0]] = false;
			_deck[hand[1]] = false;
			for (int k = 0; k < CN_CARD_COL; k++)
				if (_deck[k])
				{
					_cards[3] = k;
					int nbT = GetTurnHandIndex(_cards, hand);
					nbT = ind[nbT];
					if (indTurn[nbT] == -1)
						indTurn[nbT] = _cnTurn[nbPl]++;
					_tabTurn[nbPl][j*CN_CARD_COL + k] = indTurn[nbT];
				}
			_deck[hand[0]] = true;
			_deck[hand[1]] = true;
		}
	}
	free(ind);
	free(indTurn);
}
////--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clFullTreeRoot::ReCalcRiverInd(int nbFile)
{
	clAnsiString as = (clAnsiString)PATH_CLASTERS + glNameRiverClasters[nbFile];
	int handle = _open(as.c_str(), O_BINARY | O_RDONLY);
	if (handle == -1)
	{
		ErrMessage("Не открывается файл ", as);
		return;
	}
	unsigned dim;
	_read(handle, &dim, sizeof(unsigned));
	int *ind = (int *)malloc(CN_NABOR_HAND_RIVER * sizeof(int));
	ReadBigBlock(handle, ind, dim * sizeof(int));
	_close(handle);

	int cnCls = 500000;
	int *indRiver = (int *)malloc(cnCls * sizeof(int));
	for (int nbPl = 0; nbPl < 2; nbPl++)
	{
		memset(indRiver, 255, cnCls * sizeof(int));
		_cnRiver[nbPl] = 0;
		for (int j = 0; j < CnHand(nbPl); j++)
		{
			int nb = NbHand(nbPl, j);
			tpHand hand(nb);
			_deck[hand[0]] = false;
			_deck[hand[1]] = false;
			for (int k = 0; k < CN_CARD_COL; k++)
				if (_deck[k])
				{
					_cards[3] = k;
					_deck[k] = false;
					for (int k1 = 0; k1 < CN_CARD_COL; k1++)
						if (_deck[k1])
						{
							_cards[4] = k1;
							int nbT;
							int nbR = GetTurnAndRiverHandIndex(_cards, hand, nbT);
							nbR = ind[nbR];
							if (indRiver[nbR] == -1)
								indRiver[nbR] = _cnRiver[nbPl]++;
							_tabRiver[nbPl][j*CN_CARD_COL*CN_CARD_COL + k*CN_CARD_COL + k1] = indRiver[nbR];
						}
					_deck[k] = true;
				}
			_deck[hand[0]] = true;
			_deck[hand[1]] = true;
		}
	}
	free(ind);
	free(indRiver);
}
////--------------------------------------------------------------------------------------------------------------------------------------------------------------
void clFullTreeRoot::operator = (clFullTreeRoot &root)
{
	*((clRoot *)this) = *((clRoot*)&root);
	for (int i = 0; i < 3; i++)
	{
		_tabFlop[i] = root._tabFlop[i];
		_tabTurn[i] = root._tabTurn[i];
		_tabRiver[i] = root._tabRiver[i];
		_cnFlop[i] = root._cnFlop[i];
		_cnTurn[i] = root._cnTurn[i];
		_cnRiver[i] = root._cnRiver[i];
	}
}
////--------------------------------------------------------------------------------------------------------------------------------------------------------------
