//---------------------------------------------------------------------------

#ifndef cTimeDataH
#define cTimeDataH
//---------------------------------------------------------------------------
class clDataDay
{
public :
    inline  clDataDay()         {_day=-1;}
    void    SetDay(int year,char *month,int day);
    inline  void    SetDay(int year,int nbM,int day)    {_day=year*10000+nbM*100+day;}
    inline  bool operator >(clDataDay &d0)   {return _day> d0._day;}
    inline  bool operator <(clDataDay &d0)   {return _day< d0._day;}
    int  operator -(clDataDay dat);
    inline  void    operator =(int d)   {int val=_day;_day=d;if(!Check()) _day=val;}
    inline  bool operator ==(clDataDay &val)     {return _day==val._day;}
    inline  bool operator !=(clDataDay &val)     {return _day!=val._day;}
    void    NextDay();
    void    PrevDay();
    inline  int Year()  {return _day/10000;}
    inline  int Month() {return (_day%10000)/100;}
    inline  int Day()   {return _day%100;}
    bool    Check();
//    AnsiString  ToString(char a);

    int _day;
};
class clDataTime
{
public:
    inline  int Hour()  {return _time/10000;}
    inline  int Minute(){return (_time%10000)/100;}
    inline  int Second(){return _time%100;}
    inline  void SetTime(int hour,int min,int sec)  {_time=hour*10000+min*100+sec;}
    inline  void SetTime(int sec)  {SetTime(sec/3600,sec%3600/60,sec%60);}
    inline  bool operator >(clDataTime &t0)   {return _time> t0._time;}
    inline  bool operator <(clDataTime &t0)   {return _time< t0._time;}
    inline  int operator -(clDataTime &t0)   {return ToSecond()-t0.ToSecond();}
    inline  int ToSecond()  {return Hour()*3600+Minute()*60+Second();}
    inline  void    operator =(int d)   {_time=d;}
    //AnsiString  ToString(char a);
    bool    Check();

    int _time;
};

struct stDayTime
{
    bool operator >(stDayTime &t0);
    bool operator <(stDayTime &t0);
    void AddSecond(int sec);
    int  operator -(stDayTime dat);
    //AnsiString  ToString(char d,char t); // d и t символы разделители даты и времени
    clDataDay _day;
    clDataTime _t;
};

#define DAY_IN_SEC      (24*3600)

stDayTime TimeDataToday();
//inline AnsiString TimeDataTodayStr()    {return TimeDataToday().ToStr();}
//---------------------------------------------------------------------------
#endif
