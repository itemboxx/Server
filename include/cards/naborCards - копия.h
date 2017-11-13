//---------------------------------------------------------------------------

#ifndef NaborCards_H
#define NaborCards_H

#include <vector>
#include <windows.system.h>
#include "c:\home\card\pokGame.h"

using namespace std;

#define NABOR_MAX_CN		0x40000000
#define NABOR_CN_FLOP		1755
#define NABOR_CN_TURN		63193
#define CN_NABOR_HAND_FLOP		1286792
#define CN_NABOR_HAND_TURN		49183602
#define CN_NABOR_HAND_RIVER		1485366090

#define NABOR_CN_RIVER			1564810

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
	float  _ev100, _ev50;
};

struct tpTurnHandProperty
{
	clHand Hand();
//	clCard	_board[4];
//	clHand	_hand;
//	int		_Tag, _nbFlopHand;
	__int64	_idBoardHand;
	float  _ev100, _ev50;
};

struct tpRiverHandProperty
{
	__int64	_idBoardHand;
	clGamePoker _game;
};

extern	tpPreflopHands glArrPreflopHand[CN_HAND];

extern vector <tpFlopHandProperty> glVectFlopHand;

extern vector <tpTurnHandProperty> glVectTurnHand;

bool	LoadDataHandsBoard();
void	CalcPreflopHandsEV();
int		GetHandIndex(clHand hand);
void	CreateAllPreFlops();

void CreateAllFlops();
void CreateAllFlopsHand();
void CreateHandsToFlopsInd();
clAnsiString	FlopToString(int nb);
clAnsiString	FlopHandToString(int nb);
int		GetFlopHandIndex(clCard *arr, clHand hand);
int		GetFlopHandIndex(tpCard *arr, tpHand hand);
void	CalcFlopsHandEV();

void	CreateAllTurns();
void	CreateAllTurnsHand();
int		GetTurnIndex(tpCard *arr);
int		GetTurnIndex(clCard *arr);
int		GetTurnHandIndex(clCard *arr, clHand hand);
int		GetTurnHandIndex(tpCard *arr, tpHand hand);
void	CalcTurnsHandEV();

void	CreateAllRivers();
size_t	CreateAllRiversHand();
int		GetRiverHandIndex(clCard *arr, clHand hand);
int		CalcRiversHandEV();

void	TestFlopHandNabors();
void	TestTurnHandNabors();
void	TestRiverHandNabors();
void	CalcGHForDist();
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
