//---------------------------------------------------------------------------
#pragma hdrstop

//#include "c:\Work\Tournament\Calc\Common\GlobalsForCalc.h"
#include "C:\home\holdem\Common\clCostStack.h"

//double  glPrizeTrn6Turbo[]={100,0,0,0,0,0};
double  glPrizeTrn6Turbo[] = { 3, 3, 0, 0, 0, 0 };

//clCostStack glCostStackICM(1, glPrizeTrn6Turbo), glCostStackTS;
clCostStack glCostStackICM(2, glPrizeTrn6Turbo), glCostStackTS;

double  VerPlace(int cnPl,int nb,int *stak,int place);
void    CalcStacDelta();
//---------------------------------------------------------------------------
clCostStack::clCostStack()
{
//    SetPrize(1,3,100);
    _func=NULL;
    _cnPrize=0;
}
//---------------------------------------------------------------------------
clCostStack::~clCostStack()
{
    free(_prize);
}
//---------------------------------------------------------------------------
int CnPrize(int cnPl)
{
    if(cnPl<=30)
        return 3;
    else if(cnPl<=60)
        return 6;
    else if(cnPl<=70)
        return 9;
    else if(cnPl<=90)
        return 10;
    else if(cnPl<=100)
        return 13;
    else if(cnPl<=130)
        return 16;
    else if(cnPl<=200)
        return 20;
    else
        return 30;
}
//---------------------------------------------------------------------------
void    clCostStack::SetPrize(int cnPrize,double *prize)
{
    _cnPrize=cnPrize;
    _prize=(double*)malloc(_cnPrize*sizeof(double));
    memcpy(_prize,prize,_cnPrize*sizeof(double));
//    _lineCost=false;
}
//---------------------------------------------------------------------------
double  clCostStack::PrizeFond()
{
    double res=0;
    for(int i=0;i<_cnPrize;i++)
        res+=_prize[i];
    return res;
}
//---------------------------------------------------------------------------
double clCostStack::CostStack(int cnPl,int nbPl,int *stack)
{
//    if(_lineCost) return stack[nbPl];
    int *ptr=(int *)malloc(cnPl*sizeof(int));
    memcpy(ptr,stack,cnPl*sizeof(int));
    double res=CostStackPrice(_cnPrize,_prize,cnPl,ptr,nbPl);
    free(ptr);
    return res;
}
//---------------------------------------------------------------------------
clDoubleCP  clCostStack::CostStack(int cnPl,int *stack)
{
    clDoubleCP res;
    for(int i=0;i<CN_PLAYER;i++)
        res[i]=(i<cnPl)? CostStack(cnPl,i,stack) : 0;
    return res;
}
//---------------------------------------------------------------------------
/*double  clCostStack::CostStackBeginGame(clGameHistory *hist,int nb)
{
    int stacks[CN_PLAYER],cnPl=0,nbBut=hist->NbButton();
    for(int i=0;i<CN_PLAYER;i++)
    {
        if(nb==i)
            nb=cnPl;
        if(nbBut==i)
            nbBut=cnPl;
        if(hist->IsPlayer(i))
            stacks[cnPl++]=hist->PlMoney(i);
    }
    return CostStack(cnPl,nb,stacks,nbBut,hist->BigBlind());
} */
//---------------------------------------------------------------------------
double  clCostStack::CostStackBeginGame(clGameHistory *hist,int nb)
{
    int cnPl=hist->CnPlayer();
    clIntCP st0;
    for(int i=0;i<cnPl;i++)
        st0[i]=hist->PlMoney(i);
    if(_func==NULL)
        return CostStack(cnPl,nb,st0._arr);
    else
        return _func(cnPl,hist->BigBlind(),st0._arr)[nb];
}
//---------------------------------------------------------------------------
double  clCostStack::CostStackEndGame(clGameHistory *hist,int nb)
{
    clSitHoldem table;
    table.ExecHist(hist);
    return CostStack(&table,nb);
}
//---------------------------------------------------------------------------
double  clCostStack::CostStack(clSitHoldem *table, int nb)
{
    int cnPl=table->CnPlayer();
    int st0[CN_PLAYER],st1[CN_PLAYER];
    for(int i=0;i<cnPl;i++)
    {
        st0[i]=table->clGameHistory::PlMoney(i);
        st1[i]=table->PlayerMoney(i);
    }
    return  CostStack(cnPl,nb,st1,st0,table->BigBlind());
}
//---------------------------------------------------------------------------
double  clCostStack::CostStack(int cnPl,int nbPl,int *stack,int *stack0,int bb)
{
    if(stack[nbPl]==0)
        return  CostPlace(CostStackCalcPlace(cnPl,stack0,stack,nbPl));
    else
    {
        clIntCP stacks;
        int cnPlNew=0;
        for(int i=0;i<cnPl;i++)
        {
            if(nbPl==i)
                nbPl=cnPlNew;
            if(stack[i]>0)
                stacks[cnPlNew++]=stack[i];
        }
        //смещаем баттон
        int money=stacks[cnPlNew-1];
        for(int i=cnPlNew-1;i>0;i--)
            stacks[i]=stacks[i-1];
        stacks[0]=money;
        nbPl++;
        if(nbPl>=cnPlNew) nbPl=0;
        if(_func==NULL)
            return CostStack(cnPlNew,nbPl,stacks._arr);
        else
            return _func(cnPlNew,bb,stacks._arr)[nbPl];
    }
}
//---------------------------------------------------------------------------
/*int     clCostStack::Delta(int cnPl,int blind,int pos)
{
    switch(cnPl)
    {
        case 3 : return
    }
}
*/
//---------------------------------------------------------------------------
//***************************************************************************
//---------------------------------------------------------------------------
double CostStackPrice(int cnPrize,double * prize,int cnPl,int *stack,int nb)
{
    if(stack[nb]<=0)
        return 0;
    int i=0;
    while(i<cnPl)
    {
        if(stack[i]<=0)
        {
            for(int j=i;j<cnPl-1;j++)
                stack[j]=stack[j+1];
            if(nb>i) nb--;
            cnPl--;
        }
        i++;
    }
    double res=0;
    int cn=(cnPl>cnPrize)? cnPrize : cnPl;
    for(int i=0;i<cn;i++)
        res+=VerPlace(cnPl,nb,stack,i)*prize[i];
    return res;
}
//---------------------------------------------------------------------------
int     Sum(int cnPl,int *stack)
{
    int res=0;
    for(int i=0;i<cnPl;i++)
        res+=stack[i];
    return res;
}
//---------------------------------------------------------------------------
double  VerPlace(int cnPl,int nb,int *stack,int place)
{
    double res=0;
    if(place==0)
        res=(double)stack[nb]/Sum(cnPl,stack);
    else
        for(int i=0;i<cnPl;i++)
            if(i!=nb)
            {
                int stack0[10],cn0=0,nb0;
                for(int j=0;j<cnPl;j++)
                    if(j!=i)
                    {
                        stack0[cn0]=stack[j];
                        cn0++;
                    }
                nb0=(i<nb)? nb-1 : nb;
                res+=VerPlace(cnPl,i,stack,0)*VerPlace(cnPl-1,nb0,stack0,place-1);
            }
    return res;
}
//---------------------------------------------------------------------------
/*int CostStackCalcPlace(clAPTable *tab,clAPTable *tab1,int myNb)
{
}
//---------------------------------------------------------------------------
int Sum(int *arr,int cn)
{
    int res=0;
    for(int i=0;i<cn;i++)
        res+=arr[i];
    return res;
}
void    CalcStacDelta()
{
//    int arr4[]={200,22,43,-48,-17,400,64,80,-81,-63,600,97,126,-129,-93,800,133,162,-167,-128,1200,207,248,-262,-194,1600,374,429,-285,-518,2000,364,379,-556,-186};
//    int arr5[]={200,21,35,42,-48,-50,400,58,58,74,-122,-68,600,79,84,113,-181,-96,800,135,149,238,-169,-353,1200,262,247,249,-290,-469,1600,278,298,303,-354,-526,2000,334,340,355,-471,-558};
//    int arr6[]={200,18,29,35,41,-51,-73,400,52,46,47,67,-118,-95,600,85,95,105,157,-140,-303,800,108,135,149,216,-192,-417,1200,241,221,273,224,-323,-637};
    int arr[]={200,27,24,19,20,28,-64,-56,400,45,44,43,47,64,-126,-119,600,78,77,96,94,153,-154,-346,800,122,121,171,157,158,-234,-497,1200,145,155,172,191,193,-336,-522};
//    int arr3[]={400,82,-98,+16,600,143,-115,-28,800,200,-162,-37,1200,293,-260,-33,1600,389,-347,-41,2000,481,-392,-89};
    int arrD[100];
    int cnPl=7,step,blind[10];
    double av[CN_PLAYER];
    step=cnPl+1;
    int cnB=sizeof(arr)/sizeof(int)/step;
    for(int i=0;i<cnB;i++)
    {
        blind[i]=arr[i*step];
        int *ptr=arr+i*step+1;
        int *delta=arrD+i*cnPl;
        for(int j=cnPl-1;j>=0;j--)
            delta[j]=Sum(ptr,j+1);
        int aver=(Sum(delta,cnPl)+cnPl/2)/cnPl;
        for(int j=0;j<cnPl;j++)
            delta[j]=(delta[j]-aver)*100.5/blind[i];
    }
    for(int j=0;j<cnPl;j++)
    {
        av[j]=0;
        for(int i=0;i<cnB;i++)
            av[j]+=arrD[i*cnPl+j];
        av[j]/=cnB;
    }
//    step=cnPl+1;
} */
//---------------------------------------------------------------------------
int     CostStackCalcPlace(int cnPl,int *st0,int *st1,int myNb)
{
    int m=1,myS=st0[myNb];
    for(int i=0;i<cnPl;i++)
    {
        if(i==myNb) continue;
        if(st1[i]>0)
            m++;
        else if(st0[i]>0)
        {
            if(st0[i]>myS)
                m++;
            else if(st0[i]==myS)
                if(HistInPosition(myNb,i))
                    m++;
        }
    }
    return m;
}
//---------------------------------------------------------------------------
/*stLobbyProperty::stLobbyProperty()
{
    _cnEntrants=_cnPl=_cnTables=_cnRebuys=_cnAddons=_rebuysChips=_addonsChips=0;
    _buyIn=_rebuysMon=_addonsMon=0;
}
//---------------------------------------------------------------------------
void    CostStackChangeTrnyLobbi(stLobbyProperty *ptrTL)
{
    double money=(glTournamentLobby._cnEntrants+glTournamentLobby._cnRebuys+glTournamentLobby._cnAddons)*glTournamentLobby._rebuysMon;
    if(money==0)
        money=100;
    glTournamentLobby=*ptrTL;
    glCostStack.SetPrize(glTournamentLobby._cnTables,glTournamentLobby._cnEntrants,money);
}
//---------------------------------------------------------------------------
*/
