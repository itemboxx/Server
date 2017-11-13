#include "stdafx.h"
#include "tpScanTable.h"


tpScanTable::tpScanTable()
{
	ClearTable();
}


tpScanTable::~tpScanTable()
{
}
//---------------------------------------------------------------------------
bool    tpScanTable::CheckTableToBuild()
{
	if (_indicator == -1) return false;
	if (_seat[_indicator]._money == 0) return false;
	//    for(int i=0;i<CN_PLAYER;i++)
	//        if(_seat[i]._busy==true && _seat[i]._inPlay==false)
	//            return false;
	return true;

}
//---------------------------------------------------------------------------
int     tpScanTable::ViewPlayer()
{
	for (int i = 0; i<CN_PLAYER; i++)
		if (_viewCard[i] && _isCard[i])
			return i;
	return -1;
}
//---------------------------------------------------------------------------
int tpScanTable::NextPlayer(int nb)
{
	for (int i = 0; i<CN_PLAYER; i++)
	{
		nb++;
		if (nb >= CN_PLAYER) nb = 0;
		if (_seat[nb]._busy) break;
	}
	return nb;
}
//---------------------------------------------------------------------------
int tpScanTable::PrevPlayer(int nb)
{
	for (int i = 0; i<CN_PLAYER; i++)
	{
		nb--;
		if (nb<0) nb = CN_PLAYER - 1;
		if (_seat[nb]._busy) return nb;
	}
	return -1;
}
//---------------------------------------------------------------------------
void    tpScanTable::ClearTable()
{
	_rect = CRect(0, 0, 0, 0);
	_nbTbl = _nbTrn = _indicator = _nbButton = _numberHand = _numberHandOld = UNDEF_NB;

	_cnPl = _cnCardTable = 0;
	for (int i = 0; i<CN_PLAYER; i++)
	{
		_seat[i]._name[0] = '\0';
		_seat[i]._busy = false;
		_isCard[i] = _viewCard[i] = false;
	}
}
//---------------------------------------------------------------------------
bool tpScanTable::operator ==(tpScanTable &table)
{
	if (_nbTrn != table._nbTrn) return false;
	//    if(_indicator != table._indicator) return false;
	if (_cnCardTable != table._cnCardTable) return false;
	if (_nbButton != table._nbButton) return false;
	for (int i = 0; i<CN_PLAYER; i++)
	{
		if (!(_seat[i] == table._seat[i])) return false;
		if (_viewCard[i] != table._viewCard[i]) return false;
		if (this->_isCard[i] != table._isCard[i]) return false;
	}
	return true;
}
//---------------------------------------------------------------------------
int     tpScanTable::PlayerPosition(int nbPl) const
{
	int nb = _nbButton, pos = POS_BUTTON;
	for (int i = 0; i<_cnPl; i++)
	{
		if (nbPl == nb)
			return pos;
		nb = NextPlayer(nb);
		if (pos == POS_BB) pos = _cnPl - 1;
		else pos--;
	}
	return POS_UNDEF;
}
//---------------------------------------------------------------------------
/*

*/

int tpScanTable::CheckScan()
{
	if (_numberHand <= 0)
		return 1;
	if (_indicator <= -1)
		return 2;
	if (_nbButton <= -1)
		return 3;
	if (_nbButton >= CN_PLAYER)
		return 1004;
	if (_indicator >= CN_PLAYER)
		return 1005;
	if (_cnPl<2 || _cnPl>CN_PLAYER)
		return 6;
	if (_cnCardTable<0 || _cnCardTable>5)
		return 1007;
	if (_bb<0 || _bb>3000)
		return 1008;
	if (_totalPot<0 || _totalPot>3000)
		return 1009;
	//    if(_ante!=_bb/5)
	//        return 1010;
	if (_numberHand <= _numberHandOld)
		return 1011;
	if (_numberHand<(__int64)1E11)
		return 1012;
	if (_numberHandOld>0 && _numberHandOld<(__int64)1E11)
		return 1013;

	for (int i = 0; i<this->GetCnCardTable(); i++)
		if (!this->_cardTable[i].Correct())
			return 1100 + i;

	for (int i = 0; i<CN_PLAYER; i++)
	{
		if (_seat[i]._busy)
		{
			if (_viewCard[i])
			{
				if (!_seat[i]._cards[0].Correct())
					return 1110 + i;
				if (!_seat[i]._cards[1].Correct())
					return 1120 + i;
			}
			if (_seat[i]._money<0 || _seat[i]._money>3000)
				return 14;
		}
	}

	if (this->ViewPlayer() == -1)
		return 15;

	return 0;
}
//---------------------------------------------------------------------------
void    tpScanTable::operator = (clSitHoldem &table)
{
	ClearTable();
	_nbTrn = table.NbTrny();
	_indicator = table.PositionToNbTable(table.Indicator());
	_cnCardTable = table.CnCardTable();
	_nbButton = table.PositionToNbTable(POS_BUTTON);
	_numberHand = table.NbGame();
	memcpy(_cardTable, table.CardTable(), sizeof(_cardTable));
	_cnPl = table.CnPlayer();
	for (int i = 0; i<table.CnPlayer(); i++)
	{
		int nb = table.PositionToNbTable(i);
		_seat[nb] = *table.GetSeat(i);
		_seat[nb]._money = table.PlayerAtr(i)->_money;
		_isCard[nb] = table.PlayerAtr(i)->_isCard;
		_viewCard[nb] = table.PlayerAtr(i)->_viewCards;
	}
}
//---------------------------------------------------------------------------
void    tpScanTable::StartNextGame()
{
	this->_cnPl = 0;
	for (int i = 0; i<CN_PLAYER; i++)
	{
		_isCard[i] = _viewCard[i] = false;
		if (_seat[i]._busy)
			if (_seat[i]._money == 0)
			{
				_seat[i]._busy = _seat[i]._inPlay = false;
			}
			else
				_cnPl++;

	}
	_indicator = -1;
	_cnCardTable = 0;
	_nbButton = NextPlayer(_nbButton);
	for (int i = 0; i<CN_PLAYER; i++)
	{
		_isCard[i] = _viewCard[i] = false;
	}
}
//---------------------------------------------------------------------------
int tpScanTable::NextPlayer(int nb) const
{
	for (int i = 0; i<CN_PLAYER; i++)
	{
		nb++;
		if (nb >= CN_PLAYER) nb = 0;
		if (_seat[nb]._busy) break;
	}
	return nb;
}
//---------------------------------------------------------------------------
int tpScanTable::CnPlayerWidthCard()
{
	int cn = 0;
	for (int i = 0; i<CN_PLAYER; i++)
		if (_seat[i]._busy && _isCard[i])
			cn++;
	return cn;
}
//---------------------------------------------------------------------------
int tpScanTable::SitOutPlayer()
{
	for (int i = 0; i<CN_PLAYER; i++)
		if (_seat[i]._busy && !_seat[i]._inPlay)
			return i;
	return -1;
}
//---------------------------------------------------------------------------
bool    tpScanTable::ReStraightNbToPos(BYTE *nbPos)
{
	for (int i = 0; i < CN_PLAYER; i++)
		nbPos[i] = -1;
	if (_nbButton == -1 || _cnPl < 2)
	{
		_indicator = -1;
		_cnCardTable = 0;
		return false;
	}
	if (_cnPl == 2)
	{
		nbPos[POS_BUTTON] = _nbButton;
		int nb = _nbButton;
		nbPos[POS_SB] = nb;
		int nbBB = NextPlayer(nb);
		nbPos[POS_BB] = nbBB;
	}
	else
	{
		nbPos[POS_BUTTON] = _nbButton;
		int nb = NextPlayer(_nbButton);
		nbPos[POS_SB] = nb;
		int nbBB = NextPlayer(nb);
		nbPos[POS_BB] = nbBB;
		nb = _nbButton;
		for (_cnPl = 3; _cnPl < CN_PLAYER; _cnPl++)
		{
			nb = PrevPlayer(nb);
			if (nb == nbBB)
				break;
			nbPos[_cnPl] = nb;
		}
	}
	for (int i = 0; i<_cnPl; i++)
		if (nbPos[i] == _indicator)
		{
			_indicator = i;
			break;
		}
	tpSeat s0[CN_PLAYER];
	bool    iC[CN_PLAYER], vC[CN_PLAYER];
	for (int i = 0; i<CN_PLAYER; i++)
	{
		s0[i] = _seat[i];
		iC[i] = _isCard[i];
		vC[i] = _viewCard[i];
	}
	for (int i = 0; i<CN_PLAYER; i++)
	{
		if (i<_cnPl)
		{
			_seat[i] = s0[nbPos[i]];
			_isCard[i] = iC[nbPos[i]];
			_viewCard[i] = vC[nbPos[i]];
		}
		else
		{
			_seat[i]._busy = _seat[i]._inPlay = false;
			_isCard[i] = false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
bool tpScanTable::operator == (clSitHoldem &table)
{
	for (int i = 0; i<CN_PLAYER; i++)
		if (_seat[i]._inPlay)
		{
			if (table.PlayerMoney(i) != _seat[i]._money) return false;
			if (table.PlayerIsCard(i) != _isCard[i]) return false;
			if (_isCard[i])
				if (table.PlayerViewCard(i) != _viewCard[i]) return false;
		}
		else
			if (table.PlayerInPlay(i)) return false;
	if (table.CnCardTable() != _cnCardTable) return false;
	if (_indicator != -1)
		return  _indicator == table.Indicator();
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool    ScanTableIsThisGame(clGameHistory *ptrH, tpScanTable *table)
{
	return (ptrH->BigBlind() == table->_bb && ptrH->NbButton() == table->_nbButton);
}
//---------------------------------------------------------------------------
void    ExDilDelt(clGameHistory *ptrH, tpScanTable *table)
{
	tpGameHH *ptrAtr = &ptrH->_atrHH;
	//    tpArrSeat  *arrSeat=&ptrH->_arrSeat;
	ptrAtr->_cnPl = 0;
	for (int i = 0; i<CN_PLAYER; i++)
		if (ptrH->_seat[i]._busy)
		{
			ptrAtr->_cnPl++;
			int money = ptrH->_seat[i]._money;
			if (money>ptrAtr->_ante) money = ptrAtr->_ante;
			if (money > 0)
				ptrH->AddAct(i, ACT_POSTANTE, money);
		}

	int money = ptrH->_seat[POS_SB]._money - ptrAtr->_ante;
	if (money>0)
	{
		if (money>ptrAtr->_bb / 2) money = ptrAtr->_bb / 2;
		ptrH->AddAct(POS_SB, ACT_POSTSB, money);
	}

	money = ptrH->_seat[POS_BB]._money - ptrAtr->_ante;
	if (money>0)
	{
		if (money>ptrAtr->_bb) money = ptrAtr->_bb;
		ptrH->AddAct(POS_BB, ACT_POSTBB, money);
	}

	ptrH->AddAct(NB_DILER, ACT_DELCARD, 0);

	for (int i = 0; i<table->_cnPl; i++)
		if (table->_viewCard[i])
			ptrH->AddAct(i, ACT_SHOW, 0);
}
//---------------------------------------------------------------------------
bool    ScanTableBuildHistory(clGameHistory *ptrH, tpScanTable *ptrT)
{
	ptrH->_atrHH._nbG = ptrT->_numberHand;
	if (ptrT->CnPlayerWidthCard() == 0)
		return false;
	if (ptrH->CnAct() == 0) ExDilDelt(ptrH, ptrT);
	clSitHoldem table;
	table.ExecHist(ptrH, ptrH->CnAct());
	if (ptrT->_indicator == -1)
		if (table.CnCardTable() == ptrT->_cnCardTable)
			return true;
	for (int i = 0; i<ptrT->_cnPl; i++)
	{
		memcpy(ptrH->PlayerCards(i), ptrT->_seat[i]._cards, 2 * sizeof(clCard));
		ptrH->GetSeat(i)->_inPlay = ptrT->_seat[i]._inPlay;
	}
	if (*ptrT == table) return true;
	for (int i = 0; i<ptrT->_cnPl; i++)
		if (!table.PlayerViewCard(i) && ptrT->_viewCard[i])
			ptrH->AddAct(i, ACT_SHOW, 0);
	int ind = table.Indicator();
	if (ind == POS_UNDEF)
	{
		memcpy(ptrH->CardTable(), ptrT->_cardTable, 5 * sizeof(clCard));
		switch (table.CnCardTable())
		{
			case 0:
				if (ptrT->_cnCardTable>0)
				{
					ptrH->AddAct(NB_DILER, ACT_DELFLOP, 0);
					return ScanTableBuildHistory(ptrH, ptrT);
				}
			case 3:
				if (ptrT->_cnCardTable>3)
				{
					ptrH->AddAct(NB_DILER, ACT_DELTURN, 0);
					return ScanTableBuildHistory(ptrH, ptrT);
				}
			case 4:
				if (ptrT->_cnCardTable>4)
				{
					ptrH->AddAct(NB_DILER, ACT_DELRIVER, 0);
					return ScanTableBuildHistory(ptrH, ptrT);
				}
			case 5:
				//                return true;
			default: return true;
		}
	}
	if (!ptrT->_isCard[ind] || !ptrT->_seat[ind]._inPlay)
	{
		ptrH->AddAct(ind, ACT_FOLD, 0);
		return ScanTableBuildHistory(ptrH, ptrT);
	}
	else
	{
		int maxBet = table.MaxBet();
		int bet = table.PlayerMoney(ind) - ptrT->_seat[ind]._money;
		enAction act;
		if (ind == POS_BB && maxBet <= ptrT->_bb && bet == 0)
			act = ACT_CHECK;
		else if (maxBet == 0)
			act = (bet == 0) ? ACT_CHECK : ACT_BET;
		else
			act = (maxBet >= bet + table.PlayerBank(ind)) ? ACT_CALL : (bet + table.PlayerBank(ind) <= ptrH->BigBlind()) ? ACT_CALL : ACT_RAISE;
		if (act == ACT_CALL && bet == 0) return false;
		ptrH->AddAct(ind, act, bet);
		return ScanTableBuildHistory(ptrH, ptrT);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool    ScanTableNewHistory(clGameHistory *ptrH, tpScanTable *table, BYTE *nbPos)
{
	tpGameHH *ptrAtr = &ptrH->_atrHH;

	//    ptrAtr->_site=SITE_POKERSTARS;
	//    ptrAtr->_nbBtn=table->_nbButton;
	ptrAtr->_cnPl = table->_cnPl;
	ptrAtr->_nbTrny = table->_nbTrn;
	ptrAtr->_bb = table->_bb;
	ptrAtr->_ante = table->_ante;
	ptrAtr->_maxSeat = 6;
	ptrAtr->_nbG = table->_numberHand;
	//    ptrH->WrNbButton(table->_nbButton);
	for (int i = 0; i<CN_PLAYER; i++)
		ptrH->_arrNbPos[i] = -1;

	bool b0 = false;
	for (int i = 0; i<CN_PLAYER; i++)
	{
		ptrH->_arrNbPos[i] = nbPos[i];
		ptrH->_seat[i] = table->_seat[i];
		if (table->_isCard[i])
			b0 = true;
	}
	if (b0)
	{
		//        ptrAtr->_cnPl=0;
		for (int i = 0; i<CN_PLAYER; i++)
			if (ptrH->_seat[i]._inPlay)
			{
				//                ptrAtr->_cnPl++;
				if (ptrAtr->_ante > 0)
				{
					ptrH->AddAct(i, ACT_POSTANTE, ptrAtr->_ante);
					ptrH->_seat[i]._money += ptrAtr->_ante;
				}
			}

		ptrH->AddAct(POS_SB, ACT_POSTSB, ptrAtr->_bb / 2);
		ptrH->_seat[POS_SB]._money += ptrAtr->_bb / 2;

		ptrH->AddAct(POS_BB, ACT_POSTBB, ptrAtr->_bb);
		ptrH->_seat[POS_BB]._money += ptrAtr->_bb;

		ptrH->AddAct(NB_DILER, ACT_DELCARD, 0);

		for (int i = 0; i<CN_PLAYER; i++)
			if (table->_viewCard[i])
				ptrH->AddAct(i, ACT_SHOW, 0);

		if (table->_numberHandOld == 0)
		{
			// int maxBet=ptrAtr->_bb;
			for (int i = 0; i<CN_PLAYER; i++)
			{
				ptrH->_seat[i]._money = 500;

				/*if(int bet=500-ptrH->_seat[i]._money)
				{
				enAction act;
				if(maxBet>=bet)
				act =  ACT_CALL;
				else
				{
				act = ACT_RAISE;
				maxBet = bet;
				}
				ptrH->AddAct(i,act,bet);
				}  */
			}
		}
	}

	return ScanTableBuildHistory(ptrH, table);
}
//---------------------------------------------------------------------------
bool    ScanTableBuildHistory(clListHistory *ptrL, tpScanTable *table)
{
	clGameHistory *ptrH;
	BYTE nbPos[CN_PLAYER];
	if (!table->ReStraightNbToPos(nbPos)) return false;
	if (ptrL->CnGame()>0)
	{
		ptrH = ptrL->GameHist(ptrL->CnGame() - 1);
		if (ScanTableIsThisGame(ptrH, table))
		{
			if (table->CheckTableToBuild())
				return ScanTableBuildHistory(ptrH, table);
			else
				return false;
		}
		//        else
		//            CompletedGame(ptrH);
	}
	ptrH = new clGameHistory;
	ptrL->Add(ptrH);
	return ScanTableNewHistory(ptrH, table, nbPos);
}
//---------------------------------------------------------------------------
