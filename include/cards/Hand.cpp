//---------------------------------------------------------------------------
#pragma hdrstop

//#include <vcl.h>

#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include "clCol.h"
#include "c:\home\card\Hand.h"

clHandDefArr glHandArr;
tpListHand glListHand;
//tpListHand glMyListHand("C:\\WORK\\Bin\\MyListHand.bin");

//---------------------------------------------------------------------------
tpListHand::tpListHand(char *nameFile)
{
	LoadFromFile(nameFile);
}
//---------------------------------------------------------------------------
int    tpListHand::LoadFromFile(char *nameFile)
{
	int handle = _open(nameFile, O_BINARY | O_RDONLY);
	if (handle == -1)
	{
		return -1;
	}
	_read(handle, this, sizeof(tpListHand));
	_close(handle);
	return 1;
}
//---------------------------------------------------------------------------
bool tpListHand::Eq(int nbH1, int nbH2)
{
	for (int i = 0; i < CN_HAND; i++)
		if (_nbH[i] == nbH1)
			return(true);
		else if (_nbH[i] == nbH2)
			return(false);
	return(false);

}
//---------------------------------------------------------------------------
double  tpListHand::UrDHand(int nbH)
{
	for (int i = 0; i < CN_HAND; i++)
		if (_nbH[i] == nbH)
			return _ur[i];
	return 1;
}
//---------------------------------------------------------------------------
double  tpListHand::UrDHand(clHand *hand)
{
	int nbH = hand->NbHand();
	return UrDHand(nbH);
}
//---------------------------------------------------------------------------
int     tpListHand::NbInArrForNbHand(int nbHand)
{
	for (int i = 0; i < CN_HAND; i++)
		if (_nbH[i] == nbHand)
			return i;
	return -1;
}
//---------------------------------------------------------------------------
int     tpListHand::NbInArrForUr(double ur)
{
	for (int i = 0; i < CN_HAND; i++)
		if (_ur[i] > ur)
			return i;
	return CN_HAND - 1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
double  HandWeight(int nb0, int nb)
{
	clHand h0;
	h0.SetHand(nb0, S_H, S_S);
	return HandWeight(2, h0._cards, nb);
}
//---------------------------------------------------------------------------
double  HandWeight(int cn0, clCard *c0, int nb)
{
	clCard cards[CN_CARD_COL];
	int cn = 0, cnH = 0;
	for (int i = 0; i < CN_CARD_FACE; i++)
		for (int j = 0; j < CN_CARD_SUIT; j++)
		{
			clCard card = Card((enFace)i, (enSuit)j);
			int k;
			for (k = 0; k < cn0; k++)
				if (card == c0[k])
					break;
			if (k == cn0)
				cards[cn++] = card;
		}
	for (int i = 0; i < cn; i++)
		for (int j = i + 1; j < cn; j++)
		{
			clHand hand;
			hand.SetHand(cards[i], cards[j]);
			if (hand.NbHand() == nb)
				cnH++;
		}
	return cnH*2. / cn / (cn - 1);

}
//---------------------------------------------------------------------------
char *clHand::NameHand(int nb, char *str)
{
	clHand h;
	h.SetHand(nb);
	return h.NameHand(str);
}
//---------------------------------------------------------------------------
int     clHand::CnHand(int nb)
{
	if (nb < CN_CARD_FACE)
		return 6;
	else
	{
		if (nb < CN_CARD_FACE*(CN_CARD_FACE - 1) / 2 + CN_CARD_FACE)
			return 4;
		else
			return 12;
	}
}
//---------------------------------------------------------------------------
int  clHand::NbHand(char *str)
{
	char name[3];
	name[0] = str[0];
	name[1] = 'c';
	name[2] = '\0';
	clHand hand;
	hand._cards[0] = CardForName(name);
	if (hand._cards[0].face == C_UNDEF) return -1;
	name[0] = str[1];
	if (str[2] != 's')
		name[1] = 'd';
	hand._cards[1] = CardForName(name);
	if (hand._cards[1].face == C_UNDEF) return -1;
	return hand.NbHand();
}
//---------------------------------------------------------------------------
bool    clHand::operator > (clHand hand)
{
	clHand hand1, hand2;
	if (_cards[0] > _cards[1])
		hand1.SetHand(_cards[0], _cards[1]);
	else
		hand1.SetHand(_cards[1], _cards[0]);
	if (hand._cards[0] > hand._cards[1])
		hand2.SetHand(hand._cards[0], hand._cards[1]);
	else
		hand2.SetHand(hand._cards[1], hand._cards[0]);
	if (hand1._cards[0].face == hand2._cards[0].face)
	{
		if (hand1._cards[1].face == hand2._cards[1].face)
		{
			if (hand1._cards[0].suit == hand2._cards[0].suit)
			{
				return (hand1._cards[1].suit > hand2._cards[1].suit);
			}
			return(hand1._cards[0].suit > hand2._cards[0].suit);
		}
		return (hand1._cards[1].face > hand2._cards[1].face);
	}
	return (hand1._cards[0].face > hand2._cards[0].face);
}
bool    clHand::operator < (clHand hand)
{
	return !(*this > hand);
}
void clHand::SetHand(int nb, enSuit s1, enSuit s2)
{
	if (nb < CN_CARD_FACE)
	{
		_cards[0].face = (enFace)nb;
		_cards[0].suit = s1;
		_cards[1].face = (enFace)nb;
		_cards[1].suit = s2;
	}
	else
	{
		nb -= CN_CARD_FACE;
		if (nb < CN_CARD_FACE*(CN_CARD_FACE - 1) / 2)
		{
			_cards[0].suit = s1;
			_cards[1].suit = s1;
		}
		else
		{
			nb -= CN_CARD_FACE*(CN_CARD_FACE - 1) / 2;
			_cards[0].suit = s1;
			_cards[1].suit = s2;
		}
		int f1 = C_2;
		for (int cn = 12; cn > 0; cn--)
		{
			if (nb < cn)
			{
				_cards[0].face = (enFace)f1;
				_cards[1].face = (enFace)(f1 + nb + 1);
				break;
			}
			nb -= cn;
			f1++;
		}
	}
}
//---------------------------------------------------------------------------
char    *clHand::NameHand(char *str)
{
	if (_cards[0].face > _cards[1].face)
	{
		str[0] = _cards[0].FaceChar();
		str[1] = _cards[1].FaceChar();
	}
	else
	{
		str[1] = _cards[0].FaceChar();
		str[0] = _cards[1].FaceChar();
	}
	if (_cards[0].suit == _cards[1].suit)
		str[2] = 's';
	else
		str[2] = ' ';
	str[3] = '\0';
	return str;
}
//---------------------------------------------------------------------------
clHand  clHand::SetHand(enFace c2, enFace c1, bool suit)
{
	if (c2 == c1)
		SetHand(Card(c2, S_S), Card(c1, S_H));
	else if (suit)
		SetHand(Card(c2, S_S), Card(c1, S_S));
	else
		SetHand(Card(c2, S_S), Card(c1, S_H));
	return *this;
}
//---------------------------------------------------------------------------
int     clHand::NbHand()
{
	if (_cards[0] == _cards[1])
		return -1;
	if (_cards[0].face == _cards[1].face)
		return _cards[0].face;
	int nb = CN_CARD_FACE;
	enFace c1, c2;
	if (_cards[0].face < _cards[1].face)
	{
		c1 = _cards[0].face;
		c2 = _cards[1].face;
	}
	else
	{
		c1 = _cards[1].face;
		c2 = _cards[0].face;
	}
	if (_cards[0].suit != _cards[1].suit)
		nb += CN_CARD_FACE*(CN_CARD_FACE - 1) / 2;
	int cn = 12;
	for (int f = C_2; f < C_A; f++)
	{
		if (f == c1)
		{
			nb += c2 - c1 - 1;
			break;
		}
		nb += cn;
		cn--;
	}
	return nb;
}
//---------------------------------------------------------------------------
int     clHand::NbHandAll()
{
	int nb0 = _cards[0].Number();
	int nb1 = _cards[1].Number();
	return(nb0 < nb1) ? nb1*(nb1 - 1) / 2 + nb0 : nb0*(nb0 - 1) / 2 + nb1;
}
//---------------------------------------------------------------------------
void clHand::SetHandAll(int nb)
{
	for (int i = 1; i < CN_CARD_COL; i++)
		if (nb >= i)
			nb -= i;
		else
		{
			_cards[0] = CardNB(i);
			_cards[1] = CardNB(nb);
			return;
		}
}
//---------------------------------------------------------------------------
int    clHand::FaceDistance()
{
	int h = _cards[0].face, s = _cards[1].face;
	if (h == s) return false;
	if (h < s)
	{
		h = s;
		s = _cards[0].face;
	}
	if (h == C_A && s < C_8) return s + 1;
	return h - s;
}
//---------------------------------------------------------------------------
enFace clHand::OtherCard(enFace f1)
{
	return (_cards[0].face == f1) ? _cards[1].face : _cards[0].face;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
clHand  CrHand(enFace f1, enSuit s1, enFace f2, enSuit s2)
{
	clHand h;
	h._cards[0].face = f1;
	h._cards[0].suit = s1;
	h._cards[1].face = f2;
	h._cards[1].suit = s2;
	return h;
}
//---------------------------------------------------------------------------
int  NbHand(enFace f1, enFace f2, bool suit)
{
	clHand h = CrHand(f1, S_D, f2, (suit) ? S_D : S_S);
	return h.NbHand();
}
//---------------------------------------------------------------------------
clHand  HandCreateHand(int nbHand, int ind)
{
	clHand h;
	if (nbHand < CN_CARD_FACE)
	{
		h._cards[0].face = h._cards[1].face = (enFace)nbHand;
		for (int j0 = S_C; j0 < S_S; j0++)
			for (int j1 = j0 + 1; j1 <= S_S; j1++)
				if (ind == 0)
				{
					h._cards[0].suit = (enSuit)j0;
					h._cards[1].suit = (enSuit)j1;
					return h;
				}
				else ind--;
	}
	nbHand -= CN_CARD_FACE;
	if (nbHand < CN_CARD_FACE*(CN_CARD_FACE - 1) / 2)
	{
		for (int i0 = 0; i0 < CN_CARD_FACE; i0++)
			for (int i1 = i0 + 1; i1 < CN_CARD_FACE; i1++)
				if (nbHand == 0)
				{
					h._cards[0].face = (enFace)i0;
					h._cards[1].face = (enFace)i1;
					h._cards[0].suit = h._cards[1].suit = (enSuit)ind;
					return h;
				}
				else nbHand--;
	}
	nbHand -= CN_CARD_FACE*(CN_CARD_FACE - 1) / 2;
	for (int i0 = 0; i0 < CN_CARD_FACE; i0++)
		for (int i1 = i0 + 1; i1 < CN_CARD_FACE; i1++)
			if (nbHand == 0)
			{
				h._cards[0].face = (enFace)i0;
				h._cards[1].face = (enFace)i1;
				for (int j0 = S_C; j0 <= S_S; j0++)
					for (int j1 = S_C; j1 <= S_S; j1++)
						if (j0 != j1)
							if (ind == 0)
							{
								h._cards[0].suit = (enSuit)j0;
								h._cards[1].suit = (enSuit)j1;
								return h;
							}
							else ind--;
			}
			else nbHand--;
			return h;
}
//---------------------------------------------------------------------------
int     NbHandAll(int nb0, int nb1)
{
	return(nb0 < nb1) ? nb1*(nb1 - 1) / 2 + nb0 : nb0*(nb0 - 1) / 2 + nb1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void    tpHand::SetHand(tpCard c1, tpCard c2)
{
	if (c1 > c2)
	{
		_cards[0] = c1;
		_cards[1] = c2;
	}
	else
	{
		_cards[1] = c1;
		_cards[0] = c2;
	}
}
//---------------------------------------------------------------------------
void	tpHand::SetHandAll(int nb)
{
	for (int i = 1; i < CN_CARD_COL; i++)
		if (nb >= i)
			nb -= i;
		else
		{
			_cards[0] = i;
			_cards[1] = nb;
			return;
		}
}
//---------------------------------------------------------------------------
int     tpHand::NbHand()
{
	clHand hand;
	hand.SetHand((int)_cards[0]._card, (int)_cards[1]._card);
	return hand.NbHand();
}
//---------------------------------------------------------------------------
void	tpHand::operator = (clHand hand)
{
	_cards[0] = hand[0];
	_cards[1] = hand[1];
}
//---------------------------------------------------------------------------
char    *tpHand::NameHandAll(char *str)
{
	clCard c0 = CardNB((int)_cards[0]);
	c0.Name(str);
	c0 = CardNB((int)_cards[1]);
	c0.Name(str + 2);
	str[4] = '\0';
	return str;
}
//---------------------------------------------------------------------------
bool	tpHand::IncludeCards(int cn, tpCard *cards)
{
	for (int i = 0; i < cn; i++)
	{
		if (_cards[0] == cards[i]) return true;
		if (_cards[1] == cards[i]) return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
clHandDefArr::clHandDefArr()
{
	for (int i = 0; i < CN_HAND; i++)
		_cnAll[i] = clHand::CnHand(i);
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		tpHand hand1; hand1.SetHandAll(i);
		_nb169[i] = hand1.NbHand();
	}
	for (int i0 = 0; i0 < ALL_CN_HAND; i0++)
	{
		tpHand hand0; hand0.SetHandAll(i0);
		_arrHand[i0] = hand0;
		for (int i1 = 0; i1 < ALL_CN_HAND; i1++)
		{
			tpHand hand1; hand1.SetHandAll(i1);
			_arrCross[i0][i1] = hand0.IncludeCardsHand(hand1);
		}
	}
	for (int i0 = 0; i0 < CN_HAND; i0++)
	{
		for (int i = 0; i < CN_HAND; i++)
			_arrMultCross169[i0][i] = 1;
		clHand h0; h0.SetHand(i0);
		int nbH = h0.NbHandAll();
		for (int i1 = 0; i1 < ALL_CN_HAND; i1++)
			if (_arrCross[nbH][i1])
			{
				int nbH169 = _nb169[i1];
				_arrMultCross169[i0][nbH169] -= 1. / _cnAll[nbH169];
			}
	}
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
