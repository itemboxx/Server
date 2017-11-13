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
double  CalcDisp(double m,int cn); //��������� �������� 0-1 � ������������ m � ������ ������ cn
void    RandNorm(double &v1,double &v2); // ��� ��������� �������� �������������� �� ����������� ������ � ������������ 0 � ���������� 1
double  RandNorm(); // ��������� �������� �������������� �� ����������� ������ � ������������ 0 � ���������� 1
double  CalcDispME(double me);//��������� �������� � ������������ me
double  AmendCalc(double cnS,double w,double midlW); // ����� cnS (10-1000) ������ �������� �����. w, midlW-�����. �������� �������� - ������� ��������� �������� � w
bool    Compare2Val(double val1,double val2,double dif); // ���� val1 ���������� �� val2 ������ ��� � dif return true;
int     Combination(int cnAll,int cn);  // ��������� cn �� cnAll
double  ProbabilityExc(int cnAll,int cnRound,int cn,int cnVar);  // ����� ���� cnAll ����������� ��� �� cnRound ������� ������� ����� cn ���� �� cnVar
double  ProbabilityOver(int cnAll,int cnRound,int cn,int cnVar);  // ����� ���� cnAll ����������� ��� �� cnRound ������� ������� >=cn ���� �� cnVar
bool    CompareAnyBytes(int cn,BYTE *arr,BYTE *arr1);
//---------------------------------------------------------------------------
#endif
