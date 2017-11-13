//---------------------------------------------------------------------------
//#include "stdafx.h"
#pragma hdrstop

#include <math.h>
#include <stdio.h>
//#include <alloc.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <errno.h>
#include <afxwin.h>

#include "C:\home\card\clCol.h"
#include "C:\home\holdem\Common\clHandsCompare.h"

clHandsCompare glHandCmp;
clListCompare glListCmp;

bool    IsFullGroupHands(const clImproveGroupHands &gr3);
//--------------------------------------------------------------------------------
void    tpWin2::operator *=(double x)
{
    _win*=x;
    _loss*=x;
    _eq*=x;
}
//--------------------------------------------------------------------------------
void    tpWin2::operator +=(tpWin2 w)
{
    _win+=w._win;
    _loss+=w._loss;
    _eq+=w._eq;
    _weight+=w._weight;
}
//--------------------------------------------------------------------------------
clListCompare::clListCompare()
{
    _listCompare=(stHandsCompare169 *)malloc(sizeof(stHandsCompare169));
	int handle;
	_sopen_s(&handle, FILE_LIST_HANDS, O_RDWR | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);    
	if (handle == -1)
    {
//		AfxMessageBox(_T(FILE_LIST_HANDS));
//		Application->MessageBox("Не открывается файл", FILE_LIST_HANDS, MB_OK);
        return;
    }
    _read(handle,&_list,sizeof(tpListHand));
    for(int i=0;i<CN_HAND;i++)
        _urH[_list._nbH[i]]=i;
//    read(handle,glListCmp._listCompare,sizeof(stHandsCompare169));
    _close(handle);
}
//--------------------------------------------------------------------------------
clListCompare::~clListCompare()
{
    free(_listCompare);
}
//--------------------------------------------------------------------------------
int clListCompare::UrHand(double ur)
{
    int i=1;
    for(;i<CN_HAND;i++)
        if(_list._ur[i]>ur)
            break;
    return i-1;
}
//--------------------------------------------------------------------------------
//------ full hands compare ----------------------------------------------------
//--------------------------------------------------------------------------------

clRezCompare CompareHands(clHand h1, clHand h2)
{
    clCol  Col;
    clCard  * ptrCol = Col.Cards();
    clCard  cardTable[5];
    Col.New();
    Col.Skip(h1._cards[0]);
    Col.Skip(h1._cards[1]);
    Col.Skip(h2._cards[0]);
    Col.Skip(h2._cards[1]);
    clRezCompare rez;
    clGamePoker gamePoker1, gamePoker2;

    for (int j=0; j<CN_CARD_COL-4; j++)
    {
        cardTable[0] = ptrCol[j];
        for (int j1=j+1; j1<CN_CARD_COL-4; j1++)
        {
            cardTable[1] = ptrCol[j1];
            for (int j2=j1+1; j2<CN_CARD_COL-4; j2++)
            {
                cardTable[2] = ptrCol[j2];
                for (int j3=j2+1; j3<CN_CARD_COL-4; j3++)
                {
                    cardTable[3] = ptrCol[j3];
                    for (int j4=j3+1; j4<CN_CARD_COL-4; j4++)
                    {
                        cardTable[4] = ptrCol[j4];

                        gamePoker1.GameHoldemQ(h1, cardTable);
                        gamePoker2.GameHoldemQ(h2, cardTable);
                        if(gamePoker1 > gamePoker2)
                        {
                            rez.AddWin();
                        }else if(gamePoker1 < gamePoker2)
                        {
                            rez.AddLose();
                        }else
                        {
                            rez.AddEq();
                        }
                    }
                }
            }
        }
    }
    return(rez);
}

//-----hands compare by 4 cards. 5-th card is random----------------------------
clRezCompare CompareHands4(clHand h1, clHand h2)
{
    clCol  Col;
    clCard  * ptrCol = Col.Cards();
    clCard  cardTable[5];
    Col.New();
    Col.Skip(h1._cards[0]);
    Col.Skip(h1._cards[1]);
    Col.Skip(h2._cards[0]);
    Col.Skip(h2._cards[1]);
    clRezCompare rez;
    clGamePoker gamePoker1, gamePoker2;

    for (int j=0; j<CN_CARD_COL-4; j++)
    {
        cardTable[0] = ptrCol[j];
        for (int j1=j+1; j1<CN_CARD_COL-4; j1++)
        {
            cardTable[1] = ptrCol[j1];
            for (int j2=j1+1; j2<CN_CARD_COL-4; j2++)
            {
                cardTable[2] = ptrCol[j2];
                for (int j3=j2+1; j3<CN_CARD_COL-4; j3++)
                {
                    cardTable[3] = ptrCol[j3];
                    while(true)
                    {
                        int rand = random(CN_CARD_COL-4);
                        if(rand!=j && rand!=j1 && rand!=j2 && rand!=j3)
                        {
                            cardTable[4] = ptrCol[rand];
                            gamePoker1.GameHoldemQ(h1, cardTable);
                            gamePoker2.GameHoldemQ(h2, cardTable);
                            if(gamePoker1 > gamePoker2)
                            {
                                rez.AddWin();
                            }else if(gamePoker1 < gamePoker2)
                            {
                                rez.AddLose();
                            }else
                            {
                                rez.AddEq();
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    return(rez);
}

//-----hands compare by 3 cards. 4 and 5-th card are random---------------------
clRezCompare CompareHands3(clHand h1, clHand h2)
{
    clCol  Col;
    clCard  * ptrCol = Col.Cards();
    clCard  cardTable[5];
    Col.New();
    Col.Skip(h1._cards[0]);
    Col.Skip(h1._cards[1]);
    Col.Skip(h2._cards[0]);
    Col.Skip(h2._cards[1]);
    clRezCompare rez;
    clGamePoker gamePoker1, gamePoker2;
    int j3, j4;

    for (int j=0; j<CN_CARD_COL-4; j++)
    {
        cardTable[0] = ptrCol[j];
        for (int j1=j+1; j1<CN_CARD_COL-4; j1++)
        {
            cardTable[1] = ptrCol[j1];
            for (int j2=j1+1; j2<CN_CARD_COL-4; j2++)
            {
                cardTable[2] = ptrCol[j2];
                while(true)
                {
                    j3 = random(CN_CARD_COL-4);
                    if(j3!=j && j3!=j1 && j3!=j2)
                    {
                        cardTable[3] = ptrCol[j3];
                        break;
                    }
                }
                while(true)
                {
                    j4 = random(CN_CARD_COL-4);
                    if(j4!=j && j4!=j1 && j4!=j2 && j4!=j3)
                    {
                        cardTable[4] = ptrCol[j4];
                        break;
                    }
                }
                gamePoker1.GameHoldemQ(h1, cardTable);
                gamePoker2.GameHoldemQ(h2, cardTable);
                if(gamePoker1 > gamePoker2)
                {
                    rez.AddWin();
                }else if(gamePoker1 < gamePoker2)
                {
                    rez.AddLose();
                }else
                {
                    rez.AddEq();
                }
            }
        }
    }
    return(rez);
}

//-----hands compare by 2 cards. 3,4 and 5-th card are random---------------------
clRezCompare CompareHands2(clHand h1, clHand h2)
{
    clCol  Col;
    clCard  * ptrCol = Col.Cards();
    clCard  cardTable[5];
    Col.New();
    Col.Skip(h1._cards[0]);
    Col.Skip(h1._cards[1]);
    Col.Skip(h2._cards[0]);
    Col.Skip(h2._cards[1]);
    clRezCompare rez;
    clGamePoker gamePoker1, gamePoker2;
    int j2, j3, j4;

    for (int j=0; j<CN_CARD_COL-4; j++)
    {
        cardTable[0] = ptrCol[j];
        for (int j1=j+1; j1<CN_CARD_COL-4; j1++)
        {
            cardTable[1] = ptrCol[j1];
                while(true)
                {
                    j2 = random(CN_CARD_COL-4);
                    if(j2!=j && j2!=j1)
                    {
                        cardTable[2] = ptrCol[j2];
                        break;
                    }
                }
                while(true)
                {
                    j3 = random(CN_CARD_COL-4);
                    if(j3!=j && j3!=j1 && j3!=j2)
                    {
                        cardTable[3] = ptrCol[j3];
                        break;
                    }
                }
                while(true)
                {
                    j4 = random(CN_CARD_COL-4);
                    if(j4!=j && j4!=j1 && j4!=j2 && j4!=j3)
                    {
                        cardTable[4] = ptrCol[j4];
                        break;
                    }
                }
                gamePoker1.GameHoldemQ(h1, cardTable);
                gamePoker2.GameHoldemQ(h2, cardTable);
                if(gamePoker1 > gamePoker2)
                {
                    rez.AddWin();
                }else if(gamePoker1 < gamePoker2)
                {
                    rez.AddLose();
                }else
                {
                    rez.AddEq();
                }
        }
    }
    return(rez);
}

//---------------------------------------------------------------------------
clRezCompare::clRezCompare()
{
    _cnWin = _cnLose = _cnEq = 0;
}
//---------------------------------------------------------------------------
clHandsCompare::clHandsCompare()
{
    clRezCompare rez;
	_handsC = (stHandsCompare*)malloc(sizeof(stHandsCompare));
	_handsCAll = (stHandsCompareAll*)malloc(sizeof(stHandsCompareAll));
	_handsC169 = (stHandsCompare169*)malloc(sizeof(stHandsCompare169));
/*        for(int i=0; i<ALL_CN_HAND; i++)
            for(int j=0; j<ALL_CN_HAND; j++)
                _handsC->_arr[i][j] = rez;
    int handle = open(FINAL_FILE_HANDS_COMPARE, O_RDWR | O_BINARY);
#ifdef CALCULATE_MODE
    if (handle == -1)
    {
        for(int i=0; i<ALL_CN_HAND; i++)
            for(int j=0; j<ALL_CN_HAND; j++)
                _handsC->_arr[i][j] = rez;
        handle=open(FINAL_FILE_HANDS_COMPARE,O_BINARY | O_RDWR | O_CREAT);
        int i=START_I,j=START_I;
        glHandCmp.WriteFileAll(handle);
        write(handle,&i,sizeof(int));
        write(handle,&j,sizeof(int));
    }
#else
    if(handle==-1)
    {
        Application->MessageBox(FINAL_FILE_HANDS_COMPARE,"Не открывается файл",MB_OK);
        return;
    }
#endif
    ReadFileAll(handle);
    lseek(handle,2*sizeof(int),SEEK_CUR);
    read(handle,glHandCmp._handsC169,sizeof(stHandsCompare169));
    close(handle);
    chmod(FINAL_FILE_HANDS_COMPARE,S_IREAD | S_IWRITE);

    _cnCompare = _handsC->_arr[0][5].Sum();
*/
}

//---------------------------------------------------------------------------
clHandsCompare::~clHandsCompare()
{
	free(_handsC);
	free(_handsCAll);
	free(_handsC169);
}
//---------------------------------------------------------------------------
void clHandsCompare::WriteFileAll(int handle)
{
    int bytes = _write(handle, _handsC,sizeof(stHandsCompare));
    if (bytes != sizeof(stHandsCompare))
    {
         printf("Error writing _arr to the file.\n");
    }
    _write(handle,_handsC169,sizeof(stHandsCompare169));
}


//----write result of compare for hands i, i1 to file. ----------------
//----Current position of file pointer is start of table --------------
void clHandsCompare::ReadFileAll(int handle)
{
    int bytes = _read(handle, _handsC, sizeof(stHandsCompare));
    if (bytes != sizeof(stHandsCompare))
    {
        printf("Read _arr Failed.\n");
    }
	for (int i = 0; i < ALL_CN_HAND; i++)
		for (int j = 0; j < ALL_CN_HAND; j++)
			_handsCAll->_arr[i][j] = _handsC->_arr[i][j];
	_read(handle, _handsC169, sizeof(stHandsCompare169));
}
//---------------------------------------------------------------------------
void clHandsCompare::WrRezCompare(clRezCompare rez, int nbHand1, int nbHand2)
{
    clRezCompare tmpRez;
    _handsC->_arr[nbHand1][nbHand2] = rez;
    tmpRez._cnWin = rez._cnLose;
    tmpRez._cnLose = rez._cnWin;
    tmpRez._cnEq = rez._cnEq;
    _handsC->_arr[nbHand2][nbHand1] = tmpRez;
}
//---------------------------------------------------------------------------
enWin clHandsCompare::Compare(clCard *card1,clCard *card2)
{
    int nb1=((clHand *)card1)->NbHandAll();
    int nb2= ((clHand *)card2)->NbHandAll();
    clRezCompare result=RezCompare(nb1,nb2);
    unsigned int ran=random(_cnCompare);
    if(ran<result._cnWin)
        return(WIN_WIN);
    if(ran<result._cnLose+result._cnWin)
        return(WIN_LOSS);
    return(WIN_EQ);
}
//---------------------------------------------------------------------------
void    CompareHands169()
{
    for(int i=0;i<CN_HAND;i++)
        for(int j=0;j<CN_HAND;j++)
            glHandCmp._handsC169->_arr[i][j].Clear();
    for(int i=0;i<ALL_CN_HAND;i++)
        for(int j=0;j<ALL_CN_HAND;j++)
        {
            clRezCompare rez=glHandCmp.RezCompare(i,j);
            if(rez.Sum() != 0)
            {
                clHand h1,h2;
                h1.SetHandAll(i);
                h2.SetHandAll(j);
                int nb1=h1.NbHand(),nb2=h2.NbHand();
                glHandCmp._handsC169->_arr[nb1][nb2].Add(rez);
            }
        }
    for(int i=0;i<CN_HAND;i++)
        for(int j=0;j<CN_HAND;j++)
        {
            glHandCmp._handsC169->_arr[i][j].Norm();
            glHandCmp._handsC169->_arr[i][j]._weight/=clHand::CnHand(i)*50*49/2;
            double w=HandWeight(i,j);
            if(glHandCmp._handsC169->_arr[i][j]._weight != w)
                i=i;
        }
}
//---------------------------------------------------------------------------
void    CompareHandsCalcList()
{
    double win[CN_HAND];
    for(int i=0;i<CN_HAND;i++)
        win[i]=0;
    for(int i=0;i<CN_HAND;i++)
        for(int j=0;j<CN_HAND;j++)
            win[i]+=glHandCmp._handsC169->_arr[i][j].WinEqWithWeigth();
    for(int i=0;i<CN_HAND;i++)
    {
        int nb=0;
        for(int j=1;j<CN_HAND;j++)
            if(win[j]>win[nb])
                nb=j;
        glListCmp._list._nbH[i]=nb;
        if(i==0)
            glListCmp._list._ur[i]=HandWeight(nb);
        else
            glListCmp._list._ur[i]=glListCmp._list._ur[i-1]+HandWeight(nb);
        win[nb]=-1;
    }
	int handle; 
	_sopen_s(&handle, FINAL_FILE_HANDS_COMPARE, O_RDWR | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (handle == -1)
    {
		AfxMessageBox(_T(FINAL_FILE_HANDS_COMPARE));
//		Application->MessageBox(FINAL_FILE_HANDS_COMPARE, "Не открывается файл", MB_OK);
        return;
    }
    _lseek(handle,sizeof(stHandsCompare)+2*sizeof(int),SEEK_SET);
    _write(handle,glHandCmp._handsC169,sizeof(stHandsCompare169));
    _close(handle);
	_sopen_s(&handle, FILE_LIST_HANDS, O_RDWR | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
    if(handle==-1)
    {
		AfxMessageBox(_T(FILE_LIST_HANDS));
//		Application->MessageBox(FILE_LIST_HANDS, "Не открывается файл", MB_OK);
        return;
    }
    _write(handle,&glListCmp._list,sizeof(tpListHand));
    _close(handle);
}
//---------------------------------------------------------------------------
void    CompareHandsCalcListList()
{
    stHandsCompare169 hc;
    for(int i=0;i<CN_HAND;i++)
    {
        tpWin2 w0;
        int nb1=glListCmp._list._nbH[i];
        for(int j=0;j<CN_HAND;j++)
        {
            int nb2=glListCmp._list._nbH[j];
            tpWin2 w=HCompWinHand169(nb1,nb2);
            w*=w._weight;
            w0+=w;
            hc._arr[i][j]=w0;
            hc._arr[i][j].Norm();
            tpWin2 w1=hc._arr[i][j];
            glListCmp._listCompare->_arr[i][j]=hc._arr[i][j];
        }
    }
/*    for(int i=0;i<CN_HAND;i++)
    {
        tpWin2 w0;
        for(int j=0;j<CN_HAND;j++)
        {
            int nb2=glListCmp._list._nbH[j];
            tpWin2 w=hc[j][i];
        }
    }
*/
//	int handle = open(FILE_LIST_HANDS, O_BINARY | O_RDWR);
	int handle;
	_sopen_s(&handle, FILE_LIST_HANDS, O_RDWR | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (handle == -1)
    {
//        Application->MessageBox("Не открывается файл",FILE_LIST_HANDS, mbOK);
		AfxMessageBox(_T(FILE_LIST_HANDS));
		return;
    }
    _write(handle,&glListCmp._list,sizeof(tpListHand));
    _write(handle,glListCmp._listCompare,sizeof(stHandsCompare169));
    _close(handle);
}
//---------------------------------------------------------------------------
tpWin2 CCCompareHands(int nbHand, const clImproveGroupHands &gr2, double &k0)
{
	double w2 = 0;
	tpWin2 res2;
	for (int i = 0; i<gr2._cnH; i++)
	{
		tpWin2 win2 = HCompWinHand169(nbHand, gr2._nbH[i]);
		double k = win2._weight / HandWeight(gr2._nbH[i]);
//		k = 1;
		//        double k1=HandWeight(nbHand,gr2._nbH[i])/HandWeight(gr2._nbH[i]);
		double w = gr2._weightH[i] * k;
		w2 += w;
		win2 *= w;
		res2 += win2;
	}
	if (w2>DOUBLE_0)
	{
		k0 = w2 / gr2.Weight();
		res2 *= 1 / w2;
	}
	else
	{
		res2._win = -100;
		k0 = 0;
	}
	return res2;
}
//---------------------------------------------------------------------------
tpWin2 CCCompareHandsWithoutK(int nbHand, const clImproveGroupHands &gr2)
{
	double w2 = 0;
	tpWin2 res2;
	for (int i = 0; i<gr2._cnH; i++)
	{
		tpWin2 win2 = HCompWinHand169(nbHand, gr2._nbH[i]);
		double w = gr2._weightH[i];
		w2 += w;
		win2 *= w;
		res2 += win2;
	}
	if (w2>DOUBLE_0)
	{
		res2 *= 1 / w2;
	}
	else
	{
		res2._win = -100;
	}
	return res2;
}
//---------------------------------------------------------------------------
double  CCComparePlayersEQ(const clImproveGroupHands &gr1, const clImproveGroupHands &gr2)
{
    tpWin2 res=CCComparePlayers(gr1,gr2);
    return res.WinEq();
}
//---------------------------------------------------------------------------
tpWin2  CCComparePlayers(const clImproveGroupHands &gr1,const clImproveGroupHands &gr2)
{
    double w1=0;
    tpWin2 res1;
    for(int i=0;i<gr1._cnH;i++)
    {
        double k;
        tpWin2 res=CCCompareHands(gr1._nbH[i],gr2,k);
        double w=gr1._weightH[i]*k;
        res*=w;
        res1+=res;
        w1+=w;
    }
    if(w1>DOUBLE_0) res1*=1/w1;
    else res1._win=-100;
    return res1;
}
//---------------------------------------------------------------------------
/*stVerManyHands CCCompareHands3Pl(int nbHand1,int nbHand2,clImproveGroupHands gr3,double &k0)
{
    stVerManyHands mhAll;
    double k=0;
    for(int i=0;i<gr3._cnH;i++)
    {
        stVerManyHands mh;
        mh=SHCHandsComprasion(nbHand1,nbHand2,gr3._nbH[i]);
        double k1=mh._weightHand3;
        k+=gr3._weightH[i]*k1;
        mhAll+=mh*(k1*gr3._weightH[i]);
    }
    if(k>DOUBLE_0)
    {
        k0=k/gr3.Weight();
        mhAll/=k;
        return mhAll;
    }
    else k0=0;
    return mhAll;
}
//---------------------------------------------------------------------------
stVerManyHands CCCompareHands3Pl(int nbHand,clImproveGroupHands gr2,clImproveGroupHands gr3,double &k0)
{
    stVerManyHands mhAll;
    double k=0;
    double wAll=0;
    for(int i=0;i<gr2._cnH;i++)
    {
        tpWin2 win2=HCompWinHand169(nbHand,gr2._nbH[i]);
        double k2,k1=win2._weight;

        stVerManyHands mh;
        mh=CCCompareHands3Pl(nbHand,gr2._nbH[i],gr3,k2);
        mhAll+=mh*(k2*gr2._weightH[i]);

        wAll+=k2*gr2._weightH[i];
        k+=k1*gr2._weightH[i];

    }
    if(wAll>DOUBLE_0)
    {
        k0=k/gr2.Weight();
        mhAll/=wAll;
    }
    else k0=0;
    return mhAll;
} */
//---------------------------------------------------------------------------
stVerManyHands  CCComparePlayers3Pl(const clImproveGroupHands &gr1,const clImproveGroupHands &gr2,const clImproveGroupHands &gr3)
{
/*    if(IsFullGroupHands(gr3))
        return CCCCompare3PlFullGH(gr1,gr2);
    if(IsFullGroupHands(gr2))
        return CCCCompare3PlFullGH(gr1,gr3).Coverter(1,3,2);
    if(IsFullGroupHands(gr1))
        return CCCCompare3PlFullGH(gr3,gr2).Coverter(3,2,1);*/
    stVerManyHands mhAll;
    for(int i1=0;i1<gr1._cnH;i1++)
    {
        int nbH1=gr1._nbH[i1];
        double k1=gr1._weightH[i1]/clHand::CnHand(nbH1);
        for(int i2=0;i2<gr2._cnH;i2++)
        {
            int nbH2=gr2._nbH[i2];
            double k2=gr2._weightH[i2]/clHand::CnHand(nbH2)*k1;
            for(int i3=0;i3<gr3._cnH;i3++)
            {
                stVerManyHands mh=SHCHandsComprasion(nbH1,nbH2,gr3._nbH[i3]);
                mhAll+=mh*(mh._cnHands3*k2*gr3._weightH[i3]/clHand::CnHand(gr3._nbH[i3]));
            }
        }
    }
    mhAll.Norm();
    return mhAll;
}

//---------------------------------------------------------------------------
void CalcResCompareFullGroup()
{
    for(int i=0;i<CN_HAND;i++)
        for(int j=0;j<CN_HAND;j++)
        {
            stVerManyHands mhAll;
            for(int k=0;k<CN_HAND;k++)
            {
                stVerManyHands mh=SHCHandsComprasion(i,j,k);
                mhAll+=mh*mh._cnHands3;
            }
            mhAll.Norm();
            glSHCarrFullGH[i*CN_HAND+j]=mhAll;
        }
	int handle;
	_sopen_s(&handle, "c:\\work\\cmp3PlFullGH.bin", O_RDWR | O_CREAT | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);

    _write(handle,glSHCarrFullGH,CN_HAND*CN_HAND*sizeof(stVerManyHands));
    _close(handle);
}
//---------------------------------------------------------------------------
bool    CompareHandsInit()
{
	int handle;// = open("c:\\WORK\\BIN\\Compare\\comp0.tmp", O_RDWR | O_BINARY);
	_sopen_s(&handle, "c:\\WORK\\BIN\\Compare\\comp0.tmp", O_RDWR | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (handle == -1)
    {
		AfxMessageBox(_T("Не открывается файл c:\\WORK\\BIN\\Compare\\comp0.tmp"));
//		Application->MessageBoxA("Не открывается файл c:\\WORK\\BIN\\Compare\\comp0.tmp", "ERROR", MB_OK);
        return false;
    }
    glHandCmp.ReadFileAll(handle);
    _close(handle);
    SHCMemSet();
    if(!SHCLoadFromFile("c:\\work\\bin\\Compare\\3HandCmp.bin"))
    {
		AfxMessageBox(_T("Не открывается файл c:\\work\\bin\\Compare\\3HandCmp.bin"));
//		Application->MessageBoxA("Не открывается файл c:\\work\\bin\\Compare\\3HandCmp.bin", "ERROR", MB_OK);
        return false;
    }
	_sopen_s(&handle, "c:\\work\\bin\\Compare\\cmp3PlFullGH.bin", O_RDWR | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
//	handle = open("c:\\work\\bin\\Compare\\cmp3PlFullGH.bin", O_RDONLY | O_BINARY);
    if(handle==-1)
    {
		AfxMessageBox(_T("Не открывается файл c:\\WORK\\BIN\\Compare\\cmp3PlFullGH.bin"));
//		Application->MessageBoxA("Не открывается файл c:\\WORK\\BIN\\Compare\\cmp3PlFullGH.bin", "ERROR", MB_OK);
        return false;
    }
    _read(handle,glSHCarrFullGH,CN_HAND*CN_HAND*sizeof(stVerManyHands));
    _close(handle);
    return true;
}
//---------------------------------------------------------------------------
bool    IsFullGroupHands(const clImproveGroupHands &gr3)
{
    if(gr3.CnHand() != CN_HAND)
        return false;
    double mult=gr3._weightH[0]/HandWeight(gr3._nbH[0]);
    for(int i=1;i<CN_HAND;i++)
        if(fabs(gr3._weightH[i]/HandWeight(gr3._nbH[i])-mult)>DOUBLE_0)
            return false;
    return true;
}
//---------------------------------------------------------------------------
/*stVerManyHands  CCCCompare3PlFullGH(int nbHand,const clImproveGroupHands &gr2,double &k0)
{
    stVerManyHands mhAll;
    double k=0;
    double wAll=0;
    for(int i=0;i<gr2._cnH;i++)
    {
        tpWin2 win2=HCompWinHand169(nbHand,gr2._nbH[i]);
        double k2=1,k1=win2._weight;

        stVerManyHands mh;
        mh=glSHCarrFullGH[nbHand*CN_HAND+gr2._nbH[i]];
        k2=mh._weightHand3;
        mhAll+=mh*(k2*gr2._weightH[i]);

        wAll+=k2*gr2._weightH[i];
        k+=k1*gr2._weightH[i];

    }
    if(wAll>DOUBLE_0)
    {
        k0=k/gr2.Weight();
        mhAll/=wAll;
    }
    else k0=0;
    return mhAll;
} */
//---------------------------------------------------------------------------
stVerManyHands  CCCCompare3PlFullGH(const clImproveGroupHands &gr1,const clImproveGroupHands &gr2)
{
    stVerManyHands mhAll;
    for(int i1=0;i1<gr1._cnH;i1++)
    {
        int nbH1=gr1._nbH[i1];
        double k1=gr1._weightH[i1]/clHand::CnHand(nbH1);
        for(int i2=0;i2<gr2._cnH;i2++)
        {
            int nbH2=gr2._nbH[i2];
            double k2=gr2._weightH[i2]/clHand::CnHand(nbH2)*k1;
            stVerManyHands mh=glSHCarrFullGH[nbH1*CN_HAND+nbH2];
            mhAll+=mh*k2;
        }
    }
    mhAll.Norm();
    return mhAll;
}
//--------------------------------------------------------------------------------

#pragma package(smart_init)
