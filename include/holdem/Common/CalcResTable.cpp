//---------------------------------------------------------------------------
#pragma hdrstop
#include <math.h>

#include "C:\home\holdem\TreeHoldem\MFCApplication1\GlobalsForCalc.h"
#include "C:\home\holdem\Common\clHandsCompare.h"
#include "C:\home\holdem\Common\CalcResTable.h"

////---------------------------------------------------------------------------
//void    clCalcResTable::operator = (clSitHoldem &sit)
//{
//    _cnPl=sit.CnPlayer();
//    _bb=sit.BigBlind();
//    for(int i=0;i<CN_PLAYER;i++)
//    {
//        tpPlAtr *atr=sit.PlayerAtr(i);
//        _isCard[i]=atr->_isCard;
//        _money[i]=atr->_money;
//        _bet[i]=atr->_totalBetPl+atr->_bankPl;
//    }
//}
////---------------------------------------------------------------------------
//void    clCalcResTable::StartTable()
//{
//    for(int i=0;i<CN_PLAYER;i++)
//    {
//        _money[i]+=_bet[i];
//        _bet[i]=0;
//    }
//}
////---------------------------------------------------------------------------
//void    clCalcResTable::EndGame()
//{
//    int bank=0,nbPl=-1;
//    for(int i=0;i<CN_PLAYER;i++)
//    {
//        if(_isCard[i])
//            nbPl=i;
//        bank+=_bet[i];
//        _bet[i]=0;
//    }
//    _money[nbPl]+=bank;
//}
////---------------------------------------------------------------------------
//void    clCalcResTable::WinPlayer(int nbPl)
//{
//    int bank=0,bet=_bet[nbPl];
//    for(int i=0;i<CN_PLAYER;i++)
//        if(bet>_bet[i])
//        {
//            bank+=_bet[i];
//            _bet[i]=0;
//        }
//        else
//        {
//            bank+=bet;
//            _bet[i]-=bet;
//        }
//    _money[nbPl]+=bank;
//    for(int i=0;i<CN_PLAYER;i++)
//        if(_bet[i]==0)
//            _isCard[i]=false;
//}
////---------------------------------------------------------------------------
//void    clCalcResTable::Win2Player(int nbPl1,int nbPl2)
//{
//    int bank=0,bet=(_bet[nbPl1]>_bet[nbPl2])? _bet[nbPl2] : _bet[nbPl1];
//    for(int i=0;i<CN_PLAYER;i++)
//        if(bet>_bet[i])
//        {
//            bank+=_bet[i];
//            _bet[i]=0;
//        }
//        else
//        {
//            bank+=bet;
//            _bet[i]-=bet;
//        }
//    _money[nbPl1]+=bank/2;
//    _money[nbPl2]+=bank/2;
//    int ost=bank%2;
//    if(ost>0)
//        if(HistInPosition(nbPl1,nbPl2)) _money[nbPl2]+=ost;
//        else _money[nbPl1]+=ost;
//    WinPlayer((_bet[nbPl1]>_bet[nbPl2])? nbPl1 : nbPl2);
//}
////---------------------------------------------------------------------------
//clDoubleCP clCalcResTable::CalcMoney(clCalcResTable *StartTable,clCostStack *cs)
//{
//    clDoubleCP res;
//    for(int i=0;i<CN_PLAYER;i++)
//        res[i]=(i<_cnPl)? cs->CostStack(_cnPl,i,_money,StartTable->_money,_bb) : 0;
//    return res;
//}
////---------------------------------------------------------------------------
//int clCalcResTable::CnPlayersIsCard()
//{
//    int cn=0;
//    for(int i=0;i<CN_PLAYER;i++)
//        if(_isCard[i])
//            cn++;
//    return cn;
//}
////---------------------------------------------------------------------------
//clStacks    clCalcResTable::GetStacks()
//{
//    clStacks st;
//    for(int i=0;i<CN_PLAYER;i++)
//        st[i]=(i<_cnPl)? PlMoney(i) : 0;
//    st._bb=_bb;
//    return st;
//}
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//clDoubleCP CalcResFor2Player(clImproveGroupHands **arrGH,clCalcResTable *table,clCalcResTable *StartTable,clCostStack *cs)
//{
//    int nbPl[CN_PLAYER],cur=0;
//    for(int i=0;i<CN_PLAYER;i++)
//        if(table->_isCard[i])
//            nbPl[cur++]=i;
////    if(nbPl[0]==0 && nbPl[1]==POS_EARLY)
////        int a=0;
//    clDoubleCP res;
//    tpWin2 w2=CCComparePlayers(*arrGH[nbPl[0]],*arrGH[nbPl[1]]);
//    clCalcResTable t0=*table;
//    t0.WinPlayer(nbPl[0]);
//    t0.EndGame();
//    res=t0.CalcMoney(StartTable,cs)*w2._win;
//    t0=*table;
//    t0.Win2Player(nbPl[0],nbPl[1]);
//    t0.EndGame();
//    res+=t0.CalcMoney(StartTable,cs)*w2._eq;
//    t0=*table;
//    t0.WinPlayer(nbPl[1]);
//    t0.EndGame();
//    res+=t0.CalcMoney(StartTable,cs)*w2._loss;
//    return res;
//}
////---------------------------------------------------------------------------
//clDoubleCP CalcResFor3Player(clImproveGroupHands **arrGH,clCalcResTable *table,clCalcResTable *StartTable,clCostStack *cs)
//{
//    int nbPl[CN_PLAYER],cur=0;
//    for(int i=0;i<CN_PLAYER;i++)
//        if(table->_isCard[i])
//            nbPl[cur++]=i;
//    clDoubleCP res,res0;
//    res=0;
//    stVerManyHands w3=CCComparePlayers3Pl(*arrGH[nbPl[0]],*arrGH[nbPl[1]],*arrGH[nbPl[2]]);
//    for(int i=0;i<3;i++)
//    {
//        clCalcResTable t0=*table;
//        t0.WinPlayer(nbPl[i]);
//        double mult=w3.WinEQHand(i);
//        if(t0.CnPlayersIsCard()==2)
//            res0=CalcResFor2Player(arrGH,&t0,StartTable,cs)*mult;
//        else
//        {
//            t0.EndGame();
//            res0=t0.CalcMoney(StartTable,cs)*mult;
//        }
//        res+=res0;
//    }
//    return res;
//}
////---------------------------------------------------------------------------
//clDoubleCP CalcResForManyPlayer(clImproveGroupHands **arrGH,clCalcResTable *table,clCalcResTable *StartTable,clCostStack *cs)
//{
//    int nbPl[CN_PLAYER],cnPl=0;
//    for(int i=0;i<CN_PLAYER;i++)
//        if(table->_isCard[i])
//            nbPl[cnPl++]=i;
//    clDoubleCP res,res0,resW;
//    resW=0;
//    for(int i=0;i<CN_PLAYER;i++)
//        if(table->_isCard[i])
//            for(int j=i+1;j<CN_PLAYER;j++)
//                if(table->_isCard[j])
//                {
//                    double w2=CCComparePlayersEQ(*arrGH[i],*arrGH[j]);
//                    resW[i]+=w2;
//                    resW[j]+=1-w2;
//                }
//    resW.Norm();
//    res=0;
//    for(int i=0;i<cnPl;i++)
//    {
//        clCalcResTable t0=*table;
//        t0.WinPlayer(nbPl[i]);
//        int cnPl0=t0.CnPlayersIsCard();
//        if(cnPl0<=1)
//        {
//            t0.EndGame();
//            res0=t0.CalcMoney(StartTable,cs)*resW[nbPl[i]];
//        }
//        else if(cnPl0==2)
//            res0=CalcResFor2Player(arrGH,&t0,StartTable,cs)*resW[nbPl[i]];
//        else if(cnPl0==3)
//            res0=CalcResFor3Player(arrGH,&t0,StartTable,cs)*resW[nbPl[i]];
//        else
//            res0=CalcResForManyPlayer(arrGH,&t0,StartTable,cs)*resW[nbPl[i]];
//        res+=res0;
//    }
//    return res;
//}
////---------------------------------------------------------------------------
///*bool  TSCalcPreFlop(clTreeSit *tree)
//{
//    if(tree->_atr._sit.Indicator()!=POS_UNDEF) return false;
//    clSitHoldem *sit=&tree->_atr._sit;
//    int cnPl=sit->CNPlayerIsCard();
//    clCalcResTable tab0,tabStart;
//    tab0=*sit;
//    tabStart=tab0;
//    tabStart.StartTable();
//    clImproveGroupHands *ghIn[CN_PLAYER];
//    for(int i=0;i<cnPl;i++)
//        ghIn[i]=&tree->_atr._arrGH[i];
//    if(cnPl==1)
//    {
//        tab0.EndGame();
//        tree->_atr._res=tab0.CalcMoney(&tabStart);
//    }
//    else if(cnPl==2)
//        tree->_atr._res=CalcResFor2Player(ghIn,&tab0,&tabStart);
//    else if(cnPl==3)
//        tree->_atr._res=CalcResFor3Player(ghIn,&tab0,&tabStart);
//    else
//        tree->_atr._res=CalcResForManyPlayer(ghIn,&tab0,&tabStart);
//
//#ifdef  ERR_MESSAGE
//    double sum=tree->_atr._res.Sum();
//    if(fabs(sum-6)>DOUBLE_0)
//    {
//        ErrMessage("Сумма стеков не равна 6","TSCalcPreFlop");
//        TSCalcPreFlop(tree);
//        return false;
//    }
////    glTrasser.WriteMessage("Exit TSCalcPreFlop");
//#endif
//    return true;
//} */
////---------------------------------------------------------------------------
//clDoubleCP    TSCalcPreFlop(clSitHoldem *sit,clCostStack *cs)
//{
//    int cnPl=sit->CNPlayerIsCard();
//    clCalcResTable tab0,tabStart;
//    tab0=*sit;
//    tabStart=tab0;
//    tabStart.StartTable();
//    if(cnPl==1)
//    {
//        tab0.EndGame();
//        return tab0.CalcMoney(&tabStart,cs);
//    }
//
//    clImproveGroupHands *grH[CN_PLAYER];
//    for(int i=0;i<sit->CnPlayer();i++)
//    {
//        grH[i]=new clImproveGroupHands;
//        if(sit->PlayerIsCard(i))
//            *grH[i]=((clHand *)sit->PlayerCards(i))->NbHand();
//    }
//    clDoubleCP res;
//    if(cnPl==2)
//        res=CalcResFor2Player(grH,&tab0,&tabStart,cs);
//    else if(cnPl==3)
//        res=CalcResFor3Player(grH,&tab0,&tabStart,cs);
//    else
//        res=CalcResForManyPlayer(grH,&tab0,&tabStart,cs);
//    for(int i=0;i<sit->CnPlayer();i++)
//        delete grH[i];
//    return res;
//}
////---------------------------------------------------------------------------
///*int     TSNbBestBranch(clTreeSit *tree)
//{
//    if(tree->CnBranch()==0) return -1;
//    int pos=tree->_atr._sit.Indicator();
//    int nb=0;
//    for(int i=1;i<tree->CnBranch();i++)
//        if(tree->PtrTreeSit(i)->_atr._res[pos]>tree->PtrTreeSit(nb)->_atr._res[pos])
//            nb=i;
//    return nb;
//} */
////---------------------------------------------------------------------------
//double  TSCalcPreFlop(clCalcResTable tab0,clImproveGroupHands **grH,int pos,bool icm)
//{
//    int cnPl=tab0.CnPlayersIsCard();
//    clCalcResTable tabStart;
//    tabStart=tab0;
//    tabStart.StartTable();
//    clCostStack *cs;
//    if(icm)
//        cs=&glCostStackICM;
//    else
//        cs=&glCostStackTS;
//    if(cnPl==1)
//    {
//        tab0.EndGame();
//        return tab0.CalcMoney(&tabStart,cs)[pos];
//    }
//
//    if(cnPl==2)
//        return CalcResFor2Player(grH,&tab0,&tabStart,cs)[pos];
//    else if(cnPl==3)
//        return CalcResFor3Player(grH,&tab0,&tabStart,cs)[pos];
//    else
//        return CalcResForManyPlayer(grH,&tab0,&tabStart,cs)[pos];
//}
////---------------------------------------------------------------------------
//
//#pragma package(smart_init)
