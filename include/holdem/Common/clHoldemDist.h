//---------------------------------------------------------------------------

#ifndef clHoldemDistH
#define clHoldemDistH
//---------------------------------------------------------------------------
#include "C:\home\holdem\Common\History.h"
#include "vector"

struct tpAtrHD
{
	tpDis _dis;
	tpFloat *_val;
};

class clHoldemDist
{
public:
	clHoldemDist();
	~clHoldemDist();
	void	Create(int dim, int dimDist);
	tpFloat Dist(clHoldemDist &atr);
	tpAtrHD &operator [] (int nb);
private:
	std::vector <tpAtrHD> _vect;
	int _dimDist;
};

//---------------------------------------------------------------------------
#endif
