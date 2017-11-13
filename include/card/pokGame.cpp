//---------------------------------------------------------------------------
//#include <vcl.h>
#include <string.h>
#pragma hdrstop

#include "pokGame.h"

char *glNameGame[]={"HiCard","Pair","2 Pair","Set","Straight","Flush",
    "Full House","Care","Straight Flush","Undef"};

void    Straight(clCard *card,int cn);
enFace  IsStraight(enFace *f0,int cn);

clGamePoker  POKER_MIN_GAME(GM_0);
//---------------------------------------------------------------------------
clGamePoker::clGamePoker()
{
    game=GM_UNDEF;
}
//---------------------------------------------------------------------------
clGamePoker::clGamePoker(enGame gm)
{
    game=gm;
    for(int i=0;i<5;i++)
        face[i]=C_2;
}
//---------------------------------------------------------------------------
bool clGamePoker::operator > (clGamePoker gm)
{
    if(game>gm.game)
        return true;
    if(game==gm.game)
        for(int i=0;i<5;i++)
        {
            if(face[i]>gm.face[i])
                return true;
            if(face[i]<gm.face[i])
                return false;
        }
    return false;
}
//---------------------------------------------------------------------------
bool clGamePoker::operator < (clGamePoker gm)
{
    if(game<gm.game)
        return true;
    if(game==gm.game)
        for(int i=0;i<5;i++)
        {
            if(face[i]<gm.face[i])
                return true;
            if(face[i]>gm.face[i])
                return false;
        }
    return false;
}
//---------------------------------------------------------------------------
bool clGamePoker::operator == (clGamePoker gm)
{
    if(game==gm.game)
    {
        for(int i=0;i<5;i++)
            if(face[i]!=gm.face[i])
                return false;
        return true;
    }
    else
        return false;
}
//---------------------------------------------------------------------------
clGamePoker  GameWhatGame(clCard c1,clCard c2,clCard c3,clCard c4,clCard c5)
{
    clCard card[5];
    card[0]=c1;
    card[1]=c2;
    card[2]=c3;
    card[3]=c4;
    card[4]=c5;
    return GameWhatGame(card);
}
//---------------------------------------------------------------------------
clGamePoker  GameWhatGame(clCard *card)
{
    clGamePoker gm;
    gm.game=GM_0;
    Straight(card,5);
    if(card[0].face==card[1].face)
    {
        gm.game=GM_2;
        gm.face[0]=card[0].face;
        gm.face[1]=card[4].face;
        gm.face[2]=card[3].face;
        gm.face[3]=card[2].face;
        gm.face[4]=C_2;
    }
    if(card[1].face==card[2].face)
    {
        if(gm.game==GM_2)
        {
            gm.game=GM_3;
            gm.face[1]=card[4].face;
            gm.face[2]=card[3].face;
            gm.face[3]=C_2;
        }
        else
        {
            gm.game=GM_2;
            gm.face[0]=card[2].face;
            gm.face[1]=card[4].face;
            gm.face[2]=card[3].face;
            gm.face[3]=card[0].face;
            gm.face[4]=C_2;
        }
    }
    if(card[2].face==card[3].face)
        switch(gm.game)
        {
            case GM_0  :
                gm.game=GM_2;
                gm.face[0]=card[3].face;
                gm.face[1]=card[4].face;
                gm.face[2]=card[1].face;
                gm.face[3]=card[0].face;
                gm.face[4]=C_2;
                break;
            case GM_2  :
                if(gm.face[0]==card[3].face)
                {
                    gm.game=GM_3;
                    gm.face[1]=card[4].face;
                    gm.face[2]=card[0].face;
                    gm.face[3]=C_2;
                }
                else
                {
                    gm.game=GM_22;
                    gm.face[0]=card[2].face;
                    gm.face[1]=card[0].face;
                    gm.face[2]=card[4].face;
                    gm.face[3]=C_2;
                }
                break;
            case GM_3  :
                gm.game=GM_4;
                gm.face[1]=card[4].face;
                gm.face[2]=C_2;
                break;
        }
    if(card[3].face==card[4].face)
        switch(gm.game)
        {
            case GM_0  :
                gm.game=GM_2;
                gm.face[0]=card[4].face;
                gm.face[1]=card[2].face;
                gm.face[2]=card[1].face;
                gm.face[3]=card[0].face;
                gm.face[4]=C_2;
                break;
            case GM_2  :
                if(gm.face[0]==card[4].face)
                {
                    gm.game=GM_3;
                    gm.face[1]=card[1].face;
                    gm.face[2]=card[0].face;
                    gm.face[3]=C_2;
                }
                else
                {
                    gm.game=GM_22;
                    gm.face[0]=card[4].face;
                    gm.face[1]=card[1].face;
                    gm.face[2]=(card[1].face==card[2].face)? card[0].face : card[2].face;
                    gm.face[3]=C_2;
                }
                break;
            case GM_22 :
                gm.game=GM_32;
                gm.face[2]=C_2;
                break;
            case GM_3  :
                if(gm.face[0]==card[4].face)
                {
                    gm.game=GM_4;
                    gm.face[1]=card[0].face;
                    gm.face[2]=C_2;
                }
                else
                {
                    gm.game=GM_32;
                    gm.face[1]=card[4].face;
                    gm.face[2]=C_2;
                }
                break;
        }
    if(gm.game != GM_0)
        return gm;
    gm.face[0]=card[4].face;
    gm.face[1]=card[3].face;
    gm.face[2]=card[2].face;
    gm.face[3]=card[1].face;
    gm.face[4]=card[0].face;
    if(card[0].face+1==card[1].face && card[1].face+1==card[2].face && card[2].face+1==card[3].face)
    {
        if(card[3].face+1==card[4].face)
            gm.game=GM_ST;
        if(card[0].face==C_2 && card[4].face==C_A)
        {
            gm.game=GM_ST;
            gm.face[0]=C_5;
            gm.face[1]=C_4;
            gm.face[2]=C_3;
            gm.face[3]=C_2;
            gm.face[4]=C_A;
        }
    }
    if(card[0].suit==card[1].suit && card[1].suit==card[2].suit && card[2].suit==card[3].suit && card[3].suit==card[4].suit)
    {
        if(gm.game==GM_0)
            gm.game=GM_FL;
        else
            gm.game=GM_FS;
    }
    return gm;
}
//---------------------------------------------------------------------------
void    clGamePoker::GameHoldem(clCard *card2,clCard *card5)
{
    clCard  c0[7],card[5];
    memcpy(c0,card2,2*sizeof(clCard));
    memcpy(c0+2,card5,5*sizeof(clCard));
    memcpy(card,card5,5*sizeof(clCard));
    *this=GameWhatGame(card);
    for(int i=0;i<7;i++)
        for(int j=i+1;j<7;j++)
        {
            int nb=0;
            for(int k=0;k<7;k++)
                if(k!=i && k != j)
                    card[nb++]=c0[k];
            clGamePoker gm=GameWhatGame(card);
            if(gm>*this)
                *this=gm;
        }
}
//---------------------------------------------------------------------------
void    clGamePoker::GameHoldem(clCard *card2,clCard *card5,int cn5)
{
    clCard  c0[7],card[5];
    switch(cn5)
    {
        case 3 :
            memcpy(card,card2,2*sizeof(clCard));
            memcpy(card+2,card5,3*sizeof(clCard));
            *this=GameWhatGame(card);
            break;
        case 4 :
            memcpy(c0,card2,2*sizeof(clCard));
            memcpy(c0+2,card5,cn5*sizeof(clCard));
            *this=GameWhatGame(c0);
            for(int i=0;i<5;i++)
            {
                int nb=0;
                for(int k=0;k<6;k++)
                    if(k!=i)
                        card[nb++]=c0[k];
                clGamePoker gm=GameWhatGame(card);
                if(gm>*this)
                    *this=gm;
            }
            break;
        case 5 : GameHoldemQ(card2,card5); break;
    }
}
//---------------------------------------------------------------------------
void    clGamePoker::GameHoldemQ(clCard *card2,clCard *card5)  //Быстрое определение игры
{
    enFace n2[3],n3[2],n4=C_UNDEF;
    int cn=1,cn2=0,cn3=0;
    clCard  card[7];
    memcpy(card,card2,2*sizeof(clCard));
    memcpy(card+2,card5,5*sizeof(clCard));
    Straight(card,7);
    for(int i=0;i<6;i++)
    {
        if(card[i].face==card[i+1].face)
            cn++;
        if(card[i].face!=card[i+1].face || i==5)
            switch(cn)
            {
                case 1 : break;
                case 2 : n2[cn2++]=card[i].face; cn=1; break;
                case 3 : n3[cn3++]=card[i].face; cn=1; break;
                case 4 : n4=card[i].face; cn=1; break;
            }
    }
    if(n4 != C_UNDEF)
    {
        game=GM_4;
        face[0]=n4;
        face[1]=(n4==card[6].face) ? card[2].face : card[6].face;
        face[2]=C_2;
        face[3]=C_2;
        face[4]=C_2;
        return;
    }
    if(cn3==2)
    {
        game=GM_32;
        face[0]=n3[1];
        face[1]=n3[0];
        face[2]=C_2;
        face[3]=C_2;
        face[4]=C_2;
        return;
    }
    if(cn3==1)
    {
        if(cn2>0)
        {
            game=GM_32;
            face[0]=n3[0];
            face[1]=n2[cn2-1];
            face[2]=C_2;
            face[3]=C_2;
            face[4]=C_2;
            return;
        }
        else
        {
            game=GM_3;
            face[0]=n3[0];
            cn=(face[0]==card[6].face) ? 3 : 6;
            face[1]=card[cn].face;
            cn--;
            if(face[0]==card[cn].face)
                cn-=3;
            face[2]=card[cn].face;
            face[3]=C_2;
            face[4]=C_2;
        }
    }
    else if(cn2>1)
    {
        game=GM_22;
        face[0]=n2[cn2-1];
        face[1]=n2[cn2-2];
        cn=6;
        if(card[6].face==face[0])
        {
            if(card[4].face==face[1])
                cn=2;
            else
                cn=4;
        }
        face[2]=card[cn].face;
        face[3]=C_2;
        face[4]=C_2;
    }
    else if(cn2==1)
    {
        game=GM_2;
        face[0]=n2[0];
        cn=6;
        for(int i=0;i<3;i++)
        {
            if(card[cn].face==face[0])
                cn-=2;
            face[i+1]=card[cn--].face;
        }
        face[4]=C_2;
    }
    else
    {
        game=GM_0;
        cn=6;
        for(int i=0;i<5;i++)
            face[i]=card[cn--].face;
    }
    int cnS[]={0,0,0,0};
    enFace f0[7];
    for(int i=0;i<7;i++)
        cnS[card[i].suit]++;
    for(int i=0;i<4;i++)
        if(cnS[i]>=5)
        {
            game=GM_FL;
            cn=0;
            for(int j=0;j<7;j++)
                if(card[j].suit==i)
                    f0[cn++]=card[j].face;
            enFace ff=IsStraight(f0,cn);
            if(ff != C_UNDEF)
            {
                game=GM_FS;
                face[0]=ff;
                face[1]=(enFace)(ff-1);
                face[2]=(enFace)(ff-2);
                face[3]=(enFace)(ff-3);
                face[4]=(ff==C_5)? C_A : (enFace)(ff-4);
            }
            else
                for(int i=0;i<5;i++)
                    face[i]=f0[cn-1-i];
            return;
        }
    cn=1;
    f0[0]=card[0].face;
    for(int i=1;i<7;i++)
        if(card[i-1].face != card[i].face)
            f0[cn++]=card[i].face;
    enFace ff=IsStraight(f0,cn);
    if(ff != C_UNDEF)
    {
        game=GM_ST;
        face[0]=ff;
        face[1]=(ff==C_5) ? C_5 : (enFace)(ff-1);
        face[2]=(enFace)(face[1]-1);
        face[3]=(enFace)(face[1]-2);
        face[4]=(enFace)(face[1]-3);
    }
}
//---------------------------------------------------------------------------
void    clGamePoker::GameTable(int cn,clCard *c0)
{
    if(cn==5)
        *this=GameWhatGame(c0);
    else
    {
        clCard card[4];
        memcpy(card,c0,cn*sizeof(clCard));
        Straight(card,cn);
        game=GM_0;
        for(int i=0;i<cn;i++)
            face[i]=card[cn-1-i].face;
        if(card[0].face==card[1].face)
        {
            game=GM_2;
            face[0]=card[0].face;
        }
        if(card[1].face==card[2].face)
        {
            if(game==GM_2)
            {
                game=GM_3;
            }
            else
            {
                game=GM_2;
                face[0]=card[2].face;
            }
        }
        if(cn==4 && card[2].face==card[3].face)
            switch(game)
            {
                case GM_0  :
                    game=GM_2;
                    face[0]=card[3].face;
                    break;
                case GM_2  :
                    if(face[0]==card[3].face)
                    {
                        game=GM_3;
                    }
                    else
                    {
                        game=GM_22;
                        face[0]=card[2].face;
                        face[1]=card[0].face;
                    }
                    break;
                case GM_3  :
                    game=GM_4;
                    break;
            }
    }
}
//---------------------------------------------------------------------------
void    Straight(clCard *card,int cn)
{
    for(int i=0;i<cn-1;i++)
    {
        int nb=i;
        for(int j=i+1;j<cn;j++)
            if(card[nb].face>card[j].face)
                nb=j;
        clCard c0=card[i];
        card[i]=card[nb];
        card[nb]=c0;
    }
}
//---------------------------------------------------------------------------
void    StraightHi(clCard *card,int cn)
{
    for(int i=0;i<cn-1;i++)
    {
        int nb=i;
        for(int j=i+1;j<cn;j++)
            if(card[nb].face<card[j].face)
                nb=j;
        clCard c0=card[i];
        card[i]=card[nb];
        card[nb]=c0;
    }
}
//---------------------------------------------------------------------------
enFace  IsStraight(enFace *f0,int cn)
{
    for(int i=cn-5;i>=0;i--)
        if(f0[i]+1==f0[i+1] && f0[i+1]+1==f0[i+2] && f0[i+2]+1==f0[i+3] && f0[i+3]+1==f0[i+4])
            return f0[i+4];
    if(f0[0]==C_2 && f0[1]==C_3 && f0[2]==C_4 && f0[3]==C_5 && f0[cn-1]==C_A)
        return C_5;
    return C_UNDEF;
}
//---------------------------------------------------------------------------
clGamePoker GamePokerMinGame()
{
    clGamePoker game;
    game.game=GM_0;
    for(int i=0;i<5;i++)
        game.face[i]=C_2;
    return(game);
}
//---------------------------------------------------------------------------
/*AnsiString  GamePokerGameToString(enGame g0)
{
    return glNameGame[g0];
}
*/
//---------------------------------------------------------------------------
enGame      GamePokerStringToGame(char *str)
{
    for(int i=0;i<GM_UNDEF;i++)
        if(strcmp(str,glNameGame[i])==0)
            return (enGame)i;
    return GM_UNDEF;
}
//---------------------------------------------------------------------------
clGamePoker  GameWhatGame(clCard *card,int cnCard)
{
    if(cnCard==5) return GameWhatGame(card);
    clGamePoker res;
	res.game=GM_0;res.face[0]=C_2;
    if(cnCard==0) return res;
    if(cnCard==1)
    {
        res.face[0]=card[0].face;
        res.face[1]=C_2;
        return res;
    }
    StraightHi(card,cnCard);
    if(cnCard==2)
    {
        if(card[0].face==card[1].face)
        {
            res.game=GM_2;
            res.face[0]=card[0].face;
            res.face[1]=C_2;
        }
        else
        {
            res.face[0]=card[0].face;
            res.face[1]=card[1].face;
        }
        return res;
    }
    if(card[0].face==card[1].face)
    {
        res.game=GM_2;
        res.face[0]=card[0].face;
        res.face[1]=card[2].face;
    }
    if(card[1].face==card[2].face)
    {
        if(res.game==GM_2)
            res.game=GM_3;
        else
        {
            res.game=GM_2;
            res.face[0]=card[1].face;
            res.face[1]=card[0].face;
        }
    }
    if(cnCard==4)
    {
        if(card[2].face==card[3].face)
        {
            if(res.game==GM_3)
                res.game=GM_4;
            else if(res.game==GM_2)
            {
                if(res.face[0]==card[3].face)
                    res.game=GM_3;
                else
                {
                    res.game=GM_22;
                    res.face[1]=card[3].face;
                }
            }
            else
            {
                res.game=GM_2;
                res.face[0]=card[3].face;
                res.face[1]=card[0].face;
                res.face[2]=card[1].face;
            }
        }
        else
        {
            if(res.game==GM_3)
                res.face[1]=card[3].face;
            else if(res.game==GM_2)
                res.face[2]=card[3].face;
        }
    }
    if(res.game==GM_0)
        for(int i=0;i<cnCard;i++)
            res.face[i]=card[i].face;
    return res;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void    stGameFace::Clear()
{
    for(int i=0;i<CN_CARD_FACE;i++)
        _weight[i]=0;
    _sum=0;
}
//---------------------------------------------------------------------------
double stGameFace::Weight()
{
    _sum=0;
    for(int i=0;i<CN_CARD_FACE;i++)
        _sum+=_weight[i];
    return _sum;
}
//---------------------------------------------------------------------------
void    stGameFace::operator += (stGameFace &val)
{
    for(int i=0;i<CN_CARD_FACE;i++)
        _weight[i]+=val._weight[i];
}
//---------------------------------------------------------------------------
void    stGameFace::operator -= (stGameFace &val)
{
    for(int i=0;i<CN_CARD_FACE;i++)
        _weight[i]-=val._weight[i];
}
//---------------------------------------------------------------------------
void    stGameFace::operator *= (double val)
{
    for(int i=0;i<CN_CARD_FACE;i++)
        _weight[i]*=val;
}
//---------------------------------------------------------------------------
void    stGameFace::operator /= (double val)
{
    for(int i=0;i<CN_CARD_FACE;i++)
        _weight[i]/=val;
}
//---------------------------------------------------------------------------
void    stGameFace::operator =  (double val)
{
    for(int i=0;i<CN_CARD_FACE;i++)
        _weight[i]=val;
}
//---------------------------------------------------------------------------
bool    stGameFace::CheckNegativeVal()
{
    for(int i=0;i<CN_CARD_FACE;i++)
        if(_weight[i]<0)
            if(_weight[i]+DOUBLE_0<0)
                return false;
            else _weight[i]=0;
    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void    clSetGame::Clear()
{
    for(int i=0;i<GM_UNDEF;i++)
        _arr[i].Clear();
}
//---------------------------------------------------------------------------
void    clSetGame::operator = (const clSetGame &val)
{
    memcpy(_arr,val._arr,GM_UNDEF*sizeof(stGameFace));
}
//---------------------------------------------------------------------------
void    clSetGame::operator = (clGamePoker val)
{
    Clear();
    _arr[val.game][val.face[0]]=1;
}
//---------------------------------------------------------------------------
void    clSetGame::operator += (clSetGame val)
{
    for(int i=0;i<GM_UNDEF;i++)
        _arr[i]+=val._arr[i];
}
//---------------------------------------------------------------------------
void    clSetGame::operator += (clGamePoker val)
{
    _arr[val.game][val.face[0]]+=1;
}
//---------------------------------------------------------------------------
void    clSetGame::operator *= (double val)
{
    for(int i=0;i<GM_UNDEF;i++)
        _arr[i]*=val;
}
//---------------------------------------------------------------------------
bool    clSetGame::NormWeight(double sum)
{
    double w0=0;
    for(int i=0;i<GM_UNDEF;i++)
        w0+=_arr[i].Weight();
    if(w0<DOUBLE_0) return false;
    sum/=w0;
    for(int i=0;i<GM_UNDEF;i++)
    {
        _arr[i]._sum*=sum;
        for(int j=0;j<CN_CARD_FACE;j++)
            _arr[i][j]*=sum;
    }
    return true;
}
//---------------------------------------------------------------------------
double  clSetGame::Weight()
{
    double res=0;
    for(int i=0;i<GM_UNDEF;i++)
        res+=_arr[i].Weight();
    return res;
}
//---------------------------------------------------------------------------
//***************************************************************************
//---------------------------------------------------------------------------
double ProbHiFace(stGameFace &gf1,stGameFace &gf2)
{
    double res=0,sum=0,w0=gf2.Weight();
    if(w0<DOUBLE_0)
        return gf1.Weight();
    for(int i=C_A;i>=C_2;i--)
    {
        res+=gf2[i]*(sum+gf1[i]/2);
        sum+=gf1[i];
    }
    return res/w0;
}
//---------------------------------------------------------------------------
clGamePoker  GameWhatGame(tpCard *card, int cnCard)
{
	clCard cards[7];
	for (int i = 0; i < cnCard; i++)
		cards[i] = card[i];
	return GameWhatGame(cards, cnCard);
}
//---------------------------------------------------------------------------

