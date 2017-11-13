//#include "stdafx.h"
#include "C:\home\Holdem\Strategy\AppRiverTrees\Source\clRiverBanks.h"
#include "c:\Home\Util\Clasters.h"

double MAX_NEAR_DIST = 0.2;
clRiverBanks glBankRiver;

//----------------------------------------------------------------------------------------------------------------------------
int clRiverBanks::NearestRiverCls(clRiverDat &dat, tpFloat &dist)
{
	if (_fastTree.IsInit())
		return FastSearchR(dat, dist);
	dist = -1;
	int nb = -1;
	for (int i = 0; i < _arr.size(); i++)
	{
		tpFloat d1 = _arr[i].Distance(dat);
		if (d1 < dist || dist == -1)
		{
			dist = d1;
			nb = i;
		}
	}
	/*	if (_fastTree.IsInit())
		{
			int nbF = FastSearchR(dat, dist);
			if (nbF != nb)
			{
				ErrMessage("NearestRiverCls::", "Результат не совпадает с FastSearch");
				//NearestRiverCls(dat, dist);
			}
		}*/
	return nb;
}
//----------------------------------------------------------------------------------------------------------------------------
void clRiverBanks::Create(char *path, int cnRecord)
{
	clAnsiString asPath; asPath = path;
	_fileUnit.Create((asPath + ".rbM").c_str(), SIZE_MB_DEF, 0, cnRecord);
	_arr.clear();
	_sopen_s(&_fastTree._handle, (asPath + ".binT").c_str(), O_BINARY | O_RDWR | O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	_chsize(_fastTree._handle, 0);
}
//----------------------------------------------------------------------------------------------------------------------------
bool clRiverBanks::OpenA(char *path, bool onlyRead, int memGB)
{
	clAnsiString asPath; asPath = path;
	int val = asPath.Length() - 4;
	char c0 = asPath[val];
	if (c0 == '.')
		asPath.SetLength(asPath.Length() - 4);
	if (!_fileUnit.OpenFileA((asPath + ".rbM").c_str(), onlyRead, memGB))
		return false;
	_arr.clear();
	_arr.resize(_fileUnit.CnRecord());
	for (int i = 0; i < _fileUnit.CnRecord(); i++)
	{
		int handle = _fileUnit.clFileRecT::LockRecord(i);
		_arr[i].ReadFile(handle);
		_fileUnit.clFileRecT::UnLockRecord(i);
	}
	if (onlyRead)
		_sopen_s(&_fastTree._handle, (asPath + ".binT").c_str(), O_BINARY | O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	else
		_sopen_s(&_fastTree._handle, (asPath + ".binT").c_str(), O_BINARY | O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (_filelength(_fastTree._handle) != 0)
		_fastTree.LoadFromFile();
	SetRegimNeuro(false);
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
bool	RiverNetGetEv(clRiverDat &dat, vector<float> *ev, clLayersBPNet &net)
{
	vector<double> inData, outData;
	inData.resize(22);
	inData[1] = 1. / (1 + 2 * dat._stack);
	inData[0] = (dat._stack * inData[1]) * 2;
	if (dat._vectUnit[0].size() != RIVERBANK_CN_HANDS)
	{
		ErrMessage("Error0 ", "TrainingPair");
		return false;
	}
	for (int k = 0; k < RIVERBANK_CN_HANDS; k++)
	{
		inData[2 + k] = dat._vectUnit[0][k]._weight;
	}
	if (dat._vectUnit[1].size() != RIVERBANK_CN_HANDS)
	{
		ErrMessage("Error1 ", "TrainingPair");
		return false;
	}
	for (int k = 0; k < RIVERBANK_CN_HANDS; k++)
	{
		inData[RIVERBANK_CN_HANDS + 2 + k] = dat._vectUnit[1][k]._weight;
	}
	net.Propogate(inData, outData);
	ev[0].resize(ALL_CN_HAND);
	ev[1].resize(ALL_CN_HAND);
	ZeroMemory(&ev[0][0], ALL_CN_HAND * sizeof(float));
	ZeroMemory(&ev[1][0], ALL_CN_HAND * sizeof(float));
	int *ptr = GetNbRiverDatDat(0);
	for (int i = 0; i < RIVERBANK_CN_HANDS; i++)
		ev[0][ptr[i]] = outData[i] * 5000 - 2500;
	ptr = GetNbRiverDatDat(1);
	for (int i = 0; i < RIVERBANK_CN_HANDS; i++)
		ev[1][ptr[i]] = outData[i + RIVERBANK_CN_HANDS] * 5000 - 2500;
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
int	clRiverBanks::FillRiverEV(clRootRiverTree *root, int nbHero, vector <float> &ev)
{
	clRiverDat dat;
	root->CreateRiverDat(dat);
	vector <float> evN;
	bool cN = false;

	tpFloat dist, d0, d1, d2;
	if (_regNet)
	{
		clRiverDat dat10;
		clRootRiverTree r0 = *root;
		r0.CreateRiverDatMin(dat10, &dat);
		vector <float> ev0[2];
		if (RiverNetGetEv(dat10, ev0, _net))
		{
			RiverBankReCalcEv(root, nbHero, dat, dat10, ev, ev0[nbHero]);
			return root->_sit.PlMoney(0);

			RiverBankReCalcEv(root, nbHero, dat, dat10, evN, ev0[nbHero]);
			cN = true;
		}
		else
		{
			r0 = *root;
			r0.CreateRiverDatMin(dat10, &dat);
			RiverNetGetEv(dat10, ev0, _net);
		}
	}

	int nb = NearestRiverCls(dat, dist);
	clRiverBankUnit *unit = LockBankUnit(nb);
	int res = unit->_root._sit.PlMoney(0);
	RiverBankReCalcEv(root, nbHero, dat, unit->_dat, ev, unit->_ev[nbHero]);
	UnLockBankUnit(nb, unit);

	if (cN)
	{
		double k0 = 0.5;
		double k1 = 1 - k0;
		for (int i = 0; i < ALL_CN_HAND; i++)
			ev[i] = ev[i] * k0 + evN[i] * k1;
	}
	return res;
}
//----------------------------------------------------------------------------------------------------------------------------
void	RecalcNbForPlayer(vector <tpRiverGHUnit> &vectIn, vector <tpRiverGHUnit> &vect, stNbBtw &nbF, int nbHand, vector <int> &vint)
{
	int j = 0, dimI = (int)vect.size(), dimJ = (int)vectIn.size();
	for (; j < dimJ; j++)
		if (vectIn[j]._nbH == nbHand)
			break;
	if (j == dimJ)
	{
		ErrMessage("Нет такой руки", " Errror");
		return;
	}

	if (vectIn[j]._wf > vect[0]._wf)
	{
		nbF._nb0 = vect[0]._nbH;
		nbF._nb1 = vect[0]._nbH;
		nbF._w0 = 1;
		nbF._w1 = 0;
		return;
	}
	for (int i = 1; i < dimI; i++)
		if (vectIn[j]._wf > vect[i]._wf)
		{
			float r0 = vect[i - 1]._wf - vectIn[j]._wf;
			float r1 = vectIn[j]._wf - vect[i]._wf;
			nbF._nb0 = vect[i - 1]._nbH;
			nbF._nb1 = vect[i]._nbH;
			nbF._w0 = r1 / (r1 + r0);
			nbF._w1 = r0 / (r1 + r0);
			return;
		}
	nbF._nb0 = vect[dimI - 1]._nbH;
	nbF._nb1 = vect[dimI - 1]._nbH;
	nbF._w0 = 1;
	nbF._w1 = 0;

	int dim = (int)vint.size();
	for (int i = 0; i < dim; i++)
	{
		if (vint[i] == nbF._nb0)
			nbF._nb0 = i;
		if (vint[i] == nbF._nb1)
			nbF._nb1 = i;
	}
}
//----------------------------------------------------------------------------------------------------------------------------
/*void	clRiverBanks::GetAllDis(clRootRiverTree &root, int nbHero, int nbHand, vector <tpRefHero> &vectRef)
{
	clRiverDat dat;
	root.CreateRiverDat(dat);
	tpFloat dist;
	int nb = NearestRiverCls(dat, dist);
	clRiverBankCls &cls = _arr[nb];
	int nb0 = 0, dimI = cls._info.size();
	double stackIn = CurStackToPot(root._sit, 0);
	tpFloat k0 = 1, kA = 0.8;
	if (stackIn > cls._info[0]._stack)
	{
		for (nb0 = 1; nb0 < dimI; nb0++)
			if (stackIn < cls._info[nb0]._stack)
				break;
		if (nb0 < dimI)
		{
			k0 = 1 - (cls._info[nb0]._stack - stackIn) / (cls._info[nb0]._stack - cls._info[nb0 - 1]._stack);
			if (k0 < 1 - kA)
			{
				k0 = 1 - k0;
				nb0--;
			}
		}
		else
			nb0 = dimI - 1;
	}
	clRiverBankUnit *unit = LockBankUnit(cls._info[nb0]._nbRecord);
	tpRefHero ref;
	ref._stack = unit->_root._sit.PlMoney(0);
	ref._nbTree = cls._info[nb0]._nbRecord;
	RecalcNbForPlayer(dat._vectUnit[nbHero], cls._dat._vectUnit[nbHero], ref._refNb, nbHand, unit->_root._vectHand[nbHero]);
	UnLockBankUnit(cls._info[nb0]._nbRecord, unit);
	if (k0 > kA)
	{
		ref._weight = 1;
		vectRef.push_back(ref);
		return;
	}
	ref._weight = k0;
	vectRef.push_back(ref);

	nb0--;
	unit = LockBankUnit(cls._info[nb0]._nbRecord);
	ref._stack = unit->_root._sit.PlMoney(0);
	ref._nbTree = cls._info[nb0]._nbRecord;
	RecalcNbForPlayer(dat._vectUnit[nbHero], cls._dat._vectUnit[nbHero], ref._refNb, nbHand, unit->_root._vectHand[nbHero]);
	UnLockBankUnit(cls._info[nb0]._nbRecord, unit);
	ref._weight = 1 - k0;
	vectRef.push_back(ref);
}*/
//----------------------------------------------------------------------------------------------------------------------------
int	clRiverBanks::FillRiverEV(clRootRiverTree *root, vector <float> *ev, tpUnfindRiverTree *ptrUt)
{
	clRiverDat dat;
	root->CreateRiverDat(dat);
	tpFloat dist, dS;
	int nb = NearestRiverCls(dat, dist);

	if (ptrUt != NULL)
		if (dist > ptrUt->_dist)
		{
			clAnsiString as = (clAnsiString)"nbNode=" + (clAnsiString)glTime1 + (clAnsiString)"; nbCard=" + (clAnsiString)glTime2;
			as += (clAnsiString)" clRiverBanks::FillRiverEV dist=" + WSDoubleToAS(dist, 6);
			glTrasser.WriteMessage(as);
			if (ptrUt->_file.IsOpen())
			{
				clInComeDataRoot rec;
				rec = *((clInComeDataRoot *)root);
				ptrUt->_file.AddRecord(&rec);
			}
		}

	return FillRiverEV(nb, dat, root, ev);
}
//----------------------------------------------------------------------------------------------------------------------------
int	clRiverBanks::FillRiverEV(int nbCls, clRiverDat &dat, clRootRiverTree *root, vector <float> *ev)
{
	clRiverBankUnit *unit = LockBankUnit(nbCls);
	unit->_root.CreateRiverDat(unit->_dat);


	int res = unit->_root._sit.PlMoney(0);
	RiverBankReCalcEv(root, 0, dat, unit->_dat, ev[0], unit->_ev[0]);
	RiverBankReCalcEv(root, 1, dat, unit->_dat, ev[1], unit->_ev[1]);
	UnLockBankUnit(nbCls, unit);
	/*	if (w0 > kA)
	return res;
	vector <float> evT[2];
	unit = LockBankUnit(cls._info[nb1]._nbRecord);
	int res1 = unit->_root._sit.PlMoney(0);
	w1 *= double(res) / res1;
	RiverBankReCalcEv(root, 0, dat, cls._dat, evT[0], unit->_ev[0]);
	RiverBankReCalcEv(root, 1, dat, cls._dat, evT[1], unit->_ev[1]);
	UnLockBankUnit(cls._info[nb1]._nbRecord, unit);
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
	ev[0][i] = ev[0][i] * w0 + evT[0][i] * w1;
	ev[1][i] = ev[1][i] * w0 + evT[1][i] * w1;
	}*/
	return res;
}
//----------------------------------------------------------------------------------------------------------------------------
bool	clRiverBanks::GetTreeForRoot(clRoot &root, clHoldemTree &tree, tpUnfindRiverTree *ptrUT)
{
	clRiverDat dat;
	//root.CreateRiverDat(dat);
	CreateRiverDatRoot(root, dat);
	double dist;
	int nb = NearestRiverCls(dat, dist);

	if (ptrUT != NULL)
		if (dist > ptrUT->_dist)
		{
			clAnsiString as = "RiverTree";
			as += (clAnsiString)" dist=" + WSDoubleToAS(dist, 6);
			glTrasser.WriteMessage(as);
			if (ptrUT->_file.IsOpen())
			{
				clInComeDataRoot rec;
				rec = *((clInComeDataRoot *)&root);
				ptrUT->_file.AddRecord(&rec);
			}
		}
	//if (dist > 0.05)
		//return false;

	clRiverBankUnit *unit = LockBankUnit(nb);

	if (unit->_root._tree.GetSit().GetLastAct()->_dis._act != ACT_DELRIVER)
		ErrMessage("Error ", "River Bad sit");

	int res = unit->_root._sit.PlMoney(0);
	tpTableChangeNb tab[2], tabCls[2];
	RecalcNbGHForRiver(dat._vectUnit[0], unit->_dat._vectUnit[0], tab[0]);  //таблица содержит прямые номера хэндов
	RecalcNbGHForRiver(dat._vectUnit[1], unit->_dat._vectUnit[1], tab[1]);
	// надо переделать tab  в таблицу соответствия кластеров
	int revCls[ALL_CN_HAND], revClsBD[ALL_CN_HAND];
	int dim = unit->_root.CnHand(0);
	for (int i = 0; i < dim; i++)
		revClsBD[unit->_root.NbHand(0, i)] = i;
	dim = root.CnHand(0);
	for (int i = 0; i < dim; i++)
		revCls[root.NbHand(0, i)] = i;
	for (int i = 0; i < dim; i++)
	{
		int nbH = root.NbHand(0, i);
		stNbBtw ch = tab[0]._arr[nbH];
		int nbT = revCls[nbH];
		stNbBtw &chCls = tabCls[0]._arr[nbT];
		chCls = ch;
		chCls._nb0 = revClsBD[ch._nb0];
		chCls._nb1 = revClsBD[ch._nb1];
	}

	dim = unit->_root.CnHand(1);
	for (int i = 0; i < dim; i++)
		revClsBD[unit->_root.NbHand(1, i)] = i;
	dim = root.CnHand(1);
	for (int i = 0; i < dim; i++)
		revCls[root.NbHand(1, i)] = i;
	for (int i = 0; i < dim; i++)
	{
		int nbH = root.NbHand(1, i);
		stNbBtw ch = tab[1]._arr[nbH];
		int nbT = revCls[nbH];
		stNbBtw &chCls = tabCls[1]._arr[nbT];
		chCls = ch;
		chCls._nb0 = revClsBD[ch._nb0];
		chCls._nb1 = revClsBD[ch._nb1];
	}

	double k = (double)root._sit.EffStackSit() / unit->_root._sit.PlMoney(0);
	FillTreeFromTree(&tree, &unit->_root._tree, k, tabCls, 1);
	UnLockBankUnit(nb, unit);
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
/*void	ChangeTreeCls(clHoldemTree *tree, clHandsGroupEx *gh, tpTableChangeNb *tab, double kStack, double mult)
{
	if (tree->EndedNode())
		return;
	if (tree->CnBranch()==1)
		return ChangeTreeCls(tree->Branch(0), gh, tab, kStack, mult);
	vector <byte> vect;
	int sz = tree->AtrTree()->_sizeParam;
	vect.resize(ALL_CN_HAND*sz, 0);
	int ind = tree->Indicator(), cnB = tree->CnBranch();
	for (int i = 0; i < ALL_CN_HAND; i++)
		if (gh[ind][i] > DOUBLE_0)
		{
			clCalcParam cp = &vect[i*sz];
			stNbBtw ch = tab[ind]._arr[i];
			clCalcParam cls = tree->AtrTree()->CalcParam(ch._nb0);
			cp.AddCalcParam(cls, cnB, ch._w0*mult);
			cls = tree->AtrTree()->CalcParam(ch._nb1);
			cp.AddCalcParam(cls, cnB, ch._w1*mult);
		}
	tree->AtrTree()->_param = vect;
	for (int i = 0; i < tree->CnBranch(); i++)
		ChangeTreeCls(tree->Branch(i), gh, tab, kStack, mult);
}*/
//----------------------------------------------------------------------------------------------------------------------------
/*double	clRiverBanks::GetTreeForRoot(clRoot &root, tpUnfindRiverTree *ptr)
{
	clRiverDat dat;
	CreateRiverDatRoot(root, dat);
	double dist;
	int nb = NearestRiverCls(dat, dist);

	clRiverBankUnit *unit = LockBankUnit(nb);
	int res = unit->_root._sit.PlMoney(0);
	tpTableChangeNb tab[2];
	RecalcNbGHForRiver(dat._vectUnit[0], unit->_dat._vectUnit[0], tab[0]);  //таблица содержит прямые номера хэндов
	RecalcNbGHForRiver(dat._vectUnit[1], unit->_dat._vectUnit[1], tab[1]);
	// надо переделать tab в номера кластеров
	int revClsBD[ALL_CN_HAND];
	int dim = unit->_root.CnHand(0);
	for (int i = 0; i < dim; i++)
		revClsBD[unit->_root.NbHand(0, i)] = i;
	for (int i = 0; i < ALL_CN_HAND; i++)
		if (root._gh[0][i] > DOUBLE_0)
		{
			stNbBtw &ch = tab[0]._arr[i];
			ch._nb0 = revClsBD[ch._nb0];
			ch._nb1 = revClsBD[ch._nb1];
		}

	dim = unit->_root.CnHand(1);
	for (int i = 0; i < dim; i++)
		revClsBD[unit->_root.NbHand(1, i)] = i;
	for (int i = 0; i < ALL_CN_HAND; i++)
		if (root._gh[1][i] > DOUBLE_0)
		{
			stNbBtw &ch = tab[1]._arr[i];
			ch._nb0 = revClsBD[ch._nb0];
			ch._nb1 = revClsBD[ch._nb1];
		}

	double k = (double)root._sit.PlMoney(0) / unit->_root._sit.PlMoney(0);
	//root.CreateTreeNL1326();
	FillTreeFromTree(&root._tree, &unit->_root._tree, k, tab, 1);
	UnLockBankUnit(nb, unit);
	//ChangeTreeCls(&root._tree, root._gh, tab, k, 0.01);
	return dist;
}*/
//----------------------------------------------------------------------------------------------------------------------------
double	clRiverBanks::GetTreeForRoot(clRootRiverTree &root, clRootRiverTree &rBD, clHoldemTree &tree)
{
	clRiverDat dat, datBD;
	root.CreateRiverDat(dat);
	rBD.CreateRiverDat(datBD);
	double dist = dat.Distance(datBD);

	tpTableChangeNb tab[2], tabCls[2];
	RecalcNbGHForRiver(dat._vectUnit[0], datBD._vectUnit[0], tab[0]);  //таблица содержит прямые номера хэндов
	RecalcNbGHForRiver(dat._vectUnit[1], datBD._vectUnit[1], tab[1]);
	// надо переделать tab  в таблицу соответствия кластеров
	int revCls[ALL_CN_HAND], revClsBD[ALL_CN_HAND];
	int dim = rBD.CnHand(0);
	for (int i = 0; i < dim; i++)
		revClsBD[rBD.NbHand(0, i)] = i;
	dim = root.CnHand(0);
	for (int i = 0; i < dim; i++)
		revCls[root.NbHand(0, i)] = i;
	for (int i = 0; i < dim; i++)
	{
		int nbH = root.NbHand(0, i);
		stNbBtw ch = tab[0]._arr[nbH];
		int nbT = revCls[nbH];
		stNbBtw &chCls = tabCls[0]._arr[nbT];
		chCls = ch;
		chCls._nb0 = revClsBD[ch._nb0];
		chCls._nb1 = revClsBD[ch._nb1];
	}

	dim = rBD.CnHand(1);
	for (int i = 0; i < dim; i++)
		revClsBD[rBD.NbHand(1, i)] = i;
	dim = root.CnHand(1);
	for (int i = 0; i < dim; i++)
		revCls[root.NbHand(1, i)] = i;
	for (int i = 0; i < dim; i++)
	{
		int nbH = root.NbHand(1, i);
		stNbBtw ch = tab[1]._arr[nbH];
		int nbT = revCls[nbH];
		stNbBtw &chCls = tabCls[1]._arr[nbT];
		chCls = ch;
		chCls._nb0 = revClsBD[ch._nb0];
		chCls._nb1 = revClsBD[ch._nb1];
	}

	double k = (double)root._sit.PlMoney(0) / rBD._sit.PlMoney(0);
	FillTreeFromTree(&tree, &rBD._tree, k, tabCls, 1);
	return dist;
}
//----------------------------------------------------------------------------------------------------------------------------
bool	clRiverBanks::CreateClsForAdd(vector <vector <int>> &cls)
{
	if (!_fastTree.IsInit()) return false;
	//double step = MAX_NEAR_DIST * 4;

	int dim = _arr.size(), n2 = 2, nU = 1;
	while (dim > n2*n2)
	{
		nU++;
		n2 *= 2;
	}
	_fastTree.GetCenters(nU, cls);
	int dimCls = cls.size();
	for (int i = 0; i < dim; i++)
	{
		//glTrasser.WriteMessage(i);
		double min = _arr[i].Distance(_arr[cls[0][0]]);
		int nbMin = 0;
		for (int k = 1; k < dimCls; k++)
		{
			//glTrasser.WriteMessage(k);
			double dist = _arr[i].Distance(_arr[cls[k][0]]);
			if (dist < min)
			{
				min = dist;
				nbMin = k;
			}
		}
		if (i != cls[nbMin][0])
			cls[nbMin].push_back(i);
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
bool	clRiverBanks::SetRegimNeuro(bool reg)
{
	_regNet = reg;
	if (reg && _net.Empty())
	{
		clAnsiString path = DIR_BANK_RIVER;
		path += "river.net";
		int handle = _open(path.c_str(), O_BINARY | O_RDONLY);
		if (handle == -1)
		{
			_regNet = false;
			return false;
		}
		_net.ReadFile(handle);
		_close(handle);
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
/*int		clRiverBanks::FillRiverData(clRootRiverTree *root, clRiverDat &dat, tpTableChangeNb *tab, int &nbTree)
{
	tpFloat dist;
	int nb = NearestRiverCls(dat, dist);
	clRiverBankCls &cls = _arr[nb];
	int nb0 = 0, dimI = cls._info.size();
	double stackIn = CurStackToPot(root->_sit, 0);
	tpFloat k0 = 1, kA = 0.8;
	if (stackIn > cls._info[0]._stack)
	{
		for (nb0 = 1; nb0 < dimI; nb0++)
			if (stackIn < cls._info[nb0]._stack)
				break;
		if (nb0 < dimI)
		{
			k0 = 1 - (cls._info[nb0]._stack - stackIn) / (cls._info[nb0]._stack - cls._info[nb0 - 1]._stack);
			if (k0 < 1 - kA)
			{
				k0 = 1 - k0;
				nb0--;
			}
		}
		else
			nb0 = dimI - 1;
	}
	nbTree = cls._info[nb0]._nbRecord;

	RecalcNbGHForRiver(dat._vectUnit[0], cls._dat._vectUnit[0], tab[0]);
	RecalcNbGHForRiver(dat._vectUnit[1], cls._dat._vectUnit[1], tab[1]);
	return 1;
}*/
//----------------------------------------------------------------------------------------------------------------------------
void    clRiverBanks::InitFastTree()
{
	vector<clRiverBankTreeInfo> arr;
	arr.resize(_arr.size());
	for (int i = 0; i < _arr.size(); i++)
	{
		arr[i]._dat = _arr[i];
		arr[i]._nbRecord = i;
	}
	_fastTree.ClearAllBranch();
	_fastTree.Init(arr);
	_fastTree.SaveToFile();
}
//----------------------------------------------------------------------------------------------------------------------------
void	AddRiverData(clRiverDat &dat0, clRiverDat &dat, double mult)
{
	dat0._stack += dat._stack*mult;
	float res = 0;
	for (int k = 0; k < 2; k++)
	{
		int i = 0, j = 0;
		int dimI = (int)dat._vectUnit[k].size(), dimJ = (int)dat0._vectUnit[k].size();
		float ost = dat0._vectUnit[k][j]._weight, weight = dat._vectUnit[k][i]._weight;
		float val = 0;
		while (i < dimI)
		{
			if (weight < ost)
			{
				val += dat._vectUnit[k][i]._wf*weight;
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
				val += dat._vectUnit[k][i]._wf*ost;
				weight -= ost;
				dat0._vectUnit[k][j]._wf += float(val*mult / dat0._vectUnit[k][j]._weight);
				val = 0;
				j++;
				if (j == dimJ)
					break;
				ost = dat0._vectUnit[k][j]._weight;
			}
		}
		if (j < dimJ)
			dat0._vectUnit[k][j]._wf += float(val*mult / dat0._vectUnit[k][j]._weight);
	}
}
//----------------------------------------------------------------------------------------------------------------------------
int clRiverBankTreeInfo::FindNbCenterElement(vector <clRiverBankTreeInfo> arr)
{
	clRiverDat dat0;
	int dim0 = 20;
	dat0._vectUnit[0].resize(dim0);
	dat0._vectUnit[1].resize(dim0);
	float step = 1.f / dim0;
	for (int i = 0; i < dim0; i++)
	{
		dat0._vectUnit[0][i] = { i,0,step };
		dat0._vectUnit[1][i] = { i,0,step };
	}
	dat0._stack = 0;

	int dim = (int)arr.size();
	for (int i = 0; i < dim; i++)
		AddRiverData(dat0, arr[i]._dat, 1. / dim);

	tpFloat dMin = dat0.Distance(arr[0]._dat);
	int nbMin = 0;
	for (int i = 1; i < arr.size(); i++)
	{
		tpFloat d = dat0.Distance(arr[i]._dat);
		if (d < dMin)
		{
			d = dMin;
			nbMin = i;
		}

	}
	return nbMin;
}
int RivBankFastSearch(vector <clRiverBankFastTree *> &arr, clRiverDat &dat, double &dist);
int		clRiverBanks::FastSearchR(clRiverDat &dat, double &dist)
{
	vector <clRiverBankFastTree *> arr;
	arr.push_back(&this->_fastTree);
	return RivBankFastSearch(arr, dat, dist);

}
int RivBankFastSearch(vector <clRiverBankFastTree *> &arr, clRiverDat &dat, double &dist)
{
	vector <clRiverBankFastTree *> arr1;
	tpFloat dMin = 100000;
	bool allEnded = true;
	vector<tpFloat> dLeft, dRight, dNone; dLeft.resize(arr.size()); dRight.resize(arr.size()); dNone.resize(arr.size());
	for (int i = 0; i < arr.size(); i++)
	{
		if (arr[i]->IsEnded())
		{
			dNone[i] = arr[i]->_val->_dat.Distance(dat);
			if (dNone[i] < dMin)
				dMin = dNone[i];
		}
		else
		{
			allEnded = false;
			dLeft[i] = arr[i]->Left()->_val->_dat.Distance(dat);
			if (dLeft[i] < dMin)
				dMin = dLeft[i];

			dRight[i] = arr[i]->Right()->_val->_dat.Distance(dat);
			if (dRight[i] < dMin)
				dMin = dRight[i];
		}
	}
	for (int i = 0; i < arr.size(); i++)
	{
		if (arr[i]->IsEnded())
		{
			if (dNone[i] - arr[i]->_val->_maxDist < dMin || dMin == dNone[i])
				arr1.push_back(arr[i]);
		}
		else
		{
			if (dLeft[i] - arr[i]->Left()->_val->_maxDist < dMin || dMin == dLeft[i])
			{
				arr1.push_back(arr[i]->Left());
			}
			if (dRight[i] - arr[i]->Right()->_val->_maxDist < dMin || dMin == dRight[i])
			{
				arr1.push_back(arr[i]->Right());
			}
		}
	}
	if (allEnded)
	{
		for (int i = 0; i < arr.size(); i++)
			if (dNone[i] == dMin)
			{
				dist = dMin;
				return arr[i]->_val->_nbRecord;;
			}
	}
	if (arr1.size() == 0)
		int a = 2;
	arr.clear(); dRight.clear(); dLeft.clear(); dNone.clear();
	return RivBankFastSearch(arr1, dat, dist);
}
int	 clRiverBankFastTree::FastSearch(clRiverDat &dat, tpFloat &dist)
{
	if (IsEnded())
	{
		dist = _val->_dat.Distance(dat);
		return _val->_nbRecord;

	}

	tpFloat dLeft = this->_left->_val->_dat.Distance(dat);
	tpFloat dRight = this->_right->_val->_dat.Distance(dat);
	if (dLeft > dRight)
	{
		int nb = Right()->FastSearch(dat, dist);
		return nb;
	}
	else
	{
		int nb = Left()->FastSearch(dat, dist);
		return nb;
	}
}
void clRiverBankFastTree::Init(vector<clRiverBankTreeInfo> &arr)
{
	if (arr.size() == 0) return;

	_val->_nbRecord = arr[0]._nbRecord;
	_val->_dat = arr[0]._dat;
	_val->_maxDist = 0;
	if (arr.size() == 1)
		return;
	for (int i = 1; i < arr.size(); i++)
	{
		tpFloat d; d = arr[0]._dat.Distance(arr[i]._dat);
		if (d > _val->_maxDist)
			_val->_maxDist = d;
	}
	clClasters <clRiverBankTreeInfo> cls;
	cls.AddCheck(&arr);
	cls.Divide(2, 4, 0);
	this->AddLeftBranch()->Init(cls[0]);
	this->AddRightBranch()->Init(cls[1]);
}
//----------------------------------------------------------------------------------------------------------------------------
void	clRiverBankFastTree::GetCenters(int ur, vector <vector <int>> &cls)
{
	if (ur > 0)
	{
		if (_left != NULL)
			((clRiverBankFastTree *)_left)->GetCenters(ur - 1, cls);
		if (_right != NULL)
			((clRiverBankFastTree *)_right)->GetCenters(ur - 1, cls);
		return;
	}
	vector <int> vint = { _val->_nbRecord };
	cls.push_back(vint);
}
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------

void	clRiverBankUnit::CalcRoot(int cnCalc)
{
	_root.CalcIndex();
	_root.CalcForce();
	_root.CalcTree(cnCalc);
	_root.CalcEV(_ev);
}
//----------------------------------------------------------------------------------------------------------------------------
void	RecalcNbGHForRiver(vector <tpRiverGHUnit> &vectIn, vector <tpRiverGHUnit> &vect, tpTableChangeNb &tab)
{
	tab.ZeroData();
	//tpRiverGHUnit *ptrIn = &datIn._vectUnit[k][0], *arr = &dat._vectUnit[k][0];
	stNbBtw *nbF = tab._arr;
	int i = 0, j = 0, dimI = (int)vect.size(), dimJ = (int)vectIn.size();
	while (j < dimJ)
	{
		if (vectIn[j]._wf > vect[0]._wf)
		{
			nbF[vectIn[j]._nbH]._nb0 = vect[0]._nbH;
			nbF[vectIn[j]._nbH]._nb1 = vect[0]._nbH;
			nbF[vectIn[j]._nbH]._w0 = 1;
			nbF[vectIn[j]._nbH]._w1 = 0;
		}
		else
			break;
		j++;
	}
	if (dimI > 1)
	{
		i++;
		while (j < dimJ)
		{
			if (vectIn[j]._wf > vect[i]._wf)
			{
				float r0 = vect[i - 1]._wf - vectIn[j]._wf;
				float r1 = vectIn[j]._wf - vect[i]._wf;
				nbF[vectIn[j]._nbH]._nb0 = vect[i - 1]._nbH;
				nbF[vectIn[j]._nbH]._nb1 = vect[i]._nbH;
				nbF[vectIn[j]._nbH]._w0 = r1 / (r1 + r0);
				nbF[vectIn[j]._nbH]._w1 = r0 / (r1 + r0);
				j++;
			}
			else
			{
				if (i == dimI - 1)
					break;
				i++;
			}
		}
	}
	while (j < dimJ)
	{
		nbF[vectIn[j]._nbH]._nb0 = vect[i]._nbH;
		nbF[vectIn[j]._nbH]._nb1 = vect[i]._nbH;
		nbF[vectIn[j]._nbH]._w0 = 1;
		nbF[vectIn[j]._nbH]._w1 = 0;
		j++;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	Get2W(vector <float> &vect, int &n0, int &n1)
{
	int dim = vect.size();
	n0 = 0; n1 = 1;
	if (vect[0] < vect[1])
	{
		n0 = 1; n1 = 0;
	}
	for (int i = 2; i < dim; i++)
	{
		if (vect[i] > vect[n0])
		{
			n1 = n0;
			n0 = i;
		}
		else if (vect[i] > vect[n1])
			n1 = i;
	}
	if (vect[n0] < vect[n1])
	{
		int temp = n0;
		n0 = n1;
		n1 = temp;
	}
}
inline void	FillNbF(vector <tpRiverGHUnit> &vectIn, vector <tpRiverGHUnit> &vect, stNbRiver *nbF, vector <float> &vectorW, int i, int j, int nb0)
{
	if (vectorW.size() == 1)
	{
		nbF[vectIn[j]._nbH]._nb0 = vect[nb0]._nbH;
		nbF[vectIn[j]._nbH]._nb1 = 0;
		nbF[vectIn[j]._nbH]._w0 = 1;
		nbF[vectIn[j]._nbH]._w1 = 0;
	}
	else
	{
		int n0, n1;
		Get2W(vectorW, n0, n1);
		nbF[vectIn[j]._nbH]._nb0 = vect[nb0 + n0]._nbH;
		nbF[vectIn[j]._nbH]._nb1 = vect[nb0 + n1]._nbH;
		nbF[vectIn[j]._nbH]._w0 = vectorW[n0] / (vectorW[n0] + vectorW[n1]);
		nbF[vectIn[j]._nbH]._w1 = vectorW[n1] / (vectorW[n0] + vectorW[n1]);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	RecalcNbGHForRiver(vector <tpRiverGHUnit> &vectIn, vector <tpRiverGHUnit> &vect, tpTableChangeNb &tab)
{
	tab.ZeroData();
	//tpRiverGHUnit *ptrIn = &datIn._vectUnit[k][0], *arr = &dat._vectUnit[k][0];
	stNbRiver *nbF = tab._arr;
	int i = 0, j = 0, dimI = (int)vect.size(), dimJ = (int)vectIn.size(), nb0=0;
	float ostIn = vectIn[0]._weight, ost= vect[0]._weight;
	vector <float> vectorW;
	vectorW.reserve(ALL_CN_HAND);
	while (j < dimJ)
	{
		if (ostIn > ost)
		{
			vectorW.push_back(ost);
			ostIn -= ost;
			i++;
			if (i >= dimI)
				break;
			ost = vect[i]._weight;
		}
		else
		{
			ost -= ostIn;
			vectorW.push_back(ostIn);

			FillNbF(vectIn, vect, nbF, vectorW, i, j, nb0);

			j++;
			if (j >= dimJ)
				break;
			ostIn = vectIn[j]._weight;
			vectorW.clear();
			nb0 = i;
		}
	}
	if (j < dimJ)
	{
		FillNbF(vectIn, vect, nbF, vectorW, i, j, nb0);
		j++;
	}
	for (; j < dimJ; j++)
		nbF[j] = nbF[j - 1];
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	RiverBankReCalcEv(clRootRiverTree *root, int nbHero, clRiverDat &datIn, clRiverDat &dat, vector <float> &evIn, vector <float> &evBD)
{
	evIn.resize(ALL_CN_HAND);
	ZeroMemory(&evIn[0], ALL_CN_HAND * sizeof(float));
	tpTableChangeNb tab;
	RecalcNbGHForRiver(datIn._vectUnit[nbHero], dat._vectUnit[nbHero], tab);
	int dim = root->CnHand(nbHero);
	for (int i0 = 0; i0 < dim; i0++)
	{
		int nb0 = root->NbHand(nbHero, i0);
		stNbBtw nn0 = tab._arr[nb0];
		evIn[nb0] = evBD[nn0._nb0] * nn0._w0 + evBD[nn0._nb1] * nn0._w1;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void	FillTreeFromTree(clHoldemTree *nodeIn, clHoldemTree *node, double kSt, tpTableChangeNb *tab, double mult)
{
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
			if (nbb._nb0 == -1) continue;
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
}
//----------------------------------------------------------------------------------------------------------------------------
