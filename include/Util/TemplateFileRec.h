//---------------------------------------------------------------------------

#ifndef TemplateFileRecH
#define TemplateFileRecH
//---------------------------------------------------------------------------
#include "c:\Home\Util\errMessage.h"
#include <fcntl.h>
//#include <alloc.h>
#include <sys\stat.h>
#include <errno.h>
#include <io.h>
#include <share.h>
#include <vector>

using namespace std;

size_t WriteBigBlock(int handle, void *ptr, size_t sizeA);
size_t ReadBigBlock(int handle, void *ptr, size_t sizeA);
template <typename T> size_t  WriteVectFile(int handle, vector <T> &vect)
{
	unsigned dim = (unsigned)vect.size();
	_write(handle, &dim, sizeof(unsigned));
	return ((dim>0))? WriteBigBlock(handle, &vect[0], dim*sizeof(T)) : 0;
}
//---------------------------------------------------------------------------
template <typename T> size_t  ReadVectFile(int handle, vector <T> &vect)
{
	unsigned dim;
	_read(handle, &dim, sizeof(unsigned));
	vect.resize(dim);
	return ((dim>0)) ? ReadBigBlock(handle, &vect[0], dim*sizeof(T)) : 0;
}
//---------------------------------------------------------------------------
template <typename T> size_t  SizeVectFile(vector <T> &vect)
{
	return sizeof(unsigned) + vect.size()*sizeof(T);
}
//---------------------------------------------------------------------------

#define HEADER_KEY_T      4125826

struct tpHeaderFileT
{
    int _key,_cnRecord,_dim,_sizeKey,_oldDim;
};

#define HEADER_SIZE_T     (sizeof(tpHeaderFileT))

template<class T> class clFileRecT
{
public:
	clFileRecT() { _handle = -1; _arrKey = NULL; _lock = CreateSemaphore(NULL, 1, 1, ""); _onlyRead = false; }
    ~clFileRecT()   {CloseFile();}
    int     AddRecord(T *val,void *bufKey=NULL)      {return AddRecord(1.,val,bufKey);}
    inline  bool    IsOpen()    {return (_handle != -1);}
    int CnRecord()              {return _header._cnRecord;}
    void    *PtrKey(int nb)         {return (void *)(PtrShiftSize(nb)+2);}  // ”казатель на ключ записи є nb
//---------------------------------------------------------------------------
bool    Create(char *name,int sizeKey=0,int cnRec=100)
{
#ifdef  ERR_MESSAGE
    errno=0;
#endif
    CloseFile();
	_onlyRead = false;
	_sopen_s(&_handle, name, O_BINARY | O_RDWR | O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
//    if((_handle=_open(name,O_BINARY | O_RDWR | O_CREAT)) == -1)
	if (_handle==-1)
        return false;

    ErrCheck("clFileRec::Create",errno,0);

    _chmod(name,S_IREAD | S_IWRITE);
    _header._key=HEADER_KEY_T;
    _header._cnRecord=0;
    _header._dim=_header._oldDim=cnRec;
    _header._sizeKey=sizeKey;
    _write(_handle,&_header,sizeof(_header));
	unsigned dimmm = AllSizeKey();
    _arrKey=(BYTE*)malloc(dimmm);
    _chsize(_handle,HEADER_SIZE_T+AllSizeKey());
    return true;
}
//---------------------------------------------------------------------------
bool    OpenFileA(char *name, bool onlyRead=false)
{
#ifdef  ERR_MESSAGE
    errno=0;
#endif
    CloseFile();
	if(_onlyRead)
		_sopen_s(&_handle, name, O_BINARY | O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	else
		_sopen_s(&_handle, name, O_BINARY | O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	//	if ((_handle = _open(name, O_BINARY | O_RDWR)) == -1)
	if (_handle==-1)
        return false;
    _read(_handle,&_header,sizeof(_header));
    if(_header._key != HEADER_KEY_T)
    {
        _close(_handle);
        _handle=-1;
        return false;
    }
	_onlyRead = onlyRead;
    int size=AllSizeKey();
    _arrKey=(BYTE*)malloc(size);
    _read(_handle,_arrKey,size);
ErrCheck("clFileRec::OpenFile",errno,0);
    return true;
}
//---------------------------------------------------------------------------
int     CloseFile()
{
#ifdef  ERR_MESSAGE
    errno=0;
#endif
    if(_handle != -1)
    {
		if(!_onlyRead)
			SaveFile();
        int res=_close(_handle);
        _handle=-1;
        free(_arrKey);
        _arrKey=NULL;
    ErrCheck("clFileRec::CloseFile",errno,0);
        return res;
    }
    return -1;
}
//---------------------------------------------------------------------------
void    ClearFile(int cnRec=1000)
{
	if (_onlyRead)
	{
		ErrMessage("‘айл открыт только по чтению", " ClearFile");
		return;
	}
    _header._cnRecord=0;
    _header._dim=_header._oldDim= cnRec;
    _lseeki64(_handle,0,SEEK_SET);
    _write(_handle,&_header,sizeof(_header));
	unsigned dimmm = AllSizeKey();
	_arrKey = (BYTE*)realloc(_arrKey, dimmm);
	_chsize_s(_handle, sizeof(_header) + AllSizeKey());
}
//---------------------------------------------------------------------------
int     AddRecord(double addition,T *val,void *bufKey=NULL)
{
	if (_onlyRead)
	{
		ErrMessage("‘айл открыт только по чтению", " AddRecord");
		return -1;
	}
#ifdef  ERR_MESSAGE
    errno=0;
#endif
	DWORD valW = WaitForSingleObject(_lock, INFINITE);
	if (valW == WAIT_OBJECT_0)
	{
		if (_header._cnRecord == _header._dim)
		{
			_header._dim *= 2;
			unsigned dimmm = AllSizeKey();
			_arrKey = (BYTE*)realloc(_arrKey, dimmm);
		}
		__int64 shift = _lseeki64(_handle, 0, SEEK_END);
		val->WriteFile(_handle);
		__int64 size = (__int64)((_telli64(_handle) - shift)*addition);
		_chsize_s(_handle, shift + size);
		WriteShifrRecord(_header._cnRecord, shift, size);
		if (bufKey != NULL)
			memcpy(PtrKey(_header._cnRecord), bufKey, _header._sizeKey);
		_header._cnRecord++;

		ErrCheck("clFileRec::AddRecord", errno, 0);
		int r = _header._cnRecord - 1;
		ReleaseSemaphore(_lock, 1, NULL);

		return r;
	}
	return -1;
}
//---------------------------------------------------------------------------
void    DelRecord(int nb)
{
	if (_onlyRead)
	{
		ErrMessage("‘айл открыт только по чтению", " DelRecord");
		return;
	}
	_header._cnRecord--;
    memcpy(PtrShiftSize(nb),PtrShiftSize(nb+1),(_header._cnRecord-nb)*SizeKey());
}
//---------------------------------------------------------------------------
int LockRecord(int nb)
{
	if (nb<0 || nb >= _header._cnRecord) return -1;
	DWORD valW = WaitForSingleObject(_lock, INFINITE);
	if (valW == WAIT_OBJECT_0)
	{
		__int64 shift = ShiftRecord(nb);
		_lseeki64(_handle, shift, SEEK_SET);

		return(_handle);
	}
}
//---------------------------------------------------------------------------
void UnLockRecord(int nb)
{
	ReleaseSemaphore(_lock, 1, NULL);
}
//---------------------------------------------------------------------------
int ReadRecord(int nb, T *buf)
{
#ifdef  ERR_MESSAGE
    errno=0;
#endif
    if(nb<0 || nb>=_header._cnRecord) return -1;
	DWORD valW = WaitForSingleObject(_lock, INFINITE);
	if (valW == WAIT_OBJECT_0)
	{
		__int64 shift = ShiftRecord(nb);
		_lseeki64(_handle, shift, SEEK_SET);

		int r = buf->ReadFile(_handle);
		ErrCheck("clFileRec::ReadRecord", errno, 0);
		ReleaseSemaphore(_lock, 1, NULL);
		return(r);
	}
	return -1;
}
//---------------------------------------------------------------------------
void    ReplaceRecord(double addition, int nb, T *val, void *bufKey = NULL)
{
	if (_onlyRead)
	{
		ErrMessage("‘айл открыт только по чтению", " ReplaceRecord");
		return;
	}
#ifdef  ERR_MESSAGE
    errno=0;
#endif
	DWORD valW = WaitForSingleObject(_lock, INFINITE);
	if (valW == WAIT_OBJECT_0)
	{
		__int64 *ptr = PtrShiftSize(nb);
		__int64 size = val->SizeInFile();
		if (size > ptr[1])
		{
			__int64 shift = _lseeki64(_handle, 0, SEEK_END);
			val->WriteFile(_handle);

			size = (__int64)(size*addition);
			_chsize_s(_handle, shift + size);
			ptr[0] = shift;
			ptr[1] = size;
		}
		else
		{
			_lseeki64(_handle, ptr[0], SEEK_SET);
			val->WriteFile(_handle);
		}
		if (bufKey != NULL)
			memcpy(PtrKey(nb), bufKey, _header._sizeKey);

		ErrCheck("clFileRec::ReplaceRecord", errno, 0);
		ReleaseSemaphore(_lock, 1, NULL);

	}
}
//---------------------------------------------------------------------------
int     SeekRecord(int nbRec)
{
	_lseeki64(_handle, ShiftRecord(nbRec), SEEK_SET);
	return _handle;
}
//---------------------------------------------------------------------------
void    SaveFile()
{
	if (_onlyRead)
	{
		ErrMessage("‘айл открыт только по чтению", " SaveFile");
		return;
	}
	if (_header._dim>_header._oldDim)
		ReStraight();
	_lseeki64(_handle, 0, SEEK_SET);
	_write(_handle, &_header, sizeof(_header));
	unsigned dimmm = AllSizeKey();
	_write(_handle, _arrKey, dimmm);
}
//---------------------------------------------------------------------------
protected:
    inline  int AllSizeKey()    {return _header._dim*SizeKey();}
	inline  int SizeKey()       { return 2 * sizeof(__int64) + _header._sizeKey; }
	__int64		*PtrShiftSize(int nb)   { return (__int64 *)(_arrKey + nb*SizeKey()); }  // ”казатель на смещение,размер и ключ записи є nb
	__int64		ShiftRecord(int nb) { return *PtrShiftSize(nb); }

void    WriteShifrRecord(int nb, __int64 shift, __int64 size)
{
    __int64 *ptr=PtrShiftSize(nb);
    ptr[0]=shift;
    ptr[1]=size;
}
//---------------------------------------------------------------------------
void    ReStraight()
{
    __int64 sizeH=HEADER_SIZE_T+AllSizeKey();
    for(int i=0;i<_header._cnRecord;i++)
        if(ShiftRecord(i)<sizeH)
        {
            __int64 shift,size,*ptr=PtrShiftSize(i);
            shift=ptr[0];
            size=ptr[1];
			if (size >= 0x100000000i64)
				ErrMessage("Ѕлок не влазит в long", "ReStraight");
			BYTE *buf = (BYTE *)malloc(size);
            _lseeki64(_handle,shift,SEEK_SET);
            _read(_handle,buf,(unsigned int)size);
            shift=_lseeki64(_handle,0,SEEK_END);
			_write(_handle, buf, (unsigned int)size);
            size=_telli64(_handle)-shift;
            WriteShifrRecord(i,shift,size);
            free(buf);
        }
    _header._oldDim=_header._dim;
}
//---------------------------------------------------------------------------

    int _handle;
    tpHeaderFileT _header;
    BYTE *_arrKey;
	HANDLE _lock;
	bool _onlyRead;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------




 
typedef unsigned long ulong;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

template<class T> class clFileRecT32
{
public:
	clFileRecT32()    { _handle = -1; _arrKey = NULL; }
	~clFileRecT32()   { CloseFile(); }
	int     AddRecord(T *val, void *bufKey = NULL)      { return AddRecord(1., val, bufKey); }
	inline  bool    IsOpen()    { return (_handle != -1); }
	int CnRecord()              { return _header._cnRecord; }
	void    *PtrKey(int nb)         { return (void *)(PtrShiftSize(nb) + 2); }  // ”казатель на ключ записи є nb
	//---------------------------------------------------------------------------
	bool    Create(char *name, int cnRec = 100)
	{
#ifdef  ERR_MESSAGE
		errno = 0;
#endif
		CloseFile();
		_sopen_s(&_handle, name, O_BINARY | O_RDWR | O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
		//    if((_handle=_open(name,O_BINARY | O_RDWR | O_CREAT)) == -1)
		if (_handle == -1)
			return false;

		ErrCheck("clFileRec::Create", errno, 0);

		_chmod(name, S_IREAD | S_IWRITE);
		_header._key = HEADER_KEY_T;
		_header._cnRecord = 0;
		_header._dim = _header._oldDim = cnRec;
		_header._sizeKey = T::_sizeKey;
		_write(_handle, &_header, sizeof(_header));

		unsigned dimmm = AllSizeKey();
		_arrKey = (BYTE*)malloc(dimmm);
		_chsize(_handle, HEADER_SIZE_T + AllSizeKey());
		return true;
	}
	//---------------------------------------------------------------------------
	bool    OpenFile(char *name)
	{
#ifdef  ERR_MESSAGE
		errno = 0;
#endif
		CloseFile();
		_sopen_s(&_handle, name, O_BINARY | O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
		//	if ((_handle = _open(name, O_BINARY | O_RDWR)) == -1)
		if (_handle == -1)
			return false;
		_read(_handle, &_header, sizeof(_header));
		if (_header._key != HEADER_KEY_T)
		{
			_close(_handle);
			_handle = -1;
			return false;
		}
		int size = AllSizeKey();
		_arrKey = (BYTE*)malloc(size);
		_read(_handle, _arrKey, size);
		ErrCheck("clFileRec::OpenFile", errno, 0);
		return true;
	}
	//---------------------------------------------------------------------------
	int     CloseFile()
	{
#ifdef  ERR_MESSAGE
		errno = 0;
#endif
		if (_handle != -1)
		{
			SaveFile();
			int res = _close(_handle);
			_handle = -1;
			free(_arrKey);
			_arrKey = NULL;
			ErrCheck("clFileRec::CloseFile", errno, 0);
			return res;
		}
		return -1;
	}
	//---------------------------------------------------------------------------
	void    ClearFile()
	{
		_header._cnRecord = 0;
		_header._dim = _header._oldDim = 1000;
		_lseeki64(_handle, 0, SEEK_SET);
		_write(_handle, &_header, sizeof(_header));
		unsigned dimmm = AllSizeKey();
		_arrKey = (BYTE*)realloc(_arrKey, dimmm);
		_chsize_s(_handle, sizeof(_header) + AllSizeKey());
	}
	//---------------------------------------------------------------------------
	int     AddRecord(double addition, T *val, void *bufKey = NULL)
	{
#ifdef  ERR_MESSAGE
		errno = 0;
#endif
		if (_header._cnRecord == _header._dim)
		{
			_header._dim *= 2;
			unsigned dimmm = AllSizeKey();
			_arrKey = (BYTE*)realloc(_arrKey, dimmm);
		}
		int shift = _lseek(_handle, 0, SEEK_END);
		val->WriteFile(_handle);
		int size = (int)((_tell(_handle) - shift)*addition);
		_chsize_s(_handle, shift + size);
		WriteShifrRecord(_header._cnRecord, shift, size);
		if (bufKey != NULL)
			memcpy(PtrKey(_header._cnRecord), bufKey, _header._sizeKey);
		_header._cnRecord++;

		ErrCheck("clFileRec::AddRecord", errno, 0);

		return _header._cnRecord - 1;
	}
	//---------------------------------------------------------------------------
	void    DelRecord(int nb)
	{
		_header._cnRecord--;
		memcpy(PtrShiftSize(nb), PtrShiftSize(nb + 1), (_header._cnRecord - nb)*SizeKey());
	}
	//---------------------------------------------------------------------------
	int ReadRecord(int nb, T *buf)
	{
#ifdef  ERR_MESSAGE
		errno = 0;
#endif
		if (nb<0 || nb >= _header._cnRecord) return -1;
		//    unsigned long *ptr=PtrShiftSize(nb);
		int shift = ShiftRecord(nb);
		_lseeki64(_handle, shift, SEEK_SET);

		ErrCheck("clFileRec::ReadRecord", errno, 0);

		return(buf->ReadFile(_handle));
	}
	//---------------------------------------------------------------------------
	void    ReplaceRecord(double addition, int nb, T *val, void *bufKey = NULL)
	{
#ifdef  ERR_MESSAGE
		errno = 0;
#endif
		int *ptr = PtrShiftSize(nb);
		int size = val->SizeInFile();
		if (size>ptr[1])
		{
			int shift = _lseek(_handle, 0, SEEK_END);
			val->WriteFile(_handle);

			size = (int)(size*addition);
			_chsize_s(_handle, shift + size);
			ptr[0] = shift;
			ptr[1] = size;
		}
		else
		{
			_lseek(_handle, ptr[0], SEEK_SET);
			val->WriteFile(_handle);
		}
		if (bufKey != NULL)
			memcpy(PtrKey(nb), bufKey, _header._sizeKey);

		ErrCheck("clFileRec::ReplaceRecord", errno, 0);
	}
	//---------------------------------------------------------------------------
	int     SeekRecord(int nbRec)
	{
		_lseek(_handle, ShiftRecord(nbRec), SEEK_SET);
		return _handle;
	}
	//---------------------------------------------------------------------------
private:
	inline  int AllSizeKey()    { return _header._dim*SizeKey(); }
	inline  int SizeKey()       { return 2 * sizeof(int) + _header._sizeKey; }
	ulong		*PtrShiftSize(int nb)   { return (ulong *)(_arrKey + nb*SizeKey()); }  // ”казатель на смещение,размер и ключ записи є nb
	ulong		ShiftRecord(int nb) { return *PtrShiftSize(nb); }

	void    SaveFile()
	{
		if (_header._dim>_header._oldDim)
			ReStraight();
		_lseek(_handle, 0, SEEK_SET);
		_write(_handle, &_header, sizeof(_header));
		unsigned dimmm = AllSizeKey();
		_write(_handle, _arrKey, dimmm);
	}
	//---------------------------------------------------------------------------
	void    WriteShifrRecord(int nb, ulong shift, ulong size)
	{
		ulong *ptr = PtrShiftSize(nb);
		ptr[0] = shift;
		ptr[1] = size;
	}
	//---------------------------------------------------------------------------
	void    ReStraight()
	{
		ulong sizeH = HEADER_SIZE_T + AllSizeKey();
		for (int i = 0; i<_header._cnRecord; i++)
			if (ShiftRecord(i)<sizeH)
			{
				ulong shift, size, *ptr = PtrShiftSize(i);
				shift = ptr[0];
				size = ptr[1];
				BYTE *buf = (BYTE *)malloc(size);
				_lseeki64(_handle, shift, SEEK_SET);
				_read(_handle, buf, (unsigned int)size);
				shift = (ulong)_lseeki64(_handle, 0, SEEK_END);
				_write(_handle, buf, (unsigned int)size);
				size = (ulong)_telli64(_handle) - shift;
				WriteShifrRecord(i, shift, size);
				free(buf);
			}
		_header._oldDim = _header._dim;
	}
	//---------------------------------------------------------------------------

	int _handle;
	tpHeaderFileT _header;
	BYTE *_arrKey;
};
//---------------------------------------------------------------------------
#endif
