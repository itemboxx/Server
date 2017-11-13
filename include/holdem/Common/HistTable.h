//---------------------------------------------------------------------------

#ifndef HistTableH
#define HistTableH
//---------------------------------------------------------------------------
#include "C:\home\holdem\Common\History.h"

//#define OLDVER       
                      
//enum enPosition  {POS_EARLY=0,POS_MIDDLE,POS_CUTTOF,POS_BUTTON,POS_SB,POS_BB,POS_UNDEF};
enum enNameTrade    {_TRADE_UNDEF,TRADE_PREFLOP,TRADE_FLOP,TRADE_TURN,TRADE_RIVER};

#define CN_POSITION     POS_UNDEF

struct tpPlAtr
{
    inline  int Stack()         {return _money+_bankPl+_totalBetPl;}
    inline  void    Clear()     {_isCard=_viewCards=false; _money=_bankPl=_totalBetPl=0;}

    int _totalBetPl,_bankPl,_money;
    bool _isCard,_viewCards;
};

class clHistTable :  public clGameHistory
{
friend class clSitHoldem;
friend class clSitTurbo6;
friend class tpScanTable;
public:
    inline  clHistTable()       {ClearTable();}
    void    ClearTable();
    void    operator = (clHistTable &table);
    inline  clGameHistory   *PtrHistory()   {return this;}
    void    ExecHist(clGameHistory *hist,int nbAct);
//    void    StartGame();
    int     CNPlayerInPlayWithMoney();
    inline  void    ExecHist(clGameHistory *hist)       {ExecHist(hist,hist->CnAct());}
    void    ExecHist(int nbAct);
    inline  int Indicator()         {return _indicator;}
    inline  void    ExecNextAct()       {ExecAct(_nbAct++);}
    inline  enNameTrade NameTrade()     {return _trade;}
    int     MaxBet();
    inline  int     CnExecAct()         {return _nbAct;}
    inline  bool    PlayerIsCard(int nb)    {return(_pl[nb]._isCard);}
    inline  int PlayerStack(int nb)     {return _pl[nb].Stack();}
    inline  int PlayerMoney(int nb)     {return _pl[nb]._money;}
    inline  bool    PlayerViewCard(int nb)  {return(_pl[nb]._viewCards);}
    inline  int     TotalBetPl(int nb)      {return(_pl[nb]._totalBetPl);}
    inline  int PlayerBank(int nb)      {return _pl[nb]._bankPl;}
    tpAction    GoPrevNextTradeAct();
    void    ExecAct(tpAction act);
    inline  void    ExecDis(tpDis dis)      {ExecAct(HistAction(_indicator,dis._act,dis._money));}
    inline  int     CNPlayerIsCard()        {return(_cnPlIsCard);}
    int     CNPlayerIsCard(int *pos);
    inline  int     CNPlayerInTrade()       {return(_cnPlInTrade);}
    inline  int     TotalPot()              {return(_totalPot);}
    inline  int     CnCardTable()           {return(_cnCardTable);}
    inline  clCard  *   CardsTable()        {return _atrHH._cardTable;}
    clGamePoker     GamePlayerHoldem(int i);
//    inline  int CnPlayer()                  {return _cnPl;}
    void    CloseBank();
    bool    TableIsAllIn();
    bool    MayBeCheck();
    inline  tpPlAtr *PlayerAtr(int nb)  {return _pl+nb;}
    int NextPlayerWithCard(int nbPl);
    bool    WrViewPlayer(int posH);
//    inline  int MaxPlayer()             {return _hist.MaxPlayer();}
//    inline  clCard *PtrCards(int nb)    {return _hist.PlCards(nb);}
//    inline  int     CnAllAct()          {return _hist.CnAct();}
//    inline  tpAction  GetAct(int nb)    {return _hist.GetAct(nb);}
//    inline  int BigBlind()              {return _hist.BigBlind();}
//    void    ExecActTo(int nbAct);
//    enPosition  Position(int nb);
//    inline  enPosition  Position()      {return Position(_heroNb);}
//    bool    ClearWater();
//    inline  int NbButton()      {return _nbButton;}
//    inline  int NbSmallBlind()            {return _nbSB;}
//    inline  int NextPlayer(int nb)      {return _hist.NextPlayerInPlay(nb);}
//    int     NbPosition(enPosition pos);
//    inline  clCard *CardsTable()        {return _hist.CardTable();}
//    inline  int SmallBlind()            {return _hist.SmallBlind();}
//    inline  bool    IsPlayer(int nb)    {return _hist.IsPlayer(nb);}
//    inline  tpSeat  *GetSeat(int nb)     {return _hist.GetSeat(nb);}
//    inline  void    WrBigBlind(int bb)  {_hist.WrBlinds(HistSmallBlind(bb),bb);}
//    void    Correct();
//    inline  int     CnCardPl()              {return _cnCardPl;}
//    inline  void    WrBankPl(int nb, int money)     {_pl[nb]._bankPl=money;}
//    tpDis   CreateDisCallCheck();

private :
    void    ExecAct(int nb);
    void    CheckIndicator();
    void    ActRaiseChangeIndicator();
    void    InitIndicatorOnFlop();
    int     NextPlayerMove(int nb);
    inline  int     NextPlayerMove()    {return NextPlayerMove(_indicator);}
    int     AnotherMaxBet(int nbPl);

//    clGameHistory _hist;
    int _nbAct,_totalPot,_cnCardTable,_posHero;
    tpPlAtr _pl[CN_PLAYER];
    enNameTrade _trade;
    int _indicator,_begTrade,_cnPlInTrade,_cnPlIsCard;
};

//enPosition  HistTablePosition(clGameHistory *hist,int nb);
//enPosition  HistTableStringToPos(char *str);
//AnsiString  HistTablePosToString(int pos);
bool    HistTableCheckHH(clGameHistory *hist);
//---------------------------------------------------------------------------
#endif
