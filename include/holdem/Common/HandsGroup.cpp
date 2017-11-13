//---------------------------------------------------------------------------
//#include "stdafx.h"
#pragma hdrstop

#include <math.h>
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include <share.h>

#include "C:\home\util\WorkStr.h"
#include "C:\home\holdem\Common\HandsGroup.h"

//---------------------------------------------------------------------------
void    clGroupHands::operator +=(clGroupHands &gh)
{
    for(unsigned i=0;i<gh.size();i++)
        if(!this->Include(gh[i]))
            this->push_back(gh[i]);
}
//---------------------------------------------------------------------------
void    clGroupHands::operator =(tpListHand &list)
{
    resize(CN_HAND);
    for(int i=0;i<CN_HAND;i++)
        (*this)[i]=list._nbH[i];
}
//---------------------------------------------------------------------------
void    clGroupHands::WriteFile(int handle)
{
    int cn=size();
    _write(handle,&cn,sizeof(int));
    _write(handle,&this[0],cn*sizeof(short));
}
//---------------------------------------------------------------------------
void    clGroupHands::ReadFile(int handle)
{
    int cn;
    _read(handle,&cn,sizeof(int));
    resize(cn);
	_read(handle, &this[0], cn*sizeof(short));
}
//---------------------------------------------------------------------------
bool    clGroupHands::AddHand(clHand hand)
{
    short nbH=hand.NbHand();
    if(Include(nbH))
        return false;
    this->push_back(nbH);
    return true;
}
//---------------------------------------------------------------------------
bool    clGroupHands::Include(clCard *card)
{
    clHand *hand=(clHand *)card;
    return Include(hand->NbHand());
}
//---------------------------------------------------------------------------
bool    clGroupHands::Include(short nbH169)
{
    for(unsigned i=0;i<size();i++)
        if((*this)[i]==nbH169)
            return true;
    return false;
}
//---------------------------------------------------------------------------
void    clGroupHands::FillBest(tpListHand *list,double ur)
{
    clear();
    for(int i=0;i<CN_HAND;i++)
        if(list->_ur[i]>ur)
            break;
        else
            this->push_back(list->_nbH[i]);
}
//---------------------------------------------------------------------------
void    clGroupHands::FillBest(tpListHand *list,int cn)
{
    resize(cn);
    for(int i=0;i<cn;i++)
        (*this)[i]=list->_nbH[i];
}
//---------------------------------------------------------------------------
void    clGroupHands::FillFromGroup(clGroupHands *list,double ur)
{
    clear();
    for(unsigned i=0;i<list->size();i++)
    {
        ur-=HandWeight((*list)[i]);
        if(ur<0)
            break;
        else
            this->push_back((*list)[i]);
    }
}
//---------------------------------------------------------------------------
void    clGroupHands::FillFromGroup(clGroupHands *list,int cn)
{
    resize(cn);
    for(int i=0;i<cn;i++)
        (*this)[i]=(*list)[i];
}
//---------------------------------------------------------------------------
void    clGroupHands::FillAllHands()
{
    resize(CN_HAND);
    for(int i=0;i<CN_HAND;i++)
        (*this)[i]=i;
}
//---------------------------------------------------------------------------
double  clGroupHands::Weight()
{
    double res=0;
    for(unsigned i=0;i<size();i++)
        res+=HandWeight((*this)[i]);
    return res;
}
//---------------------------------------------------------------------------
void    clGroupHands::Skip(clGroupHands *gh)
{
    for(unsigned i=0;i<gh->size();i++)
        Skip((*gh)[i]);
}
//---------------------------------------------------------------------------
void    clGroupHands::Skip(short nbH)
{
    for(unsigned i=0;i<size();i++)
        if((*this)[i]==nbH)
            erase(begin()+i);
}
//---------------------------------------------------------------------------
//***************************************************************************
//---------------------------------------------------------------------------
void    clVGroupHands::WriteFile(int handle)
{
    int cn=size();
    _write(handle,&cn,sizeof(int));
    for(int i=0;i<cn;i++)
        (*this)[i].WriteFile(handle);
}
//---------------------------------------------------------------------------
void    clVGroupHands::ReadFile(int handle)
{
    int cn;
    _read(handle,&cn,sizeof(int));
    resize(cn);
    for(int i=0;i<cn;i++)
        (*this)[i].ReadFile(handle);
}
//---------------------------------------------------------------------------
int     clVGroupHands::NbGroupForHand(short nbH)
{
    for(unsigned i=0;i<size();i++)
    {
        clGroupHands &gh=(*this)[i];
        for(unsigned j=0;j<gh.size();j++)
            if(gh[j]==nbH)
                return i;
    }
    return -1;
}
//---------------------------------------------------------------------------
//***************************************************************************
//---------------------------------------------------------------------------
void    clGH0::operator -=(clGH0 &gh)
{
    for(int i=0;i<gh._cnH;i++)
    {
        for(int j=0;j<_cnH;j++)
        {
            if(_nbH[j]==gh._nbH[i])
            {
                if(_weightH[j]<=gh._weightH[i])
                {
                    DelRecord(j);
                }
                else
                    _weightH[j]-=gh._weightH[i];
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
bool    clGH0::operator ==(clGH0 &gh)
{
    if(_cnH != gh._cnH) return false;
    for(int i=0;i<_cnH;i++)
        if(fabs(_weightH[i]-gh._weightH[i])>DOUBLE_0)
            return false;
    return true;
}
//---------------------------------------------------------------------------
bool    clGH0::operator !=(clGH0 &gh)
{
    return !(*this==gh);
}
//---------------------------------------------------------------------------
int     clGH0::CheckRecord(int nbHand)
{
    for(int i=0;i<_cnH;i++)
        if(_nbH[i]==nbHand)
            return(i);
    return(-1);
}
//---------------------------------------------------------------------------
bool    clGH0::AddHand(int nbHand,int weightPerCent)
{
    if(CheckRecord(nbHand)!=-1)
        return(false);
    _nbH[_cnH]=nbHand;
    _weightH[_cnH]=weightPerCent*HandWeight(nbHand)/100;
    _cnH++;
    return(true);
}
//---------------------------------------------------------------------------
bool    clGH0::DelRecord(int nbRecord)
{
#ifdef  ERR_MESSAGE
    if(!ErrCheck("clImproveGroupHands::DelRecord",nbRecord,0,_cnH-1))
        return(false);
#endif
    for(int i=nbRecord;i<_cnH-1;i++)
    {
        _nbH[i]=_nbH[i+1];
        _weightH[i]=_weightH[i+1];
    }
    _cnH--;
    return(true);
}
//---------------------------------------------------------------------------
clGH0::clGH0()
{
    _cnH=0;
    for(int i=0;i<CN_HAND;i++)
    {
        _nbH[i]=0;
        _weightH[i]=0;
    }

}
//---------------------------------------------------------------------------
double  clGH0::Weight() const 
{
    double sum=0;
    for(int i=0;i<_cnH;i++)
        sum+=_weightH[i];
    return(sum);
}
//---------------------------------------------------------------------------
void    clGH0::ChangeWeight(int perCent,int nbRecord)
{
    _weightH[nbRecord]=perCent*HandWeight(_nbH[nbRecord])/100;
}
//---------------------------------------------------------------------------
double  clGH0::WeightHand(int nbH)
{
    for(int i=0;i<_cnH;i++)
        if(_nbH[i]==nbH)
            return _weightH[i];
    return(0);
}
//---------------------------------------------------------------------------
int     clGH0::WeightInPerCent(int nbHand)
{
    if(!ErrCheck("clGH0::WeightInPerCent",nbHand,0,CN_HAND-1))
        return 0;
    for(int i=0;i<_cnH;i++)
        if(_nbH[i]==nbHand)
            return(WeightInPerCentInArr(i));
    return(0);
}
//---------------------------------------------------------------------------
/*int     clGH0::WeightForAutoPlay(int nbHand)
{
    if(!ErrCheck("clGH0::WeightForAutoPlay",nbHand,0,CN_HAND-1))
        return 0;
    for(int i=0;i<_cnH;i++)
        if(_nbH[i]==nbHand)
            return(WeightForAutoPlayInArr(i));
    return(0);
} */
//---------------------------------------------------------------------------
void    clGH0::AddOneToOneWithoutLess(clGH0 &gh)
{
    for(int i=0;i<gh._cnH;i++)
    {
        int nbR=CheckRecord(gh._nbH[i]);
        if(nbR==-1)
        {
            AddHand(gh._nbH[i],gh._weightH[i]);
        }
        else
        {
            _weightH[nbR]+=gh._weightH[i];
        }
    }
}
//---------------------------------------------------------------------------
int     clGH0::WeightInPerCentInArr(int nb)
{
#ifdef  ERR_MESSAGE
    if(!ErrCheck("clImproveGroupHands::DelRecord",nb,0,_cnH-1))
        return(0);
#endif
    return(_weightH[nb]/HandWeight(_nbH[nb])*100+0.5);
}
//---------------------------------------------------------------------------
/*int     clGH0::WeightForAutoPlayInArr(int nb)
{
#ifdef  ERR_MESSAGE
    if(!ErrCheck("clImproveGroupHands::DelRecord",nb,0,_cnH-1))
        return(0);
#endif
    return(_weightH[nb]/HandWeight(_nbH[nb])*10000+0.5);
} */
//---------------------------------------------------------------------------
double  clGH0::CorrectWeight(double weightToCor)
{
    for(int i=0;i<_cnH;i++)
    {
        if(WeightInPerCentInArr(i)>=100)
            _weightH[i]=HandWeight(_nbH[i]);
    }
    double wHNew;
    if(Weight()<weightToCor)
    {
        wHNew=CorrectStep();
        while(wHNew<weightToCor)
        {
            wHNew=CorrectStep();
        }
    }
    else
    {
        wHNew=CorrectStepLess();
        while(wHNew>weightToCor)
        {
            wHNew=CorrectStepLess();
        }
    }
    return(wHNew);
}
//---------------------------------------------------------------------------
void    clGH0::CorrectFinal()
{
    for(int i=_cnH-1;i>=0;i--)
    {
        if(WeightInPerCentInArr(i)>=100)
            _weightH[i]=HandWeight(_nbH[i]);
        if(WeightInPerCentInArr(i)<0)
            DelRecord(i);

    }
}
//---------------------------------------------------------------------------
void  clGH0::CorrectWeightFaster(double weightToCor)
{
    double sum=0,sum100=0;
    for(int i=0;i<_cnH;i++)
    {
        if(WeightInPerCentInArr(i)<100)
            sum+=_weightH[i];
        else
            sum100+=_weightH[i];
    }
    double cof=(weightToCor-sum100)/sum;
    for(int i=0;i<_cnH;i++)
    {
        if(WeightInPerCentInArr(i)<100)
            _weightH[i]*=cof;
        if(WeightInPerCentInArr(i)>=100)
            _weightH[i]=HandWeight(_nbH[i]);
    }
}
void  clGH0::CorrectWeightFasterWithoutAnyLess100(double weightToCor)
{
    double sum=0;
    for(int i=0;i<_cnH;i++)
    {
            sum+=_weightH[i];
    }
    double cof=(weightToCor)/sum;
    for(int i=0;i<_cnH;i++)
    {
            _weightH[i]*=cof;
    }
}
double  clGH0::CorrectStep()
{
    double sum=0;
    for(int i=0;i<_cnH;i++)
    {
        double wH=HandWeight(_nbH[i]);
        _weightH[i]*=1.01;
        if(_weightH[i]>wH)
            _weightH[i]=wH;
        sum+=_weightH[i];
    }
    return(sum);
}
//---------------------------------------------------------------------------
double  clGH0::CorrectStepLess()
{
    double sum=0;
    for(int i=0;i<_cnH;i++)
    {
        int perCent=WeightInPerCentInArr(i);
        if(perCent<100)
            _weightH[i]*=0.99;
        sum+=_weightH[i];
    }
    return(sum);
}
//---------------------------------------------------------------------------
void    clGH0::ReMake(double weightGroup)
{
    if(weightGroup<=0) return;
    double sum=Weight();
    double x=sum/weightGroup;
    for(int i=0;i<_cnH;i++)
        _weightH[i]/=x;
}
//---------------------------------------------------------------------------
void    clGH0::ReMake()
{

}
//---------------------------------------------------------------------------
void    clGH0::AddOneHand(int nbHand)
{
    int nb=CheckRecord(nbHand);
    if(nb==-1)
    {
        _nbH[_cnH]=nbHand;
        _weightH[_cnH]=1;
        _cnH++;
    }
    else
        _weightH[nb]++;
}
//---------------------------------------------------------------------------
void    clGH0::ReadFilePack(int handle)
{
    _read(handle,&_cnH,sizeof(_cnH));
    _read(handle,_nbH,_cnH*sizeof(int));
    _read(handle,_weightH,_cnH*sizeof(double));
}
//---------------------------------------------------------------------------
void    clGH0::WriteFilePack(int handle)
{
    _write(handle,&_cnH,sizeof(_cnH));
    _write(handle,_nbH,_cnH*sizeof(int));
    _write(handle,_weightH,_cnH*sizeof(double));
}
//---------------------------------------------------------------------------
int     clGH0::SizeInFilePack()
{
    return sizeof(_cnH)+_cnH*(sizeof(int)+sizeof(double));
}
//---------------------------------------------------------------------------
void    clGH0::WriteFile(char *path)
{
	int handle; _sopen_s(&handle, path, O_BINARY | O_CREAT | O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
    WriteFile(handle);
    _close(handle);
}
void    clGH0::WriteFile(int handle)
{
    _write(handle,&_cnH,sizeof(int));
    _write(handle,&_nbH,CN_HAND*sizeof(int));
    _write(handle,&_weightH,CN_HAND*sizeof(double));
}
void    clGH0::ReadFile(char *path)
{
	int handle; _sopen_s(&handle, path, O_BINARY | O_CREAT | O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	ReadFile(handle);
    _close(handle);
}
void    clGH0::ReadFile(int handle)
{
    _read(handle,&_cnH,sizeof(int));
    _read(handle,&_nbH,CN_HAND*sizeof(int));
    _read(handle,&_weightH,CN_HAND*sizeof(double));
}
//---------------------------------------------------------------------------
bool    clGH0::AddHand(int nbHand,double weight)
{
    int nb=CheckRecord(nbHand);
    if(nb==-1)
    {
        _nbH[_cnH]=nbHand;
        _weightH[_cnH]=weight;
        _cnH++;
    }
    else
        _weightH[nb]+=weight;
    return(true);
}
//---------------------------------------------------------------------------
void    clGH0::operator +=(clGH0 &gh)
{
    for(int i=0;i<gh._cnH;i++)
    {
        int nbR=CheckRecord(gh._nbH[i]);
        if(nbR==-1)
            AddHand(gh._nbH[i],gh._weightH[i]);
        else
        {
            _weightH[nbR]+=gh._weightH[i];
            if(WeightInPerCentInArr(nbR)>=100)
                _weightH[nbR]=HandWeight(_nbH[nbR]);
        }
    }
}
//---------------------------------------------------------------------------
void    clGH0::MultWeight(double mult)    // умножаем все веса на mult
{
    for(int i=0;i<_cnH;i++)
        _weightH[i]*=mult;
}
//---------------------------------------------------------------------------
void    clGH0::FillBestHands(tpListHand *gr,int cn)
{
    _cnH=cn;
    for(int i=0;i<_cnH;i++)
    {
        int nb=gr->_nbH[i];
        double w=HandWeight(nb);
        _nbH[i]=nb;
        _weightH[i]=w;
    }
}
//---------------------------------------------------------------------------
void    clGH0::FillBestHands(tpListHand *gr,double weight)
{
    for(_cnH=0;_cnH<CN_HAND;_cnH++)
    {
        int nb=gr->_nbH[_cnH];
        double w=HandWeight(nb);
        _nbH[_cnH]=nb;
        if(weight>w)
        {
            weight-=w;
            _weightH[_cnH]=w;
        }
        else
        {
            _weightH[_cnH]=weight;
            _cnH++;
            break;
        }
    }
}
//---------------------------------------------------------------------------
void    clGH0::FillBestHands(tpListHand *gr,double w0,double w1)
{
    double sum=0;
    int i,nb;
    for(i=0;i<CN_HAND;i++)
    {
        nb=gr->_nbH[i];
        double w=HandWeight(nb);
        sum+=w;
        if(sum>w0)
            break;
    }
    sum-=w0;
    _cnH=1;
    _nbH[0]=nb;
    double weight=w1-w0;
    if(weight>sum)
        _weightH[0]=sum;
    else
    {
        _weightH[0]=weight;
        return;
    }
    for(i++;i<CN_HAND;i++)
    {
        int nb=gr->_nbH[i];
        double w=HandWeight(nb);
        _nbH[_cnH]=nb;
        if(w<weight)
        {
            weight-=w;
            _weightH[_cnH++]=w;
        }
        else
        {
            _weightH[_cnH]=weight;
            _cnH++;
            break;
        }
    }
}
//---------------------------------------------------------------------------
void    clGH0::BuildFrom(clGH0 *gr,double weight)
{
#ifdef ERR_MESSAGE
    if(!ErrCheck("clGH0::BuildFrom",gr->_cnH,0,CN_HAND)) return;
#endif
    double sumAll=0;
    for(int i=0;i<gr->_cnH;i++)
    {
        _nbH[i]=gr->_nbH[i];
        if(sumAll+gr->_weightH[i]>weight)
        {
            _weightH[i]=(weight-sumAll)/gr->_weightH[i]*HandWeight(_nbH[i]);
            _cnH=i+1;
            return;
        }
        _weightH[i]=HandWeight(_nbH[i]);
        sumAll+=gr->_weightH[i];
    }
    _cnH=gr->_cnH;
}
//---------------------------------------------------------------------------
void    clGH0::FillBestHands(clGH0 *gr,double weight)
{
#ifdef ERR_MESSAGE
    if(!ErrCheck("clGH0::FillBestHands",gr->_cnH,0,CN_HAND)) return;
#endif
    double sumAll=0;
    for(int i=0;i<gr->_cnH;i++)
    {
        _nbH[i]=gr->_nbH[i];
        if(sumAll+gr->_weightH[i]>weight)
        {
            _weightH[i]=weight-sumAll;
            _cnH=i+1;
            return;
        }
        _weightH[i]=gr->_weightH[i];
        sumAll+=gr->_weightH[i];
    }
    _cnH=gr->_cnH;
}
//---------------------------------------------------------------------------
void    clGH0::RemoveBestHands(clGH0 *gr,double weight)
{
    for(int i=0;i<gr->_cnH;i++)
    {
        int nb=CheckRecord(gr->_nbH[i]);
        if(nb==-1)
        {
            nb=_cnH;
            this->AddHand(gr->_nbH[i],0.);
        }
        if(gr->_weightH[i]>weight)
        {
            _weightH[nb]+=weight;
            gr->_weightH[nb]-=weight;
            return;
        }
        weight-=gr->_weightH[i];
        _weightH[nb]+=gr->_weightH[i];
        gr->_weightH[i]=0;
    }
}
//---------------------------------------------------------------------------
void    clGH0::FillBestHands(clGH0 *gr,double w0,double w1)
{
    double sum=0;
    int i,nb;
    for(i=0;i<gr->_cnH;i++)
    {
        nb=gr->_nbH[i];
        double w=gr->_weightH[i];
        sum+=w;
        if(sum>w0)
            break;
    }
    sum-=w0;
    _cnH=1;
    _nbH[0]=nb;
    double weight=w1-w0;
    if(weight>sum)
    {
        _weightH[0]=sum;
        weight-=sum;
    }
    else
    {
        _weightH[0]=weight;
        return;
    }
    for(i++;i<gr->_cnH;i++)
    {
        int nb=gr->_nbH[i];
        double w=gr->_weightH[i];
        _nbH[_cnH]=nb;
        if(w<weight)
        {
            weight-=w;
            _weightH[_cnH++]=w;
        }
        else
        {
            _weightH[_cnH]=weight;
            _cnH++;
            break;
        }
    }
}
//---------------------------------------------------------------------------
void    clGH0::ClearNullRec()
{
    int nb=0;
    for(int i=0;i<_cnH;i++)
        if(_weightH[i]>DOUBLE_0)
        {
            _nbH[nb]=_nbH[i];
            _weightH[nb++]=_weightH[i];
        }
    _cnH=nb;
}
//---------------------------------------------------------------------------
void    clGH0::FillAllHands()
{
    for(int i=0;i<CN_HAND;i++)
    {
        _nbH[i]=i;
        _weightH[i]=HandWeight(i);
    }
    _cnH=CN_HAND;
}
//---------------------------------------------------------------------------
void    clGH0::Cut(double w0,double weight)
{
    int i=0;
    for(;i<_cnH;i++)
        if(_weightH[i]<w0)
            w0-=_weightH[i];
        else break;
    if(i==_cnH)
    {
        _cnH=0;
        return;
    }
    _weightH[i]-=w0;
    int j=0,cn=_cnH-i;
    for(;j<cn;j++,i++)
    {
        _nbH[j]=_nbH[i];
        if(_weightH[i]<weight)
        {
            _weightH[j]=_weightH[i];
            weight-=_weightH[i];
            if(weight<DOUBLE_0)
                break;
        }
        else
        {
            _weightH[j]=weight;
            break;
        }
    }
    _cnH=j+1;
}
//---------------------------------------------------------------------------
void    clGH0::Skip(clGH0 *gh)
{
    for(int i=0;i<gh->_cnH;i++)
    {
        int nbR=CheckRecord(gh->_nbH[i]);
        if(nbR==-1) continue;
        _weightH[nbR]-=gh->_weightH[i];
        if(_weightH[nbR]<=0)
            DelRecord(nbR);
    }
}
//---------------------------------------------------------------------------
void    clGH0::SkipHand(int nbH,double weight)
{
    for(int i=0;i<_cnH;i++)
        if(_nbH[i]==nbH)
        {
            _weightH[i]-=weight;
            if(_weightH[i]<DOUBLE_0)
                DelRecord(i);
            break;
        }
}
//---------------------------------------------------------------------------
void    clGH0::SkipHandInArr(int nb,double weight)
{
    _weightH[nb]-=weight;
    if(_weightH[nb]<DOUBLE_0)
        DelRecord(nb);
}
//---------------------------------------------------------------------------
void    clGH0::operator =(tpListHand &list)
{
    for(int i=0;i<CN_HAND;i++)
    {
        _nbH[i]=list._nbH[i];
        _weightH[i]=HandWeight(_nbH[i]);
    }
    _cnH=CN_HAND;
}
//---------------------------------------------------------------------------
void    clGH0::operator =(clGroupHands &gh)
{
    for(unsigned i=0;i<gh.size();i++)
    {
        _nbH[i]=gh[i];
        _weightH[i]=HandWeight(_nbH[i]);
    }
    _cnH=gh.size();
}
//---------------------------------------------------------------------------
void    clGH0::operator =(int nbHand)
{
    _nbH[0]=nbHand;
    _weightH[0]=1;
    _cnH=1;
}
//---------------------------------------------------------------------------
bool    clGH0::IsFull()
{
    return(_cnH==CN_HAND && fabs(Weight()-1)<DOUBLE_0);
//    return _cnH==CN_HAND;
}
//---------------------------------------------------------------------------
void    clGH0::ChangeGH(clGH0 *gh,double ch)
{
    for(int i=0;i<_cnH;i++)
    {
        double w=gh->WeightHand(_nbH[i]);
        _weightH[i]+=(w-_weightH[i])*ch;
    }
    for(int i=0;i<gh->_cnH;i++)
        if(!IsHand(gh->_nbH[i]))
            AddHand(gh->_nbH[i],gh->_weightH[i]*ch);
}
//---------------------------------------------------------------------------
bool    clGH0::IsHand(int nbH)
{
    for(int i=0;i<_cnH;i++)
        if(_nbH[i]==nbH)
            return true;
    return false;
}
//---------------------------------------------------------------------------
void    clGH0::ReStraight(tpListHand &list)
{
    int cn=0;
    for(int i=0;i<CN_HAND;i++)
    {
        if(cn==_cnH) break;
        int nb=CheckRecord(list._nbH[i]);
        if(nb != -1)
        {
            int a=_nbH[nb];
            _nbH[nb]=_nbH[cn];
            _nbH[cn]=a;
            double d=_weightH[nb];
            _weightH[nb]=_weightH[cn];
            _weightH[cn]=d;
            cn++;
        }
    }
}
//---------------------------------------------------------------------------
void    clGH0::operator =(clShortGroupHands &gh)
{
    _cnH=0;
    for(unsigned i=0;i<gh.size();i++)
    {
        _nbH[_cnH]=gh[i]._nbHand;
        _weightH[_cnH++]=gh[i]._cn;
    }
}
//---------------------------------------------------------------------------
double  clGH0::Norm(double sum)
{
    double w=Weight();
    if(w<DOUBLE_0) return w;
    sum/=w;
    for(int i=0;i<_cnH;i++)
        _weightH[i]*=sum;
    return w;
}
//---------------------------------------------------------------------------
clAnsiString  clGH0::GetString(int nb)
{
    char str[12];
    clAnsiString as=clHand::NameHand(_nbH[nb],str);
    as+=(clAnsiString)"  "+WSDoubleToAS(_weightH[nb],6);
    return as;
}
//---------------------------------------------------------------------------
//***************************************************************************
//***************************************************************************
//---------------------------------------------------------------------------
void    clGH1::ReCalc()
{
    _wGr=0;
    for(int i=0;i<_cnH;i++)
    {
        if(_weightH[i]<0)
            ErrMessage("отрицательный вес","clImproveGroupHands(clGH1::ReCalc)");
        _wGr+=_weightH[i];
        _info[i]._wProc=WeightInPerCentInArr(i);
        clHand::NameHand(_nbH[i],_info[i]._nameH);
    }
}
//---------------------------------------------------------------------------
void    clGH1::FillAllHands()
{
    for(int i=0;i<CN_HAND;i++)
    {
        _nbH[i]=i;
        _weightH[i]=HandWeight(i);
    }
    _cnH=CN_HAND;
    ReCalc();
}
//---------------------------------------------------------------------------
void    clGH1::operator =(tpListHand &list)
{
    *(clGH0 *)this=list;
    ReCalc();
}
//---------------------------------------------------------------------------
void    clGH1::operator =(int nbHand)
{
    _nbH[0]=nbHand;
    _weightH[0]=HandWeight(nbHand);
    _cnH=1;
    _info[0]._wProc=100;
    clHand::NameHand(nbHand,_info[0]._nameH);
}
//---------------------------------------------------------------------------
void    clGH1::operator +=(clGH1 &gh)
{
    *(clGH0 *)this+=gh;
    ReCalc();
}
//---------------------------------------------------------------------------
void    clGH1::Skip(clGH1 *gh)
{
    clGH0::Skip((clGH0 *)gh);
    ReCalc();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
clImproveGroupHands HGBuildGroup(int cn)
{
    clImproveGroupHands gh;
    for(int i=0;i<cn;i++)
    {
        gh._nbH[i]=glListHand._nbH[i];
        gh._weightH[i]=HandWeight(gh._nbH[i]);
    }
    int cnH=4*cn;
#ifdef  ERR_MESSAGE
    if(cnH==cn)
    {
        ErrMessage("Error","HGBuildGroup");
        return gh;
    }
#endif
    double step=1./(cnH-cn);
    if(cnH>CN_HAND)
    {
        cnH=CN_HAND;
        if(step>0.01)
            step=0.01;
    }
    gh._cnH=cnH;
    for(int i=cn;i<cnH;i++)
    {
        gh._nbH[i]=glListHand._nbH[i];
        gh._weightH[i]=(1-(i+1-cn)*step)*HandWeight(gh._nbH[i]);
    }
    return gh;
}
//---------------------------------------------------------------------------
void HGLineApr(clImproveGroupHands  *gh1,clImproveGroupHands *gh2,double dw)
{
    for(int i=0;i<gh2->_cnH;i++)
    {
        gh2->_weightH[i]*=dw;
        if(i<gh1->_cnH)
            gh2->_weightH[i]+=gh1->_weightH[i]*(1-dw);
    }
}
//---------------------------------------------------------------------------
#define OLD_HANDGROUP
clImproveGroupHands HGGet3BetGroup(double weightGr)
{
#ifdef OLD_HANDGROUP
    clImproveGroupHands gh0,gh;
    gh0=glListHand;
    gh.FillBestHands(&gh0,weightGr);
#ifdef GH_TEST
    gh.ReCalc();
#endif
    return gh;
#else
    clImproveGroupHands gh1,gh2;
    double weight1=HandWeight(glListHand._nbH[0]);
    gh1._cnH=1;
    gh1._nbH[0]=glListHand._nbH[0];
    gh1._weightH[0]=weight1;
    if(weightGr<weight1)
    {
        gh1._weightH[0]=weightGr;
        return gh1;
    }
    for(int i=0;i<CN_HAND;i++)
    {
        gh2=HGBuildGroup(i+1);
        double weight2=gh2.Weight();
        if(weightGr<weight2)
        {
            double dw=(weightGr-weight1)/(weight2-weight1);
            HGLineApr(&gh1,&gh2,dw);
#ifdef  ERR_MESSAGE
            dw=gh2.Weight();
            if(fabs(dw-weightGr)>DOUBLE_0)
                ErrMessage("Не правильно определили вес","HGGet3BetGroup");
            ErrCheck("HGGet3BetGroup",gh2._cnH,0,CN_HAND);
#endif
            break;
        }
        weight1=weight2;
        gh1=gh2;
    }
#ifdef GH_TEST
    gh2.ReCalc();
#endif
    return gh2;
#endif
}
//---------------------------------------------------------------------------
clImproveGroupHands HGGetCall3BetGroup(double weightGr)
{
    return  HGGet3BetGroup(weightGr);
}
//---------------------------------------------------------------------------
//***************************************************************************
//---------------------------------------------------------------------------
void    clShortGroupHands::WriteFile(int handle)
{
    unsigned cn=size();
    _write(handle,&cn,sizeof(cn));
    _write(handle,&this[0],cn*sizeof(stUnitGH));
}
//---------------------------------------------------------------------------
void    clShortGroupHands::ReadFile(int handle)
{
    unsigned cn;
    _read(handle,&cn,sizeof(cn));
    resize(cn);
    _read(handle,&this[0],cn*sizeof(stUnitGH));
}
//---------------------------------------------------------------------------
void    clShortGroupHands::operator = (clImproveGroupHands &gh)
{
    clear();
    for(int i=0;i<gh._cnH;i++)
        if(gh._weightH[i]>0.5)
        {
            stUnitGH val;
            val._nbHand=gh._nbH[i];
            val._cn=gh._weightH[i]+0.5;
            this->push_back(val);
        }
}
//---------------------------------------------------------------------------
void    clShortGroupHands::operator +=(clShortGroupHands &val)
{
    for(unsigned i=0;i<val.size();i++)
    {
        int nb=FindNbHand(val[i]._nbHand);
        if(nb==-1)
            this->push_back(val[i]);
        else
            (*this)[nb]._cn+=val[i]._cn;
    }
}
//---------------------------------------------------------------------------
int     clShortGroupHands::FindNbHand(int nbHand)
{
    for(unsigned i=0;i<size();i++)
        if((*this)[i]._nbHand==nbHand)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
unsigned    clShortGroupHands::CnHands() const
{
    unsigned cn=0;
    for(unsigned i=0;i<size();i++)
        cn+=(*this)[i]._cn;
    return cn;
}
//---------------------------------------------------------------------------
//***************************************************************************
//***************************************************************************
//---------------------------------------------------------------------------
void    GHTransformGH(clVGroupHands *vgh,clImproveGroupHands &gr,clImproveGroupHands &source)
{
    gr.Clear();
    for(int i=0;i<source.CnHand();i++)
    {
        int nbH=source.NbHandInArr(i);
//        char str[6];
//        clHand::NameHand(nbH,str);
        int nb=vgh->NbGroupForHand(nbH);
        gr.AddHand(nb,source.WeightHandInArr(i));
    }
}
//---------------------------------------------------------------------------
void    GHTransformGH(clVGroupHands *vgh,clImproveGroupHands &gr,const clShortGroupHands &source)
{
    gr.Clear();
    for(unsigned i=0;i<source.size();i++)
    {
        int nb=vgh->NbGroupForHand(source[i]._nbHand);
        gr.AddHand(nb,(double)source[i]._cn);
    }
}
//---------------------------------------------------------------------------
void    GHTransformGHToHands(clVGroupHands *vgh,clImproveGroupHands &gr,clImproveGroupHands &source)
{
    gr.Clear();
    for(int i=0;i<source.CnHand();i++)
    {
        int nbGr=source.NbHandInArr(i);
        double wGr=0;
        clGroupHands &gh0=(*vgh)[nbGr];
        for(unsigned j=0;j<gh0.size();j++)
            wGr+=HandWeight(gh0[j]);
        double k=source.WeightHandInArr(i)/wGr;
        for(unsigned j=0;j<gh0.size();j++)
            gr.AddHand(gh0[j],HandWeight(gh0[j])*k);
    }
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
