//---------------------------------------------------------------------------
#ifndef CardH
#define CardH

//#include <mem.h>
//#include <string>
#include "c:\Home\Util\errMessage.h"
#include "c:\home\Util\WorkStr.h"

//---------------------------------------------------------------------------

#define CN_CARD_FACE        13
#define CN_CARD_SUIT        4
#define CN_ALL_CARDS        (CN_CARD_SUIT*CN_CARD_FACE)

enum enFace : unsigned char { C_2 = 0, C_3, C_4, C_5, C_6, C_7, C_8, C_9, C_T, C_J, C_Q, C_K, C_A, C_UNDEF };
enum enSuit : unsigned char { S_C = 0, S_D, S_H, S_S, S_UNDEF };

char FaceCh(enFace face);
char SuitCh(enSuit suit);

struct tpCard;
class clCard
{
public :
    inline char FaceChar() {return FaceCh(face);}
    inline  bool operator > (clCard card)    {return (face==card.face)? suit>card.suit : face>card.face;}
    friend inline bool operator < (clCard card1,clCard card2)    {return (card1.face==card2.face)? card1.suit<card2.suit : card1.face<card2.face;}
    inline  bool operator !=(clCard card)    {return (face!=card.face || suit!=card.suit);}
    inline  bool operator ==(clCard card)    {return (face==card.face && suit==card.suit);}
	void	operator = (tpCard &c);
    inline  int Number()    {return face*CN_CARD_SUIT+suit;}
    void    Name(char *name);
	clAnsiString Name();
    inline  bool    Correct()       {return face>=C_2 && face<=C_A && suit>=S_C && suit<=S_S;}
	void	SetForNumber(int nb)	{ face = (enFace)(nb / CN_CARD_SUIT); suit = (enSuit)(nb%CN_CARD_SUIT); }

    enFace face;
    enSuit suit;
};

struct tpCard
{
public:
	tpCard()				{ _card = 0; }
	tpCard(int nb)			{ _card = nb; }
	tpCard(clCard card)		{ _card = card.Number(); }
	enFace	GetFace()		{ return (enFace)(_card >> 2); }
	enSuit	GetSuit()		{ return (enSuit)(_card & 3); }
	void	SetFace(enFace f)		{ _card = (f << 2) + (_card & 3); }
	void	SetSuit(enSuit s)		{ _card = (_card & 0x3C) + s; }
	operator int()					{ return _card; }
	void	operator = (int nb)		{ _card = nb; }
	void	operator = (clCard nb)		{ _card = nb.Number(); }
	bool	operator > (tpCard nb)		{ return _card > nb._card; }
	bool	operator == (tpCard nb)		{ return _card == nb._card; }
	bool	operator < (tpCard nb)		{ return _card < nb._card; }
	int		Number()					{ return _card; }

	byte _card;
};

inline  clCard  Card(enFace f, enSuit s)    { clCard c0; c0.face = f; c0.suit = s; return c0; }
inline  clCard  CardNB(int nb)    {clCard c0;c0.face=(enFace)(nb/CN_CARD_SUIT);c0.suit=(enSuit)(nb%CN_CARD_SUIT);return c0;}
void    CardStraight(int cn,clCard *cards);
clCard  CardForName(char *name);
bool    CardCheckCard(clCard card);
enFace  CardFaceFromChar(char face);
inline  int CardNumber(enFace face,enSuit suit)     {return face*CN_CARD_SUIT+suit;}

class clBitArrFace
{
public:
    bool    GetVal(enFace i);
    void    SetVal(enFace i,bool v);
private:
    int _mask;
};

extern clCard CARDUNDEF;

class clCardMap
{
public:
    void    Clear();
    void    Full();
    inline  int &operator[](int nb)     {return _arrF[nb];}
    clCard  ScipCard(enFace f0);
    inline  bool    AddCard(enFace f0,enSuit s0)    {if(!_card[f0][s0]){_card[f0][s0]=true;_arrF[f0]++;return true;}else return false;}
    bool    ScipCard(clCard card);
    void    AddCard(clCard card);

    bool _card[CN_CARD_FACE][CN_CARD_SUIT];
    int _count,_arrF[CN_CARD_FACE];
};
clCard  WSReadCard(char *word);
//---------------------------------------------------------------------------
#endif
