//---------------------------------------------------------------------------

#ifndef FileRecH
#define FileRecH
//#define DEBUG_MOF_FC
//---------------------------------------------------------------------------
#include <io.h>
#include <vector>
//#    define NULL 0

typedef unsigned long ulong;

class clFileConstRec
{
public:
    clFileConstRec();
    inline  ~clFileConstRec()   {CloseFile();}
	bool    Create(char *name, ulong size);
    bool    OpenFile(char *name);
    int     CloseFile();
    void    ClearFile();
    inline  int     CnRecord()  {return _cn;}
	ulong	AddRecord(void *buf = 0);
	void    DelRecord(ulong nb);
	int		ReadRecord(ulong nb, void *buf);
	int     WriteRecord(ulong nb, void *buf);
	int		LockRecord(ulong nb, ulong shift = 0);   // Устанавливает позицию в файле на запись nb + shift, возвращает handle файла
//	void	UnLockRecord(int nb)				{ _lock = false; }
    void    CopyFileA(clFileConstRec file);
    inline  bool    IsOpen()    {return (_handle != -1);}
    inline  int HandleF()       {return _handle;}
	inline  ulong SizeRecord()    { return _size; }
	int		ReadBuf(void *buf, ulong size);
	int		WriteBuf(void *buf, ulong size);
	inline  ulong FileLength()    { return _filelength(_handle); }
//private :
	int _handle;
	ulong _cn, _size;
//	bool _lock;
};

//---------------------------------------------------------------------------
#endif
