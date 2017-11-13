#pragma once
#ifndef ValidateServer
#define ValidateServer
#include <map>
#include <vector>
#include "C:\home\holdem\common\sitHoldem.h"
#include "c:\Home\Util\TemplateFileRec.h"


enum enNbError { AllIsGood, ERR_CNPL, ERR_MXSEAT, ERR_TABLECOST, ERR_BB, ERR_ANTE, ERR_NBTRNY, ERR_NBG ,
	ERR_TABLE_CARD0, ERR_TABLE_CARD1, ERR_TABLE_CARD2, ERR_TABLE_CARD3, ERR_TABLE_CARD4,
	ERR_PL_CARDS0, ERR_PL_CARDS1, ERR_PL_CARDS2, ERR_PL_CARDS3, ERR_PL_CARDS4,
	ERR_NAMETABLE, ERR_NBPOS, ERR_NAMEPLAYER, ERR_PL_MONEY, ERR_PL_INPLAY, ERR_PL_BUSY, ERR_OLD_ACT,
	ERR_POS_HERO, ERR_CNACT, ERR_EXACTACT, ERR_CNCARDTABLE, ERR_POT, ERR_NBACT, ERR_NAMETRADE, ERR_INDICATOR, 
	ERR_CNPLINTRADE, ERR_SITPL, ERR_CNPLISCARD, ERR_PLMONEY, ERR_DEAL
};


class clValidateServer
{
public:
	clValidateServer();
	~clValidateServer();

	enNbError Add(clSitHoldem sit);
	//bool PlayerProgressCheck(clSitHoldem sitOld, clSitHoldem sit, tpDis dis);

private:
	std::map<int , std::vector<clSitHoldem>> _base;
};

enNbError S_Check(clSitHoldem sit);
enNbError Inheritance(clSitHoldem sitOld, clSitHoldem sitNew);
extern clValidateServer glVSServer;
#endif
