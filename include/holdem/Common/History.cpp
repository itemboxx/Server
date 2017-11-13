//---------------------------------------------------------------------------
////#include "stdafx.h"
#pragma hdrstop

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>

#include "..\..\Util\WorkStr.h"
#include "History.h"

//enSite  globalSite=SITE_UNDEF;

tpDis TPDIS_UNDEF={ACT_UNDEF,0};
//---------------------------------------------------------------------------
bool tpSeat::operator == (tpSeat &seat)
{
    if(_money != seat._money) return false;
    if(_inPlay != seat._inPlay) return false;
    if(_busy != seat._busy) return false;
    return true;
}
//---------------------------------------------------------------------------
clListHistory::clListHistory()
{
    _cn=_dim=0;
    _ptrLH=NULL;
}
//---------------------------------------------------------------------------
clListHistory::~clListHistory()
{
    Clear();
}
//---------------------------------------------------------------------------
void    clListHistory::Clear()
{
    for(int i=0;i<_cn;i++)
        delete _ptrLH[i];
    free(_ptrLH);
    _ptrLH=NULL;
    _cn=_dim=0;
}
//---------------------------------------------------------------------------
void    clListHistory::Clear(int nb)  //Очистить все кроме nb
{
    clGameHistory *ptr=_ptrLH[nb];
    for(int i=0;i<_cn;i++)
        if(i != nb)
            delete _ptrLH[i];
    _ptrLH[0]=ptr;
    _cn=1;
}
//---------------------------------------------------------------------------
void    clListHistory::operator =(clListHistory &list)
{
    Clear();
    _cn=list._cn;
    _dim=list._dim;
    _ptrLH=(clGameHistory**)malloc(_dim*sizeof(clGameHistory*));
    for(int i=0;i<_cn;i++)
    {
        _ptrLH[i]=new clGameHistory;
        *_ptrLH[i]=*list._ptrLH[i];
    }
}
//---------------------------------------------------------------------------
bool clListHistory::Add(clGameHistory *ptrH)
{
    if(_cn==_dim)
    {
        _dim+=1000;
        _ptrLH=(clGameHistory**)realloc(_ptrLH,_dim*sizeof(clGameHistory*));
#ifdef  ERR_MESSAGE
        if(_ptrLH==NULL)
            ErrMessage("Нет памяти","clListHistory::Add");
#endif
    }
    _ptrLH[_cn]=ptrH;
    _cn++;
    return true;
}
//---------------------------------------------------------------------------
bool clListHistory::AddCopy(clGameHistory *ptrH)
{
    if(_cn==_dim)
    {
        _dim+=1000;
        _ptrLH=(clGameHistory**)realloc(_ptrLH,_dim*sizeof(clGameHistory*));
#ifdef  ERR_MESSAGE
        if(_ptrLH==NULL)
            ErrMessage("Нет памяти","clListHistory::Add");
#endif
    }
    _ptrLH[_cn]=new clGameHistory;
    *_ptrLH[_cn]=*ptrH;
    _cn++;
    return true;
}
//---------------------------------------------------------------------------
void clListHistory::InsertCopy(clGameHistory *ptrH,int nb)
{
    if(nb>_cn)
        return;
    if(_cn==_dim)
    {
        _dim+=1000;
        _ptrLH=(clGameHistory**)realloc(_ptrLH,_dim*sizeof(clGameHistory*));
#ifdef  ERR_MESSAGE
        if(_ptrLH==NULL)
            ErrMessage("Нет памяти","clListHistory::Add");
#endif
    }
    _ptrLH[_cn]=new clGameHistory;
    _cn++;
    for(int i=_cn-1;i>nb;i--)
        _ptrLH[i]=_ptrLH[i-1];
    *_ptrLH[nb]=*ptrH;
}
//---------------------------------------------------------------------------
void    clListHistory::DelGameHist(int nb)
{
    if(nb>=_cn)
        return;
    delete _ptrLH[nb];
    _cn--;
    memcpy(_ptrLH+nb,_ptrLH+nb+1,(_cn-nb)*sizeof(clGameHistory*));
//    _ptrLH=(clGameHistory**)realloc(_ptrLH,_cn*sizeof(clGameHistory*));
}
//---------------------------------------------------------------------------
void   clListHistory::RepairListHistToStraight()
{
    for(int i=0;i<_cn;i++)
    {
        int nb=i;
        clGameHistory *hist=_ptrLH[i];
        for(int j=i+1;j<_cn;j++)
        {
            stDayTime d1=_ptrLH[j]->Date();
            stDayTime d2=hist->Date();
            if(d1<d2)
            {
                nb=j;
                hist=_ptrLH[j];
            }
        }
        _ptrLH[nb]=_ptrLH[i];
        _ptrLH[i]=hist;
    }
}
//---------------------------------------------------------------------------
int     clListHistory::CnTrny()
{
    int nb=0,cn=0;
    for(int i=0;i<_cn;i++)
    {
        int nb0=_ptrLH[i]->NbTrny();
        if( nb0 != nb && nb0>0)
        {
            nb=nb0;
            cn++;
        }
    }
    return cn;
}
//---------------------------------------------------------------------------
/*void    clListHistory::Divide(clListHistory *ptrL)
{
    int nb=ptrL->_ptrLH[0]->_nbTrny,i=1;
    for(;i<ptrL->_cn;i++)
    {
        int nb0=ptrL->_ptrLH[i]->_nbTrny;
        if(nb0 != nb && nb0>0)
            break;
    }
    _cn=_dim=i;
    _ptrLH=(clGameHistory**)malloc(_cn*sizeof(clGameHistory*));
    for(i=0;i<_cn;i++)
        _ptrLH[i]=ptrL->_ptrLH[i];
    for(;i<ptrL->_cn;i++)
        ptrL->_ptrLH[i-_cn]=ptrL->_ptrLH[i];
    ptrL->_cn-=_cn;
} */
//---------------------------------------------------------------------------
int     clListHistory::WriteFile(int handle)
{
    _write(handle,&_cn,sizeof(int));
    for(int i=0;i<_cn;i++)
        _ptrLH[i]->WriteFile(handle);
    return 0;
}
//---------------------------------------------------------------------------
int     clListHistory::ReadFile(int handle)
{
    Clear();
    _read(handle,&_cn,sizeof(int));
    _dim=_cn;
    _ptrLH=(clGameHistory**)malloc(_cn*sizeof(clGameHistory*));
    for(int i=0;i<_cn;i++)
    {
        _ptrLH[i]=new clGameHistory;
        _ptrLH[i]->ReadFile(handle);
    }
    return 0;
}
//---------------------------------------------------------------------------
/*void    clListHistory::Caption(char *str)
{
    if(_cn>0)
    {
        clGameHistory *ptrH=_ptrLH[0];
        itoa(ptrH->Date()._day._day,str,10);
        strcat(str,"  ");
        strcat(str,ptrH->PlayerName(ptrH->ViewPlayer()));
        strcat(str,"  ");
        itoa(ptrH->_tableCost,str+strlen(str),10);
        strcat(str,"$");
    }
    else
        str[0]='\0';
}
//---------------------------------------------------------------------------
AnsiString    clListHistory::Name()
{
    clGameHistory *ptr=_ptrLH[0];
    AnsiString as=ptr->_tableCost;
    as+='_';
    as+=ptr->PlayerName(ptr->ViewPlayer());
    return as;
}
//---------------------------------------------------------------------------
int     clListHistory::SizeInBuf()
{
    int size=sizeof(int);
    for(int i=0;i<_cn;i++)
        size+=sizeof(clGameHistory)+_ptrLH[i]->CnAct()*sizeof(tpAction);
    return size;
}
//---------------------------------------------------------------------------
void    clListHistory::ReadBuf(byte *buf)
{
    Clear();
    _cn=*((int*)buf);
    buf+=sizeof(int);
    clGameHistory *ptrGH;
    for(int i=0;i<_cn;i++)
    {
        ptrGH=new(clGameHistory);
        *ptrGH=*((clGameHistory*)buf);
        buf+=sizeof(clGameHistory);
        Add(ptrGH);
    }
}
//---------------------------------------------------------------------------
void    clListHistory::WriteBuf(byte *buf)
{
    *((int*)buf)=_cn;
    buf+=sizeof(int);
    for(int i=0;i<_cn;i++)
    {
        *((clGameHistory*)buf)=*_ptrLH[i];
        buf+=sizeof(clGameHistory);
    }
} */
//---------------------------------------------------------------------------
//***************************************************************************
//---------------------------------------------------------------------------
void    tpArrAct::ReadFile(int handle)
{
    _read(handle,&_cnAct,sizeof(_cnAct));
    _read(handle,_act,_cnAct*sizeof(tpAction));
}
//---------------------------------------------------------------------------
void    tpArrAct::WriteFile(int handle)
{
    _write(handle,&_cnAct,sizeof(_cnAct));
    _write(handle,_act,_cnAct*sizeof(tpAction));
}
//---------------------------------------------------------------------------
inline  int tpArrAct::SizeInFile()
{
    return sizeof(_cnAct)+_cnAct*sizeof(tpAction);
}
//---------------------------------------------------------------------------
void    tpGameHH::Clear()
{
    _cnPl=0;
    _maxSeat=CN_PLAYER;
}
//---------------------------------------------------------------------------
//***************************************************************************
//---------------------------clGameHistory-----------------------------------
clGameHistory::clGameHistory()
{
    Clear();
}
//---------------------------------------------------------------------------
clGameHistory::~clGameHistory()
{
}
//---------------------------------------------------------------------------
void clGameHistory::Clear()
{
    _atrHH.Clear();
    _nameTable[0]='\0';
    for(int i=0;i<CN_PLAYER;i++)
    {
        _seat[i].Clear();
        _arrNbPos[i]=-1;
    }
    _arrAct.Clear();
}
//---------------------------------------------------------------------------
void    clGameHistory::operator =(clGameHistory &hist)
{
    _atrHH=hist._atrHH;
    strcpy_s(_nameTable,hist._nameTable);
    for(int i=0;i<CN_PLAYER;i++)
    {
        _arrNbPos[i]=hist._arrNbPos[i];
        _seat[i]=hist._seat[i];
    }
    _arrAct=hist._arrAct;
}
//---------------------------------------------------------------------------
bool    clGameHistory::operator ==(clGameHistory &hist)
{
	if (_arrAct._cnAct != hist._arrAct._cnAct)
		return false;
	for (int i = 0; i < _arrAct._cnAct; i++)
		if (_arrAct._act[i] != hist._arrAct._act[i])
			return false;
	return true;
}
//---------------------------------------------------------------------------
int     clGameHistory::ReadFile(int handle)
{
//    long sh=tell(handle);
    _read(handle,&_atrHH,sizeof(_atrHH));
    BYTE size;
    _read(handle,&size,sizeof(size));
    _read(handle,_nameTable,size);
    _read(handle,_arrNbPos,CN_PLAYER);
    _read(handle,_seat,_atrHH._cnPl*sizeof(tpSeat));
    _arrAct.ReadFile(handle);
	return 0;
}
//---------------------------------------------------------------------------
int     clGameHistory::WriteFile(int handle)
{
    _write(handle,&_atrHH,sizeof(_atrHH));
	BYTE size = (BYTE)strlen(_nameTable) + (BYTE)1;
    _write(handle,&size,sizeof(size));
    _write(handle,_nameTable,size);
    _write(handle,_arrNbPos,CN_PLAYER);
    _write(handle,_seat,_atrHH._cnPl*sizeof(tpSeat));
    _arrAct.WriteFile(handle);
	return 0;
}
//---------------------------------------------------------------------------
int clGameHistory::SizeInFile()
{
    int size=sizeof(_atrHH);
    size+=(int)strlen(_nameTable)+2;
    size+=_atrHH._cnPl*(sizeof(tpSeat)+1);
    size+=_arrAct.SizeInFile();
    return size;
}
//---------------------------------------------------------------------------
int clGameHistory::CnPlayerInPlay()
{
    int cn=0;
    for(int i=0;i<CN_PLAYER;i++)
        if(_seat[i]._busy && _seat[i]._inPlay)
            cn++;
    return(cn);
}
//---------------------------------------------------------------------------
int     clGameHistory::ViewPlayer()
{
    int i;
    for(i=0;i<_arrAct._cnAct;i++)
        if(_arrAct._act[i]._dis._act==ACT_DELCARD)
            break;
    if(i<_arrAct._cnAct-1)
        if(_arrAct._act[i+1]._dis._act==ACT_SHOW)
            return _arrAct._act[i+1]._pos;
    return POS_UNDEF;
}
//---------------------------------------------------------------------------
void    clGameHistory::WrSeatInfo(int nb,char *name,int money,bool inPlay)
{
    _seat[nb]._busy=true;
    _seat[nb]._inPlay=inPlay;
    strcpy_s(_seat[nb]._name,name);
    _seat[nb]._money=money;
}
//---------------------------------------------------------------------------
/*void    clGameHistory::WrPlayerOut(int nb)
{
    _cnPl--;
    _arrSeat[nb].Clear();
}
*/
//---------------------------------------------------------------------------
int clGameHistory::AddAct(int nb,enAction act,int money)
{
#ifdef  ERR_MESSAGE
    if(_arrAct._cnAct>=MAX_CN_ACTHISTORY)
    {
        ErrMessage("clGameHistory::AddAct : Число актов больше MAX_CN_ACTHISTORY","Error");
        return -1;
    }
#endif
    _arrAct._act[_arrAct._cnAct]._dis._act=act;
    _arrAct._act[_arrAct._cnAct]._dis._money=money;
    _arrAct._act[_arrAct._cnAct]._pos=nb;
    _arrAct._cnAct++;
    return _arrAct._cnAct-1;
}
//---------------------------------------------------------------------------
int     clGameHistory::InsertAct(int nbAct,tpAction act)
{
#ifdef  ERR_MESSAGE
    if(_arrAct._cnAct>=MAX_CN_ACTHISTORY)
    {
        ErrMessage("clGameHistory::InsAct : Число актов больше MAX_CN_ACTHISTORY","Error");
        return -1;
    }
#endif
    for(int i=_arrAct._cnAct;i>nbAct;i--)
        _arrAct._act[i]=_arrAct._act[i-1];
    _arrAct._act[nbAct]=act;
    _arrAct._cnAct++;
    return nbAct;
}
bool    clGameHistory::DelAct(int nbAct)
{
    for(int i=nbAct;i<_arrAct._cnAct;i++)
        _arrAct._act[i]=_arrAct._act[i+1];
    _arrAct._cnAct--;
	return true;
}
//---------------------------------------------------------------------------
/*void    clGameHistory::WrAct(int nbAct,tpAction act)
{
    if(nbAct>=_cnAct)
        AddAct(&act);
    else
        _arrAct[nbAct]=act;
}
*/
//---------------------------------------------------------------------------
int     clGameHistory::FindSeat(char *name)
{
    for(int i=0;i<CN_PLAYER;i++)
        if(_seat[i]._busy)
            if(strcmp(_seat[i]._name,name)==0)
                return i;
    return -1;
}
//---------------------------------------------------------------------------
void    clGameHistory::WrFlop(clCard *card)
{
    memcpy(_atrHH._cardTable,card,3*sizeof(clCard));
    AddAct(NB_DILER,ACT_DELFLOP,0);
}
//---------------------------------------------------------------------------
void    clGameHistory::WrTurn(clCard card)
{
    _atrHH._cardTable[3]=card;
    AddAct(NB_DILER,ACT_DELTURN,0);
}
//---------------------------------------------------------------------------
void    clGameHistory::WrRiver(clCard card)
{
    _atrHH._cardTable[4]=card;
    AddAct(NB_DILER,ACT_DELRIVER,0);
}
//---------------------------------------------------------------------------
void    clGameHistory::DilDelt0(bool blind)
{
/*
    int money,nb,ante=Ante();
    if(_atrHH._tpGame != GAME_CASH)
        if(blind)
        {
            if(CnPlayer()==2)
            {
                money=(ante<PlMoney(_atrHH._nbBtn))? PlMoney(_atrHH._nbBtn)-ante : 0;
                money=(SmallBlind()<money)? SmallBlind():money;
                AddAct(_atrHH._nbBtn,ACT_POSTSB,money);
                nb=NextPlayerInPlay(_atrHH._nbBtn);
                money=(ante<PlMoney(nb))? PlMoney(nb)-ante : 0;
                money=(_atrHH._bb<money)? _atrHH._bb:money;
                AddAct(nb,ACT_POSTBB,money);
            }
            else
            {
                nb=NextPlayerInPlay(_atrHH._nbBtn);
                money=(ante<PlMoney(nb))? PlMoney(nb)-ante : 0;
                money=(SmallBlind()<money)? SmallBlind():money;
                AddAct(nb,ACT_POSTSB,money);
                nb=NextPlayerInPlay(nb);
                money=(ante<PlMoney(nb))? PlMoney(nb)-ante : 0;
                money=(_atrHH._bb<money)? _atrHH._bb:money;
                AddAct(nb,ACT_POSTBB,money);
            }
        }
        else
        {
            nb=NextPlayerInPlay(_atrHH._nbBtn);
            money=(ante<PlMoney(nb))? PlMoney(nb)-ante : 0;
            money=(_atrHH._bb<money)? _atrHH._bb:money;
            AddAct(nb,ACT_POSTBB,money);
        }
    AddAct(NB_DILER,ACT_DELCARD,0);
*/
}
//---------------------------------------------------------------------------
int     clGameHistory::NextPlayer(int nb)
{
    for(int i=0;i<CN_PLAYER;i++)
    {
        nb++;
        if(nb>=CN_PLAYER)
            nb=0;
        if(_seat[nb]._busy)
            return nb;
    }
    ErrMessage("clGameHistory::NextPlayer","Нет игроков");
    return -1;
}
//---------------------------------------------------------------------------
int     clGameHistory::PrevPlayer(int nb)
{
    for(int i=0;i<CN_PLAYER;i++)
    {
        nb--;
        if(nb<0)
            nb=CN_PLAYER-1;
        if(_seat[nb]._busy)
            return nb;
    }
    ErrMessage("clGameHistory::PrevPlayer","Нет игроков");
    return -1;
}
//---------------------------------------------------------------------------
int     clGameHistory::NextPlayerInPlay(int nb)
{
    for(int i=0;i<CN_PLAYER;i++)
    {
        nb++;
        if(nb>=CN_PLAYER)
            nb=0;
        if(_seat[nb]._busy && _seat[nb]._inPlay)
            return nb;
    }
    ErrMessage("clGameHistory::NextPlayerInPlay","Нет игроков");
    return -1;
}
//---------------------------------------------------------------------------
int     clGameHistory::PrevPlayerInPlay(int nb)
{
    for(int i=0;i<CN_PLAYER;i++)
    {
        nb--;
        if(nb<0)
            nb=CN_PLAYER-1;
        if(_seat[nb]._busy && _seat[nb]._inPlay)
            return nb;
    }
    ErrMessage("clGameHistory::PrevPlayerInPlay","Нет игроков");
    return -1;
}
//---------------------------------------------------------------------------
/*int     clGameHistory::NbBigBlind()
{
    if(_atrHH._site==SITE_POKERSTARS)
    {
        if(_atrHH._cnPl==2)
            return NextPlayerInPlay(_atrHH._nbBtn);
        int nb=NextPlayer(_atrHH._nbBtn);
        return NextPlayer(nb);
    }
    int i;
    for(i=0;i<_arrAct._cnAct;i++)
        if(_arrAct._act[i]._dis._act==ACT_POSTBB)
            return _arrAct._act[i]._nbSeat;
    return -1;
} */
//---------------------------------------------------------------------------
/*int     clGameHistory::Ante()
{
    for(int i=0;i<_arrAct._cnAct;i++)
        if(_arrAct._act[i]._dis._act==ACT_DELCARD)
            return 0;
        else if(_arrAct._act[i]._dis._act==ACT_POSTANTE)
            if(_arrAct._act[i]._dis._money<PlMoney(_arrAct._act[i]._nbSeat))
                return _arrAct._act[i]._dis._money;
    return 0;
} */
//---------------------------------------------------------------------------
/*void    clGameHistory::CheckBlinds()
{
    int ante=Ante();
    _bb=40;
    int i=0;
    for(;i<_cnAct;i++)
        if(_arrAct[i]._dis._act==ACT_POSTSB)
        {
            if(_arrAct[i]._dis._money+ante<PlMoney(_arrAct[i]._nbSeat))
                _bb=2*_arrAct[i]._dis._money;
        }
        else if(_arrAct[i]._dis._act==ACT_POSTBB)
        {
            if(_arrAct[i]._dis._money+ante<PlMoney(_arrAct[i]._nbSeat))
                _bb=_arrAct[i]._dis._money;
            break;
        }
}
*/
//---------------------------------------------------------------------------
void    clGameHistory::WrTableCostCash(int cost)
{
    _atrHH._tableCost=cost;
    _atrHH._bb=cost;
}
//---------------------------------------------------------------------------
/*AnsiString clGameHistory::HistCaption()
{
    AnsiString as;
    int nbH=ViewPlayer();
    if(nbH==-1)
        as="noHero";
    else
    {
        as=PlayerName(nbH);
        clCard *card=PlayerCards(nbH);
        as+=" ";
        as+=card[0].Name();
        as+=card[1].Name();
    }
    as+=" ";
    as+=_atrHH._tableCost;
    return as;
}*/
//---------------------------------------------------------------------------
/*int     clGameHistory::CnCardPoker()
{
    if(_tpP==TPP_OMAHA)
        return 4;
    return 2;
}
//---------------------------------------------------------------------------
int     clGameHistory::SizeInBuf()
{
    return sizeof(clGameHistory)+_cnAct*sizeof(tpAction);
}
//---------------------------------------------------------------------------
void    clGameHistory::WriteToBuf(byte *buf)
{
    memcpy(buf,this,sizeof(clGameHistory));
}
//---------------------------------------------------------------------------
void    clGameHistory::ReadFromBuf(byte *buf)
{
    memcpy(this,buf,sizeof(clGameHistory));
}
//---------------------------------------------------------------------------
bool    clGameHistory::IsAct(int nbPl,enAction act)
{
    for(int i=0;i<_cnAct;i++)
        if(_arrAct[i]._nbSeat==nbPl && _arrAct[i]._dis._act==act)
            return true;
    return false;
}*/
//---------------------------------------------------------------------------
int     clGameHistory::NbFirstAct(enAction act)
{
    for(int i=0;i<_arrAct._cnAct;i++)
        if(_arrAct._act[i]._dis._act==act)
            return i;
    return INT_UNDEF;
}
//---------------------------------------------------------------------------
int     clGameHistory::NbFirstTradeAct(int nbAct)
{
    for(int i=nbAct+1;i<_arrAct._cnAct;i++)
        if(HistIsTradeAct(_arrAct._act[i]._dis._act))
            return i;
    return INT_UNDEF;
}
//---------------------------------------------------------------------------
int     clGameHistory::NbTradeActPl(int nbPl,int nbAct)
{
    for(int i=nbAct+1;i<_arrAct._cnAct;i++)
        if(_arrAct._act[i]._pos==nbPl && HistIsTradeAct(_arrAct._act[i]._dis._act))
            return i;
    return INT_UNDEF;
}
//---------------------------------------------------------------------------
/*int     clGameHistory::NextTradeAct(int nbAct,int nbPl)
{
    for(int i=nbAct+1;i<_cnAct;i++)
        if(_arrAct[i]._nbSeat==nbPl && HistIsTradeAct(_arrAct[i]._dis._act))
            return i;
    return INT_UNDEF;
}
//---------------------------------------------------------------------------
bool    clGameHistory::IsCorrect()
{
    if(_cnPl<2 || _cnPl>10)
        return false;
    for(int i=0;i<_cnAct;i++)
        if(_arrAct[i]._dis._act==ACT_WIN)
            return true;
    return false;
}
*/
//---------------------------------------------------------------------------
int     clGameHistory::NbTable()
{
    return atoi(_nameTable);
}
//---------------------------------------------------------------------------
void    clGameHistory::WrNbTable(int nb)
{
	_itoa_s(nb, _nameTable, 10);
}
//---------------------------------------------------------------------------
bool    clGameHistory::CheckActShow(int nbPl)
{
    for(int i=0;i<_arrAct._cnAct;i++)
    {
        if(_arrAct._act[i]._dis._act==ACT_SHOW && _arrAct._act[i]._pos == nbPl)
            return true;
    }
    return false;
}
void    clGameHistory::RestraightNbToPos()
{
    int posPl[CN_PLAYER];
    for(int i=0;i<CN_PLAYER;i++)
        posPl[i]=POS_UNDEF;
    int nb=_arrNbPos[POS_BUTTON];
    posPl[nb]=POS_BUTTON;
    if(CnPlayer()>2)
        nb=NextPlayer(nb);
    _arrNbPos[POS_SB]=nb;
    posPl[nb]=POS_SB;
    nb=NextPlayer(nb);
    _arrNbPos[POS_BB]=nb;
    posPl[nb]=POS_BB;
    nb=_arrNbPos[POS_BUTTON];

    if(CnPlayer()>3)
    {
        int pos=POS_BUTTON;
        for(int i=0;i<CN_PLAYER;i++)
        {
            nb=this->PrevPlayer(nb);
            if(nb==_arrNbPos[POS_BB])
                break;
            pos++;
            _arrNbPos[pos]=nb;
            posPl[nb]=pos;
        }
    }
    tpSeat  seat[CN_PLAYER];
    for(int i=0;i<CN_PLAYER;i++)
        seat[i]=_seat[i];
    for(int i=0;i<CN_PLAYER;i++)
        if(posPl[i] != POS_UNDEF)
            _seat[posPl[i]]=seat[i];
    for(int i=CnPlayer();i<CN_PLAYER;i++)
        _seat[i].Clear();
    for(int i=0;i<_arrAct._cnAct;i++)
        if(_arrAct._act[i]._pos<CN_PLAYER)
            _arrAct._act[i]._pos=posPl[_arrAct._act[i]._pos];
}
//---------------------------------------------------------------------------
int     clGameHistory::NbToPosition(int nb)
{
    for(int i=0;i<CN_PLAYER;i++)
        if(_arrNbPos[i]==nb)
            return i;
    return POS_UNDEF;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
clAnsiString  HistDisToString(tpDis dis)
{
    clAnsiString as;
    switch(dis._act)
    {
        case ACT_FOLD  : return "Fold";
        case ACT_CHECK : return "Check";
        case ACT_CALL  : as="Call ";as+=(clAnsiString)dis._money;break;
        case ACT_SHOW  : return "Show";
		case ACT_BET: as = "Bet "; as += (clAnsiString)dis._money; break;
		case ACT_RAISE: as = "Raise "; as += (clAnsiString)dis._money; break;
		case ACT_RAISE_TO: as = "Raise_To "; as += (clAnsiString)dis._money; break;
		case ACT_ALL_IN: as = "All-In "; as += (clAnsiString)dis._money; break;
		case ACT_POSTSB: as = "PostSB "; as += (clAnsiString)dis._money; break;
		case ACT_POSTBB: as = "PostBB "; as += (clAnsiString)dis._money; break;
		case ACT_POSTANTE: as = "PostAnte "; as += (clAnsiString)dis._money; break;
		case ACT_POST_DEAD:as = "PostDead "; as += (clAnsiString)dis._money; break;
		case ACT_DELFLOP: as = "DealFlop "; break;
		case ACT_DELTURN: as = "DealTurn "; break;
		case ACT_DELRIVER:as = "DealRiver "; break;
		default:   return "Undef";
    }
    return as;
}
//---------------------------------------------------------------------------
tpDis   HistStringToDis(char *str,int &shift)
{
    char word[32];
    int len=(int)strlen(str);
    shift=WSReadWord(str,len,word);
    tpDis res;res._money=0;
    bool readM=false;
    if(strcmp(word,"Fold")==0) res._act=ACT_FOLD;
    else if(strcmp(word,"Check")==0) res._act=ACT_CHECK;
    else if(strcmp(word,"Call")==0)
    {   res._act=ACT_CHECK; readM=true;}
    else if(strcmp(word,"Bet")==0)
    {   res._act=ACT_BET; readM=true;}
    else if(strcmp(word,"Raise")==0)
    {   res._act=ACT_RAISE; readM=true;}
    else if(strcmp(word,"All-In")==0)
    {   res._act=ACT_ALL_IN; readM=true;}
    if(readM)
    {
        int size;
        res._money=WSReadInt(str+shift,len-shift,size);
        shift+=size;
    }
    return res;
}
bool AccuratDis(tpDis disRef, tpDis disCheck)
{
	if (disRef == disCheck)
		return true;
	switch (disRef._act)
	{
	case ACT_CHECK:
	case ACT_FOLD:
	case ACT_CALL:
		return disRef._act == disCheck._act;
		break;
	case ACT_ALL_IN:
		if (disCheck._act == ACT_RAISE || 
			disCheck._act == ACT_BET || 
			disCheck._act == ACT_ALL_IN ||
			disCheck._act == ACT_CALL)
			return (disRef._money <= disCheck._money);
		else
			return false;
		break;
	case ACT_BET:
	case ACT_RAISE:
		return(disRef._act == disCheck._act &&
			(disRef._money / 2) >= disCheck._money &&
			(disRef._money * 2) <= disCheck._money);
		break;
	}
	return true;
}
//---------------------------------------------------------------------------
clAnsiString  HistActToString(tpAction act)
{
	clAnsiString  as = HistPosToString(act._pos);
	as += " ";
	as += HistDisToString(act._dis);
	return as;
}
//---------------------------------------------------------------------------
clAnsiString  HistActToString(enAction act)
{
	switch (act)
	{
	case ACT_FOLD: return "Fold";
	case ACT_CHECK: return "Check";
	case ACT_CALL: return "Call";
	case ACT_BET: return "Bet ";
	case ACT_RAISE_TO:
	case ACT_RAISE: return "Raise ";
	default:   return "Undef";
	}
}
//---------------------------------------------------------------------------
clAnsiString  HistPosToString(int pos)
{
    switch(pos)
    {
        case POS_BB    : return "BB";
        case POS_SB    : return "SB";
        case POS_BUTTON: return "Button";
        case POS_CUTOFF: return "Cutoff";
        case POS_MIDDLE: return "Middle";
		case POS_EARLY: return "Early";
		case NB_DILER : return "Dealer";
		default:   return "Undef";
    }
}
//---------------------------------------------------------------------------
int     HistReadFile(int handle,void *ptr)
{
    clGameHistory *ptrH=(clGameHistory *)ptr;
    ptrH->ReadFile(handle);
    return 0;
}
//---------------------------------------------------------------------------
int     HistWriteFile(int handle,void *ptr)
{
    clGameHistory *ptrH=(clGameHistory *)ptr;
    ptrH->WriteFile(handle);
    return 0;
}
//---------------------------------------------------------------------------
int     HistListReadFile(int handle,void *ptr)
{
    clListHistory *ptrH=(clListHistory *)ptr;
    ptrH->ReadFile(handle);
    return 0;
}
//---------------------------------------------------------------------------
int     HistListWriteFile(int handle,void *ptr)
{
    clListHistory *ptrH=(clListHistory *)ptr;
    ptrH->WriteFile(handle);
    return 0;
}
//---------------------------------------------------------------------------
int     HistListSizeInFile(int handle,void *ptr)
{
    clListHistory *ptrH=(clListHistory *)ptr;
    int size=sizeof(int);
    for(int i=0;i<ptrH->CnGame();i++)
        size+=ptrH->GameHist(i)->SizeInFile();
    return size;
}
//---------------------------------------------------------------------------
bool HistInPosition(int nbPl1,int nbPl2) //true if nbPl1 in position
{
    if(nbPl1==POS_SB) return false;
    if(nbPl1==POS_BB) return nbPl2==POS_SB;
    if(nbPl2<POS_BUTTON) return true;
    return nbPl1<nbPl2;
}
//---------------------------------------------------------------------------

