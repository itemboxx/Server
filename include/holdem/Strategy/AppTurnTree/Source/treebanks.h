#ifndef TreeBanks_H
#define TreeBanks_H

#pragma once

#include "TurnTree.h"
#include "..\..\..\..\Util\TemplateFileRec.h"
#include "..\..\..\..\Util\FileSwapping.h"
#include "..\..\..\..\Util\FileRec64.h"
#include "..\..\..\..\NeuroNet\NeuroNet.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
class clTreeBankUnit
{
public:

	inline int WriteFile(int handle) { _dat.WriteFile(handle); WriteVectFile(handle, _ev[0]); WriteVectFile(handle, _ev[1]); return _root.WriteFile(handle); }
	inline int ReadFile(int handle) { _dat.ReadFile(handle); ReadVectFile(handle, _ev[0]); ReadVectFile(handle, _ev[1]); return _root.ReadFile(handle); }
	inline size_t SizeInFile() { return _dat.SizeInFile() + SizeVectFile(_ev[0]) + SizeVectFile(_ev[1]) + _root.SizeInFile(); }
	void	CalcRootNode(int cnCalc, tpUnfindTree *ut);
	//void	CalcRootTree(int cnCalc);
	bool	CompactTree() { return _root._tree.CutTree(ACT_DELRIVER); }

	clStreetDat _dat;
	vector <float>  _ev[2];
	clRootStreetTree _root;
};

class clBinTreeInfo
{
public:
	tpFloat Distance(clBinTreeInfo &treeInf)
	{
		return _dat.Distance(treeInf._dat);
	}
	static int FindNbCenterElement(vector <clBinTreeInfo> arr);
	int WriteFile(int handle)
	{
		_dat.WriteFile(handle);
		_write(handle, &_maxDist, sizeof(_maxDist));
		_write(handle, &_nbRecord, sizeof(_nbRecord));
		//_write(handle, &_cnUnit, sizeof(_cnUnit));
		return 0;
	}
	int ReadFile(int handle)
	{
		_dat.ReadFile(handle);
		_read(handle, &_maxDist, sizeof(_maxDist));
		_read(handle, &_nbRecord, sizeof(_nbRecord));
		//_read(handle, &_cnUnit, sizeof(_cnUnit));
		return 0;
	}
	void	Clear() {}

	clStreetDat _dat;
	tpFloat _maxDist;
	int _nbRecord; // nb RiverBankCls
};

class clBankFastTree : public clBinTree <clBinTreeInfo>
{
public:
	clBankFastTree() { clBinTree::clBinTree();	_handle = -1; }
	~clBankFastTree() { if (_handle != -1) _close(_handle); }
	void Init(vector<clBinTreeInfo> &arr);
	bool IsInit() { return this->_left != NULL; }
	bool IsEnded() { return this->_left == NULL && this->_right == NULL; }
	//	int	 FastSearch(clRiverDat &dat, tpFloat &dist);

	clBankFastTree *AddLeftBranch() { return (clBankFastTree *)clBinTree <clBinTreeInfo>::AddLeftBranch(); }
	clBankFastTree *AddRightBranch() { return (clBankFastTree *)clBinTree <clBinTreeInfo>::AddRightBranch(); }

	clBankFastTree *Left() { return (clBankFastTree *)_left; }
	clBankFastTree *Right() { return (clBankFastTree *)_right; }
	void	LoadFromFile() { if (_handle != -1) { _lseek(_handle, 0, SEEK_SET); ReadFile(_handle); } }
	void	SaveToFile() { if (_handle != -1) { _lseek(_handle, 0, SEEK_SET); WriteFile(_handle); } }
	void	GetCenters(int ur, vector <vector <int>> &cls);

	int _handle;
};

struct tpUnfindTree
{
	double _dist;
	clFileRecT <clInComeDataRoot> _file;
};

class clTreeBank
{
public:
	void Create(char *path, int cnRecord = 0);
	bool IsOpen() { return _fileUnit.IsOpen(); }
	bool OpenA(char *path, bool onlyRead);
	int		NearestClsNb(clStreetDat &dat, tpFloat &dist);
	clTreeBankUnit *LockBankUnit(int nb) 
	{
		clTreeBankUnit * ptr = _fileUnit.LockRecord(nb);
		if (ptr->_root._tree.GetSit().GetLastAct()->_dis._act != ACT_DELTURN)
			ErrMessage("Error ", "bad sit");
		ptr->_root.CalcIndex();
		return ptr;
	}
	void	UnLockBankUnit(int nb, clTreeBankUnit *unit = NULL) { _fileUnit.UnLockRecord(nb); }
	int	FillEV(clRootStreetTree *root, int nbHero, vector <float> &ev);
	int	FillEV(clRootStreetTree *root, int nbHero, vector <float> &ev, clLayersBPNet &_net);
	//int	FillEVQ(clRootStreetTree *root, int nbHero, vector <float> &ev, clLayersBPNet &_net);
	bool	GetTreeForRoot(clRootStreetTree &root, tpUnfindTree *ptr);
	bool	GetTreeForRoot(clRoot &root, tpUnfindTree *ptr);
	//bool	FindTreeForSit(clRoot &hs0);
	void    InitFastTree();
	int		FastSearch(clStreetDat &dat, double &dist);
	bool	CreateClsForAdd(vector <vector <int>> &cls);
	//bool	SetRegimNeuro(bool reg);

	clFileSwap <clTreeBankUnit> _fileUnit;
	clBankFastTree _fastTree;
	vector <clStreetDat> _arr;
	tpUnfindTree _uf;
	clLayersBPNet _net;
	//bool _regNet;
};

extern clTreeBank glBankTurn;

int StreetBankFastSearch(vector <clBankFastTree *> &arr, clStreetDat &dat, double &dist);
void	RecalcNbGH(vector <tpStreetDatGH> &vectIn, vector <tpStreetDatGH> &vect, tpTableChangeNb &tab);
void	BankReCalcEv(clRoot *root, int nbHero, clStreetDat &datIn, clStreetDat &dat, vector <float> &evIn, vector <float> &evBD);
//---------------------------------------------------------------------------------------------------------------------------------------------------

#endif