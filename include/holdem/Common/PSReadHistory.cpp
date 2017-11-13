//---------------------------------------------------------------------------


#pragma hdrstop

//#include <alloc.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>

//#include "..\..\Util\WorkStr.h"
#include "PSReadHistory.h"

enum    enPSReadAct {READ_BEGINGAME,READ_GAMEINFO,READ_TABLENB,READ_BUTTON,READ_CNPLAYER,READ_SEATINFO,READ_ACT,READ_GAME_END,TABLE_CLOSED};
enPSReadAct glPSReadAct;
clGameHistory *glPSPtrGame;
//bool glPSReadEmail;

//void    PSReadHistory(char *buf,int size,clListHistory *listH);
//void    PSReadHistory_Email(char *buf,int size,clListHistory *listH);
void    PSReadHistory_HH(char *buf,int size,clListHistory *listH,bool restraight);
int     PSReadGameHistory(char *buf,int size);
//int     PSReadGameHistoryEmail(char *buf,int sizeBuf);
int     PSAnalisStr(char *str,int len);
//int     PSAnalisStrEmail(char *str,int len);
int     PSReadHeaderGame(char *str,int len);
//int     PSReadHeaderGameCash(char *str,int len);
//int     PSReadHeaderEmail(char *buf,int sizeBuf);

int     PSReadEndStr(char *str,int size);
void    PSReadCards(char *str,int len,int cn,clCard *card);
int     PSFindPlayer(char *str,int len,int &size);
void    PSReadAction(int nb,char *str,int len);
int     PSReadHHReadMoney(char *str,int len,int &size);
//int     PSFindDigBeg(char *str);
//int     PSFindDigEnd(char *str);
//int     PSReadPlace(char *str,int len);
//clDataTime  PSReadTime(char *str,int len,int &size0);
//void    PSReadBalance(int nb,char *str,int len);
//int     PSReadFullName(char *str,int len,char *name,char *endWord);
//---------------------------------------------------------------------------
int     PSReadFullName(char *str,int len,char *name,char *endWord)
{
    int i;
    for(i=0;i<len;i++)
    {
        for(int j=0;;j++)
        {
            if(endWord[j]=='\0')
            {
//                if(name[i-1]==' ')
//                    name[i-1]='\0';
//                else
                    name[i]='\0';
                return(i);
            }
            if(i+j>=len)
                return(-1);
            if(*(str+i+j)!=endWord[j])  break;
        }
        if(i>=MAX_LEN_NAME_PLAYER)
            return(-1);
        name[i]=str[i];

    }
    if(i>=MAX_LEN_NAME_PLAYER)
        return(-1);
    name[i]='\0';
    return(-1);
}
//---------------------------------------------------------------------------
void PSReadHistoryF(int handle,clListHistory *listH,bool rst)
{
    listH->Clear();
    char *buf;
    int size=_filelength(handle);
    buf=(char*)malloc(size);
    _lseek(handle,0,SEEK_SET);
    size=_read(handle,buf,size);
    PSReadHistory_HH(buf,size-1,listH,rst);
    free(buf);
    listH->RepairListHistToStraight();
}
//---------------------------------------------------------------------------
/*void PSReadHistory(char *buf,int sizeBuf,clListHistory *listH)
{
    int size=0,size0,len;
    char *str;
    while(size<sizeBuf)
    {
        str=buf+size;
        len=PSReadEndStr(str,sizeBuf-size);
        if(len>6)
        {
            if(WSStrFind(str,"PokerStars",0,size0))
            {
                PSReadHistory_HH(str,sizeBuf-size,listH);
                break;
            }
        }
        size+=len;
    }
} */
//---------------------------------------------------------------------------
void    PSReadHistory_HH(char *buf,int sizeBuf,clListHistory *listH,bool rst)
{
    int size=0;
    while(size<sizeBuf)
    {
        glPSPtrGame=new clGameHistory;
//        glPSPtrGame->_atrHH._site=SITE_POKERSTARS;
        size+=PSReadGameHistory(buf+size,sizeBuf-size);
        if(glPSPtrGame->CnAct()>3)
        {
            if(rst)
                glPSPtrGame->RestraightNbToPos();
            listH->Add(glPSPtrGame);
        }
        else
            delete glPSPtrGame;
    }
}
//---------------------------------------------------------------------------
int PSReadGameHistory(char *buf,int sizeBuf)
{
    glPSReadAct=READ_BEGINGAME;
    char *str;
    int size=0,len;
    while(size<sizeBuf)
    {
        str=buf+size;
        len=PSReadEndStr(str,sizeBuf-size);
        if(PSAnalisStr(str,len)==-1)
            break;
        size+=len;
        if(glPSReadAct==TABLE_CLOSED)
            break;
    }
    return size;
}
//---------------------------------------------------------------------------
int     PSAnalisStr(char *str,int len)
{
    int size0;
    switch(glPSReadAct)
    {
        case READ_BEGINGAME :
            if(len>18)
                if(WSStrFind(str,"PokerStars Hand #",5,size0))
                {
                    PSReadHeaderGame(str+size0,len-size0);
                    glPSReadAct=READ_TABLENB;
                }
                break;
        case READ_TABLENB :
            if(WSStrFind(str,"Table '",0,size0))
            {
                int sizeT=0;
                char word[32];
                size0+=WSReadWord(str+size0,len,word);
//                glPSPtrGame->WrNbTable(WSReadInt(str+size0,len-size0,sizeT));
//                size0+=sizeT;
                if(WSStrFind(str+size0,"6-max",len-size0,sizeT))
                {
                    glPSPtrGame->WrCnSeat(6);
                    size0+=sizeT;
                }
                else if(WSStrFind(str+size0,"2-max",len-size0,sizeT))
                {
                    glPSPtrGame->WrCnSeat(6);
                    size0+=sizeT;
                }
                else return -1;
                if(WSStrFind(str+size0,"Seat #",len-size0,sizeT))
                {
                    size0+=sizeT;
                    glPSPtrGame->WrNbButton(WSReadInt(str+size0,len-size0,sizeT)-1);
                }
                else
                    ErrMessage("PSReadHistory","Unknown Button");
                glPSReadAct=READ_SEATINFO;
                return 0;
            }
            else
                return(-1);
        case READ_SEATINFO :
            if(WSStrFind(str,"Seat ",0,size0))
            {
                int nb,money;
                char name[MAX_LEN_NAME_PLAYER];
                nb=WSReadInt(str,len,size0)-1;
                size0+=2;
                str+=size0;
                len-=size0;
                size0=PSReadFullName(str,len,name," (")+1;
//                if(glPSPtrGame->_tpTrny==TPT_TRN_CASH)
//                    money=WSReadMoney(str+size0+3,len,size0);
//                else
                str+=size0;
                len-=size0;
                money=WSReadChips(str,len,size0);
                str+=size0;
                len-=size0;
                bool inPlay=!WSStrFind(str,"is sitting out",len,size0);
                glPSPtrGame->WrSeatInfo(nb,name,money,inPlay);
                glPSPtrGame->_atrHH._cnPl++;
                return 0;
            }
            else if(WSStrFind(str,"posts",len,size0))
            {
                char name[MAX_LEN_NAME_PLAYER];
                PSReadFullName(str,len,name,": posts");
                int size1,nb=glPSPtrGame->FindSeat(name);
                if(nb != -1)
                {
                    if(WSStrFind(str+size0,"ante",len-size0,size1))
                    {
                        size0+=size1;
                        glPSPtrGame->AddAct(nb,ACT_POSTANTE,WSReadInt(str+size0,len-size0,size1));
                    }
                    else if(WSStrFind(str+size0,"small",len-size0,size1))
                    {
                        size0+=size1;
                        int money;
//                        if(glPSPtrGame->_tpTrny==TPT_TRN_CASH)
//                            money=WSReadMoney(str+size0,len-size0,size1);
//                        else
                            money=WSReadChips(str+size0+1,len-size0,size1);
                        glPSPtrGame->AddAct(nb,ACT_POSTSB,money);
                    }
                    else if(WSStrFind(str+size0,"big",len-size0,size1))
                    {
                        size0+=size1;
                        int money;
//                        if(glPSPtrGame->_tpTrny==TPT_TRN_CASH)
//                            money=WSReadMoney(str+size0,len-size0,size1);
//                        else
                            money=WSReadChips(str+size0+1,len-size0,size1);
                        glPSPtrGame->AddAct(nb,ACT_POSTBB,money);
                    }
                    return 0;
                }
            }
//            else if(WSStrFind(str,"The button is in seat #",len,size0))
//            {
//                glPSPtrGame->WrNbButton(WSReadInt(str+4,len,size0)-1);
//                return 0;
//            }
            else if(WSStrFind(str,"*** HOLE CARDS ***",4,size0))
            {
                glPSPtrGame->AddAct(NB_DILER,ACT_DELCARD,0);
                glPSReadAct=READ_ACT;
                return 0;
            }
            break;
        case READ_ACT :
         {
            int size=0;
            char word[4],name[MAX_LEN_NAME_PLAYER];
            if(WSStrFind(str,">You have options",0,size0))
            {
                return 0;
            }
            if(WSStrFind(str,"Dealt to",0,size))
            {
                size+=PSReadFullName(str+size+1,len,name," [");
                clCard *ptrC=glPSPtrGame->PlayerCards(glPSPtrGame->FindSeat(name));
                PSReadCards(str+size,len,2,ptrC);
                glPSPtrGame->AddAct(name,ACT_SHOW,0);
                return 0;
            }
            if(WSStrFind(str,"***",0,size0))
            {
                if(WSStrFind(str+4,"FLOP",0,size))
                {
                    clCard card[3];
                    PSReadCards(str+size+6,len,3,card);
                    glPSPtrGame->WrFlop(card);
                    return 0;
                }
                if(WSStrFind(str+4,"TURN",0,size))
                {
                    clCard card;
                    PSReadCards(str+size+18,len,1,&card);
                    glPSPtrGame->WrTurn(card);
                    return 0;
                }
                if(WSStrFind(str+4,"SHOW DOWN",0,size))
                {
//                    glPSReadAct=READ_GAME_END;
                    return 0;
                }
                if(WSStrFind(str+4,"RIVER",0,size))
                {
                    clCard card;
                    PSReadCards(str+size+20,len,1,&card);
                    glPSPtrGame->WrRiver(card);
                    return 0;
                }
                if(WSStrFind(str+4,"SUMMARY",0,size))
                {
                    glPSReadAct=READ_GAME_END;
                    return 0;
                }
                return -1;
            }
//            if(WSStrFind(str,"Game",0,size0))
//            {
//                if(WSStrFind(str+size,"starts.",len,size0))
//                    glPSReadAct=READ_GAME_END;
//                return 0;
//            }

/*
            if(!WSStrFind(str,":",len,size0) && !WSStrFind(str,"collected",len,size0))
            {
                return 0;
            }
            else if(WSStrFind(str," said",len,size0))
            {
                return 0;
            }
            size=PSReadFullName(str,len,name,": checks");
            if(size==-1)
                size=PSReadFullName(str,len,name,": folds");
            if(size==-1)
            {
                size=PSReadFullName(str,len,name,": raises");
            }
            if(size==-1)
                size=PSReadFullName(str,len,name,": calls");
            if(size==-1)
                size=PSReadFullName(str,len,name,": bets");
            if(size==-1)
                size=PSReadFullName(str,len,name,": shows [");
            if(size==-1)
                size=PSReadFullName(str,len,name,": mucks");
            if(size==-1)
                size=PSReadFullName(str,len,name," collected");
            if(size==-1)
            {
                if(WSStrFind(str,"returned to ",len,size))
                {
                    size+=PSReadFullName(str+size,len,name,"\n");
                    size=0;
                }
            }
            if(size==-1)
                return(0);
            int nb=glPSPtrGame->FindSeat(name);
*/
            int nb=PSFindPlayer(str,0,size);
            if(nb == -1)
                if(WSStrFind(str,"Uncalled bet",len,size))
                {
                    nb=PSFindPlayer(str,len-1,size);
                    size=0;
                }
            if(nb != -1)
                PSReadAction(nb,str+size,len-size);
            return 0;
         }
         case  READ_GAME_END:
         {
            if(len==1)
                return(-1);
            int size;
            if(WSStrFind(str,"mucked",len,size))
            {
                int sizeP;
                int nbPl=WSReadInt(str,len,sizeP)-1;
                PSReadCards(str+size,len-size,2,glPSPtrGame->GetSeat(nbPl)->_cards);

            }

         } break;


         default : return -1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int PSFindPlayer(char *str,int len,int &size)
{
    for(int i=0;i<glPSPtrGame->MaxCnSeat();i++)
        if(glPSPtrGame->IsPlayer(i))
            if(WSStrFind(str,glPSPtrGame->PlayerName(i),len,size))
                return i;
    return -1;
}
//---------------------------------------------------------------------------
int PSReadHeaderGame(char *str,int len)
{
    int size;
    long long nbG=WSReadInt64(str,len,size);
    glPSPtrGame->WrNbGame(nbG);
    if(WSStrFind(str,"Tournament #",len,size))
    {
//        glPSPtrGame->_atrHH._tpGame=GAME_TRNY_SINGLE;
        str+=size;
        len-=size;
        glPSPtrGame->_atrHH._nbTrny=WSReadInt(str,len,size);
        glPSPtrGame->_atrHH._timeLevel=3;
        str+=size;
        len-=size;
    }
//    else
//        glPSPtrGame->_atrHH._tpGame=GAME_CASH;
/*
    if(WSFindChar(str,len,'$') != -1)
    {
        int money=PSReadHHReadMoney(str,len,size);
        str+=size;
        len-=size;
        money+=PSReadHHReadMoney(str,len,size);
        glPSPtrGame->_atrHH._tableCost=money;
        str+=size;
        len-=size;
    }
*/
    if(!WSStrFind(str,"Hold'em",len,size))
//        glPSPtrGame->_arrSeat._cnCardPoker=2;
//    else
        ErrMessage("PSReadHistory","Holdem or Omaha?");
    str+=size;
    len-=size;
    size=WSFindChar(str,len,'/');
    if(size==-1)
    {
        ErrMessage("PSReadHeaderGame","Не читаются блайнды");
        return 1;
    }
    str+=size;
    len-=size;
    glPSPtrGame->WrBigBlinds(WSReadInt(str,len,size));
    str+=size;
    len-=size;
    int day,hour,min,sec,year,month;
    year=WSReadInt(str,len,size);
    str+=size;
    len-=size;
    month=WSReadInt(str,len,size);
    str+=size;
    len-=size;
    day=WSReadInt(str,len,size);
    str+=size;
    len-=size;
    hour=WSReadInt(str,len,size);
    str+=size;
    len-=size;
    min=WSReadInt(str,len,size);
    sec=WSReadInt(str+size,len-size,size);
    stDayTime dt;
    dt._t.SetTime(hour,min,sec);
    dt._day.SetDay(year,month,day);
    glPSPtrGame->WriteDate(dt);
    return 0;
}
//---------------------------------------------------------------------------
int    PSReadEndStr(char *buf,int sizeBuf)
{
    int size;
    size=WSFindChar(buf,sizeBuf,'\n');
    if(size==-1)
        return sizeBuf;
    if(size>0 && buf[size-1]=='=')
        size+=PSReadEndStr(buf+size,sizeBuf-size);
    return size+1;
}
//---------------------------------------------------------------------------
void    PSReadCards(char *str,int len,int cn,clCard *card)
{
    int size=WSFindChar(str,32,'[');
    char word[16];
    for(int i=0;i<cn;i++)
    {
        size+=WSReadWord(str+size+1,len,word);
		card[i] = CardForName(word);
    }
}
//---------------------------------------------------------------------------
enAction    PSReadAct(char *word)
{
    int len=strlen(word)-1;
    if(word[len]=='.')
        word[len]='\0';
    if(strcmp(word,"calls")==0)
        return ACT_CALL;
    if(strcmp(word,"folds")==0)
        return ACT_FOLD;
//    if(strcmp(word,"raises")==0)
//        return ACT_RAISE;
//    if(strcmp(word,"raisesto")==0)
//        return ACT_RAISE_TO;
    if(strcmp(word,"checks")==0)
        return ACT_CHECK;
    if(strcmp(word,"bets")==0)
        return ACT_BET;
    if(strcmp(word,"shows")==0)
        return ACT_SHOW;
    if(strcmp(word,"mucks")==0)
        return ACT_MUCK;
//    if(strcmp(word,"doesn't")==0)
//        return ACT_SHOW;
    if(strcmp(word,"collected")==0 || strcmp(word,"ties")==0)
        return ACT_WIN;
    if(strcmp(word,"isall-In")==0)
        return ACT_ALL_IN;
    if(strcmp(word,"finished")==0)
        return ACT_FINISH;
    if(strcmp(word,"Uncalled")==0)
        return ACT_RETURN_UNCALLED_BET;
    return ACT_UNDEF;
}
//---------------------------------------------------------------------------
void    PSReadAction(int nb,char *str,int len)
{
    char word[32];
    int size,money,toReadAct=len;
    enAction act;
    clHand hand;
    size=WSReadWord(str,len,word);
    if(strcmp(word,"raises")==0)
    {
        WSStrFind(str+size,"to",len-size,toReadAct);
//        toReadAct=len-toReadAct;
        size+=toReadAct;
        act=ACT_RAISE_TO;
    }
    else
        act=PSReadAct(word);
    switch(act)
    {
        case ACT_FOLD :
        case ACT_CHECK :glPSPtrGame->AddAct(nb,act,0);break;
        case ACT_CALL :
        case ACT_BET  :
//        case ACT_RAISE :
        case ACT_RETURN_UNCALLED_BET :
        case ACT_RAISE_TO :
//            if(glPSPtrGame->_tpTrny==TPT_TRN_CASH)
//                money=WSReadMoney(str+size,len,size);
//            else
                money=WSReadChips(str+size,len,size);
            glPSPtrGame->AddAct(nb,act,money);
            break;
        case ACT_ALL_IN  : glPSPtrGame->AddAct(nb,act,0);break;
        case ACT_WIN  :
//            if(glPSPtrGame->_tpTrny==TPT_TRN_CASH)
//                money=WSReadMoney(str+size,len,size);
//            else
                money=WSReadChips(str+size,len,size);
            glPSPtrGame->AddAct(nb,act,money);
            break;
        case ACT_SHOW :
            {
                clCard *ptrC=glPSPtrGame->PlayerCards(nb);
                PSReadCards(str+size,len,2,ptrC);
                glPSPtrGame->AddAct(nb,act,0);
                break;
            }
        case ACT_MUCK :
            {
                if(!glPSPtrGame->CheckActShow(nb))
                    glPSPtrGame->AddAct(nb,ACT_SHOW,0);
                break;
            }
//        case ACT_FINISH : glPSPtrGame->AddAct(nb,act,PSReadPlace(str+size,len));
//                        break;
    }
}
//---------------------------------------------------------------------------
/*int    PSReadPlace(char *str,int len)
{
    char word[32];
    int size=WSReadWord(str,len,word);
    if(strcmp(word,"in") != 0)
        return -1;
    WSReadWord(str+size,len,word);
//    if(strcmp(word,"first") == 0)
//        return 1;
    if(strcmp(word,"second") == 0)
        return 2;
    if(strcmp(word,"third") == 0)
        return 3;
    if(strcmp(word,"fourth") == 0)
        return 4;
    if(strcmp(word,"fifth") == 0)
        return 5;
    if(strcmp(word,"sixth") == 0)
        return 6;
    if(strcmp(word,"seventh") == 0)
        return 7;
    if(strcmp(word,"eighth") == 0)
        return 8;
    if(strcmp(word,"ninth") == 0)
        return 9;
    if(strcmp(word,"tenth") == 0)
        return 10;
    return -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*int     PSAnalisStrEmail(char *str,int len)
{
    int size0;
    switch(glPSReadAct)
    {
        case READ_BEGINGAME :
            size0=(len>10)? 10 : len;
            if(WSStrFind(str,"Hand History for Game",size0,size0))
            {
                char word[SIZE_NB_GAME];
                WSReadWord(str+size0,len,word);
                glPSPtrGame->WrNbGame(word);
                glPSReadAct=READ_GAMEINFO;
                return 0;
            }
            break;
        case READ_GAMEINFO :
            if(WSStrFind(str,"Tournament",len,size0))
            {
                int size1=size0;
                glPSPtrGame->_nbTrny=WSReadInt(str+size1,len-size1,size0);
                size1+=size0;
                WSReadInt(str+size1,len-size1,size0);
                size1+=size0;
                clDataTime t0=ReadTime(str+size1,len-size1,size0);
                if(t0<glParamH._time)
                {
                    glParamH._time=t0;
                    glParamH._data.NextDay();
                }
                glPSPtrGame->WriteDate(glParamH._data,glParamH._time);
                glPSPtrGame->_timeLevel=glParamH._timeLevel;
                glPSPtrGame->WrNbTable(glParamH._nbTable);
                glPSPtrGame->_tableCost=glParamH._buyIn;
                glPSPtrGame->_tpP=glParamH._tpP;
                glPSPtrGame->_tpTrny=glParamH._tpTrny;
                glPSReadAct=READ_BUTTON;
                return 0;
            }
            break;
         case READ_BUTTON :
            {
                int size1=WSFindChar(str,len,'-');
                if(WSStrFind(str+size1,"is the button",len,size0))
                {
                    glPSPtrGame->WrNbButton(WSReadInt(str+size1+4,len,size0)-1);
                    glPSReadAct=READ_CNPLAYER;
                    return 0;
                }
                return -1;
            }
        case READ_CNPLAYER :
            if(WSStrFind(str,"Total number of players",0,size0))
            {
                glPSPtrGame->WrCnPlayer(WSReadInt(str+size0,len,size0));
                glPSReadAct=READ_SEATINFO;
                return 1;
            }
            return -1;
        case READ_SEATINFO :
            if(WSStrFind(str,"Seat ",0,size0))
            {
                int nb,many;
                char name[MAX_LEN_NAME];
                nb=WSReadInt(str,len,size0)-1;
                size0+=WSReadWord(str+size0+1,len,name);
                many=WSReadInt(str+size0+1,len,size0);
                glPSPtrGame->WrSeatInfo(nb,name,many);
                return 0;
            }
            else if(WSStrFind(str,"posts",len,size0))
            {
                char name[MAX_LEN_NAME];
                WSReadWord(str,len,name);
                int size1,nb=glPSPtrGame->FindSeat(name);
                if(nb != -1)
                {
                    if(WSStrFind(str+size0,"ante",len-size0,size1))
                    {
                        size0+=size1;
                        glPSPtrGame->AddAct(nb,ACT_POSTANTE,WSReadInt(str+size0,len-size0,size1));
                    }
                    if(WSStrFind(str+size0,"small",len-size0,size1))
                    {
                        size0+=size1;
                        glPSPtrGame->PlayerPostSB(nb,WSReadInt(str+size0,len-size0,size1));
                    }
                    else if(WSStrFind(str+size0,"big",len-size0,size1))
                    {
                        size0+=size1;
                        glPSPtrGame->PlayerPostBB(nb,WSReadInt(str+size0,len-size0,size1));
                    }
                    return 0;
                }
            }
            else if(WSStrFind(str,"Dealing down cards",4,size0))
            {
                glPSPtrGame->AddAct(NB_DILER,ACT_DELCARD,0);
                glPSReadAct=READ_ACT;
                return 0;
            }
            break;

        case READ_ACT :
         {
            int size=0;
            char word[4],name[MAX_LEN_NAME];
            if(WSStrFind(str,"Dealt to",0,size))
            {
                size+=WSReadWord(str+size,len,name);
                clCard *ptrC=glPSPtrGame->Cards(glPSPtrGame->FindSeat(name));
                ReadCards(str+size,len,2,ptrC);
                glPSPtrGame->AddAct(name,ACT_SHOW,0);
                return 0;
            }
            if(WSStrFind(str,"**",0,size0))
            {
                if(WSStrFind(str+3,"Dealing Flop",0,size))
                {
                    clCard card[3];
                    ReadCards(str+size+3,len,3,card);
                    glPSPtrGame->WrFlop(card);
                    return 0;
                }
                if(WSStrFind(str+3,"Dealing Turn",0,size))
                {
                    clCard card;
                    ReadCards(str+size+3,len,1,&card);
                    glPSPtrGame->WrTurn(card);
                    return 0;
                }
                if(WSStrFind(str+3,"Dealing River",0,size))
                {
                    clCard card;
                    ReadCards(str+size+3,len,1,&card);
                    glPSPtrGame->WrRiver(card);
                    return 0;
                }
                if(WSStrFind(str+3,"Summary",0,size))
                    return 0;
                return -1;
            }
            size=WSReadWord(str,len,name);
            int nb=glPSPtrGame->FindSeat(name);
            if(nb != -1)
            {
                if(WSStrFind(str+size+1,"balance",0,size0))
                {
                    size+=size0;
                    ReadBalance(nb,str+size,len-size);
                }
                else
                    ReadAction(nb,str+size,len-size);
            }
            return 0;
         }
         default : return -1;
    }
    return 0;
}*/
//---------------------------------------------------------------------------
/*
void    PSReadBalance(int nb,char *str,int len)
{
    int size0,size=WSFindChar(str,len,',');
    char word[32];
    size+=WSReadWord(str+size,len-size,word);
    if(strcmp(word,"bet")==0)
    {
        size+=WSFindChar(str+size,len-size,',');
        size+=WSFindChar(str+size,len-size,',');
        int many=WSReadInt(str+size,len-size,size0);
        glPSPtrGame->AddAct(nb,ACT_WIN,many);
        size+=size0;
        size0=WSFindChar(str+size,len-size,'[');
        if(size0 != -1)
        {
            clCard *ptrC=glPSPtrGame->Cards(nb);
            PSReadCards(str+size,len-size,2,ptrC);
            glPSPtrGame->AddAct(nb,ACT_SHOW,0);
        }
    }
    else if(strcmp(word,"lost")==0)
    {
        int many=WSReadInt(str+size,len-size,size0);
        glPSPtrGame->AddAct(nb,ACT_WIN,-many);
        size+=size0;
        size0=WSFindChar(str+size,len-size,'[');
        if(size0 != -1)
        {
            clCard *ptrC=glPSPtrGame->Cards(nb);
            PSReadCards(str+size,len-size,2,ptrC);
            glPSPtrGame->AddAct(nb,ACT_SHOW,0);
        }
    }
}

*/
//---------------------------------------------------------------------------
int     PSReadHHReadMoney(char *str,int len,int &size)
{
    int x1=WSReadInt(str,len,size)*100;
    if(str[size]=='.')
        if(WSSmblIsDig(str[size+1]))
        {
            int size1;
            double x2=WSReadInt(str+size,len-size-1,size1);
            if(size1==1)
                x2*=10;
            x1+=x2;
            size+=size1;
        }
    return x1;
}
//---------------------------------------------------------------------------
#define PSRH_SIZE_IN_FILE    3000
int    PSReadHistoryFLastHist(int handle,clGameHistory *hist)
{
    clListHistory listH;
    char *buf;
    int size,len=_filelength(handle);
    if(len==0) return(0);
    buf=(char*)malloc(len);
    size=(len>PSRH_SIZE_IN_FILE)? PSRH_SIZE_IN_FILE : len;
    _lseek(handle,-size,SEEK_END);
    size=_read(handle,buf,size);
    PSReadHistory_HH(buf,size-1,&listH,true);
    free(buf);
    if(listH.CnGame()>0)
    {
        *hist=*listH.LastGameHist();
        return size;
    }
    return 0;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
