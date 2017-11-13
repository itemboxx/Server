//---------------------------------------------------------------------------

#ifndef StrategyPlayH
#define StrategyPlayH
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
//#include <list>
#include "TreeOnFile.h"

struct tpSaveSit
{
	clSitHoldem _sit;
	int _time, _sec;
};

struct tpArgVect
{
	unsigned _nbRec0,_cnLoc,_nbFile;
	clAtrStratHoldem *_ptr;
};

class clHistCalcS : public clStrategy
{
public:
	clHistCalcS()							{  }
	void	Clear()							{ _handsIn[0]._vect.clear(); _tree.ClearAllBranch(); }
	bool	IsEmpty()						{ return _handsIn[0]._vect.size() == 0; }
	void	BuildTree(clSitHoldem &sit0, clSitHoldem &sit);
	bool	RestraightTree(clSitHoldem &sit);
	void	CalcNesh(int cnMaxSec, double minDist,int cnThread);
	tpDis	Decide(clSitHoldem &sit, tpArrResBranch8 &arr);

};

struct stSPatr
{
//	stSPatr()					{}
//	stSPatr(stSPatr &val)		{ *this = val; }
	short _stackInBB;
	clTreeOnFile _tree;
	clFileRecT <clAtrStratHoldem> _file;
	vector <int> *_indFlop, *_indTurn, *_indRiver;

	stSPatr &operator = (stSPatr &val);
};

#define MAX_CN_CALCUNIT		1000

class clStrategyPlay
{
public:
	clStrategyPlay()		
	{
		_indRiver = (int *)malloc(CN_NABOR_HAND_RIVER*sizeof(int)); 
		//memset(_indRiver, -1, CN_NABOR_HAND_RIVER*sizeof(int));
		_lock = CreateSemaphore(NULL, 1, 1, "");
		_calcAtr.reserve(MAX_CN_CALCUNIT+10);
	}
	~clStrategyPlay();
	bool	Load(char *str);
	bool	LoadFromFile0(char *str);
	void	Clear();
	void	ClearCalcAtr();
	clAtrStratHoldem *LockNode(int nbfile, int nbRec, enNameTrade trade);
	void	UnLockNode(int nbfile, int nbRec, bool save = false);
	bool	CreateHistCalc(clHistCalcS &histC, clSitHoldem &sit);
	tpDis	DecideRemote(clSitHoldem &sit, tpArrResBranch8 &arr, int sec);
	tpDis	Decide(clSitHoldem &sit, clHistCalcS &histC, tpArrResBranch8 &arr);
	tpDis	DecideTree(clSitHoldem &sit, tpArrResBranch8 &arr);
	tpDis	FindDisForHistCalc(clSitHoldem &sit, tpArrResBranch8 &arr);
	void	ClearOldHistCalc();

	bool	GetGroupHandsIn(clHistCalcS &histC, clSitHoldem &sit);
	bool	GetGH(int nbTree,clTreeOnFile *tree, clSitHoldem &sit, int ind, int cnA,clHandsGroupEx  *handsIn);
	void	ChangeGHAll(int nbTree, clTreeOnFile *tree, clSitHoldem &sit, int nbB, clHandsGroupEx &handsIn);
	struct stVectorName
	{
		vector <int> _vectInt;
		clAnsiString _name;
	};
	int		FindVect(clAnsiString &name, vector <stVectorName> &vect);
//	int		FindNearestStack(clSitHoldem &sit);
	bool	FindNearestStacks(clSitHoldem &sit, int &nb1, int &nb2, double &w1, double &w2);
	clHistCalcS *GetHistCalc(clSitHoldem &sit);
private:
	tpDis	Decide(clSitHoldem &sit, tpArrResBranch8 &arr);
	tpDis	DecideRemote(clSitHoldem &sit, clHistCalcS &histC, tpArrResBranch8 &arr);
	bool	InitDataTree(clHistCalcS &histC);
	void	InitDataTree(clTreeStrat *tree, int nbTreeonFile, int *ptrTurn, int *ptrRiver);
	void	DeleteCalcAtr(int nb);

	vector <stSPatr *> _vectPtr;
	vector <tpArgVect> _vect;
	vector <stVectorName> _vectFlop, _vectTurn, _vectRiver;
	struct tpsitCalcAtr
	{
		__int64 _nbHand;
		int _curAct;
		time_t _time;
		clHistCalcS _histCalc;
	};
	vector <tpsitCalcAtr *> _calcAtr;
	int *_indRiver;
	HANDLE _lock;

//	int _nbCurTree;
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
