//---------------------------------------------------------------------------
#pragma hdrstop

#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <time.h>

#include "cTimeData.h"

//---------------------------------------------------------------------------
void    clDataDay::SetDay(int year,char *month,int day)
{
    int nbM;
    if(strcmp(month,"January")==0)
        nbM=1;
    else if(strcmp(month,"February")==0)
        nbM=2;
    else if(strcmp(month,"March")==0)
        nbM=3;
    else if(strcmp(month,"April")==0)
        nbM=4;
    else if(strcmp(month,"May")==0)
        nbM=5;
    else if(strcmp(month,"June")==0)
        nbM=6;
    else if(strcmp(month,"July")==0)
        nbM=7;
    else if(strcmp(month,"August")==0)
        nbM=8;
    else if(strcmp(month,"September")==0)
        nbM=9;
    else if(strcmp(month,"October")==0)
        nbM=10;
    else if(strcmp(month,"November")==0)
        nbM=11;
    else if(strcmp(month,"December")==0)
        nbM=12;
    else
        nbM=99;
    SetDay(year,nbM,day);
}
//---------------------------------------------------------------------------
void    clDataDay::NextDay()
{
    int month=Month(),year=Year(),day=Day(),cnD;
    if(month==2)
        cnD=(year%4==0)? 29 : 28;
    else if(month==4 || month==6 || month==9 || month==11)
        cnD=30;
    else cnD=31;
    day++;
    if(day>cnD)
    {
        month++;
        day=1;
        if(month>12)
        {
            month=1;
            year++;
        }
    }
    SetDay(year,month,day);
}
//---------------------------------------------------------------------------
void    clDataDay::PrevDay()
{
    int month=Month(),year=Year(),day=Day();
    day--;
    if(day==0)
    {
        month--;
        if(month==0)
        {
            month=12;
            year--;
        }
        if(month==2)
            day=(year%4==0)? 29 : 28;
        else if(month==4 || month==6 || month==9 || month==11)
            day=30;
        else day=31;
    }
    SetDay(year,month,day);
}
//---------------------------------------------------------------------------
/*AnsiString  clDataDay::ToString(char a)
{
    AnsiString as,as0;
    as=(AnsiString)Year()+(AnsiString)a;
    int val=Month();
    if(val<10) as+=0;
    as+=(AnsiString)val+(AnsiString)a;
    val=Day();
    if(val<10) as+=0;
    as+=val;
    return as;
}*/
//---------------------------------------------------------------------------
bool    clDataDay::Check()
{
    int month=Month(),year=Year(),day=Day(),cnD;
    if(year<0)
        return false;
    if(month<1 || month>12)
        return false;
    if(month==2)
        cnD=(year%4==0)? 29 : 28;
    else if(month==4 || month==6 || month==9 || month==11)
        cnD=30;
    else cnD=31;
    if(day<1 || day>cnD)
        return false;
    return true;
}
//---------------------------------------------------------------------------
int  clDataDay::operator -(clDataDay dat)
{
    int res=0;
    while(_day>dat._day)
    {
        dat.NextDay();
        res++;
    }
    while(_day<dat._day)
    {
        dat.PrevDay();
        res--;
    }
    return res;
}
//---------------------------------------------------------------------------
stDayTime TimeDataToday()
{
   time_t timer;
   struct tm *tblock;
   timer = time(NULL);
   tblock = localtime(&timer);
   stDayTime data;
   data._day.SetDay(1900+tblock->tm_year,1+tblock->tm_mon,tblock->tm_mday);
   data._t.SetTime(tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
   return data;
}
//---------------------------------------------------------------------------
/*AnsiString  clDataTime::ToString(char a)
{
    AnsiString as,as0;
    as=(AnsiString)Hour()+(AnsiString)a;
    int val=Minute();
    if(val<10) as+=0;
    as+=(AnsiString)val+(AnsiString)a;
    val=Second();
    if(val<10) as+=0;
    as+=val;
    return as;
}*/
//---------------------------------------------------------------------------
bool    clDataTime::Check()
{
    if(_time<0) return false;
    if(Hour()>23) return false;
    if(Minute()>59) return false;
    if(Second()>59) return false;
    return true;
}
//---------------------------------------------------------------------------
void stDayTime::AddSecond(int sec)
{
    sec+=_t.ToSecond();
    while(sec<0)
    {
        sec+=DAY_IN_SEC;
        _day.PrevDay();
    }
    while(sec>DAY_IN_SEC)
    {
        sec-=DAY_IN_SEC;
        _day.NextDay();
    }
    _t.SetTime(sec);
}
//---------------------------------------------------------------------------
bool stDayTime::operator >(stDayTime &t0)
{
    if(_day>t0._day)
        return(true);
    else if(_day<t0._day)
        return(false);
    else if(_t>t0._t)
        return(true);
    else if(_t<t0._t)
        return(false);
    return(false);
}
//---------------------------------------------------------------------------
bool stDayTime::operator <(stDayTime &t0)
{
    if(_day>t0._day)
        return(false);
    else if(_day<t0._day)
        return(true);
    else if(_t>t0._t)
        return(false);
    else if(_t<t0._t)
        return(true);
    return(false);
}
//---------------------------------------------------------------------------
int  stDayTime::operator -(stDayTime dat)
{
    int res=0;
    while(_day>dat._day)
    {
        dat._day.NextDay();
        res+=DAY_IN_SEC;
    }
    while(_day<dat._day)
    {
        dat._day.PrevDay();
        res-=DAY_IN_SEC;
    }
    return res+(_t-dat._t);
}
//---------------------------------------------------------------------------
/*AnsiString  stDayTime::ToString(char d,char t)
{
    return _day.ToString(d)+(AnsiString)' '+_t.ToString(t);
}*/
//---------------------------------------------------------------------------

#pragma package(smart_init)
