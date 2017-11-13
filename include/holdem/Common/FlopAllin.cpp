//---------------------------------------------------------------------------
//#include "stdafx.h"
#pragma hdrstop

#include "C:\home\holdem\Common\FlopAllIn.h"
#include "C:\home\card\Hand.h"
#include "c:\home\card\naborCards.h"

vector <float>	glVectFlopAllin;

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
float FlopAllInResult(clHand h0, clHand h1, clCard *card)
{
	card[3] = h1[0];
	card[4] = h1[1];
	int nbR = GetRiverHandIndex(card, h0);
	return glVectFlopAllin[nbR];
}
//---------------------------------------------------------------------------
float FlopAllInResult(int nbH0, int nbH1, tpCard *card0)
{
	tpHand h0(nbH0), h1(nbH1);
	tpCard card[5];
	memcpy(card, card0, 3 * sizeof(tpCard));
	card[3] = h1[0];
	card[4] = h1[1];
	int nbT, nbR = GetTurnAndRiverHandIndex(card, h0, nbT);
	return glVectFlopAllin[nbR];
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
