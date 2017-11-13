//---------------------------------------------------------------------------

#ifndef HistoryH
#define HistoryH
#include <windows.system.h>
#include "..\..\Util\cTimeData.h"
#include "..\..\Util\workstr.h"
#include "..\..\card\pokGame.h"
//---------------------------------------------------------------------------
#define CN_PLAYER                   6
#define MAX_CN_PLAYER               10
#define NB_DILER        13
#define UNDEF_NAME      "Undef"
#define MAX_CN_ACTHISTORY       128
#define MAX_LEN_NAME_PLAYER     32
#define MAX_LEN_TABLE_NAME      128

#define POS_BB      0
#define POS_SB      1
#define POS_BUTTON  2
#define POS_CUTOFF  3
#define POS_MIDDLE  4
#define POS_EARLY   5
#define POS_UNDEF   -1

bool HistInPosition(int nbPl1,int nbPl2); //true if nbPl1 in position

#define CALC_FOR_TOURN

enum enSite         {SITE_PARTYPOKER,SITE_FULLTILT,SITE_POKERSTARS,SITE_UNDEF};
enum enTypePoker    {TPP_UNDEF=0,TPP_HOLDEM,TPP_OMAHA};
enum enTypeLimit    {TPL_UNDEF=0,TPL_LIMIT,TPL_POTLIMIT,TPL_NOLIMIT};
enum enTypeGame     {GAME_CASH=0,GAME_TRNY_SINGLE,GAME_TRNY_MULTY};
enum enAction:char  {ACT_UNDEF=0,ACT_POSTANTE,ACT_POSTSB,ACT_POSTBB,
    ACT_FOLD,ACT_CHECK,ACT_CALL,ACT_BET,ACT_RAISE,ACT_RAISE_TO,ACT_ALL_IN,
    ACT_WIN,ACT_SHOW,ACT_DELCARD,ACT_DELFLOP,ACT_DELTURN,
    ACT_DELRIVER,ACT_FINISH,ACT_CONNLOST,ACT_RETURN_UNCALLED_BET,
    ACT_MUCK,ACT_RETURNED,ACT_SIT_OUT,ACT_POST_DEAD,ACT_DISCONNECTED,ACT_PLAYER_IN_ALLIN};

struct tpDis
{
    inline  bool operator ==(tpDis &dis)     {return (_act==dis._act && _money==dis._money);}

    enAction _act;
    int _money;
};
#define _nbSeat         _pos
struct tpAction
{
    tpDis _dis;
    byte _pos;

	bool operator == (tpAction &act)		{ return _pos == act._pos && _dis == act._dis; }
	bool operator != (tpAction &act)		{ return (!(*this == act)); }
};
struct tpArrAct
{
    inline  void    Clear()         {_cnAct=0;}
    void    ReadFile(int handle);
    void    WriteFile(int handle);
    int     SizeInFile();

    int _cnAct;
    tpAction _act[MAX_CN_ACTHISTORY];
};

inline  tpDis   HistCreateDis(enAction act,int money=0)       {tpDis dis;dis._act=act;dis._money=money;return dis;}

struct tpSeat
{
    inline  tpSeat()    {Clear();}
    inline  void    Clear()     {_busy=_inPlay=false;_money=0;_name[0]='\0';}
    bool operator == (tpSeat &seat);
    char _name[MAX_LEN_NAME_PLAYER];
    clCard _cards[2];
    int _money;
    bool _inPlay,_busy;
};

struct tpGameHH
{
friend class clGameHistory;
friend class clListHistory;
friend class clHistTable;

    void    Clear();

//private:
    BYTE _cnPl,_timeLevel,_maxSeat;
    int _tableCost,_bb,_ante;
    long _nbTrny;
    stDayTime _date;
    long long _nbG;
    clCard  _cardTable[5];
    BYTE _param[8];
};

class clGameHistory
{
friend class clListHistory;
//friend class clSitHoldem;
friend class clHistTable;
public:
    clGameHistory();
    ~clGameHistory();
	void    operator =(clGameHistory &hist);
	bool    operator ==(clGameHistory &hist);
	void    Clear();
    int     ReadFile(int handle);
    int     WriteFile(int handle);
    inline  long    NbTrny()            {return _atrHH._nbTrny;}
    inline  int CnAct() {return _arrAct._cnAct;}
    inline  tpAction *GetAct(int nb)     {return &_arrAct._act[nb];}
    int     CnPlayerInPlay();
    inline  int CnPlayer()              {return _atrHH._cnPl;}
    inline  bool PlayerInPlay(int nb)   {return _seat[nb]._inPlay;}
    inline  int MaxCnSeat()             {return _atrHH._maxSeat;}
    int     ViewPlayer();
    int     NbTable();
    inline  char    *PlayerName(int nb) {return _seat[nb]._name;}
    inline  int PlMoney(int nb)         {return _seat[nb]._money; }
    inline  clCard  *PlayerCards(int nb){return _seat[nb]._cards;}
    inline  clCard *CardTable()         {return _atrHH._cardTable;}
    inline  int TableBuyIn()            {return _atrHH._tableCost;}
//    inline  enTypeGame  TypeGame()      {return (enTypeGame)_atrHH._tpGame;}
    inline  long long   NbGame()        {return _atrHH._nbG;}
    inline  bool IsPlayer(int nb)       {return _seat[nb]._busy;}
    inline  stDayTime Date()            {return _atrHH._date;}
    inline  int SmallBlind()            {return _atrHH._bb/2;}
    inline  int BigBlind()              {return _atrHH._bb;}
    int     FindSeat(char *name);
//    inline  int     CnCardPoker()       {return _arrSeat._cnCardPoker;}
    int     NextPlayer(int nb);
    int     PrevPlayer(int nb);
    int     NextPlayerInPlay(int nb);
    int     PrevPlayerInPlay(int nb);
    int     NbFirstTradeAct(int nbAct);
    int     NbTradeActPl(int nbPl,int nbAct);
	inline  double *PtrParamDouble() { return (double *)_atrHH._param; }
	inline  int &ParamInt() { return *((int *)_atrHH._param); }

        // Функции построения истории
    int     AddAct(int nb,enAction act,int many);
    inline  int AddAct(char *name,enAction act,int many)    {int nb=FindSeat(name); return (nb != -1)?  AddAct(nb,act,many):-1;}
    int     InsertAct(int nbAct,tpAction act);
    bool    DelAct(int nbAct);
    inline  void    WrNbGame(long long nb)  {_atrHH._nbG=nb;}
    void    WrNbTable(int nb);
    void    WrNameTable(char *name);
    inline  void    WrNbButton(int nb)  {_arrNbPos[POS_BUTTON]=nb;}
    inline  void    WrCnSeat(int cn)    {_atrHH._maxSeat=cn;}
    void    WrSeatInfo(int nb,char *name,int many,bool inPlay=true);
    void    DilDelt0(bool blind);
    void    WrFlop(clCard *card);
    void    WrTurn(clCard card);
    void    WrRiver(clCard card);
    inline  void    WrStakes(int b1)        {_atrHH._bb=b1;}
    inline  void    WrBigBlinds(int b1)     {_atrHH._bb=b1;}
    inline  void    WriteDate(stDayTime day)  {_atrHH._date=day;}
    void    WrTableCostCash(int cost);
    bool    CheckActShow(int nbPl);

    inline  tpSeat   *GetSeat(int nb)   {return _seat+nb;}
	inline  int NbButton()      { return (CnPlayer() == 2) ? _arrNbPos[POS_SB] : _arrNbPos[POS_BUTTON]; }
    inline  int GetCnAct()      {return _arrAct._cnAct;}
//    AnsiString HistCaption();
    inline  int AddAct(tpAction *act)    {return AddAct(act->_pos,act->_dis._act,act->_dis._money);}
//    int     NbBigBlind();
    int     NbFirstAct(enAction act);
    inline  int     Ante()              { return _atrHH._ante;}
    inline  int PositionToNbTable(int pos)      {return _arrNbPos[pos];}
    int     NbToPosition(int nb);
    void    RestraightNbToPos();
    int SizeInFile();

//private:
    tpGameHH _atrHH;
    char _nameTable[MAX_LEN_TABLE_NAME];
    BYTE _arrNbPos[CN_PLAYER];
    tpSeat _seat[CN_PLAYER];
    tpArrAct   _arrAct;
};

class clListHistory
{
public:
    clListHistory();
    ~clListHistory();
    void    Clear();
    void    Clear(int nb);  //Очистить все кроме nb
    void    operator =(clListHistory &list);
    inline  bool    Empty()     {return _cn==0;}
    bool Add(clGameHistory *ptrH);
    bool AddCopy(clGameHistory *ptrH);
    void InsertCopy(clGameHistory *ptrH,int nb);
    void    DelGameHist(int nb);
    inline clGameHistory *GameHist(int nb)  {return _ptrLH[nb];}
    inline clGameHistory *LastGameHist()    {return _ptrLH[_cn-1];}
    inline int CnGame() {return _cn;}
    int     CnTrny();
    void   RepairListHistToStraight();
//    void    Divide(clListHistory *ptrL);
//    inline  enTypeTrny  TpTrny()    {return (_cn>0)? _ptrLH[0]->_tpTrny:TPT_UNDEF;}
    inline  long  NbTrny()    {return (_cn>0)? _ptrLH[0]->_atrHH._nbTrny:0;}
//    inline  int   BuyIn()     {return (_cn>0)? _ptrLH[0]->_tableCost:0;}
//    inline  int TimeLevel() {return (_cn>0)? _ptrLH[0]->_timeLevel:0;}
    int     WriteFile(int handle);
    int     ReadFile(int handle);
//    void    Caption(char *str);
    inline  int     NbViewPlayer()  {return _ptrLH[0]->ViewPlayer();}
//    inline  int     FindSeat(char *name)  {return _ptrLH[0]->FindSeat(name);}
//    AnsiString    Name();
//    int     SizeInBuf();
//    void    ReadBuf(byte *buf);
//    void    WriteBuf(byte *buf);
//    inline  char *NameTable()   {return _ptrLH[0]->_nameTable;}
private:
    int _cn,_dim;
    clGameHistory **_ptrLH;
};

inline  int     HistSmallBlind(int bb)      {return bb/2;}
inline  tpAction    HistAction(int nb, enAction act0,int money)   {tpAction act;act._dis._act=act0;act._dis._money=money;act._pos=nb; return act;}
inline  bool    HistIsActiveTradeAct(enAction act)  {return (act>=ACT_CALL && act<=ACT_ALL_IN);}
inline  bool    HistIsRaiseAct(enAction act)  {return (act>=ACT_BET && act<=ACT_ALL_IN);}
inline  bool    HistIsTradeAct(enAction act)  {return (act>=ACT_FOLD && act<=ACT_ALL_IN);}
clAnsiString  HistDisToString(tpDis dis);
clAnsiString  HistActToString(tpAction act); 
clAnsiString  HistActToString(enAction act);
clAnsiString  HistPosToString(int pos);
tpDis   HistStringToDis(char *str,int &shift);
bool    AccuratDis(tpDis disRef, tpDis disCheck);
int     HistReadFile(int handle,void *ptr);
int     HistWriteFile(int handle,void *ptr);
int     HistListReadFile(int handle,void *ptr);
int     HistListWriteFile(int handle,void *ptr);
int     HistListSizeInFile(int handle,void *ptr);
//---------------------------------------------------------------------------
#endif
