//#include "stdafx.h"
#include <time.h>

#include "C:\home\Holdem\Strategy\AppTurnTree\Source\NodeBanks.h"
#include "c:\Home\Util\Clasters.h"
#include "c:\Home\Util\Sort.h"

clNodeBank glTurnNodeBank, glFlopNodeBank, glPreFlopNodeBank;
//----------------------------------------------------------------------------------------------------------------------------
int clNodeBank::NearestClsNb(clStreetDat &dat, tpFloat &dist)
{
	if (_fastTree.IsInit())
		return FastSearch(dat, dist);
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
		tpFloat dist1;
		int nbF = FastSearch(dat, dist1);
		if (nbF != nb)
		{
			ErrMessage("NearestRiverCls::", "Результат не совпадает с FastSearch");
			NearestClsNb(dat, dist1);
		}
	}*/
	return nb;
}
//----------------------------------------------------------------------------------------------------------------------------
int		clNodeBank::FastSearch(clStreetDat &dat, double &dist)
{
	vector <clBankFastTree *> arr;
	arr.push_back(&this->_fastTree);
	return StreetBankFastSearch(arr, dat, dist);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
bool	clNodeBank::CreateClsForAdd(vector <vector <int>> &cls)
{
	return CreateClsForAddReal(cls, _arr, _fastTree);
}
//----------------------------------------------------------------------------------------------------------------------------
bool	CreateClsForAddReal(vector <vector <int>> &cls, vector <clStreetDat> &arr, clBankFastTree &fastTree)
{	
	if (!fastTree.IsInit()) return false;
	//double step = MAX_NEAR_DIST * 4;

	int dim = arr.size(), n2=2, nU=1;
	while (dim / n2 > 100)
	{
		nU++;
		n2 *= 2;
	}
	fastTree.GetCenters(nU, cls);
	int dimCls = cls.size();
	for (int i = 0; i < dim; i++)
	{
		glTrasser.WriteMessage(i);
		double min = arr[i].Distance(arr[cls[0][0]]);
		int nbMin = 0;
		for (int k = 1; k < dimCls; k++)
		{
			glTrasser.WriteMessage(k);
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
	arr.clear(); dRight.clear(); dLeft.clear(); dNone.clear();
	return StreetBankFastSearch(arr1, dat, dist);
}
//----------------------------------------------------------------------------------------------------------------------------
void clNodeBank::Create(char *path, int cnRecord)
{
	clAnsiString asPath; asPath = path;
	_fileUnit.Create((asPath + ".tbM").c_str(), SIZE_MB_DEF, 0, cnRecord);
	_arr.clear();
	_sopen_s(&_fastTree._handle, (asPath + ".binT").c_str(), O_BINARY | O_RDWR | O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	_chsize(_fastTree._handle, 0);
}
//----------------------------------------------------------------------------------------------------------------------------
bool clNodeBank::OpenA(char *path, bool onlyRead)
{
	clAnsiString asPath; asPath = path;
	int val = asPath.Length() - 4;
	char c0 = asPath[val];
	if (c0 == '.')
		asPath.SetLength(asPath.Length() - 4);
	if (!_fileUnit.OpenFileA((asPath + ".tbM").c_str(), onlyRead, 10000))
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

	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
void	clNodeBank::Close()
{
	_fileUnit.CloseFile();
	if (_fastTree._handle != -1)
		_close(_fastTree._handle);
	_fastTree._handle = -1;
	_arr.clear();
}
//----------------------------------------------------------------------------------------------------------------------------
int	clNodeBank::FillEV(clRoot *root, clStreetDat &dat, int nbHero, vector <float> &ev, tpUnfindTree *ut)
{
	tpFloat dist;
	int nb = NearestClsNb(dat, dist);

	if (ut != NULL && dist > ut->_dist)
	{
		clAnsiString as = (clAnsiString)"nbTree=" + (clAnsiString)glTime1 + (clAnsiString)"; nbNode=" + (clAnsiString)glTime2;
		as += (clAnsiString)" clNodeBank::FillEV dist=" + WSDoubleToAS(dist, 6);
		glTrasser.WriteMessage(as);
		if (ut->_file.IsOpen())
		{
			tpPrepareBd rec;
			rec._gset = *((clInComeDataRoot *)root);
			rec._dat = dat;
			ut->_file.AddRecord(&rec);
		}
	}

	stNodeBankUnit *unit = LockBankUnit(nb);
	int res = unit->_node._sit.PlMoney(0);
	/*	if (unit->GetCnCalc() == 0)
		{
			ErrMessage("Не посчитанный узел", "");
			return res;
		}*/
	NodeBankReCalcEv(root, nbHero, dat, unit->_dat, ev, unit->_node._ev[nbHero]);
	UnLockBankUnit(nb, unit);
	/*if (w0 > kA)
		return res;
	vector <float> evT; evT.resize(ALL_CN_HAND);
	unit = LockBankUnit(cls._info[nb1]._nbRecord);
	int res1 = unit->_sit.PlMoney(0);
	w1 = w1*res / res1;
	NodeBankReCalcEv(root, nbHero, dat, cls._dat, evT, unit->_ev[nbHero]);
	UnLockBankUnit(cls._info[nb1]._nbRecord, unit);
	for (int i = 0; i < ALL_CN_HAND; i++)
		ev[i] = ev[i] * w0 + evT[i] * w1;*/
	return res;
}
//----------------------------------------------------------------------------------------------------------------------------
void    clNodeBank::InitFastTree()
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
void	CheckBinTree(clBinTree <clBinTreeInfo> *tree, vector <int> &vint)
{
	ErrCheck(tree->_val->_nbRecord, 0, vint.size() - 1);
	if (tree->EndedNode())
	{
		vint[tree->_val->_nbRecord]++;
	}
	else 
	{
		if (tree->_left == NULL)
			ErrMessage("Error", "левая ветвь равна NULL");
		if (tree->_right == NULL)
			ErrMessage("Error", "правая ветвь равна NULL");
		CheckBinTree(tree->_left, vint);
		CheckBinTree(tree->_right, vint);
	}
}
//----------------------------------------------------------------------------------------------------------------------------
bool	clNodeBank::CheckFastTree()
{
	int dim = _arr.size();
	vector <int> vint;
	vint.resize(dim, 0);
	CheckBinTree(&_fastTree, vint);
	for(int i=0;i<dim;i++)
		if (vint[i] != 1)
		{
			ErrMessage("Error ", "CheckFastTree");
			return false;
		}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
/*void	RecalcNbGH(clRootStreetTree *rootIn, clRootStreetTree *root, tpTableChangeNb &tab)
{
		rootIn->CalcForce();
		root->CalcForce();
		tab.ZeroData();
		for (int k = 0; k < 2; k++)
		{
			vector <tpStreetDatGH> &vectIn = rootIn->_vectUnit[k];
			vector <tpStreetDatGH> &vect = root->_vectUnit[k];
			int dimIn = vectIn.size();
			int dim = vect.size();
			for (int i = 0; i < dimIn; i++)
			{
				stNbRiver &nbN = tab._arr[k][vectIn[i]._nb];
				float dL=1000, dH=1000;
				int nbL = -1, nbH = -1;
				for (int j = 0; j < dim; j++)
				{
					float dist = vectIn[i].SqrDist(vect[j]);
					if (vectIn[i] > vect[j])
					{
						if (dist < dH)
						{
							dH = dist;
							nbH = j;
						}
					}
					else
					{
						if (dist < dL)
						{
							dL = dist;
							nbL = j;
						}
					}
				}
				if (nbL == -1)
				{
					nbN._nb0 = vect[nbH]._nb;
					nbN._w0 = 1;
					nbN._nb1 = 0;
					nbN._w1 = 0;
				}
				else if (nbH == -1)
				{
					nbN._nb0 = vect[nbL]._nb;
					nbN._w0 = 1;
					nbN._nb1 = 0;
					nbN._w1 = 0;
				}
				else
				{
					nbN._nb0 = vect[nbL]._nb;
					nbN._nb1 = vect[nbH]._nb;
					dL = sqrt(dL); dH = sqrt(dH);
					float sum = dL + dH;
					nbN._w0 = dH/sum;
					nbN._w1 = dL/sum;
				}
			}
		}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	RecalcNbGH(vector <tpStreetDatGH> &vectIn, vector <tpStreetDatGH> &vect, tpTableChangeNb &tab)  ///
{
	tab.ZeroData();
	stNbRiver *nbF = tab._arr;
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
void	NodeBankReCalcEv(clRoot *root, int nbHero, clStreetDat &datIn, clStreetDat &dat, vector <float> &evIn, vector <float> &evBD)
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
		stNbRiver nn0 = tab._arr[nb];
		evIn[nb] = evBD[nn0._nb0] * nn0._w0 + evBD[nn0._nb1] * nn0._w1;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
bool	FuncComp12(tpStreetDatGH val1, tpStreetDatGH val2) { return (val1._force + val1._force50 < val2._force + val2._force50); }

void	AddStreetData(clStreetDat &dat0, clStreetDat &datIn, double mult)
{
	dat0._stack += datIn._stack*mult;
	float res = 0;
	for (int k = 0; k < 2; k++)
	{
		vector <tpStreetDatGH> vect;
		vect = datIn._vectUnit[k];
		int dimI = (int)vect.size(), dimJ = (int)dat0._vectUnit[k].size();
		SortShell(dimI, &vect[0], FuncComp12);
		int i = 0, j = 0;
		float ost = dat0._vectUnit[k][j]._weight, weight = vect[i]._weight;
		float val = 0, val50 = 0;
		while (i < dimI)
		{
			if (weight < ost)
			{
				val += vect[i]._force*weight;
				val50 += vect[i]._force50*weight;
				ost -= weight;
				i++;
				if (i == dimI)
					break;
				weight = vect[i]._weight;
				if (weight < DOUBLE_0)
					break;
			}
			else
			{
				val += vect[i]._force*ost;
				val50 += vect[i]._force50*ost;
				weight -= ost;
				dat0._vectUnit[k][j]._force += float(val*mult / dat0._vectUnit[k][j]._weight);
				dat0._vectUnit[k][j]._force50 += float(val50*mult / dat0._vectUnit[k][j]._weight);
				val = 0; val50 = 0;
				j++;
				if (j == dimJ)
					break;
				ost = dat0._vectUnit[k][j]._weight;
			}
		}
		if (j < dimJ)
		{
			dat0._vectUnit[k][j]._force += float(val*mult / dat0._vectUnit[k][j]._weight);
			dat0._vectUnit[k][j]._force50 += float(val50*mult / dat0._vectUnit[k][j]._weight);
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------------
int clBinTreeInfo::FindNbCenterElement(vector <clBinTreeInfo> arr)
{
	clStreetDat dat0;
	int dim0 = 20;
	dat0._vectUnit[0].resize(dim0);
	dat0._vectUnit[1].resize(dim0);
	float step = 1.f / dim0;
	for (int i = 0; i < dim0; i++)
	{
		dat0._vectUnit[0][i] = { i,step,0,0 };
		dat0._vectUnit[1][i] = { i,step,0,0 };
	}
	dat0._stack = 0;

	int dim = (int)arr.size();
	for (int i = 0; i < dim; i++)
		AddStreetData(dat0, arr[i]._dat, 1. / dim);

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
	cls.Divide(2, 3, 0);
	this->AddLeftBranch()->Init(cls[0]);
	this->AddRightBranch()->Init(cls[1]);
}
//----------------------------------------------------------------------------------------------------------------------------
void	clBankFastTree::GetCenters(int ur, vector <vector <int>> &cls)
{
	if (ur > 0)
	{
		if(_left != NULL)
			((clBankFastTree *)_left)->GetCenters(ur - 1, cls);
		if (_right != NULL)
			((clBankFastTree *)_right)->GetCenters(ur - 1, cls);
		return;
	}
	vector <int> vint = { _val->_nbRecord };
	cls.push_back(vint);
}
//----------------------------------------------------------------------------------------------------------------------------
