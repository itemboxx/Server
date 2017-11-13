//---------------------------------------------------------------------------
#pragma hdrstop

#include <stdlib.h>
#include "clCol.h"

//---------------------------------------------------------------------------
void    tpBooleanDeck::operator = (clCol &deck)
{
    SetDeck(false);
    for(int i=0;i<deck.CnCards();i++)
        _card[deck[i].Number()]=true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
clCol::clCol()
{
    cnCard=ptrC=0;
}
//---------------------------------------------------------------------------
void    clCol::New()
{
    cnCard=0;
    for(int i=0;i<CN_CARD_FACE;i++)
        for(int j=0;j<CN_CARD_SUIT;j++)
        {
            card[cnCard].suit=(enSuit)j;
            card[cnCard].face=(enFace)i;
            cnCard++;
        }
#ifdef IS_JOKER
    card[cnCard++]=CARD_JOKER;
#endif
    ptrC=0;
}
//---------------------------------------------------------------------------
void    clCol::New(int cn,clCard *c0)
{
    cnCard=cn;
    memcpy(card,c0,cn*sizeof(clCard));
    ptrC=0;
}
//---------------------------------------------------------------------------
void    clCol::Shufle()
{
    if(ptrC != 0)
        ShiftCardToBegin();

    for(int i=cnCard-1;i>0;i--)
    {
        clCard c0=card[i];
		int nb = random(i + 1);
        card[i]=card[nb];
        card[nb]=c0;
    }
}
//---------------------------------------------------------------------------
void    clCol::ShufleOldCol()
{
    cnCard+=ptrC;
    ptrC=0;
    for(int i=0;i<cnCard;i++)
    {
        clCard c0=card[i];
        int nb=random(cnCard);
        card[i]=card[nb];
        card[nb]=c0;
    }
}
//---------------------------------------------------------------------------
clCard  clCol::GiveCard()
{
    cnCard--;
    return card[ptrC++];
}
//---------------------------------------------------------------------------
void    clCol::Skip(clCard c0)
{
    for(int i=0;i<cnCard;i++)
        if(card[i+ptrC]==c0)
        {
            for(;i<cnCard-1;i++)
                card[i+ptrC]=card[i+ptrC+1];
            cnCard--;
            return;
        }
}
//---------------------------------------------------------------------------
void    clCol::Skip(int nb)
{
    for(int i=nb;i<cnCard-1;i++)
        card[i+ptrC]=card[i+ptrC+1];
    cnCard--;
}
//---------------------------------------------------------------------------
int     clCol::NbCardInArr(clCard c0)
{
    for(int i=0;i<cnCard;i++)
        if(card[i+ptrC]==c0)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
int     clCol::MovePtr(int cnC)
{
    if(ptrC+cnC<0)
        cnC=-ptrC;
    else if(cnC>cnCard)
        cnC=cnCard;
    ptrC+=cnC;
    cnCard-=cnC;
    return cnC;
}
//---------------------------------------------------------------------------
void    clCol::operator = (tpBooleanDeck &deck)
{
    cnCard=ptrC=0;
    for(int i=0;i<CN_CARD_COL;i++)
        if(deck[i])
            card[cnCard++]=CardNB(i);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void    clDeck::New()
{
	for (int i = 0; i<CN_CARD_COL; i++)
		_card[i] = i;
	_cnCard = CN_CARD_COL;
	_ptrC = 0;
}
//---------------------------------------------------------------------------
void    clDeck::Shufle()
{
	if (_ptrC != 0)
		ShiftCardToBegin();

	for (int i = _cnCard - 1; i>0; i--)
	{
		tpCard c0 = _card[i];
		int nb = random(i + 1);
		_card[i] = _card[nb];
		_card[nb] = c0;
	}
}
//---------------------------------------------------------------------------
void    clDeck::ShufleOldCol()
{
	_cnCard += _ptrC;
	_ptrC = 0;
	for (int i = _cnCard - 1; i>0; i--)
	{
		tpCard c0 = _card[i];
		int nb = random(i + 1);
		_card[i] = _card[nb];
		_card[nb] = c0;
	}
}
//---------------------------------------------------------------------------
tpCard  clDeck::GiveCard()
{
	_cnCard--;
	return _card[_ptrC++];
}
//---------------------------------------------------------------------------
void    clDeck::Skip(tpCard c0)
{
	for (int i = 0; i<_cnCard; i++)
		if (_card[i + _ptrC] == c0)
		{
			for (; i<_cnCard - 1; i++)
				_card[i + _ptrC] = _card[i + _ptrC + 1];
			_cnCard--;
			return;
		}
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
