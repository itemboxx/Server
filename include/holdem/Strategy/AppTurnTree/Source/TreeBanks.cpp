//#include "stdafx.h"
#include <time.h>

#include "TreeBanks.h"
#include "..\..\..\..\Util\Clasters.h"
#include "..\..\AppRiverTrees\Source\clRiverBanks.h"

clTreeBank glBankTurn;

//----------------------------------------------------------------------------------------------------------------------------
void	clTreeBankUnit::CalcRootNode(int cnCalc, tpUnfindTree *ut)
{
#ifdef TURN_FULLTREE
	_root.CalcTree(cnCalc);
	_root.CalcEvTreeTurn(_ev);
#else
	_root.CalcAll();
	_root.CalcMultiHandsTree(cnCalc);
	_root.CalcEvTree(_ev, ut);
	_root.CalcWeightStrat();
#endif
}
//----------------------------------------------------------------------------------------------------------------------------
/*void	clTreeBankUnit::CalcRootTree(int cnCalc)
{
	_root.CalcMultiHandsTree(cnCalc);
	_root.CalcEvTree(_ev);
	_root.CalcWeightStrat();
}*/
//----------------------------------------------------------------------------------------------------------------------------
int clTreeBank::NearestClsNb(clStreetDat &dat, tpFloat &dist)
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
void clTreeBank::Create(char *path, int cnRecord)
{
	clAnsiString asPath; asPath = path;
	_fileUnit.Create((asPath + ".sbM").c_str(), SIZE_MB_DEF, 0, cnRecord);
	_arr.clear();
	_sopen_s(&_fastTree._handle, (asPath + ".binT").c_str(), O_BINARY | O_RDWR | O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	_chsize(_fastTree._handle, 0);
}
//----------------------------------------------------------------------------------------------------------------------------
int CheckRiverNode(clAtrHoldemTree *atr) { return (atr->_sitA.NameTrade() == TRADE_RIVER) ? 1 : 0; }
bool clTreeBank::OpenA(char *path, bool onlyRead)
{
	clAnsiString asPath; asPath = path;
	int val = asPath.Length() - 4;
	char c0 = asPath[val];
	if (c0 == '.')
		asPath.SetLength(asPath.Length() - 4);
	if (!_fileUnit.OpenFileA((asPath + ".sbM").c_str(), onlyRead, 10000))
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

	/*if (_fileUnit.CnRecord() > 0)
	{
		clTreeBankUnit rec;
		_fileUnit.ReadRecord(0, &rec);
		int cn = rec._root._tree.CnNode(CheckRiverNode);
#ifdef TURN_FULLTREE
		if (cn == 0)
		{
			ErrMessage("TurnBank ", "Не соответситвие версий");
			_fileUnit.ClearFile();
			return false;
		}
#else
		if (cn > 0)
		{
			ErrMessage("TurnBank ", "Не соответситвие версий");
			_fileUnit.ClearFile();
			return false;
		}
#endif
	}*/

	clAnsiString pathN = DIR_TURN_BANK;
	pathN += "turn.net";
	int handle = _open(pathN.c_str(), O_BINARY | O_RDONLY);
	if (handle != -1)
	{
		_net.ReadFile(handle);
		_close(handle);
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
/*bool	clTreeBank::SetRegimNeuro(bool reg)
{
	_regNet = reg;
	if (reg && _net.Empty())
	{
		clAnsiString path = DIR_TURN_BANK;
		path += "turn.net";
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
}*/
//----------------------------------------------------------------------------------------------------------------------------
int StreetBankFastSearch(vector <clBankFastTree *> &arr, clStreetDat &dat, double &dist)
{
	vector <clBankFastTree *> arr1;
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
	//arr.clear(); dRight.clear(); dLeft.clear(); dNone.clear();
	return StreetBankFastSearch(arr1, dat, dist);
}
//----------------------------------------------------------------------------------------------------------------------------
int		clTreeBank::FastSearch(clStreetDat &dat, double &dist)
{
	vector <clBankFastTree *> arr;
	arr.push_back(&this->_fastTree);
	return StreetBankFastSearch(arr, dat, dist);
}
//----------------------------------------------------------------------------------------------------------------------------
bool	CreateClsForAddReal(vector <vector <int>> &cls, vector <clStreetDat> &arr, clBankFastTree &fastTree)
{
	if (!fastTree.IsInit()) return false;
	//double step = MAX_NEAR_DIST * 4;

	int dim = arr.size(), n2 = 2, nU = 1;
	while (dim / n2 > 100)
	{
		nU++;
		n2 *= 2;
	}
	fastTree.GetCenters(nU, cls);
	int dimCls = cls.size();
	for (int i = 0; i < dim; i++)
	{
		//glTrasser.WriteMessage(i);
		double min = arr[i].Distance(arr[cls[0][0]]);
		int nbMin = 0;
		for (int k = 1; k < dimCls; k++)
		{
			//glTrasser.WriteMessage(k);
			double dist = arr[i].Distance(arr[cls[k][0]]);
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
bool	clTreeBank::CreateClsForAdd(vector <vector <int>> &cls)
{
	return CreateClsForAddReal(cls, _arr, _fastTree);
}
//----------------------------------------------------------------------------------------------------------------------------
void	CompareTree(clHoldemTree *nodeIn, clHoldemTree *node, double kSt, tpTableChangeNb *tab)
{
	if (nodeIn->CnBranch() < 2) return;
	int cnCls = nodeIn->AtrTree()->CnClasters();
	int ind = nodeIn->Indicator();
	for (int i = 0; i < cnCls; i++)
	{
		clCalcParam clsIn = nodeIn->AtrTree()->CalcParam(i);
		tpFloat wIn[MAX_CN_BRANCH];
		clsIn.GetWeightStrat(nodeIn->CnBranch(), wIn);
		stNbBtw chNb = tab[ind]._arr[i];

		clCalcParam cls = node->AtrTree()->CalcParam(chNb._nb0);
		tpFloat w[MAX_CN_BRANCH], wT[MAX_CN_BRANCH];
		cls.GetWeightStrat(node->CnBranch(), w);
		for (int j = 0; j < node->CnBranch(); j++)
			wT[j] = w[j] * chNb._w0;
		if (chNb._w1 > DOUBLE_0)
		{
			cls = node->AtrTree()->CalcParam(chNb._nb1);
			cls.GetWeightStrat(node->CnBranch(), w);
			for (int j = 0; j < node->CnBranch(); j++)
				wT[j] += w[j] * chNb._w1;
		}

		for (int j = 0; j < node->CnBranch(); j++)
			if (fabs(wIn[j] - wT[j]) > 0.01)
			{
				clAnsiString as = (clAnsiString)"погрешность=" + WSDoubleToAS(fabs(wIn[j] - wT[j]), 6);
				glTrasser.WriteMessage(as);
			}
	}
	for (int i = 0; i < nodeIn->CnBranch(); i++)
		CompareTree(nodeIn->Branch(i), node->Branch(i), 1, tab);
}
//----------------------------------------------------------------------------------------------------------------------------
void	RecalcNbGH(vector <tpStreetDatGH> &vectIn, vector <tpStreetDatGH> &vect, tpTableChangeNb &tab)  ///
{
	tab.ZeroData();
	stNbBtw *nbF = tab._arr;
	int dim = vect.size(), dimIn = vectIn.size();
	for (int i = 0; i < dimIn; i++)
	{
		int nbH = -1, nbL = -1;
		float minL = 1000, minH = 1000;
		for (int j = 0; j < dim; j++)
		{
			float dist = vectIn[i].Distance(vect[j]);
			if (vectIn[i] > vect[j])
			{
				if (dist < minH)
				{
					minH = dist;
					nbH = j;
				}
			}
			else
			{
				if (dist < minL)
				{
					minL = dist;
					nbL = j;
				}
			}
		}
		int nbHand = vectIn[i]._nb;
		if (nbL != -1) nbL = vect[nbL]._nb;
		if (nbH != -1) nbH = vect[nbH]._nb;
		if (minL < minH)
		{
			if (nbH == -1)
			{
				nbF[nbHand]._nb0 = nbL;
				nbF[nbHand]._w0 = 1;
				nbF[nbHand]._nb1 = 0;
				nbF[nbHand]._w1 = 0;
			}
			else
			{
				float dist = minH / (minH + minL);
				nbF[nbHand]._nb0 = nbL;
				nbF[nbHand]._w0 = dist;
				nbF[nbHand]._nb1 = nbH;
				nbF[nbHand]._w1 = 1 - dist;
			}
		}
		else
			if (nbL == -1)
			{
				nbF[nbHand]._nb0 = nbH;
				nbF[nbHand]._w0 = 1;
				nbF[nbHand]._nb1 = 0;
				nbF[nbHand]._w1 = 0;
			}
			else
			{
				float dist = minL / (minH + minL);
				nbF[nbHand]._nb0 = nbH;
				nbF[nbHand]._w0 = dist;
				nbF[nbHand]._nb1 = nbL;
				nbF[nbHand]._w1 = 1 - dist;
			}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	clTreeBank::GetTreeForRoot(clRootStreetTree &root, tpUnfindTree *ptrUT)
{
	clStreetDat dat;
	root.CreateStreetDat(dat);
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
				ptrUT->_file.AddRecord(&rec);
			}
		}

	//if (dist > 0.12)
		//return false;

	clTreeBankUnit *unit = LockBankUnit(nb);
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
			chCls._nb1 = 0;
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
			chCls._nb1 = 0;
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
bool	clTreeBank::GetTreeForRoot(clRoot &root, tpUnfindTree *ptrUT)
{
	clStreetDat dat;
	CreateStreetDatRoot(root, dat);
	tpFloat dist;
	int nb = NearestClsNb(dat, dist);

	if (ptrUT != NULL)
		if (dist > ptrUT->_dist)
		{
			clAnsiString as = "TurnTree";
			as += (clAnsiString)" dist=" + WSDoubleToAS(dist, 6);
			glTrasser.WriteMessage(as);
			if (ptrUT->_file.IsOpen())
			{
				clInComeDataRoot rec;
				rec = *((clInComeDataRoot *)&root);
				ptrUT->_file.AddRecord(&rec);
			}
		}

	clTreeBankUnit *unit = LockBankUnit(nb);
	int res = unit->_root._sit.PlMoney(0);
	tpTableChangeNb tab[2], tabCls[2];
	RecalcNbGH(dat._vectUnit[0], unit->_dat._vectUnit[0], tab[0]);		//таблица содержит прямые номера хэндов
	RecalcNbGH(dat._vectUnit[1], unit->_dat._vectUnit[1], tab[1]);
	// надо переделать tab на номера кластеров
	int revClsBD[ALL_CN_HAND];
	int dim = unit->_root.CnHand(0);
	for (int i = 0; i < dim; i++)
		revClsBD[unit->_root.NbHand(0, i)] = i;
	dim = root.CnHand(0);
	for (int i = 0; i < dim; i++)
	{
		int nbH = root.NbHand(0, i);
		stNbBtw ch = tab[0]._arr[nbH];
		stNbBtw &chCls = tabCls[0]._arr[i];
		chCls = ch;
		if (ch._w1 < 0.1)
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
	{
		int nbH = root.NbHand(1, i);
		stNbBtw ch = tab[1]._arr[nbH];
		stNbBtw &chCls = tabCls[1]._arr[i];
		chCls = ch;
		if (ch._w1 < 0.1)
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

	double k = (double)root._sit.EffStackSit() / unit->_root._sit.PlMoney(0);
	FillTreeFromTree(&root._tree, &unit->_root._tree, k, tabCls, 0.01);
	UnLockBankUnit(nb, unit);

	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
void	BankReCalcEv(clRoot *root, int nbHero, clStreetDat &datIn, clStreetDat &dat, vector <float> &evIn, vector <float> &evBD)
{
	evIn.resize(ALL_CN_HAND);
	ZeroMemory(&evIn[0], ALL_CN_HAND * sizeof(float));
	tpTableChangeNb tab;
	vector <tpStreetDatGH> &vect = dat._vectUnit[nbHero];
	vector <tpStreetDatGH> &vectIn = datIn._vectUnit[nbHero];
	RecalcNbGH(vectIn, vect, tab);
	int dim = root->CnHand(nbHero);
	for (int i0 = 0; i0 < dim; i0++)
	{
		int nb = root->NbHand(nbHero, i0);
		stNbBtw nn0 = tab._arr[nb];
		evIn[nb] = evBD[nn0._nb0] * nn0._w0 + evBD[nn0._nb1] * nn0._w1;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
bool	TurnNetGetEv(clRootStreetTree &root, vector<float> *ev, clLayersBPNet &net)
{
	int mon = root._sit.PlMoney(0);
	int monA = mon * 2;
	//vector <tpStreetDatGH> &vect0 = dat._vectUnit[0];
	//vector <tpStreetDatGH> &vect1 = dat._vectUnit[1];
	if (root.CnHand(0) != CN_STREETDAT_UNIT || root.CnHand(1) != CN_STREETDAT_UNIT)
	{
		glTrasser.WriteMessage((clAnsiString)"Error TurnNetGetEv");
		ErrMessage("Error ", "TurnNetGetEv");
		return false;
	}
	int *ptrHb0 = GetNbHandTurnDatDat(0);
	int *ptrHb1 = GetNbHandTurnDatDat(1);
	vector<double> inData, outData;
	inData.resize(22);
	inData[0] = NNDouble01ToVal((double)root._sit.PlayerMoney(0) / mon);
	inData[1] = NNDouble01ToVal((double)root._sit.TotalPot() / monA);
	for (int k = 0; k < CN_STREETDAT_UNIT; k++)
	{
		inData[2 + k] = NNDouble01ToVal(root._gh[0][ptrHb0[k]]);
		inData[CN_STREETDAT_UNIT + 2 + k] = NNDouble01ToVal(root._gh[1][ptrHb1[k]]);
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
	double sum=0;
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		int nb = ptrHb0[i];
		ev[0][nb] = outData[i] * monA - mon;
		sum += ev[0][nb] * root._gh[0][nb];

		nb = ptrHb1[i];
		ev[1][nb] = outData[i + CN_STREETDAT_UNIT] * monA - mon;
		sum += ev[1][nb] * root._gh[1][nb];
	}
	if (sum > DOUBLE_0)
	{
		sum /= 2;
		for (int i = 0; i < CN_STREETDAT_UNIT; i++)
		{
			ev[0][ptrHb0[i]] -= sum;
			ev[1][ptrHb1[i]] -= sum;
		}
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
double	DistEV(clHandsGroupEx gh, vector <float> &ev1, vector <float> &ev2)
{
	double res = 0;
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		double val = ev1[i] - ev2[i];
		res += val*val*gh[i];
	}
	return sqrt(res);
}
//----------------------------------------------------------------------------------------------------------------------------
int	clTreeBank::FillEV(clRootStreetTree *root, int nbHero, vector <float> &ev, clLayersBPNet &net)
{
	clStreetDat dat;
	if (!root->CreateStreetDat(dat))
	//if (!root->CreateTurnDatQuick(dat))
		return root->_sit.PlMoney(0);
	vector <float> evN;
	bool cN = false;

	clStreetDat datMin;
	clRootStreetTree r0 = *root;
	r0.CreateStreetDatMin(datMin, &dat);
	vector <float> ev0[2];
	if (TurnNetGetEv(r0, ev0, net))
	{
		BankReCalcEv(root, nbHero, dat, datMin, ev, ev0[nbHero]);

		/*			tpFloat dist, dS, d0=1000;
		int nb = NearestClsNb(dat, dist);
		clTreeBankUnit *unit0 = LockBankUnit(nb);
		vector <float> evN;
		BankReCalcEv(root, nbHero, dat, unit0->_dat, evN, unit0->_ev[nbHero]);
		UnLockBankUnit(nb, unit0);
		dS = DistEV(root->_gh[nbHero], ev, evN);
		if (dS > d0)
		{
		ErrMessage("", "BigDistEV");
		dist = datMin.Distance(unit0->_dat);
		}*/
		return root->_sit.PlMoney(0);
		cN = true;
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
	clTreeBankUnit *unit = LockBankUnit(nb);
	int res = unit->_root._sit.PlMoney(0);
	BankReCalcEv(root, nbHero, dat, unit->_dat, ev, unit->_ev[nbHero]);
	UnLockBankUnit(nb, unit);
	return res;
}
//----------------------------------------------------------------------------------------------------------------------------
int	clTreeBank::FillEV(clRootStreetTree *root, int nbHero, vector <float> &ev)
{
	clStreetDat dat;
	if (!root->CreateStreetDat(dat))
	//if (!root->CreateTurnDatQuick(dat))
		return root->_sit.PlMoney(0);
	tpFloat dist, dS;
	int nb = NearestClsNb(dat, dist);

	/*	if (_uf._file.IsOpen())
			if (dist > _uf._dist)
			{
				clAnsiString as = (clAnsiString)"nbNode=" + (clAnsiString)glTime1 + (clAnsiString)"; nbCard=" + (clAnsiString)glTime2;
				as += (clAnsiString)" dist=" + WSDoubleToAS(dist, 6);
				glTrasser.WriteMessage(as);
				tpPrepareBd rec;
				if (root->CnHand(0) > CN_STREETDAT_UNIT || root->CnHand(1) > CN_STREETDAT_UNIT)
				{
					clRootStreetTree root1 = *root;
					clStreetDat datIn = dat;

					root1.ReCalcParam(&datIn);
					root1.CalcIndex();
					root1.FindMinParam(datIn);
					root1.CreateStreetDat(datIn);

					rec._gset = *((clInComeDataRoot *)&root1);
					rec._dat = datIn;
					_uf._file.AddRecord(&rec);
				}
				else
				{
					rec._gset = *((clInComeDataRoot *)root);
					rec._dat = dat;
					_uf._file.AddRecord(&rec);
				}
			}*/

	clTreeBankUnit *unit = LockBankUnit(nb);
	int res = unit->_root._sit.PlMoney(0);
	BankReCalcEv(root, nbHero, dat, unit->_dat, ev, unit->_ev[nbHero]);
	UnLockBankUnit(nb, unit);
	return res;
}
//----------------------------------------------------------------------------------------------------------------------------
void    clTreeBank::InitFastTree()
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
void clBankFastTree::Init(vector<clBinTreeInfo> &arr)
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
	clClasters <clBinTreeInfo> cls;
	cls.AddCheck(&arr);
	cls.DivideForBin(_val->_maxDist / 2);
	//cls.Divide(2,4,0);
	this->AddLeftBranch()->Init(cls[0]);
	this->AddRightBranch()->Init(cls[1]);
}
//----------------------------------------------------------------------------------------------------------------------------
int clBinTreeInfo::FindNbCenterElement(vector <clBinTreeInfo> arr)
{
	clStreetDat dat0;
	dat0._vectUnit[0].resize(CN_STREETDAT_UNIT);
	dat0._vectUnit[1].resize(CN_STREETDAT_UNIT);
	for (int i = 0; i < CN_STREETDAT_UNIT; i++)
	{
		dat0._vectUnit[0][i] = { i,0,0,0 };
		dat0._vectUnit[1][i] = { i,0,0,0 };
	}
	dat0._stack[0] = dat0._stack[1] = dat0._stack[2] = dat0._pot = 0;

	int dim = (int)arr.size();
	float mult = 1.f / dim;
	vector <tpStreetDatGH> &v0 = dat0._vectUnit[0];
	vector <tpStreetDatGH> &v1 = dat0._vectUnit[1];
	for (int i = 0; i < dim; i++)
	{
		vector <tpStreetDatGH> &av0 = arr[i]._dat._vectUnit[0];
		vector <tpStreetDatGH> &av1 = arr[i]._dat._vectUnit[1];
		if (av0.size() != CN_STREETDAT_UNIT || av1.size() != CN_STREETDAT_UNIT)
			int a = 0;
		for (int k = 0; k < CN_STREETDAT_UNIT; k++)
		{
			v0[k]._force += av0[k]._force*av0[k]._weight;
			v0[k]._force50 += av0[k]._force50*av0[k]._weight;
			v0[k]._weight += av0[k]._weight;

			v1[k]._force += av1[k]._force*av1[k]._weight;
			v1[k]._force50 += av1[k]._force50*av1[k]._weight;
			v1[k]._weight += av1[k]._weight;
		}
		dat0._stack[0] += arr[i]._dat._stack[0];
		dat0._stack[1] += arr[i]._dat._stack[1];
		dat0._stack[2] += arr[i]._dat._stack[2];
		dat0._pot += arr[i]._dat._pot;
	}
	for (int k = 0; k < CN_STREETDAT_UNIT; k++)
	{
		if (v0[k]._weight > DOUBLE_0)
		{
			v0[k]._force /= v0[k]._weight;
			v0[k]._force50 /= v0[k]._weight;
			v0[k]._weight /= CN_STREETDAT_UNIT;
		}

		if (v1[k]._weight > DOUBLE_0)
		{
			v1[k]._force /= v1[k]._weight;
			v1[k]._force50 /= v1[k]._weight;
			v1[k]._weight /= CN_STREETDAT_UNIT;
		}
	}
	dat0._stack[0] /= dim;
	dat0._stack[1] /= dim;
	dat0._stack[2] /= dim;
	dat0._pot /= dim;
	dat0.Copy50AndSort();

	tpFloat dMin = dat0.Distance(arr[0]._dat);
	int nbMin = 0;
	for (int i = 1; i < dim; i++)
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
void	clBankFastTree::GetCenters(int ur, vector <vector <int>> &cls)
{
	if (ur > 0)
	{
		if (_left != NULL)
			((clBankFastTree *)_left)->GetCenters(ur - 1, cls);
		if (_right != NULL)
			((clBankFastTree *)_right)->GetCenters(ur - 1, cls);
		return;
	}
	vector <int> vint = { _val->_nbRecord };
	cls.push_back(vint);
}
//----------------------------------------------------------------------------------------------------------------------------
