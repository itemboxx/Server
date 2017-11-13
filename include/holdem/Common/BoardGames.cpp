//---------------------------------------------------------------------------
#pragma hdrstop

#include "c:\home\holdem\Common\BoardGames.h"
//---------------------------------------------------------------------------
clHandValue::clHandValue()
{
    _param1=DPHV_UNDEF;
    _param2=DPHV2_UNDEF;
}
//---------------------------------------------------------------------------
void    clHandValue::ChangeBoard(clCard *board,int cnBoard)
{
    clCard card0;
    int nb;
    for(int i=0;i<cnBoard;i++)
    {
        card0=board[i];
        nb=i;
        for(int j=i;j<cnBoard;j++)
        {
            if(board[j].face<card0.face)
            {
                nb=j;
                card0=board[j];
            }
        }
        board[nb]=board[i];
        board[i]=card0;
    }
}
//---------------------------------------------------------------------------
enFace    clHandValue::IsStraight(clCard *board,int cnBoard,clCard *cardPl)
{
/*
    bool ch1=false,ch2=false;
    for(int i=0;i<cnBoard;i++)
    {
        if(board[i].face>cardPl[0].face && !ch1)
        {
            for(int j=cnBoard-1;j>i;j--)
                board[i]=board[i-1];
            cnBoard++;
            board[i]=cardPl[0];
            if(ch2) break;
            ch1=true;
        }
        if(board[i].face>cardPl[1].face && !ch2)
        {
            for(int j=cnBoard-1;j>i;j--)
                board[i]=board[i-1];
            cnBoard++;
            board[i]=cardPl[1];
            if(ch1) break;
            ch2=true;
        }
    }
*/
    bool arrFace[CN_CARD_FACE];
    for(int i=0;i<CN_CARD_FACE;i++)
        arrFace[i]=false;
    for(int i=0;i<cnBoard;i++)
        arrFace[(int)board[i].face]=true;
    for(int i=0;i<2;i++)
        arrFace[(int)cardPl[i].face]=true;

    for(int i=CN_CARD_FACE-1;i>0;i--)
    {
        if(i>3)
        {
            int j;
            for(j=0;j<5;j++)
                if(!arrFace[i-j]) break;
            if(j==5)
                return((enFace)i);
            else
                i-=j;
        }
        else if(i==3)
        {
            for(int j=0;j<4;j++)
                if(!arrFace[i-j])
                    return(C_UNDEF);
            if(!arrFace[CN_CARD_FACE-1])
                return(C_UNDEF);
            return(C_5);
        }
        else
            return(C_UNDEF);
    }
    return(C_UNDEF);
}
//---------------------------------------------------------------------------
void    clHandValue::InitHV(clCard *cardPl,clCard *cardT,int cnCardT)
{
    Init(cardPl,cardT,cnCardT);
    InitFlushDraw(cardPl,cardT,cnCardT);
    InitStraightDraw(cardPl,cardT,cnCardT);
}
//---------------------------------------------------------------------------
void    clHandValue::SearchCards(clCard *board,int cnBoard,int &biggerCard,int &secondCard)
{
    clCard card0=board[0];
    biggerCard=0;
    for(int i=0;i<cnBoard;i++)
        if(card0.face<board[i].face)
        {
            card0=board[i];
            biggerCard=i;
        }
    clCard card1;card1.face=C_2;card1.suit=S_C;
    for(int i=0;i<cnBoard;i++)
        if(board[i].face!=card0.face)
            if(board[i].face>card1.face)
            {
                card1=board[i];
                secondCard=i;
            }
}
void    clHandValue::SearchCardsNoCard(clCard *board,int cnBoard,enFace face,int &biggerCard,int &secondCard)
{
    clCard card0;card0.face=C_2;card0.suit=S_C;
    biggerCard=0;
    for(int i=0;i<cnBoard;i++)
        if(card0.face<board[i].face && face!=board[i].face)
        {
            card0=board[i];
            biggerCard=i;
        }
    secondCard=-1;
    clCard card1;card1.face=C_2;card1.suit=S_C;
    for(int i=0;i<cnBoard;i++)
        if(board[i].face!=card0.face)
            if(board[i].face>card1.face && face!=board[i].face)
            {
                card1=board[i];
                secondCard=i;
            }
}
//---------------------------------------------------------------------------
void    clHandValue::Init(clCard *plCards,clCard *board,int cnC)
{

    _secondP=PHV2_UNDEF;
    if(cnC<3 || cnC>5)
    {
        _firstP=PHV1_UNDEF;
        return;
    }
    bool chCard=false;
    if(plCards[0].face<plCards[1].face)
    {
        chCard=true;
        clCard card0=plCards[0];
        plCards[0]=plCards[1];
        plCards[1]=card0;
    }
    _game.GameHoldem(plCards,board,cnC);
//    _game=g.game;
    switch(_game.game)
    {
        case GM_0  :
        {
            int cnOver=0;
            for(int i=0;i<2;i++)
                for(int j=0;j<cnC;j++)
                    if(board[j].face>plCards[i].face)
                    {
                        cnOver++;break;
                    }
            switch (cnOver)
            {
                case 0 : _firstP=PHV1_TWO_OVER; break;
                case 1 : _firstP=PHV1_ONE_OVER; break;
                case 2 : _firstP=PHV1_NO_OVER; break;
            }
            break;
        }
        case GM_2  :
        {
            if(plCards[0].face==plCards[1].face)
            {
                _firstP=PHV1_POCKET_PAIR;
                int cnOver=0;
                for(int j=0;j<cnC;j++)
                    if(board[j].face>plCards[0].face)
                        cnOver++;

                switch (cnOver)
                {
                    case 0  : _secondP=PHV2_OVER_PAIR; break;
                    case 1  : _secondP=PHV2_SECOND_PAIR; break;
                    default : _secondP=PHV2_LOW_PAIR;
                }
            }
            else
            {
                int nbP,nbKick=-1;
                if(_game.face[0]==plCards[0].face)
                {
                    nbP=0;nbKick=1;
                }
                else if(_game.face[0]==plCards[1].face)
                {
                    nbP=1;nbKick=0;
                }
                else
                {
                    _firstP=PHV1_PAIRED_BOARD;
                    int cnOver=0;
                    for(int i=0;i<2;i++)
                        for(int j=0;j<cnC;j++)
                            if(board[j].face>plCards[i].face)
                            {
                                cnOver++;break;
                            }
                    switch (cnOver)
                    {
                        case 0 : _secondP=PHV2_TWO_OVER;break;
                        case 1 : _secondP=PHV2_ONE_OVER;break;
                        default : _secondP=PHV2_NO_OVER;break;
                    }
                }
                if(nbKick!=-1)
                {
                    int cnOver=0;
                    for(int j=0;j<cnC;j++)
                        if(board[j].face>plCards[nbP].face)
                            cnOver++;
                    switch(cnOver)
                    {
                        case 0:
                            _firstP=PHV1_TOP_PAIR;
                            if(plCards[nbP].face==C_A)
                            {
                                if(plCards[nbKick].face==C_K)
                                    _secondP=PHV2_TOP_KICKER;
                                else if(plCards[nbKick].face==C_Q || plCards[nbKick].face==C_J || plCards[nbKick].face==C_T)
                                    _secondP=PHV2_GOOD_KICKER;
                                else
                                    _secondP=PHV2_WEAK_KICKER;
                            }
                            else
                            {
                                if(plCards[nbKick].face==C_A)
                                    _secondP=PHV2_TOP_KICKER;
                                else if(plCards[nbKick].face==C_Q || plCards[nbKick].face==C_J || plCards[nbKick].face==C_T|| plCards[nbKick].face==C_K)
                                    _secondP=PHV2_GOOD_KICKER;
                                else
                                    _secondP=PHV2_WEAK_KICKER;
                            }break;
                        case 1:
                            _firstP=PHV1_SECOND_PAIR;
                            if(plCards[nbKick].face==C_A)
                                _secondP=PHV2_ACE_KICKER;
                            else
                                _secondP=PHV2_NOT_ACE_KICKER;
                            break;
                        default : _firstP=PHV1_BOTTOM_PAIR;
                            if(plCards[nbKick].face==C_A)
                                _secondP=PHV2_ACE_KICKER;
                            else
                                _secondP=PHV2_NOT_ACE_KICKER;
                            break;
                    }
                }
            }
            break;
        }
        case GM_22 :
        {
            if((_game.face[0]==plCards[0].face || _game.face[0]==plCards[1].face) && (_game.face[1]==plCards[0].face || _game.face[1]==plCards[1].face))
            {
                _firstP=PHV1_BOTH_CARD_PAIRED;
                int topC,secC;
                SearchCards(board,cnC,topC,secC);
                if(plCards[0].face==board[topC].face)
                {
                    if(plCards[1].face==board[secC].face)
                        _secondP=PHV2_TOP_TWO_PAIR;
                    else
                        _secondP=PHV2_TOP_PAIR_AND_PAIR;
                }
                else
                    _secondP=PHV2_MIDDLE_PAIR_AND_BOTTOM;
            }
            else if(plCards[0].face==plCards[1].face)
            {
                if(plCards[0].face!=_game.face[0] && plCards[0].face!=_game.face[1])
                    _firstP=PHV1_TWO_PAIR_ON_BOARD;
                else
                {
                    _firstP=PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD;
                    int i;
                    for(i=0;i<cnC;i++)
                        if(board[i].face>plCards[0].face) break;
                    if(i==cnC)
                        _secondP=PHV2_2P_OVER_PAIR;
                    else if(plCards[0].face==_game.face[0])
                        _secondP=PHV2_2P_BIGGER_PAIR_ON_BOARD;
                    else
                        _secondP=PHV2_2P_LESS_PAIR_ON_BOARD;
                }
            }
            else  if(plCards[0].face!=_game.face[0] && plCards[0].face!=_game.face[1] && plCards[1].face!=_game.face[0] && plCards[1].face!=_game.face[1])
            {
                _firstP=PHV1_TWO_PAIR_ON_BOARD;
            }
            else
            {
                _firstP=PHV1_PAIR_AND_PAIR_ON_BOARD;
                int topC,secC;
                if(_game.face[0]!=plCards[0].face && _game.face[0] !=plCards[1].face)
                    SearchCardsNoCard(board,cnC,_game.face[0],topC,secC);
                else
                    SearchCardsNoCard(board,cnC,_game.face[1],topC,secC);
                if(board[topC].face==plCards[0].face || board[topC].face==plCards[1].face)
                    _secondP=PHV2_2P_TOP_PAIR;
                else if(board[secC].face==plCards[0].face || board[secC].face==plCards[1].face)
                    _secondP=PHV2_2P_SECOND_PAIR;
                else
                    _secondP=PHV2_2P_BOTTOM_PAIR;
            }
            break;
        }
        case GM_3  :
        {
            if(plCards[0].face==plCards[1].face)
            {
                _firstP=PHV1_SET;
                int topC,secC;
                SearchCards(board,cnC,topC,secC);
                if(board[topC].face==plCards[0].face)
                    _secondP=PHV2_HIGH_SET;
                else if(board[secC].face==plCards[0].face)
                    _secondP=PHV2_SECOND_SET;
                else
                    _secondP=PHV2_LOW_SET;
            }
            else if(_game.face[0]==plCards[0].face || _game.face[0]==plCards[1].face)
            {
                _firstP=PHV1_TRIPS;
                int cnOver=0;
                for(int j=0;j<cnC;j++)
                    if(board[j].face>_game.face[0])
                        cnOver++;
                int nbK;
                if(_game.face[0]==plCards[0].face)
                    nbK=1;
                else
                    nbK=0;
                switch(cnOver)
                {
                    case 0:
                        if(plCards[nbK].face==C_A || plCards[nbK].face==C_K || plCards[nbK].face==C_Q)
                            _secondP=PHV2_HIGH_TR_HIGH_KICKER;
                        else
                            _secondP=PHV2_HIGH_TR_LOW_KICKER;
                        break;
                    case 1:
                        if(plCards[nbK].face==C_A || plCards[nbK].face==C_K || plCards[nbK].face==C_Q)
                            _secondP=PHV2_SECOND_TR_HIGH_KICKER;
                        else
                            _secondP=PHV2_SECOND_TR_LOW_KICKER;
                        break;
                    default:
                        if(plCards[nbK].face==C_A || plCards[nbK].face==C_K || plCards[nbK].face==C_Q)
                            _secondP=PHV2_LOW_TR_HIGH_KICKER;
                        else
                            _secondP=PHV2_LOW_TR_LOW_KICKER;
                }
            }
            else
                _firstP=PHV1_TRIPS_ON_BOARD;

            break;
        }
        case GM_ST :
        {
            enFace faceStreet[5]={C_5,C_4,C_3,C_2,C_A};
            if(_game.face[0]!=C_5)
            {
                faceStreet[0]=_game.face[0];
                for(int i=1;i<5;i++)
                    faceStreet[i] = (enFace)(_game.face[0]-(enFace)i);
            }
            int cnOv=0;
            for(int i=0;i<5;i++)
                if(faceStreet[i]==plCards[0].face || faceStreet[i]==plCards[1].face)
                    cnOv++;
            for(int i=0;i<cnC;i++)
                if(board[i].face==plCards[0].face || board[i].face==plCards[1].face)
                    cnOv--;
            if(cnOv==0)
            {
                _firstP=PHV1_STRAIGHT_ON_BOARD;
            }
            bool d=true;
            for(int i=0;i<CN_CARD_FACE;i++)
                if(d)
                    for(int j=i+1;j<CN_CARD_FACE;j++)
                    {
                        clCard cardT[2];cardT[0].face=(enFace)i;cardT[1].face=(enFace)j;
                        enFace f=IsStraight(board,cnC,cardT);
                        if(f!=C_UNDEF && f>_game.face[0])
                        {
                            if(cnOv==2)
                                _firstP=PHV1_TWO_CARDS_STRAIGHT;
                            else
                                _firstP=PHV1_ONE_CARD_STRAIGHT;
                            d=false;break;
                        }
                    }
                else break;
            if(d)
            {
                if(cnOv==2)
                    _firstP=PHV1_TWO_CARDS_NUTS_STRAIGHT;
                else
                    _firstP=PHV1_ONE_CARD_NUTS_STRAIGHT;
            }
            break;
        }
        case GM_FL :
        {
            int arrFl[CN_CARD_SUIT];
            for(int i=0;i<CN_CARD_SUIT;i++)
                arrFl[i]=0;
            for(int i=0;i<cnC;i++)
                arrFl[(int)board[i].suit]++;
            enSuit fl;
            int cnCardFlBoard=0;
            for(int i=0;i<CN_CARD_SUIT;i++)
                if(arrFl[i]>=3)
                {
                    cnCardFlBoard=arrFl[i];
                    fl=(enSuit)i;break;
                }

            clCard cardToFl;cardToFl.face=C_UNDEF;cardToFl.suit=S_UNDEF;
            if(plCards[0].suit==fl)
                cardToFl=plCards[0];
            if(plCards[1].suit==fl)
                if(cardToFl.face==C_UNDEF)
                    cardToFl=plCards[1];
                else if(cardToFl.face<plCards[1].face)
                    cardToFl=plCards[1];

            switch(cnCardFlBoard)
            {
                case 3:
                    if(cardToFl.face>=C_T)
                    {
                        int cnOvFl=0;
                        for(int i=0;i<cnC;i++)
                            if(board[i].suit==fl && board[i].face>cardToFl.face)
                                cnOvFl++;
                        if(cardToFl.face+cnOvFl==C_A)
                            _firstP=PHV1_3CARD_NUTS_FLUSH;
                        else
                            _firstP=PHV1_3CARD_HIGH_FLUSH;
                    }
                    else
                        _firstP=PHV1_3CARD_LOW_FLUSH;
                    break;
                case 4:
                    if(cardToFl.face>=C_T)
                    {
                        int cnOvFl=0;
                        for(int i=0;i<cnC;i++)
                            if(board[i].suit==fl && board[i].face>cardToFl.face)
                                cnOvFl++;
                        if(cardToFl.face+cnOvFl==C_A)
                            _firstP=PHV1_4CARD_NUTS_FLUSH;
                        else
                            _firstP=PHV1_4CARD_HIGH_FLUSH;
                    }
                    else
                        _firstP=PHV1_4CARD_LOW_FLUSH;
                    break;
                case 5:
                    if(cardToFl.face==C_UNDEF)
                        _firstP=PHV1_FLUSH_ON_BOARD_LOW;
                    else
                    {
                        if(cardToFl.face>=C_T)
                        {
                            int cnOvFl=0;
                            for(int i=0;i<cnC;i++)
                                if(board[i].suit==fl && board[i].face>cardToFl.face)
                                    cnOvFl++;
                            if(cardToFl.face+cnOvFl==C_A)
                                _firstP=PHV1_FLUSH_ON_BOARD_NUTS;
                            else
                                _firstP=PHV1_FLUSH_ON_BOARD_HIGH;
                        }
                        else
                            _firstP=PHV1_FLUSH_ON_BOARD_LOW;
                    }
                    break;
            }
/*
            int cnCard=0;
            for(int i=0;i<5;i++)
                if((plCards[0].suit==fl && plCards[0].face==g.face[i]) || (plCards[1].suit==fl && plCards[1].face==g.face[i]))
                    cnCard++;
            if(cnCard>0)
            {
                clCard cardToFl;
                if(cnCard==2)
                    if(plCards[0].face>plCards[1].face)
                        cardToFl=plCards[0];
                    else
                        cardToFl=plCards[1];
                if(cardToFl.face>=C_T)
                {

                }
                else
                {
                }
            }
*/
            break;
        }
        case GM_32 :
        {
            bool arrS[5];
            enFace arrFG[5]={_game.face[0],_game.face[0],_game.face[0],_game.face[1],_game.face[1]};
            for(int i=0;i<5;i++)
                arrS[i]=true;
            int cnOurC=5;
            for(int i=0;i<cnC;i++)
                for(int j=0;j<5;j++)
                    if(arrS[j])
                        if(arrFG[j]==board[i].face)
                        {
                            arrS[j]=false;
                            cnOurC--;
                            break;
                        }
            int cn=0;
            for(int i=0;i<cnC;i++)
                if(board[i].face==_game.face[0])
                    cn++;
            if(cnOurC==2)
            {
                if(plCards[0].face==plCards[1].face)
                    if(cn==3)
                    {
                        _secondP=PHV2_POCKET_PAIR_AND_TRIPS_ON_B;
                        if(plCards[0].face==C_A || (plCards[0].face==C_K && _game.face[0]==C_A))
                            _firstP=PHV1_FULL_2CARD_NUTS;
                        else
                        {
                            int cnOver=0;
                            for(int j=0;j<cnC;j++)
                                if(board[j].face>_game.face[1] && board[j].face!=_game.face[0])
                                    cnOver++;
                            if(cnOver==0)
                                _firstP=PHV1_FULL_2CARD_GOOD;
                            else
                                _firstP=PHV1_FULL_2CARD_BAD;
                        }
                    }
                    else
                    {
                        int cnOver=0;
                        for(int j=0;j<cnC;j++)
                            if(board[j].face>_game.face[0])
                                cnOver++;
                        if(cnOver==0 && _game.face[0]>_game.face[1])
                            _firstP=PHV1_FULL_2CARD_NUTS;
                        else if (_game.face[0]>_game.face[1])
                            _firstP=PHV1_FULL_2CARD_GOOD;
                        else
                        {
                            clBoard bd;bd.Init(board,cnC);
                            if(bd._pairB!=DGP_2_2)
                                _firstP=PHV1_FULL_2CARD_GOOD;
                            else
                                _firstP=PHV1_FULL_2CARD_BAD;
                        }
                        _secondP=PHV2_POCKET_PAIR_NO_TRIPS_ON_B;
                    }
                else
                {
                    int cnOver3=0;
                    for(int j=0;j<cnC;j++)
                        if(board[j].face>_game.face[0])
                            cnOver3++;
                    int cnOver=0;
                    for(int j=0;j<cnC;j++)
                        if(board[j].face>_game.face[1] && board[j].face!=_game.face[0])
                            cnOver++;
                    if(cnOver==0 && cnOver3==0)
                        _firstP=PHV1_FULL_2CARD_NUTS;
                    else
                        _firstP=PHV1_FULL_2CARD_GOOD;
                    _secondP=PHV2_NO_P_NO_TRIPS;
                }
            }
            else if(cnOurC==1)
            {
                if(cn==3)
                {
                    int cnOver=0;
                    for(int j=0;j<cnC;j++)
                        if(board[j].face>_game.face[1] && board[j].face!=_game.face[0])
                            cnOver++;
                    if(_game.face[0]==C_A && _game.face[1]==C_K)
                        _firstP=PHV1_FULL_1CARD_NUTS;
                    else if(cnOver==0)
                        _firstP=PHV1_FULL_1CARD_GOOD;
                    else
                        _firstP=PHV1_FULL_2CARD_BAD;
                    _secondP=PHV2_FULL_TRIPS_ON_BOARD;
                }
                else if(_game.face[0]>_game.face[1])
                {

                    int cnOver=0;
                    for(int j=0;j<cnC;j++)
                        if(board[j].face>_game.face[1] && board[j].face!=_game.face[0])
                            cnOver++;
                    if(cnOver==0)
                        _firstP=PHV1_FULL_1CARD_NUTS;
                    else
                        _firstP=PHV1_FULL_1CARD_GOOD;
                    _secondP=PHV2_FULL_TOP_PAIR_NO_TR;
                }
                else
                {
                    _firstP=PHV1_FULL_2CARD_BAD;
                    _secondP=PHV2_FULL_BOTTOB_PAIR_NO_TR;
                }
            }
            else
                _firstP=PHV1_FULL_ON_BOARD;
            break;
        }
        case GM_4  :
        {
            if(plCards[1].face==plCards[0].face && plCards[0].face==_game.face[0])
            {
                _firstP=PHV1_SQUARE_WITH_POCKET;
            }
            else if(plCards[0].face==_game.face[0] || plCards[1].face==_game.face[0])
                _firstP=PHV1_SQUARE_WITHOUT_POCKET;
            else
                _firstP=PHV1_SQUARE_ON_BOARD;
            break;
        }
        case GM_FS :
        {
            int arrFl[CN_CARD_SUIT];
            for(int i=0;i<CN_CARD_SUIT;i++)
                arrFl[i]=0;
            for(int i=0;i<cnC;i++)
                arrFl[(int)board[i].suit]++;
            enSuit fl;
            for(int i=0;i<CN_CARD_SUIT;i++)
                if(arrFl[i]>=3)
                {
                    fl=(enSuit)i;break;
                }
            int cnCardPl=0;
            for(int i=0;i<2;i++)
                if(plCards[i].suit==fl)
                    if(_game.face[0]-plCards[i].face<5)
                        cnCardPl++;
            if(cnCardPl==2)
                _firstP=PHV1_STRAIGHT_FLUSH_2CARD;
            else if(cnCardPl==1)
                _firstP=PHV1_STRAIGHT_FLUSH_1CARD;
            else
                _firstP=PHV1_STRAIGHT_FLUSH_ON_BOARD;

            break;
        }
    }
    if(chCard)
    {
        clCard card0=plCards[0];
        plCards[0]=plCards[1];
        plCards[1]=card0;
    }
}
//---------------------------------------------------------------------------
CString clHandValue::FirstParamStr()
{
    switch(_firstP)
    {
        case PHV1_TWO_OVER                      : return("PHV1_TWO_OVER");
        case PHV1_ONE_OVER                      : return("PHV1_ONE_OVER");
        case PHV1_NO_OVER                       : return("PHV1_NO_OVER");
        case PHV1_POCKET_PAIR                   : return("PHV1_POCKET_PAIR");
        case PHV1_TOP_PAIR                      : return("PHV1_TOP_PAIR");
        case PHV1_SECOND_PAIR                   : return("PHV1_SECOND_PAIR");
        case PHV1_BOTTOM_PAIR                   : return("PHV1_BOTTOM_PAIR");
        case PHV1_PAIRED_BOARD                  : return("PHV1_PAIRED_BOARD");
        case PHV1_BOTH_CARD_PAIRED              : return("PHV1_BOTH_CARD_PAIRED");
        case PHV1_PAIR_AND_PAIR_ON_BOARD        : return("PHV1_PAIR_AND_PAIR_ON_BOARD");
        case PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD : return("PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD");
        case PHV1_TWO_PAIR_ON_BOARD             : return("PHV1_TWO_PAIR_ON_BOARD");
        case PHV1_SET                           : return("PHV1_SET");
        case PHV1_TRIPS                         : return("PHV1_TRIPS");
        case PHV1_TRIPS_ON_BOARD                : return("PHV1_TRIPS_ON_BOARD");
        case PHV1_TWO_CARDS_NUTS_STRAIGHT       : return("PHV1_TWO_CARDS_NUTS_STRAIGHT");
        case PHV1_TWO_CARDS_STRAIGHT            : return("PHV1_TWO_CARDS_STRAIGHT");
        case PHV1_ONE_CARD_NUTS_STRAIGHT        : return("PHV1_ONE_CARD_NUTS_STRAIGHT");
        case PHV1_ONE_CARD_STRAIGHT             : return("PHV1_ONE_CARD_STRAIGHT");
        case PHV1_STRAIGHT_ON_BOARD             : return("PHV1_STRAIGHT_ON_BOARD");
        case PHV1_3CARD_NUTS_FLUSH              : return("PHV1_3CARD_NUTS_FLUSH");
        case PHV1_3CARD_HIGH_FLUSH              : return("PHV1_3CARD_HIGH_FLUSH");
        case PHV1_3CARD_LOW_FLUSH               : return("PHV1_3CARD_LOW_FLUSH");
        case PHV1_4CARD_NUTS_FLUSH              : return("PHV1_4CARD_NUTS_FLUSH");
        case PHV1_4CARD_HIGH_FLUSH              : return("PHV1_4CARD_HIGH_FLUSH");
        case PHV1_4CARD_LOW_FLUSH               : return("PHV1_4CARD_LOW_FLUSH");
        case PHV1_FLUSH_ON_BOARD_NUTS           : return("PHV1_FLUSH_ON_BOARD_NUTS");
        case PHV1_FLUSH_ON_BOARD_HIGH           : return("PHV1_FLUSH_ON_BOARD_HIGH");
        case PHV1_FLUSH_ON_BOARD_LOW            : return("PHV1_FLUSH_ON_BOARD_LOW");
        case PHV1_FULL_2CARD_GOOD               : return("PHV1_FULL_2CARD_GOOD");
        case PHV1_FULL_1CARD_GOOD               : return("PHV1_FULL_1CARD_GOOD");
        case PHV1_FULL_2CARD_NUTS               : return("PHV1_FULL_2CARD_NUTS");
        case PHV1_FULL_1CARD_NUTS               : return("PHV1_FULL_1CARD_NUTS");
        case PHV1_FULL_2CARD_BAD                : return("PHV1_FULL_2CARD_BAD");
        case PHV1_FULL_1CARD_BAD                : return("PHV1_FULL_1CARD_BAD");
        case PHV1_FULL_ON_BOARD                 : return("PHV1_FULL_ON_BOARD");
        case PHV1_SQUARE_WITH_POCKET            : return("PHV1_SQUARE_WITH_POCKET");
        case PHV1_SQUARE_WITHOUT_POCKET         : return("PHV1_SQUARE_WITHOUT_POCKET");
        case PHV1_SQUARE_ON_BOARD               : return("PHV1_SQUARE_ON_BOARD");
        case PHV1_STRAIGHT_FLUSH_2CARD          : return("PHV1_STRAIGHT_FLUSH_2CARD");
        case PHV1_STRAIGHT_FLUSH_1CARD          : return("PHV1_STRAIGHT_FLUSH_1CARD");
        case PHV1_STRAIGHT_FLUSH_ON_BOARD       : return("PHV1_STRAIGHT_FLUSH_ON_BOARD");
        default                                 : return("PHV1_UNDEF");
    }
}
//-------------------------------------------------------------------------
CString clHandValue::SecondParamStr()
{
    switch(_secondP)
    {
        case PHV2_OVER_PAIR                   :  return("PHV2_OVER_PAIR");
        case PHV2_SECOND_PAIR                 :  return("PHV2_SECOND_PAIR");
        case PHV2_LOW_PAIR                    :  return("PHV2_LOW_PAIR");
        case PHV2_TOP_KICKER                  :  return("PHV2_TOP_KICKER");
        case PHV2_GOOD_KICKER                 :  return("PHV2_GOOD_KICKER");
        case PHV2_WEAK_KICKER                 :  return("PHV2_WEAK_KICKER");
        case PHV2_ACE_KICKER                  :  return("PHV2_ACE_KICKER");
        case PHV2_NOT_ACE_KICKER              :  return("PHV2_NOT_ACE_KICKER");
        case PHV2_TWO_OVER                    :  return("PHV2_TWO_OVER");
        case PHV2_ONE_OVER                    :  return("PHV2_ONE_OVER");
        case PHV2_NO_OVER                     :  return("PHV2_NO_OVER");
        case PHV2_TOP_TWO_PAIR                :  return("PHV2_TOP_TWO_PAIR");
        case PHV2_TOP_PAIR_AND_PAIR           :  return("PHV2_TOP_PAIR_AND_PAIR");
        case PHV2_MIDDLE_PAIR_AND_BOTTOM      :  return("PHV2_MIDDLE_PAIR_AND_BOTTOM");
        case PHV2_2P_TOP_PAIR                 :  return("PHV2_2P_TOP_PAIR");
        case PHV2_2P_SECOND_PAIR              :  return("PHV2_2P_SECOND_PAIR");
        case PHV2_2P_BOTTOM_PAIR              :  return("PHV2_2P_BOTTOM_PAIR");
        case PHV2_2P_OVER_PAIR                :  return("PHV2_2P_OVER_PAIR");
        case PHV2_2P_BIGGER_PAIR_ON_BOARD     :  return("PHV2_2P_BIGGER_PAIR_ON_BOARD");
        case PHV2_2P_LESS_PAIR_ON_BOARD       :  return("PHV2_2P_LESS_PAIR_ON_BOARD");
        case PHV2_HIGH_SET                    :  return("PHV2_HIGH_SET");
        case PHV2_SECOND_SET                  :  return("PHV2_SECOND_SET");
        case PHV2_LOW_SET                     :  return("PHV2_LOW_SET");
        case PHV2_HIGH_TR_HIGH_KICKER         :  return("PHV2_HIGH_TR_HIGH_KICKER");
        case PHV2_HIGH_TR_LOW_KICKER          :  return("PHV2_HIGH_TR_LOW_KICKER");
        case PHV2_SECOND_TR_HIGH_KICKER       :  return("PHV2_SECOND_TR_HIGH_KICKER");
        case PHV2_SECOND_TR_LOW_KICKER        :  return("PHV2_SECOND_TR_LOW_KICKER");
        case PHV2_LOW_TR_HIGH_KICKER          :  return("PHV2_LOW_TR_HIGH_KICKER");
        case PHV2_LOW_TR_LOW_KICKER           :  return("PHV2_LOW_TR_LOW_KICKER");
        case PHV2_POCKET_PAIR_NO_TRIPS_ON_B   :  return("PHV2_POCKET_PAIR_NO_TRIPS_ON_B");
        case PHV2_POCKET_PAIR_AND_TRIPS_ON_B  :  return("PHV2_POCKET_PAIR_AND_TRIPS_ON_B");
        case PHV2_NO_P_NO_TRIPS               :  return("PHV2_NO_P_NO_TRIPS");
        case PHV2_FULL_TRIPS_ON_BOARD         :  return("PHV2_FULL_TRIPS_ON_BOARD");
        case PHV2_FULL_TOP_PAIR_NO_TR         :  return("PHV2_FULL_TOP_PAIR_NO_TR");
        case PHV2_FULL_BOTTOB_PAIR_NO_TR      :  return("PHV2_FULL_BOTTOB_PAIR_NO_TR");
        default                               :  return("PHV2_UNDEF");
    }
}
//---------------------------------------------------------------------------
CString clHandValue::Param1()
{
    switch (_param1)
    {
        case DPHV_2CARD_NUTS_FLDR           :  return("DPHV_2CARD_NUTS_FLDR");
        case DPHV_2CARD_HIGH_FLDR           :  return("DPHV_2CARD_HIGH_FLDR");
        case DPHV_2CARD_LOW_FLDR            :  return("DPHV_2CARD_LOW_FLDR");
        case DPHV_1CARD_NUTS_FLDR           :  return("DPHV_1CARD_NUTS_FLDR");
        case DPHV_1CARD_HIGH_FLDR           :  return("DPHV_1CARD_HIGH_FLDR");
        case DPHV_1CARD_LOW_FLDR            :  return("DPHV_1CARD_LOW_FLDR");
        case DPHV_2CARD_NUTS_BACKDOOR_FLDR  :  return("DPHV_2CARD_NUTS_BACKDOOR_FLDR");
        case DPHV_2CARD_HIGH_BACKDOOR_FLDR  :  return("DPHV_2CARD_HIGH_BACKDOOR_FLDR");
        case DPHV_1CARD_NUTS_BACKDOOR_FLDR  :  return("DPHV_1CARD_NUTS_BACKDOOR_FLDR");
        case DPHV_1CARD_HIGH_BACKDOOR_FLDR  :  return("DPHV_1CARD_HIGH_BACKDOOR_FLDR");
        case DPHV_FLUSH                     :  return("DPHV_FLUSH");
        default                             :  return("DPHV_NO_FLUSH_DRAW");
    }
}
//---------------------------------------------------------------------------
CString clHandValue::Param2()
{
    switch(_param2)
    {
        case DPHV2_2CARD_STR_DRAW   :return("DPHV2_2CARD_STR_DRAW");
        case DPHV2_2CARD_GTSH_DRAW  :return("DPHV2_2CARD_GTSH_DRAW");
        case DPHV2_1CARD_STR_DRAW   :return("DPHV2_1CARD_STR_DRAW");
        case DPHV2_1CARD_GTSH_DRAW  :return("DPHV2_1CARD_GTSH_DRAW");
        default   :                  return("DPHV2_NO_STR_DRAW");
    }
}
//---------------------------------------------------------------------------
void    clHandValue::InitFlushDraw(clCard *plCards,clCard *board,int cnC)
{
    _param1=DPHV_NO_FLUSH_DRAW;
    if(cnC<3 || cnC>4)
        return;
    int arrFl[CN_CARD_SUIT];
    for(int i=0;i<CN_CARD_SUIT;i++)
        arrFl[i]=0;
    for(int i=0;i<cnC;i++)
        arrFl[(int)board[i].suit]++;
    for(int i=0;i<2;i++)
        arrFl[(int)plCards[i].suit]++;
    enSuit fl, flB;
    int cnCardFlBoard=0;
    for(int i=0;i<CN_CARD_SUIT;i++)
    if(arrFl[i]>=4)
    {
        cnCardFlBoard=arrFl[i];
        fl=(enSuit)i;break;
    }
	else if (arrFl[i] == 3)
	{
		cnCardFlBoard = arrFl[i];
		flB = (enSuit)i;
	}
    if(cnCardFlBoard==5)
        _param1=DPHV_FLUSH;
    else if(cnCardFlBoard==4)
    {
        int cnPlCardsFl=0;
        clCard cardToFl;cardToFl.face=C_UNDEF;cardToFl.suit=S_UNDEF;
        if(plCards[0].suit==fl)
        {
            cnPlCardsFl++;
            cardToFl=plCards[0];
        }
        if(plCards[1].suit==fl)
        {
            cnPlCardsFl++;
            if(cardToFl.face==C_UNDEF)
                cardToFl=plCards[1];
            else if(cardToFl.face<plCards[1].face)
                cardToFl=plCards[1];
        }
        if(cardToFl.face==C_UNDEF)
            _param1=DPHV_NO_FLUSH_DRAW;
        else
        {
            if(cardToFl.face>=C_T)
            {
                int cnOvFl=0;
                for(int i=0;i<cnC;i++)
                    if(board[i].suit==fl && board[i].face>cardToFl.face)
                        cnOvFl++;
                    if(cardToFl.face+cnOvFl==C_A)
                        if(cnPlCardsFl==2)
                            _param1=DPHV_2CARD_NUTS_FLDR;
                        else
                            _param1=DPHV_1CARD_NUTS_FLDR;
                    else
                        if(cnPlCardsFl==2)
                            _param1=DPHV_2CARD_HIGH_FLDR;
                        else
                            _param1=DPHV_1CARD_HIGH_FLDR;
            }
            else
            {
                if(cnPlCardsFl==2)
                    _param1=DPHV_2CARD_LOW_FLDR;
                else
                    _param1=DPHV_1CARD_LOW_FLDR;
            }
        }

    }
	else if (cnCardFlBoard == 3 && cnC == 3)
	{
		int cnPlCardsFl = 0;
		clCard cardToFl; cardToFl.face = C_UNDEF; cardToFl.suit = S_UNDEF;
		if (plCards[0].suit == flB)
		{
			cnPlCardsFl++;
			cardToFl = plCards[0];
		}
		if (plCards[1].suit == flB)
		{
			cnPlCardsFl++;
			if (cardToFl.face == C_UNDEF)
				cardToFl = plCards[1];
			else if (cardToFl.face<plCards[1].face)
				cardToFl = plCards[1];
		}
		if (cardToFl.face == C_UNDEF)
			_param1 = DPHV_NO_FLUSH_DRAW;
		else
		{
			if (cnPlCardsFl == 2)
				_param1 = DPHV_2CARD_HIGH_BACKDOOR_FLDR;
			else
				_param1 = DPHV_1CARD_HIGH_BACKDOOR_FLDR;
		}
	}
}
//---------------------------------------------------------------------------
int     clHandValue::IsStraight(bool *arrFace,enFace face,enFace pl1,enFace pl2,bool *arrFaceBoard)
{
    int f=(int)face;
    if(arrFace[f]) return(-1);
    if(pl1!=C_UNDEF)
        if(arrFaceBoard[(int)pl1])
            pl1=C_UNDEF;
    if(pl2!=C_UNDEF)
        if(arrFaceBoard[(int)pl2])
            pl2=C_UNDEF;
    if(face==C_A || face<=C_5)
    {
        arrFace[f]=true;
        int i,cnSC=0;
        for(i=0;i<4;i++)
            if(arrFace[i])
            {
                if(i==(int)pl1 || i==(int)pl2)
                    cnSC++;
            }
            else break;
        if(i==4 && arrFace[C_A])
        {
            if((C_A==(int)pl1 || C_A==(int)pl2) && !arrFace[4])
                cnSC++;
            if(arrFace[4] && arrFace[5] && (0==(int)pl1 || 0==(int)pl2))
                cnSC--;
            arrFace[f]=false;
            return(cnSC);
        }
        arrFace[f]=false;
    }
    int cn=1,cnSC=0;
    for(int i=f+1;i<CN_CARD_FACE;i++)
        if(arrFace[i])
        {
            if(i==(int)pl1 || i==(int)pl2)
                cnSC++;
            cn++;
            if(cn==5)
                return(cnSC);
        }
        else break;
    for(int i=f-1;i>=0;i--)
    {
        if(arrFace[i])
        {
            if(i==(int)pl1 || i==(int)pl2)
                cnSC++;
            cn++;
            if(cn==5)
                return(cnSC);
        }
        else
            return(-1);
    }
    return(-1);

}
//---------------------------------------------------------------------------
void    clHandValue::InitStraightDraw(clCard *plCards,clCard *board,int cnC)
{
    if(cnC<3 || cnC>4)
    {
        _param2=DPHV2_NO_STR_DRAW;return;
    }
    bool arrFace[CN_CARD_FACE],arrFaceBoard[CN_CARD_FACE];
    for(int i=0;i<CN_CARD_FACE;i++)
        arrFace[i]=arrFaceBoard[i]=false;
    for(int i=0;i<cnC;i++)
        arrFace[board[i].face]=arrFaceBoard[board[i].face]=true;
    for(int i=0;i<2;i++)
        arrFace[plCards[i].face]=true;
    int cnCardToStraight=0;
    bool cardStr2=false;
    int nbLessC;
    nbLessC=plCards[0].face<plCards[1].face ? 0 : 1;
    for(int i=0;i<CN_CARD_FACE;i++)
    {
        int cnOurC=IsStraight(arrFace,(enFace)i,plCards[0].face,plCards[1].face,arrFaceBoard);
        if(cnOurC>0)
        {
            if(cnOurC==2)
            {
                arrFace[plCards[1-nbLessC].face]=false;
                if(IsStraight(arrFace,(enFace)i,plCards[nbLessC].face,C_UNDEF,arrFaceBoard)>0)
                {
                }
                else
                    cardStr2=true;
                arrFace[plCards[1-nbLessC].face]=true;
            }
            cnCardToStraight++;
        }
    }
    if(cnCardToStraight==2)
        if(cardStr2)
            _param2=DPHV2_2CARD_STR_DRAW;
        else
            _param2=DPHV2_1CARD_STR_DRAW;
    else if(cnCardToStraight==1)
        if(cardStr2)
            _param2=DPHV2_2CARD_GTSH_DRAW;
        else
            _param2=DPHV2_1CARD_GTSH_DRAW;
    else
        _param2=DPHV2_NO_STR_DRAW;
}
//---------------------------------------------------------------------------
bool    clHandValue::IsFlushDraw2Card()
{
    enFlushDrawParamHV_1 val=FlushDraw();
    return (val==DPHV_2CARD_NUTS_FLDR || val==DPHV_2CARD_HIGH_FLDR || val==DPHV_2CARD_LOW_FLDR);
}
//---------------------------------------------------------------------------
bool    clHandValue::IsGoodFlushDraw1Card()
{
    enFlushDrawParamHV_1 val=FlushDraw();
    return (val==DPHV_1CARD_NUTS_FLDR || val==DPHV_1CARD_HIGH_FLDR);
}
//---------------------------------------------------------------------------
//---------------------------clBoard-----------------------------------------
//---------------------------------------------------------------------------
void clBoard::Init(clCard *board,int cnC)
{
    for(int i=0;i<cnC;i++)
        _board[i]=board[i];
    _cnBoard=cnC;
    for(int i=0;i<_cnBoard;i++)
    {
        clCard card0=_board[i];
        int nb=i;
        for(int j=i;j<_cnBoard;j++)
        {
            if(_board[j]>card0)
            {
                nb=j;
                card0=_board[j];
            }
        }
        _board[nb]=_board[i];
        _board[i]=card0;
    }

    {
        _flushB=DGF_NO;
        int arrS[CN_CARD_SUIT];
        for(int i=0;i<CN_CARD_SUIT;i++)
            arrS[i]=0;
        for(int i=0;i<cnC;i++)
            arrS[(int)board[i].suit]++;
        for(int i=0;i<CN_CARD_SUIT;i++)
            if(arrS[i]==2)
                if(_flushB==DGF_2)
                {
                    _flushB=DGF_2_2;break;
                }
                else
                    _flushB=DGF_2;
            else if(arrS[i]==3)
            {
                _flushB=DGF_3;break;
            }
            else if(arrS[i]==4)
            {
                _flushB=DGF_4;break;
            }
    }
    {
        _pairB=DGP_NO;
        int arrF[CN_CARD_FACE];
        for(int i=0;i<CN_CARD_FACE;i++)
            arrF[i]=0;
        for(int i=0;i<cnC;i++)
            arrF[(int)board[i].face]++;
        for(int i=0;i<CN_CARD_FACE;i++)
            if(arrF[i]==2)
                if(_pairB==DGP_2)
                {
                    _pairB=DGP_2_2;break;
                }
                else if(_pairB==DGP_3)
                {
                    _pairB=DGP_3_2;break;
                }
                else
                    _pairB=DGP_2;
            else if(arrF[i]==3)
            {
                if(_pairB==DGP_2)
                {
                    _pairB=DGP_3_2;break;
                }
                else
                    _pairB=DGP_3;
            }
            else if(arrF[i]==4)
            {
                _pairB=DGP_4;break;
            }
    }
    {
        bool arrFace[CN_CARD_FACE];
        for(int i=0;i<CN_CARD_FACE;i++)
            arrFace[i]=false;
        for(int i=0;i<cnC;i++)
            arrFace[board[i].face]=true;
        int cnCardsStr1=0,cnCardsStr1MoreThen=0,cnCardsStr2=0,cnCardsStr2MoreThen=0,cnCardsStr3=0,cnCardsStr3MoreThen=0;
        for(int i=0;i<CN_CARD_FACE;i++)
        {
            if(arrFace[i]) continue;
            if(IsStraight(arrFace,(enFace)i))
            {
                cnCardsStr1++;
                if(i>MIN_CARD_TO_STR_INT)
                    cnCardsStr1MoreThen++;

            }
            if(cnCardsStr1==0)
            {
                arrFace[i]=true;
                for(int j=i+1;j<CN_CARD_FACE;j++)
                {
                    if(arrFace[j]) continue;
                    if(IsStraight(arrFace,(enFace)j))
                    {
                        cnCardsStr2++;
                        if(i>MIN_CARD_TO_STR_INT)
                            cnCardsStr2MoreThen++;
                    }
                    if(cnCardsStr2==0)
                    {
                        arrFace[j]=true;
                        bool searchOne=false;
                        for(int k=0;k<CN_CARD_FACE;k++)
                        {
                            if(arrFace[k]) continue;
                            if(IsStraight(arrFace,(enFace)k))
                            {
                                if(searchOne)
                                {
                                    cnCardsStr3++;
                                    if(i>MIN_CARD_TO_STR_INT)
                                        cnCardsStr3MoreThen++;
                                }
                                searchOne=true;
                            }
                        }
                        arrFace[j]=false;
                    }
                }
                arrFace[i]=false;
            }
        }
        if(cnCardsStr1>0)
        {
            _straightB._straightB=DGS_1;
            _straightB._cnCard=cnCardsStr1;
            _straightB._cnCardMoreThen=cnCardsStr1MoreThen;
        }
        else if(cnCardsStr2>0)
        {
            _straightB._straightB=DGS_2;
            _straightB._cnCard=cnCardsStr2;
            _straightB._cnCardMoreThen=cnCardsStr2MoreThen;
        }
        else if(cnCardsStr3>0)
        {
            _straightB._straightB=DGS_DRAW;
            _straightB._cnCard=cnCardsStr3;
            _straightB._cnCardMoreThen=cnCardsStr3MoreThen;
        }
        else
        {
            _straightB._straightB=DGS_NO;
            _straightB._cnCard=0;
        }

    }

}
//---------------------------------------------------------------------------
enFace   clBoard::BoardHighCard()
{
    enFace face=C_2;
    for(int i=0;i<_cnBoard;i++)
    {
        if(_board[i].face>face)
            face=_board[i].face;
    }
    return(face);
}
//---------------------------------------------------------------------------
bool     clBoard::IsStraight(bool *arrFace,enFace face)
{
    int f=(int)face;
    if(arrFace[f]) return(-1);
    if(face==C_A || face<=C_5)
    {
        arrFace[f]=true;
        int i;
        for(i=0;i<4;i++)
            if(arrFace[i])
            {
            }
            else break;
        if(i==4 && arrFace[C_A])
        {
            arrFace[f]=false;
            return(true);
        }
        arrFace[f]=false;
    }
    int cn=1;
    for(int i=f+1;i<CN_CARD_FACE;i++)
        if(arrFace[i])
        {
            cn++;
            if(cn==5)
                return(true);
        }
        else break;
    for(int i=f-1;i>=0;i--)
    {
        if(arrFace[i])
        {
            cn++;
            if(cn==5)
                return(true);
        }
        else
            return(false);
    }
    return(false);

}
//---------------------------------------------------------------------------
CString clBoard::PairB()
{
   switch(_pairB)
    {
        case DGP_4   :  return("DGP_4");
        case DGP_3_2 :  return("DGP_3_2");
        case DGP_2_2 :  return("DGP_2_2");
        case DGP_3   :  return("DGP_3");
        case DGP_2   :  return("DGP_2");
        default      :  return("DGP_NO");
    }

}
//---------------------------------------------------------------------------
CString clBoard::FlushB()
{
    switch(_flushB)
    {
        case DGF_5   :  return("DGF_5");
        case DGF_4   :  return("DGF_4");
        case DGF_3   :  return("DGF_3");
        case DGF_2_2 :  return("DGF_2_2");
        case DGF_2   :  return("DGF_2");
        default      :  return("DGP_NO");
    }
}
//---------------------------------------------------------------------------
CString clBoard::StraightB()
{
	CString as; char str[255]; _itoa(_straightB._cnCard, str, 10);
    switch(_straightB._straightB)
    {
		case  DGS_1: as = "DGS_1  "; _itoa(_straightB._cnCard, str, 10); as += str; as += " "; _itoa(_straightB._cnCardMoreThen, str, 10); as += str; return(as);
		case  DGS_2: as = "DGS_2  "; _itoa(_straightB._cnCard, str, 10); as += str; as += " "; _itoa(_straightB._cnCardMoreThen, str, 10); as += str; return(as);
		case  DGS_DRAW: as = "DGS_DRAW  "; _itoa(_straightB._cnCard, str, 10); as += str; as += " "; _itoa(_straightB._cnCardMoreThen, str, 10); as += str; return(as);
        default  : return("DGS_NO");
    }

}
//---------------------------------------------------------------------------
enDangerBoard   clBoard::DangerDraw()
{
    enDangerBoard db=DangerFlushDraw();
    enDangerBoard db1=DangerStraightDraw();
    if(db==DANGER_BOARD_MIDDLE && db1==DANGER_BOARD_MIDDLE) return DANGER_BOARD_HIGH;
    return (db>db1)? db : db1;
}
//---------------------------------------------------------------------------
enDangerBoard   clBoard::DangerFlushDraw()
{
    switch(_flushB)
    {
        case DGF_NO : return DANGER_BOARD_NO;
        case DGF_2  :
        case DGF_2_2: return (_cnBoard==5)? DANGER_BOARD_NO : DANGER_BOARD_MIDDLE;
        case DGF_3  : return (_cnBoard==5)? DANGER_BOARD_MIDDLE : DANGER_BOARD_HIGH;
        case DGF_4  :
        case DGF_5  : return DANGER_BOARD_HORROR;
    }
    return DANGER_BOARD_NO;
}
//---------------------------------------------------------------------------
enDangerBoard   clBoard::DangerStraightDraw()
{
    switch(_straightB._straightB)
    {
        case DGS_NO : return DANGER_BOARD_NO;
        case DGS_DRAW :
            if(_cnBoard==5) return DANGER_BOARD_NO;
            if(_straightB._cnCardMoreThen>5) return DANGER_BOARD_HIGH;
            if(_straightB._cnCardMoreThen>3) return DANGER_BOARD_MIDDLE;
            return DANGER_BOARD_LOW;
        case DGS_2  :
            return (_straightB._cnCardMoreThen<=2)? DANGER_BOARD_MIDDLE : DANGER_BOARD_HIGH;
        case DGS_1  : return DANGER_BOARD_HORROR;
    }
    return DANGER_BOARD_NO;
}
//---------------------------------------------------------------------------
enDangerBoard   clBoard::DangerFullHouse()
{
    switch(_pairB)
    {
        case DGP_NO : return DANGER_BOARD_NO;
        case DGP_2  : return DANGER_BOARD_MIDDLE;
        case DGP_2_2:
        case DGP_3  : return DANGER_BOARD_HIGH;
    }
    return DANGER_BOARD_HORROR;
}
//---------------------------------------------------------------------------
enDangerBoard   clBoard::DangerStraight()
{
    switch(_straightB._straightB)
    {
        case DGS_2  : return (_straightB._cnCard<=2)? DANGER_BOARD_MIDDLE : DANGER_BOARD_HIGH;
        case DGS_1  : return DANGER_BOARD_HORROR;
    }
    return DANGER_BOARD_NO;
}
//---------------------------------------------------------------------------
enDangerBoard   clBoard::DangerFlush()
{
    switch(_flushB)
    {
        case DGF_3  : return DANGER_BOARD_MIDDLE;
        case DGF_4  :
        case DGF_5  : return DANGER_BOARD_HORROR;
    }
    return DANGER_BOARD_NO;
}
//---------------------------------------------------------------------------
enDangerBoard   clBoard::DangerStrOrFlush()
{
    enDangerBoard dbF=DangerFlush(),dbS=DangerStraight();
    if(dbF==DANGER_BOARD_MIDDLE && dbS==DANGER_BOARD_MIDDLE) return DANGER_BOARD_HIGH;
    if(dbF>dbS) dbS=dbF;
    return dbS;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CString BGParamHV_1ToString(enParamHV_1 p1)
{
    switch(p1)
    {
        case PHV1_TWO_OVER                      : return("PHV1_TWO_OVER");
        case PHV1_ONE_OVER                      : return("PHV1_ONE_OVER");
        case PHV1_NO_OVER                       : return("PHV1_NO_OVER");
        case PHV1_POCKET_PAIR                   : return("PHV1_POCKET_PAIR");
        case PHV1_TOP_PAIR                      : return("PHV1_TOP_PAIR");
        case PHV1_SECOND_PAIR                   : return("PHV1_SECOND_PAIR");
        case PHV1_BOTTOM_PAIR                   : return("PHV1_BOTTOM_PAIR");
        case PHV1_PAIRED_BOARD                  : return("PHV1_PAIRED_BOARD");
        case PHV1_BOTH_CARD_PAIRED              : return("PHV1_BOTH_CARD_PAIRED");
        case PHV1_PAIR_AND_PAIR_ON_BOARD        : return("PHV1_PAIR_AND_PAIR_ON_BOARD");
        case PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD : return("PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD");
        case PHV1_TWO_PAIR_ON_BOARD             : return("PHV1_TWO_PAIR_ON_BOARD");
        case PHV1_SET                           : return("PHV1_SET");
        case PHV1_TRIPS                         : return("PHV1_TRIPS");
        case PHV1_TRIPS_ON_BOARD                : return("PHV1_TRIPS_ON_BOARD");
        case PHV1_TWO_CARDS_NUTS_STRAIGHT       : return("PHV1_TWO_CARDS_NUTS_STRAIGHT");
        case PHV1_TWO_CARDS_STRAIGHT            : return("PHV1_TWO_CARDS_STRAIGHT");
        case PHV1_ONE_CARD_NUTS_STRAIGHT        : return("PHV1_ONE_CARD_NUTS_STRAIGHT");
        case PHV1_ONE_CARD_STRAIGHT             : return("PHV1_ONE_CARD_STRAIGHT");
        case PHV1_STRAIGHT_ON_BOARD             : return("PHV1_STRAIGHT_ON_BOARD");
        case PHV1_3CARD_NUTS_FLUSH              : return("PHV1_3CARD_NUTS_FLUSH");
        case PHV1_3CARD_HIGH_FLUSH              : return("PHV1_3CARD_HIGH_FLUSH");
        case PHV1_3CARD_LOW_FLUSH               : return("PHV1_3CARD_LOW_FLUSH");
        case PHV1_4CARD_NUTS_FLUSH              : return("PHV1_4CARD_NUTS_FLUSH");
        case PHV1_4CARD_HIGH_FLUSH              : return("PHV1_4CARD_HIGH_FLUSH");
        case PHV1_4CARD_LOW_FLUSH               : return("PHV1_4CARD_LOW_FLUSH");
        case PHV1_FLUSH_ON_BOARD_NUTS           : return("PHV1_FLUSH_ON_BOARD_NUTS");
        case PHV1_FLUSH_ON_BOARD_HIGH           : return("PHV1_FLUSH_ON_BOARD_HIGH");
        case PHV1_FLUSH_ON_BOARD_LOW            : return("PHV1_FLUSH_ON_BOARD_LOW");
        case PHV1_FULL_2CARD_GOOD               : return("PHV1_FULL_2CARD_GOOD");
        case PHV1_FULL_1CARD_GOOD               : return("PHV1_FULL_1CARD_GOOD");
        case PHV1_FULL_2CARD_NUTS               : return("PHV1_FULL_2CARD_NUTS");
        case PHV1_FULL_1CARD_NUTS               : return("PHV1_FULL_1CARD_NUTS");
        case PHV1_FULL_2CARD_BAD                : return("PHV1_FULL_2CARD_BAD");
        case PHV1_FULL_1CARD_BAD                : return("PHV1_FULL_1CARD_BAD");
        case PHV1_FULL_ON_BOARD                 : return("PHV1_FULL_ON_BOARD");
        case PHV1_SQUARE_WITH_POCKET            : return("PHV1_SQUARE_WITH_POCKET");
        case PHV1_SQUARE_WITHOUT_POCKET         : return("PHV1_SQUARE_WITHOUT_POCKET");
        case PHV1_SQUARE_ON_BOARD               : return("PHV1_SQUARE_ON_BOARD");
        case PHV1_STRAIGHT_FLUSH_2CARD          : return("PHV1_STRAIGHT_FLUSH_2CARD");
        case PHV1_STRAIGHT_FLUSH_1CARD          : return("PHV1_STRAIGHT_FLUSH_1CARD");
        case PHV1_STRAIGHT_FLUSH_ON_BOARD       : return("PHV1_STRAIGHT_FLUSH_ON_BOARD");
        default                                 : return("PHV1_UNDEF");
    }
}
CString BGParamHV_2ToString(enParamHV_2 p2)
{
    switch(p2)
    {
        case PHV2_OVER_PAIR                   :  return("PHV2_OVER_PAIR");
        case PHV2_SECOND_PAIR                 :  return("PHV2_SECOND_PAIR");
        case PHV2_LOW_PAIR                    :  return("PHV2_LOW_PAIR");
        case PHV2_TOP_KICKER                  :  return("PHV2_TOP_KICKER");
        case PHV2_GOOD_KICKER                 :  return("PHV2_GOOD_KICKER");
        case PHV2_WEAK_KICKER                 :  return("PHV2_WEAK_KICKER");
        case PHV2_ACE_KICKER                  :  return("PHV2_ACE_KICKER");
        case PHV2_NOT_ACE_KICKER              :  return("PHV2_NOT_ACE_KICKER");
        case PHV2_TWO_OVER                    :  return("PHV2_TWO_OVER");
        case PHV2_ONE_OVER                    :  return("PHV2_ONE_OVER");
        case PHV2_NO_OVER                     :  return("PHV2_NO_OVER");
        case PHV2_TOP_TWO_PAIR                :  return("PHV2_TOP_TWO_PAIR");
        case PHV2_TOP_PAIR_AND_PAIR           :  return("PHV2_TOP_PAIR_AND_PAIR");
        case PHV2_MIDDLE_PAIR_AND_BOTTOM      :  return("PHV2_MIDDLE_PAIR_AND_BOTTOM");
        case PHV2_2P_TOP_PAIR                 :  return("PHV2_2P_TOP_PAIR");
        case PHV2_2P_SECOND_PAIR              :  return("PHV2_2P_SECOND_PAIR");
        case PHV2_2P_BOTTOM_PAIR              :  return("PHV2_2P_BOTTOM_PAIR");
        case PHV2_2P_OVER_PAIR                :  return("PHV2_2P_OVER_PAIR");
        case PHV2_2P_BIGGER_PAIR_ON_BOARD     :  return("PHV2_2P_BIGGER_PAIR_ON_BOARD");
        case PHV2_2P_LESS_PAIR_ON_BOARD       :  return("PHV2_2P_LESS_PAIR_ON_BOARD");
        case PHV2_HIGH_SET                    :  return("PHV2_HIGH_SET");
        case PHV2_SECOND_SET                  :  return("PHV2_SECOND_SET");
        case PHV2_LOW_SET                     :  return("PHV2_LOW_SET");
        case PHV2_HIGH_TR_HIGH_KICKER         :  return("PHV2_HIGH_TR_HIGH_KICKER");
        case PHV2_HIGH_TR_LOW_KICKER          :  return("PHV2_HIGH_TR_LOW_KICKER");
        case PHV2_SECOND_TR_HIGH_KICKER       :  return("PHV2_SECOND_TR_HIGH_KICKER");
        case PHV2_SECOND_TR_LOW_KICKER        :  return("PHV2_SECOND_TR_LOW_KICKER");
        case PHV2_LOW_TR_HIGH_KICKER          :  return("PHV2_LOW_TR_HIGH_KICKER");
        case PHV2_LOW_TR_LOW_KICKER           :  return("PHV2_LOW_TR_LOW_KICKER");
        case PHV2_POCKET_PAIR_NO_TRIPS_ON_B   :  return("PHV2_POCKET_PAIR_NO_TRIPS_ON_B");
        case PHV2_POCKET_PAIR_AND_TRIPS_ON_B  :  return("PHV2_POCKET_PAIR_AND_TRIPS_ON_B");
        case PHV2_NO_P_NO_TRIPS               :  return("PHV2_NO_P_NO_TRIPS");
        case PHV2_FULL_TRIPS_ON_BOARD         :  return("PHV2_FULL_TRIPS_ON_BOARD");
        case PHV2_FULL_TOP_PAIR_NO_TR         :  return("PHV2_FULL_TOP_PAIR_NO_TR");
        case PHV2_FULL_BOTTOB_PAIR_NO_TR      :  return("PHV2_FULL_BOTTOB_PAIR_NO_TR");
        default                               :  return("PHV2_UNDEF");
    }
}
CString BGFlushDrawParamHV_1(enFlushDrawParamHV_1 p3)
{
    switch (p3)
    {
        case DPHV_2CARD_NUTS_FLDR           :  return("DPHV_2CARD_NUTS_FLDR");
        case DPHV_2CARD_HIGH_FLDR           :  return("DPHV_2CARD_HIGH_FLDR");
        case DPHV_2CARD_LOW_FLDR            :  return("DPHV_2CARD_LOW_FLDR");
        case DPHV_1CARD_NUTS_FLDR           :  return("DPHV_1CARD_NUTS_FLDR");
        case DPHV_1CARD_HIGH_FLDR           :  return("DPHV_1CARD_HIGH_FLDR");
        case DPHV_1CARD_LOW_FLDR            :  return("DPHV_1CARD_LOW_FLDR");
        case DPHV_2CARD_NUTS_BACKDOOR_FLDR  :  return("DPHV_2CARD_NUTS_BACKDOOR_FLDR");
        case DPHV_2CARD_HIGH_BACKDOOR_FLDR  :  return("DPHV_2CARD_HIGH_BACKDOOR_FLDR");
        case DPHV_1CARD_NUTS_BACKDOOR_FLDR  :  return("DPHV_1CARD_NUTS_BACKDOOR_FLDR");
        case DPHV_1CARD_HIGH_BACKDOOR_FLDR  :  return("DPHV_1CARD_HIGH_BACKDOOR_FLDR");
        case DPHV_FLUSH                     :  return("DPHV_FLUSH");
        default                             :  return("DPHV_NO_FLUSH_DRAW");
    }
}
CString BGenStraightDrawParamHV_2(enStraightDrawParamHV_2 p4)
{
    switch(p4)
    {
        case DPHV2_2CARD_STR_DRAW   :return("DPHV2_2CARD_STR_DRAW");
        case DPHV2_2CARD_GTSH_DRAW  :return("DPHV2_2CARD_GTSH_DRAW");
        case DPHV2_1CARD_STR_DRAW   :return("DPHV2_1CARD_STR_DRAW");
        case DPHV2_1CARD_GTSH_DRAW  :return("DPHV2_1CARD_GTSH_DRAW");
        default   :                  return("DPHV2_NO_STR_DRAW");
    }
}

enParamHV_1 BGStringToParamHV_1(char *str)
{
    if(strcmp(str,"PHV1_TWO_OVER")==0) return(PHV1_TWO_OVER);
    if(strcmp(str,"PHV1_ONE_OVER")==0) return(PHV1_ONE_OVER);
    if(strcmp(str,"PHV1_NO_OVER")==0) return(PHV1_NO_OVER );
    if(strcmp(str,"PHV1_POCKET_PAIR")==0) return(PHV1_POCKET_PAIR );
    if(strcmp(str,"PHV1_TOP_PAIR")==0) return(PHV1_TOP_PAIR);
    if(strcmp(str,"PHV1_SECOND_PAIR")==0) return(PHV1_SECOND_PAIR );
    if(strcmp(str,"PHV1_BOTTOM_PAIR")==0) return(PHV1_BOTTOM_PAIR );
    if(strcmp(str,"PHV1_PAIRED_BOARD")==0) return(PHV1_PAIRED_BOARD);
    if(strcmp(str,"PHV1_BOTH_CARD_PAIRED")==0) return(PHV1_BOTH_CARD_PAIRED);
    if(strcmp(str,"PHV1_PAIR_AND_PAIR_ON_BOARD")==0) return(PHV1_PAIR_AND_PAIR_ON_BOARD);
    if(strcmp(str,"PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD")==0) return(PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD );
    if(strcmp(str,"PHV1_TWO_PAIR_ON_BOARD")==0) return(PHV1_TWO_PAIR_ON_BOARD );
    if(strcmp(str,"PHV1_SET")==0) return(PHV1_SET );
    if(strcmp(str,"PHV1_TRIPS")==0) return(PHV1_TRIPS );
    if(strcmp(str,"PHV1_TRIPS_ON_BOARD")==0) return(PHV1_TRIPS_ON_BOARD);
    if(strcmp(str,"PHV1_TWO_CARDS_NUTS_STRAIGHT")==0) return(PHV1_TWO_CARDS_NUTS_STRAIGHT );
    if(strcmp(str,"PHV1_TWO_CARDS_STRAIGHT")==0) return(PHV1_TWO_CARDS_STRAIGHT);
    if(strcmp(str,"PHV1_ONE_CARD_NUTS_STRAIGHT")==0) return(PHV1_ONE_CARD_NUTS_STRAIGHT);
    if(strcmp(str,"PHV1_ONE_CARD_STRAIGHT")==0) return(PHV1_ONE_CARD_STRAIGHT );
    if(strcmp(str,"PHV1_STRAIGHT_ON_BOARD")==0) return(PHV1_STRAIGHT_ON_BOARD );
    if(strcmp(str,"PHV1_3CARD_NUTS_FLUSH")==0) return(PHV1_3CARD_NUTS_FLUSH);
    if(strcmp(str,"PHV1_3CARD_HIGH_FLUSH")==0) return(PHV1_3CARD_HIGH_FLUSH);
    if(strcmp(str,"PHV1_3CARD_LOW_FLUSH")==0) return(PHV1_3CARD_LOW_FLUSH );
    if(strcmp(str,"PHV1_4CARD_NUTS_FLUSH")==0) return(PHV1_4CARD_NUTS_FLUSH);
    if(strcmp(str,"PHV1_4CARD_HIGH_FLUSH")==0) return(PHV1_4CARD_HIGH_FLUSH);
    if(strcmp(str,"PHV1_4CARD_LOW_FLUSH")==0) return(PHV1_4CARD_LOW_FLUSH );
    if(strcmp(str,"PHV1_FLUSH_ON_BOARD_NUTS")==0) return(PHV1_FLUSH_ON_BOARD_NUTS );
    if(strcmp(str,"PHV1_FLUSH_ON_BOARD_HIGH")==0) return(PHV1_FLUSH_ON_BOARD_HIGH );
    if(strcmp(str,"PHV1_FLUSH_ON_BOARD_LOW")==0) return(PHV1_FLUSH_ON_BOARD_LOW);
    if(strcmp(str,"PHV1_FULL_2CARD_GOOD")==0) return(PHV1_FULL_2CARD_GOOD);
    if(strcmp(str,"PHV1_FULL_1CARD_GOOD")==0) return(PHV1_FULL_1CARD_GOOD);
    if(strcmp(str,"PHV1_FULL_2CARD_NUTS")==0) return(PHV1_FULL_2CARD_NUTS);
    if(strcmp(str,"PHV1_FULL_1CARD_NUTS")==0) return(PHV1_FULL_1CARD_NUTS);
    if(strcmp(str,"PHV1_FULL_2CARD_BAD")==0) return(PHV1_FULL_2CARD_BAD);
    if(strcmp(str,"PHV1_FULL_1CARD_BAD")==0) return(PHV1_FULL_1CARD_BAD);
    if(strcmp(str,"PHV1_FULL_ON_BOARD")==0) return(PHV1_FULL_ON_BOARD );
    if(strcmp(str,"PHV1_SQUARE_WITH_POCKET")==0) return(PHV1_SQUARE_WITH_POCKET);
    if(strcmp(str,"PHV1_SQUARE_WITHOUT_POCKET")==0) return(PHV1_SQUARE_WITHOUT_POCKET );
    if(strcmp(str,"PHV1_SQUARE_ON_BOARD")==0) return(PHV1_SQUARE_ON_BOARD );
    if(strcmp(str,"PHV1_STRAIGHT_FLUSH_2CARD")==0) return(PHV1_STRAIGHT_FLUSH_2CARD);
    if(strcmp(str,"PHV1_STRAIGHT_FLUSH_1CARD")==0) return(PHV1_STRAIGHT_FLUSH_1CARD);
    if(strcmp(str,"PHV1_STRAIGHT_FLUSH_ON_BOARD")==0) return(PHV1_STRAIGHT_FLUSH_ON_BOARD);
    return(PHV1_UNDEF);
}
enParamHV_2 BGStringToParamHV_2(char *str)
{
    if(strcmp(str,"PHV2_OVER_PAIR")==0) return(PHV2_OVER_PAIR  );
    if(strcmp(str,"PHV2_SECOND_PAIR")==0) return(PHV2_SECOND_PAIR);
    if(strcmp(str,"PHV2_LOW_PAIR")==0) return(PHV2_LOW_PAIR   );
    if(strcmp(str,"PHV2_TOP_KICKER")==0) return(PHV2_TOP_KICKER );
    if(strcmp(str,"PHV2_GOOD_KICKER")==0) return(PHV2_GOOD_KICKER);
    if(strcmp(str,"PHV2_WEAK_KICKER")==0) return(PHV2_WEAK_KICKER);
    if(strcmp(str,"PHV2_ACE_KICKER")==0) return(PHV2_ACE_KICKER );
    if(strcmp(str,"PHV2_NOT_ACE_KICKER")==0) return(PHV2_NOT_ACE_KICKER );
    if(strcmp(str,"PHV2_TWO_OVER")==0) return(PHV2_TWO_OVER   );
    if(strcmp(str,"PHV2_ONE_OVER")==0) return(PHV2_ONE_OVER   );
    if(strcmp(str,"PHV2_NO_OVER")==0) return(PHV2_NO_OVER);
    if(strcmp(str,"PHV2_TOP_TWO_PAIR")==0) return(PHV2_TOP_TWO_PAIR   );
    if(strcmp(str,"PHV2_TOP_PAIR_AND_PAIR")==0) return(PHV2_TOP_PAIR_AND_PAIR  );
    if(strcmp(str,"PHV2_MIDDLE_PAIR_AND_BOTTOM")==0) return(PHV2_MIDDLE_PAIR_AND_BOTTOM );
    if(strcmp(str,"PHV2_2P_TOP_PAIR")==0) return(PHV2_2P_TOP_PAIR);
    if(strcmp(str,"PHV2_2P_SECOND_PAIR")==0) return(PHV2_2P_SECOND_PAIR );
    if(strcmp(str,"PHV2_2P_BOTTOM_PAIR")==0) return(PHV2_2P_BOTTOM_PAIR );
    if(strcmp(str,"PHV2_2P_OVER_PAIR")==0) return(PHV2_2P_OVER_PAIR   );
    if(strcmp(str,"PHV2_2P_BIGGER_PAIR_ON_BOARD")==0) return(PHV2_2P_BIGGER_PAIR_ON_BOARD);
    if(strcmp(str,"PHV2_2P_LESS_PAIR_ON_BOARD  ")==0) return(PHV2_2P_LESS_PAIR_ON_BOARD  );
    if(strcmp(str,"PHV2_HIGH_SET")==0) return(PHV2_HIGH_SET   );
    if(strcmp(str,"PHV2_SECOND_SET")==0) return(PHV2_SECOND_SET );
    if(strcmp(str,"PHV2_LOW_SET")==0) return(PHV2_LOW_SET);
    if(strcmp(str,"PHV2_HIGH_TR_HIGH_KICKER")==0) return(PHV2_HIGH_TR_HIGH_KICKER);
    if(strcmp(str,"PHV2_HIGH_TR_LOW_KICKER")==0) return(PHV2_HIGH_TR_LOW_KICKER );
    if(strcmp(str,"PHV2_SECOND_TR_HIGH_KICKER")==0) return(PHV2_SECOND_TR_HIGH_KICKER  );
    if(strcmp(str,"PHV2_SECOND_TR_LOW_KICKER")==0) return(PHV2_SECOND_TR_LOW_KICKER   );
    if(strcmp(str,"PHV2_LOW_TR_HIGH_KICKER")==0) return(PHV2_LOW_TR_HIGH_KICKER );
    if(strcmp(str,"PHV2_LOW_TR_LOW_KICKER")==0) return(PHV2_LOW_TR_LOW_KICKER  );
    if(strcmp(str,"PHV2_POCKET_PAIR_NO_TRIPS_ON_B")==0) return(PHV2_POCKET_PAIR_NO_TRIPS_ON_B  );
    if(strcmp(str,"PHV2_POCKET_PAIR_AND_TRIPS_ON_B")==0) return(PHV2_POCKET_PAIR_AND_TRIPS_ON_B );
    if(strcmp(str,"PHV2_NO_P_NO_TRIPS")==0) return(PHV2_NO_P_NO_TRIPS  );
    if(strcmp(str,"PHV2_FULL_TRIPS_ON_BOARD")==0) return(PHV2_FULL_TRIPS_ON_BOARD);
    if(strcmp(str,"PHV2_FULL_TOP_PAIR_NO_TR")==0) return(PHV2_FULL_TOP_PAIR_NO_TR);
    if(strcmp(str,"PHV2_FULL_BOTTOB_PAIR_NO_TR")==0) return(PHV2_FULL_BOTTOB_PAIR_NO_TR );
    return(PHV2_UNDEF);
}
enFlushDrawParamHV_1 BGStringToFlushDrawParamHV_1(char *str)
{
    if(strcmp(str,"DPHV_2CARD_NUTS_FLDR")==0) return(DPHV_2CARD_NUTS_FLDR);
    if(strcmp(str,"DPHV_2CARD_HIGH_FLDR")==0) return(DPHV_2CARD_HIGH_FLDR);
    if(strcmp(str,"DPHV_2CARD_LOW_FLDR")==0) return(DPHV_2CARD_LOW_FLDR);
    if(strcmp(str,"DPHV_1CARD_NUTS_FLDR")==0) return(DPHV_1CARD_NUTS_FLDR);
    if(strcmp(str,"DPHV_1CARD_HIGH_FLDR")==0) return(DPHV_1CARD_HIGH_FLDR);
    if(strcmp(str,"DPHV_1CARD_LOW_FLDR")==0) return(DPHV_1CARD_LOW_FLDR);
    if(strcmp(str,"DPHV_2CARD_NUTS_BACKDOOR_FLDR")==0) return(DPHV_2CARD_NUTS_BACKDOOR_FLDR );
    if(strcmp(str,"DPHV_2CARD_HIGH_BACKDOOR_FLDR")==0) return(DPHV_2CARD_HIGH_BACKDOOR_FLDR );
    if(strcmp(str,"DPHV_1CARD_NUTS_BACKDOOR_FLDR")==0) return(DPHV_1CARD_NUTS_BACKDOOR_FLDR );
    if(strcmp(str,"DPHV_1CARD_HIGH_BACKDOOR_FLDR")==0) return(DPHV_1CARD_HIGH_BACKDOOR_FLDR );
    if(strcmp(str,"DPHV_FLUSH")==0) return(DPHV_FLUSH);
    return(DPHV_UNDEF);
}
enStraightDrawParamHV_2 BGStringToStraightDrawParamHV_2(char *str)
{
    if(strcmp(str,"DPHV2_2CARD_STR_DRAW")==0) return(DPHV2_2CARD_STR_DRAW);
    if(strcmp(str,"DPHV2_2CARD_GTSH_DRAW")==0) return(DPHV2_2CARD_GTSH_DRAW);
    if(strcmp(str,"DPHV2_1CARD_STR_DRAW")==0) return(DPHV2_1CARD_STR_DRAW);
    if(strcmp(str,"DPHV2_1CARD_GTSH_DRAW")==0) return(DPHV2_1CARD_GTSH_DRAW);
    if(strcmp(str,"DPHV2_NO_STR_DRAW")==0) return(DPHV2_NO_STR_DRAW);
    return(DPHV2_UNDEF);
}
int BGCnParam1(enGame g,enParamHV_1 *arr1)
{
    switch(g)
    {
        case GM_0       :arr1[0]=PHV1_TWO_OVER;arr1[1]=PHV1_ONE_OVER;arr1[2]=PHV1_NO_OVER;return(3);
        case GM_2       :arr1[0]=PHV1_POCKET_PAIR;arr1[1]=PHV1_TOP_PAIR;arr1[2]=PHV1_SECOND_PAIR;arr1[3]=PHV1_BOTTOM_PAIR;
                         arr1[4]=PHV1_PAIRED_BOARD;return(5);
        case GM_22      :arr1[0]=PHV1_BOTH_CARD_PAIRED;arr1[1]=PHV1_PAIR_AND_PAIR_ON_BOARD;arr1[2]=PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD;
                         arr1[3]=PHV1_TWO_PAIR_ON_BOARD;return(4);
        case GM_3       :arr1[0]=PHV1_SET;arr1[1]=PHV1_TRIPS;arr1[2]=PHV1_TRIPS_ON_BOARD;return(3);
        case GM_ST      :arr1[0]=PHV1_TWO_CARDS_NUTS_STRAIGHT;arr1[1]=PHV1_TWO_CARDS_STRAIGHT;arr1[2]=PHV1_ONE_CARD_NUTS_STRAIGHT;arr1[3]=PHV1_ONE_CARD_STRAIGHT;
                         arr1[4]=PHV1_STRAIGHT_ON_BOARD;return(5);
        case GM_FL      :arr1[0]=PHV1_3CARD_NUTS_FLUSH;arr1[1]=PHV1_3CARD_HIGH_FLUSH;arr1[2]=PHV1_3CARD_LOW_FLUSH;arr1[3]=PHV1_4CARD_NUTS_FLUSH;
                         arr1[4]=PHV1_4CARD_HIGH_FLUSH;arr1[5]=PHV1_4CARD_LOW_FLUSH;arr1[6]=PHV1_FLUSH_ON_BOARD_NUTS;arr1[7]=PHV1_FLUSH_ON_BOARD_HIGH;
                         arr1[8]=PHV1_FLUSH_ON_BOARD_LOW;return(9);
        case GM_32      :arr1[0]=PHV1_FULL_2CARD_GOOD;arr1[1]=PHV1_FULL_2CARD_NUTS;arr1[2]=PHV1_FULL_2CARD_BAD;
                         arr1[3]=PHV1_FULL_1CARD_GOOD;arr1[4]=PHV1_FULL_1CARD_NUTS;arr1[5]=PHV1_FULL_1CARD_BAD;
                         arr1[6]=PHV1_FULL_ON_BOARD;return(7);
        case GM_4       :arr1[0]=PHV1_SQUARE_WITH_POCKET;arr1[1]=PHV1_SQUARE_WITHOUT_POCKET;arr1[2]=PHV1_SQUARE_ON_BOARD;return(3);
        case GM_FS      :arr1[0]=PHV1_STRAIGHT_FLUSH_2CARD;arr1[1]=PHV1_STRAIGHT_FLUSH_1CARD;arr1[2]=PHV1_STRAIGHT_FLUSH_ON_BOARD;return(3);
        default         :return(0);

    }
}
int BGCnParam2(enParamHV_1 p1,enParamHV_2 *arr2)
{
    switch(p1)
    {
        case PHV1_POCKET_PAIR : arr2[0]=PHV2_OVER_PAIR;arr2[1]=PHV2_SECOND_PAIR;arr2[2]=PHV2_LOW_PAIR;return(3);
        case PHV1_TOP_PAIR    : arr2[0]=PHV2_TOP_KICKER;arr2[1]=PHV2_GOOD_KICKER;arr2[2]=PHV2_WEAK_KICKER;return(3);
        case PHV1_SECOND_PAIR :
        case PHV1_BOTTOM_PAIR : arr2[0]=PHV2_ACE_KICKER;arr2[1]=PHV2_NOT_ACE_KICKER;return(2);
        case PHV1_PAIRED_BOARD: arr2[0]=PHV2_TWO_OVER;arr2[1]=PHV2_ONE_OVER;arr2[2]=PHV2_NO_OVER;return(3);
        case PHV1_BOTH_CARD_PAIRED    : arr2[0]=PHV2_TOP_TWO_PAIR;arr2[1]=PHV2_TOP_PAIR_AND_PAIR;arr2[2]=PHV2_MIDDLE_PAIR_AND_BOTTOM;return(3);
        case PHV1_PAIR_AND_PAIR_ON_BOARD    : arr2[0]=PHV2_2P_TOP_PAIR;arr2[1]=PHV2_2P_SECOND_PAIR;arr2[2]=PHV2_2P_BOTTOM_PAIR;return(3);
        case PHV1_POCKET_PAIR_AND_PAIR_ON_BOARD    : arr2[0]=PHV2_2P_OVER_PAIR;arr2[1]=PHV2_2P_BIGGER_PAIR_ON_BOARD;arr2[2]=PHV2_2P_LESS_PAIR_ON_BOARD;return(3);
        case PHV1_SET    : arr2[0]=PHV2_HIGH_SET;arr2[1]=PHV2_SECOND_SET;arr2[2]=PHV2_LOW_SET;return(3);
        case PHV1_TRIPS    : arr2[0]=PHV2_HIGH_TR_HIGH_KICKER;arr2[1]=PHV2_HIGH_TR_LOW_KICKER;arr2[2]=PHV2_SECOND_TR_HIGH_KICKER;
                            arr2[3]=PHV2_SECOND_TR_LOW_KICKER;arr2[4]=PHV2_LOW_TR_HIGH_KICKER;arr2[5]=PHV2_LOW_TR_LOW_KICKER;return(6);
        case PHV1_FULL_2CARD_GOOD:
        case PHV1_FULL_2CARD_NUTS:
        case PHV1_FULL_2CARD_BAD:
                                 arr2[0]=PHV2_POCKET_PAIR_NO_TRIPS_ON_B;arr2[1]=PHV2_POCKET_PAIR_AND_TRIPS_ON_B;arr2[2]=PHV2_NO_P_NO_TRIPS;return(3);
        case PHV1_FULL_1CARD_GOOD    :
        case PHV1_FULL_1CARD_NUTS    :
        case PHV1_FULL_1CARD_BAD    :
                                 arr2[0]=PHV2_FULL_TRIPS_ON_BOARD;arr2[1]=PHV2_FULL_TOP_PAIR_NO_TR;arr2[2]=PHV2_FULL_BOTTOB_PAIR_NO_TR;return(3);
    }
    return(0);
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
