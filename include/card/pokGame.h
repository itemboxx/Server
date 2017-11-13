//---------------------------------------------------------------------------
#ifndef pokGameH
#define pokGameH

#include "Card.h"
#include "Hand.h"
//---------------------------------------------------------------------------

enum enGame : unsigned char{ GM_0 = 0, GM_2, GM_22, GM_3, GM_ST, GM_FL, GM_32, GM_4, GM_FS, GM_UNDEF };

#define CN_POK_GAME     GM_UNDEF

class clGamePoker
{
public :
    clGamePoker();
    clGamePoker(enGame gm);
    bool operator > (clGamePoker gm);
    bool operator < (clGamePoker gm);
    bool operator == (clGamePoker gm);
	int		Compare(clGamePoker &gm)
	{
		if (*this > gm)
			return 1;
		else if (*this < gm)
				return -1;
		else return 0;
	}
    void    GameHoldem(clCard *card2,clCard *card5);
    void    GameHoldemQ(clCard *card2,clCard *card5);
    void    GameHoldem(clCard *card2,clCard *card5,int cn5);
    inline void GameHoldem(clHand hand,clCard *card5)    {GameHoldem(hand._cards,card5);}
    inline void GameHoldemQ(clHand hand,clCard *card5)   {GameHoldemQ(hand._cards,card5);}
    inline void GameHoldem(clHand hand,clCard *card5,int cn5)    {GameHoldem(hand._cards,card5,cn5);}
    void    GameTable(int cn,clCard *card);

    enGame  game;
    enFace  face[5];
};

struct stGameFace
{
    stGameFace()        {Clear();}
    double &operator[] (int nb)      {return _weight[nb];}
    void    operator += (stGameFace &val);
    void    operator -= (stGameFace &val);
    void    operator *= (double val);
    void    operator /= (double val);
    void    operator =  (double val);
    void    Clear();
    double Weight();
    bool    CheckNegativeVal();

    double _weight[CN_CARD_FACE],_sum;
};

class  clSetGame
{
public:
    clSetGame()             {Clear();}
    void    Clear();
    stGameFace &operator[] (enGame gm)      {return _arr[gm];}
    void    operator = (const clSetGame &val);
    void    operator = (clGamePoker val);
    void    operator += (clSetGame val);
    void    operator += (clGamePoker val);
    void    operator *= (double val);
    bool    NormWeight(double sum=1);
    double  Weight();

private :
    stGameFace _arr[GM_UNDEF];
};

clGamePoker  GameWhatGame(clCard *card);
clGamePoker  GameWhatGame(clCard c1,clCard c2,clCard c3,clCard c4,clCard c5);
clGamePoker  GameWhatGame(clCard *card,int cnCard);
clGamePoker GamePokerMinGame();
//AnsiString  GamePokerGameToString(enGame g0);
enGame      GamePokerStringToGame(char *str);

clGamePoker  GameWhatGame(tpCard *card, int cnCard);

double ProbHiFace(stGameFace &gf1,stGameFace &gf2);

extern clGamePoker  POKER_MIN_GAME;
//---------------------------------------------------------------------------
#endif
