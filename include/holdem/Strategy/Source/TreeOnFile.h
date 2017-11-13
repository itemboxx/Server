//---------------------------------------------------------------------------

#ifndef TreeOnFileH
#define TreeOnFileH
//---------------------------------------------------------------------------
using namespace std;

#include <vector>
#include "Strategy.h"

class clAtrTreeOnFile
{
public:
	int		WriteFile(int handle)		{ return _write(handle, this, sizeof(clAtrTreeOnFile)); }
	int		ReadFile(int handle)		{ return _read(handle, this, sizeof(clAtrTreeOnFile)); }
	void	Clear()						{ _nbRec = -1; }

	int _nbRec;
	tpDis _dis;
};

class clAtrStratHoldem
{
public:
	void operator = (clAtrTreeStrat &atr);
	int		WriteFile(int handle);
	int		ReadFile(int handle);
	tpFloat0 *GetStratWeight(int nbCls)				{ return &_param[nbCls*_cnB]; }
	tpFloat0 *GetStratWeight(int cnB, clSitHoldem &sit);
//	int		GetNbCls(tpHand hand, tpCard *board);

//	clSitHoldem _sitA;
	byte _cnB;
	vector <short> _vectInd;
	double _statCn;
	vector <int> *_ptrIndex;
	vector <tpFloat0> _param;
};

struct stNodeW0
{
	int _nbTree;
	void	*_ptr;
	double	_weight;
};
class clTreeOnFile : public clTree < clAtrTreeOnFile >
{
	friend class clTreeStrat;
public:
	void	operator = (clTreeStrat &tree);
	//int AddBranch();
	clTreeOnFile *Branch(int nb)				{ return (clTreeOnFile *)clTree < clAtrTreeOnFile >::Branch(nb); }
	bool	EndedNode()						{ return CnBranch() == 0; }
	clTreeOnFile *FindSitNode(clSitHoldem *sit);
	void		FindSitNode(clSitHoldem *sit, vector<stNodeW0> &vect);
	clTreeOnFile *FindBranch(tpDis dis, int bb);
	tpDis	&GetDis()						{ return AtrTree()->_dis; }
	void	FindBranchBetween(tpDis dis, int &nb1, int &nb2, double &w1, double &w2);

private:
	clTreeOnFile *FindSitNode(clSitHoldem &sit, int ind, int nbA);
	bool		FindSitNode(clSitHoldem &sit, int ind, int nbA, vector<stNodeW0> &vect, double k);

};

//int		SaveTreeToFile(CStringA &ss, clTreeStrat &tree);
int		SaveTreeToFile(char *str, clStrategy &strat);
//---------------------------------------------------------------------------------------------------------------------------------------------------
#endif
