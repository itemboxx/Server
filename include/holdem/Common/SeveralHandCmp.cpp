//---------------------------------------------------------------------------
//#include "stdafx.h"
#pragma hdrstop
#include "C:\home\holdem\Common\SeveralHandCmp.h"
#include <io.h>
#include <fcntl.h>
//#include <time.h>
#include <sys\stat.h>
#include <share.h>

stVerManyHands ***glSHCarrHands;
//stVerManyHandsOld ***glSHCarrHandsOld;
stVerManyHands *glSHCarrFullGH;         // массив 169х169 результатов сравнений хенд1,хенд2,полная группа

//---------------------------------------------------------------------------
stVerManyHands stVerManyHands::operator *(float a)
{
    stVerManyHands mh;
    mh._win111=_win111*a;
    mh._win112=_win112*a;
    mh._win121=_win121*a;
    mh._win122=_win122*a;
    mh._win123=_win123*a;
    mh._win132=_win132*a;
    mh._win211=_win211*a;
    mh._win212=_win212*a;
    mh._win221=_win221*a;
    mh._win213=_win213*a;
    mh._win231=_win231*a;
    mh._win321=_win321*a;
    mh._win312=_win312*a;
    return(mh);
}
double stVerManyHands::WinEQHand(int nbPl)
{
    if(nbPl==0)
        return WinEQFirstHand();
    else if(nbPl==1)
        return WinEQSecondHand();
    else if(nbPl==2)
        return WinEQThirdHand();
    return (-1000000000);
}
double  stVerManyHands::WinEQHandWithout(int nbPl,int nbPl0)
{
    if(nbPl==0)
    {
        if(nbPl0==1)
            return(WinEQFirstWithoutSecond());
        if(nbPl0==2)
            return(WinEQFirstWithoutThird());
    }
    else if(nbPl==1)
    {
        if(nbPl0==0)
            return(WinEQSecnondWithoutFirst());
        if(nbPl0==2)
            return(WinEQSecondWithoutThird());
    }
    else if(nbPl==2)
    {
        if(nbPl0==0)
            return(WinEQThirdWithoutFirst());
        if(nbPl0==1)
            return(WinEQThirdWithoutSecond());
    }
    return (-1000000000);

}
stVerManyHands stVerManyHands::Coverter(int nb1,int nb2,int nb3)
{
    stVerManyHands mh;mh._cnHands3=_cnHands3;
    if(nb1==1 && nb2==2 && nb3==3)
    {
        return *this;

    }
    else if(nb1==1 && nb2==3 && nb3==2)
    {
        mh._win111=_win111;
        mh._win112=_win121;
        mh._win121=_win112;
        mh._win122=_win122;
        mh._win123=_win132;
        mh._win132=_win123;
        mh._win211=_win211;
        mh._win212=_win221;
        mh._win221=_win212;
        mh._win213=_win231;
        mh._win231=_win213;
        mh._win321=_win312;
        mh._win312=_win321;
    }
    else if(nb1==2 && nb2==1 && nb3==3)
    {
        mh._win111=_win111;
        mh._win112=_win112;
        mh._win121=_win211;
        mh._win122=_win212;
        mh._win123=_win213;
        mh._win132=_win312;
        mh._win211=_win121;
        mh._win212=_win122;
        mh._win221=_win221;
        mh._win213=_win123;
        mh._win231=_win321;
        mh._win321=_win231;
        mh._win312=_win132;
    }
    else if(nb1==2 && nb2==3 && nb3==1)
    {
        mh._win111=_win111;
        mh._win112=_win121;
        mh._win121=_win211;
        mh._win122=_win221;
        mh._win123=_win231;
        mh._win132=_win321;
        mh._win211=_win112;
        mh._win212=_win122;
        mh._win221=_win212;
        mh._win213=_win132;
        mh._win231=_win312;
        mh._win321=_win213;
        mh._win312=_win123;
    }
    else if(nb1==3 && nb2==1 && nb3==2)
    {
        mh._win111=_win111;
        mh._win112=_win211;
        mh._win121=_win112;
        mh._win122=_win212;
        mh._win123=_win312;
        mh._win132=_win213;
        mh._win211=_win121;
        mh._win212=_win221;
        mh._win221=_win122;
        mh._win213=_win321;
        mh._win231=_win123;
        mh._win321=_win132;
        mh._win312=_win231;
    }
    else if(nb1==3 && nb2==2 && nb3==1)
    {
        mh._win111=_win111;
        mh._win112=_win211;
        mh._win121=_win121;
        mh._win122=_win221;
        mh._win123=_win321;
        mh._win132=_win231;
        mh._win211=_win112;
        mh._win212=_win212;
        mh._win221=_win122;
        mh._win213=_win312;
        mh._win231=_win132;
        mh._win321=_win123;
        mh._win312=_win213;
    }
    return mh;
}
//---------------------------------------------------------------------------
void    stVerManyHands::Norm(double val)
{
    double cn=CnAll();
    if(cn<DOUBLE_0) return;
    val/=cn;
    _win111*=val;
    _win112*=val;
    _win121*=val;
    _win122*=val;
    _win123*=val;
    _win132*=val;
    _win211*=val;
    _win212*=val;
    _win221*=val;
    _win213*=val;
    _win231*=val;
    _win321*=val;
    _win312*=val;
}
//---------------------------------------------------------------------------
void stVerManyHands::operator =(stVerManyHandsOld &val)
{
        _win111=val._win111;
        _win112=val._win112;
        _win121=val._win121;
        _win122=val._win122;
        _win123=val._win123;
        _win132=val._win132;
        _win211=val._win211;
        _win212=val._win212;
        _win221=val._win221;
        _win213=val._win213;
        _win231=val._win231;
        _win321=val._win321;
        _win312=val._win312;
        _cnHands3=0;
} 
//---------------------------------------------------------------------------
/*void SHCMemSetOld()
{
    glSHCarrHandsOld=(stVerManyHandsOld ***)malloc(CN_HAND*sizeof(stVerManyHandsOld**));
    for(int i=0;i<CN_HAND;i++)
    {
        glSHCarrHandsOld[i]=(stVerManyHandsOld **)malloc((i+1)*sizeof(stVerManyHandsOld*));
        for(int j=0;j<=i;j++)
            glSHCarrHandsOld[i][j]=(stVerManyHandsOld *)malloc((j+1)*sizeof(stVerManyHandsOld));
    }
}
void SHCLoadFromFileOld(int handle)
{
    for(int i=0;i<CN_HAND;i++)
    {
        for(int j=0;j<=i;j++)
            read(handle,glSHCarrHandsOld[i][j],(j+1)*sizeof(stVerManyHandsOld));
    }
}
bool SHCLoadFromFileOld(char *path)
{
    int handle=open(path,O_BINARY | O_RDWR);
    if(handle==-1)
        return(false);
    chmod(path,S_IREAD | S_IWRITE);
    SHCLoadFromFileOld(handle);
    close(handle);
    return(true);
} */
//---------------------------------------------------------------------------
void SHCMemSet()
{
    glSHCarrHands=(stVerManyHands ***)malloc(CN_HAND*sizeof(stVerManyHands**));
    for(int i=0;i<CN_HAND;i++)
    {
        glSHCarrHands[i]=(stVerManyHands **)malloc((i+1)*sizeof(stVerManyHands*));
        for(int j=0;j<=i;j++)
            glSHCarrHands[i][j]=(stVerManyHands *)malloc((j+1)*sizeof(stVerManyHands));
    }
    glSHCarrFullGH=(stVerManyHands *)malloc(CN_HAND*CN_HAND*sizeof(stVerManyHands));
}
void SHCClearCount()
{
    for(int i=0;i<CN_HAND;i++)
        for(int j=0;j<=i;j++)
            for(int k=0;k<=j;k++)
                glSHCarrHands[i][j][k]._cnHands3=0;
}
void SHCMemFree()
{
    for(int i=0;i<CN_HAND;i++)
    {
        for(int j=0;j<=i;j++)
            free(glSHCarrHands[i][j]);
        free(glSHCarrHands[i]);
    }
    free(glSHCarrHands);
    free(glSHCarrFullGH);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void SHCLoadFromFile(int handle)
{
	for (int i = 0; i<CN_HAND; i++)
	{
		for (int j = 0; j <= i; j++)
			_read(handle, glSHCarrHands[i][j], (j + 1)*sizeof(stVerManyHands));
	}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void SHCLoadFromFile(int handle)
{
	for (int i = 0; i<CN_HAND; i++)
	{
		for (int j = 0; j <= i; j++)
			for (int k = 0; k < j + 1; k++)
			{
				stVerManyHandsOld h;
				_read(handle, &h, sizeof(stVerManyHandsOld));
				glSHCarrHands[i][j][k] = h;
			}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void SHCWriteToFile(int handle)
{
    for(int i=0;i<CN_HAND;i++)
    {
        for(int j=0;j<=i;j++)
            _write(handle,glSHCarrHands[i][j],(j+1)*sizeof(stVerManyHands));
    }
}
void SHCWriteToFile(char *path)
{
	int handle;
	_sopen_s(&handle, "c:\\WORK\\binTrn6\\comp0.tmp", O_RDWR | O_BINARY | O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
    SHCWriteToFile(handle);
    _close(handle);
}
bool SHCLoadFromFile(char *path)
{
	int handle;
	_sopen_s(&handle, path, O_RDWR | O_BINARY | O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
    if(handle==-1)
        return(false);
    SHCLoadFromFile(handle);
    _close(handle);
    return(true);
}
//---------------------------------------------------------------------------
stVerManyHands  SHCHandsComprasion(int nbHand1,int nbHand2,int nbHand3)
{
    stVerManyHands mh;
    if(nbHand1>=nbHand2 && nbHand2>=nbHand3)
    {
        return (glSHCarrHands[nbHand1][nbHand2][nbHand3]);
    }
    else if(nbHand1>=nbHand3 && nbHand3>=nbHand2)
    {
        return (glSHCarrHands[nbHand1][nbHand3][nbHand2].Coverter(1,3,2));
//        mh=SHCHandsComprasion(nbHand1,nbHand3,nbHand2).Coverter(1,3,2);
    }
    else if(nbHand3>=nbHand1 && nbHand1>=nbHand2)
    {
        return (glSHCarrHands[nbHand3][nbHand1][nbHand2].Coverter(3,1,2));
//        mh=SHCHandsComprasion(nbHand3,nbHand1,nbHand2).Coverter(3,1,2);
    }
    else if(nbHand1<=nbHand2 && nbHand2<=nbHand3)
    {
        return (glSHCarrHands[nbHand3][nbHand2][nbHand1].Coverter(3,2,1));
//        mh=SHCHandsComprasion(nbHand3,nbHand2,nbHand1).Coverter(3,2,1);
    }
    else if(nbHand2>=nbHand1 && nbHand1>=nbHand3)
    {
        return (glSHCarrHands[nbHand2][nbHand1][nbHand3].Coverter(2,1,3));
//        mh=SHCHandsComprasion(nbHand2,nbHand1,nbHand3).Coverter(2,1,3);
    }
    else if(nbHand2>=nbHand3 && nbHand3>=nbHand1)
    {
        return (glSHCarrHands[nbHand2][nbHand3][nbHand1].Coverter(2,3,1));
//        mh=SHCHandsComprasion(nbHand2,nbHand3,nbHand1).Coverter(2,3,1);
    }
//    return mh;
}
//---------------------------------------------------------------------------
/*void SHCHandCmprnResult(int nbHand1,int nbHand2,int nbHand3,double *result,double *weight)
{
    stVerManyHands mh;
    if(nbHand1>=nbHand2 && nbHand2>=nbHand3)
    {
        mh=SHCHandsComprasion(nbHand1,nbHand2,nbHand3);
        result[0]=mh.WinEQHand(0);
        result[1]=mh.WinEQHand(1);
        result[2]=mh.WinEQHand(2);
        if(weight!=NULL)
        {
            weight[0]=mh._weightHand1;
            weight[1]=mh._weightHand2;
            weight[2]=mh._weightHand3;
        }
    }
    else if(nbHand1>=nbHand2 && nbHand2<=nbHand3 && nbHand1 >=nbHand3)
    {
        mh=SHCHandsComprasion(nbHand1,nbHand3,nbHand2);
        result[0]=mh.WinEQHand(0);
        result[1]=mh.WinEQHand(2);
        result[2]=mh.WinEQHand(1);
        if(weight!=NULL)
        {
            weight[0]=mh._weightHand1;
            weight[1]=mh._weightHand3;
            weight[2]=mh._weightHand2;
        }
    }
    else if(nbHand1>=nbHand2 && nbHand2<=nbHand3 && nbHand1 <=nbHand3)
    {
        mh=SHCHandsComprasion(nbHand3,nbHand1,nbHand2);
        result[0]=mh.WinEQHand(2);
        result[1]=mh.WinEQHand(0);
        result[2]=mh.WinEQHand(1);
        if(weight!=NULL)
        {
            weight[0]=mh._weightHand3;
            weight[1]=mh._weightHand1;
            weight[2]=mh._weightHand2;
        }
    }
    else if(nbHand1<=nbHand2 && nbHand2<=nbHand3)
    {
        mh=SHCHandsComprasion(nbHand3,nbHand2,nbHand1);
        result[0]=mh.WinEQHand(2);
        result[1]=mh.WinEQHand(1);
        result[2]=mh.WinEQHand(0);
        if(weight!=NULL)
        {
            weight[0]=mh._weightHand3;
            weight[1]=mh._weightHand2;
            weight[2]=mh._weightHand1;
        }
    }
    else if(nbHand1<=nbHand2 && nbHand2>=nbHand3 && nbHand1>=nbHand3)
    {
        mh=SHCHandsComprasion(nbHand2,nbHand1,nbHand3);
        result[0]=mh.WinEQHand(1);
        result[1]=mh.WinEQHand(0);
        result[2]=mh.WinEQHand(2);
        if(weight!=NULL)
        {
            weight[0]=mh._weightHand2;
            weight[1]=mh._weightHand1;
            weight[2]=mh._weightHand3;
        }
    }
    else if(nbHand1<=nbHand2 && nbHand2>=nbHand3 && nbHand1<=nbHand3)
    {
        mh=SHCHandsComprasion(nbHand2,nbHand3,nbHand1);
        result[0]=mh.WinEQHand(1);
        result[1]=mh.WinEQHand(2);
        result[2]=mh.WinEQHand(0);
        if(weight!=NULL)
        {
            weight[0]=mh._weightHand2;
            weight[1]=mh._weightHand3;
            weight[2]=mh._weightHand1;
        }
    }
} */
//---------------------------------------------------------------------------
void    SHCIncreaseCount(int nbHand1,int nbHand2,int nbHand3)
{
    if(nbHand1>=nbHand2 && nbHand2>=nbHand3)
        glSHCarrHands[nbHand1][nbHand2][nbHand3]._cnHands3++;
    else if(nbHand1>=nbHand3 && nbHand3>=nbHand2)
//        SHCIncreaseCount(nbHand1,nbHand3,nbHand2);
        glSHCarrHands[nbHand1][nbHand3][nbHand2]._cnHands3++;
    else if(nbHand3>=nbHand1 && nbHand1>=nbHand2)
//        SHCIncreaseCount(nbHand3,nbHand1,nbHand2);
        glSHCarrHands[nbHand3][nbHand1][nbHand2]._cnHands3++;
    else if(nbHand3>=nbHand2 && nbHand2>=nbHand1)
//        SHCIncreaseCount(nbHand3,nbHand2,nbHand1);
        glSHCarrHands[nbHand3][nbHand2][nbHand1]._cnHands3++;
    else if(nbHand2>=nbHand1 && nbHand1>=nbHand3)
//        SHCIncreaseCount(nbHand2,nbHand1,nbHand3);
        glSHCarrHands[nbHand2][nbHand1][nbHand3]._cnHands3++;
    else if(nbHand2>=nbHand3 && nbHand3>=nbHand1)
//        SHCIncreaseCount(nbHand2,nbHand3,nbHand1);
        glSHCarrHands[nbHand2][nbHand3][nbHand1]._cnHands3++;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)

