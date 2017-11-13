//---------------------------------------------------------------------------
//#include "stdafx.h"
#pragma hdrstop

#include "C:\home\holdem\Common\sitHoldem.h"

char    *glSitHoldemName[CN_SIT]={"Чистая вода","Лимпы","Raise","2-Raise","Horror","Reply",};

enTradeAction WhatTA(enAction act0,enAction act1);

//---------------------------------------------------------------------------
bool    PosInPosition(int pos1,int pos2)
{
    if(pos2==POS_BB) return pos1 != POS_SB;
    if(pos2==POS_SB) return true;
    if(pos1<=POS_SB) return false;
    return pos1<pos2;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void clStacks::Norm(int sum)
{
    int sum1=Sum();
    if(sum1==0) return;
    double m=((double)sum)/sum1;
    for(int i=0;i<CN_PLAYER;i++)
        _arr[i]=(int)(_arr[i]*m+0.5);
    sum1=Sum()-sum;
    for(int i=0;i<abs(sum1);i++)
        _arr[i]+=(sum1>0)? -1 : 1;
}
//---------------------------------------------------------------------------
void    clStacks::Init(int bb, int ante, int mon0, int mon1, int mon2, int mon3, int mon4, int mon5)
{
	_bb = bb;
	_ante = ante;
	_arr[0] = mon0;
	_arr[1] = mon1;
	_arr[2] = mon2;
	_arr[3] = mon3;
	_arr[4] = mon4;
	_arr[5] = mon5;
}
//---------------------------------------------------------------------------
int     clStacks::CnPlayer()
{
    int cn=0;
    for(int i=0;i<CN_PLAYER;i++)
        if(_arr[i]==0)
            return cn;
        else
            cn++;
    return cn;
}
//---------------------------------------------------------------------------
void    clStacks::MoveButton(int cnPl)
{
    int money=_arr[cnPl-1];
    for(int i=cnPl-1;i>0;i--)
        _arr[i]=_arr[i-1];
    _arr[0]=money;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*AnsiString  SitHoldemSitToString(enSitName ns)
{
    return glSitHoldemName[ns];
}*/
//---------------------------------------------------------------------------
void    clSitHoldem::Clear()
{
	clGameHistory::Clear();
	_nbAct = _posHero = -1;
	_totalPot = _cnCardTable = 0;
	for (int i = 0; i<CN_PLAYER; i++)
		_pl[i].Clear();
	_trade = TRADE_UNDEF;
	_indicator = _begTrade = -1;
	_cnPlInTrade=_cnPlIsCard=0;
}
//---------------------------------------------------------------------------
void    clSitHoldem::operator = (clSitHoldem &table)
{
	clGameHistory *ptrH = &table;
	*((clGameHistory*)this) = *ptrH;
	char *ptrThis = (char *)this, *ptr = (char *)&table;
	memcpy(ptrThis + sizeof(clGameHistory), ptr + sizeof(clGameHistory), sizeof(clSitHoldem) - sizeof(clGameHistory));
}
//---------------------------------------------------------------------------
bool    clSitHoldem::operator == (clSitHoldem &table)
{
	if (_nbAct != table._nbAct)
		return false;
	clGameHistory hist1 = *PtrHistory(), hist2 = *table.PtrHistory();
	hist1._arrAct._cnAct = _nbAct;
	hist2._arrAct._cnAct = _nbAct;
	return (hist1 == hist2);
}
//---------------------------------------------------------------------------
bool    clSitHoldem::Include(clSitHoldem &table)
{
	if (_nbAct < table._nbAct)
		return false;
	clGameHistory hist1 = *PtrHistory(), hist2 = *table.PtrHistory();
	hist1._arrAct._cnAct = table._nbAct;
	hist2._arrAct._cnAct = table._nbAct;
	return (hist1 == hist2);
}
//---------------------------------------------------------------------------
void    clSitHoldem::ExecHist(clGameHistory *hist, int nbAct)
{
	*((clGameHistory*)this) = *hist;
	ExecHist(nbAct);
}
//---------------------------------------------------------------------------
void    clSitHoldem::ExecHist(int nbAct)
{
	_nbAct = nbAct;
	for (int i = 0; i<CN_PLAYER; i++)
	{
		_pl[i]._totalBetPl = _pl[i]._bankPl = 0;
		_pl[i]._isCard = _pl[i]._viewCards = false;
		_pl[i]._money = PlMoney(i);
	}
	_indicator = -1;
	_totalPot = _cnCardTable = 0;
	_posHero = ViewPlayer();
	//    _cnPl=_atrHH._cnPl;
	//    _nbSB=_nbBB=INT_UNDEF;
	for (int i = 0; i<nbAct; i++)
		ExecAct(i);
}
//---------------------------------------------------------------------------
void    clSitHoldem::CloseBank()
{
	for (int i = 0; i<CN_PLAYER; i++)
	{
		_totalPot += _pl[i]._bankPl;
		_pl[i]._totalBetPl += _pl[i]._bankPl;
		_pl[i]._bankPl = 0;
	}
}
//---------------------------------------------------------------------------
void    clSitHoldem::CheckIndicator()
{
	if (_indicator == -1)
	{
		_begTrade = -1;
		if (_cnCardTable == 5)
			CloseBank();
	}
}
//---------------------------------------------------------------------------
void    clSitHoldem::ExecAct(int nb)
{
	tpAction *act = GetAct(nb);
	switch (act->_dis._act)
	{
		//        case ACT_SIT_OUT :  _seat[ptrAct->_nbSeat]._outPlay=true;break;
		//        case ACT_RETURNED : _seat[ptrAct->_nbSeat]._outPlay=false;break;
		//        case ACT_UNDEF  :   break;
	case ACT_POSTANTE:
		_pl[act->_pos]._bankPl += act->_dis._money;
		_pl[act->_pos]._money -= act->_dis._money;
		break;
	case ACT_CHECK:
		_indicator = NextPlayerMove();
		CheckIndicator();
		break;
	case ACT_FOLD:   _pl[act->_pos]._isCard = false;
		//            if(_atrHH._site==SITE_POKERSTARS)
		//                if(_pl[act->_nbSeat]._money==0)
		//                {
		//                    _cnPlIsCard--;
		//                    break;
		//                }
		_cnPlInTrade--;
		_cnPlIsCard--;
		if (_begTrade == _indicator)
			_begTrade = _indicator = NextPlayerMove();
		else
			_indicator = NextPlayerMove();
		CheckIndicator();
		break;
	case ACT_CALL:
		_pl[act->_nbSeat]._bankPl += act->_dis._money;
		_pl[act->_nbSeat]._money -= act->_dis._money;
		if (_pl[_indicator]._money == 0)
			_cnPlInTrade--;
		if (_begTrade == _indicator && _pl[_indicator]._money == 0)
			_begTrade = _indicator = NextPlayerMove();
		else
			_indicator = NextPlayerMove();
		CheckIndicator();
		break;
	case ACT_BET:
		_pl[act->_nbSeat]._bankPl += act->_dis._money;
		_pl[act->_nbSeat]._money -= act->_dis._money;
		if (_pl[_indicator]._money == 0)
			_cnPlInTrade--;
		_begTrade = act->_nbSeat;
		_indicator = NextPlayerMove();
		break;
	case ACT_RAISE_TO:
		act->_dis._money = (act->_dis._money - _pl[act->_nbSeat]._bankPl);
		act->_dis._act = ACT_RAISE;
	case ACT_RAISE:
		_pl[act->_nbSeat]._bankPl += act->_dis._money;
		_pl[act->_nbSeat]._money -= act->_dis._money;
		ActRaiseChangeIndicator();
		break;
	case ACT_ALL_IN:
		_pl[act->_nbSeat]._bankPl += act->_dis._money;
		_pl[act->_nbSeat]._money -= act->_dis._money;
		_cnPlInTrade--;
		if (_pl[_indicator]._bankPl <= AnotherMaxBet(_indicator))
		{
			if (_begTrade == _indicator)
			{
				_begTrade = -1;
				_begTrade = _indicator = NextPlayerMove();
			}
			else
				_indicator = NextPlayerMove();
			CheckIndicator();
		}
		else
		{
			_begTrade = -1;
			_begTrade = _indicator = NextPlayerMove();
		}
		break;
	case ACT_POSTSB:
		if (_pl[POS_SB]._bankPl>0)
			CloseBank();
		_pl[POS_SB]._bankPl += act->_dis._money;
		_pl[POS_SB]._money -= act->_dis._money;
		break;
	case ACT_POSTBB:
		if (_pl[POS_BB]._bankPl>0)
			CloseBank();
		_pl[POS_BB]._bankPl += act->_dis._money;
		_pl[POS_BB]._money -= act->_dis._money;
		break;
#ifndef CALC_FOR_TOURN
	case ACT_POST_DEAD:
		if (act->_dis._money>BigBlind())
		{
			_totalPot = act->_dis._money - BigBlind();
			_pl[act->_nbSeat]._bankPl += BigBlind();
		}
		else
			_pl[act->_nbSeat]._bankPl += act->_dis._money;
		_pl[act->_nbSeat]._money -= act->_dis._money;
		break;
#endif
	case ACT_RETURN_UNCALLED_BET:
	{
		if (act->_dis._money>0)
		{
			_totalPot -= act->_dis._money;
			_pl[act->_nbSeat]._totalBetPl -= act->_dis._money;
			_pl[act->_nbSeat]._money += act->_dis._money;
		}
		break;
	}
	case ACT_WIN:
		if (_totalPot != 0)
		{
			_totalPot = 0;
			for (int i = 0; i<MaxCnSeat(); i++)
				_pl[i]._bankPl = 0;
		}
		if (act->_dis._money>0)
		{
			_pl[act->_nbSeat]._bankPl = act->_dis._money;
			_pl[act->_nbSeat]._money += _pl[act->_nbSeat]._bankPl;
		}
		break;
	case ACT_SHOW:
		_pl[act->_nbSeat]._viewCards = true;
		break;
	case ACT_DELCARD:
		for (int i = 0; i<CN_PLAYER; i++)
			if (IsPlayer(i))
				_pl[i]._isCard = true;
		_trade = TRADE_PREFLOP;
		_begTrade = -1;
		_cnPlInTrade = 0;
		_cnPlIsCard = 0;
		for (int i = 0; i<MaxCnSeat(); i++)
			if (_pl[i]._isCard)
			{
				_cnPlIsCard++;
				if (_pl[i]._money>0)
					_cnPlInTrade++;
				else if (!PlayerInPlay(i))//_atrHH._site==SITE_POKERSTARS &&
					_cnPlInTrade++;
			}
		_begTrade = _indicator = NextPlayerMove(POS_BB);
		break;
	case ACT_DELFLOP:
		_cnCardTable = 3;
		CloseBank();
		_trade = TRADE_FLOP;
		InitIndicatorOnFlop();
		break;
	case ACT_DELTURN:
		_cnCardTable = 4;
		CloseBank();
		_trade = TRADE_TURN;
		InitIndicatorOnFlop();
		break;
	case ACT_DELRIVER:
		_cnCardTable = 5;
		CloseBank();
		_trade = TRADE_RIVER;
		InitIndicatorOnFlop();
		break;
	}
}
//---------------------------------------------------------------------------
inline  void    clSitHoldem::InitIndicatorOnFlop()
{
	if (_cnPlInTrade <= 1)
		_begTrade = _indicator = -1;
	else
	{
		_begTrade = -1;
		if (this->CnPlayer() == 2)
			_begTrade = _indicator = POS_BB;
		else
			_begTrade = _indicator = NextPlayerMove(POS_BUTTON);
	}
}
//---------------------------------------------------------------------------
bool	clSitHoldem::EndedSit()
{
	if (Indicator() != -1) return false;
	if (CNPlayerIsCard() == 1) return true;
	return CnCardTable() == 5;
}
//---------------------------------------------------------------------------
void	clSitHoldem::DealCards(clCol deck)
{
	for (int i = 0; i < CnPlayer(); i++)
	{
		PlayerCards(i)[0] = deck.GiveCard();
		PlayerCards(i)[1] = deck.GiveCard();
	}
	for (int j = 0; j < 5; j++)
		CardTable()[j] = deck.GiveCard();

}
//---------------------------------------------------------------------------
int		clSitHoldem::TotalBetAndBankAllPl()
{
	int total = 0;
	for (int i = 0; i < this->CnPlayer(); i++)
		total += (TotalBetPl(i) + PlayerBank(i));
	return total;
}
//---------------------------------------------------------------------------
clStacks    clSitHoldem::SitHGetStacks(clSitHoldem *table)
{
	clStacks res;
	res._bb = table->BigBlind();
	res._ante = table->Ante();
	for (int i = 0; i<CN_PLAYER; i++)
		if (table->IsPlayer(i))
			res[i] = table->PlayerStack(i);
		else
			res[i] = 0;
	return res;
}
//---------------------------------------------------------------------------
/*enPosition  clSitHoldem::Position(int nb)
{
int n=0,nb0=_hist.NextPlayerInPlay(NbBigBlind());
for(int i=0;i<CN_PLAYER;i++)
{
if(nb0==nb)
return  HistTablePositionForPlayer(&_hist,n);
nb0=_hist.NextPlayerInPlay(nb0);
n++;
}
return POS_UNDEF;
}
//---------------------------------------------------------------------------
int clSitHoldem::NbPosition(enPosition pos)
{
int n=pos;
n+=_hist.CnPlayerIsPlay()-6;
int nb0=_hist.NextPlayerInPlay(NbBigBlind());
for(int i=0;i<n;i++)
nb0=_hist.NextPlayerInPlay(nb0);
return nb0;
}
//---------------------------------------------------------------------------
bool    clSitHoldem::ClearWater()
{
if(NameTrade()==TRADE_PREFLOP)
for(int i=_hist.NbFirstAct(ACT_DELCARD)+1;i<_nbAct;i++)
if(HistIsActiveTradeAct(_hist._arrAct[i]._dis._act))
return false;
return true;
}*/
//---------------------------------------------------------------------------
void    clSitHoldem::ActRaiseChangeIndicator()
{
	if (_pl[_indicator]._money == 0)
	{
		_cnPlInTrade--;
		_begTrade = -1;
		_begTrade = _indicator = NextPlayerMove();
	}
	else
	{
		_begTrade = _indicator;
		_indicator = NextPlayerMove();
	}
}
//---------------------------------------------------------------------------
int  clSitHoldem::NextPlayerMove(int nb)
{
	if (_cnPlInTrade == 0) return -1;
	int nb0 = -1;
	for (int i = 0; i<CN_PLAYER - 1; i++)
	{
		if (--nb<0)
			nb = POS_EARLY;
		if (_pl[nb]._isCard)
			if (_pl[nb]._money>0)// && GetSeat(nb)->_inPlay)
			{
				nb0 = nb; break;
			}
			else if (!GetSeat(nb)->_inPlay)//_atrHH._site==SITE_POKERSTARS &&
			{
				nb0 = nb; break;
			}
	}
	int res = (nb0 == _begTrade) ? -1 : nb0;
	if (_cnPlInTrade == 1 && res != -1)
		if (MaxBet() <= _pl[res]._bankPl)
			res = -1;
	return res;
}
//---------------------------------------------------------------------------
int     clSitHoldem::AnotherMaxBet(int nbPl)
{
	int max = 0;
	for (int i = 0; i<MaxCnSeat(); i++)
		if (_pl[i]._isCard && _pl[i]._bankPl>max && i != nbPl)
			max = _pl[i]._bankPl;
	return max;
}
//---------------------------------------------------------------------------
int     clSitHoldem::MaxBet()
{
	int max = 0;
	for (int i = 0; i<MaxCnSeat(); i++)
		if (_pl[i]._isCard && _pl[i]._bankPl>max)
			max = _pl[i]._bankPl;
	return max;
}
//---------------------------------------------------------------------------
void    clSitHoldem::ExecAct(tpAction act)
{
	_arrAct._cnAct = _nbAct;
	AddAct(&act);
	ExecAct(_nbAct++);
}
//---------------------------------------------------------------------------
clGamePoker     clSitHoldem::GamePlayerHoldem(int nb)
{
	clGamePoker game;
	if (_cnCardTable == 5)
		game.GameHoldemQ(PlayerCards(nb), CardTable());
	else
		game.game = GM_UNDEF;
	return game;
}
//---------------------------------------------------------------------------
tpAction clSitHoldem::GoPrevNextTradeAct()
{
	if (_nbAct<CnAct())
		ExecAct(_nbAct++);
	tpAction *act = NULL;
	while (_nbAct<CnAct())
	{
		act = GetAct(_nbAct);
		if (HistIsTradeAct(act->_dis._act))
			break;
		ExecAct(_nbAct++);
	}
	return *act;
}
//---------------------------------------------------------------------------
int     clSitHoldem::CNPlayerInPlayWithMoney()
{
	int cn = 0;
	for (int i = 0; i<CN_PLAYER; i++)
		if (_seat[i]._busy && _pl[i]._money>0)
			cn++;
	return(cn);
}
//---------------------------------------------------------------------------
/*void    clSitHoldem::StartGame()
{
int cnPl=0;
for(int i=0;i<CN_PLAYER;i++)
{
_pl[i]._isCard=false;
_arrSeat._seat[i]._money=_pl[i]._money;
if(_pl[i]._money<=0)
_arrSeat._seat[i]._inPlay=_arrSeat._seat[i]._busy=false;
else
{
_arrSeat._seat[i]._inPlay=_arrSeat._seat[i]._busy=true;
cnPl++;
}
_pl[i]._totalBetPl=_pl[i]._bankPl=0;
_pl[i]._viewCards=false;
}
_atrHH._cnPl=cnPl;
_totalPot=_cnCardTable=0;
int nb1=NextPlayer(NbButton());
WrNbButton(nb1);
_arrAct.Clear();
_nbAct=0;
for(int i=0;i<CN_PLAYER;i++)
{
if(_arrSeat._seat[i]._busy)
{
tpAction act;
act._nbSeat=i;
act._dis._act=ACT_POSTANTE;
act._dis._money=(_arrSeat._seat[i]._money>_atrHH._ante)? _atrHH._ante : _arrSeat._seat[i]._money;
ExecAct(act);
}
}
_nbSB=NextPlayer(nb1);
_nbBB=NextPlayer(_nbSB);
tpAction act;
act._nbSeat=_nbSB;
act._dis._act=ACT_POSTSB;
int sb=BigBlind()/2;
act._dis._money=(_pl[_nbSB]._money>sb)? sb : _pl[_nbSB]._money;
if(act._dis._money>0)
ExecAct(act);
act._nbSeat=_nbBB;
act._dis._act=ACT_POSTBB;
act._dis._money=(_pl[_nbBB]._money>BigBlind())? BigBlind() : _pl[_nbBB]._money;
ExecAct(act);
act._dis._act=ACT_DELCARD;
act._nbSeat=NB_DILER;
ExecAct(act);
} */
//---------------------------------------------------------------------------
/*tpDis   clSitHoldem::CreateDisCallCheck()
{
int bet=MaxBet()-_pl[_indicator]._bankPl;
return (bet==0)? HistCreateDis(ACT_CHECK,0) : HistCreateDis(ACT_CALL,bet);
}*/
//---------------------------------------------------------------------------
bool    clSitHoldem::TableIsAllIn()
{
	int cn = 0, cnC = 0;
	if (_indicator != -1) return false;
	for (int i = 0; i<this->MaxCnSeat(); i++)
		if (this->PlayerIsCard(i))
		{
			cnC++;
			if (this->PlayerMoney(i)>0)
				cn++;
			if (cn >= 2)
				return false;
		}
	return (cnC >= 2);
}
//---------------------------------------------------------------------------
bool    clSitHoldem::MayBeCheck()
{
	return (_pl[_indicator]._bankPl == MaxBet());
}
//---------------------------------------------------------------------------
int clSitHoldem::NextPlayerWithCard(int nbPl)
{
	for (int i = 1; i<CN_PLAYER; i++)
	{
		nbPl++;
		if (nbPl >= CN_PLAYER) nbPl = 0;
		if (_pl[nbPl]._isCard) return nbPl;
	}
	return POS_UNDEF;
}
//---------------------------------------------------------------------------
int     clSitHoldem::CNPlayerIsCard(int *pos)
{
	int cn = 0;
	for (int i = 0; i<CN_PLAYER; i++)
		if (_pl[i]._isCard)
			pos[cn++] = i;
	return cn;
}
//---------------------------------------------------------------------------
bool    clSitHoldem::WrViewPlayer(int posH)
{
	int nbAct = this->NbFirstAct(ACT_DELCARD);
	if (nbAct == -1) return false;
	nbAct++;
	bool fnd;
	if (nbAct == this->CnAct()) fnd = false;
	else fnd = (GetAct(nbAct)->_dis._act == ACT_SHOW);
	if (fnd)
		GetAct(nbAct)->_pos = posH;
	else
	{
		InsertAct(nbAct, HistAction(posH, ACT_SHOW, 0));
		_nbAct++;
		_pl[posH]._viewCards = true;
	}
	_posHero = posH;
	return true;
}
//---------------------------------------------------------------------------
void    clSitHoldem::Build(clGameHistory *ptrH,int nbAct)
{
    if(nbAct==-1)
    {
        for(nbAct=2;nbAct<ptrH->CnAct();nbAct++)
            if(HistIsTradeAct(ptrH->GetAct(nbAct)->_dis._act))
                break;
    }
/*    else
        for(int i=2;i<nbAct;i++)
            if(ptrH->GetAct(i)->_dis._act==ACT_DELFLOP)
            {
                nbAct=i;
                break;
            }
*/
    ExecHist(ptrH,nbAct);
}
//---------------------------------------------------------------------------
/*void    clSitHoldem::Build(clShortSit *ss)
{
    SHBuildHistToStacks(PtrHistory(),ss->_stack);
    ExecHist(CnAct());
    for(int i=0;i<ss->_cnAct;i++)
        this->ExecAct(ss->_arrAct[i]);
    WrViewPlayer(_indicator);
} */
//---------------------------------------------------------------------------
bool    clSitHoldem::FirstSit()
{
    int nbA=NbFirstTradeAct(0);
    if(nbA==-1) return false;
    ExecHist(nbA);
    return true;
}
//---------------------------------------------------------------------------
bool    clSitHoldem::NextSit()
{
    int nbA=this->NbFirstTradeAct(_nbAct);
    if(nbA==-1) return false;
    for(;_nbAct<nbA;_nbAct++)
    {
//        if(GetAct(_nbAct)->_dis._act==ACT_DELFLOP)
//            return false;
        this->ExecAct(_nbAct);
    }
    return true;
}
//---------------------------------------------------------------------------
bool    clSitHoldem::NextSitPreFlop()
{
    int nbA=this->NbFirstTradeAct(_nbAct);
    if(nbA==-1) return false;
    for(;_nbAct<nbA;_nbAct++)
    {
        if(GetAct(_nbAct)->_dis._act==ACT_DELFLOP)
            return false;
        this->ExecAct(_nbAct);
    }
    return true;
}
//---------------------------------------------------------------------------
void	clSitHoldem::SetPrevTurnInd()
{
	if (_indicator != -1)
		for (int i = _nbAct - 2; i>0; i--)
			if (_arrAct._act[i]._pos == _indicator && HistIsTradeAct(_arrAct._act[i]._dis._act))
			{
				ExecHist(i - 1);
				return;
			}
}
//---------------------------------------------------------------------------
void    clSitHoldem::SitForIndicator(int round)
{
    int ind=Position();
    int nbA=NbTradeActPl(ind,0);
    if(round==1)
        nbA=NbTradeActPl(ind,nbA);
    ExecHist(nbA);
}
//---------------------------------------------------------------------------
enSitName   clSitHoldem::NameSit()
{
    if(RoundTrade()==1)
    {
        int cnR=CnRaiser();
        switch(cnR)
        {
            case 0 : return (CnLimper()==0)? SIT_CLEARWATER : SIT_LIMP;
            case 1 : return SIT_RAISE;
            case 2 : return SIT_RAISE2;
            default: return SIT_HORROR;
        }
    }
    else
        return SIT_REPLY;
}
//---------------------------------------------------------------------------
tpDis   clSitHoldem::CreateDis(enAction act, int money)
{
	switch (act)
	{
		case ACT_FOLD:
			if (MaxBet() == PlayerBank(Indicator()))
				return HistCreateDis(ACT_CHECK, 0);
			return HistCreateDis(act, money);
		case ACT_CALL:
		{
			int maxB = MaxBet();
			maxB -= PlayerBank(Indicator());
			if (maxB == 0)
				return HistCreateDis(ACT_CHECK, 0);
			maxB = (maxB>IndicatorMoney()) ? IndicatorMoney() : maxB;
			return HistCreateDis(ACT_CALL, maxB);
		}
		case ACT_RAISE_TO:
			money -= PlayerBank(Indicator());
		case ACT_RAISE:
		{
			tpDis dis0 = this->MinRaise();
			if (dis0._money > money) money = dis0._money;
			int maxB = MaxBet();
			if (money<maxB)
				return CreateDis(ACT_CALL, 0);

			int moneyI = IndicatorMoney();
			if (money>moneyI) 
				return HistCreateDis(ACT_ALL_IN, moneyI);
			enAction act = (maxB == 0 && this->NameTrade() > TRADE_PREFLOP) ? ACT_BET : ACT_RAISE;
			return HistCreateDis(act, money);
		}
		case ACT_ALL_IN:
		{
			int maxB = MaxBet();
			int moneyI = IndicatorMoney();
			int maxBI = maxB - PlayerBank(Indicator());
			if (moneyI<=maxBI)
				return HistCreateDis(ACT_CALL, moneyI);
			int i = 0;
			for (; i<CnPlayer(); i++)
				if (i != Indicator())
					if (this->PlayerIsCard(i) && this->PlayerMoney(i) + PlayerBank(i)>maxB)
						break;
			if (i == CnPlayer())
				return HistCreateDis(ACT_CALL, maxBI);
			return HistCreateDis(ACT_ALL_IN, moneyI);
		}
		default: return HistCreateDis(act, money);
	}
}
//---------------------------------------------------------------------------
tpDis   clSitHoldem::MinRaise()
{
	int i = 0;
	enAction act = ACT_UNDEF;
	switch (CnCardTable())
	{
		case 3: act = ACT_DELFLOP; break;
		case 4: act = ACT_DELTURN; break;
		case 5: act = ACT_DELRIVER; break;
	}
	if (act != ACT_UNDEF)
		for (i = _nbAct - 1; i > 0; i--)
			if (GetAct(i)->_dis._act == act)
				break;

	int arrBet[CN_PLAYER] = { 0, 0, 0, 0, 0, 0 };
	int bet = BigBlind(), maxBet = 0;
	for (; i < _nbAct; i++)
	{
		tpAction act = *GetAct(i);
		if (act._pos<CnPlayer() && act._pos>=0)
		{
			arrBet[act._pos] += act._dis._money;
			int curBet = arrBet[act._pos] - maxBet;
			if (curBet>bet) bet = curBet;
			if (arrBet[act._pos] > maxBet) maxBet = arrBet[act._pos];
		}
	}
	int moneyI=IndicatorMoney();
	if (maxBet - PlayerBank(Indicator()) >= moneyI)
        return HistCreateDis(ACT_CALL,moneyI);
	if (this->CNPlayerInTrade() == 1)
        return HistCreateDis(ACT_CALL,bet);
	int curBet = maxBet + bet;
	bet = curBet - PlayerBank(Indicator());
    if(bet>=moneyI)
        return HistCreateDis(ACT_ALL_IN,moneyI);
	return (maxBet == 0) ? HistCreateDis(ACT_BET, bet) : HistCreateDis(ACT_RAISE, bet);
}
//---------------------------------------------------------------------------
int     clSitHoldem::RoundTrade()
{
	int nbA = 2;
	switch (NameTrade())
	{
		case TRADE_PREFLOP: nbA = 3; break;
		case TRADE_FLOP: nbA = NbFirstAct(ACT_DELFLOP); break;
		case TRADE_TURN: nbA = NbFirstAct(ACT_DELTURN); break;
		case TRADE_RIVER: nbA = NbFirstAct(ACT_DELRIVER); break;
	}
    int round=1;
    for(int i=nbA;i<_nbAct;i++)
    {
        tpAction *act=GetAct(i);
        if(HistIsTradeAct(act->_dis._act))
            if(act->_pos==Indicator())
                round++;
    }
    return round;
}
//---------------------------------------------------------------------------
int     clSitHoldem::PosRaiser(int nb)
{
    tpAction arrAct[CN_PLAYER];
    int cnR=CnRaiser(arrAct);
    return (cnR<=nb)? POS_UNDEF : arrAct[nb]._pos;
}
//---------------------------------------------------------------------------
int     clSitHoldem::CnRaiser(tpAction *arrAct)
{
    int cnR=0;
    for(int i=2;i<_nbAct;i++)
    {
        tpAction *act=GetAct(i);
        if(HistIsRaiseAct(act->_dis._act))
            arrAct[cnR++]=*act;
        if(act->_dis._act==ACT_CALL && cnR>0)
            arrAct[cnR++]=*act;
    }
    return cnR;
}
//---------------------------------------------------------------------------
int     clSitHoldem::CnRaiser()
{
    tpAction arrAct[CN_PLAYER];
    return CnRaiser(arrAct);
}
//---------------------------------------------------------------------------
int     clSitHoldem::CnLimper()
{
    int cnL=0;
    for(int i=2;i<_nbAct;i++)
    {
        tpAction *act=GetAct(i);
        if(HistIsRaiseAct(act->_dis._act))
            return cnL;
        if(act->_dis._act==ACT_CALL)
            cnL++;
    }
    return cnL;
}
//---------------------------------------------------------------------------
int     clSitHoldem::CnLimper(tpAction *arrAct)
{
    int cnL=0;
    for(int i=2;i<_nbAct;i++)
    {
        tpAction *act=GetAct(i);
        if(HistIsRaiseAct(act->_dis._act))
            return cnL;
        if(act->_dis._act==ACT_CALL)
            arrAct[cnL++]=*act;
    }
    return cnL;
}
//---------------------------------------------------------------------------
clStacks    clSitHoldem::GetStacksInBB()
{
    clStacks res;
    res._bb=100;
    for(int i=0;i<CN_PLAYER;i++)
        if(this->IsPlayer(i))
			res[i] = this->StackPlayerInBB100(i);
        else
            res[i]=0;
    return res;
}
//---------------------------------------------------------------------------
tpDis    clSitHoldem::ChangeDis(tpDis dis)
{
    int bet=dis._money+this->PlayerBank(this->Indicator());
    switch(dis._act)
    {
        case ACT_RAISE_TO: bet=dis._money;dis._act=ACT_RAISE;
        case ACT_RAISE  :
            if(this->CNPlayerInTrade()==1)
                dis._act=ACT_ALL_IN;
            else if(dis._money>=this->IndicatorMoney())
                dis._act=ACT_ALL_IN;
            else if(bet>2*BigBlind())
                dis._act=ACT_ALL_IN;
            else
            {
                int max=0;
                for(int i=0;i<CN_PLAYER;i++)
                    if(i != Indicator() && this->PlayerIsCard(i))
                        if(this->PlayerMoney(i)>max)
                            max=PlayerMoney(i);
                if(dis._money>=max)
                    dis._act=ACT_ALL_IN;
            }
            break;
        case ACT_CALL   :
            if(this->CNPlayerInTrade()==1)
                dis._act=ACT_ALL_IN;
            else if(dis._money>=this->IndicatorMoney())
                dis._act=ACT_ALL_IN;
            else
            {
                int max=0;
                for(int i=0;i<CN_PLAYER;i++)
                    if(i != Indicator() && this->PlayerIsCard(i))
                        if(this->PlayerMoney(i)>max)
                            max=PlayerMoney(i);
                if(dis._money>=max)
                    dis._act=ACT_ALL_IN;
            }
            break;
    }
    return dis;
}
//---------------------------------------------------------------------------
bool    clSitHoldem::IsEqualDis(tpDis dis1,tpDis dis2)
{
    if(dis1._act != ACT_RAISE && dis1._act==dis2._act) return true;
    if(dis1._act==ACT_FOLD || dis2._act==ACT_FOLD) return false;
    dis1=ChangeDis(dis1);
    dis2=ChangeDis(dis2);
    return dis1._act==dis2._act;
}
//---------------------------------------------------------------------------
tpDis   clSitHoldem::CreateDisCallCheck()
{
    int bet=MaxBet()-_pl[_indicator]._bankPl;
	if (bet == 0)
		return HistCreateDis(ACT_CHECK, 0);
	if (bet > _pl[_indicator]._money)
		bet = _pl[_indicator]._money;
	return HistCreateDis(ACT_CALL, bet);
}
//---------------------------------------------------------------------------
bool    clSitHoldem::NoFold()
{
    int bet=MaxBet()-_pl[_indicator]._bankPl;
    return (bet<_pl[_indicator]._bankPl);
}
//---------------------------------------------------------------------------
int     clSitHoldem::EffStackSitForPlayer(int posH)
{
    if(!this->PlayerIsCard(posH)) return 0;
    int res=PlayerStack(posH),max=0;
    for(int i=0;i<CnPlayer();i++)
        if(i != posH)
            if(this->PlayerIsCard(i))
                if(max<PlayerStack(i))
                    max=PlayerStack(i);
    return (res>max)? max : res;
}
//---------------------------------------------------------------------------
int     clSitHoldem::EffStackSit()
{
	int res = 0, max = 0;
	for (int i = 0; i<CnPlayer(); i++)
		if (this->PlayerIsCard(i))
			if (max < PlayerStack(i))
			{
				res = max;
				max = PlayerStack(i);
			}
			else if (res < PlayerStack(i))
				res = PlayerStack(i);
	return res;
}
//---------------------------------------------------------------------------
int     clSitHoldem::EffStackAfterCall()
{
	int res = 0, max = 0;
	int maxBet = MaxBet();
	for (int i = 0; i<CnPlayer(); i++)
		if (this->PlayerIsCard(i))
		{
			int st = PlayerMoney(i) + PlayerBank(i) - maxBet;
			if (st < 0) st = 0;
			if (max < st)
			{
				res = max;
				max = st;
			}
			else if (res < st)
				res = st;
		}
	return res;
}
//---------------------------------------------------------------------------
int     clSitHoldem::EffStackAfterCall(int nbPl)
{
	if (!this->PlayerIsCard(nbPl)) return 0;
	int maxBet = MaxBet();
	int bet = maxBet - PlayerBank(nbPl);
	int res = PlayerMoney(nbPl)-bet, max = 0;
	if (res <= 0) return 0;
	for (int i = 0; i<CnPlayer(); i++)
		if (i != nbPl)
			if (this->PlayerIsCard(i))
			{
				int st = PlayerMoney(i) + PlayerBank(i) - maxBet;
				if (st < 0) st = 0;
				if (max < st)
					max = st;
		}
	return (res>max) ? max : res;
}
//---------------------------------------------------------------------------
int     clSitHoldem::TotalPotForIndicator()
{
    int pot=TotalPot();
    int bankH=PlayerBank(Indicator());
    for(int i=0;i<CN_PLAYER;i++)
        if(this->PlayerIsCard(i))
            if(PlayerBank(i)>bankH)
                pot-=(PlayerBank(i)-bankH);
    return pot;
}
//--------------------------------------------------------------------------
int     clSitHoldem::MaxBetForIndicator()
{
    int bet=MaxBet();
    return bet-PlayerBank(Indicator());
}
//--------------------------------------------------------------------------
enTradeAction   clSitHoldem::TradeActionPlayer(int nbPl,enNameTrade t)
{
    int nbA=0;
    switch(t)
    {
        case TRADE_FLOP : nbA=this->NbFirstAct(ACT_DELFLOP);break;
        case TRADE_TURN : nbA=this->NbFirstAct(ACT_DELTURN);break;
        case TRADE_RIVER: nbA=this->NbFirstAct(ACT_DELRIVER);break;
    }
    nbA=this->NbTradeActPl(nbPl,nbA);
    if(nbA==-1) return TA_UNDEF;
    enAction act0=GetAct(nbA)->_dis._act;
    nbA=this->NbTradeActPl(nbPl,nbA);
    enAction act1=(nbA==-1)? ACT_UNDEF : GetAct(nbA)->_dis._act;
    enTradeAction ta=WhatTA(act0,act1);
    if(t==TRADE_PREFLOP && ta==TA_CALL)
        if(MaxBet()<=BigBlind()) ta=TA_LIMP;
    return ta;
}
//--------------------------------------------------------------------------
bool    clSitHoldem::BoardInPosition()
{
    int posI=Indicator();
    for(int i=0;i<CnPlayer();i++)
        if(i !=Indicator()  && PlayerIsCard(i))
            if(!PosInPosition(posI,i))
                return false;
    return true;
}
//--------------------------------------------------------------------------
bool    clSitHoldem::IsPlayerInAllIn()
{
    for(int i=0;i<CnPlayer();i++)
        if(i !=Indicator()  && PlayerIsCard(i))
            if(this->PlayerMoney(i)<BigBlind())
                return true;
    return false;
}
//---------------------------------------------------------------------------
int		clSitHoldem::PotLimitBank()
{
	if (Indicator() == -1) return 0;
	int pot = TotalPot();
	int maxBet = 0;
	for (int i = 0; i<CN_PLAYER; i++)
		if (this->IsPlayer(i))
		{
			if (PlayerBank(i) > maxBet)
				maxBet = PlayerBank(i);
			if (i != Indicator())
				pot += PlayerBank(i);

		}
	pot += maxBet;
	return pot;
}
//---------------------------------------------------------------------------
void	clSitHoldem::Init(clStacks st)
{
	Clear();
	_atrHH._cnPl = st.CnPlayer();
	_atrHH._bb = st._bb;
	_atrHH._ante = st._ante;
	for (int i = 0; i<CnPlayer(); i++)
	{
		clAnsiString as = (clAnsiString)"Player" + (clAnsiString)i;
		strcpy(_seat[i]._name, as.c_str());
		_seat[i]._money = st[i];
		_seat[i]._inPlay = true;
		_seat[i]._busy = true;
	}
	int bet, nbSB = 1, nbBB = 0;
	if (_atrHH._ante>0)
		for (int i = 0; i<CnPlayer(); i++)
		{
			bet = (st[i]>_atrHH._ante) ? _atrHH._ante : st[i];
			AddAct(i, ACT_POSTANTE, bet);
			st[i] -= bet;
		}
	bet = (st[nbSB]>_atrHH._bb / 2) ? _atrHH._bb / 2 : st[nbSB];
	AddAct(nbSB, ACT_POSTSB, bet);
	st[nbSB] -= bet;
	bet = (st[nbBB]>_atrHH._bb) ? _atrHH._bb : st[nbBB];
	AddAct(nbBB, ACT_POSTBB, bet);
	st[nbBB] -= bet;
	AddAct(NB_DILER, ACT_DELCARD, 0);
	ExecHist(_arrAct._cnAct);
}
//---------------------------------------------------------------------------
clDoubleCP	clSitHoldem::RestraightResPosToNb(clDoubleCP val)
{
	clDoubleCP res; res = 0;
	for (int i = 0; i < CnPlayer(); i++)
		res[PositionToNbTable(i)] = val[i];
	return res;
}
//---------------------------------------------------------------------------
double	clSitHoldem::ChangeBB(int newBB)
{
	if (_atrHH._bb == 0)
	{
		return 1;
	}
	double k = 100. / _atrHH._bb;
	_atrHH._bb = _atrHH._bb*k + 0.5;
	_atrHH._ante = _atrHH._ante*k + 0.5;
	for (int i = 0; i < CnAct(); i++)
		GetAct(i)->_dis._money = GetAct(i)->_dis._money*k + 0.5;
	for (int i = 0; i < CnPlayer(); i++)
	{
		_seat[i]._money = _seat[i]._money*k + 0.5;
		_pl[i]._totalBetPl = _pl[i]._totalBetPl*k + 0.5;
		_pl[i]._bankPl = _pl[i]._bankPl*k + 0.5;
		_pl[i]._money = _pl[i]._money*k + 0.5;
	}
	_totalPot = _totalPot*k + 0.5;
	return k;
}
//---------------------------------------------------------------------------
clIntCP clSitHoldem::CalcShowDown()
{
	clIntCP money; money = 0;
	if (NbFirstAct(ACT_DELRIVER) == -1)
		return money;
//	for (int i = 0; i < CN_PLAYER; i++)
//		money[i] = (i<CnPlayer()) ? PlMoney(i) : 0;
	clGamePoker gamePoker[CN_PLAYER];
	clGamePoker minGame = GamePokerMinGame();
	for (int i = 0; i<CN_PLAYER; i++)
	{
		if (PlayerIsCard(i))
			gamePoker[i] = GamePlayerHoldem(i);
		else
			gamePoker[i] = minGame;
	}
	DistribPot(gamePoker);
	for (int i = 0; i < CnPlayer(); i++)
		money[i] = PlayerMoney(i) - PlMoney(i);
	return money;

}
//---------------------------------------------------------------------------
/*clGamePoker PokerWin(clGamePoker *gamePoker)
{
	clGamePoker game = POKER_MIN_GAME;
	for (int i = 0; i<CN_PLAYER; i++)
	{
		if (game<gamePoker[i])
			game = gamePoker[i];
	}
	return(game);

}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clSitHoldem::DistribPot(clGamePoker *gamePoker)
{
	CloseBank();
	bool win[CN_PLAYER];
	int cnC = CNPlayerIsCard();
	int totalBetPl[CN_PLAYER];
	//    int rake=0;
	for (int i = 0; i<CN_PLAYER; i++)
	{
		win[i] = false;
		totalBetPl[i] = TotalBetPl(i);
	}
	clGamePoker minGame = GamePokerMinGame();
	while (cnC>0)
	{
		int n = 0, bet = -1;
		clGamePoker comb= POKER_MIN_GAME;
		for (int i = 0; i<CN_PLAYER; i++)
		{
			if (PlayerIsCard(i) && comb<gamePoker[i])
				comb = gamePoker[i];
		}
		for (int i = 0; i<CN_PLAYER; i++)
		{
			if (PlayerIsCard(i) && comb == gamePoker[i])
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
				ExecAct(HistAction(i, ACT_WIN, totalB / n + ost));
				ost = 0;
			}
		for (int i = 0; i<CN_PLAYER; i++)
		{
			if (totalBetPl[i] == 0)
			{
				if (PlayerIsCard(i) && !(gamePoker[i] == minGame))
				{
					win[i] = false;
					cnC--;
					gamePoker[i] = minGame;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool    SitHoldemStacksForCallTogether(clStacks st)
{
    if(st.CnPlayer() != 3) return false;
    int min=st[0];
    for(int i=1;i<3;i++)
        if(st[i]<min)
            min=st[i];
    return min<st._bb+st._bb/5;
}
//---------------------------------------------------------------------------
bool    SitHoldemStacksForRaiseTogether(clStacks st)
{
    if(st.CnPlayer() != 3) return false;
    int min=st[0];
    for(int i=1;i<3;i++)
        if(st[i]<min)
            min=st[i];
    int ante=st._bb/5;
    if(min<2*st._bb+ante && min>st._bb+ante) return true;
    if(st._bb==50 && (st[0]<3*st._bb+ante && st[0]>st._bb+ante)) return true;
    return false;
}
//---------------------------------------------------------------------------
bool    SitIsSitForCallTogether(clSitHoldem *sit)
{
    if(sit->CnPlayer() != 3) return false;
    if(sit->CNPlayerIsCard() != 3) return false;
//    if(sit->RoundTrade()> 1) return false;
    int minPos=POS_BB;
    for(int i=POS_SB;i<=POS_BUTTON;i++)
		if (sit->StackPlayerInBB100(i)<sit->StackPlayerInBB100(minPos))
            minPos=i;
	if (sit->StackPlayerInBB100(minPos)>120) return false;
    int posHero=sit->Indicator();
    if(minPos==posHero) return false;
    if(sit->MaxBet()>sit->BigBlind()) return false;
/*    switch(posHero)
    {
        case POS_BUTTON : return true;
        case POS_SB     :
        {
            int nbA=sit->NbFirstTradeAct(0);
            return (nbA == -1 || sit->GetAct(nbA)->_dis._act == ACT_CALL);
        }
        default     :
        {
            int nbA=sit->NbFirstTradeAct(0);
            if(sit->GetAct(nbA)->_dis._act != ACT_CALL) return false;
            nbA=sit->NbFirstTradeAct(nbA);
            if(nbA != -1 && sit->GetAct(nbA)->_dis._act != ACT_CALL) return false;
            return true;
        }
    }
*/
    return true;
}
//---------------------------------------------------------------------------
bool    SitIsSitForRaiseTogether(clSitHoldem *sit)
{
    if(sit->CnPlayer() != 3) return false;
    if(sit->CNPlayerIsCard() != 3) return false;
    if(sit->BigBlind()>=400) return false;
    int minPos=POS_BB;
    for(int i=POS_SB;i<=POS_BUTTON;i++)
        if(sit->PlMoney(i)<sit->PlMoney(minPos))
            minPos=i;
    int posHero=sit->Indicator();
    if(minPos==posHero) return false;
	int minS = sit->StackPlayerInBB100(minPos);
    bool rt=(minS<=220 && minS>120);
    if(!rt)
        rt=minPos==POS_BB && sit->BigBlind()==50 && minS<=320 && minS>120;
    if(!rt) return false;
    int bet=(minS<220)? 2*sit->BigBlind() : 3*sit->BigBlind();
    if(sit->MaxBet()>bet) return false;
	if (sit->StackIndicatorInBB100()<420) return false;
    return true;
}
//---------------------------------------------------------------------------
bool    SitDisIsRaiseTogether(clSitHoldem *sit,tpDis dis)
{
    int bankPl=sit->PlayerBank(sit->Indicator())+dis._money;
    return bankPl<=2*sit->BigBlind() || (sit->BigBlind()==50 && bankPl<3*sit->BigBlind());
}
//---------------------------------------------------------------------------
void    SHBuildHistToStacks(clGameHistory *hist,clStacks st)
{
    hist->WrNbGame(0);
    hist->WrBigBlinds(st._bb);
    for(int i=0;i<CN_PLAYER;i++)
        if(st[i]>0)
        {
//			AnsiString as = (AnsiString)"player" + i;
			char strN[32] = { 'p', 'l', 'a', 'y', 'e', 'r' };
			_itoa_s(i, strN + 6,32 - 6, 10);
            hist->WrSeatInfo(i,strN,st[i],true);
            hist->_atrHH._cnPl++;
        }
    hist->_atrHH._bb=st._bb;
    hist->_atrHH._ante=st._bb/5;
    int ante=hist->_atrHH._ante;
    for(int i=0;i<CN_PLAYER;i++)
        if(hist->IsPlayer(i))
            hist->AddAct(i,ACT_POSTANTE,(st[i]>ante)? ante : st[i]);
    int money=st[POS_SB]-ante;
    if(money>0)
        hist->AddAct(POS_SB,ACT_POSTSB,(money>st._bb/2)? st._bb/2 : money);
    money=st[POS_BB]-ante;
    if(money>0)
        hist->AddAct(POS_BB,ACT_POSTBB,(money>st._bb)? st._bb : money);
    hist->AddAct(NB_DILER,ACT_DELCARD,0);
}
//---------------------------------------------------------------------------
enTradeAction WhatTA(enAction act0,enAction act1)
{
    if(act1==ACT_UNDEF)
        switch(act0)
        {
            case ACT_CHECK : return TA_CHECK;
            case ACT_CALL  : return TA_CALL;
            case ACT_BET   : return TA_BET;
            case ACT_RAISE_TO :
            case ACT_RAISE : return TA_RAISE;
            default : return TA_UNDEF;
        }
    switch(act0)
    {
        case ACT_CHECK :
            switch(act1)
            {
                case ACT_FOLD  : return TA_CHECK_FOLD;
                case ACT_CALL  : return TA_CHECK_CALL;
                case ACT_RAISE_TO :
                case ACT_RAISE : return TA_CHECK_RAISE;
                case ACT_ALL_IN : return TA_CHECK_ALL_IN;
            }
            return TA_UNDEF;
        case ACT_BET   :
            switch(act1)
            {
                case ACT_FOLD  : return TA_BET_FOLD;
                case ACT_CALL  : return TA_BET_CALL;
                case ACT_RAISE_TO :
                case ACT_RAISE : return TA_BET_RAISE;
                case ACT_ALL_IN : return TA_BET_ALL_IN;
            }
            return TA_UNDEF;
        case ACT_RAISE_TO :
        case ACT_RAISE :
            switch(act1)
            {
                case ACT_FOLD  : return TA_RAISE_FOLD;
                case ACT_CALL  : return TA_RAISE_CALL;
                case ACT_RAISE_TO :
                case ACT_RAISE : return TA_RAISE_RAISE;
                case ACT_ALL_IN : return TA_RAISE_ALL_IN;
            }
            return TA_UNDEF;
        default : return TA_UNDEF;
    }
}
//---------------------------------------------------------------------------


