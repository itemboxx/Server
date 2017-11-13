//---------------------------------------------------------------------------

#ifndef HandsGroupH
#define HandsGroupH

#include <vector>
#include "c:\home\Util\WorkStr.h"
#include "C:\home\card\Hand.h"
//#include "clHandsCompare.h"

//#define GH_TEST

class clGroupHands : public std::vector <short>
{
public:
    void    operator =(tpListHand &list);
    void    operator +=(clGroupHands &gh);
    void    WriteFile(int handle);
    void    ReadFile(int handle);
    bool    Include(clCard *card);
    bool    Include(short nbH169);
    void    FillBest(tpListHand *list,double ur);
    void    FillBest(tpListHand *list,int cn);
    void    FillFromGroup(clGroupHands *list,double ur);
    void    FillFromGroup(clGroupHands *list,int cn);
    void    FillAllHands();
    bool    AddHand(clHand hand);
    inline  bool    IsEmpty()       {return size()==0;}
    double  Weight();
    void    Skip(clGroupHands *gh);
    void    Skip(short nbH);

};

class clVGroupHands : public std::vector <clGroupHands>
{
public:
    void    WriteFile(int handle);
    void    ReadFile(int handle);
    int     NbGroupForHand(short nbH);
};

class clGH0
{
friend class clShortGroupHands;
public:
    clGH0();
    inline  void    Clear()     {_cnH=0;}
    inline  int CnHand() const            {return _cnH;}
    inline  int NbHandInArr(int n)  {return _nbH[n];}

    void    AddOneHand(int nbHand);
    void    WriteFile(char *path);
    void    WriteFile(int handle);
    void    ReadFile(char *path);
    void    ReadFile(int handle);
    void    ReadFilePack(int handle);
    void    WriteFilePack(int handle);
    int     SizeInFilePack();
    void    ReMake(double weightGroup);
    void    ReMake();
    double  Weight() const;
    double  WeightHand(int nbH);
    inline  double  WeightHandInArr(int nb)         {return _weightH[nb];}
    int     WeightInPerCent(int nbHand);
    int     WeightInPerCentInArr(int nb);
    int     WeightForAutoPlayInArr(int nb);
    int     WeightForAutoPlay(int nb);
    void    ChangeWeight(int perCent,int nbRecord);
    int     CheckRecord(int nbHand);
    bool    DelRecord(int nbRecord);
    bool    AddHand(int nbHand,int weightPerCent);
    bool    AddHand(int nbHand,double weight);
//    inline  bool    AddHandWidthCheck(int nbHand,double weight) {return AddHand(nbHand,weight);}
    inline int GetNbHand(int nbRecord)      {return(_nbH[nbRecord]);}
    void    operator +=(clGH0 &gh);
    void    operator -=(clGH0 &gh);
    bool    operator ==(clGH0 &gh);
    bool    operator !=(clGH0 &gh);
    void    operator =(tpListHand &list);
    void    operator =(clGroupHands &gh);
    void    operator =(int nbHand);
    void    operator =(clShortGroupHands &gh);
    void    AddOneToOneWithoutLess(clGH0 &gh);
    void    Skip(clGH0 *gh);
    void    SkipHand(int nbH,double weight);
    void    SkipHandInArr(int nb,double weight);
    bool    IsHand(int nbH);
    void    ChangeGH(clGH0 *gh,double ch);
    void    MultWeight(double mult);    // умножаем все веса на mult
    void    BuildFrom(clGH0 *gr,double weight);
    void    FillBestHands(clGH0 *gr,double weight);
    void    RemoveBestHands(clGH0 *gr,double weight);
    void    FillBestHands(clGH0 *gr,double w0,double w1); // хэнды от w0 до w1
    void    FillBestHands(tpListHand *gr,int cn);
    void    FillBestHands(tpListHand *gr,double weight);
    void    FillBestHands(tpListHand *gr,double w0,double w1);// хэнды от w0 до w1
    void    FillAllHands();
    void    Cut(double w0,double weight);

    inline bool    IsEmpty()                {return(_cnH==0);}
    bool    IsFull();
    double  Norm(double sum=1);

    void    CorrectWeightFaster(double weightToCor);
    void    CorrectWeightFasterWithoutAnyLess100(double weightToCor);

    void    CorrectFinal();
    double  CorrectWeight(double weirghtToCor);
    double  CorrectStep();
    double  CorrectStepLess();
    inline  void    ReCalc()            {}
    void    ReStraight(tpListHand &list);
    void    ClearNullRec();
    clAnsiString  GetString(int nb);

    int _cnH,_nbH[CN_HAND];
    double _weightH[CN_HAND];
};

class clGH1 : public clGH0
{
public :
    void    ReCalc();
    void    FillAllHands();
    void    operator =(tpListHand &list);
    void    operator =(int nbHand);
    void    Skip(clGH1 *gh);
    void    operator +=(clGH1 &gh);

    double _wGr;
    struct
    {
        char _nameH[4];
        int _wProc;
    }_info[CN_HAND];
};
#ifdef GH_TEST
typedef clGH1 clImproveGroupHands;
#else
typedef clGH0 clImproveGroupHands;
#endif

struct stUnitGH
{
    unsigned short _nbHand,_cn;
};

class clShortGroupHands : public std::vector <stUnitGH>
{
public:
    void    WriteFile(int handle);
    void    ReadFile(int handle);
    unsigned    CnHands() const;
    void    operator = (clImproveGroupHands &gh);
    void    operator +=(clShortGroupHands &val);
    int     FindNbHand(int nbHand);
};

extern clVGroupHands glVGH_CW_16,glVGH_CW_73,glVGH_Raise_46,glVGH_Raise_93,glMyVGH;

void    GHTransformGHToHands(clVGroupHands *vgh,clImproveGroupHands &gr,clImproveGroupHands &source);
void    GHTransformGH(clVGroupHands *vgh,clImproveGroupHands &gr,clImproveGroupHands &source);
void    GHTransformGH(clVGroupHands *vgh,clImproveGroupHands &gr,const clShortGroupHands &source);
//---------------------------------------------------------------------------
#endif
