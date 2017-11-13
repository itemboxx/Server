//---------------------------------------------------------------------------

#ifndef HandH
#define HandH

#include "c:\Home\Card\Card.h"

//---------------------------------------------------------------------------
#define CN_HAND         169
#define ALL_CN_HAND     (52*51/2)

#define AA_NB_HAND      C_A
#define KK_NB_HAND      C_K
#define QQ_NB_HAND      C_Q
#define JJ_NB_HAND      C_J
#define TT_NB_HAND      C_T
#define AKS_NB_HAND     90
#define AK_NB_HAND      168

class clHand
{
public:
    inline void SetHand(clCard c1,clCard c2)    {_cards[0]=c1;_cards[1]=c2;}
    void SetHand(int nb169,enSuit s1=S_C,enSuit s2=S_D);
    void SetHandAll(int nb);
    clHand  SetHand(enFace c2,enFace c1,bool suit=false);
    inline  void    SetHand(int c1,int c2)      {_cards[0]=CardNB(c1);_cards[1]=CardNB(c2);}
    int     NbHand();
    int     NbHandAll();
    char    *NameHand(char *str);
    static int  CnHand(int nb);
    static int  NbHand(char *str);
    static char *NameHand(int nb,char *str);
	inline  bool operator ==(clHand hand)    { return ((_cards[0] == hand._cards[0] && _cards[1] == hand._cards[1]) || (_cards[1] == hand._cards[0] && _cards[0] == hand._cards[1])); }
    bool    operator > (clHand hand);
    bool    operator < (clHand hand);
	clCard	&operator [] (int nb)		{ return _cards[nb]; }
    inline  enFace    PocketPair()      {return (_cards[0].face==_cards[1].face)? _cards[0].face : C_UNDEF;}
    inline  enSuit    SuitCards()       {return (_cards[0].suit==_cards[1].suit)? _cards[0].suit : S_UNDEF;}
    inline  bool    IsCard(enFace f0)   {return (_cards[0].face==f0 || _cards[1].face==f0);}
    int    FaceDistance();
    enFace OtherCard(enFace f1);
    inline  bool    IsAnyBrodway()      {return _cards[0].face>=C_T && _cards[1].face>=C_T;}
    inline  enFace  HighCard()          {return (_cards[0].face>_cards[1].face)? _cards[0].face : _cards[1].face;}

    clCard  _cards[2];
    static const int _cnAllHand=52*51/2;
};

struct  tpListHand
{
    inline  tpListHand()    {}
    tpListHand(char *nameFile);
    int		LoadFromFile(char *nameFile);
    bool Eq(int nbH1,int nbH2);
    double  UrDHand(int nbH);
    double  UrDHand(clHand *hand);
    int     NbInArrForNbHand(int nbHand);
    int     NbInArrForUr(double ur);

    int _nbH[CN_HAND];
    double _ur[CN_HAND];
};

struct tpHand
{
	tpHand()	{}
	explicit inline tpHand(int nbAll);
	tpHand(tpCard c1, tpCard c2)	{ SetHand(c1, c2); }
	tpHand(clCard c1, clCard c2)	{ SetHand(c1.Number(), c2.Number()); }
	void    SetHand(tpCard c1, tpCard c2);
	void	SetHandAll(int nb);
	int     NbHand();
	int     NbHandAll()				{ return  (_cards[0]._card>_cards[1]._card) ? _cards[0]._card * (_cards[0]._card - 1) / 2 + _cards[1]._card : _cards[1]._card * (_cards[1]._card - 1) / 2 + _cards[0]._card; }
	void	operator = (clHand hand);
	tpCard	&operator [] (int nb)		{ return _cards[nb]; }
	char    *NameHandAll(char *str);
	bool	IncludeCards(int cn, tpCard *cards);
	bool	IncludeCardsHand(tpHand &hand)		{ return (_cards[0] == hand[0] || _cards[0] == hand[1] || _cards[1] == hand[0] || _cards[1] == hand[1]); }

	tpCard _cards[2];
	static const int _cnAllHand = 52 * 51 / 2;
};
//---------------------------------------------------------------------------
extern tpListHand glListHand;
class clHandDefArr
{
public:
	clHandDefArr();

	byte _arrCross[ALL_CN_HAND][ALL_CN_HAND];
	tpHand _arrHand[ALL_CN_HAND];
	double _arrMultCross169[CN_HAND][CN_HAND];
	int _cnAll[CN_HAND];
	int _nb169[ALL_CN_HAND];
};

extern	clHandDefArr glHandArr;

int     NbHand(enFace f1,enFace f2,bool suit);
clHand  CrHand(enFace f1,enSuit s1,enFace f2,enSuit s2);
inline  double  HandWeight(int nb)  {return (double)clHand::CnHand(nb)/clHand::_cnAllHand;}
double  HandWeight(int nb,int nb0);  //вероятность хэнда nb0 когда уже есть хэнд nb
double  HandWeight(int cnCard,clCard *cards,int nbH);
clHand  HandCreateHand(int nbHand169,int i);
int     NbHandAll(int c1, int c2);
inline	bool	HandCrossHandNB(int nbH0, int nbH1)		{ return bool(glHandArr._arrCross[nbH0][nbH1]); }
inline	double	HandMultCrossHand169(int nb1, int nb2) { return glHandArr._arrMultCross169[nb1][nb2]; }
inline	tpHand  HandQiuckCreate(int nb)							{ return glHandArr._arrHand[nb]; }
inline	char *	HandName(int nb, char *str) { tpHand hand(nb); return hand.NameHandAll(str); }
inline tpHand::tpHand(int nbAll)
{
	*this = glHandArr._arrHand[nbAll];
}
inline	int	HandCnHand169(int nb169) { return glHandArr._cnAll[nb169]; }
inline	int HandAllGetNb169(int nbAll) { return glHandArr._nb169[nbAll]; }
//---------------------------------------------------------------------------

#endif
