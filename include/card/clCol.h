//---------------------------------------------------------------------------

#ifndef clColH
#define clColH
//---------------------------------------------------------------------------
#include <random>
#include "../util/errMessage.h"
#include "Card.h"

#define CN_CARD_COL     (CN_CARD_FACE*CN_CARD_SUIT)

class clCol;
struct tpBooleanDeck
{
	tpBooleanDeck()				{ SetDeck(true); }
    void    SetDeck(bool b0)    { memset(this,b0,CN_CARD_COL); }
    int CnCard()
    {
        int res=0;
        for(int i=0;i<CN_CARD_COL;i++)
            if(_card[i]) res++;
        return res;
    }
    void    operator = (clCol &deck);
    bool &operator[] (int nb)   {return _card[nb];}
	void    Skip(clCard card)   { _card[card.Number()] = false; }
	void    Skip(tpCard card)   { _card[card._card] = false; }
	clCard	GiveCard(int nb)
	{
		for (int i = 0; i < CN_CARD_COL; i++)
			if (_card[i])
				if (nb == 0)
				{
					_card[i] = false;
					return CardNB(i);
				}
				else
					nb--;
		return CARDUNDEF;
	}
	tpCard	GiveRandomCard(int cnCard)
	{
		int r = random(cnCard);
/*		for (int i = 0; i <= r; i++)
			if (!_card[i])
				r ++ ;
		_card[r] = false;
		return r;*/

		for (int i = 0; i < CN_CARD_COL; i++)
			if (_card[i])
				if (r == 0)
				{
					_card[i] = false;
					return i;
				}
				else
					r--;
		return -1;
	}

	clCard	GiveCardWD(int nb)
	{
		for (int i = 0; i < CN_CARD_COL; i++)
			if (_card[i])
				if (nb == 0)
				{
					return CardNB(i);
				}
				else
					nb--;
		return CARDUNDEF;
	}
    bool _card[CN_CARD_COL];
};

class clCol
{
public:
	clCol();
	clCard  &operator [] (int nb)  { return card[nb]; }
	void    operator = (tpBooleanDeck &deck);
	void    New();
	void    New(int cn, clCard *c0);
	void    Shufle();
	void    ShufleOldCol();
	clCard  GiveCard();
	inline  clCard  *Cards()        { return card + ptrC; }
	void    Skip(clCard c0);
	void    Skip(int nb);
	inline  void    ReBuid()    { cnCard += ptrC; ptrC = 0; }
	inline  int CnCards()       { return cnCard; }
	inline void ShiftCardToBegin()  { memcpy(card, card + ptrC, cnCard*sizeof(clCard)); ptrC = 0; }
	int     NbCardInArr(clCard card);
	int     MovePtr(int cnC);
	inline  void    AddCard(clCard card0)        { card[cnCard++] = card0; }
private:
	int cnCard, ptrC;
	clCard card[CN_CARD_COL];
};

class clDeck
{
public:
	clDeck()		{ New(); }
	tpCard  &operator [] (int nb)  { return _card[nb]; }
//	void    operator = (tpBooleanDeck &deck);
	void    New();
//	void    New(int cn, clCard *c0);
	void    Shufle();
	void    ShufleOldCol();
	tpCard  GiveCard();
	inline  tpCard  *Cards()        { return _card + _ptrC; }
	void    Skip(tpCard c0);
	void    Skip(int nb);
	inline  void    ReBuid()    { _cnCard += _ptrC; _ptrC = 0; }
	inline  int CnCards()       { return _cnCard; }
	inline void ShiftCardToBegin()  { memcpy(_card, _card + _ptrC, _cnCard*sizeof(tpCard)); _ptrC = 0; }
//	int     NbCardInArr(tpCard card);
//	int     MovePtr(int cnC);
//	inline  void    AddCard(clCard card0)        { card[cnCard++] = card0; }
private:
	byte _cnCard, _ptrC;
	tpCard _card[CN_CARD_COL];
};
//---------------------------------------------------------------------------
#endif
