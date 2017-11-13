#ifndef FlopBanks_H
#define FlopBanks_H

#pragma once

#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\TreeBanks.h"
#include "c:\home\Util\TemplateFileRec.h"
#include "c:\home\Util\FileSwapping.h"
#include "c:\home\Util\FileRec64.h"
#include "c:\Home\NeuroNet\Common\NeuroNet.h"
#include "C:\Home\Holdem\Strategy\AppFlopStrategy\Source\FlopTree.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
class clFlopBankUnit
{
public:

	inline int WriteFile(int handle) { _dat.WriteFile(handle); WriteVectFile(handle, _ev[0]); WriteVectFile(handle, _ev[1]); return _root.WriteFile(handle); }
	inline int ReadFile(int handle) { _dat.ReadFile(handle); ReadVectFile(handle, _ev[0]); ReadVectFile(handle, _ev[1]); return _root.ReadFile(handle); }
	inline size_t SizeInFile() { return _dat.SizeInFile() + SizeVectFile(_ev[0]) + SizeVectFile(_ev[1]) + _root.SizeInFile(); }
	//void	CalcRootNode(int cnCalc, tpUnfindTree *ut);
	void	CalcRootEV(int cnCalc, clLayersBPNet *net);
	//bool	CompactTree() { return _root._tree.CutTree(ACT_DELRIVER); }

	clStreetDat _dat;
	vector <float>  _ev[2];
	clRootFlopTree _root;
};

class clFlopBank
{
public:
	void Create(char *path, int cnRecord = 0);
	bool IsOpen() { return _fileUnit.IsOpen(); }
	bool OpenA(char *path, bool onlyRead);
	int		NearestClsNb(clStreetDat &dat, tpFloat &dist);
	clFlopBankUnit *LockBankUnit(int nb)
	{
		clFlopBankUnit * ptr = _fileUnit.LockRecord(nb);
		if (ptr->_root._tree.GetSit().GetLastAct()->_dis._act != ACT_DELFLOP)
			ErrMessage("Error ", "bad sit");
		ptr->_root.CalcIndex();
		return ptr;
	}
	void	UnLockBankUnit(int nb, clFlopBankUnit *unit = NULL) { _fileUnit.UnLockRecord(nb); }
	//int	FillEV(clRootStreetTree *root, int nbHero, vector <float> &ev);
	int	FillEV(clRootFlopTree *root, int nbHero, vector <float> &ev, clLayersBPNet *_net);
	//int	FillEVQ(clRootStreetTree *root, int nbHero, vector <float> &ev, clLayersBPNet &_net);
	bool	GetTreeForRoot(clRoot &root, tpUnfindTree *ptr);
	//bool	GetTreeForRoot(clRoot &root);
	//bool	FindTreeForSit(clRoot &hs0);*/
	void    InitFastTree();
	int		FastSearch(clStreetDat &dat, double &dist);
	//bool	CreateClsForAdd(vector <vector <int>> &cls);
	//bool	SetRegimNeuro(bool reg);

	clFileSwap <clFlopBankUnit> _fileUnit;
	clBankFastTree _fastTree;
	vector <clStreetDat> _arr;
	tpUnfindTree _uf;
	clLayersBPNet _net;
	//bool _regNet;
};

extern clFlopBank glBankFlop;
//---------------------------------------------------------------------------------------------------------------------------------------------------

#endif