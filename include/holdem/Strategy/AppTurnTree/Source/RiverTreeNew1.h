//---------------------------------------------------------------------------

#ifndef RiverTreeH
#define RiverTreeH
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "C:\home\holdem\Common\sitHoldem.h"
#include "C:\home\Holdem\Common\HoldemTree.h"
#include "c:\Home\Util\Matrix.h"

class clRootRiverTree;

class tpRiverGHUnit
{
public:
	float	Distance(tpRiverGHUnit &val) { return fabs(_wf - val._wf); }

	int _nbH;
	float _wf, _weight;
};

//#define DISTANCE_SQRT_EV

class clRiverDat
{
public:
	size_t	clRiverDat::SizeInFile() { return SizeVectFile(_vectUnit[0]) + SizeVectFile(_vectUnit[1]) + SizeVectFile(_vectUnit[2]); }
	int 	clRiverDat::WriteFile(int handle)
	{
		WriteVectFile(handle, _vectUnit[0]);
		WriteVectFile(handle, _vectUnit[1]);
		WriteVectFile(handle, _vectUnit[2]);
		return 0;
	}
	int 	clRiverDat::ReadFile(int handle)
	{
		ReadVectFile(handle, _vectUnit[0]);
		ReadVectFile(handle, _vectUnit[1]);
		ReadVectFile(handle, _vectUnit[2]);
		return 0;
	}
	int		CnHand(int nbPl) { return _vectUnit[nbPl].size(); }
	int		NbHand(int nbPl, int nb) { return _vectUnit[nbPl][nb]._nbH; }
	float	Weight(int nbPl, int nb) { return _vectUnit[nbPl][nb]._weight; }
	float	DistanceGH(clRiverDat &dat);

	vector <tpRiverGHUnit> _vectUnit[3];
};

class clRootRiverTree : public clGentlemanSet
{
public:
	void	operator =  (clRootRiverTree &root);
	size_t	SizeInFile();
	int 	WriteFile(int handle);
	int 	ReadFile(int handle);
	void	CalcTree(size_t cnCalc);
	void	CreateTreeNL();
	void	CalcIndexHands();
	void	CalcForce();
	//void	CalcAbsWeight(vector <float> *weight);
	//tpFloat AbsAntistrategy();
	//void	CalcEV(vector <float> *ev);
	//void	CompactGH(tpFloat minW, int cnMin, clRiverDat *dat=NULL);
	void	CreateRiverDat(clRiverDat &dat);
	int		CnHand(int nbPl) { return _vectHand[nbPl].size(); }
	int		NbHand(int nbPl, int nb) { return _vectHand[nbPl][nb]; }

	clHoldemTree _tree;
	vector <int> _vectHand[3];
	int _force[ALL_CN_HAND];
private:

};

void	CreateSitRiver(clSitHoldem &sit, int stack, int pot);
inline	double  CurStackToPot(clSitHoldem &sit, int nb) { sit.CloseBank(); return (double)sit.PlayerMoney(nb) / sit.TotalPot(); }
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
