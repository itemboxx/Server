//---------------------------------------------------------------------------

#ifndef RiverTreeH
#define RiverTreeH
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "C:\home\holdem\Common\sitHoldem.h"
#include "C:\home\Holdem\Common\HoldemTree.h"
#include "c:\Home\Util\Matrix.h"
#include "C:\home\Holdem\Common\clRoot.h"

class clRootRiverTree;

/*class clRiverTree : public clHoldemTree
{
public:
	clRiverTree *Branch(int nb) { return (clRiverTree *)clTree < clAtrHoldemTree >::Branch(nb); }
//private:
	tpFloat		CalcNeshA(clRootRiverTree *root, int nbHero, int nbClsHero, tpFloat kH, vector <tpFloat> &kOpp);
	void		CalcNeshATest(clRootRiverTree *root, int nbHero, int nbClsHero, tpFloat kH, vector <tpFloat> &kOpp, vector <tpFloat> &ev);
	tpFloat		CalcNeshA(clRootRiverTree *root, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <tpFloat> &ev);
	tpFloat CalcAntiStratEV(clRootRiverTree *root, int nbHero, int nbHand, clHandsGroupEx &gh);
	tpFloat CalcQAntiStratEV(clRootRiverTree *root, int nbHero, int nbHand, clHandsGroupEx &gh);
	tpFloat	NodeResult(clRootRiverTree *root, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <tpFloat> &ev);
	tpFloat	AddResEVToNode(clRootRiverTree *root, int nbHero, int cnB, vector <tpFloat> &resEV, tpFloat *w, vector <tpFloat> *evCur, vector <tpFloat> &kH, vector <tpFloat> &kOpp);
};*/

class tpRiverGHUnit
{
public:
	float	Distance(tpRiverGHUnit &val) { return fabs(_wf - val._wf); }
	static int FindNbCenterElement(vector <tpRiverGHUnit> &vect);

	int _nbH;
	float _wf, _weight;
};

//#define DIST_DAT_SQR
#define RIVERBANK_CN_HANDS		10

class clRiverDat
{
public:
	size_t	clRiverDat::SizeInFile() { return sizeof(_stack)+SizeVectFile(_vectUnit[0]) + SizeVectFile(_vectUnit[1]) + SizeVectFile(_vectUnit[2]); }
	int 	clRiverDat::WriteFile(int handle)
	{
		_write(handle, &_stack, sizeof(_stack));
		WriteVectFile(handle, _vectUnit[0]);
		WriteVectFile(handle, _vectUnit[1]);
		WriteVectFile(handle, _vectUnit[2]);
		return 0;
	}
	int 	clRiverDat::ReadFile(int handle)
	{
		_read(handle, &_stack, sizeof(_stack));
		ReadVectFile(handle, _vectUnit[0]);
		ReadVectFile(handle, _vectUnit[1]);
		ReadVectFile(handle, _vectUnit[2]);
		return 0;
	}
	//int		CnHand(int nbPl) { return (int)_vectUnit[nbPl].size(); }
	//int		NbHand(int nbPl, int nb) { return _vectUnit[nbPl][nb]._nbH; }
	//float	Weight(int nbPl, int nb) { return _vectUnit[nbPl][nb]._weight; }
	float	Distance(clRiverDat &dat);
	float	DistanceStack(clRiverDat &dat) { float ds = (dat._stack > _stack) ? (dat._stack - _stack) / dat._stack : (_stack - dat._stack) / _stack; return ds; }

	//float	DistanceGHNew(clRiverDat &dat);
	//float	WeightRD(int nbPl);

	float _stack;
	vector <tpRiverGHUnit> _vectUnit[3];
};

class clRootRiverTree : public clRoot
{
public:
	void	operator =  (clRootRiverTree &root);
	void	operator =  (clRoot &root);
	size_t	SizeInFile();
	int 	WriteFile(int handle);
	int 	ReadFile(int handle);
	int 	ReadFileRoot(int handle);
	//int		CnHand(int nbPl) { return (int)_vectHand[nbPl].size(); }
	//int		NbHand(int nbPl, int nb) { return _vectHand[nbPl][nb]; }
	void	CalcTree(size_t cnCalc);
	void	CalcTreeKOpp(size_t cnCalc);
	void	CalcTreeOnevsGroup(size_t cnCalc);
	void	CalcTreeOnevsGroupTest(size_t cnCalc);
	void	CalcTreeGroupvsGroup(size_t cnCalc);
	void	CreateTreeNL();
	static void	CalcRiverForce(clRoot *root, int *force);
	void	CalcForce(int *force)	{ CalcRiverForce(this, force); }
	void	CalcForce() { CalcForce(_force); }
	//clDoubleCP ResEV();
	double	TreeVsTree(clHoldemTree &tree);
	void	SetWeightNodeStrat();
	void	CalcAbsWeight(vector <float> *weight);
	tpFloat AbsAntistrategy();
	//tpFloat QAntistrategy();
	void	CalcEV(vector <float> *ev);
	void	CompactGH(tpFloat minW, int cnMin, clRiverDat *dat = NULL);
	void	ReCalcParam(int cnHand, clRiverDat *dat = NULL);
	void	CreateRiverDat(clRiverDat &dat);
	void	CreateRiverDatMin(clRiverDat &dat, clRiverDat *datIn=NULL);
	void	FindMinParam(clRiverDat &datIn);

	int _force[ALL_CN_HAND];
private:
	tpFloat		CalcNeshA(clHoldemTree *node, int nbHero, int nbClsHero, tpFloat kH, vector <tpFloat> &kOpp);
	void		CalcNeshATest(clHoldemTree *node, int nbHero, int nbClsHero, tpFloat kH, vector <tpFloat> &kOpp, vector <tpFloat> &ev);
	tpFloat		CalcNeshA(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <tpFloat> &ev);
	tpFloat		CalcAntiStratEV(clHoldemTree *node, int nbHero, int nbHand, clHandsGroupEx &gh);
	tpFloat		NodeResult(clHoldemTree *node, int nbHero, vector <tpFloat> &kH, vector <tpFloat> &kOpp, vector <tpFloat> &ev);
	tpFloat		AddResEVToNode(clHoldemTree *node, int nbHero, int cnB, vector <tpFloat> &resEV, tpFloat *w, vector <tpFloat> *evCur, vector <tpFloat> &kH, vector <tpFloat> &kOpp);
};

void	CreateSitRiver(clSitHoldem &sit, int stack, int pot);
//inline	double  CurStackToPot(clSitHoldem &sit) { sit.CloseBank(); return (double)sit.EffStackAfterCall() / sit.TotalPot(); }
//tpFloat DistanceEV(clRootRiverTree &root, clFloatMatrix2 &matr, clFloatMatrix2 &ms);
//tpFloat DistanceEV(clRootRiverTree &root, vector <float> *ev, vector <float> *ev1);
void	InitRiverDatDat();
int *GetNbRiverDatDat(int nb);
void	CreateRiverDatRoot(clRoot &root, clRiverDat &dat);
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
