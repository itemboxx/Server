//---------------------------------------------------------------------------

#ifndef WorkDirH
#define WorkDirH
#include <windows.h>
#include <vector>
#include "c:\home\Util\WorkStr.h"

typedef void(*LPSEARCHFUNC)(LPCTSTR lpszFileName);

bool SearchFiles(
	LPCTSTR lpszFileName,               // ���������� � ����� ������ ��� ������
	LPSEARCHFUNC lpSearchFunc,          // �������, ���������� ��� ���� ��������� ������.
	bool bInnerFolders);				// TRUE - ����� � ���������. 
										// FALSE - ����� ������ � �������� �����
bool SearchFiles(LPCTSTR lpszFileName, std::vector <clAnsiString > &vect, bool bInnerFolders);
//---------------------------------------------------------------------------
#endif
