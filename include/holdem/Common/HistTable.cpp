//---------------------------------------------------------------------------
#pragma hdrstop

#include "C:\home\holdem\Common\HistTable.h"

//inline  enPosition HistTablePositionForPlayer(clGameHistory *hist,int n)     {return (enPosition)(n+6-hist->CnPlayerInPlay()) ;}

//---------------------------------------------------------------------------
void    clHistTable::ClearTable()
{
    clGameHistory::Clear();
    for(int i=0;i<CN_PLAYER;i++)
        _pl[i].Clear();
}
//---------------------------------------------------------------------------
void    clHistTable::operator = (clHistTable &table)
{
    clGameHistory *ptrH=&table;
    *((clGameHistory*)this)=*ptrH;
    char *ptrThis=(char *)this,*ptr=(char *)&table;
    memcpy(ptrThis+sizeof(clGameHistory),ptr+sizeof(clGameHistory),sizeof(clHistTable)-sizeof(clGameHistory));
}
//---------------------------------------------------------------------------
void    clHistTable::ExecHist(clGameHistory *hist,int nbAct)
{
    *((clGameHistory*)this)=*hist;
    ExecHist(nbAct);
}
//---------------------------------------------------------------------------
void    clHistTable::ExecHist(int nbAct)
{
    _nbAct=nbAct;
    for(int i=0;i<CN_PLAYER;i++)
    {
        _pl[i]._totalBetPl=_pl[i]._bankPl=0;
        _pl[i]._isCard=_pl[i]._viewCards=false;
        _pl[i]._money=PlMoney(i);
    }
    _indicator=-1;
    _totalPot=_cnCardTable=0;
    _posHero=ViewPlayer();
//    _cnPl=_atrHH._cnPl;
//    _nbSB=_nbBB=INT_UNDEF;
    for(int i=0;i<nbAct;i++)
        ExecAct(i);
}
//---------------------------------------------------------------------------
void    clHistTable::CloseBank()
{
    for(int i=0;i<CN_PLAYER;i++)
    {
        _totalPot+=_pl[i]._bankPl;
        _pl[i]._totalBetPl+=_pl[i]._bankPl;
        _pl[i]._bankPl=0;
    }
}
//---------------------------------------------------------------------------
void    clHistTable::CheckIndicator()
{
    if(_indicator==-1)
    {
        _begTrade=-1;
        if (_cnCardTable==5)
            CloseBank();
    }
}
//---------------------------------------------------------------------------
void    clHistTable::ExecAct(int nb)
{
    tpAction *act=GetAct(nb);
    switch(act->_dis._act)
    {
//        case ACT_SIT_OUT :  _seat[ptrAct->_nbSeat]._outPlay=true;break;
//        case ACT_RETURNED : _seat[ptrAct->_nbSeat]._outPlay=false;break;
//        case ACT_UNDEF  :   break;
        case ACT_POSTANTE  :
            _pl[act->_pos]._bankPl+=act->_dis._money;
            _pl[act->_pos]._money-=act->_dis._money;
            break;
        case ACT_CHECK  :
            _indicator=NextPlayerMove();
            CheckIndicator();
            break;
        case ACT_FOLD   :   _pl[act->_pos]._isCard=false;
//            if(_atrHH._site==SITE_POKERSTARS)
//                if(_pl[act->_nbSeat]._money==0)
//                {
//                    _cnPlIsCard--;
//                    break;
//                }
            _cnPlInTrade--;
            _cnPlIsCard--;
            if(_begTrade==_indicator)
                _begTrade=_indicator=NextPlayerMove();
            else
                _indicator=NextPlayerMove();
            CheckIndicator();
            break;
       case ACT_CALL   :
            _pl[act->_nbSeat]._bankPl+=act->_dis._money;
            _pl[act->_nbSeat]._money-=act->_dis._money;
            if(_pl[_indicator]._money==0)
                _cnPlInTrade--;
            if(_begTrade==_indicator && _pl[_indicator]._money==0)
                _begTrade=_indicator=NextPlayerMove();
            else
                _indicator=NextPlayerMove();
            CheckIndicator();
            break;
        case ACT_BET    :
            _pl[act->_nbSeat]._bankPl+=act->_dis._money;
            _pl[act->_nbSeat]._money-=act->_dis._money;
            if(_pl[_indicator]._money==0)
                _cnPlInTrade--;
            _begTrade=act->_nbSeat;
            _indicator=NextPlayerMove();
            break;
        case ACT_RAISE_TO  :
            act->_dis._money=(act->_dis._money-_pl[act->_nbSeat]._bankPl);
            act->_dis._act=ACT_RAISE;
        case ACT_RAISE  :
            _pl[act->_nbSeat]._bankPl+=act->_dis._money;
            _pl[act->_nbSeat]._money-=act->_dis._money;
            ActRaiseChangeIndicator();
            break;
        case ACT_ALL_IN  :
            _pl[act->_nbSeat]._bankPl+=act->_dis._money;
            _pl[act->_nbSeat]._money-=act->_dis._money;
            _cnPlInTrade--;
            if(_pl[_indicator]._bankPl<=AnotherMaxBet(_indicator))
            {
                if(_begTrade==_indicator)
                {
                    _begTrade=-1;
                    _begTrade=_indicator=NextPlayerMove();
                }
                else
                    _indicator=NextPlayerMove();
                CheckIndicator();
            }
            else
            {
                _begTrade=-1;
                _begTrade=_indicator=NextPlayerMove();
            }
            break;
        case ACT_POSTSB :
            if(_pl[POS_SB]._bankPl>0)
                CloseBank();
            _pl[POS_SB]._bankPl+=act->_dis._money;
            _pl[POS_SB]._money-=act->_dis._money;
            break;
        case ACT_POSTBB :
            if(_pl[POS_BB]._bankPl>0)
                CloseBank();
            _pl[POS_BB]._bankPl+=act->_dis._money;
            _pl[POS_BB]._money-=act->_dis._money;
            break;
#ifndef CALC_FOR_TOURN
        case ACT_POST_DEAD :
            if(act->_dis._money>BigBlind())
            {
                _totalPot=act->_dis._money-BigBlind();
                _pl[act->_nbSeat]._bankPl+=BigBlind();
            }
            else
                _pl[act->_nbSeat]._bankPl+=act->_dis._money;
            _pl[act->_nbSeat]._money-=act->_dis._money;
            break;
#endif
        case ACT_RETURN_UNCALLED_BET    :
        {
            if(act->_dis._money>0)
            {
                _totalPot-=act->_dis._money;
                _pl[act->_nbSeat]._totalBetPl-=act->_dis._money;
                _pl[act->_nbSeat]._money+=act->_dis._money;
            }
            break;
        }
        case ACT_WIN    :
            if(_totalPot!=0)
            {
                _totalPot=0;
                for(int i=0;i<MaxCnSeat();i++)
                    _pl[i]._bankPl=0;
            }
            if(act->_dis._money>0)
            {
                _pl[act->_nbSeat]._bankPl=act->_dis._money;
                _pl[act->_nbSeat]._money+=_pl[act->_nbSeat]._bankPl;
            }
            break;
        case ACT_SHOW   :
            _pl[act->_nbSeat]._viewCards=true;
            break;
        case ACT_DELCARD:
            for(int i=0;i<CN_PLAYER;i++)
                if(IsPlayer(i))
                    _pl[i]._isCard=true;
            _trade=TRADE_PREFLOP;
            _begTrade=-1;
            _cnPlInTrade=0;
            _cnPlIsCard=0;
            for (int i=0;i<MaxCnSeat();i++)
                if (_pl[i]._isCard)
                {
                    _cnPlIsCard++;
                    if(_pl[i]._money>0)
                        _cnPlInTrade++;
                    else if(!PlayerInPlay(i))//_atrHH._site==SITE_POKERSTARS &&
                        _cnPlInTrade++;
                }
            _begTrade=_indicator=NextPlayerMove(POS_BB);
            break;
        case ACT_DELFLOP:
            _cnCardTable=3;
            CloseBank();
            _trade=TRADE_FLOP;
            InitIndicatorOnFlop();
            break;
        case ACT_DELTURN:
            _cnCardTable=4;
            CloseBank();
            _trade=TRADE_TURN;
            InitIndicatorOnFlop();
            break;
        case ACT_DELRIVER:
            _cnCardTable=5;
            CloseBank();
            _trade=TRADE_RIVER;
            InitIndicatorOnFlop();
            break;
    }
}
//---------------------------------------------------------------------------
inline  void    clHistTable::InitIndicatorOnFlop()
{
    if (_cnPlInTrade<=1)
        _begTrade=_indicator=-1;
    else
    {
        _begTrade=-1;
        if(this->CnPlayer()==2)
            _begTrade=_indicator=POS_BB;
        else
            _begTrade=_indicator=NextPlayerMove(POS_BUTTON);
    }
}
//---------------------------------------------------------------------------
/*enPosition  clHistTable::Position(int nb)
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
int clHistTable::NbPosition(enPosition pos)
{
    int n=pos;
    n+=_hist.CnPlayerIsPlay()-6;
    int nb0=_hist.NextPlayerInPlay(NbBigBlind());
    for(int i=0;i<n;i++)
        nb0=_hist.NextPlayerInPlay(nb0);
    return nb0;
}
//---------------------------------------------------------------------------
bool    clHistTable::ClearWater()
{
    if(NameTrade()==TRADE_PREFLOP)
        for(int i=_hist.NbFirstAct(ACT_DELCARD)+1;i<_nbAct;i++)
            if(HistIsActiveTradeAct(_hist._arrAct[i]._dis._act))
                return false;
    return true;
}*/
//---------------------------------------------------------------------------
void    clHistTable::ActRaiseChangeIndicator()
{
    if(_pl[_indicator]._money==0)
    {
         _cnPlInTrade--;
         _begTrade=-1;
         _begTrade=_indicator=NextPlayerMove();
    }
    else
    {
        _begTrade=_indicator;
        _indicator=NextPlayerMove();
    }
}
//---------------------------------------------------------------------------
int  clHistTable::NextPlayerMove(int nb)
{
    if(_cnPlInTrade==0) return -1;
    int nb0=-1;
    for(int i=0;i<CN_PLAYER-1;i++)
    {
        if(--nb<0)
            nb=POS_EARLY;
        if (_pl[nb]._isCard)
            if(_pl[nb]._money>0)// && GetSeat(nb)->_inPlay)
            {
                nb0=nb;break;
            }
            else if(!GetSeat(nb)->_inPlay)//_atrHH._site==SITE_POKERSTARS &&
            {
                nb0=nb;break;
            }
    }
    int res= (nb0==_begTrade)? -1 : nb0;
    if (_cnPlInTrade==1 && res!=-1)
        if(MaxBet()<=_pl[res]._bankPl)
            res=-1;
    return res;
}
//---------------------------------------------------------------------------
int     clHistTable::AnotherMaxBet(int nbPl)
{
    int max=0;
    for(int i=0;i<MaxCnSeat();i++)
        if(_pl[i]._isCard && _pl[i]._bankPl>max && i!=nbPl)
            max=_pl[i]._bankPl;
    return max;
}
//---------------------------------------------------------------------------
int     clHistTable::MaxBet()
{
    int max=0;
    for(int i=0;i<MaxCnSeat();i++)
        if(_pl[i]._isCard && _pl[i]._bankPl>max)
            max=_pl[i]._bankPl;
    return max;
}
//---------------------------------------------------------------------------
void    clHistTable::ExecAct(tpAction act)
{
    _arrAct._cnAct=_nbAct;
    AddAct(&act);
    ExecAct(_nbAct++);
}
//---------------------------------------------------------------------------
clGamePoker     clHistTable::GamePlayerHoldem(int nb)
{
    clGamePoker game;
    if(_cnCardTable == 5)
        game.GameHoldemQ(PlayerCards(nb),CardTable());
    else
        game.game=GM_UNDEF;
    return game;
}
//---------------------------------------------------------------------------
tpAction clHistTable::GoPrevNextTradeAct()
{
    if(_nbAct<CnAct())
        ExecAct(_nbAct++);
    tpAction *act;
    while(_nbAct<CnAct())
    {
        act=GetAct(_nbAct);
        if(HistIsTradeAct(act->_dis._act))
            break;
        ExecAct(_nbAct++);
    }
    return *act;
}
//---------------------------------------------------------------------------
int     clHistTable::CNPlayerInPlayWithMoney()
{
    int cn=0;
    for(int i=0;i<CN_PLAYER;i++)
        if(_seat[i]._busy && _pl[i]._money>0)
            cn++;
    return(cn);
}
//---------------------------------------------------------------------------
/*void    clHistTable::StartGame()
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
/*tpDis   clHistTable::CreateDisCallCheck()
{
    int bet=MaxBet()-_pl[_indicator]._bankPl;
    return (bet==0)? HistCreateDis(ACT_CHECK,0) : HistCreateDis(ACT_CALL,bet);
}*/
//---------------------------------------------------------------------------
bool    clHistTable::TableIsAllIn()
{
    int cn=0,cnC=0;
    if(_indicator != -1) return false;
    for(int i=0;i<this->MaxCnSeat();i++)
        if(this->PlayerIsCard(i))
        {
            cnC++;
            if(this->PlayerMoney(i)>0)
                cn++;
            if(cn>=2)
                return false;
        }
    return (cnC>=2);
}
//---------------------------------------------------------------------------
bool    clHistTable::MayBeCheck()
{
    return (_pl[_indicator]._bankPl==MaxBet());
}
//---------------------------------------------------------------------------
int clHistTable::NextPlayerWithCard(int nbPl)
{
    for(int i=1;i<CN_PLAYER;i++)
    {
        nbPl++;
        if(nbPl>=CN_PLAYER) nbPl=0;
        if(_pl[nbPl]._isCard) return nbPl;
    }
    return POS_UNDEF;
}
//---------------------------------------------------------------------------
int     clHistTable::CNPlayerIsCard(int *pos)
{
    int cn=0;
    for(int i=0;i<CN_PLAYER;i++)
        if(_pl[i]._isCard)
            pos[cn++]=i;
    return cn;
}
//---------------------------------------------------------------------------
bool    clHistTable::WrViewPlayer(int posH)
{
    int nbAct=this->NbFirstAct(ACT_DELCARD);
    if(nbAct==-1) return false;
    nbAct++;
    bool fnd;
    if(nbAct==this->CnAct()) fnd=false;
    else fnd=(GetAct(nbAct)->_dis._act == ACT_SHOW);
    if(fnd)
        GetAct(nbAct)->_pos=posH;
    else
    {
        InsertAct(nbAct,HistAction(posH,ACT_SHOW,0));
        _nbAct++;
        _pl[posH]._viewCards=true;
    }
    _posHero=posH;
} 
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*enPosition  HistTableStringToPos(char *str)
{
    if(strcmp(str,"Early")==0)
        return POS_EARLY;
    else if(strcmp(str,"Middle")==0)
        return POS_MIDDLE;
    else if(strcmp(str,"Cuttof")==0)
        return POS_CUTTOF;
    else if(strcmp(str,"Button")==0)
        return POS_BUTTON;
    else if(strcmp(str,"SB")==0)
        return POS_SB;
    else if(strcmp(str,"BB")==0)
        return POS_BB;
    else
        return POS_UNDEF;
}
//---------------------------------------------------------------------------
*/
/*AnsiString  HistTablePosToString(int pos)
{
    switch(pos)
    {
        case POS_EARLY  : return "Early";
        case POS_MIDDLE : return "Middle";
        case POS_CUTOFF : return "Cuttof";
        case POS_BUTTON : return "Button";
        case POS_SB     : return "SB";
        case POS_BB     : return "BB";
    }
    return "Undef";
}*/
//---------------------------------------------------------------------------
bool    HistTableCheckHH(clGameHistory *hist)
{
    clSitHoldem table;
    table.ExecHist(hist,0);
    for(int i=0;i<hist->CnAct();i++)
    {
        tpAction *act=hist->GetAct(i);
        if(act->_dis._act==ACT_DELFLOP)
            break;
        if(HistIsTradeAct(act->_dis._act))
            if(act->_pos != table.Indicator())
                return false;
        table.ExecNextAct();
    }
    return true;
}
//---------------------------------------------------------------------------
/*enPosition  HistTablePosition(clGameHistory *hist,int nb)
{
    if(!hist->IsPlayer(nb)) return POS_UNDEF;
    int nb0=hist->NbBigBlind();
    enPosition pos=POS_BB;
    for(;pos>POS_EARLY;pos=(enPosition)((int)pos-1))
    {
        if(nb==nb0)
            break;
        nb0=hist->PrevPlayer(nb0);
    }
    return pos;
} */
//---------------------------------------------------------------------------

#pragma package(smart_init)
