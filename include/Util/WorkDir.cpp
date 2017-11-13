//---------------------------------------------------------------------------
#pragma hdrstop

#include "C:\Home\Util\WorkDir.h"

//---------------------------------------------------------------------------
bool SearchFiles(LPCTSTR lpszFileName, LPSEARCHFUNC lpSearchFunc, bool bInnerFolders)
{
	LPTSTR part;
	char tmp[MAX_PATH]; // ��������� ������
	char name[MAX_PATH];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

	// ������� ����� ������ ��������� ������ ...
	if (bInnerFolders)
	{
		if (GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) return FALSE;
		strcpy(name, part);
		strcpy(part, "*.*");

		// ���� ����� ����������, �� ������ �����
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if (!((hSearch = FindFirstFile(tmp, &wfd)) == INVALID_HANDLE_VALUE))
			do
			{
				// � ������ ����� ���� ��� ����� � ������� "." � ".."
				// � ��� ����� �� �� �������

				// ���������� ����� "." � ".."
				if (!strncmp(wfd.cFileName, ".", 1) || !strncmp(wfd.cFileName, "..", 2))
					continue;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // ���� �� ����� �����
				{
					char next[MAX_PATH];
					if (GetFullPathName(lpszFileName, MAX_PATH, next, &part) == 0) return FALSE;
					strcpy(part, wfd.cFileName);
					strcat(next, "\\");
					strcat(next, name);

					SearchFiles(next, lpSearchFunc, TRUE);
				}
			} while (FindNextFile(hSearch, &wfd)); // ���� ��������� ����

			FindClose(hSearch); // ����������� �����
	}

	if ((hSearch = FindFirstFile(lpszFileName, &wfd)) == INVALID_HANDLE_VALUE)
		return TRUE; // � ��������� ������ �������
	do
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // ���� �� ����� ����
		{
			char file[MAX_PATH];
			if (GetFullPathName(lpszFileName, MAX_PATH, file, &part) == 0) return FALSE;
			strcpy(part, wfd.cFileName);

			lpSearchFunc(file);
		}
	while (FindNextFile(hSearch, &wfd)); // ���� ��������� ����
	FindClose(hSearch); // ����������� �����

	return TRUE;
}
//---------------------------------------------------------------------------
bool  SearchFiles(LPCTSTR lpszFileName, std::vector <clAnsiString > &vect, bool bInnerFolders)
{
	LPTSTR part;
	char tmp[MAX_PATH]; // ��������� ������
	char name[MAX_PATH];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

	// ������� ����� ������ ��������� ������ ...
	if (bInnerFolders)
	{
		if (GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) return FALSE;
		strcpy(name, part);
		strcpy(part, "*.*");

		// ���� ����� ����������, �� ������ �����
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if (!((hSearch = FindFirstFile(tmp, &wfd)) == INVALID_HANDLE_VALUE))
			do
			{
				// � ������ ����� ���� ��� ����� � ������� "." � ".."
				// � ��� ����� �� �� �������

				// ���������� ����� "." � ".."
				if (!strncmp(wfd.cFileName, ".", 1) || !strncmp(wfd.cFileName, "..", 2))
					continue;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // ���� �� ����� �����
				{
					char next[MAX_PATH];
					if (GetFullPathName(lpszFileName, MAX_PATH, next, &part) == 0) return FALSE;
					strcpy(part, wfd.cFileName);
					strcat(next, "\\");
					strcat(next, name);

					SearchFiles(next, vect, TRUE);
				}
			} while (FindNextFile(hSearch, &wfd)); // ���� ��������� ����

			FindClose(hSearch); // ����������� �����
	}

	if ((hSearch = FindFirstFile(lpszFileName, &wfd)) == INVALID_HANDLE_VALUE)
		return TRUE; // � ��������� ������ �������
	do
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // ���� �� ����� ����
		{
			char file[MAX_PATH];
			if (GetFullPathName(lpszFileName, MAX_PATH, file, &part) == 0) return FALSE;
			strcpy(part, wfd.cFileName);

			vect.push_back(file);
		}
	while (FindNextFile(hSearch, &wfd)); // ���� ��������� ����
	FindClose(hSearch); // ����������� �����

	return TRUE;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
