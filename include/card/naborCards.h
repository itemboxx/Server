//---------------------------------------------------------------------------

#ifndef NaborCards_H
#define NaborCards_H

#include <vector>
#include <windows.system.h>
#include "pokGame.h"


using namespace std;
#define COMP2
//#define GT_PROG

#ifdef COMP2
#define	CN_THREAD		12
#elif defined COMP_C
#define	CN_THREAD		8
#else
#define	CN_THREAD		8
#endif

#define NABOR_MAX_CN		0x40000000
#define ALL_CN_FLOP			(52*51*50/6)
#define NABOR_CN_FLOP		1755
#define NABOR_CN_TURN		63193
#define CN_NABOR_HAND_FLOP		1286792
#define CN_NABOR_HAND_TURN		50375052
#define CN_NABOR_HAND_RIVER		1189591338

#define NABOR_CN_RIVER			1564810

#define	CN_CHANGE_SUIT						24
extern byte glArrChangeSuit[CN_CHANGE_SUIT];
void	ChangeSuit(tpCard &c, byte cs);


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
	int		ToInt()		{ return ((int)_arr[0] << 16) + ((int)_arr[1] << 8) + _arr[2]; }

	byte _arr[3];
};

struct tpPreflopHands
{
	clHand _hand;
	float  _ev100, _ev50;
};

struct stNaborProperty
{
	void	WrProperty(int cnCard,int id);

	int _id, _cnNabor;
	byte _cnCard;
	clCard _cards[5];
	clGamePoker _game;
};

struct tpFlopHandProperty
{
	void	WrProperty(int id);
	clAnsiString	ToString();

	clCard	_board[3];
	clHand	_hand;
	int		_idBoardHand, _cnNabor;
};

struct tpTurnHandProperty
{
	void	TpHand(tpHand &hand);
	void	GetBoard(tpCard *c0);
	clHand	Hand();
	void	GetBoard(clCard *c0);

	__int64	_idBoardHand;
};

extern	tpPreflopHands glArrPreflopHand[CN_HAND];

extern vector <stNaborProperty> glVectFlop;
extern vector <tpFlopHandProperty> glVectFlopHand;

extern vector <tpTurnHandProperty> glVectTurnHand;

//extern vector <tpRiverHandProperty> glVectRiverHand;
extern vector <tpGame24> glVectRiverHandGame;

extern vector <float> glVectEV50, glVectEV100, glVectFlopEV50, glVectFlopEV100, glVectTurnEV50, glVectTurnEV100, glVectRiverEV;

typedef unsigned int tpIndex;

#define REG_LOAD_FORCALC_NABOR		0b100
#define REG_LOAD_FORCALC_TREE		0b1000
#define REG_LOAD_FORCALC_PLAY		0b10000

bool	LoadDataHandsBoard(int reg);
void	CalcPreflopHandsEV();
int		GetHandIndex(clHand hand);
int		GetHandIndex(tpHand hand);
void	CreateAllPreFlops();

void CreateAllFlops();
void CreateAllFlopsHand();
void CreateHandsToFlopsInd();
clAnsiString	FlopToString(int nb);
clAnsiString	FlopHandToString(int nb);
int		GetFlopIndex(tpCard *arr);
int		GetFlopIndex(clCard *arr);
int		GetFlopIndex(clCard *arr, byte &ref);
int		GetFlopHandIndex(clCard *arr, clHand hand);
int		GetFlopHandIndex(tpCard *arr, tpHand hand);
int		GetFlopHandIndexFromTurnHandId(__int64 id);
void	CalcFlopsHandEV();

void	CreateAllTurns();
void	CreateAllTurnsHand();
int		GetTurnIndex(tpCard *arr);
int		GetTurnIndex(clCard *arr);
int		GetTurnHandIndex(clCard *arr, clHand hand);
int		GetTurnHandIndex(tpCard *arr, tpHand hand);
tpIndex	GetTurnHandIndexFromRiverHandIndex(tpIndex nb1);
void	CalcTurnsHandEV();

void	CreateAllRivers();
size_t	CreateAllRiversHand();
int		GetRiverHandIndex(clCard *arr, clHand hand);
int		GetTurnAndRiverHandIndex(tpCard *arr, tpHand hand,int &indTurn);
int		CalcRiversHandEV();

void	TestFlopHandNabors();
void	TestTurnHandNabors();
void	TestRiverHandNabors();
void	CalcGHForDist();

inline	int		NaborCardsGetGame(tpHand hand, tpCard *cards)
{
	int indTurn, indRiver = GetTurnAndRiverHandIndex(cards, hand, indTurn);
	return glVectRiverHandGame[indRiver].ToInt();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
