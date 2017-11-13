//---------------------------------------------------------------------------

#ifndef GlobalsForCalcH
#define GlobalsForCalcH
//---------------------------------------------------------------------------
#include "..\..\card\naborCards.h"

#define REG_NO_NABOR_TAB		0
#define REG_STREET_TREE 		0b1
#define REG_FLOP_ALLIN	 		0b10

bool    GlobalsForCalcLoad(int reg=0);
void    CloseGlobalsForCalc();
//---------------------------------------------------------------------------
#endif
