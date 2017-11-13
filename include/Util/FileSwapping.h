//---------------------------------------------------------------------------

#ifndef SwappingFileH
#define SwappingFileH
//---------------------------------------------------------------------------
#include "TemplateFileRec.h"

using namespace std;

#define	SIZE_MB_DEF		1000

struct tpSwapUnit
{
	//tpSwapUnit() { _ptr = NULL; _cnLoc = _cn0 = 0; _save = false; }
	void *_ptr;
	int _cnLoc, _cn0;
	bool _save;
};

template<class T> class clFileSwap : public clFileRecT <T>
{
public:
	clFileSwap() { _maxSize = 0; _sem = CreateSemaphore(NULL, 1, 1, "");}
	~clFileSwap() { CloseFile(); }
	vector <tpSwapUnit> _vectU;
	vector <int> _vectNb;
	size_t _maxSize, _curSize, _curNb;
	HANDLE _sem;

	bool	FileIsInit() { return _maxSize > 0; }
	int     AddRecord(T *val, void *bufKey = NULL) { return AddRecord(1., val, bufKey); }
	//---------------------------------------------------------------------------
	void	Init(size_t sizeMem)
	{
		_maxSize = sizeMem * 1024 * 1024;
		//_maxSize = 1024 * 1024;
		_curSize = _curNb = 0;
		_vectU.resize(CnRecord());
		_vectNb.reserve(CnRecord());
		if(CnRecord()>0)
			ZeroMemory(&_vectU[0], CnRecord() * sizeof(tpSwapUnit));
	}
	//---------------------------------------------------------------------------
	bool    Create(char *name, int sizeGB = SIZE_MB_DEF, int sizeKey = 0, int cnRec = 100)
	{
		if (clFileRecT <T>::Create(name, sizeKey, cnRec))
		{
			Init(sizeGB);
			return true;
		}
		return false;
	}
	//---------------------------------------------------------------------------
	bool    OpenFileA(char *name, bool onlyRead, int sizeMB = SIZE_MB_DEF)
	{
		if (clFileRecT <T>::OpenFileA(name, onlyRead))
		{
			Init(sizeMB);
			return true;
		}
		return false;
	}
	//---------------------------------------------------------------------------
	int     CloseFile()
	{
		int dim = _vectNb.size();
#ifdef  ERR_MESSAGE
		int cn = 10;
		for (int i=0; i<dim; i++)
			if (_vectU[_vectNb[i]]._cnLoc > 0)
			{
				clAnsiString as = (clAnsiString)"nbRec=" + (clAnsiString)_vectNb[i] + (clAnsiString)"; cnLoc=" + (clAnsiString)_vectU[_vectNb[i]]._cnLoc;
				ErrMessage("Кто то не освободил запись", as);
				if (--cn == 0)
					break;
			}
#endif
		for (int i = 0; i < dim; i++)
		{
			tpSwapUnit *it = &_vectU[_vectNb[i]];
			if (it->_ptr != NULL)
			{
				if (it->_save)
					ReplaceRecord(1, _vectNb[i], (T*)it->_ptr);
				delete (T*)it->_ptr;
			}
		}
		_vectU.clear();
		_vectNb.clear();
		return clFileRecT <T>::CloseFile();
	}
	//---------------------------------------------------------------------------
	void	ClearSwap()
	{
		for (vector <tpSwapUnit>::iterator it = _vectU.begin(); it != _vectU.end(); it++)
			if (it->_ptr != NULL)
				delete (T*)it->_ptr;
		_vectU.clear();
		_vectNb.clear();
		_curSize = _curNb = 0;
	}
	//---------------------------------------------------------------------------
	void    ClearFile(int cnRec = 1000)
	{
		ClearSwap();
		clFileRecT <T>::ClearFile(cnRec);
	}
	//---------------------------------------------------------------------------
	int     AddRecord(double addition, T *val, void *bufKey = NULL)
	{
		int nb = clFileRecT <T>::AddRecord(addition, val, bufKey);
		if (_vectU.size() == _vectU.capacity())
		{
			if (_vectU.size() < 256)
				_vectU.reserve(256);
			else
				_vectU.reserve(2 * _vectU.size());
		}
		ErrCheck(nb, _vectU.size());
		tpSwapUnit unit = { NULL,0,0,false };
		_vectU.push_back(unit);
		return nb;
	}
	//---------------------------------------------------------------------------
	void    DelRecord(int nb)
	{
	}
	//---------------------------------------------------------------------------
	T *LockRecord(int nb)
	{
		DWORD valW = WaitForSingleObject(_sem, INFINITE);
		if (valW == WAIT_OBJECT_0)
		{
			if (_vectU[nb]._ptr == NULL)
				ReadRecord(nb);
			tpSwapUnit &un = _vectU[nb];
			un._cnLoc++;
			if (un._ptr == NULL)
				ErrMessage("Пишем по нулевому адресу", "");
			ReleaseSemaphore(_sem, 1, NULL);
			return (T*)un._ptr;
		}
		ErrMessage("Пишем по нулевому адресу", "");
		return NULL;
	}
	//---------------------------------------------------------------------------
	int  UnLockRecord(int nb, bool save = false)
	{
		DWORD valW = WaitForSingleObject(_sem, INFINITE);
		if (valW == WAIT_OBJECT_0)
		{
			tpSwapUnit &un = _vectU[nb];
			un._cnLoc--;
			un._cn0++;
			if (save) un._save = true;
			ReleaseSemaphore(_sem, 1, NULL);
			return un._cnLoc;
		}
		return -1;
	}
	//---------------------------------------------------------------------------
	int ReadRecord(int nb, T *buf)
	{
		T *rec = LockRecord(nb);
		*buf = *rec;
		UnLockRecord(nb);
		return 0;
	}
	//---------------------------------------------------------------------------
private:
	//void    ReplaceRecord(double addition, int nb, T *val, void *bufKey = NULL) {}
	//int     SeekRecord(int nbRec) {}
	int		ReadRecord(int nbRec)
	{
		//static int cnIn = 0, cnNew = 0, cnDel = 0;

		__int64	*ptr = PtrShiftSize(nbRec);
		size_t size = ptr[1], dim = _vectNb.size();
		int cn0 = 0;
		bool b0 = false;
		while (_curSize + size > _maxSize && _vectNb.size()>0)
		{
			if (_curNb >= dim) _curNb = 0;
			int curRec = _vectNb[_curNb];
			tpSwapUnit &un = _vectU[curRec];
			if (un._cnLoc == 0)
				if (un._cn0 == 1)
				{
					ptr = PtrShiftSize(curRec);
					_curSize -= ptr[1];
					if (un._save)
						clFileRecT <T>::ReplaceRecord(1, curRec, (T*)un._ptr);
					delete (T*)un._ptr; 
					un._ptr=NULL;
					_vectNb.erase(_vectNb.begin() + _curNb);
					cn0 = 0;
					b0 = true;
					dim--;
					_curNb--;
				}
				else
				{
					un._cn0--;
					cn0 = 0;
				}
			_curNb++;
			cn0++;
			if (cn0 == dim+1) break;
		}
		tpSwapUnit &un = _vectU[nbRec];
		un._ptr = new T;
		_vectNb.push_back(nbRec);
		clFileRecT <T>::ReadRecord(nbRec, (T*)un._ptr);
		_curSize += size;
		return (int)dim;
	}
};
//---------------------------------------------------------------------------

#endif
