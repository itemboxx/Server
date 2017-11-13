#include <math.h>
#include <time.h>

#include "ClastersBoard.h"
#include "Strategy.h"
#include "StrategyPlay.h"
#include "..\..\Common\HoldemTree.h"
#include "..\..\..\Util\FileRec.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	FindBranchBetween(clTreeStrat *node, tpDis dis, int &nb1, int &nb2, double &w1, double &w2)
{
	if (dis._act == ACT_FOLD || dis._act == ACT_CHECK || dis._act == ACT_CALL || dis._act == ACT_ALL_IN)
	{
		for (int i = 0; i < node->CnBranch(); i++)
			if (node->Branch(i)->GetDis()._act == dis._act)
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
		int nbB=-1, nbL=-1;
		for (int i = 0; i < node->CnBranch(); i++)
		{
			tpDis &disT = node->Branch(i)->GetDis();
			if (disT._money > 0)
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
					else if (disT._money < node->Branch(nbB)->GetDis()._money)
						nbB = i;
				}
				else if (disT._money<dis._money)
					if (nbL == -1)
						nbL = i;
					else if (disT._money > node->Branch(nbL)->GetDis()._money)
						nbL = i;
			}
		}
		if (nbB == -1)
		{
			nb1 = nbL;
			nb2 = -1;
			w1 = 1;
			return;
		}
		if (nbL == -1)
		{
			nb1 = nbB;
			nb2 = -1;
			w1 = 1;
			return;
		}
		int bb = node->GetSit().BigBlind();
		int dB = node->Branch(nbB)->GetDis()._money - dis._money;
		int dL = dis._money - node->Branch(nbL)->GetDis()._money;
		if (dB < dL)
		{
			nb1 = nbB;
			nb2 = nbL;
			w1 = (double)dL/(dB+dL);
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
bool	ChangeGroupHandsIn(clTreeStrat *node, clSitHoldem &sitIn, clHandsGroupEx *ghH)
{
	int nbA = node->GetSit().CurNbAct();
	if ( nbA == sitIn.CurNbAct())
		return true;

	tpDis dis = sitIn.GetAct(nbA)->_dis;
	int nbH = node->GetSit().Indicator();
	if (nbH == -1)
	{
		for (int i1 = 0; i1 < sitIn.CnPlayer(); i1++)
			if (ghH[i1]._vect.size() == ALL_CN_HAND)
			{
				switch (dis._act)
				{
					case ACT_DELFLOP:
						ghH[i1].SkipPreFlopAllHands(sitIn.CardsTable()[0]);
						ghH[i1].SkipPreFlopAllHands(sitIn.CardsTable()[1]);
						ghH[i1].SkipPreFlopAllHands(sitIn.CardsTable()[2]);
						break;
					case ACT_DELTURN: ghH[i1].SkipPreFlopAllHands(sitIn.CardsTable()[3]); break;
					case ACT_DELRIVER: ghH[i1].SkipPreFlopAllHands(sitIn.CardsTable()[4]); break;
				}
				ghH[i1].Norm();
			}
		return ChangeGroupHandsIn(node->Branch(0), sitIn, ghH);
	}
	else
	{
		int nb1 = -1, nb2 = -1;
		double w1, w2;
		FindBranchBetween(node, dis, nb1, nb2, w1, w2);
		if (nb1 == -1)
		{
#ifdef COMP1
			ErrMessage("ChangeGroupHandsIn", " FindBranchBetween");
#endif
			return false;
		}
		tpCard board[5];
		int cnCT = node->Branch(nb1)->GetSit().CnCardTable();
		for (int i = 0; i < cnCT; i++)
			board[i] = sitIn.CardsTable()[i];
		if (nb2 != -1 && w2>0.2)
		{
			clHandsGroupEx gh[CN_PLAYER];
			for (int i = 0; i < sitIn.CnPlayer(); i++)
				gh[i] = ghH[i];
			if (ghH[nbH]._vect.size() == ALL_CN_HAND)
				node->ChangeGHAll(nb2, board, gh[nbH]);
			return ChangeGroupHandsIn(node->Branch(nb2), sitIn, gh);
			if (ghH[nbH]._vect.size() == ALL_CN_HAND)
				node->ChangeGHAll(nb1, board, ghH[nbH]);
			return ChangeGroupHandsIn(node->Branch(nb1), sitIn, ghH);
			for (int i = 0; i < sitIn.CnPlayer(); i++)
				if (ghH[nbH]._vect.size() == ALL_CN_HAND)
					for (int j = 0; j < ALL_CN_HAND; j++)
						ghH[i][j] = ghH[i][j]*w1+gh[i][j]*w2;
		}
		else
		{
			if (ghH[nbH]._vect.size() == ALL_CN_HAND)
				node->ChangeGHAll(nb1, board, ghH[nbH]);
			return ChangeGroupHandsIn(node->Branch(nb1), sitIn, ghH);
		}
	}
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHistCalcS::BuildTree(clSitHoldem &sit0, clSitHoldem &sit1)
{
	_tree.AtrTree()->_verTree = TREE_FOR_CALC;
	//_tree.AtrTree()->_verTree = 0;
	clStrategy::CreateTreeNL(sit0, sit1);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clHistCalcS::RestraightTree(clSitHoldem &sit)
{
	clTreeStrat *node = _tree.FindBeforeEqualDis(sit);
	clSitHoldem sit0 = node->GetSit();
//	if (ChangeGroupHandsIn(&_tree, sit0, _handsIn))
	{
//		BuildTree(sit0, sit);
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define CN_THREAD	8
struct stThreadNeshAtr
{
	bool _stop;
	clStrategy *_strat;
	int _rand0,_cnCalc;
};

unsigned long _stdcall CalcThreadNesh(void *ptr)
{
	stThreadNeshAtr *atr = (stThreadNeshAtr *)ptr;
	srand(atr->_rand0);
	//srand(0);
	int cnCalc = 10000;
	while (!atr->_stop)
	//while (atr->_cnCalc<100000)
	{
		atr->_strat->CalcNesh(cnCalc);
//		atr->_strat->CalcNeshVectHero(cnCalc,100);
		atr->_cnCalc += cnCalc;
	}
	atr->_stop = false;
	return 0;
}

void	clHistCalcS::CalcNesh(int cnMaxSec, double minDist, int cnThread)
{
//	cnThread = 1;
	time_t t0 = time(NULL);
	double arr[16];
	clTreeStrat *tree = &_tree;
	for (int i = 0; i < tree->CnBranch(); i++)
		arr[i] = tree->Branch(i)->_weight;
	stThreadNeshAtr atr[CN_THREAD];
	if (cnThread>CN_THREAD) cnThread = CN_THREAD;
	for (int i = 0; i < cnThread; i++)
	{
		atr[i]._strat = this;
		atr[i]._stop = false;
		atr[i]._cnCalc = 0;
//		atr[i]._rand0 = i;
		atr[i]._rand0 = time(NULL) + i;
		CreateThread(NULL, 0, &CalcThreadNesh, &atr[i], 0, NULL);
	}
	while (time(NULL) - t0 < cnMaxSec)
	//while (atr[0]._cnCalc<100000)
	{
		Sleep(500);
		_tree.CalcWeightStat();
		double dist = 0;
		for (int i = 0; i < tree->CnBranch(); i++)
		{
			double d0=arr[i] - tree->Branch(i)->_weight;
			dist += d0*d0;
			arr[i] = tree->Branch(i)->_weight;
		}
		if (sqrt(dist) < minDist)
			break;
	}
	for (int i = 0; i < cnThread; i++)
		atr[i]._stop = true;
	while (true)
	{
		Sleep(30);
		int i = 0;
		for (; i < cnThread; i++)
			if (atr[i]._stop)
				break;
		if (i == cnThread)
			break;
	}
	int cnCalc = 0;
	for (int i = 0; i < cnThread; i++)
		cnCalc+=atr[i]._cnCalc;
	clAnsiString as = (clAnsiString)"time=" + (clAnsiString)(time(NULL) - t0) + (clAnsiString)"сек; cnCalc=" + (clAnsiString)cnCalc;
	//glTrasser.WriteMessage(as);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clHistCalcS::Decide(clSitHoldem &sit, tpArrResBranch8 &arr)
{
	tpDis dis=GetDis(sit,&arr);

	int k = 0;
	double sum = 0;
	for (int i = 0; i < arr._cn; i++)
	{
		if (arr._weight[i] > 0.03)
		{
			sum += arr._weight[i];
			arr._dis[k] = arr._dis[i];
			arr._ev[k] = arr._ev[i];
			arr._weight[k] = arr._weight[i];
			k++;
		}
	}
	arr._cn = k;
	if (sum > DOUBLE_0)
		for (int i = 0; i < k; i++)
			arr._weight[i] /= sum;
	return dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
stSPatr &stSPatr::operator = (stSPatr &val)
{
	_stackInBB=val._stackInBB;
	_tree=val._tree;
	_file=val._file;
	_indFlop = val._indFlop;
	_indTurn = val._indTurn;
	_indRiver=val._indRiver;
	return *this;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int clStrategyPlay::FindVect(clAnsiString &name, vector <stVectorName> &vect)
{
	for (int i = 0; i < vect.size(); i++)
		if (name == vect[i]._name)
			return i;
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clStrategyPlay::~clStrategyPlay()
{
	Clear();
	if (_indRiver != NULL) free(_indRiver);
	for (int i = _calcAtr.size() - 1; i >= 0; i--)
	{
		_calcAtr[i]->_histCalc._indRiver = NULL;
		delete _calcAtr[i];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyPlay::Clear()
{
	for (int i = 0; i < _vectPtr.size(); i++)
		delete _vectPtr[i];
	_vectPtr.clear();
	_vect.clear();
	_vectFlop.clear();
	_vectTurn.clear();
	_vectRiver.clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyPlay::ClearCalcAtr()
{
	for (int i = _calcAtr.size() - 1; i >= 0; i--)
	{
		_calcAtr[i]->_histCalc._indRiver = NULL;
		delete _calcAtr[i];
	}
	_calcAtr.clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlay::Load(char *str)
{
	Clear();
	clAnsiString dir = str;
	int len = dir.Length();
	for (; len > 0; len--)
		if (dir[len] == '_')
			break;
	dir.SetLength(len+1);
	int cnSt = 100;
	_vectFlop.reserve(cnSt);
	_vectTurn.reserve(cnSt);
	_vectRiver.reserve(cnSt);
	for (int i = 1; i <= cnSt; i++)
	{
		clAnsiString ss = dir + (clAnsiString)i + (clAnsiString)".aaa";
		if (!_access(ss.c_str(), 0))
		{
			int nbA = _vectPtr.size();
			_vectPtr.push_back(0);
			_vectPtr[nbA] = new stSPatr;
			stSPatr *atr = _vectPtr[nbA];
			atr->_stackInBB = i;
			atr->_file.OpenFileA(ss.c_str());
			ss += ".bin";
			int handleS = _open(ss.c_str(), O_RDWR | O_BINARY);
			if (handleS == -1) return false;
			clAnsiString name, as;

			name.ReadFile(handleS);
			if (name.Length() == 1)
				atr->_indFlop = NULL;
			else
			{
				int nb = FindVect(name, _vectFlop);
				if (nb == -1)
				{
					nb = _vectFlop.size();
					stVectorName  vn;
					_vectFlop.push_back(vn);
					_vectFlop[nb]._name = name;
					as = (clAnsiString)PATH_CLASTERS + name;
					int handle = _open(as.c_str(), O_RDONLY | O_BINARY);
					if (handle == -1)
					{
						ErrMessage("Не открывается файл ", as);
						return false;
					}
					ReadVectFile<int>(handle, _vectFlop[nb]._vectInt);
					_close(handle);
				}
				atr->_indFlop = &_vectFlop[nb]._vectInt;
			}

			name.ReadFile(handleS);
			if (name.Length() == 1)
				atr->_indTurn = NULL;
			else
			{
				int nb = FindVect(name, _vectTurn);
				if (nb == -1)
				{
					nb = _vectTurn.size();
					stVectorName  vn;
					_vectTurn.push_back(vn);
					_vectTurn[nb]._name = name;
					as = (clAnsiString)PATH_CLASTERS + name;
					int handle = _open(as.c_str(), O_RDONLY | O_BINARY);
					if (handle == -1)
					{
						ErrMessage("Не открывается файл ", as);
						return false;
					}
					ReadVectFile<int>(handle, _vectTurn[nb]._vectInt);
					_close(handle);
				}
				atr->_indTurn = &_vectTurn[nb]._vectInt;
			}

			name.ReadFile(handleS);
			//if (name.Length() == 1)
				atr->_indRiver = NULL;
			/*else
			{
				int nb = FindVect(name, _vectRiver);
				if (nb == -1)
				{
					nb = _vectRiver.size();
					stVectorName  vn;
					_vectRiver.push_back(vn);
					_vectRiver[nb]._name = name;
					as = (clAnsiString)PATH_CLASTERS + name;
					int handle = _open(as.c_str(), O_RDONLY | O_BINARY);
					if (handle == -1)
					{
						ErrMessage("Не открывается файл ", as);
						return false;
					}
					ReadVectFile<int>(handle, _vectRiver[nb]._vectInt);
					_close(handle);
				}
				atr->_indRiver = &_vectRiver[nb]._vectInt;
			}*/
			atr->_tree.ReadFile(handleS);
			_close(handleS);
		}
	}

	_vect.clear();
	_vect.reserve(100);
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*bool	clStrategyPlay::LoadFromFile(char *str)
{
	CString ss = str;
	_file.OpenFile(ss.GetBuffer());
	ss += ".bin";
	int handleS = _open(ss.GetBuffer(), O_RDWR | O_BINARY);
	if (handleS == -1) return false;
	clAnsiString name, as;
	name.ReadFile(handleS);
	if (name.Length() == 1)
		_indFlop.clear();
	else
	{
		as = (clAnsiString)PATH_CLASTERS + name;
		int handle = _open(as.c_str(), O_RDONLY | O_BINARY);
		if (handle == -1)
		{
			ErrMessage("Не открывается файл ", as);
			return false;
		}
		ReadVectFile<int>(handle, _indFlop);
		_close(handle);
	}

	name.ReadFile(handleS);
	as = (clAnsiString)PATH_CLASTERS + name;
	int handle = _open(as.c_str(), O_RDONLY | O_BINARY);
	if (handle == -1)
	{
		ErrMessage("Не открывается файл ", as);
		return false;
	}
	ReadVectFile<int>(handle, _indTurn);
	_close(handle);

	name.ReadFile(handleS);
	if (name.Length() == 1)
		_indRiver.clear();
	else
	{
		as = (clAnsiString)PATH_CLASTERS + name;
		handle = _open(as.c_str(), O_RDONLY | O_BINARY);
		if (handle == -1)
		{
			ErrMessage("Не открывается файл ", as);
			return false;
		}
		ReadVectFile<int>(handle, _indRiver);
		_close(handle);
	}

	_tree.ReadFile(handleS);
	_close(handleS);
	_vect.clear();
	_vect.reserve(100);
	//	_calcTurn.resize(CN_NABOR_HAND_TURN);
	//	_calcRiver.resize(CN_NABOR_HAND_RIVER);
	return true;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clAtrStratHoldem *clStrategyPlay::LockNode(int nbfile, int nbRec, enNameTrade trade)
{
	DWORD val = WaitForSingleObject(_lock, INFINITE);
	if (val == WAIT_OBJECT_0)
	{
		int dim = _vect.size();
		for (int i = 0; i < dim; i++)
			if (_vect[i]._nbFile == nbfile && _vect[i]._nbRec0 == nbRec)
			{
				_vect[i]._cnLoc++;
				clAtrStratHoldem *res= _vect[i]._ptr;
				ReleaseSemaphore(_lock, 1, NULL);
				return res;
			}
		tpArgVect arg; arg._ptr = new clAtrStratHoldem;
		_vectPtr[nbfile]->_file.ReadRecord(nbRec, arg._ptr);
		arg._cnLoc = 1;
		arg._nbFile = nbfile;
		arg._nbRec0 = nbRec;
		if (trade == TRADE_FLOP)
			arg._ptr->_ptrIndex = _vectPtr[nbfile]->_indFlop;
		else if (trade == TRADE_TURN)
			arg._ptr->_ptrIndex = _vectPtr[nbfile]->_indTurn;
		else if (trade == TRADE_RIVER)
			arg._ptr->_ptrIndex = _vectPtr[nbfile]->_indRiver;
		_vect.push_back(arg);
		clAtrStratHoldem *res = _vect[dim]._ptr;
		ReleaseSemaphore(_lock, 1, NULL);
		return res;
	}
	ErrMessage("atr=", "NULL");
	return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyPlay::UnLockNode(int nbfile, int nbRec, bool save)
{
	DWORD val = WaitForSingleObject(_lock, INFINITE);
	if (val == WAIT_OBJECT_0)
	{
		int dim = _vect.size();
		for (int i = 0; i < dim; i++)
			if (_vect[i]._nbFile == nbfile && _vect[i]._nbRec0 == nbRec)
			{
				_vect[i]._cnLoc--;
				if (_vect[i]._cnLoc == 0)
				{
					delete _vect[i]._ptr;
					_vect.erase(_vect.begin() + i);
				}
			}
		ReleaseSemaphore(_lock, 1, NULL);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlay::CreateHistCalc(clHistCalcS &histC, clSitHoldem &sit)
{
	int nbA = sit.NbFirstAct(ACT_DELTURN);
	clSitHoldem sit1;
	sit1.ExecHist(sit.PtrHistory(), nbA + 1);
	if (!GetGroupHandsIn(histC, sit1))
	{
		histC._handsIn[0]._vect.clear();
		return false;
	}
	histC._indTurn = (int *)realloc(histC._indTurn, CN_NABOR_HAND_TURN*sizeof(int));
	histC._indRiver = _indRiver;
	histC.BuildTree(sit1, sit);
	time_t t0 = GetTickCount();
//	InitDataTree(histC);
	t0 = GetTickCount() - t0;
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*bool Compare(int cnB,clAtrTreeStrat *atr, clAtrTreeStrat *atr1)
{
	if (atr->CnClasters() != atr1->CnClasters())
		return false;
	int cn = atr->CnClasters();
	for (int i = 0; i < cn; i++)
	{
		clCalcParam cls = atr->CalcParam(i);
		clCalcParam cls1 = atr1->CalcParam(i);
		tpFloat w[12], *w1;
		cls.GetWeight(cnB, w);
		w1=cls1.GetWeightForCalc(cnB);
		for (int j = 0; j < cnB; j++)
			if (fabs(w[j] - w1[j]) > DOUBLE_0)
				return false;
	}
	return true;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategyPlay::DecideRemote(clSitHoldem &sit0, tpArrResBranch8 &arr, int sec)
{
/*	tpSaveSit param;
	static clFileConstRec fileSit;
	static bool bFirst = true;
	if (bFirst)
	{
		if (!fileSit.OpenFile("c:\\home\\data\\fileSaveSit.sit"))
			fileSit.Create("c:\\home\\data\\fileSaveSit.sit",sizeof(tpSaveSit));
		bFirst = false;
	}
	param._sit = sit0;
	param._sec = sec;
	param._time = time(NULL);
	fileSit.AddRecord(&param);*/


	clSitHoldem sit = sit0;
	double k = 1 / sit.ChangeBB(100);
	tpDis dis = HistCreateDis(ACT_UNDEF);

	/*dis = Decide(sit, arr);
	dis._money = dis._money*k + 0.5;
	arr.MultMoney(k);
	return dis;*/

	int i = 0;
	for (; i < _calcAtr.size(); i++)
		if (sit.NbGame() == _calcAtr[i]->_nbHand)
			break;
	if (i >= _calcAtr.size()) // не нашли
	{
		if (sit.NameTrade() < TRADE_TURN)
			dis = Decide(sit, arr);
		else
		{
			tpsitCalcAtr *atr = new tpsitCalcAtr;
			atr->_nbHand = sit.NbGame();
			atr->_curAct = sit.CurAct();
			if (!CreateHistCalc(atr->_histCalc, sit))
			{
				arr._cn = 0;
				dis._act = ACT_UNDEF;
				delete atr;
				return dis;
			}
			dis = DecideRemote(sit, atr->_histCalc, arr);
			time_t t0 = time(NULL);
			atr->_time = t0 + sec;
			_calcAtr.push_back(atr);
		}
	}
	else
	{
		if (sit.NameTrade() < TRADE_TURN)
		{
			DeleteCalcAtr(i);
			dis = Decide(sit, arr);
		}
		else
		{
			if (!_calcAtr[i]->_histCalc.RestraightTree(sit))
			{
				_calcAtr[i]->_histCalc.RestraightTree(sit);
				arr._cn = 0;
				return HistCreateDis(ACT_UNDEF);
			}

			dis = DecideRemote(sit, _calcAtr[i]->_histCalc, arr);
			time_t t0 = time(NULL);
			_calcAtr[i]->_time = t0 + sec;
		}
	}
	ClearOldHistCalc();
	dis._money = dis._money*k + 0.5;
	arr.MultMoney(k);
	return dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategyPlay::FindDisForHistCalc(clSitHoldem &sit0, tpArrResBranch8 &arr)
{
	clSitHoldem sit = sit0;
	double k = 1 / sit.ChangeBB(100);
	tpDis dis = HistCreateDis(ACT_UNDEF);
	int i = 0;
	for (; i < _calcAtr.size(); i++)
		if (sit.NbGame() == _calcAtr[i]->_nbHand)
			break;
	if (i >= _calcAtr.size()) // не нашли
		return dis;
	dis = _calcAtr[i]->_histCalc.Decide(sit, arr);
	dis._money = dis._money*k + 0.5;
	arr.MultMoney(k);
	return dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyPlay::ClearOldHistCalc()
{
	time_t t0 = time(NULL);
	for (int i = _calcAtr.size() - 1; i >= 0; i--)
		if (_calcAtr[i]->_time < t0)
			DeleteCalcAtr(i);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
struct stDisWeight
{
	tpDis _dis;
	double _weight;
	int _cnCalc;
};

int		CutAndNorm(int cn, stDisWeight *arr)
{
	int k = 0;
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
				if (arr[i]._dis== arr[j]._dis)
					break;
		if (j == k)
			arr[k++] = arr[i];
		else
			arr[j]._weight += arr[i]._weight;
	}

	double sum = 0;
	cn = k; k = 0;
	for (int i = 0; i <cn; i++)
		if (arr[i]._weight > 0.03)
		{
			sum += arr[i]._weight;
			arr[k++] = arr[i];
		}
	if (sum > DOUBLE_0)
		for (int i = 0; i < k; i++)
			arr[i]._weight /= sum;
	return k;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategyPlay::Decide(clSitHoldem &sit, tpArrResBranch8 &arrR)
{
	int nb1, nb2;
	double w1, w2;
	double st = (double)sit.EffStackSit() / sit.BigBlind();
	if (st < 5.0 && sit.NameTrade() == TRADE_PREFLOP)
	{
		glTrasser.WriteMessage((clAnsiString)"Решения для маленького стэка");
		tpDis dis = GetDisPushFold(sit);
		return dis;
	}
	glTrasser.WriteMessage((clAnsiString)"Решения для обычного стэка");
	if (!FindNearestStacks(sit, nb1, nb2, w1, w2))
		return HistCreateDis(ACT_UNDEF);
	glTrasser.WriteMessage((clAnsiString)"FindNearestStacks = true ");
	vector <stNodeW0> vect;
	_vectPtr[nb1]->_tree.FindSitNode(&sit, vect);
	if (w2 > 0.2)
	{
		int cn1 = vect.size();
		_vectPtr[nb2]->_tree.FindSitNode(&sit, vect);
		for (int i = 0; i < cn1; i++)
		{
			vect[i]._weight *= w1;
			vect[i]._nbTree = nb1;
		}
		for (int i = cn1; i < vect.size(); i++)
		{
			vect[i]._weight *= w2;
			vect[i]._nbTree = nb2;
		}
	}
	else
		for (int i = 0; i < vect.size(); i++)
			vect[i]._nbTree = nb1;
	if (vect.size()==0)
	{
//		ErrMessage("Думайте сами", "");
		return HistCreateDis(ACT_UNDEF);
	}
	enNameTrade trade = sit.NameTrade();
	stDisWeight arr[100];
	int cn = 0;
	for (int i = 0; i < vect.size(); i++)
	{
		clTreeOnFile *node = (clTreeOnFile *)vect[i]._ptr;
		clAtrStratHoldem *atr = LockNode(vect[i]._nbTree,node->AtrTree()->_nbRec, trade);
		tpFloat0 *weight = atr->GetStratWeight(node->CnBranch(), sit);
		for (int j = 0; j < node->CnBranch(); j++)
		{
			if (cn == 100)
				break;
			arr[cn]._dis = node->Branch(j)->GetDis();
			arr[cn]._weight = weight[j] * vect[i]._weight;
			cn++;
		}
		UnLockNode(vect[i]._nbTree, node->AtrTree()->_nbRec);
	}
	cn = CutAndNorm(cn, arr);
	int nbB = cn - 1;
	tpFloat0 val = (tpFloat0)rand() / RAND_MAX;
	for (int i = 0; i < cn; i++)
		if (arr[i]._weight > val)
		{
			nbB = i;
			break;
		}
		else
			val -= arr[i]._weight;
	tpDis dis = sit.CreateDis(arr[nbB]._dis);
	arrR._cn = (cn > MAX_CN_BRANCH) ? MAX_CN_BRANCH : cn;
	arrR._cnCalc = 100000;
	for (int i = 0; i < arrR._cn; i++)
	{
		for (int j = i + 1; j < cn;j++)
			if (arr[i]._weight < arr[j]._weight)
			{
				arrR._dis[i] = arr[j]._dis;
				arr[j]._dis = arr[i]._dis;
				arr[i]._dis = arrR._dis[i];
				arrR._weight[i] = arr[j]._weight;
				arr[j]._weight = arr[i]._weight;
				arr[i]._weight = arrR._weight[i];
			}
		arrR._dis[i] = arr[i]._dis;
		arrR._weight[i] = arr[i]._weight;
	}
	return dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategyPlay::DecideRemote(clSitHoldem &sit, clHistCalcS &histC, tpArrResBranch8 &arr)
{
	histC.CalcNesh(5, 0.01, CN_THREAD);
	tpDis dis = histC.Decide(sit, arr);
	return (arr._cnCalc < 50) ? HistCreateDis(ACT_UNDEF) : dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategyPlay::Decide(clSitHoldem &sit0, clHistCalcS &histC, tpArrResBranch8 &arr)
{
	clSitHoldem sit = sit0;
	double k = 1/sit.ChangeBB(100);
	if (sit.NameTrade() < TRADE_TURN)
	{
		histC.Clear();
		tpDis dis = Decide(sit,arr);
		dis._money = dis._money*k+0.5;
		arr.MultMoney(k);
		return dis;
	}
	if (histC.IsEmpty())
	{
		int nbA = sit.NbFirstAct(ACT_DELTURN);
		clSitHoldem sit1;
		sit1.ExecHist(sit.PtrHistory(), nbA+1);
		GetGroupHandsIn(histC, sit1);
		histC.BuildTree(sit1, sit);
	}
	else
	{ 
		if (!histC.RestraightTree(sit))
		{
			histC._handsIn[0]._vect.clear();
			arr._cn = 0;
			return HistCreateDis(ACT_UNDEF);
		}
	}
	histC.CalcNesh(5,0.01,8);
	tpDis dis = histC.Decide(sit, arr);
	dis._money = dis._money*k + 0.5;
	arr.MultMoney(k);
	return (arr._cnCalc<50)? HistCreateDis(ACT_UNDEF) :  dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpDis	clStrategyPlay::DecideTree(clSitHoldem &sit0, tpArrResBranch8 &arr)
{
	clSitHoldem sit = sit0;
	double k = 1 / sit.ChangeBB(100);
	tpDis dis = Decide(sit, arr);
	dis._money = dis._money*k + 0.5;
	arr.MultMoney(k);
	return dis;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlay::InitDataTree(clHistCalcS &histC)
{
	clSitHoldem &sit = histC._tree.GetSit();
	int nb1, nb2;
	double w1, w2;
	if (!FindNearestStacks(sit, nb1, nb2, w1, w2))
		return false;

	enNameTrade trade = sit.NameTrade();
	if (trade != TRADE_TURN) return false; 
	clDeck deck; deck.New();
	tpCard cards[5];
	for (int i = 0; i < sit.CnCardTable(); i++)
	{
		cards[i] = sit.CardsTable()[i];
		deck.Skip(cards[i]);
	}

	size_t size = histC._tree.AtrTree()->CnClasters()*sizeof(int);
	int *ptrTurn = (int *)malloc(size);
	vector <int> &vectTurn = *(_vectPtr[nb1]->_indTurn);
	memset(ptrTurn, -1, size);
	for (int i = 0; i < deck.CnCards(); i++)
		for (int j = i + 1; j < deck.CnCards(); j++)
		{
			tpHand hand; hand.SetHand(deck[j], deck[i]);
			int nbT = GetTurnHandIndex(cards, hand);
			int nb0 = histC._indTurn[nbT];
			ptrTurn[nb0] = vectTurn[nbT];
		}

	clTreeStrat *node = histC._tree.FindBranch(TRADE_RIVER);
	size = node->AtrTree()->CnClasters()*sizeof(int);
	int *ptrRiver = (int *)malloc(size);
	vector <int> &vectRiver = *(_vectPtr[nb1]->_indRiver);
	memset(ptrRiver, -1, size);
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
						int nb0 = histC._indRiver[nbT];
						ptrRiver[nb0] = vectRiver[nbT];
					}
	}

	InitDataTree(&histC._tree, nb1, ptrTurn, ptrRiver);
	free(ptrTurn);
	free(ptrRiver);
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyPlay::DeleteCalcAtr(int nb)
{
	_calcAtr[nb]->_histCalc._indRiver = NULL;
	delete _calcAtr[nb];
	_calcAtr.erase(_calcAtr.begin() + nb);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyPlay::InitDataTree(clTreeStrat *tree, int nbTreeonFile, int *ptrTurn, int *ptrRiver)
{
	clSitHoldem &sit = tree->GetSit();
	if (tree->CnBranch() == 0) return;
	if (tree->CnBranch() == 1)
	{
		InitDataTree(tree->Branch(0), nbTreeonFile, ptrTurn, ptrRiver);
		return;
	}

	vector <stNodeW0> vect;
	enNameTrade trade = sit.NameTrade();
	clTreeOnFile *node = _vectPtr[nbTreeonFile]->_tree.FindSitNode(&sit);
	if (node == NULL || node->CnBranch()<=1) return;
	clAtrStratHoldem *atrF = LockNode(nbTreeonFile, node->AtrTree()->_nbRec, trade);
	clAtrTreeStrat *atr = tree->AtrTree();
	size_t dim = atr->CnClasters();
	int *ptr = (trade == TRADE_TURN) ? ptrTurn : ptrRiver;
	tpFloat0 arrK[MAX_CN_BRANCH][MAX_CN_BRANCH], w[MAX_CN_BRANCH];
	ZeroMemory(arrK, MAX_CN_BRANCH*MAX_CN_BRANCH*sizeof(tpFloat0));
	int nb1, nb2;
	double w1, w2;
	for (int i = 0; i < tree->CnBranch(); i++)
	{
		node->FindBranchBetween(tree->Branch(i)->GetDis(), nb1, nb2, w1, w2);
		if (nb1 != -1) arrK[i][nb1] = w1;
		if (nb2 != -1) arrK[i][nb2] = w2;
	}
	for (size_t i = 0; i < dim; i++)
	{
		int nb = ptr[i];
		tpFloat0 *ptrFloat = &atrF->_param[nb*node->CnBranch()];
		for (int j = 0; j < tree->CnBranch(); j++)
		{
			w[j] = 0;
			for (int k = 0; k < node->CnBranch(); k++)
				w[k] += ptrFloat[k] * arrK[j][k];
		}
		atr->CalcParam(i).SetParam(tree->CnBranch(),w, 10);
	}
	UnLockNode(nbTreeonFile, node->AtrTree()->_nbRec, false);

	for (int i = 0; i < tree->CnBranch(); i++)
		InitDataTree(tree->Branch(i), nbTreeonFile, ptrTurn, ptrRiver);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlay::GetGroupHandsIn(clHistCalcS &histC, clSitHoldem &sit0)
{
	histC._tree.GetSit() = sit0;
	for (int i = 0; i < sit0.CnPlayer(); i++)
		if (sit0.PlayerIsCard(i))
			histC._handsIn[i].FullPreFlopAllHands();
	clSitHoldem sit;
	sit = sit0;
	int ind = sit.Indicator();
	int nbA = sit.CurNbAct();
	sit.FirstSit();
	int nb1, nb2;
	double w1, w2;
	if(!FindNearestStacks(sit,nb1,nb2,w1,w2))
		return false;
	if (!GetGH(nb1, &_vectPtr[nb1]->_tree, sit, ind, nbA, histC._handsIn))
		return false;
	if (w2 > 0.2)
	{
		clHandsGroupEx gh[CN_PLAYER];
		for (int i = 0; i < sit0.CnPlayer(); i++)
			gh[i].FullPreFlopAllHands();
		sit = sit0;
		sit.FirstSit();
		if (!GetGH(nb2, &_vectPtr[nb2]->_tree, sit, ind, nbA, gh))
			return false;
		for (int i = 0; i < sit0.CnPlayer(); i++)
			for (int j = 0; j < ALL_CN_HAND; j++)
				histC._handsIn[i][j] = histC._handsIn[i][j] * w1 + gh[i][j]*w2;
	}
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlay::GetGH(int nbTree, clTreeOnFile *node, clSitHoldem &sit, int ind, int nbA, clHandsGroupEx  *handsIn)
{
	//if (GetSit().Indicator() == ind && sit.CurNbAct() == nbA)
	if (sit.CurNbAct() == nbA)
		return true;

	tpDis dis = sit.GetActIndicator()->_dis;
	int nbH = sit.Indicator();
	if (nbH == -1)
	{
		for (int i1 = 0; i1 < sit.CnPlayer(); i1++)
			if (handsIn[i1]._vect.size() == ALL_CN_HAND)
			{
				switch (dis._act)
				{
					case ACT_DELFLOP:
						handsIn[i1].SkipPreFlopAllHands(sit.CardsTable()[0]);
						handsIn[i1].SkipPreFlopAllHands(sit.CardsTable()[1]);
						handsIn[i1].SkipPreFlopAllHands(sit.CardsTable()[2]);
						break;
					case ACT_DELTURN: handsIn[i1].SkipPreFlopAllHands(sit.CardsTable()[3]);	break;
					case ACT_DELRIVER: handsIn[i1].SkipPreFlopAllHands(sit.CardsTable()[4]);	break;
				}
				handsIn[i1].Norm();
			}
		sit.ExecDis(dis);
		return GetGH(nbTree,node->Branch(0), sit, ind, nbA, handsIn);
	}
	else
	{
		int nb1 = -1, nb2 = -1;
		double w1, w2;
		node->FindBranchBetween(dis, nb1, nb2, w1, w2);
		if (nb1 == -1)
		{
#ifdef COMP1
			ErrMessage("GetGH", " FindBranchBetween");
#endif
			return false;
		}
		if (nb2 != -1 && w2>0.2)
		{
			clHandsGroupEx gh[CN_PLAYER];
			for (int i = 0; i < sit.CnPlayer(); i++)
				gh[i] = handsIn[i];
			if (handsIn[nbH]._vect.size() == ALL_CN_HAND)
				ChangeGHAll(nbTree, node, sit, nb2, gh[nbH]);
			clSitHoldem sit2 = sit;
			sit2.ExecDis(node->Branch(nb2)->GetDis());
			if (!GetGH(nbTree, node->Branch(nb2), sit2, ind, nbA, gh)) return false;
			if (handsIn[nbH]._vect.size() == ALL_CN_HAND)
				ChangeGHAll(nbTree, node, sit, nb1, handsIn[nbH]);
			sit.ExecDis(node->Branch(nb1)->GetDis());
			if (!GetGH(nbTree, node->Branch(nb1), sit, ind, nbA, handsIn)) return false;
			for (int i = 0; i < sit.CnPlayer(); i++)
				if (handsIn[nbH]._vect.size() == ALL_CN_HAND)
					for (int j = 0; j < ALL_CN_HAND; j++)
						handsIn[i][j] = handsIn[i][j] * w1 + gh[i][j] * w2;
			return true;
		}
		else
		{
			if (handsIn[nbH]._vect.size() == ALL_CN_HAND)
				ChangeGHAll(nbTree, node, sit, nb1, handsIn[nbH]);
			sit.ExecDis(node->Branch(nb1)->GetDis());
			return GetGH(nbTree, node->Branch(nb1), sit, ind, nbA, handsIn);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clStrategyPlay::ChangeGHAll(int nbTree, clTreeOnFile *node, clSitHoldem &sit, int nbB, clHandsGroupEx &ghH)
{
//	int ind = Indicator();
//	if (ind == -1 || CnBranch() == 1) return;
	tpCard cards[5];
	for (int k = 0; k < sit.CnCardTable(); k++)
		cards[k] = sit.CardsTable()[k];
	enNameTrade trade = sit.NameTrade();
	clAtrStratHoldem *atr = LockNode(nbTree, node->AtrTree()->_nbRec, trade);
	if (trade == TRADE_PREFLOP)
	{
		for (int i = 0; i < CN_CARD_COL; i++)
			for (int j = i + 1; j < CN_CARD_COL; j++)
			{
				int nbH = j*(j - 1) / 2 + i;
				if (ghH[nbH] > DOUBLE_0)
				{
					tpHand hand = { j, i };
					int nbI = GetHandIndex(hand);
					tpFloat0 *w = atr->GetStratWeight(nbI);
					ghH[nbH] *= w[nbB];
				}
			}
	}
	else if (trade == TRADE_FLOP)
	{
		for (int i = 0; i < CN_CARD_COL; i++)
			for (int j = i + 1; j < CN_CARD_COL; j++)
			{
				int nbH = j*(j - 1) / 2 + i;
				if (ghH[nbH] > DOUBLE_0)
				{
					tpHand hand = { j, i };
					int nbI = GetFlopHandIndex(cards, hand);
					if (atr->_ptrIndex!=NULL)
						nbI = atr->_ptrIndex[0][nbI];
					tpFloat0 *w = atr->GetStratWeight(nbI);
					ghH[nbH] *= w[nbB];
				}
			}
	}
	else if (trade == TRADE_TURN)
	{
		for (int i = 0; i < CN_CARD_COL; i++)
			for (int j = i + 1; j < CN_CARD_COL; j++)
			{
				int nbH = j*(j - 1) / 2 + i;
				if (ghH[nbH] > DOUBLE_0)
				{
					tpHand hand = { j, i };
					int nbI = GetTurnHandIndex(cards, hand);
					nbI = atr->_ptrIndex[0][nbI];
					tpFloat0 *w = atr->GetStratWeight(nbI);
					ghH[nbH] *= w[nbB];
				}
			}
	}
	else if (trade == TRADE_RIVER)
	{
		for (int i = 0; i < CN_CARD_COL; i++)
			for (int j = i + 1; j < CN_CARD_COL; j++)
			{
				int nbH = j*(j - 1) / 2 + i,nbT;
				if (ghH[nbH] > DOUBLE_0)
				{
					tpHand hand = { j, i };
					int nbI = GetTurnAndRiverHandIndex(cards, hand,nbT);
					nbI = atr->_ptrIndex[0][nbI];
					tpFloat0 *w = atr->GetStratWeight(nbI);
					ghH[nbH] *= w[nbB];
				}
			}
	}
	UnLockNode(nbTree, node->AtrTree()->_nbRec);
	ghH.Norm();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*int		clStrategyPlay::FindNearestStack(clSitHoldem &sit)
{
	double st = (double)sit.EffStackSit() / sit.BigBlind();
	double min = fabs(_vectPtr[0]->_stackInBB - st);
	int nbMin = 0;
	for (int i = 1; i < _vectPtr.size(); i++)
	{
		double d0 = fabs(_vectPtr[i]->_stackInBB - st);
		if (d0 < min)
		{
			min = d0;
			nbMin = i;
		}
	}
	return nbMin;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clStrategyPlay::FindNearestStacks(clSitHoldem &sit, int &nb1, int &nb2, double &w1, double &w2)
{
	double st = (double)sit.EffStackSit() / sit.BigBlind();
	if (st < 3)
		return false;
	
	int nbB = -1, nbL = -1;
	for (int i = 0; i < _vectPtr.size(); i++)
	{
		if (_vectPtr[i]->_stackInBB >= st)
		{
			if (nbB == -1)
				nbB = i;
			else if (_vectPtr[i]->_stackInBB < _vectPtr[nbB]->_stackInBB)
				nbB = i;
		}

		if (_vectPtr[i]->_stackInBB < st)
		{
			if (nbL == -1)
				nbL = i;
			else if (_vectPtr[i]->_stackInBB > _vectPtr[nbL]->_stackInBB)
				nbL = i;
		}
	}
	if (nbB == -1)
	{
		nb1 = nbL;
		w1 = 1;
		nb2 = -1;
		w2 = 0;
		return true;
	}
	if (nbL == -1)
	{
		nb1 = nbB;
		w1 = 1;
		nb2 = -1;
		w2 = 0;
		return true;
	}
	double dB = _vectPtr[nbB]->_stackInBB - st;
	double dL = st - _vectPtr[nbL]->_stackInBB;
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
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clHistCalcS *clStrategyPlay::GetHistCalc(clSitHoldem &sit)
{
	int dim = _calcAtr.size();
	for (int i = 0; i < dim; i++)
		if (sit.Include(_calcAtr[i]->_histCalc._tree.GetSit()))
			return &_calcAtr[i]->_histCalc;
	return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
