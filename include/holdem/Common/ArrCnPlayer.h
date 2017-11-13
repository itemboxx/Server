//---------------------------------------------------------------------------

#ifndef ArrCnPlayerH
#define ArrCnPlayerH
//---------------------------------------------------------------------------
#include "History.h"

class clDoubleCP
{
public:
	clDoubleCP()							{}
	clDoubleCP(tpFloat0 val)							{ *this = val; }
	void    FillArr(tpFloat0 val);
	inline tpFloat0& operator[](int elem)   { return _arr[elem]; }
	clDoubleCP operator * (tpFloat0 val);
    void    operator +=(clDoubleCP val);
	void    operator =(tpFloat0 val);
	void    operator /=(tpFloat0 val);
	void    Norm(tpFloat0 sum = 1);
	inline  operator tpFloat0 *()             { return _arr; }
	tpFloat0  Sum();

	tpFloat0 _arr[CN_PLAYER];
};

class clIntCP
{
public:
    inline int& operator[]( int elem )   {return _arr[ elem ];}
    clIntCP operator * (double val);
    void    operator +=(clIntCP val);
    int     Sum();
    void    operator =(int val);
    bool    operator ==(clIntCP val);

    int _arr[CN_PLAYER];
};

double  ArrCnPlDist(clDoubleCP res0,clDoubleCP res);
//---------------------------------------------------------------------------
#endif
