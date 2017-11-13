#ifndef ScanTableH
#define ScanTableH
#pragma once
#include "C:\home\holdem\Common\sitHoldem.h"
enum enTypeTrnEx { ENTYPETRN_UNDEF };
class tpScanTable
{
public:
	tpScanTable();
	~tpScanTable();
	inline  void    WrIndicator(int i)  { _indicator = i; }
	inline  void    AddCnCardTable(int cn = 1)    { _cnCardTable += cn; }
	inline  void    WrNbButton(int nb)          { _nbButton = nb; }
	inline  void    WrIsCard(int nb, bool b)     { _isCard[nb] = b; }
	inline  void    WrViewCard(int nb, bool b)   { _viewCard[nb] = b; }
	inline  void    WrBigB(int bb)  { _bb = bb; }
	inline  int     GetCnCardTable()            { return _cnCardTable; }
	int     ViewPlayer();
	bool    ReStraightNbToPos(BYTE *nbPos);
	void    ShiftPlayers(int shift);
	int NextPlayer(int nb);
	int PrevPlayer(int nb);
	bool    CheckTableToBuild();
	bool operator == (clSitHoldem &table);


	void    ClearTable();
	bool operator == (tpScanTable &tab);
	void    operator = (clSitHoldem &tab);
	inline  bool    OurDecide()     { return (_indicator == -1) ? false : _viewCard[_indicator]; }

	inline  void    AddCardTable(clCard card)       { _cardTable[_cnCardTable++] = card; }
	int CheckScan();
	void    StartNextGame();
	int NextPlayer(int nb) const;
	int CnPlayerWidthCard();
	int     PlayerPosition(int nbPl)const;
	inline  int PlayerMoney(int nb)  const           { return _seat[nb]._money; }
	int SitOutPlayer();

	CRect   _rect;
	short   _nbButton, _indicator, _cnPl, _cnCardTable;
	int _bb, _nbTbl, _ante, _totalPot;
	__int64 _numberHand, _numberHandOld, _nbTrn;
	tpSeat _seat[CN_PLAYER];
	clCard _cardTable[5];
	bool _isCard[CN_PLAYER], _viewCard[CN_PLAYER];



	enTypeTrnEx _game;

};

struct stPSTableProperty
{
	CRect _rect;
	bool _badDimension;
	tpScanTable _table;
};
#define  CSC_SIZE_BUFFER     (sizeof(stPSTableProperty)+sizeof(char))


struct stCloseTable
{
	__int64 _nbTrn;
	char _path[MAX_PATH];
};
bool    ScanTableBuildHistory(clListHistory *ptrL, tpScanTable *table);
tpDis   ScanTableQuickDec(const tpScanTable *table);
tpDis   ScanTableDecWarn(tpScanTable *table);
#endif