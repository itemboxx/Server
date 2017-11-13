//---------------------------------------------------------------------------

#ifndef ClastersBoard_H
#define ClastersBoard_H
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//#include "C:\home\holdem\Common\sitHoldem.h"
#include "..\..\..\card\naborCards.h"
#include "..\..\..\Util\Clasters.h"

#ifdef COMP2
#define	PATH_CLASTERS				"d:\\home\\bin\\BoardHands\\Clasters\\"
#elif defined COMP_C
#define	PATH_CLASTERS				"c:\\home\\bin\\BoardHands\\Clasters\\"
#else
#define	PATH_CLASTERS				"d:\\data\\Clasters\\"
#endif
#define	FILE_CLASTERS_FLOP			"d:\\home\\bin\\BoardHands\\Clasters\\ClastersFlop.bin"
#define	FILE_CLASTERS_TURN			"d:\\home\\bin\\BoardHands\\Clasters\\ClastersTurn.bin"
#define	FILE_CLASTERS_RIVER			"d:\\home\\bin\\BoardHands\\Clasters\\ClastersRiver.bin"
#define	FILE_CLASTERS_TEMP_TURN			"d:\\home\\bin\\BoardHands\\Clasters\\ClastersTempTurn.bin"
#define	FILE_CLASTERS_TEMP_RIVER		"d:\\home\\bin\\BoardHands\\Clasters\\ClastersTempRiver.bin"

double	DistFlopHand(int nb1, int nb2);
double	DistTurnHand(int nb1, int nb2);
double	DistRiverHand(int nb1, int nb2);

struct tpFlopHandUnit
{
	double Distance(tpFlopHandUnit val)		{ return DistFlopHand(_nbBH, val._nbBH); }
	void	operator = (int val)			{ _nbBH = val; }
	void	SetEmpty()						{ _nbBH = -1; }
	bool	IsEmpty()						{ return _nbBH == -1; }
//	static	void	Get2Centers(vector <tpFlopHandUnit> &vect, int &v1, int &v2);
	operator int()	{ return _nbBH; }

	int _nbBH;
};

struct tpTurnHandUnit
{
	double Distance(tpTurnHandUnit val)		{ return DistTurnHand(_nbBH, val._nbBH); }
	void	operator = (int val)			{ _nbBH = val; }
	void	SetEmpty()						{ _nbBH = -1; }
	bool	IsEmpty()						{ return _nbBH == -1; }
	//	static	void	Get2Centers(vector <tpTurnHandUnit> &vect, int &v1, int &v2);
	operator int()	{ return _nbBH; }

	int _nbBH;
};

struct tpRiverHandUnit
{
	double Distance(tpRiverHandUnit val)		{ return DistRiverHand(_nbBH, val._nbBH); }
	void	operator = (int val)			{ _nbBH = val; }
	void	SetEmpty()						{ _nbBH = -1; }
	bool	IsEmpty()						{ return _nbBH == -1; }
	//	static	void	Get2Centers(vector <tpTurnHandUnit> &vect, int &v1, int &v2);
	operator int()	{ return _nbBH; }

	int _nbBH;
};

int		FindCnCls(vector <int> &vect);
template <class T> void	CreateCls(clClasters<T> &cls, vector <int> &vect)
{
	int dim = FindCnCls(vect);
	cls.Resize(dim);
	dim = vect.size();
	for (int i = 0; i < dim; i++)
	{
		int nbCls = vect[i];
		T val;val = i;
		VectAddCheck<T>(cls[nbCls], val);
	}
}

void	CalcFlopHandCls(int cnCls);
void	CalcTurnHandCls(int cnCls);
void	CalcTurnHandClsThread(clClasters <int> &cls,int cnCls,double dist);
void	CalcRiverHandCls(int cnCls);
void	CalcRiverHandClsThread(clClasters <int> &cls, int cnCls, double dist);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
