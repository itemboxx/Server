//---------------------------------------------------------------------------
#pragma hdrstop

#include "c:\Work\MultiTrn\Calc\Common\TreeSit.h"
#include "c:\Work\Card\Common\clCol.h"
#include "c:\Work\MultiTrn\Calc\Common\sitHoldem.h"
#include "c:\Work\MultiTrn\Calc\Common\HeroDecide.h"
#include "c:\Work\MultiTrn\Analiz\Common\FinResTrn.h"
#include "c:\work\util\usefullProc.h"

#define CN_CALC_AUTOPL          10000

int     CheckAllInOutRiver(clGameHistoryTrn *hist,clSitHoldem *sit);
int     CheckAllInPreFlop(clGameHistory *hist);
//---------------------------------------------------------------------------
double    FinResGame(clCostStack *costStack,clGameHistoryTrn *hist,int nb)
{
    if(!hist->GetSeat(nb)->_busy)
        return(0);
    return costStack->CostStackEndGame(hist,nb)-costStack->CostStackBeginGame(hist,nb);
}
//---------------------------------------------------------------------------
double    FinResGameEV(clCostStack *costStack,clGameHistoryTrn *hist,int nb,double &disp)
{
    if(!hist->GetSeat(nb)->_busy)
        return(0);
    double mon=0,monS=costStack->CostStackBeginGame(hist,nb);
    clSitHoldem sit,sit0;
    int nbAct=CheckAllInOutRiver(hist,&sit);
    if(nbAct==-1)
    {
        disp=0;
        return(costStack->CostStackEndGame(hist,nb)-monS);
    }
    if(costStack->GetTypeCS() != CS_SINGLE && !sit.PlayerIsCard(nb))
    {
        disp=0;
        return(costStack->CostStackEndGame(hist,nb)-monS);
    }
//    if(sit.NameTrade()==TRADE_PREFLOP && sit.CNPlayerIsCard()<=3)
//    {
//        clDoubleCP res=TSCalcPreFlop(&sit);
//        return res[nb]-monS;
//    }
    clCol col;
    col.New();
    int myNb=hist->ViewPlayer();
//    int nbCnPl=0;
    for(int i=0;i<CN_PLAYER;i++)
        if(sit.PlayerIsCard(i) || i==myNb)     //????? Наши карты
            for(int j=0;j<2;j++)
                col.Skip(hist->GetSeat(i)->_cards[j]);
    for(int i=0;i<sit.CnCardTable();i++)
        col.Skip(sit.CardsTable()[i]);
    double *res=(double *)malloc(CN_CALC_AUTOPL*sizeof(double));
    for(int i=0;i<CN_CALC_AUTOPL;i++)
    {
        clCol col1=col;
        sit0=sit;
        col1.Shufle();
        col=col1;
        for(int j=sit.CnCardTable();j<5;j++)
            sit0.CardsTable()[j]=col1.GiveCard();
        sit0.DeltToRiver();
        HeroDecDistribWin(&sit0);
        res[i]=costStack->CostStackTable(&sit0,nb);
    }
    double val=AverageAndDisp(CN_CALC_AUTOPL,res,disp);
    free(res);
    return val-monS;
}
//---------------------------------------------------------------------------
int  CheckAllInOutRiver(clGameHistoryTrn *hist,clSitHoldem *sit)
{
    sit->ExecHist(hist,0);
    for(int i=0;i<hist->CnAct();i++)
    {
        sit->ExecNextAct();
        if(sit->TableIsAllIn())
            return i;
    }
    return(-1);
}
//---------------------------------------------------------------------------
int     CheckAllInPreFlop(clGameHistory *hist)
{
    int nbA=hist->NbFirstAct(ACT_DELFLOP);
    if(nbA==-1) return -1;
    for(int i=nbA;i<hist->CnAct();i++)
        if(HistIsTradeAct(hist->GetAct(i)->_dis._act))
            return -1;
    return nbA;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
