//---------------------------------------------------------------------------

#ifndef SeveralHandCmpH
#define SeveralHandCmpH

#include "..\..\card\Hand.h"


struct stVerManyHandsOld
{
    inline stVerManyHandsOld()         {Clear();}
    inline void Clear() {_win111=_win112=_win121=_win122=_win123=_win132=_win211=_win212=_win221=_win213=_win231=_win321=_win312=0;}
    inline double CnAll()  {return _win111+_win112+_win121+_win122+_win123+_win132+_win211+_win212+_win221+_win213+_win231+_win321+_win312;}
    inline double WinFirstHand()    {return CnAll()==0 ? 0 : double(_win123+_win132+_win122)/CnAll();}
    inline double WinSecondHand()    {return CnAll()==0 ? 0 : double(_win212+_win213+_win312)/CnAll();}
    inline double WinThirdHand()    {return CnAll()==0 ? 0 : double(_win221+_win231+_win321)/CnAll();}
    inline double WinEQFirstHand()    {return CnAll()==0 ? 0 : double(_win123+_win132+_win122+_win111/3.0 +_win112/2.0+_win121/2.0)/CnAll();}
    inline double WinEQSecondHand()    {return CnAll()==0 ? 0 : double(_win212+_win213+_win312+_win111/3.0+_win112/2.0+_win211/2.0)/CnAll();}
    inline double WinEQThirdHand()    {return CnAll()==0 ? 0 : double(_win221+_win231+_win321+_win111/3.0+_win121/2.0+_win211/2.0)/CnAll();}

    inline double WinSecnondWithoutFirst() {return CnAll()==0 ? 0 : double(_win112+_win123+_win212+_win213+_win312)/CnAll();}
    inline double WinThirdWithoutFirst() {return CnAll()==0 ? 0 : double(_win121+_win132+_win221+_win231+_win321)/CnAll();}

    inline double WinEQSecnondWithoutFirst() {return CnAll()==0 ? 0 : double(_win111/2+_win112+_win122/2+_win123+_win211/2+_win212+_win213+_win312)/CnAll();}
    inline double WinEQThirdWithoutFirst() {return CnAll()==0 ? 0 : double(_win111/2+_win121+_win122/2+_win132+_win211/2+_win221+_win231+_win321)/CnAll();}
    inline double WinEQFirstWithoutSecond() {return CnAll()==0 ? 0 : double(_win111/2+_win112+_win121/2+_win122+_win123+_win132+_win212/2+_win213)/CnAll();}
    inline double WinEQThirdWithoutSecond() {return CnAll()==0 ? 0 : double(_win111/2+_win211+_win121/2+_win221+_win321+_win231+_win212/2+_win312)/CnAll();}
    inline double WinEQFirstWithoutThird() {return CnAll()==0 ? 0 : double(_win111/2+_win112/2+_win121+_win122+_win123+_win132+_win221/2+_win231)/CnAll();}
    inline double WinEQSecondWithoutThird() {return CnAll()==0 ? 0 : double(_win111/2+_win112/2+_win211+_win212+_win213+_win312+_win221/2+_win321)/CnAll();}

    double  WinEQHandWithout(int nbPl,int nbPl0);       // 0 ,1 ,2  розыгрыш 2-ого банка
//    stVerManyHands Coverter(int nb1,int nb2,int nb3);   // подавать только 1 2 3 пример 1 2 3  просто скопирует , 1 3 2 меняет местами 2-ого и 3-его
    double WinEQHand(int nbPl);  // 0 , 1 , 2
    float _win111,_win112,_win121,_win122,_win123,_win132;
    float _win211,_win212,_win221,_win213,_win231;
    float _win321,_win312;
    double _weightHand1,_weightHand2,_weightHand3;                  // вес хенда относительно 2 других
    inline void operator /=(float del)        {_win111/=del; _win112/=del;
            _win121/=del; _win122/=del; _win123/=del; _win132/=del; _win211/=del; _win212/=del;
            _win221/=del; _win213/=del; _win231/=del; _win321/=del; _win312/=del;}

};

struct stVerManyHands
{
    inline stVerManyHands()         {Clear();}
    inline void Clear() {_win111=_win112=_win121=_win122=_win123=_win132=_win211=_win212=_win221=_win213=_win231=_win321=_win312=0;}
    inline double CnAll()  {return _win111+_win112+_win121+_win122+_win123+_win132+_win211+_win212+_win221+_win213+_win231+_win321+_win312;}
    inline double WinFirstHand()    {return CnAll()==0 ? 0 : double(_win123+_win132+_win122)/CnAll();}
    inline double WinSecondHand()    {return CnAll()==0 ? 0 : double(_win212+_win213+_win312)/CnAll();}
    inline double WinThirdHand()    {return CnAll()==0 ? 0 : double(_win221+_win231+_win321)/CnAll();}
    inline double WinEQFirstHand()    {return CnAll()==0 ? 0 : double(_win123+_win132+_win122+_win111/3.0 +_win112/2.0+_win121/2.0)/CnAll();}
    inline double WinEQSecondHand()    {return CnAll()==0 ? 0 : double(_win212+_win213+_win312+_win111/3.0+_win112/2.0+_win211/2.0)/CnAll();}
    inline double WinEQThirdHand()    {return CnAll()==0 ? 0 : double(_win221+_win231+_win321+_win111/3.0+_win121/2.0+_win211/2.0)/CnAll();}

    inline double WinSecnondWithoutFirst() {return CnAll()==0 ? 0 : double(_win112+_win123+_win212+_win213+_win312)/CnAll();}
    inline double WinThirdWithoutFirst() {return CnAll()==0 ? 0 : double(_win121+_win132+_win221+_win231+_win321)/CnAll();}

    inline double WinEQSecnondWithoutFirst() {return CnAll()==0 ? 0 : double(_win111/2+_win112+_win122/2+_win123+_win211/2+_win212+_win213+_win312)/CnAll();}
    inline double WinEQThirdWithoutFirst() {return CnAll()==0 ? 0 : double(_win111/2+_win121+_win122/2+_win132+_win211/2+_win221+_win231+_win321)/CnAll();}
    inline double WinEQFirstWithoutSecond() {return CnAll()==0 ? 0 : double(_win111/2+_win112+_win121/2+_win122+_win123+_win132+_win212/2+_win213)/CnAll();}
    inline double WinEQThirdWithoutSecond() {return CnAll()==0 ? 0 : double(_win111/2+_win211+_win121/2+_win221+_win321+_win231+_win212/2+_win312)/CnAll();}
    inline double WinEQFirstWithoutThird() {return CnAll()==0 ? 0 : double(_win111/2+_win112/2+_win121+_win122+_win123+_win132+_win221/2+_win231)/CnAll();}
    inline double WinEQSecondWithoutThird() {return CnAll()==0 ? 0 : double(_win111/2+_win112/2+_win211+_win212+_win213+_win312+_win221/2+_win321)/CnAll();}

    double  WinEQHandWithout(int nbPl,int nbPl0);       // 0 ,1 ,2  розыгрыш 2-ого банка
    stVerManyHands Coverter(int nb1,int nb2,int nb3);   // подавать только 1 2 3 пример 1 2 3  просто скопирует , 1 3 2 меняет местами 2-ого и 3-его
    double WinEQHand(int nbPl);  // 0 , 1 , 2
    float _win111,_win112,_win121,_win122,_win123,_win132;
    float _win211,_win212,_win221,_win213,_win231;
    float _win321,_win312;
    int _cnHands3;  // сколько различных сочетаний 3 хендов возможно, фактически относительный вес появления трех хендов
    inline void operator +=(stVerManyHands ver2)        {_win111+=ver2._win111; _win112+=ver2._win112;
            _win121+=ver2._win121; _win122+=ver2._win122; _win123+=ver2._win123; _win132+=ver2._win132; _win211+=ver2._win211; _win212+=ver2._win212;
            _win221+=ver2._win221; _win213+=ver2._win213; _win231+=ver2._win231; _win321+=ver2._win321; _win312+=ver2._win312;}
    stVerManyHands operator *(float a);
    inline void operator /=(float del)        {_win111/=del; _win112/=del;
            _win121/=del; _win122/=del; _win123/=del; _win132/=del; _win211/=del; _win212/=del;
            _win221/=del; _win213/=del; _win231/=del; _win321/=del; _win312/=del;}
    void operator =(stVerManyHandsOld &val);
    void    Norm(double sum=1);

};

extern stVerManyHands ***glSHCarrHands;
//extern stVerManyHandsOld ***glSHCarrHandsOld;
extern stVerManyHands *glSHCarrFullGH;         // массив 169х169 результатов сравнений хенд1,хенд2,полная группа

//void SHCMemSetOld();
//bool SHCLoadFromFileOld(char *path);

void SHCMemSet();
void SHCMemFree();
void SHCClearArray(); // зануляет массив
void SHCClearCount();
void SHCWriteToFile(char *path);
bool SHCLoadFromFile(char *path);
stVerManyHands  SHCHandsComprasion(int nbHand1,int nbHand2,int nbHand3);
void   SHCHandCmprnResult(int nbHand1,int nbHand2,int nbHand3,double *result,double *weight=NULL); // не важно возрастание , result - 3 * double или error
void    SHCIncreaseCount(int nbH1,int nbH2,int nbH3);
//---------------------------------------------------------------------------
#endif
