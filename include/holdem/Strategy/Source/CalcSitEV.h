//---------------------------------------------------------------------------

#ifndef CalcSitEV_H
#define CalcSitEV_H
//---------------------------------------------------------------------------
#include "..\..\Common\sitHoldem.h"

using namespace std;

struct tpSitRes
{
	short _nbPl[CN_PLAYER],_cnPl;
	std::vector <clDoubleCP> _res;
};

struct tpCardsGame
{
	void	DealCards(clDeck &col);
	void	SetHandsToSit(clSitHoldem &sit);
	void	GetHandsFromSit(clSitHoldem &sit);
	void	CalcAllIndex();
	void	CalcAllIndex(int nbPl);

	int _cnPl;
	tpHand _handsPl[CN_PLAYER];
	tpCard _cardTable[5];
	int _indFlop[CN_PLAYER];
	int _indTurn[CN_PLAYER];
	int _indRiver[CN_PLAYER];
	int _game[CN_PLAYER];
};
clDoubleCP CalcSitEV(clSitHoldem &sit);
clDoubleCP CalcSitEV(clSitHoldem &sit,clHand *hand);
void	CalcSitRes(clSitHoldem &sit, tpSitRes *res);
clDoubleCP CalcSitEV(clSitHoldem &sit, tpSitRes *res);
clDoubleCP CalcSitEV(clSitHoldem &sit, tpCardsGame *hand, tpSitRes *res);
clDoubleCP CalcSitDistribWin(clSitHoldem &sit);

int 	FinResGame(clGameHistory &hist, int nbH);
double	FinResGameEV(clGameHistory &hist, int nb);
int		FinResReplayAllIn(clGameHistory &hist, int nb);
//---------------------------------------------------------------------------
#endif
