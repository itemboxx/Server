//---------------------------------------------------------------------------

#ifndef usefullProcH
#define usefullProcH
//---------------------------------------------------------------------------
#include <vector>
using namespace std;

struct stResCount
{
    double _res;
    int _cn;
};
typedef unsigned char BYTE;

void    SortArr(int cn,double *arr);
void    SortArr(int cn,int *arr);
void    SortArr(int cn,int *arr,int *arrNb);
double  Average(int cn,double *arr);
double  Average(vector <stResCount> &vect);
double  AverageAndDisp(int cn,double *arr,double &disp);
double  AverageAndDisp(double &disp,vector <stResCount> &vect);
double  AverageAndDispOnRound(int cn,double *arr,double &disp);
double  DistBetweenArr(int cn,double *arr1,double *arr2);
double  UsefullMax(int cn,double *arr);
double  UsefullMin(int cn,double *arr);
int     UsefullNbMax(int cn,double *arr);
int     UsefullNbMin(int cn,double *arr);
int     UsefullMax(int cn,int *arr);
int     UsefullMin(int cn,int *arr);
double  CalcDisp(double m,int cn); //дисперсия процесса 0-1 с матожиданием m и числом опытов cn
void    RandNorm(double &v1,double &v2); // Две случайные величины распределенных по нормальному закону с матожиданием 0 и дисперсией 1
double  RandNorm(); // Случайная величины распределенная по нормальному закону с матожиданием 0 и дисперсией 1
double  CalcDispME(double me);//дисперсия процесса с матожиданием me
double  AmendCalc(double cnS,double w,double midlW); // после cnS (10-1000) опытов получено матож. w, midlW-матож. похожего процесса - функция вычисляет поправку к w
bool    Compare2Val(double val1,double val2,double dif); // если val1 отличается от val2 меньше чем в dif return true;
int     Combination(int cnAll,int cn);  // сочетание cn из cnAll
double  ProbabilityExc(int cnAll,int cnRound,int cn,int cnVar);  // всего карт cnAll вероятность что за cnRound попыток прийдет ровно cn карт из cnVar
double  ProbabilityOver(int cnAll,int cnRound,int cn,int cnVar);  // всего карт cnAll вероятность что за cnRound попыток прийдет >=cn карт из cnVar
bool    CompareAnyBytes(int cn,BYTE *arr,BYTE *arr1);
//---------------------------------------------------------------------------
#endif
