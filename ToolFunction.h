#ifndef __TOOL_FUNCTION_H__
#define __TOOL_FUNCTION_H__
#include "stdhead.h"
#include "DataStructAndConstant.h"

void FormatString(char *strStrToFormat);
void errexit(const char *format, ...);

DWORD GetFileList(LPFILE_INFO lpFI, DWORD dwFIsize, const TCHAR* strPath);
DWORD StorFileList(TCHAR* stStorBuff, DWORD dwMaxSize, TCHAR* strPath);

long fileSize(FILE *stream);
TCHAR* mytcscat_s(TCHAR *dst, const TCHAR *src);
TCHAR* ANSIToUnicode(const char* str);
char* UnicodeToANSI(const TCHAR* str);
int issubFileOrDir(TCHAR* parentDir, TCHAR *subdir);

int FileSend(SOCKET s, const char *filename);
#endif