//---------------------------------------------------------------------------

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
#include "c:\Home\Util\TemplateFileRec.h"

#ifndef COMP_C 
#define	FILE_PREFLOP_HANDS	"d:\\home\\bin\\BoardHands\\PreflopHands.bin"
#define	FILE_NABOR3			"d:\\home\\bin\\BoardHands\\Flop.bin"
#define	FILE_NABOR4			"d:\\home\\bin\\BoardHands\\Turn.bin"
#define	FILE_FLOP_HANDS		"d:\\home\\bin\\BoardHands\\FlopHands.bin"
#define	FILE_TURN_HANDS		"d:\\home\\bin\\BoardHands\\TurnHands.bin"
#define	FILE_HAND_TO_FLOP	"d:\\home\\bin\\BoardHands\\HandsToFlop.bin"
#define	FILE_NABOR5			"d:\\home\\bin\\BoardHands\\River.bin"
#define	FILE_RIVER_HANDS		"d:\\home\\bin\\BoardHands\\RiverHands.bin"
#define	FILE_RIVER_HANDS_GAME	"d:\\home\\bin\\BoardHands\\RiverHandsGame.bin"
#define	FILE_RIVER_HANDS_ID	"d:\\home\\bin\\BoardHands\\RiverHandsId.bin"
#define	FILE_RIVER_HANDS_EV		"d:\\home\\bin\\BoardHands\\RiverHandsEV.bin"
#define	FILE_BOARD_HANDS_EV		"d:\\home\\bin\\BoardHands\\BoardHandsEV.bin"
#define	FILE_GH_DIST		"d:\\home\\bin\\BoardHands\\DistGH.bin"
#else
#define	FILE_PREFLOP_HANDS	"c:\\home\\bin\\BoardHands\\PreflopHands.bin"	//
#define	FILE_NABOR3			"c:\\home\\bin\\BoardHands\\Flop.bin"			//
#define	FILE_NABOR4			"c:\\home\\bin\\BoardHands\\Turn.bin"			//
#define	FILE_FLOP_HANDS		"c:\\home\\bin\\BoardHands\\FlopHands.bin"		//
#define	FILE_TURN_HANDS		"c:\\home\\bin\\BoardHands\\TurnHands.bin"		//
#define	FILE_HAND_TO_FLOP	"c:\\home\\bin\\BoardHands\\HandsToFlop.bin"
#define	FILE_NABOR5			"c:\\home\\bin\\BoardHands\\River.bin"
#define	FILE_RIVER_HANDS		"c:\\home\\bin\\BoardHands\\RiverHands.bin"	//
#define	FILE_RIVER_HANDS_GAME	"c:\\home\\bin\\BoardHands\\RiverHandsGame.bin"	//
#define	FILE_RIVER_HANDS_ID	"c:\\home\\bin\\BoardHands\\RiverHandsId.bin"
#define	FILE_RIVER_HANDS_EV		"c:\\home\\bin\\BoardHands\\RiverHandsEV.bin"
#define	FILE_BOARD_HANDS_EV		"c:\\home\\bin\\BoardHands\\BoardHandsEV.bin"	//
#define	FILE_GH_DIST		"c:\\home\\bin\\BoardHands\\DistGH.bin"
#endif

struct tpRefToProperty
{
	byte	GetChangeSuit()				{ return _val >> 24; }
	void	CreateRef(byte cs, int nb)	{ _val = (((int)cs) << 24) + nb; }
	int		Number()					{ return _val & 0xFFFFFF; }

	int _val;
};

struct tpId7
{
	tpId7()					{ Clear(); }
	tpId7(__int64 val)		{ Clear(); SetId(val); }
	void	Clear()			{ _id[0] = _id[1] = _id[2] = _id[3] = _id[4] = _id[5] = 0; }
	void	SetId(__int64 val);
	__int64 GetId();

	byte _id[6];
};

struct tpBoardHand
{
	clCard _cards[5];
	clHand _hand;
	float _ev50, _ev100;
};
tpPreflopHands glArrPreflopHand[CN_HAND];
vector <byte> glArrNbHand;

vector <tpRefToProperty> glVectRefFlop;
vector <stNaborProperty> glVectFlop;
vector <int> glVectFlopHandsNb;
vector <tpFlopHandProperty> glVectFlopHand;

vector <tpRefToProperty> glVectRefTurn;
vector <stNaborProperty> glVectTurn;
vector <int> glVectTurnHandsNb;
vector <tpTurnHandProperty> glVectTurnHand;

//vector <int> glArrNb5;
//vector <stNaborProperty> glVectRiver;
vector <int> glVectRiverHandsNb;
//vector <tpRiverHandProperty> glVectRiverHand;
vector <tpGame24> glVectRiverHandGame;
vector <tpId7> glVectRiverHandId;

vector <float> glVectEV50, glVectEV100, glVectFlopEV50, glVectFlopEV100, glVectTurnEV50, glVectTurnEV100,glVectRiverEV;

byte glArrChangeSuit[] = { 0xE4, 0xB4, 0xD8, 0x78, 0x9C, 0x6C, 0xE1, 0xB1, 0xC9, 0x39, 0x8D, 0x2D, 0xD2, 0x72, 0xC6, 0x36, 0x4E, 0x1E, 0x93, 0x63, 0x87, 0x27, 0x4B, 0x1B };

void	ChangeSuit(tpCard &c, byte cs)
{
	byte s = c._card & 3;
	c._card = (c._card & 60) + ((cs >> 2 * s) & 3);
}

int GetFaceCardNB(int nb)				{ return nb / CN_CARD_SUIT; }
int GetSuitCardNB(int nb)				{ return nb % CN_CARD_SUIT; }
void	SetSuitCardNB(int &nb, int s)			{ nb = nb / CN_CARD_SUIT *CN_CARD_SUIT + s; }
bool	BetterChange(byte cs0, byte cs);
int		CnSuited(int cn, int *arrC, enSuit &ss);
int		CnCardSuited(enSuit s0, int cn, int *cards);
int		BaseNaborFlop(int id, byte &cs);
int		BaseNaborFlopHand(int idBoard, int nbC1, int nbC2);
int		MaxIdFlopHand(int *cards);
int		BaseNaborTurn(int id, byte &cs);
__int64		BaseNaborTurnHand(int idBoard, tpHand hand);
__int64		MaxIdTurnHand(int *cards);
__int64		BaseNaborRiverHand(__int64 idBoard, int nbR, bool &ns);
void	ChangeSuitMask(tpCard &card, byte cs);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	ChangeSuitMask(tpCard &card, byte cs)
{
	card.SetSuit((enSuit)((cs >> 2 * (int)card.GetSuit()) & 3));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpTurnHandProperty::TpHand(tpHand &hand)
{
	hand[0] = (_idBoardHand >> 6) & 0x3F;
	hand[1] = _idBoardHand & 0x3F;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpTurnHandProperty::GetBoard(tpCard *c0)
{
	c0[0] = (_idBoardHand >> 30) & 0x3F;
	c0[1] = (_idBoardHand >> 24) & 0x3F;
	c0[2] = (_idBoardHand >> 18) & 0x3F;
	c0[3] = (_idBoardHand >> 12) & 0x3F;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clHand tpTurnHandProperty::Hand()
{
	clHand hand = { CardNB((_idBoardHand >> 6) & 0x3F), CardNB(_idBoardHand & 0x3F) };
	return hand;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpTurnHandProperty::GetBoard(clCard *c0)
{
	c0[0] = CardNB((_idBoardHand >> 30) & 0x3F);
	c0[1] = CardNB((_idBoardHand >> 24) & 0x3F);
	c0[2] = CardNB((_idBoardHand >> 18) & 0x3F);
	c0[3] = CardNB((_idBoardHand >> 12) & 0x3F);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpId7::SetId(__int64 id)
{
	memcpy(_id, &id, 6);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
__int64 tpId7::GetId()
{
	__int64 id=0;
	memcpy((byte *)&id, _id, 6);
	return id;
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
clAnsiString	tpFlopHandProperty::ToString()
{
//	char str[16];
	clAnsiString as = "Hand ";
	as += _hand[0].Name();
	as += _hand[1].Name();
	as += " Board ";
	as += _board[0].Name();
	as += _board[1].Name();
	as += _board[2].Name();
	return as;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	LoadDataHandsBoard(int reg)
{
	int handle = 0;

	handle = _open(FILE_PREFLOP_HANDS, O_RDONLY | O_BINARY);
	if (handle == -1) return false;
	ReadVectFile <byte>(handle, glArrNbHand);
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
	ReadVectFile<tpFlopHandProperty>(handle, glVectFlopHand);
	ReadVectFile<int>(handle, glVectFlopHandsNb);
	_close(handle);

	handle = _open(FILE_NABOR4, O_RDONLY | O_BINARY);
	if (handle == -1) return false;
	ReadVectFile<tpRefToProperty>(handle, glVectRefTurn);
	ReadVectFile<stNaborProperty>(handle, glVectTurn);
	_close(handle);

	handle = _open(FILE_TURN_HANDS, O_RDONLY | O_BINARY);
	if (handle == -1) return false;
	ReadVectFile<tpTurnHandProperty>(handle, glVectTurnHand);
	ReadVectFile<int>(handle, glVectTurnHandsNb);
	_close(handle);

	handle = _open(FILE_RIVER_HANDS, O_RDONLY | O_BINARY);
	if (handle == -1) return false;
	ReadVectFile<int>(handle, glVectRiverHandsNb);
	_close(handle);

	handle = _open(FILE_RIVER_HANDS_GAME, O_RDONLY | O_BINARY);
	if (handle == -1) return false;
	ReadVectFile<tpGame24>(handle, glVectRiverHandGame);
	_close(handle);

	/*if (reg & REG_LOAD_FORCALC_TREE)
	{
		handle = _open(FILE_RIVER_HANDS_ID, O_RDONLY | O_BINARY);
		if (handle == -1) return false;
		ReadVectFile<tpId7>(handle, glVectRiverHandId);
		_close(handle);

		handle = _open(FILE_RIVER_HANDS_EV, O_RDONLY | O_BINARY);
		if (handle != -1)
		{
			ReadVectFile<float>(handle, glVectRiverEV);
			_close(handle);
		}
	}*/

//	if (reg == REG_LOAD_FORCALC_NABOR)
	{
		handle = _open(FILE_BOARD_HANDS_EV, O_RDONLY | O_BINARY);
		if (handle != -1)
		{
			ReadVectFile<float>(handle, glVectTurnEV50);
			ReadVectFile<float>(handle, glVectTurnEV100);
			ReadVectFile<float>(handle, glVectFlopEV50);
			ReadVectFile<float>(handle, glVectFlopEV100);
			ReadVectFile<float>(handle, glVectEV50);
			ReadVectFile<float>(handle, glVectEV100);
			_close(handle);
		}
	}

	return true;
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
		WriteVectFile <byte>(handle, glArrNbHand);
		_write(handle, &glArrPreflopHand, sizeof(glArrPreflopHand));
		_close(handle);
	}
}
//---------------------------------------------------------------------------
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
tpRefToProperty		GetRefFlop(clCard *arr)
{
	int id = (arr[0].Number() << 12) + (arr[1].Number() << 6) + arr[2].Number();
	return glVectRefFlop[id];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetFlopIndex(tpCard *arr)
{
	int val = ((int)arr[0]._card << 12) + ((int)arr[1]._card << 6) + arr[2]._card;
	return glVectRefFlop[val].Number();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetFlopIndex(clCard *arr)
{
	tpCard cards[] = { arr[0].Number(), arr[1].Number(), arr[2].Number() };
	return GetFlopIndex(cards);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetFlopIndex(clCard *arr, byte &suit)
{
	int val = ((int)arr[0].Number() << 12) + ((int)arr[1].Number() << 6) + arr[2].Number();
	tpRefToProperty res = glVectRefFlop[val];
	suit = res.GetChangeSuit();
	return res.Number();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetFlopHandIndex(clCard *arr, clHand hand)
{
	tpRefToProperty nbFlop = GetRefFlop(arr);
	byte cs = nbFlop.GetChangeSuit();
	hand[0].suit = (enSuit)((cs >> 2 * hand[0].suit) & 3);
	hand[1].suit = (enSuit)((cs >> 2 * hand[1].suit) & 3);
	int nbHand = hand.NbHandAll();
	int nbV = nbFlop.Number()*ALL_CN_HAND + nbHand;
#ifdef  ERR_MESSAGE
	if (nbV < 0 || nbV >= glVectFlopHandsNb.size())
	{
		ErrMessage("GetFlopHandIndex ", (clAnsiString)nbV);
		nbV = 0;
	}
#endif
	return glVectFlopHandsNb[nbV];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetFlopHandIndex(tpCard *arr, tpHand hand)
{
	int val = ((int)arr[0]._card << 12) + ((int)arr[1]._card << 6) + arr[2]._card;
	tpRefToProperty ref = glVectRefFlop[val];
	byte cs = ref.GetChangeSuit();
	ChangeSuitMask(hand[0], cs);
	ChangeSuitMask(hand[1], cs);
	int nbHand = hand.NbHandAll();
	int nbV = ref.Number()*ALL_CN_HAND + nbHand;
#ifdef  ERR_MESSAGE
	if (nbV < 0 || nbV >= glVectFlopHandsNb.size())
	{
		ErrMessage("GetFlopHandIndex ", (clAnsiString)nbV);
		nbV = 0;
	}
#endif
	return glVectFlopHandsNb[nbV];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetFlopHandIndexFromTurnHandId(__int64 id)
{
	int val = int(id>>18);
	tpRefToProperty ref = glVectRefFlop[val];
	byte cs = ref.GetChangeSuit();
	tpHand hand = { (tpCard)((id >> 6) & 0x3F), id & 0x3F };
	ChangeSuitMask(hand[0], cs);
	ChangeSuitMask(hand[1], cs);
	int nbHand = hand.NbHandAll();
	return glVectFlopHandsNb[ref.Number()*ALL_CN_HAND + nbHand];
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
bool	BetterChange(byte cs0, byte cs)
{
	int cn = 0, cn0 = 0;
	for (int i = 0; i < 4; i++)
	{
		if (((cs >> 2 * i) & 3) == i)
			cn++;
		if (((cs0 >> 2 * i) & 3) == i)
			cn0++;
	}
	return cn0 > cn;
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
int		BaseNaborFlopHand(int idBoard, int nbC1, int nbC2)
{
	int cards[5] = { (idBoard >> 12) & 0x3F, (idBoard >> 6) & 0x3F, idBoard & 0x3F };
	cards[3] = (nbC1>nbC2) ? nbC1 : nbC2;
	cards[4] = (nbC1>nbC2) ? nbC2 : nbC1;
	int max = MaxIdFlopHand(cards);
	char cs = glArrChangeSuit[0];
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
//---------------------------------------------------------------------------
void CreateAllFlopsHand()
{
	glVectFlopHandsNb.resize(NABOR_CN_FLOP*ALL_CN_HAND);
	FillMemory(&glVectFlopHandsNb[0], NABOR_CN_FLOP*ALL_CN_HAND*sizeof(int), 0xFF);
	glVectFlopHand.clear();
	int nbCur = 0;
	for (int i = 0; i < NABOR_CN_FLOP; i++)
	{
		stNaborProperty &flop = glVectFlop[i];
		clCol deck;
		deck.New();
		deck.Skip(flop._cards[0]);
		deck.Skip(flop._cards[1]);
		deck.Skip(flop._cards[2]);
		vector <tpFlopHandProperty> vect5;
		tpFlopHandProperty prop;
		int curi = i*ALL_CN_HAND;
		for (int i1 = 0; i1 < 48; i1++)
			for (int i2 = i1 + 1; i2 < 49; i2++)
			{
				prop._hand[0] = deck[i1];
				prop._hand[1] = deck[i2];
				prop._idBoardHand = BaseNaborFlopHand(flop._id, deck[i1].Number(), deck[i2].Number());
				int dim = (int)vect5.size(), j = 0, nbHand = prop._hand.NbHandAll();
				for (; j < dim; j++)
					if (prop._idBoardHand == vect5[j]._idBoardHand)
					{
						glVectFlopHandsNb[curi+nbHand] = nbCur + j;
						break;
					}
				if (j == dim)
				{
					prop._board[0] = CardNB((prop._idBoardHand >> 24) & 0x3F);
					prop._board[1] = CardNB((prop._idBoardHand >> 18) & 0x3F);
					prop._board[2] = CardNB((prop._idBoardHand >> 12) & 0x3F);
					prop._hand[0] = CardNB((prop._idBoardHand >> 6) & 0x3F);
					prop._hand[1] = CardNB(prop._idBoardHand & 0x3F);
					vect5.push_back(prop);
					glVectFlopHandsNb[curi+nbHand] = nbCur + dim;
				}
			}
		glVectFlopHand.insert(glVectFlopHand.end(), vect5.begin(), vect5.end());
		nbCur += (int)vect5.size();
	}
	int handle = _open(FILE_FLOP_HANDS, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteVectFile<tpFlopHandProperty>(handle, glVectFlopHand);
		WriteVectFile<int>(handle, glVectFlopHandsNb);
		_close(handle);
	}
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
		WriteVectFile<tpRefToProperty>(handle, glVectRefTurn);
		WriteVectFile<stNaborProperty>(handle, glVectTurn);
		_close(handle);
	}
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
__int64		BaseNaborTurnHand(int idBoard, tpHand hand)
{
	int cards[6] = { (idBoard >> 18) & 0x3F, (idBoard >> 12) & 0x3F, (idBoard >> 6) & 0x3F, idBoard & 0x3F };
	cards[4] = hand[0]._card;
	cards[5] = hand[1]._card;
	bool bs = GetSuitCardNB(cards[4]) == GetSuitCardNB(cards[5]);
	enSuit ss;
	int cnS = CnSuited(3, cards, ss);
	if (cnS == 3)
	{
		if (GetSuitCardNB(cards[4]) != ss)
			SetSuitCardNB(cards[4], (bs)? S_C : S_D);
		if (GetSuitCardNB(cards[5]) != ss)
			SetSuitCardNB(cards[5], S_C);
	}
	else if (cnS == 2)
	{
		if (bs)
		{
			if (GetSuitCardNB(cards[4]) < S_H)
			{
				SetSuitCardNB(cards[4], S_C);
				SetSuitCardNB(cards[5], S_C);
			}
		}
		else if (GetSuitCardNB(cards[4]) < S_H && GetSuitCardNB(cards[5]) < S_H)
		{
			SetSuitCardNB(cards[4], S_D);
			if (cards[4] == cards[3])
			{
				SetSuitCardNB(cards[4], S_C);
				SetSuitCardNB(cards[5], S_D); 
			}
			else
				SetSuitCardNB(cards[5], S_C);
		}
		else
		{
			if (GetSuitCardNB(cards[4]) < S_H)
				SetSuitCardNB(cards[4], S_C);
			if (GetSuitCardNB(cards[5]) < S_H)
				SetSuitCardNB(cards[5], S_C);
		}
	}
	return MaxIdTurnHand(cards);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateAllTurnsHand()
{
	glVectTurnHandsNb.resize(NABOR_CN_TURN*ALL_CN_HAND);
	FillMemory(&glVectTurnHandsNb[0], NABOR_CN_TURN*ALL_CN_HAND*sizeof(int), 0xFF);
	glVectTurnHand.clear();
	int nbCur = 0;
	for (size_t i = 0; i < NABOR_CN_TURN; i++)
	{
		stNaborProperty &turn = glVectTurn[i];
		clCol deck;
		deck.New();
		deck.Skip(turn._cards[0]);
		deck.Skip(turn._cards[1]);
		deck.Skip(turn._cards[2]);
		deck.Skip(turn._cards[3]);
		vector <tpTurnHandProperty> vect5;
		tpTurnHandProperty prop;
		size_t curi = i*ALL_CN_HAND;
		for (int i1 = 0; i1 < 47; i1++)
			for (int i2 = i1 + 1; i2 < 48; i2++)
			{
				tpHand hand(deck[i2], deck[i1]);
				prop._idBoardHand = BaseNaborTurnHand(turn._id, hand);
				int dim = (int)vect5.size(), j = 0, nbHand = hand.NbHandAll();
				for (; j < dim; j++)
					if (prop._idBoardHand == vect5[j]._idBoardHand)
					{
						glVectTurnHandsNb[curi+nbHand] = nbCur + j;
						break;
					}
				if (j == dim)
				{
					vect5.push_back(prop);
					glVectTurnHandsNb[curi+nbHand] = nbCur + dim;
				}
			}
		glVectTurnHand.insert(glVectTurnHand.end(), vect5.begin(), vect5.end());
		nbCur += (int)vect5.size();
	}
	int handle = _open(FILE_TURN_HANDS, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteVectFile<tpTurnHandProperty>(handle, glVectTurnHand);
		WriteVectFile<int>(handle, glVectTurnHandsNb);
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpRefToProperty		GetRefTurn(clCard *arr)
{
	int id = (arr[0].Number() << 18) + (arr[1].Number() << 12) + (arr[2].Number() << 6) + arr[3].Number();
	return glVectRefTurn[id];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetTurnHandIndex(clCard *arr, clHand hand)
{
	tpRefToProperty nbTurn = GetRefTurn(arr);
	byte cs = nbTurn.GetChangeSuit();
	hand[0].suit = (enSuit)((cs >> 2 * hand[0].suit) & 3);
	hand[1].suit = (enSuit)((cs >> 2 * hand[1].suit) & 3);
	int nbHand = hand.NbHandAll();
	return glVectTurnHandsNb[nbTurn.Number()*ALL_CN_HAND+nbHand];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetTurnHandIndex(tpCard *arr, tpHand hand)
{
	int val = ((int)arr[0]._card << 18) + ((int)arr[1]._card << 12) + ((int)arr[2]._card << 6) + arr[3]._card;
	tpRefToProperty ref = glVectRefTurn[val];
	byte cs = ref.GetChangeSuit();
	ChangeSuitMask(hand[0], cs);
	ChangeSuitMask(hand[1], cs);
	int nbHand = hand.NbHandAll();
	return glVectTurnHandsNb[ref.Number()*ALL_CN_HAND + nbHand];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
tpIndex	GetTurnHandIndexFromRiverHandIndex(tpIndex nb1)
{
	__int64 id = glVectRiverHandId[nb1].GetId();
	unsigned val = unsigned(id>>18);
	tpRefToProperty ref = glVectRefTurn[val];
	byte cs = ref.GetChangeSuit();
	tpHand hand = { (id >> 6) & 0x3F, id & 0x3F };
	ChangeSuitMask(hand[0], cs);
	ChangeSuitMask(hand[1], cs);
	int nbHand = hand.NbHandAll();
	return glVectTurnHandsNb[ref.Number()*ALL_CN_HAND + nbHand];
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
	else if (cnS == 2)
	{
		if (CnCardSuited(S_S, 4, cards) == 2 && CnCardSuited(S_H, 4, cards) == 2) // 2+2
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
struct tpRiverHandProperty0
{
	__int64	_idBoardHand;
	clGamePoker _game;
};
 
void CreateAllRivers()
{
}

size_t CreateAllRiversHand()
{
	size_t dim = glVectTurnHand.size();
	glVectRiverHandsNb.resize(dim*CN_ALL_CARDS);
	FillMemory(&glVectRiverHandsNb[0], dim*CN_CARD_COL*sizeof(int), 0xFF);
	glVectRiverHandGame.clear();
	glVectRiverHandGame.reserve(1500000000);
	glVectRiverHandId.clear();
	glVectRiverHandId.reserve(1500000000);
	size_t nbCur = 0;
	for (size_t i = 0; i < dim; i++)
	{
		//		if (i % 1000 == 0)
		//			glTrasser.WriteMessage(i);
		tpTurnHandProperty &bh = glVectTurnHand[i];
		clCard	cards[5] = { CardNB((bh._idBoardHand >> 30) & 0x3F), CardNB((bh._idBoardHand >> 24) & 0x3F), CardNB((bh._idBoardHand >> 18) & 0x3F), CardNB((bh._idBoardHand >> 12) & 0x3F) };
		int c0[] = { (bh._idBoardHand >> 30) & 0x3F, (bh._idBoardHand >> 24) & 0x3F, (bh._idBoardHand >> 18) & 0x3F, (bh._idBoardHand >> 12) & 0x3F, (bh._idBoardHand >> 6) & 0x3F, bh._idBoardHand & 0x3F };
		vector <tpRiverHandProperty0> vect5;
		tpRiverHandProperty0 prop;
		size_t sh = i*CN_ALL_CARDS;
		int arrNS[CN_CARD_FACE];
		FillMemory(arrNS, CN_CARD_FACE*sizeof(int), 0xFF);
		for (int i1 = 0; i1 < CN_CARD_COL; i1++)
			if (i1 != c0[0] && i1 != c0[1] && i1 != c0[2] && i1 != c0[3] && i1 != c0[4] && i1 != c0[5])
			{
				bool b0 = false;
				prop._idBoardHand = BaseNaborRiverHand(bh._idBoardHand, i1, b0);
				if (b0)	arrNS[GetFaceCardNB(i1)] = i1;
				int dim = (int)vect5.size(), j = 0;
				for (; j < dim; j++)
					if (prop._idBoardHand == vect5[j]._idBoardHand)
					{
						glVectRiverHandsNb[sh + i1] = int(nbCur + j);
						break;
					}
				if (j == dim)
				{
					cards[4] = CardNB(i1);
					prop._game.GameHoldemQ(bh.Hand(), cards);
					vect5.push_back(prop);
					glVectRiverHandsNb[sh + i1] = int(nbCur + dim);
				}
			}
		int dim0 = (int)vect5.size();
		for (int k = 0; k < dim0; k++)
		{
			glVectRiverHandGame.push_back(vect5[k]._game);
			glVectRiverHandId.push_back(vect5[k]._idBoardHand);
		}
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
		WriteVectFile<int>(handle, glVectRiverHandsNb);
		_close(handle);
	}
	handle = _open(FILE_RIVER_HANDS_GAME, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteVectFile<tpGame24>(handle, glVectRiverHandGame);
		_close(handle);
	}
	handle = _open(FILE_RIVER_HANDS_ID, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteVectFile<tpId7>(handle, glVectRiverHandId);
		_close(handle);
	}
	return nbCur;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetRiverHandIndex(clCard *arr, clHand hand)
{
	tpRefToProperty nbTurn = GetRefTurn(arr);
	byte cs = nbTurn.GetChangeSuit();
	hand[0].suit = (enSuit)((cs >> 2 * hand[0].suit) & 3);
	hand[1].suit = (enSuit)((cs >> 2 * hand[1].suit) & 3);
	int nbHand = hand.NbHandAll();
	size_t nbBH=glVectTurnHandsNb[nbTurn.Number()*ALL_CN_HAND + nbHand];
	/*
	int nbT = nbTurn.Number();
	stNaborProperty &nab = glVectTurn[nbT];
	tpTurnHandProperty &prop = glVectTurnHand[nbBH];
	clHand hand1 = prop.Hand();
	clCard c0[5];
	prop.GetBoard(c0);
	*/
	clCard card = arr[4];
	card.suit = (enSuit)((cs >> 2 * card.suit) & 3);
	return glVectRiverHandsNb[nbBH*CN_ALL_CARDS + card.Number()];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetTurnAndRiverHandIndex(tpCard *arr, tpHand hand, int &indTurn)
{
	int val = ((int)arr[0]._card << 18) + ((int)arr[1]._card << 12) + ((int)arr[2]._card << 6) + arr[3]._card;
	tpRefToProperty ref = glVectRefTurn[val];
	byte cs = ref.GetChangeSuit();
	ChangeSuitMask(hand[0], cs);
	ChangeSuitMask(hand[1], cs);
	int nbHand = hand.NbHandAll();
	size_t nbV = ref.Number()*ALL_CN_HAND + nbHand;
#ifdef  ERR_MESSAGE
	if (nbV >= glVectTurnHandsNb.size())
	{
		ErrMessage("GetTurnAndRiverHandIndexT ", (clAnsiString)nbV);
		nbV = 0;
	}
#endif
	indTurn = glVectTurnHandsNb[nbV];

	tpCard card = arr[4];
	ChangeSuitMask(card, cs);
	nbV = ((size_t)indTurn)*CN_ALL_CARDS + card.Number();
#ifdef  ERR_MESSAGE
	if (nbV >= glVectRiverHandsNb.size())
	{
		ErrMessage("GetTurnAndRiverHandIndexT ", (clAnsiString)nbV);
		nbV = 0;
	}
#endif
	return glVectRiverHandsNb[nbV];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	TestFlopHandNabors()
{
	for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
		for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
		{
			glTrasser.WriteMessage(i1);
			for (int i2 = i1 + 1; i2 < CN_ALL_CARDS; i2++)
				for (int iT = 0; iT < CN_ALL_CARDS; iT++)
					if (iT != i1 && iT != i2 && iT != i0)
						for (int iR = 0; iR < CN_ALL_CARDS; iR++)
							if (iR != i1 && iR != i2 && iR != i0  && iR != iT)
							{
								clCard card[] = { CardNB(i0), CardNB(i1), CardNB(i2) };
								clHand hand = { CardNB(iT), CardNB(iR) };
								int nb = GetFlopHandIndex(card, hand);
								if (nb < 0 || nb >= CN_NABOR_HAND_TURN)
								{
									clAnsiString as = "board=";
									for (int i = 0; i < 5; i++)
										as += card[i].Name();
									as += "hand=";
									as += hand[0].Name();
									as += hand[1].Name();
									glTrasser.WriteMessage(as);
									nb = GetFlopHandIndex(card, hand);
								}
							}
		}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	TestTurnHandNabors()
{
	for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
		for (int i1 = i0 + 1; i1 < CN_ALL_CARDS; i1++)
			for (int i2 = i1 + 1; i2 < CN_ALL_CARDS; i2++)
				for (int i3 = i2 + 1; i3 < CN_ALL_CARDS; i3++)
					for (int iT = 0; iT < CN_ALL_CARDS; iT++)
						if (iT != i1 && iT != i2 && iT != i0 && iT != i3)
							for (int iR = 0; iR < CN_ALL_CARDS; iR++)
								if (iR != i1 && iR != i2 && iR != i0  && iR != i3 && iR != iT)
								{
									clCard card[] = { CardNB(i0), CardNB(i1), CardNB(i2), CardNB(i3) };
									clHand hand = { CardNB(iT), CardNB(iR) };
									int nb = GetTurnHandIndex(card, hand);
									if (nb < 0 || nb >= CN_NABOR_HAND_TURN)
									{
										clAnsiString as = "board=";
										for (int i = 0; i < 5; i++)
											as += card[i].Name();
										as += "hand=";
										as += hand[0].Name();
										as += hand[1].Name();
										glTrasser.WriteMessage(as);
										nb = GetTurnHandIndex(card, hand);
									}
								}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	TestRiverHandNabors()
{
	for (int i0 = 0; i0 < CN_ALL_CARDS; i0++)
		for (int i1 = i0+1; i1 < CN_ALL_CARDS; i1++)
		{
//			glTrasser.WriteMessage(i1);
			for (int i2 = i1+1; i2 < CN_ALL_CARDS; i2++)
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
											clAnsiString as = "board=";
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
	glTrasser.WriteMessage((clAnsiString)"Вышли");
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
/*clHandsGroupEx gh;
	gh.FullPreFlopAllHands();
	for (int i = 0; i < ALL_CN_HAND; i++)
		gh[i] = 1;*/
	for (unsigned i = atr->_nbFirst; i<(unsigned)atr->_nbLast; i++)
	{
		if (i % 100000 == 0)
			glTrasser.WriteMessage((int)i);

		__int64 id = glVectRiverHandId[i].GetId();
		clCard	cards[5] = { CardNB((id >> 36) & 0x3F), CardNB((id >> 30) & 0x3F), CardNB((id >> 24) & 0x3F), CardNB((id >> 18) & 0x3F), CardNB((id >> 12) & 0x3F) };
		clHand hand = { CardNB((id >> 6) & 0x3F), CardNB(id & 0x3F) };
		clCol deck;
		deck.New();
		deck.Skip(cards[0]);
		deck.Skip(cards[1]);
		deck.Skip(cards[2]);
		deck.Skip(cards[3]);
		deck.Skip(cards[4]);
		deck.Skip(hand[0]);
		deck.Skip(hand[1]);
		int game = glVectRiverHandGame[i].ToInt();
		int nbTH = GetTurnIndex(cards);
		clHandsGroupEx &gh = atr->_arrGH[nbTH];

		float cnAll = 0, sum = 0;
		for (int i3 = 0; i3 < deck.CnCards(); i3++)
			for (int i4 = i3 + 1; i4 < deck.CnCards(); i4++)
			{
				clHand handOpp = { deck[i4], deck[i3] };
				int nbH = handOpp.NbHandAll();
				if (gh[nbH] < DOUBLE_0) continue;
				int nbBHOpp = GetRiverHandIndex(cards, handOpp);
				int gameOpp = glVectRiverHandGame[nbBHOpp].ToInt();
				if (game > gameOpp)
					sum += gh[nbH];
				else if (game == gameOpp)
					sum += 0.5f * gh[nbH];
				cnAll += gh[nbH];
//				cnAll++;
			}
		atr->_arrEV[i] = sum / cnAll;
	}
	t0 = time(NULL) - t0;
	atr->_stop = true;
	return 0;
}

int	CalcRiversHandEV()
{
	int handle = _open(FILE_RIVER_HANDS_GAME, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadVectFile<tpGame24>(handle, glVectRiverHandGame);
		_close(handle);
	}
	else return 0;

	handle = _open(FILE_RIVER_HANDS_ID, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadVectFile<tpId7>(handle, glVectRiverHandId);
		_close(handle);
	}
	else return 0;

	handle = _open(FILE_GH_DIST, O_RDONLY | O_BINARY);
	vector <clHandsGroupEx> vectGH, vectGHFlop;
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

	int dim = (int)glVectRiverHandId.size();
	vector <float> vectEV;
	vectEV.resize(dim);
//	for (int i = 0; i < dim; i++)
//		vectEV[i] = -1;

	time_t t0 = time(NULL);
	tpThreadEv atr[8];
//	int CN_THREAD = 8;
	int cn0 = 0;
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
	t0 = time(NULL) - t0;

	//	return t0;
	handle = _open(FILE_RIVER_HANDS_EV, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteVectFile<float>(handle, vectEV);
		_close(handle);
	}
	else
		ErrMessage("Не открывается файл", FILE_RIVER_HANDS_EV);
	glVectRiverHandGame.clear();
	return (int)t0;
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
void	CalcTurnsHandEV()
{
	vector <float> vectEV,turn50,turn100;
	int handle = _open(FILE_RIVER_HANDS_EV, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadVectFile<float>(handle, vectEV);
		_close(handle);
	}
	else
	{
		ErrMessage("Не открывается файл", FILE_RIVER_HANDS_EV);
		return;
	}

	int dim = (int)glVectTurnHand.size();
	turn100.resize(dim);
	turn50.resize(dim);

/*	vector <tpBoardHand> vectH;
	int dim0 = glVectTurnHand.size();
	vectH.resize(dim0);
	for (int i = 0; i < dim0; i++)
	{
		vectH[i]._ev50 = 0;
		vectH[i]._ev100 = 0;
		vectH[i]._hand = glVectTurnHand[i].Hand();
		glVectTurnHand[i].GetBoard(vectH[i]._cards);
	}*/

	float arrRes[CN_CARD_COL];
	for (int i = 0; i < dim; i++)
	{
		if (i%10000==0)
			glTrasser.WriteMessage(i);
		tpTurnHandProperty &bh = glVectTurnHand[i];
		float ev100 = 0,ev50 = 0;
		clCard	cards[5];
		bh.GetBoard(cards);
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
			ev100 += arrRes[j];
		}
		turn100[i] = ev100/deck.CnCards();
		SortShellDef<float>(deck.CnCards(), arrRes);
		int cn = deck.CnCards() / 2;
		for (int j = deck.CnCards() / 2; j < deck.CnCards(); j++)
			ev50 += arrRes[j];
		turn50[i] = ev50/(deck.CnCards() - deck.CnCards() / 2);
//		vectH[i]._ev100 = turn100[i];
	}

	handle = _open(FILE_BOARD_HANDS_EV, O_RDWR | O_BINARY | O_CREAT);
	if (handle != -1)
	{
		WriteVectFile<float>(handle, turn50);
		WriteVectFile<float>(handle, turn100);
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcFlopsHandEV()
{
	vector <float> turn50, turn100, flop50, flop100;
	int handle = _open(FILE_BOARD_HANDS_EV, O_RDWR | O_BINARY);
	if (handle != -1)
	{
		ReadVectFile<float>(handle, turn50);
		ReadVectFile<float>(handle, turn100);
		_close(handle);
	}
	else return;

/*	vector <tpBoardHand> vectH;
	int dim0 = glVectTurnHand.size();
	vectH.resize(dim0);
	for (int i = 0; i < dim0; i++)
	{
		vectH[i]._ev50 = turn50[i];
		vectH[i]._ev100 = turn100[i];
		vectH[i]._hand = glVectTurnHand[i].Hand();
		glVectTurnHand[i].GetBoard(vectH[i]._cards);
	}*/

	int dim = (int)glVectFlopHand.size();
	flop100.resize(dim);
	flop50.resize(dim);
	float arrRes[CN_CARD_COL];
	for (int i = 0; i < dim; i++)
	{
//		glTrasser.WriteMessage(i);
		tpFlopHandProperty &th = glVectFlopHand[i];
		clCard cards[5] = { th._board[0], th._board[1], th._board[2] };
		float ev100 = 0,ev50=0;
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
			arrRes[j] = turn100[nbRH];
			ev100 += arrRes[j];
		}
		flop100[i] = ev100/deck.CnCards();
		SortShellDef<float>(deck.CnCards(), arrRes);
		int cn = deck.CnCards() / 2;
		for (int j = deck.CnCards() / 2; j < deck.CnCards(); j++)
			ev50 += arrRes[j];
		flop50[i] = ev50/(deck.CnCards() - deck.CnCards() / 2);
	}

	handle = _open(FILE_BOARD_HANDS_EV, O_RDWR | O_BINARY);
	if (handle != -1)
	{
		WriteVectFile<float>(handle, turn50);
		WriteVectFile<float>(handle, turn100);
		WriteVectFile<float>(handle, flop50);
		WriteVectFile<float>(handle, flop100);
		_close(handle);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcPreflopHandsEV()
{
	vector <float> turn50, turn100, flop50, flop100,vect50,vect100;
	int handle = _open(FILE_BOARD_HANDS_EV, O_RDWR | O_BINARY);
	if (handle != -1)
	{
		ReadVectFile<float>(handle, turn50);
		ReadVectFile<float>(handle, turn100);
		ReadVectFile<float>(handle, flop50);
		ReadVectFile<float>(handle, flop100);
		_close(handle);
	}
	vect50.resize(CN_HAND);
	vect100.resize(CN_HAND);
	clCard cards[5];

/*	vector <tpBoardHand> vectH;
	int dim0 = glVectFlopHand.size();
	vectH.resize(dim0);
	for (int i = 0; i < dim0; i++)
	{
		vectH[i]._ev50 = turn50[i];
		vectH[i]._ev100 = turn100[i];
		vectH[i]._hand = glVectFlopHand[i]._hand;
		memcpy(vectH[i]._cards,glVectFlopHand[i]._board,3*sizeof(clCard));
	}*/

	for (int i = 0; i < CN_HAND; i++)
	{
		tpPreflopHands &fh = glArrPreflopHand[i];
		fh._ev100 = fh._ev50 = 0;
		clCol deck;
		deck.New();
		deck.Skip(fh._hand[0]);
		deck.Skip(fh._hand[1]);
		int cnAll = 50 * 49 * 48 / 2 / 3;
		vector <float> vectRes;
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
					vectRes[nb] = flop100[nbBH];
					if (vectRes[nb] < 0 || vectRes[nb]>1)
					{
						tpFlopHandProperty prop = glVectFlopHand[nbBH];
						vectRes[nb] = flop100[nbBH];
					}
					fh._ev100 += vectRes[nb];
					nb++;
				}
			}
		}
		fh._ev100 /= cnAll;
		SortShellDef<float>(cnAll, &vectRes[0]);
		int cn = cnAll / 2;
		for (int j = cn; j < cnAll; j++)
			fh._ev50 += vectRes[j];
		fh._ev50 /= (cnAll - cn);
		vect50[i] = fh._ev50;
		vect100[i] = fh._ev100;
	}
	handle = _open(FILE_BOARD_HANDS_EV, O_RDWR | O_BINARY);
	if (handle != -1)
	{
		WriteVectFile<float>(handle, turn50);
		WriteVectFile<float>(handle, turn100);
		WriteVectFile<float>(handle, flop50);
		WriteVectFile<float>(handle, flop100);
		WriteVectFile<float>(handle, vect50);
		WriteVectFile<float>(handle, vect100);
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
		vector <float> vectRes;
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
				vectRes[nb] = (float)w2.WinEq();
				res100[i] += vectRes[nb];
				nb++;
			}
		}
		res100[i] /= cnAll;
		if (fabs(fh._ev100 - res100[i])>0.001)
			fh._ev100 = fh._ev100;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double AverageAndDisp0(int cn, double *arr, clHandsGroupEx &ghF, double &disp)
{
	disp = 0;
	if (cn == 0) return 0;
	double aver = 0, sum = 0;
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
	disp = sqrt(disp / cn / sum);
	return aver;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcGHForDist()
{
	clHandsGroupEx gh;
	gh.FullPreFlopAllHands();
	int handle = _open(FILE_PREFLOP_HANDS, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadVectFile <byte>(handle, glArrNbHand);
		_read(handle, &glArrPreflopHand, sizeof(glArrPreflopHand));
		_close(handle);
	}
	else return;
	handle = _open(FILE_NABOR4, O_RDONLY | O_BINARY);
	if (handle != -1)
	{
		ReadVectFile<tpRefToProperty>(handle, glVectRefTurn);
		ReadVectFile<stNaborProperty>(handle, glVectTurn);
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
		float ev = float((arr[i] - aver) / disp / 1.3);
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

	vector <float> turn50, turn100, flop50, flop100, vect50, vect100;
	handle = _open(FILE_BOARD_HANDS_EV, O_RDWR | O_BINARY);
	if (handle != -1)
	{
		ReadVectFile<float>(handle, turn50);
		ReadVectFile<float>(handle, turn100);
		ReadVectFile<float>(handle, flop50);
		ReadVectFile<float>(handle, flop100);
		_close(handle);
	}

	vector <clHandsGroupEx> vectGH, vectGHFlop;
	vectGH.resize(NABOR_CN_TURN);
	vectGHFlop.resize(NABOR_CN_FLOP);
	for (int i = 0; i < NABOR_CN_FLOP; i++)
	{
		glTrasser.WriteMessage(i);
		stNaborProperty &flop = glVectFlop[i];
		clHandsGroupEx ghF = gh;
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
					arr[nb] = flop50[nbBH];
				}
				else
				{
					arr[nb] = 0;
					ghF[nb] = 0;
				}
			}
		aver = AverageAndDisp0(ALL_CN_HAND, arr, ghF, disp);
		for (int j = 0; j < ALL_CN_HAND; j++)
		{
			float ev = float((arr[j] - aver) / disp);
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
				int id = (flop._id << 6) + iT;
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
									glTrasser.WriteMessage(i0 * 100 + i1);
								arr[nb] = turn50[nbBH];
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
						float ev = float((arr[j] - aver) / disp);
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
int		GetHandIndex(clHand hand)
{
	return glArrNbHand[hand[0].Number() + (hand[1].Number() << 6)];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		GetHandIndex(tpHand hand)
{
	return glArrNbHand[hand[0] + (((int)hand[1]) << 6)];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//#pragma package(smart_init)
