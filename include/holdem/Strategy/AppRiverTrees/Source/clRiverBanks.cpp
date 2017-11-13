//#include "stdafx.h"
#include <algorithm>
#include "clRiverBanks.h"
#include "..\..\..\..\Util\Clasters.h"

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
/*void	clRiverBanks::NearestRiverMultiCls0(clRiverDat &dat, int cn, int *nb)
{
	double dist[10];
	for (int i = 0; i < cn; i++)
		dist[i] = 1000;

	for (int i = 0; i < _arr.size(); i++)
	{
		tpFloat d1 = _arr[i].Distance(dat);
		for (int j = 0; j < cn; j++)
			if (d1 < dist[j])
			{
				for (int k = cn - 1; k > j; k--)
				{
					dist[k] = dist[k - 1];
					nb[k] = nb[k - 1];
				}
				dist[j] = d1;
				nb[j] = i;
				break;
			}
	}
}*/
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
	//SetRegimNeuro(false);

	clAnsiString pathN = DIR_BANK_RIVER;
	pathN += "river.net";
	int handle = _open(pathN.c_str(), O_BINARY | O_RDONLY);
	if (handle != -1)
	{
		_net.ReadFile(handle);
		_close(handle);
	}

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
int	clRiverBanks::FillRiverEVNet(clRootRiverTree *root, vector <float> *ev, clLayersBPNet &net)
{
	clRiverDat dat;
	root->CreateRiverDat(dat);
	tpFloat dist, d0, d1, d2;

	clRiverDat dat10;
	clRootRiverTree r0 = *root;
	r0.CreateRiverDatMin(dat10, &dat);
	//CreateRiverDatMinBD(dat10, dat);
	vector <float> ev0[2];
	if (RiverNetGetEv(dat10, ev0, _net))
	{
		double mult = root->_sit.EffStackSit() / 2500.;
		RiverBankReCalcEv(root, 0, dat, dat10, ev[0], ev0[0], mult);
		RiverBankReCalcEv(root, 1, dat, dat10, ev[1], ev0[1], mult);
	}
	return root->_sit.PlMoney(0);
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
int	clRiverBanks::FillRiverEV_(clRootRiverTree *root, vector <float> *ev, tpUnfindRiverTree *ptrUt)
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

	return FillRiverEV_(nb, dat, root, ev);
}
//----------------------------------------------------------------------------------------------------------------------------
int	clRiverBanks::FillRiverEV_(int nbCls, clRiverDat &dat, clRootRiverTree *root, vector <float> *ev)
{
	clRiverBankUnit *unit = LockBankUnit(nbCls);
	//unit->_root.CreateRiverDat(unit->_dat);

	double res = unit->_root._sit.EffStackSit();
	double k = root->_sit.EffStackSit() / res;
	RiverBankReCalcEv(root, 0, dat, unit->_dat, ev[0], unit->_ev[0], k);
	RiverBankReCalcEv(root, 1, dat, unit->_dat, ev[1], unit->_ev[1], k);
	UnLockBankUnit(nbCls, unit);
	return res;
}
//----------------------------------------------------------------------------------------------------------------------------
void	VectMVect(vector <float> &res, vector <float> &t, vector <float> &x)
{
	res = t;
	int dim = res.size();
	for (int i = 0; i < dim; i++)
		res[i] -= x[i];
}
//----------------------------------------------------------------------------------------------------------------------------
double MultScal(vector <float> &t, vector <float> &x)
{
	int dim = t.size();
	double res = 0;
	for (int i = 0; i < dim; i++)
		res += t[i] * x[i];
	return (res);
}
//----------------------------------------------------------------------------------------------------------------------------
void	ResFuncFi(vector <float> &res, vector <float> &a, vector <float> &b, double k)
{
	double ka = 1 - k;
	int dim = a.size();
	res.resize(dim);
	for (int i = 0; i < dim; i++)
		res[i] = a[i] * ka + b[i] * k;
}
//----------------------------------------------------------------------------------------------------------------------------
#define DIMENTION_DAT	3

void	ConvertDat(float *t, vector <tpRiverGHUnit> &dat)
{
	int dimD = dat.size();
	for (int i = 0; i < dimD; i++)
		if (dat[i]._wf > 0.7)
			t[0] += dat[i]._weight*dat[i]._wf;
		else if (dat[i]._wf > 0.4)
			t[1] += dat[i]._weight*dat[i]._wf;
		else
			t[2] += dat[i]._weight*dat[i]._wf;
}

void	ConvertDat(vector <float> &t, clRiverDat &dat)
{
	int dim = 2 * DIMENTION_DAT;
	t.resize(dim);
	ZeroMemory(&t[0], dim * sizeof(float));
	ConvertDat(&t[0], dat._vectUnit[0]);
	ConvertDat(&t[DIMENTION_DAT], dat._vectUnit[1]);
}

/*bool	clRiverBanks::FillRiverEV_IP(clRootRiverTree *root, vector <float> *ev, int level)
{
	clRiverDat dat;
	root->CreateRiverDat(dat);

	vector <float> t;
	ConvertDat(t, dat);

	int nbUnit[10];
	NearestRiverMultiCls(dat, level, nbUnit);
	vector <float> evN[2], x[10], y[10], f[10], dt, dx;
	//int *ptrNb0 = GetNbRiverDatDat(0), *ptrNb1 = GetNbRiverDatDat(1);
	int dimY = root->CnHand(0) + root->CnHand(1);
	for (int i = 0; i < level; i++)
	{
		FillRiverEV_(nbUnit[i], dat, root, evN);
		//x[i].resize(dim);
		y[i].resize(dimY);
		clRiverBankUnit *unit = LockBankUnit(nbUnit[i]);
		ConvertDat(x[i], unit->_dat);
		int nbY = 0;
		for (int j = 0; j < root->CnHand(0); j++)
			y[i][nbY++] = evN[0][root->NbHand(0, j)];
		for (int j = 0; j < root->CnHand(1); j++)
			y[i][nbY++] = evN[1][root->NbHand(1, j)];
		UnLockBankUnit(nbUnit[i], unit);
	}

	for (int iN = 1; iN < level; iN++)
	{
		for (int i = 0; i < level - iN; i++)
		{
			VectMVect(dt, t, x[i]);
			VectMVect(dx, x[i + iN], x[i]);
			double k = MultScal(dx, dx);
			if (k > DOUBLE_0)
				k = MultScal(dt, dx) / k;
			else
				ErrMessage("", "к==0");
			ResFuncFi(f[i], y[i], y[i + 1], k);
		}
		for (int i = 0; i < level - iN; i++)
			y[i] = f[i];
	}

	ev[0].resize(ALL_CN_HAND);
	ZeroMemory(&ev[0][0], ALL_CN_HAND * sizeof(float));
	ev[1] = ev[0];
	int nbY = 0;
	for (int j = 0; j < root->CnHand(0); j++)
		ev[0][root->NbHand(0, j)]= y[0][nbY++];
	for (int j = 0; j < root->CnHand(1); j++)
		ev[1][root->NbHand(1, j)] = y[0][nbY++];

	return true;
}*/
//----------------------------------------------------------------------------------------------------------------------------
int		CalcIP(int level, vector <float> &t, vector <float> *x, vector <float> *y0)
{
	vector <float> y[10], f[10], dt, dx;
	for (int i = 0; i < level; i++)
		y[i] = y0[i];
	for (int iN = 1; iN < level; iN++)
	{
		for (int i = 0; i < level - iN; i++)
		{
			VectMVect(dt, t, x[i]);
			VectMVect(dx, x[i + iN], x[i]);
			double k = MultScal(dx, dx);
			if (k > DOUBLE_0)
				k = MultScal(dt, dx) / k;
			else
				ErrMessage("", "к==0");
			if (k < -1 || k > 1)
				return i + iN;
			ResFuncFi(f[i], y[i], y[i + 1], k);
		}
		for (int i = 0; i < level - iN; i++)
			y[i] = f[i];
	}
	y0[0] = y[0];
	return 0;
}
//----------------------------------------------------------------------------------------------------------------------------
#define MAX_LEVEL_IP		10
int		CheckKForIP(int level, vector <float> &t, vector <float> *x, vector <float> *y)
{
	double ak[MAX_LEVEL_IP][MAX_LEVEL_IP];
	vector <float> dt, dx;
	ZeroMemory(ak, sizeof(ak));
	int nb1 = 0, nb2 = 1;
	for (int iN = 1; iN < level; iN++)
	{
		for (int i = 0; i < level - iN; i++)
		{
			VectMVect(dt, t, x[i]);
			VectMVect(dx, x[i + iN], x[i]);
			double k = MultScal(dx, dx);
			if (k > DOUBLE_0)
				ak[i][i + iN] = fabs(MultScal(dt, dx) / k);
			else
				ak[i][i + iN] = 1000;
			if (ak[nb1][nb2] > ak[i][i + iN])
			{
				nb1 = i;
				nb2 = i + iN;
			}
		}
	}
	bool b0[MAX_LEVEL_IP];
	ZeroMemory(b0, sizeof(b0));
	for (int i = level - 1; i >= 0; i--)
		if (i != nb1 && i != nb2)
		{
			for (int j = 0; j < i; j++)
				if (ak[j][i] > 1)
					b0[i] = true;
		}
	int nb = 0;
	for (int i = 0; i < level; i++)
		if (!b0[i])
		{
			if (nb != i)
			{
				x[nb] = x[i];
				y[nb] = y[i];
			}
			nb++;
		}
	return nb;
}
//----------------------------------------------------------------------------------------------------------------------------
bool	clRiverBanks::FillRiverEV_IP(clRootRiverTree *root, vector <float> *ev, int level)
{
	clRiverDat dat;
	root->CreateRiverDat(dat);

	clRiverDat dat10;
	clRootRiverTree r0 = *root;
	r0.CreateRiverDatMin(dat10, &dat);
	int dim = 2 * RIVERBANK_CN_HANDS;
	vector <float> t(dim);
	for (int j = 0; j < RIVERBANK_CN_HANDS; j++)
	{
		t[j] = dat10._vectUnit[0][j]._weight;
		t[j + RIVERBANK_CN_HANDS] = dat10._vectUnit[1][j]._weight;
	}

	tpIntDouble nbUnit[10];
	NearestRiverMultiCls(dat, level, nbUnit);
	vector <float> evN[2], x[MAX_LEVEL_IP], y[MAX_LEVEL_IP], f[MAX_LEVEL_IP], dt, dx;
	//int *ptrNb0 = GetNbRiverDatDat(0), *ptrNb1 = GetNbRiverDatDat(1);
	int dimY = root->CnHand(0) + root->CnHand(1);
	for (int i = 0; i < level; i++)
	{
		FillRiverEV_(nbUnit[i]._i, dat, root, evN);
		x[i].resize(dim);
		y[i].resize(dimY);
		clRiverBankUnit *unit = LockBankUnit(nbUnit[i]._i);
		for (int j = 0; j < RIVERBANK_CN_HANDS; j++)
		{
			x[i][j] = unit->_dat._vectUnit[0][j]._weight;
			x[i][j + RIVERBANK_CN_HANDS] = unit->_dat._vectUnit[1][j]._weight;
		}
		int nbY = 0;
		for (int j = 0; j < root->CnHand(0); j++)
			y[i][nbY++] = evN[0][root->NbHand(0, j)];
		for (int j = 0; j < root->CnHand(1); j++)
			y[i][nbY++] = evN[1][root->NbHand(1, j)];
		UnLockBankUnit(nbUnit[i]._i, unit);
	}

	if (level > 2)
		level = CheckKForIP(level, t, x, y);

	for (int iN = 1; iN < level; iN++)
	{
		for (int i = 0; i < level - iN; i++)
		{
			VectMVect(dt, t, x[i]);
			VectMVect(dx, x[i + iN], x[i]);
			double k = MultScal(dx, dx);
			if (k > DOUBLE_0)
				k = MultScal(dt, dx) / k;
			else
				ErrMessage("", "к==inf");
			//if (k < -1 || k > 1)
				//return i + iN;
			ResFuncFi(f[i], y[i], y[i + 1], k);
		}
		for (int i = 0; i < level - iN; i++)
			y[i] = f[i];
	}

	ev[0].resize(ALL_CN_HAND);
	ZeroMemory(&ev[0][0], ALL_CN_HAND * sizeof(float));
	ev[1] = ev[0];
	int nbY = 0;
	for (int j = 0; j < root->CnHand(0); j++)
		ev[0][root->NbHand(0, j)] = y[0][nbY++];
	for (int j = 0; j < root->CnHand(1); j++)
		ev[1][root->NbHand(1, j)] = y[0][nbY++];
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
bool	clRiverBanks::BuildTreeForRoot(clRoot &root, tpUnfindRiverTree *ptrUT)
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
#ifdef ERR_MESSAGE
	if (unit->_root._tree.CnCalc() == 0)
	{
		clAnsiString as = (clAnsiString)"Не посчитанное дерево ривера N " + (clAnsiString)nb;
		glTrasser.WriteMessage(as);
	}
#endif

	bool resB = GetTreeBD(root, dat, unit->_root, unit->_dat, 1);

	UnLockBankUnit(nb, unit);
	return resB;
}
//----------------------------------------------------------------------------------------------------------------------------
bool clRiverBanks::GetTreeBD(clRoot &rootIn, clRiverDat &datIn, clRootRiverTree &root0, clRiverDat &dat0, double mult)
{
	tpTableChangeNb tab[2], tabCls[2];
	RecalcNbGHForRiver(datIn._vectUnit[0], dat0._vectUnit[0], tab[0]);  //таблица содержит прямые номера хэндов
	RecalcNbGHForRiver(datIn._vectUnit[1], dat0._vectUnit[1], tab[1]);
	// надо переделать tab  в таблицу соответствия кластеров
	int revCls[ALL_CN_HAND], revClsBD[ALL_CN_HAND];
	int dim = root0.CnHand(0);
	for (int i = 0; i < dim; i++)
		revClsBD[root0.NbHand(0, i)] = i;
	dim = rootIn.CnHand(0);
	for (int i = 0; i < dim; i++)
		revCls[rootIn.NbHand(0, i)] = i;
	for (int i = 0; i < dim; i++)
	{
		int nbH = rootIn.NbHand(0, i);
		stNbBtw ch = tab[0]._arr[nbH];
		int nbT = revCls[nbH];
		stNbBtw &chCls = tabCls[0]._arr[nbT];
		chCls = ch;
		chCls._nb0 = revClsBD[ch._nb0];
		chCls._nb1 = revClsBD[ch._nb1];
	}

	dim = root0.CnHand(1);
	for (int i = 0; i < dim; i++)
		revClsBD[root0.NbHand(1, i)] = i;
	dim = rootIn.CnHand(1);
	for (int i = 0; i < dim; i++)
		revCls[rootIn.NbHand(1, i)] = i;
	for (int i = 0; i < dim; i++)
	{
		int nbH = rootIn.NbHand(1, i);
		stNbBtw ch = tab[1]._arr[nbH];
		int nbT = revCls[nbH];
		stNbBtw &chCls = tabCls[1]._arr[nbT];
		chCls = ch;
		chCls._nb0 = revClsBD[ch._nb0];
		chCls._nb1 = revClsBD[ch._nb1];
	}

	double k = (double)rootIn._sit.EffStackSit() / root0._sit.EffStackSit();
	bool resB = FillTreeFromTree(&rootIn._tree, &root0._tree, k, tabCls, mult);
	return resB;
}
//----------------------------------------------------------------------------------------------------------------------------
bool	clRiverBanks::BuildTreeForRootIP(clRootRiverTree &root, int level, tpUnfindRiverTree *ptr)
{
	if (level != 2)
		return false;
	clRiverDat dat;
	root.CreateRiverDat(dat);

	clRiverDat dat10;
	clRootRiverTree r0 = root;
	r0.CreateRiverDatMin(dat10, &dat);
	int dim = 2 * RIVERBANK_CN_HANDS;
	vector <float> t(dim);
	for (int j = 0; j < RIVERBANK_CN_HANDS; j++)
	{
		t[j] = dat10._vectUnit[0][j]._weight;
		t[j + RIVERBANK_CN_HANDS] = dat10._vectUnit[1][j]._weight;
	}

	tpIntDouble nbUnit[10];
	NearestRiverMultiCls(dat, level, nbUnit);

	vector <float> x[MAX_LEVEL_IP], dt, dx;
	int dimY = root.CnHand(0) + root.CnHand(1);
	for (int i = 0; i < level; i++)
	{
		x[i].resize(dim);
		clRiverBankUnit *unit = LockBankUnit(nbUnit[i]._i);
		for (int j = 0; j < RIVERBANK_CN_HANDS; j++)
		{
			x[i][j] = unit->_dat._vectUnit[0][j]._weight;
			x[i][j + RIVERBANK_CN_HANDS] = unit->_dat._vectUnit[1][j]._weight;
		}
		UnLockBankUnit(nbUnit[i]._i, unit);
	}
	//if (level > 2)
		//level = CheckKForIP(level, t, x, nbUnit);

	VectMVect(dt, t, x[0]);
	VectMVect(dx, x[1], x[0]);
	double k = MultScal(dx, dx);
	if (k > DOUBLE_0)
		k = MultScal(dt, dx) / k;
	else
		ErrMessage("", "к==inf");
	if (k > 1 || k < 0)
	{
		return false;
		clRiverBankUnit *unit = LockBankUnit(nbUnit[0]._i);
		bool resB = GetTreeBD(root, dat, unit->_root, unit->_dat, 1);
		UnLockBankUnit(nbUnit[0]._i, unit);
		return resB;
	}
	bool resB = true;
	if (fabs(1 - k) > 0.1)
	{
		clRiverBankUnit *unit = LockBankUnit(nbUnit[0]._i);
		resB = GetTreeBD(root, dat, unit->_root, unit->_dat, 1 - k);
		UnLockBankUnit(nbUnit[0]._i, unit);
	}
	//return resB;
	if (!resB) return false;
	if (fabs(k) > 0.1)
	{
		clRiverBankUnit *unit = LockBankUnit(nbUnit[1]._i);
		resB = GetTreeBD(root, dat, unit->_root, unit->_dat, k);
		UnLockBankUnit(nbUnit[1]._i, unit);
	}
	return resB;
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

#ifdef ERR_MESSAGE
	if (rBD._tree.CnCalc() == 0)
	{
		clAnsiString as = (clAnsiString)"Не посчитанное дерево ривера ";
		glTrasser.WriteMessage(as);
	}
#endif
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
void	CreateNewDat(vector <tpRiverGHUnit> &vect10, vector <tpRiverGHUnit> &vectIn, vector <tpRiverGHUnit> &vectBD)
{
	int dim = vectBD.size(), revNb[ALL_CN_HAND];
	vect10 = vectBD;
	for (int i = 0; i < dim; i++)
	{
		vect10[i]._wf = vect10[i]._weight = 0;
		revNb[vectBD[i]._nbH] = i;
	}
	tpTableChangeNb tab;
	RecalcNbGHForRiver(vectIn, vectBD, tab);

	dim = vectIn.size();
	for (int i0 = 0; i0 < dim; i0++)
	{
		int nbH = vectIn[i0]._nbH;
		stNbBtw nn0 = tab._arr[nbH];

		int nb0 = revNb[nn0._nb0];
		float ww = vect10[nb0]._weight + vectIn[i0]._weight*nn0._w0;
		vect10[nb0]._wf = (vect10[nb0]._wf*vect10[nb0]._weight + vectIn[i0]._wf*vectIn[i0]._weight*nn0._w0) / ww;
		vect10[nb0]._weight = ww;

		int nb1 = revNb[nn0._nb1];
		ww = vect10[nb1]._weight + vectIn[i0]._weight*nn0._w1;
		vect10[nb1]._wf = (vect10[nb1]._wf*vect10[nb1]._weight + vectIn[i0]._wf*vectIn[i0]._weight*nn0._w1) / ww;
		vect10[nb1]._weight = ww;
	}
}
//----------------------------------------------------------------------------------------------------------------------------
void	clRiverBanks::CreateRiverDatMinBD(clRiverDat &dat10, clRiverDat &dat)
{
	double dist;
	int nbCls = NearestRiverCls(dat, dist);
	clRiverBankUnit *unit = LockBankUnit(nbCls);

	dat10._stack = dat._stack;
	CreateNewDat(dat10._vectUnit[0], dat._vectUnit[0], unit->_dat._vectUnit[0]);
	CreateNewDat(dat10._vectUnit[1], dat._vectUnit[1], unit->_dat._vectUnit[1]);

	UnLockBankUnit(nbCls, unit);
}
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
//----------------------------------------------------------------------------------------------------------------------------
class clFastSearchRes
{
public:
	tpIntDouble	GetDistance(clRiverBankTreeInfo *info, clRiverDat &dat)
	{
		size_t dim = _vect.size();
		bool b0 = false;

		size_t first = 0;
		size_t last = dim;

		if (dim == 0)
		{
			tpIntDouble cd;
			cd._i = info->_nbRecord;
			cd._val = info->_dat.Distance(dat);
			_vect.push_back(cd);
			return cd;
		}
		else if (_vect[0]._i > info->_nbRecord)
			last = 0;
		else if (_vect[dim - 1]._i < info->_nbRecord)
			last = dim;
		else
			while (first < last)
			{
				size_t mid = first + (last - first) / 2;

				if (info->_nbRecord == _vect[mid]._i)
					return _vect[mid];
				else if (info->_nbRecord < _vect[mid]._i)
					last = mid;
				else
					first = mid + 1;
			}
		if (dim == _vect.capacity())
			_vect.reserve(2 * dim);
		tpIntDouble cd;
		cd._i = info->_nbRecord;
		cd._val = info->_dat.Distance(dat);
		_vect.insert(_vect.begin() + last, cd);
		return cd;
	}

	vector <tpIntDouble>	_vect;
};

int RivBankFastSearch(vector <clRiverBankFastTree *> &arr, clRiverDat &dat, double &dist, clFastSearchRes &res);
int		clRiverBanks::FastSearchR(clRiverDat &dat, double &dist)
{
	vector <clRiverBankFastTree *> arr;
	arr.push_back(&this->_fastTree);
	dist = 1000;
	clFastSearchRes res;
	return RivBankFastSearch(arr, dat, dist, res);
}
//----------------------------------------------------------------------------------------------------------------------------
int RivBankFastSearch(vector <clRiverBankFastTree *> &arr, clRiverDat &dat, double &dMin, clFastSearchRes &res)
{
	vector <clRiverBankFastTree *> arr1;
	bool allEnded = true;
	vector<tpFloat> dLeft, dRight, dNone; dLeft.resize(arr.size()); dRight.resize(arr.size()); dNone.resize(arr.size());
	for (int i = 0; i < arr.size(); i++)
	{
		if (arr[i]->IsEnded())
		{
			dNone[i] = res.GetDistance(arr[i]->_val, dat)._val;
			if (dNone[i] < dMin)
				dMin = dNone[i];
		}
		else
		{
			allEnded = false;
			dLeft[i] = res.GetDistance(arr[i]->Left()->_val, dat)._val;
			if (dLeft[i] < dMin)
				dMin = dLeft[i];

			dRight[i] = res.GetDistance(arr[i]->Right()->_val, dat)._val;
			if (dRight[i] < dMin)
				dMin = dRight[i];
		}
	}
	if (allEnded)
	{
		for (int i = 0; i < arr.size(); i++)
			if (dNone[i] == dMin)
			{
				return arr[i]->_val->_nbRecord;;
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
	return RivBankFastSearch(arr1, dat, dMin, res);
}
//----------------------------------------------------------------------------------------------------------------------------
void RivBankFastSearchMultiCls(vector <clRiverBankFastTree *> &arr, clRiverDat &dat, int cn, tpIntDouble *nb, clFastSearchRes &res);
void	clRiverBanks::NearestRiverMultiCls(clRiverDat &dat, int cn, tpIntDouble *nb)
{
	vector <clRiverBankFastTree *> arr;
	arr.push_back(&this->_fastTree);
	clFastSearchRes res;
	RivBankFastSearchMultiCls(arr, dat, cn, nb, res);
}
//----------------------------------------------------------------------------------------------------------------------------
inline	bool	CompareDist(tpIntDouble d0, tpIntDouble d1) { return d0._val > d1._val; }
void RivBankFastSearchMultiCls(vector <clRiverBankFastTree *> &arr, clRiverDat &dat, int cn, tpIntDouble *nb, clFastSearchRes &res)
{
	vector <clRiverBankFastTree *> arr1;
	bool allEnded = true;
	size_t dim = arr.size();
	vector<tpIntDouble> dLeft, dRight, dNone; dLeft.resize(dim); dRight.resize(dim); dNone.resize(dim);
	vector <tpIntDouble> dist; dist.reserve(2 * dim);
	for (int i = 0; i < dim; i++)
	{
		if (arr[i]->IsEnded())
		{
			dNone[i] = res.GetDistance(arr[i]->_val, dat);
			dist.push_back(dNone[i]);
		}
		else
		{
			allEnded = false;
			dLeft[i] = res.GetDistance(arr[i]->Left()->_val, dat);
			dist.push_back(dLeft[i]);

			dRight[i] = res.GetDistance(arr[i]->Right()->_val, dat);
			dist.push_back(dRight[i]);
		}
	}
	double dMin = 10000;
	if (dist.size() >= cn)
	{
		SortShell(dist.size(), &dist[0], CompareDist);
		dMin = dist[cn - 1]._val;
	}
	if (allEnded)
	{
		for (int i = 0; i < dim; i++)
			if (i < cn)
				nb[i] = dist[i];
		return;
	}
	for (int i = 0; i < dim; i++)
	{
		if (arr[i]->IsEnded())
		{
			if (dNone[i]._val - arr[i]->_val->_maxDist < dMin || dMin == dNone[i]._val)
				arr1.push_back(arr[i]);
		}
		else
		{
			if (dLeft[i]._val - arr[i]->Left()->_val->_maxDist < dMin || dMin == dLeft[i]._val)
			{
				arr1.push_back(arr[i]->Left());
			}
			if (dRight[i]._val - arr[i]->Right()->_val->_maxDist < dMin || dMin == dRight[i]._val)
			{
				arr1.push_back(arr[i]->Right());
			}
		}
	}
	return RivBankFastSearchMultiCls(arr1, dat, cn, nb, res);
}
//----------------------------------------------------------------------------------------------------------------------------
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
void	RiverBankReCalcEv(clRootRiverTree *root, int nbHero, clRiverDat &datIn, clRiverDat &dat, vector <float> &evIn, vector <float> &evBD, double mult)
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
		evIn[nb0] = (evBD[nn0._nb0] * nn0._w0 + evBD[nn0._nb1] * nn0._w1) * mult;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
