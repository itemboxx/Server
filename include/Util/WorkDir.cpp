//---------------------------------------------------------------------------
#pragma hdrstop

#include "C:\Home\Util\WorkDir.h"

//---------------------------------------------------------------------------
bool SearchFiles(LPCTSTR lpszFileName, LPSEARCHFUNC lpSearchFunc, bool bInnerFolders)
{
	LPTSTR part;
	char tmp[MAX_PATH]; // временный массив
	char name[MAX_PATH];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

	// сначало поиск внутри вложенных папках ...
	if (bInnerFolders)
	{
		if (GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) return FALSE;
		strcpy(name, part);
		strcpy(part, "*.*");

		// если папки существуют, то делаем поиск
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if (!((hSearch = FindFirstFile(tmp, &wfd)) == INVALID_HANDLE_VALUE))
			do
			{
				// в каждой папке есть две папки с именами "." и ".."
				// и эти папки мы не трогаем

				// пропускаем папки "." и ".."
				if (!strncmp(wfd.cFileName, ".", 1) || !strncmp(wfd.cFileName, "..", 2))
					continue;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // если мы нашли папку
				{
					char next[MAX_PATH];
					if (GetFullPathName(lpszFileName, MAX_PATH, next, &part) == 0) return FALSE;
					strcpy(part, wfd.cFileName);
					strcat(next, "\\");
					strcat(next, name);

					SearchFiles(next, lpSearchFunc, TRUE);
				}
			} while (FindNextFile(hSearch, &wfd)); // ищем следующий файл

			FindClose(hSearch); // заканчиваем поиск
	}

	if ((hSearch = FindFirstFile(lpszFileName, &wfd)) == INVALID_HANDLE_VALUE)
		return TRUE; // в противном случае выходим
	do
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // если мы нашли файл
		{
			char file[MAX_PATH];
			if (GetFullPathName(lpszFileName, MAX_PATH, file, &part) == 0) return FALSE;
			strcpy(part, wfd.cFileName);

			lpSearchFunc(file);
		}
	while (FindNextFile(hSearch, &wfd)); // ищем следующий файл
	FindClose(hSearch); // заканчиваем поиск

	return TRUE;
}
//---------------------------------------------------------------------------
bool  SearchFiles(LPCTSTR lpszFileName, std::vector <clAnsiString > &vect, bool bInnerFolders)
{
	LPTSTR part;
	char tmp[MAX_PATH]; // временный массив
	char name[MAX_PATH];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

	// сначало поиск внутри вложенных папках ...
	if (bInnerFolders)
	{
		if (GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) return FALSE;
		strcpy(name, part);
		strcpy(part, "*.*");

		// если папки существуют, то делаем поиск
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if (!((hSearch = FindFirstFile(tmp, &wfd)) == INVALID_HANDLE_VALUE))
			do
			{
				// в каждой папке есть две папки с именами "." и ".."
				// и эти папки мы не трогаем

				// пропускаем папки "." и ".."
				if (!strncmp(wfd.cFileName, ".", 1) || !strncmp(wfd.cFileName, "..", 2))
					continue;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // если мы нашли папку
				{
					char next[MAX_PATH];
					if (GetFullPathName(lpszFileName, MAX_PATH, next, &part) == 0) return FALSE;
					strcpy(part, wfd.cFileName);
					strcat(next, "\\");
					strcat(next, name);

					SearchFiles(next, vect, TRUE);
				}
			} while (FindNextFile(hSearch, &wfd)); // ищем следующий файл

			FindClose(hSearch); // заканчиваем поиск
	}

	if ((hSearch = FindFirstFile(lpszFileName, &wfd)) == INVALID_HANDLE_VALUE)
		return TRUE; // в противном случае выходим
	do
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // если мы нашли файл
		{
			char file[MAX_PATH];
			if (GetFullPathName(lpszFileName, MAX_PATH, file, &part) == 0) return FALSE;
			strcpy(part, wfd.cFileName);

			vect.push_back(file);
		}
	while (FindNextFile(hSearch, &wfd)); // ищем следующий файл
	FindClose(hSearch); // заканчиваем поиск

	return TRUE;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
