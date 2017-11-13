//---------------------------------------------------------------------------
#pragma hdrstop

#include "Card.h"

#ifdef IS_JOKER
clCard CARD_JOKER={C_JOKER,(enSuit)0};
#endif

char glCardFaceChar[]={'2','3','4','5','6','7','8','9','T','J','Q','K','A'};
char glCardSuitChar[]={'c','d','h','s'};

clCard CARDUNDEF={C_UNDEF,S_UNDEF};
//---------------------------------------------------------------------------
void    clCard::Name(char *name)
{
    name[0]=FaceCh(face);
    name[1]=SuitCh(suit);
}
//---------------------------------------------------------------------------
clAnsiString clCard::Name()
{
    char c[3]; 
    Name(c);
    c[2]='\0';
    clAnsiString as=c;
    return(as);
}

//---------------------------------------------------------------------------
void	clCard::operator = (tpCard &c)
{
	face = c.GetFace();
	suit = c.GetSuit();
}
//---------------------------------------------------------------------------
char FaceCh(enFace face)
{
    return ((int)face<C_UNDEF)? glCardFaceChar[face] : ' ';
}
//---------------------------------------------------------------------------
char SuitCh(enSuit face)
{
    return ((int)face<S_UNDEF)? glCardSuitChar[face] : ' ';
}
//---------------------------------------------------------------------------
void    CardStraight(int cn,clCard *card)
{
    for(int i=0;i<cn-1;i++)
    {
        int nb=i;
        for(int j=i+1;j<cn;j++)
            if(card[nb]>card[j])
                nb=j;
        clCard c0=card[i];
        card[i]=card[nb];
        card[nb]=c0;
    }
}
//---------------------------------------------------------------------------
enFace  CardFaceFromChar(char face)
{
    for(int i=0;i<CN_CARD_FACE;i++)
        if(face==glCardFaceChar[i])
            return (enFace)i;
    if(face=='1') return C_T;
    return C_UNDEF;
}
//---------------------------------------------------------------------------
clCard  CardForName(char *name)
{
    clCard res;
    switch(name[0])
    {
        case '2' : res.face=C_2;break;
        case '3' : res.face=C_3;break;
        case '4' : res.face=C_4;break;
        case '5' : res.face=C_5;break;
        case '6' : res.face=C_6;break;
        case '7' : res.face=C_7;break;
        case '8' : res.face=C_8;break;
        case '9' : res.face=C_9;break;
        case 'T' : res.face=C_T;break;
        case 'J' : res.face=C_J;break;
        case 'Q' : res.face=C_Q;break;
        case 'K' : res.face=C_K;break;
        case 'A' : res.face=C_A;break;
        default  : res.face=C_UNDEF;break;
    }
    switch(name[1])
    {
        case 'c' : res.suit=S_C;break;
        case 'd' : res.suit=S_D;break;
        case 'h' : res.suit=S_H;break;
        case 's' : res.suit=S_S;break;
        default  : res.suit=S_UNDEF;break;
    }
    return res;
}
//---------------------------------------------------------------------------
bool    CardCheckCard(clCard card)
{
    return (card.face>=C_2 && card.face<=C_A && card.suit>=S_C && card.suit<=S_S);
}
//---------------------------------------------------------------------------
void    clCardMap::Clear()
{
    for(int i=0;i<CN_CARD_FACE;i++)
        for(int j=0;j<CN_CARD_SUIT;j++)
            _card[i][j]=false;
    _count=0;
    for(int i=0;i<CN_CARD_FACE;i++)
        _arrF[i]=0;
}
//---------------------------------------------------------------------------
void    clCardMap::Full()
{
    for(int i=0;i<CN_CARD_FACE;i++)
        for(int j=0;j<CN_CARD_SUIT;j++)
            _card[i][j]=true;
    _count=CN_CARD_FACE*CN_CARD_SUIT;
    for(int i=0;i<CN_CARD_FACE;i++)
        _arrF[i]=CN_CARD_SUIT;
}
//---------------------------------------------------------------------------
clCard  clCardMap::ScipCard(enFace f0)
{
    for(int i=0;i<CN_CARD_SUIT;i++)
        if(_card[f0][i])
        {
            _card[f0][i]=false;
            _arrF[f0]--;
            _count--;
            return Card(f0,(enSuit)i);
        }
    return CARDUNDEF;
}
//---------------------------------------------------------------------------
bool    clCardMap::ScipCard(clCard card)
{
    if(_card[card.face][card.suit])
    {
        _card[card.face][card.suit]=false;
        _arrF[card.face]--;
        _count--;
        return true;
    }
    else return false;
}
//---------------------------------------------------------------------------
void    clCardMap::AddCard(clCard card)
{
    if(!_card[card.face][card.suit])
    {
        _card[card.face][card.suit]=true;
        _arrF[card.face]++;
        _count++;
    }
}
//---------------------------------------------------------------------------
clCard  WSReadCard(char *word)
{
	clCard card;
	switch (word[0])
	{
		case '2': card.face = C_2; break;
		case '3': card.face = C_3; break;
		case '4': card.face = C_4; break;
		case '5': card.face = C_5; break;
		case '6': card.face = C_6; break;
		case '7': card.face = C_7; break;
		case '8': card.face = C_8; break;
		case '9': card.face = C_9; break;
		case 'T': card.face = C_T; break;
		case 'J': card.face = C_J; break;
		case 'Q': card.face = C_Q; break;
		case 'K': card.face = C_K; break;
		case 'A': card.face = C_A; break;
		default: card.face = C_UNDEF; return card;
	}
	switch (word[1])
	{
		case 'c': card.suit = S_C; break;
		case 'd': card.suit = S_D; break;
		case 'h': card.suit = S_H; break;
		case 's': card.suit = S_S; break;
		default: card.face = C_UNDEF; break;
	}
	return card;
}
//---------------------------------------------------------------------------

