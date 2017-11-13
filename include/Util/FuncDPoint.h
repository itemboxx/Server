//---------------------------------------------------------------------------

#ifndef FuncDPointH
#define FuncDPointH
//---------------------------------------------------------------------------
#include <vector>

#include "c:\\Home\\Util\\errMessage.h"

class clDPoint
{
public:
	double  Distance(clDPoint p0);
	bool operator  == (clDPoint p0)		{ return x == p0.x && y == p0.y; }

	tpFloat x,y;
};

class clDRect
{
public:
	inline  tpFloat  Width()     { return right - left; }
	inline  tpFloat  Height()    { return top - bottom; }

	tpFloat top, bottom, left, right;
};

/*class clArrDPoint
{
public:
	clArrDPoint();
	inline  ~clArrDPoint()          {Clear();}
	void    Clear();
	void    operator = (clArrDPoint &arr);
	void    operator += (double k);
	void    operator /= (double k);
	int WriteFile(int handle);
	int ReadFile(int handle);
	int AddPoint(double x,double y);
	inline  int AddPoint(clDPoint p)        {return AddPoint(p._x,p._y);}
	int InsertPoint(clDPoint p);
	int InsertPoint(int nb,clDPoint p);
	void    DelPoint(int nb);
	void    CutLeft(int nb);
    inline  void    CutRight(int nb)        {SetDim(nb);}
    inline  void    WrPoint(int nb,double x,double y)   {_arr[nb]._x=x;_arr[nb]._y=y;}
    inline  void    WrPoint(int nb,clDPoint p)   {_arr[nb]=p;}
    void    SetDim(int dim);
    inline  clDPoint operator [](int nb)         {return _arr[nb];}
    inline  int Dimension()         {return _cnV;}
    inline  double  Width()         {return _arr[_cnV-1]._x-_arr[0]._x;}
    inline  clDPoint    LastPoint()         {return _arr[_cnV-1];}
    void    Derivative(clArrDPoint *func);
    void    Derivative(clArrDPoint *func,double dx);
    double  DerivativeLineI(double x,double dx);
    double  LineInterpol(double x);
    double ReverseLineInterpol(double y);
    void    ShiftX(double x);
    inline  bool IsIncreaseFunc()           {if(_cnV<2) return false; return _arr[_cnV-1]._y>_arr[0]._y;}

    int _cnV;
    clDPoint *_arr;
};
*/
tpFloat	PerpendicularDistance(clDPoint p, clDPoint a1, clDPoint a2); // Расстояние от точки p до прямой (a1,a2)
size_t	DouglasPeucker(size_t dim, clDPoint *arr, tpFloat epsilon);				// Сокращаем ломаную до ломаной с меньшим кол-вом точек, с точностью epsilon, возвращаем кол-во точек новой ломаной
//---------------------------------------------------------------------------
#endif
