#include "stdhead.h"
#include "DataStructAndConstant.h"

void FormatString(TCHAR *strStrToFormat)
{
	for (ULONG i = 0; i<_tcslen(strStrToFormat); i++)
	{
		if (strStrToFormat[i] == 13 || strStrToFormat[i] == 10)
		{
			strStrToFormat[i] = 0;
		}
	}
}


TCHAR* mytcscat_s(TCHAR *dst, const TCHAR *src)
{
	TCHAR *p = dst;
	while (*p != '\0')
	{
		*p++;
	}
	while (*src != '\0')
	{
		*p = *src;
		*p++;
		*src++;
	}
	*p = '\0';
	return dst;
}

DWORD GetFileList(LPFILE_INFO lpFI, DWORD dwFIsize, const TCHAR* strPath)
{
	TCHAR strFileName[260];
	_tcscpy_s(strFileName, strPath);
	FormatString(strFileName);
	if (strFileName[_tcslen(strFileName) - 1] != '\\')
	{
		_tcscat_s(strFileName, L"\\*.*");
	}
	else
	{
		_tcscat_s(strFileName, L"*.*");
	}
	// 列出文件夹strFileName中所有的文件
	WIN32_FIND_DATA wfd;
	HANDLE hFile = FindFirstFile(strFileName, &wfd);
	DWORD count = 0;
	if (hFile != INVALID_HANDLE_VALUE)
	{
		_tcscpy_s(lpFI[count].cFileName, wfd.cFileName);
		lpFI[count].dwFileAttributes = wfd.dwFileAttributes;
		lpFI[count].ftCreationTime = wfd.ftCreationTime;
		lpFI[count].ftLastAccessTime = wfd.ftLastAccessTime;
		lpFI[count].ftLastWriteTime = wfd.ftLastWriteTime;
		lpFI[count].nFileSizeHigh = wfd.nFileSizeHigh;
		lpFI[count].nFileSizeLow = wfd.nFileSizeLow;
		while (FindNextFile(hFile, &wfd) && count < dwFIsize)
		{
			count++;
			_tcscpy_s(lpFI[count].cFileName, wfd.cFileName);
			lpFI[count].dwFileAttributes = wfd.dwFileAttributes;
			lpFI[count].ftCreationTime = wfd.ftCreationTime;
			lpFI[count].ftLastAccessTime = wfd.ftLastAccessTime;
			lpFI[count].ftLastWriteTime = wfd.ftLastWriteTime;
			lpFI[count].nFileSizeHigh = wfd.nFileSizeHigh;
			lpFI[count].nFileSizeLow = wfd.nFileSizeLow;
		}
		count++;
		FindClose(hFile);
	}
	else
		return -1;
	return count;
}

DWORD StorFileList(TCHAR* stStorBuff, DWORD dwstStorBuff, TCHAR* strPath)
{
	FILE_INFO Files[30];
	DWORD dwFiles = GetFileList(Files, 30, strPath);
	//cout << dwFiles << endl;
	memset(stStorBuff, L'\0', sizeof(stStorBuff));
	setlocale(LC_ALL, "Chinese-simplified");//设置中文环境
	for (DWORD i = 0; i < dwFiles; i++)
	{
		if (_tcscmp(Files[i].cFileName, L".") == 0)	{ continue; }
		if (_tcscmp(Files[i].cFileName, L"..") == 0) { continue; }

		bool bFolder = false;
		if (Files[i].dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) { bFolder = true; }

		TCHAR tmpBuff[1024] = L"0";
		memset(tmpBuff, L'\0', sizeof(tmpBuff));

		//写入文件的权限
		if (bFolder) { mytcscat_s(stStorBuff, L"drw-rw-rw-"); }
		else
		{
			mytcscat_s(stStorBuff, L"-rw-rw-rw-");
		}
		if (_DEBUG)
		{
			printf("字符串 %ws 长度为 %d\n", Files[i].cFileName, _tcslen(Files[i].cFileName));
		}
		//写入文件的数目
		DWORD dwFilein;
		if (bFolder)
		{
			TCHAR strFilePath[1024];
			_tcscpy_s(strFilePath, strPath);
			if (strFilePath[_tcslen(strFilePath) - 1] != L'\\')
			{
				_tcscat_s(strFilePath, L"\\");
			}
			_tcscat_s(strFilePath, Files[i].cFileName);
			FILE_INFO Filein[MAX_FILE];
			dwFilein = GetFileList(Filein, MAX_FILE, strFilePath) - 2;
		}
		else dwFilein = 1;
		memset(tmpBuff, L'\0', sizeof(tmpBuff));
		wsprintf(tmpBuff, L"%4d", dwFilein);
		mytcscat_s(stStorBuff, tmpBuff);

		//写入文件的类型
		memset(tmpBuff, L'\0', sizeof(tmpBuff));
		if (bFolder)
		{
			mytcscat_s(stStorBuff, L"  <DIR>  ");
		}
		else
		{
			mytcscat_s(stStorBuff, L"  <FIL>  ");
		}

		//写入文件的大小
		memset(tmpBuff, L'\0', sizeof(tmpBuff));
		wsprintf(tmpBuff, L"%4d ", Files[i].nFileSizeLow);
		mytcscat_s(stStorBuff, tmpBuff);

		//写入文件的修改时间
		SYSTEMTIME *Stime, Tmp;
		Stime = &Tmp;
		FileTimeToSystemTime(&(Files[i].ftLastWriteTime), Stime);
		TCHAR* Month[] = { L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" };
		memset(tmpBuff, L'\0', sizeof(tmpBuff));
		if (bFolder)
		{
			wsprintf(tmpBuff, L"%s %2d %4d ", Month[--Stime->wMonth], Stime->wDay, Stime->wYear);
		}
		else
		{
			wsprintf(tmpBuff, L"%s %2d %02d:%02d ", Month[--Stime->wMonth], Stime->wDay, Stime->wHour, Stime->wMinute);
		}
		mytcscat_s(stStorBuff, tmpBuff);

		//写入文件名称
		mytcscat_s(stStorBuff, Files[i].cFileName);
		mytcscat_s(stStorBuff, L"\r\n");
	}
	if (_DEBUG)
	{
		printf("字符串 %ws 长度为 %d\n", stStorBuff, sizeof(stStorBuff));
	}
	return dwFiles;
}

/*
* convert ANSI to Unicode
* http://www.cnblogs.com/gakusei/articles/1585211.html
*/
TCHAR* ANSIToUnicode(const char* str)
{
	int  len = strlen(str);
	int  unicodeLen = MultiByteToWideChar(CP_ACP,
		0,
		str,
		-1,
		NULL,
		0);
	TCHAR *  pUnicode = new  TCHAR[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP,
		0,
		str,
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	return  pUnicode;
}

/*
*  Unicode to ANSI
*/
char* UnicodeToANSI(const TCHAR* str)
{
	// wide char to multi char
	int iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		str,
		-1,
		NULL,
		0,
		NULL,
		NULL);
	char* pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char)* (iTextLen + 1));
	::WideCharToMultiByte(CP_ACP,
		0,
		str,
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL);
	return pElementText;
}

int issubFileOrDir(TCHAR* parentDir, TCHAR *subdir)
{
	// -1: not exist	1: subdir	2: subfile
	//
	FILE_INFO Files[10];
	DWORD dwFiles = GetFileList(Files, 10, parentDir);
	for (DWORD i = 0; i < dwFiles; i++)
	{
		if (_tcscmp(Files[i].cFileName, L".") == 0)	{ continue; }
		if (_tcscmp(Files[i].cFileName, L"..") == 0) { continue; }

		bool bFolder = false;
		if ((Files[i].dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && _tcscmp(Files[i].cFileName, subdir) == 0)
		{
			return 1;
		}

		if (_tcscmp(Files[i].cFileName, subdir) == 0)
		{
			// this is a file
			return 2;
		}
	}
	return -1;
}

long fileSize(FILE *stream)
{
	long curPos, length;
	curPos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curPos, SEEK_SET);
	return length;
}


int FileSend(SOCKET s, const char *filename)
{

	FILE* fp;
	errno_t err;
	if ((err = fopen_s(&fp, filename, "rb")) != 0)
	{
		return -1;
	}

	FILE_SEND fileinfo;
	memset(&fileinfo, 0x00, sizeof(fileinfo));
	memcpy(fileinfo.filename, filename, sizeof(fileinfo.filename));
	fileinfo.fileBlock = SEND_BUFFER_SIZE;
	long size = fileSize(fp);
	fileinfo.filesize = size;
	if (size < SEND_BUFFER_SIZE) fileinfo.BlockNum = 1;
	else
		fileinfo.BlockNum = size / SEND_BUFFER_SIZE + 1;

	unsigned int infolen = sizeof(fileinfo);
	// 发送文件基本信息: 文件名，文件大小等等
	if ((send(s, (char *)&fileinfo, sizeof(fileinfo), 0)) < 0)
	{
		fclose(fp);
		return SOCKET_ERROR;
	}
	// 发送文件
	BYTE sendData[SEND_BUFFER_SIZE];
	memset(sendData, 0x00, SEND_BUFFER_SIZE);
	int allSendSize = fileinfo.filesize;
	int BLOCK_NUM = fileinfo.BlockNum - 1;
	int END_BUFFER_SIZE = size % SEND_BUFFER_SIZE;
	int block = 1;
	do
	{
		unsigned long sendThisTime;
		while (!feof(fp))
		{
			memset(sendData, 0, SEND_BUFFER_SIZE);
			if (block <= BLOCK_NUM)	fread(sendData, SEND_BUFFER_SIZE, 1, fp);
			else
			{
				fread(sendData, END_BUFFER_SIZE, 1, fp);
			}
			if (allSendSize < SEND_BUFFER_SIZE) sendThisTime = END_BUFFER_SIZE;
			else
				sendThisTime = SEND_BUFFER_SIZE;

			do
			{
				unsigned long doneSoFar;
				if ((doneSoFar = send(s, (char *)sendData, sendThisTime, 0)) < 0)
				{
					fclose(fp);
					return SOCKET_ERROR;
				}
				sendThisTime -= doneSoFar;
				allSendSize -= doneSoFar;
			} while (sendThisTime > 0);
			// 接受从客户端发送回来的文件块数
			recv(s, (char *)&block, sizeof(int), 0);
			//cout << "recv from client " << block << endl;
		}
	} while (allSendSize > 0);
	cout << "Translate file " << filename << " success.\n";
	fclose(fp);
	return TRAN_SUCCESS;
}

void errexit(const char *format, ...)
{
	va_list	args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	WSACleanup();
	exit(1);
}
