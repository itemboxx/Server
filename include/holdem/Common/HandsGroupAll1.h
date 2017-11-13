//---------------------------------------------------------------------------

#ifndef HandsGroupAllH
#define HandsGroupAllH
#include <io.h>
#include "C:\home\card\Hand.h"

struct tpFloatArrAllHands
{
	void    Clear()		{ ZeroMemory(_arr, sizeof(tpFloatArrAllHands)); }
	inline	void    WriteFile(int handle) { _write(handle, _arr, sizeof(_arr)); }
	inline	void    ReadFile(int handle) { _read(handle, _arr, sizeof(_arr)); }
	tpFloat	&operator [](int nb) { return _arr[nb]; }
	void	operator *=(tpFloat val) 
	{ 
		for (int i = 0; i < ALL_CN_HAND; i++)
			_arr[i] *= val;
	}
	tpFloat Sum()
	{
		tpFloat sum = 0;
		for (int i = 0; i < ALL_CN_HAND; i++)
			sum += _arr[i];
		return sum;
	}
	tpFloat Distance(tpFloatArrAllHands &arr);

	tpFloat _arr[ALL_CN_HAND];
};


class clGroupHandsAll : public tpFloatArrAllHands
{
public:
	int		CnHand();

	tpFloat	&operator [](int nb)				{ return _arr[nb]; }
	tpFloat	&operator [](clHand hand)			{ return _arr[hand.NbHandAll()]; }
	void    ReadFileFloat(int handle);

    void	SetFull();
	tpFloat    Norm(tpFloat sum=1);
	double 	Weight();
	void	SkipCard(tpCard nb);

//	tpFloat _weightH[ALL_CN_HAND];
};

#define CN_BAZE_HAND	20

class clBazeRiverGroupHands 
{
public:
	void    Clear() { ZeroMemory(_arr, sizeof(clBazeRiverGroupHands)); }
	inline	void    WriteFile(int handle) { _write(handle, _arr, sizeof(_arr)); }
	inline	void    ReadFile(int handle) { _read(handle, _arr, sizeof(_arr)); }
	tpFloat	&operator [](int nb) { return _arr[nb]; }
	tpFloat Sum()
	{
		tpFloat sum = 0;
		for (int i = 0; i < CN_BAZE_HAND; i++)
			sum += _arr[i];
		return sum;
	}
	tpFloat Distance(tpFloatArrAllHands &arr);
	tpFloat    Norm(tpFloat sum = 1);

	tpFloat _arr[CN_BAZE_HAND];
};

#include "c:\Home\Util\Matrix.h"

class clMatrixRiverEVEV : public clMatrix <tpFloat>
{
public:
	clMatrixRiverEVEV() { clMatrix <tpFloat>(CN_BAZE_HAND, CN_BAZE_HAND); }
};
//---------------------------------------------------------------------------
#endif
