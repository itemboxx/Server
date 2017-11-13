//---------------------------------------------------------------------------

#ifndef clHandsCompareH
#define clHandsCompareH

#include "C:\home\card\pokGame.h"
#include "C:\home\holdem\Common\HandsGroup.h"
#include "C:\home\holdem\Common\SeveralHandCmp.h"
//---------------------------------------------------------------------------
#define CN_HANDS_COMPARE (ALL_CN_HAND*ALL_CN_HAND)
#define SIZE_HANDS_COMPARE  (CN_HANDS_COMPARE* sizeof(clRezCompare))

//#define CALCULATE_MODE
#define START_I     0
#define FINAL_I     ALL_CN_HAND
#define FINAL_FILE_HANDS_COMPARE "comp0.tmp"
//#define FILE_LIST_HANDS "c:\\WORK\\HoldemTurn\\HandsComp\\listHand.bin"
#define FILE_LIST_HANDS "C:\\WORK\\Bin\\ListHand.bin"

enum    enWin {WIN_WIN=0,WIN_EQ,WIN_LOSS};

class clRezCompare
{
public :
    clRezCompare();
    inline void AddWin() {_cnWin++;}
    inline void AddLose() {_cnLose++;}
    inline void AddEq() {_cnEq++;}
    inline  int     Sum()           {return _cnWin+_cnLose+_cnEq;}
    inline  double  WeightWin()     {int sum=Sum();return (sum==0)? -1 : double(_cnWin+_cnEq/2)/sum;}

    unsigned int _cnWin, _cnLose, _cnEq;
};

struct stHandsCompare
{
    clRezCompare _arr[ALL_CN_HAND][ALL_CN_HAND];
};

struct tpWin2
{
    inline  tpWin2()    {Clear();}
    inline  void    Clear()  {_win=_loss=_eq=_weight=0;}
    inline  void    Add(clRezCompare rez)   {_win+=rez._cnWin;_loss+=rez._cnLose;_eq+=rez._cnEq;_weight++;}
    inline  void    Norm()   {double sum=_win+_loss+_eq;_win/=sum;_loss/=sum;_eq/=sum;}
    inline  double  WinEqWithWeigth()   {return (_win+_eq/2)*_weight;}
	inline  double  WinEq()             { return (_win + _eq / 2); }
	inline  double  LossEq()             { return (_loss + _eq / 2); }
	void    operator *=(double x);
	void    operator +=(tpWin2 w);
	void    operator =(clRezCompare res)
	{
		_weight = res.Sum();
		if (_weight > DOUBLE_0)
		{
			_win = res._cnWin / _weight;
			_loss = res._cnLose / _weight;
			_eq = res._cnEq / _weight;
		}
		else
		{
			_win = _loss = _eq=0;
		}
	}

    double _win,_loss,_eq,_weight;
};

struct stHandsCompare169
{
	tpWin2 _arr[CN_HAND][CN_HAND];
};
struct stHandsCompareAll
{
	tpWin2 _arr[ALL_CN_HAND][ALL_CN_HAND];
};

class clHandsCompare
{
public :
    clHandsCompare();
    ~clHandsCompare();
    void WriteFileAll(int handle);
    void WriteFile(int handle, int i, int j);
    void ReadFileAll(int handle);
    inline clRezCompare RezCompare(int nbHand1, int nbHand2)
            { return _handsC->_arr[nbHand1][nbHand2];}
    void WrRezCompare(clRezCompare rez, int nbHand1, int nbHand2);
    enWin   Compare(clCard *card1,clCard *card2);


//private:
    stHandsCompare * _handsC;
	stHandsCompareAll * _handsCAll;
	unsigned int _cnCompare;
    stHandsCompare169 *_handsC169;
};
class clListCompare
{
public:
    clListCompare();
    ~clListCompare();

    inline  int UrHand(clCard *cards)   {clHand *ptr=(clHand *)cards;return _urH[ptr->NbHand()];}
    inline  int UrHand(int nbHand)      {return _urH[nbHand];}
    int UrHand(double ur);
    inline  double UrHandD(clCard *cards) {return _list._ur[UrHand(cards)];}
    inline  double UrHandD(int nbHList)        {return _list._ur[nbHList];}

    tpListHand _list;
    int _urH[CN_HAND];
    stHandsCompare169 *_listCompare;
};

clRezCompare CompareHands(clHand h1, clHand h2);
clRezCompare CompareHands4(clHand h1, clHand h2);
clRezCompare CompareHands3(clHand h1, clHand h2);
clRezCompare CompareHands2(clHand h1, clHand h2);
void    CompareHands169();
void    CompareHandsCalcList();
void    CompareHandsCalcListList();

extern clHandsCompare glHandCmp;
extern clListCompare glListCmp;

void CalcResCompareFullGroup();

bool    CompareHandsInit();
inline  double  HCompUrHand(int n) {return glListCmp._list._ur[n];}
inline  tpWin2  HCompListList(int nb1,int nb2)  {return glListCmp._listCompare->_arr[nb1][nb2];}
inline  tpWin2  HCompWinHand169(int nb1, int nb2)    { return glHandCmp._handsC169->_arr[nb1][nb2]; }
inline  tpWin2  HCompWinHandAll(int nb1, int nb2)    { return glHandCmp._handsCAll->_arr[nb1][nb2]; }
tpWin2 CCCompareHands(int nbHand, const clImproveGroupHands &gr2, double &k0);
tpWin2 CCCompareHandsWithoutK(int nbHand, const clImproveGroupHands &gr2);
double  CCComparePlayersEQ(const clImproveGroupHands &gr1, const clImproveGroupHands &gr2);
tpWin2  CCComparePlayers(const clImproveGroupHands &gr1,const clImproveGroupHands &gr2);
stVerManyHands  CCComparePlayers3Pl(const clImproveGroupHands &gr1,const clImproveGroupHands &gr2,const clImproveGroupHands &gr3);
stVerManyHands  CCCCompare3PlFullGH(const clImproveGroupHands &gr1,const clImproveGroupHands &gr2);
//---------------------------------------------------------------------------
#endif
