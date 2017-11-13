//---------------------------------------------------------------------------

#ifndef clCostStackH
#define clCostStackH
//---------------------------------------------------------------------------
#include "C:\home\holdem\Common\sitHoldem.h"
#include "C:\home\holdem\Common\ArrCnPlayer.h"

//#define CNPRIZE         3

typedef clDoubleCP (*tpPtrFuncCostStack)(int cnPl,int bb,int *stacks);

double CostStackPrice(int cnPrize,double * prize,int cnPl,int *stack,int nb);

class clCostStack
{
public:
    clCostStack();
    inline  clCostStack(int cnPl,double *prize)     {SetPrize(cnPl,prize);_func=NULL;}
    ~clCostStack();
    void    SetPrize(int cnPl,double *prize);
    double  PrizeFond();
    inline  double  CostPlace(int m)    {return (m>_cnPrize)? 0:_prize[m-1];}
    double  CostStack(int cnPl,int nbPl,int *stack);
    clDoubleCP  CostStack(int cnPl,int *stack);
    double  CostStack(int cnPl,int nbPl,int *stack,int *startStack,int bb);
    clDoubleCP  CostStack(int cnPl,int *stack,int *startStack,int bb);
    inline  void    SetFunc(tpPtrFuncCostStack func)         {_func=func;}
    double  CostStackBeginGame(clGameHistory *hist,int nb);
    double  CostStackEndGame(clGameHistory *hist,int nb);
	double  CostStack(clSitHoldem *table, int nb);
    tpPtrFuncCostStack Func()       {return _func;}
private:
    int _cnPrize;
    double *_prize;
    tpPtrFuncCostStack _func;
//    double _delta[CN_PLAYER][CN_PLAYER];
};

extern clCostStack glCostStackICM,glCostStackTS;
//extern clCostStack glCostStackPS6Turbo;
int     CostStackCalcPlace(int cnPl,int *st0,int *st1,int myNb);
//---------------------------------------------------------------------------
#endif
