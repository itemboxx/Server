//---------------------------------------------------------------------------

#ifndef MATRIX_H
#define MATRIX_H
//---------------------------------------------------------------------------
//#include "c:\Home\Util\Matrix.h"
#include "errMessage.h"

template <class T> class clMatrix
{
public:
	clMatrix() { _arr = NULL; _dimX = _dimY = 0; }
	~clMatrix() { if (_arr != NULL) free(_arr); }
	clMatrix(int x, int y) { _dimX = x;	_dimY = y;	_arr = (T *)malloc(SizeByte()); }
	clMatrix(clMatrix <T> &m) { _arr = NULL; *this = m; }
	void	SetDim(int x, int y) 
	{ 
		_dimX = x; 
		_dimY = y; 
		_arr = (T *)realloc(_arr, SizeByte()); 
	}
	int Size() { return _dimX*_dimY; }
	int SizeByte() { return _dimX*_dimY * sizeof(T); }
	void	ZeroData() { ZeroMemory(_arr, SizeByte()); }
	//---------------------------------------------------------------------------
	void	operator = (clMatrix <T> &m)
	{
		if (m._arr == NULL)
		{
			if (_arr != NULL)
				free(_arr);
			_arr = NULL;
			_dimX = _dimY = 0;
		}
		else
		{
			SetDim(m._dimX, m._dimY);
			memcpy(_arr, m._arr, m.SizeByte());
		}
	}
	//---------------------------------------------------------------------------
	T		*operator [](int nb) { return _arr + (nb*_dimY); }
	T		&Element(int x, int y) { return _arr[x*_dimY + y]; }
	size_t	SizeInFile() { return 2 * sizeof(int) + SizeByte(); }
	//---------------------------------------------------------------------------
	int 	WriteFile(int handle)
	{
		_write(handle, &_dimX, sizeof(_dimX));
		_write(handle, &_dimY, sizeof(_dimY));
		if (_dimX*_dimY>0)
			_write(handle, _arr, SizeByte());
		return 0;
	}
	//---------------------------------------------------------------------------
	int 	ReadFile(int handle)
	{
		_read(handle, &_dimX, sizeof(_dimX));
		_read(handle, &_dimY, sizeof(_dimY));
		if (_dimX*_dimY > 0)
		{
			_arr = (T *)realloc(_arr, SizeByte());
			_read(handle, _arr, SizeByte());
		}
		return 0;
	}
	//---------------------------------------------------------------------------

	T *_arr;
	int _dimX, _dimY;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

class clFloatMatrix2 : public clMatrix <float>
{
public:
	clFloatMatrix2() { clMatrix <float>::clMatrix(); }
	clFloatMatrix2(int x, int y) { clMatrix <float>::clMatrix(x, y); }
	void	NormData();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

template <class T> class clMatrixCub
{
public:
	clMatrixCub() { _arr = NULL; _dimX = _dimY = _dimZ = 0; }
	~clMatrixCub() { if (_arr != NULL) free(_arr); }
	clMatrixCub(int x, int y, int z = 1) { _dimX = x; _dimY = y; _dimZ = z; _arr = (T *)malloc(SizeByte()); }
	clMatrixCub(clMatrixCub &m) { _arr = NULL; *this = m; }
	void	SetDim(int x, int y, int z = 1) { _dimX = x; _dimY = y; _dimZ = z; _arr = (T *)realloc(_arr, SizeByte()); }
	int Size() { return _dimX*_dimY*_dimZ; }
	int SizeByte() { return _dimX*_dimY *_dimZ * sizeof(T); }
	void	ZeroData() { ZeroMemory(_arr, SizeByte()); }

	//---------------------------------------------------------------------------
	void	operator = (clMatrixCub <T> &m)
	{
		if (m._arr == NULL)
		{
			if (_arr != NULL)
				free(_arr);
			_arr = NULL;
			_dimX = _dimY = _dimZ = 0;
		}
		else
		{
			SetDim(m._dimX, m._dimY, m._dimZ);
			memcpy(_arr, m._arr, m.SizeByte());
		}
	}
	//---------------------------------------------------------------------------
	T		&Element(int x, int y, int z) { return _arr[x*_dimZ*_dimY + y*_dimZ + z]; }
	size_t	SizeInFile() { return 3 * sizeof(int) + SizeByte(); }
	T		*Array(int x, int y) { return &_arr[x*_dimZ*_dimY + y*_dimZ]; }
	//---------------------------------------------------------------------------
	int 	WriteFile(int handle)
	{
		_write(handle, &_dimX, sizeof(_dimX));
		_write(handle, &_dimY, sizeof(_dimY));
		_write(handle, &_dimZ, sizeof(_dimZ));
		if (_dimX*_dimY*_dimZ > 0)
			_write(handle, _arr, SizeByte());
		return 0;
	}
	//---------------------------------------------------------------------------
	int 	ReadFile(int handle)
	{
		_read(handle, &_dimX, sizeof(_dimX));
		_read(handle, &_dimY, sizeof(_dimY));
		_read(handle, &_dimZ, sizeof(_dimZ));
		if (_dimX*_dimY*_dimZ > 0)
		{
			_arr = (T *)realloc(_arr, SizeByte());
			_read(handle, _arr, SizeByte());
		}
		return 0;
	}
	//---------------------------------------------------------------------------

	T *_arr;
	int _dimX, _dimY, _dimZ;
};

//---------------------------------------------------------------------------
#endif
