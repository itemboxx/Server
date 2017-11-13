//---------------------------------------------------------------------------
#pragma hdrstop

#define _CRT_SECURE_NO_WARNINGS

#include <fcntl.h>
//#include <alloc.h>
#include <sys\stat.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <io.h>
#include <Windows.h>

#include "c:\home\Util\errMessage.h"
#include "c:\home\Util\FileRec64.h"

#define HEADER_KEY_CONSTREC      3328592
#define HEADER_SIZE_CONSTREC     (sizeof(int)+2*sizeof(__int64))
#define HEADER_KEY      3328593

//---------------------------------------------------------------------------
clFileConstRec64::clFileConstRec64()
{
    _handle=-1;
    _cn=0;
	_lock = CreateSemaphore(NULL, 1, 1, "");
}
//---------------------------------------------------------------------------
bool     clFileConstRec64::Create(char *name, __int64 size)
{
    if((_handle=_open(name,O_BINARY | O_RDWR | O_CREAT)) == -1)
        return false;
    {
        _chmod(name,S_IREAD | S_IWRITE);
        int p=HEADER_KEY_CONSTREC;
        _write(_handle,&p,sizeof(int));
        _cn=0;
		_write(_handle, &_cn, sizeof(_cn));
        _size=size;
		_write(_handle, &_size, sizeof(_size));
        _chsize(_handle,HEADER_SIZE_CONSTREC);
    }
//	_lock = false;
	return true;
}
//---------------------------------------------------------------------------
bool     clFileConstRec64::OpenFile(char *name)
{
    if(_handle != -1)
        _close(_handle);
    if((_handle=_open(name,O_BINARY | O_RDWR))==-1)
        return false;
	int val;
	_read(_handle, &val, sizeof(val));
    if(val != HEADER_KEY_CONSTREC)
    {
        _close(_handle);
        _handle=-1;
        return false;
    }
	_read(_handle, &_cn, sizeof(_cn));
	_read(_handle, &_size, sizeof(_size));
//	_lock = false;
	return true;
}
//---------------------------------------------------------------------------
int     clFileConstRec64::CloseFile()
{
    if(_handle != -1)
    {
        int res=_close(_handle);
        _handle=-1;
        _cn=0;
        return res;
    }
    return -1;
}
//---------------------------------------------------------------------------
void    clFileConstRec64::ClearFile()
{
    _lseeki64(_handle,0,SEEK_SET);
    _chsize(_handle,0);
    int p=HEADER_KEY_CONSTREC;
    _write(_handle,&p,sizeof(int));
    _cn=0;
	_write(_handle, &_cn, sizeof(_cn));
	_write(_handle, &_size, sizeof(_size));
//	_lock = false;
}
//---------------------------------------------------------------------------
__int64 clFileConstRec64::AddRecord(void *buf)
{
	DWORD val = WaitForSingleObject(_lock, INFINITE);
	if (val == WAIT_OBJECT_0)
	{
		if (buf != NULL)
		{
			_lseeki64(_handle, HEADER_SIZE_CONSTREC + _cn*_size, SEEK_SET);
			_write(_handle, buf, _size);
		}
		_cn++;
		_lseeki64(_handle, sizeof(int), SEEK_SET);
		_write(_handle, &_cn, sizeof(_cn));
		ReleaseSemaphore(_lock, 1, NULL);
		return _cn - 1;
	}
	return -1;
}
//---------------------------------------------------------------------------
int clFileConstRec64::ReadRecord(__int64  nb, void *buf)
{
	DWORD val = WaitForSingleObject(_lock, INFINITE);
	if (val == WAIT_OBJECT_0)
	{
		LockRecord(nb);
		int res = _read(_handle, buf, _size);
		ReleaseSemaphore(_lock, 1, NULL);
		return res;
	}
	return -1;
}
//---------------------------------------------------------------------------
int clFileConstRec64::WriteRecord(__int64 nb, void *buf)
{
	DWORD val = WaitForSingleObject(_lock, INFINITE);
	if (val == WAIT_OBJECT_0)
	{
		LockRecord(nb);
		int res = _write(_handle, buf, _size);
		ReleaseSemaphore(_lock, 1, NULL);
		return res;
	}
	return -1;
}
//---------------------------------------------------------------------------
int clFileConstRec64::LockRecord(__int64 nb, __int64 shift)
{ 
//	while (_lock)
//		Sleep(5);
//	_lock = true;
	_lseeki64(_handle, HEADER_SIZE_CONSTREC + nb*_size + shift, SEEK_SET);
	return _handle; 
}
//---------------------------------------------------------------------------
void    clFileConstRec64::CopyFileA(clFileConstRec64 file)
{
	__int64 size = (HEADER_SIZE_CONSTREC>file._size) ? HEADER_SIZE_CONSTREC : file._size;
    char *buf=(char *)malloc(size);
    _lseeki64(_handle,0,SEEK_SET);
	_lseeki64(file._handle, 0, SEEK_SET);
    _read(file._handle,buf,HEADER_SIZE_CONSTREC);
    _write(_handle,buf,HEADER_SIZE_CONSTREC);
    for(int i=0;i<file._cn;i++)
    {
        _read(file._handle,buf,file._size);
        _write(_handle,buf,file._size);
    }
    _cn=file._cn;
    _size=file._size;
    free(buf);
}
//---------------------------------------------------------------------------
void    clFileConstRec64::DelRecord(__int64 nb)
{
//	while (_lock)
//		Sleep(5);
//	_lock = true;
	char *buf = (char *)malloc(_size);
    for(int i=nb+1;i<_cn;i++)
    {
        ReadRecord(i,buf);
        WriteRecord(i-1,buf);
    }
    _cn--;
	_lseeki64(_handle, sizeof(int), SEEK_SET);
    _write(_handle,&_cn,sizeof(_cn));
    free(buf);
//	_lock = false;
}
//---------------------------------------------------------------------------
int     clFileConstRec64::ReadBuf(void *buf, unsigned int size)
{
    return _read(_handle,buf,size);
}
//---------------------------------------------------------------------------
int     clFileConstRec64::WriteBuf(void *buf, unsigned int size)
{
    return _write(_handle,buf,size);
}
//---------------------------------------------------------------------------
//***************************************************************************
//---------------------------------------------------------------------------
