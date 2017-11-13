//---------------------------------------------------------------------------

#ifndef WorkDirH
#define WorkDirH
#include <windows.h>
#include <vector>
#include "c:\home\Util\WorkStr.h"

typedef void(*LPSEARCHFUNC)(LPCTSTR lpszFileName);

bool SearchFiles(
	LPCTSTR lpszFileName,               // Директория и маска файлов для поиска
	LPSEARCHFUNC lpSearchFunc,          // Функция, вызываемая для всех найденных файлов.
	bool bInnerFolders);				// TRUE - поиск в подпапках. 
										// FALSE - поиск только в заданной папке
bool SearchFiles(LPCTSTR lpszFileName, std::vector <clAnsiString > &vect, bool bInnerFolders);
//---------------------------------------------------------------------------
#endif
