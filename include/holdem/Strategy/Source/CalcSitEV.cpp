#include "CalcSitEV.h"
#include "..\..\Common\clHandsCompare.h"
#include "HandsGroupEX.h"
#include "..\..\..\Util\Sort.h" 
#include "..\..\..\util\usefullProc.h"

using namespace std;

class clCalcResTable
{
public:
	void    operator = (clSitHoldem &sit);
	void    StartTable();
	void    EndGame();
	void    WinPlayer(int nbPl);
	void    Win2Player(int nbPl1, int nbPl2);
	void    Win3Player(int nbPl1, int nbPl2, int nbPl3);
	clDoubleCP CalcMoney(clCalcResTable *StartTable);
	int CnPlayersIsCard();
	clStacks    GetStacks();
	int     PlMoney(int nb)     { return _money[nb] + _bet[nb]; }

	bool _isCard[CN_PLAYER];
	int  _bb, _cnPl, _money[CN_PLAYER], _bet[CN_PLAYER], _nbHAll[CN_PLAYER];
};

clDoubleCP CalcAllInPreFlop(clSitHoldem &sit);
clDoubleCP CalcAllInFlop(clSitHoldem &sit);
clDoubleCP CalcAllInTurn(clSitHoldem &sit);
clDoubleCP CalcShowDownD(clSitHoldem &sit);
void DistribPot(clSitHoldem *table, clGamePoker *gamePoker);
clDoubleCP CalcResFor2Player(clCalcResTable *tab0, clCalcResTable *tabStart);
clDoubleCP CalcResFor3Player(clCalcResTable *tab0, clCalcResTable *tabStart);
clDoubleCP CalcResForManyPlayer(clCalcResTable *tab0, clCalcResTable *tabStart);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpCardsGame::DealCards(clDeck &col)
{
	for (int i = 0; i < _cnPl; i++)
		_handsPl[i].SetHand(col.GiveCard(),col.GiveCard());
	for (int i = 0; i < 5; i++)
		_cardTable[i] = col.GiveCard();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpCardsGame::SetHandsToSit(clSitHoldem &sit)
{
	for (int i = 0; i < _cnPl; i++)
	{
		sit.PlayerHand(i)[0] = CardNB(_handsPl[i][0]);
		sit.PlayerHand(i)[1] = CardNB(_handsPl[i][1]);
	}
	for (int i = 0; i < 5; i++)
		sit.CardTable()[i]=CardNB(_cardTable[i]);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpCardsGame::GetHandsFromSit(clSitHoldem &sit)
{
	_cnPl = sit.CnPlayer();
	for (int i = 0; i < _cnPl; i++)
	{
		_handsPl[i][0] = sit.PlayerHand(i)[0];
		_handsPl[i][1] = sit.PlayerHand(i)[1];
	}
	for (int i = 0; i < 5; i++)
		_cardTable[i] = sit.CardTable()[i];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpCardsGame::CalcAllIndex()
{
	for (int i = 0; i < _cnPl; i++)
	{
		_indFlop[i] = GetFlopHandIndex(_cardTable, _handsPl[i]);
		_indRiver[i] = GetTurnAndRiverHandIndex(_cardTable, _handsPl[i], _indTurn[i]);
		_game[i] = glVectRiverHandGame[_indRiver[i]].ToInt();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	tpCardsGame::CalcAllIndex(int i)
{
	{
		_indFlop[i] = GetFlopHandIndex(_cardTable, _handsPl[i]);
		_indRiver[i] = GetTurnAndRiverHandIndex(_cardTable, _handsPl[i], _indTurn[i]);
		_game[i] = glVectRiverHandGame[_indRiver[i]].ToInt();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcSitEV(clSitHoldem &sit, clHand *hand)
{
	for (int i = 0; i < sit.CnPlayer(); i++)
		sit.PlayerHand(i) = hand[i];
	return CalcSitEV(sit);
}
//---------------------------------------------------------------------------
clDoubleCP CalcSitEV(clSitHoldem &sit)
{
	switch (sit.NameTrade())
	{
		case TRADE_PREFLOP	: return CalcAllInPreFlop(sit);
		case TRADE_FLOP		: return CalcAllInFlop(sit);
		case TRADE_TURN		: return CalcAllInTurn(sit);
		default				: return CalcShowDownD(sit);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcAllInPreFlop(clSitHoldem &sit)
{
	int cnPl = sit.CNPlayerIsCard();
	clCalcResTable tab0, tabStart;
	tab0 = sit;
	tabStart = tab0;
	tabStart.StartTable();
	if (cnPl == 1)
	{
		tab0.EndGame();
		return tab0.CalcMoney(&tabStart);
	}
	else if (cnPl == 2)
		return CalcResFor2Player(&tab0, &tabStart);
	else if (cnPl == 3)
		return CalcResFor3Player(&tab0, &tabStart);
	else
		return CalcResForManyPlayer(&tab0, &tabStart);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcAllInFlop(clSitHoldem &sit)
{
	if (sit.CNPlayerIsCard() == 1)
	{
		clCalcResTable tab0, tabStart;
		tab0 = sit;
		tabStart = tab0;
		tabStart.StartTable();
			tab0.EndGame();
			return tab0.CalcMoney(&tabStart);
	}
	clDoubleCP money; money = 0;
	ErrMessage("Не реализованная функция CalcAllInFlop","Error");
	return money;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcAllInTurn(clSitHoldem &sit)
{
	if (sit.CNPlayerIsCard() == 1)
	{
		clCalcResTable tab0, tabStart;
		tab0 = sit;
		tabStart = tab0;
		tabStart.StartTable();
		tab0.EndGame();
		return tab0.CalcMoney(&tabStart);
	}
	clDoubleCP money; money = 0;
	ErrMessage("Не реализованная функция CalcAllInTurn", "Error");
	return money;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcShowDownD(clSitHoldem &sit)
{
	clIntCP res = sit.CalcShowDown();
	clDoubleCP money; 
	for (int i = 0; i < CN_PLAYER; i++)
		money[i] = res[i];
	return money;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*clGamePoker PokerWin(clGamePoker *gamePoker)
{
	clGamePoker game = gamePoker[0];
	for (int i = 1; i<CN_PLAYER; i++)
	{
		if (game<gamePoker[i])
			game = gamePoker[i];
	}
	return(game);

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void DistribPot(clSitHoldem *table, clGamePoker *gamePoker)
{
	bool win[CN_PLAYER];
	int colViewCards = table->CNPlayerIsCard();
	int totalBetPl[CN_PLAYER];
	//    int rake=0;
	for (int i = 0; i<CN_PLAYER; i++)
	{
		win[i] = false;
		totalBetPl[i] = table->TotalBetPl(i);
	}
	clGamePoker minGame = GamePokerMinGame();
	while (colViewCards>0)
	{
		int n = 0, bet = -1;
		clGamePoker comb;
		comb = PokerWin(gamePoker);
		for (int i = 0; i<CN_PLAYER; i++)
		{
			if (table->PlayerIsCard(i) && comb == gamePoker[i])
			{
				win[i] = true;
				n++;
				if (bet == -1)
				{
					bet = totalBetPl[i];
				}
				else if (bet>totalBetPl[i])
				{
					bet = totalBetPl[i];
				}
			}
		}
		int totalB = 0;
		for (int i = 0; i<CN_PLAYER; i++)
		{
			if (totalBetPl[i] <= bet)
			{
				totalB += totalBetPl[i];
				totalBetPl[i] = 0;
			}
			else
			{
				totalB += bet;
				totalBetPl[i] -= bet;
			}
		}
		int ost = 0;

		if (n == 0)
		{
			ErrMessage("Деление на 0", "Ошибка");
			n = 1;
		}

		ost = totalB%n;
		for (int i = 0; i<CN_PLAYER; i++)
			if (win[i])
			{
				table->ExecAct(HistAction(i, ACT_WIN, totalB / n + ost));
				ost = 0;
			}
		for (int i = 0; i<CN_PLAYER; i++)
		{
			if (totalBetPl[i] == 0)
			{
				if (!(gamePoker[i] == minGame))
				{
					win[i] = false;
					colViewCards--;
					gamePoker[i] = minGame;
				}
			}
		}
	}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void    clCalcResTable::operator = (clSitHoldem &sit)
{
	_cnPl = sit.CnPlayer();
	_bb = sit.BigBlind();
	for (int i = 0; i<CN_PLAYER; i++)
		if (sit.PlayerInPlay(i))
	{
		tpPlAtr *atr = sit.PlayerAtr(i);
		_isCard[i] = atr->_isCard;
		_money[i] = atr->_money;
		_bet[i] = atr->_totalBetPl + atr->_bankPl;
		_nbHAll[i] = ((clHand *)sit.PlayerCards(i))->NbHandAll();
	}
		else
			_isCard[i] = false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void    clCalcResTable::StartTable()
{
	for (int i = 0; i<CN_PLAYER; i++)
	{
		_money[i] += _bet[i];
		_bet[i] = 0;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP clCalcResTable::CalcMoney(clCalcResTable *StartTable)				
{
	clDoubleCP res; 
	for (int i = 0; i < CN_PLAYER;i++)
		res[i]=(i<_cnPl)? _money[i] - StartTable->_money[i] : 0.;
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void    clCalcResTable::EndGame()
{
	int bank = 0, nbPl = -1;
	for (int i = 0; i<_cnPl; i++)
	{
		if (_isCard[i])
			nbPl = i;
		bank += _bet[i];
		_bet[i] = 0;
	}
	_money[nbPl] += bank;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void    clCalcResTable::WinPlayer(int nbPl)
{
	int bank = 0, bet = _bet[nbPl];
	for (int i = 0; i<_cnPl; i++)
		if (bet>_bet[i])
		{
			bank += _bet[i];
			_bet[i] = 0;
		}
		else
		{
			bank += bet;
			_bet[i] -= bet;
		}
	_money[nbPl] += bank;
	for (int i = 0; i<CN_PLAYER; i++)
		if (_bet[i] == 0)
			_isCard[i] = false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void    clCalcResTable::Win2Player(int nbPl1, int nbPl2)
{
	int bank = 0, bet = (_bet[nbPl1]>_bet[nbPl2]) ? _bet[nbPl2] : _bet[nbPl1];
	for (int i = 0; i<_cnPl; i++)
		if (bet>_bet[i])
		{
			bank += _bet[i];
			_bet[i] = 0;
		}
		else
		{
			bank += bet;
			_bet[i] -= bet;
		}
	_money[nbPl1] += bank / 2;
	_money[nbPl2] += bank / 2;
	int ost = bank % 2;
	if (ost>0)
		if (HistInPosition(nbPl1, nbPl2)) _money[nbPl2] += ost;
		else _money[nbPl1] += ost;
	WinPlayer((_bet[nbPl1]>_bet[nbPl2]) ? nbPl1 : nbPl2);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void    clCalcResTable::Win3Player(int nbPl1, int nbPl2, int nbPl3)
{
	int bank = 0, nbMin=nbPl1;
	if (_bet[nbPl1] > _bet[nbPl2])
		nbMin = nbPl2;
	if (_bet[nbMin] > _bet[nbPl3])
		nbMin = nbPl3;
	int bet = _bet[nbMin];
	for (int i = 0; i<_cnPl; i++)
		if (bet>_bet[i])
		{
			bank += _bet[i];
			_bet[i] = 0;
		}
		else
		{
			bank += bet;
			_bet[i] -= bet;
		}
	_money[nbPl1] += bank / 3;
	_money[nbPl2] += bank / 3;
	_money[nbPl3] += bank / 3;
	int ost = bank % 3;
	if (ost > 0)
	{
		int nb0 = (HistInPosition(nbPl1, nbPl2)) ? nbPl2 : nbPl1;
		nb0 = (HistInPosition(nb0, nbPl3)) ? nbPl3 : nb0;
		_money[nb0] += ost;
	}
	if (nbMin == nbPl1)
		Win2Player(nbPl2, nbPl3);
	else if (nbMin == nbPl2)
		Win2Player(nbPl1, nbPl3);
	else
		Win2Player(nbPl1, nbPl2);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int clCalcResTable::CnPlayersIsCard()
{
	int cn = 0;
	for (int i = 0; i<CN_PLAYER; i++)
		if (_isCard[i])
			cn++;
	return cn;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//clStacks    clCalcResTable::GetStacks();
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcResFor2Player(clCalcResTable *table, clCalcResTable *StartTable)
{
	int nbPl[CN_PLAYER], cur = 0;
	for (int i = 0; i<CN_PLAYER; i++)
		if (table->_isCard[i])
			nbPl[cur++] = i;
	clDoubleCP res;
	tpWin2 w2 = HCompWinHandAll(table->_nbHAll[nbPl[0]], table->_nbHAll[nbPl[1]]);
	clCalcResTable t0 = *table;
	t0.WinPlayer(nbPl[0]);
	t0.EndGame();
	res = t0.CalcMoney(StartTable)*w2._win;
	t0 = *table;
	t0.Win2Player(nbPl[0], nbPl[1]);
	t0.EndGame();
	res += t0.CalcMoney(StartTable)*w2._eq;
	t0 = *table;
	t0.WinPlayer(nbPl[1]);
	t0.EndGame();
	res += t0.CalcMoney(StartTable)*w2._loss;
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcResFor3Player(clCalcResTable *table, clCalcResTable *StartTable)
{
	return CalcResForManyPlayer(table, StartTable);
	int nbPl[CN_PLAYER], cur = 0;
	for (int i = 0; i<CN_PLAYER; i++)
		if (table->_isCard[i])
			nbPl[cur++] = i;
	clDoubleCP res, res0; res = 0;
	clHand hand;
	hand.SetHandAll(table->_nbHAll[nbPl[0]]);
	int nb0 = hand.NbHand();
	hand.SetHandAll(table->_nbHAll[nbPl[1]]);
	int nb1 = hand.NbHand();
	hand.SetHandAll(table->_nbHAll[nbPl[2]]);
	int nb2 = hand.NbHand();
	stVerManyHands w3 = SHCHandsComprasion(nb0,nb1,nb2);
	for (int i = 0; i<3; i++)
	{
		clCalcResTable t0 = *table;
		t0.WinPlayer(nbPl[i]);
		double mult = w3.WinEQHand(i);
		if (t0.CnPlayersIsCard() == 2)
			res0 = CalcResFor2Player(&t0, StartTable)*mult;
		else
		{
			t0.EndGame();
			res0 = t0.CalcMoney(StartTable)*mult;
		}
		res += res0;
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcResForManyPlayer(clCalcResTable *table, clCalcResTable *StartTable)
{
	int nbPl[CN_PLAYER], cnPl = 0;
	for (int i = 0; i < CN_PLAYER; i++)
		if (table->_isCard[i])
			nbPl[cnPl++] = i;
	clDoubleCP resW;
	resW = 0;
	for (int i = 0; i < cnPl-1; i++)
		for (int j = i + 1; j < cnPl; j++)
				{
					tpWin2 w2 = HCompWinHandAll(table->_nbHAll[nbPl[i]], table->_nbHAll[nbPl[j]]); 
					resW[i] += w2.WinEq();
					resW[j] += w2.LossEq();
				}
	resW.Norm();
	clDoubleCP res, res0; res = 0;
	for (int i = 0; i < cnPl; i++)
	{
		clCalcResTable t0 = *table;
		t0.WinPlayer(nbPl[i]);
		int cnPl0 = t0.CnPlayersIsCard();
		if (cnPl0 <= 1)
		{
			t0.EndGame();
			res0 = t0.CalcMoney(StartTable)*resW[i];
		}
		else if (cnPl0 == 2)
			res0 = CalcResFor2Player(&t0, StartTable)*resW[i];
		else if (cnPl0 == 3)
			res0 = CalcResFor3Player(&t0, StartTable)*resW[i];
		else
			res0 = CalcResForManyPlayer(&t0, StartTable)*resW[i];
		res += res0;
	}
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcSitRes(clSitHoldem &sit, tpSitRes *res)
{

	clCalcResTable tab0, tabStart;
	tab0 = sit;
	tabStart = tab0;
	tabStart.StartTable();
	int nbPl[CN_PLAYER], cnPl = 0;
	for (int i = 0; i < CN_PLAYER; i++)
		if (tab0._isCard[i])
			nbPl[cnPl++] = i;
	for (int i = 0; i < CN_PLAYER; i++)
		res->_nbPl[i] = (i < cnPl) ? nbPl[i] : 0;
	res->_cnPl = cnPl;
	if (cnPl == 1)
	{
		res->_res.resize(1);
		tab0.EndGame();
		res->_res[0] = tab0.CalcMoney(&tabStart);
	}
	else if (cnPl == 2)
	{
		res->_res.resize(3);
		clCalcResTable t0 = tab0;
		t0.WinPlayer(nbPl[0]);
		t0.EndGame();
		res->_res[0] = t0.CalcMoney(&tabStart);
		t0 = tab0;
		t0.Win2Player(nbPl[0], nbPl[1]);
		t0.EndGame();
		res->_res[1] = t0.CalcMoney(&tabStart);
		t0 = tab0;
		t0.WinPlayer(nbPl[1]);
		t0.EndGame();
		res->_res[2] = t0.CalcMoney(&tabStart);
	}
	else if (cnPl == 3)
	{
		res->_res.resize(13);
		clCalcResTable t0 = tab0;
		t0.WinPlayer(nbPl[0]);
		t0.WinPlayer(nbPl[1]);
		t0.EndGame();
		res->_res[0] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.WinPlayer(nbPl[0]);
		t0.WinPlayer(nbPl[2]);
		t0.EndGame();
		res->_res[1] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.WinPlayer(nbPl[0]);
		t0.Win2Player(nbPl[1], nbPl[2]);
		t0.EndGame();
		res->_res[2] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.WinPlayer(nbPl[1]);
		t0.WinPlayer(nbPl[0]);
		t0.EndGame();
		res->_res[3] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.WinPlayer(nbPl[1]);
		t0.WinPlayer(nbPl[2]);
		t0.EndGame();
		res->_res[4] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.WinPlayer(nbPl[1]);
		t0.Win2Player(nbPl[0], nbPl[2]);
		t0.EndGame();
		res->_res[5] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.WinPlayer(nbPl[2]);
		t0.WinPlayer(nbPl[0]);
		t0.EndGame();
		res->_res[6] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.WinPlayer(nbPl[2]);
		t0.WinPlayer(nbPl[1]);
		t0.EndGame();
		res->_res[7] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.WinPlayer(nbPl[2]);
		t0.Win2Player(nbPl[0], nbPl[1]);
		t0.EndGame();
		res->_res[8] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.Win2Player(nbPl[0], nbPl[1]);
		t0.EndGame();
		res->_res[9] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.Win2Player(nbPl[0], nbPl[2]);
		t0.EndGame();
		res->_res[10] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.Win2Player(nbPl[1], nbPl[2]);
		t0.EndGame();
		res->_res[11] = t0.CalcMoney(&tabStart);

		t0 = tab0;
		t0.Win3Player(nbPl[0], nbPl[1], nbPl[2]);
		t0.EndGame();
		res->_res[12] = t0.CalcMoney(&tabStart);

	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcSitEV(clSitHoldem &sit, tpCardsGame *cards, tpSitRes *res)
{
	if (sit.NameTrade() == TRADE_PREFLOP)
	{
		if (res->_cnPl == 1)
			return res->_res[0];
		else if (res->_cnPl == 2)
		{
			clDoubleCP res0;
			int n0 = res->_nbPl[0], n1 = res->_nbPl[1];
			tpWin2 w2 = HCompWinHandAll(cards->_handsPl[n0].NbHandAll(), cards->_handsPl[n1].NbHandAll());
			res0 = res->_res[0] * w2._win;
			res0 += res->_res[1] * w2._eq;
			res0 += res->_res[2] * w2._loss;
			return res0;
		}
		else if (res->_cnPl == 3)
		{
			int n0 = res->_nbPl[0], n1 = res->_nbPl[1], n2 = res->_nbPl[2];
			stVerManyHands w3 = SHCHandsComprasion(cards->_handsPl[n0].NbHand(), cards->_handsPl[n1].NbHand(), cards->_handsPl[n2].NbHand());

			clDoubleCP res0;
			res0 = res->_res[0] * w3._win123;
			res0 += res->_res[1] * w3._win132;
			res0 += res->_res[2] * w3._win122;
			res0 += res->_res[3] * w3._win213;
			res0 += res->_res[4] * w3._win312;
			res0 += res->_res[5] * w3._win212;
			res0 += res->_res[6] * w3._win231;
			res0 += res->_res[7] * w3._win321;
			res0 += res->_res[8] * w3._win221;
			res0 += res->_res[9] * w3._win112;
			res0 += res->_res[10] * w3._win121;
			res0 += res->_res[11] * w3._win211;
			res0 += res->_res[12] * w3._win111;
			return res0;
		}
		else
		{
			cards->SetHandsToSit(sit);
			return CalcSitEV(sit);
		}
	}
	else if (res->_cnPl == 1)
			return res->_res[0];
	else if (res->_cnPl == 2)
	{
		int n0 = res->_nbPl[0], n1 = res->_nbPl[1];
		if (cards->_game[n0] > cards->_game[n1])
			return res->_res[0];
		else if (cards->_game[n1] > cards->_game[n0])
			return res->_res[2];
		else
			return res->_res[1];
	}
	else if (sit.CnPlayer() == 3)
	{
		int n0 = res->_nbPl[0], n1 = res->_nbPl[1], n2 = res->_nbPl[2];
		if (cards->_game[n0] > cards->_game[n1])
		{
			if (cards->_game[n1] > cards->_game[n2])
				return res->_res[0];
			else if (cards->_game[n1] == cards->_game[n2])
				return res->_res[2];
			else if (cards->_game[n0] > cards->_game[n2])
				return res->_res[1];
			else if (cards->_game[n0] < cards->_game[n2])
				return res->_res[6];
			else
				return res->_res[10];
		}
		if (cards->_game[n0] < cards->_game[n1])
		{
			if (cards->_game[n0] > cards->_game[n2])
				return res->_res[3];
			else if (cards->_game[n0] == cards->_game[n2])
				return res->_res[5];
			else if (cards->_game[n1] > cards->_game[n2])
				return res->_res[4];
			else if (cards->_game[n1] < cards->_game[n2])
				return res->_res[7];
			else
				return res->_res[11];
		}
		else
		{
			if (cards->_game[n0] > cards->_game[n2])
				return res->_res[9];
			else if (cards->_game[n0] == cards->_game[n2])
				return res->_res[12];
			else
				return res->_res[8];
		}
	}
	else
	{
		cards->SetHandsToSit(sit);
		return CalcSitEV(sit);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcSitDistribWin(clSitHoldem &sit)
{
	clDoubleCP res;
	if (sit.CNPlayerIsCard() == 1)
	{
		res = 0;
		for (int i = 0; i < sit.CnPlayer(); i++)
			res[i] = sit.PlMoney(i);
		sit.CloseBank();
		for (int i = 0; i < sit.CnPlayer(); i++)
		{
			if (sit.PlayerIsCard(i))
				sit.PlayerAtr(i)->_money += sit.TotalPot();
			res[i] = sit.PlayerMoney(i) - res[i];
		}
	}
	else
	{
		res = CalcShowDownD(sit);
//		for (int i = 0; i < sit.CnPlayer(); i++)
//			sit.PlayerAtr(i)->_money += res[i];
	}
	sit.TotalPot() = 0;
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CN_CALC_AUTOPL          10000

int		CheckAllIn(clGameHistory &hist);
clDoubleCP	PlayToEnd(clSitHoldem &sit);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double    FinResGameEV(clGameHistory &hist, int nb)
{
	if (!hist.GetSeat(nb)->_busy)
		return(0);
	int nbA = CheckAllIn(hist);
	if (nbA == -1)
	{
		return FinResGame(hist,nb);
	}
	else
	{
		clSitHoldem sit, sit0;
		sit.Build(&hist, nbA+1);
		if (sit.NameTrade() == TRADE_PREFLOP)
		{
			clDoubleCP res = CalcSitEV(sit);
			return res[nb];
		}
		else
		{
			clCol col;
			col.New();
			int myNb = hist.ViewPlayer();
			for (int i = 0; i<CN_PLAYER; i++)
				if (sit.PlayerIsCard(i) || i == myNb)     //????? Íàøè êàðòû
				{
					col.Skip(hist.GetSeat(i)->_cards[0]);
					col.Skip(hist.GetSeat(i)->_cards[1]);
				}
			for (int i = 0; i<sit.CnCardTable(); i++)
				col.Skip(sit.CardsTable()[i]);
			double *res = (double *)malloc(CN_CALC_AUTOPL*sizeof(double));
			for (int i = 0; i<CN_CALC_AUTOPL; i++)
			{
				col.Shufle();
				clCol col1 = col;
				sit0 = sit;
				for (int j = sit.CnCardTable(); j<5; j++)
					sit0.CardsTable()[j] = col1.GiveCard();
				res[i] = PlayToEnd(sit0)[nb];
			}
			double disp,val = AverageAndDisp(CN_CALC_AUTOPL, res, disp);
			free(res);
			return val;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		FinResGame(clGameHistory &hist, int nbH)
{
	clSitHoldem sit;
	sit.Build(&hist,hist.CnAct());
	return sit.PlayerMoney(nbH) - sit.PlMoney(nbH);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		CheckAllIn(clGameHistory &hist)
{
	clSitHoldem sit;
	sit.ExecHist(&hist, 0);
	for (int i = 0; i<hist.CnAct(); i++)
	{
		sit.ExecNextAct();
//		if (sit.CNPlayerInTrade() == 0 && sit.CNPlayerIsCard()>1)
		if (sit.CNPlayerInTrade() < 2 && sit.CNPlayerIsCard() > 1 && sit.Indicator() == -1)
			return i;
	}
	return(-1);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP	PlayToEnd(clSitHoldem &sit)
{
	if (sit.CNPlayerInTrade() < 2 && sit.CNPlayerIsCard()>1 && sit.Indicator() == -1)
		switch (sit.NameTrade())
	{
			case TRADE_PREFLOP: sit.ExecDis(HistCreateDis(ACT_DELFLOP)); 
			case TRADE_FLOP: sit.ExecDis(HistCreateDis(ACT_DELTURN));
			case TRADE_TURN: sit.ExecDis(HistCreateDis(ACT_DELRIVER));
	}
	return CalcShowDownD(sit);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		FinResReplayAllIn(clGameHistory &hist, int nb)
{
	if (!hist.GetSeat(nb)->_busy)
		return(0);
	int nbA = CheckAllIn(hist);
	if (nbA == -1)
		return FinResGame(hist, nb);
	else
	{
		clSitHoldem sit, sit0;
		sit.Build(&hist, nbA + 1);

		clCol col;
		col.New();
		int myNb = hist.ViewPlayer();
		for (int i = 0; i<CN_PLAYER; i++)
			if (sit.PlayerIsCard(i) || i == myNb)     //????? Íàøè êàðòû
			{
				col.Skip(hist.GetSeat(i)->_cards[0]);
				col.Skip(hist.GetSeat(i)->_cards[1]);
			}
		for (int i = 0; i<sit.CnCardTable(); i++)
			col.Skip(sit.CardsTable()[i]);

		col.Shufle();
		clCol col1 = col;
		sit0 = sit;
		for (int j = sit.CnCardTable(); j<5; j++)
			sit0.CardsTable()[j] = col1.GiveCard();
		return PlayToEnd(sit0)[nb];
	}
}
//---------------------------------------------------------------------------
