//---------------------------------------------------------------------------

#ifndef BoardGamesH
#define BoardGamesH
//---------------------------------------------------------------------------
//#include "c:\NewWork\Calc\Common\sitHoldem.h"
#include "..\..\card\pokGame.h"
//#include "afxdialogex.h"

enum enParamHV_1  {PHV1_TWO_OVER=0,/**/PHV1_ONE_OVER,/**/PHV1_NO_OVER,                                      // NO_PAIR
                   PHV1_POCKET_PAIR,/**/PHV1_TOP_PAIR,/**/PHV1_SECOND_PAIR,/**/PHV1_BOTTOM_PAIR,/**/
                   PHV1_PAIRED_BOARD,                                                                       // ONE_PAIR
                   PHV1_BOTH_CARD_PAIRED,/**/PHV1_PAIR_AND_PAIR_ON_BOARD,/**/
                   PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD,/**/PHV1_TWO_PAIR_ON_BOARD,                           // TWO_PAIR
                   PHV1_SET,/**/PHV1_TRIPS,/**/PHV1_TRIPS_ON_BOARD,                                         // GM_3
                   PHV1_TWO_CARDS_NUTS_STRAIGHT,/**/PHV1_TWO_CARDS_STRAIGHT,/**/PHV1_ONE_CARD_NUTS_STRAIGHT,
                   PHV1_ONE_CARD_STRAIGHT,/**/PHV1_STRAIGHT_ON_BOARD,                                       // STRAIGHT
                   PHV1_3CARD_NUTS_FLUSH,/**/PHV1_3CARD_HIGH_FLUSH,/**/PHV1_3CARD_LOW_FLUSH,
                   PHV1_4CARD_NUTS_FLUSH,/**/PHV1_4CARD_HIGH_FLUSH,/**/PHV1_4CARD_LOW_FLUSH,
                   PHV1_FLUSH_ON_BOARD_NUTS,/**/PHV1_FLUSH_ON_BOARD_HIGH,/**/PHV1_FLUSH_ON_BOARD_LOW,       // FLUSH
                   PHV1_FULL_2CARD_GOOD,/**/PHV1_FULL_2CARD_NUTS,/**/PHV1_FULL_2CARD_BAD,
                   PHV1_FULL_1CARD_GOOD,/**/PHV1_FULL_1CARD_NUTS,/**/PHV1_FULL_1CARD_BAD,
                   PHV1_FULL_ON_BOARD,                                                                      // FULL
                   PHV1_SQUARE_WITH_POCKET,/**/PHV1_SQUARE_WITHOUT_POCKET,/**/PHV1_SQUARE_ON_BOARD,         // GM_4
                   PHV1_STRAIGHT_FLUSH_2CARD,/**/PHV1_STRAIGHT_FLUSH_1CARD,/**/PHV1_STRAIGHT_FLUSH_ON_BOARD,// STR FLUSH
                   PHV1_UNDEF
                  };
enum enParamHV_2 {PHV2_OVER_PAIR=0,/**/PHV2_SECOND_PAIR,/**/PHV2_LOW_PAIR,                                  // PHV1_POCKET_PAIR
                  PHV2_TOP_KICKER,/**/PHV2_GOOD_KICKER,/**/PHV2_WEAK_KICKER,                                // PHV1_TOP_PAIR
                  PHV2_ACE_KICKER,/**/PHV2_NOT_ACE_KICKER,                                                  // PHV1_SECOND_PAIR && PHV1_BOTTOM_PAIR
                  PHV2_TWO_OVER,/**/PHV2_ONE_OVER,/**/PHV2_NO_OVER,                                         // PHV1_PAIRED_BOARD
                  PHV2_TOP_TWO_PAIR,/**/PHV2_TOP_PAIR_AND_PAIR,/**/PHV2_MIDDLE_PAIR_AND_BOTTOM,             // PHV1_BOTH_CARD_PAIRED
                  PHV2_2P_TOP_PAIR,/**/PHV2_2P_SECOND_PAIR,/**/PHV2_2P_BOTTOM_PAIR,                         // PHV1_PAIR_AND_PAIR_ON_BOARD
                  PHV2_2P_OVER_PAIR,/**/PHV2_2P_BIGGER_PAIR_ON_BOARD,/**/PHV2_2P_LESS_PAIR_ON_BOARD,        // PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD
                  PHV2_HIGH_SET,/**/PHV2_SECOND_SET,/**/PHV2_LOW_SET,                                       // PHV1_SET
                  PHV2_HIGH_TR_HIGH_KICKER,/**/PHV2_HIGH_TR_LOW_KICKER,/**/PHV2_SECOND_TR_HIGH_KICKER,
                  PHV2_SECOND_TR_LOW_KICKER,/**/PHV2_LOW_TR_HIGH_KICKER,/**/PHV2_LOW_TR_LOW_KICKER,         // PHV1_TRIPS
                  PHV2_POCKET_PAIR_NO_TRIPS_ON_B,/**/PHV2_POCKET_PAIR_AND_TRIPS_ON_B,/**/PHV2_NO_P_NO_TRIPS,// PHV1_FULL_2CARD
                  PHV2_FULL_TRIPS_ON_BOARD,/**/PHV2_FULL_TOP_PAIR_NO_TR,/**/PHV2_FULL_BOTTOB_PAIR_NO_TR,    // PHV1_FULL_1CARD
                  PHV2_UNDEF
                 };
enum enFlushDrawParamHV_1   {DPHV_NO_FLUSH_DRAW=0,DPHV_2CARD_NUTS_BACKDOOR_FLDR,/**/DPHV_2CARD_HIGH_BACKDOOR_FLDR,
                             DPHV_1CARD_NUTS_BACKDOOR_FLDR,/**/DPHV_1CARD_HIGH_BACKDOOR_FLDR,
                             DPHV_1CARD_LOW_FLDR,DPHV_1CARD_HIGH_FLDR,DPHV_1CARD_NUTS_FLDR,
                             DPHV_2CARD_LOW_FLDR,DPHV_2CARD_HIGH_FLDR,DPHV_2CARD_NUTS_FLDR,
                             DPHV_FLUSH,DPHV_UNDEF
                            };
enum enStraightDrawParamHV_2  {DPHV2_2CARD_STR_DRAW=0,/**/DPHV2_2CARD_GTSH_DRAW,/**/DPHV2_1CARD_STR_DRAW,
                               DPHV2_1CARD_GTSH_DRAW,/**/DPHV2_NO_STR_DRAW,/**/DPHV2_UNDEF
                              };
class clHandValue
{
public :
    clHandValue();
    void    InitHV(clCard *cardPl,clCard *cardT,int cnCardT);
//    void    Init(clSitHoldem *sit,int nbPl);
//    void    Init(clHistTable *table,int nbPl);
    inline  clGamePoker GameHoldem()             {return _game;}
    inline  enParamHV_1   FirstParam()    {return _firstP;}
    inline  enParamHV_2   SecondParam()   {return _secondP;}
    CString FirstParamStr();
	CString SecondParamStr();
	CString Param1();
	CString Param2();
    inline  enFlushDrawParamHV_1    FlushDraw()     {return _param1;}
    inline  enStraightDrawParamHV_2 StraightDraw()  {return _param2;}
    bool    IsFlushDraw2Card();
    bool    IsGoodFlushDraw1Card();
//    bool    IsStraightDraw2Card();

private :
    void    Init(clCard *plCards,clCard *board,int cnCardBoard);
    void    SearchCards(clCard *board,int cnBoard,int &biggerCard,int &secondCard);
    void    SearchCardsNoCard(clCard *board,int cnBoard,enFace face,int &biggerCard,int &secondCard);
    enFace  IsStraight(clCard *board,int cnBoard,clCard *cardPl);
    void    ChangeBoard(clCard *board,int cnBoard);
    void    InitFlushDraw(clCard *plCards,clCard *board,int cnCardBoard);
    void    InitStraightDraw(clCard *plCards,clCard *board,int cnCardBoard);
    int     IsStraight(bool *arrFace,enFace face,enFace pl1,enFace pl2,bool *arrFaceBoard);

    clGamePoker _game;
    enParamHV_1 _firstP;
    enParamHV_2 _secondP;
    enFlushDrawParamHV_1 _param1;
    enStraightDrawParamHV_2 _param2;
};
enum enDangerBoard  {DANGER_BOARD_NO=0,DANGER_BOARD_LOW,DANGER_BOARD_MIDDLE,DANGER_BOARD_HIGH,
                DANGER_BOARD_HORROR};
enum enDGPairBoard  {DGP_NO=0,/**/DGP_2,/**/DGP_2_2,/**/DGP_3,/**/DGP_3_2,/**/DGP_4};
enum enDGFlushBoard  {DGF_NO=0,/**/DGF_2,/**/DGF_2_2,/**/DGF_3,/**/DGF_4,/**/DGF_5};
enum enDGStraightBoard  {DGS_NO=0,/**/DGS_DRAW,/**/DGS_2,/**/DGS_1};
#define     MIN_CARD_TO_STR_INT     C_5
struct stDGStraightBoard
{
    int _cnCard;
    int _cnCardMoreThen;
    enDGStraightBoard _straightB;

};

class clBoard
{
    friend class clHandValue;
public :
    void Init(clCard *board,int cnBoard);
    enDangerBoard   DangerDraw();
    enDangerBoard   DangerFlushDraw();
    enDangerBoard   DangerStraightDraw();
    enDangerBoard   DangerFullHouse();
    enDangerBoard   DangerFlush();
    enDangerBoard   DangerStraight();
    enDangerBoard   DangerStrOrFlush();
    inline stDGStraightBoard StraightDanger()       {return(_straightB);}
    inline int      CnCardOnBoard()         {return _cnBoard;}
    inline clCard  CardOnBoard(int cn)      {return ((cn>=_cnBoard) ? Card(C_UNDEF,S_UNDEF) : _board[cn] );}
    enFace          BoardHighCard();
	CString PairB();
	CString FlushB();
	CString StraightB();

//private :

    bool IsStraight(bool *arrFace,enFace face);

    clCard     _board[5];
    int        _cnBoard;
    enDGPairBoard  _pairB;
    enDGFlushBoard  _flushB;
    stDGStraightBoard  _straightB;
};

CString BGParamHV_1ToString(enParamHV_1 p1);
CString BGParamHV_2ToString(enParamHV_2 p2);
CString BGFlushDrawParamHV_1(enFlushDrawParamHV_1 p3);
CString BGenStraightDrawParamHV_2(enStraightDrawParamHV_2 p4);
enParamHV_1 BGStringToParamHV_1(char *str);
enParamHV_2 BGStringToParamHV_2(char *str);
enFlushDrawParamHV_1 BGStringToFlushDrawParamHV_1(char *str);
enStraightDrawParamHV_2 BGStringToStraightDrawParamHV_2(char *str);
int BGCnParam1(enGame g,enParamHV_1 *arr1);
int BGCnParam2(enParamHV_1 p1,enParamHV_2 *arr2);
//---------------------------------------------------------------------------
#endif
