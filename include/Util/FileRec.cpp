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
#include "c:\home\Util\FileRec.h"

#define HEADER_KEY_CONSTREC      3328592
#define HEADER_SIZE_CONSTREC     (3*sizeof(ulong))
#define HEADER_KEY      3328593

//---------------------------------------------------------------------------
clFileConstRec::clFileConstRec()
{
    _handle=-1;
    _cn=0;
//	_lock = false;
}
//---------------------------------------------------------------------------
bool     clFileConstRec::Create(char *name, ulong size)
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
bool     clFileConstRec::OpenFile(char *name)
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
int     clFileConstRec::CloseFile()
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
void    clFileConstRec::ClearFile()
{
    _lseek(_handle,0,SEEK_SET);
    _chsize(_handle,0);
    int p=HEADER_KEY_CONSTREC;
    _write(_handle,&p,sizeof(int));
    _cn=0;
	_write(_handle, &_cn, sizeof(_cn));
	_write(_handle, &_size, sizeof(_size));
//	_lock = false;
}
//---------------------------------------------------------------------------
ulong clFileConstRec::AddRecord(void *buf)
{
//	while (_lock)
//		Sleep(5);
//	_lock = true;
	if(buf != NULL)
    {
		_lseek(_handle, HEADER_SIZE_CONSTREC + _cn*_size, SEEK_SET);
        _write(_handle,buf,_size);
    }
    _cn++;
	_lseek(_handle, sizeof(ulong), SEEK_SET);
	_write(_handle, &_cn, sizeof(_cn));
//	_lock = false;
	return _cn - 1;
}
//---------------------------------------------------------------------------
int clFileConstRec::ReadRecord(ulong  nb, void *buf)
{
	LockRecord(nb);
    int res=_read(_handle,buf,_size);
//	UnLockRecord(nb);
	return res;
}
//---------------------------------------------------------------------------
int clFileConstRec::WriteRecord(ulong nb, void *buf)
{
	LockRecord(nb);
	int res = _write(_handle, buf, _size);
//	UnLockRecord(nb);
	return res;
}
//---------------------------------------------------------------------------
int clFileConstRec::LockRecord(ulong nb, ulong shift)
{ 
//	while (_lock)
//		Sleep(5);
//	_lock = true;
	_lseek(_handle, HEADER_SIZE_CONSTREC + nb*_size + shift, SEEK_SET);
	return _handle; 
}
//---------------------------------------------------------------------------
void    clFileConstRec::CopyFileA(clFileConstRec file)
{
	ulong size = (HEADER_SIZE_CONSTREC>file._size) ? HEADER_SIZE_CONSTREC : file._size;
    char *buf=(char *)malloc(size);
    _lseek(_handle,0,SEEK_SET);
	_lseek(file._handle, 0, SEEK_SET);
    _read(file._handle,buf,HEADER_SIZE_CONSTREC);
    _write(_handle,buf,HEADER_SIZE_CONSTREC);
	for (ulong i = 0; i<file._cn; i++)
    {
        _read(file._handle,buf,file._size);
        _write(_handle,buf,file._size);
    }
    _cn=file._cn;
    _size=file._size;
    free(buf);
}
//---------------------------------------------------------------------------
void    clFileConstRec::DelRecord(ulong nb)
{
//	while (_lock)
//		Sleep(5);
//	_lock = true;
	char *buf = (char *)malloc(_size);
	for (ulong i = nb + 1; i<_cn; i++)
    {
        ReadRecord(i,buf);
        WriteRecord(i-1,buf);
    }
    _cn--;
	_lseek(_handle, sizeof(int), SEEK_SET);
    _write(_handle,&_cn,sizeof(_cn));
    free(buf);
//	_lock = false;
}
//---------------------------------------------------------------------------
int     clFileConstRec::ReadBuf(void *buf, ulong size)
{
    return _read(_handle,buf,size);
}
//---------------------------------------------------------------------------
int     clFileConstRec::WriteBuf(void *buf, ulong size)
{
    return _write(_handle,buf,size);
}
//---------------------------------------------------------------------------
//***************************************************************************
//---------------------------------------------------------------------------
