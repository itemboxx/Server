//#include "stdafx.h"
#include <time.h>

#include "FlopBanks.h"
#include "..\..\AppRiverTrees\Source\clRiverBanks.h"

clFlopBank glBankFlop;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clFlopBankUnit::CalcRootEV(int cnCalc, clLayersBPNet *net)
{
	_root.CalcAll();
	_root.CalcNeshMultiHands(cnCalc, net);
	_root.CalcEvTree(_ev, net);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
int clFlopBank::NearestClsNb(clStreetDat &dat, tpFloat &dist)
{
	if (_fastTree.IsInit())
		return FastSearch(dat, dist);
	dist = 1000;
	int nb = -1;
	for (int i = 0; i < _arr.size(); i++)
	{
		tpFloat d1 = _arr[i].Distance(dat);
		if (d1 < dist)
		{
			dist = d1;
			nb = i;
		}
	}
	if (_fastTree.IsInit())
	{
		tpFloat dist1;
		int nbF = FastSearch(dat, dist1);
		if (nbF != nb)
		{
			ErrMessage("NearestClsNb::", "Результат не совпадает с FastSearch");
			bool b0 = false;
			if (b0)
				NearestClsNb(dat, dist1);
		}
	}
	return nb;
}
//----------------------------------------------------------------------------------------------------------------------------
void clFlopBank::Create(char *path, int cnRecord)
{
	clAnsiString asPath; asPath = path;
	_fileUnit.Create((asPath + ".fbM").c_str(), SIZE_MB_DEF, 0, cnRecord);
	_arr.clear();
	_sopen_s(&_fastTree._handle, (asPath + ".binT").c_str(), O_BINARY | O_RDWR | O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	_chsize(_fastTree._handle, 0);
}
//----------------------------------------------------------------------------------------------------------------------------
bool clFlopBank::OpenA(char *path, bool onlyRead)
{
	clAnsiString asPath; asPath = path;
	int val = asPath.Length() - 4;
	char c0 = asPath[val];
	if (c0 == '.')
		asPath.SetLength(asPath.Length() - 4);
	if (!_fileUnit.OpenFileA((asPath + ".fbM").c_str(), onlyRead, 10000))
		return false;
	_arr.clear();
	_arr.resize(_fileUnit.CnRecord());
	for (int i = 0; i < _fileUnit.CnRecord(); i++)
	{
		int handle = _fileUnit.clFileRecT::LockRecord(i);
		_arr[i].ReadFile(handle);
		_fileUnit.clFileRecT::UnLockRecord(i);
	}
	_sopen_s(&_fastTree._handle, (asPath + ".binT").c_str(), O_BINARY | O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (_filelength(_fastTree._handle) != 0)
		_fastTree.LoadFromFile();

	clAnsiString pathN = DIR_FLOP_BANK;
	pathN += "flop.net";
	int handle = _open(pathN.c_str(), O_BINARY | O_RDONLY);
	if (handle != -1)
	{
		_net.ReadFile(handle);
		_close(handle);
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
int		clFlopBank::FastSearch(clStreetDat &dat, double &dist)
{
	vector <clBankFastTree *> arr;
	arr.push_back(&this->_fastTree);
	return StreetBankFastSearch(arr, dat, dist);
}
//----------------------------------------------------------------------------------------------------------------------------
void    clFlopBank::InitFastTree()
{
	vector<clBinTreeInfo> arr;
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
bool	FlopNetGetEv(clStreetDat &dat, vector<float> *ev, clLayersBPNet &net)
{
	int monA = dat._stack[0] * 2 + dat._pot;
	int mon = monA / 2;
	vector <tpStreetDatGH> &vect0 = dat._vectUnit[0];
	vector <tpStreetDatGH> &vect1 = dat._vectUnit[1];
	if (vect0.size() != CN_FLOP_STANDART || vect1.size() != CN_FLOP_STANDART)
	{
		glTrasser.WriteMessage((clAnsiString)"Error FlopNetGetEv");
		ErrMessage("Error ", "FlopNetGetEv");
		return false;
	}
	int *ptrHb0 = GetNbHandFlopStandart(0);
	int *ptrHb1 = GetNbHandFlopStandart(1);
	vector<double> inData, outData;
	inData.resize(22);
	inData[0] = NNDouble01ToVal((double)dat._stack[0] / mon);
	inData[1] = NNDouble01ToVal((double)dat._pot / monA);
	for (int k = 0; k < CN_FLOP_STANDART; k++)
	{
		int nbH = ptrHb0[k], j = 0;
		for (; j < CN_FLOP_STANDART; j++)
			if (vect0[j]._nb == nbH)
				break;
		inData[2 + k] = NNDouble01ToVal(vect0[j]._weight);
	}
	for (int k = 0; k < CN_FLOP_STANDART; k++)
	{
		int nbH = ptrHb1[k], j = 0;
		for (; j < vect1.size(); j++)
			if (vect1[j]._nb == nbH)
				break;
		inData[CN_FLOP_STANDART + 2 + k] = NNDouble01ToVal(vect1[j]._weight);
	}
	//for (int i = 0; i < outData.size(); i++)
	//outData[i] = NNValToDouble01(outData[i]);
	net.Propogate(inData, outData);
	for (int i = 0; i < outData.size(); i++)
		outData[i] = NNValToDouble01(outData[i]);
	ev[0].resize(ALL_CN_HAND);
	ev[1].resize(ALL_CN_HAND);
	ZeroMemory(&ev[0][0], ALL_CN_HAND * sizeof(float));
	ZeroMemory(&ev[1][0], ALL_CN_HAND * sizeof(float));
	for (int i = 0; i < CN_FLOP_STANDART; i++)
		ev[0][ptrHb0[i]] = outData[i] * monA - mon;
	for (int i = 0; i < CN_FLOP_STANDART; i++)
		ev[1][ptrHb1[i]] = outData[i + CN_FLOP_STANDART] * monA - mon;
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
int	clFlopBank::FillEV(clRootFlopTree *root, int nbHero, vector <float> &ev, clLayersBPNet *net)
{
	clStreetDat dat;
	root->CreateStreetDat(dat);
	//vector <float> evN;
	//bool cN = false;

	clStreetDat datMin;
	clRootFlopTree r0 = *root;
	r0.CreateStreetDatMin(datMin, &dat);
	vector <float> ev0[2];
	if (net != NULL)
		if (FlopNetGetEv(datMin, ev0, *net))
		{
			BankReCalcEv(root, nbHero, dat, datMin, ev, ev0[nbHero]);
			return root->_sit.PlMoney(0);
		}
		else
		{
			glTrasser.WriteMessage((clAnsiString)"Не справилась нейросеть");
			//r0 = *root;
			//r0.CreateStreetDatMin(datMin, &dat);
			//RiverNetGetEv(datMin, ev0, _net);
		}

	tpFloat dist, dS;
	int nb = NearestClsNb(dat, dist);
	clFlopBankUnit *unit = LockBankUnit(nb);
	int res = unit->_root._sit.PlMoney(0);
	BankReCalcEv(root, nbHero, dat, unit->_dat, ev, unit->_ev[nbHero]);
	UnLockBankUnit(nb, unit);
	return res;
}
//----------------------------------------------------------------------------------------------------------------------------
bool	clFlopBank::GetTreeForRoot(clRoot &root, tpUnfindTree *ptrUT)
{
	clStreetDat dat;
	CreateFlopDat(root, dat);
	tpFloat dist;
	int nb = NearestClsNb(dat, dist);

	if (ptrUT != NULL)
		if (dist > ptrUT->_dist)
		{
			static int cn = 0;
			cn++;
			clAnsiString as = (root._sit.NameTrade() == TRADE_TURN) ? "TurnTree" : "FlopTree";
			as += (clAnsiString)cn + (clAnsiString)" dist=" + WSDoubleToAS(dist, 6);
			glTrasser.WriteMessage(as);
			if (ptrUT->_file.IsOpen())
			{
				clInComeDataRoot rec;
				rec = *((clInComeDataRoot *)&root);
				//ptrUT->_file.AddRecord(&rec);
			}
		}

	//if (dist > 0.12)
	//return false;

	clFlopBankUnit *unit = LockBankUnit(nb);
	int res = unit->_root._sit.PlMoney(0);
	tpTableChangeNb tab[2], tabCls[2];
	RecalcNbGH(dat._vectUnit[0], unit->_dat._vectUnit[0], tab[0]);		//таблица содержит прямые номера хэндов
	RecalcNbGH(dat._vectUnit[1], unit->_dat._vectUnit[1], tab[1]);
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
		if (chCls._w1 < 0.1)
		{
			chCls._nb0 = revClsBD[ch._nb0];
			chCls._nb1 = -1;
			chCls._w0 = 1;
			chCls._w1 = 0;
		}
		else
		{
			chCls._nb0 = revClsBD[ch._nb0];
			chCls._nb1 = revClsBD[ch._nb1];
		}
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
		if (chCls._w1 < 0.1)
		{
			chCls._nb0 = revClsBD[ch._nb0];
			chCls._nb1 = -1;
			chCls._w0 = 1;
			chCls._w1 = 0;
		}
		else
		{
			chCls._nb0 = revClsBD[ch._nb0];
			chCls._nb1 = revClsBD[ch._nb1];
		}
	}

	double k = (double)root._sit.PlMoney(0) / unit->_root._sit.PlMoney(0);
	FillTreeFromTree(&root._tree, &unit->_root._tree, k, tabCls, 0.01);

	//if(k0>0.99)
	//	CompareTree(&root._tree, &unit->_root._tree, k, tabCls);

	UnLockBankUnit(nb, unit);
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
