//---------------------------------------------------------------------------

#ifndef FlopAllInH
#define FlopAllInH

#include <vector>
using namespace std;
#include "c:\home\card\Hand.h"

extern vector <float>	glVectFlopAllin;

float FlopAllInResult(clHand h0, clHand h1, clCard *card);
float FlopAllInResult(int nbH0, int nbH1, tpCard *cards);
//---------------------------------------------------------------------------
#endif
