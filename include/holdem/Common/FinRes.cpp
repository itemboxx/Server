//---------------------------------------------------------------------------
#pragma hdrstop

#include "c:\Home\Holdem\Common\sitHoldem.h"
#include "c:\Home\Holdem\Common\FinRes.h"
#include "C:\home\holdem\Strategy\Source\CalcSitEV.h"

#define CN_CALC_AUTOPL          1000

//---------------------------------------------------------------------------
double    FinResGameEV(clGameHistory *hist,int nb)
{
    int monAvr[CN_PLAYER];
    if(!hist->GetSeat(nb)->_busy)
        return(0);
    int nbA=CheckAllIn(hist);
	if (nbA == -1)
		return FinResGame(hist, nb);
	else
	{
		clSitHoldem sit,sit0;
		sit.Build(hist, nbA);
		if (sit.NameTrade() == TRADE_PREFLOP)
		{
			clDoubleCP res = CalcSitEV(sit);
			return res[nb];
		}
		else
		{
			clCol col;
			col.New();
			int myNb = hist->ViewPlayer();
			for (int i = 0; i<CN_PLAYER; i++)
				if (sit.PlayerIsCard(i) || i == myNb)     //????? Íàøè êàðòû
				{
					col.Skip(hist->GetSeat(i)->_cards[0]);
					col.Skip(hist->GetSeat(i)->_cards[1]);
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
				sit0.DeltToRiver();
				HeroDecDistribWin(&sit0);
				res[i] = costStack->CostStackTable(&sit0, nb);
			}
			double val = AverageAndDisp(CN_CALC_AUTOPL, res, disp);
			free(res);
			return val - monS;
		}
	}
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
