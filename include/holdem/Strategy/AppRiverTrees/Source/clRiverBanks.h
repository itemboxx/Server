#ifndef TurnBanksH
#define TurnBanksH

#pragma once

#include "C:\home\holdem\Strategy\AppRiverTrees\Source\RiverTree.h"
#include "c:\Home\Util\FileSwapping.h"
#include "c:\home\Util\FileRec64.h"
#include "c:\Home\Util\Matrix.h"
#include "C:\Home\Holdem\Strategy\Source\defaultType.h"
#include "c:\Home\NeuroNet\Common\NeuroNet.h"
#include	"c:\\Home\\Util\\AnyType.h"

#ifdef COMP_C
#define	FILE_RIVER_PREPARE	"c:\\Data\\riverPrepare.bin"
#define	DIR_BANK_RIVER		"c:\\data\\RiverBank\\"
#define	PATH_BANK_RIVER		"c:\\data\\RiverBank\\FileRiverBank"
#define	FILE_TURN_PREPARE	"c:\\Data\\turnsPrepare.bin"
#define	FILE_FLOP_PREPARE	"c:\\Data\\flopPrepare.bin"
#define	FILE_PREFLOP_PREPARE	"c:\\Data\\PreflopPrepare.bin"
#define	FILE_TURN_BANK		"c:\\data\\TurnBank\\FileTurnBank"
#define	DIR_TURN_BANK		"c:\\data\\TurnBank\\"
#define	DIR_FLOP_BANK		"c:\\data\\FlopBank\\"
#define	FILE_FLOP_BANK		"c:\\data\\FlopBank\\FileFlopBank"
#define	TURN_NODE_BANK		"c:\\data\\TurnBank\\FileTurnNodeBank"
#define	FLOP_NODE_BANK		"c:\\data\\FlopBank\\FileFlopNodeBank"
#define	PREFLOP_NODE_BANK	"c:\\data\\PreFlopBank\\FilePreflopNodeBank"
#define	FILE_PREFLOP_BANK	"c:\\data\\PreFlopBank\\FilePreflopBank.bnk"
#else
#define	FILE_RIVER_PREPARE	"d:\\Data\\riverPrepare.bin"
#define	DIR_BANK_RIVER		"d:\\data\\RiverBank\\"
#define	PATH_BANK_RIVER		"d:\\data\\RiverBank\\FileRiverBank"
#define	FILE_TURN_PREPARE	"d:\\Data\\turnsPrepare.bin"
#define	FILE_FLOP_PREPARE	"d:\\Data\\flopPrepare.bin"
#define	FILE_PREFLOP_PREPARE	"d:\\Data\\PreflopPrepare.bin"
#define	FILE_TURN_BANK		"d:\\data\\TurnBank\\FileTurnBank"
#define	DIR_TURN_BANK		"d:\\data\\TurnBank\\"
#define	DIR_FLOP_BANK		"d:\\data\\FlopBank\\"
#define	FILE_FLOP_BANK		"d:\\data\\FlopBank\\FileFlopBank"
#define	TURN_NODE_BANK		"d:\\data\\TurnBank\\FileTurnNodeBank"
#define	FLOP_NODE_BANK		"d:\\data\\FlopBank\\FileFlopNodeBank"
#define	PREFLOP_NODE_BANK	"d:\\data\\PreFlopBank\\FilePreflopNodeBank"
#define	FILE_PREFLOP_BANK	"d:\\data\\PreFlopBank\\FilePreflopBank.bnk"
#endif

//#define RB_MAX_NEAR_DIST	0.15
//#define RB_STACK_SHIFT		1.2		// Должен быть больше 1

#define	MAX_CN_HANDS	100

//---------------------------------------------------------------------------------------------------------------------------------------------------
struct tpPrepareRiverBd
{
	int  WriteFile(int handle) { _gset.WriteFile(handle); _dat.WriteFile(handle); return 0; }
	int  ReadFile(int handle) { _gset.ReadFile(handle); _dat.ReadFile(handle); return 0; }
	size_t SizeInFile() { return _gset.SizeInFile() + _dat.SizeInFile(); }

	clInComeDataRoot _gset;
	clRiverDat _dat;
};

class clRiverBankUnit
{
public:
	inline int WriteFile(int handle) { _dat.WriteFile(handle); WriteVectFile(handle, _ev[0]); WriteVectFile(handle, _ev[1]); return _root.WriteFile(handle); }
	inline int ReadFile(int handle) { _dat.ReadFile(handle); ReadVectFile(handle, _ev[0]); ReadVectFile(handle, _ev[1]); return _root.ReadFile(handle); }
	inline size_t SizeInFile() { return _dat.SizeInFile() + SizeVectFile(_ev[0]) + SizeVectFile(_ev[1]) + _root.SizeInFile(); }
	void	CalcRoot(int cnCalc);

	clRiverDat _dat;
	vector <float>  _ev[2];
	clRootRiverTree _root;
};

/*struct stRiverBankUnitInfo
{
	int _nbRecord;
	tpFloat _stack;
};

class clRiverBankCls
{
public:
	//	clRiverBankOne();
	//	~clRiverBankOne();
	//bool CheckStack(tpFloat stack);
	//int  NearestStackAlways(tpFloat stack, tpFloat &dist);
	//void AddStack(tpFloat stack);
	int WriteFile(int handle);
	int ReadFile(int handle);
	size_t SizeInFile();

	clHandsGroupEx _gh[2];
	clRiverDat _dat;
	clCard _board[5];
	vector <stRiverBankUnitInfo> _info;
};*/

class clRiverBankTreeInfo
{
public:
	tpFloat Distance(clRiverBankTreeInfo &treeInf)
	{
		return _dat.Distance(treeInf._dat);
	}
	static int FindNbCenterElement(vector <clRiverBankTreeInfo> arr);
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
	void	Clear(){}

	clRiverDat _dat;
	double _maxDist;
	int _nbRecord; // nb RiverBankCls
};

class clRiverBankFastTree : public clBinTree <clRiverBankTreeInfo>
{
public:
	clRiverBankFastTree() { clBinTree::clBinTree();	_handle = -1; }
	~clRiverBankFastTree() { if(_handle != -1) _close(_handle); }
	void Init(vector<clRiverBankTreeInfo> &arr);
	bool IsInit() { return this->_left != NULL; }
	bool IsEnded() { return this->_left == NULL && this->_right == NULL; }
	int	 FastSearch(clRiverDat &dat, tpFloat &dist);
	
	clRiverBankFastTree *AddLeftBranch() { return (clRiverBankFastTree *)clBinTree <clRiverBankTreeInfo>::AddLeftBranch(); }
	clRiverBankFastTree *AddRightBranch() { return (clRiverBankFastTree *)clBinTree <clRiverBankTreeInfo>::AddRightBranch(); }

	clRiverBankFastTree *Left() { return (clRiverBankFastTree *)_left; }
	clRiverBankFastTree *Right() { return (clRiverBankFastTree *)_right; }
	void	LoadFromFile() { if (_handle != -1) { _lseek(_handle, 0, SEEK_SET); ReadFile(_handle); } }
	void	SaveToFile() { if (_handle != -1) { _lseek(_handle, 0, SEEK_SET); WriteFile(_handle); } }
	void	GetCenters(int ur, vector <vector <int>> &cls);

	int _handle;
};

struct tpUnfindRiverTree
{
	double _dist;
	clFileRecT <clInComeDataRoot> _file;
};

class clRiverBanks
{
public:
	void Create(char *path, int cnRecordU = 100);
	bool IsOpen() { return _fileUnit.IsOpen(); }
	bool OpenA(char *path, bool onlyRead, int memGB= SIZE_MB_DEF);
	void	Close() { _fileUnit.CloseFile(); _close(_fastTree._handle); _fastTree._handle = -1; }
	//int 	ReadUnit(int nbRec, clRiverBankUnit *unit) { return _fileUnit.ReadRecord(nbRec, unit); }
	int NearestRiverCls(clRiverDat &dat, tpFloat &dist);
	//void	NearestRiverMultiCls0(clRiverDat &dat, int cn, int *nb);
	void	NearestRiverMultiCls(clRiverDat &dat, int cn, tpIntDouble *nb);
	clRiverBankUnit *LockBankUnit(int nb) { return _fileUnit.LockRecord(nb); }
	void	UnLockBankUnit(int nb, clRiverBankUnit *unit) { _fileUnit.UnLockRecord(nb); }
	int	FillRiverEVNet(clRootRiverTree *root, vector <float> *ev, clLayersBPNet &net);
	//int	FillRiverEV_(clRootRiverTree *root, int nbHero, vector <float> &ev);
	int	FillRiverEV_(clRootRiverTree *root, vector <float> *ev, tpUnfindRiverTree *ptr);
	bool	FillRiverEV_IP(clRootRiverTree *root, vector <float> *ev, int level);
	int	FillRiverEV_(int nbCls, clRiverDat &datIn, clRootRiverTree *root, vector <float> *ev);
	void    InitFastTree();
	int		FastSearchR(clRiverDat &dat, double &dist);
	bool	BuildTreeForRoot(clRoot &root, tpUnfindRiverTree *ptr);
	bool	BuildTreeForRootIP(clRootRiverTree &root, int level, tpUnfindRiverTree *ptr);
	static double	GetTreeForRoot(clRootRiverTree &root, clRootRiverTree &rBD, clHoldemTree &tree);
	static bool GetTreeBD(clRoot &rootIn, clRiverDat &datIn, clRootRiverTree &root, clRiverDat &dat, double mult);
	//double	GetTreeForRoot(clRoot &root, tpUnfindRiverTree *ptr);
	bool	CreateClsForAdd(vector <vector <int>> &cls);
	void	CreateRiverDatMinBD(clRiverDat &dat10, clRiverDat &dat);

	clFileSwap <clRiverBankUnit> _fileUnit;
	clRiverBankFastTree _fastTree;
	vector <clRiverDat> _arr;
	clLayersBPNet _net;
	//bool _regNet;
};

extern clRiverBanks glBankRiver;

void	RecalcNbGHForRiver(vector <tpRiverGHUnit> &vectIn, vector <tpRiverGHUnit> &vect, tpTableChangeNb &tab);
void	RiverBankReCalcEv(clRootRiverTree *root, int nbHero, clRiverDat &datIn, clRiverDat &dat, vector <float> &evIn, vector <float> &ev, double mult);
//void	FillTreeFromTree(clHoldemTree *nodeIn, clHoldemTree *node, double kSt, tpTableChangeNb *tab, double mult);
//void	ChangeTreeCls(clHoldemTree *tree, clHandsGroupEx *gh, tpTableChangeNb *tab, double kStack, double mult);
//---------------------------------------------------------------------------------------------------------------------------------------------------

#endif