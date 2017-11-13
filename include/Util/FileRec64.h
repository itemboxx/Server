//---------------------------------------------------------------------------

#ifndef FileRec64H
#define FileRec64H
//#define DEBUG_MOF_FC
//---------------------------------------------------------------------------
#include <io.h>
#include <vector>
//#    define NULL 0

class clFileConstRec64
{
public:
	clFileConstRec64();
	inline  ~clFileConstRec64()   { CloseFile(); }
	bool    Create(char *name, __int64 size);
    bool    OpenFile(char *name);
    int     CloseFile();
    void    ClearFile();
    inline  __int64     CnRecord()  {return _cn;}
	__int64 AddRecord(void *buf = 0);
	void    DelRecord(__int64 nb);
	int		ReadRecord(__int64 nb, void *buf);
	int     WriteRecord(__int64 nb, void *buf);
	int		LockRecord(__int64 nb, __int64 shift=0);   // Устанавливает позицию в файле на запись nb + shift, возвращает handle файла
//	void	UnLockRecord(int nb)				{ReleaseSemaphore(_lock, 1, NULL);}
	void    CopyFileA(clFileConstRec64 file);
    inline  bool    IsOpen()    {return (_handle != -1);}
    inline  int HandleF()       {return _handle;}
	inline  __int64 SizeRecord()    { return _size; }
	int		ReadBuf(void *buf, unsigned int size);
	int		WriteBuf(void *buf, unsigned int size);
	inline  unsigned __int64 FileLength()    { return _filelengthi64(_handle); }
//private :
	int _handle;
	__int64 _cn, _size;
	HANDLE  _lock;
};

//---------------------------------------------------------------------------
#endif
