#ifndef NodeBanks_H
#define NodeBanks_H

#pragma once

#include "C:\home\holdem\Strategy\Source\defaultType.h"
#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\TurnTree.h"
#include "c:\home\Util\TemplateFileRec.h"
#include "c:\home\Util\FileSwapping.h"
#include "c:\home\Util\FileRec64.h"
//#include "c:\Home\Util\Matrix.h"

#define TB_MAX_NEAR_DIST	0.1

//---------------------------------------------------------------------------------------------------------------------------------------------------
class stNodeBankUnit
{
public:
	inline size_t SizeInFile() { return _dat.SizeInFile() + _node.SizeInFile(); }
	int WriteFile(int handle)
	{
		_dat.WriteFile(handle);
		_node.WriteFile(handle);
		return 0;
	}
	int ReadFile(int handle)
	{
		_dat.ReadFile(handle);
		_node.ReadFile(handle);
		return 0;
	}

	clStreetDat _dat;
	clStreetNode _node;
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
		return 0;
	}
	int ReadFile(int handle)
	{
		_dat.ReadFile(handle);
		_read(handle, &_maxDist, sizeof(_maxDist));
		_read(handle, &_nbRecord, sizeof(_nbRecord));
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
	clFileRecT <tpPrepareBd> _file;
};

class clNodeBank
{
public:
	~clNodeBank() { Close(); }
	void	Close();
	void Create(char *path, int cnRecord = 0);
	bool IsOpen() { return _fileUnit.IsOpen(); }
	bool OpenA(char *path, bool onlyRead);
	int NearestClsNb(clStreetDat &dat, tpFloat &dist);
	stNodeBankUnit *LockBankUnit(int nb) { return _fileUnit.LockRecord(nb); }
	void	UnLockBankUnit(int nb, stNodeBankUnit *unit = NULL) { _fileUnit.UnLockRecord(nb); }
	int	FillEV(clRootStreetTree *root, int nbHero, vector <float> &ev, tpUnfindTree *ut)
	{
		clStreetDat dat;
		root->CreateStreetDat(dat);
		return FillEV(root, dat, nbHero, ev, ut);
	}
	int FillEV(clRoot *root, clStreetDat &dat, int nbHero, vector <float> &ev, tpUnfindTree *ut);
	void    InitFastTree();
	bool	CheckFastTree();
	int		FastSearch(clStreetDat &dat, double &dist);
	bool	CreateClsForAdd(vector <vector <int>> &cls);

	clFileSwap <stNodeBankUnit> _fileUnit;
	clBankFastTree _fastTree;
	vector <clStreetDat> _arr;
};

extern clNodeBank glTurnNodeBank, glFlopNodeBank, glPreFlopNodeBank;

void	NodeBankReCalcEv(clRoot *root, int nbHero, clStreetDat &datIn, clStreetDat &dat, vector <float> &evIn, vector <float> &ev);
void	RecalcNbGH(vector <tpStreetDatGH> &vectIn, vector <tpStreetDatGH> &vect, tpTableChangeNb &tab); // в tab[i] прямые номера хэндов
int		StreetBankFastSearch(vector <clBankFastTree *> &arr, clStreetDat &dat, double &dist);
bool	CreateClsForAddReal(vector <vector <int>> &cls, vector <clStreetDat> &arr, clBankFastTree &fastTree);
//---------------------------------------------------------------------------------------------------------------------------------------------------

#endif