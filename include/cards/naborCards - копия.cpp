//---------------------------------------------------------------------------
//#include "stdafx.h"


#pragma hdrstop

#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include <share.h>
#include <afxwin.h>
#include "c:\home\card\naborCards.h"
#include "c:\Home\Util\Sort.h"
#include "C:\home\card\clCol.h"
#include "C:\home\holdem\Strategy\Source\HandsGroupEX.h"
#include "c:\home\util\usefullProc.h"
#include "C:\home\holdem\Common\clHandsCompare.h"

#define	FILE_PREFLOP_HANDS	"e:\\home\\bin\\BoardHands\\PreflopHands.bin"
#define	FILE_NABOR3			"e:\\home\\bin\\BoardHands\\Flop.bin"
#define	FILE_NABOR4			"e:\\home\\bin\\BoardHands\\Turn.bin"
#define	FILE_FLOP_HANDS		"e:\\home\\bin\\BoardHands\\FlopHands.bin"
#define	FILE_TURN_HANDS		"e:\\home\\bin\\BoardHands\\TurnHands.bin"
#define	FILE_HAND_TO_FLOP	"e:\\home\\bin\\BoardHands\\HandsToFlop.bin"
#define	FILE_NABOR5			"e:\\home\\bin\\BoardHands\\River.bin"
#define	FILE_RIVER_HANDS		"e:\\home\\bin\\BoardHands\\RiverHands.bin"
#define	FILE_RIVER_HANDS_GAME	"e:\\home\\bin\\BoardHands\\RiverHandsGame.bin"
#define	FILE_RIVER_HANDS_IND	"e:\\home\\bin\\BoardHands\\RiverHandsInd.bin"
#define	FILE_RIVER_HANDS_EV		"e:\\home\\bin\\BoardHands\\RiverHandsEV.bin"
#define	FILE_GH_DIST		"e:\\home\\bin\\BoardHands\\DistGH.bin"

struct tpRefToProperty
{
	byte	GetChangeSuit()				{ return _val >> 24; }
	void	CreateRef(byte cs, int nb)	{ _val = (((int)cs) << 24) + nb; }
	int		Number()					{ return _val & 0xFFFFFF; }

	int _val;
};

struct tpGame24
{
	tpGame24()	{ _arr[0] = _arr[1] = _arr[2] = 0; }
	tpGame24(clGamePoker game)
	{
		*this = game;
	}
	void operator = (clGamePoker game)
	{
		_arr[0] = (game.game << 4) + game.face[0];
		_arr[1] = (game.face[1] << 4) + game.face[2];
		_arr[2] = (game.face[3] << 4) + game.face[4];
	}
	int		ToInt()		{return ((int)_arr[0] << 16) + ((int)_arr[1] << 8) + _arr[2]; }

	byte _arr[3];
};

tpPreflopHands glArrPreflopHand[CN_HAND];
vector <byte> glArrNbHand;

vector <tpRefToProperty> glVectRefFlop;
vector <stNaborProperty> glVectFlop;
vector <tpRefToProperty> glVectRefFlopHand;
vector <tpFlopHandProperty> glVectFlopHand;

vector <tpRefToProperty> glVectRefTurn;
vector <stNaborProperty> glVectTurn;
vector <int> glVectTurnHandsNb;
vector <tpTurnHandProperty> glVectTurnHand;

vector <int> glArrNb5;
vector <stNaborProperty> glVectRiver;
vector <int> glVectRiverHandsNb;
//vector <tpRiverHandProperty> glVectRiverHand;
vector <tpGame24> glVectRiverHandGame;

#define	CN_CHANGE_SUIT						24
byte glArrChangeSuit[] = { 0xE4, 0xB4, 0xD8, 0x78, 0x9C, 0x6C, 0xE1, 0xB1, 0xC9, 0x39, 0x8D, 0x2D, 0xD2, 0x72, 0xC6, 0x36, 0x4E, 0x1E, 0x93, 0x63, 0x87, 0x27, 0x4B, 0x1B };

int GetFaceCardNB(int nb)				{ return nb / CN_CARD_SUIT; }
int GetSuitCardNB(int nb)				{ return nb % CN_CARD_SUIT; }
void	SetSuitCardNB(int &nb, int s)			{ nb=nb / CN_CARD_SUIT *CN_CARD_SUIT+s; }
int	MaxIdFlopHand(int *cards);
int	BaseNaborFlopHand(int id, byte &cs);
int		CnSuited(int cn, int *arrC, enSuit &ss);
int		CnCardSuited(enSuit s0, int cn, int *cards);
int	BaseNaborTurn(int id, byte &cs);
__int64		BaseNaborTurnHand(int idBoard, int nbTurn0, bool &ns);
__int64		MaxIdTurnHand(int *cards);
__int64		BaseNaborRiverHand(__int64 idBoard, int nbTurn0, bool &ns);
__int64		MaxIdRiverHand(int *cards);
double AverageAndDisp0(int cn, double *arr, clHandsGroupEx &ghF, double &disp);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	stNaborProperty::WrProperty(int cnCard, int id)
{
	_id = id;
	_cnNabor = 0;
	_cnCard = cnCard;
	for (int i = 0; i < cnCard; i++)
		_cards[i] = CardNB((id >> (cnCard - 1 - i) * 6) & 0x3F);
	_game = GameWhatGame(_cards, _cnCard);
	for (int i = 0; i < cnCard; i++)
		_cards[i] = CardNB((id >> (cnCard - 1 - i) * 6) & 0x3F);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clHand tpTurnHandProperty::Hand()
{
	clHand hand = { CardNB((_idBoardHand >> 6) & 0x3F), CardNB(_idBoardHand & 0x3F) };
	return hand;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t ReadBigBlock(int handle, void *ptr, size_t sizeA)
{
	size_t step = 0x40000000, res = 0;
	byte *buf = (byte *)ptr;
	while (sizeA > 0)
	{
		size_t size = (sizeA > step) ? step : sizeA;
		int val = _read(handle, buf, size);
		res += val;
		if (val != size) break;
		sizeA -= size;
		buf += size;
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t WriteBigBlock(int handle, void *ptr, size_t sizeA)
{
	size_t step = 0x40000000, res = 0;
	byte *buf = (byte *)ptr;
	while (sizeA > 0)
	{
		size_t size = (sizeA > step) ? step : sizeA;
		int val = _write(handle, buf, size);
		res += val;
		if (val != size) break;
		sizeA -= size;
		buf += size;
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T> size_t  WriteBigVectToFile(int handle, vector <T> &vect)
{
	unsigned dim = vect.size();
	_write(handle, &dim, sizeof(unsigned));
	return WriteBigBlock(handle, &vect[0], dim*sizeof(T));
}
//---------------------------------------------------------------------------
template <typename T> size_t  ReadBigVectToFile(int handle, vector <T> &vect)
{
	unsigned dim;
	_read(handle, &dim, sizeof(unsigned));
	vect.resize(dim);
	return ReadBigBlock(handle, &vect[0], dim*sizeof(T));
}
//---------------------------------------------------------------------------
void	tpFlopHandProperty::WrProperty(int id)
{
	_idBoardHand = id;
	_cnNabor = 0;
	_board[0] = CardNB((id >> 24) & 0x3F);
	_board[1] = CardNB((id >> 18) & 0x3F);
	_board[2] = CardNB((id >> 12) & 0x3F);
	_hand[0] = CardNB((id >> 6) & 0x3F);
	_hand[1] = CardNB(id & 0x3F);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetHandIndex(clHand hand)
{
	return glArrNbHand[hand[0].Number() + (hand[1].Number() << 6)];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CreateAllPreFlops()
{
	int sizeA = 0x1000;
	glArrNbHand.resize(sizeA);
	ZeroMemory(&glArrNbHand[0], sizeA);
	int nb = 0;
	for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
		for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
		{
			int id = i0 + (i1 << 6);
			tpHand hand;
			hand.SetHand(i0, i1);
			glArrNbHand[id] = hand.NbHand();
			id = i1 + (i0 << 6);
			glArrNbHand[id] = hand.NbHand();
		}
	for (int i = 0; i < CN_HAND; i++)
	{
		glArrPreflopHand[i]._hand.SetHand(i);
		glArrPreflopHand[i]._ev100 = glArrPreflopHand[i]._ev50 = -1;
	}
	int handle = _open(FILE_PREFLOP_HANDS, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteBigVectToFile <byte>(handle, glArrNbHand);
		_write(handle, &glArrPreflopHand, sizeof(glArrPreflopHand));
		_close(handle);
	}
}
//---------------------------------------------------------------------------
bool	LoadDataHandsBoard()
{
	int handle = _open(FILE_PREFLOP_HANDS, O_RDONLY | O_BINARY);
	if (handle == -1) return false;
	ReadBigVectToFile <byte>(handle, glArrNbHand);
	_read(handle, &glArrPreflopHand, sizeof(glArrPreflopHand));
	_close(handle);

	handle = _open(FILE_NABOR3, O_RDONLY | O_BINARY);
	if (handle == -1) return false;
	glVectRefFlop.resize(0x40000);
	_read(handle, &glVectRefFlop[0], 0x40000 * sizeof(tpRefToProperty));
	glVectFlop.resize(NABOR_CN_FLOP);
	_read(handle, &glVectFlop[0], NABOR_CN_FLOP*sizeof(stNaborProperty));
	_close(handle);

	handle = _open(FILE_FLOP_HANDS, O_RDONLY | O_BINARY);
	if (handle == -1) return false;
	size_t sizeA = 0x40000000 * sizeof(tpRefToProperty);
	glVectRefFlopHand.resize(0x40000000);
	size_t val=ReadBigBlock(handle, &glVectRefFlopHand[0], sizeA);
	ReadBigVectToFile <tpFlopHandProperty>(handle, glVectFlopHand);
	_close(handle);

	handle = _open(FILE_TURN_HANDS, O_RDONLY | O_BINARY);
	if (handle == -1) return false;
	ReadBigVectToFile<tpTurnHandProperty>(handle, glVectTurnHand);
	ReadBigVectToFile<int>(handle, glVectTurnHandsNb);
	_close(handle);

	handle = _open(FILE_RIVER_HANDS, O_RDONLY | O_BINARY );
	if (handle == -1) return false;
//	ReadBigVectToFile<tpRiverHandProperty>(handle, glVectRiverHand);
	ReadBigVectToFile<int>(handle, glVectRiverHandsNb);
	_close(handle);

	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpRefToProperty		GetFlopHandRef(clCard *arr, clHand hand)
{
	int val = (arr[0].Number() << 24) + (arr[1].Number() << 18) + (arr[2].Number()<<12) + (hand[0].Number() << 6) + hand[1].Number();
	return glVectRefFlopHand[val];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetFlopHandIndex(clCard *arr, clHand hand)
{
	tpRefToProperty ref = GetFlopHandRef(arr, hand);
	return ref.Number();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpRefToProperty		GetFlopHandRef(tpCard *arr, tpHand hand)
{
	int val = ((int)arr[0]._card << 24) + ((int)arr[1]._card << 18) + ((int)arr[2]._card << 12) + ((int)hand[0]._card << 6) + hand[1]._card;
	return glVectRefFlopHand[val];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetFlopHandIndex(tpCard *arr, tpHand hand)
{
	tpRefToProperty ref = GetFlopHandRef(arr, hand);
	return ref.Number();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clAnsiString	FlopToString(int nb)
{
	clAnsiString as;
/*	stNaborProperty &pr = glVectFlop[nb];
	for (int i = 0; i < pr._cnCard; i++)
		as += pr._cards[i].Name() + (clAnsiString)" ";*/
	return as;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clAnsiString FlopHandToString(int nb)
{
	tpFlopHandProperty &pr = glVectFlopHand[nb];
	clAnsiString as = "Flop ";
	for (int i = 0; i < 3; i++)
		as += pr._board[i].Name();
	as += "; Hand ";
	for (int i = 0; i < 2; i++)
		as += pr._hand._cards[i].Name();
	return as;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
inline	void	SortFlop(int *cards)
{
	int max;
	if (cards[0]<cards[1])
		if (cards[1] < cards[2])
		{
			max = cards[2];
			cards[2] = cards[0];
			cards[0] = max;
		}
		else
		{
			max = cards[1];
			if (cards[0] < cards[2])
			{
				cards[1] = cards[2];
				cards[2] = cards[0];
				cards[0] = max;
			}
			else
			{
				cards[1] = cards[0];
				cards[0] = max;
			}
		}
	else
		if (cards[0] < cards[2])
		{
			max = cards[2];
			cards[2] = cards[1];
			cards[1] = cards[0];
			cards[0] = max;
		}
		else if (cards[1] < cards[2])
		{
			max = cards[2];
			cards[2] = cards[1];
			cards[1] = max;
		}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	MaxIdFlopHand(int *cards)
{
	SortFlop(cards);
	if (cards[3] < cards[4])
	{
		int temp = cards[4];
		cards[4] = cards[3];
		cards[3] = temp;
	}
	return (cards[0] << 24) + (cards[1] << 18) + (cards[2] << 12) + (cards[3] << 6) + cards[4];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	BetterChange(byte cs0, byte cs)
{
	int cn = 0, cn0 = 0;
	for (int i = 0; i < 4; i++)
	{
		if ((cs >> 2 * i) & 3 == i)
			cn++;
		if ((cs0 >> 2 * i) & 3 == i)
			cn0++;
	}
	return cn0 > cn;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	BaseNaborFlopHand(int id, byte &cs)
{
	int cards[] = { (id >> 24) & 0x3F, (id >> 18) & 0x3F, (id >> 12) & 0x3F, (id >> 6) & 0x3F, id & 0x3F };
	int max = MaxIdFlopHand(cards);
	cs = glArrChangeSuit[0];
	for (int i = 1; i<CN_CHANGE_SUIT; i++)
	{
		char cs0 = glArrChangeSuit[i];
		int c0[5];
		memcpy(c0, cards, sizeof(c0));
		for (int j = 0; j < 5; j++)
		{
			int s = GetSuitCardNB(c0[j]);
			SetSuitCardNB(c0[j], (cs0 >> 2 * s) & 3);
		}
		int val = MaxIdFlopHand(c0);
		if (val > max)
		{
			max = val;
			cs = cs0;
		}
		else if (val == max)
			if (BetterChange(cs0, cs))
				cs = cs0;
	}
	return max;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int MaxIdFlop(int *cards)
{
	SortFlop(cards);
	return (cards[0] << 12) + (cards[1] << 6) + cards[2];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	BaseNaborFlop(int id, byte &cs)
{
	int cards[3] = { (id >> 12) & 0x3F, (id >> 6) & 0x3F, id & 0x3F };
	int max = MaxIdFlop(cards);
	cs = glArrChangeSuit[0];
	for (int i = 1; i<CN_CHANGE_SUIT; i++)
	{
		char cs0 = glArrChangeSuit[i];
		int c0[3];
		memcpy(c0, cards, sizeof(c0));
		for (int j = 0; j < 3; j++)
		{
			int s = GetSuitCardNB(c0[j]);
			SetSuitCardNB(c0[j], (cs0 >> 2 * s) & 3);
		}
		int val = MaxIdFlop(c0);
		if (val > max)
		{
			max = val;
			cs = cs0;
		}
		else if (val == max)
			if (BetterChange(cs0, cs))
				cs = cs0;
	}
	return max;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateAllFlops()
{
	int sizeA = 0x40000 * sizeof(tpRefToProperty);
	glVectRefFlop.resize(0x40000);
	ZeroMemory(&glVectRefFlop[0], sizeA);
	glVectFlop.resize(NABOR_CN_FLOP);
	tpRefToProperty val;
	int nb = 0;
	byte cs;
	for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
		for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
			for (int i2 = i1 + 1; i2 < CN_ALL_CARDS; i2++)
			{
				int id = i0 + (i1 << 6) + (i2 << 12);
				int idB = BaseNaborFlop(id, cs);
				if (glVectRefFlop[idB].GetChangeSuit() == 0)
				{
					glVectRefFlop[idB].CreateRef(cs, nb);
					glVectFlop[nb++].WrProperty(3, idB);
				}
				val.CreateRef(cs, glVectRefFlop[idB].Number());
				glVectFlop[glVectRefFlop[idB].Number()]._cnNabor++;
				glVectRefFlop[id] = val;
				id = i0 + (i2 << 6) + (i1 << 12);
				glVectRefFlop[id] = val;
				id = i1 + (i0 << 6) + (i2 << 12);
				glVectRefFlop[id] = val;
				id = i1 + (i2 << 6) + (i0 << 12);
				glVectRefFlop[id] = val;
				id = i2 + (i0 << 6) + (i1 << 12);
				glVectRefFlop[id] = val;
				id = i2 + (i1 << 6) + (i0 << 12);
				glVectRefFlop[id] = val;
			}
	int handle = _open(FILE_NABOR3, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		_write(handle, &glVectRefFlop[0], sizeA);
		_write(handle, &glVectFlop[0], NABOR_CN_FLOP*sizeof(stNaborProperty));
		_close(handle);
	}
}
//---------------------------------------------------------------------------
void CreateAllFlopsHand()
{
	size_t sizeA = 0x40000000 * sizeof(tpRefToProperty);
	glVectRefFlopHand.resize(0x40000000);
	ZeroMemory(&glVectRefFlopHand[0], sizeA);
	glVectFlopHand.resize(CN_NABOR_HAND_FLOP);
	tpRefToProperty val;
	int nb = 0;
	byte cs;
	for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
		for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
			for (int i2 = i1 + 1; i2 < CN_ALL_CARDS; i2++)
				for (int h0 = 0; h0 < CN_ALL_CARDS; h0++)
					if (h0 != i0 && h0 != i1 && h0 != i2)
						for (int h1 = h0 + 1; h1 < CN_ALL_CARDS; h1++)
							if (h1 != i0 && h1 != i1 && h1 != i2)
							{
								int id = (i0<<24) + (i1 << 18) + (i2 << 12) + (h1 << 6) + h0;
								int idB = BaseNaborFlopHand(id, cs);
								if (glVectRefFlopHand[idB].GetChangeSuit() == 0)
								{
									glVectRefFlopHand[idB].CreateRef(cs, nb);
									glVectFlopHand[nb++].WrProperty(idB);
								}
								val.CreateRef(cs, glVectRefFlopHand[idB].Number());
								glVectFlopHand[glVectRefFlopHand[idB].Number()]._cnNabor++;
								glVectRefFlopHand[id] = val;
								id = (i0 << 24) + (i2 << 18) + (i1 << 12) + (h1 << 6) + h0;
								glVectRefFlopHand[id] = val;
								id = (i1 << 24) + (i0 << 18) + (i2 << 12) + (h1 << 6) + h0;
								glVectRefFlopHand[id] = val;
								id = (i1 << 24) + (i2 << 18) + (i0 << 12) + (h1 << 6) + h0;
								glVectRefFlopHand[id] = val;
								id = (i2 << 24) + (i0 << 18) + (i1 << 12) + (h1 << 6) + h0;
								glVectRefFlopHand[id] = val;
								id = (i2 << 24) + (i1 << 18) + (i0 << 12) + (h1 << 6) + h0;
								glVectRefFlopHand[id] = val;

								id = (i0 << 24) + (i1 << 18) + (i2 << 12) + (h0 << 6) + h1;
								glVectRefFlopHand[id] = val;
								id = (i0 << 24) + (i2 << 18) + (i1 << 12) + (h0 << 6) + h1;
								glVectRefFlopHand[id] = val;
								id = (i1 << 24) + (i0 << 18) + (i2 << 12) + (h0 << 6) + h1;
								glVectRefFlopHand[id] = val;
								id = (i1 << 24) + (i2 << 18) + (i0 << 12) + (h0 << 6) + h1;
								glVectRefFlopHand[id] = val;
								id = (i2 << 24) + (i0 << 18) + (i1 << 12) + (h0 << 6) + h1;
								glVectRefFlopHand[id] = val;
								id = (i2 << 24) + (i1 << 18) + (i0 << 12) + (h0 << 6) + h1;
								glVectRefFlopHand[id] = val;
							}
	int handle = _open(FILE_FLOP_HANDS, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteBigBlock(handle, &glVectRefFlopHand[0], sizeA);
		WriteBigVectToFile <tpFlopHandProperty>(handle, glVectFlopHand);
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateAllTurns()
{
	int sizeA = 0x1000000 * sizeof(tpRefToProperty);
	glVectRefTurn.resize(0x1000000);
	ZeroMemory(&glVectRefTurn[0], sizeA);
	glVectTurn.resize(NABOR_CN_TURN);
	tpRefToProperty val;
	int nb = 0;
	byte cs;
	for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
		for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
			for (int i2 = i1 + 1; i2 < CN_ALL_CARDS; i2++)
				for (int iT = 0; iT < CN_ALL_CARDS; iT++)
					if (iT != i1 && iT != i2 && iT != i0)
					{
						int id = iT + (i0 << 6) + (i1 << 12) + (i2 << 18);
						int idB = BaseNaborTurn(id, cs);
						if (glVectRefTurn[idB].GetChangeSuit() == 0)
						{
							glVectRefTurn[idB].CreateRef(cs, nb);
							glVectTurn[nb++].WrProperty(4, idB);
						}
						val.CreateRef(cs, glVectRefTurn[idB].Number());
						glVectTurn[glVectRefTurn[idB].Number()]._cnNabor++;
						glVectRefTurn[id] = val;
						id = iT + ((i0 << 6) + (i2 << 12) + (i1 << 18));
						glVectRefTurn[id] = val;
						id = iT + ((i1 << 6) + (i0 << 12) + (i2 << 18));
						glVectRefTurn[id] = val;
						id = iT + ((i1 << 6) + (i2 << 12) + (i0 << 18));
						glVectRefTurn[id] = val;
						id = iT + ((i2 << 6) + (i0 << 12) + (i1 << 18));
						glVectRefTurn[id] = val;
						id = iT + ((i2 << 6) + (i1 << 12) + (i0 << 18));
						glVectRefTurn[id] = val;
					}
	int handle = _open(FILE_NABOR4, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteBigVectToFile<tpRefToProperty>(handle, glVectRefTurn);
		WriteBigVectToFile<stNaborProperty>(handle, glVectTurn);
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateAllTurnsHand()
{
	size_t dim = glVectFlopHand.size();
	glVectTurnHandsNb.resize(dim*CN_ALL_CARDS);
	FillMemory(&glVectTurnHandsNb[0], dim*CN_CARD_COL*sizeof(int), 0xFF);
	glVectTurnHand.clear();
	int nbCur = 0;
	for (int i = 0; i < dim; i++)
	{
		tpFlopHandProperty &bh = glVectFlopHand[i];
		int c0[5] = { bh._board[0].Number(), bh._board[1].Number(), bh._board[2].Number(), bh._hand[0].Number(), bh._hand[1].Number()};
		vector <tpTurnHandProperty> vect5;
		tpTurnHandProperty prop;
		int sh = i*CN_ALL_CARDS;
		int arrNS[CN_CARD_FACE];
		FillMemory(arrNS, CN_CARD_FACE*sizeof(int), 0xFF);
		for (int i1 = 0; i1 < CN_CARD_COL; i1++)
			if (i1 != c0[0] && i1 != c0[1] && i1 != c0[2] && i1 != c0[3] && i1 != c0[4])
			{
				bool b0 = false;
				prop._idBoardHand = BaseNaborTurnHand(bh._idBoardHand, i1,b0);
				if (b0)	arrNS[GetFaceCardNB(i1)] = i1;
				int dim = vect5.size(), j = 0;
				for (; j < dim; j++)
					if (prop._idBoardHand == vect5[j]._idBoardHand)
					{
						glVectTurnHandsNb[sh+i1] = nbCur + j;
						break;
					}
				if (j == dim)
				{
					vect5.push_back(prop);
					glVectTurnHandsNb[sh+i1] = nbCur + dim;
				}
			}
		glVectTurnHand.insert(glVectTurnHand.end(), vect5.begin(), vect5.end());
		nbCur += vect5.size();
		int nb0 = arrNS[GetFaceCardNB(c0[4])];
		if (nb0 != -1)	glVectTurnHandsNb[sh + c0[4]] = glVectTurnHandsNb[sh + nb0];
		nb0 = arrNS[GetFaceCardNB(c0[3])];
		if (nb0 != -1)	glVectTurnHandsNb[sh + c0[3]] = glVectTurnHandsNb[sh + nb0];
	}
	int handle = _open(FILE_TURN_HANDS, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteBigVectToFile<tpTurnHandProperty>(handle, glVectTurnHand);
		WriteBigVectToFile<int>(handle, glVectTurnHandsNb);
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		CnSuited(int cn, int *arrC, enSuit &ss)
{
	int max = 0;
	ss = S_UNDEF;
	for (int s = S_C; s <= S_S; s++)
	{
		int res = 0;
		for (int i = 0; i < cn; i++)
			if (GetSuitCardNB(arrC[i]) == s)
				res++;
		if (res > max)
		{
			max = res;
			ss = (enSuit)s;
		}
	}
	return max;
}
//---------------------------------------------------------------------------
__int64		BaseNaborTurnHand(int idBoard, int nbTurn0,bool &ns)
{
	int cards[6] = { (idBoard >> 24) & 0x3F, (idBoard >> 18) & 0x3F, (idBoard >> 12) & 0x3F, nbTurn0, (idBoard >> 6) & 0x3F, idBoard & 0x3F };
	enSuit ss;
	int cnS = CnSuited(3, cards, ss);
	ns = false;
	if (cnS == 3)
	{
		if (GetSuitCardNB(cards[3]) != ss)
		{
			SetSuitCardNB(cards[3], S_C);
			ns = true;
		}
	}
	else if (cnS == 2)
	{
		if (GetSuitCardNB(cards[3]) < S_H)
		{
			SetSuitCardNB(cards[3], S_C);
			if (cards[3] == cards[5] || cards[3] == cards[4])
				SetSuitCardNB(cards[3], S_D);
			ns = true;
		}
	}
	return MaxIdTurnHand(cards);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
__int64	MaxIdTurnHand(int *cards)
{
	SortFlop(cards);
	if (cards[4] < cards[5])
	{
		int temp = cards[4];
		cards[4] = cards[5];
		cards[5] = temp;
	}
	__int64 res = 0;
	for (int i = 0; i < 6; i++)
	{
		res = res << 6;
		res += cards[i];
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetTurnIndex(tpCard *arr)
{
	int val = ((int)arr[0]._card << 18) + ((int)arr[1]._card << 12) + ((int)arr[2]._card << 6) + arr[3]._card;
	return glVectRefTurn[val].Number();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetTurnIndex(clCard *arr)
{
	tpCard cards[] = { arr[0].Number(), arr[1].Number(), arr[2].Number(), arr[3].Number() };
	return GetTurnIndex(cards);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetTurnHandIndex(clCard *arr, clHand hand)
{
	tpRefToProperty ref = GetFlopHandRef(arr, hand);
	byte cs = ref.GetChangeSuit();
	clCard turn = arr[3];
	turn.suit = (enSuit)((cs >> 2 * turn.suit) & 3);
	return glVectTurnHandsNb[(size_t)ref.Number()*CN_ALL_CARDS + turn.Number()];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetTurnHandIndex(tpCard *arr, tpHand hand)
{
	tpRefToProperty ref = GetFlopHandRef(arr, hand);
	byte cs = ref.GetChangeSuit();
	tpCard turn = arr[3];
	turn.SetSuit((enSuit)((cs >> 2 * turn.GetSuit()) & 3));
	return glVectTurnHandsNb[(size_t)ref.Number()*CN_ALL_CARDS + turn.Number()];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int MaxIdTurn(int *cards)
{
	SortFlop(cards);
	return (cards[0] << 18) + (cards[1] << 12) + (cards[2] << 6) + cards[3];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		MaxIdRiver(int *cards)
{
	SortFlop(cards);
	return (cards[0] << 24) + (cards[1] << 18) + (cards[2] << 12) + (cards[3] << 6) + cards[4];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	BaseNaborTurn(int id, byte &cs)
{
	int cards[4] = { (id >> 18) & 0x3F, (id >> 12) & 0x3F, (id >> 6) & 0x3F, id & 0x3F };
	int max = MaxIdTurn(cards);
	cs = glArrChangeSuit[0];
	for (int i = 1; i<CN_CHANGE_SUIT; i++)
	{
		char cs0 = glArrChangeSuit[i];
		int c0[4];
		memcpy(c0, cards, sizeof(c0));
		for (int j = 0; j < 4; j++)
		{
			int s = GetSuitCardNB(c0[j]);
			SetSuitCardNB(c0[j], (cs0 >> 2 * s) & 3);
		}
		int val = MaxIdTurn(c0);
		if (val > max)
		{
			max = val;
			cs = cs0;
		}
		else if (val == max)
			if (BetterChange(cs0, cs))
				cs = cs0;
	}
	return max;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int	BaseNaborRiver(int id)
{
	int idTurn = id >> 6;
	byte cs;
	idTurn = BaseNaborTurn(idTurn,cs);
	int card5 = id & 0x3F;
	int s5 = GetSuitCardNB(card5);
	s5=(cs >> 2 * s5) & 3;
	int arrC[5] = { (idTurn >> 18) & 0x3F, (idTurn >> 12) & 0x3F, (idTurn >> 6) & 0x3F, idTurn & 0x3F };

	enSuit ss;
	int cnS = CnSuited(4, arrC, ss);
	if (cnS == 4)
	{
		if (s5 != S_S)
			s5 = S_H;
		SetSuitCardNB(card5, s5);
	}
	else if (cnS == 3)
	{
		if (s5 != ss)
			s5 = S_D;
		SetSuitCardNB(card5, s5);
	}
	else if (cnS == 2)
	{
		if (CnCardSuited(S_D, 4, arrC) == 0)		// масть 2+2
		{
			if (s5 <S_H)
				s5 = S_D;
		}
		else
		{
			if (s5 != ss)
				s5 = S_C;
		}
		SetSuitCardNB(card5, s5);
	}
	else
	{
		s5 = S_S;
		for (; s5 >= S_C; s5--)
		{
			SetSuitCardNB(card5, s5);
			if (card5 != arrC[0] && card5 != arrC[1] && card5 != arrC[2] && card5 != arrC[3])
				break;
		}
	}
	arrC[4] = card5;
	return MaxIdRiver(arrC);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateAllRivers()
{
	size_t sizeA = 0x40000000 * sizeof(int);
	glArrNb5.resize(0x40000000);
	FillMemory(&glArrNb5[0], sizeA, 0xFF);
	glVectRiver.resize(NABOR_CN_RIVER);
	int nb = 0;
	for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
		for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
			for (int i2 = i1 + 1; i2 < CN_ALL_CARDS; i2++)
				for (int iT = 0; iT < CN_ALL_CARDS; iT++)
					if (iT != i1 && iT != i2 && iT != i0)
						for (int iR = 0; iR < CN_ALL_CARDS; iR++)
							if (iR != i1 && iR != i2 && iR != i0 && iR != iT)
							{
								int id = iR + (iT << 6) + (i0 << 12) + (i1 << 18) + (i2 << 24);
								int idB = BaseNaborRiver(id);
								if (glArrNb5[idB] == -1)
								{
									glArrNb5[idB] = nb;
									glVectRiver[nb++].WrProperty(5, idB);
								}
								int val = glArrNb5[idB];
								glVectRiver[val]._cnNabor++;
								glArrNb5[id] = val;
								id = iR + (iT << 6) + (i0 << 12) + (i2 << 18) + (i1 << 24);
								glArrNb5[id] = val;
								id = iR + (iT << 6) + (i1 << 12) + (i0 << 18) + (i2 << 24);
								glArrNb5[id] = val;
								id = iR + (iT << 6) + (i1 << 12) + (i2 << 18) + (i0 << 24);
								glArrNb5[id] = val;
								id = iR + (iT << 6) + (i2 << 12) + (i0 << 18) + (i1 << 24);
								glArrNb5[id] = val;
								id = iR + (iT << 6) + (i2 << 12) + (i1 << 18) + (i0 << 24);
								glArrNb5[id] = val;
							}
	int handle = _open(FILE_NABOR5, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteBigVectToFile <stNaborProperty>(handle, glVectRiver);
		WriteBigVectToFile<int>(handle, glArrNb5);
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t CreateAllRiversHand()
{
	size_t dim = glVectTurnHand.size();
	glVectRiverHandsNb.resize(dim*CN_ALL_CARDS);
	FillMemory(&glVectRiverHandsNb[0], dim*CN_CARD_COL*sizeof(int), 0xFF);
	glVectRiverHandGame.clear();
	glVectRiverHandGame.reserve(1500000000);
	size_t nbCur = 0;
	for (int i = 0; i < dim; i++)
	{
//		if (i % 1000 == 0)
//			glTrasser.WriteMessage(i);
		tpTurnHandProperty &bh = glVectTurnHand[i];
		clCard	cards[5] = { CardNB((bh._idBoardHand >> 30) & 0x3F), CardNB((bh._idBoardHand >> 24) & 0x3F), CardNB((bh._idBoardHand >> 18) & 0x3F), CardNB((bh._idBoardHand >> 12) & 0x3F)};
		int c0[] = { (bh._idBoardHand >> 30) & 0x3F, (bh._idBoardHand >> 24) & 0x3F, (bh._idBoardHand >> 18) & 0x3F, (bh._idBoardHand >> 12) & 0x3F, (bh._idBoardHand >> 6) & 0x3F, bh._idBoardHand & 0x3F };
		vector <tpRiverHandProperty> vect5;
		tpRiverHandProperty prop;
		//prop._nbFlopHand = i;
		//prop._hand = bh._hand;
		size_t sh = i*CN_ALL_CARDS;
		int arrNS[CN_CARD_FACE];
		FillMemory(arrNS, CN_CARD_FACE*sizeof(int), 0xFF);
		for (int i1 = 0; i1 < CN_CARD_COL; i1++)
			if (i1 != c0[0] && i1 != c0[1] && i1 != c0[2] && i1 != c0[3] && i1 != c0[4] && i1 != c0[5])
			{
				bool b0 = false;
				prop._idBoardHand = BaseNaborRiverHand(bh._idBoardHand, i1, b0);
				if (b0)	arrNS[GetFaceCardNB(i1)] = i1;
				int dim = vect5.size(), j = 0;
				for (; j < dim; j++)
					if (prop._idBoardHand == vect5[j]._idBoardHand)
					{
						glVectRiverHandsNb[sh + i1] = nbCur + j;
						break;
					}
				if (j == dim)
				{
					cards[4] = CardNB(i1);
					prop._game.GameHoldemQ(bh.Hand(), cards);
					vect5.push_back(prop);
					glVectRiverHandsNb[sh+i1] = nbCur + dim;
				}
			}
		int dim = vect5.size();
		for (int k = 0; k < dim; k++)
			glVectRiverHandGame.push_back((tpGame24)(vect5[k]._game));
//		glVectRiverHand.insert(glVectRiverHand.end(), vect5.begin(), vect5.end());
		nbCur += vect5.size();
		int nb0 = arrNS[GetFaceCardNB(c0[4])];
		if (nb0 != -1)	glVectRiverHandsNb[sh + c0[4]] = glVectRiverHandsNb[sh + nb0];
		nb0 = arrNS[GetFaceCardNB(c0[3])];
		if (nb0 != -1)	glVectRiverHandsNb[sh + c0[3]] = glVectRiverHandsNb[sh + nb0];
		nb0 = arrNS[GetFaceCardNB(c0[5])];
		if (nb0 != -1)	glVectRiverHandsNb[sh + c0[5]] = glVectRiverHandsNb[sh + nb0];
	}
//	return nbCur;

	int handle = _open(FILE_RIVER_HANDS, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteBigVectToFile<int>(handle, glVectRiverHandsNb);
		_close(handle);
	}
	handle = _open(FILE_RIVER_HANDS_GAME, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteBigVectToFile<tpGame24>(handle, glVectRiverHandGame);
		_close(handle);
	}
	return nbCur;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		CnCardSuited(enSuit s0, int cn, int *cards)
{
	int res = 0;
	for (int i = 0; i < cn; i++)
		if (GetSuitCardNB(cards[i]) == s0)
			res++;
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
__int64		BaseNaborRiverHand(__int64 idBoard, int nbR, bool &ns)
{
	int cards[] = { (idBoard >> 30) & 0x3F, (idBoard >> 24) & 0x3F, (idBoard >> 18) & 0x3F, (idBoard >> 12) & 0x3F, nbR, (idBoard >> 6) & 0x3F, idBoard & 0x3F };
	enSuit ss;
	int cnS = CnSuited(4, cards, ss);
	ns = false;
	if (cnS >= 3)
	{
		if (GetSuitCardNB(cards[4]) != ss)
		{
			SetSuitCardNB(cards[4], S_D);
			if (cards[4] == cards[5] || cards[4] == cards[6] || cards[4] == cards[3])
			{
				SetSuitCardNB(cards[4], S_C);
				if (cards[4] == cards[5] || cards[4] == cards[6] || cards[4] == cards[3])
					SetSuitCardNB(cards[4], S_H);
			}
			ns = true;
		}
	}
	else if (cnS == 2)
	{
		if (CnCardSuited(S_S, 4, cards) == 2 && CnCardSuited(S_H, 4, cards)==2) // 2+2
		{
			if (GetSuitCardNB(cards[4]) < S_H)
			{
				SetSuitCardNB(cards[4], S_D);
				if (cards[4] == cards[5] || cards[4] == cards[6] || cards[4] == cards[3])
					SetSuitCardNB(cards[4], S_C);
				ns = true;
			}
		}
		else if (GetSuitCardNB(cards[4]) != ss)
		{
			for (int s0 = S_C; s0 <= S_S; s0++)
				if (s0 != ss)
				{
					SetSuitCardNB(cards[4], s0);
					if (!(cards[4] == cards[0] || cards[4] == cards[1] || cards[4] == cards[2] || cards[4] == cards[5] || cards[4] == cards[6] || cards[4] == cards[3]))
						break;
				}
			ns = true;
		}
	}
	else
	{
		for (int s0 = S_C; s0 <= S_S; s0++)
		{
			SetSuitCardNB(cards[4], s0);
			if (!(cards[4] == cards[0] || cards[4] == cards[1] || cards[4] == cards[2] || cards[4] == cards[5] || cards[4] == cards[6] || cards[4] == cards[3]))
				break;
		}
		ns = true;
	}
	return MaxIdRiverHand(cards);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
__int64	MaxIdRiverHand(int *cards)
{
	SortFlop(cards);
	if (cards[5] < cards[6])
	{
		int temp = cards[6];
		cards[6] = cards[5];
		cards[5] = temp;
	}
	__int64 res = 0;
	for (int i = 0; i < 7; i++)
	{
		res = res << 6;
		res += cards[i];
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
struct tpThreadEv
{
	bool _stop;
	int _nbFirst, _nbLast, _nbT;
	float *_arrEV;
	clHandsGroupEx *_arrGH;
};

unsigned long _stdcall CalcThreadRiverHandsEV(void *ptr)
{
	time_t t0 = time(NULL);
	tpThreadEv *atr = (tpThreadEv *)ptr;
//	atr->_nbLast = 1000;
	int cnC = 0;
	for (size_t i = atr->_nbFirst; i<atr->_nbLast; i++)
	{
		if (i % 10000 == 0)
			glTrasser.WriteMessage((int)i);
			
		tpTurnHandProperty &bh = glVectTurnHand[i];
		clCard	cards[5] = { CardNB((bh._idBoardHand >> 30) & 0x3F), CardNB((bh._idBoardHand >> 24) & 0x3F), CardNB((bh._idBoardHand >> 18) & 0x3F), CardNB((bh._idBoardHand >> 12) & 0x3F), };
		clHand hand = { CardNB((bh._idBoardHand >> 6) & 0x3F), CardNB(bh._idBoardHand & 0x3F) };
		clCol deck;
		deck.New();
		deck.Skip(cards[0]);
		deck.Skip(cards[1]);
		deck.Skip(cards[2]);
		deck.Skip(cards[3]);
//		deck.Skip(cards[4]);
		deck.Skip(hand[0]);
		deck.Skip(hand[1]);
		int nbT = GetTurnIndex(cards);
		clHandsGroupEx &gh = atr->_arrGH[nbT];
		for (int i1 = 0; i1 < deck.CnCards(); i1++)
		{
			clCol deck1 = deck;
			deck1.Skip(i1);
			cards[4] = deck[i1];
			int nbBH = glVectRiverHandsNb[i*CN_ALL_CARDS + deck[i1].Number()];
			if (nbBH < 0 || nbBH >= CN_NABOR_HAND_RIVER)
			{
				int nb0=i*CN_ALL_CARDS + deck[i1].Number();
				nbBH = glVectRiverHandsNb[i*CN_ALL_CARDS + deck[i1].Number()];
				clAnsiString as = (clAnsiString)"Ай! Пропуск i=" + (clAnsiString)(int)i + (clAnsiString)"; i1=" + (clAnsiString)i1 + (clAnsiString)"; nbBH=" + (clAnsiString)nbBH;
				glTrasser.WriteMessage(as);
			}
			if (atr->_arrEV[nbBH] > -0.5)
			{
				bool b0 = true;
				if (b0)
				continue;
			}
			int game = glVectRiverHandGame[nbBH].ToInt();
			cnC++;
/*			int cnAll = 0, sum = 0;
			for (int i3 = 0; i3 < 44; i3++)
				for (int i4 = i3 + 1; i4 < 45; i4++)
				{
					clHand handOpp = { deck1[i4], deck1[i3] };
					int nbBHOpp = GetRiverHandIndex(cards, handOpp);
					int gameOpp = glVectRiverHandGame[nbBHOpp].ToInt();
					if (game > gameOpp)
						sum += 2;
					else if (game == gameOpp)
						sum += 1;
					cnAll++;
				}
			atr->_arrEV[nbBH] = (float)sum / 2 / cnAll;*/

			float cnAll = 0, sum = 0;
			for (int i3 = 0; i3 < 44; i3++)
				for (int i4 = i3 + 1; i4 < 45; i4++)
				{
					clHand handOpp = { deck1[i4], deck1[i3] };
					int nbH = handOpp.NbHandAll();
					if (gh[nbH] < DOUBLE_0) continue;
					int nbBHOpp = GetRiverHandIndex(cards, handOpp);
					int gameOpp = glVectRiverHandGame[nbBHOpp].ToInt();
					if (game > gameOpp)
						sum += gh[nbH];
					else if (game == gameOpp)
						sum += gh[nbH]/2;
					cnAll += gh[nbH];
				}
			atr->_arrEV[nbBH] = (float)sum / cnAll;
		}
	}
	t0 = time(NULL) - t0;
	atr->_stop = true;
	return 0;
}

int	CalcRiversHandEV()
{
	int handle = _open(FILE_RIVER_HANDS_GAME, O_RDONLY | O_BINARY );
	if (handle != -1)
	{
		ReadBigVectToFile<tpGame24>(handle, glVectRiverHandGame);
		_close(handle);
	}
	else return 0;

	handle = _open(FILE_GH_DIST, O_RDONLY | O_BINARY);
	vector <clHandsGroupEx> vectGH,vectGHFlop;
	clHandsGroupEx gh;
	vectGH.resize(NABOR_CN_TURN);
	vectGHFlop.resize(NABOR_CN_FLOP);
	if (handle != -1)
	{
		gh._vect.resize(ALL_CN_HAND);
		_read(handle, &gh[0], ALL_CN_HAND*sizeof(float));
		for (int i = 0; i < NABOR_CN_FLOP; i++)
		{
			vectGHFlop[i]._vect.resize(ALL_CN_HAND);
			_read(handle, &vectGHFlop[i][0], ALL_CN_HAND*sizeof(float));
		}
		for (int i = 0; i < NABOR_CN_TURN; i++)
		{
			vectGH[i]._vect.resize(ALL_CN_HAND);
			_read(handle, &vectGH[i][0], ALL_CN_HAND*sizeof(float));
		}
		_close(handle);
	}
	else return 0;

	handle = _open(FILE_NABOR4, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadBigVectToFile<tpRefToProperty>(handle, glVectRefTurn);
		ReadBigVectToFile<stNaborProperty>(handle, glVectTurn);
		_close(handle);
	}
	else return 0;

	int dim = glVectRiverHandGame.size();
	vector <float> vectEV;
	vectEV.resize(dim);
//	FillMemory(&vectEV[0], dim*sizeof(float),0xFF);
	for (int i = 0; i < dim; i++)
		vectEV[i] = -1;

	time_t t0 = time(NULL);
	tpThreadEv atr[8];
	int CN_THREAD = 1;
	int cn0 = 0;
	dim = glVectTurnHand.size();
	for (int i = 0; i<CN_THREAD; i++)
	{
		atr[i]._nbFirst = cn0;
		cn0 += (dim - cn0) / (CN_THREAD - i);
		atr[i]._nbLast = cn0;
		atr[i]._arrEV = &vectEV[0];
		atr[i]._arrGH = &vectGH[0];
		atr[i]._nbT = i;
		atr[i]._stop = false;
		CreateThread(NULL, 0, &CalcThreadRiverHandsEV, &atr[i], 0, NULL);
	}
	while (true)
	{
		int j = 0;
		for (; j<CN_THREAD; j++)
			if (!atr[j]._stop)
				break;
		if (j == CN_THREAD)
			break;
		Sleep(100);
	}
	t0 = time(NULL)-t0;

//	return t0;
	handle = _open(FILE_RIVER_HANDS_EV, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteBigVectToFile<float>(handle, vectEV);
		_close(handle);
	}
	else
		ErrMessage("Не открывается файл", FILE_RIVER_HANDS_EV);
	glVectRiverHandGame.clear();
	return t0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetRiverHandIndex(clCard *arr, clHand hand)
{
	tpRefToProperty ref = GetFlopHandRef(arr, hand);
	byte cs = ref.GetChangeSuit();
	clCard card = arr[3];
	card.suit = (enSuit)((cs >> 2 * card.suit) & 3);
	size_t nb = glVectTurnHandsNb[(size_t)ref.Number()*CN_ALL_CARDS + card.Number()];

	card = arr[4];
	card.suit = (enSuit)((cs >> 2 * card.suit) & 3);
	return glVectRiverHandsNb[nb*CN_ALL_CARDS + card.Number()];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcTurnsHandEV()
{
	vector <float> vectEV;
	int handle = _open(FILE_RIVER_HANDS_EV, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadBigVectToFile<float>(handle, vectEV);
		_close(handle);
	}
	else
	{
		ErrMessage("Не открывается файл", FILE_RIVER_HANDS_EV);
		return;
	}

	int dim = glVectTurnHand.size();
	tpFloat arrRes[CN_CARD_COL];
	for (int i = 0; i < dim; i++)
	{
//		glTrasser.WriteMessage(i);
		tpTurnHandProperty &bh = glVectTurnHand[i];
		bh._ev100 = bh._ev50 = 0;
		clCard	cards[5] = { CardNB((bh._idBoardHand >> 30) & 0x3F), CardNB((bh._idBoardHand >> 24) & 0x3F), CardNB((bh._idBoardHand >> 18) & 0x3F), CardNB((bh._idBoardHand >> 12) & 0x3F), };
		clHand hand = bh.Hand();
		clCol deck;
		deck.New();
		deck.Skip(cards[0]);
		deck.Skip(cards[1]);
		deck.Skip(cards[2]);
		deck.Skip(cards[3]);
		deck.Skip(hand[0]);
		deck.Skip(hand[1]);
		for (int j = 0; j < deck.CnCards(); j++)
		{
			cards[4] = deck[j];
			int nbRH = GetRiverHandIndex(cards, hand);
			arrRes[j] = vectEV[nbRH];
			bh._ev100 += arrRes[j];
		}
		bh._ev100 /= deck.CnCards();
		SortShellDef<tpFloat>(deck.CnCards(), arrRes);
		int cn = deck.CnCards() / 2;
		for (int j = deck.CnCards() / 2; j < deck.CnCards(); j++)
			bh._ev50 += arrRes[j];
		bh._ev50 /= (deck.CnCards() - deck.CnCards() / 2);
		bh._ev50 = bh._ev50;
	}

	handle = _open(FILE_TURN_HANDS, O_RDWR | O_BINARY);
	if (handle != -1)
	{
		WriteBigVectToFile<tpTurnHandProperty>(handle, glVectTurnHand);
//		WriteBigVectToFile<int>(handle, glVectTurnHandsNb);
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcFlopsHandEV()
{
	int dim = glVectFlopHand.size();
	tpFloat arrRes[CN_CARD_COL];
	for (int i = 0; i < dim; i++)
	{
		tpFlopHandProperty &th = glVectFlopHand[i];
		clCard cards[5] = { th._board[0], th._board[1], th._board[2] };
		th._ev100 = th._ev50 = 0;
		clCol deck;
		deck.New();
		deck.Skip(th._board[0]);
		deck.Skip(th._board[1]);
		deck.Skip(th._board[2]);
		deck.Skip(th._hand[0]);
		deck.Skip(th._hand[1]);
		for (int j = 0; j < deck.CnCards(); j++)
		{
			cards[3] = deck[j];
			int nbRH = GetTurnHandIndex(cards, th._hand);
			arrRes[j] = glVectTurnHand[nbRH]._ev100;
			th._ev100 += arrRes[j];
		}
		th._ev100 /= deck.CnCards();
		SortShellDef<tpFloat>(deck.CnCards(), arrRes);
		int cn = deck.CnCards() / 2;
		for (int j = deck.CnCards() / 2; j < deck.CnCards(); j++)
			th._ev50 += arrRes[j];
		th._ev50 /= (deck.CnCards() - deck.CnCards() / 2);
		th._ev50 = th._ev50;
	}

	int handle = _open(FILE_FLOP_HANDS, O_RDWR | O_BINARY);
	if (handle != -1)
	{
		size_t sizeA = 0x40000000 * sizeof(tpRefToProperty);
		WriteBigBlock(handle, &glVectRefFlopHand[0], sizeA);
		WriteBigVectToFile <tpFlopHandProperty>(handle, glVectFlopHand);
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcPreflopHandsEV()
{
	clCard cards[5];
	for (int i = 0; i < CN_HAND; i++)
	{
		tpPreflopHands &fh = glArrPreflopHand[i];
		fh._ev100 = fh._ev50 = 0;
		clCol deck;
		deck.New();
		deck.Skip(fh._hand[0]);
		deck.Skip(fh._hand[1]);
		int cnAll = 50 * 49 * 48 / 2 / 3;
		vector <tpFloat> vectRes;
		vectRes.resize(cnAll);
		int nb = 0;
		for (int i1 = 0; i1 < 48; i1++)
		{
			cards[0] = deck[i1];
			for (int i2 = i1 + 1; i2 < 49; i2++)
			{
				cards[1] = deck[i2];
				for (int i3 = i2 + 1; i3 < 50; i3++)
				{
					cards[2] = deck[i3];
					int nbBH = GetFlopHandIndex(cards, fh._hand);
					vectRes[nb] = glVectFlopHand[nbBH]._ev100;
					if (vectRes[nb] < 0 || vectRes[nb]>1)
					{
						tpFlopHandProperty prop = glVectFlopHand[nbBH];
						vectRes[nb] = glVectFlopHand[nbBH]._ev100;
					}
					fh._ev100 += vectRes[nb];
					nb++;
				} 
			}
		}
		fh._ev100 /= cnAll;
		SortShellDef<tpFloat>(cnAll, &vectRes[0]);
		int cn = cnAll / 2;
		for (int j = cn; j < cnAll; j++)
			fh._ev50 += vectRes[j];
		fh._ev50 /= (cnAll - cn);
		fh._ev50 = fh._ev50;
	}
	int handle = _open(FILE_PREFLOP_HANDS, O_RDWR | O_BINARY);
	if (handle != -1)
	{
		WriteBigVectToFile <byte>(handle, glArrNbHand);
		_write(handle, &glArrPreflopHand, sizeof(glArrPreflopHand));
		_close(handle);
	}
	tpFloat res100[CN_HAND];
	for (int i = 0; i < CN_HAND; i++)
	{
		res100[i] = 0;
		tpPreflopHands &fh = glArrPreflopHand[i];
		clCol deck;
		deck.New();
		deck.Skip(fh._hand[0]);
		deck.Skip(fh._hand[1]);
		int cnAll = 50 * 49 / 2;
		vector <tpFloat> vectRes;
		vectRes.resize(cnAll);
		int nb = 0;
		clHand hand;
		for (int i1 = 0; i1 < 49; i1++)
		{
			hand[0] = deck[i1];
			for (int i2 = i1 + 1; i2 < 50; i2++)
			{
				hand[1] = deck[i2];
				int nbH = hand.NbHand();
				tpWin2  w2 = HCompWinHand169(i, nbH);
				vectRes[nb] = w2.WinEq();
				res100[i] += vectRes[nb];
				nb++;
			}
		}
		res100[i] /= cnAll;
		if (fabs(fh._ev100-res100[i])>0.001)
			fh._ev100 = fh._ev100;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	TestFlopHandNabors()
{
	size_t dim = glVectFlopHand.size();
	for (size_t i = 0; i < dim; i++)
	{
		if (glVectFlopHand[i]._ev100 < 0 || glVectFlopHand[i]._ev100>1)
			dim = dim;
		if (glVectFlopHand[i]._ev50 < 0 || glVectFlopHand[i]._ev50>1)
			dim = dim;
	}
	return;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	TestTurnHandNabors()
{
	size_t dim = glVectTurnHand.size();
	for (size_t i = 0; i < dim; i++)
	{
		if (glVectTurnHand[i]._ev100 < 0 || glVectTurnHand[i]._ev100>1)
			dim = dim;
		if (glVectTurnHand[i]._ev50 < 0 || glVectTurnHand[i]._ev50>1)
			dim = dim;
	}
	return;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	TestRiverHandNabors()
{
	vector <float> vectEV;
	int handle = _open(FILE_RIVER_HANDS_EV, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadBigVectToFile<float>(handle, vectEV);
		_close(handle);
	}
	else
	{
		ErrMessage("Не открывается файл", FILE_RIVER_HANDS_EV);
		return;
	}
	size_t dim = vectEV.size();
	for (size_t i = 0; i < dim; i++)
	{
		if (vectEV[i] < 0 || vectEV[i]>1)
			vectEV[i] = vectEV[i];
		if (_isnan(vectEV[i]))
		{
			float val = vectEV[i];
			vectEV[i] = vectEV[i];
		}
	}
	return;

	for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
		for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
			for (int i2 = i1 + 1; i2 < CN_ALL_CARDS; i2++)
				for (int i3 = i2 + 1; i3 < CN_ALL_CARDS; i3++)
					for (int i4 = i3 + 1; i4 < CN_ALL_CARDS; i4++)
						for (int iT = 0; iT < CN_ALL_CARDS; iT++)
							if (iT != i1 && iT != i2 && iT != i0 && iT != i3 && iT != i4)
								for (int iR = 0; iR < CN_ALL_CARDS; iR++)
									if (iR != i1 && iR != i2 && iR != i0  && iR != i3 && iR != i4 && iR != iT)
									{
										clCard card[] = { CardNB(i0), CardNB(i1), CardNB(i2), CardNB(i3), CardNB(i4) };
										clHand hand = { CardNB(iT), CardNB(iR) };
										int nb = GetRiverHandIndex(card, hand);
										if (nb < 0 || nb >= CN_NABOR_HAND_RIVER)
										{
											clAnsiString as="board=";
											for (int i = 0; i < 5; i++)
												as += card[i].Name();
											as += "hand=";
											as += hand[0].Name();
											as += hand[1].Name();
											glTrasser.WriteMessage(as);
											nb = GetRiverHandIndex(card, hand);
										}
									}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcForDist()
{
	clHandsGroupEx gh;
	gh.FullPreFlopAllHands();
	int handle = _open(FILE_PREFLOP_HANDS, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadBigVectToFile <byte>(handle, glArrNbHand);
		_read(handle, &glArrPreflopHand, sizeof(glArrPreflopHand));
		_close(handle);
	}
	else return;
	handle = _open(FILE_NABOR4, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadBigVectToFile<tpRefToProperty>(handle, glVectRefTurn);
		ReadBigVectToFile<stNaborProperty>(handle, glVectTurn);
		_close(handle);
	}
	else return;

	double arr[ALL_CN_HAND];
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		clHand hand;
		hand.SetHandAll(i);
		int nb = GetHandIndex(hand);
		arr[i] = glArrPreflopHand[nb]._ev50;
	}
	double disp, aver = AverageAndDisp(ALL_CN_HAND, arr, disp);
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		float ev = (arr[i]-aver)/disp/1.3;
		if (ev<1 && ev>-1)
			gh[i] *= (ev + 1) / 2;
		else if (ev < -1)
			gh[i] = 0; 
	}
	gh.Norm();
/*	struct st1
	{
		clHand _hand;
		float _weight, _k,_ev;
	} ghA[CN_HAND];
	for (int i = 0; i < CN_HAND; i++)
		ghA[i]._weight = 0;
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		clHand hand;
		hand.SetHandAll(i);
		int nb = hand.NbHand();
		ghA[nb]._weight+=gh[i];
		ghA[nb]._hand = hand;
	}
	for (int i = 0; i < CN_HAND; i++)
	{
		ghA[i]._k = ghA[i]._weight / HandWeight(i);
		ghA[i]._ev = glArrPreflopHand[i]._ev50;
	}*/
	vector <clHandsGroupEx> vectGH,vectGHFlop;
	vectGH.resize(NABOR_CN_TURN);
	vectGHFlop.resize(NABOR_CN_FLOP);
	int dim = glVectTurnHand.size();
	for (int i = 0; i < dim; i++)
	{
		tpTurnHandProperty &bh = glVectTurnHand[i];
		tpCard	cards[5] = { (bh._idBoardHand >> 30) & 0x3F, (bh._idBoardHand >> 24) & 0x3F, (bh._idBoardHand >> 18) & 0x3F, (bh._idBoardHand >> 12) & 0x3F};
		tpHand hand = { (bh._idBoardHand >> 6) & 0x3F, bh._idBoardHand & 0x3F };
		clCol deck;
		deck.New();
		deck.Skip(cards[0]);
		deck.Skip(cards[1]);
		deck.Skip(cards[2]);
		deck.Skip(cards[3]);
		//		deck.Skip(cards[4]);
		deck.Skip(hand[0]);
		deck.Skip(hand[1]);
		int nbT = GetTurnIndex(cards);
		clHandsGroupEx &gh = atr->_arrGH[nbT];
	}





	for (int i = 0; i < NABOR_CN_FLOP; i++)
	{
		glTrasser.WriteMessage(i);
		stNaborProperty &flop = glVectFlop[i];
		clHandsGroupEx ghF=gh;
		tpCard cards[5] = { flop._cards[0].Number(), flop._cards[1].Number(), flop._cards[2].Number() };
		tpBooleanDeck deck;
		deck.Skip(cards[0]);
		deck.Skip(cards[1]);
		deck.Skip(cards[2]);
		for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
			for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
			{
				tpHand hand(i1, i0);
				int nb = hand.NbHandAll();
				if (deck[i0] && deck[i1])
				{
					int nbBH = GetFlopHandIndex(cards, hand);
					arr[nb] = glVectFlopHand[nbBH]._ev50;
				}
				else
				{
					arr[nb] = 0;
					ghF[nb] = 0;
				}
			}
		aver = AverageAndDisp0(ALL_CN_HAND, arr, ghF,disp);
		for (int j = 0; j < ALL_CN_HAND; j++)
		{
			float ev = (arr[j] - aver) / disp;
			if (ev<1 && ev>-1)
				ghF[j] *= (ev + 1) / 2;
			else if (ev < -1)
				ghF[j] = 0;
		}
		ghF.Norm();
		vectGHFlop[i] = ghF;
		for (int iT = 0; iT < CN_ALL_CARDS; iT++)
			if (deck[iT])
			{
				int id = (flop._id<<6)+iT;
				byte cs;
				int idB = BaseNaborTurn(id, cs);
				if (id == idB)
				{
					clHandsGroupEx ghT = ghF;
					tpBooleanDeck deckT = deck;
					deckT.Skip(iT);
					cards[3] = iT;
					for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
						for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
						{
							tpHand hand(i1, i0);
							int nb = hand.NbHandAll();
							if (deckT[i0] && deckT[i1])
							{
								int nbBH = GetTurnHandIndex(cards, hand);
								if (nbBH < 0 || nbBH >= CN_NABOR_HAND_TURN)
									glTrasser.WriteMessage(i0*100+i1);
								arr[nb] = glVectTurnHand[nbBH]._ev50;
							}
							else
							{
								arr[nb] = 0;
								ghT[nb] = 0;
							}
						}
					aver = AverageAndDisp0(ALL_CN_HAND, arr, ghT, disp);
					for (int j = 0; j < ALL_CN_HAND; j++)
					{
						float ev = (arr[j] - aver) / disp;
						if (ev<1 && ev>-1)
							ghT[j] *= (ev + 1) / 2;
						else if (ev < -1)
							ghT[j] = 0;
					}
					ghT.Norm(); 
					vectGH[glVectRefTurn[idB].Number()] = ghT;
				}
			}
	}
	for (int i = 0; i < NABOR_CN_TURN; i++)
		if (vectGH[i]._vect.size() != ALL_CN_HAND)
			vectGH[i][0];

	handle = _open(FILE_GH_DIST, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		_write(handle, &gh[0], ALL_CN_HAND*sizeof(float));
		for (int i = 0; i < NABOR_CN_FLOP; i++)
			_write(handle, &vectGHFlop[i][0], ALL_CN_HAND*sizeof(float));
		for (int i = 0; i < NABOR_CN_TURN; i++)
			_write(handle, &vectGH[i][0], ALL_CN_HAND*sizeof(float));
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double AverageAndDisp0(int cn, double *arr, clHandsGroupEx &ghF, double &disp)
{
	disp = 0;
	if (cn == 0) return 0;
	double aver = 0,sum=0;
	for (int i = 0; i < cn; i++)
	{
		aver += arr[i] * ghF[i];
		sum += ghF[i];
	}
	if (sum < DOUBLE_0)
		return 0;
	aver /= sum;
	for (int i = 0; i<cn; i++)
	{
		double x = arr[i] - aver;
		disp += x*x*ghF[i];
	}
	disp = sqrt(disp / cn/sum);
	return aver;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//#pragma package(smart_init)
