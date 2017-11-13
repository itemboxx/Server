//---------------------------------------------------------------------------

#ifndef sitHoldemH
#define sitHoldemH
//---------------------------------------------------------------------------
#include "C:\home\holdem\Common\ArrCnPlayer.h"
#include "C:\home\holdem\Common\History.h"
#include "C:\home\card\clCol.h"

//#define OLDVER       

//enum enPosition  {POS_EARLY=0,POS_MIDDLE,POS_CUTTOF,POS_BUTTON,POS_SB,POS_BB,POS_UNDEF};
//enum enNameTrade { TRADE_UNDEF = 0, TRADE_PREFLOP, TRADE_FLOP, TRADE_TURN, TRADE_RIVER };
enum enNameTrade { TRADE_PREFLOP = 0, TRADE_FLOP, TRADE_TURN, TRADE_RIVER, TRADE_UNDEF };
#define CN_POSITION     POS_UNDEF

struct tpPlAtr
{
	inline  int Stack() { return _money + _bankPl + _totalBetPl; }
	inline  void    Clear() { _isCard = _viewCards = false; _money = _bankPl = _totalBetPl = 0; }

	int _totalBetPl, _bankPl, _money;
	bool _isCard, _viewCards;
};

enum enSitName { SIT_CLEARWATER = 0, SIT_LIMP, SIT_RAISE, SIT_RAISE2, SIT_HORROR, SIT_REPLY, SIT_UNDEF };

#define CN_SIT  SIT_UNDEF

enum enTradeAction {
	TA_RAISE, TA_RAISE_FOLD, TA_RAISE_CALL, TA_RAISE_RAISE, TA_RAISE_ALL_IN,
	TA_BET, TA_BET_FOLD, TA_BET_CALL, TA_BET_RAISE, TA_BET_ALL_IN,
	TA_CHECK, TA_CHECK_FOLD, TA_CHECK_CALL, TA_CHECK_RAISE, TA_CHECK_MINIRAISE, TA_CHECK_ALL_IN,
	TA_LIMP, TA_CALL, TA_UNDEF
};

class clStacks : public clIntCP
{
public:
	void Norm(int sh);
	//clDoubleCP ClaccisCS(int cnPl=-1);
	void    Init(int bb, int ante, int mon0, int mon1, int mon2 = 0, int mon3 = 0, int mon4 = 0, int mon5 = 0);
	int     CnPlayer();
	inline  void    operator =(int val) { *(clIntCP *)this = val; }
	void    MoveButton(int cnPl);

	int _bb, _ante;
};


#define MAX_CNACT_SHORTSIT             10

inline  int SitHoldemStackBB100(int stack, int bb) { return stack * 100 / bb; }
class clSitHoldem : public clGameHistory
{
public:
	clSitHoldem() { Clear(); }
	void    Clear();
	void    operator = (clSitHoldem &table);
	bool	operator == (clSitHoldem &table);
	bool	Include(clSitHoldem &table);
	inline  clGameHistory   *PtrHistory() { return this; }
	void    ExecHist(clGameHistory *hist, int nbAct);
	void    StartNextGame();
	int     CNPlayerInPlayWithMoney();
	inline  void    ExecHist(clGameHistory *hist) { ExecHist(hist, hist->CnAct()); }
	void    ExecHist(int nbAct);
	inline  int Indicator() { return _indicator; }
	inline  void    ExecNextAct() { ExecAct(_nbAct++); }
	inline  enNameTrade NameTrade() { return _trade; }
	int     MaxBet();
	inline  int     CnExecAct() { return _nbAct; }
	inline  bool    PlayerIsCard(int nb) { return(_pl[nb]._isCard); }
	inline  int PlayerStack(int nb) { return _pl[nb].Stack(); }
	inline  int PlayerMoney(int nb) { return _pl[nb]._money; }
	inline  bool    &PlayerViewCard(int nb) { return(_pl[nb]._viewCards); }
	inline  int     TotalBetPl(int nb) { return(_pl[nb]._totalBetPl); }
	inline  int     BetPl(int nb) { return(_pl[nb]._totalBetPl + _pl[nb]._bankPl); }
	int		TotalBetAndBankAllPl();
	inline  int PlayerBank(int nb) { return _pl[nb]._bankPl; }
	tpAction    GoPrevNextTradeAct();
	void    ExecAct(tpAction act);
	inline  void    ExecDis(tpDis dis) { ExecAct(HistAction((_indicator == -1) ? NB_DILER : _indicator, dis._act, dis._money)); }
	inline  int     CNPlayerIsCard() { return(_cnPlIsCard); }
	int     CNPlayerIsCard(int *pos);
	inline  int     CNPlayerInTrade() { return(_cnPlInTrade); }
	inline  int     &TotalPot() { return(_totalPot); }
	inline  int     CnCardTable() { return(_cnCardTable); }
	inline  clCard  *   CardsTable() { return _atrHH._cardTable; }
	clGamePoker     GamePlayerHoldem(int i);
	//    inline  int CnPlayer()                  {return _cnPl;}
	void    CloseBank();
	bool    TableIsAllIn();
	bool    MayBeCheck();
	inline  tpPlAtr *PlayerAtr(int nb) { return _pl + nb; }
	int NextPlayerWithCard(int nbPl);
	bool    WrViewPlayer(int posH);
	clHand	&PlayerHand(int nb) { return *((clHand *)PlayerCards(nb)); }
	//	inline	int		GetNbHandPlayer(int nbPl);
	//	void	SetHandPlayer(int nbPl, int nbHand)				{ ((clHand *)PlayerCards(nbPl))->SetHand(nbHand); }

	void    Build(clGameHistory *ptrH, int nbAct = -1);
	//    void    Build(clShortSit *ss);
	inline  int     Position() { return _indicator; }
	inline  int     PosHero() { return _posHero; }
	inline  void    SetPosHero(int pos) { _posHero = pos; }
	inline  void    SetNbTable(int pos, int nbT) { _arrNbPos[pos] = nbT; }
	bool    FirstSit();
	bool    NextSit();
	bool    NextSitPreFlop();
	void    SitForIndicator(int round = 0);
	void	SetPrevTurnInd();
	inline  int     StackIndicator() { return PlMoney(Indicator()); }
	inline  int     StackPlayerInBB100(int pos) { return SitHoldemStackBB100(PlMoney(pos), BigBlind()); }
	inline  int     StackIndicatorInBB100() { return StackPlayerInBB100(Indicator()); }
	int     EffStackSitForPlayer(int posH);
	int     EffStackSit();
	int     EffStackAfterCall();
	int     EffStackAfterCall(int pos);
	inline  tpAction    *GetActIndicator() { return GetAct(_nbAct); }
	inline  tpAction    *GetLastAct() { return GetAct(_nbAct - 1); }
	inline  clCard  *IndicatorCards() { return PlayerCards(Indicator()); }
	inline  int CurAct() { return _nbAct; }
	enSitName   NameSit();
	tpDis   CreateDis(enAction act, int money = 0);
	inline  tpDis   CreateDis(tpDis dis) { return CreateDis(dis._act, dis._money); }
	inline  int     IndicatorMoney() { return PlayerMoney(Indicator()); }
	int		ResultGame(int nbPl) { return PlayerMoney(nbPl) - PlMoney(nbPl); }
	int     RoundTrade();
	int     CnRaiser(tpAction *arrAct);
	int     CnRaiser();
	int     PosRaiser(int nb);
	int     CnLimper();
	int     CnLimper(tpAction *arrAct);
	clStacks    GetStacksInBB();
	inline  clStacks    GetStacks() { return SitHGetStacks(this); }
	double  HeroCSClassic();
	tpDis   ChangeDis(tpDis dis);
	bool    IsEqualDis(tpDis dis1, tpDis dis2);
	tpDis   CreateDisCallCheck();
	static clStacks    SitHGetStacks(clSitHoldem *table);
	bool    NoFold();
	int     MaxBetForIndicator();
	enTradeAction   TradeActionPlayer(int nbPl, enNameTrade t);
	int     TotalPotForIndicator();
	bool    BoardInPosition();
	bool    IsPlayerInAllIn();
	tpDis   MinRaise();
	inline  int CurNbAct() { return _nbAct; }

	int		PotLimitBank();
	inline  int		NeedMoneyToCall() { return MaxBetForIndicator(); }
	void	Init(clStacks stacks);
	bool	EndedSit();
	void	DealCards(clCol deck);
	clDoubleCP	RestraightResPosToNb(clDoubleCP res);
	double	ChangeBB(int newBB);
	double	ChangeStacks(int newS);
	clIntCP CalcShowDown();
	void	DistribPot(clGamePoker *gamePoker);

private:
	void    ExecAct(int nb);
	void    CheckIndicator();
	void    ActRaiseChangeIndicator();
	void    InitIndicatorOnFlop();
	int     NextPlayerMove(int nb);
	inline  int     NextPlayerMove() { return NextPlayerMove(_indicator); }
	int     AnotherMaxBet(int nbPl);

	int _nbAct, _totalPot, _cnCardTable, _posHero;
	tpPlAtr _pl[CN_PLAYER];
	enNameTrade _trade;
	int _indicator, _begTrade, _cnPlInTrade, _cnPlIsCard;
};

void    SHBuildHistToStacks(clGameHistory *hist, clStacks st);

bool    SitIsSitForCallTogether(clSitHoldem *sit);
bool    SitIsSitForRaiseTogether(clSitHoldem *sit);
bool    SitDisIsRaiseTogether(clSitHoldem *sit, tpDis dis);
bool    SitHoldemStacksForCallTogether(clStacks st);
bool    SitHoldemStacksForRaiseTogether(clStacks st);
//AnsiString  SitHoldemSitToString(enSitName ns);
//---------------------------------------------------------------------------
#endif
